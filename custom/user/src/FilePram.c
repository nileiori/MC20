#include "include.h"


//********************************ºê¶¨Òå************************************

#define EepromPramID_SIZE (sizeof(EepromPramAddress)/sizeof(EepromPramAddress[0]))


//********************************È«¾Ö±äÁ¿**********************************
/////////////////////
const unsigned long c_usEepromPramID[] = 
{
    E2_FIRMWARE_VERSION_ID,             //°æ±¾ºÅ,ÌØÊâ´¦Àí,²»ÓÃÐ´,
    E2_TERMINAL_HEARTBEAT_ID,           //01²ÎÊýID0x0001£¬ÖÕ¶ËÐÄÌø·¢ËÍ¼ä¸ô£¬µ¥Î»Ãë£¬Ä¬ÈÏÖµ60
    E2_MAIN_SERVER_APN_ID,              //02²ÎÊýID0x0010£¬Ö÷·þÎñÆ÷APN
    E2_MAIN_SERVER_DIAL_NAME_ID,        //03²ÎÊýID0x0011£¬Ö÷·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÓÃ»§Ãû
    E2_MAIN_SERVER_DIAL_PASSWORD_ID,    //04²ÎÊýID0x0012£¬Ö÷·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÃÜÂë
    E2_MAIN_SERVER_IP_ID,               //05²ÎÊýID0x0013£¬Ö÷·þÎñÆ÷µØÖ·£¬IP»òÓòÃû
    E2_BACKUP_SERVER_APN_ID,			//06²ÎÊýID0x0014,±¸·Ý·þÎñÆ÷APN//ÎÞÏßÍ¨ÐÅ²¦ºÅ·ÃÎÊµã
    E2_BACKUP_SERVER_DIAL_NAME_ID, 		//07²ÎÊýID0x0015,±¸·Ý·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÓÃ»§Ãû
 	E2_BACKUP_SERVER_DIAL_PASSWORD_ID, 	//08²ÎÊýID0x0016,±¸·Ý·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÃÜÂë
	E2_BACKUP_SERVER_IP_ID,				//09²ÎÊýID0x0017,±¸·Ý·þÎñÆ÷µØÖ·//IP»òÓòÃû
    E2_MAIN_SERVER_TCP_PORT_ID,         //10²ÎÊýID0x0018£¬Ö÷·þÎñÆ÷TCP¶Ë¿Ú
    E2_SLEEP_REPORT_TIME_ID,            //11²ÎÊýID0x0027£¬ÐÝÃßÊ±»ã±¨Ê±¼ä¼ä¸ô£¬µ¥Î»ÎªÃë£¬Ä¬ÈÏÖµÎª60
    E2_EMERGENCY_REPORT_TIME_ID,		//12²ÎÊýID0x0028,½ô¼±±¨¾¯Ê±»ã±¨Ê±¼ä¼ä¸ô//µ¥Î»ÎªÃë//Ä¬ÈÏÖµÎª10
    E2_ACCON_REPORT_TIME_ID,            //13²ÎÊýID0x0029,È±Ê¡Ê±¼ä»ã±¨¼ä¸ô,µ¥Î»ÎªÃë£¬Ä¬ÈÏÖµÎª10 
	E2_CORNER_REPORT_ID, 				//14²ÎÊýID0x0030,¹Õµã²¹´«½Ç¶È,µ¥Î»¶È/Ãë//Ä¬ÈÏÖµÎª30
	E2_MONITOR_SERVER_SMS_PHONE_ID,		//0x0043//¼à¿ØÆ½Ì¨SMSµç»°ºÅÂë
	E2_SMS_TEXT_ALARM_PHONE_ID,			//	0x0044//SMSÎÄ±¾±¨¾¯µç»°ºÅÂë
	E2_ALARM_MASK_ID, 					//15²ÎÊýID0x0050,±¨¾¯ÆÁ±Î×Ö//bit9,11,12Ä¬ÈÏÆÁ±Î,LCDÆÁ¹ÊÕÏ,TTS¹ÊÕÏ,IC¿¨Ä£¿é¹ÊÕÏÄ¬ÈÏÎªÆÁ±Î
	E2_MAX_SPEED_ID, 					//16²ÎÊýID0x0055,×î¸ßËÙ¶È,µ¥Î»km/h,DWORD,Ä¬ÈÏÖµ130,¾ÅÍ¨ÐèÇóÎª100
	E2_OVER_SPEED_KEEP_TIME_ID, 		//17²ÎÊýID0x0056,³¬ËÙ³ÖÐøÊ±¼ä,µ¥Î»ÎªÃë,DWORD,Ä¬ÈÏÎª10Ãë,¾ÅÍ¨ÐèÇóÎªÁ¢ÂíÉÏ±¨
	E2_SET_CRASH_ALARM_ID,				//18²ÎÊýID0x005D,Åö×²±¨¾¯²ÎÊýÉèÖÃ,Åö×²¼ÓËÙ¶ÈÄ¬ÈÏÉèÖÃÎª4g
	E2_SET_ROLLOVER_ALARM_ID,	        //19²ÎÊýID0x005E,²à·­±¨¾¯²ÎÊýÉèÖÃ£º²à·­½Ç¶È£¬µ¥Î»1 ¶È£¬Ä¬ÈÏÎª45 ¶È
	E2_CAR_TOTAL_MILE_ID,				//20²ÎÊýID0x0080£¬³µÁ¾Àï³Ì±í¶ÁÊý£¬0.1km
	E2_CAR_PROVINCE_ID, 				//21²ÎÊýID0x0081,³µÁ¾ËùÔÚµÄÊ¡ÓòID
	E2_CAR_CITY_ID, 					//22²ÎÊýID0x0082,³µÁ¾ËùÔÚµÄÊÐÓòID
	E2_CAR_PLATE_NUM_ID, 				//23²ÎÊýID0x0083,¹«°²½»Í¨¹ÜÀí²¿ÃÅ°ä·¢µÄ»ú¶¯³µºÅÅÆ
	E2_CAR_PLATE_COLOR_ID, 				//24²ÎÊýID0x0084,³µÅÆÑÕÉ«//°´ÕÕJT/T415-2006µÄ5.4.12,1:À¶É«,2:»ÆÉ«,3:ºÚÉ«,4:°×É«,9:ÆäËü
    E2_GPS_SET_MODE_ID,                 //25²ÎÊýID0x0090 GNSS ¶¨Î»Ä£Ê½£¬Ä¬ÈÏÎª±±¶·¶¨Î»
	E2_LOGINGPRS_PASSWORD_ID,			//26²ÎÊýID0xF200+0x00£¬×Ô¶¨Òå£¬¼øÈ¨Âë
	E2_MANUFACTURE_ID,					//27²ÎÊýID0xF205£¬×Ô¶¨Òå£¬ÖÆÔìÉÌID£¬5×Ö½Ú	
    E2_DEVICE_ID,                       //28²ÎÊýID0xF200+0x06£¬×Ô¶¨Òå£¬³§ÉÌÖÕ¶ËID£¬7×Ö½Ú
    E2_DEVICE_PHONE_ID,                 //29²ÎÊýID0xF200+0x07£¬×Ô¶¨Òå£¬ÖÕ¶ËÊÖ»úºÅ£¬6×Ö½Ú
    E2_CAR_VIN_ID,						//30²ÎÊýID0xF208£¬×Ô¶¨Òå£¬³µÁ¾Ê¶±ðÂë(VIN)£¬17×Ö½Ú£¬ASCIIÂë
    E2_DEFINE_ALARM_MASK_WORD_ID,		//31²ÎÊýID0xF243£¬×Ô¶¨Òå£¬×Ô¶¨Òå±¨¾¯ÆÁ±Î×Ö£¬DWORD
    E2_DEFINE_HARD_VER_ID,				//32²ÎÊýID0x0001F0004£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓ²¼þ°æ±¾ºÅ£¬DWORD
    E2_SMS_ALARM_PHONE_NUM_ID,			//²ÎÊýID 0x0001F005£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓÃ»§¶ÌÐÅ±¨¾¯µç»°£¬DWORD
	E2_SMS_OPERATE_PWD_ID,				//²ÎÊýID 0x0001F006£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓÃ»§¶ÌÐÅ²Ù×÷ÃÜÂë£¬DWORD
	E2_LOW_VOL_THRESHOLD_ID,			//²ÎÊýID 0x0001F00F£¬×Ô¶¨Òå£¬×Ô¶¨ÒåµÍµçÑ¹ãÐÖµ£¬WORD
};
////////////////////////
const u8 EepromPramLength[] = 
{
    0,
    E2_TERMINAL_HEARTBEAT_LEN,          //01²ÎÊýID0x0001£¬ÖÕ¶ËÐÄÌø·¢ËÍ¼ä¸ô£¬µ¥Î»Ãë£¬Ä¬ÈÏÖµ60
    E2_MAIN_SERVER_APN_LEN,             //02²ÎÊýID0x0010£¬Ö÷·þÎñÆ÷APN
    E2_MAIN_SERVER_DIAL_NAME_LEN,       //03²ÎÊýID0x0011£¬Ö÷·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÓÃ»§Ãû
    E2_MAIN_SERVER_DIAL_PASSWORD_LEN,   //04²ÎÊýID0x0012£¬Ö÷·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÃÜÂë
    E2_MAIN_SERVER_IP_LEN,              //05²ÎÊýID0x0013£¬Ö÷·þÎñÆ÷µØÖ·£¬IP»òÓòÃû
	E2_BACKUP_SERVER_APN_LEN,			//06²ÎÊýID0x0014,±¸·Ý·þÎñÆ÷APN//ÎÞÏßÍ¨ÐÅ²¦ºÅ·ÃÎÊµã
	E2_BACKUP_SERVER_DIAL_NAME_LEN,		//07²ÎÊýID0x0015,±¸·Ý·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÓÃ»§Ãû
	E2_BACKUP_SERVER_DIAL_PASSWORD_LEN,	//08²ÎÊýID0x0016,±¸·Ý·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÃÜÂë
	E2_BACKUP_SERVER_IP_LEN, 			//09²ÎÊýID0x0017,±¸·Ý·þÎñÆ÷µØÖ·//IP»òÓòÃû
    E2_MAIN_SERVER_TCP_PORT_LEN,        //10²ÎÊýID0x0018£¬Ö÷·þÎñÆ÷TCP¶Ë¿Ú
    E2_SLEEP_REPORT_TIME_LEN,           //11²ÎÊýID0x0027£¬ÐÝÃßÊ±»ã±¨Ê±¼ä¼ä¸ô£¬µ¥Î»ÎªÃë£¬Ä¬ÈÏÖµÎª60
	E2_EMERGENCY_REPORT_TIME_LEN,		//12²ÎÊýID0x0028,½ô¼±±¨¾¯Ê±»ã±¨Ê±¼ä¼ä¸ô//µ¥Î»ÎªÃë//Ä¬ÈÏÖµÎª10
    E2_ACCON_REPORT_TIME_LEN,           //13²ÎÊýID0x0029È±Ê¡Ê±¼ä»ã±¨¼ä¸ô,µ¥Î»ÎªÃë£¬Ä¬ÈÏÖµÎª10
	E2_CORNER_REPORT_LEN, 				//14²ÎÊýID0x0030,¹Õµã²¹´«½Ç¶È,µ¥Î»¶È/Ãë//Ä¬ÈÏÖµÎª30
	E2_MONITOR_SERVER_SMS_PHONE_LEN,    //²ÎÊýID0x0043£¬¼à¿ØÆ½Ì¨SMSµç»°ºÅÂë
	E2_SMS_TEXT_ALARM_PHONE_LEN,		//0x0044//SMSÎÄ±¾±¨¾¯µç»°ºÅÂë
	E2_ALARM_MASK_LEN,					//15²ÎÊýID0x0050,±¨¾¯ÆÁ±Î×Ö//bit9,11,12Ä¬ÈÏÆÁ±Î,LCDÆÁ¹ÊÕÏ,TTS¹ÊÕÏ,IC¿¨Ä£¿é¹ÊÕÏÄ¬ÈÏÎªÆÁ±Î
	E2_MAX_SPEED_LEN, 					//16²ÎÊýID0x0055,×î¸ßËÙ¶È,µ¥Î»km/h,DWORD,Ä¬ÈÏÖµ130,¾ÅÍ¨ÐèÇóÎª100
	E2_OVER_SPEED_KEEP_TIME_LEN,		//17²ÎÊýID0x0056,³¬ËÙ³ÖÐøÊ±¼ä,µ¥Î»ÎªÃë,DWORD,Ä¬ÈÏÎª10Ãë,¾ÅÍ¨ÐèÇóÎªÁ¢ÂíÉÏ±¨
	E2_SET_CRASH_ALARM_LEN,				//18²ÎÊýID0x005D,Åö×²±¨¾¯²ÎÊýÉèÖÃ,Åö×²¼ÓËÙ¶ÈÄ¬ÈÏÉèÖÃÎª4g
	E2_SET_ROLLOVER_ALARM_LEN,			//19²ÎÊýID0x005E,²à·­±¨¾¯²ÎÊýÉèÖÃ£º²à·­½Ç¶È£¬µ¥Î»1 ¶È£¬Ä¬ÈÏÎª45 ¶È
	E2_CAR_TOTAL_MILE_LEN,				//20²ÎÊýID0x0080£¬³µÁ¾Àï³Ì±í¶ÁÊý£¬0.1km
	E2_CAR_PROVINCE_LEN,				//21²ÎÊýID0x0081,³µÁ¾ËùÔÚµÄÊ¡ÓòID
	E2_CAR_CITY_LEN,					//22²ÎÊýID0x0082,³µÁ¾ËùÔÚµÄÊÐÓòID
	E2_CAR_PLATE_NUM_LEN, 				//23²ÎÊýID0x0083,¹«°²½»Í¨¹ÜÀí²¿ÃÅ°ä·¢µÄ»ú¶¯³µºÅÅÆ
	E2_CAR_PLATE_COLOR_LEN,				//24²ÎÊýID0x0084,³µÅÆÑÕÉ«//°´ÕÕJT/T415-2006µÄ5.4.12,1:À¶É«,2:»ÆÉ«,3:ºÚÉ«,4:°×É«,9:ÆäËü
    E2_GPS_SET_MODE_LEN,                //25²ÎÊýID0x0090 GNSS ¶¨Î»Ä£Ê½£¬Ä¬ÈÏÎª±±¶·¶¨Î»   
	E2_LOGINGPRS_PASSWORD_ID_LEN,		//26²ÎÊýID0xF200+0x00£¬×Ô¶¨Òå£¬¼øÈ¨Âë
	E2_MANUFACTURE_ID_LEN,				//27²ÎÊýID0xF205£¬×Ô¶¨Òå£¬ÖÆÔìÉÌID£¬5×Ö½Ú	
    E2_DEVICE_ID_LEN,                   //28²ÎÊýID0xF200+0x06£¬×Ô¶¨Òå£¬³§ÉÌÖÕ¶ËID£¬7×Ö½Ú
    E2_DEVICE_PHONE_ID_LEN,             //29²ÎÊýID0xF200+0x07£¬×Ô¶¨Òå£¬ÖÕ¶ËÊÖ»úºÅ£¬6×Ö½Ú
    E2_CAR_VIN_ID_LEN,					//30²ÎÊýID0xF208£¬×Ô¶¨Òå£¬³µÁ¾Ê¶±ðÂë(VIN)£¬17×Ö½Ú£¬ASCIIÂë
    E2_DEFINE_ALARM_MASK_WORD_ID_LEN,	//31²ÎÊýID0xF243£¬×Ô¶¨Òå£¬×Ô¶¨Òå±¨¾¯ÆÁ±Î×Ö£¬DWORD
    E2_DEFINE_HARD_VER_ID_LEN,			//32²ÎÊýID0x0001F0004£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓ²¼þ°æ±¾ºÅ£¬DWORD
	E2_SMS_ALARM_PHONE_NUM_ID_LEN,		//²ÎÊýID 0x0001F005£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓÃ»§¶ÌÐÅ±¨¾¯µç»°£
	E2_SMS_OPERATE_PWD_ID_LEN,			//²ÎÊýID 0x0001F006£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓÃ»§¶ÌÐÅ²Ù×÷ÃÜÂë
	E2_LOW_VOL_THRESHOLD_ID_LEN,		//²ÎÊýID 0x0001F00F£¬×Ô¶¨Òå£¬×Ô¶¨ÒåµÍµçÑ¹ãÐÖµ£¬WORD
};
//////////Ö»ÄÜÎª0»ò1////////////
const u8 EepromPramSwitch[] = 
{
    0,
    0,		//01²ÎÊýID0x0001£¬ÖÕ¶ËÐÄÌø·¢ËÍ¼ä¸ô£¬µ¥Î»Ãë£¬Ä¬ÈÏÖµ60
    1,		//02²ÎÊýID0x0010£¬Ö÷·þÎñÆ÷APN
    1,      //03²ÎÊýID0x0011£¬Ö÷·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÓÃ»§Ãû
    1,      //04²ÎÊýID0x0012£¬Ö÷·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÃÜÂë
    1,		//05²ÎÊýID0x0013£¬Ö÷·þÎñÆ÷µØÖ·£¬IP»òÓòÃû
	1,		//06²ÎÊýID0x0014£¬±¸·Ý·þÎñÆ÷APN£¬ÎÞÏßÍ¨ÐÅ²¦ºÅ·ÃÎÊµã
	1,		//07²ÎÊýID0x0015£¬±¸·Ý·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÓÃ»§Ãû
	1,		//08²ÎÊýID0x0016£¬±¸·Ý·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÃÜÂë
	1,		//09²ÎÊýID0x0017£¬±¸·Ý·þÎñÆ÷µØÖ·£¬IP»òÓòÃû
    0,		//10²ÎÊýID0x0018£¬Ö÷·þÎñÆ÷TCP¶Ë¿Ú
    0,		//11²ÎÊýID0x0027£¬ÐÝÃßÊ±»ã±¨Ê±¼ä¼ä¸ô£¬µ¥Î»ÎªÃë£¬Ä¬ÈÏÖµÎª60
    0,		//12²ÎÊýID0x0028,½ô¼±±¨¾¯Ê±»ã±¨Ê±¼ä¼ä¸ô//µ¥Î»ÎªÃë//Ä¬ÈÏÖµÎª10
    0,		//13²ÎÊýID0x0029£¬È±Ê¡Ê±¼ä»ã±¨¼ä¸ô,µ¥Î»ÎªÃë£¬Ä¬ÈÏÖµÎª10  
	0,		//14²ÎÊýID0x0030£¬¹Õµã²¹´«½Ç¶È,µ¥Î»¶È/Ãë£¬Ä¬ÈÏÖµÎª15
	1,		//0x0043//¼à¿ØÆ½Ì¨SMSµç»°ºÅÂë
	1,		//0x0044//SMSÎÄ±¾±¨¾¯µç»°ºÅÂë
	0,		//15²ÎÊýID0x0050£¬±¨¾¯ÆÁ±Î×Ö£¬Ä¬ÈÏÎªÈ«¿ª
	0,		//16²ÎÊýID0x0055£¬×î¸ßËÙ¶È
	0,		//17²ÎÊýID0x0056£¬³¬ËÙ³ÖÐøÊ±¼ä£¬µ¥Î»ÎªÃë£¬Ä¬ÈÏÎª10Ãë
    0,		//18²ÎÊýID0x005D,Åö×²±¨¾¯²ÎÊýÉèÖÃ 
    0,		//19²ÎÊýID0x005E,²à·­±¨¾¯²ÎÊýÉèÖÃ£º²à·­½Ç¶È£¬µ¥Î»1 ¶È£¬Ä¬ÈÏÎª30 ¶È
	0,		//20²ÎÊýID0x0080£¬³µÁ¾Àï³Ì±í¶ÁÊý£¬0.1km
	0,		//21²ÎÊýID0x0081£¬³µÁ¾ËùÔÚµÄÊ¡ÓòID
	0,		//22²ÎÊýID0x0082£¬³µÁ¾ËùÔÚµÄÊÐÓòID
	1,		//23²ÎÊýID0x0083£¬¹«°²½»Í¨¹ÜÀí²¿ÃÅ°ä·¢µÄ»ú¶¯³µºÅÅÆ
	0,		//24²ÎÊýID0x0084£¬³µÅÆÑÕÉ«£¬°´ÕÕJT/T415-2006µÄ5.4.12
    0,      //25²ÎÊýID0x0090 GNSS ¶¨Î»Ä£Ê½£¬Ä¬ÈÏÎª±±¶·¶¨Î»
	1,		//26²ÎÊýID0xF200+0x00£¬×Ô¶¨Òå£¬¼øÈ¨Âë
	1,		//27²ÎÊýID0xF200+0x05£¬×Ô¶¨Òå£¬ÖÆÔìÉÌID£¬5×Ö½Ú
	1,		//28²ÎÊýID0xF200+0x06£¬×Ô¶¨Òå£¬³§ÉÌÖÕ¶ËID£¬7×Ö½Ú
	1,		//29²ÎÊýID0xF200+0x07£¬×Ô¶¨Òå£¬ÖÕ¶ËÊÖ»úºÅ£¬6×Ö½Ú
	1,		//30²ÎÊýID0xF200+0x08£¬×Ô¶¨Òå£¬³µÁ¾Ê¶±ðÂë
	0,		//31²ÎÊýID0xF200+0x43£¬×Ô¶¨Òå£¬×Ô¶¨Òå±¨¾¯ÆÁ±Î×Ö£¬DWORD
	1,		//32²ÎÊýID0x0001F0004£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓ²¼þ°æ±¾ºÅ£¬DWORD
	1,		//²ÎÊýID 0x0001F005£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓÃ»§¶ÌÐÅ±¨¾¯µç»°£
	1,		//²ÎÊýID 0x0001F006£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓÃ»§¶ÌÐÅ²Ù×÷ÃÜÂë
	0,		//²ÎÊýID 0x0001F00F£¬×Ô¶¨Òå£¬×Ô¶¨ÒåµÍµçÑ¹ãÐÖµ£¬WORD
};
const u16 EepromPramAddress[] = 
{
    0,
    E2_TERMINAL_HEARTBEAT_ADDR,             //01²ÎÊýID0x0001£¬ÖÕ¶ËÐÄÌø·¢ËÍ¼ä¸ô£¬µ¥Î»Ãë£¬Ä¬ÈÏÖµ60
    E2_MAIN_SERVER_APN_ADDR,                //02²ÎÊýID0x0010£¬Ö÷·þÎñÆ÷APN  
    E2_MAIN_SERVER_DIAL_NAME_ADDR,          //03²ÎÊýID0x0011£¬Ö÷·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÓÃ»§Ãû
    E2_MAIN_SERVER_DIAL_PASSWORD_ADDR,      //04²ÎÊýID0x0012£¬Ö÷·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÃÜÂë
    E2_MAIN_SERVER_IP_ADDR,                 //05²ÎÊýID0x0013£¬Ö÷·þÎñÆ÷µØÖ·£¬IP»òÓòÃû  
	E2_BACKUP_SERVER_APN_ADDR,				//06²ÎÊýID0x0014£¬±¸·Ý·þÎñÆ÷APN£¬ÎÞÏßÍ¨ÐÅ²¦ºÅ·ÃÎÊµã
	E2_BACKUP_SERVER_DIAL_NAME_ADDR,		//07²ÎÊýID0x0015£¬±¸·Ý·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÓÃ»§Ãû
	E2_BACKUP_SERVER_DIAL_PASSWORD_ADDR,	//08²ÎÊýID0x0016£¬±¸·Ý·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÃÜÂë
	E2_BACKUP_SERVER_IP_ADDR,				//09²ÎÊýID0x0017£¬±¸·Ý·þÎñÆ÷µØÖ·£¬IP»òÓòÃû
    E2_MAIN_SERVER_TCP_PORT_ADDR,           //10²ÎÊýID0x0018£¬Ö÷·þÎñÆ÷TCP¶Ë¿Ú 
    E2_SLEEP_REPORT_TIME_ADDR,              //11²ÎÊýID0x0027£¬ÐÝÃßÊ±»ã±¨Ê±¼ä¼ä¸ô£¬µ¥Î»ÎªÃë£¬Ä¬ÈÏÖµÎª60    
    E2_EMERGENCY_REPORT_TIME_ADR,			//12²ÎÊýID0x0028,½ô¼±±¨¾¯Ê±»ã±¨Ê±¼ä¼ä¸ô//µ¥Î»ÎªÃë//Ä¬ÈÏÖµÎª10
    E2_ACCON_REPORT_TIME_ADDR,              //13²ÎÊýID0x0029 È±Ê¡Ê±¼ä»ã±¨¼ä¸ô,µ¥Î»ÎªÃë£¬Ä¬ÈÏÖµÎª10  
    E2_CORNER_REPORT_ADDR,					//14²ÎÊýID0x0030£¬¹Õµã²¹´«½Ç¶È,µ¥Î»¶È/Ãë£¬Ä¬ÈÏÖµÎª15
    E2_MONITOR_SERVER_SMS_PHONE_ADDR,       //²ÎÊýID0x0043£¬¼à¿ØÆ½Ì¨SMSµç»°ºÅÂë
    E2_SMS_TEXT_ALARM_PHONE_ADDR,			//0x0044//SMSÎÄ±¾±¨¾¯µç»°ºÅÂë
	E2_ALARM_MASK_ADDR,						//15²ÎÊýID0x0050£¬±¨¾¯ÆÁ±Î×Ö£¬Ä¬ÈÏÎªÈ«¿ª
	E2_MAX_SPEED_ADDR,						//16²ÎÊýID0x0055£¬×î¸ßËÙ¶È
	E2_OVER_SPEED_KEEP_TIME_ADDR,			//17²ÎÊýID0x0056£¬³¬ËÙ³ÖÐøÊ±¼ä£¬µ¥Î»ÎªÃë£¬Ä¬ÈÏÎª10Ãë
	E2_SET_CRASH_ALARM_ADDR,				//18²ÎÊýID0x005D//Åö×²±¨¾¯²ÎÊýÉèÖÃ 
	E2_SET_ROLLOVER_ALARM_ADDR,				//19²ÎÊýID0x005E//²à·­±¨¾¯²ÎÊýÉèÖÃ£º²à·­½Ç¶È£¬µ¥Î»1 ¶È£¬Ä¬ÈÏÎª30 ¶È
	E2_CAR_TOTAL_MILE_ADDR,					//20²ÎÊýID0x0080£¬³µÁ¾Àï³Ì±í¶ÁÊý£¬0.1km	
	E2_CAR_PROVINCE_ID_ADDR,				//21²ÎÊýID0x0081£¬³µÁ¾ËùÔÚµÄÊ¡ÓòID
	E2_CAR_CITY_ID_ADDR,					//22²ÎÊýID0x0082£¬³µÁ¾ËùÔÚµÄÊÐÓòID
	E2_CAR_PLATE_NUM_ADDR,					//23²ÎÊýID0x0083£¬¹«°²½»Í¨¹ÜÀí²¿ÃÅ°ä·¢µÄ»ú¶¯³µºÅÅÆ
	E2_CAR_PLATE_COLOR_ADDR,				//24²ÎÊýID0x0084£¬³µÅÆÑÕÉ«£¬°´ÕÕJT/T415-2006µÄ5.4.12
    E2_GPS_SET_MODE_ADDR,                   //25²ÎÊýID0x0090,GNSS ¶¨Î»Ä£Ê½£¬Ä¬ÈÏÎª±±¶·¶¨Î»    
   	E2_LOGINGPRS_PASSWORD_ID_ADDR,			//26²ÎÊýID0xF200+0x00£¬×Ô¶¨Òå£¬¼øÈ¨Âë
	E2_MANUFACTURE_ID_ADDR,					//27²ÎÊýID0xF200+0x05£¬×Ô¶¨Òå£¬ÖÆÔìÉÌID£¬5×Ö½Ú
	E2_DEVICE_ID_ADDR,						//28²ÎÊýID0xF200+0x06£¬×Ô¶¨Òå£¬³§ÉÌÖÕ¶ËID£¬7×Ö½Ú
	E2_DEVICE_PHONE_ID_ADDR,				//29²ÎÊýID0xF200+0x07£¬×Ô¶¨Òå£¬ÖÕ¶ËÊÖ»úºÅ£¬6×Ö½Ú
	E2_CAR_VIN_ID_ADDR,						//30²ÎÊýID0xF200+0x08£¬×Ô¶¨Òå£¬³µÁ¾Ê¶±ðÂë
	E2_DEFINE_ALARM_MASK_WORD_ID_ADDR,		//31²ÎÊýID0xF200+0x43£¬×Ô¶¨Òå£¬×Ô¶¨Òå±¨¾¯ÆÁ±Î×Ö£¬DWORD
	E2_DEFINE_HARD_VER_ID_ADDR,				//32²ÎÊýID0x0001F0004£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓ²¼þ°æ±¾ºÅ£¬DWORD
	E2_SMS_ALARM_PHONE_NUM_ID_ADDR, 		//²ÎÊýID 0x0001F005£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓÃ»§¶ÌÐÅ±¨¾¯µç»°
	E2_SMS_OPERATE_PWD_ID_ADDR,     		//²ÎÊýID 0x0001F006£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓÃ»§¶ÌÐÅ²Ù×÷ÃÜÂë 
	E2_LOW_VOL_THRESHOLD_ID_ADDR,			//²ÎÊýID 0x0001F00F£¬×Ô¶¨Òå£¬×Ô¶¨ÒåµÍµçÑ¹ãÐÖµ£¬WORD
};

