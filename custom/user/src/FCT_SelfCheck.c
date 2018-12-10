#include "include.h"
#include "FCT_SelfCheck.h"


static u8 selfCheckFlg = 0;
extern u8 FCT_Test;
extern u32 battery_voltage;

#define cmdWatch_FCT_Handle( vNote, vFunction ) void vFunction(void* pRev)

cmdWatch_FCT_Handle(GetID,FCT_Response_GetID)
{
	u8 tab[20];
	char ch[20] = {'\0'};

	pRev = pRev;
	
	EepromPram_ReadPram(E2_DEVICE_ID,tab); 	
	Public_ConvertBcdToAsc(ch,tab,7);
	
	APP_DEBUG("AT+GETID=%s\r\n",ch);
}

cmdWatch_FCT_Handle(GetServerAddr,FCT_Response_GetServerAddr)
{
	char ch[20] = {'\0'};

	pRev = pRev;
	
	EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID,ch);	
	
	APP_DEBUG("AT+GETADDR=%s\r\n",ch);
}

cmdWatch_FCT_Handle(GetServerPort,FCT_Response_GetServerPort)
{
	u8 Buffer[4];
	u32 portVal;

	pRev = pRev;
	
	EepromPram_ReadPram(E2_MAIN_SERVER_TCP_PORT_ID,Buffer); 
	portVal = Public_ConvertBufferToLong(Buffer);
	
	APP_DEBUG("AT+GETPORT=%d\r\n",portVal);
}

cmdWatch_FCT_Handle(GetReportInterval,FCT_Response_GetReportInterval)
{
	u32 interval =	Report_GetDefaultReportInterval();

	pRev = pRev;
	
	APP_DEBUG("AT+GETSENDINTERVAL=%d\r\n",interval);
}
cmdWatch_FCT_Handle(GetSimCardNum,FCT_Response_GetSimCardNum)
{
	char ch[20] = {'\0'};	
	u8 Buffer[20];
	u8 terminalPhoneNum[6];
	u8 BufferLen = EepromPram_ReadPram(E2_DEVICE_ID, Buffer);

	pRev = pRev;
	
	Ql_memcpy(terminalPhoneNum,Buffer+1,6);
	terminalPhoneNum[0] &= 0x0f;

	Public_ConvertBcdToAsc(ch,terminalPhoneNum,6);
	
	APP_DEBUG("AT+GETSIM=%s\r\n",ch);
}
cmdWatch_FCT_Handle(GetDeviceInfo,FCT_Response_GetDeviceInfo)
{

	u8 tab[20];
	char ch[20] = {'\0'};
	u32 portVal = 0;
	
	pRev = pRev;
	
	EepromPram_ReadPram(E2_DEVICE_ID,tab); 	
	Public_ConvertBcdToAsc(ch,tab,7);	
	APP_DEBUG("ID:%s\r\n",ch);

	Ql_memset(ch,0,Ql_strlen(ch));
	EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID,ch);	
	APP_DEBUG("SERVER IP:%s\r\n",ch);

	EepromPram_ReadPram(E2_MAIN_SERVER_TCP_PORT_ID,tab);	
	portVal = Public_ConvertBufferToLong(tab);	
	APP_DEBUG("SERVER PORT:%d\r\n",portVal);

	Ql_memset(ch,0,Ql_strlen(ch));
	RIL_SIM_GetCCID(ch);
	APP_DEBUG("ICCID:%s\r\n",ch);

	Ql_memset(ch,0,Ql_strlen(ch));
	RIL_SIM_GetIMSI(ch);
	APP_DEBUG("IMSI:%s\r\n",ch+2);// 要去掉0D 0A 回车换行

	Ql_memset(ch,0,Ql_strlen(ch));
	RIL_GetIMEI(ch);
	APP_DEBUG("IMEI:%s\r\n",ch+2);

	APP_DEBUG("CELL VOLTAGE:%d\r\n",battery_voltage);
	
	Ql_memset(ch,0,Ql_strlen(ch));
	Version_GetHardwareInformation(ch);
	APP_DEBUG("FW VERSION:%s\r\n",ch);
	
	if(PUBLIC_CHECKBIT(FCT_Test,0))
	{
		APP_DEBUG("OK:SIMCARD CHECK OK\r\n");
	}
	else
	{
		APP_DEBUG("ERROR:SIMCARD CHECK FAILED\r\n");
	}

	if(PUBLIC_CHECKBIT(FCT_Test,1))
	{
		APP_DEBUG("OK:GPRS READY\r\n");
	}
	else
	{
		APP_DEBUG("ERROR:GPRS FAIL TO SIGN IN\r\n");
	}

	if(PUBLIC_CHECKBIT(FCT_Test,2))
	{
		APP_DEBUG("OK:NORMAL NETWORK TEST NTP\r\n");
	}
	else
	{
		APP_DEBUG("ERROR:NETWORK TEST FAILURE NTP\r\n");
	}
}

