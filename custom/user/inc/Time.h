#ifndef __TIME_H
#define __TIME_H
//时间结构体类型
typedef struct
{
    s8 year;    //年:00~99
    s8 month;   //月:1~12
    s8 day;     //日:1~31
    s8 hour;    //时:0~23
    s8 min;     //分:0~59
    s8 sec;     //秒:0~59
}TIME_T;

#define START_YEAR      15//行标检测时需要放开，否则影响校时
#define END_YEAR        99
//时间宏定义
#define MIN_SECOND      60
#define HOUR_SECOND     0xE10
#define DAY_SECOND      0x15180
#define YEAR_SECOND     0x1E13380

#include "ql_time.h"

//**********函数声明**********
void SetRtc(ST_Time* dateTime);
void ReadRtc(void);
u8 LeapYear(u8 year);
u16 YearDay(u8 yy,s8 mm,s8 dd);
u32 ConverseGmtime(TIME_T *tt);
void Gmtime(TIME_T *tt, u32 counter);
void RTC_Configuration(void);
void Num2NumStr(u32 Num, u8 *NumStr);
void RTC_GetCurTime(TIME_T *tt);
ErrorStatus CheckTimeStruct(TIME_T *tt);
void ReadTime(void);
/*************************************************************
** 函数名称: Public_CheckTimeIsInRange
** 功能描述: 判断指定时间是否在指定范围内
** 入口参数: curTime指定时间,startTime开始时间,endTime结束时间
** 出口参数: 
** 返回参数: 1:在范围内，0:不在范围内 
** 全局变量: 无
** 调用模块: 
*************************************************************/
unsigned char Public_CheckTimeIsInRange(unsigned long curTime,unsigned long startTime,unsigned long endTime);

#endif

