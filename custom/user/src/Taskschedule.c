#include "include.h"

/* extern function -----------------------------------------------------------*/
void EvNopHandleTask();
FunctionalState TimerNopHandleTask();
void Callback_TaskScheduleTimer(u32 timeId,void *param);

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIME_TASK MyTimerTask[] =
{
    {ENABLE,0,SYSTICK_1SECOND,Msg_Gps_TimeTask},            //1:GPS控制处理, 
    {DISABLE,0,SYSTICK_1SECOND,Msg_GpsMile_TimeTask},        //2:GPS里程计算,
    {ENABLE,0,SECOND,Heartbeat_TimeTask},                  //3:心跳
    {ENABLE,0,SYSTICK_1SECOND,Msg_Sleep_TimeTask},          //5:休眠
    {DISABLE,0,SYSTICK_10SECOND,Msg_Link1Login_TimeTask},   //6:终端注册鉴权    
    {ENABLE,0,1,Msg_Io_TimeTask},                          //7:IO 状态检测
    {DISABLE,0,1,Msg_RadioProtocolResend_TimeTask},         //8:无线协议重发       
    {DISABLE,0,1,Msg_DelayTrigTimeTask},                    //9:延时触发
    {ENABLE,0,1,Msg_SpeedMonitor_TimeTask},                //11:超速监控,
    {ENABLE,0,1,Msg_AreaManage_TimeTask},                  //12:区域
    {DISABLE,0,1,Msg_ControlOil_TimeTask},                  //13:断油路
    {DISABLE,0,1,Msg_SleepDeep_TimeTask},                   //14:深度休眠
    {DISABLE,0,1,Msg_Polygon_TimeTask},                     //15:多边形   
    {ENABLE,0,SECOND,Msg_Corner_TimeTask},                  //17:拐点补传
};

EVEN_TASK MyEvTask[] = {                                 
    {DISABLE, MSG_Gps_EvTask},       		//2:GPS数据处理 Gps_EvTask
    {DISABLE, MSG_Report_EvTask},       	 //3:上报一条位置信息 
    {DISABLE, MSG_GetAkey_EvTask},			//4: 获取鉴权码任务 
    {DISABLE, MSG_FtpUpdata_EvTask},		//6: FTP无线升级 
};




static u32 s_ulTimerCount=0;
/*******************************************************************************
* Function Name  : Timer_Val
* Description    : This function .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
u32 Timer_Val()
{
    return s_ulTimerCount; 
}

/*******************************************************************************
* Function Name  : Timer_ISR
* Description    : This function .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Timer_ISR()
{
    s_ulTimerCount++;  
}
//关闭系统任务定时器
void Stop_TaskScheduleTimer(void)
{
	 Ql_Timer_Stop(TIME_TASKSCHEDULE_ID);
}
//开启系统任务定时器
void Start_TaskScheduleTimer(void)
{
	Ql_Timer_Start(TIME_TASKSCHEDULE_ID, SYSTICK_TICKS, TRUE);
}

//任务时间定时器初始化
void init_TaskScheduleTimer(void)
{ 	
	s32 ret;
	//注册GPS定位定时器
	ret = Ql_Timer_Register(TIME_TASKSCHEDULE_ID, Callback_TaskScheduleTimer, NULL);
	if(ret <0)
    {
		//保存错误代号
        APP_DEBUG("\r\n<--failed!!,init_TaskScheduleTimer Ql_Timer_Register: timer(%d) fail ,ret = %d -->\r\n",TIME_TASKSCHEDULE_ID,ret);
    }
	Ql_Timer_Start(TIME_TASKSCHEDULE_ID, SYSTICK_TICKS, TRUE);
	APP_DEBUG("\r\n<--init_TaskScheduleTimer -->\r\n");
}

/******************************************************************************
*	类别:定时器返回函数
*	功能:
*	参数:定时Id
*	返回:无
******************************************************************************/
void Callback_TaskScheduleTimer(u32 timeId,void *param)
{
	if(TIME_TASKSCHEDULE_ID==  timeId)
		{
			Timer_ISR();
			TimerTaskScheduler();
			EvTaskScheduler();
		}
}
                                                 
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : InitTimerTask
* Description    : Initialize Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitTimerTask(void)
{

}

