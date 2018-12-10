#ifndef __FILEPRAM_H__
#define __FILEPRAM_H__

//#define FRAM_BufferWrite(WriteAddr, pBuffer, NumBytesToWrite) 0// E2prom_WriteBytes(WriteAddr, pBuffer, NumBytesToWrite)

//#define FRAM_BufferRead(pBuffer,NumBytesToRead,ReadAddr)  0// E2prom_ReadBytes(ReadAddr,pBuffer,NumBytesToRead)

#define PRAM_FILE_NAME    "device_pram.txt"


#define PARAMTER_MAX_LEN                   25//È¡Ç°Ãæ¶¨ÒåµÄ²ÎÊı³¤¶È×î´óÖµ

/***********²ÎÊıID¶¨Òå£¬Ë³Ğò°´ÕÕPARAMETER_STRUCTÅÅÁĞ**********************/
#define E2_TERMINAL_HEARTBEAT_ID 			0x0001//²ÎÊıID 0x0001,ÖÕ¶ËĞÄÌø·¢ËÍ¼ä¸ô//µ¥Î»Ãë//Ä¬ÈÏÖµ60
#define E2_MAIN_SERVER_APN_ID 				0x0010//²ÎÊıID 0x0010,Ö÷·şÎñÆ÷APN
#define E2_MAIN_SERVER_DIAL_NAME_ID 		0x0011//²ÎÊıID 0x0011,Ö÷·şÎñÆ÷ÎŞÏßÍ¨ĞÅ²¦ºÅÓÃ»§Ãû
#define E2_MAIN_SERVER_DIAL_PASSWORD_ID 	0x0012//²ÎÊıID 0x0012,Ö÷·şÎñÆ÷ÎŞÏßÍ¨ĞÅ²¦ºÅÃÜÂë
#define E2_MAIN_SERVER_IP_ID 				0x0013//²ÎÊıID 0x0013,Ö÷·şÎñÆ÷µØÖ·//IP»òÓòÃû
#define E2_BACKUP_SERVER_APN_ID 			0x0014//²ÎÊıID 0x0014,±¸·İ·şÎñÆ÷APN//ÎŞÏßÍ¨ĞÅ²¦ºÅ·ÃÎÊµã
#define E2_BACKUP_SERVER_DIAL_NAME_ID 		0x0015//²ÎÊıID 0x0015,±¸·İ·şÎñÆ÷ÎŞÏßÍ¨ĞÅ²¦ºÅÓÃ»§Ãû
#define E2_BACKUP_SERVER_DIAL_PASSWORD_ID 	0x0016//²ÎÊıID 0x0016,±¸·İ·şÎñÆ÷ÎŞÏßÍ¨ĞÅ²¦ºÅÃÜÂë
#define E2_BACKUP_SERVER_IP_ID 				0x0017//²ÎÊıID 0x0017,±¸·İ·şÎñÆ÷µØÖ·//IP»òÓòÃû
#define E2_MAIN_SERVER_TCP_PORT_ID 			0x0018//²ÎÊıID 0x0018,Ö÷·şÎñÆ÷TCP¶Ë¿Ú
#define E2_SLEEP_REPORT_TIME_ID 			0x0027//²ÎÊıID 0x0027,ĞİÃßÊ±»ã±¨Ê±¼ä¼ä¸ô//µ¥Î»ÎªÃë//Ä¬ÈÏÖµÎª240
#define E2_EMERGENCY_REPORT_TIME_ID 		0x0028//²ÎÊıID 0x0028,½ô¼±±¨¾¯Ê±»ã±¨Ê±¼ä¼ä¸ô//µ¥Î»ÎªÃë//Ä¬ÈÏÖµÎª10
#define	E2_ACCON_REPORT_TIME_ID				0x0029//²ÎÊıID 0x0029,È±Ê¡Ê±¼ä»ã±¨¼ä¸ô,µ¥Î»ÎªÃë,Ä¬ÈÏÎª30
#define E2_CORNER_REPORT_ID 				0x0030//²ÎÊıID 0x0030,¹Õµã²¹´«½Ç¶È,µ¥Î»¶È/Ãë//Ä¬ÈÏÖµÎª30
#define E2_MONITOR_SERVER_SMS_PHONE_ID      0x0043//¼à¿ØÆ½Ì¨SMSµç»°ºÅÂë
#define E2_SMS_TEXT_ALARM_PHONE_ID 			0x0044//SMSÎÄ±¾±¨¾¯µç»°ºÅÂë
#define E2_ALARM_MASK_ID 					0x0050//²ÎÊıID 0x0050,±¨¾¯ÆÁ±Î×Ö//bit9,11,12Ä¬ÈÏÆÁ±Î,LCDÆÁ¹ÊÕÏ,TTS¹ÊÕÏ,IC¿¨Ä£¿é¹ÊÕÏÄ¬ÈÏÎªÆÁ±Î
#define E2_MAX_SPEED_ID 					0x0055//²ÎÊıID 0x0055,×î¸ßËÙ¶È,µ¥Î»km/h,DWORD,Ä¬ÈÏÖµ130,¾ÅÍ¨ĞèÇóÎª100
#define E2_OVER_SPEED_KEEP_TIME_ID 			0x0056//²ÎÊıID 0x0056,³¬ËÙ³ÖĞøÊ±¼ä,µ¥Î»ÎªÃë,DWORD,Ä¬ÈÏÎª10Ãë,¾ÅÍ¨ĞèÇóÎªÁ¢ÂíÉÏ±¨
#define	E2_SET_CRASH_ALARM_ID				0x005D//²ÎÊıID 0x005D,Åö×²±¨¾¯²ÎÊıÉèÖÃ,Åö×²¼ÓËÙ¶ÈÄ¬ÈÏÉèÖÃÎª4g
#define	E2_SET_ROLLOVER_ALARM_ID	        0x005E//²ÎÊıID 0x005E,²à·­±¨¾¯²ÎÊıÉèÖÃ£º²à·­½Ç¶È£¬µ¥Î»1 ¶È£¬Ä¬ÈÏÎª45 ¶È
#define	E2_CAR_TOTAL_MILE_ID				0x0080//²ÎÊıID 0x0080£¬³µÁ¾Àï³Ì±í¶ÁÊı£¬0.1km
#define E2_CAR_PROVINCE_ID 					0x0081//²ÎÊıID 0x0081,³µÁ¾ËùÔÚµÄÊ¡ÓòID
#define E2_CAR_CITY_ID 						0x0082//²ÎÊıID 0x0082,³µÁ¾ËùÔÚµÄÊĞÓòID
#define E2_CAR_PLATE_NUM_ID 				0x0083//²ÎÊıID 0x0083,¹«°²½»Í¨¹ÜÀí²¿ÃÅ°ä·¢µÄ»ú¶¯³µºÅÅÆ
#define E2_CAR_PLATE_COLOR_ID 				0x0084//³µ²ÎÊıID 0x0084,³µÅÆÑÕÉ«//°´ÕÕJT/T415-2006µÄ5.4.12,1:À¶É«,2:»ÆÉ«,3:ºÚÉ«,4:°×É«,9:ÆäËü

