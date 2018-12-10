
/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Co., Ltd. 2013
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   main.c
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   This app demonstrates how to send AT command with RIL API, and transparently
 *   transfer the response through MAIN UART. And how to use UART port.
 *   Developer can program the application based on this example.
 * 
 ****************************************************************************/
#ifdef __CUSTOMER_CODE__
#include "include.h"
#include "report.h"
#include "fota_main.h"
#include "FCT_SelfCheck.h"
#include "NMEA_Packet_Protocol.h"

#define SERIAL_RX_BUFFER_LEN  128

#if DEBUG_ENABLE 
char DBG_BUFFER[DBG_BUF_LEN];
#endif
#if FCT_DEBUG_ENABLE
char FCT_DBG_BUFFER[FCT_DBG_BUF_LEN];
#endif

u8 ProjectInitFinshFlag = 0;

// Define the UART port and the receive data buffer
static Enum_SerialPort m_myUartPort  = UART_PORT1;
static u8 m_RxBuf_Uart1[SERIAL_RX_BUFFER_LEN];
s32 m_pwrOnReason;

static void CallBack_UART_ParaSet_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void* customizedPara);
static s32 ATResponse_Handler(char* line, u32 len, void* userData);
void Callback_PowerKey_Hdlr(s32 param1, s32 param2)
{
	APP_DEBUG("<--Power Key: %s,%s-->\r\n", \
					param1==POWER_OFF?"Power Off":"Power On", \
					param2==KEY_DOWN?"Key Down":"Key Up" \
					);
	if(POWER_ON == param1)
	{
		APP_DEBUG("<--App Lock Power Key-->\r\n");
		Ql_LockPower();
	}
	else if(POWER_OFF == param1)
	{
		//forbid power down automatically
		APP_DEBUG("<--System forbid power down automatically 别闹-->\r\n");
		return;
		APP_DEBUG("<--System Power Off-->\r\n");
		Ql_PowerDown(1);
	}
}