/*************************************************************
** º¯ÊýÃû³Æ: Eepromflash_ReadBytes
** ¹¦ÄÜÃèÊö: ¶Á×Ö½Ú²Ù×÷
** Èë¿Ú²ÎÊý: Address¶ÁµÄÆðÊ¼µØÖ·,*data ¶Á³öÊý¾Ý´æ·ÅµØÖ·Ö¸Õë,len ¶ÁµÄ×Ö½ÚÊý
** ³ö¿Ú²ÎÊý: 
** ·µ»Ø²ÎÊý: 
** È«¾Ö±äÁ¿: 
** µ÷ÓÃÄ£¿é: 
*************************************************************/	
u8 E2prom_ReadBytes(char* lpFileName,unsigned long Address, unsigned char *data, unsigned short len)
{
	s32 ret=0;
	s32 handle=-1;
	u32 hasreadenLen = 0;//ÒÑ¾­Ð´ÈëÎÄ¼þµÄ×Ö½ÚÊý£¬³õÊ¼ÖµÎªÁã

	handle = Ql_FS_Open(lpFileName,QL_FS_READ_ONLY);
	 if(handle<0)
	 {
		 APP_DEBUG("\r\n<--The %s file does not exist,handle:%d-->\r\n",lpFileName,handle);
		 handle = Ql_FS_Open(lpFileName,QL_FS_READ_ONLY);
		 if(handle<0)
		  {
			  APP_DEBUG("\r\n<--The %s file does not exist,handle:%d-->\r\n",lpFileName,handle);
			  return 0;
		  }
	 }
	 ret = Ql_FS_Seek(handle,Address,QL_FS_FILE_BEGIN); 
	 ret = Ql_FS_Read(handle, data, len, &hasreadenLen);
	 /*
	 APP_DEBUG("\r\n<--Eepromflash_ReadBytes  ret =%d  hasreadenLen =%d-->\r\n",ret,hasreadenLen);
	 if(len>5)
	 {
		APP_DEBUG("\r\n<--E2prom_ReadBytes len=%d Address=%d	Data=%s-->\r\n",len,Address,data);
 	}
 	else
 	{
 		APP_DEBUG("\r\n<--E2prom_ReadBytes len=%d Address=%d Data=%d",len,Address,data[0]);
 		u8 i;
 		for(i=1;i<len;i++)
 			{
 			APP_DEBUG("%d",data[i]);
 			}
 		APP_DEBUG("-->\r\n");
 	 }
 	 */
 	 Ql_FS_Close(handle);//close the file
	if(ret==QL_RET_OK)
	{
		return len;
	}
	else
	{
		return 0;
	}
}
/*************************************************************
** º¯ÊýÃû³Æ: Eepromflash_WriteBytes
** ¹¦ÄÜÃèÊö: Ð´×Ö½Ú²Ù×÷
** Èë¿Ú²ÎÊý: AddressÐ´µÄÆðÊ¼µØÖ·,*data Ð´ÈëÊý¾Ý´æ·ÅµØÖ·Ö¸Õë,len Ð´µÄ×Ö½ÚÊý
** ³ö¿Ú²ÎÊý: 
** ·µ»Ø²ÎÊý: 1:³É¹¦£¬0:Ê§°Ü
** È«¾Ö±äÁ¿: 
** µ÷ÓÃÄ£¿é: 
*************************************************************/
unsigned char E2prom_WriteBytes(char* lpFileName,unsigned long Address, unsigned char *data,unsigned short len)
{
s32 ret=0;
s32 handle=-1;
u32 haswritenLen = 0;//ÒÑ¾­Ð´ÈëÎÄ¼þµÄ×Ö½ÚÊý£¬³õÊ¼ÖµÎªÁã

handle = Ql_FS_Open(lpFileName,QL_FS_READ_WRITE);
if(handle<0)
{
   APP_DEBUG("\r\n<--The %s file does not exist,handle:%d-->\r\n",lpFileName,handle);
   handle = Ql_FS_Open(lpFileName,QL_FS_READ_WRITE);
	if(handle<0)
	{
	   APP_DEBUG("\r\n<--The %s file does not exist,handle:%d-->\r\n",lpFileName,handle);
	   return 0;
	}
}
ret = Ql_FS_Seek(handle,Address,QL_FS_FILE_BEGIN);	//seek end 
ret = Ql_FS_Write(handle, data, len, &haswritenLen);
//APP_DEBUG("\r\n<--!! Function (E2prom_WriteBytes)  ret =%d  haswritenLen =%d-->\r\n",ret,haswritenLen);
//APP_DEBUG("%s",data);
/*
if(len>5)
{
APP_DEBUG("\r\n<--E2prom_WriteBytes len=%d Address=%d	Data=%s-->\r\n",len,Address,data);
}
else
{
	APP_DEBUG("\r\n<--E2prom_WriteBytes len=%d Address=%d Data=%d",len,Address,data[0]);
	u8 i;
	for(i=1;i<len;i++)
		{
		APP_DEBUG("-%d",data[i]);
		}
	APP_DEBUG("-->\r\n");
}
*/
Ql_FS_Flush(handle); //fflushÇ¿ÖÆ»º´æÐ´ÈëÎÄ¼þ
Ql_FS_Close(handle);//close the file
return haswritenLen;
}