#define	E2_GPS_SET_MODE_ID					0x0090//²ÎÊıID 0x0090,GNSS ¶¨Î»Ä£Ê½£¬Ä¬ÈÏÎª0x02 ±±¶·¶¨Î»

#define E2_PRAM_BASE_CUSTOM_ID           0xF200
#define E2_PRAM_BASE_CUSTOM_ID1          0x0001F000

#define E2_LOGINGPRS_PASSWORD_ID		(E2_PRAM_BASE_CUSTOM_ID+0x00)//²ÎÊıID 0xF200,¼øÈ¨Âë
#define E2_MANUFACTURE_ID				(E2_PRAM_BASE_CUSTOM_ID+0x05)//²ÎÊıID 0xF205£¬×Ô¶¨Òå£¬ÖÆÔìÉÌID£¬5×Ö½Ú	
#define E2_DEVICE_ID					(E2_PRAM_BASE_CUSTOM_ID+0x06)//²ÎÊıID 0xF206£¬×Ô¶¨Òå£¬³§ÉÌÖÕ¶ËID£¬7×Ö½Ú
#define E2_DEVICE_PHONE_ID				(E2_PRAM_BASE_CUSTOM_ID+0x07)//²ÎÊıID 0xF207£¬×Ô¶¨Òå£¬ÖÕ¶ËÊÖ»úºÅ£¬6×Ö½Ú£¬
#define E2_CAR_VIN_ID 					(E2_PRAM_BASE_CUSTOM_ID+0x08)//²ÎÊıID 0xF208£¬×Ô¶¨Òå£¬³µÁ¾Ê¶±ğÂë(VIN)£¬17×Ö½Ú£¬ASCIIÂë£¬
#define	E2_FIRMWARE_VERSION_ID			(E2_PRAM_BASE_CUSTOM_ID+0x0C)//²ÎÊıID 0xF20C£¬×Ô¶¨Òå£¬¹Ì¼ş°æ±¾ºÅ
#define E2_DEFINE_ALARM_MASK_WORD_ID	(E2_PRAM_BASE_CUSTOM_ID+0x43)//²ÎÊıID 0xF243£¬×Ô¶¨Òå£¬×Ô¶¨Òå±¨¾¯ÆÁ±Î×Ö£¬DWORD
#define E2_DEFINE_HARD_VER_ID			(E2_PRAM_BASE_CUSTOM_ID1+0x04)//²ÎÊıID 0x0001F004£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓ²¼ş°æ±¾ºÅ£¬DWORD
#define E2_SMS_ALARM_PHONE_NUM_ID		(E2_PRAM_BASE_CUSTOM_ID1+0x05)//²ÎÊıID 0x0001F005£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓÃ»§¶ÌĞÅ±¨¾¯µç»°£¬DWORD
#define E2_SMS_OPERATE_PWD_ID			(E2_PRAM_BASE_CUSTOM_ID1+0x06)//²ÎÊıID 0x0001F006£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓÃ»§¶ÌĞÅ²Ù×÷ÃÜÂë£¬DWORD
#define E2_LOW_VOL_THRESHOLD_ID			(E2_PRAM_BASE_CUSTOM_ID1+0x0f)//²ÎÊıID 0x0001F00F£¬×Ô¶¨Òå£¬×Ô¶¨ÒåµÍµçÑ¹ãĞÖµ£¬WORD
#define E2_DEVICE_BACKUP_ID             (E2_PRAM_BASE_CUSTOM_ID+0x2a)//²ÎÊıID 0xF22a£¬×Ô¶¨Òå£¬ÖÕ¶ËIDºÅ±¸·İ
#define	E2_SIM_ICCID_NUMBER_ID          (E2_PRAM_BASE_CUSTOM_ID+0x7F)//²ÎÊıID 0xF27F, ×Ô¶¨Òå£¬SIM¿¨µÄICCIDºÅÂë,BCD[10],Ö»¶Á

