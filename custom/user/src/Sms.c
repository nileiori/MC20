#include "include.h"
#include "ril.h"
#include "ril_util.h"
#include "ril_sms.h"
#include "ril_telephony.h"
#include "ril_system.h"
#include "ql_stdlib.h"
#include "ql_error.h"
#include "ql_trace.h"
#include "ql_uart.h"
#include "ql_system.h"
#include "ql_memory.h"
#include "ql_timer.h"

#include "Sms.h"

u8 SmsOilFlag=0;
u8	Simccid[12]; //CCID编号

char SmsTextMonitorPhoNume[20];//监控平台SMS电话号码
char SmsTextAlarmPhoNume[20];//用户短信报警电话
extern TIME_T CurTime;

/***********************************************************************
 * MACRO CONSTANT DEFINITIONS
************************************************************************/

#define CON_SMS_BUF_MAX_CNT   (1)
#define CON_SMS_SEG_MAX_CHAR  (160)
#define CON_SMS_SEG_MAX_BYTE  (4 * CON_SMS_SEG_MAX_CHAR)
#define CON_SMS_MAX_SEG       (7)

/***********************************************************************
 * STRUCT TYPE DEFINITIONS
************************************************************************/
typedef struct
{
    u8 aData[CON_SMS_SEG_MAX_BYTE];
    u16 uLen;
} ConSMSSegStruct;

typedef struct
{
    u16 uMsgRef;
    u8 uMsgTot;

    ConSMSSegStruct asSeg[CON_SMS_MAX_SEG];
    bool abSegValid[CON_SMS_MAX_SEG];
} ConSMSStruct;

/***********************************************************************
 * FUNCTION DECLARATIONS
************************************************************************/
static bool ConSMSBuf_IsIntact(ConSMSStruct *pCSBuf,u8 uCSMaxCnt,u8 uIdx,ST_RIL_SMS_Con *pCon);
static bool ConSMSBuf_AddSeg(ConSMSStruct *pCSBuf,u8 uCSMaxCnt,u8 uIdx,ST_RIL_SMS_Con *pCon,u8 *pData,u16 uLen);
static s8 ConSMSBuf_GetIndex(ConSMSStruct *pCSBuf,u8 uCSMaxCnt,ST_RIL_SMS_Con *pCon);
static bool ConSMSBuf_ResetCtx(ConSMSStruct *pCSBuf,u8 uCSMaxCnt,u8 uIdx);

/***********************************************************************
 * GLOBAL DATA DEFINITIONS
************************************************************************/
ConSMSStruct g_asConSMSBuf[CON_SMS_BUF_MAX_CNT];

/***********************************************************************
 * MACRO FUNCTION DEFINITIONS
************************************************************************/
u8 AsicToUnicode(u8 *_str,u32 decnum)
{
	u8 pr;
	u8 i;
	u32 de;
	u8 flag=0;
	u32 temp;
	u8 bb;
	u8 seg;
	char *str = _str;
	
	
	 //先判断decnum的有效位数
	 temp = decnum;
	 de=10;
	 bb=1;
	 for(i=0;i<10;i++)
	 {
		 if(temp/de>0)
			 bb++;
		 else
			 break;
		 de*=10;
	 }
	 seg=0;
	 de=1000000000;
	 for(i=0;i<10;i++)
	 {
		 pr=decnum/de%10;
		 de/=10;
		 if((pr==0)&&(flag==0)&&(de>0))
		 continue;
		 flag=1; 
		 if(seg>128)return 0;
		 str[seg++]='0';
		 str[seg++]='0';
		 str[seg++]='3';
		 str[seg++]=pr+'0';
		 //seg++;
	 }
	 str[seg]='\0'; //加上结束符
	 return seg;

	
}



/**
  * @brief	比较两条命令是否一致
  * @param	*pdst: 指向目标数据地址
  * @param	*psrc: 指向源数据地址
  * @retval 返回对比差值 0表示相等
  */
u16  ModLib_AtCmp(u8 *pdst, u8 *psrc)
{
	u16  len=0;
	
	len = strlen((char*)psrc);

	len = strncmp((char *)pdst,(char *)psrc,len);
	
	return len;
}


/*
  * @brief	授权电话设置
  * @param	*p:指向搜索地址
  * @param	len:搜索地址数据的长度
  * @retval None 123456,center,xxxxxxxxxxx#
 */
u8	Sms_Set_TextAlarmPhone(u8 *pRec, u16 len)
{
	u8 *p=pRec,i=0;
	u8 datalen;
    u8 ntab[20]={0};
	char ch[10] = {'\0'};
	for(i=0;i<len;i++)
	{
	
	if(*p==',')
		break;
		p++;
	}
	p++;
	datalen = EepromPram_ReadPram(E2_SMS_OPERATE_PWD_ID, ch);
	if(ModLib_AtCmp(pRec,ch) != 0)//密码校验错误
	{
		//恢复初始密码123456
		const char *defaultPsw = "123456";
		EepromPram_WritePram(E2_SMS_OPERATE_PWD_ID, defaultPsw, E2_SMS_OPERATE_PWD_ID_LEN);
		SMS_TextMode_Send(LIB_SMS_CHARSET_UCS2,SmsTextAlarmPhoNume,SET_ALARM_PHONE_WRONG_PASSWORD,Ql_strlen(SET_ALARM_PHONE_WRONG_PASSWORD));
		return 0;
	}
   	if(ModLib_AtCmp(p,"center,d") == 0)////////SMS监控号码 
	{
		memset(ntab,0,sizeof(ntab)); 
		EepromPram_WritePram(E2_SMS_ALARM_PHONE_NUM_ID, ntab, sizeof(ntab));
		SMS_TextMode_Send(LIB_SMS_CHARSET_UCS2,SmsTextAlarmPhoNume,DEL_ALARM_PHONE_SUCCESS,Ql_strlen(DEL_ALARM_PHONE_SUCCESS));
	}
	else//设置
	{
		//写密码
		memcpy(ntab,pRec,E2_SMS_OPERATE_PWD_ID_LEN);
		EepromPram_WritePram(E2_SMS_OPERATE_PWD_ID, ntab, E2_SMS_OPERATE_PWD_ID_LEN);
		p=p+7;
		for(datalen=0;datalen<len-7;datalen++)
		{
			if((*(p+datalen))=='#')
			{
				break;
			}
		}
		memset(ntab,0,sizeof(ntab));
	    memcpy(ntab,p,datalen);  
		EepromPram_WritePram(E2_SMS_ALARM_PHONE_NUM_ID, ntab, datalen);
		UpSmsAlarmPhoNum();
		SMS_TextMode_Send(LIB_SMS_CHARSET_UCS2,SmsTextAlarmPhoNume,SET_ALARM_PHONE_SUCCESS,Ql_strlen(SET_ALARM_PHONE_SUCCESS));
	}
	return 1;
}

