#include "include.h"

static u32 CurTimeID = 0;//当前时间，用ID的形式表示

TIME_T CurTime;//当前时间，每秒钟更新一次，在位置汇报任务中更新，采用的是系统时钟

/*********************************************************************
//函数名称	:ReadTime(TIME_T *tt)
//功能		:读实时时钟
//输入		:
//输出		:读取的结果存放在结构体tt中
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:    
//备注		:
*********************************************************************/
void ReadTime(void)
{
	ST_Time time;
	if(Ql_GetLocalTime(&time)!=NULL)//读取MC20时间
		{
			CurTime.year=time.year%2000;//年
			CurTime.month=time.month;//月
			CurTime.day=time.day;//日
			CurTime.hour=time.hour;//时
			CurTime.min=time.minute;//分
			CurTime.sec=time.second;//秒
		//APP_DEBUG("<-- Ql_GetLocalTime(%d.%02d.%02d %02d:%02d:%02d timezone=%02d) -->\n\r", 
        //time.year, time.month, time.day, time.hour, time.minute, time.second, time.timezone);

		
		//APP_DEBUG("<-- Ql_GetLocalTime(%d.%02d.%02d %02d:%02d:%02d) -->\n\r", 
        //CurTime.year, CurTime.month, CurTime.day, CurTime.hour, CurTime.min, CurTime.sec);
		
		}
}


/*********************************************************************
//函数名称	:SetRtc(ST_Time* dateTime)
//功能		:设置RTC计数器的值
//输入		:tt,时间
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void SetRtc(ST_Time* dateTime)
{	
    //获得RTC计数值
    //CurTimeID = ConverseGmtime(tt);
    
    Ql_SetLocalTime(dateTime);
}
/*********************************************************************
//函数名称	:ReadRtc(TIME_T *tt)
//功能		:读实时时钟
//输入		:
//输出		:读取的结果存放在结构体tt中
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:    
//备注		:
*********************************************************************/
void ReadRtc(void)
{     
	//读RTC计数器的值
	//CurTimeID = RTC_GetCounter();
	//获得用年、月、日、时、分、秒表示的时间        
	//Gmtime(&CurTime, CurTimeID);
}

/*********************************************************************
//函数名称	:RTC_GetCurTime
//功能		:获取当前时间
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:       
//备注:
*********************************************************************/
void RTC_GetCurTime(TIME_T *tt)
{
    tt->year    = CurTime.year;
    tt->month   = CurTime.month;
    tt->day     = CurTime.day;
    tt->hour    = CurTime.hour;
    tt->min     = CurTime.min;
    tt->sec     = CurTime.sec;
}
/*********************************************************************
//函数名称	:LeapYear
//功能		:判断是否为闰年
//输入		:年份
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:是闰年返回1，不是闰年返回0
//备注		:year的取值为0-99，此函数转换的时间是相对于2000年
*********************************************************************/
u8 LeapYear(u8 year)
{
    unsigned short year2;	
    year2 = year + 2000;
    //////////////////
    if(year2%4  != 0)return 0;//返回0//不是闰年的多
    ////////////////////
    if(year2%100!= 0)return 1;//为4的倍数但不是100的倍数,则是闰年
    ///////////////
    if(year2%400!= 0)return 0;//为100的倍数但不是400的倍数,则不是闰年
    /////////////
    return 1;//是400的倍数则闰年
}
/*********************************************************************
//函数名称	:YearDay
//功能		:输入年、月、日，计算是该年的第几天
//输入		:年、月、日
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:返回该年的第几天
//备注		:此函数转换的时间是相对于2000年
*********************************************************************/
u16 YearDay(u8 yy,s8 mm,s8 dd)
{
    u16 TotalDay = 0;
    unsigned short jdays[] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
    TotalDay += jdays[mm];
    TotalDay += dd;
    if(LeapYear(yy)&&(mm > 2))
        TotalDay++;
    return TotalDay;
}
/*********************************************************************
//函数名称	:CheckTimeStruct(TIME_T *tt)
//功能		:检查时间结构体内的值是否正确
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:正确，返回SUCCESS，错误返回ERROR
//备注		:
*********************************************************************/
ErrorStatus CheckTimeStruct(TIME_T *tt)
{
    //检查年、月、日、时、分、秒时间参数
    if((tt->year > END_YEAR)||(tt->year < START_YEAR)||
    (tt->month > 12)||(tt->month < 1)||
    (tt->day > 31)||(tt->day < 1)||
    (tt->hour > 23)||(tt->hour < 0)||
    (tt->min > 59)||(tt->min < 0)||
    (tt->sec > 59)||(tt->sec < 0))
    {
        return ERROR;
    }
    else
    {
        return SUCCESS;
    }
}
/*********************************************************************
//函数名称	:ConverseGmtime
//功能		:把结构体类型时间转换成32位无符号的统一计数值
//输入		:TIME_T 结构体类型时间
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:32位无符号的统一计数值
//备注		:此函数转换的时间是相对于2000年
*********************************************************************/
u32 ConverseGmtime(TIME_T *tt)
{
    s16 i;
    s16 year;
    u32 TotalDay;
    u32 TotalSec;

    TotalDay = 0;
    TotalSec = 0;
    year = tt->year;

    if((year >= START_YEAR)&&(year <= END_YEAR))	//判断年是否符合范围
    {
        for(i = START_YEAR; i < tt->year; i++)	//计算year年之前的天数
        {
            TotalDay += (365+LeapYear(i));
        }
        TotalDay += YearDay(tt->year, tt->month, tt->day);	//加上year当年的天数
        TotalSec = (TotalDay-1)*DAY_SECOND + tt->hour*HOUR_SECOND+ tt->min*60 + tt->sec;	//计算总的秒数
    }
    return TotalSec;
}