/*****************ID¶ÔÓ¦µÄ³¤¶È***************************/
#define E2_TERMINAL_HEARTBEAT_LEN 		4//²ÎÊıID 0x0001,ÖÕ¶ËĞÄÌø·¢ËÍ¼ä¸ô//µ¥Î»Ãë//Ä¬ÈÏÖµ60
#define E2_MAIN_SERVER_APN_LEN 			25//²ÎÊıID 0x0010,Ö÷·şÎñÆ÷APN
#define E2_MAIN_SERVER_DIAL_NAME_LEN 		25//²ÎÊıID 0x0011,Ö÷·şÎñÆ÷ÎŞÏßÍ¨ĞÅ²¦ºÅÓÃ»§Ãû
#define E2_MAIN_SERVER_DIAL_PASSWORD_LEN 	25//²ÎÊıID 0x0012,Ö÷·şÎñÆ÷ÎŞÏßÍ¨ĞÅ²¦ºÅÃÜÂë
#define E2_MAIN_SERVER_IP_LEN 			25//²ÎÊıID 0x0013,Ö÷·şÎñÆ÷µØÖ·//IP»òÓòÃû
#define E2_BACKUP_SERVER_APN_LEN 		25//²ÎÊıID 0x0014,±¸·İ·şÎñÆ÷APN//ÎŞÏßÍ¨ĞÅ²¦ºÅ·ÃÎÊµã
#define E2_BACKUP_SERVER_DIAL_NAME_LEN 		25//²ÎÊıID 0x0015,±¸·İ·şÎñÆ÷ÎŞÏßÍ¨ĞÅ²¦ºÅÓÃ»§Ãû
#define E2_BACKUP_SERVER_DIAL_PASSWORD_LEN 	25//²ÎÊıID 0x0016,±¸·İ·şÎñÆ÷ÎŞÏßÍ¨ĞÅ²¦ºÅÃÜÂë
#define E2_BACKUP_SERVER_IP_LEN 		25//²ÎÊıID 0x0017,±¸·İ·şÎñÆ÷µØÖ·//IP»òÓòÃû
#define E2_MAIN_SERVER_TCP_PORT_LEN 		4//²ÎÊıID 0x0018,Ö÷·şÎñÆ÷TCP¶Ë¿Ú
#define E2_SLEEP_REPORT_TIME_LEN 		4//²ÎÊıID 0x0027,ĞİÃßÊ±»ã±¨Ê±¼ä¼ä¸ô//µ¥Î»ÎªÃë//Ä¬ÈÏÖµÎª240
#define E2_EMERGENCY_REPORT_TIME_LEN		4//²ÎÊıID0x0028£¬½ô¼±±¨¾¯Ê±»ã±¨Ê±¼ä¼ä¸ô£¬µ¥Î»ÎªÃë£¬Ä¬ÈÏÖµÎª10
#define	E2_ACCON_REPORT_TIME_LEN		4//²ÎÊıID 0x0029,È±Ê¡Ê±¼ä»ã±¨¼ä¸ô,µ¥Î»ÎªÃë,Ä¬ÈÏÎª30
#define E2_CORNER_REPORT_LEN 			4//²ÎÊıID 0x0030,¹Õµã²¹´«½Ç¶È,µ¥Î»¶È/Ãë//Ä¬ÈÏÖµÎª30
#define E2_MONITOR_SERVER_SMS_PHONE_LEN		20	//²ÎÊıID0x0043£¬¼à¿ØÆ½Ì¨SMSµç»°ºÅÂë
#define E2_SMS_TEXT_ALARM_PHONE_LEN     	20  //²ÎÊıID0x0044£¬SMSÎÄ±¾±¨¾¯µç»°ºÅÂë
#define E2_ALARM_MASK_LEN 			4//²ÎÊıID 0x0050,±¨¾¯ÆÁ±Î×Ö//bit9,11,12Ä¬ÈÏÆÁ±Î,LCDÆÁ¹ÊÕÏ,TTS¹ÊÕÏ,IC¿¨Ä£¿é¹ÊÕÏÄ¬ÈÏÎªÆÁ±Î
#define E2_MAX_SPEED_LEN 			4//²ÎÊıID 0x0055,×î¸ßËÙ¶È,µ¥Î»km/h,DWORD,Ä¬ÈÏÖµ130,¾ÅÍ¨ĞèÇóÎª100
#define E2_OVER_SPEED_KEEP_TIME_LEN 		4//²ÎÊıID 0x0056,³¬ËÙ³ÖĞøÊ±¼ä,µ¥Î»ÎªÃë,DWORD,Ä¬ÈÏÎª10Ãë,¾ÅÍ¨ĞèÇóÎªÁ¢ÂíÉÏ±¨
#define	E2_SET_CRASH_ALARM_LEN			2//²ÎÊıID 0x005D,Åö×²±¨¾¯²ÎÊıÉèÖÃ,Åö×²¼ÓËÙ¶ÈÄ¬ÈÏÉèÖÃÎª4g
#define	E2_SET_ROLLOVER_ALARM_LEN	        2//²ÎÊıID 0x005E,²à·­±¨¾¯²ÎÊıÉèÖÃ£º²à·­½Ç¶È£¬µ¥Î»1 ¶È£¬Ä¬ÈÏÎª45 ¶È
#define E2_CAR_TOTAL_MILE_LEN			4	//²ÎÊıID0x0080£¬³µÁ¾Àï³Ì±í¶ÁÊı£¬0.1km
#define E2_CAR_PROVINCE_LEN 			2//²ÎÊıID 0x0081,³µÁ¾ËùÔÚµÄÊ¡ÓòID
#define E2_CAR_CITY_LEN 			2//²ÎÊıID 0x0082,³µÁ¾ËùÔÚµÄÊĞÓòID
#define E2_CAR_PLATE_NUM_LEN 			25//²ÎÊıID 0x0083,¹«°²½»Í¨¹ÜÀí²¿ÃÅ°ä·¢µÄ»ú¶¯³µºÅÅÆ
#define E2_CAR_PLATE_COLOR_LEN 			1//³µ²ÎÊıID 0x0084,ÅÆÑÕÉ«//°´ÕÕJT/T415-2006µÄ5.4.12,1:À¶É«,2:»ÆÉ«,3:ºÚÉ«,4:°×É«,9:ÆäËü
#define	E2_GPS_SET_MODE_LEN			1//²ÎÊıID 0x0090,GNSS ¶¨Î»Ä£Ê½£¬Ä¬ÈÏÎª0x02 ±±¶·¶¨Î»

