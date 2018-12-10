#include "include.h"
#include <stdio.h> 
#include "Report.h"
#include "Public.h"
//#include "Gps_App.h"
#include "RadioProtocol.h"
#include "GPIOControl.h"
#include "Io.h"
#include "Blind.h"
#include "Ril_cell.h"
#include "ql_memory.h"

extern u32 battery_capacity;
extern u32 battery_voltage;
extern s32 m_pwrOnReason;

//#define FLASH_BLIND_STEP_LEN                124

/********************本地变量*************************/
static u32 Link1ReportTime = 0;//连接1位置汇报时间间隔，单位秒
static u32 Link1ReportTimeCount = 0;//连接1位置汇报时间计数
//static u32 Link1LastReportDistance = 0;//连接1上一个上报点的距离，单位米
static u32 Link1ReportDistance = 0;//连接1下一个上报点的距离
static u8 Link1ReportAckFlag = 0;//连接1上一条位置信息应答标志，1表示有应答，0表示无应答
static u8 Link1ReportBuffer[FLASH_BLIND_STEP_LEN] = {0};//连接1位置上报缓冲
static u8 Link1ReportBufferLen = 0;//连接1位置上报缓冲内容长度
//static u32 ReportStrategy = 0;//汇报策略
//static u32 ReportScheme = 0;//汇报方案
//static u32 EmergencyReportTimeSpace = 0;//紧急报警汇报时间间隔
//static u32 EmergencyReportDistanceSpace = 0;//紧急报警汇报距离间隔
static u32 OverSpeedReportTimeSpace = 0;//超速报警汇报时间间隔
static u32 OverTimeReportTimeSpace = 0;//超时报警汇报时间间隔
static u32 AccOnReportTimeSpace = 0;//缺省汇报时间间隔
//static u32 AccOnReportDistanceSpace = 0;//缺省汇报时间间隔
static u32 AccOffReportTimeSpace = 0;//休眠汇报时间间隔
//static u32 AccOffReportDistanceSpace = 0;//休眠汇报时间间隔
//static u32 UnloginReportTimeSpace = 0;//驾驶员未登录汇报时间间隔
//static u32 UnloginReportDistanceSpace = 0;//驾驶员未登录汇报时间间隔
static u32 TempTrackTimeSpace = 0;//临时跟踪时间间隔
static u32 TempTrackCount = 0;//临时跟踪次数
static u32 ReportEventSwitch = 0;//位置上报事件项开关，bit0:为1表示开启超速报警位置上报；bit1:为1表示开启超时报警位置上报
u16 DelayAckCommand = 0;//延时应答命令
u8  DelayBuffer[300];//延时缓冲，可把要延时发送的数据拷贝到这里，注意最大长度限制
u8  DelayBufferLen;//延时缓冲数据长度

static u32 RTCWakeUpTimeOffset = 0;//RTC唤醒上报时间偏移

static ReportStatus_TypeDef	ReportStatus = ReportStatus_Off;

static ST_LocInfo locinf = {0};

/********************全局变量*************************/
u16 Link1LastReportSerialNum = 0;//连接1上一条位置上报的流水号
u16 Link2LastReportSerialNum = 0;//连接2上一条位置上报的流水号
u32 Link1ReportTimeSpace = 0;//同Link1ReportTime一样，用作外部访问
u32 Link1ReportDistanceSpace = 0;//连接1位置上报时间间隔，单位米

/********************外部变量*************************/
extern TIME_T CurTime;

extern u16     CarControlAckSerialNum;//车辆控制应答流水号
extern u8	DelaySendChannel ;//延时发送通道号
extern u8	TerminalAuthorizationFlag;//终端鉴权标志,鉴权成功为1,失败为0

/********************本地函数声明*********************/
static void Report_GetLink1NextReportPoint(u32 *NextReportTime, u32 *NextReportDistance);//获取下一个上报点
//static u32 Report_GetCurDistance(void);//获取当前的累计行驶里程
static u16 Report_GetPositionAdditionalInfo(u8 *pBuffer);//获取位置附加信息
static u16 Report_GetPositionStdAdditionalInfo(u8 *pBuffer);//获取标准的位置附加信息
static u16 Report_GetPositionEiAdditionalInfo(u8 *pBuffer);//获取伊爱自定义的位置附加信息
static void Report_Get_Location(void);//获取经纬度信息


#define REPORT_TICKS     30000
/********************函数定义*************************/

//关闭系统任务定时器
void Stop_ReportTimer(void)
{
	 Ql_Timer_Stop(TIME_REPORT_TIME_ID);
}
//开启系统任务定时器
void Start_ReportTimer(void)
{
	Ql_Timer_Start(TIME_REPORT_TIME_ID, SYSTICK_TICKS, TRUE);
}
/******************************************************************************
*	类别:定时器返回函数
*	功能:
*	参数:定时Id
*	返回:无
******************************************************************************/
void Callback_ReportTimer(u32 timeId,void *param)
{

}