cmdWatch_FCT_Handle(EnterFactory,FCT_Response_EnterFactory)
{

	pRev = pRev;
	
	APP_DEBUG("AT+POWEROFF\r\nOK");
	Ql_Sleep(100);
	REMOTE_SHUTDOWN();
}

cmdWatch_FCT_Handle(SysReset,FCT_Response_SysReset)
{

	pRev = pRev;
	
	APP_DEBUG("AT+RESTART\r\nOK");
	Ql_Sleep(100);
	Ql_Reset(0);
}


cmdWatch_FCT_Handle(SetID,FCT_Response_SetID)
{
	char *ch = (char*)pRev;
	u8 buffer[7];
	
	ch += 9;
	Public_ConvertAscToBCD(buffer,ch,14);
	EepromPram_WritePram(E2_DEVICE_ID,buffer,7);
	EepromPram_WritePram(E2_DEVICE_PHONE_ID, buffer+1, E2_DEVICE_PHONE_ID_LEN);	
	APP_DEBUG("AT+SETID=%s\r\nOK",ch);

}

cmdWatch_FCT_Handle(SetAddr,FCT_Response_SetAddr)
{
	char *ch = (char*)pRev;
	
	ch += 11;
	EepromPram_WritePram(E2_MAIN_SERVER_IP_ID,ch,Ql_strlen(ch));	
	APP_DEBUG("AT+SETADDR=%s\r\nOK",ch);

}

cmdWatch_FCT_Handle(SetPort,FCT_Response_SetPort)
{
	char *ch = (char*)pRev;
	u32 tmp;
	u8  Buffer[4];
	
	ch += 11;
	tmp = atol(ch);
	Public_ConvertLongToBuffer(tmp, Buffer);
    EepromPram_WritePram(E2_MAIN_SERVER_TCP_PORT_ID, Buffer, E2_MAIN_SERVER_TCP_PORT_LEN);	
	APP_DEBUG("AT+SETPORT=%s\r\nOK",ch);

}

cmdWatch_FCT_Handle(SetUpInterval,FCT_Response_SetUpInterval)
{
	char *ch = (char*)pRev;
	u32 tmp;
	u8  Buffer[4];
	
	ch += 19;
	tmp = atol(ch);
	Public_ConvertLongToBuffer(tmp, Buffer);
    EepromPram_WritePram(E2_SLEEP_REPORT_TIME_ID, Buffer, E2_SLEEP_REPORT_TIME_LEN);	
	APP_DEBUG("AT+SETSENDINTERVAL=%s\r\nOK",ch);

}

//FCT读自检标志
u8 FCT_ReadSelfCheckFlg(void)
{
	return selfCheckFlg;
}

void FCT_SelfCheckInit(void)
{

	FRAM_BufferRead(&selfCheckFlg, FRAM_FCT_SELF_CHECK_LEN, FRAM_FCT_SELF_CHECK_ADDR);
	
	if(0xaa != selfCheckFlg)
	{
		selfCheckFlg = 0xaa;
		FRAM_BufferWrite(FRAM_FCT_SELF_CHECK_ADDR,&selfCheckFlg,FRAM_FCT_SELF_CHECK_LEN);
		//进入FCT模式
		FCT_DEBUG("MODE:FCTMODE\r\n");
	}
	else
	{
		selfCheckFlg = 0;
		FCT_DEBUG("MODE:NORMALMODE\r\n");
	}
}