#define E2_LOGINGPRS_PASSWORD_ID_LEN		25//²ÎÊıID 0xF200,¼øÈ¨Âë
#define E2_MANUFACTURE_ID_LEN			5//²ÎÊıID 0xF205£¬×Ô¶¨Òå£¬ÖÆÔìÉÌID£¬5×Ö½Ú	
#define E2_DEVICE_ID_LEN			7//²ÎÊıID 0xF206£¬×Ô¶¨Òå£¬³§ÉÌÖÕ¶ËID£¬7×Ö½Ú
#define E2_DEVICE_PHONE_ID_LEN			6//²ÎÊıID 0xF207£¬×Ô¶¨Òå£¬ÖÕ¶ËÊÖ»úºÅ£¬6×Ö½Ú£¬
#define	E2_CAR_VIN_ID_LEN				17//²ÎÊıID 0xF208³µÁ¾Ê¶±ğÂë(VIN)£¬17×Ö½Ú£¬ASCIIÂë
#define	E2_FIRMWARE_VERSION_ID_LEN		5//²ÎÊıID 0xF20C£¬×Ô¶¨Òå£¬¹Ì¼ş°æ±¾ºÅ
#define E2_DEVICE_BACKUP_ID_LEN			7//²ÎÊıID 0xF22a£¬×Ô¶¨Òå£¬³§ÉÌÖÕ¶ËID£¬7×Ö½Ú
#define E2_DEFINE_ALARM_MASK_WORD_ID_LEN	4//²ÎÊıID 0xF243£¬×Ô¶¨Òå£¬×Ô¶¨Òå±¨¾¯ÆÁ±Î×Ö£¬DWORD
#define E2_DEFINE_HARD_VER_ID_LEN			10//²ÎÊıID0x0001F0004£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓ²¼ş°æ±¾ºÅ£¬DWORD
#define E2_SMS_ALARM_PHONE_NUM_ID_LEN		20	//²ÎÊıID 0x0001F005£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓÃ»§¶ÌĞÅ±¨¾¯µç»°£
#define E2_SMS_OPERATE_PWD_ID_LEN     		6  //²ÎÊıID 0x0001F006£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓÃ»§¶ÌĞÅ²Ù×÷ÃÜÂë
#define E2_LOW_VOL_THRESHOLD_ID_LEN			2//²ÎÊıID 0x0001F00F£¬×Ô¶¨Òå£¬×Ô¶¨ÒåµÍµçÑ¹ãĞÖµ£¬WORD

/////////×Ô¶¨ÒåÏî×î´óIDÎª×îºóµÄID¼Ó1,Çë¸ù¾İ×îºóÒ»ÏîÈË¹¤ĞŞ¸Ä/////////////////////////////////////
#define	E2_SELF_DEFINE_ID_MAX               (E2_DEFINE_ALARM_MASK_WORD_ID+1)//×Ô¶¨ÒåÏî×î´óID,×¢Òâ£ºÃ¿´ÎĞÂÔö²ÎÊıÊ±ĞèÒª½«×îºóµÄ²ÎÊıID·ÅÔÚÕâ






#define FRAM_BASE_ADDR               0//Ç°ÃæÒ»°Ù×Ö½Úµ±×öÌúµçÓÃ
#define EEPROM_PRAM_BASE_ADDR           100//Ç°ÃæÒ»°Ù×Ö½Úµ±×öÌúµçÓÃ
//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
//***************************************²ÎÊı±£´æµÄÎÄ¼şÎ»ÖÃ*******************************
//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
#define E2_PRAM_START_ADDR                  EEPROM_PRAM_BASE_ADDR
#define E2_PRAM_END_ADDR                    E2_PRAM_START_ADDR+E2_SMS_OPERATE_PWD_ID_ADDR+E2_SMS_OPERATE_PWD_ID_LEN+4
//²ÎÊıµÄÆğÊ¼µØÖ·

#define E2_TERMINAL_HEARTBEAT_ADDR          EEPROM_PRAM_BASE_ADDR//4×Ö½Ú		
//²ÎÊıID0x0001£¬ÖÕ¶ËĞÄÌø·¢ËÍ¼ä¸ô£¬µ¥Î»Ãë£¬Ä¬ÈÏÖµ60

#define E2_MAIN_SERVER_APN_ADDR             (E2_TERMINAL_HEARTBEAT_ADDR+E2_TERMINAL_HEARTBEAT_LEN+1)//25×Ö½Ú		
//²ÎÊıID0x0010£¬Ö÷·şÎñÆ÷APN

#define E2_MAIN_SERVER_DIAL_NAME_ADDR       (E2_MAIN_SERVER_APN_ADDR+E2_MAIN_SERVER_APN_LEN+2)		
//²ÎÊıID0x0011£¬Ö÷·şÎñÆ÷ÎŞÏßÍ¨ĞÅ²¦ºÅÓÃ»§Ãû

#define E2_MAIN_SERVER_DIAL_PASSWORD_ADDR   (E2_MAIN_SERVER_DIAL_NAME_ADDR+E2_MAIN_SERVER_DIAL_NAME_LEN+2)			
//²ÎÊıID0x0012£¬Ö÷·şÎñÆ÷ÎŞÏßÍ¨ĞÅ²¦ºÅÃÜÂë

#define E2_MAIN_SERVER_IP_ADDR              (E2_MAIN_SERVER_DIAL_PASSWORD_ADDR+E2_MAIN_SERVER_DIAL_PASSWORD_LEN+2)	
//²ÎÊıID0x0013£¬Ö÷·şÎñÆ÷µØÖ·£¬IP»òÓòÃû

#define E2_BACKUP_SERVER_APN_ADDR		(E2_MAIN_SERVER_IP_ADDR+E2_MAIN_SERVER_IP_LEN+2)				
//²ÎÊıID0x0014£¬±¸·İ·şÎñÆ÷APN£¬ÎŞÏßÍ¨ĞÅ²¦ºÅ·ÃÎÊµã

#define E2_BACKUP_SERVER_DIAL_NAME_ADDR		(E2_BACKUP_SERVER_APN_ADDR+E2_BACKUP_SERVER_APN_LEN+2)	
//²ÎÊıID0x0015£¬±¸·İ·şÎñÆ÷ÎŞÏßÍ¨ĞÅ²¦ºÅÓÃ»§Ãû

#define E2_BACKUP_SERVER_DIAL_PASSWORD_ADDR	(E2_BACKUP_SERVER_DIAL_NAME_ADDR+E2_BACKUP_SERVER_DIAL_NAME_LEN+2)		
//²ÎÊıID0x0016£¬±¸·İ·şÎñÆ÷ÎŞÏßÍ¨ĞÅ²¦ºÅÃÜÂë

#define E2_BACKUP_SERVER_IP_ADDR		(E2_BACKUP_SERVER_DIAL_PASSWORD_ADDR+E2_BACKUP_SERVER_DIAL_PASSWORD_LEN+2)			
//²ÎÊıID0x0017£¬±¸·İ·şÎñÆ÷µØÖ·£¬IP»òÓòÃû

