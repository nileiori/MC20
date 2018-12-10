/********************************************************************************
 * File Name:           heartbeat.c
 * Function Describe:	发送心跳包，保持线路连接
 * Relate Module:       heartbeat.h
 * Writer:              Joneming
 * Date:                2015-05-12
 * Rewriter: 			
 * Date:				
 *******************************************************************************/

//********************************头文件************************************

#include "include.h"

//********************************本地变量**********************************
static u8  	HeartbeatTimeSpace = 0;//心跳包时间间隔
static u32 	HeartbeatCount = 0;//心跳时间计数
//********************************函数声明**********************************
/*********************************************************************
//函数名称	:Heartbeat_ReportHeartbeat(void)
//功能		:Heartbeat_ReportHeartbeat
//输入		:无参数
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void Heartbeat_ReportHeartbeat(void)
{    
    if(0==HeartbeatTimeSpace)return;	
    ///////////////
    HeartbeatCount++;
    ////////////////
    //if(Io_ReadIoStatusBit(IO_STATUS_BIT_SLEEP))return;//休眠状态就不心跳了
    ////////////////
    if(HeartbeatCount<HeartbeatTimeSpace)return;
    ////////////////
    HeartbeatCount = 0;
    /////////////////////
    if(0==GetTerminalAuthorizationFlag())return;
    //////////////////////
    RadioProtocol_SendData(0x0002,NULL,0,02);
	
	APP_DEBUG("<--!!!发送心跳!!!!-->\r\n");
    //RadioProtocol_TerminalHeartbeat();
}

/*********************************************************************
//函数名称	:Heartbeat_TimeTask(void)
//功能		:每隔一定间隔发送一个心跳包
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
FunctionalState HeartBeat_TimeTask(void)
{ 
    Updata_SendResultNoteTimeTask();
    //Heartbeat_ReportHeartbeat();//不发心跳了
	return ENABLE;
}

/*********************************************************************
//函数名称	:HeartBeat_UpdatePram(void)
//功能		:更新心跳包时间间隔
//输入		:无参数
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void HeartBeat_UpdatePram(void)
{
	u8 Buffer[5] = {0};
	u8 PramLen;
	PramLen = EepromPram_ReadPram(E2_TERMINAL_HEARTBEAT_ID,Buffer);    
	if(E2_TERMINAL_HEARTBEAT_LEN == PramLen)
	{	
        HeartbeatTimeSpace = (u8)Public_ConvertBufferToLong(Buffer);
        /////////////////
        if(HeartbeatTimeSpace<10)HeartbeatTimeSpace=10;//防止流量过多
        if(HeartbeatTimeSpace>180)HeartbeatTimeSpace=180;//
	}
	else
	{
		HeartbeatTimeSpace = 60;//默认值,实际应用使用
	}
}

/*********************************************************************
//函数名称	:Heartbeat_ClrHeartbeatCount(void)
//功能		:清除变量HeartbeatCount
//输入		:无参数
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void Heartbeat_ClrHeartbeatCount(void)
{
    HeartbeatCount = 0;
}
/*********************************************************************
//函数名称	:Heartbeat_GetHeartbeatTime(void)
//功能		:清除变量HeartbeatCount
//输入		:无参数
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
unsigned char Heartbeat_GetHeartbeatTime(void)
{
    return HeartbeatTimeSpace;
}
/*********************************************************************
//函数名称	:HeartBeat_ParamInit
//功能		
//输入		:无参数
//输出		:
*********************************************************************/
void HeartBeat_ParamInit(void)
{
    HeartbeatCount = 0;
    HeartBeat_UpdatePram();
    //SetTimerTask(TIME_HEART_BEAT,SECOND);
}
/******************************************************************************
**                            End Of File
******************************************************************************/

