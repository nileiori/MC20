/*************************************************************************
*
* Copyright (c) 2008,深圳市伊爱高新技术开发有限公司
* All rights reserved.
*
* 文件名称 : MileMeter.h
* 功能     : 
*
* 当前版本 : 
* 开发者   : 
* 修改时间 : 
*
* 历史版本 : 
* 开发者   : 
* 完成时间 : 
* 
* 备注 :
*************************************************************************/

#ifndef _MileMeter_H
#define _MileMeter_H

#include <math.h>

#define REarth 6371004  //m
#define PI 3.14159265358979323846 
#define StillFloatDistanceMax 11.5//m
#define StillFloatDistanceAverage 4.80//m
#define StillFloatDistanceAverageExceptMax 4.00//m
#define StillFloatDistanceAverageStandard 3.12//m
 
#define Test 0
#define	MILE_TEST	0
typedef double d64;
typedef struct
{
	d64 LonDegree;
	d64 LatDegree;
} Point_Struct;

#if Test
#define N   21


typedef struct
{

	u8  LatitueD; //度
	u8  LatitueM; //分
	u16 LatitueMF; //分小数部分,单位为0.0001分
	u16 LongitueD;
	u8  LongitueM; 
	u16 LongitueMF; //经度分的小数部分,单位为0.0001分

} Test_Pos_Data;
d64 Max,Average,TwoPointDistanceTest[N-1];

void FloatResult(void);
void MileMeter_DistanceSumTest(Test_Pos_Data *TestGpsData);
void SimulateDistanceSumTest(void);
#endif

FunctionalState GpsMile_TimeTask(void);
u32 ReadMileSum(void);
u32 MileMeter_GetTotalMile(void);
u8 ReadGpsPosSpeed(void);
void MileMeter_MinuteSpeed(void);
u8 MileMeter_GetMinuteSpeed(void);
/********************************************************************
* 名称 : GpsMile_UpdatePram(void)
* 功能 : 更新GPS总里程
* 备注: 
********************************************************************/
void GpsMile_UpdatePram(void);
/********************************************************************
* 名称 : GpsMile_SetPram(u32 TmpMilege)
* 功能 : 设置GPS总里程
* 备注: 
********************************************************************/
void GpsMile_SetPram(u32 TmpMilege);
/********************************************************************
* 名称 : MileMeter_ClearOldPoint
* 功能 : 上一个坐标点清0
* 备注: 
********************************************************************/
void MileMeter_ClearOldPoint(void);

#if	MILE_TEST
void MileMeter_Test(void);
#endif

#endif
















