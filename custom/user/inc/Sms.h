#ifndef _SMS_H_
#define _SMS_H_

#include "ril_sms.h"

 
 //中心号码删除成功!
#define DEL_ALARM_PHONE_SUCCESS  	"4e2d5fc353f77801522096646210529fff01\0"
//密码错误，已恢复成为初始密码！
#define SET_ALARM_PHONE_WRONG_PASSWORD 	"5bc6780195198befff0c5df26062590d62104e3a521d59cb5bc67801ff01\0"

 //授权电话设置成功，新的授权号码将启用！
#define SET_ALARM_PHONE_SUCCESS  "6388674375358bdd8bbe7f6e6210529fff0c65b076846388674353f778015c06542f7528ff01\0"
 //服务器设置成功！
#define SET_IP_PORT_SUCCESS "670d52a156688bbe7f6e6210529fff01\0"
 //短信设置定时上报成功
#define SET_REPORT_TIME_SUCCESS  "4e0a4f2095f4969465f695f48bbe7f6e6210529fff0195f4969465f695f44e3a\0"
 //邋ID号设置成功！
#define SET_DEVIECE_ID_SUCCESS "0049004453f78bbe7f6e6210529fff01\0"
 //指令接收成功即将重启
#define SET_REST_SUCCESS "63074ee463a565366210529fff0c53735c0691cd542fff01\0"
 //恢复油电成功！
#define SET_OPEN_OIL_SUCCESS  "6062590d6cb98def6210529fff01\0"
 //断油电成功！
#define SET_OFF_OIL_SUCCESS   "65ad6cb98def6210529fff01\0"
 //当前不允许断油电
#define ILLAGAL_SET_OIL  "5f53524d4e0d51418bb865ad6cb97535\0"
//指令不支持或者错误，请仔细核对！
#define ILLAGAL_ORDER "63074ee44e0d652f63016216800595198befff0c8bf74ed47ec668385bf9ff01\0"


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
  *  TRUE:	This function works SUCCESS.
  *  FALSE: This function works FAIL!
  *****************************************************************************/
 bool SMS_Initialize(void);
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
void Hdlr_RecvNewSMS(u32 nIndex, bool bAutoReply);

void InitSmsData(void);
void UpSmsAlarmPhoNum(void);

void SMS_TextMode_Send(LIB_SMS_CharSetEnum SmsType,char *strPhNum,char *strTestMsg,u8 MsgLen);
extern u8 SmsOilFlag;
extern char SmsTextAlarmPhoNume[20];

/*
  * @brief  应用层调用CCID
  * @param  *p :指向ccid号码
  * @retval None
  */
void  Modem_Api_CCID(u8 *p);
/**
  * @brief  获取终端SIM卡ICCID号
  * @param 
  * @retval None.
  */
void MOD_GetIccidBcdValue(void);
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
u8  Modem_Api_Type(void);


#endif
