#ifndef	__IO_H
#define	__IO_H

#include "GPIOControl.h"
#include "custom_user_cfg.h"
//*************宏定义****************

extern u8 	BeepOnOffFlag;
extern u8	GprsLedOnOffFlag;
extern u8	GpsLedOnOffFlag;

extern u32	BeepOnTime;
extern u32	BeepOffTime;
extern u32	GprsLedOnTime;
extern u32	GprsLedOffTime;
extern u32	GpsLedOnTime;
extern u32	GpsLedOffTime;

//************  LED控制************
#define  GPRS_LED_ON()  GpioOutOn(GPRS_LED)
#define  GPRS_LED_OFF()	GpioOutOff(GPRS_LED)
   
#ifdef	 PRODUCT_MODEL
#define  GPS_LED_ON()  
#define  GPS_LED_OFF()	

#define  POWER_LED_ON() 
#define  POWER_LED_OFF() 

//************电池供电控制************************
#define VBATTN_ON()    
#define VBATTN_OFF()  
//************电池充电控制************************
#define BATTERYCHARGE_ON()   
#define BATTERYCHARGE_OFF()   
#else
#define  GPS_LED_ON()  	GpioOutOff(GPS_LED)
#define  GPS_LED_OFF()	GpioOutOn(GPS_LED)

#define  POWER_LED_ON() GpioOutOn(POWER_LED)
#define  POWER_LED_OFF() GpioOutOff(POWER_LED)

//************电池供电控制************************
#define VBATTN_ON()    GpioOutOn(VBATIN_CTR)
#define VBATTN_OFF()   GpioOutOff(VBATIN_CTR)
//************电池充电控制************************
#define BATTERYCHARGE_ON()    GpioOutOn(BAT_CHARGE)
#define BATTERYCHARGE_OFF()   GpioOutOff(BAT_CHARGE)
#endif
//************看门狗************************
#define WATCHDOG_ON()    GpioOutOn(WATCHDOG)
#define WATCHDOG_OFF()   GpioOutOff(WATCHDOG)





