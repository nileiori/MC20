//********************************头文件************************************

#include "include.h"
#include "report.h"
//********************************变量定义**********************************
#define SYS_USE_BIG_LITTLE_FLAG   0//系统大端小端标志,0为小端
static u8 RadioProtocolRxBuffer[RADIOPROTOCOL_BUFFER_SIZE];//当接收缓冲的数据全被解析完时,忙标志清0;在解析时,忙标志置1
static u8 RadioProtocolTxBuffer[RADIOPROTOCOL_BUFFER_SIZE];//平台数据发送缓冲区
u8 ShareBuffer[RADIOPROTOCOL_BUFFER_SIZE];

//********************外部模块调用的变量*******************
extern u8	TerminalAuthorizationFlag;//终端鉴权标志,鉴权成功为1,失败为0
extern u16 DelayAckCommand;//延时应答命令

static unsigned char TerminalPhoneNumber[6] = {0};//终端手机号
static unsigned short RadioProtocolRxLen = 0;//RadioProtocolRxBuffer接收数据长度
static unsigned short MessageSerialNum = 0;//消息流水号
static unsigned short s_usRecvSerialNum=0;
static unsigned short s_usRecvMessageID=0;
static unsigned char s_ucCurRecvChanel = CHANNEL_DATA_1;	//串口2接收
unsigned char s_ucRecvChanel = CHANNEL_DATA_1;	//串口2接收
static unsigned char s_ucRecvTimeCnt = 0;//解析是否需要等待
static unsigned char s_ucUpdataResultNoteAckFlag = 0;//升级结果通知应答标志,1为收到平台的通用应答了
u16     CarControlAckSerialNum = 0;//车辆控制应答流水号
u8	DelaySendChannel = 0;//延时发送通道号
static MESSAGE_HEAD	RadioProtocolRxHead;//接收的二进制数据经解析后得到的帧头放在此结构体中


//注册应答
/*********************************************************************
//函数名称	:RadioProtocol_ServerGeneralAck(u8 *pBuffer, u16 BufferLen)
//功能		:平台（服务器）通用应答
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8001
*********************************************************************/
ProtocolACK RadioProtocol_ServerGeneralAck(u8 *pBuffer, u16 BufferLen)
{
    u16	SerialNum;
    u16	ID;
    u8	Result;
    u8	*p;
    u8	HighByte;
    u8	LowByte;
    u8      Buffer[3] = {0};
    static u8	Count = 0;
    p = pBuffer;
	

    //判断长度是否正确
    if((5 != BufferLen))
    {
        return ACK_ERROR;
    }
    /////////////////////////////
    //读取应答流水号
    HighByte = *p++;
    LowByte = *p++;
    SerialNum = (HighByte << 8)|LowByte;
    if(0 == SerialNum)
    {
        //为编译不报错
    }

    //读取应答ID
    HighByte = *p++;
    LowByte = *p++;
    ID = (HighByte << 8)|LowByte;

    //判断是否为盲区补报应答
    if(0x0704 == ID)
    {
        Blind_Link1ReportAck();
				//Blind_ClrLink1ReportAck();//清除盲区上报应答标志
				//Blind_UpdateCirQueueInfoWhenEmpty();
				//Ql_Timer_Stop(LOGIC_BLIND_REPORT_TMR_ID);
				//Blind_GiveSemaphore();//释放信号量
    }
    else
    if(0x200==ID)
    {
       Report_CmdAck();
    }
    //判断是否为鉴权应答
    else if(0x0102 == ID)
    {
        //读取结果
        Result = *p;
        if(0 == Result)
        {
            //成功
            Count++;
			APP_DEBUG("<-- 鉴权成功-->\r\n");
            //SetTerminalAuthorizationFlag(CHANNEL_DATA_1);
            //显示鉴权成功
            //Blind_StartLink1Report();//开启盲区补报
            //Report_UploadPositionInfo();
			
			//Report_SetReportStatus(ReportStatus_Init);
            //SetEvTask(MSG_Report_EvTask);//鉴权成功后立马上报1条位置信息,dxl,2014.2.14
        }
        else
        {
			APP_DEBUG("<-- 鉴权失败-->\r\n");
            //清标志
            ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);
            ClearTerminalAuthorizationCode(CHANNEL_DATA_1);
        }
    }
    else if(0x0003 == ID)//终端注销
    {
        //读取结果
        TerminalAuthorizationFlag = 0;
    }
    else if(0x0108 == ID)//终端升级结果通知
    {
		s_ucUpdataResultNoteAckFlag=1;
    }
    else
    {
        if(Count >= 1)
        {
            //全部显示
            //SetMainFullDisplayFlag();
            //开启主界面显示
            //ClrDisplayControlFlag();
        }
        Count = 0;
        //读取结果
        Result = *p;

        if(0 == Result)
        {
            //成功
        }
        else if(1 == Result)
        {
            //失败
        }
        else if(2 == Result)
        {
            //消息有误
        }
        else if(3 == Result)
        {
            //不支持
        }
        else if(4 == Result)//报警处理确认
        {
            //清楚所有需人工确认的报警
            //清除紧急报警
            Io_WriteAlarmBit(ALARM_BIT_EMERGENCY, RESET);
            //清除危险预警
            Io_WriteAlarmBit(ALARM_BIT_DANGER_PRE_ALARM, RESET);
            //清除进出区域报警
            Io_WriteAlarmBit(ALARM_BIT_IN_OUT_AREA, RESET);
            //清除进出线路报警
            Io_WriteAlarmBit(ALARM_BIT_IN_OUT_ROAD, RESET);
            //清除线路行驶过长或不足报警
            Io_WriteAlarmBit(ALARM_BIT_LOAD_OVER_DRIVE, RESET);

            //清除非法点火
            Io_WriteAlarmBit(ALARM_BIT_LAWLESS_ACC, RESET);
            //清除非法位移
            Io_WriteAlarmBit(ALARM_BIT_LAWLESS_MOVEMENT, RESET);
            //清除非法开车门
            Io_WriteAlarmBit(ALARM_BIT_LAWLESS_OPEN_DOOR, RESET);

        }
    }
    
    if(0 == Buffer[0])
    {
    
    }
    return ACK_OK;
}

/*********************************************************************
//函数名称	:RadioProtocol_TerminalRegisterAck(u8 *pBuffer, u16 BufferLen)
//功能		:终端注册应答
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8100
*********************************************************************/
ProtocolACK RadioProtocol_TerminalRegisterAck(u8 *pBuffer, u16 BufferLen)
{
	u8	ucTmp[30],len;

	if((BufferLen < 3)||(BufferLen > LOGIN_PASSWORD_LEN+1))return ACK_ERROR;
	//判断结果
	if(0 != pBuffer[2])
    {
    APP_DEBUG("<-- 注册失败-->\r\n");
        //RadioProtocol_TerminalLogout();
		RadioProtocol_SendData(0x0003,NULL,0,CHANNEL_DATA_1);//功能		:终端注销//备注		:指令ID：0x0003
        return ACK_ERROR;
    }
    //读取鉴权码
    APP_DEBUG("<-- 注册成功，鉴权码:%s鉴权码长度%d-->\r\n",&pBuffer[3],BufferLen-3);
    EepromPram_WritePram(E2_LOGINGPRS_PASSWORD_ID, &pBuffer[3], BufferLen-3);
	len=EepromPram_ReadPram(E2_LOGINGPRS_PASSWORD_ID, ucTmp);
	ucTmp[len]='\0';
    APP_DEBUG("<-- 注册成功，鉴权码:%s鉴权码长度%d-->\r\n",ucTmp,len);
	SetLoginAuthorizationState(LOGIN_SEND_AUTHORIZATION_CMD);	
	return ACK_OK;
}

/*********************************************************************
//函数名称	:RadioProtocol_PramSet(u8 *pBuffer, u16 BufferLen)
//功能		:设置终端参数
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8103
*********************************************************************/
ProtocolACK RadioProtocol_PramSet(u8 *pBuffer, u16 BufferLen)
{

	s16	TotalPramCount;//参数总数
	//s16	PacketPramCount;//包参数个数
	u8	*p;
	s16	length;
	u8 	AckResult = 0;
	
	p = pBuffer;
	
	//长度不允许超过1024
	//获取长度
	length = BufferLen;

	//读取参数总数
	TotalPramCount = *p++;
	length--;
	
	if(0 == TotalPramCount)
	{
		return ACK_ERROR;
	}
	RadioProtocol_WritePram(p, length);
	//应答结果
	AckResult = 0;

	return RadioProtocol_TerminalGeneralAck(AckResult);
	
}