/*********************************************************************
//函数名称	:Gmtime
//功能		:把32位无符号的统一计数值转换成结构体类型时间
//输入		:指向TIME_T类型的指针，计数值
//输出		:指向TIME_T类型的指针
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void Gmtime(TIME_T *tt, u32 counter)
{
    s16 i;
    u32 sum ;
    u32 temp;
    u32 counter1 = 0;
    u8 flag;
    unsigned short jdays[] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    //////////////////
    if(counter > 0xBC191380)//超过开始年的一百年,则不再转换
    {
        tt->year    = START_YEAR;
        tt->month   = 1;
        tt->day     = 1;
        tt->hour    = 0;
        tt->min     = 0;
        tt->sec     = 0;
        return;//超过开始年的一百年,则不再转换
    }
    ///////////////////////
    sum = 0;
    temp = 0;    
    for(i=START_YEAR; sum<=counter&&i<END_YEAR; i++)   /* 计算年份 */
    {
        sum += (LeapYear(i)*DAY_SECOND + YEAR_SECOND);
        if(sum <= counter)
        {
            temp = sum;
        }
    }
    ///////////////////////////
    tt->year = i-1;
    flag=LeapYear(tt->year);
    counter1=counter-temp;
    /////////////////////////
    sum=counter1;
    counter1=counter1%DAY_SECOND;
    temp =(sum-counter1)/DAY_SECOND;
    temp++;//TotalSec = (TotalDay-1)*DAY_SECOND + tt->hour*HOUR_SECOND+ tt->min*60 + tt->sec;	//计算总的秒数
    for(i=12; i>0; i--)//
    {            
        if((temp>(jdays[i]+flag)&&i>2)||(temp>jdays[i]&&i<=2))            
        {
            tt->month=i;
            break;
        }
    }
    //////////////////////////////////////
    tt->day=temp-jdays[tt->month];
    if(tt->month>2)tt->day-=flag;
    ///////////
    tt->hour =counter1/HOUR_SECOND;
    ///////////////////////////
    counter1=counter1%HOUR_SECOND;
    /////////////////////
    tt->min =counter1/MIN_SECOND;
    /////////////
    tt->sec=counter1%MIN_SECOND;
    /////////////////////
}
/*********************************************************************
//函数名称	:
//功能		:
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:       
//备注:
*********************************************************************/
void Num2NumStr(u32 Num, u8 *NumStr)
{
    if(0 == Num)
    {
        *NumStr = 0;
        return ;
    }
    ////////////////////
    sprintf((char *)NumStr,"%02ul",Num);
    #if 0//modify by joneming
    u8 *p;
    u8 NumChar[10];
    u8 i;
    p = NumChar;
    *p = Num/1000000000 + 48;
    p++;
    *p = Num%1000000000/100000000 + 48;
    p++;
    *p = Num%100000000/10000000 + 48;
    p++;
    *p = Num%10000000/1000000 + 48;
    p++;
    *p = Num%1000000/100000 + 48;
    p++;
    *p = Num%100000/10000 + 48;
    p++;
    *p = Num%10000/1000 + 48;
    p++;
    *p = Num%1000/100 + 48;
    p++;
    *p = Num%100/10 + 48;
    p++;
    *p = Num%10 + 48;

    p = NumStr;
    i =0;
    while(NumChar[i] == '0')
    {
        i++;
    }
    if(10 == i)  //当Num值为0时
    {
        *p = '0';
        p++;
        *p = '0';
        p++;
        *p = '\0';
    }
    else if(9 == i) //当Num值小于10时
    {
        *p = '0';
        p++;
        *p = NumChar[i];
        p++;
        *p = '\0';
    }
    else //当Num值大于等于10时
    {
        for(; i<10; i++,p++)
        {
            *p = NumChar[i];
        }
        *p = '\0';
    }
    #endif

}

/*************************************************************
** 函数名称: Public_CheckTimeIsInRange
** 功能描述: 判断指定时间是否在指定范围内
** 入口参数: curTime指定时间,startTime开始时间,endTime结束时间
** 出口参数: 
** 返回参数: 1:在范围内，0:不在范围内 
** 全局变量: 无
** 调用模块: 
*************************************************************/
unsigned char Public_CheckTimeIsInRange(unsigned long curTime,unsigned long startTime,unsigned long endTime)
{
    if(startTime<endTime)//小于
    {
        if(startTime<=curTime&&curTime<=endTime)return 1;
    }
    else//大于
    if(startTime>endTime)//
    {
        if(startTime<=curTime||curTime<=endTime)return 1;
    }
    else
    if(startTime==endTime)//等于
    {
        if(startTime==curTime)return 1;
    }
    return 0;
}