void proc_main_task(s32 taskId)
{
    s32 ret;
	TIME_T saveTime;
    ST_MSG msg;
	u8 sucUpdatacount;//写升级标志
    // Register & open UART port
    ST_UARTDCB dcb;
    Enum_SerialPort mySerialPort = UART_PORT1;
	//FCT_SelfCheckInit();
    dcb.baudrate = 115200;
    dcb.dataBits = DB_8BIT;
    dcb.stopBits = SB_ONE;
    dcb.parity   = PB_ODD;
    dcb.flowCtrl = FC_NONE;
    ret = Ql_UART_Register(mySerialPort, CallBack_UART_ParaSet_Hdlr, NULL);
    if (ret < QL_RET_OK)
    {
        Ql_Debug_Trace("<--Ql_UART_Register(mySerialPort=%d)=%d-->\r\n", mySerialPort, ret);
    }
    ret = Ql_UART_OpenEx(mySerialPort, &dcb);
	APP_DEBUG("<--OpenCPU: proc_subtask1-->\r\n");
	
    //ret = Ql_UART_Register(m_myUartPort, CallBack_UART_ParaSet_Hdlr, NULL);
    //ret = Ql_UART_Open(m_myUartPort, 115200, FC_NONE);
    if (ret < QL_RET_OK)
    {
        APP_DEBUG("Fail to open serial port[%d], ret=%d\r\n", m_myUartPort, ret);
    }
	#if	configUSE_MTK_NMEA	
	MTK_SendCmd_Packet_Type_010("001");
	MTK_SendCmd_Packet_Type_011();
	MTK_SendCmd_Packet_Type_001("869","3");
	#endif
	m_pwrOnReason = Ql_GetPowerOnReason();
	APP_DEBUG("\r\n<-- m_pwrOnReason %d -- >\r\n",m_pwrOnReason );

	//s64 freeSpace = Ql_FS_GetFreeSpace(Ql_FS_UFS);
	//APP_DEBUG("freeSpace = %d\r\n",freeSpace);
	//Ql_PwrKey_Register(Callback_PowerKey_Hdlr);
	PowerOnUpdata();
	PeripheralInit();
#if	( configUSE_GNSS )
	init_GPStimer();
#endif
	init_tcp();
	Report_Blind_Callback_Init();
	//注册一个定时器用来控制盲区上报
	Blind_Timer_Register();
	//上电恢复追踪信息
	Report_PramResume();
	FCT_SelfCheckInit();
	WDG_Hard_Init();//要接外部看门狗
	
	ProjectInitFinshFlag = 1;
	//m_pwrOnReason = Ql_GetPowerOnReason();

	APP_DEBUG("OpenCPU: Customer Application\r\n");
#if	MILE_TEST	
	MileMeter_Test();
#endif
    // START MESSAGE LOOP OF THIS TASK
    while(TRUE)
    {
        Ql_OS_GetMessage(&msg);
        switch(msg.message)
        {
        case MSG_ID_RIL_READY:
            APP_DEBUG("<-- RIL is ready -->\r\n");
            Ql_RIL_Initialize();
			// Set cfun (1) if power on alarm by sending AT+CFUN=1 through RIL
			//APP_DEBUG("\r\n<-- m_pwrOnReason %d -- >\r\n",m_pwrOnReason );
            if (RTCPWRON == m_pwrOnReason)
            {
            	Report_RtcWakeUpOffsetResume();
            	// Power on Alarm, need to set CFUN (1)
                s32 iRet = Ql_RIL_SendATCmd("AT+CFUN=1", 9, NULL, NULL, 0);
                APP_DEBUG("<-- Power on alarm, switch CFUN from 0 to 1, iRet=%d -->\r\n", iRet);
                // TODO: Do what you want on alarm, or put it in "URC_ALARM_RING_IND"
                // ...
            }
			MOD_GetIccidBcdValue();
			InitSmsData();
            break;
        case MSG_ID_URC_INDICATION:
            //APP_DEBUG("<-- Received URC: type: %d, -->\r\n", msg.param1);
            switch (msg.param1)
            {
            case URC_SYS_INIT_STATE_IND:
                APP_DEBUG("<-- Sys Init Status %d -->\r\n", msg.param2);
				if (SYS_STATE_SMSOK == msg.param2)
				{
					APP_DEBUG("\r\n<-- SMS module is ready -->\r\n");
					APP_DEBUG("\r\n<-- Initialize SMS-related options -->\r\n");
					ret = SMS_Initialize(); 		
					if (!ret)
					{
						APP_DEBUG("Fail to initialize SMS ret=%\r\n",ret);
					}
				}
				break;
            case URC_SIM_CARD_STATE_IND:
                APP_DEBUG("<-- SIM Card Status:%d -->\r\n", msg.param2);
                break;
            case URC_GSM_NW_STATE_IND:
                if (NW_STAT_REGISTERED == msg.param2 || NW_STAT_REGISTERED_ROAMING == msg.param2)
				{
					//char c_networkTime[30] = {0};
					//RIL_Sync_Network_Time(c_networkTime);//同步网络时间
					//APP_DEBUG("<-- Module has registered to GSM network -->\r\n");
				}else{
					GsmDrop_callback();
					APP_DEBUG("<-- GSM network status:%d -->\r\n", msg.param2);
					/* status: 0 = Not registered, module not currently search a new operator
					 *		   2 = Not registered, but module is currently searching a new operator
					 *		   3 = Registration denied 
					 */
				}
                break;
            case URC_GPRS_NW_STATE_IND:
                if (NW_STAT_REGISTERED == msg.param2 || NW_STAT_REGISTERED_ROAMING == msg.param2)
                {
                    APP_DEBUG("<-- Module has registered to GPRS network -->\r\n");
                    // Module has registered to GPRS network, and app may start to program with QuecLocator
                    
                }else{
                	GsmDrop_callback();
                    APP_DEBUG("<-- GPRS network status:%d -->\r\n", msg.param2);
                    /* status: 0 = Not registered, module not currently search a new operator
                    *         2 = Not registered, but module is currently searching a new operator
                    *         3 = Registration denied 
                    */
                }
                break;
            case URC_CFUN_STATE_IND:
                APP_DEBUG("<-- CFUN Status:%d -->\r\n", msg.param2);
                break;
            case URC_COMING_CALL_IND:
                {
                    ST_ComingCall* pComingCall = (ST_ComingCall*)msg.param2;
                    APP_DEBUG("<-- Coming call, number:%s, type:%d -->\r\n", pComingCall->phoneNumber, pComingCall->type);
                    break;
                }
            case URC_CALL_STATE_IND:
                APP_DEBUG("<-- Call state:%d\r\n", msg.param2);
                break;
            case URC_NEW_SMS_IND:
                APP_DEBUG("\r\n<-- New SMS Arrives: index=%d\r\n", msg.param2);
				Hdlr_RecvNewSMS((msg.param2), FALSE);
				break;
            case URC_MODULE_VOLTAGE_IND:
                APP_DEBUG("<-- VBatt Voltage Ind: type=%d\r\n", msg.param2);
                break;
			case URC_ALARM_RING_IND:
                APP_DEBUG("<-- alarm is ringing -->\r\n");
                // TODO: Do what you want on alarm
                // ...官方这版SDK好像有问题
                if (RTCPWRON != m_pwrOnReason)
	            {
	            	m_pwrOnReason = RTCPWRON;
	            	Report_RtcWakeUpOffsetResume();
	            	// Power on Alarm, need to set CFUN (1)
	                s32 iRet = Ql_RIL_SendATCmd("AT+CFUN=1", 9, NULL, NULL, 0);
	                APP_DEBUG("<-- Power on alarm, switch CFUN from 0 to 1, iRet=%d -->\r\n", iRet);
	            }
                break;
            default:
                APP_DEBUG("<-- Other URC: type=%d\r\n", msg.param1);
				//异常也保存盲区节点数据,小心驶得万年船
				//Blind_UpdateCirQueueInfo();
                break;
            }
            break;
		case MSG_ID_BLIND_SAVE:
			ret=Ql_Timer_Start(LOGIC_BLIND_SAVE_TMR_ID, msg.param1, FALSE);//10s没回应存盲区
			APP_DEBUG("\r\n<-- Blind Ql_Timer_Start %d -- >\r\n",ret );
			break;
		case MSG_ID_BLIND_CALLBACK:
			// 启动盲区上报回调函数定时器,单次
		    ret = Ql_Timer_Start(LOGIC_BLIND_REPORT_TMR_ID, msg.param1,FALSE);
			APP_DEBUG("\r\n<-- Blind Report Interval %d -- >\r\n",msg.param1 );
			break;
		case MSG_ID_RESET_MODULE_REQ:
			sucUpdatacount = 3;//写升级失败标志
            FRAM_BufferWrite(FRAM_FIRMWARE_UPDATA_FLAG_ADDR,&sucUpdatacount,1);
			Ql_Reset(0);
			break;
		case MSG_ID_GNSS_CTR:
			if(MSG_GNSS_ON==msg.param1)
			{
				JO_OpenGPSTimer();
			}
			else 
			if(MSG_GNSS_OFF==msg.param1)
			{
				CtrGnssOFF();
			}
			break;
        default:
            break;
        }
//		SoftWdg_Feed(BIT0);
    }
}