//**************12V/24V供电控制***************
#define   PWR_TYPE_24V()	//GpioOutOn(PWR_TYPE)
#define   PWR_TYPE_12V()	//GpioOutOff(PWR_TYPE)
//麦克风控制索引
typedef enum
{
    MIC_BIT_TALK = 0,//bit0代表通话控制
    MIC_BIT_RECORD,//bit1代表录音控制
    MIC_BIT_TEST,//bit2代表检测控制
    MIC_BIT_MAX,//表示最后一位,无实际意义
}MIC_BIT;
//扬声器控制索引
typedef enum
{
    SPEAKER_BIT_TALK = 0,//bit0代表通话控制
    SPEAKER_BIT_TTS,//bit1代表TTS控制
    SPEAKER_BIT_MAX,//表示最后一位,无实际意义
}SPEAKER_BIT;
//状态位
//录音开启控制索引
typedef enum
{
    RECORD_BIT_TIME = 0,//bit0代表定时录音或一直循环录音
    RECORD_BIT_EMERGENCY,//bit1代表紧急报警录音
    RECORD_BIT_TAXI,//bit2代表空车转重车录音
    RECORD_BIT_MAX,//表示最后一位,无实际意义
}RECORD_BIT;
//状态位
typedef enum
{
    STATUS_BIT_ACC=0,		//0.0：ACC 关；1： ACC 开//zengliang add 2017-3-15
    STATUS_BIT_NAVIGATION,	//1.0：未定位；1：定位
    STATUS_BIT_SOUTH,		//2.0：北纬；1：南纬
    STATUS_BIT_EAST,		//3.0：东经；1：西经
    STATUS_BIT_STOP_WORK,	//4.0：运营状态；1：停运状态
    STATUS_BIT_ENCRYPT,		//5.0：经纬度未经保密插件加密；1：经纬度已经保密插件加密
    STATUS_BIT_RESERVER6,	//6.保留
    STATUS_BIT_RESERVER7,       //7.保留
    STATUS_BIT_FULL_LOAD1,      //8
    STATUS_BIT_FULL_LOAD2,	//9.[9:8]表示：00空车,01半载;10保留;11满载
    STATUS_BIT_OIL_CUT,		//10.0：车辆油路正常；1：车辆油路断开
    STATUS_BIT_CIRCUIT_CUT,	//11.0：车辆电路正常；1：车辆电路断开
    STATUS_BIT_DOOR_LOCK,	//12.0：车门解锁；1：车门加锁
    STATUS_BIT_DOOR1,           //13.0：门1 关；1：门1 开（前门）		
    STATUS_BIT_DOOR2,	        //14.0：门2 关；1：门2 开（中门）
    STATUS_BIT_DOOR3,	        //15.0：门3 关；1：门3 开（后门）
    STATUS_BIT_DOOR4,	        //16.0：0：门4 关；1：门4 开（驾驶席门）
    STATUS_BIT_DOOR5,	        //17.0：门5 关；1：门5 开（自定义）
    STATUS_BIT_GPS,             //18.0：未使用GPS 卫星进行定位；1：使用GPS 卫星进行定位
    STATUS_BIT_COMPASS,         //19.0：未使用北斗卫星进行定位；1：使用北斗卫星进行定位
    STATUS_BIT_GLONASS,         //20.0：未使用GLONASS 卫星进行定位；1：使用GLONASS 卫星进行定位
    STATUS_BIT_GALILEO,         //21.0：未使用Galileo 卫星进行定位；1：使用Galileo 卫星进行定位
    STATUS_BIT_RESERVER22,      //22.保留
    STATUS_BIT_RESERVER23,      //23.保留
    STATUS_BIT_RESERVER24,      //24.保留
    STATUS_BIT_RESERVER25,      //25.保留
    STATUS_BIT_RESERVER26,      //26.保留
    STATUS_BIT_RESERVER27,      //27.保留
    STATUS_BIT_RESERVER28,      //28.保留
    STATUS_BIT_RESERVER29,      //29.保留
    STATUS_BIT_RESERVER30,      //30.保留
    STATUS_BIT_RESERVER31,      //31.保留
}STATUS_BIT;
//扩展车辆状态位
typedef enum
{
    CAR_STATUS_BIT_NEAR_LIGHT=0,    //0	近光灯
    CAR_STATUS_BIT_FAR_LIGHT,       //1	远光灯
    CAR_STATUS_BIT_RIGHT_LIGHT,     //2	右转向灯
    CAR_STATUS_BIT_LEFT_LIGHT,      //3	左转向灯
    CAR_STATUS_BIT_BRAKE,           //4	制动（刹车）
    CAR_STATUS_BIT_REVERSE,         //5	倒档
    CAR_STATUS_BIT_FOG_LIGTH,       //6	雾灯
    CAR_STATUS_BIT_SIDE_LIGHT,      //7	示廊灯
    CAR_STATUS_BIT_HORN,            //8	喇叭
    CAR_STATUS_BIT_AIR_CONDITION,   //9	空调
    CAR_STATUS_BIT_NEUTRAL,         //10	空挡
    CAR_STATUS_BIT_DERAILLEUR,      //11	缓速器工作
    CAR_STATUS_BIT_ABS,             //12	ABS工作
    CAR_STATUS_BIT_HEATER,          //13	加热器工作				
    CAR_STATUS_BIT_CLUTCH,          //14	离合器工作
    CAR_STATUS_BIT_RESERVER15,      //15	15-31保留
    CAR_STATUS_BIT_RESERVER16,
    CAR_STATUS_BIT_RESERVER17,
    CAR_STATUS_BIT_RESERVER18,
    CAR_STATUS_BIT_RESERVER19,
    CAR_STATUS_BIT_RESERVER20,
    CAR_STATUS_BIT_RESERVER21,
    CAR_STATUS_BIT_RESERVER22,
    CAR_STATUS_BIT_RESERVER23,
    CAR_STATUS_BIT_RESERVER24,
    CAR_STATUS_BIT_RESERVER25,
    CAR_STATUS_BIT_RESERVER26,
    CAR_STATUS_BIT_RESERVER27,
    CAR_STATUS_BIT_RESERVER28,
    CAR_STATUS_BIT_RESERVER29,
    CAR_STATUS_BIT_RESERVER30,
    CAR_STATUS_BIT_RESERVER31,
}CAR_STATUS_BIT;
//IO状态位
typedef enum
{
    IO_STATUS_BIT_DEEP_SLEEP=0,     //0	深度休眠状态
    IO_STATUS_BIT_SLEEP,            //1	休眠状态
    IO_STATUS_BIT_RESERVER2,
    IO_STATUS_BIT_RESERVER3,
    IO_STATUS_BIT_RESERVER4,
    IO_STATUS_BIT_RESERVER5,
    IO_STATUS_BIT_RESERVER6,
    IO_STATUS_BIT_RESERVER7,
    IO_STATUS_BIT_RESERVER8,
    IO_STATUS_BIT_RESERVER9,
    IO_STATUS_BIT_RESERVER10,
    IO_STATUS_BIT_RESERVER11,
    IO_STATUS_BIT_RESERVER12,
    IO_STATUS_BIT_RESERVER13,
    IO_STATUS_BIT_RESERVER14,
    IO_STATUS_BIT_RESERVER15,
}IO_STATUS_BIT;
//报警位
typedef enum
{
    ALARM_BIT_EMERGENCY=0,          //0	1：紧急报警,收到应答后清除
    ALARM_BIT_OVER_SPEED,           //1	1：超速,标志维持至报警条件解除
    ALARM_BIT_TIRED_DRIVE,          //2	1：疲劳驾驶,标志维持至报警条件解除
    ALARM_BIT_DANGER_PRE_ALARM,     //3	1：危险预警,收到应答后清零
    ALARM_BIT_GNSS_FAULT,           //4	1：GNSS模块故障,标志维持至报警条件解除
    ALARM_BIT_ANT_SHUT,             //5	1：GNSS天线断开,标志维持至报警条件解除
    ALARM_BIT_ANT_SHORT,            //6	1：GNSS天线短路,标志维持至报警条件解除
    ALARM_BIT_POWER_LOW,            //7	1：主电欠压,标志维持至报警条件解除
    ALARM_BIT_POWER_SHUT,           //8	1：主电掉电,标志维持至报警条件解除
    ALARM_BIT_LCD_FAULT,            //9	1：LCD显示屏故障,标志维持至报警条件解除
    ALARM_BIT_TTS_FAULT,            //10	1：TTS故障,标志维持至报警条件解除
    ALARM_BIT_CAMERA_FAULT,         //11	1：摄像头故障,标志维持至报警条件解除
    ALARM_BIT_IC_ERROR,             //12    1：道路运输证IC 卡模块故障,标志维持至报警条件解除
    ALARM_BIT_OVER_SPEED_PRE_ALARM, //13    1：超速预警,标志维持至报警条件解除
    ALARM_BIT_TIRED_DRIVE_PRE_ALARM,//14    1：疲劳驾驶预警,标志维持至报警条件解除
    ALARM_BIT_RESERVER15,           //15    保留
    ALARM_BIT_RESERVER16,           //16    保留
    ALARM_BIT_RESERVER17,           //17	保留
    ALARM_BIT_DAY_OVER_DRIVE,       //18	1：当天累计驾驶超时,标志维持至报警条件解除
    ALARM_BIT_STOP_OVER,            //19	1：超时停车,标志维持至报警条件解除
    ALARM_BIT_IN_OUT_AREA,          //20	1：进出区域,收到应答后清零
    ALARM_BIT_IN_OUT_ROAD,          //21	1：进出路线,收到应答后清除
    ALARM_BIT_LOAD_OVER_DRIVE,      //22	1：路段行驶时间不足/过长,收到应答后清除
    ALARM_BIT_LOAD_EXCURSION,       //23	1：路线偏离报警,标志维持至报警条件解除
    ALARM_BIT_VSS_FAULT,            //24	1：车辆VSS 故障,标志维持至报警条件解除
    ALARM_BIT_OIL_FAULT,            //25	1：车辆油量异常,标志维持至报警条件解除
    ALARM_BIT_STEAL,                //26	1：车辆被盗(通过车辆防盗器),标志维持至报警条件解除
    ALARM_BIT_LAWLESS_ACC,          //27	1：车辆非法点火,收到应答后清零
    ALARM_BIT_LAWLESS_MOVEMENT,     //28	1：车辆非法位移,收到应答后清零
    ALARM_BIT_IMPACT_PRE_ALARM,     //29    1：碰撞预警,标志维持至报警条件解除
    ALARM_BIT_SIDE_TURN_PRE_ALARM,  //30    1：侧翻预警,标志维持至报警条件解除
    ALARM_BIT_LAWLESS_OPEN_DOOR,    //31    1：非法开门报警（终端未设置区域时，不判断非法开门）,收到应答后清零
}ALARM_BIT;