u8 FRAM_BufferWrite(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short  NumBytesToWrite) 
{
	E2prom_WriteBytes(PRAM_FILE_NAME,WriteAddr, pBuffer, NumBytesToWrite);
}

u8 FRAM_BufferRead(unsigned char *pBuffer,unsigned short NumBytesToRead,unsigned long ReadAddr) 
{
	return	E2prom_ReadBytes(PRAM_FILE_NAME,ReadAddr,pBuffer,NumBytesToRead);
}

//********************************±¾µØ±äÁ¿**********************************
/*************************************************************OK
** º¯ÊýÃû³Æ: EepromPram_ConvertParamIDToIndex
** ¹¦ÄÜÃèÊö: ¶þ·Ö·¨¼ìË÷£¬·µ»ØÒª²éÕÒµÄ¼ÇÂ¼
** Èë¿Ú²ÎÊý: 
** ³ö¿Ú²ÎÊý: ·µ»ØÒª²éÕÒµÄIndex
** ·µ»Ø²ÎÊý: 
** È«¾Ö±äÁ¿: ÎÞ
** µ÷ÓÃÄ£¿é:
*************************************************************/
unsigned char EepromPram_ConvertParamIDToIndex(unsigned int ParamID)//
{
    unsigned char index=0;
    ///////////////
    for(index = 1; index<EepromPramID_SIZE;index++)
    {
        if(ParamID==c_usEepromPramID[index])return index;;//ÕÒµ½IDÌø³ö
    }
	return 0;
}