/*********************************************************************
//函数名称	:RadioProtocol_PramSearch(u8 *pBuffer, u16 BufferLen)
//功能		:查询终端参数
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8104
*********************************************************************/
ProtocolACK RadioProtocol_PramSearch(u8 *pBuffer, u16 BufferLen)
{

    u8 RadioShareBuffer[RADIOPROTOCOL_BUFFER_SIZE];
    u8	*p = NULL;
    u8	*p1 = NULL;
    u8	AckPramCount = 0;//包参数个数
    s16	length = 0;
    
    //长度不允许超过1024
    //获取应答流水号
    //AckSerialNum = RadioProtocolRxHead.SerialNum;
    //应答流水号写入缓冲
    p = RadioShareBuffer;
    *p++ = (s_usRecvSerialNum & 0xff00) >> 8;
    *p++ = s_usRecvSerialNum & 0xff;
    *p++;//预留1个字节存储应答参数个数
    if(0 == BufferLen)//读取全部的参数
    {
        AckPramCount=E2ParamApp_DisposeReadAllParam(p,&length);
    }
    else//读取指定的参数
    {
        p1 = pBuffer;//第1个为需要查询的参数总个数
        p1++;//略过
        AckPramCount = RadioProtocol_ReadPram(p, &length, p1, BufferLen-1);
    }
    //写入包参数个数
    *(RadioShareBuffer+2) = AckPramCount;
    //长度加上应答流水号,应答参数个数
    length += 3;
    //消息头
    return RadioProtocol_SendData(0x0104,RadioShareBuffer,length,s_ucRecvChanel);
   
}
/*********************************************************************
//函数名称	:RadioProtocol_TerminalControl(u8 *pBuffer, u16 BufferLen)
//功能		:终端控制
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8105
*********************************************************************/
ProtocolACK RadioProtocol_TerminalControl(u8 *pBuffer, u16 BufferLen)
{
    u8 Command;
    u8	*p;
    s16	length;
    u8	AckResult;
    u8	ControlLinkWord;
    u8	Buffer[30];
    u8	ErrorFlag = 0;
	u32	i,j;
    u8	Count;
    u16	PramLen;
    ////////////
    AckResult = 1;
    p = pBuffer;
    length = BufferLen;

    //读取命令字
    Command = *p++;
    length--;

    if(1 == Command)//无线升级
    {
        //参数依次为：URL地址;拨号点名称;拨号用户名;拨号密码;地址;TCP端口;
        //UDP端口;制造商ID;硬件版本;固件版本;连接到指定服务器时限
        i = 0;
        j = 0;
        Count = 0;
        while(length > 2)
		{
					if(';' == *(p+i))//找到了分割符
					{	
						Count++;
						PramLen = i-j;
						switch(Count)
						{
		
							case 1://URL地址
							{
								if(Net_Ftp_Parameter(1,p+j,PramLen))
								{
									ErrorFlag =1;
								}
								break;
							}
							case 2://拨号点名称
							{
								if(Net_Ftp_Parameter(2,p+j+1,PramLen-1))
								  {
									  ErrorFlag =1;
								  }
							   break;
							}
							case 3://拨号用户名
							{
								if(Net_Ftp_Parameter(3,p+j+1,PramLen-1))
								  {
									  ErrorFlag =1;
								  }
								break;
							}
							case 4://拨号密码
							{
								if(Net_Ftp_Parameter(4,p+j+1,PramLen-1))
								  {
									  ErrorFlag =1;
								  } 					   
								break;
							}
							case 5://地址
							{
								if(Net_Ftp_Parameter(5,p+j+1,PramLen-1))//liamtsen
								{
									ErrorFlag =1;
								}
								break;
							}
							case 6://TCP端口
							{
								if(Net_Ftp_Parameter(6,p+j+1,PramLen-1))//liamtsen
								{
									ErrorFlag =1;
								}
								break;
							}
							case 7://UDP端口
							{
								//EepromPram_WritePram(E2_UPDATA_UDP_PORT_ID, p+j+1, PramLen-1); //dxl,2017.2.23,待完善
								//EepromPram_ReadPram(E2_UPDATA_UDP_PORT_ID, Buffer); 
		//						  if(Public_CheckArrayValIsEqual(p+j+1, Buffer,PramLen-1))
		//						  {
		//							  ErrorFlag =1;
		//						  }
								break;
							}
							case 8://制造商ID
							{
								//EepromPram_WritePram(E2_UPDATA_MANUFACTURE_ID, p+j+1, PramLen-1); //dxl,2017.2.23,待完善
								//EepromPram_ReadPram(E2_UPDATA_MANUFACTURE_ID, Buffer); 
		//						  if(Public_CheckArrayValIsEqual(p+j+1, Buffer,PramLen-1))
		//						  {
		//							  ErrorFlag =1;
		//						  }
								break;
							}
							case 9://硬件版本
							{
								//EepromPram_WritePram(E2_UPDATA_HARDWARE_VERSION_ID, p+j+1, PramLen-1); //dxl,2017.2.23,待完善
								//EepromPram_ReadPram(E2_UPDATA_HARDWARE_VERSION_ID, Buffer);
		//						  if(Public_CheckArrayValIsEqual(p+j+1, Buffer,PramLen-1))
		//						  {
		//							  ErrorFlag =1;
		//						  }
								break;
							}
							case 10://固件版本
							{
								//EepromPram_WritePram(E2_UPDATA_FIRMWARE_VERSION_ID, p+j+1, PramLen-1); //dxl,2017.2.23,待完善
								//EepromPram_ReadPram(E2_UPDATA_FIRMWARE_VERSION_ID, Buffer);
		//						  if(Public_CheckArrayValIsEqual(p+j+1, Buffer,PramLen-1))
		//						  {
		//							  ErrorFlag =1;
		//						  }
								break;
							}
							default :
								break;
						}
						j = i;
					}
					i++;
					length--;	
					if(10 == Count)
					{
						break;
					}
				}
		        if(0 == ErrorFlag)//只有数据全部正确了才会开启FTP远程升级,并应答
		        {
		            if(Net_Other_FtpStart())
		            {
		                AckResult = 0;
		                Buffer[0] = 3;//升级标志 失败 3
		                FRAM_BufferWrite(FRAM_FIRMWARE_UPDATA_FLAG_ADDR,Buffer,1);
						RadioProtocol_TerminalGeneralAck(AckResult);    
						Net_First_Close();
						 ftp_downfile_timer();
						 return ACK_OK;
					}
		        }
    }
    else if(2 == Command)//控制终端连接指定服务器
    {
        //参数依次为：连接控制;监管平台鉴权码;拨号点名称;拨号用户名;拨号密码;地址;TCP端口;
        //UDP端口;连接到指定服务器时限
        //读取连接控制字
        //ControlLinkWord = *p;
        AckResult = 0;
                
    }
	else if(3 == Command)//硬件关机
    {
        
        AckResult = 0;
        DelayAckCommand = 12;//关机
        //触发延时任务
        SetTimerTask(TIME_DELAY_TRIG,3*SECOND);
                
    }
	else
    if(4 == Command)//终端复位
    {
         AckResult = 0;
        //if(1 == Io_ReadStatusBit(STATUS_BIT_ACC))
        {            
            DelayAckCommand = 7;//下线，重启
        }
        //else
        //{            
        //    DelayAckCommand = 11;//下线，进入深度休眠        
        //}
        //触发延时任务
        SetTimerTask(TIME_DELAY_TRIG,3*SECOND);   
	}
    else 
    if(5 == Command)//恢复出厂设置
    {
        AckResult = 0;
        EepromPram_DefaultSet();
        DelayAckCommand = 7;//下线，重启
        //触发延时任务
        SetTimerTask(TIME_DELAY_TRIG,3*SECOND);
	}
    
    RadioProtocol_TerminalGeneralAck(AckResult);    
	
    return ACK_OK;

}
/*********************************************************************
//函数名称	:RadioProtocol_TerminalAttribute(u8 *pBuffer, u16 BufferLen)
//功能		:查询终端属性
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8107
*********************************************************************/
/*
ProtocolACK RadioProtocol_TerminalAttribute(u8 *pBuffer, u16 BufferLen)
{
    u8	*p;
    u8	Buffer[150];
    u8	Buffer2[50];
    u8	Buffer3[50];
    u8	PramLen = 0;
    u8	SmsPhone[20];
    u8	channel = 0;
    u16	length = 0;
    u8	tmplen = 0;


    MESSAGE_HEAD	head;

    p = Buffer;
    length = 0;

    //读取终端类型 Bytes:2 
    //默认为适用于客运、危险品、货运，类型：一体机
    Buffer2[0] = 0;
    Buffer2[1] = 0x07;
  
    memcpy(p,Buffer2,2);
    p += 2;
    length += 2;

    PramLen = EepromPram_ReadPram(E2_MANUFACTURE_ID, Buffer2); 
    if(E2_MANUFACTURE_ID_LEN == PramLen)
    {
        memcpy(p,Buffer2,E2_MANUFACTURE_ID_LEN);
    }
    else
    {
        memcpy(p,"70108",5);
    }
    //////////////////
    p += 5; 
    length += 5;
    //////////////////////
    //读取终端型号
    memset(Buffer2,0,20);
    Version_GetProductSeriesName((char *)Buffer2);
    memcpy(p,Buffer2,20);
    p += 20;
    length += 20;
    //读取终端ID
    PramLen = EepromPram_ReadPram(E2_DEVICE_ID, Buffer2);
    if(E2_DEVICE_ID_LEN == PramLen)//取终端ID后面7位作为登录注册的终端ID
    {
        Public_ConvertBcdToAsc(Buffer3,&Buffer2[3],4);
    }
    else
    {

        memset(Buffer3,0,10);
        strcpy((char *)&Buffer3[1],"3100028");
    }
    ///读取终端ID///////////////
    memcpy(p,&Buffer3[1],7);
    p += 7;
    length += 7;
    //终端SIM卡ICCID BCD[10]  Bytes:10
    MOD_GetIccidBcdValue(Buffer2);
    memcpy(p,Buffer2,10 );
    p += 10 ;
    length += 10 ;

    //终端硬件版本长度 BYTE  Bytes:1
    tmplen = Version_GetHardwareInformation(Buffer2);
    *p++ = tmplen;
    length++;
    //终端硬件版本
    memcpy(p,Buffer2,tmplen);//HUOYUN_DEBUG_OPEN货运平台调试前该行没有，为后来新增
    p += tmplen;
    length += tmplen;
    //终端固件版本长度 BYTE  Bytes:1
    tmplen = Version_GetFirmwareInformation(Buffer2);
    *p++ = tmplen;
    length++;
    //终端固件版本号 STRING
    memcpy(p,Buffer2,tmplen);
    p += tmplen ;
    length += tmplen;

    //GNSS模块属性
   // if(E2_GPS_ATTRIBUTE_ID_LEN == EepromPram_ReadPram( E2_GPS_ATTRIBUTE_ID , Buffer2  ))
   // {
      //  *p = Buffer2[0];
      //  p++;
      //  length++;  
   // }
    //else
   // {
        *p = 0x03;//没有设置过的化是支持GPS+北斗
        p++;
        length++;  
   // }
    //通信模块属性
    *p = 0x01;//GPRS通讯
    p++;
    length++; 

    //消息ID
    head.MessageID = 0x0107;
    head.Attribute = (length & 0x3ff);

    //判断是否为短消息
    if( CHANNEL_SMS == s_ucRecvChanel)
    {
        channel = CHANNEL_SMS;
    }

    if(SUCCESS == RadioProtocol_SendCmd(head, Buffer, length, channel))
    {
        return ACK_OK;
    } 
    else 
    {
        return ACK_ERROR;

    } 

}
*/
/*********************************************************************
//函数名称	:RadioProtocol_PositionSearch(u8 *pBuffer, u16 BufferLen)
//功能		:位置信息查询
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8201
*********************************************************************/
ProtocolACK RadioProtocol_PositionSearch(u8 *pBuffer, u16 BufferLen)
{
	u16	length;
	u8	Buffer[150];
	u8	PramLen;
	
	if(0 != BufferLen)
	{
		return ACK_ERROR;
	}
	length = 0;
	//添加应答流水号
	//AckSerialNum = RadioProtocolRxHead.SerialNum;
    Public_ConvertShortToBuffer(s_usRecvSerialNum, Buffer);
	length += 2;
	//获取位置信息,拷贝到Buffer中
	PramLen = Report_GetPositionInfo(Buffer+2);
	length += PramLen;
    
	//发送位置汇报信息
	return RadioProtocol_SendData(0x0201,Buffer,length,s_ucRecvChanel);
	//消息ID	
}