#define E2_MAIN_SERVER_TCP_PORT_ADDR        (E2_BACKUP_SERVER_IP_ADDR+E2_BACKUP_SERVER_IP_LEN+2)			
//²ÎÊıID0x0018£¬Ö÷·şÎñÆ÷TCP¶Ë¿Ú

#define E2_SLEEP_REPORT_TIME_ADDR           (E2_MAIN_SERVER_TCP_PORT_ADDR+E2_MAIN_SERVER_TCP_PORT_LEN+2)		
//²ÎÊıID0x0027£¬ĞİÃßÊ±»ã±¨Ê±¼ä¼ä¸ô£¬µ¥Î»ÎªÃë£¬Ä¬ÈÏÖµÎª60

#define E2_EMERGENCY_REPORT_TIME_ADR  (E2_SLEEP_REPORT_TIME_ADDR+E2_SLEEP_REPORT_TIME_LEN+1)
//²ÎÊıD0x0028,½ô¼±±¨¾¯Ê±»ã±¨Ê±¼ä¼ä¸ô//µ¥Î»ÎªÃë//Ä¬ÈÏÖµÎª10

#define	E2_ACCON_REPORT_TIME_ADDR           (E2_EMERGENCY_REPORT_TIME_ADR+E2_EMERGENCY_REPORT_TIME_LEN+1)		
//E2_PRAM_BASE_CUSTOM_ID+0x0029È±Ê¡Ê±¼ä»ã±¨¼ä¸ô,µ¥Î»ÎªÃë

#define E2_CORNER_REPORT_ADDR		(E2_ACCON_REPORT_TIME_ADDR+E2_ACCON_REPORT_TIME_LEN+1)	
//²ÎÊıID0x0030£¬¹Õµã²¹´«½Ç¶È,µ¥Î»¶È/Ãë£¬Ä¬ÈÏÖµÎª15

#define E2_MONITOR_SERVER_SMS_PHONE_ADDR    (E2_CORNER_REPORT_ADDR+E2_CORNER_REPORT_LEN+1) 
//²ÎÊıID0x0043£¬¼à¿ØÆ½Ì¨SMSµç»°ºÅÂë

#define E2_SMS_TEXT_ALARM_PHONE_ADDR		(E2_MONITOR_SERVER_SMS_PHONE_ADDR+E2_MONITOR_SERVER_SMS_PHONE_LEN+2)	
//²ÎÊıID0x0044£¬SMSÎÄ±¾±¨¾¯µç»°ºÅÂë

#define E2_ALARM_MASK_ADDR					(E2_SMS_TEXT_ALARM_PHONE_ADDR+E2_SMS_TEXT_ALARM_PHONE_LEN+2)
//²ÎÊıID0x0050£¬±¨¾¯ÆÁ±Î×Ö£¬Ä¬ÈÏÎªÈ«¿ª

#define E2_MAX_SPEED_ADDR			(E2_ALARM_MASK_ADDR+E2_ALARM_MASK_LEN+1)				
//²ÎÊıID0x0055£¬×î¸ßËÙ¶È

#define E2_OVER_SPEED_KEEP_TIME_ADDR		(E2_MAX_SPEED_ADDR+E2_MAX_SPEED_LEN+1)			
//²ÎÊıID0x0056£¬³¬ËÙ³ÖĞøÊ±¼ä£¬µ¥Î»ÎªÃë£¬Ä¬ÈÏÎª10Ãë

#define E2_SET_CRASH_ALARM_ADDR		                (E2_OVER_SPEED_KEEP_TIME_ADDR+E2_OVER_SPEED_KEEP_TIME_LEN+1)
//²ÎÊıID0x005D//Åö×²±¨¾¯²ÎÊıÉèÖÃ 

#define E2_SET_ROLLOVER_ALARM_ADDR		        (E2_SET_CRASH_ALARM_ADDR+E2_SET_CRASH_ALARM_LEN+1)
//²ÎÊıID0x005E//²à·­±¨¾¯²ÎÊıÉèÖÃ£º²à·­½Ç¶È£¬µ¥Î»1 ¶È£¬Ä¬ÈÏÎª30 ¶È

#define E2_CAR_TOTAL_MILE_ADDR			(E2_SET_ROLLOVER_ALARM_ADDR+E2_SET_ROLLOVER_ALARM_LEN+1)		
//²ÎÊıID0x0080£¬³µÁ¾Àï³Ì±í¶ÁÊı£¬0.1km

#define E2_CAR_PROVINCE_ID_ADDR			(E2_CAR_TOTAL_MILE_ADDR+E2_CAR_TOTAL_MILE_LEN+1)		
//²ÎÊıID0x0081£¬³µÁ¾ËùÔÚµÄÊ¡ÓòID

#define E2_CAR_CITY_ID_ADDR			(E2_CAR_PROVINCE_ID_ADDR+E2_CAR_PROVINCE_LEN+1)			
//²ÎÊıID0x0082£¬³µÁ¾ËùÔÚµÄÊĞÓòID

#define E2_CAR_PLATE_NUM_ADDR			(E2_CAR_CITY_ID_ADDR+E2_CAR_CITY_LEN+1)			
//²ÎÊıID0x0083£¬¹«°²½»Í¨¹ÜÀí²¿ÃÅ°ä·¢µÄ»ú¶¯³µºÅÅÆ

#define E2_CAR_PLATE_COLOR_ADDR			(E2_CAR_PLATE_NUM_ADDR+E2_CAR_PLATE_NUM_LEN+2)	
//²ÎÊıID0x0084£¬³µÅÆÑÕÉ«£¬°´ÕÕJT/T415-2006µÄ5.4.12

#define E2_GPS_SET_MODE_ADDR		                (E2_CAR_PLATE_COLOR_ADDR+E2_CAR_PLATE_COLOR_LEN+1)
//²ÎÊıID0x0090//GNSS ¶¨Î»Ä£Ê½£¬Ä¬ÈÏÎªE2_PRAM_BASE_CUSTOM_ID+0x ±±¶·¶¨Î»


