/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Quectel Co., Ltd. 2013
*
*****************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   example_watchdog.c
 *
 * Project:
 * --------
 *   OpenCPU
 *
 * Description:
 * ------------
 *   This example demonstrates how to use watchdog function with APIs in OpenCPU.
 *   In main task ,it start a GPTimer and set reset mode .
 *   In proc_subtask1,it start a watchdog and start a timer to feed the dog .If you do not feed 
 *   the dog ,the modem will reset when the dog overflow.
 *   All debug information will be output through DEBUG port.
 *   
 * Usage:
 * ------
 *   Compile & Run:
 *
 *     Set "C_PREDEF=-D __EXAMPLE_WATCHDOG__" in gcc_makefile file. And compile the 
 *     app using "make clean/new".
 *     Download image bin to module to run.
 * 
 *   Operation:
 *            
 * 
 * Author:
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 *----------------------------------------------------------------------------
 * 
 ****************************************************************************/
#include "include.h"
#include "ql_wtd.h"


static void WDT_Callback_OnTimer(u32 timerId, void* param);


/**************************************************************
* watchdog setup
***************************************************************/
void WDG_Hard_Init(void)
{
    
    s32 wtdid;
    s32 ret;  

    // Initialize external watchdog:
    // specify the GPIO pin (PINNAME_NETLIGHT) and the overflow time is 600ms.
    //ret = Ql_WTD_Init(0, WATCHDOG_PIN, 600);
    //if (0 == ret)
    //{
    //    APP_DEBUG("\r\n<--OpenCPU: watchdog init OK!-->\r\n");         
    //}

    // Create a logic watchdog, the interval is 3s
    //wtdid = Ql_WTD_Start(3000);

    // Register & start a timer to feed the logic watchdog.
    // The watchdog id will be passed into callback function as parameter.
    //ret = Ql_Timer_Register(LOGIC_WTD_TMR_ID, WDT_Callback_OnTimer, &wtdid);
	ret = Ql_Timer_Register(LOGIC_WTD_TMR_ID, WDT_Callback_OnTimer, NULL);
	//Ql_WTD_Feed(wtdid);//先喂一下狗
	HARD_FEED_DOG();
	Ql_Sleep(2000);//拉低2s
	//HARD_FEED_DOG1();
	GpioInInit(WATCHDOG,PINLEVEL_LOW);//配置成输入
    if(ret < 0)
    {
        APP_DEBUG("<--main task: register fail ret=%d-->\r\n",ret);
        return;
    }
    // The real feeding interval is 3s
    ret = Ql_Timer_Start(LOGIC_WTD_TMR_ID, 3000,TRUE);
    if(ret < 0)
    {
        APP_DEBUG("<--main task: start timer fail ret=%d-->\r\n",ret);        
        return;
    }
    APP_DEBUG("<--main task: start timer OK  ret=%d-->\r\n",ret);

}

/**************************************************************
* the 1st sub task
***************************************************************/
static void WDT_Callback_OnTimer(u32 timerId, void* param)
{
	static u8 dogStatus = 0;
	static u16 feedCnt = 0;
	static u8 dogErrCnt = 0;//看门狗模块错误计数器
	Enum_PinLevel gpioLvl ;
    //s32* wtdid = (s32*)param;
    //Ql_WTD_Feed(*wtdid);
    if(LOGIC_WTD_TMR_ID == timerId)
	{
		if(0 == dogStatus)//监测看门狗管脚电平
		{
		    gpioLvl= Ql_GPIO_GetLevel(WATCHDOG_PIN);
			if(PINLEVEL_LOW == gpioLvl)
			{
			//正常
				dogErrCnt = 0;
				APP_DEBUG("<-- 看门狗回应正常，电平 = %d-->\r\n",gpioLvl); 
			}
			else
			{
			//重启看门狗模块
				if(dogErrCnt++ >= 5)
				{
					dogErrCnt = 0;
					APP_DEBUG("<-- 看门狗回应异常，重启看门狗模块，电平 = %d-->\r\n",gpioLvl); 
					WDT_RESET_PIN_SETLOW();
					Ql_Sleep(1000);
					WDT_RESET_PIN_SETHIGH();
				}
			}
			GpioOutInit(WATCHDOG,PINLEVEL_HIGH);//配置成输出
			HARD_FEED_DOG1();//拉高
			dogStatus = 1;
			feedCnt = 0;
		}
		else
		if(1 == dogStatus)//喂狗
		{
			if(feedCnt++ > 30)//两分钟喂一次狗
			{
				dogStatus = 0;
				HARD_FEED_DOG();
				Ql_Sleep(1000);
				GpioInInit(WATCHDOG,PINLEVEL_HIGH);//配置成输入
				APP_DEBUG("<-- 喂狗 -->\r\n"); 	
			}
		}
	}
                   
}
/*
static u8 softWdgStatus = 0;
//软件喂狗
void SoftWdg_Feed(u8 bitSta)
{
	softWdgStatus |= bitSta;
}
//清除喂狗标志
void SoftWdg_Clr(void)
{
	softWdgStatus = 0;
}


void proc_softWdg_task(s32 taskId)
{
	
	while(1)
	{		
		Ql_Sleep(10000);//10s不正常重启
		SoftWdg_Feed(BIT5);

		if(BIT_WDT == softWdgStatus)
		{
			SoftWdg_Clr();
			APP_DEBUG("<-- 软件狗正常.... -->\r\n");
		}
		else
		{
			APP_DEBUG("<-- 程序跑飞了重启..... -->\r\n");
			Ql_Reset(0);
		}
	}
}
*/