/*
  * @brief  短信设置IP
  * @param  type: IP地址连接为0，域名连接为1
  * @param  *p:指向搜索地址
  * @param  len:搜索地址数据的长度
  * @retval None
 */
u8  Sms_Set_Ip(u8 type,u8 *pRec, u16 len)
{

    u16 i=0;// ip,219.142.106.193,port,8500#  surl,www.gps7788.com,port,8500#
    u8  *p=NULL;
    u8  tab[50]={0};
    u8  ip[25]={0};
    u8  port[5]={0};
    u16 dport=0;
    u8  cnt1=0;//计算','的个数
    u8  cnt2=0;//计算'.'的个数
    u8  cnt3=0;//计算'#'的个数
    u8  addrbuf[5]={0};
	

    if(len > sizeof(tab))
    {
        return 0;//内存溢出
    }
    memcpy(tab,pRec,len);
    p=tab;

    for(i=0;i<len;i++)
    {
        if(*(p+i) == ',')
        {
            addrbuf[cnt1++] = i;
        }
        else if(*(p+i) == '.')
        {
            cnt2++;
        }
        else if(*(p+i) == '#')
        {
            cnt3++;
            addrbuf[cnt1] = i;
        }        
    }
    if(type == 0)//ip地址连接
    {
        if((cnt1 != 3)||(cnt2 != 3)||(cnt3 != 1))
        {
            return 0;//格式不符合要求返回错误
        }
    }
    else//域名连接
    {
        if((cnt1 != 3)||(cnt3 != 1))
        {
            return 0;//格式不符合要求返回错误
        }
    }
    
	memset(ip,0, sizeof(ip));
	
    memcpy(ip,p+addrbuf[0]+1,addrbuf[1]-addrbuf[0]-1);//IP地址或域名
    memcpy(port,p+addrbuf[2]+1,addrbuf[3]-addrbuf[2]-1);//端口
    
    dport =  atoi((char*)port);
    port[0] = 0;
    port[1] = 0;
    port[2] = dport>>8;
    port[3] = dport&0xff;
    port[4] = 0;
    EepromPram_WritePram(E2_MAIN_SERVER_IP_ID,ip,Ql_strlen(ip));
    EepromPram_WritePram(E2_MAIN_SERVER_TCP_PORT_ID,port,E2_MAIN_SERVER_TCP_PORT_LEN);
	SMS_TextMode_Send(LIB_SMS_CHARSET_UCS2,SmsTextAlarmPhoNume,SET_IP_PORT_SUCCESS,Ql_strlen(SET_IP_PORT_SUCCESS));

	
    return 1;
}

/*
  * @brief  短信设置终端手机号
  * @param  *p:指向搜索地址
  * @param  len:搜索地址数据的长度
  * @retval None
 */
u8  Sms_Set_DevicePhone(u8 *pRec, u16 len)
{//ID,0000xxxxxxx0#  (11位ID)

    u8 tab[20]={0};
    u8 num[10]={0};
    u8 tabnum[20]={0};
    u8 nlen=0;

    u8 i;
    u8 *p = pRec;
    u8 cnt=0;
    u8 addr;
    
    for(i=0;i<len;i++)
    {
        if(*(p+i) == '#')
        {
            cnt++;
            addr=i;
        }
    }
    if(cnt != 1)
    {
        return 0;
    }


    memcpy(tab,p+3,addr-3);
    nlen = strlen((char *)tab);
    if(nlen%2)
    {
        tabnum[0]='0';
        memcpy(tabnum+1,tab,nlen);
        nlen++;
    }
    else
    {
        memcpy(tabnum,tab,nlen);
    }
    ModLib_StrByte(tabnum,num,nlen);  
    EepromPram_WritePram(E2_DEVICE_PHONE_ID,num,E2_DEVICE_PHONE_ID_LEN); 
	SMS_TextMode_Send(LIB_SMS_CHARSET_UCS2,SmsTextAlarmPhoNume,SET_DEVIECE_ID_SUCCESS,Ql_strlen(SET_DEVIECE_ID_SUCCESS));

    return 1;
}