/*********************************************************************
//函数名称	:RadioProtocol_TempPositionControl(u8 *pBuffer, u16 BufferLen)
//功能		:临时位置跟踪控制
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8202
*********************************************************************/
ProtocolACK RadioProtocol_TempPositionControl(u8 *pBuffer, u16 BufferLen)
{

	u8	*p;
	u8	AckResult;
	u16 TimeSpace;//时间间隔
	u32 ValidTime;//有效期
	u8	Buffer[5];
	
	

	p = pBuffer;
	
	//if(6 != BufferLen),有误需屏蔽,当要停止跟踪控制时是2字节,HUOYUN_DEBUG_OPEN测试时有问题
	//{
		//AckResult = 2;
		//return RadioProtocol_TerminalGeneralAck(AckResult);
	//}
	
	//写入临时跟踪时间间隔
	TimeSpace = 0;
	TimeSpace |= *p << 8;
	Buffer[0] = *p;
	p++;
	TimeSpace |= *p;
	Buffer[1] = *p;
	p++;
	/*if(TimeSpace>30)//时间间隔太大返回消息有误
	{
		return RadioProtocol_TerminalGeneralAck(0x2);
	}*/
	FRAM_BufferWrite(FRAM_TEMP_TRACK_SPACE_ADDR, Buffer, FRAM_TEMP_TRACK_SPACE_LEN);
	Buffer[2] = Buffer[0];
	Buffer[3] = Buffer[1];
	Buffer[0] = 0;
	Buffer[1] = 0;
	EepromPram_WritePram(E2_ACCON_REPORT_TIME_ID, Buffer, E2_ACCON_REPORT_TIME_LEN);
	Report_UpdateOnePram(PRAM_TEMP_TRACK_TIME);
	if(6 == BufferLen)
	{
		//写入临时跟踪有效期
		ValidTime = 0;
		ValidTime |= *p++ << 24;
		ValidTime |= *p++ << 16;
		ValidTime |= *p++ << 8;
		ValidTime |= *p;
		ValidTime = ValidTime/TimeSpace;
		Buffer[0] = (ValidTime&0xff000000) >> 24;
		Buffer[1] = (ValidTime&0xff0000) >> 16;
		Buffer[2] = (ValidTime&0xff00) >> 8;
		Buffer[3] = ValidTime&0xff;
		FRAM_BufferWrite(FRAM_TEMP_TRACK_NUM_ADDR, Buffer, FRAM_TEMP_TRACK_NUM_LEN);
		Report_UpdateOnePram(PRAM_TEMP_TRACK_COUNT);

		Report_PramBackup();

		DelayAckCommand = 14;//上报
        SetTimerTask(TIME_DELAY_TRIG, 3*SECOND);
	}

	AckResult = 0;

	return RadioProtocol_TerminalGeneralAck(AckResult);
}

/*********************************************************************
//函数名称	:RadioProtocol_AlarmConfirmManual(u8 *pBuffer, u16 BufferLen)
//功能		:人工确人报警
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:  成功,返回ACK_OK;失败返回ACK_ERROR;
//备注		:指令ID：0x8203
*********************************************************************/
ProtocolACK RadioProtocol_AlarmConfirmManual(u8 *pBuffer, u16 BufferLen)
{

  u8 *p =NULL;
  u8 AckResult;
  static u16 AlarmSerialNum;
  static u32 AlarmType;
  u8 flag = 0;  
		
  if( BufferLen != 6 )
  { 	
	  AckResult = 1;			  
  }
  else
  {


	  p = pBuffer;

	  /*** 需要人工确认的报警流水号, 0表示报警所有消息	***/
	  AlarmSerialNum = 0;
	  AlarmSerialNum = *p++;
	  AlarmSerialNum <<=8;
	  AlarmSerialNum += *p++;
		
	  /*** 人工确认报警类型  ***/
	  
	   AlarmType = 0;
	   AlarmType += *p++;
		
	   AlarmType <<= 8;
	   AlarmType += *p++;
		 
	   AlarmType <<= 8;    
	   AlarmType += *p++;
		
	   AlarmType <<= 8;
	   AlarmType += *p++;
	   
	   
	   if( AlarmType & ( 1<< 0 ) ) {
		Io_WriteAlarmBit( ALARM_BIT_EMERGENCY , RESET );	//0 1：紧急报警,收到应答后清除
			flag = 0;
			FRAM_BufferWrite(FRAM_EMERGENCY_FLAG_ADDR, &flag, FRAM_EMERGENCY_FLAG_LEN);//清除铁电存储标志
	   
	   }
	   
	  if( AlarmType & ( 1<< 3 ) ) {
		  Io_WriteAlarmBit( ALARM_BIT_DANGER_PRE_ALARM , RESET );		 //3	1：危险预警,收到应答后清零
	   
	   }
	   
	   
	  if( AlarmType & ( 1<< 20 ) ) {
		  Io_WriteAlarmBit( ALARM_BIT_IN_OUT_AREA , RESET );		//20	1：进出区域,收到应答后清零
	   
	   }  
		
	  if( AlarmType & ( 1<< 21 ) ) {
		  Io_WriteAlarmBit( ALARM_BIT_IN_OUT_ROAD , RESET );		//21	1：进出路线,收到应答后清除
	   
	   } 
		
		
	  if( AlarmType & ( 1<< 22 ) ) {
		  Io_WriteAlarmBit( ALARM_BIT_LOAD_OVER_DRIVE , RESET );			//22	1：路段行驶时间不足/过长,收到应答后清除
		
	   }
		
		
	  if( AlarmType & ( 1<< 27 ) ) {
		if(1 == Io_ReadAlarmBit(ALARM_BIT_LAWLESS_ACC))
		{
			 // LawlessAccAlarmEnableFlag = 0;
		  Io_WriteAlarmBit( ALARM_BIT_LAWLESS_ACC , RESET );		//27	1：车辆非法点火,收到应答后清零
			  
		}
		
	   }	
		
		
	  if( AlarmType & ( 1<< 28 ) ) {
		Io_WriteAlarmBit( ALARM_BIT_LAWLESS_MOVEMENT , RESET ); 		//28	1：车辆非法位移,收到应答后清零
	   
	   }
	   
	   
	   AckResult = 0;		

  }

  return RadioProtocol_TerminalGeneralAck(AckResult);
}