/*******************************************************************************
* Function Name  : SetTimerTask
* Description    : Setup timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetTimerTask(TIMERTASK task, u32 time)
{
    MyTimerTask[task].TaskTimerState= ENABLE;
    MyTimerTask[task].TaskExTimer   = time;
    MyTimerTask[task].TaskTimerCount= time + Timer_Val();
}

/*******************************************************************************
* Function Name  : ClrTimerTask
* Description    : Cancel timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ClrTimerTask(TIMERTASK task)
{
    MyTimerTask[task].TaskTimerState = DISABLE;
}
/*******************************************************************************
* Function Name  : GetTimerTaskTimerState
* Description    : 
* Input          : None
* Output         : None
* Return         : TaskTimerState
*******************************************************************************/
FunctionalState GetTimerTaskTimerState(TIMERTASK task)
{
    return MyTimerTask[task].TaskTimerState;
}
/*******************************************************************************
* Function Name  : TimerTaskScheduler
* Description    : Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TimerTaskScheduler(void)
{
    u8 i;
    u32 nowtimer;
    //FunctionalState validflag;
	FUNCTION_MSG tsk_msg;	
    nowtimer = Timer_Val();
    for(i = 0; i < MAX_TIMETASK; i++)
    {
        if(ENABLE == MyTimerTask[i].TaskTimerState)
        {
            if(nowtimer >= MyTimerTask[i].TaskTimerCount)
            {
                MyTimerTask[i].TaskTimerCount = nowtimer + MyTimerTask[i].TaskExTimer;
				//validflag = MyTimerTask[i].operate;
                //MyTimerTask[i].TaskTimerState = validflag;
                tsk_msg = MyTimerTask[i].operate;
                Ql_OS_SendMessage (sysCtrl_task_id, MSG_ID_TIME_USER_FUNCTION, tsk_msg,0);//发送时间函数执行消息
            }
        }
    }
}

/*******************************************************************************
* Function Name  : InitEvTask
* Description    : Initialize Schedule Timer task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitEvTask(void)
{
      u8 i;
      
      for(i = 0; i < MSG_MAX_EVENTASK; i++)
      {
            MyEvTask[i].evTaskOnState = DISABLE;
      }
      //增加初始化函数
}

/*******************************************************************************
* Function Name  : SetEvTask
* Description    : Setup even task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetEvTask(u8 ev)
{
    MyEvTask[ev].evTaskOnState = ENABLE;
}

/*******************************************************************************
* Function Name  : EvTaskScheduler
* Description    : Schedule even task .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EvTaskScheduler(void)
{
    u8 i;
	EVENTTASK_MSG ev_msg;	
    for(i = 0; i < MSG_MAX_EVENTASK; i++)
    {
        if(ENABLE == MyEvTask[i].evTaskOnState)
        {
            MyEvTask[i].evTaskOnState = DISABLE;
            ev_msg=MyEvTask[i].operate;
		    Ql_OS_SendMessage (sysCtrl_task_id, MSG_ID_EV_USER_FUNCTION, ev_msg,0);//发送事件函数消息
        }   
    }
} 
/*******************************************************************************
* Function Name  : EvNopHandleTask
* Description    : 空事件处理 .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
FunctionalState TimerNopHandleTask()
{  
    return DISABLE;
}

/*******************************************************************************
* Function Name  : EvNopHandleTask
* Description    : 空事件处理 .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EvNopHandleTask()
{

}

/*******************************************************************************
 *                             end of module
 *******************************************************************************/


