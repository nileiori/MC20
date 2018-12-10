#include "include.h"

#define POWERLOW     11700//低于11.7V欠压
#define POWERSHUT    5000
#define POWERCHANGE  9

u32 Mc20AdcValue=0;
Enum_PinName adcPin = PIN_ADC0;

static u32 ADC_CustomParam = 1;
static void Callback_OnADCSampling(Enum_ADCPin adcPin, u32 adcValue, void *customParam)
{
	u32 mainPown;
	static u8 AdcConter=0;
	Mc20AdcValue=adcValue;
   // APP_DEBUG("<-- Callback_OnADCSampling: sampling voltage(mV)=%d  times=%d -->\r\n", adcValue, *((s32*)customParam))
    *((s32*)customParam) += 1;

    mainPown=adcValue*POWERCHANGE;
	//APP_DEBUG("<-- 主电电压=%d -->\r\n",mainPown)
		
    if((mainPown<=POWERLOW)&&(mainPown>=POWERSHUT))
    {
    	AdcConter++;
		if(AdcConter>30)
			{
			AdcConter=0;
      		  Io_WriteAlarmBit(ALARM_BIT_POWER_LOW, SET);//主电欠压置位
      		  Io_WriteAlarmBit(ALARM_BIT_POWER_SHUT, RESET);//主电掉电清除
			}
		//APP_DEBUG("<-- 主电欠压 -->\r\n")
    }
    else if(mainPown<POWERSHUT)
    {
    	AdcConter=0;
        Io_WriteAlarmBit(ALARM_BIT_POWER_LOW, RESET);//主电欠压清除
        Io_WriteAlarmBit(ALARM_BIT_POWER_SHUT, SET);//主电掉电置位
		//APP_DEBUG("<-- 主电掉电 -->\r\n")
    }
	else
	{
		//APP_DEBUG("<-- 主电正常 -->\r\n")
		AdcConter=0;
        Io_WriteAlarmBit(ALARM_BIT_POWER_SHUT, RESET);//主电掉电清除
		Io_WriteAlarmBit(ALARM_BIT_POWER_LOW, RESET);//主电欠压清除
	}
}


/*********************************************************************
//函数名称	:Ad_GetValue
//功能		:根据AD编号,获得当前AD值
//输入		:adNum AD编号,参考ADC_CHAN_TYPE枚举.
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:当前采集到的AD值.
//备注		:
*********************************************************************/
u32 Ad_GetValue(u8 Channel)
{
	return Mc20AdcValue*POWERCHANGE;//得到实际电压，单位mV
}

void KillAdc(void)
{
    // Stop  sampling ADC
    Ql_ADC_Sampling(adcPin, FALSE);

}

void Ad_Init(void)
{

    // Register callback foR ADC
    APP_DEBUG("<-- Register callback for ADC -->\r\n")
    Ql_ADC_Register(adcPin, Callback_OnADCSampling, (void *)&ADC_CustomParam);

    // Initialize ADC (sampling count, sampling interval)
    APP_DEBUG("<-- Initialize ADC (sampling count=5, sampling interval=200ms) -->\r\n")
    Ql_ADC_Init(adcPin, 5, 200);

    // Start ADC sampling
    APP_DEBUG("<-- Start ADC sampling -->\r\n")
    Ql_ADC_Sampling(adcPin, TRUE);

    // Stop  sampling ADC
    //Ql_ADC_Sampling(adcPin, FALSE);
}


