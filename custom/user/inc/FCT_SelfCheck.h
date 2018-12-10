#ifndef __FCT_SELF_CHECK_H
#define __FCT_SELF_CHECK_H



typedef struct
{
	const char* instruct;
	void (*response)(void* pRev);
}FCT_INSTRUCT_PARSE;

//FCT¶Á×Ô¼ì±êÖ¾
u8 FCT_ReadSelfCheckFlg(void);

void FCT_SelfCheckInit(void);


void FCT_Response_GetID(void* pRev);
void FCT_Response_GetServerAddr(void* pRev);
void FCT_Response_GetServerPort(void* pRev);
void FCT_Response_GetReportInterval(void* pRev);
void FCT_Response_GetSimCardNum(void* pRev);
void FCT_Response_GetDeviceInfo(void* pRev);
void FCT_Response_EnterFactory(void* pRev);
void FCT_Response_SysReset(void* pRev);
void FCT_Response_SetID(void* pRev);
void FCT_Response_SetAddr(void* pRev);
void FCT_Response_SetPort(void* pRev);
void FCT_Response_SetUpInterval(void* pRev);

#endif

