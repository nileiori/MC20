#ifndef	__LEDCTR_H
#define	__LEDCTR_H


typedef enum 
{
	LED_STATUS_POWER_ON = 0,	//开机
	LED_STATUS_POWER_NORMAL = LED_STATUS_POWER_ON,//正常状态
	LED_STATUS_POWER_DOWN,		//关机
	LED_STATUS_POWER_LOW,		//低功耗
	LED_STATUS_POWER_CHARGE		//充电
}LED_STATUS;
typedef struct
{
	u8 state;
	void(*ledCtrlCB)(u8 sta);
}LED_CTRL_DEF;


void Led_Init(void);
/*********************************************************************
//函数名称	:LowVol_UpdatePram(void)
//功能		:更新低电压阈值
//输入		:无参数
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void LowVol_UpdatePram(void);

//led状态变化
void Led_Status_Change(u8 sta);

u16 uart_get_busy_status(void);
void uart_busy_time_reduce(void);
void uart_set_default_mode(void);

#endif