//短信号码设置ACC_ON上报时间
void Sms_Set_ReportTime(u8 *pRec, u16 len)
{
    u32	temp=0;
	s8 err;
	u8 Buffer[4];
	
    APP_DEBUG("\r\n< Sms_Set_ReportTime %s... >\r\n",pRec);
	temp=asctodec(pRec,len-1,&err);
	//0x0029	缺省时间汇报间隔,单位为秒,默认为30
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_SLEEP_REPORT_TIME_ID, Buffer, E2_SLEEP_REPORT_TIME_LEN))
    {
        EepromPram_WritePram(E2_SLEEP_REPORT_TIME_ID, Buffer, E2_SLEEP_REPORT_TIME_LEN);
    }
	Report_UpdatePram();


	Ql_memset(ShareBuffer,0,sizeof(ShareBuffer));
	
	Ql_memcpy(ShareBuffer,SET_REPORT_TIME_SUCCESS,Ql_strlen(SET_REPORT_TIME_SUCCESS));
	temp=AsicToUnicode(ShareBuffer+Ql_strlen(SET_REPORT_TIME_SUCCESS),temp);

	//后面增加S标志
	ShareBuffer[Ql_strlen(SET_REPORT_TIME_SUCCESS)+temp]='0';
	ShareBuffer[Ql_strlen(SET_REPORT_TIME_SUCCESS)+temp+1]='0';
	ShareBuffer[Ql_strlen(SET_REPORT_TIME_SUCCESS)+temp+2]='5';
	ShareBuffer[Ql_strlen(SET_REPORT_TIME_SUCCESS)+temp+3]='3';


	
    APP_DEBUG("\r\n<-- Sms_data: %s-- >\r\n",ShareBuffer);
	SMS_TextMode_Send(LIB_SMS_CHARSET_UCS2,SmsTextAlarmPhoNume,ShareBuffer,Ql_strlen(SET_REPORT_TIME_SUCCESS)+temp+4);
}

void Sms_Check_DevicePram(u8 *pRec, u16 len)
{
	u8 temp=0;
	u8 tab[50];
	u32 pradata;
	Ql_memset(ShareBuffer,0,sizeof(ShareBuffer));
	
	Ql_memcpy(ShareBuffer,"ID:",3);
	temp+=3;

    EepromPram_ReadPram(E2_DEVICE_ID,tab); 
	
	Public_ConvertBcdToAsc(ShareBuffer+temp,tab,7);
	temp+=14;
	
	Ql_memcpy(ShareBuffer+temp,",apn:",5);
	temp+=5;
	
	temp += EepromPram_ReadPram(E2_MAIN_SERVER_APN_ID,ShareBuffer+temp); 
	
	Ql_memcpy(ShareBuffer+temp,",ip:",4);
	temp+=4;

	
	pradata= EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID,tab); 
	Ql_memcpy(ShareBuffer+temp,tab,pradata);
	temp+=pradata;
	
	//temp += EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID,ShareBuffer+temp); 

	Ql_memcpy(ShareBuffer+temp,",upload:",8);
	temp+=8;


	EepromPram_ReadPram(E2_SLEEP_REPORT_TIME_ID,tab);
	pradata = Public_ConvertBufferToLong(tab);
	temp+=dectostr(pradata,(ShareBuffer+temp),0);
	//temp+=AsicToUnicode((ShareBuffer+temp),pradata);

	
	Ql_memcpy(ShareBuffer+temp,",sleep:",7);
	temp+=7;
	
	EepromPram_ReadPram(E2_SLEEP_REPORT_TIME_ID,tab);
	pradata = Public_ConvertBufferToLong(tab);
	//temp+=AsicToUnicode((ShareBuffer+temp),pradata);
	temp+=dectostr(pradata,(ShareBuffer+temp),0);

	
    APP_DEBUG("\r\n<-- Sms_data: %s-- >\r\n",ShareBuffer);
	SMS_TextMode_Send(LIB_SMS_CHARSET_GSM,SmsTextAlarmPhoNume,ShareBuffer,temp);

}
#if 0//追货宝没这个功能
void Sms_Check_GooglePosition(u8 *pRec, u16 len)
{
	u8 temp=0;
    float gps=0;
	GPS_STRUCT Position;
	Gps_CopygPosition(&Position);


	
	Ql_memset(ShareBuffer,0,sizeof(ShareBuffer));

	
	Ql_memcpy(ShareBuffer,"<Datetime:",10);
	temp+=10;
	temp+=dectostr(CurTime.year,(ShareBuffer+temp),0);//年
	ShareBuffer[temp]='-';temp+=1;
	temp+=dectostr(CurTime.month,(ShareBuffer+temp),0);//月
	ShareBuffer[temp]='-';temp+=1;
	temp+=dectostr(CurTime.day,(ShareBuffer+temp),0);//日
	
	ShareBuffer[temp]=' ';temp+=1;
	temp+=dectostr(CurTime.hour,(ShareBuffer+temp),0);//时
	ShareBuffer[temp]=':';temp+=1;
	temp+=dectostr(CurTime.min,(ShareBuffer+temp),0);//分
	ShareBuffer[temp]=':';temp+=1;
	temp+=dectostr(CurTime.sec,(ShareBuffer+temp),0);//秒
	ShareBuffer[temp]='>';temp+=1;
	
	Ql_memcpy(ShareBuffer+temp,"http://maps.google.com/maps?q=",30);
	temp+=30;
	if(Position.North==1)
	{
		ShareBuffer[temp]='N';temp+=1;
	}
	else
	{
		ShareBuffer[temp]='S';temp+=1;
	}

	//纬度
	temp+=dectostr(Position.Latitue_D,(ShareBuffer+temp),0);//度
	ShareBuffer[temp]='.';temp+=1;
    gps=Position.Latitue_F+Position.Latitue_FX/100.0;//分
	temp+=dectostr(gps,(ShareBuffer+temp),0);
	ShareBuffer[temp]=',';temp+=1;

	
	if(Position.East==1)
	{
		ShareBuffer[temp]='E';temp+=1;
	}
	else
	{
		ShareBuffer[temp]='W';temp+=1;
	}
	//经度
	temp+=dectostr(Position.Longitue_D,(ShareBuffer+temp),0);//度
	ShareBuffer[temp]='.';temp+=1;
    gps=Position.Longitue_F+Position.Longitue_FX/100.0;//分
	temp+=dectostr(gps,(ShareBuffer+temp),0);	
	

	
	SMS_TextMode_Send(LIB_SMS_CHARSET_GSM,SmsTextAlarmPhoNume,ShareBuffer,temp);

}
void Sms_OilControl(u8 *pRec, u16 len)
{
	SmsOilFlag=1;
	if(ModLib_AtCmp(pRec,"relay,1") == 0)
		{
			OilControl_OffOil();
		}
	else
		{ 
            OilControl_OpenOil();
		}
	
}
#endif
void SmsDataCheck(char *pPhoneNum,u8 *pData,u8 len)
{
    if(ModLib_AtCmp(pData,"pw,") == 0)////////SMS监控号码 
		{
			Sms_Set_TextAlarmPhone(pData+3,len-3);
			return;
		}
	if(Ql_strcmp( pPhoneNum,SmsTextAlarmPhoNume)==0)
		{
			if ((ModLib_AtCmp(pData,"ip,") == 0)||(ModLib_AtCmp(pData,"surl,") == 0))////IP设置
			{	//ip,219.142.106.193,port,8500#  surl,www.gps7788.com,port,8500# 
				Sms_Set_Ip(1,pData,len);
			}
			else if(ModLib_AtCmp(pData,"upload,") == 0)///////////////////设置定时上报时间间隔
		    {
				Sms_Set_ReportTime(pData+7,len-7);
		    }
		    else if(ModLib_AtCmp(pData,"ID,") == 0)//////////////////终端手机号设置  
			{//ID,0000xxxxxxx0#  (11位ID)
				Sms_Set_DevicePhone(pData,len);
			}
			else if(ModLib_AtCmp(pData,"reset") == 0)///////////////////系统复位
		    {    
		    	//恢复初始密码123456
				const char *defaultPsw = "123456";
				EepromPram_WritePram(E2_SMS_OPERATE_PWD_ID, defaultPsw, E2_SMS_OPERATE_PWD_ID_LEN);
				SMS_TextMode_Send(LIB_SMS_CHARSET_UCS2,SmsTextAlarmPhoNume,SET_REST_SUCCESS,Ql_strlen(SET_REST_SUCCESS));
				extern u16 DelayAckCommand;//延时应答命令
		        DelayAckCommand = 7;//下线，重启    
		        //触发延时任务
		        SetTimerTask(TIME_DELAY_TRIG,3*SECOND);   
		       // Modem_Api_SmsSendString(smstpye,"rest is OK!");
		       //NetOther_SysRestDelay = MODEM_TMIN;//延时1分钟重启
		       //NetOther_State        = MODEM_APP_SYSREST;
		    }
			else if(ModLib_AtCmp(pData,"check#") == 0)///////////////////
		    {
				Sms_Check_DevicePram(pData,len);
		    }
			/*else if(ModLib_AtCmp(pData,"url#") == 0)///////////////////
		    {
				Sms_Check_GooglePosition(pData,len);
		    }
			else if(ModLib_AtCmp(pData,"relay,") == 0)///////////////////
		    {
				Sms_OilControl(pData,len);
		    }*/
			else
			{
				SMS_TextMode_Send(LIB_SMS_CHARSET_UCS2,SmsTextAlarmPhoNume,ILLAGAL_ORDER,Ql_strlen(ILLAGAL_ORDER));
			}
		}
}



