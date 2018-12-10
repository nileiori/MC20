/********************************************************************
//版权说明	:Shenzhen E-EYE Co. Ltd., Copyright 2009- All Rights Reserved.
//文件名称	:Gps_App.c		
//功能		:GPS模块
//版本号	:
//开发人	:dxl
//开发时间	:2011.8
//修改者	:
//修改时间	:
//修改简要说明	:
//备注		:
***********************************************************************/
/*************************修改记录*************************/
//***********************创建(用于八厅分体机)*******************
//2011.8.25--1.整理GPS模块,将GPS模块分成3个部分：
//-----------GPS_RxIsr(void)：负责数据接收
//-----------GpsParse_EvTask(void):负责协议解析
//-----------Gps_TimeTask(void):负责模块管理
//20118.25--2.GPS模块稳定性措施：
//-----------(1)程序上电复位,GPS模块也上电复位,需要先关再开
//-----------(2)校验出错或漂移次数达到一定次数时,置位出错标志,对GPS模块复位
//--------------复位:模块先关闭5秒,后再打开,只有ACC ON时才会检查错误标志
//-----------(3)ACC OFF时,关GPS模块,每小时开1分钟
//-----------(4)连续40秒没有收到数据,置位出错标志
//-----------(5)漂移连续10次,置位出错标志
//-----------(6)校验出错连续10次,置位出错标志
//2011.8.25--3.去漂移的方法：
//-----------(1)纬度方向,距离除以时间>=250公里/小时,认为是漂移
//-----------(2)经度方向,距离除以时间>=250公里/小时,认为是漂移
//-----------(3)速度向上增大时,速度变化量/时间>=20海里/小时认为是漂移
//-----------(4)虽然导航,但星数小于3或水平精度因子大于等于10,认为是漂移
//**********************修改*********************
//2012.6.5--4.去漂移方法改为：
//-----------(1)纬度方向,距离除以时间>=250公里/小时,认为是漂移
//-----------(2)经度方向,距离除以时间>=250公里/小时,认为是漂移
//-----------(3)速度向上增大时,速度变化量/时间>=9海里/小时认为是漂移
//-----------(4)增加去漂移使能开关，连续10次2D导航后，开启去漂移，连续10次漂移后，关闭去漂移；
//****************文件包含**************************
#include "include.h"
#include "report.h"
//***************变量定义***********************
static u16	GprmcVerifyErrorCount = 0;//GPRMC校验出错计数
static u16	GpggaVerifyErrorCount = 0;//GPGGA校验出错计数
static u16	GprmcParseErrorCount = 0;//GPRMC解析出错计数
static u16	GpggaParseErrorCount = 0;//GPGGA解析出错计数
static u16	GpsParseEnterErrorCount = 0;//没有进入解析函数计数
static u8   GnssStatus = 0;
static u8	StopExcursionEnable = 0;//车辆停止时去漂移使能开关,0为不使能,1为使能
static u8   GpsRunFlag = 0;
static TDF_STU_GPS STU_GPS;
u32	gPositionTime = 0;//gPosition有效时的时间 
u32	PositionTime = 0;//Position有效时的时间 

u8	LastLocationFlag = 0;//上一次定位状态
u8	LocationFlag = 0;//当前定位状态
//static u16	AccOffCount = 0;
//static u16	AccOnCount = 0; BY WYF
u8	LastLocationSpeed = 0;//上一次定位的速度 
GPS_STRUCT	gPosition;//当前有效位置
u8	GpsOnOffFlag = 1;//GPS开关标志,0为GPS关,1为GPS开
u8      GpsInitFlag=0;//GPS初始化是否完成，liamtsen 2017-05-02
GPS_STRUCT	 Position;//临时解析用
CELLULAR_STRUCT  Qcellloc;//基站位置信息；
//*****************外部变量********************
u8	AccOffGpsControlFlag=1;//ACC OFF时GPS模块控制标志,0为关,1为开
extern u8	GprmcBuffer[];//存放GPRMC数据
extern u8	GprmcBufferBusyFlag;//GprmcBuffer缓冲忙标志
extern u8	GpggaBuffer[];//存放GPGGA数据
extern u8	GpggaBufferBusyFlag;//GpggaBuffer缓冲忙标志
extern u8	GpgsvBuffer[];//存放GPGGA数据
extern u8	GpgsvBufferBusyFlag;//GpggaBuffer缓冲忙标志
//****************函数定义*********************
/*********************************************************************
//函数名称	:Gps_ReadStaNum(void)
//功能		:获取GPS的定位星数
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u8 Gps_ReadStaNum(void)
{
	return gPosition.SatNum;
}
/*********************************************************************
//函数名称	:GPS_AdjustRtc(GPS_STRUCT *Position)
//功能		:GPS校时
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
ErrorStatus Gps_AdjustRtc(GPS_STRUCT *Position)
{
	
	ST_Time time;

	//TIME_T time;
	//u32 timecount;
	
	time.year = Position->Year;
	time.year += 2000;
	time.month = Position->Month;
	time.day = Position->Date;
	time.hour = Position->Hour;
	time.minute = Position->Minute;
	time.second = Position->Second;
	time.timezone = 32;
	/*if(SUCCESS == CheckTimeStruct(&time))
	{	
		timecount = ConverseGmtime(&time);
		timecount += 8*3600;
		Gmtime(&time, timecount);
		if(SUCCESS == CheckTimeStruct(&time))
		{
			SetRtc(&time);
			return SUCCESS;		
		}
		else
		{
			return ERROR;
		}
	}
	else
	{
		return ERROR;
	}*/

	Ql_SetLocalTime(&time);

	return SUCCESS; 
}	
/*********************************************************************
//函数名称	:GPS_GpsDataIsOk(u8 *pBuffer, u8 BufferLen)
//功能		:判断GPS数据是否正确
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:正确返回1，错误返回0
//备注		:
*********************************************************************/
u8 Gps_DataIsOk(u8 *pBuffer, u8 BufferLen)
{
	u8	i;
	u8	j;
	u8	temp;
	u8	sum = 0;
	u8	count = 0;
	u8	High;
	u8	Low;
	u8	verify;

	for(i=1; i<BufferLen; i++)//第1个是起始符'$'
	{
		temp = *(pBuffer+i);
		if((0x0a == temp)||(0x0d == temp))
		{
			break;//跳出循环
		}
		else if('*' == temp)
		{
			j = i;	
			break;
		}
		else
		{
			sum ^= temp;//异或？
			if(',' == temp)
			{
				count++;
			}
		}
	}
	High = *(pBuffer+j+1);
	Low = *(pBuffer+j+2);
	if((High >= '0')&&(High <= '9'))
	{
		High = High - 0x30;
	}
	else if((High >= 'A')&&(High <= 'F'))
	{
		High = High - 0x37;
	}
	else
	{
		return 0;
	}
	if((Low >= '0')&&(Low <= '9'))
	{
		Low = Low - 0x30;
	}
	else if((Low >= 'A')&&(Low <= 'F'))
	{
		Low = Low - 0x37;
	}
	else
	{
		return 0;
	}
	verify = (High << 4)|Low;
	if(verify == sum)
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}
/*********************************************************************
//函数名称	:GPS_GprmcIsLocation(u8 *pBuffer, u8 BufferLen)
//功能		:判断GPRMC数据是否定位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:正确返回1，错误返回0
//备注		:
*********************************************************************/
u8 Gps_GprmcIsLocation(u8 *pBuffer, u8 BufferLen)
{
	u8	i;
	u8	temp;
	u8	count = 0;

	for(i=0; i<BufferLen; i++)
	{
		temp = *(pBuffer+i);
		if(',' == temp)
		{
			count++;
		}
		else if('A' == temp)
		{
			if(2 == count)
			{
				return 1;
			}	
		}
	}
	
	return 0;	
}

