/*************************************************************************
*
* Copyright (c) 2008,深圳市伊爱高新技术开发有限公司
* All rights reserved.
*
* 文件名称 : MileMeter.c
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
#include "include.h"
d64 MileSum;//m
d64 OldMileSum;
Point_Struct OldPoint;
Point_Struct Point;
u8 GpsPosSpeed;
u8 GpsPosSpeedTimeCount;
//d64 Max,Average,TwoPointDistanceTest[N-1];
static u8	GpsMinuteSpeed = 0;//GPS每分钟平均速度
static u16	GpsSpeedSum = 0;//GSP瞬时速度累积和
static u8	MinuteSpeedCount = 0;//平均速度计数
static u32	OldMileTime;//里程统计的上一个位置点的时间
static u32	MileTime;//里程统计的当前位置点的时间
extern GPS_STRUCT	gPosition;//当前有效位置
void MileMeter_GpsPosSpeedCount(void);
extern u8	AccOffGpsControlFlag;//ACC OFF时GPS模块控制标志,0为关,1为开
/********************************************************************
* 名称 : DecimalDegree
* 功能 :
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:
********************************************************************/
void MileMeter_DecimalDegree(Point_Struct *Point,GPS_STRUCT *GpsData)
{

    d64 temp;
	
	temp = GpsData->Longitue_FX/10000.0;
	temp = temp + GpsData->Longitue_F;
	temp = temp/60.0;
	temp = temp + GpsData->Longitue_D;
	Point->LonDegree = temp;
	temp = GpsData->Latitue_FX/10000.0;
	temp = temp + GpsData->Latitue_F;
	temp = temp/60.0;
	temp = temp + GpsData->Latitue_D;
	Point->LatDegree = temp;
	
}
/********************************************************************
* 名称 :
* 功能 :
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:
********************************************************************/

