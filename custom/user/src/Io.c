#include "include.h"
#include "report.h"
//****************宏定义****************
//***************变量定义***************
//********************A板相关状态位变量************
static u16 CameraStatu=0;//A板记录仪故障信息
//********************报警及状态位变量************
static u32	Status = 0;//32位状态位,对应基本位置信息汇报里的状态位定义
static u32	ExtCarStatus = 0;//扩展的车辆状态位,对应附加位置信息汇报里的扩展车辆信号状态定义
static u32	IoStatus = 0;//IO状态位,对应附加位置信息汇报里的扩展IO状态定义
static u32	Alarm = 0;//32位报警位,对应基本位置信息汇报里的报警位定义
static u32	SelfDefineStatus = 0;//自定义表1，附加ID0xE1
static u32	SelfDefineStatusMaskWord = 0;//自定义表1屏蔽字
//static u32  SelfDefineStatusHighValid = 0;//自定义1高有效标志，对应位为1表示高有效，0为低有效
static u8	MicControl = 0;//麦克风控制字
//********************报警及状态位对应的外部IO口高低有效定义************
static u32	AlarmHighValid = 0xffffffff;  //高电压有效状态标志，每个位与Alarm对应，1为高有效，0为低有效
static u32	StatusHighValid = 0x0;        //高电压有效状态标志，每个位与IoStatus对应，1为高有效，0为低有效
//static u32	ExtCarStatusHighValid = 0x0;  //高电压有效状态标志，每个位与ExtCarStatus对应，1为高有效，0为低有效

//*********************通信模块，定位模块指示灯控制************
u32	GprsLedOnTime = 0;
u32	GprsLedOffTime = 0;
u32	GpsLedOnTime = 0;
u32	GpsLedOffTime = 0;
u8	GprsLedOnOffFlag = 0;//蜂鸣器开关标志
u8	GpsLedOnOffFlag = 0;//蜂鸣器开关标志
u8	GprsLedState = 0;//GPRS LED灯状态,0：不在线熄灭,1：在线闪烁,2：故障常亮
u8	GpsLedState = 0;//GPS LED灯状态,0：不导航熄灭,1：导航闪烁,2：故障常亮
//***************12/24V电源自适应**************
static u16	PowerLowMaxValue = 0;//代表0V
static u16	PowerShutValue = 50;//代表5.0V