//////////////////////////////////////////////////////////////
//*************************×Ô¶¨Òå²ÎÊıIDµØÖ·************************
#define E2_LOGINGPRS_PASSWORD_ID_ADDR			(E2_GPS_SET_MODE_ADDR+E2_GPS_SET_MODE_LEN+1)
//²ÎÊıIDE2_PRAM_BASE_CUSTOM_ID+0x00£¬×Ô¶¨Òå£¬³§ÉÌÖÕ¶ËID£¬7×Ö½Ú

#define E2_MANUFACTURE_ID_ADDR		(E2_LOGINGPRS_PASSWORD_ID_ADDR+E2_LOGINGPRS_PASSWORD_ID_LEN+2)
//²ÎÊıIDE2_PRAM_BASE_CUSTOM_ID+0x05£¬ÖÆÔìÉÌID£¬5×Ö½Ú	

#define E2_DEVICE_ID_ADDR			(E2_MANUFACTURE_ID_ADDR+E2_MANUFACTURE_ID_LEN+2)
//²ÎÊıIDE2_PRAM_BASE_CUSTOM_ID+0x06£¬×Ô¶¨Òå£¬³§ÉÌÖÕ¶ËID£¬7×Ö½Ú

#define E2_DEVICE_PHONE_ID_ADDR			(E2_DEVICE_ID_ADDR+E2_DEVICE_ID_LEN+2)
//²ÎÊıIDE2_PRAM_BASE_CUSTOM_ID+0x07£¬×Ô¶¨Òå£¬ÖÕ¶ËÊÖ»úºÅ£¬6×Ö½Ú

#define E2_CAR_VIN_ID_ADDR			(E2_DEVICE_PHONE_ID_ADDR+E2_DEVICE_PHONE_ID_LEN+2)
//²ÎÊıID 0xF208³µÁ¾Ê¶±ğÂë(VIN)£¬17×Ö½Ú£¬ASCIIÂë

#define	E2_FIRMWARE_VERSION_ID_ADR	(E2_CAR_VIN_ID_ADDR+E2_CAR_VIN_ID_LEN+2)
//²ÎÊıIDE2_PRAM_BASE_CUSTOM_ID 0x0C£¬×Ô¶¨Òå£¬¹Ì¼ş°æ±¾ºÅ

#define E2_DEVICE_BACKUP_ID_ADDR			(E2_FIRMWARE_VERSION_ID_ADR+E2_FIRMWARE_VERSION_ID_LEN+2)
//²ÎÊıIDE2_PRAM_BASE_CUSTOM_ID+0x2a£¬×Ô¶¨Òå£¬³§ÉÌÖÕ¶ËID£¬7×Ö½Ú

#define	E2_DEFINE_ALARM_MASK_WORD_ID_ADDR		(E2_DEVICE_BACKUP_ID_ADDR+E2_DEVICE_BACKUP_ID_LEN+2)
//²ÎÊıIDE2_PRAM_BASE_CUSTOM_ID+0x43£¬×Ô¶¨Òå£¬³µÁ¾Ê¶±ğÂë

//32²ÎÊıID0x0001F0004£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓ²¼ş°æ±¾ºÅ£¬DWORD
#define	E2_DEFINE_HARD_VER_ID_ADDR 		(E2_DEFINE_ALARM_MASK_WORD_ID_ADDR+E2_DEFINE_ALARM_MASK_WORD_ID_LEN+2)	

//²ÎÊıID 0x0001F005£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓÃ»§¶ÌĞÅ±¨¾¯µç»°
#define	E2_SMS_ALARM_PHONE_NUM_ID_ADDR 		(E2_DEFINE_HARD_VER_ID_ADDR+E2_DEFINE_HARD_VER_ID_LEN+2)	

//²ÎÊıID 0x0001F006£¬×Ô¶¨Òå£¬×Ô¶¨ÒåÓÃ»§¶ÌĞÅ²Ù×÷ÃÜÂë
#define E2_SMS_OPERATE_PWD_ID_ADDR     		(E2_SMS_ALARM_PHONE_NUM_ID_ADDR+E2_SMS_ALARM_PHONE_NUM_ID_LEN+2)  

//²ÎÊıID 0x0001F00F£¬×Ô¶¨Òå£¬×Ô¶¨ÒåµÍµçÑ¹ãĞÖµ£¬WORD
#define E2_LOW_VOL_THRESHOLD_ID_ADDR		(E2_SMS_OPERATE_PWD_ID_ADDR+E2_SMS_OPERATE_PWD_ID_LEN+2)

//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
//***************************************×Ô¶¨ÒåÊı¾İÀàĞÍ********************************
//*****************************************************************************************
//*****************************************************************************************
//*****************************************************************************************
#define FRAM_OIL_CTRL_ADDR                      FRAM_BASE_ADDR //¶ÏÓÍÂ·
#define FRAM_OIL_CTRL_ADDR_LEN                  1
///////////////////////////////////

///////////////////////////////
//Éı¼¶ÈÎÎñ´¥·¢±êÖ¾£¬µÈÓÚ1±íÊ¾´¥·¢ÁËÔ¶³ÌÉı¼¶£¬µÈÓÚ2±íÊ¾Éı¼¶³É¹¦£¬Ò»×Ö½Ú
//µÈÓÚ3±íÊ¾Éı¼¶Ê§°Ü£¬µÈÓÚ4±íÊ¾ÒÑ·¢ËÍ¹ı0x0108ÃüÁîÁË£¬Ã¿´ÎÉÏµç¼øÈ¨Íê³ÉºóĞèÒª¶ÁÈ¡¸ÃÖµ£¬
#define FRAM_FIRMWARE_UPDATA_FLAG_ADDR          2//Éı¼¶ÈÎÎñ´¥·¢±êÖ¾£¬µÈÓÚ1±íÊ¾´¥·¢ÁËÔ¶³ÌÉı¼¶£¬µÈÓÚ2±íÊ¾Éı¼¶³É¹¦£¬                                                    //µÈÓÚ3±íÊ¾Éı¼¶Ê§°Ü£¬µÈÓÚ4±íÊ¾ÒÑ·¢ËÍ¹ı0x0108ÃüÁîÁË£¬Ã¿´ÎÉÏµç¼øÈ¨Íê³ÉºóĞèÒª¶ÁÈ¡¸ÃÖµ£¬
#define FRAM_FIRMWARE_UPDATA_FLAG_LEN           1