/*********************************************************************
//函数名称	:RadioProtocol_CarControl(u8 *pBuffer, u16 BufferLen)
//功能		:车辆控制
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8500
*********************************************************************/
ProtocolACK RadioProtocol_CarControl(u8 *pBuffer, u16 BufferLen) 
{
    u8 ControlFlag;
    u8 AckResult;
    ControlFlag = *pBuffer;
    if(PUBLIC_CHECKBIT(ControlFlag,6))
    {
        if(PUBLIC_CHECKBIT(ControlFlag,7))//断油
        {
            //OilControl_OffOil();
        }
        else//通油
        {
            //OilControl_OpenOil();
        }
    }
    else
    if(0x01 == (ControlFlag & 0x01))
    {
        //触发车门加锁任务
        //CtrlLock(1);
    }
    else
    {
        //触发车门解锁任务
        //CtrlLock(0);
    }
    //记下车辆控制应答流水号
    if(GetTimerTaskTimerState(TIME_DELAY_TRIG) == DISABLE)
    {
        CarControlAckSerialNum = RadioProtocolRxHead.SerialNum;
        DelayAckCommand = 0x0500;
        DelaySendChannel = s_ucRecvChanel;
        if(CHANNEL_SMS == DelaySendChannel)
        {
        }
        //触发延时任务
        SetTimerTask(TIME_DELAY_TRIG, PUBLICSECS(3));//2秒后应答,主要是为了确保锁车/解锁后的状态能采集到
        //只有应答
        AckResult = 0;
        return RadioProtocol_TerminalGeneralAck(AckResult);
    }
    return ACK_OK;
}
/*********************************************************************
//函数名称	:RadioProtocol_SetRoundArea(u8 *pBuffer, u16 BufferLen)
//功能		:设置圆形区域
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8600
*********************************************************************/
ProtocolACK RadioProtocol_SetRoundArea(u8 *pBuffer, u16 BufferLen)
{
    u8 AckResult = 0;
    //AckResult = Area_SetRoundArea(pBuffer,BufferLen);
    return RadioProtocol_TerminalGeneralAck(AckResult);
}
/*********************************************************************
//函数名称	:RadioProtocol_DelRoundArea(u8 *pBuffer, u16 BufferLen)
//功能		:删除圆形区域
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8601
*********************************************************************/
ProtocolACK RadioProtocol_DelRoundArea(u8 *pBuffer, u16 BufferLen) //SSSS
{
    u8 AckResult = 0;
    //AckResult = Area_DelRoundArea(pBuffer,BufferLen);
    return RadioProtocol_TerminalGeneralAck(AckResult);
}
/*********************************************************************
//函数名称	:RadioProtocol_SetRectangleArea(u8 *pBuffer, u16 BufferLen)
//功能		:设置矩形区域
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8602
*********************************************************************/
ProtocolACK RadioProtocol_SetRectangleArea(u8 *pBuffer, u16 BufferLen)
{
    u8 AckResult = 0;
   	//AckResult = Area_SetRectArea(pBuffer,BufferLen);
    return RadioProtocol_TerminalGeneralAck(AckResult);
}
/*********************************************************************
//函数名称	:RadioProtocol_DelRectangleArea(u8 *pBuffer, u16 BufferLen)
//功能		:删除矩形区域
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8603
*********************************************************************/
ProtocolACK RadioProtocol_DelRectangleArea(u8 *pBuffer, u16 BufferLen) //SSSS
{
    u8 AckResult = 0;
    //AckResult = Area_DelRectArea(pBuffer,BufferLen);
    return RadioProtocol_TerminalGeneralAck(AckResult);
}
/*********************************************************************
//函数名称	:RadioProtocol_SetPolygonArea(u8 *pBuffer, u16 BufferLen)
//功能		:设置多边形区域
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8604
*********************************************************************/
ProtocolACK RadioProtocol_SetPolygonArea(u8 *pBuffer, u16 BufferLen)
{
    u8	AckResult = 3;//多边形暂时不支持
   // AckResult = Polygon_SetArea(pBuffer,BufferLen); 
    return RadioProtocol_TerminalGeneralAck(AckResult);
}
/*********************************************************************
//函数名称	:RadioProtocol_DelPolygonArea(u8 *pBuffer, u16 BufferLen)
//功能		:删除多边形区域
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8605
*********************************************************************/
ProtocolACK RadioProtocol_DelPolygonArea(u8 *pBuffer, u16 BufferLen)
{
    u8	AckResult = 3;//多边形暂时不支持
    //AckResult = Polygon_DeleteArea(pBuffer,BufferLen);
    return RadioProtocol_TerminalGeneralAck(AckResult);
}


/*********************************************************************
//函数名称	:RadioProtocol_OriginalDataDownTrans(u8 *pBuffer, u16 BufferLen)
//功能		:数据下行透传
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8900
*********************************************************************/
ProtocolACK RadioProtocol_OriginalDataDownTrans(u8 *pBuffer, u16 BufferLen)
{
/*
    u8 AckResult;
    //只有应答,先暂时调试
    u8 flag;
    flag=*pBuffer;
    //////////////////////////
    #ifdef USE_EIEXPAND_PROTOCOL
    if(EIEXPAND_PROTOCOL_TRAN_TYPE == flag)//伊爱扩展协议透传类型
    {
        AckResult=EIExpand_RadioProtocolParse(pBuffer+1,BufferLen-1);
        if(0xff==AckResult)return ACK_OK;
    }
    #endif
    return RadioProtocol_TerminalGeneralAck(AckResult);
    */
        return ACK_OK;
}



