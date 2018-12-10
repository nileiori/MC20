#ifndef __GPIOCONTROL_H
#define __GPIOCONTROL_H
//**************输出脚*******************   
#include "ql_gpio.h"

#define GPRS_LED_PIN               			PINNAME_NETLIGHT//GPRS指示灯  
#define RESET_KEY_PIN						PINNAME_DCD
#define WATCHDOG_PIN                        PINNAME_RI//看门狗
#define REMOTE_SHUTDOWN_PIN					PINNAME_SD_DATA//远程关机
#define OPEN_PULL_LOW_PIN					PINNAME_DTR	//开机就拉低
#define WDT_RESET_PIN						PINNAME_CTS//看门狗复位脚

#define WDT_RESET_PIN_SETHIGH()				Ql_GPIO_SetLevel(WDT_RESET_PIN, PINLEVEL_HIGH)//看门狗复位脚拉搞
#define WDT_RESET_PIN_SETLOW()				Ql_GPIO_SetLevel(WDT_RESET_PIN, PINLEVEL_LOW)//看门狗复位脚拉低
#define REMOTE_SHUTDOWN()					Ql_GPIO_SetLevel(REMOTE_SHUTDOWN_PIN, PINLEVEL_HIGH)
#define HARD_FEED_DOG()						Ql_GPIO_SetLevel(WATCHDOG_PIN, PINLEVEL_LOW)//喂狗
#define HARD_FEED_DOG1()					Ql_GPIO_SetLevel(WATCHDOG_PIN, PINLEVEL_HIGH)//喂狗
#define RESET_KEY_PIN_HIGH()				Ql_GPIO_SetLevel(RESET_KEY_PIN, PINLEVEL_HIGH)//上电3s拉高

//****************结构定义*******************   
//输出脚,输出脚编号
typedef enum 
{  
    GPRS_LED=0,          //GPRS_LED,输出
    RESET_KEY,			
    WATCHDOG,			//看门狗
    REMOTE_POWERDOWN,
    OPEN_PULL_LOW,
    WDT_RESET,
    GPIO_OUT_IN_MAX,
}GPIO_OUT_IN_NUM;  


//****************函数声明*******************
void GpioOutInit(GPIO_OUT_IN_NUM IoNum,Enum_PinLevel level);
void GpioOutOn(GPIO_OUT_IN_NUM IoNum);
void GpioOutOff(GPIO_OUT_IN_NUM IoNum);
void GpioOutToggle(GPIO_OUT_IN_NUM IoNum);

void GpioInInit(GPIO_OUT_IN_NUM IoNum,Enum_PinLevel level);
u8 GpioInGetState(GPIO_OUT_IN_NUM IoNum);
u8 GpioOutGetState(GPIO_OUT_IN_NUM IoNum);

#endif /* __GPIOCONTROL_H */

