#ifndef __TCP_LOG__H_
#define __TCP_LOG__H_


#define TCP_TIMER_PERIOD     800
#define TIMEOUT_90S_PERIOD   90000

#define LOGIN_TIMER_ID_PERIOD 2000

/**************************************
	TCP建立过程状态 
*****************************************/
typedef enum{
    STATE_NW_GET_SIMSTATE,
    STATE_NW_QUERY_STATE,
    STATE_GPRS_REGISTER,
    STATE_GPRS_CONFIG,
    STATE_GPRS_ACTIVATE,
    STATE_GPRS_ACTIVATING,
    STATE_GPRS_GET_DNSADDRESS,
    STATE_GPRS_GET_LOCALIP,
    STATE_CHACK_SRVADDR,
    STATE_SOC_REGISTER,
    STATE_SOC_CREATE,
    STATE_SOC_CONNECT,
    STATE_SOC_CONNECTING,
    STATE_IDLE,
    STATE_SOC_CLOSE,
    STATE_GPRS_DEACTIVATE,
    STATE_TOTAL_NUM
}Enum_TCPSTATE;


//注册鉴权任务状态
typedef enum {
  LOGIN_READ_AUTHORIZATION_CODE=0,//读取鉴权码，每次连接上网络就是此状态
  LOGIN_SEND_LOGIN_CMD,//发送注册命令
  LOGIN_SEND_AUTHORIZATION_CMD,//发送鉴权命令
  LOGIN_CLOSE_GPRS,//断开网络
  LOGIN_DELAY,//延时
  LOGIN_RECONNECT_GPRS,//重连网络
  LOGIN_CLOSE_TASK,//关闭任务
} LOGIN_STATE;  



typedef enum
{
	MAIN_SERVER=0,//主服务器
	BACK_SERVER,//备份服务器
}ServerType;

typedef struct
{
	ServerType curIpType;
	u8 ipConnectFailureCnt;
}ServerContInfo;

/**
  * @brief  连接登录，主要是触发注册鉴权任务
  * @param  type: 保留 
            state: 下一步执行的状态
  * @retval None
  */
void Net_First_Login(void);


FunctionalState  Link1Login_TimeTask(void);



/*********************************************************************
//函数名称	:ClearTerminalAuthorizationCode(u8 channel)
//功能		:清除某个通道的鉴权码
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:bit0:空缺或预留;bit1:表示CHANNEL_DATA_1，bit2:CHANNEL_DATA_2
//          :channel的值可以为以上两个通道值或者是它们的组合
*********************************************************************/
void ClearTerminalAuthorizationCode(u8 channel);

/*********************************************************************
//函数名称	:System_ClearLoginGprsPassword
//功能		:擦除鉴权码
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void System_ClearLoginGprsPassword(void);

/*********************************************************************
//函数名称	:GetTerminalAuthorizationFlag(void)
//功能		:获取鉴权标志的值
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:
*********************************************************************/
u8 GetTerminalAuthorizationFlag(void);


/*********************************************************************
//函数名称	:SetTerminalAuthorizationFlag(void)
//功能		:置位鉴权标志
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:
*********************************************************************/
void SetTerminalAuthorizationFlag(u8 channel);


/*********************************************************************
//函数名称	:ClearTerminalAuthorizationFlag(void)
//功能		:清除鉴权标志
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:
*********************************************************************/
void ClearTerminalAuthorizationFlag(u8 channel);

/*******************************************************************************
* Function Name  : SetLoginAuthorizationState
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetLoginAuthorizationState(u8 State);

/**
  * @brief  接获取参数
  * @param  type: 参数类型
  * @retval 返回参数结构 1->成功  0->非法数据
  */
u8  Net_First_Value(ServerType type );


void GsmDrop_callback(void);


/*
  * @brief  关闭连接
  * @param  None
  * @retval None
  */
void Net_First_Close(void);

void Socket_Restart(void);

#endif