//*****************第二部分：标准的终端上发指令**********************
/*********************************************************************
//函数名称	:RadioProtocol_TerminalGeneralAck(u16 SerialNum, u16 MessageID, u8 Result)
//功能		:终端通用应答
//输入		:Result:应答结果,0为成功，1为失败，2消息有误，3不支持
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:成功,返回ACK_OK;失败返回ACK_ERROR;
//备注		:指令ID：0x0001
*********************************************************************/
ProtocolACK RadioProtocol_TerminalGeneralAck(u8 AckResult)
{
	u8 Buffer[5];
	//消息内容
	Buffer[0] = (s_usRecvSerialNum & 0xff00) >> 8;
	Buffer[1] = s_usRecvSerialNum;
	Buffer[2] = (s_usRecvMessageID & 0xff00) >> 8;
	Buffer[3] = s_usRecvMessageID;
	Buffer[4] = AckResult;
    return RadioProtocol_SendData(0x0001,Buffer,5,s_ucRecvChanel);
}
const char* c_terminalModel = "VTKG-21N";
/*********************************************************************
//函数名称	:RadioProtocol_TerminalRegister(void)
//功能		:终端注册
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:  成功,返回ACK_OK;失败返回ACK_ERROR;
//备注		:指令ID：0x0100
*********************************************************************/
ProtocolACK RadioProtocol_TerminalRegister(void)
{

	u8	*p=NULL;
	u8	*q=NULL;
	u8	Buffer[150] = {0};
	u8	Buffer2[30] = {0};
	u8	Buffer3[50] = {0};
	u8	PramLen = 0;
	u8	SmsPhone[20] = {0};
	u8	channel = 0;
	u16	length = 0;
	u8	BcdHigh = 0;
	u8	BcdLow = 0;
	u8	value = 0;
	MESSAGE_HEAD	head;
	u32	ID = 0;
	p = Buffer;
	length = 0;
	
	

	//读取省域ID
	PramLen = EepromPram_ReadPram(E2_CAR_PROVINCE_ID, Buffer2);
	if(2 == PramLen)
	{
		*p++ = Buffer2[0];
		*p++ = Buffer2[1];
		length += 2;
	}
	else
	{
		*p++ = 0;
		*p++ = 0;
		length += 2;
	}
	

	//读取市域ID
	PramLen = EepromPram_ReadPram(E2_CAR_CITY_ID, Buffer2);
	if(2 == PramLen)
	{
		*p++ = Buffer2[0];
		*p++ = Buffer2[1] & 0xff;
		length += 2;
	}
	else
	{
		*p++ = 0;
		*p++ = 0;
		length += 2;
	}

	//读取生产商ID
	PramLen = EepromPram_ReadPram(E2_MANUFACTURE_ID, Buffer2); 
	if(5 == PramLen)
	{
		memcpy(p,Buffer2,5);
		p += 5;
		length += 5;
	}
	else
	{
		memcpy(p,"70108",5);
		p += 5;
		length += 5;
	}	
	//读取终端型号
    memset((char *)Buffer2,0,20);
    strcpy((char *)Buffer2,c_terminalModel);
	memcpy(p,Buffer2,20);
	p += 20;
	length += 20;
	//读取终端ID
	PramLen = RadioProtocol_ReadDeviceID(p);
	p += 7;
	length +=7;
	//读取车牌颜色
	PramLen = EepromPram_ReadPram(E2_CAR_PLATE_COLOR_ID, Buffer2); //dxl,2017.2.24,待完善
    if(1 == PramLen)
    {
        *p++ = Buffer2[0];
        length++;

    }
    else
    {

         *p++ = 0;
         length++;
    }
    //读取车牌,用车牌号码注册
    PramLen = EepromPram_ReadPram(E2_CAR_PLATE_NUM_ID, Buffer2); //dxl,2017.2.24,待完善
    if((PramLen < 20)&&(0 != PramLen))
    {
          memcpy(p,Buffer2,PramLen);
      length += PramLen;
          //p += PramLen;
          //*p = 0;
          //length++;
    }
    else
    {
          memcpy(p,"粤b012346",9);
      length += 9;
          // p += 9;
          //*p = 0;
          //length++;
    }

	//消息ID
	head.MessageID = 0x0100;
	//消息属性
	head.Attribute = (length & 0x3ff);
	
	//读取注册短消息号码
	
	//判断是否为短消息
	//if(CHANNEL_SMS == s_ucRecvChanel)
	//{
		//channel = CHANNEL_SMS;
	//}
	if(SUCCESS == RadioProtocol_SendCmd(head, Buffer, length, channel))
	{
		return ACK_OK;
	}
	else
	{
		return ACK_ERROR;
	}
	
}
/*********************************************************************
//函数名称	:RadioProtocol_TerminalQualify(void)
//功能		:终端鉴权
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:   成功,返回ACK_OK;失败返回ACK_ERROR;
//备注		:指令ID：0x0102
*********************************************************************/
ProtocolACK RadioProtocol_TerminalQualify(void)
{
	u8	Buffer[20];
	u8	length;

	//获取鉴权码
	length = EepromPram_ReadPram(E2_LOGINGPRS_PASSWORD_ID, Buffer);
	Buffer[length]='\0';
    APP_DEBUG("<-- 终端鉴权，鉴权码:%s鉴权码长度%d-->\r\n",Buffer,length);

    return RadioProtocol_SendData(0x0102,Buffer,length,0);
}
/*********************************************************************
//函数名称	:RadioProtocol_FirmwareUpdataResultNote(u8 *pBuffer, u16 BufferLen)
//功能		:远程固件升级结果通知
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:  成功,返回ACK_OK;失败返回ACK_ERROR;
//备注		:指令ID：0x0108
*********************************************************************/
ProtocolACK RadioProtocol_FirmwareUpdataResultNote(u8 UpdataType,u8 AckResult)
{
		u8 Buffer[5];
		u8 length;
		u8 SmsPhone[20];
		u8 channel = 0;
		MESSAGE_HEAD head;
		
		if( AckResult > 2 )
		{
			return ACK_ERROR;	 
		}
		
		//消息ID
		head.MessageID = 0x0108;
		
		//消息属性
		length = 2;
		head.Attribute = length;
		
		//消息内容
		Buffer[0] = UpdataType;//升级类型
		Buffer[1] = AckResult; //升级结果
		
		//判断是否为短消息
		if(CHANNEL_SMS == s_ucRecvChanel)
		{
			channel = CHANNEL_SMS;
		}
		
		if(SUCCESS == RadioProtocol_SendCmd(head, Buffer, length, channel))
		{
			return ACK_OK;
		}
		else
		{
			return ACK_ERROR;
		}

}

/*********************************************************************
//函数名称	:RadioProtocol_PostionInformationReport(u8 *pBuffer, u16 length)
//功能		:位置信息汇报
//输入		:pBuffer:指向数据（位置信息）指针;BufferLen:数据的长度;
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:    成功,返回ACK_OK;失败返回ACK_ERROR;
//备注		:指令ID：0x0200
*********************************************************************/
ProtocolACK  RadioProtocol_PostionInformationReport(u8 *pBuffer, u16 BufferLen)
{
	MESSAGE_HEAD	head;
	u8	SmsPhone[20];
	u8	channel = 0;
	u16	length;
	
	
	if(BufferLen >= 1024)
	{
		return ACK_ERROR;
	}
	length = BufferLen;
	//消息ID
	head.MessageID = 0x0200;
	//消息属性
	head.Attribute = (BufferLen & 0x3ff);
	//判断是否为短消息
	//if(CHANNEL_SMS == s_ucRecvChanel)
	//{
		//channel = CHANNEL_SMS;
	//}
	if(SUCCESS == RadioProtocol_SendCmd(head, pBuffer, length, channel))
	{
		return ACK_OK;
	}
	else
	{
		return ACK_ERROR;
	}
}


/*********************************************************************
//函数名称	:RadioProtocol_PostionInformation_BulkUpTrans(u8 *pBuffer, u16 length)
//功能		:定位数据批量上传
//输入		:pBuffer:指向数据（位置信息）指针;BufferLen:数据的长度;
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		: 成功,返回ACK_OK;失败返回ACK_ERROR;
//备注		: 指令ID：0x0704
//参见          : 页面 P45  <<道路运输车辆卫星定位系统北斗兼容车载终端通讯协议技术规范2013-01版>> 
*********************************************************************/
ProtocolACK  RadioProtocol_PostionInformation_BulkUpTrans(u8 *pBuffer, u16 BufferLen)
{
	MESSAGE_HEAD	head;
	//u8	SmsPhone[20];
	u8	channel = 0;
	u16	length;

	//判断长度
	if(BufferLen >= 1024)
	{
		return ACK_ERROR;
	}
	
	length = BufferLen;
	//消息ID
	head.MessageID = 0x0704;
	//消息属性
	head.Attribute = (BufferLen & 0x3ff);
        
	if(SUCCESS == RadioProtocol_SendCmd(head, pBuffer, length, channel))
	{
		return ACK_OK;
	}
	else
	{
		return ACK_ERROR;
	}
}
/*********************************************************************
//函数名称	:RadioProtocol_OriginalDataUpTrans(u8 Type, u32 TotalLen, u8 *pBuffer, u16 BufferLen)
//功能		:数据上行透传
//输入		:pBuffer:指向数据（透传数据）的指针;BufferLen:当前透传数据包长度
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:    成功,返回ACK_OK;失败返回ACK_ERROR;
//备注		:指令ID：0x0900
*********************************************************************/
ProtocolACK RadioProtocol_OriginalDataUpTrans(u8 Type, u8 *pBuffer, u16 BufferLen)
{
    unsigned char *buffer;
    u8 SmsPhone[20];
    u8 channel = 0;
    u16 length;
    MESSAGE_HEAD head;
    if(BufferLen >= 1023)
    {
        return ACK_ERROR;
    }
    buffer =ShareBuffer;
    //指向首地址 
    memcpy(buffer+1,pBuffer,BufferLen);
    buffer[0] = Type;
    length = BufferLen+1;
    //消息ID
    head.MessageID = 0x0900;
    //消息属性
    head.Attribute = (length & 0x3ff);
    //判断是否为短消息
    if(CHANNEL_SMS == s_ucRecvChanel)
    {
        channel = CHANNEL_SMS;
    }
    if(SUCCESS == RadioProtocol_SendCmd(head, buffer, length, channel))
    {
        return ACK_OK;
    }
    else
    {
        return ACK_ERROR;
    }
}
/*********************************************************************
//函数名称	:RadioProtocol_DataCompressUpTrans(u8 *pBuffer, u16 BufferLen)
//功能		:数据压缩上传
//输入		:pBuffer:指向数据（压缩数据）的指针;BufferLen:压缩数据长度
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:    成功,返回ACK_OK;失败返回ACK_ERROR;
//备注		:指令ID：0x0901
*********************************************************************/
ProtocolACK RadioProtocol_DataCompressUpTrans(u8 *pBuffer, u16 BufferLen)
{
	
	u8	temp;
	u16 length;
	u8	SmsPhone[20];
	u8	channel = 0;
	u8	*p = NULL;
	u8	*p1 = NULL;
	u8	Buffer[1024];
	
	p = Buffer;
	p1 = p;
	if(NULL == p)
	{
		return ACK_ERROR;
	}
	//压缩数据长度
	temp = (BufferLen & 0xff000000) >> 24;
	*p++ = temp;
	temp = (BufferLen & 0xff0000) >> 16;
	*p++ = temp;
	temp = (BufferLen & 0xff00) >> 8;
	*p++ = temp;
	temp = BufferLen & 0xff;
	*p++ = temp;

	memcpy(p,pBuffer,BufferLen);
	p += BufferLen;
	length = BufferLen+4;

	MESSAGE_HEAD	head;

	//消息ID
	head.MessageID = 0x0901;
	//消息属性
	head.Attribute = (length & 0x3ff);
	
	//判断是否为短消息
	if(CHANNEL_SMS == s_ucRecvChanel)
	{
		channel = CHANNEL_SMS;
	}
	if(SUCCESS == RadioProtocol_SendCmd(head, p1, length, channel))
	{
		return ACK_OK;
	}
	else
	{
		return ACK_ERROR;
	}
}

