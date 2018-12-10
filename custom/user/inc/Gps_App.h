#ifndef	__GPS_APP_H
#define	__GPS_APP_H
//******************�ļ�����*********************
//#include "include.h"
#include "RadioProtocol.h"
extern u8	GpsOnOffFlag;//GPS���ر�־,0ΪGPS��,1ΪGPS��

typedef struct 		
{
	u8  	Year;		//��	
	u8  	Month;		//��
	u8  	Date;		//��
	u8  	Hour;		//ʱ
	u8  	Minute;		//��
	u8  	Second;		//��
	u8		North;		//1:��γ0:��γ
	double  Latitue;
	u8  	Latitue_D;	//γ��,��
	u8  	Latitue_F;	//γ��,��
	u16 	Latitue_FX;	//γ��,�ֵ�С��,��λΪ0.0001��
	u8		East;		//1:����0:����
	double 	Longitue;	
	u8 		Longitue_D;	//����,��,���180��
	u8  	Longitue_F;	//����,��		
	u16 	Longitue_FX;	//����,�ֵ�С��,��λΪ0.0001��
	double	Speed_D;
	u8  	Speed;		//�ٶ�,��λΪ����/Сʱ
	u8		SpeedX;		//�ٶȵ�С��
	u16 	Course;		//����,��λΪ��
	s16 	High;		//����,��λΪ��
    s16     HighOffset;     //����ƫ��ֵ
	u8  	SatNum;		//��������
	u8		HDOP;		//ˮƽ��������
	u8		Status;		//1:��Ч��λ 0:��Ч��λ
	u8		Error;		//1:GPSģ�����������������  0:ģ������
}GPS_STRUCT;			//GPS��λ���ݽṹ��

#define QCELLOCLEN  15

typedef struct
{
  u8 LatitueLocation[QCELLOCLEN];//γ��
  u8 LongitueLocation[QCELLOCLEN];//����
}CELLULAR_STRUCT;

//gps�˲�
typedef enum
{
    WORKING = 0,
    ENTER_STOP,
    SLEEPING,
    WAKE_UP,
    SWITCHING,
}GPS_WORK_STATUS;

typedef struct 
{
	GPS_WORK_STATUS GPS_work_status;

    u8  Valid;            
	u8	Not_Locate_Cnt;
	u8	Reflash_Flag;
	u8  Speed_Up_Cnt;
	u16 Speed_Down_Cnt;
}TDF_STU_GPS;