//自定义表2//zengliang 修改为表2
typedef enum
{
    ALARM_BIT_CALLTAXI=0,//0,电召标志
    DEFINE_BIT_1,		//1,定时汇报标志,这样通过上报的位置信息就可知道是定时汇报还是定距汇报		
    DEFINE_BIT_2,		//2,定距汇报标志,这样通过上报的位置信息就可知道是定时汇报还是定距汇报	
    DEFINE_BIT_EXIST_SD,//3,SD卡插入
    DEFINE_BIT_SD_FAULT,//4,SD卡故障	
    DEFINE_BIT_5,		//5,上下班签到标志,1为上班	
    DEFINE_BIT_6,		//6,保留
    DEFINE_BIT_7,		//7,保留
    DEFINE_BIT_GAS_PRESS,		//8,机油压力标志位	
    DEFINE_BIT_9,		//9,箱盖状态（渣土车版本使用）,1为开启	
    DEFINE_BIT_10,		//10,车厢举升,1为举升
    DEFINE_BIT_11,		//11,报警喇叭,1为开
    DEFINE_BIT_12,                 //12    
    DEFINE_BIT_13,     //[14,13]:00为保留,01反转,10正转,11停转  
    DEFINE_BIT_OILMETER,      //14   
    DEFINE_BIT_15,               //15    
    DEFINE_BIT_16,               //16    
    DEFINE_BIT_17,		//17	
    DEFINE_BIT_18,	        //18	备电欠压报警,zengliang add 2017-04-06
    ALARM_BIT_OVER_LOAD,      //19超载报警位
    ALARM_BIT_TAXIMETER,            //20计价器故障标	
    ALARM_BIT_COMPJA,               //21评价器故障标	
    ALARM_BIT_COMLED,               //22LED屏故障	
    ALARM_BIT_TOPLIGHT,             //23顶灯故障标志	
    DEFINE_BIT_24,		//24	
    ALARM_BIT_WEIGHT_SENSOR_ABNORMAL,   //25,载重传感器故障	
    DEFINE_BIT_26,		        //26,正反转传感器故障
    ALARM_BIT_CURVE_OVER_SPEED,		//27弯道超速报警位	
    DEFINE_BIT_28,	        //28	
    DEFINE_BIT_29,	        //29,箱盖传感器故障  
    DEFINE_BIT_30,	//30,车厢举升传感器故障 
    DEFINE_BIT_31,	//31   
}E_DEFINE_STATUS_BIT;