/*****************************************************************************
 * FUNCTION
 *  ConSMSBuf_GetIndex
 *
 * DESCRIPTION
 *  This function is used to get available index in <pCSBuf>
 *  
 * PARAMETERS
 *  <pCSBuf>     The SMS index in storage,it starts from 1
 *  <uCSMaxCnt>  TRUE: The module should reply a SMS to the sender; FALSE: The module only read this SMS.
 *  <pCon>       The pointer of 'ST_RIL_SMS_Con' data
 *
 * RETURNS
 *  -1:   FAIL! Can not get available index
 *  OTHER VALUES: SUCCESS.
 *
 * NOTE
 *  1. This is an internal function
 *****************************************************************************/
static s8 ConSMSBuf_GetIndex(ConSMSStruct *pCSBuf,u8 uCSMaxCnt,ST_RIL_SMS_Con *pCon)
{
	u8 uIdx = 0;
	
    if(    (NULL == pCSBuf) || (0 == uCSMaxCnt) 
        || (NULL == pCon)
      )
    {
        APP_DEBUG("Enter ConSMSBuf_GetIndex,FAIL! Parameter is INVALID. pCSBuf:%x,uCSMaxCnt:%d,pCon:%x\r\n",pCSBuf,uCSMaxCnt,pCon);
        return -1;
    }

    if((pCon->msgTot) > CON_SMS_MAX_SEG)
    {
        APP_DEBUG("Enter ConSMSBuf_GetIndex,FAIL! msgTot:%d is larger than limit:%d\r\n",pCon->msgTot,CON_SMS_MAX_SEG);
        return -1;
    }
    
	for(uIdx = 0; uIdx < uCSMaxCnt; uIdx++)  //Match all exist records
	{
        if(    (pCon->msgRef == pCSBuf[uIdx].uMsgRef)
            && (pCon->msgTot == pCSBuf[uIdx].uMsgTot)
          )
        {
            return uIdx;
        }
	}

	for (uIdx = 0; uIdx < uCSMaxCnt; uIdx++)
	{
		if (0 == pCSBuf[uIdx].uMsgTot)  //Find the first unused record
		{
            pCSBuf[uIdx].uMsgTot = pCon->msgTot;
            pCSBuf[uIdx].uMsgRef = pCon->msgRef;
            
			return uIdx;
		}
	}

    APP_DEBUG("Enter ConSMSBuf_GetIndex,FAIL! No avail index in ConSMSBuf,uCSMaxCnt:%d\r\n",uCSMaxCnt);
    
	return -1;
}