/*********************************************************************
//函数名称	:RadioProtocol_TerminalRSA(u32 RSAe, u32 RSAn)
//功能		:终端RSA公钥
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:  成功,返回ACK_OK;失败返回ACK_ERROR;
//备注		:指令ID：0x0A00
*********************************************************************/
ProtocolACK RadioProtocol_TerminalRSA(u32 RSAe, u8 *pRSAn)
{
	u16	length;
	u8	Buffer[140];
    //////////////
    Public_ConvertLongToBuffer(RSAe, Buffer);
	//存储n值
	memcpy(Buffer+4,pRSAn,128);

	length = 132;    
    return RadioProtocol_SendData(0x0a00,Buffer,length,0);	
}

//*****************第三部分：自定义的平台下发指令**********************
//*****************第四部分：自定义的终端上发指令**********************
//*****************第五部分：对外接口函数**********************
/*********************************************************************
//函数名称	:RadioProtocol_UpdateTerminalPhoneNum(void)
//功能		:更新终端电话号码
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:上电初始化时需调用
*********************************************************************/
void RadioProtocol_UpdateTerminalPhoneNum(void)
{
        u8  	Buffer[13];
	u8	BufferLen;

	BufferLen = EepromPram_ReadPram(E2_DEVICE_PHONE_ID, Buffer);
        if(E2_DEVICE_PHONE_ID_LEN == BufferLen)
        {
             memcpy(TerminalPhoneNumber,Buffer,E2_DEVICE_PHONE_ID_LEN);
			 //TerminalPhoneNumber[0] &= 0x0f;
        }
        else
        {

		//读手机号码备份区域
		//BufferLen = EepromPram_ReadPram(E2_DEVICE_PHONE_BACKUP_ID, Buffer);
		//if(E2_DEVICE_PHONE_BACKUP_ID_LEN == BufferLen)
		//{
		//	memcpy(TerminalPhoneNumber,Buffer,E2_DEVICE_PHONE_BACKUP_ID_LEN);
		//}
		//else
		{
			TerminalPhoneNumber[0] = 0x01;
              		TerminalPhoneNumber[1] = 0x37;
              		TerminalPhoneNumber[2] = 0x60;
             	 	TerminalPhoneNumber[3] = 0x25;
              		TerminalPhoneNumber[4] = 0x00;
              		TerminalPhoneNumber[5] = 0x00;
		}

        }

}


/*********************************************************************
//函数名称	:RadioProtocol_CheckRecvData
//功能		:将接受到平台的数据分离出标准部标数据帧，标准部标数据帧7E开头7E结尾
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:接受到平台数据后调用该函数
*********************************************************************/
void RadioProtocol_CheckRecvData(unsigned char *recvData,unsigned short recvDataLen)
{
    unsigned short iPosition[2],datalen=0;
    unsigned short i;
    unsigned char *buffer;
    unsigned char flag;
    buffer=recvData;
    flag =0;     
    for(i = 0; i<recvDataLen; i++)
    {
        if(0x7E==buffer[i])
        {
            datalen = i;
            iPosition[flag++] = i ;            
            if(2==flag)
            {
                datalen = iPosition[1]-iPosition[0]-1;
                if(datalen>RADIOPROTOCOL_BUFFER_SIZE)break;                
                s_ucRecvChanel = s_ucCurRecvChanel;
				if(datalen>=10)
				{
                    RadioProtocol_ProtocolParse(&buffer[iPosition[0]+1], datalen);
                    datalen = iPosition[1]+1;//
				}
                else
                {
                    datalen = iPosition[1];//
                }
                break;
            }
        }
    }
    //////////////////////
    if(0==flag)
    {
        RadioProtocolRxLen = 0;
        return;
    } 
    ////看是否存在多条情况/////////////////////////
    if(0==datalen)return ;
    ////////////
    if(RadioProtocolRxLen>datalen&&RadioProtocolRxLen<=RADIOPROTOCOL_BUFFER_SIZE)
    {
        RadioProtocolRxLen -= datalen;
        memmove(RadioProtocolRxBuffer,&RadioProtocolRxBuffer[datalen],RadioProtocolRxLen); 
    }
    else
    {
        RadioProtocolRxLen = 0;
    }
}
/*********************************************************************
//函数名称	:RadioProtocol_AddRecvDataForParse
//功能		:添加数据
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:当往协议接收缓冲拷贝完数据后需调用此函数,将新接收到的数据
//		:（可能同时收到好几条命令,所以要用一个表列）登记进解析列表,
//		:然后触发解析任务
*********************************************************************/
void RadioProtocol_AddRecvDataForParse(unsigned char channel,unsigned char *recvData,unsigned short recvDataLen)
{
/*
    #if 0
    if(LZM_QueueDataFreeSpace(&s_stRadioRecvQueue)>recvDataLen)
    {
        s_ucCurRecvChanel = channel;
        LZM_QueueDataBatchInsert(&s_stRadioRecvQueue,recvData,recvDataLen);        
        if(DISABLE==GetTimerTaskTimerState(TIME_RADIO_PARSE))
        SetTimerTask(TIME_RADIO_PARSE, 1);
    }
    #else    
    if(RadioProtocolRxLen+recvDataLen<=RADIOPROTOCOL_BUFFER_SIZE)
    {
        s_ucCurRecvChanel = channel;
        memcpy(&RadioProtocolRxBuffer[RadioProtocolRxLen],recvData,recvDataLen);
        RadioProtocolRxLen +=recvDataLen; 
        s_ucRecvTimeCnt = 0;
        SetTimerTask(TIME_RADIO_PARSE, 1);
    }
    else
    {
        if(RadioProtocolRxLen)//
        {
            if(DISABLE==GetTimerTaskTimerState(TIME_RADIO_PARSE))
            SetTimerTask(TIME_RADIO_PARSE, 1);
        }
    }
    #endif
    */
}
//////////////////////////////////////////////////////////////////////
//添加任务
/////////////////////////////////////////////////////////////////////



