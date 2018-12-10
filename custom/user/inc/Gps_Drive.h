

#ifndef __GPS_DRIVE_H__
#define __GPS_DRIVE_H__

#include "ql_type.h"

#define __GNSS_NEW_API__            // receive NMEA by callback funciton

#define MSG_GNSS_ON   0
#define MSG_GNSS_OFF  1

#define GPS_TIMER_PERIOD			1000								//GPS定位轮询时间
#define TIMEOUT_30S_PERIOD  	 	20000									//GPS定位超时时间

#define GPRMC_BUFFER_SIZE  200
#define GPGGA_BUFFER_SIZE  200

typedef enum{
	STATE_WAIT_GPRS_NET=0,					//等待MC20模块注册网络成功
	STATE_TIME_SYNC,						//查询时间是否同步，否，则同步
	STATE_NTP_WAIT,						//等待时间同步
	STATE_GPS_NET,						//获取基站位置激活网络
	STATE_GET_LOC,							//获取基站位置
	STATE_WAIT_LOC,						//等待获取基站位置
	STATE_EPO_START,						//使能EPO
	STATE_CHECK_FIX,						//获取GPS数据
	STATE_GNSS_OFF,   						//关闭EPO GNSS
	STATE_GPS_IDLE//空闲状态
}Enum_GPSSTATE;												//GPS定位流程


//初始化 gps  ，gps超时，
void init_GPStimer(void);
void JO_OpenGPSOutTimer(void);
void JO_OpenGPSTimer(void);
void SentGnssOnMsgToTask(void);
void  GPS_Conversion_Agreement(u8 *gpsbuf);
void CtrGnssOn(void);
//外部关闭GNSS，调用该函数，GNSS关闭，同时控制GNSS状态定时器会关闭
void CtrGnssOFF(void);
//返回gnss epo状态
bool Gnss_ReadStatus(void);
void Gps_NMEA_Hdlr(u8 *multi_nmea, u16 len, void *customizePara);

#endif