/*********************************************************************
//º¯ÊýÃû³Æ	:EepromPram_WritePram
//¹¦ÄÜ		:Ð´eeprom²ÎÊý
//ÊäÈë		:PramID:²ÎÊýID;pBuffer:Ö¸ÏòÊý¾Ý£¨´ýÐ´ÈëµÄ×Ö½ÚÁ÷,Ð´ÈëµÄÊÇÊýÖµµÄ»°Ôò¸ß×Ö½ÚÓ¦ÔÚÇ°£©µÄÖ¸Õë;
//		    :BufferLen:Êý¾ÝµÄ³¤¶È
//Êä³ö		:
//Ê¹ÓÃ×ÊÔ´	:
//È«¾Ö±äÁ¿	:   
//µ÷ÓÃº¯Êý	:
//ÖÐ¶Ï×ÊÔ´	:  
//·µ»Ø		:
//±¸×¢		:Ð´ÈëÕýÈ·Ê±£¬·µ»ØµÄ³¤¶ÈµÈÓÚBufferLen;Ð´Èë´íÎóÊ±£¬·µ»Ø0,´ËÊ±¿É³¢ÊÔÔÙµ÷ÓÃ¸Ãº¯ÊýÐ´Ò»´Î
*********************************************************************/
u16 EepromPram_WritePramEx(unsigned short index, u8 *pBuffer, u8 BufferLen)
{
    u8	VerifyByte;
    u8	sum;
    u8	len;
    u8	Buffer[52];
    u8	tmp[52];
    u8	PramType;//0ÎªÖµ,1Îª×Ö·û´®
    u16	Address;
    //////////////////
    if(0==index)//²»ÈÏÊ¶µÄID
    {
        return 0;//ÏÈÔÝÊ±²»×ö
    }
    //ÅÐ¶Ï³¤¶ÈÊÇ·ñÕýÈ·
    if((BufferLen > EepromPramLength[index])||(0 == BufferLen))
    {
        return 0;//³¤¶È»ò²ÎÊýID³ö´í
    }
    //////////////////////////////    
    //ÇóÐ£ÑéºÍ//ÎªÊµ¼ÊÄÚÈÝµÄÀÛ¼ÓºÍ
    VerifyByte = Public_GetSumVerify(pBuffer,BufferLen);
    //»ñÈ¡²ÎÊýÀàÐÍ
    PramType = EepromPramSwitch[index];
    //**************Ð´²ÎÊý****************
    //»ñÈ¡µØÖ·
    Address = EepromPramAddress[index];
	//APP_DEBUG("\r\n<--WritePramID Address=%d  PramType =%d-->\r\n",Address,PramType);
    //////////////////////
    len = 0;
    ////////////////
    if(PramType)//Ð´µÄÊÇ×Ö·û´®,//µÚÒ»¸ö×Ö½ÚÐ´Èë×Ö·û´®µÄ³¤¶È
    {
        PramType = 1;
        Buffer[len++] = BufferLen;
    }
    ////////ÄÚÈÝÐ´Èëµ½eeprom/////////////
    memcpy(&Buffer[len],pBuffer,BufferLen);
    len +=BufferLen;
    //Ð´ÈëÐ£Ñé×Ö½Ú
    Buffer[len++] = VerifyByte;
    //½á¹ûÐ´Èëµ½eeprom
    //APP_DEBUG("³¤¶È...%d..%d....%04x",index,BufferLen,Address);
	//if(index==36)
	//{
	//	Address = E2_TERMINAL_HEARTBEAT_ADDR;
	//}
    E2prom_WriteBytes(PRAM_FILE_NAME,Address,Buffer,len);
    //**************¶Á²ÎÊý****************
    if(BufferLen > 49)return BufferLen;//×Ö½ÚÊý´óÓÚµÈÓÚ30×Ö½ÚµÄÖ±½Ó·µ»Ø,²»Ð£Ñé
    ///////////////
    //¶Á³öÐ´ÈëµÄÄÚÈÝ£¬³¤¶ÈºÍÐ£Ñé×Ö½Ú³ýÍâ
    E2prom_ReadBytes(PRAM_FILE_NAME,Address+PramType, tmp, BufferLen+1);
	/*
		if(PramType==1)
			{
	 		APP_DEBUG("\r\n<--E2prom_ReadBytes len=%d Address=%d Data=%c",BufferLen+1,Address+PramType,tmp[0]);
 		u8 i;
 		for(i=1;i<BufferLen+1;i++)
 			{
 			APP_DEBUG("%c",tmp[i]);
 			}
 		APP_DEBUG("-->\r\n");
			}
		else
			{
	 		APP_DEBUG("\r\n<--E2prom_ReadBytes len=%d Address=%d Data=%d",BufferLen+1,Address+PramType,tmp[0]);
 		u8 i;
 		for(i=1;i<BufferLen+1;i++)
 			{
 			APP_DEBUG("-%d",tmp[i]);
 			}
 		APP_DEBUG("-->\r\n");
			}
			*/
    //¼ÆËã¶Á³öÊý¾ÝµÄÐ£ÑéºÍ
    sum  = Public_GetSumVerify(tmp,BufferLen);
    //±È½ÏÐ£ÑéºÍ
    if((sum == VerifyByte)&&(tmp[BufferLen]==VerifyByte))
    {
        return BufferLen;
    }
    ///////Ð£Ñé³ö´í,ÔÙÐ´¶àÒ»±é/////////
    E2prom_WriteBytes(PRAM_FILE_NAME,Address,Buffer,len);
    ////////////
    return 0; 
}
/*********************************************************************
//º¯ÊýÃû³Æ	:EepromPram_WritePram(u32 PramID, u8 *pBuffer, BufferLen)
//¹¦ÄÜ		:Ð´eeprom²ÎÊý
//ÊäÈë		:PramID:²ÎÊýID;pBuffer:Ö¸ÏòÊý¾Ý£¨´ýÐ´ÈëµÄ×Ö½ÚÁ÷,Ð´ÈëµÄÊÇÊýÖµµÄ»°Ôò¸ß×Ö½ÚÓ¦ÔÚÇ°£©µÄÖ¸Õë;
//		:BufferLen:Êý¾ÝµÄ³¤¶È
//Êä³ö		:
//Ê¹ÓÃ×ÊÔ´	:
//È«¾Ö±äÁ¿	:   
//µ÷ÓÃº¯Êý	:
//ÖÐ¶Ï×ÊÔ´	:  
//·µ»Ø		:
//±¸×¢		:Ð´ÈëÕýÈ·Ê±£¬·µ»ØµÄ³¤¶ÈµÈÓÚBufferLen;Ð´Èë´íÎóÊ±£¬·µ»Ø0,´ËÊ±¿É³¢ÊÔÔÙµ÷ÓÃ¸Ãº¯ÊýÐ´Ò»´Î
*********************************************************************/
u16 EepromPram_WritePram(u32 PramID, u8 *pBuffer, u8 BufferLen)
{
    unsigned char index;
//APP_DEBUG("\r\n<--WritePramID:=%d-->\r\n",PramID);
    index = EepromPram_ConvertParamIDToIndex(PramID); 
//APP_DEBUG("\r\n<--WritePramID_INDEX:=%d-->\r\n",index);
    return EepromPram_WritePramEx(index,pBuffer,BufferLen);
}
/*********************************************************************
//º¯ÊýÃû³Æ	:EepromPram_ReadPram(u32 PramID, u8 *pBuffer)
//¹¦ÄÜ		:¶Áeeprom²ÎÊý
//ÊäÈë		:PramID:²ÎÊýID;pBuffer:Ö¸ÏòÊý¾Ý£¨¶Á³öµÄ×Ö½ÚÁ÷£¬¶Á³öµÄÊÇÊýÖµÔò¸ß×Ö½ÚÔÚÇ°£©µÄÖ¸Õë;
//Êä³ö		:
//Ê¹ÓÃ×ÊÔ´	:
//È«¾Ö±äÁ¿	:   
//µ÷ÓÃº¯Êý	:
//ÖÐ¶Ï×ÊÔ´	:  
//·µ»Ø		:
//±¸×¢		:·µ»ØÖµ>0±íÊ¾ÕýÈ·¶Á³ö£¬·µ»ØÖµ==0±íÊ¾¶Á´íÎó;
*********************************************************************/
u16 EepromPram_ReadPramEx(unsigned long pramID,unsigned short index, u8 *pBuffer)
{
    u16	Address;
    u8 PramType;
    u8 length;    
    u8 sum;
    u8 Buffer[52];
    /////////////////////////
    if(0==index)return 0;//ÏÈÔÝÊ±²»×ö//²»ÈÏÊ¶µÄID
    ///////////////
    if(0 == EepromPramLength[index])//²ÎÊýIDºÅÎª±£ÁôµÄ
    {
        return 0;
    }
    ///////////////
    Address = EepromPramAddress[index];
    ///////////////////
    PramType = EepromPramSwitch[index];
	
    if(PramType)
    {
    	//if(index == 36)
		//	Address = E2_TERMINAL_HEARTBEAT_ADDR;
        E2prom_ReadBytes(PRAM_FILE_NAME,Address, Buffer, 1);
        length = Buffer[0];//³¤¶È
        //APP_DEBUG("³¤¶È...%d..%d...%04x",index,length,Address);
        Address++;		
    }
    else
    {
        length = EepromPramLength[index];
    }
	
    ///////////////
    if(length > 49) return 0;   
    //°´³¤¶ÈÔÙ¶ÁÈ¡Êý¾ÝºÍÐ£Ñé×Ö½Ú
    E2prom_ReadBytes(PRAM_FILE_NAME,Address, Buffer, length+1);
    //¼ÆËãÐ£ÑéºÍ
    sum = Public_GetSumVerify(Buffer,length);
    if(sum == Buffer[length])//Ð£ÑéÕýÈ·
    {//APP_DEBUG("\r\nÐ£ÑéÕýÈ·.....%d\r\n",length)
    		//if(pramID == E2_DEVICE_PHONE_ID)Buffer[0] &= 0x0f;
        memcpy(pBuffer,Buffer,length);
        return length;
    }
    ////////////
    pBuffer[0]=0;
    return 0;
}
/*********************************************************************
//º¯ÊýÃû³Æ	:EepromPram_ReadPram(u32 PramID, u8 *pBuffer)
//¹¦ÄÜ		:¶Áeeprom²ÎÊý
//ÊäÈë		:PramID:²ÎÊýID;pBuffer:Ö¸ÏòÊý¾Ý£¨¶Á³öµÄ×Ö½ÚÁ÷£¬¶Á³öµÄÊÇÊýÖµÔò¸ß×Ö½ÚÔÚÇ°£©µÄÖ¸Õë;
//Êä³ö		:
//Ê¹ÓÃ×ÊÔ´	:
//È«¾Ö±äÁ¿	:   
//µ÷ÓÃº¯Êý	:
//ÖÐ¶Ï×ÊÔ´	:  
//·µ»Ø		:
//±¸×¢		:·µ»ØÖµ>0±íÊ¾ÕýÈ·¶Á³ö£¬·µ»ØÖµ==0±íÊ¾¶Á´íÎó;
*********************************************************************/
u16 EepromPram_ReadPram(u32 PramID, u8 *pBuffer)
{
    unsigned char index;    
    index = EepromPram_ConvertParamIDToIndex(PramID);
    return EepromPram_ReadPramEx(PramID,index,pBuffer);
}
/*********************************************************************
//º¯ÊýÃû³Æ	:EepromPram_UpdateVariable(u16 PramID)
//¹¦ÄÜ		:¸üÐÂPramID±äÁ¿
//ÊäÈë		:
//Êä³ö		:
//Ê¹ÓÃ×ÊÔ´	:
//È«¾Ö±äÁ¿	:   
//µ÷ÓÃº¯Êý	:
//ÖÐ¶Ï×ÊÔ´	:  
//·µ»Ø		:
//±¸×¢		:Ç§Íò×¢Òâ:EepromPram_UpdateVariable(E2_CAR_TOTAL_MILE_ID)²»¿ÉËæ±ãµ÷ÓÃ£¬Ö»ÄÜÔÚÉèÖÃÍêE2_CAR_TOTAL_MILE_IDºóµ÷ÓÃ              
*********************************************************************/
void EepromPram_UpdateVariable(u32 PramID)
{
	u8	Buffer[30];
	u8	BufferLen;
	u32	temp;
       
	switch(PramID)
	{
	case E2_TERMINAL_HEARTBEAT_ID:		
		{
			HeartBeat_UpdatePram();
			break;
		}//²ÎÊýID0x0001£¬ÖÕ¶ËÐÄÌø·¢ËÍ¼ä¸ô£¬µ¥Î»Ãë£¬Ä¬ÈÏÖµ60
	case E2_MAIN_SERVER_APN_ID:	
                {
                        break;
                }//²ÎÊýID0x0010£¬Ö÷·þÎñÆ÷APN
	//E2_MAIN_SERVER_DIAL_NAME_ID:			//²ÎÊýID0x0011£¬Ö÷·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÓÃ»§Ãû
	//E2_MAIN_SERVER_DIAL_PASSWORD_ID:		//²ÎÊýID0x0012£¬Ö÷·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÃÜÂë
	//E2_MAIN_SERVER_IP_ID:				//²ÎÊýID0x0013£¬Ö÷·þÎñÆ÷µØÖ·£¬IP»òÓòÃû
	//E2_BACKUP_SERVER_APN_ID:			//²ÎÊýID0x0014£¬±¸·Ý·þÎñÆ÷APN£¬ÎÞÏßÍ¨ÐÅ²¦ºÅ·ÃÎÊµã
	//E2_BACKUP_SERVER_DIAL_NAME_ID:		//²ÎÊýID0x0015£¬±¸·Ý·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÓÃ»§Ãû
	//E2_BACKUP_SERVER_DIAL_PASSWORD_ID:		//²ÎÊýID0x0016£¬±¸·Ý·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÃÜÂë
	//E2_BACKUP_SERVER_IP_ID:			//²ÎÊýID0x0017£¬±¸·Ý·þÎñÆ÷µØÖ·£¬IP»òÓòÃû
	//E2_MAIN_SERVER_TCP_PORT_ID:			//²ÎÊýID0x0018£¬Ö÷·þÎñÆ÷TCP¶Ë¿Ú
	//E2_MAIN_SERVER_UDP_PORT_ID:			//²ÎÊýID0x0019£¬Ö÷·þÎñÆ÷UDP¶Ë¿Ú
	//E2_BACKUP_SERVER_TCP_PORT_ID:		//²ÎÊýID0x001a£¬±¸·Ý·þÎñÆ÷TCP¶Ë¿Ú
	//E2_BACKUP_SERVER_UDP_PORT_ID:		//²ÎÊýID0x001b£¬±¸·Ý·þÎñÆ÷UDP¶Ë¿Ú
	//E2_REGISTER_SMS_CENTER_NUM_ID:		//²ÎÊýID0x001C£¬×¢²á¶ÌÐÅÖÐÐÄºÅÂë
	//E2_PRIVILEGE_SMS_CENTER_NUM_ID:		//²ÎÊýID0x001D£¬¿ØÖÆ¶ÌÐÅºÅÂë£¨ÌØÈ¨£©
	//0,0,						//²ÎÊýID0x001E~0x001f£¬±£Áô
	case E2_SLEEP_REPORT_TIME_ID://0X0027 ¸üÐÂACC OFF»ã±¨Ê±¼ä¼ä¸ô
		{
			Report_UpdatePram();
			break;
		}
	case E2_EMERGENCY_REPORT_TIME_ID://0X0028 ¸üÐÂ½ô¼±±¨¾¯Ê±»ã±¨Ê±¼ä¼ä¸ô
		{
			//Report_UpdateOnePram(PRAM_EMERGENCY_REPORT_TIME);
			//Report_UpdatePram();
			break;
		}
	case E2_ACCON_REPORT_TIME_ID://0X0029 ¸üÐÂACC ON»ã±¨Ê±¼ä¼ä¸ô
		{
			Report_UpdatePram();
			//Report_UpdateOnePram(PRAM_ACC_ON_REPORT_TIME);
			break;
		}
	case E2_CORNER_REPORT_ID://²ÎÊýID0x0030£¬¹Õµã²¹´«½Ç¶È,µ¥Î»¶È/Ãë£¬Ä¬ÈÏÖµÎª15
		{
			//Corner_UpdataPram();
			break;
		}
	case E2_ALARM_MASK_ID://²ÎÊýID0x0050£¬±¨¾¯ÆÁ±Î×Ö£¬Ä¬ÈÏÎªÈ«¿ª
	case E2_DEFINE_ALARM_MASK_WORD_ID://²ÎÊýID£¬×Ô¶¨Òå£¬×Ô¶¨Òå±¨¾¯ÆÁ±Î×Ö£¬DWORD
		{
			Io_UpdataPram();
            //Photo_UpdatePram();
			break;
		}
	case E2_MAX_SPEED_ID://²ÎÊýID0x0055£¬×î¸ßËÙ¶È
		{
             //Overspeed_UpdateParameter(); //Õý³£Ê¹ÓÃ
			break;
		}
	case E2_OVER_SPEED_KEEP_TIME_ID://²ÎÊýID0x0056£¬³¬ËÙ³ÖÐøÊ±¼ä£¬µ¥Î»ÎªÃë£¬Ä¬ÈÏÎª10Ãë
		{
              //Overspeed_UpdateParameter();//Õý³£Ê¹ÓÃ

			break;
		}
        case E2_SET_CRASH_ALARM_ID:	//²ÎÊýID0x005D,Åö×²±¨¾¯²ÎÊýÉèÖÃ
		{
			//UpdataBmaAppPra();
			break;
		}
        case E2_SET_ROLLOVER_ALARM_ID:	//²ÎÊýID0x005E,²à·­±¨¾¯²ÎÊýÉèÖÃ£º²à·­½Ç¶È£¬µ¥Î»1 ¶È£¬Ä¬ÈÏÎª30 ¶È 
		{
			//BMA250_UpdataRollOverAlarmParam();
			//UpdataBmaAppPra();
			break;
		}            
	case E2_CAR_TOTAL_MILE_ID:	//²ÎÊýID0x0080£¬³µÁ¾Àï³Ì±í¶ÁÊý£¬0.1km,Ç§Íò×¢Òâ:EepromPram_UpdateVariable(E2_CAR_TOTAL_MILE_ID)²»¿ÉËæ±ãµ÷ÓÃ£¬Ö»ÄÜÔÚÉèÖÃÍêE2_CAR_TOTAL_MILE_IDºóµ÷ÓÃ                                  			
		{
			//¶ÁÈ¡¸Õ¸ÕÉèÖÃµÄÀï³Ì
			EepromPram_ReadPram(E2_CAR_TOTAL_MILE_ID,Buffer);//0.1¹«ÀïÎªµ¥Î»
			temp = 0;
			temp |= Buffer[0] << 24;
			temp |= Buffer[1] << 16;
			temp |= Buffer[2] << 8;
			temp |= Buffer[3];
			//ÉèÖÃ³õÊ¼Àï³Ì
			GpsMile_SetPram(temp);
			
			break;
		}           
        case E2_GPS_SET_MODE_ID://²ÎÊýID0x0090,GNSS ¶¨Î»Ä£Ê½£¬Ä¬ÈÏÎª0x02 ±±¶·¶¨Î»	
		{
              //GnssCheckChangePar();
			
			break;
		}
	case E2_MANUFACTURE_ID://²ÎÊýIDE2_PRAM_BASE_CUSTOM_ID+0x05£¬×Ô¶¨Òå£¬ÖÆÔìÉÌID£¬5×Ö½Ú				
		{
			BufferLen = EepromPram_ReadPram(E2_MANUFACTURE_ID, Buffer);
			if(0 == BufferLen)
			{
			
			}
			break;
		}
	case E2_DEVICE_ID://²ÎÊýIDE2_PRAM_BASE_CUSTOM_ID+0x06£¬×Ô¶¨Òå£¬ÖÕ¶ËID£¬7×Ö½Ú
		{
			BufferLen = EepromPram_ReadPram(E2_DEVICE_ID, Buffer);
			if(0 == BufferLen)
			{
			
			}
			break;
		}
	case E2_DEVICE_PHONE_ID://²ÎÊýIDE2_PRAM_BASE_CUSTOM_ID+0x07£¬×Ô¶¨Òå£¬ÖÕ¶ËÊÖ»úºÅ£¬6×Ö½Ú
		{
			//RadioProtocol_UpdateTerminalPhoneNum();
			break;
		}
	case E2_MONITOR_SERVER_SMS_PHONE_ID:
			{
				UpSmsMonitorPhoNum();
				break;
			}
	case E2_SMS_ALARM_PHONE_NUM_ID:
			{
				UpSmsAlarmPhoNum();
				break;
			}
		default:	
          break;
	}

}

