#include "include.h"
//****************宏定义********************

//****************外部变量*******************

//****************全局变量*******************
const u16 GPIO_OUT_IN_PIN[GPIO_OUT_IN_MAX] = 
{
    GPRS_LED_PIN,
	RESET_KEY_PIN,
    WATCHDOG_PIN,
    REMOTE_SHUTDOWN_PIN,
    OPEN_PULL_LOW_PIN,
    WDT_RESET_PIN
};
/****************函数声明*******************/

/****************函数定义******************
//函数名称	:GpioOutInit
//功能		:初始化GPIO输出引脚
//输入		:IoNum 输入值,参考GPIO_OUT_IN_NUM枚举
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void GpioOutInit(GPIO_OUT_IN_NUM IoNum,Enum_PinLevel level)
{
	s32 ret;
    // Initialize the GPIO pin (output high level, pull up)
    Ql_GPIO_Uninit(GPIO_OUT_IN_PIN[IoNum]);
    ret=Ql_GPIO_Init(GPIO_OUT_IN_PIN[IoNum], PINDIRECTION_OUT, level, PINPULLSEL_PULLDOWN);
	
	APP_DEBUG("\r\n<-- GpioOutInit PIN=%d ret%d -->\r\n",GPIO_OUT_IN_PIN[IoNum],ret);
  	//GPIO_Init(GPIO_OUT_IN_PORT[IoNum], &GPIO_InitStructure);
}
/****************函数定义******************
//函数名称	:GpioInInit
//功能		:初始化输入引脚
//输入		:IoNum 输入值,参考GPIO_OUT_IN_NUM枚举
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void GpioInInit(GPIO_OUT_IN_NUM IoNum,Enum_PinLevel level)
{
	s32 ret;
	Ql_GPIO_Uninit(GPIO_OUT_IN_PIN[IoNum]);

    // Initialize the GPIO pin (output high level, pull up)
    ret=Ql_GPIO_Init(GPIO_OUT_IN_PIN[IoNum], PINDIRECTION_IN, level, PINPULLSEL_DISABLE);
	Ql_GPIO_SetDirection(GPIO_OUT_IN_PIN[IoNum],PINDIRECTION_IN);
	APP_DEBUG("\r\n<-- GpioInInit PIN=%d ret%d -->\r\n",GPIO_OUT_IN_PIN[IoNum],ret);
   //GPIO_Init(GPIO_OUT_IN_PORT[IoNum], &GPIO_InitStructure);
}

/****************函数定义******************
//函数名称	:GpioOutOn
//功能		:设置GPIO输出引脚为高(即数字1)
//输入		:IoNum 输入值,参考GPIO_OUT_IN_NUM枚举
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void GpioOutOn(GPIO_OUT_IN_NUM IoNum)
{
    Ql_GPIO_SetLevel(GPIO_OUT_IN_PIN[IoNum], PINLEVEL_HIGH);
 	// GPIO_OUT_IN_PORT[IoNum]->BSRR = GPIO_OUT_IN_PIN[IoNum];
}
/****************函数定义******************
//函数名称	:GpioOutOff
//功能		:设置GPIO输出引脚为低(即数字0)
//输入		:IoNum 输入值,参考GPIO_OUT_IN_NUM枚举
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void GpioOutOff(GPIO_OUT_IN_NUM IoNum)
{
    Ql_GPIO_SetLevel(GPIO_OUT_IN_PIN[IoNum], PINLEVEL_LOW);
   //GPIO_OUT_IN_PORT[IoNum]->BRR = GPIO_OUT_IN_PIN[IoNum];  
}

/****************函数定义******************
//函数名称	:GpioInGetState
//功能		:获取输入引脚状态
//输入		:IoNum 输入值,参考GPIO_OUT_IN_NUM枚举
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:当前IO口状态值,0或1
//备注		:
*********************************************************************/
u8 GpioInGetState(GPIO_OUT_IN_NUM IoNum)
{
	return Ql_GPIO_GetLevel(GPIO_OUT_IN_PIN[IoNum]);
    //return GPIO_ReadInputDataBit((GPIO_TypeDef *)GPIO_OUT_IN_PORT[IoNum], GPIO_OUT_IN_PIN[IoNum]);
}
/****************函数定义******************
//函数名称	:GpioOutGetState
//功能		:获取输出引脚状态
//输入		:IoNum 输出值,参考GPIO_OUT_IN_NUM枚举
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:当前IO口状态值,0或1
//备注		:
*********************************************************************/
u8 GpioOutGetState(GPIO_OUT_IN_NUM IoNum)
{
	return Ql_GPIO_GetLevel(GPIO_OUT_IN_PIN[IoNum]);
    //return GPIO_ReadOutputDataBit((GPIO_TypeDef *)GPIO_OUT_IN_PORT[IoNum], GPIO_OUT_IN_PIN[IoNum]);
}

