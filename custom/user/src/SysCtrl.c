#include "include.h"
#include "report.h"
#include "heartbeat.h"

//系统逻辑处理任务
void proc_sysCtrl_task(s32 taskId)
{
    ST_MSG msg;
	FunctionalState ctr=ENABLE;
	u32 ttt;
	while(1)
	{
	
    Ql_OS_GetMessage(&msg);
    switch (msg.message)
	{
	    case MSG_ID_TIME_USER_FUNCTION:
		    {
				ttt=msg.param1;
			switch(msg.param1)
					{
						case Msg_Gps_TimeTask://GPS控制处理
							{
								#if	( configUSE_GNSS )
								ctr=Gps_TimeTask();
								#endif
							}
							break;
						case Msg_GpsMile_TimeTask://GPS里程计算
							{
								//ctr=GpsMile_TimeTask();
							}
							break;
						case Msg_Sleep_TimeTask://休眠
							{
								//ctr=Sleep_TimeTask();
							}
							break;
						case Heartbeat_TimeTask://心跳
							{
								HeartBeat_TimeTask();
							}
							break;
						case Msg_Link1Login_TimeTask://终端注册鉴权
						 	{
								ctr=Link1Login_TimeTask();
							}
						 	break;

						case Msg_Io_TimeTask://IO 状态检测
							{
								ctr=Io_TimeTask();	
							}
							break;
						case Msg_DelayTrigTimeTask://延时触发
							{
								ctr=DelayTrigTimeTask();
							}
							break;
						case Msg_SpeedMonitor_TimeTask://超速监控
							{
								//ctr=SpeedMonitor_TimeTask();
							}
							break;

						 case Msg_ControlOil_TimeTask://断油路
						 	{
								//ctr=ControlOil_TimeTask();
							}
						    break;
						 case Msg_Corner_TimeTask://拐点补传
					 		{
							    //ctr=Corner_TimeTask();
							}
						 	break;
						 case Msg_AreaManage_TimeTask://区域
						 	{
						 		//ctr=AreaManage_TimeTask();
						 	}
						 	break;
						default:
							break;
					}
					if((ctr==DISABLE)&&(0<=ttt<=MAX_TIMETASK))
					{
						ClrTimerTask(ttt);
					}
			}
			break;
		case MSG_ID_EV_USER_FUNCTION:
			{
			switch(msg.param1)
					{
						case MSG_Gps_EvTask:
							{
								Gps_EvTask();
								//APP_DEBUG("<-- Proc_sysCtrl_task Gps_EvTask-->\r\n");
							}
							break;
						case MSG_Report_EvTask:
							{
								//Report_UploadPositionInfo();//上报一条位置信息
							}
							break;
					}
			}
        default:
            break;
	}
//		SoftWdg_Feed(BIT3);
	}
}