//****************外部变量*****************
extern u8 GpsOnOffFlag;//GPS开关标志,0为GPS关,1为GPS开
u32	AlarmMaskWord = 0;//报警屏蔽字,设置对应位后该报警位将不再报警,相当于该报警功能没有了
///////////////////////
void Io_ScanTimeTask(void);
//****************函数定义*****************
/*********************************************************************
//函数名称	:Io_Init(void)
//功能		:IO初始化
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void Io_Init(void)
{
//输出引脚
	APP_DEBUG("\r\n<-- 设置gpio开始-->\r\n");

    GpioOutInit(GPRS_LED,PINLEVEL_HIGH);
	GpioOutInit(RESET_KEY,PINLEVEL_LOW);
	GpioOutInit(WATCHDOG,PINLEVEL_HIGH);
    GpioOutInit(REMOTE_POWERDOWN,PINLEVEL_LOW);
	GpioOutInit(OPEN_PULL_LOW,PINLEVEL_LOW);
	GpioOutInit(WDT_RESET,PINLEVEL_HIGH);
	APP_DEBUG("\r\n<-- 设置gpio结束-->\r\n");
}
//**********************第一部分：初始化****************
/*********************************************************************
//函数名称	:Io_UpdataPram(void)
//功能		:更新变量
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void Io_UpdataPram(void)
{    

	u8	Buffer[5];
	u8	BufferLen;
          
	//读报警屏蔽字
	BufferLen = EepromPram_ReadPram(E2_ALARM_MASK_ID,Buffer);
	if(BufferLen == E2_ALARM_MASK_LEN)
	{
		AlarmMaskWord = Public_ConvertBufferToLong(Buffer);
	}

    //自定义表1屏蔽字
    BufferLen = EepromPram_ReadPram(E2_DEFINE_ALARM_MASK_WORD_ID,Buffer);
	if(BufferLen == E2_DEFINE_ALARM_MASK_WORD_ID_LEN)
	{
		SelfDefineStatusMaskWord = Public_ConvertBufferToLong(Buffer);
	}
	//读上一次紧急报警状态
	BufferLen = FRAM_BufferRead(Buffer, FRAM_EMERGENCY_FLAG_LEN, FRAM_EMERGENCY_FLAG_ADDR);
	if(1 == BufferLen)
	{
		if(1 == Buffer[0])
		{
			Io_WriteAlarmBit(ALARM_BIT_EMERGENCY, SET);
		}
		else
		{
			Io_WriteAlarmBit(ALARM_BIT_EMERGENCY, RESET);
		}
	}
	
	//读取定位模式
	BufferLen = EepromPram_ReadPram(E2_GPS_SET_MODE_ID,Buffer);
	if(E2_GPS_SET_MODE_LEN == BufferLen)//bit0:GPS定位;bit1:北斗定位;bit2:GLONASS定位;bit3:Galileo定位
	{
		if(0x01 == (0x01&Buffer[0]))
		{
			Io_WriteStatusBit(STATUS_BIT_GPS, SET);
		}
		else
		{
			Io_WriteStatusBit(STATUS_BIT_GPS, RESET);
		}
		
		if(0x02 == (0x02&Buffer[0]))
		{
			Io_WriteStatusBit(STATUS_BIT_COMPASS, SET);
		}
		else
		{
			Io_WriteStatusBit(STATUS_BIT_COMPASS, RESET);
		}
		
		if(0x04 == (0x04&Buffer[0]))
		{
			Io_WriteStatusBit( STATUS_BIT_GLONASS, SET);
		}
		else
		{
			Io_WriteStatusBit( STATUS_BIT_GLONASS, RESET);
		}
		
		if(0x08 == (0x08&Buffer[0]))
		{
			Io_WriteStatusBit(STATUS_BIT_GALILEO, SET);
		}
		else
		{
			Io_WriteStatusBit(STATUS_BIT_GALILEO, RESET);
		}		
	}
	
}

/*********************************************************************
//函数名称	:Io_ReadStatus
//功能		:读标准状态字
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u32 Io_ReadStatus(void)
{
	return Status;
}
/*********************************************************************
//函数名称	:Io_ReadStatusBit(u8 index)
//功能		:读标准状态位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u8 Io_ReadStatusBit(STATUS_BIT bit)
{
	if(bit >= 32)return 0;
    return PUBLIC_CHECKBIT_EX(Status, bit);
}
/*********************************************************************
//函数名称	:Io_WriteStatusBit
//功能		:写标准状态位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteStatusBit(STATUS_BIT bit, FlagStatus state)
{
	if(bit >= 32)return;
    if(SET==state)
    {
        PUBLIC_SETBIT(Status, bit);
    }
    else if(RESET==state)
    {
        PUBLIC_CLRBIT(Status, bit);
    }
}
/*********************************************************************
//函数名称	:Io_ReadExtCarStatus
//功能		:读扩展车辆状态字
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u32 Io_ReadExtCarStatus(void)
{
	return ExtCarStatus;
}
/*********************************************************************
//函数名称	:Io_ReadExtCarStatusBit(u8 index)
//功能		:读扩展车辆状态位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u8 Io_ReadExtCarStatusBit(CAR_STATUS_BIT bit)
{
    if(bit >= 32)return 0;
    return PUBLIC_CHECKBIT_EX(ExtCarStatus, bit);
}
/*********************************************************************
//函数名称	:Io_WriteExtCarStatusBit
//功能		:写扩展车辆状态位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteExtCarStatusBit(CAR_STATUS_BIT bit, FlagStatus state)
{
	if(bit >= 32)return;
    if(SET==state)
    {
        PUBLIC_SETBIT(ExtCarStatus, bit);
    }
    else 
    if(RESET==state)
    {
        PUBLIC_CLRBIT(ExtCarStatus, bit);
    }
}
/*********************************************************************
//函数名称	:Io_ReadIoStatus
//功能		:读IO状态字
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u16 Io_ReadIoStatus(void)
{
	return IoStatus;
}
/*********************************************************************
//函数名称	:Io_ReadIoStatusBit(u8 index)
//功能		:读IO状态位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u8 Io_ReadIoStatusBit(IO_STATUS_BIT bit)
{
    if(bit >= 16)return 0;
    return PUBLIC_CHECKBIT_EX(IoStatus, bit);
}
/*********************************************************************
//函数名称	:Io_WriteIoStatusBit
//功能		:写IO状态位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteIoStatusBit(IO_STATUS_BIT bit, FlagStatus state)
{
    if(bit >= 16)return;
    if(SET==state)
    {
        PUBLIC_SETBIT(IoStatus, bit);
    }
    else if(RESET==state)
    {
        PUBLIC_CLRBIT(IoStatus, bit);
    }
}
/*********************************************************************
//函数名称	:Io_ReadAlarm
//功能		:读报警字
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u32 Io_ReadAlarm(void)
{
    return (Alarm&((~AlarmMaskWord)));
}
/*********************************************************************
//函数名称	:Io_ReadAlarmMaskBit(u8 index)
//功能		:读报警屏蔽位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:关键报警位对应为1，直接返回0；关键报警位对应位为0，返回屏蔽报警位
*********************************************************************/
u8 Io_ReadAlarmMaskBit(ALARM_BIT bit)
{
    if(bit >= 32)return 0;
    return PUBLIC_CHECKBIT_EX(AlarmMaskWord, bit);
}
/*********************************************************************
//函数名称	:Io_ReadSelfDefine2
//功能		:读自定义状态表1
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u32 Io_ReadSelfDefine2(void)
{
	return (SelfDefineStatus&(~SelfDefineStatusMaskWord));
}
/*********************************************************************
//函数名称	:Io_ReadSelfDefine2Bit(u8 index)
//功能		:读自定义表1的位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u8 Io_ReadSelfDefine2Bit(E_DEFINE_STATUS_BIT bit)
{
	u8	tmp = 0;

	if(bit < 32)
	{
		tmp = ((SelfDefineStatus&(~SelfDefineStatusMaskWord)) & (1<< bit))>> bit;
	}
	return tmp;
}
/*********************************************************************
//函数名称	:Io_ReadAlarmBit(u8 index)
//功能		:读报警位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
u8 Io_ReadAlarmBit(ALARM_BIT bit)
{
	u8	tmp = 0;

	if(bit < 32)
	{
		tmp = ((Alarm&((~AlarmMaskWord))) & (1<< bit))>> bit;
	}
	return tmp;
}

/*********************************************************************
//函数名称	:Io_WriteAlarmBit
//功能		:写某一报警位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteAlarmBit(ALARM_BIT bit, FlagStatus state)
{
    if(bit >= 32)return;	
    if(SET==state)
    {
        PUBLIC_SETBIT(Alarm,bit);
    }
    else if(RESET==state)
    {
        PUBLIC_CLRBIT(Alarm,bit);
    }
}

/*********************************************************************
//函数名称	:Io_WriteSelfDefine2Bit
//功能		:写自定义表1的位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteSelfDefine2Bit(E_DEFINE_STATUS_BIT bit, FlagStatus state)
{
	if(bit >= 32)return;
    if(SET==state)
    {
        PUBLIC_SETBIT(SelfDefineStatus, bit);
    }
    else if(RESET==state)
    {
        PUBLIC_CLRBIT(SelfDefineStatus, bit);
    }
}
/*********************************************************************
//函数名称	:Io_WriteMicConrtolBit
//功能		:写某一麦克控制位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteMicConrtolBit(MIC_BIT bit, FlagStatus Status)
{
	if(bit >= MIC_BIT_MAX)return;		
    if(SET==Status)
    {
        PUBLIC_SETBIT(MicControl, bit);
    }
    else if(RESET==Status)
    {
        PUBLIC_CLRBIT(MicControl, bit);
    }
	if(0 == MicControl)
	{
		//MIC_BASE_OFF();//关闭
	}
	else
	{
		//MIC_BASE_ON();//打开
	}
}
/*********************************************************************
//函数名称	:Io_WriteSpeakerConrtolBit
//功能		:写某一扬声器控制位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteSpeakerConrtolBit(SPEAKER_BIT bit, FlagStatus state)
{
/*
    if(bit >= SPEAKER_BIT_MAX)return;
    if(SET==state)
    {   
        PUBLIC_SETBIT(SpeakerControl, bit);
    }
    else 
    if(RESET==state)
    {
        PUBLIC_CLRBIT(SpeakerControl, bit);
    }
    if(0 == SpeakerControl)
    {
        SPEAKER_OFF();//关闭
    }
    else
    {
        SPEAKER_ON();//打开
    }
    */
}
/********************第三部分：内部函数*******************
/*********************************************************************
//函数名称	:Io_DetectStatus
//功能		:检测属于状态位的IO输入
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void Io_DetectStatus(GPIO_OUT_IN_NUM IoNum, u8 index)
{
	unsigned char checknum;
	unsigned char state;
	static u8 sucStatusHighCount[32] = {0};
	static u8 sucStatusLowCount[32] = {0};
	checknum = 10;
	state = GpioInGetState(IoNum);
	if(1 == state)
	{
		if(sucStatusHighCount[index] < checknum)
		{
			sucStatusHighCount[index]++;
		}
		sucStatusLowCount[index] = 0;
	}
	else
	{
		sucStatusHighCount[index] = 0;		
		if(sucStatusLowCount[index] <checknum)
		{
			sucStatusLowCount[index]++;
		}
	}
    if(sucStatusHighCount[index] >=checknum)
    {
        if(PUBLIC_CHECKBIT(StatusHighValid,index))
        {
            PUBLIC_SETBIT(Status,index);
        }
        else
        {
            PUBLIC_CLRBIT(Status,index);
        }
    }
	else if(sucStatusLowCount[index] >=checknum)
	{
        if(0==PUBLIC_CHECKBIT(StatusHighValid,index))
        {
            PUBLIC_SETBIT(Status,index);
        }
        else
        {
            PUBLIC_CLRBIT(Status,index);
        }
	}
}

/*********************************************************************
//函数名称	:Io_DetectBackPower
//功能		:检测备电电压
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:50ms调度一次
*********************************************************************/
void Io_DetectBackPower(void)
{
    if(!Io_ReadAlarmBit(ALARM_BIT_POWER_SHUT))return;//主电
	//带添加代码
}
/*********************************************************************
//函数名称	:Io_DetectMainPower
//功能		:检测主电电压
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:100ms调度一次
*********************************************************************/
void Io_DetectMainPower(void)
{
    #define MAIN_POWER_BUFFER_LEN     6
    static unsigned char sucCnt=0;
    static unsigned char sucAdValCount = 0;
    static unsigned char sucAdValCnt = 0;
    static unsigned short susBatValue[MAIN_POWER_BUFFER_LEN];
    //////////////////////////
    static unsigned char LowAlarmCount = 0;
    static unsigned char NoLowAlarmCount = 0;
    static unsigned char ShutAlarmCount = 0;
    static unsigned char NoShutAlarmCount = 0;    
    unsigned char AlarmCountMax;
    unsigned short AdMin = 0xffff;//AD转换值最小值
    unsigned short AdMax = 0;//AD转换值最大值
    unsigned long AdSum = 0;//AD转换值累计和    
    unsigned char i;
    ////////////
    sucCnt++;
    sucCnt = 0;
    ////////////////////
    AlarmCountMax = 150;//实际应用时使用
    susBatValue[sucAdValCnt] = Ad_GetValue(1); 
    sucAdValCnt++;
    if(sucAdValCnt>=MAIN_POWER_BUFFER_LEN)sucAdValCnt=0;
    /////////////////////////////
    if(sucAdValCount<MAIN_POWER_BUFFER_LEN)
    {
        sucAdValCount++;
        return;
    }
    AdSum = 0;
    for(i=0;i<sucAdValCount;i++)
    {
        if(AdMin > susBatValue[i])
        {
            AdMin = susBatValue[i];
        }
        if(AdMax < susBatValue[i])
        {
            AdMax = susBatValue[i];
        }
        AdSum += susBatValue[i];
    }
    //计算平均值
    AdSum -= AdMin;
    AdSum -= AdMax;
    AdSum = (AdSum /(MAIN_POWER_BUFFER_LEN-2));//(MAIN_POWER_BUFFER_LEN-2)
    //转成以0.1V为单位的值
    AdSum = AdSum/100;
    //置位或清除欠压报警
    if((AdSum < PowerLowMaxValue)&&(AdSum > PowerShutValue))
    {
        if(LowAlarmCount < AlarmCountMax)//实际应用取值
        {
            LowAlarmCount++;
        }
        NoLowAlarmCount = 0;
    }
    else
    if(AdSum >(PowerLowMaxValue+2))
    {
        LowAlarmCount = 0;        			
        if(NoLowAlarmCount < AlarmCountMax)//实际应用取值
        {
            NoLowAlarmCount++;
        }
    }
    if(LowAlarmCount >=AlarmCountMax)
    {
        Io_WriteAlarmBit(ALARM_BIT_POWER_LOW, SET);//主电欠压置位
        Io_WriteAlarmBit(ALARM_BIT_POWER_SHUT, RESET);//主电掉电清除
    }
    else if(NoLowAlarmCount >=AlarmCountMax)
    {
        Io_WriteAlarmBit(ALARM_BIT_POWER_LOW, RESET);//主电欠压清除
    }
    //置位或清除断电报警
    if(AdSum < PowerShutValue)
    {
        if(ShutAlarmCount < AlarmCountMax)
        {
            ShutAlarmCount++;
        }
        NoShutAlarmCount = 0;
    }
    else
    if(AdSum >(PowerShutValue+10))
    {
        ShutAlarmCount = 0;        
        if(NoShutAlarmCount < AlarmCountMax)
        {
            NoShutAlarmCount++;
        }
    }
    if(ShutAlarmCount >=AlarmCountMax)
    {
        Io_WriteAlarmBit(ALARM_BIT_POWER_SHUT, SET);//主电掉电置位
        Io_WriteAlarmBit(ALARM_BIT_POWER_LOW, RESET);//主电欠压清除
    }
    else if(NoShutAlarmCount >=AlarmCountMax)
    {
        Io_WriteAlarmBit(ALARM_BIT_POWER_SHUT, RESET);//主电掉电清除
    }    
}
/*********************************************************************
//函数名称	:Io_PowerOnDetectMainPower
//功能		:上电检测主电电压,判断是12V供电还是24V供电
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void Io_PowerOnDetectMainPower(void)
{	
	u32	AdSum = 0;//AD转换值累计和
	Ql_Sleep(1000);//等待ADC转换完成
    /////////////////////////
	AdSum = Ad_GetValue(1); 
	//计算平均值
	//转成以0.1V为单位的值
	AdSum = AdSum/100;
	if(AdSum >= 200)//大于等于20V认为是24V系统,否则是12V系统
	{
		PowerLowMaxValue = 228;
		//PWR_TYPE_24V();
	}
	else
	{
		PowerLowMaxValue = 117;
		//PWR_TYPE_12V();
	}		
}

/*********************************************************************
//函数名称	:Io_WriteStatusHighValidBit
//功能		:写StatusHighValid某一状态位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteStatusHighValidBit(STATUS_BIT bit, FlagStatus state)
{
    if(bit >= 32)return;
    if(SET==state)
    {
        PUBLIC_SETBIT(StatusHighValid, bit);
    }
    else if(RESET==state)
    {
        PUBLIC_CLRBIT(StatusHighValid, bit);
    }
}

/*********************************************************************
//函数名称	:Io_WriteAlarmHighValidBit
//功能		:写AlarmHighValid某一状态位
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteAlarmHighValidBit(ALARM_BIT bit, FlagStatus state)
{
	if(bit >= 32)return;
    if(SET==state)
    {
        PUBLIC_SETBIT(AlarmHighValid, bit);
    }
    else if(RESET==state)
    {
        PUBLIC_CLRBIT(AlarmHighValid, bit);
    }
}
/*********************************************************************
//函数名称	:Io_ScanIoStatusAndAlarm
//功能		:IO及报警位检测任务
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:100ms调度一次
*********************************************************************/
void Io_ScanIoStatusAndAlarm(void)
{
    u8	Acc;
    //Io_DetectStatus(IO_ACC, STATUS_BIT_ACC);                   //检测ACC
    //////////////////
    //Io_DetectStatus(IO_DOOR, STATUS_BIT_DOOR1);                   //门,低有效
    ///////////////////////
    //Io_DetectMainPower();//检测主电是否正常
    /////////////////
    Io_DetectBackPower();//检测备电    
    ////////////////////////////
    Acc = Io_ReadStatusBit(STATUS_BIT_ACC);
}
extern s32 m_pwrOnReason;
/*********************************************************************
//函数名称	:Io_StatusAlarmAndPhoto
//功能		:IO及报警位检测任务
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:100ms调度一次
*********************************************************************/
void Io_StatusAlarmAndPhoto(void)
{
      //zengliang add 2017-04-06
     static u32	LastStatus = 0;//上一次IO状态
     static u32	LastAlarm = 0;//上一次报警状态
     static u32	LastSelfDefineStatus = 0;//上一次报警状态
	 u32 Status,Alarm,SelfDefineStatus;
	 
	 Status = Io_ReadStatus();
	 Alarm = Io_ReadAlarm();
	 SelfDefineStatus = Io_ReadSelfDefine2();
	 if(RTCPWRON != m_pwrOnReason)
	 if(ReportStatus_On == Report_GetReportStatus()){
	     //状态变化立即上报
	     if(LastStatus != Status)
	     {
	       Report_UploadPositionInfo();//上报一条位置信息
	       APP_DEBUG("<--状态变化上报位置信息 %d-->\r\n",Status);
	     }
	     //产生报警立即上报
	     if(LastAlarm != Alarm)
	     {
	       Report_UploadPositionInfo();//上报一条位置信息
	       APP_DEBUG("<--报警状态变化上报位置信息 -->\r\n");
	     }
		 //扩展车辆报警状态位
		 if(LastSelfDefineStatus != SelfDefineStatus)
	     {
	       Report_UploadPositionInfo();//上报一条位置信息
	       APP_DEBUG("<--扩展车辆报警状态变化上报位置信息 -->\r\n");
	     }
	 }
     LastAlarm = Alarm;
     LastStatus = Status;
	 LastSelfDefineStatus = SelfDefineStatus;
}
/*********************************************************************
//函数名称	:Io_StatusAlarmAndPhoto
//功能		:LED状态控制
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:100ms调度一次
*********************************************************************/
void Io_DisposeLEDAndBeep(void)
{
/*
    u32	CurrentTime = 0;
    static u8 led_flash=0;
    static u8 last_led_mode;
    u8 led_mode=0;
    led_mode=Gps_ReadStatus()<<4|communicatio_GetMudulState(COMMUNICATE_STATE);
    if(last_led_mode!=led_mode)
    {
      GprsLedOnOffFlag=0;
    }
    last_led_mode=led_mode;
  
    if((1 == Io_ReadAlarmBit(ALARM_BIT_ANT_SHUT))
     ||(1 == Io_ReadAlarmBit(ALARM_BIT_ANT_SHORT))
     ||(1 == Io_ReadAlarmBit(ALARM_BIT_GNSS_FAULT)))
    {
      led_mode=0xff;
    }
    ///////////////
    //***********蜂鸣器*************
    CurrentTime = Timer_Val();        
    //*********GPRS LED控制**********
    //if(0 == communicatio_GetMudulState(COMMUNICATE_STATE))
    if(led_mode==0x11)//导航并上线双闪
    {
      //双闪
      if(led_flash<3)
      {
        led_flash++;
        if(1 == GprsLedOnOffFlag)//开
        {
          if((CurrentTime - GprsLedOnTime) >= 6)
          {
            GPS_LED_OFF();
            GPRS_LED_OFF();
          }
        }
        else//关
        {
          if((CurrentTime - GprsLedOffTime) >= 1)
          {
            GPS_LED_ON();
            GPRS_LED_ON();
          }
        } 
      }
      else
      {
        GpioOutOff(GPRS_LED);
        if((CurrentTime - GprsLedOnTime) >= 20)
        {
          led_flash=0;
          GPS_LED_OFF();
          GPRS_LED_OFF();
        }
      }    
    }
    else
    if(led_mode==0x01)//上线未导航快闪
    {
    //快闪
        if(1 == GprsLedOnOffFlag)//关
        {
            if((CurrentTime - GprsLedOnTime) >= 8)
            {
              GPS_LED_OFF();
                GPRS_LED_OFF();
            }
        }
        else//开
        {
            if((CurrentTime - GprsLedOffTime) >= 8)
            {
              GPS_LED_ON();
                GPRS_LED_ON();
            }
        }
    }
    else
    if(led_mode==0x10)//导航未上线慢闪
    {
      //慢闪
      if(1 == GprsLedOnOffFlag)//关
      {
        if((CurrentTime - GprsLedOnTime) >= 40)
        {
          GPS_LED_OFF();
          GPRS_LED_OFF();
        }
      }
      else//开
      {
        if((CurrentTime - GprsLedOffTime) >= 40)
        {
          GPS_LED_ON();
          GPRS_LED_ON();
        }
      }
    }
    else
    if(led_mode==0x00)//未上线、不导航熄灭
    {
      GPRS_LED_OFF();
//      //慢闪
//      if(1 == GprsLedOnOffFlag)//关
//      {
//        if((CurrentTime - GprsLedOnTime) >= 200)
//        {
//          GPS_LED_OFF();
//          GPRS_LED_OFF();
//        }
//      }
//      else//开
//      {
//        if((CurrentTime - GprsLedOffTime) >= 200)
//        {
//          GPS_LED_ON();
//          GPRS_LED_ON();
//        }
//      }
    }
    else
    if(led_mode==0xFF)//故障常亮
    {
      GPS_LED_ON();
      GPRS_LED_ON();
    }
    //*********GPS LED控制**********
//    if((1 == Io_ReadAlarmBit(ALARM_BIT_ANT_SHUT))
//     ||(1 == Io_ReadAlarmBit(ALARM_BIT_ANT_SHORT))
//     ||(1 == Io_ReadAlarmBit(ALARM_BIT_GNSS_FAULT)))
//    {
//        GPS_LED_ON();
//    }
//    else 
//    if(0 == Gps_ReadStatus())//不在线
//    {
//        GPS_LED_OFF();
//    }
//    else 
//    if(1 == Gps_ReadStatus())
//    {
//        if(1 == GpsLedOnOffFlag)//关
//        {
//            if((CurrentTime - GpsLedOnTime) >= SYSTICK_1SECOND)
//            {
//                GPS_LED_OFF();
//            }
//        }
//        else//开
//        {
//            if((CurrentTime - GpsLedOffTime) >= SYSTICK_1SECOND)
//            {
//                GPS_LED_ON();
//            }
//        }
//    }
*/
}
/*********************************************************************
//函数名称	:Io_AccDetec
//功能		:acc检测
*********************************************************************/
#if 0
void Io_AccDetec( void)
{

  static u8 AccOnLinCont=0;
  static u8 AccOffLineCont=0;
	if(Ql_GPIO_GetLevel(ACC_DET_PIN)==1)
	{
		//APP_DEBUG("\r\n<--ACC_LINE HIGHT AccOnLinCont=%d-->\r\n",AccOnLinCont);
		AccOffLineCont=0;
		if(AccOnLinCont<201)AccOnLinCont++;
		
		if(AccOnLinCont==200)
		{

			Io_WriteStatusBit(STATUS_BIT_ACC, SET);//ACC_ON
			//SetAccLineFlag(1);
			BATTERYCHARGE_ON();
		}
		return;
	}
	else
	{
		AccOnLinCont=0;
		//if(ReadAccLineFlag()==1)
		{
			//APP_DEBUG("\r\n<--ACC_LINE LOWER AccOffLineCont=%d-->\r\n",AccOffLineCont);
				if(AccOffLineCont<201)AccOffLineCont++;
				
				if(AccOffLineCont==200)
					{
						//ShockTickCnt=0;
						Io_WriteStatusBit(STATUS_BIT_ACC, RESET);//ACC_OFF
						BATTERYCHARGE_OFF();
						return;
					}
				return ;
		}
	}

}
#endif
/*********************************************************************
//函数名称	:Io_ScanTimeTask
//功能		:IO及报警位检测任务扫描
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:100ms调度一次
*********************************************************************/
void Io_ScanTimeTask(void)
{
	//Io_AccDetec();
    //Io_ScanIoStatusAndAlarm();
    Io_StatusAlarmAndPhoto();
}
/*********************************************************************
//函数名称	:Io_TimeTask
//功能		:IO及报警位检测任务
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:50ms调度一次
*********************************************************************/
FunctionalState Io_TimeTask(void)
{
	Io_ScanTimeTask();
    return ENABLE;
}