//任务时间定时器初始化
void init_ReportTimer(void)
{ 	
	s32 ret;
	//注册GPS定位定时器
	ret = Ql_Timer_Register(TIME_REPORT_TIME_ID, Callback_ReportTimer, NULL);
	if(ret <0)
    {
		//保存错误代号
        APP_DEBUG("\r\n<--failed!!,init_TaskScheduleTimer Ql_Timer_Register: timer(%d) fail ,ret = %d -->\r\n",TIME_REPORT_TIME_ID,ret);
    }
	Ql_Timer_Start(TIME_REPORT_TIME_ID, REPORT_TICKS, TRUE);
	APP_DEBUG("\r\n<--init_TaskScheduleTimer -->\r\n");
}
void Report_RtcWakeUpOffsetSave(void)
{
	if(RTCPWRON != m_pwrOnReason)
	{
		// 写入RTC唤醒时间偏移量
		FRAM_BufferWrite(FRAM_RTC_WAKEUP_ADDR,(u8*)&RTCWakeUpTimeOffset, FRAM_RTC_WAKEUP_LEN);
	}
}
void Report_RtcWakeUpOffsetResume(void)
{
	// 恢复RTC唤醒时间偏移量
	FRAM_BufferRead((u8*)&RTCWakeUpTimeOffset, FRAM_RTC_WAKEUP_LEN, FRAM_RTC_WAKEUP_ADDR);
	APP_DEBUG("\r\n...RTC唤醒时间偏移量:%d...\r\n",RTCWakeUpTimeOffset);
}
/**************************************************************************
//函数名：Report_EnterPowerDownAfterReported
//功能：上报完成后进入关机模式
//输入：参数类型
//输出：
//返回值：无
//备注：
***************************************************************************/
void Report_EnterPowerDownAfterReported(void* param)
{
	BlindSavePara* blindSave = (BlindSavePara*)param;
	u32 upInterval = blindSave->interval;	
	//u8 updateFlg = GetUpdateFlag();

	//if(updateFlg)return;//正在升级则不进入低功耗模式
	//if(TempTrackCount)return;//如果处于临时追踪状态则不进入低功耗模式
	if(upInterval <= 5*60)
	{
		//Led_Status_Change(LED_STATUS_POWER_LOW);
		//Io_WriteIoStatusBit(IO_STATUS_BIT_SLEEP, SET);
		//if(0xffff == uart_get_busy_status())
		{
			//APP_DEBUG("\r\n...上报后进入睡眠模式....\r\n");
			//Ql_SleepEnable();//上报间隔小于5分钟就进入睡眠模式
		}
	}
	else
	{
		RIL_Alarm_Create(&blindSave->DateTime,0);
		APP_DEBUG("\r\nSet Alarm :\r\n%02d/%02d/%02d %02d:%02d:%02d\r\n",
			blindSave->DateTime.year, blindSave->DateTime.month,blindSave->DateTime.day, 
			blindSave->DateTime.hour, blindSave->DateTime.minute, blindSave->DateTime.second);
		//Led_Status_Change(LED_STATUS_POWER_DOWN);
		//关机前喂一次狗
		GpioOutInit(WATCHDOG,PINLEVEL_HIGH);//配置成输出
		HARD_FEED_DOG();
		Ql_Sleep(200);
		//休眠之前备份盲区节点信息
        Blind_UpdateCirQueueInfo();
		Report_RtcWakeUpOffsetSave();
		// 关闭gnss电源
		
#ifdef __GNSS_NEW_API__
		Ql_GNSS_PowerDown();
#else
		RIL_GPS_Open(0);
#endif
		Ql_Sleep(1000);
		Ql_PowerDown(1);//关机模式
	}
}

static void *pBlindSave = NULL;

void Callback_BlindSave_OnTimer(u32 timerId, void* param)
{
	
	BlindSavePara* blindSaveInfo = (BlindSavePara*)param;
	u8 updateFlg = GetUpdateFlag();

	if(LOGIC_BLIND_SAVE_TMR_ID == timerId)
	{
		if(0 == Link1ReportAckFlag)
		{
			APP_DEBUG("\r\nCallback_BlindSave_OnTimer写盲区....\r\n");
			Blind_Link1Save(blindSaveInfo->pBuf,blindSaveInfo->BufLen,CHANNEL_DATA_1);
		}
		if(updateFlg == 2)//远程升级错误重试中...
		{
			ftp_downfile_timer();
			return;
		}
		else if(updateFlg == 1)//远程升级中...
		{
			return;
		}
		if(TempTrackCount)return;//如果处于临时追踪状态则不进入低功耗模式
		if((0 == Blind_GetReportState())||(0 == GetTerminalAuthorizationFlag()))
		{
			APP_DEBUG("\r\nCallback_BlindSave_OnTimer....%d\r\n",timerId);
			Report_EnterPowerDownAfterReported(param);//定时进入关机模式
		}
	}
}
/**************************************************************************
//函数名：Report_Blind_Callback_Init
//功能：位置上报后盲区回调函数初始化
//			注册盲区回调函数，设置回调参数
//输入：参数类型
//输出：
//返回值：无
//备注：
***************************************************************************/
void Report_Blind_Callback_Init(void)
{
	pBlindSave = Ql_MEM_Alloc(sizeof(BlindSavePara));
	if(NULL != pBlindSave)
	{
		Ql_Timer_Register(LOGIC_BLIND_SAVE_TMR_ID, Callback_BlindSave_OnTimer, pBlindSave);
		APP_DEBUG("\r\n<-- Blind Ql_Timer_Register -- >\r\n");
	}
	else
	{
		APP_DEBUG("\r\nMEM APPLY FAIL.....\r\n");
		Ql_Reset(0);
	}	
}

void proc_report_task(s32 taskId)
{
			
	while(TRUE)
	{
		Ql_Sleep(200);
		if(ProjectInitFinshFlag==1)
		{
			Report_TimeTask();
			//HeartBeat_TimeTask();
		}
//		SoftWdg_Feed(BIT1);
	}
}

/**************************************************************************
//函数名：GetNextReportTime
//功能：获取距离下一次上报的时间
//输入：无
//输出：无
//返回值：
//备注：
***************************************************************************/
u32 GetNextReportTime(void)
{
    return Link1ReportTime - Link1ReportTimeCount;
}
/**************************************************************************
//函数名：Report_GetReportInterval
//功能：获取缺省汇报时间间隔
//输入：无
//输出：无
//返回值：
//备注：
***************************************************************************/
u32 Report_GetDefaultReportInterval(void)
{
    return AccOffReportTimeSpace;
}

/**************************************************************************
//函数名：Report_GetReportStatus
//功能：获取上报状态
//输入：
//输出：
//返回值：
//备注：
***************************************************************************/
ReportStatus_TypeDef Report_GetReportStatus(void)
{
	return ReportStatus;
}
/**************************************************************************
//函数名：Report_SetReportStatus
//功能：置位上报状态
//输入：参数类型
//输出：
//返回值：无
//备注：
***************************************************************************/
void Report_SetReportStatus(ReportStatus_TypeDef reportSta)
{
	ReportStatus = reportSta;
}

