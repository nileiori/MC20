#include "include.h"
#include "report.h"
#include "FCT_SelfCheck.h"

static LED_CTRL_DEF ledCtrlHd;
static u16  LowVolThreshold = 0;//低电压阈值
static u16 uart_busy_cnt = 1200;//按键开机留120s给串口通讯,如果串口活动,这个值维持1200

u32 battery_capacity;
u32 battery_voltage;

extern u8	TerminalAuthorizationFlag;//终端鉴权标志,鉴权成功为1,失败为0
/*
慢闪（亮1秒，暗2秒）表示电池供电；
快闪（亮0.1秒，暗2.9秒）表示外电供电
说明：电源灯可以省掉。
*/
void PowerLedFlashCtr(void)
{
	static u8 PowerLedCnt=0;
	PowerLedCnt++;
	if( Io_ReadAlarmBit( ALARM_BIT_POWER_SHUT))//主电掉电说明是电池供电
		{
			if(PowerLedCnt<=5)
				{
					POWER_LED_ON();
				}
				else if(PowerLedCnt<=20)
				{
					POWER_LED_OFF();
				}
				else
				{
				PowerLedCnt=0;
				}
				
		}
		else
		{
			if(PowerLedCnt<=1)
				{
					POWER_LED_ON();
				}
				else if(PowerLedCnt<=29)
				{
					POWER_LED_OFF();
				}
				else
				{
				PowerLedCnt=0;
				}
		}

}
/*
慢闪（亮1秒，暗2秒）表示GPS不定位；
快闪（亮0.1秒，暗2.9秒）表示GPS定位;
快闪（亮0.1秒，暗9.9秒），表示休眠状态；
长亮：表示卫星定位模块故障。
*/
void GpsLedFlashCtr(void)
{
	static u8 GpsLedCnt=0;
	GpsLedCnt++;
	if((1 == Io_ReadAlarmBit(ALARM_BIT_ANT_SHUT))
	 ||(1 == Io_ReadAlarmBit(ALARM_BIT_ANT_SHORT))
	 ||(1 == Io_ReadAlarmBit(ALARM_BIT_GNSS_FAULT)))
		{
			//APP_DEBUG("\r\n<-- GNSS 故障 -->\r\n")
			GPS_LED_OFF();
		}
		else if(Io_ReadIoStatusBit(IO_STATUS_BIT_SLEEP)==1)
		{
			//APP_DEBUG("\r\n<-- 休眠 -->\r\n")
			if(GpsLedCnt<=1)
			{
				GPS_LED_ON();
			}
			else if(GpsLedCnt<=99)
			{
				GPS_LED_OFF();
			}
			else
			{
			GpsLedCnt=0;
			}
		}
		else if(0 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
		{
			//APP_DEBUG("\r\n<-- 不导航 -->\r\n")
			if(GpsLedCnt<=5)
			{
				GPS_LED_ON();
			}
			else if(GpsLedCnt<=20)
			{
				GPS_LED_OFF();
			}
			else
			{
			GpsLedCnt=0;
			}
		}
		else //if(1 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))
		{
			//APP_DEBUG("\r\n<-- 导航 -->\r\n")
			if(GpsLedCnt<=1)
			{
				GPS_LED_ON();
			}
			else if(GpsLedCnt<=29)
			{
				GPS_LED_OFF();
			}
			else
			{
			GpsLedCnt=0;
			}
		}
}
/*
快闪（亮0.1秒，暗2.9秒）表示GPRS连上网络；
慢闪（亮1秒，暗2秒）表示GSM初始化；
不亮表示没有GSM信号。
*/
void GprsLedFlashCtr(void)
{
	static u8  NetLedCnt=0;
	NetLedCnt++;
	if(TerminalAuthorizationFlag!=0)
	{
		if(NetLedCnt<=1)
		{
			GPRS_LED_ON();
		}
		else if(NetLedCnt<=29)
		{
			GPRS_LED_OFF();
		}
		else
		{
		NetLedCnt=0;
		}	
	}
	else
	{
		if(NetLedCnt<=10)
		{
			GPRS_LED_ON();
		}
		else if(NetLedCnt<=20)
		{
			GPRS_LED_OFF();
		}
		else
		{
		NetLedCnt=0;
		}	
	}
}
u16 uart_get_busy_status(void)
{
	return uart_busy_cnt;
}
void uart_busy_time_reduce(void)
{
	if((uart_busy_cnt)&&(uart_busy_cnt != 0xffff))
		uart_busy_cnt--;
}
void uart_set_default_mode(void)
{
	uart_busy_cnt = 1200;
}
void uart_set_idle_mode(void)
{
	uart_busy_cnt = 0xffff;
}

//led状态变化
void Led_Status_Change(u8 sta)
{
	ledCtrlHd.state = sta;
}
//LED状态控制
void LedControlCallback(u8 sta)
{
	/*uart_busy_time_reduce();
	if(0 == uart_get_busy_status())
	{
		uart_set_idle_mode();
		{
			APP_DEBUG("\r\n...进入睡眠模式....\r\n");
			GPRS_LED_OFF();
			Ql_SleepEnable();//进入睡眠模式
		}
		return;
	}	
	if(0xffff == uart_get_busy_status())return;
	//APP_DEBUG("\r\n...睡眠模式计数器....%d\r\n",uart_busy_cnt);
	*/
	switch(sta)
	{
		case LED_STATUS_POWER_ON:
		LED_STATUS_POWER_NORMAL:
		{
 			GprsLedFlashCtr();
		}break;
		case LED_STATUS_POWER_DOWN:
		{
			GPRS_LED_OFF();
		}break;
		case LED_STATUS_POWER_LOW:
		{
			GPRS_LED_OFF();
		}break;
		case LED_STATUS_POWER_CHARGE:
		{
			GPRS_LED_ON();
		}break;
		default:break;
	}
}

void Led_Init(void)
{
	ledCtrlHd.state = LED_STATUS_POWER_NORMAL;
	ledCtrlHd.ledCtrlCB = LedControlCallback;
}

/*********************************************************************
//函数名称	:LowVol_UpdatePram(void)
//功能		:更新低电压阈值
//输入		:无参数
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void LowVol_UpdatePram(void)
{
	u8 Buffer[5] = {0};
	u8 PramLen;
	PramLen = EepromPram_ReadPram(E2_LOW_VOL_THRESHOLD_ID,Buffer);    
	if(E2_LOW_VOL_THRESHOLD_ID_LEN == PramLen)
	{	
        LowVolThreshold = Public_ConvertBufferToShort(Buffer);
        /////////////////
        if(LowVolThreshold<3300)LowVolThreshold=3300;//防止流量过多
        if(LowVolThreshold>3700)LowVolThreshold=3700;//
	}
	else
	{
		LowVolThreshold = 3500;//默认值,实际应用使用
	}
}

//监控电压值小于阈值就硬件关机
void WatchLowVol(void)
{
	static u8 lowVolCnt = 0;
	static u8 csqCnt = 0;
	u32 temp,bit_err;
	if(battery_voltage<LowVolThreshold)
	{
		if(lowVolCnt++ > 5)
		{
			lowVolCnt = 0;
			APP_DEBUG("\r\n<-- 电压值低于阈值硬件关机 当前电压:%d  电压阈值:%d-- >\r\n",battery_voltage,LowVolThreshold);
			//掉电关机之前写盲区节点信息到Flash
			Blind_UpdateCirQueueInfo();
			REMOTE_SHUTDOWN();
		}
	}
	else
	{
		lowVolCnt = 0;
	}
	if(csqCnt++ > 40)
	{csqCnt = 0;
		RIL_NW_GetSignalQuality((u32*)&temp, (u32*)&bit_err);
		//if(0xaa == FCT_ReadSelfCheckFlg())
		{
			APP_DEBUG("CSQ:%d\r\n",temp);
			APP_DEBUG("CELLVOLTAGE:%d\r\n",battery_voltage);
		}
	}
}

void proc_ledctr_task(s32 taskid)
{
	static u8 ResetPinPullHighFlg = 0;
	static u8 RstPinCnt = 0;
	static u8 LowVolCheck = 0;
	static u8 LowVolCnt = 0;
	static u8 LowVolResumeCnt = 0;
	
	while(1)
	{
		
		Ql_Sleep(100);
		if(ProjectInitFinshFlag==1)
		{
			if(0 == ResetPinPullHighFlg)
			{
				RstPinCnt++;
				if(RstPinCnt > 35)
				{
					ResetPinPullHighFlg = 1;
					RESET_KEY_PIN_HIGH();//Reset脚拉高
					APP_DEBUG("\r\nreset脚拉高........\r\n");
				}
			}
			
			//PowerLedFlashCtr();
			//GpsLedFlashCtr();
			ledCtrlHd.ledCtrlCB(ledCtrlHd.state);
			ReadTime();
			
			//升级过程中不监测电压
			
			if((0 == GetUpdateFlag())/*&&GetTerminalAuthorizationFlag()*/)
			{
				if(LowVolCheck++ >= 10)//1s一次
				{
					LowVolCheck = 0;
					RIL_GetPowerSupply(&battery_capacity, &battery_voltage);
					if(battery_voltage<3700)//小于3700mv
					{
						LowVolResumeCnt = 0;
						if(LowVolCnt++ > 10)
						{
							LowVolCnt = 10;
							Io_WriteSelfDefine2Bit(DEFINE_BIT_18, SET);
						}
					}
					else
					{
						LowVolCnt = 0;
						if(LowVolResumeCnt++ > 10)
						{
							LowVolResumeCnt = 10;
							Io_WriteSelfDefine2Bit(DEFINE_BIT_18, RESET);
						}
					}
					WatchLowVol();//监测电压					
				}
			}
			//POWER_LED_ON();
			//Ql_Sleep(100);//延时一秒	
			//POWER_LED_OFF();
		}
//		SoftWdg_Feed(BIT4);
	}
}