/*********************************************************************
//函数名称	:RadioProtocol_SendMessage(MESSAGE_HEAD	head, u8 *SrcData, u16 SrcLen, u8 SendCh)
//功能		:发送消息
//输入		:MESSAGE_HEAD	head:消息头;SrcData:指向消息体的指针;
//		:SrcLen:消息体长度;SendCh:发送通道,0为GPRS,1为短信;
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:解析完一条指令后，通常需要应答，应答时需调用该函数；
//		:主动上发一条消息命令时也需要调用该函数。
*********************************************************************/
ErrorStatus RadioProtocol_SendCmd(MESSAGE_HEAD	head, u8 *SrcData, u16 SrcLen, u8 SendCh )
{
    //u8	RadioProtocolTxBuffer[RADIOPROTOCOL_BUFFER_SIZE];
    u8	VerifyCode;
    u16	datalen;
    u16	length;
    //鉴权成功后才发其他指令
    
    //终端手机号
    memcpy(head.PhoneNum,TerminalPhoneNumber,6);
    //消息流水号
    MessageSerialNum++;
    head.SerialNum = MessageSerialNum;

    //MESSAGE_HEAD结构体变字节流
    datalen = RadioProtocol_HeadtoBytes(RadioProtocolTxBuffer+1,head);
    if(SrcLen > 0)
    {
        memcpy(RadioProtocolTxBuffer+1+datalen,SrcData,SrcLen);
        datalen +=SrcLen;
    }     
    VerifyCode=Public_GetXorVerify(RadioProtocolTxBuffer+1, datalen);
    RadioProtocolTxBuffer[1+datalen++] = VerifyCode;
    //转义
    length = TransMean(RadioProtocolTxBuffer+1,datalen);
    length++;
    //补充一头一尾的标志位
    RadioProtocolTxBuffer[0] = 0x7e;
    RadioProtocolTxBuffer[length] = 0x7e;
    length++;
    //发送
    datalen = Communication_SndMsg2Svr(SendCh,RadioProtocolTxBuffer, length);
    if(length != datalen)
    {
        return ERROR;
    }
    return SUCCESS;
}
/*********************************************************************
//函数名称	:RadioProtocol_SendData
//函数功能	:
//备注		:
*********************************************************************/
ProtocolACK RadioProtocol_SendData(u16 cmd, u8 *SrcData, u16 SrcLen, u8 SendCh)
{
    //u8	RadioProtocolTxBuffer[RADIOPROTOCOL_BUFFER_SIZE];
    MESSAGE_HEAD head; 
    u8	VerifyCode;
    u16	datalen;
    u16	length;
    //鉴权成功后才发其他指令
    if(0 == GetTerminalAuthorizationFlag())
    {
        if(0x0100 == cmd)//注册
        {
        }
        else 
        if(0x0102 == cmd)//鉴权
        {
        }
        else 
        if(0x0003 == cmd)//注销
        {
        }
        else
        if((0x0001 == cmd)&&(CHANNEL_SMS == SendCh))//短信类终端应答
        {
        }
        else
        {
            return ACK_ERROR;
        }
    }
     //消息ID
    head.MessageID = cmd;
    //消息属性
    head.Attribute = SrcLen;
    //终端手机号
    memcpy(head.PhoneNum,TerminalPhoneNumber,6);
    //消息流水号
    MessageSerialNum++;
    head.SerialNum = MessageSerialNum;

    //MESSAGE_HEAD结构体变字节流
    datalen = RadioProtocol_HeadtoBytes(RadioProtocolTxBuffer+1,head);
    if(SrcLen > 0)
    {
        memcpy(RadioProtocolTxBuffer+1+datalen,SrcData,SrcLen);
        datalen +=SrcLen;
    }     
    VerifyCode=Public_GetXorVerify(RadioProtocolTxBuffer+1, datalen);
    RadioProtocolTxBuffer[1+datalen++] = VerifyCode;
    //转义
    length = TransMean(RadioProtocolTxBuffer+1,datalen);
    length++;
    //补充一头一尾的标志位
    RadioProtocolTxBuffer[0] = 0x7e;
    RadioProtocolTxBuffer[length] = 0x7e;
    length++;
    //发送
    datalen = Communication_SndMsg2Svr(SendCh,RadioProtocolTxBuffer, length);
    if(length != datalen)
    {
        return ACK_ERROR;
    }
    return ACK_OK;
}