/*****************************************************************************
 * FUNCTION
 *  ConSMSBuf_AddSeg
 *
 * DESCRIPTION
 *  This function is used to add segment in <pCSBuf>
 *  
 * PARAMETERS
 *  <pCSBuf>     The SMS index in storage,it starts from 1
 *  <uCSMaxCnt>  TRUE: The module should reply a SMS to the sender; FALSE: The module only read this SMS.
 *  <uIdx>       Index of <pCSBuf> which will be stored
 *  <pCon>       The pointer of 'ST_RIL_SMS_Con' data
 *  <pData>      The pointer of CON-SMS-SEG data
 *  <uLen>       The length of CON-SMS-SEG data
 *
 * RETURNS
 *  FALSE:   FAIL!
 *  TRUE: SUCCESS.
 *
 * NOTE
 *  1. This is an internal function
 *****************************************************************************/
static bool ConSMSBuf_AddSeg(ConSMSStruct *pCSBuf,u8 uCSMaxCnt,u8 uIdx,ST_RIL_SMS_Con *pCon,u8 *pData,u16 uLen)
{
    u8 uSeg = 1;
    
    if(    (NULL == pCSBuf) || (0 == uCSMaxCnt) 
        || (uIdx >= uCSMaxCnt)
        || (NULL == pCon)
        || (NULL == pData)
        || (uLen > (CON_SMS_SEG_MAX_CHAR * 4))
      )
    {
        APP_DEBUG("Enter ConSMSBuf_AddSeg,FAIL! Parameter is INVALID. pCSBuf:%x,uCSMaxCnt:%d,uIdx:%d,pCon:%x,pData:%x,uLen:%d\r\n",pCSBuf,uCSMaxCnt,uIdx,pCon,pData,uLen);
        return FALSE;
    }

    if((pCon->msgTot) > CON_SMS_MAX_SEG)
    {
        APP_DEBUG("Enter ConSMSBuf_GetIndex,FAIL! msgTot:%d is larger than limit:%d\r\n",pCon->msgTot,CON_SMS_MAX_SEG);
        return FALSE;
    }

    uSeg = pCon->msgSeg;
    pCSBuf[uIdx].abSegValid[uSeg-1] = TRUE;
    Ql_memcpy(pCSBuf[uIdx].asSeg[uSeg-1].aData,pData,uLen);
    pCSBuf[uIdx].asSeg[uSeg-1].uLen = uLen;
    
	return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  ConSMSBuf_IsIntact
 *
 * DESCRIPTION
 *  This function is used to check the CON-SMS is intact or not
 *  
 * PARAMETERS
 *  <pCSBuf>     The SMS index in storage,it starts from 1
 *  <uCSMaxCnt>  TRUE: The module should reply a SMS to the sender; FALSE: The module only read this SMS.
 *  <uIdx>       Index of <pCSBuf> which will be stored
 *  <pCon>       The pointer of 'ST_RIL_SMS_Con' data
 *
 * RETURNS
 *  FALSE:   FAIL!
 *  TRUE: SUCCESS.
 *
 * NOTE
 *  1. This is an internal function
 *****************************************************************************/
static bool ConSMSBuf_IsIntact(ConSMSStruct *pCSBuf,u8 uCSMaxCnt,u8 uIdx,ST_RIL_SMS_Con *pCon)
{
    u8 uSeg = 1;
	
    if(    (NULL == pCSBuf) 
        || (0 == uCSMaxCnt) 
        || (uIdx >= uCSMaxCnt)
        || (NULL == pCon)
      )
    {
        APP_DEBUG("Enter ConSMSBuf_IsIntact,FAIL! Parameter is INVALID. pCSBuf:%x,uCSMaxCnt:%d,uIdx:%d,pCon:%x\r\n",pCSBuf,uCSMaxCnt,uIdx,pCon);
        return FALSE;
    }

    if((pCon->msgTot) > CON_SMS_MAX_SEG)
    {
        APP_DEBUG("Enter ConSMSBuf_GetIndex,FAIL! msgTot:%d is larger than limit:%d\r\n",pCon->msgTot,CON_SMS_MAX_SEG);
        return FALSE;
    }
        
	for (uSeg = 1; uSeg <= (pCon->msgTot); uSeg++)
	{
        if(FALSE == pCSBuf[uIdx].abSegValid[uSeg-1])
        {
            APP_DEBUG("Enter ConSMSBuf_IsIntact,FAIL! uSeg:%d has not received!\r\n",uSeg);
            return FALSE;
        }
	}
    
    return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  ConSMSBuf_ResetCtx
 *
 * DESCRIPTION
 *  This function is used to reset ConSMSBuf context
 *  
 * PARAMETERS
 *  <pCSBuf>     The SMS index in storage,it starts from 1
 *  <uCSMaxCnt>  TRUE: The module should reply a SMS to the sender; FALSE: The module only read this SMS.
 *  <uIdx>       Index of <pCSBuf> which will be stored
 *
 * RETURNS
 *  FALSE:   FAIL!
 *  TRUE: SUCCESS.
 *
 * NOTE
 *  1. This is an internal function
 *****************************************************************************/
static bool ConSMSBuf_ResetCtx(ConSMSStruct *pCSBuf,u8 uCSMaxCnt,u8 uIdx)
{
    if(    (NULL == pCSBuf) || (0 == uCSMaxCnt) 
        || (uIdx >= uCSMaxCnt)
      )
    {
        APP_DEBUG("Enter ConSMSBuf_ResetCtx,FAIL! Parameter is INVALID. pCSBuf:%x,uCSMaxCnt:%d,uIdx:%d\r\n",pCSBuf,uCSMaxCnt,uIdx);
        return FALSE;
    }
    
    //Default reset
    Ql_memset(&pCSBuf[uIdx],0x00,sizeof(ConSMSStruct));

    //TODO: Add special reset here
    
    return TRUE;
}

//监控平台电话号码
void UpSmsMonitorPhoNum(void)
{
	Ql_memset(SmsTextMonitorPhoNume, 0,sizeof(SmsTextMonitorPhoNume));

	SmsTextMonitorPhoNume[0]='+';
	SmsTextMonitorPhoNume[1]='8';
	SmsTextMonitorPhoNume[2]='6';
	EepromPram_ReadPram(E2_MONITOR_SERVER_SMS_PHONE_ID, SmsTextMonitorPhoNume+3);
	APP_DEBUG("SmsTextMonitorPhoNume=%s\r\n",SmsTextMonitorPhoNume);

}
//用户短信报警电话
void UpSmsAlarmPhoNum(void)
{
	Ql_memset(SmsTextAlarmPhoNume, 0,sizeof(SmsTextAlarmPhoNume));

	SmsTextAlarmPhoNume[0]='+';
	SmsTextAlarmPhoNume[1]='8';
	SmsTextAlarmPhoNume[2]='6';
	EepromPram_ReadPram(E2_SMS_ALARM_PHONE_NUM_ID, SmsTextAlarmPhoNume+3);
	APP_DEBUG("SmsAlarmPhoNum=%s\r\n",SmsTextAlarmPhoNume);

}

/*****************************************************************************
 * FUNCTION
 *  SMS_Initialize
 *
 * DESCRIPTION
 *  Initialize SMS environment.
 *  
 * PARAMETERS
 *  VOID
 *
 * RETURNS
 *  TRUE:  This function works SUCCESS.
 *  FALSE: This function works FAIL!
 *****************************************************************************/
bool SMS_Initialize(void)
{
    s32 iResult = 0;
    u8  nCurrStorage = 0;
    u32 nUsed = 0;
    u32 nTotal = 0;

	UpSmsAlarmPhoNum();

	
    // Set SMS storage:
    // By default, short message is stored into SIM card. You can change the storage to ME if needed, or
    // you can do it again to make sure the short message storage is SIM card.
    #if 0
    {
        iResult = RIL_SMS_SetStorage(RIL_SMS_STORAGE_TYPE_SM,&nUsed,&nTotal);
        if (RIL_ATRSP_SUCCESS != iResult)
        {
            APP_DEBUG("Fail to set SMS storage, cause:%d\r\n", iResult);
            return FALSE;
        }
        APP_DEBUG("<-- Set SMS storage to SM, nUsed:%u,nTotal:%u -->\r\n", nUsed, nTotal);

        iResult = RIL_SMS_GetStorage(&nCurrStorage, &nUsed ,&nTotal);
        if(RIL_ATRSP_SUCCESS != iResult)
        {
            APP_DEBUG("Fail to get SMS storage, cause:%d\r\n", iResult);
            return FALSE;
        }
        APP_DEBUG("<-- Check SMS storage: curMem=%d, used=%d, total=%d -->\r\n", nCurrStorage, nUsed, nTotal);
    }
    #endif

    // Enable new short message indication
    // By default, the auto-indication for new short message is enalbed. You can do it again to 
    // make sure that the option is open.
    #if 0
    {
        iResult = Ql_RIL_SendATCmd("AT+CNMI=2,1",Ql_strlen("AT+CNMI=2,1"),NULL,NULL,0);
        if (RIL_AT_SUCCESS != iResult)
        {
            APP_DEBUG("Fail to send \"AT+CNMI=2,1\", cause:%d\r\n", iResult);
            return FALSE;
        }
        APP_DEBUG("<-- Enable new SMS indication -->\r\n");
    }
    #endif

    // Delete all existed short messages (if needed)
    iResult = RIL_SMS_DeleteSMS(0, RIL_SMS_DEL_ALL_MSG);
    if (iResult != RIL_AT_SUCCESS)
    {
        APP_DEBUG("Fail to delete all messages, iResult=%d,cause:%d\r\n", iResult, Ql_RIL_AT_GetErrCode());
        return FALSE;
    }
    APP_DEBUG("Delete all existed messages\r\n");
    
    return TRUE;
}



void SMS_TextMode_Send(LIB_SMS_CharSetEnum SmsType,char *strPhNum,char *strTestMsg,u8 MsgLen)
{
    s32 iResult;
    u32 nMsgRef;
    iResult = RIL_SMS_SendSMS_Text(strPhNum, Ql_strlen(strPhNum), SmsType, strTestMsg, MsgLen, &nMsgRef);
    if (iResult != RIL_AT_SUCCESS)
    {   
        APP_DEBUG("< Fail to send Text SMS, iResult=%d, cause:%d >\r\n", iResult, Ql_RIL_AT_GetErrCode());
        return;
    }
    APP_DEBUG("< Send Text SMS successfully, MsgRef:%u >\r\n", nMsgRef); 
}

/*****************************************************************************
 * FUNCTION
 *  Hdlr_RecvNewSMS
 *
 * DESCRIPTION
 *  The handler function of new received SMS.
 *  
 * PARAMETERS
 *  <nIndex>     The SMS index in storage,it starts from 1
 *  <bAutoReply> TRUE: The module should reply a SMS to the sender; 
 *               FALSE: The module only read this SMS.
 *
 * RETURNS
 *  VOID
 *
 * NOTE
 *  1. This is an internal function
 *****************************************************************************/
void Hdlr_RecvNewSMS(u32 nIndex, bool bAutoReply)
{
    s32 iResult = 0;
    u32 uMsgRef = 0;
    ST_RIL_SMS_TextInfo *pTextInfo = NULL;
    ST_RIL_SMS_DeliverParam *pDeliverTextInfo = NULL;
    char aPhNum[RIL_SMS_PHONE_NUMBER_MAX_LEN] = {0,};
    const char aReplyCon[] = {"Module has received SMS."};
    bool bResult = FALSE;
    
    pTextInfo = Ql_MEM_Alloc(sizeof(ST_RIL_SMS_TextInfo));
    if (NULL == pTextInfo)
    {
        APP_DEBUG("%s/%d:Ql_MEM_Alloc FAIL! size:%u\r\n", sizeof(ST_RIL_SMS_TextInfo), __func__, __LINE__);
        return;
    }
    Ql_memset(pTextInfo, 0x00, sizeof(ST_RIL_SMS_TextInfo));
    iResult = RIL_SMS_ReadSMS_Text(nIndex, LIB_SMS_CHARSET_GSM, pTextInfo);
    if (iResult != RIL_AT_SUCCESS)
    {
        Ql_MEM_Free(pTextInfo);
        APP_DEBUG("Fail to read text SMS[%d], cause:%d\r\n", nIndex, iResult);
        return;
    }        
    
    if ((LIB_SMS_PDU_TYPE_DELIVER != (pTextInfo->type)) || (RIL_SMS_STATUS_TYPE_INVALID == (pTextInfo->status)))
    {
        Ql_MEM_Free(pTextInfo);
        APP_DEBUG("WARNING: NOT a new received SMS.\r\n");    
        return;
    }
    
    pDeliverTextInfo = &((pTextInfo->param).deliverParam);    

    if(TRUE == pDeliverTextInfo->conPres)  //Receive CON-SMS segment
    {
        s8 iBufIdx = 0;
        u8 uSeg = 0;
        u16 uConLen = 0;

        iBufIdx = ConSMSBuf_GetIndex(g_asConSMSBuf,CON_SMS_BUF_MAX_CNT,&(pDeliverTextInfo->con));
        if(-1 == iBufIdx)
        {
            APP_DEBUG("Enter Hdlr_RecvNewSMS,WARNING! ConSMSBuf_GetIndex FAIL! Show this CON-SMS-SEG directly!\r\n");

            APP_DEBUG(
                "status:%u,type:%u,alpha:%u,sca:%s,oa:%s,scts:%s,data length:%u,cp:1,cy:%d,cr:%d,ct:%d,cs:%d\r\n",
                    (pTextInfo->status),
                    (pTextInfo->type),
                    (pDeliverTextInfo->alpha),
                    (pTextInfo->sca),
                    (pDeliverTextInfo->oa),
                    (pDeliverTextInfo->scts),
                    (pDeliverTextInfo->length),
                    pDeliverTextInfo->con.msgType,
                    pDeliverTextInfo->con.msgRef,
                    pDeliverTextInfo->con.msgTot,
                    pDeliverTextInfo->con.msgSeg
            );
            APP_DEBUG("data = %s\r\n",(pDeliverTextInfo->data));

            Ql_MEM_Free(pTextInfo);
        
            return;
        }

        bResult = ConSMSBuf_AddSeg(
                    g_asConSMSBuf,
                    CON_SMS_BUF_MAX_CNT,
                    iBufIdx,
                    &(pDeliverTextInfo->con),
                    (pDeliverTextInfo->data),
                    (pDeliverTextInfo->length)
        );
        if(FALSE == bResult)
        {
            APP_DEBUG("Enter Hdlr_RecvNewSMS,WARNING! ConSMSBuf_AddSeg FAIL! Show this CON-SMS-SEG directly!\r\n");

            APP_DEBUG(
                "status:%u,type:%u,alpha:%u,sca:%s,oa:%s,scts:%s,data length:%u,cp:1,cy:%d,cr:%d,ct:%d,cs:%d\r\n",
                (pTextInfo->status),
                (pTextInfo->type),
                (pDeliverTextInfo->alpha),
                (pTextInfo->sca),
                (pDeliverTextInfo->oa),
                (pDeliverTextInfo->scts),
                (pDeliverTextInfo->length),
                pDeliverTextInfo->con.msgType,
                pDeliverTextInfo->con.msgRef,
                pDeliverTextInfo->con.msgTot,
                pDeliverTextInfo->con.msgSeg
            );
            APP_DEBUG("data = %s\r\n",(pDeliverTextInfo->data));

            Ql_MEM_Free(pTextInfo);
        
            return;
        }

        bResult = ConSMSBuf_IsIntact(
                    g_asConSMSBuf,
                    CON_SMS_BUF_MAX_CNT,
                    iBufIdx,
                    &(pDeliverTextInfo->con)
        );
        if(FALSE == bResult)
        {
            APP_DEBUG(
                "Enter Hdlr_RecvNewSMS,WARNING! ConSMSBuf_IsIntact FAIL! Waiting. cp:1,cy:%d,cr:%d,ct:%d,cs:%d\r\n",
                pDeliverTextInfo->con.msgType,
                pDeliverTextInfo->con.msgRef,
                pDeliverTextInfo->con.msgTot,
                pDeliverTextInfo->con.msgSeg
            );

            Ql_MEM_Free(pTextInfo);

            return;
        }

        //Show the CON-SMS
        APP_DEBUG(
            "status:%u,type:%u,alpha:%u,sca:%s,oa:%s,scts:%s",
            (pTextInfo->status),
            (pTextInfo->type),
            (pDeliverTextInfo->alpha),
            (pTextInfo->sca),
            (pDeliverTextInfo->oa),
            (pDeliverTextInfo->scts)
        );
        
        uConLen = 0;
        for(uSeg = 1; uSeg <= pDeliverTextInfo->con.msgTot; uSeg++)
        {
            uConLen += g_asConSMSBuf[iBufIdx].asSeg[uSeg-1].uLen;
        }

        APP_DEBUG(",data length:%u",uConLen);
        APP_DEBUG("\r\n"); //Print CR LF

        for(uSeg = 1; uSeg <= pDeliverTextInfo->con.msgTot; uSeg++)
        {
            APP_DEBUG("data = %s ,len = %d",
                g_asConSMSBuf[iBufIdx].asSeg[uSeg-1].aData,
                g_asConSMSBuf[iBufIdx].asSeg[uSeg-1].uLen
            );
        }

        APP_DEBUG("\r\n"); //Print CR LF

        //Reset CON-SMS context
        bResult = ConSMSBuf_ResetCtx(g_asConSMSBuf,CON_SMS_BUF_MAX_CNT,iBufIdx);
        if(FALSE == bResult)
        {
            APP_DEBUG("Enter Hdlr_RecvNewSMS,WARNING! ConSMSBuf_ResetCtx FAIL! iBufIdx:%d\r\n",iBufIdx);
        }

        Ql_MEM_Free(pTextInfo);
        
        return;
    }
    
    APP_DEBUG("<-- RIL_SMS_ReadSMS_Text OK. eCharSet:LIB_SMS_CHARSET_GSM,nIndex:%u -->\r\n",nIndex);
    APP_DEBUG("status:%u,type:%u,alpha:%u,sca:%s,oa:%s,scts:%s,data length:%u\r\n",
        pTextInfo->status,
        pTextInfo->type,
        pDeliverTextInfo->alpha,
        pTextInfo->sca,
        pDeliverTextInfo->oa,
        pDeliverTextInfo->scts,
        pDeliverTextInfo->length);
    	APP_DEBUG("data = %s\r\n",(pDeliverTextInfo->data));
	
	//if(Ql_strcmp( pDeliverTextInfo->oa,SmsTextAlarmPhoNume)==0)
	//	{
	//	APP_DEBUG("<-- 授权电话号码正确... -->\r\n");
    		SmsDataCheck(pDeliverTextInfo->oa,pDeliverTextInfo->data,pDeliverTextInfo->length);
	//	}
	//else
	//	{
	//	APP_DEBUG("<-- 授权电话号码错误... -->\r\n");
	//	}
	
    Ql_strcpy(aPhNum, pDeliverTextInfo->oa);
    Ql_MEM_Free(pTextInfo);
    
    if (bAutoReply)
    {
        if (!Ql_strstr(aPhNum, "10086"))  // Not reply SMS from operator
        {
            APP_DEBUG("<-- Replying SMS... -->\r\n");
            iResult = RIL_SMS_SendSMS_Text(aPhNum, Ql_strlen(aPhNum),LIB_SMS_CHARSET_GSM,(u8*)aReplyCon,Ql_strlen(aReplyCon),&uMsgRef);
            if (iResult != RIL_AT_SUCCESS)
            {
                APP_DEBUG("RIL_SMS_SendSMS_Text FAIL! iResult:%u\r\n",iResult);
                return;
            }
            APP_DEBUG("<-- RIL_SMS_SendTextSMS OK. uMsgRef:%d -->\r\n", uMsgRef);
        }
    }
    return;
}

static void CallBack_UART_Hdlr(Enum_SerialPort port, Enum_UARTEventType msg, bool level, void* customizedPara)
{

}
static void InitSerialPort(void)
{
    s32 iResult = 0;

    //Register & Open UART port
    iResult = Ql_UART_Register(UART_PORT1, CallBack_UART_Hdlr, NULL);
    if (iResult != QL_RET_OK)
    {
        Ql_Debug_Trace("Fail to register UART port[%d]:%d\r\n",UART_PORT1);
    }
    
    iResult = Ql_UART_Open(UART_PORT1, 115200, FC_NONE);
    if (iResult != QL_RET_OK)
    {
        Ql_Debug_Trace("Fail to open UART port[%d], baud rate:115200, FC_NONE\r\n", UART_PORT1);
    }    
}

void InitSmsData(void)
{
	u8 i;
	for(i = 0; i < CON_SMS_BUF_MAX_CNT; i++)
	{
		ConSMSBuf_ResetCtx(g_asConSMSBuf,CON_SMS_BUF_MAX_CNT,i);
	}
	
}

/*
  * @brief  应用层调用CCID
  * @param  *p :指向ccid号码
  * @retval None
  */
void  Modem_Api_CCID(u8 *p)
{
    memcpy(p,Simccid,10);
}
/**
  * @brief  获取终端SIM卡ICCID号
  * @param  *Bcdbuff :指向被保存的缓存.
  * @retval None.
  */

/********************************************************************
* 名称 :  ModLib_StrByte
* 功能 : 可打印字符串转换为字节数据
* 输入 :  nSrcLength - 源字符串长度
                  pSrc - 源字符串指针
* 输出 :  pDst - 目标字符串指针
* 返回 : 目标字符串长度
* 全局变量:
* 调用函数:
* 中断资源:  (没有,不写)
* 备注: "C832" --> {0xC8, 0x32}
********************************************************************/
int ModLib_StrByte(const u8* pSrc, u8* pDst, int nSrcLength)
{
	int i;	
	if ((pSrc == NULL)||(pDst == NULL))
		return 0;

	for (i=0; i<nSrcLength; i += 2) {
        if ((*pSrc >= '0')&&(*pSrc <= '9'))// 输出高4位
            *pDst = (*pSrc - '0') << 4;
        else
            *pDst = (*pSrc - 'A' + 10) << 4;
		
        pSrc++;
        if ((*pSrc>='0') && (*pSrc<='9')) // 输出低4位
            *pDst |= *pSrc - '0';
        else
            *pDst |= *pSrc - 'A' + 10;
		
        pSrc++;
        pDst++;
    }

    return (nSrcLength / 2);
}

void MOD_GetIccidBcdValue(void)
{
    char  tmp[60]={'\0'};
	RIL_SIM_GetCCID(tmp);
    ModLib_StrByte(tmp,Simccid,Ql_strlen(tmp));
	APP_DEBUG("\r\n<-- ACCID :%s -->\r\n",tmp);
}


/*
  * @brief  查询modem类型
  * @param  None
  * @retval 
            bit0，0：不支持GPRS通信，     1：支持GPRS通信；
            bit1，0：不支持CDMA通信，     1：支持CDMA通信；
            bit2，0：不支持TD-SCDMA通信， 1：支持TD-SCDMA通信；
            bit3，0：不支持WCDMA通信，    1：支持WCDMA通信；
            bit4，0：不支持CDMA2000通信， 1：支持CDMA2000通信。
            bit5，0：不支持TD-LTE通信，   1：支持TD-LTE通信；
            bit7，0：不支持其他通信方式， 1：支持其他通信方式。  
  */
u8  Modem_Api_Type(void)
{
    return 1;// Modem_State_Type(1);
}