//**************��������********************
/*********************************************************************
//��������	:Gps_ReadStaNum(void)
//����		:��ȡGPS�Ķ�λ����
//��ע		:
*********************************************************************/
u8 Gps_ReadStaNum(void);
/*********************************************************************
//��������	:GPS_AdjustRtc(GPS_STRUCT *Position)
//����		:GPSУʱ
//��ע		:
*********************************************************************/
ErrorStatus Gps_AdjustRtc(GPS_STRUCT *Position);
/*********************************************************************
//��������	:GPS_GprmcIsLocation(u8 *pBuffer, u8 BufferLen)
//����		:�ж�GPRMC�����Ƿ�λ
//����		:��ȷ����1�����󷵻�0
//��ע		:
*********************************************************************/
u8 Gps_GprmcIsLocation(u8 *pBuffer, u8 BufferLen);
/*********************************************************************
//��������	:GPS_GpsDataIsOk(u8 *pBuffer, u8 BufferLen)
//����		:�ж�GPS�����Ƿ���ȷ
//����		:��ȷ����1�����󷵻�0
//��ע		:
*********************************************************************/
u8 Gps_DataIsOk(u8 *pBuffer, u8 BufferLen);
/*********************************************************************
//��������	:GPS_GprmcParse(u8 *pBuffer, u8 BufferLen)
//����		:����GPRMC����
//��ע		:
*********************************************************************/
void Gps_GprmcParse(u8 *pBuffer, u8 BufferLen);
/*********************************************************************
//��������	:GPS_GpggaParse(u8 *pBuffer, u8 BufferLen)
//����		:����GPGGA����
//��ע		:
*********************************************************************/
void Gps_GpggaParse(u8 *pBuffer, u8 BufferLen);
/*********************************************************************
//��������	:GPS_CopygPosition(GPS_STRUCT *dest, GPS_STRUCT *src)
//����		:��ȡһ�����µ�gpsλ�����ݣ�������Ч�Ķ�λ����
//��ע		:
*********************************************************************/
void Gps_CopyToPosition(GPS_STRUCT *dest);
void Gps_CopygPosition(GPS_STRUCT *dest);
/*********************************************************************
//��������	:GPS_CopyPosition(GPS_STRUCT *dest, GPS_STRUCT *src)
//����		:����һ�������յ���gpsλ�����ݣ�����һ������Ч�Ķ�λ����
//��ע		:
*********************************************************************/
void Gps_CopyPosition(GPS_STRUCT *dest);
/*********************************************************************
//��������	:GPS_UpdatagPosition(void)
//����		:���±���gPosition
//��ע		:
*********************************************************************/
void Gps_UpdatagPosition(void);
/*********************************************************************
//��������	:GPS_ReadGpsSpeed(void)
//����		:��ȡGPS�ٶ�
//��ע		:��λ��km/h
*********************************************************************/
u8 Gps_ReadSpeed(void);
/*********************************************************************
//��������	:GPS_ReadGpsStatus(void)
//����		:��ȡGPS��״̬
//��ע		:
*********************************************************************/
u8 Gps_ReadStatus(void);
/*********************************************************************
//��������	:GPS_ReadGpsCourse(void)
//����		:��ȡGPS�ķ���
//��ע		:
*********************************************************************/
u16 Gps_ReadCourse(void);
/*********************************************************************
//��������	:GPS_ReadGpsCourseDiv2(void)
//����		:��ȡGPS�ķ��򣨳�����2��
//��ע		:
*********************************************************************/
u8 Gps_ReadCourseDiv2(void);
/*********************************************************************
//��������	:Gps_PowerOnClearPosition(void)
//����		:����ʹ�þ�γ�ȳ�ʼ��Ϊ��
*********************************************************************/
void Gps_PowerOnClearPosition(void);
/*********************************************************************
//��������	:GPS_PowerOnUpdataPosition(void)
//����		:�ϵ���¾�γ��
//��ע		:
*********************************************************************/
void Gps_PowerOnUpdataPosition(void);
/*********************************************************************
//��������	:GPS_SavePositionToFram(void)
//����		:��λ�ñ��浽eeprom��
//��ע		:
*********************************************************************/
void Gps_SavePositionToFram(void);
/*********************************************************************
//��������	:GpsParse_EvTask(void)
//����		:GPS��������
//��ע		:
*********************************************************************/
/*********************************************************************
//��������	:Gps_GetRunFlag
//����		:��ȡȥƯ����ʻ��־ 
//����		:1Ϊ��ʻ��0Ϊͣʻ
//��ע		:
*********************************************************************/
u8 Gps_GetRunFlag(void);
/*********************************************************************
//��������	:Gps_IsInRunning
//����		:�жϵ�ǰ�Ǵ�����ʻ״̬����ͣʻ״̬ 
//����		:1Ϊ��ʻ��0Ϊͣʻ
//��ע		:1�����1��
*********************************************************************/
void Gps_IsInRunning(void);
/*********************************************************************
//��������	:GpsParse_EvTask(void)
//����		:GPS��������
*********************************************************************/
void Gps_EvTask(void);
/*********************************************************************
//��������	:Gps_TimeTask(void)
//����		:GPSʱ������
//��ע		:1�����1��
*********************************************************************/
FunctionalState  Gps_TimeTask(void);
/*********************************************************************
//��������	:Gps_UpdateParam
//����		
//����		:
//���		:
//ʹ����Դ	:
//ȫ�ֱ���	:   
//���ú���	:
//�ж���Դ	:  
//����		:
//��ע		:
*********************************************************************/
void Gps_UpdateParam(void);
u8 Gps_Off_flag(void);
void Gps_Qcellloc_ASCII2(u8 *pBuffer);
//���GPS���ݽ������������
void Gps_ClrParseErr(void);

#endif