static s32 ReadSerialPort(Enum_SerialPort port, /*[out]*/u8* pBuffer, /*[in]*/u32 bufLen)
{
    s32 rdLen = 0;
    s32 rdTotalLen = 0;
    if (NULL == pBuffer || 0 == bufLen)
    {
        return -1;
    }
    Ql_memset(pBuffer, 0x0, bufLen);
    while (1)
    {
        rdLen = Ql_UART_Read(port, pBuffer + rdTotalLen, bufLen - rdTotalLen);
        if (rdLen <= 0)  // All data is read out, or Serial Port Error!
        {
            break;
        }
        rdTotalLen += rdLen;
        // Continue to read...
    }
    if (rdLen < 0) // Serial Port Error!
    {
        APP_DEBUG("Fail to read from port[%d]\r\n", port);
        return -99;
    }
    return rdTotalLen;
}


const FCT_INSTRUCT_PARSE FCT_InstructTab[] =
{
	{"AT+GETID",FCT_Response_GetID},							// 0   查询设备ID
	{"AT+GETADDR",FCT_Response_GetServerAddr},				// 1 查询服务器地址
	{"AT+GETPORT",FCT_Response_GetServerPort},				// 2   查询服务器端口
	{"AT+GETSENDINTERVAL",FCT_Response_GetReportInterval},	// 3 查询上报间隔
	{"AT+GETSIM",FCT_Response_GetSimCardNum},					// 4 查询SIM卡号
	{"AT+INFO",FCT_Response_GetDeviceInfo},					// 5 查询设备信息
	{"AT+POWEROFF",FCT_Response_EnterFactory},				// 6 进入库存模式
	{"AT+RESTART",FCT_Response_SysReset},						// 7 重启设备
	{"AT+SETID=",FCT_Response_SetID},							// 8设置ID
	{"AT+SETADDR=",FCT_Response_SetAddr},						// 9 设置服务器地址
	{"AT+SETPORT=",FCT_Response_SetPort},						// 10 设置端口
	{"AT+SETSENDINTERVAL=",FCT_Response_SetUpInterval}		// 11 设置上报间隔
};
#define INSTRUCTTAB_LEN		(sizeof(FCT_InstructTab)/sizeof(FCT_INSTRUCT_PARSE))