/*********************************************************************
//函数名称	:GPS_GprmcParse(u8 *pBuffer, u8 BufferLen)
//功能		:解析GPRMC数据
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void Gps_GprmcParse(u8 *pBuffer, u8 BufferLen)
{
	u8	i = 0;
	u8	j = 0;
	u8	k = 0;
	u8	l = 0;
	u8	m = 0;
	s8	z = 0;
	u8	temp;
	u8	flag = 0;
	u8	count = 0;
	u16	temp2;
	u16	temp3;

	for(i=0; i<BufferLen; i++)
	{
		temp = *(pBuffer+i);
		if((0x0a == temp)||(0x0d == temp))
		{
			break;
		}
		else if('.' == temp)
		{
			l = i;//.号的位置
		}
		else if(',' == temp)
		{
			k = i;//当前逗号的位置
			count++;
			switch(count)
			{
				case 2://第2个逗号,解析时分秒
					{
						if(7 == (l-j))
						{
							//解析时间,小数点后的秒抛弃
							Position.Hour = (*(pBuffer+j+1)-0x30)*10+(*(pBuffer+j+2)-0x30);
							Position.Minute = (*(pBuffer+j+3)-0x30)*10+(*(pBuffer+j+4)-0x30);
							Position.Second = (*(pBuffer+j+5)-0x30)*10+(*(pBuffer+j+6)-0x30);	
						}
						else if(1 == (k-j))//没有数据
						{
							
						}
						else
						{
							flag = 1;//错误
						}
					
						break;
					}
				case 3://第3个逗号,解析有效标志
					{
						if(2 == (k-j))
						{
                                                    	//解析有效标志
							if('A' == *(pBuffer+j+1))
							{
								Position.Status = 1;
							}
							else if('V' == *(pBuffer+j+1))
							{
								Position.Status = 0;
							}
							else
							{
								flag = 1;//错误
							}
							
						}
						else if(1 == (k-j))//没有数据
						{
							
						}
						else
						{
							flag = 1;
						}
						break;
					}
				case 4://第4个逗号,解析纬度值，精确到0.0001分，即取小数点后四位
					{
						if((k > l)&&(5 == l-j))
						{
                                                    	//解析纬度值
							Position.Latitue_D = (*(pBuffer+j+1)-0x30)*10+(*(pBuffer+j+2)-0x30);
							Position.Latitue_F = (*(pBuffer+j+3)-0x30)*10+(*(pBuffer+j+4)-0x30);
							temp2 = 0;
                                                        //以下这段代码在分的小数位不足4位时解析有问题,dxl,2013.6.27
							for(m=l+1; m<l+5; m++)
							{
								temp3 = (*(pBuffer+m)-0x30);
								for(z=0; z<l+4-m; z++)
								{
									temp3 = temp3 * 10;	
								}
								temp2 += temp3;
							}
							Position.Latitue_FX = temp2;
                                                        
							
						}
						else if(1 == (k-j))//没有数据
						{
							
						}
						else
						{
							flag = 1;
						}
						break;
					}
				case 5://第5个逗号,解析纬度方向
					{
						if(2 == (k-j))
						{
                                                    	//解析纬度方向
							if('N' == *(pBuffer+j+1))
							{
								Position.North = 1;
								
							}
							else if('S' == *(pBuffer+j+1))
							{
								Position.North = 0;
								
							}
							else
							{
								flag = 1;
							}
						}
						else if(1 == (k-j))//没有数据
						{
							
						}
						else
						{
							flag = 1;
						}
						break;
					}
				case 6://第6个逗号,解析经度值，精确到0.0001分
					{
						if((k > l)&&(6 == l-j))
						{
                                                    	//解析经度值
							Position.Longitue_D = (*(pBuffer+j+1)-0x30)*100+(*(pBuffer+j+2)-0x30)*10+*(pBuffer+j+3)-0x30;
							Position.Longitue_F = (*(pBuffer+j+4)-0x30)*10+(*(pBuffer+j+5)-0x30);
							temp2 = 0;
							for(m=l+1; m<l+5; m++)
							{
								temp3 = (*(pBuffer+m)-0x30);
								for(z=0; z<l+4-m; z++)
								{
									temp3 = temp3 * 10;	
								}
								temp2 += temp3;
							}
							Position.Longitue_FX = temp2;
						}
						else if(1 == (k-j))//没有数据
						{
							
						}
						else
						{
							flag = 1;
						}
						break;
					}
				case 7://第7个逗号,解析经度方向
					{
						if(2 == (k-j))
						{
                                                    	//解析经度方向
							if('E' == *(pBuffer+j+1))
							{
								Position.East = 1;
								
							}
							else if('W' == *(pBuffer+j+1))
							{
								Position.East = 0;
								
							}
							else
							{
								flag = 1;
							}
						}
						else if(1 == (k-j))//没有数据
						{
							
						}
						else
						{
							flag = 1;
						}
						break;
					}
				case 8://第8个逗号,解析速度，精确到整数，小数部分抛弃
					{
						if((k>l)&&(l>j))
						{
							temp2 = 0;
							for(m=j+1; m<l; m++)
							{
								temp3 = (*(pBuffer+m)-0x30);
								for(z=0; z<l-1-m; z++)
								{
									temp3 = temp3 * 10;	
								}
								temp2 += temp3;
							}
							Position.Speed = temp2;
                                                        Position.SpeedX = *(pBuffer+l+1)-0x30;
                                                        if(Position.SpeedX >= 10)
                                                        {
                                                              //解析错误
                                                              Position.SpeedX = 0;
                                                        }
						}
						else if(1 == (k-j))//没有数据
						{
							
						}
						else
						{
							flag = 1;
						}
						break;
					}
				case 9://第9个逗号,解析方向，精确到整数，小数部分抛弃
					{
						if((k>l)&&(l>j))
						{
							temp2 = 0;
							for(m=j+1; m<l; m++)
							{
								temp3 = (*(pBuffer+m)-0x30);
								for(z=0; z<l-1-m; z++)
								{
									temp3 = temp3 * 10;	
								}
								temp2 += temp3;
							}
							Position.Course = temp2;		
						}
						else if(1 == (k-j))//没有数据
						{
							Position.Course = 0;
						}
						else
						{
							flag = 1;
						}
						break;
					}
				case 10://第10个逗号,解析年月日
					{
						if(7 == (k-j))
						{
							//解析
							Position.Date = (*(pBuffer+j+1)-0x30)*10+(*(pBuffer+j+2)-0x30);
							Position.Month = (*(pBuffer+j+3)-0x30)*10+(*(pBuffer+j+4)-0x30);
							Position.Year = (*(pBuffer+j+5)-0x30)*10+(*(pBuffer+j+6)-0x30);
                                                        //Position.Hour += 8;//加8小时,行标检测不能屏蔽这段，因为行标检测时不校时。
                                                        //if(Position.Hour >= 24)//实际使用时加8小时是在校时函数里
                                                        //{
                                                               //Position.Hour -= 24;
                                                               //Position.Date++;
                                                       // }
						}
						else if(1 == (k-j))//没有数据
						{
							
						}
						else
						{
							flag = 1;
						}
						break;
					}
				default : break;
				
			}
			j = i;//上一个逗号的位置
		}
		if(1 == flag)//如果检测到错误，则提前跳出
		{
			break;
		}
		
	}
	if(1 == flag)
	{
		GprmcParseErrorCount++;
	}
	else
	{
		GprmcParseErrorCount = 0;	
	}
}
/*********************************************************************
//函数名称	:GPS_GpggaParse(u8 *pBuffer, u8 BufferLen)
//功能		:解析GPGGA数据
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void Gps_GpggaParse(u8 *pBuffer, u8 BufferLen)
{
	u8	i = 0;
	u8	j = 0;
	u8	k = 0;
	u8	l = 0;
	u8	m = 0;
	s8	z = 0;
	u8	temp;
	s16	temp2;
	s16	temp3;
	u8	flag = 0;
	u8	count = 0;
	for(i=0; i<BufferLen; i++)
	{
		temp = *(pBuffer+i);
		if((0x0a == temp)||(0x0d == temp))
		{
			break;
		}
		else if('.' == temp)
		{
			l = i;
		}
		else if(',' == temp)
		{
			k = i;
			count++;
			switch(count)
			{
			
				case 8://第8个逗号，解析星数
					{
						if(2 == (k-j))//只有一位数
						{
							Position.SatNum = (*(pBuffer+j+1)-0x30);
						}
						else if(3 == (k-j))//两位数
						{
							Position.SatNum = (*(pBuffer+j+1)-0x30)*10+(*(pBuffer+j+2)-0x30);
						}
						else if(1 == (k-j))//没有数据
						{
							
						}
						else
						{
							flag = 1;
						}
						break;
					}
				case 9://第9个逗号，解析水平精度因子，精确到整数，小数抛弃
					{
						temp2 = 0;
						for(m=j+1; m<l; m++)
						{
							temp3 = (*(pBuffer+m)-0x30);
							for(z=0; z<l-m-1; z++)
							{
								temp3 = temp3 * 10;	
							}
							temp2 += temp3;
						}
						Position.HDOP = temp2;
						break;
					}
				case 10://第10个逗号，解析天线离海平面的高度，精确到整数，小数抛弃
					{
						temp2 = 0;
						if(*(pBuffer+j+1) == '-')//负海拔
						{
							for(m=j+2; m<l; m++)
							{
								temp3 = (*(pBuffer+m)-0x30);
								for(z=0; z<l-m-1; z++)
								{
									temp3 = temp3 * 10;	
								}
								temp2 += temp3;
							}
							Position.High = -temp2;
						}
						else//正海拔
						{
							for(m=j+1; m<l; m++)
							{
								temp3 = (*(pBuffer+m)-0x30);
								for(z=0; z<l-m-1; z++)
								{
									temp3 = temp3 * 10;	
								}
								temp2 += temp3;
							}
							Position.High = temp2;
						}
						break;
					}
                                case 12://解析高程误差值
                                  {
                                        temp2 = 0;
					if(*(pBuffer+j+1) == '-')//负偏差
					{
						for(m=j+2; m<l; m++)
						{
							temp3 = (*(pBuffer+m)-0x30);
							for(z=0; z<l-m-1; z++)
							{
								temp3 = temp3 * 10;	
							}
							temp2 += temp3;
						}
						Position.HighOffset = -temp2;
					}
					else//正偏差
					{
						for(m=j+1; m<l; m++)
						{
							temp3 = (*(pBuffer+m)-0x30);
							for(z=0; z<l-m-1; z++)
							{
								temp3 = temp3 * 10;	
							}
							temp2 += temp3;
						}
						Position.HighOffset = temp2;
					}
                                  }
				default : break;
			}
			j = i;
		}
	}
	if(1 == flag)
	{
		GpggaParseErrorCount++;
	}
	else
	{
		GpggaParseErrorCount = 0;	
	}
}
void Gps_CopyToPosition(GPS_STRUCT *dest)
{
    Ql_memcpy(&Position,dest,sizeof(GPS_STRUCT));
}
/*********************************************************************
//函数名称	:GPS_CopygPosition(GPS_STRUCT *dest, GPS_STRUCT *src)
//功能		:拷贝一份最新的gps位置数据，它是一份有效的定位数据
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void Gps_CopygPosition(GPS_STRUCT *dest)
{
    #if 1
    memcpy(dest,&gPosition,sizeof(GPS_STRUCT));
    #else
    dest->Year 		= gPosition.Year;		//年
    dest->Month 		= gPosition.Month;		//月
    dest->Date 		= gPosition.Date;		//日
    dest->Hour 		= gPosition.Hour;		//时
    dest->Minute 		= gPosition.Minute;		//分
    dest->Second 		= gPosition.Second;		//秒
    dest->North		= gPosition.North;		//1:北纬,0:南纬
    dest->Latitue_D 	= gPosition.Latitue_D;	//纬度,度
    dest->Latitue_F 	= gPosition.Latitue_F;	//纬度,分
    dest->Latitue_FX 	= gPosition.Latitue_FX;	//纬度,分的小数,单位为0.0001分
    dest->East 		= gPosition.East;		//1:东经0:西经
    dest->Longitue_D 	= gPosition.Longitue_D;	//经度,度,最大180度
    dest->Longitue_F 	= gPosition.Longitue_F;	//经度,分		
    dest->Longitue_FX 	= gPosition.Longitue_FX;	//经度,分的小数,单位为0.0001分
    dest->Speed 		= gPosition.Speed;		//速度,单位为海里/小时
    dest->SpeedX 		= gPosition.SpeedX;		//速度的小数
    dest->Course 		= gPosition.Course;		//航向,单位为度
    dest->High 		= gPosition.High;		//海拔,单位为米
    dest->HighOffset 	= gPosition.HighOffset;		//海拔偏差,单位为米
    dest->SatNum 		= gPosition.SatNum;		//卫星数量
    dest->HDOP 		= gPosition.HDOP;		//水平精度因子
    dest->Status 		= gPosition.Status;	//1:有效定位 0:无效定位
    dest->Error 		= gPosition.Error;	//1:GPS模块出错，不发送数据  0:模块正常
    #endif
}
/*********************************************************************
//函数名称	:GPS_CopyPosition(GPS_STRUCT *dest, GPS_STRUCT *src)
//功能		:拷贝一份最新收到的gps位置数据，它不一定是有效的定位数据
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void Gps_CopyPosition(GPS_STRUCT *dest)
{
    #if 1
    memcpy(dest,&Position,sizeof(GPS_STRUCT));
    #else
	dest->Year 		= Position.Year;		//年
	dest->Month 		= Position.Month;		//月
	dest->Date 		= Position.Date;		//日
	dest->Hour 		= Position.Hour;		//时
	dest->Minute 		= Position.Minute;		//分
	dest->Second 		= Position.Second;		//秒
	dest->North		= Position.North;		//1:北纬,0:南纬
	dest->Latitue_D 	= Position.Latitue_D;	//纬度,度
	dest->Latitue_F 	= Position.Latitue_F;	//纬度,分
	dest->Latitue_FX 	= Position.Latitue_FX;	//纬度,分的小数,单位为0.0001分
	dest->East 		= Position.East;		//1:东经0:西经
	dest->Longitue_D 	= Position.Longitue_D;	//经度,度,最大180度
	dest->Longitue_F 	= Position.Longitue_F;	//经度,分		
	dest->Longitue_FX 	= Position.Longitue_FX;	//经度,分的小数,单位为0.0001分
	dest->Speed 		= Position.Speed;		//速度,单位为海里/小时
	dest->SpeedX 		= Position.SpeedX;		//速度的小数
	dest->Course 		= Position.Course;		//航向,单位为度
	dest->High 		= Position.High;		//海拔,单位为米
    dest->HighOffset 	= Position.HighOffset;		//海拔偏差,单位为米
	dest->SatNum 		= Position.SatNum;		//卫星数量
	dest->HDOP 		= Position.HDOP;		//水平精度因子
	dest->Status 		= Position.Status;	//1:有效定位 0:无效定位
	dest->Error 		= Position.Error;	//1:GPS模块出错，不发送数据  0:模块正常
    #endif
}
/*********************************************************************
//函数名称	:GPS_UpdatagPosition(void)
//功能		:更新变量gPosition
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void Gps_UpdatagPosition(void)
{
    #if 1
    memcpy(&gPosition,&Position,sizeof(GPS_STRUCT));
    #else
	gPosition.Year 		= Position.Year;		//年
	gPosition.Month 	= Position.Month;		//月
	gPosition.Date 		= Position.Date;		//日
	gPosition.Hour 		= Position.Hour;		//时
	gPosition.Minute 	= Position.Minute;		//分
	gPosition.Second 	= Position.Second;		//秒
	gPosition.North		= Position.North;		//1:北纬,0:南纬
	gPosition.Latitue_D 	= Position.Latitue_D;	//纬度,度
	gPosition.Latitue_F 	= Position.Latitue_F;	//纬度,分
	gPosition.Latitue_FX 	= Position.Latitue_FX;	//纬度,分的小数,单位为0.0001分
	gPosition.East 		= Position.East;		//1:东经0:西经
	gPosition.Longitue_D 	= Position.Longitue_D;	//经度,度,最大180度
	gPosition.Longitue_F 	= Position.Longitue_F;	//经度,分		
	gPosition.Longitue_FX 	= Position.Longitue_FX;	//经度,分的小数,单位为0.0001分
	gPosition.Speed 	= Position.Speed;		//速度,单位为海里/小时
	gPosition.SpeedX 	= Position.SpeedX;		//速度的小数
	gPosition.Course 	= Position.Course;		//航向,单位为度
	gPosition.High 		= Position.High;		//海拔,单位为米
    gPosition.HighOffset    = Position.HighOffset;		//海拔偏差,单位为米
	gPosition.SatNum 	= Position.SatNum;		//卫星数量
	gPosition.HDOP 		= Position.HDOP;		//水平精度因子
	gPosition.Status 	= Position.Status;	//1:有效定位 0:无效定位
	gPosition.Error 	= Position.Error;	//1:GPS模块出错，不发送数据  0:模块正常
    #endif
}
/*********************************************************************
//函数名称	:GPS_ReadGpsSpeed(void)
//功能		:获取GPS速度
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u8 Gps_ReadSpeed(void)
{
	u8	Speed;

    Speed = (u8)(gPosition.Speed_D*1.8520);
	
	//if(Speed < 3)//GPS速度小于3公里/小时都强制等于0
    if(Speed <= 5)//GPS速度小于5公里/小时都强制等于0,dxl,2014.7.26
	{
		Speed = 0;
	}
        
	return Speed;
}
/*********************************************************************
//函数名称	:GPS_ReadGpsStatus(void)
//功能		:获取GPS的状态
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u8 Gps_ReadStatus(void)
{
	return gPosition.Status;
}
/*********************************************************************
//函数名称	:GPS_ReadGpsCourse(void)
//功能		:获取GPS的方向
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u16 Gps_ReadCourse(void)
{
	return gPosition.Course;
}
/*********************************************************************
//函数名称	:GPS_ReadGpsCourseDiv2(void)
//功能		:获取GPS的方向（除以了2）
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u8 Gps_ReadCourseDiv2(void)
{
	u8	Course;

	Course = gPosition.Course >> 1;

	return Course;
}
/*********************************************************************
//函数名称	:Gps_PowerOnClearPosition(void)
//功能		:初次使用经纬度初始化为零
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void Gps_PowerOnClearPosition(void)
{
	memset(&gPosition,0,sizeof(GPS_STRUCT));
}

/*********************************************************************
//函数名称	:GPS_PowerOnUpdataPosition(void)
//功能		:上电更新经纬度
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void Gps_PowerOnUpdataPosition(void)
{
  u8	Buffer[12] = {0};
    u8	PramLen = 0;

    PramLen = FRAM_BufferRead(Buffer, 11, FRAM_LAST_LOCATION_ADDR); 
	if(11!=PramLen)
	{
		Ql_memset(&gPosition,0,sizeof(GPS_STRUCT));
	}
    else
    {
        gPosition.North      	= Buffer[0];	//1:北纬0:南纬,1字节 
        gPosition.Latitue_D  	= Buffer[1];//度,1字节
        gPosition.Latitue_F  	= Buffer[2];//分,1字节
        gPosition.Latitue_FX 	= Buffer[3]*256 + Buffer[4];//分小数部分,单位为0.0001分,2字节,高字节
        gPosition.East	 	    = Buffer[5];//1:东经0:西经,1字节
        gPosition.Longitue_D 	= Buffer[6];//度 最大180度,1字节,
        gPosition.Longitue_F 	= Buffer[7];//分，1字节，			
        gPosition.Longitue_FX 	= Buffer[8]*256 + Buffer[9];//经度分的小数部分,单位为0.0001分,2字节,高字节
        gPosition.Speed = 0;
        gPosition.SpeedX = 0;
        gPosition.Course = 0;
        gPosition.High = 0;
        gPosition.HighOffset = 0;
        gPosition.SatNum = 0;
        gPosition.HDOP = 0;
        gPosition.Status =  0; 
        gPosition.Error = 0;
    }

	Ql_memset((void*)&STU_GPS,0,sizeof(TDF_STU_GPS));
	STU_GPS.Reflash_Flag = 1;
}
/*********************************************************************
//函数名称	:GPS_SavePositionToFram(void)
//功能		:把位置保存到eeprom中
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:执行条件ACC ON->ACC OFF || 导航->不导航
*********************************************************************/
void Gps_SavePositionToFram(void)
{
    u8	Buffer[12];

    Buffer[0] = gPosition.North;	//1:北纬0:南纬,1字节 
    Buffer[1] = gPosition.Latitue_D;//度,1字节
    Buffer[2] = gPosition.Latitue_F;//分,1字节
    Buffer[3] = (gPosition.Latitue_FX&0xff00)>>8;//分小数部分,单位为0.0001分,2字节,高字节
    Buffer[4] = gPosition.Latitue_FX&0x00ff;//分小数部分,单位为0.0001分,2字节,低字节
    Buffer[5] = gPosition.East;//1:东经0:西经,1字节
    Buffer[6] = gPosition.Longitue_D;//度，1字节
    Buffer[7] = gPosition.Longitue_F;//1字节			
    Buffer[8] = (gPosition.Longitue_FX&0xff00)>>8;//经度分的小数部分,单位为0.0001分,2字节,高字节
    Buffer[9] = gPosition.Longitue_FX&0x00ff;//经度分的小数部分,单位为0.0001分,2字节,低字节
    Buffer[10]= gPosition.Status ;	//1:有效定位 0:无效定位

    if(0 == Buffer[0])
    {
      
    }
    FRAM_BufferWrite(FRAM_LAST_LOCATION_ADDR, Buffer, 11);

}
/*********************************************************************
//函数名称	:Gps_GetRunFlag
//功能		:获取去漂移行驶标志
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:1为行驶，0为停驶
//备注		:
*********************************************************************/
u8 Gps_GetRunFlag(void)
{
    return GpsRunFlag;
}