//B板记录仪状态
typedef enum
{
    CAMERA_STATUS_BIT_VEDIO=0,           //0	录像状态，0正常，1异常
    CAMERA_STATUS_BIT_TF_CARD,           //1	存储卡状态，0正常，1异常
    CAMERA_STATUS_BIT_MIC,               //2        麦克状态
    CAMERA_STATUS_BIT_WIFI,              //3         WIFI状态
    CAMERA_STATUS_BIT_BLUETOOTH,         //4         蓝牙状态
    CAMERA_STATUS_BIT_REMOTEKEY,         //5         遥控按键状态
    CAMERA_STATUS_BIT_PICTUREDET,        //6         图像运动检测报警
    CAMERA_STATUS_BIT_SENSOR,            //7         加速度传感器状态
    CAMERA_STATUS_BIT_PICTURESENSOR,     //8         图像传感器状态
    CAMERA_STATUS_BIT_RESERVER9,
    CAMERA_STATUS_BIT_RESERVER10,
    CAMERA_STATUS_BIT_RESERVER11,
    CAMERA_STATUS_BIT_RESERVER12,
    CAMERA_STATUS_BIT_RESERVER13,
    CAMERA_STATUS_BIT_RESERVER14,
    CAMERA_STATUS_BIT_RESERVER15,
}CAMERA_STATUS_BIT;
//*************函数声明**************
//**********************第一部分：初始化****************
/*********************************************************************
//函数名称	:Io_Init(void)
//功能		:IO初始化
*********************************************************************/
void Io_Init(void);
/*********************************************************************
//函数名称	:Io_InitStatusHighValid(void)
//功能		:初始化高电平有效标志
*********************************************************************/
void Io_InitStatusHighValid(void);
/*********************************************************************
//函数名称	:Io_InitExtCarStatusHighValid(void)
//功能		:初始化高电平有效标志
*********************************************************************/
void Io_InitExtCarStatusHighValid(void);
/*********************************************************************
//函数名称	:Io_InitDefineStatusHighValid(void)
//功能		:初始化高电平有效标志
//备注		:
*********************************************************************/
void Io_InitDefineStatusHighValid(void);
/*********************************************************************
//函数名称	:Io_InitAlarmHighValid(void)
//功能		:初始化高电平有效标志
*********************************************************************/
void Io_InitAlarmHighValid(void);
/*********************************************************************
//函数名称	:Io_UpdataPram(void)
//功能		:更新变量
*********************************************************************/
void Io_UpdataPram(void);
//**********************第二部分：外部调用****************
/*********************************************************************
//函数名称	:Io_TimeTask
//功能		:IO及报警位检测任务
//备注		:50ms调度一次
*********************************************************************/
FunctionalState Io_TimeTask(void);
/*********************************************************************
//函数名称	:Io_ReadStatus
//功能		:读标准状态字
*********************************************************************/
u32 Io_ReadStatus(void);
/*********************************************************************
//函数名称	:Io_ReadStatusBit(u8 index)
//功能		:读标准状态位
*********************************************************************/
u8 Io_ReadStatusBit(STATUS_BIT bit);
/*********************************************************************
//函数名称	:Io_WriteStatusBit
//功能		:写标准状态位
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteStatusBit(STATUS_BIT bit, FlagStatus state);
/*********************************************************************
//函数名称	:Io_ReadExtCarStatus
//功能		:读扩展车辆状态字
*********************************************************************/
u32 Io_ReadExtCarStatus(void);
/*********************************************************************
//函数名称	:Io_ReadExtCarStatusBit(u8 index)
//功能		:读扩展车辆状态位
*********************************************************************/
u8 Io_ReadExtCarStatusBit(CAR_STATUS_BIT bit);
/*********************************************************************
//函数名称	:Io_WriteExtCarStatusBit
//功能		:写扩展车辆状态位
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteExtCarStatusBit(CAR_STATUS_BIT bit, FlagStatus state);
/*********************************************************************
//函数名称	:Io_ReadIoStatus
//功能		:读IO状态字
*********************************************************************/
u16 Io_ReadIoStatus(void);
/*********************************************************************
//函数名称	:Io_ReadIoStatusBit(u8 index)
//功能		:读IO状态位
*********************************************************************/
u8 Io_ReadIoStatusBit(IO_STATUS_BIT bit);
/*********************************************************************
//函数名称	:Io_WriteIoStatusBit
//功能		:写IO状态位
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteIoStatusBit(IO_STATUS_BIT bit, FlagStatus state);
/*********************************************************************
//函数名称	:Io_ReadAlarmMaskBit(u8 index)
//功能		:读报警屏蔽位
*********************************************************************/
u8 Io_ReadAlarmMaskBit(ALARM_BIT bit);
/*********************************************************************
//函数名称	:Io_ReadAlarm
//功能		:读报警字
*********************************************************************/
u32 Io_ReadAlarm(void);
/*********************************************************************
//函数名称	:Io_WriteSpeakerConrtolBit
//功能		:写某一扬声器控制位
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteSpeakerConrtolBit(SPEAKER_BIT bit, FlagStatus state);
/*********************************************************************
//函数名称	:Io_ReadAlarmBit(u8 index)
//功能		:读报警位
*********************************************************************/
u8 Io_ReadAlarmBit(ALARM_BIT bit);
/*********************************************************************
//函数名称	:Io_ReadKeyAlarmBit(ALARM_BIT bit)
//功能		:读关键报警的某一个位
//备注		:
*********************************************************************/
u8 Io_ReadKeyAlarmBit(ALARM_BIT bit);
/*********************************************************************
//函数名称	:Io_WriteAlarmBit
//功能		:写某一报警位
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteAlarmBit(ALARM_BIT bit, FlagStatus state);

/*********************************************************************
//函数名称	:Io_ReadSelfDefine2
//功能		:读自定义状态表1
//备注		:
*********************************************************************/
u32 Io_ReadSelfDefine2(void);
/*********************************************************************
//函数名称	:Io_ReadSelfDefine2Bit(u8 index)
//功能		:读自定义表1的位
//备注		:
*********************************************************************/
u8 Io_ReadSelfDefine2Bit(E_DEFINE_STATUS_BIT bit);
/*********************************************************************
//函数名称	:Io_WriteSelfDefine2Bit
//功能		:写自定义表1的位
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteSelfDefine2Bit(E_DEFINE_STATUS_BIT bit, FlagStatus state);

/*********************************************************************
//函数名称	:Io_WriteMicConrtolBit
//功能		:写某一麦克控制位
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteMicConrtolBit(MIC_BIT bit, FlagStatus Status);

//********************第三部分：内部函数*******************
/*********************************************************************
//函数名称	:Io_DetectStatus
//功能		:检测属于状态位的IO输入
*********************************************************************/
void Io_DetectStatus(GPIO_OUT_IN_NUM IoNum, u8 index);
/*********************************************************************
//函数名称	:Io_DetectExtCarStatus
//功能		:检测属于扩展车辆状态字的IO输入
*********************************************************************/
void Io_DetectExtCarStatus(GPIO_OUT_IN_NUM IoNum,u8 index);
/*********************************************************************
//函数名称	:Io_DetectDefine2Status
//功能		:检测属于自定义2状态字的IO输入
//备注		:
*********************************************************************/
void Io_DetectDefine2Status(GPIO_OUT_IN_NUM IoNum,u8 index);
/*********************************************************************
//函数名称	:Io_DetectFrontDoor
//功能		:检测前门
*********************************************************************/
void Io_DetectFrontDoor(void);
/*********************************************************************
//函数名称	:Io_DetectLoad
//功能		:检测空重车状态，空重车传感器故障
//备注		:只有渣土车版本才有,用ADC1检测,100ms调度一次
//              :空车：3.2V~4.5V
//              :重车：2.0V~3.2V
//              :传感器短路：0V~2.0V
//              :传感器开路：4.5V~
*********************************************************************/
void Io_DetectLoad(void);
/*********************************************************************
//函数名称	:Io_DetectPhoneKey
//功能		:检测一键通话键
*********************************************************************/
u8 Io_DetectPhoneKey(void);
/*********************************************************************
//函数名称	:Io_DetectMainPower
//功能		:检测主电电压
//备注		:100ms调度一次
*********************************************************************/
void Io_DetectMainPower(void);
/*********************************************************************
//函数名称	:Io_PowerOnDetectMainPower
//功能		:上电检测主电电压,判断是12V供电还是24V供电
//备注		:上电在硬件初始化完成后进入while前需调用该函数
*********************************************************************/
void Io_PowerOnDetectMainPower(void);
/*********************************************************************
//函数名称	:Io_WriteStatusHighValidBit
//功能		:写StatusHighValid某一状态位
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteStatusHighValidBit(STATUS_BIT bit, FlagStatus state);
/*********************************************************************
//函数名称	:Io_WriteExtCarStatusHighValidBit
//功能		:写ExtCarStatusHighValid某一状态位
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteExtCarStatusHighValidBit(CAR_STATUS_BIT bit, FlagStatus state);
/*********************************************************************
//函数名称	:Io_WriteSelfDefine2HighValidBit
//功能		:写SelfDefine2HighValid某一状态位
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteSelfDefine2HighValidBit(E_DEFINE_STATUS_BIT bit, FlagStatus state);
/*********************************************************************
//函数名称	:Io_WriteAlarmHighValidBit
//功能		:写AlarmHighValid某一状态位
//备注		:注意请不要在中断函数内调用此函数
*********************************************************************/
void Io_WriteAlarmHighValidBit(ALARM_BIT bit, FlagStatus state);
#endif

/******************************************************************************
**                            End Of File
******************************************************************************/