/////////////////////////////////////
#define FRAM_FIRMWARE_UPDATA_TYPE_ADDR          4//Ô¶³ÌÉı¼¶ÀàĞÍ,0ÎªÖÕ¶Ë,9ÎªÔØÖØ¿ØÖÆÆ÷,Ä¿Ç°Ö»Ö§³ÖÕâÁ½¸ö
#define FRAM_FIRMWARE_UPDATA_TYPE_LEN           1

#define FRAM_LAST_LOCATION_ADDR                 6	//ÉÏÒ»¸öÓĞĞ§¶¨Î»µã
#define FRAM_LAST_LOCATION_LEN                  11	//ÉÏÒ»¸öÓĞĞ§¶¨Î»µã

#define FRAM_MILEAGE_ADDR                       18	//×ÜÀï³Ì´æ´¢µØÖ·,´øĞ£Ñé×Ö½Ú,
#define FRAM_MILEAGE_LEN                        4	//×ÜÀï³Ì´æ´¢³¤¶È,´øĞ£Ñé×Ö½Ú

#define FRAM_EMERGENCY_FLAG_ADDR				23	//½ô¼±±¨¾¯ÉÏÒ»´Î×´Ì¬
#define FRAM_EMERGENCY_FLAG_LEN					1

#define FRAM_ROLLOVER_BASS_ADDR					25	//²à·­»ù×¼½Ç¶È
#define FRAM_ROLLOVER_BASS_LEN					4

#define FRAM_SLEEP_FLAG_ADDR					30	//ĞİÃß½øÈë±êÖ¾
#define FRAM_SLEEP_FLAG_LEN						1

#define FRAM_ACC_LINE_FLAG_ADDR					32	//ÊÇ·ñÓĞACC Ïß
#define FRAM_ACC_LINE_FLAG_LEN					1

#define FRAM_TEMP_TRACK_NUM_ADDR            	34 //ÁÙÊ±¸ú×ÙÊ£Óà´ÎÊı
#define FRAM_TEMP_TRACK_NUM_LEN             	4

#define FRAM_TEMP_TRACK_SPACE_ADDR              38   //ÁÙÊ±¸ú×Ù¼ä¸ô
#define FRAM_TEMP_TRACK_SPACE_LEN               2  

#define FRAM_TEMP_TRACK_VERIFY_ADDR              40   //crc32Ğ£Ñé
#define FRAM_TEMP_TRACK_VERIFY_LEN               4  

#define FRAM_FCT_SELF_CHECK_ADDR              	44   //FCT×Ô¼ì±êÖ¾
#define FRAM_FCT_SELF_CHECK_LEN               	1  

#define FRAM_RTC_WAKEUP_ADDR              		45   //RTC»½ĞÑÉÏ±¨Ê±¼äÆ«ÒÆÁ¿
#define FRAM_RTC_WAKEUP_LEN               		4  

//////////////////////










//-------------------------------------------------------------------------------------------------
//------------------------------------º¯ÊıÉùÃ÷-----------------------------------------------------
//-------------------------------------------------------------------------------------------------
//********************************±¾µØ±äÁ¿**********************************
/*************************************************************OK
** º¯ÊıÃû³Æ: EepromPram_ConvertParamIDToIndex
** ¹¦ÄÜÃèÊö: ¶ş·Ö·¨¼ìË÷£¬·µ»ØÒª²éÕÒµÄ¼ÇÂ¼
** Èë¿Ú²ÎÊı: 
** ³ö¿Ú²ÎÊı: ·µ»ØÒª²éÕÒµÄIndex
** ·µ»Ø²ÎÊı: 
** È«¾Ö±äÁ¿: ÎŞ
** µ÷ÓÃÄ£¿é:
*************************************************************/
unsigned char EepromPram_ConvertParamIDToIndex(unsigned int ParamID);
/*********************************************************************
//º¯ÊıÃû³Æ	:EepromPram_WritePram
//¹¦ÄÜ		:Ğ´eeprom²ÎÊı
//ÊäÈë		:PramID:²ÎÊıID;pBuffer:Ö¸ÏòÊı¾İ£¨´ıĞ´ÈëµÄ×Ö½ÚÁ÷,Ğ´ÈëµÄÊÇÊıÖµµÄ»°Ôò¸ß×Ö½ÚÓ¦ÔÚÇ°£©µÄÖ¸Õë;
//		    :BufferLen:Êı¾İµÄ³¤¶È
//Êä³ö		:
//Ê¹ÓÃ×ÊÔ´	:
//È«¾Ö±äÁ¿	:   
//µ÷ÓÃº¯Êı	:
//ÖĞ¶Ï×ÊÔ´	:  
//·µ»Ø		:
//±¸×¢		:Ğ´ÈëÕıÈ·Ê±£¬·µ»ØµÄ³¤¶ÈµÈÓÚBufferLen;Ğ´Èë´íÎóÊ±£¬·µ»Ø0,´ËÊ±¿É³¢ÊÔÔÙµ÷ÓÃ¸Ãº¯ÊıĞ´Ò»´Î
*********************************************************************/
u16 EepromPram_WritePramEx(unsigned short index, u8 *pBuffer, u8 BufferLen);
/*********************************************************************
//º¯ÊıÃû³Æ	:EepromPram_WritePram(u32 PramID, u8 *pBuffer, BufferLen)
//¹¦ÄÜ		:Ğ´eeprom²ÎÊı
//ÊäÈë		:PramID:²ÎÊıID;pBuffer:Ö¸ÏòÊı¾İ£¨´ıĞ´ÈëµÄ×Ö½ÚÁ÷,Ğ´ÈëµÄÊÇÊıÖµµÄ»°Ôò¸ß×Ö½ÚÓ¦ÔÚÇ°£©µÄÖ¸Õë;
//		:BufferLen:Êı¾İµÄ³¤¶È
//Êä³ö		:
//Ê¹ÓÃ×ÊÔ´	:
//È«¾Ö±äÁ¿	:   
//µ÷ÓÃº¯Êı	:
//ÖĞ¶Ï×ÊÔ´	:  
//·µ»Ø		:
//±¸×¢		:Ğ´ÈëÕıÈ·Ê±£¬·µ»ØµÄ³¤¶ÈµÈÓÚBufferLen;Ğ´Èë´íÎóÊ±£¬·µ»Ø0,´ËÊ±¿É³¢ÊÔÔÙµ÷ÓÃ¸Ãº¯ÊıĞ´Ò»´Î
*********************************************************************/
u16 EepromPram_WritePram(u32 PramID, u8 *pBuffer, u8 BufferLen);
/*********************************************************************
//º¯ÊıÃû³Æ	:EepromPram_ReadPram(u32 PramID, u8 *pBuffer)
//¹¦ÄÜ		:¶Áeeprom²ÎÊı
//ÊäÈë		:PramID:²ÎÊıID;pBuffer:Ö¸ÏòÊı¾İ£¨¶Á³öµÄ×Ö½ÚÁ÷£¬¶Á³öµÄÊÇÊıÖµÔò¸ß×Ö½ÚÔÚÇ°£©µÄÖ¸Õë;
//Êä³ö		:
//Ê¹ÓÃ×ÊÔ´	:
//È«¾Ö±äÁ¿	:   
//µ÷ÓÃº¯Êı	:
//ÖĞ¶Ï×ÊÔ´	:  
//·µ»Ø		:
//±¸×¢		:·µ»ØÖµ>0±íÊ¾ÕıÈ·¶Á³ö£¬·µ»ØÖµ==0±íÊ¾¶Á´íÎó;
*********************************************************************/
u16 EepromPram_ReadPramEx(unsigned long pramID,unsigned short index, u8 *pBuffer);