/*********************************************************************
//º¯ÊýÃû³Æ	:EepromPram_DefaultSet(void)
//¹¦ÄÜ		:ÏµÍ³µÚ1´ÎÔËÐÐÊ±µÄÄ¬ÈÏÉèÖÃ
//ÊäÈë		:
//Êä³ö		:
//Ê¹ÓÃ×ÊÔ´	:
//È«¾Ö±äÁ¿	:   
//µ÷ÓÃº¯Êý	:
//ÖÐ¶Ï×ÊÔ´	:  
//·µ»Ø		:
//±¸×¢		:Ê¹ÓÃ»Ö¸´Ä¬ÈÏ³ö³§ÉèÖÃÒ²»á½«²ÎÊýÉèÖÃÎªÈçÏÂËùÊ¾
*********************************************************************/
void EepromPram_DefaultSet(void)
{
    u32	temp=0;
	u8	Buffer[31]={0};
	u8	BufferLen=0;
    
    //0x0001	ÖÕ¶ËÐÄÌø·¢ËÍ¼ä¸ô//µ¥Î»Ãë//Ä¬ÈÏÖµ50//60»áÖ÷¶¯¶ÏÏß
    temp = 50;//60»áÖ÷¶¯¶ÏÏß
    Public_ConvertLongToBuffer(temp, Buffer);
    EepromPram_WritePram(E2_TERMINAL_HEARTBEAT_ID, Buffer, E2_TERMINAL_HEARTBEAT_LEN); 

	//0x0010	   Ö÷·þÎñÆ÷APN
	strcpy((char *)Buffer,"cmnet");
	BufferLen = strlen((char const *)Buffer);
	if(0 == EepromPram_WritePram(E2_MAIN_SERVER_APN_ID, Buffer, BufferLen))
	{
		EepromPram_WritePram(E2_MAIN_SERVER_APN_ID, Buffer, BufferLen);
	}
	//0x0011  Ö÷·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÓÃ»§Ãû
	strcpy((char *)Buffer,"card");
	BufferLen = strlen((char const *)Buffer);
	if(0 == EepromPram_WritePram(E2_MAIN_SERVER_DIAL_NAME_ID, Buffer, BufferLen))
	{
		EepromPram_WritePram(E2_MAIN_SERVER_DIAL_NAME_ID, Buffer, BufferLen);
	}
	//0x0012  Ö÷·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÃÜÂë
	strcpy((char *)Buffer,"card");
	BufferLen = strlen((char const *)Buffer);
	if(0 == EepromPram_WritePram(E2_MAIN_SERVER_DIAL_PASSWORD_ID, Buffer, BufferLen))
	{
		EepromPram_WritePram(E2_MAIN_SERVER_DIAL_PASSWORD_ID, Buffer, BufferLen);
	}

	//0x0014 ±¸·Ý·þÎñÆ÷APN
	strcpy((char *)Buffer,"cmnet");
	BufferLen = strlen((char const *)Buffer);
	if(0 == EepromPram_WritePram(E2_BACKUP_SERVER_APN_ID, Buffer, BufferLen))
	{
		EepromPram_WritePram(E2_BACKUP_SERVER_APN_ID, Buffer, BufferLen);
	}
	//0x0015 ±¸·Ý·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÓÃ»§Ãû
	strcpy((char *)Buffer,"card");
	BufferLen = strlen((char const *)Buffer);
	if(0 == EepromPram_WritePram(E2_BACKUP_SERVER_DIAL_NAME_ID, Buffer, BufferLen))
	{
		EepromPram_WritePram(E2_BACKUP_SERVER_DIAL_NAME_ID, Buffer, BufferLen);
	}
	//0x0016 ±¸·Ý·þÎñÆ÷ÎÞÏßÍ¨ÐÅ²¦ºÅÃÜÂë
	strcpy((char *)Buffer,"card");
	BufferLen = strlen((char const *)Buffer);
	if(0 == EepromPram_WritePram(E2_BACKUP_SERVER_DIAL_PASSWORD_ID, Buffer, BufferLen))
	{
		EepromPram_WritePram(E2_BACKUP_SERVER_DIAL_PASSWORD_ID, Buffer, BufferLen);
	}	 
    //0x0027	ÐÝÃßÊ±»ã±¨Ê±¼ä¼ä¸ô//µ¥Î»ÎªÃë//Ä¬ÈÏÖµÎª1200
    temp = 1200;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_SLEEP_REPORT_TIME_ID, Buffer, E2_SLEEP_REPORT_TIME_LEN))
    {
        EepromPram_WritePram(E2_SLEEP_REPORT_TIME_ID, Buffer, E2_SLEEP_REPORT_TIME_LEN);
    }

    //0x0028	½ô¼±±¨¾¯Ê±»ã±¨Ê±¼ä¼ä¸ô//µ¥Î»ÎªÃë//Ä¬ÈÏÖµÎª10
    temp = 10;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_EMERGENCY_REPORT_TIME_ID, Buffer, E2_EMERGENCY_REPORT_TIME_LEN))
    {
        EepromPram_WritePram(E2_EMERGENCY_REPORT_TIME_ID, Buffer, E2_EMERGENCY_REPORT_TIME_LEN);
    }

    //0x0029	È±Ê¡Ê±¼ä»ã±¨¼ä¸ô,µ¥Î»ÎªÃë,Ä¬ÈÏÎª30
    temp = 30;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_ACCON_REPORT_TIME_ID, Buffer, E2_ACCON_REPORT_TIME_LEN))
    {
        EepromPram_WritePram(E2_ACCON_REPORT_TIME_ID, Buffer, E2_ACCON_REPORT_TIME_LEN);
    }
    //0x0030	¹Õµã²¹´«½Ç¶È,µ¥Î»¶È/Ãë//Ä¬ÈÏÖµÎª30
    temp = 30;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_CORNER_REPORT_ID, Buffer, E2_CORNER_REPORT_LEN))
    {
        EepromPram_WritePram(E2_CORNER_REPORT_ID, Buffer, E2_CORNER_REPORT_LEN);
    }
    //0x0050	±¨¾¯ÆÁ±Î×Ö//bit9,11,12Ä¬ÈÏÆÁ±Î,LCDÆÁ¹ÊÕÏ,ÉãÏñÍ·¹ÊÕÏ,IC¿¨Ä£¿é¹ÊÕÏ,VSSÁ¬½ÓÒì³£Ä¬ÈÏÎªÆÁ±Î
    temp = 0x01001a00;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_ALARM_MASK_ID, Buffer, E2_ALARM_MASK_LEN))
    {
        EepromPram_WritePram(E2_ALARM_MASK_ID, Buffer, E2_ALARM_MASK_LEN);
    }
    //0x0055	×î¸ßËÙ¶È,µ¥Î»km/h,DWORD,Ä¬ÈÏÖµ100km/h
    temp = 100;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_MAX_SPEED_ID, Buffer, E2_MAX_SPEED_LEN))
    {
        EepromPram_WritePram(E2_MAX_SPEED_ID, Buffer, E2_MAX_SPEED_LEN);
    }
    //0x0056	³¬ËÙ³ÖÐøÊ±¼ä,µ¥Î»ÎªÃë,DWORD,Ä¬ÈÏÎª10Ãë,¾ÅÍ¨ÒªÇóÁ¢ÂíÉÏ±¨
    temp = 10;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_OVER_SPEED_KEEP_TIME_ID, Buffer, E2_OVER_SPEED_KEEP_TIME_LEN))
    {
        EepromPram_WritePram(E2_OVER_SPEED_KEEP_TIME_ID, Buffer, E2_OVER_SPEED_KEEP_TIME_LEN);
    }
    //0x005D	Åö×²±¨¾¯²ÎÊýÉèÖÃ,Åö×²Ê±¼äÄ¬ÈÏÎª4ms,Åö×²¼ÓËÙ¶ÈÄ¬ÈÏÉèÖÃÎª4g
    temp = 0x2801;//4g,4ms
    Public_ConvertShortToBuffer(temp, Buffer);//modify by joneming
    if(0 == EepromPram_WritePram(E2_SET_CRASH_ALARM_ID, Buffer, E2_SET_CRASH_ALARM_LEN))
    {
        EepromPram_WritePram(E2_SET_CRASH_ALARM_ID, Buffer, E2_SET_CRASH_ALARM_LEN);
    }
    //0x005E	²à·­±¨¾¯²ÎÊýÉèÖÃ£º²à·­½Ç¶È£¬µ¥Î»1 ¶È£¬Ä¬ÈÏÎª45 ¶È
    temp = 45;//45¶È
    Public_ConvertShortToBuffer(temp, Buffer);//modify by joneming
    if(0 == EepromPram_WritePram(E2_SET_ROLLOVER_ALARM_ID, Buffer, E2_SET_ROLLOVER_ALARM_LEN))
    {
        EepromPram_WritePram(E2_SET_ROLLOVER_ALARM_ID, Buffer, E2_SET_ROLLOVER_ALARM_LEN);
    }
    //0x0080,ÀÛ¼ÆÐÐÊ»Àï³ÌÇå0,³µÁ¾Àï³Ì±í¶ÁÊý, µ¥Î»0.1km
    temp = 0;
    Public_ConvertLongToBuffer(temp, Buffer);
    if(0 == EepromPram_WritePram(E2_CAR_TOTAL_MILE_ID, Buffer, E2_CAR_TOTAL_MILE_LEN))
    {
        EepromPram_WritePram(E2_CAR_TOTAL_MILE_ID, Buffer, E2_CAR_TOTAL_MILE_LEN);
    }

    //0x0090  GNSS ¶¨Î»Ä£Ê½£¬Ä¬ÈÏÎª0x03 ±±¶·¶¨Î»+GPS£¬È«¹ú»õÔËÆ½Ì¨Áªµ÷ÉèÖÃ
    temp = 3;
    Buffer[0] = temp;//modify by joneming
    if(0 == EepromPram_WritePram(E2_GPS_SET_MODE_ID, Buffer, E2_GPS_SET_MODE_LEN))
    {
        EepromPram_WritePram(E2_GPS_SET_MODE_ID, Buffer, E2_GPS_SET_MODE_LEN);
    }
    //²ÎÊýID 0xF205£¬×Ô¶¨Òå£¬ÖÆÔìÉÌID£¬5×Ö½Ú	ÖÆÔìÉÌID,Ä¬ÈÏÎª70108
    strcpy((char *)Buffer,"70108");
    if(0 == EepromPram_WritePram(E2_MANUFACTURE_ID, Buffer, E2_MANUFACTURE_ID_LEN))
    {
        EepromPram_WritePram(E2_MANUFACTURE_ID, Buffer, E2_MANUFACTURE_ID_LEN);
    }

	//0x0001f006 ÓÃ»§¶ÌÐÅ²Ù×÷ÃÜÂë
	strcpy((char *)Buffer,"123456");
	BufferLen = strlen((char const *)Buffer);
	if(0 == EepromPram_WritePram(E2_SMS_OPERATE_PWD_ID, Buffer, BufferLen))
	{
		EepromPram_WritePram(E2_SMS_OPERATE_PWD_ID, Buffer, BufferLen);
	}

	//0x0001f00f µÍµçÑ¹¹Ø»úµçÑ¹
	temp = 3500;//3500mv
    Public_ConvertShortToBuffer(temp, Buffer);
	if(0 == EepromPram_WritePram(E2_LOW_VOL_THRESHOLD_ID, Buffer, E2_LOW_VOL_THRESHOLD_ID_LEN))
	{
		EepromPram_WritePram(E2_LOW_VOL_THRESHOLD_ID, Buffer, E2_LOW_VOL_THRESHOLD_ID_LEN);
	}
}
/*************************************************************OK
** º¯ÊýÃû³Æ: EepromPram_GetLenEepromPramID
** ¹¦ÄÜÃèÊö: 
** Èë¿Ú²ÎÊý: 
** ³ö¿Ú²ÎÊý: 
** ·µ»Ø²ÎÊý: 
** È«¾Ö±äÁ¿: ÎÞ
** µ÷ÓÃÄ£¿é:
*************************************************************/
unsigned short EepromPram_GetLenEepromPramID(void)
{
    return EepromPramID_SIZE;
}