/*********************************************************************
//函数名称	:Gps_IsInRunning
//功能		:判断当前是处于行驶状态还是停驶状态
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:1为行驶，0为停驶
//备注		:1秒调用1次
*********************************************************************/
void Gps_IsInRunning(void)
{
    static u32 UpCount = 0;
    static u32 DownCount = 0;    
    u8 Speed;
    
	//Speed = Position.Speed;    
    Speed = (Position.Speed*18520+Position.SpeedX*1852+5000)/10000;
    if(Speed > 5)//dxl,2014.8.26,原来是速度大于0,现在纠正为5km/h
    {
        DownCount = 0;
        UpCount++;
        if(UpCount >= 10)
        {
            GpsRunFlag = 1;//大于0持续10秒为行驶
        }
    }
    else
    {
        UpCount = 0;
        DownCount++;
        if(DownCount >= 10)
        {
            GpsRunFlag = 0;//等于0持续10秒为停驶
        }
    }
}
/*************************************************************
** 函数名称: Public_GetCurGpsCoordinate
** 功能描述: 获得当前坐标(纬度值、经度值)
** 入口参数: flag方向:1:倒序,0:顺序
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
void Public_GetCurGpsCoordinate(unsigned char *data,unsigned char flag)
{  
    unsigned long temp;
    GPS_STRUCT Position;
    //读GPS信息
    Gps_CopygPosition(&Position);
    //读纬度值
    temp = (Position.Latitue_FX*100UL+Position.Latitue_F*1000000UL)/60+(Position.Latitue_D*1000000UL);
    Public_Mymemcpy(&data[0],(unsigned char *)&temp,4,flag);
    //读取经度值
    temp = (Position.Longitue_FX*100UL+Position.Longitue_F*1000000UL)/60+(Position.Longitue_D*1000000UL);
    Public_Mymemcpy(&data[4],(unsigned char *)&temp,4,flag);
}
extern s32 m_pwrOnReason;

/***********************************************
功能:经纬度数据处理与滤波
返回:0--漂移的数据，>0--正常的数据
备注:参考输出格式，将经纬度的度与分部分转换为浮点数的经纬度
************************************************/
u8 GPS_DataFilter(void)
{
	double	   Speed;
	u32 Lat_Temp = 0ul;
	u32 Log_Temp = 0ul;
	static u8 RtcAdjFlg = 0;
	static u8 LocValidCnt = 0;
	static u32 Last_Lat = 0ul;
	static u32 Last_Log = 0ul;
	static u32 Delat_Lat = 0ul;
	static u32 Delat_Log = 0ul;
	u8	val = 0;
	//清零模块出错标志
    Io_WriteAlarmBit(ALARM_BIT_GNSS_FAULT, RESET);
	Lat_Temp = (Position.Latitue_FX*100UL+Position.Latitue_F*1000000)/60+(Position.Latitue_D*1000000UL);//纬度
	Log_Temp = (Position.Longitue_FX*100UL+Position.Longitue_F*1000000)/60+(Position.Longitue_D*1000000UL);//经度
	//Lat_Temp = Position.Latitue*10000;
	//Log_Temp = Position.Longitue*10000;
	
	/*天朝经纬度范围，北纬3~53，东经73~135*/
	//if((Lat_Temp < 3000000)||(Lat_Temp > 53000000))return 0;

	//if((Log_Temp < 73000000)||(Log_Temp > 135000000))return 0;
	
	//还没有定位
	if(!Position.Status)
	{
		if(RTCPWRON != m_pwrOnReason)//如果不是闹钟唤醒
		{
			val = 120;
		}
		else
		{
			val = 30;
		}
		if(STU_GPS.Not_Locate_Cnt++ > val)//2分钟没定位，尽力了，置上报标志
		{
			STU_GPS.Not_Locate_Cnt= 0;
			//不导航,导航标志清0
        	Io_WriteStatusBit(STATUS_BIT_NAVIGATION, RESET);
			if(ReportStatus_Off == Report_GetReportStatus()){
			Report_SetReportStatus(ReportStatus_Init);
			APP_DEBUG("<\r\n...gps未定位....\r\n");
			}
		}
		else
		if(STU_GPS.Not_Locate_Cnt++ > 10)
		{			
			//速度清0
			Position.Speed_D = 0.0;
			gPosition.Speed_D = 0.0;
            Position.Speed = 0;
            gPosition.Speed = 0;
            Position.SpeedX = 0;
            gPosition.SpeedX = 0;
            //卫星数清0
            Position.SatNum = 0;
            gPosition.SatNum = 0;
			LocationFlag = 0;
		}
		STU_GPS.Valid = 0;
		STU_GPS.Speed_Up_Cnt = 0;
		STU_GPS.Speed_Down_Cnt = 0;
	}
	else
	{
		if(0 == RtcAdjFlg){
		if(LocValidCnt++ >10 )
		{
			Gps_AdjustRtc(&Position);
		}RtcAdjFlg = 1;
		}
		STU_GPS.Not_Locate_Cnt= 0;
		Speed = Position.Speed_D*1.8520;
		//APP_DEBUG("...Speed=%0.4f...\r\n",Speed);
	    if(Speed > 5.0)//dxl,2014.8.26,原来是速度大于0,现在纠正为5km/h
	    {	        
	        STU_GPS.Speed_Up_Cnt++;
	        if(STU_GPS.Speed_Up_Cnt >= 10)
	        {
	        	STU_GPS.Speed_Down_Cnt = 0;
	        	STU_GPS.Speed_Up_Cnt = 0; 
	            STU_GPS.Reflash_Flag = 1;
	        }
	    }
	    else
	    {	        
			STU_GPS.Speed_Down_Cnt++;
			APP_DEBUG("...Speed_Down_Cnt=%d...\r\n",STU_GPS.Speed_Down_Cnt);
			if(STU_GPS.Speed_Down_Cnt >= 3600)//停驶1小时重新刷新GPS数据一次
			{
				STU_GPS.Speed_Down_Cnt = 0;
				STU_GPS.Reflash_Flag = 1;
			}
			else
	        if(STU_GPS.Speed_Down_Cnt >= 10)
	        {  
	        	STU_GPS.Speed_Up_Cnt = 0;
	            STU_GPS.Reflash_Flag = 0;
	        }
	    }
		//GPS数据刷新状态
		if(0 == STU_GPS.Reflash_Flag)
		{
			/******静止状态，经纬度不更新******/
		    Lat_Temp = Last_Lat;
		    Log_Temp = Last_Log;
			STU_GPS.Valid = 0;			
		}
		else
		{
			/*****经纬度是否合法，是否需要更新****/
			if(Last_Lat == 0ul)
			{
				Last_Lat = Lat_Temp;
				Last_Log = Log_Temp;
			}
			else
			{
				Delat_Lat = abs(Last_Lat - Lat_Temp);
				Delat_Log = abs(Last_Log - Log_Temp);
				APP_DEBUG("...Delat_Lat=%d,Delat_Log=%d...\r\n",Delat_Lat,Delat_Log);
				if((Delat_Lat > 1000)||(Delat_Log > 1000))
				{
				    /**飘呀飘**/
				    Last_Lat = 0ul;
				    Last_Log = 0ul;
				    STU_GPS.Valid = 0;
				}
				else
				{
				    Last_Lat = Lat_Temp;
				    Last_Log = Log_Temp;
					STU_GPS.Valid = 1;
					LocationFlag = 1;
					APP_DEBUG("<\r\n...GPS已定位....\r\n");
					
					//置位导航标志
	                Io_WriteStatusBit(STATUS_BIT_NAVIGATION, SET);
					if(ReportStatus_Off == Report_GetReportStatus()){
						Report_SetReportStatus(ReportStatus_Init);	
					}
				}
			}
		}
	}

	return (STU_GPS.Valid);

}