/*********************************************************************
//º¯ÊıÃû³Æ	:EepromPram_UpdateVariable(u16 PramID)
//¹¦ÄÜ		:¸üĞÂPramID±äÁ¿
//ÊäÈë		:
//Êä³ö		:
//Ê¹ÓÃ×ÊÔ´	:
//È«¾Ö±äÁ¿	:   
//µ÷ÓÃº¯Êı	:
//ÖĞ¶Ï×ÊÔ´	:  
//·µ»Ø		:
//±¸×¢		:Ç§Íò×¢Òâ:EepromPram_UpdateVariable(E2_CAR_TOTAL_MILE_ID)²»¿ÉËæ±ãµ÷ÓÃ£¬Ö»ÄÜÔÚÉèÖÃÍêE2_CAR_TOTAL_MILE_IDºóµ÷ÓÃ              
*********************************************************************/
void EepromPram_UpdateVariable(u32 PramID);
/*********************************************************************
//º¯ÊıÃû³Æ	:EepromPram_DefaultSet(void)
//¹¦ÄÜ		:ÏµÍ³µÚ1´ÎÔËĞĞÊ±µÄÄ¬ÈÏÉèÖÃ
//ÊäÈë		:
//Êä³ö		:
//Ê¹ÓÃ×ÊÔ´	:
//È«¾Ö±äÁ¿	:   
//µ÷ÓÃº¯Êı	:
//ÖĞ¶Ï×ÊÔ´	:  
//·µ»Ø		:
//±¸×¢		:Ê¹ÓÃ»Ö¸´Ä¬ÈÏ³ö³§ÉèÖÃÒ²»á½«²ÎÊıÉèÖÃÎªÈçÏÂËùÊ¾
*********************************************************************/
void EepromPram_DefaultSet(void);
/*************************************************************OK
** º¯ÊıÃû³Æ: EepromPram_GetLenEepromPramID
** ¹¦ÄÜÃèÊö: 
** Èë¿Ú²ÎÊı: 
** ³ö¿Ú²ÎÊı: 
** ·µ»Ø²ÎÊı: 
** È«¾Ö±äÁ¿: ÎŞ
** µ÷ÓÃÄ£¿é:
*************************************************************/
unsigned short EepromPram_GetLenEepromPramID(void);

unsigned char FRAM_BufferWrite(unsigned short WriteAddr,unsigned char *pBuffer,unsigned short  NumBytesToWrite);


unsigned char FRAM_BufferRead(unsigned char *pBuffer,unsigned short NumBytesToRead,unsigned long ReadAddr);
/*************************************************************
** º¯ÊıÃû³Æ: Eepromflash_ReadBytes
** ¹¦ÄÜÃèÊö: ¶Á×Ö½Ú²Ù×÷
** Èë¿Ú²ÎÊı: Address¶ÁµÄÆğÊ¼µØÖ·,*data ¶Á³öÊı¾İ´æ·ÅµØÖ·Ö¸Õë,len ¶ÁµÄ×Ö½ÚÊı
** ³ö¿Ú²ÎÊı: 
** ·µ»Ø²ÎÊı: 
** È«¾Ö±äÁ¿: 
** µ÷ÓÃÄ£¿é: 
*************************************************************/	
u8 E2prom_ReadBytes(char* lpFileName,unsigned long Address, unsigned char *data, unsigned short len);
/*************************************************************
** º¯ÊıÃû³Æ: Eepromflash_WriteBytes
** ¹¦ÄÜÃèÊö: Ğ´×Ö½Ú²Ù×÷
** Èë¿Ú²ÎÊı: AddressĞ´µÄÆğÊ¼µØÖ·,*data Ğ´ÈëÊı¾İ´æ·ÅµØÖ·Ö¸Õë,len Ğ´µÄ×Ö½ÚÊı
** ³ö¿Ú²ÎÊı: 
** ·µ»Ø²ÎÊı: 1:³É¹¦£¬0:Ê§°Ü
** È«¾Ö±äÁ¿: 
** µ÷ÓÃÄ£¿é: 
*************************************************************/
unsigned char E2prom_WriteBytes(char* lpFileName,unsigned long Address, unsigned char *data,unsigned short len);

#endif