//*****************第六部分：模块内部函数**********************
/*********************************************************************
//函数名称	:RadioProtocol_ProtocolParse(u8 *pBuffer, u16 BufferLen)
//功能		:协议解析,解析一条指令
//输入		:pBuffer:指向数据(待解析的数据)的指针;BufferLen:数据的长度
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:成功,返回SUCCESS;失败,返回ERROR
//备注		:平台下发的所有命令都是由该函数解析，该函数的输入参数
//		:是7E开头,7E结尾的一串字节序列
*********************************************************************/
ErrorStatus RadioProtocol_ProtocolParse(u8 *pBuffer, u16 BufferLen)
{
    unsigned char ParseBuffer[RADIOPROTOCOL_BUFFER_SIZE];
    u8 *p;
    u8 IDHighByte;
    u8 IDLowByte;
    //u8 Index;
    u8 offset;
    u16	length;
    u16	Attribute;
    //MESSAGE_HEAD RadioProtocolRxHead;
    //////////////////
    memcpy(ParseBuffer, pBuffer,BufferLen);    
    //心跳包计数清0
    Heartbeat_ClrHeartbeatCount();
    ////////////////////
    //反转义
    length = unTransMean(ParseBuffer, BufferLen);
    //检查校验码
    IDHighByte =Public_GetXorVerify(ParseBuffer, length-1);
    if(IDHighByte != ParseBuffer[length-1])//dxl,2013.11.30开启校验,防止垃圾短信
    {
        return ERROR;
    }
    //指向接收缓冲
    p = ParseBuffer;
    //读取消息ID，按照网络字节序解析，即高字节在前
    IDHighByte = *p++;
    IDLowByte  = *p++;
    //读取消息体属性
    Attribute = Public_ConvertBufferToShort(p); 
    //////////////
    offset = 12;//无分包项
    if(0x2000 == (Attribute&0x2000))//有分包项
    {
        offset = 16;
    }
    ////////////////////
    length -= offset;//减消息头16字节
    length--;//减校验码1字节
    ////////////////////
    if(length != (Attribute & 0x03ff))//判断长度
    {
        return ERROR;
    }
    ///消息ID///////////////////
    s_usRecvMessageID = (IDHighByte << 8)|IDLowByte;
    //消息流水号
    p += 8;//消息体属性2+终端手机号6
    //终端手机号
    //memcpy(RadioProtocolRxHead.PhoneNum,p,6);
    //p += 6;
    s_usRecvSerialNum = Public_ConvertBufferToShort(p);
    ////////////////////
    p = ParseBuffer+offset;
    switch(s_usRecvMessageID)
    {
        case 0x8001://平台（服务器）通用应答
            RadioProtocol_ServerGeneralAck(p, length);
            break;
        case 0x8100://终端注册应答
            RadioProtocol_TerminalRegisterAck(p, length);
            break;  
        case 0x8103://设置终端参数
            RadioProtocol_PramSet(p, length);
            break;
        case 0x8104://查询终端参数
        case 0x8106://查询终端参数
            RadioProtocol_PramSearch(p, length);
            break;
        case 0x8105://终端控制
            RadioProtocol_TerminalControl(p, length);
            break;
        case 0x8107:// 查询终端属性,//Add By Shigle 2013-04-01
            RadioProtocol_TerminalAttribute(p, length);	
            break;
        case 0x8201://位置信息查询
            RadioProtocol_PositionSearch(p, length);
            break;
        case 0x8202://临时位置跟踪控制
            RadioProtocol_TempPositionControl(p, length);
            break;
		case 0x8203://人工确认报警
			RadioProtocol_AlarmConfirmManual(p, length);
			break;
		case 0x8500://车辆控制
			RadioProtocol_CarControl(p, length);
			break;
		case 0x8600://设置圆形区域
			RadioProtocol_SetRoundArea(p, length);
			break;
		case 0x8601://删除圆形区域
			RadioProtocol_DelRoundArea(p, length);
			break;	
		case 0x8602://设置矩形区域
			RadioProtocol_SetRectangleArea(p, length);
			break;	
		case 0x8603://设置矩形区域
			RadioProtocol_DelRectangleArea(p, length);
			break;	
		case 0x8604://设置矩形区域
			RadioProtocol_SetPolygonArea(p, length);
			break;		
		case 0x8605://设置矩形区域
			RadioProtocol_DelPolygonArea(p, length);
			break;	
        case 0x8900://数据下行透传
            RadioProtocol_OriginalDataDownTrans(p, length);
            break;
        default: 
            RadioProtocol_TerminalGeneralAck(3);
            return ERROR;
    }
    return SUCCESS;
}
/*********************************************************************
//函数名称	:unTransMean
//功能		:实现反转义功能
//		:0x7e----->0x7d后跟一个0x02;0x7d----->0x7d后跟一个0x01
//输入		:解析前字节流，解析前字节流长度
//输出		:解析后字节流
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:转义解析后字节流长度
//备注		:协议解析函数需调用此函数，先进行反转义，然后再解析。
*********************************************************************/
u16 unTransMean(u8 *pBuffer, u16 BufferLen)
{
    u16	i;
    u16	NewLen;
    u8 temp;
    i = 0;
    NewLen = BufferLen;
    while(i < BufferLen)
    {
        if(0x7d == *pBuffer)
        {
            temp = *(pBuffer+1);
            if((0x02 == temp)||(0x01 == temp))
            {
                *pBuffer = 0x7c + temp;
                NewLen--;
                i++;
                memmove(pBuffer+1,pBuffer+2,BufferLen-i);
            }
        }
        i++;
        pBuffer++;
    }
    return NewLen;
}
/*********************************************************************
//函数名称	:TransMean
//功能		:实现打包时的转义处理功能
//输入		:转义前字节流，转义前字节流长度
//输出		:转义后字节流
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:转义后字节流长度
//备注		:0x7e----->0x7d后跟一个0x02;0x7d----->0x7d后跟一个0x01
*********************************************************************/
u16 TransMean(u8 *pBuffer, u16 BufferLen)
{
	u16	i;
	u16	NewLen;
	u8	temp;
	i = 0;
	NewLen = BufferLen;
	while(i < BufferLen)
	{
		if((0x7e == *pBuffer)||(0x7d == *pBuffer))
		{
            temp = (*pBuffer-0x7c);
			*pBuffer = 0x7d;
			memmove(pBuffer+2,pBuffer+1,BufferLen-i);
			pBuffer++;
			*pBuffer = temp;
			NewLen++;
		}
		i++;
		pBuffer++;
	}
	return NewLen;
}
/*********************************************************************
//函数名称	:RadioProtocol_HeadtoBytes(u8 *pBuffer, MESSAGE_HEAD head)
//功能		:把消息头变成字节流
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:
*********************************************************************/
u8 RadioProtocol_HeadtoBytes(u8 *pBuffer, MESSAGE_HEAD head)
{
	//u8	length;
	//u16	Attribute;

	//主机字节序转换成网络字节序
	//消息ID
	//head.MessageID = RadioProtocol_htons(head.MessageID);
	//消息体属性
	//Attribute = head.Attribute;
	//head.Attribute = RadioProtocol_htons(head.Attribute);
	//终端手机号
	//消息流水号
	//head.SerialNum = RadioProtocol_htons(head.SerialNum);
	//判断有无封装项,,超长待机不会有分包封装项
	//if(0x2000 == (Attribute & 0x2000))//有封装项
	//{
	//	head.TotalPackage = RadioProtocol_htons(head.TotalPackage);
	//	head.CurrentPackage = RadioProtocol_htons(head.CurrentPackage);
	//	length = 16;
	//}
	//else//没有封装项
	//{
		//length = 12;
	//}
    Public_ConvertShortToBuffer(head.MessageID, pBuffer);
    Public_ConvertShortToBuffer(head.Attribute, pBuffer+2);
    memcpy(pBuffer+4, head.PhoneNum, 6);
    Public_ConvertShortToBuffer(head.SerialNum, pBuffer+10);
	//memcpy((char *)pBuffer, (char *)&head, 12);
	return 12;
}
/*********************************************************************
//函数名称	:RadioProtocol_ntohs(u16 value)
//功能		:网络字节序转主机字节序
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:
*********************************************************************/
u16 RadioProtocol_ntohs(u16 value)
{
	u8	byte;
	u8	*p = NULL;
	u16	ConvertValue;

	p = (u8 *)&value;

    #if (1 == SYS_USE_BIG_LITTLE_FLAG)	
	{
		ConvertValue = value;
	}
	#else //交换字节序
	{
		ConvertValue = 0;	
		byte = *p++;
		ConvertValue |= byte << 8;
		byte = *p++;
		ConvertValue |= byte;	
	}
    #endif
	return ConvertValue;
	
}
/*********************************************************************
//函数名称	:RadioProtocol_ntoh(u32 value)
//功能		:网络字节序转主机字节序
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:
*********************************************************************/
u32 RadioProtocol_ntoh(u32 value)
{
	u8	byte;
	u32	ConvertValue;
	

	#if (1 == SYS_USE_BIG_LITTLE_FLAG)
	{
		ConvertValue = value;
	}
	#else //交换字节序
	{
		ConvertValue = 0;
		//读取高字节
		byte = (value & 0xff000000) >> 24;
		ConvertValue |= byte;
		//读取次高字节
		byte = (value & 0xff0000) >> 16;
		ConvertValue |= (byte << 8);
		//读取中字节
		byte = (value & 0xff00) >> 8;
		ConvertValue |= (byte << 16);
		//读取中字节
		byte = value;
		ConvertValue |= (byte << 24);
		
	}
    #endif
	return ConvertValue;
	
}
/*********************************************************************
//函数名称	:RadioProtocol_htons(u16 value)
//功能		:主机字节序转网络字节序
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:
*********************************************************************/
u16 RadioProtocol_htons(u16 value)
{
	u8	byte;
	u8	*p = NULL;
	u16	ConvertValue;

	p = (u8 *)&value;

	#if (1 == SYS_USE_BIG_LITTLE_FLAG)
	{
		ConvertValue = value;
	}
	#else //交换字节序
	{
		ConvertValue = 0;	
		byte = *p++;
		ConvertValue |= byte << 8;
		byte = *p++;
		ConvertValue |= byte;	
	}
    #endif
	return ConvertValue;
	
}
/*********************************************************************
//函数名称	:RadioProtocol_hton(u32 value)
//功能		:主机字节序转网络字节序
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:
*********************************************************************/
u32 RadioProtocol_hton(u32 value)
{
	u8	byte;
	u8	*p;
	u32	ConvertValue;

	p = (u8 *)&value;

	#if (1 == SYS_USE_BIG_LITTLE_FLAG)
	{
		ConvertValue = value;
	}
	#else //交换字节序
	{
		ConvertValue = 0;	
		byte = *p++;
		ConvertValue |= byte << 24;
		byte = *p++;
		ConvertValue |= byte << 16;
		byte = *p++;
		ConvertValue |= byte << 8;
		byte = *p++;
		ConvertValue |= byte;	
	}
    #endif
	return ConvertValue;
	
}
/*********************************************************************
//函数名称	:NullOperate(u8 *pBuffer, u16 BufferLen)
//功能		:空操作,不干任何事
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:成功,返回ACK_OK;失败返回ACK_ERROR;
//备注		:
*********************************************************************/
ProtocolACK NullOperate(u8 *pBuffer, u16 BufferLen)
{
	return ACK_OK;
}
/*********************************************************************
//函数名称	:RadioProtocol_ReadDeviceID
//功能		:平台读取终端ID
//备注		:
*********************************************************************/
unsigned char RadioProtocol_ReadDeviceID(unsigned char *data)
{
    u8	Buffer[30];
	u8	Buffer1[50];
    u8	PramLen;
    PramLen = EepromPram_ReadPram(E2_DEVICE_ID, Buffer);
    if(7 == PramLen)//取终端ID后面7位作为登录注册的终端ID
    {
        Public_ConvertBcdToAsc(Buffer1,&Buffer[3],4);
   }
    else
    {
        //读取备份的终端ID号
        strcpy((char *)&Buffer1[1],"3100028");
    }
    ///读取终端ID///////////////
    memcpy(data,&Buffer1[1],7);
	//Buffer[PramLen]='\0';
    //APP_DEBUG("<-- 平台读取终端ID:%s-->\r\n",Buffer);
    return 7;
}



/*********************************************************************
//函数名称	:RadioProtocol_TerminalAttribute(u8 *pBuffer, u16 BufferLen)
//功能		:查询终端属性
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:指令ID：0x8107
*********************************************************************/
ProtocolACK RadioProtocol_TerminalAttribute(u8 *pBuffer, u16 BufferLen)
{
    u8 buffer[150];
    u8 tmplen;
    u16 length;
    
    length = 0;
    //读取终端类型 Bytes:2 
    //默认为适用于客运、危险品、货运，类型：一体机
    buffer[length++] = 0;
    buffer[length++] = 0x07;
    memcpy(&buffer[length],"70108",5);
    //////////////////
    length += 5;
    //////////////////////
    //读取终端型号
    memset(&buffer[length],0,20);
    Version_GetProductSeriesName((char *)&buffer[length]);
    length += 20;
    //读取终端ID
    tmplen=RadioProtocol_ReadDeviceID(&buffer[length]);
    length += tmplen;
    //终端SIM卡ICCID BCD[10]  Bytes:10
    Modem_Api_CCID(&buffer[length]);
    length += 10;

    //终端硬件版本信息
    tmplen = Version_GetHardwareInformation(&buffer[length+1]);
    buffer[length++]= tmplen;
    length   +=tmplen;
    //终端固件版本信息
    tmplen = Version_GetFirmwareInformation(&buffer[length+1]);
    buffer[length++]= tmplen;
    length   +=tmplen;
    //GNSS模块属性    
    buffer[length++] = 0x03;//没有设置过的化是支持GPS+北斗
    //通信模块属性
    buffer[length++] = 0x01;//移动通讯    
    return RadioProtocol_SendData(0x0107,buffer,length,s_ucRecvChanel);
}

/*********************************************************************
//函数名称	:RadioProtocol_GetCurRecvSerialNum
//功能		:取得当前接收流水号
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:
*********************************************************************/
unsigned short RadioProtocol_GetCurRecvSerialNum(void) 
{
    return s_usRecvSerialNum;
}

/*********************************************************************
//函数名称	:RadioProtocol_GetUpdataResultNoteAckFlag
//功能		:取得升级结果通知应答标志
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:1为收到平台的通用应答了
//备注		:
*********************************************************************/
unsigned char RadioProtocol_GetUpdataResultNoteAckFlag(void) 
{
    return s_ucUpdataResultNoteAckFlag;
}