void MileMeter_InitialLonLat(GPS_STRUCT *GpsData)
{
	Point_Struct Point ;
	TIME_T	tt;
	if((OldPoint.LonDegree == 0)&&(OldPoint.LatDegree == 0)) //可能需要根据开关量
	{
		MileMeter_DecimalDegree(&Point,GpsData);
		OldPoint.LonDegree = Point.LonDegree;
		OldPoint.LatDegree = Point.LatDegree;
		tt.year = GpsData->Year;
		tt.month = GpsData->Month;
		tt.day = GpsData->Date;
		tt.hour = GpsData->Hour;
		tt.min = GpsData->Minute;
		tt.sec = GpsData->Second;
		OldMileTime = ConverseGmtime(&tt);
		
	}
	
}
/********************************************************************
* 名称 : CalDistance
* 功能 : 计算两点间的距离
*
* 输入: 两点的经度和纬度，以度为单位
* 输出: 两点之间的距离，以米为单位
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:
********************************************************************/
d64 MileMeter_CalDistance(Point_Struct *Point1,Point_Struct *Point2)
{

	d64 distance,temp,RadLon1,RadLat1,RadLon2,RadLat2;
	u8	flag = 0;
		
	RadLon1 = Point1->LonDegree*PI/180.0;
	RadLat1 = Point1->LatDegree*PI/180.0;
	RadLon2 = Point2->LonDegree*PI/180.0;
	RadLat2 = Point2->LatDegree*PI/180.0;
	
	//纬度或经度变化要超过一定范围,(1分==1852米,推出1米==0.000009度)
	temp = (d64)fabs(Point1->LonDegree-Point2->LonDegree);
	if(temp > 0.000009)
	{
		flag = 1;
	}
	temp = (d64)fabs(Point1->LatDegree-Point2->LatDegree);
	if(temp > 0.000009)
	{
		flag = 1;
	}
	if(1 == flag)
	{
		temp = 2.0-2.0*cos(RadLat1)*cos(RadLat2)*cos(RadLon2-RadLon1)-2.0*sin(RadLat2)*sin(RadLat1);
		if(temp > 0)//保证为正
		{
			distance = REarth*sqrt(temp);
			return distance;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
	
			
}

#if	MILE_TEST
// https://www.cnblogs.com/softfair/p/lat_lon_distance_bearing_new_lat_lon.html
const double Ea = 6378137;     //   赤道半径  
const double Eb = 6356725;     //   极半径 
static void MileMeter_GetJWDB(double GLAT, double GLON, double distance, double angle, double *outBJD, double *outBWD)

{

    double dx = distance * 1000 * sin(angle * PI / 180.0);

    double dy = distance * 1000 * cos(angle * PI / 180.0);



    //double ec = 6356725 + 21412 * (90.0 - GLAT) / 90.0;

    // 21412 是赤道半径与极半径的差

    double ec = Eb + (Ea-Eb) * (90.0 - GLAT) / 90.0;



    double ed = ec * cos(GLAT * PI / 180);

    *outBJD = (dx / ed + GLON * PI / 180.0) * 180.0 / PI;

    *outBWD = (dy / ec + GLAT * PI / 180.0) * 180.0 / PI;

}


#define EARTH_RADIUS            6378.137

// Calculate radian
static d64 MileMeter_Radian(d64 d)  
{  
     return d * PI / 180.0;
}  

d64 MileMeter_CalDistance1(Point_Struct *Point1,Point_Struct *Point2)
{  
     d64 radLat1 = MileMeter_Radian(Point1->LatDegree);  
     d64 radLat2 = MileMeter_Radian(Point2->LatDegree);  
     d64 a = radLat1 - radLat2;  
     d64 b = MileMeter_Radian(Point1->LonDegree) - MileMeter_Radian(Point2->LonDegree);  
	 
     d64 dst = 2 * asin((sqrt(pow(sin(a / 2), 2) + cos(radLat1) * cos(radLat2) * pow(sin(b / 2), 2) )));  
     dst = dst * EARTH_RADIUS;  
     dst= round(dst * 10000) / 10000;  
     return dst;  
} 

static double HaverSin(double theta)
{
   double v = sin(theta / 2);
   return v * v;
}
static double ConvertDegreesToRadians(double degrees)
{
    return degrees * PI / 180;
}

static double ConvertRadiansToDegrees(double radian)
{
    return radian * 180.0 / PI;
}

/// <summary>
/// 给定的经度1，纬度1；经度2，纬度2. 计算2个经纬度之间的距离。
/// </summary>
/// <param name="lat1">经度1</param>
/// <param name="lon1">纬度1</param>
/// <param name="lat2">经度2</param>
/// <param name="lon2">纬度2</param>
/// <returns>距离（公里、千米）</returns>
static double MileMeter_CalDistance2(Point_Struct *Point1,Point_Struct *Point2)
{
   //用haversine公式计算球面两点间的距离。
   //经纬度转换成弧度
   double lat1 = ConvertDegreesToRadians(Point1->LatDegree);
   double lon1 = ConvertDegreesToRadians(Point1->LonDegree);
   double lat2 = ConvertDegreesToRadians(Point2->LatDegree);
   double lon2 = ConvertDegreesToRadians(Point2->LonDegree);

   //差值
   double vLon = fabs(lon1 - lon2);
   double vLat = fabs(lat1 - lat2);

   //h is the great circle distance in radians, great circle就是一个球体上的切面，它的圆心即是球心的一个周长最大的圆。
   double h = HaverSin(vLat) + cos(lat1) * cos(lat2) * HaverSin(vLon);

   double distance = 2 * EARTH_RADIUS * asin(sqrt(h));

   return distance;
}

void MileMeter_Test(void)
{  
	d64 dst,dst1,dst2 ;
	double outBJD;
	double outBWD;
	Point_Struct p1 = {116.41615,39.90744};  
	Point_Struct p2 = {116.30746,39.90744};
	
	// test: float as arguments
	dst = MileMeter_CalDistance(&p1, &p2); 
	dst1 = MileMeter_CalDistance1(&p1, &p2);   
	dst2 = MileMeter_CalDistance2(&p1, &p2);
	MileMeter_GetJWDB(39.90744, 116.41615, 98.5623, 25.6, &outBJD, &outBWD);

	APP_DEBUG("dst = %0.4fm,dst1 = %0.4fkm,dst2 = %0.4fkm,outBJD = %0.4f,outBWD = %0.4f,\r\n", dst,dst1,dst2,outBJD, outBWD);
} 

#endif


/********************************************************************
* 名称 : GpsMile_UpdatePram(void)
* 功能 : 更新GPS总里程
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:
********************************************************************/
void GpsMile_UpdatePram(void)
{
	u8	TmpData[4];
	//u8	Buffer[20];
	u8	PramLen;
	//u8	length;
	u32	Tmp = 0;

	
	PramLen = EepromPram_ReadPram(E2_CAR_TOTAL_MILE_ID, TmpData); //读取总里程
	//检查校验字节是否正确
	if(E2_CAR_TOTAL_MILE_LEN == PramLen)//校验和正确
	{
		Tmp = 0;
		Tmp |= TmpData[0] << 24;					//高字节
		Tmp |= TmpData[1] << 16;		//次高字节
		Tmp |= TmpData[2] << 8;		//
		Tmp |= TmpData[3];		//低
	}
	else
	{
		//读历史轨迹中的里程数
		Tmp = 0;
	}
	MileSum = (d64)Tmp * 10.0;//铁电存储的数据是以10m为单位的里程数
}
/********************************************************************
* 名称 : GpsMile_SetPram(u32 TmpMilege)
* 功能 : 设置GPS总里程
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:TmpMilege单位为0.1公里,调用完此函数后需要紧接着调用更新gps里程和脉冲里程函数
********************************************************************/
void GpsMile_SetPram(u32 TmpMilege)
{
	u8 	TmpData[4]={0};
        u32     MileSum = 0;
        
	MileSum = TmpMilege * 10;
	TmpData[3] = MileSum;		//低字节,dxl,2013.11.20之前使用的是TmpMilege,有误
	TmpData[2] = MileSum>>8;	//中字节
	TmpData[1] = MileSum>>16;	//中字节
	TmpData[0] = MileSum>>24;	//高字节
	//FRAM_BufferWrite(FRAM_MILEAGE_ADDR, TmpData, FRAM_MILEAGE_LEN); //存储总里程
    EepromPram_WritePram(E2_CAR_TOTAL_MILE_ID,TmpData,4);    
    //更新相应变量
    GpsMile_UpdatePram();
    ////更新下一次定距汇报的里程/////////////
    //Report_UpdataNextReportMile();
}
/********************************************************************
* 名称 :
* 功能 :
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:
********************************************************************/
FunctionalState GpsMile_TimeTask(void)
{

	d64 TwoPointDistance;
	u8	RunState;
	u8	TmpData[5];
	u32	Mile;
	TIME_T	tt;
	static  u8	count = 0;
	static  u16  saveMileCount=0;//写入里程时间计算，每十分钟写入一次到文件系统，每1h小时写入到安全数据区
	
	//计算每分钟平均速度
	MileMeter_MinuteSpeed();
	
	//读行驶标志
	if(0 == AccOffGpsControlFlag)//ACC OFF时GPS模块关闭
    {
	    //RunState = SpeedMonitor_GetCurRunStatus();
    }
    else//GPS模块始终是打开的
    {
        RunState = Gps_GetRunFlag();
    }
	count++;
	saveMileCount++;
	if(count >= 3)//3秒钟计算一次里程
	{
	
		count = 0;
	 	//如果ACC打开且处于行驶状态
        	if(1 == RunState)
        	{
			MileMeter_InitialLonLat(&gPosition);
			MileMeter_DecimalDegree(&Point, &gPosition);
			TwoPointDistance = MileMeter_CalDistance(&OldPoint, &Point);
			tt.year = gPosition.Year;
			tt.month = gPosition.Month;
			tt.day = gPosition.Date;
			tt.hour = gPosition.Hour;
			tt.min = gPosition.Minute;
			tt.sec = gPosition.Second;
			MileTime = ConverseGmtime(&tt);
			if(TwoPointDistance > StillFloatDistanceAverageStandard)
			{
				
				if(TwoPointDistance > 55*(MileTime-OldMileTime))//平均时速小于200公里/小时
				{
				
				}
				else
				{
					MileSum += TwoPointDistance;//考虑飘移
					
					if(saveMileCount>600)//10分钟存一次
						{
							saveMileCount=0;
							//把里程值存入铁电
							Mile = (u32)(MileSum/10.0);//为了与脉冲里程保持一致,所以除以10,单位是0.01公里
							TmpData[3] = Mile;	//低字节
							TmpData[2] = Mile>>8;	//中字节
							TmpData[1] = Mile>>16;	//中字节
							TmpData[0] = Mile>>24;	//高字节

							EepromPram_WritePram(E2_CAR_TOTAL_MILE_ID,TmpData,4);    
							//FRAM_BufferWrite(FRAM_MILEAGE_ADDR, TmpData, FRAM_MILEAGE_LEN); //存储总里程
						}
				}
				OldPoint.LonDegree = Point.LonDegree;
				OldPoint.LatDegree = Point.LatDegree;
				OldMileTime = MileTime;
			}
		}
	}
	return ENABLE;
}
	
/********************************************************************
* 名称 :
* 功能 :
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:
********************************************************************/
void MileMeter_GpsPosSpeedCount(void)
{
	s32 MileSubTemp;
	
	GpsPosSpeedTimeCount ++;
	if(GpsPosSpeedTimeCount >= 20)
	{
		GpsPosSpeedTimeCount = 0;
		MileSubTemp = (s32)(MileSum - OldMileSum);
		if(MileSubTemp > 0)
		{
        		GpsPosSpeed = (u8)(MileSubTemp*6/100);
	    		OldMileSum = MileSum;
		}
	}


}
/********************************************************************
* 名称 :
* 功能 :
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:
********************************************************************/
void MileMeter_MinuteSpeed(void)
{
	MinuteSpeedCount++;
	GpsSpeedSum += Gps_ReadSpeed();
	if(MinuteSpeedCount >= 60)
	{
		MinuteSpeedCount = 0;
		GpsMinuteSpeed = GpsSpeedSum/60;
		GpsSpeedSum = 0;
	}
}
/********************************************************************
* 名称 :MileMeter_GetMinuteSpeed
* 功能 :获取GPS每分钟平均速度
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:
********************************************************************/
u8 MileMeter_GetMinuteSpeed(void)
{
	return  GpsMinuteSpeed;
}
/********************************************************************
* 名称 :ReadMileSum
* 功能 :读gps累计行驶里程
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:MileSum单位为米
********************************************************************/
u32 ReadMileSum(void)
{
	return (u32)MileSum/100;//单位为0.1公里
}
/********************************************************************
* 名称 :
* 功能 :
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:单位为0.01公里
********************************************************************/
u32 MileMeter_GetTotalMile(void)
{
	return (u32)MileSum/10;//MileSum单位为米
}
/********************************************************************
* 名称 : MileMeter_ClearOldPoint
* 功能 : 上一个坐标点清0
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:
********************************************************************/
void MileMeter_ClearOldPoint(void)
{
		OldPoint.LonDegree = 0;
		OldPoint.LatDegree = 0;
}
/********************************************************************
* 名称 :
* 功能 :
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:
********************************************************************/
u8 ReadGpsPosSpeed(void)
{
	return GpsPosSpeed;
}
#if Test//

/********************************************************************
* 名称 :
* 功能 :
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:
********************************************************************/

void MileMeter_DecimalDegreeTest(Point_Struct *Point,Test_Pos_Data *TestGpsData)
{

    d64 temp;
	
	temp = TestGpsData->LongitueMF/10000.0;
	temp = temp + TestGpsData->LongitueM;
	temp = temp/60.0;
	temp = temp + TestGpsData->LongitueD;
	Point->LonDegree = temp;
	temp = TestGpsData->LatitueMF/10000.0;
	temp = temp + TestGpsData->LatitueM;
	temp = temp/60.0;
	temp = temp + TestGpsData->LatitueD;
	Point->LatDegree = temp;
	
}


/*漂移测试*/

/********************************************************************
* 名称 :
* 功能 :
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:
********************************************************************/

void FloatTest(d64 *Max,d64 *Average,Point_Struct Point[],u8 Num)
{

	d64 MaxTemp = 0,AverageTemp = 0,DistanceTemp = 0;
	u8 i;
		
	for(i=0;i<Num-1;i++)
	{
	
	    DistanceTemp = MileMeter_CalDistance(&Point[i],&Point[i+1]);
		TwoPointDistanceTest[i] = DistanceTemp;
	    AverageTemp += DistanceTemp;
		if(MaxTemp < DistanceTemp)
			MaxTemp = DistanceTemp;
		
	}
	*Max = MaxTemp;
	*Average = AverageTemp/(Num-1);
//	*Average = (AverageTemp-MaxTemp)/(Num-2);
		
}

/********************************************************************
* 名称 :
* 功能 :
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:
********************************************************************/

void FloatResult(void)
{
	u8 i;
	
	Test_Pos_Data Data[N]=
	{
		{114,05,5420,22,33,5930},

		{114,05,5440,22,33,5870},
		
		{114,05,5430,22,33,5800},

		{114,05,5420,22,33,5760},

		{114,05,5440,22,33,5770},

		{114,05,5450,22,33,5800},

		{114,05,5460,22,33,5840},

		{114,05,5460,22,33,5850},

		{114,05,5450,22,33,5840},

		{114,05,5450,22,33,5850},

		{114,05,5440,22,33,5870},

		{114,05,5420,22,33,5880},

		{114,05,5400,22,33,5890},

		{114,05,5380,22,33,5900},

		{114,05,5370,22,33,5910},

		{114,05,5360,22,33,5930},

		{114,05,5350,22,33,5940},

		{114,05,5340,22,33,5950},

		{114,05,5330,22,33,5970},

		{114,05,5340,22,33,6010},

		{114,05,5370,22,33,6010}
	};

	Point_Struct PointData[N];
	for(i=0;i<N;i++)
	MileMeter_DecimalDegreeTest(&PointData[i],&Data[i]);
    FloatTest(&Max,&Average,PointData,N);

}


/*里程累加测试*/

/********************************************************************
* 名称 :
* 功能 :
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:
********************************************************************/
void MileMeter_DistanceSumTest(Test_Pos_Data *TestGpsData)
{
    Point_Struct Point;
	d64 TwoPointDistance;
	
	MileMeter_DecimalDegreeTest(&Point,TestGpsData);
	TwoPointDistance = MileMeter_CalDistance(&OldPoint, &Point);
//	if(TwoPointDistance > StillFloatDistance)
	MileSum += TwoPointDistance;//考虑飘移
	OldPoint.LonDegree = Point.LonDegree;
	OldPoint.LatDegree = Point.LatDegree;
	

}


/********************************************************************
* 名称 :
* 功能 :
*
* 输入:
* 输出:
*
* 全局变量:
* 调用函数:
*
* 中断资源:
*
* 备注:
********************************************************************/

void SimulateDistanceSumTest(void)
{
	u8 i ;
	Test_Pos_Data Data[N]=
	{
		{114,05,5430,22,33,5780},
		{114,05,5420,22,33,5750},

		{114,05,5430,22,33,5750},

		{114,05,5460,22,33,5760},

		{114,05,5460,22,33,5710},

		{114,05,5410,22,33,5620},

		{114,05,5380,22,33,5580},

		{114,05,5380,22,33,5580},
	
		{114,05,5390,22,33,5580},

		{114,05,5440,22,33,5600}
		
	};
	
	MileMeter_DecimalDegreeTest(&OldPoint,&Data[0]);
	for(i = 1;i < N;i++)
    MileMeter_DistanceSumTest(&Data[i]);
	
}

/*
//每隔6s同一位置漂移
{
	{114,05,5430,22,33,5780},

	{114,05,5420,22,33,5750},

	{114,05,5430,22,33,5750},

	{114,05,5460,22,33,5760},

	{114,05,5460,22,33,5710},

	{114,05,5410,22,33,5620},

	{114,05,5380,22,33,5580},

	{114,05,5380,22,33,5580},
	
	{114,05,5390,22,33,5580},

	{114,05,5440,22,33,5600}
}
*/
//每隔3s同一位置漂移

/*
	{
		{114,05,5420,22,33,5930},

		{114,05,5440,22,33,5870},
		
		{114,05,5430,22,33,5800},

		{114,05,5420,22,33,5760},

		{114,05,5440,22,33,5770},

		{114,05,5450,22,33,5800},

		{114,05,5460,22,33,5840},

		{114,05,5460,22,33,5840},

		{114,05,5450,22,33,5840},

		{114,05,5450,22,33,5850},

		{114,05,5440,22,33,5870},

		{114,05,5420,22,33,5880},

		{114,05,5400,22,33,5890},

		{114,05,5380,22,33,5900},

		{114,05,5370,22,33,5910},

		{114,05,5360,22,33,5930},

		{114,05,5350,22,33,5940},

		{114,05,5340,22,33,5950},

		{114,05,5330,22,33,5970},

		{114,05,5340,22,33,6010},

		{114,05,5370,22,33,6010}
	}
*/
#endif



