static void CallBack_UART_ParaSet_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void* customizedPara)
{
    //APP_DEBUG("CallBack_UART_Hdlr: port=%d, event=%d, level=%d, p=%x\r\n", port, msg, level, customizedPara);
    if(!ProjectInitFinshFlag)return;
    switch (msg)
    {
    case EVENT_UART_READY_TO_READ:
        {
            if (m_myUartPort == port)
            {
                s32 totalBytes = ReadSerialPort(port, m_RxBuf_Uart1, sizeof(m_RxBuf_Uart1));
                if (totalBytes <= 0)
                {
                    APP_DEBUG("<-- No data in UART buffer! -->\r\n");
                    return;
                }
				uart_set_default_mode();
				// Command: Ql_PowerDown
	            char* p = Ql_strstr(m_RxBuf_Uart1, "Ql_PowerDown");
	            if(p)
	            {
	            	//power down
	                APP_DEBUG("power down\r\n");
            		Blind_UpdateCirQueueInfo();
	                Ql_PowerDown(1);
					return;
	            }
				if(Ql_strstr(m_RxBuf_Uart1, "Ql_SysReset"))
	            {
	            	//power down
	                APP_DEBUG("Sys Reset\r\n");
					//重启之前备份盲区节点信息
            		Blind_UpdateCirQueueInfo();
	                Ql_Reset(0);
					return;
	            }
				//FCT模式下的一些指令解析
				if(0xaa == FCT_ReadSelfCheckFlg())
				{
					u8 i;
					char *ptr;
					for(i=0;i<INSTRUCTTAB_LEN;i++)
					{
						ptr = Ql_strstr(m_RxBuf_Uart1, FCT_InstructTab[i].instruct);
						if(NULL != ptr)
						{
							FCT_InstructTab[i].response(ptr);
							break;
						}
					}
				}
                {
					// Read data from UART 解析伊爱串口协议0xD0
                    //s32 ret;
                    ComParam_DisposeRecvData(m_RxBuf_Uart1, totalBytes);
                    // Echo
                    //Ql_UART_Write(m_myUartPort, m_RxBuf_Uart1, totalBytes);

                }
            }
            break;
        }
	case EVENT_UART_DTR_IND:// DTR level changed, developer can wake up the module here
        {
            if(0 == level)
            {
                APP_DEBUG("DTR set to low =%d  wake !!\r\n", level);
                Ql_SleepDisable();
            } else {
                APP_DEBUG("DTR set to high =%d  Sleep \r\n", level);
                Ql_SleepEnable();
            }

        }
    case EVENT_UART_READY_TO_WRITE:
        break;
    default:
        break;
    }
}

static s32 ATResponse_Handler(char* line, u32 len, void* userData)
{
    Ql_UART_Write(m_myUartPort, (u8*)line, len);
    
    if (Ql_RIL_FindLine(line, len, "OK"))
    {  
        return  RIL_ATRSP_SUCCESS;
    }
    else if (Ql_RIL_FindLine(line, len, "ERROR"))
    {  
        return  RIL_ATRSP_FAILED;
    }
    else if (Ql_RIL_FindString(line, len, "+CME ERROR"))
    {
        return  RIL_ATRSP_FAILED;
    }
    else if (Ql_RIL_FindString(line, len, "+CMS ERROR:"))
    {
        return  RIL_ATRSP_FAILED;
    }
    return RIL_ATRSP_CONTINUE; //continue wait
}

#endif // __CUSTOMER_CODE__