/*********************************************************************
//函数名称	:GpsParse_EvTask(void)
//功能		:GPS解析任务
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
#if 1
void Gps_EvTask(void)
{
	if(GPS_DataFilter())
	{
		Gps_UpdatagPosition();
	}
}
#else
void Gps_EvTask(void)
{
    u8	VerifyFlag = 0;
    u8	flag;
    u8	Num;
    u32	s1;
    u32	s2;
    u8  ACC;

    static  u16	DriftCount = 0;
    static  u16	noNavigationCount = 0;//不导航计数，导航或校验出错计数清0
    static  u16     VerifyErrorCount = 0;//校验错误计数，校验正确清0
    static  u32     NavigationCount = 0;//dxl,2014.7.28,导航点计数，不导航或校验出错计数清0，该变量用于从不导航到导航滤除前面3个导航点 
    static  u8      AdjustRtcFlag = 0;//校时标志，上电后从不导航到导航校时1次，今后每隔50小时校时1次

#if PRODUCT_MODEL_VTKG_22G == PRODUCT_MODEL
	ACC = 1;
#else
    ACC = Io_ReadStatusBit(STATUS_BIT_ACC);
#endif

    //校验是否通过
    //if(1 == VerifyFlag)//通过
    {
        VerifyErrorCount = 0;
        //行标检测时屏蔽结束
        //GpsParseEnterErrorCount = 0;
        //清零模块出错标志
        Io_WriteAlarmBit(ALARM_BIT_GNSS_FAULT, RESET);
        /*
        if((0 == GpsOnOffFlag)||
        ((1 == GpsOnOffFlag)&&(0 == AccOffGpsControlFlag)&&(0 == ACC)))//GPS模块关闭状态,dxl,2014.9.24去掉ACC OFF时GPS开启5分钟期间的解析    
        {
            Position.Speed = 0;
            gPosition.Speed = 0;  
            Position.SpeedX = 0;
            gPosition.SpeedX = 0;  
            return ;
        }
        */
        //判断是否定位
        //flag = Gps_GprmcIsLocation(GprmcBuffer, GPRMC_BUFFER_SIZE);
		flag = Position.Status;
        if(1 != flag)
        {
        	//APP_DEBUG("<--!!!不导航不解析GPS数据!!!!-->\r\n");
            NavigationCount = 0;
            //置位忙标志
            //GprmcBufferBusyFlag = 1;
            //解析
            //Gps_GprmcParse(GprmcBuffer, GPRMC_BUFFER_SIZE);//为行标送检添加
            //清除忙标志
            //GprmcBufferBusyFlag = 0;

            LocationFlag = 0;//当前点导航状态
            //不导航,导航标志清0
            Io_WriteStatusBit(STATUS_BIT_NAVIGATION, RESET);//不导航立即显示
            noNavigationCount++;
            if(noNavigationCount >= 10)//连续10秒种不导航,速度清0
            {
                noNavigationCount = 0;
                //速度清0
                Position.Speed = 0;
                gPosition.Speed = 0;
                Position.SpeedX = 0;
                gPosition.SpeedX = 0;
                //卫星数清0,liamtsen add 2017-06-13
                gPosition.SatNum = 0;
                
            }
            //有短路或断路报警的话,速度和导航标志清0
            if((1 == Io_ReadAlarmBit(ALARM_BIT_ANT_SHUT))
            ||(1 == Io_ReadAlarmBit(ALARM_BIT_ANT_SHORT))
            ||(1 == StopExcursionEnable))
            {
                StopExcursionEnable = 0;
                gPosition.Status = 0;
                //不导航,导航标志清0
                Io_WriteStatusBit(STATUS_BIT_NAVIGATION, RESET);
            }
        }
        else
        {
            if(0 == AdjustRtcFlag)
            {
                if(SUCCESS == Gps_AdjustRtc(&Position))
                {
                    Gps_SavePositionToFram();
                    AdjustRtcFlag = 1;
                }
            }
            noNavigationCount = 0;
		#if 0
            //*************解析GPRMC************
            //置位忙标志
            GprmcBufferBusyFlag = 1;
            //解析
            Gps_GprmcParse(GprmcBuffer, GPRMC_BUFFER_SIZE);
            //清除忙标志
            GprmcBufferBusyFlag = 0;
            //记录时间
            PositionTime = 0;//////RTC_GetCounter();
            //*************解析GPGGA*************
            //置位忙标志
            GpggaBufferBusyFlag = 1;
            //解析
            Gps_GpggaParse(GpggaBuffer, GPGGA_BUFFER_SIZE);
            //清除忙标志
            GpggaBufferBusyFlag = 0;
		#endif
            //**************去漂移**********************
            //行标检测时屏蔽开始
            flag = 0;
            //v=s/t >= 200km/h(55m/s)，认为是漂移,1分等价于1海里，即1852米
            //纬度距离
            s1 = ((Position.Latitue_D*60+Position.Latitue_F)*1000+Position.Latitue_FX/10)*2;//用2代替1.852
            s2 = ((gPosition.Latitue_D*60+gPosition.Latitue_F)*1000+gPosition.Latitue_FX/10)*2; 
            if((s1 > s2)&&(0 != s1)&&(0 != s2))
            {
                if(((s1-s2)/(PositionTime - gPositionTime) > 55)&&(0 != Position.Latitue_D)&&(0 != gPosition.Latitue_D)&&(0 != gPositionTime))//刚上电没导航过经维度为0
                {
                    flag = 1;//是漂移点
                }
            }
            else if((0 != s1)&&(0 != s2))
            {
                if(((s2-s1)/(PositionTime - gPositionTime) > 55)&&(0 != Position.Latitue_D)&&(0 != gPosition.Latitue_D)&&(0 != gPositionTime))
                {
                    flag = 1;//是漂移点
                }
            }
            //经度距离
            s1 = ((Position.Longitue_D*60+Position.Longitue_F)*1000+Position.Longitue_FX/10)*2;//用2代替1.852
            s2 = ((gPosition.Longitue_D*60+gPosition.Longitue_F)*1000+gPosition.Longitue_FX/10)*2; 
            if((s1 > s2)&&(0 != s1)&&(0 != s2))
            {
                if(((s1-s2)/(PositionTime - gPositionTime) > 55)&&(0 != Position.Longitue_D)&&(0 != gPosition.Longitue_D)&&(0 != gPositionTime))
                {
                    flag = 1;//是漂移点
                }
            }
            else if((0 != s1)&&(0 != s2))
            {
                if(((s2-s1)/(PositionTime - gPositionTime) > 55)&&(0 != Position.Longitue_D)&&(0 != gPosition.Longitue_D)&&(0 != gPositionTime))
                {
                    flag = 1;//是漂移点
                }
            }
            //检测速度变化,当前速度小于上一次的速度变化率不做限制
            //if(Position.Speed >= LastLocationSpeed)
            if((Position.Speed > LastLocationSpeed)&&(0 != gPositionTime))//dxl,2013.6.4
            {
                if((Position.Speed-LastLocationSpeed) >= (PositionTime - gPositionTime)*6)
                {
                    flag = 1;
                }
            }
            //以上速度，加速度去漂移，当连续300个点（5分钟）都漂移时可能是时间错误，则不去漂移，先纠正时间
            //有天线断路发生的话需要重新定位，StopExcursionEnable = 0,dxl,2013.10.16
            if(1 == Io_ReadAlarmBit(ALARM_BIT_ANT_SHUT))
            {
                StopExcursionEnable = 0;
            }
            if(1 == flag)
            {
                DriftCount++;
                if(DriftCount == 10)
                {
                    Position.Speed = 0;
                    gPosition.Speed = 0;  
                    Position.SpeedX = 0;
                    gPosition.SpeedX = 0;  
                }
                else if(DriftCount >= 300)
                {
                    flag = 0;
                }
            }
            else
            {
                DriftCount = 0;

                if(0 == Gps_GetRunFlag())//停驶
                {
                    if(1 == StopExcursionEnable)
                    {
                        flag = 1;//停驶不更新数据 
                    }
                }
                else
                {
                    StopExcursionEnable = 0;
                }
            }
            if(0 == LastLocationFlag)
            {
                Num = 3;
            }
            else
            {
                Num = 2;
            }
            if(0 == AccOffGpsControlFlag)//ACC OFF时GPS模块关闭
            {
                //受ACC状态控制
            }
            else
            {
                //不受ACC状态控制
                ACC = 1;
            }
			
            if((Position.SatNum>Num)&&(Position.HDOP<=8)&&(Position.Speed<=110)&&(0 == flag)&&(1 == ACC))
            //if((Position.SatNum>Num)&&(Position.HDOP<=8)&&(Position.Speed<=110)&&(0 == flag))//liamtsen  修改 2017-06-13
            {
                //置位导航标志
                NavigationCount++;
                //找到第一个静止点,开启停车去漂移，行标检测时屏蔽开始

                if(NavigationCount > 3)//前3个导航点滤除掉
                {
                    if(DriftCount >= 300)
                    {
                        DriftCount = 0;
                    }
                    Gps_UpdatagPosition();
					
                    //记录时间
                    gPositionTime = 0;////////RTC_GetCounter();
                    //上一次速度
                    LastLocationSpeed = gPosition.Speed;
                    //置位导航标志
                    Io_WriteStatusBit(STATUS_BIT_NAVIGATION, SET);
                    LocationFlag = 1;
                    //找到第一个静止点,开启停车去漂移，行标检测时屏蔽开始
                    if(0 == Gps_GetRunFlag())
                    {
                        StopExcursionEnable = 1;
                    }
                }
            }
            else
            {
                LocationFlag = 0;
            }            

            //行标检测时屏蔽开始
        }
        //行标检测时屏蔽开始
    }
	
    //行标检测时屏蔽开始
    //从不导航到导航,校时

    //校时，行标检测时需屏蔽这行
    //记录上1次定位状态
    LastLocationFlag = gPosition.Status;


}
#endif
/*********************************************************************
//函数名称	:Gps_TimeTask(void)
//功能		:GPS时间任务
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:1秒调度1次
*********************************************************************/
FunctionalState Gps_TimeTask(void)
{
    static u8 	count = 0;
    static u8   AccOffSaveFlag = 0;
    static u32  EnterCount = 0;
		static u16	gnss_pwn_cnt = 0;
    u8	ACC;

    //static u32  EnterCount2 = 0;
    static u32  AccOnCount = 0;
    static u32  AccOffCount = 0;

#if PRODUCT_MODEL_VTKG_22G == PRODUCT_MODEL
	ACC = 1;
#else
    ACC = Io_ReadStatusBit(STATUS_BIT_ACC);
#endif
    //保存最后一个有效点到铁电
    count++;
    if(count > 5)
    {
        count = 0;
        if(0 == AccOffGpsControlFlag)//受ACC控制
        {
            if(0 == ACC)//ACC 关
            {
                if(0 == AccOffSaveFlag)
                {
                    Gps_SavePositionToFram();
                    AccOffSaveFlag = 1;
                }
            }
            else// ACC开
            {
                AccOffSaveFlag = 0;
                Gps_SavePositionToFram();
            }
        }
        else
        {
            AccOffSaveFlag = 0;
            Gps_SavePositionToFram();
        }
    }

    //用GPS速度判断当前是行驶还是停止，停止时不更新GPS数据
    Gps_IsInRunning();
    if(GpsRunFlag)
  	{
  		gnss_pwn_cnt = 0;
			//APP_DEBUG("<\r\n...车辆行驶中....\r\n");
  	}
		else
		{
			//APP_DEBUG("<\r\n...车辆静止....%d\r\n",gnss_pwn_cnt);
			#ifdef __GNSS_NEW_API__
			if(gnss_pwn_cnt++ == 1800)//30分钟静止关闭gnss模块
			{
				Ql_GNSS_PowerDown();			
			}
			else if(gnss_pwn_cnt >= 3600)//间隔60分钟重新开启gnss模块
			{
				gnss_pwn_cnt = 0;
				Ql_GNSS_PowerOn(RMC_EN | GGA_EN, Gps_NMEA_Hdlr, &Position); 
			}
			#endif
		}
    //每小时更新一次时间
    EnterCount++;
    if((EnterCount >= 3600*50)&&(1 == LocationFlag))
    {
        EnterCount = 0;
        Gps_AdjustRtc(&Position);
    }
    
	
    return ENABLE ;
}

/*********************************************************************
//函数名称	:Gps_UpdateParam
//功能		
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void Gps_UpdateParam(void)
{
    //dxl,2017.2.23,无此功能
}
/*********************************************************************
//函数名称	:Gps_Off_flag
//功能		:通知gps模块已经关闭
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u8 Gps_Off_flag(void)
{
 return GpsOnOffFlag;//=0 ;
}
/*********************************************************************
//函数名称	:StrcpyNum
//功能		:从源地址拷贝指定数据之前的字节到目标地址
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u8 StrcpyNum(u8 *src,u8 *dest,u8 Endchar)
{
  u8 cpyLen=0;
  while(*src!=Endchar)
  {
    cpyLen++;
    *dest=*src;
    dest++;
    src++;
  }
  return cpyLen;
}
//清除GPS数据解析错误计数器
void Gps_ClrParseErr(void)
{
	GpsParseEnterErrorCount = 0;
	GnssStatus = 0;
}

/******************************************************************************
**                            End Of File
******************************************************************************/