u8 Report_IsReady(u64 time)
{
	if(RTCPWRON != m_pwrOnReason)
	{
		if(Link1ReportTimeCount >= Link1ReportTime)
		{
			RTCWakeUpTimeOffset = time%Link1ReportTime;
			return 1;
		}
	}
	else
	{
		if(RTCWakeUpTimeOffset == time%Link1ReportTime)
			return 1;
	}
	return 0;
}
/**************************************************************************
//函数名：Report_TimeTask
//功能：实现位置汇报（包括实时跟踪、紧急报警、超速报警、超时报警、定时定距等位置汇报）
//输入：无
//输出：无
//返回值：始终是ENABLE
//备注：位置汇报定时任务，1秒钟进入1次，任务调度器需要调用此函数
***************************************************************************/
FunctionalState Report_TimeTask(void)
{
	u8 Link1ReportFlag = 0;//位置上报标志，1表示要上报，0表示不用上报
	u8 Ack = 0;
	u8 TempBuffer[6];
	//u8 ReportAddTimeCount = 0;
	u32 CurTimeCount,crc32Verify;
	static u32 LastTimeCount = 0;
	ST_Time dateTime;
	u64 mkTime;
	static u64 LastmkTime = 0;
	static u32 OnlineOvertimeCnt = 0;//长期不上线计数器，时间到了进入关机模式
	//static u16 gnssPwrCnt = 0xffff;

	if(ReportStatus_Off == Report_GetReportStatus())
	{
		//if(OnlineOvertimeCnt++ > 80)//大约30s
		//{
		//	Report_SetReportStatus(ReportStatus_Init);//大约一分钟转入下一步
		//}
		return ENABLE;
	}
	else if(ReportStatus_Init == Report_GetReportStatus())
	{
		if(GetTerminalAuthorizationFlag())
		{
			OnlineOvertimeCnt = 0;
			Link1ReportTimeCount = Report_GetDefaultReportInterval()-5;
			Report_SetReportStatus(ReportStatus_On);   
		}
		else
		{
			Report_SetReportStatus(ReportStatus_Wait);
		}
		Report_Get_Location();//取经纬度信息
		Report_UpdatePram();
		LastTimeCount = ConverseGmtime(&CurTime);
		return ENABLE;
	}
	else if(ReportStatus_Wait == Report_GetReportStatus())
	{
		if(GetTerminalAuthorizationFlag())
		{
			OnlineOvertimeCnt = 0;
			Report_SetReportStatus(ReportStatus_On);   
		}
	// 长期不上线的低功耗处理,有必要吗???????
	/*
		if(OnlineOvertimeCnt++ > 3000)//大约10分钟
		{
			//设置闹钟
			u32 interval,temp;
			interval = Report_GetDefaultReportInterval();//获取上报间隔
			Ql_GetLocalTime(&dateTime);//获取实时时间
			mkTime = Ql_Mktime(&dateTime);			
			temp = mkTime%interval;//距离下次需要上报的时间
			if(temp > 120)//大于2分钟
			{
				mkTime += temp;
				mkTime -= 60;//设置闹钟提前一分钟开机
				Ql_MKTime2CalendarTime(mkTime, &dateTime);
				dateTime.year -= 2000;
				RIL_Alarm_Create(&dateTime,0);
				APP_DEBUG("\r\nSet Alarm :\r\n%02d/%02d/%02d %02d:%02d:%02d\r\n",
				dateTime.year, dateTime.month,dateTime.day, 
				dateTime.hour, dateTime.minute, dateTime.second);
				//关机前喂一次狗
				GpioOutInit(WATCHDOG,PINLEVEL_HIGH);//配置成输出
				HARD_FEED_DOG();
				Ql_Sleep(200);
				APP_DEBUG("<-- 超时不上线进入关机模式-- >\r\n");
				Ql_PowerDown(1);//关机模式
			}
		}*/
	}
	
	Report_GetLink1NextReportPoint(&Link1ReportTime,&Link1ReportDistance);

	if(0 != Link1ReportTime)//Link1ReportTime等于0表示不开启定时上报
	{
		Ql_GetLocalTime(&dateTime);//获取实时时间
		mkTime = Ql_Mktime(&dateTime);
		if(mkTime == LastmkTime)return ENABLE;
		//gnssPwrCnt--;
		//if(0 == gnssPwrCnt)
		//{
		//	gnssPwrCnt = 0xffff;
		//	Ql_OS_SendMessage (main_task_id, MSG_ID_GNSS_CTR, MSG_GNSS_ON,0);
		//}
		Link1ReportTimeCount++;
		LastmkTime = mkTime;
		if(Report_IsReady(mkTime))
		{
			Link1ReportFlag = 1;
			APP_DEBUG("\r\nreport time is :\r\n%02d/%02d/%02d %02d:%02d:%02d:%02d\r\n",
			dateTime.year, dateTime.month,dateTime.day, 
			dateTime.hour, dateTime.minute, dateTime.second,Link1ReportTime);
			if((TempTrackTimeSpace > 0)&&(TempTrackCount > 0))
			{
				TempTrackCount--;
                if(0 == TempTrackCount)
                {
                    TempTrackTimeSpace = 0;
                }
				Report_PramBackup();
				APP_DEBUG("<-- 临时追踪次数.... %d -- >\r\n", TempTrackCount);
			}
		}
		
	}

	if(1 == Link1ReportFlag)
	{	                
		APP_DEBUG("<-- Report PositionInfo -- >\r\n" );
		RIL_NW_GetCell();
		RIL_GetPowerSupply(&battery_capacity, &battery_voltage);
		Io_WriteIoStatusBit(IO_STATUS_BIT_SLEEP, RESET);
		Link1ReportBufferLen = Report_GetPositionInfo(Link1ReportBuffer);

		Ack = RadioProtocol_PostionInformationReport(Link1ReportBuffer,Link1ReportBufferLen);
		
		((pBlindSavePara)pBlindSave)->pBuf = Link1ReportBuffer;		
		((pBlindSavePara)pBlindSave)->BufLen = Link1ReportBufferLen;
		((pBlindSavePara)pBlindSave)->interval = Link1ReportTime;
		//设置闹钟
		//Ql_GetLocalTime(&dateTime);//获取实时时间
		//mkTime = Ql_Mktime(&dateTime);
		mkTime += Link1ReportTime;//下一次上报时间
		mkTime -= 60;//设置闹钟提前60s开机
		Ql_MKTime2CalendarTime(mkTime, &dateTime);
		dateTime.year -= 2000;
		Ql_memcpy(&((pBlindSavePara)pBlindSave)->DateTime,&dateTime,sizeof(ST_Time));
		if(GetUpdateFlag())//正在远程升级或重试升级中...
		{
			SetUpdateFlag(1);
		}
		Ql_OS_SendMessage(main_task_id, MSG_ID_BLIND_SAVE, REPORT_OVERTIME, 0);

		Link1ReportTimeCount = 0;
		Link1ReportAckFlag = 0;	
		// 如果GPS未定位成功就启用基站定位
		if(0 == Gps_ReadStatus())
		{
			Report_Get_Location();//取经纬度信息
		}
		//Ql_OS_SendMessage (main_task_id, MSG_ID_GNSS_CTR, MSG_GNSS_OFF,0);
		//gnssPwrCnt = Report_GetDefaultReportInterval()-30;
	}
	return ENABLE;	
}
/*******************************************************************************
* Function Name  : DelayTrigTimeTask(void)
* Description    : 延时触发
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState DelayTrigTimeTask(void)
{
    u8 Buffer[FLASH_BLIND_STEP_LEN+20];
    u8 flag;
    u8 Len;
    u16 length;
    MESSAGE_HEAD head;

    if(0 == DelayAckCommand)
    {
        return DISABLE;
    }

    flag = 0;
    switch(DelayAckCommand)
    {
        case 0:
        {
            break;
        }
        case 0x0500://0x0500车辆控制
        {
            //应答流水号
            length = 0;
            Buffer[0] = (CarControlAckSerialNum & 0xff00) >> 8;
            Buffer[1] = CarControlAckSerialNum & 0xff;
            length += 2;
            //获取位置信息,拷贝到Buffer中
            Len = Report_GetPositionInfo(Buffer+2);
            length += Len;
            //发送位置汇报信息
            //消息ID
            head.MessageID = 0x0500;
            head.Attribute = length;
            //发送
            if(ERROR == RadioProtocol_SendCmd(head, Buffer, length, DelaySendChannel))
            {
                flag = 1;
            }
            break;
        }        
        case 2:
        {
            //Report_EraseBlindArea();//升级前擦除一次盲区flash myh 130808
            // 触发无线升级任务
            SetEvTask(MSG_FtpUpdata_EvTask);
            break;
        }
        case 0x0003://0x0003,终端注销
        {
            TerminalAuthorizationFlag = 0;//行标检测修改
            break;
        }
        case 4:
        {
            //Communication_Init();
            break;
        }
        case 5:
        {
            //重启
            Ql_Reset(0);
            break;
        }
        case 6:
        {
            // 触发连接监管平台任务
            //SetEvTask(EV_LINK_GOV_SERVER);
            break;
        }
        case 7:
        {
            //重启之前备份盲区节点信息
            Blind_UpdateCirQueueInfo();
            DelayAckCommand = 5;//下一步重启
            flag = 1;
            break;
        }
        case 8:
        {
            //关Communication_TimeTask任务
            break;
        }
        case 9:
        {
            //MOD_CloseMainIp();//模块主动断开连接
            DelayAckCommand = 10;//下一步是关闭通信模块任务
            //触发延时任务
            SetTimerTask(TIME_DELAY_TRIG, 5*SECOND);
            TerminalAuthorizationFlag = 0;
            flag = 1;
            break;
        }
        case 10:
        {
            //dxl,2017.2.24,无此功能
            break;
        }
        case 11:
        {
            //MOD_CloseMainIp();//模块主动断开连接
            DelayAckCommand = 12;//下一步是关机
            //触发延时任务
            SetTimerTask(TIME_DELAY_TRIG, 5*SECOND);
            TerminalAuthorizationFlag = 0;
            flag = 1;
			//重启
            Ql_Reset(0);
            break;
        }
        case 12:
        {
			APP_DEBUG("\r\n<-- 硬件关机 -- >\r\n");
			//掉电关机之前写盲区节点信息到Flash
			Blind_UpdateCirQueueInfo();
			REMOTE_SHUTDOWN();
            break;
        }
        case 13:
        {
            if(ACK_OK != RadioProtocol_OriginalDataUpTrans(0x41, DelayBuffer, DelayBufferLen))
            {
                flag = 1;
            }
            break;
        }
		case 14:
        {
            Report_UploadPositionInfo();
			Report_ClearReportTimeCount();
            break;
        }
        default :break;
    }

    if(0 == flag)
    {
        DelayAckCommand  = 0;
        return DISABLE;
    }
    else
    {
        return ENABLE;
    }
}

/**************************************************************************
//函数名：Report_UpdatePram
//功能：更新位置汇报所有变量
//输入：无
//输出：无
//返回值：无
//备注：主要是更新位置汇报模块所使用到的变量，运行任务调度之前需调用此函数
***************************************************************************/
void Report_UpdatePram(void)
{
  u8 Buffer[5];
  
  if(E2_SLEEP_REPORT_TIME_LEN == EepromPram_ReadPram(E2_SLEEP_REPORT_TIME_ID,Buffer))
  {
    AccOffReportTimeSpace = Public_ConvertBufferToLong(Buffer);
  }
  if(E2_ACCON_REPORT_TIME_LEN == EepromPram_ReadPram(E2_ACCON_REPORT_TIME_ID,Buffer))
  {
    /*AccOnReportTimeSpace*/TempTrackTimeSpace = Public_ConvertBufferToLong(Buffer);
  }

}
/**************************************************************************
//函数名：Report_CmdAck
//功能：位置上报应答处理
//输入：通道号
//输出：无
//返回值：无
//备注：收到位置上报的通用应答时需调用此函数
***************************************************************************/
void Report_CmdAck(void)
{
    Link1ReportAckFlag = 1;
}
//追踪信息备份
void Report_PramBackup(void)
{
	u8 Buffer[10];
	u32 crc32Verify;

	Public_ConvertLongToBuffer(TempTrackCount,Buffer);	
	Public_ConvertShortToBuffer(TempTrackTimeSpace,Buffer+4);
	crc32Verify = Public_CRC_32( Buffer, 6 );
	Public_ConvertLongToBuffer(crc32Verify,Buffer+6);
	FRAM_BufferWrite(FRAM_TEMP_TRACK_NUM_ADDR,Buffer,10);
}
//上电恢复追踪信息
void Report_PramResume(void)
{
	u32 crc32Verify,crc32VerifyTmp;
	u8 Buffer[30];

	FRAM_BufferRead(Buffer, 10, FRAM_TEMP_TRACK_NUM_ADDR);
	crc32Verify = Public_CRC_32( Buffer, 6 );
	crc32VerifyTmp = Public_ConvertBufferToLong(Buffer+6);
	if(crc32Verify == crc32VerifyTmp)
	{
		if(FRAM_TEMP_TRACK_SPACE_LEN == FRAM_BufferRead(Buffer, 2, FRAM_TEMP_TRACK_SPACE_ADDR))
	    {
	        TempTrackTimeSpace = Public_ConvertBufferToShort(Buffer);
	    }
	    if(FRAM_TEMP_TRACK_NUM_LEN == FRAM_BufferRead(Buffer, 4, FRAM_TEMP_TRACK_NUM_ADDR))
	    {
	        TempTrackCount = Public_ConvertBufferToLong(Buffer);
	    }
		APP_DEBUG("\r\n...临时追踪校验通过%04x   %04x  %d   %d...\r\n",crc32Verify,crc32VerifyTmp,TempTrackTimeSpace,TempTrackCount);
	}
	else
	{
		APP_DEBUG("\r\n...临时追踪校验不正确...\r\n");
	}
	
}

