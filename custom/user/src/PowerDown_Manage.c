#include "include.h"
#include "powerdown_manage.h"
#include "report.h"

#if 0
static  u32  g_EventGrpId = 0;

u32 PwrDn_Manage_GetEventGrpId(void)
{
	return g_EventGrpId;
}
void proc_powerdown_task(s32 taskid)
{
	g_EventGrpId = Ql_OS_CreateEvent(PWRDN_EVTGRP_NAME);
	
	while(TRUE)
	{APP_DEBUG("\r\nproc_powerdown_task\r\n");
		if(QL_RET_OK == Ql_OS_WaitEvent(g_EventGrpId, PWRDN_EVENT_FLAG_NORMAL_UP | PWRDN_EVENT_FLAG_BLIND_UP))
		{
			if(QL_RET_OK == Ql_OS_WaitEvent(g_EventGrpId, PWRDN_EVENT_FLAG_NORMAL_UP | PWRDN_EVENT_FLAG_BLIND_UP))
			Report_EnterPowerDownAfterReported();
		}
	}
}
#endif