/**************************************************************************
//函数名：Report_UpdateOnePram
//功能：更新某个参数
//输入：参数类型
//输出：该参数类型对应的变量
//返回值：无
//备注：串口或远程设置位置汇报相关参数时需要调用此函数
***************************************************************************/
void Report_UpdateOnePram(u8 type)
{
    u8 Buffer[30];
    u8 BufferLen;
    u32 TcpPort;
    
    switch(type)
    {
    case PRAM_TEMP_TRACK_TIME://临时跟踪时间间隔
        {
            if(FRAM_TEMP_TRACK_SPACE_LEN == FRAM_BufferRead(Buffer, 2, FRAM_TEMP_TRACK_SPACE_ADDR))
            {
                TempTrackTimeSpace = Public_ConvertBufferToShort(Buffer);
            }
            break;
        }
    case PRAM_TEMP_TRACK_COUNT://临时跟踪次数
        {
            if(FRAM_TEMP_TRACK_NUM_LEN == FRAM_BufferRead(Buffer, 4, FRAM_TEMP_TRACK_NUM_ADDR))
            {
                TempTrackCount = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
#if 0
    case PRAM_EMERGENCY_REPORT_TIME://紧急报警汇报时间间隔
        {
            if(E2_EMERGENCY_REPORT_TIME_LEN == EepromPram_ReadPram(E2_EMERGENCY_REPORT_TIME_ID,Buffer))
            {
                EmergencyReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_EMERGENCY_REPORT_DISTANCE://紧急报警汇报距离间隔
        {
            if(E2_EMERGENCY_REPORT_DISTANCE_LEN == EepromPram_ReadPram(E2_EMERGENCY_REPORT_DISTANCE_ID,Buffer))
            {
                EmergencyReportDistanceSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_OVER_SPEED_REPORT_TIME://超速报警汇报时间间隔
        {
            if(E2_OVERSPEED_ALARM_REPORT_TIME_ID_LEN == EepromPram_ReadPram(E2_OVERSPEED_ALARM_REPORT_TIME_ID,Buffer))
            {
                OverSpeedReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_OVER_TIME_REPORT_TIME://超时报警汇报时间间隔
        {
            if(E2_TIRED_DRIVE_REPORT_TIME_ID_LEN == EepromPram_ReadPram(E2_TIRED_DRIVE_REPORT_TIME_ID,Buffer))
            {
                OverTimeReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_ACC_ON_REPORT_TIME://缺省汇报时间间隔
        {
            if(E2_ACCON_REPORT_TIME_LEN == EepromPram_ReadPram(E2_ACCON_REPORT_TIME_ID,Buffer))
            {
                AccOnReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_ACC_ON_REPORT_DISTANCE://缺省汇报距离间隔
        {
            if(E2_ACCON_REPORT_DISTANCE_LEN == EepromPram_ReadPram(E2_ACCON_REPORT_DISTANCE_ID,Buffer))
            {
                AccOnReportDistanceSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_ACC_OFF_REPORT_TIME://休眠汇报时间间隔
        {
            if(E2_SLEEP_REPORT_TIME_LEN == EepromPram_ReadPram(E2_SLEEP_REPORT_TIME_ID,Buffer))
            {
                AccOffReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_ACC_OFF_REPORT_DISTANCE://休眠汇报距离间隔
        {
            if(E2_SLEEP_REPORT_DISTANCE_LEN == EepromPram_ReadPram(E2_SLEEP_REPORT_DISTANCE_ID,Buffer))
            {
                AccOffReportDistanceSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_UNLOGIN_REPORT_TIME://驾驶员未登录汇报时间间隔
        {
            if(E2_DRIVER_UNLOGIN_REPORT_TIME_LEN == EepromPram_ReadPram(E2_DRIVER_UNLOGIN_REPORT_TIME_ID,Buffer))
            {
                UnloginReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_UNLOGIN_REPORT_DISTANCE://驾驶员未登录汇报距离间隔
        {
            if(E2_DRIVER_UNLOGIN_REPORT_DISTANCE_LEN == EepromPram_ReadPram(E2_DRIVER_UNLOGIN_REPORT_DISTANCE_ID,Buffer))
            {
                UnloginReportDistanceSpace = Public_ConvertBufferToLong(Buffer);
            }
            break;
        }
    case PRAM_SECOND_LINK_ACC_ON_REPORT_TIME://连接2 ACC ON汇报时间间隔
        {
            if(E2_SECOND_ACC_ON_REPORT_TIME_ID_LEN == EepromPram_ReadPram(E2_SECOND_ACC_ON_REPORT_TIME_ID,Buffer))
            {
                Link2AccOnReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            
            break;
        }
    case PRAM_SECOND_LINK_ACC_OFF_REPORT_TIME://连接2 ACC OFF汇报时间间隔
        {
            
            if(E2_SECOND_ACC_OFF_REPORT_TIME_ID_LEN == EepromPram_ReadPram(E2_SECOND_ACC_OFF_REPORT_TIME_ID,Buffer))
            {
                Link2AccOffReportTimeSpace = Public_ConvertBufferToLong(Buffer);
            }
            
            break;
        }
    case PRAM_SECOND_LINK_OPEN_FLAG://连接2 开启标志
        {   
            BufferLen = EepromPram_ReadPram(E2_SECOND_MAIN_SERVER_IP_ID, Buffer);
            if(0 == BufferLen)
            {
                Link2OpenFlag = 0;
            }
            else
            {
                BufferLen = EepromPram_ReadPram(E2_SECOND_MAIN_SERVER_TCP_PORT_ID, Buffer);
                if(E2_SECOND_MAIN_SERVER_TCP_PORT_ID_LEN == BufferLen)
                {
                    TcpPort = Public_ConvertBufferToLong(Buffer);
                    if(0 == TcpPort)
                    {
                        Link2OpenFlag = 0;
                    }
                    else
                    {
                        Link2OpenFlag = 1;
                    }
                }
                else
                {
                    Link2OpenFlag = 0;
                }
        
            }
            
            break;
        }
    case PRAM_REPORT_EVENT_SWITCH://位置上报事件项开关
        {
            
            if(E2_REPORT_FREQ_EVENT_SWITCH_ID_LEN == EepromPram_ReadPram(E2_REPORT_FREQ_EVENT_SWITCH_ID,Buffer))
            {
                ReportEventSwitch = Public_ConvertBufferToLong(Buffer);
            }
            
            break;
        }
#endif    
        default:break;
    }
		
	Link1ReportTimeCount = 0;//设置参数时，定时计数清0		
}

/**************************************************************************
//函数名：Report_UploadPositionInfo
//功能：上传一条位置信息
//输入：通道号
//输出：无
//返回值：0上传成功，1上传失败
//备注：当前的位置信息==位置基本信息+位置附加信息，位置信息最长为150字节
***************************************************************************/
u8 Report_UploadPositionInfo(void)
{
	u8 Buffer[FLASH_BLIND_STEP_LEN+1];
	u8 BufferLen;
        
	BufferLen = Report_GetPositionInfo(Buffer);
        
        if(ACK_OK == RadioProtocol_PostionInformationReport(Buffer,BufferLen))
        {
          return 0;
        }
        else
        {//APP_DEBUG("\r\nReport_UploadPositionInfo写盲区....\r\n");
          Blind_Link1Save(Buffer, BufferLen,CHANNEL_DATA_1); //CHANNEL_DATA_1==0x02
          return 1;
        }
}
/**************************************************************************
//函数名：Report_GetPositionInfo
//功能：获取当前的位置信息
//输入：无
//输出：位置信息
//返回值：位置信息长度
//备注：当前的位置信息==位置基本信息+位置附加信息，位置信息最长为150字节
***************************************************************************/
u16 Report_GetPositionInfo(u8 *pBuffer)
{
	u8 *p = NULL;
	u16 length;

	p = pBuffer;
	length = 0;

	length = Report_GetPositionBasicInfo(p);//获取位置基本信息
	p = p+length;

	length += Report_GetPositionAdditionalInfo(p);//获取位置附加信息

	return length;

}

//取经纬度
static void Report_Callback_Location(s32 result, ST_LocInfo* loc_info)
{
	locinf = *loc_info;
    APP_DEBUG("\r\n<-- Module location: latitude=%f, longitude=%f -->\r\n", loc_info->latitude, loc_info->longitude);
}
static void Report_Get_Location(void)
{
	
	/****************************************************************************
	* Definition for APN
	****************************************************************************/
	#define APN      "CMNET\0"
	#define USERID   ""
	#define PASSWD   ""
	
	s32 ret;
	u8  pdpCntxtId;

	// Set PDP context
	ret = Ql_GPRS_GetPDPContextId();
	APP_DEBUG("<-- The PDP context id available is: %d (can be 0 or 1)-->\r\n", ret);
	if (ret >= 0)
	{
	    pdpCntxtId = (u8)ret;
	} else {
    	APP_DEBUG("<-- Fail to get PDP context id, try to use PDP id(0) -->\r\n");
	    pdpCntxtId = 0;
	}

	ret = RIL_NW_SetGPRSContext(pdpCntxtId);
	APP_DEBUG("<-- Set PDP context id to %d -->\r\n", pdpCntxtId);
	if (ret != RIL_AT_SUCCESS)
	{
	    APP_DEBUG("<-- Ql_RIL_SendATCmd error  ret=%d-->\r\n",ret );
	}

	// Set APN
	ret = RIL_NW_SetAPN(1, APN, USERID, PASSWD);
	APP_DEBUG("<-- Set APN -->\r\n");

	// Request to get location
	{
		APP_DEBUG("<--Ql_Getlocation-->\r\n");
		ret = RIL_GetLocation(Report_Callback_Location);
		if(ret!=RIL_AT_SUCCESS)
		{
			APP_DEBUG("<-- Ql_Getlocation error  ret=%d-->\r\n",ret );
		}
	}

}

/**************************************************************************
//函数名：Report_GetPositionBasicInfo
//功能：获取位置基本信息
//输入：无
//输出：位置基本信息
//返回值：位置基本信息长度
//备注：长度固定为28字节，位置基本信息：报警字、状态字、纬度、经度、高程、速度、方向、时间
***************************************************************************/
u16 Report_GetPositionBasicInfo(u8 *pBuffer)
{
	u8 *p = NULL;
	u32 temp;
	u16 temp2;
	GPS_STRUCT Position;

	p = pBuffer;
	Gps_CopygPosition(&Position);
	
	temp = Io_ReadAlarm();//报警字
	Public_ConvertLongToBuffer(temp,p);
	p += 4;

	temp = Io_ReadStatus();//状态字
	Public_ConvertLongToBuffer(temp,p);
	p += 4;
	
	if(Gps_ReadStatus()){
		temp = (Position.Latitue_FX*100UL+Position.Latitue_F*1000000)/60+(Position.Latitue_D*1000000UL);//纬度
	}else{
		temp = (u32)(locinf.latitude*1000000UL);//纬度
	}
	Public_ConvertLongToBuffer(temp,p);
	p += 4;

	if(Gps_ReadStatus()){
		temp = (Position.Longitue_FX*100UL+Position.Longitue_F*1000000)/60+(Position.Longitue_D*1000000UL);//经度
	}else{
		temp = (u32)(locinf.longitude*1000000UL);//经度
	}
	Public_ConvertLongToBuffer(temp,p);
	p += 4;
	
	temp2 = Position.High+Position.HighOffset;//高程
	Public_ConvertShortToBuffer(temp2,p);
	p += 2;

	temp2 = 10*Gps_ReadSpeed();//速度
	Public_ConvertShortToBuffer(temp2,p);
	p += 2;

	temp2 = Position.Course;//方向
	Public_ConvertShortToBuffer(temp2,p);
	p += 2;
	
	*p++ = Public_HEX2BCD(CurTime.year);//读取时间
	*p++ = Public_HEX2BCD(CurTime.month);
	*p++ = Public_HEX2BCD(CurTime.day);
	*p++ = Public_HEX2BCD(CurTime.hour);
	*p++ = Public_HEX2BCD(CurTime.min);
	*p++ = Public_HEX2BCD(CurTime.sec);
	APP_DEBUG("<--Report Ql_GetLocalTime(%d.%02d.%02d %02d:%02d:%02d) -->\n\r", CurTime.year, CurTime.month, CurTime.day, CurTime.hour, CurTime.min, CurTime.sec);

	return 28;
	
}
/**************************************************************************
//函数名：Report_ClearReportTimeCount
//功能：上报时间计数变量清0
//输入：无
//输出：无
//返回值：无
//备注：两个连接的计数变量都清0
***************************************************************************/
void Report_ClearReportTimeCount(void)
{
	Link1ReportTimeCount = 0;
}

/**************************************************************************
//函数名：Report_GetReportEventSwitch
//功能：获取ReportEventSwitch的值
//输入：无
//输出：无
//返回值：无
//备注：ReportEventSwitch的值等于参数ID 0xF27D的设置值
***************************************************************************/
u32 Report_GetReportEventSwitch(void)
{
	return ReportEventSwitch;
}
/**************************************************************************
//函数名：Report_GetOverSpeedReportTimeSpace
//功能：获取OverSpeedReportTimeSpace的值
//输入：无
//输出：无
//返回值：无
//备注：OverSpeedReportTimeSpace的值等于参数ID 0xF259的设置值
***************************************************************************/
u32 Report_GetOverSpeedReportTimeSpace(void)
{
	return OverSpeedReportTimeSpace;
}
/**************************************************************************
//函数名：Report_GetOverTimeReportTimeSpace
//功能：获取OverTimeReportTimeSpace的值
//输入：无
//输出：无
//返回值：无
//备注：OverTimeReportTimeSpace的值等于参数ID 0xF277的设置值
***************************************************************************/
u32 Report_GetOverTimeReportTimeSpace(void)
{
	return OverTimeReportTimeSpace;
}
/**************************************************************************
//函数名：Report_GetLink1NextReportPoint
//功能：获取连接1的下一个位置汇报点（汇报时间和汇报距离）
//输入：无
//输出：无
//返回值：无
//备注：
***************************************************************************/
static void Report_GetLink1NextReportPoint(u32 *NextReportTime, u32 *NextReportDistance)
{
	
    if((TempTrackTimeSpace > 0)&&(TempTrackCount > 0))//临时跟踪
    {
        *NextReportTime = TempTrackTimeSpace;
	*NextReportDistance = 0;
	Link1ReportDistanceSpace = 0;
    }
	#ifdef PRODUCT_MODEL
	else
	{
		*NextReportTime = AccOffReportTimeSpace;
        *NextReportDistance = 0;
        Link1ReportDistanceSpace = 0;
	}
	#else
    else if(1 == Io_ReadStatusBit(STATUS_BIT_ACC))//ACC ON
    {
        *NextReportTime = AccOnReportTimeSpace;
        *NextReportDistance = 0;
        Link1ReportDistanceSpace = 0;
    }
    else//ACC OFF
    {
        *NextReportTime = AccOffReportTimeSpace;
        *NextReportDistance = 0;
        Link1ReportDistanceSpace = 0;				
    }
	#endif
}

/**************************************************************************
//函数名：Report_GetCurDistance
//功能：获取当前的距离（累计行驶里程）
//输入：无
//输出：无
//返回值：距离（累计行驶里程）
//备注：单位米
***************************************************************************/
//static u32 Report_GetCurDistance(void)
//{
  //dxl,2017.2.24,待完善

	//return 0;
//}
/**************************************************************************
//函数名：Report_GetPositionAdditionalInfo
//功能：获取位置附加信息
//输入：无
//输出：位置附加信息
//返回值：位置附加信息长度
//备注：
***************************************************************************/
static u16 Report_GetPositionAdditionalInfo(u8 *pBuffer)
{
	u8 *p = NULL;
	u16 length;

	p = pBuffer;
	length = 0;

	length = Report_GetPositionStdAdditionalInfo(p);//获取标准的位置附加信息
	p = p+length;

	length += Report_GetPositionEiAdditionalInfo(p);//获取伊爱自定义的位置附加信息

	return length;	
}
//没用到的暂时都填0
//#define ReadMileSum()								(0)
//#define Gps_ReadSpeed()								(0)
#define Area_GetInOutAreaAlarmSubjoinInfo(ptr)		(0)
//#define Gps_ReadStaNum()							(0)


/**************************************************************************
//函数名：Report_GetPositionStdAdditionalInfo
//功能：获取标准的位置附加信息
//输入：无
//输出：标准的位置附加信息
//返回值：标准的位置附加信息长度
//备注：附加信息ID:0x01~0x04,0x11~0x13,0x25,0x2a,0x2b,0x30,0x31
***************************************************************************/
static u16 Report_GetPositionStdAdditionalInfo(u8 *pBuffer)
{
	u8 *p = NULL;
	u16 length;
	u32 temp;
	u16 temp2;
	u32 bit_err;

	p = pBuffer;
	length = 0;



	*p++ = 0x01;//里程，ID为0x01	
	*p++ = 4;
	temp = ReadMileSum();
	Public_ConvertLongToBuffer(temp,p);
	p += 4;
	length += 6;

#if 0
//ID 03 02 没有

	*p++ = 0x03;//行驶记录仪速度，ID为0x03，此处与标准要求不一样，做了处理
	*p++ = 2;//（按广州伊爱需求处理：当选择GPS速度时，若脉冲速度不为0使用脉冲速度，若为0使用GPS速度）
	
	temp2 = 10*Gps_ReadSpeed();
	Public_ConvertShortToBuffer(temp2,p);
	p += 2;
	length += 4;

	///信息ID为0x12///进出区域报警附加信息包8/////////////////
	temp2= Area_GetInOutAreaAlarmSubjoinInfo(p);//进出区域报警;
	length +=temp2;
	p+=temp2;
	
//ID 25 没有

	*p++ = 0x25;//扩展车辆信号状态，ID为0x25	
	*p++ = 4;
	temp = Io_ReadExtCarStatus();
	Public_ConvertLongToBuffer(temp,p);
	p += 4;
	length += 6;

	*p++ = 0x2a;//IO状态，ID为0x2a	
	*p++ = 2;
	temp2 = Io_ReadIoStatus();
	if((1 == Io_ReadStatusBit(STATUS_BIT_ACC))&&(0x02 == (temp2&0x02)))//当出现ACC ON，SLEEP ON这样矛盾的情况时以ACC ON为准
	{
		temp2 = temp2&0xfc;
	}
	Public_ConvertShortToBuffer(temp2,p);
	p += 2;
	length += 4;
//2B 没有

	//temp2 = Ad_GetValue(ADC_EXTERN1);dxl,2017.2.24,待完善
	//temp3 = Ad_GetValue(ADC_EXTERN2);dxl,2017.2.24,待完善
        temp2 = 0;
        temp3 = 0;
	*p++ = 0x2b;//模拟量，ID为0x2b，高16位为模拟量2，低16位为模拟量1
	*p++ = 4;
	Public_ConvertShortToBuffer(temp3,p);
	p += 2;
	Public_ConvertShortToBuffer(temp2,p);
	p += 2;
	length += 6;
#endif
	*p++ = 0x30;//无线网络信号强度，ID为0x30	
	*p++ = 1;
	RIL_NW_GetSignalQuality((u32*)&temp, (u32*)&bit_err);
	*p++ = (u8)temp>0?(u8)temp:0; 
	length += 3;

	*p++ = 0x31;//定位星数，ID为0x31	
	*p++ = 1;
	*p++ = Gps_ReadStaNum();
	length += 3;
	
	return length;
	
}
/**************************************************************************
//函数名：Report_Get_BatteryInfo_Ext_0xE6
//功能：获取电池电压及电量百分比，如果只有电压信息，则只有2个字节
//输入：pBuffer--->数据指针mode:1  有电量百分比信息,   0   没有
//输出：伊爱自定义的位置附加信息
//返回值：伊爱自定义的位置附加信息长度
//备注：
***************************************************************************/
static u32 Report_Get_BatteryInfo_Ext_0xE6(u8 *pBuffer,u8 mode)
{
	u8*p = pBuffer;

	*p++ = 0xE6;
	if(mode)
	{
		*p++ = 0x4;
	}
	else
	{
		*p++ = 0x2;
	}
	Public_ConvertShortToBuffer((unsigned short)(battery_voltage/10),p);
	Public_ConvertShortToBuffer((unsigned short)battery_capacity,p+2);

	return mode?(2+4):(2+2);
}
/**************************************************************************
//函数名：Report_GetPositionEiAdditionalInfo
//功能：获取伊爱自定义的位置附加信息
//输入：无
//输出：伊爱自定义的位置附加信息
//返回值：伊爱自定义的位置附加信息长度
//备注：
***************************************************************************/
static u16 Report_GetPositionEiAdditionalInfo(u8 *pBuffer)
{
	u8 length;
	u8 *p = NULL;
	u32 temp;
	u8 VerifyCode;

	length = 0;
	p = pBuffer;

	p += 2;//空开伊爱附加ID，标志符1字节，长度1字节

	*p++ = 0xE1;//自定义状态字2，ID为0xE1
	*p++ = 4;
	temp = Io_ReadSelfDefine2();
	Public_ConvertLongToBuffer(temp,p);
	p += 4;
	length += 6;

 	temp = Report_Get_BatteryInfo_Ext_0xE6(p,1);
	p += temp;
	length += temp;
	
	temp = PackgQENG_Info(p);//获取基站信息，只有在不导航的时候才上传基站
	p += temp;
	length += temp;
	
	VerifyCode = Public_GetSumVerify(pBuffer+2,length);//校验和
	*p++ = VerifyCode;
	length++;

	*pBuffer = 0xF0;//伊爱扩展附加信息标志
	*(pBuffer+1) = length;

	return length+2;
	
}





