#include <include.h>
#include "report.h"
#include "FCT_SelfCheck.h"

u8	TerminalAuthorizationFlag=0;//终端鉴权标志,鉴权成功为1,失败为0
u8	Link1LoginCount = 0;//注册命令发送次数，每次30秒，3次不成功，需断开网络，10分钟后重连
u8	Link1AuthorizationCount = 0;//鉴权命令发送次数，每次30秒，10次不成功，擦除鉴权码，重走注册流程
u8	Link1LoginAuthorizationState = 0;//读取鉴权码，发送注册命令，发送鉴权命令，断开网络，延时、重连网络、关闭任务几个状态
u8	Link1LoginAuthorizationLastState = 0;//上一次状态
u16	Link1LoginAuthorizationDelayTime = 0;//注册鉴权延时时间
u8 Link1ManualCloseFlag = 0;//连接1手动关闭标志,1为关闭,0为开启

//***************本地变量******************
static u16	Link1LoginAuthorizationDelayCount = 0;//延时计数，1秒钟计数1
static s32 TCP_error_time=0;//初始化过程中TCP出错计数器
bool TIMER_TCP_flag		=	FALSE;									//判断TCP轮询是否开启(初始化未打开)				
bool SIM_STATE_flag = FALSE;//SIM卡状态标志，TRUE位SIM卡准备好，FALSE 为SIM卡没有准备好

u8 FCT_Test = 0;//FCT测试位

ServerContInfo ConnectInfo;


extern u32 battery_capacity;
extern u32 battery_voltage;

//TCP相关
static u8 m_tcp_state = STATE_NW_GET_SIMSTATE;//TCP状态机控制
/*****************************************************************
* timer param
******************************************************************/


static s32 timeout_90S_monitor = FALSE;
/*****************************************************************
* APN Param
******************************************************************/
SOC_IPVAL net_info={
		{
		"CMNET",    // APN name
    	"card",         // User name for APN
		"card",         // Password for APN
		0,
    	NULL,
    	NULL,
		},
		0,
		0,
		0
};

/*****************************************************************
* Server Param
******************************************************************/
#define SRVADDR_BUFFER_LEN  100
#define SEND_BUFFER_LEN     2048
#define RECV_BUFFER_LEN     2048

static u8 m_send_buf[SEND_BUFFER_LEN];
static u8 m_recv_buf[RECV_BUFFER_LEN];
static u64 m_nSentLen  = 0;      // Bytes of number sent data through current socket    

//static u8  m_SrvADDR[SRVADDR_BUFFER_LEN] = "momo.e-eye.cn";
//static u32 m_SrvPort = 11111;
static u8  m_ipaddress[5];  //only save the number of server ip, remove the comma

static s32 m_socketid = -1; 

static s32 m_remain_len = 0;     // record the remaining number of bytes in send buffer.
static char *m_pCurrentPos = NULL; 

/*****************************************************************
* GPRS and socket callback function
******************************************************************/
//This callback function is invoked by Ql_SocketConnect when the return value of Ql_SocketConnect is SOC_WOULDBLOCK
void socket_connect_callback(s32 socketId, s32 errCode, void* customParam );

// This callback function is invoked when the socket connection is disconnected by server or network.
void socket_close_callback(s32 socketId, s32 errCode, void* customParam );

//Accept a connection on a socket when the module is a server. This function is valid when the module is used as TCP server only
void socket_accept_callback(s32 listenSocketId, s32 errCode, void* customParam );

// This callback function is invoked when socket data arrives.
void socket_read_callback(s32 socketId, s32 errCode, void* customParam );

// This callback function is invoked in the following case:
// The return value is less than the data length to send when calling Ql_SOC_Send(), which indicates
// the socket buffer is full. Application should stop sending socket data till this callback function
// is invoked, which indicates application can continue to send data to socket.
void socket_write_callback(s32 socketId, s32 errCode, void* customParam );

//When the return value of Ql_GPRS_Activate is SOC_WOULDBLOCK, this callback function will be invoked later.
void GPRS_Actived_callback(u8 contexId, s32 errCode, void* customParam);

// This callback function is invoked when GPRS drops down.
void GPRS_Deactived_callback(u8 contextId, s32 errCode, void* customParam );

//This function is 
void GetIpByName_callback(u8 contexId, u8 requestId, s32 errCode,  u32 ipAddrCnt, u32* ipAddr);


ST_PDPContxt_Callback     callback_gprs_func = 
{
    GPRS_Actived_callback,
    GPRS_Deactived_callback
};
ST_SOC_Callback      callback_soc_func=
{
    socket_connect_callback,
    socket_close_callback,
    socket_accept_callback,
    socket_read_callback,    
    socket_write_callback
};


/*****************************************************************
* timer callback function
******************************************************************/
static void Timer_Callback(u32 timerId, void* param);


static void checkErr_AckNumber(s32 err_code)
{
    if(SOC_INVALID_SOCKET == err_code)
    {
        APP_DEBUG("<-- Invalid socket ID -->\r\n");
    }
    else if(SOC_INVAL == err_code)
    {
        APP_DEBUG("<-- Invalid parameters for ACK number -->\r\n");
    }
    else if(SOC_ERROR == err_code)
    {
        APP_DEBUG("<-- Unspecified error for ACK number -->\r\n");
    }
    else
    {
        // get the socket option successfully
    }
}

static void Timer_Callback(u32 timerId, void* param)
{
	s32 ret;
	u32 csq, bit_err;
    if (TIMEOUT_90S_TIMER_ID == timerId)
    {
        APP_DEBUG("<--90s time out!!!-->\r\n");
        APP_DEBUG("<-- Close socket.-->\r\n");
        
        Ql_SOC_Close(m_socketid);
        m_socketid = -1;

        m_tcp_state = STATE_GPRS_DEACTIVATE;
				if(RIL_AT_SUCCESS == RIL_NW_GetSignalQuality((u32*)&csq, (u32*)&bit_err))
				{
					if((csq > 10)&&(csq <= 31))
					{
						ConnectInfo.ipConnectFailureCnt++;
						APP_DEBUG("<--ConnectInfo.ipConnectFailureCnt=%d-->\r\n",ConnectInfo.ipConnectFailureCnt);
						if(ConnectInfo.ipConnectFailureCnt>3)
						{		
							Blind_UpdateCirQueueInfo();
							Report_RtcWakeUpOffsetSave();
							Ql_Reset(0);
							//Ql_RIL_SendATCmd("AT+CFUN=1", 9, NULL, NULL, 0);
						}
					}
				}
        timeout_90S_monitor = FALSE;
    }
    else if (TCP_TIMER_ID == timerId)
    {
        //APP_DEBUG("<--...........m_tcp_state=%d..................-->\r\n",m_tcp_state);
        switch (m_tcp_state)
        {
            case STATE_NW_GET_SIMSTATE://等待SIM卡准备好
            {
                s32 simStat = 0;
				APP_DEBUG("<--SIM card status checking!-->\r\n");
				if((ConnectInfo.curIpType==MAIN_SERVER)&&(ConnectInfo.ipConnectFailureCnt>2))
					{
					Net_First_Value(MAIN_SERVER);
					}
					else if((ConnectInfo.curIpType==BACK_SERVER)&&(ConnectInfo.ipConnectFailureCnt>2))
						{
						Net_First_Value(MAIN_SERVER);
						}
		
                RIL_SIM_GetSimState(&simStat);
                if (simStat == SIM_STAT_READY)
                {
					ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);  
                    m_tcp_state = STATE_NW_QUERY_STATE;
                    APP_DEBUG("<--SIM card status is normal!-->\r\n");
                    if(0xaa == FCT_ReadSelfCheckFlg())
                    {
						FCT_DEBUG("\r\nOK:SIMCARD CHACK OK\r\n");
						PUBLIC_SETBIT(FCT_Test,0);
                    }
                }
				else
				if (simStat == SIM_STAT_NOT_INSERTED)
                {APP_DEBUG("\r\nERROR:NOTINSERTED\r\n");
                    if(0xaa == FCT_ReadSelfCheckFlg())
                    {
						FCT_DEBUG("\r\nERROR:NOTINSERTED\r\n");
                    }
                }
				else
				if (simStat == SIM_STAT_PIN_REQ)
                {APP_DEBUG("\r\nERROR:SIMCARD PIN OPEN\r\n");
                    if(0xaa == FCT_ReadSelfCheckFlg())
                    {
						FCT_DEBUG("\r\nERROR:SIMCARD PIN OPEN\r\n");
                    }
                }
				else
				if (simStat == SIM_STAT_NOT_READY)
                {APP_DEBUG("\r\nERROR:SIMCARD NOT READY\r\n");
                    if(0xaa == FCT_ReadSelfCheckFlg())
                    {
						FCT_DEBUG("\r\nERROR:SIMCARD NOT READY\r\n");
                    }
                }
				else
                {
                //    Ql_Timer_Stop(TCP_TIMER_ID);
                APP_DEBUG("\r\nERROR:SIMCARD UNNORMAL\r\n");
                    if(0xaa == FCT_ReadSelfCheckFlg())
                    {
						FCT_DEBUG("\r\nERROR:SIMCARD UNNORMAL\r\n");
                    }
                }
                break;
            }        
            case STATE_NW_QUERY_STATE://等待GSM,SPRS网络注册好
            {
                s32 creg = 0;
                s32 cgreg = 0;
                //Ql_NW_GetNetworkState(&creg, &cgreg);
                ret = RIL_NW_GetGSMState(&creg);
                ret = RIL_NW_GetGPRSState(&cgreg);
                APP_DEBUG("<--Network State:creg=%d,cgreg=%d-->\r\n",creg,cgreg);
                if((cgreg == NW_STAT_REGISTERED)||(cgreg == NW_STAT_REGISTERED_ROAMING))
                {
                    m_tcp_state = STATE_GPRS_REGISTER;
                }
				else
				{
					if(!timeout_90S_monitor)//启动超定时器
					{
						Ql_Timer_Start(TIMEOUT_90S_TIMER_ID, TIMEOUT_90S_PERIOD, FALSE);
						timeout_90S_monitor = TRUE;
					}
				}
                break;
            }
            case STATE_GPRS_REGISTER://注册GPRS相关回调函数
            {
                ret = Ql_GPRS_Register(0, &callback_gprs_func, NULL);
                if (GPRS_PDP_SUCCESS == ret)
                {
                	if(0xaa == FCT_ReadSelfCheckFlg())
                    {
						FCT_DEBUG("\r\nOK:GPRS READY\r\n");
						PUBLIC_SETBIT(FCT_Test,1);
                    }
                    APP_DEBUG("<--Register GPRS callback function successfully.-->\r\n");
                    m_tcp_state = STATE_GPRS_CONFIG;
                }else if (GPRS_PDP_ALREADY == ret)
                {
                    APP_DEBUG("<--GPRS callback function has already been registered,ret=%d.-->\r\n",ret);
                    m_tcp_state = STATE_GPRS_CONFIG;
                }else
                {
                	if(0xaa == FCT_ReadSelfCheckFlg())
                    {
						FCT_DEBUG("\r\nERROR:GPRS FAIL TO SIGN IN\r\n");
                    }
                    APP_DEBUG("<--Register GPRS callback function failure,ret=%d.-->\r\n",ret);
                }
                break;
            }
            case STATE_GPRS_CONFIG://配置GPRS相关参数，包括APN ,拨号用户名，密码等
            {
                ret = Ql_GPRS_Config(0, &net_info.m_GprsConfig);
                if (GPRS_PDP_SUCCESS == ret)
                {
                    APP_DEBUG("<--configure GPRS param successfully.-->\r\n");
                }else
                {
                    APP_DEBUG("<--configure GPRS param failure,ret=%d.-->\r\n",ret);
                }
                
                m_tcp_state = STATE_GPRS_ACTIVATE;
                break;
            }
            case STATE_GPRS_ACTIVATE://激活网络
            {
                m_tcp_state = STATE_GPRS_ACTIVATING;
                ret = Ql_GPRS_Activate(0);
                if (ret == GPRS_PDP_SUCCESS)//激活网络成功
                {
                    APP_DEBUG("<--Activate GPRS successfully.-->\r\n");
                    m_tcp_state = STATE_GPRS_GET_DNSADDRESS;
                }else
                if (ret == GPRS_PDP_WOULDBLOCK)//等待激活网络成功，激活成功系统会自动调用回调函数 Callback_GPRS_Actived
                {  
                	if(!timeout_90S_monitor)//启动超定时器
                      {
                        Ql_Timer_Start(TIMEOUT_90S_TIMER_ID, TIMEOUT_90S_PERIOD, FALSE);
                        timeout_90S_monitor = TRUE;
                      }
                     APP_DEBUG("<--Waiting for the result of GPRS activated.,ret=%d.-->\r\n",ret);
                    //waiting Callback_GPRS_Actived
                }else
                if (ret == GPRS_PDP_ALREADY)//网络激活场景存在
                {
                    APP_DEBUG("<--GPRS has already been activated,ret=%d.-->\r\n",ret);
                    m_tcp_state = STATE_GPRS_GET_DNSADDRESS;
                }else//error
                {
                    APP_DEBUG("<--Activate GPRS failure,ret=%d.-->\r\n",ret);
                    m_tcp_state = STATE_GPRS_ACTIVATE;
                }
                break;
            }
            case STATE_GPRS_GET_DNSADDRESS://获取本地DNS服务地址
            {            
                u8 primaryAddr[16] = {0};
                u8 bkAddr[16] = {0};
                ret =Ql_GPRS_GetDNSAddress(0, (u32*)primaryAddr,  (u32*)bkAddr);
                if (ret == GPRS_PDP_SUCCESS)
                {
                    APP_DEBUG("<--Get DNS address successfully,primaryAddr=%d.%d.%d.%d,bkAddr=%d.%d.%d.%d-->\r\n",primaryAddr[0],primaryAddr[1],primaryAddr[2],primaryAddr[3],bkAddr[0],bkAddr[1],bkAddr[2],bkAddr[3]);            
                    m_tcp_state = STATE_GPRS_GET_LOCALIP;
                }else
                {
                     APP_DEBUG("<--Get DNS address failure,ret=%d.-->\r\n",ret);
                    m_tcp_state = STATE_GPRS_DEACTIVATE;
                }
                break;
            }
            case STATE_GPRS_GET_LOCALIP://获取本地IP地址
            {
                u8 ip_addr[5];
                Ql_memset(ip_addr, 0, 5);
                ret = Ql_GPRS_GetLocalIPAddress(0, (u32 *)ip_addr);
                if (ret == GPRS_PDP_SUCCESS)
                {
                    APP_DEBUG("<--Get Local Ip successfully,Local Ip=%d.%d.%d.%d-->\r\n",ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]);
                    m_tcp_state = STATE_CHACK_SRVADDR;
                }else
                {
                    APP_DEBUG("<--Get Local Ip failure,ret=%d.-->\r\n",ret);
                }
                break;
            }
            case STATE_CHACK_SRVADDR://检测服务器ip(域名)地址
            {
                Ql_memset(m_ipaddress,0,5);
				//Net_First_Value(MAIN_SERVER);
				//APP_DEBUG("<--MAIN_SERVER checking!-->\r\n");
                ret = Ql_IpHelper_ConvertIpAddr(net_info.ip, (u32 *)m_ipaddress);
                if(ret == SOC_SUCCESS) // ip address, xxx.xxx.xxx.xxx
                {
                    APP_DEBUG("<--Convert Ip Address successfully,m_ipaddress=%d,%d,%d,%d-->\r\n",m_ipaddress[0],m_ipaddress[1],m_ipaddress[2],m_ipaddress[3]);
                    m_tcp_state = STATE_SOC_REGISTER;
                    
                }else  //domain name
                {
                    ret = Ql_IpHelper_GetIPByHostName(0, 0, net_info.ip, GetIpByName_callback);
                    if(ret == SOC_SUCCESS)
                    {
                        APP_DEBUG("<--Get ip by hostname successfully.-->\r\n");
                    }
                    else if(ret == SOC_WOULDBLOCK)
                    {
                        APP_DEBUG("<--Waiting for the result of Getting ip by hostname,ret=%d.-->\r\n",ret);
                        //waiting CallBack_getipbyname
                    }
                    else
                    {
                        APP_DEBUG("<--Get ip by hostname failure:ret=%d-->\r\n",ret);
                        if(ret == SOC_BEARER_FAIL)  
                        {
                             m_tcp_state = STATE_GPRS_DEACTIVATE;
                        }
                        else
                        {
                             m_tcp_state = STATE_GPRS_GET_DNSADDRESS;
                        } 
                    }
                }
                break;
            }
            case STATE_SOC_REGISTER://注册网络
            {
                ret = Ql_SOC_Register(callback_soc_func, NULL);
                if (SOC_SUCCESS == ret)
                {
                    APP_DEBUG("<--Register socket callback function successfully.-->\r\n");
                    m_tcp_state = STATE_SOC_CREATE;
                }else if (SOC_ALREADY == ret)
                {
                    APP_DEBUG("<--Socket callback function has already been registered,ret=%d.-->\r\n",ret);
                    m_tcp_state = STATE_SOC_CREATE;
                }else
                {
                    APP_DEBUG("<--Register Socket callback function failure,ret=%d.-->\r\n",ret);
                }
                break;
            }
            case STATE_SOC_CREATE://创建一个TCP链接
            {
                m_socketid = Ql_SOC_Create(0, SOC_TYPE_TCP);
                if (m_socketid >= 0)
                {
                    APP_DEBUG("<--Create socket id successfully,socketid=%d.-->\r\n",m_socketid);
                    m_tcp_state = STATE_SOC_CONNECT;
                }else
                {
                    APP_DEBUG("<--Create socket id failure,error=%d.-->\r\n",m_socketid);
                }
                break;
            }
            case STATE_SOC_CONNECT://建立一个TCP链接
            {
                m_tcp_state = STATE_IDLE;
                ret = Ql_SOC_Connect(m_socketid,(u32) m_ipaddress, net_info.port);
                if(ret == SOC_SUCCESS)
                {
                    APP_DEBUG("<--The socket is already connected.-->\r\n");
                    m_tcp_state = STATE_SOC_CLOSE;//进入关闭状态
					
					ret=Ql_Timer_Stop(TCP_TIMER_ID);//关闭TCP链接初始化定时器
					if(ret <0)
					{
						//保存错误代号
						APP_DEBUG("\r\n<--failed!!,CLOSE TCP_TIMER_ID:(%d)fail ,ret = %d -->\r\n",TCP_TIMER_ID,ret);
					}
					else
					{
					 	APP_DEBUG("\r\n<--CloseTCP_TIMER_ID Success! -->\r\n");
					}
					//Net_First_Login();//连接登录，主要是触发注册鉴权任务
                }else if(ret == SOC_WOULDBLOCK)
                {
                      if (!timeout_90S_monitor)//start timeout monitor
                      {
                        Ql_Timer_Start(TIMEOUT_90S_TIMER_ID, TIMEOUT_90S_PERIOD, FALSE);
                        timeout_90S_monitor = TRUE;
                      }
                      APP_DEBUG("<--Waiting for the result of socket connection,ret=%d.-->\r\n",ret);
                      //waiting CallBack_getipbyname
                      
                }else //error
                {
                    APP_DEBUG("<--Socket Connect failure,ret=%d.-->\r\n",ret);
                    APP_DEBUG("<-- Close socket.-->\r\n");
                    Ql_SOC_Close(m_socketid);
                    m_socketid = -1;
                    
                    if(ret == SOC_BEARER_FAIL)  
                    {
                        m_tcp_state = STATE_GPRS_DEACTIVATE;
                    }
                    else
                    {
                        m_tcp_state = STATE_GPRS_GET_DNSADDRESS;
                    }
                }
                break;
            }
            case STATE_IDLE:
            {
				;
                //m_tcp_state = STATE_SOC_SENDING;
                break;
            }  
			case STATE_SOC_CLOSE:
            {
				// 监测链路状态????
                break;
            } 
            case STATE_GPRS_DEACTIVATE:
            {
				u8 updateFlg = GetUpdateFlag();
				if(updateFlg)return;
                m_tcp_state = STATE_NW_GET_SIMSTATE;//重新走tcp初始化流程
                APP_DEBUG("<--Deactivate GPRS.-->\r\n");
                Ql_GPRS_Deactivate(0);
                break;
            }
            default:
                break;
        }    
    }
}

void Socket_Restart(void)
{
	Ql_SOC_Close(m_socketid);
	Ql_Timer_Start(TCP_TIMER_ID, TCP_TIMER_PERIOD, TRUE);//开启TCP链接初始化定时器
	m_tcp_state = STATE_NW_GET_SIMSTATE;//重新走tcp初始化流程
}


//GPRS激活成功回调函数
void GPRS_Actived_callback(u8 contexId, s32 errCode, void* customParam)
{
    if(errCode == SOC_SUCCESS)
    {
        APP_DEBUG("<--CallBack: active GPRS successfully.-->\r\n");
        m_tcp_state = STATE_GPRS_GET_DNSADDRESS;
    }else
    {
        APP_DEBUG("<--CallBack: active GPRS successfully,errCode=%d-->\r\n",errCode);
        m_tcp_state = STATE_GPRS_ACTIVATE;
    }      
}


//服务器IP地址转换回调函数
void GetIpByName_callback(u8 contexId, u8 requestId, s32 errCode,  u32 ipAddrCnt, u32* ipAddr)
{
    u8 i=0;
    u8* ipSegment = (u8*)ipAddr;
    
    APP_DEBUG("<-- %s:contexid=%d, requestId=%d,error=%d,num_entry=%d -->\r\n", __func__, contexId, requestId,errCode,ipAddrCnt);
    if (errCode == SOC_SUCCESS)
    {
        APP_DEBUG("<--CallBack: get ip by name successfully.-->\r\n");
        for(i=0;i<ipAddrCnt;i++)
        {
            ipSegment = (u8*)(ipAddr + i);
            APP_DEBUG("<--Entry=%d, ip=%d.%d.%d.%d-->\r\n",i,ipSegment[0],ipSegment[1],ipSegment[2],ipSegment[3]);
        }

        // Fetch the first ip address as the valid IP
        Ql_memcpy(m_ipaddress, ipAddr, 4);
        m_tcp_state = STATE_SOC_REGISTER;
    }
}

//static u8 gprs_deact_cnt = 0;

//TCP链接成功回调函数
void socket_connect_callback(s32 socketId, s32 errCode, void* customParam )
{
s32 ret;
//u32 csq, bit_err;
//static u8 connect_fail_cnt = 0;

    if (errCode == SOC_SUCCESS)
    {
        if (timeout_90S_monitor) //stop timeout monitor
        {
           Ql_Timer_Stop(TIMEOUT_90S_TIMER_ID);
           timeout_90S_monitor = FALSE;
		   APP_DEBUG("<-- Close timeout_90S_monitor...-->\r\n");
        }
		if(0xaa == FCT_ReadSelfCheckFlg())
        {
			FCT_DEBUG("\r\nOK:NORMAL NETWORK TEST NTP\r\n");
			PUBLIC_SETBIT(FCT_Test,2);
        }
        APP_DEBUG("<--Callback: socket connect successfully.-->\r\n");
        m_tcp_state = STATE_SOC_CLOSE;
		
		Ql_Timer_Stop(TCP_TIMER_ID);//关闭TCP链接初始化定时器
		if(ret <0)
		{
			//保存错误代号
			APP_DEBUG("\r\n<--failed!!,CLOSE TCP_TIMER_ID:(%d)fail ,ret = %d -->\r\n",TCP_TIMER_ID,ret);
		}
		else
		{
			APP_DEBUG("\r\n<--CloseTCP_TIMER_ID Success! -->\r\n");
		}
		//gprs_deact_cnt = 0;
		//connect_fail_cnt = 0;
		Net_First_Login();//连接登录，主要是触发注册鉴权任务
    }else
    {
    	if(0xaa == FCT_ReadSelfCheckFlg())
        {
			FCT_DEBUG("\r\nERROR:NETWORK TEST FAILURE NTP!\r\n");
        }
        APP_DEBUG("<--Callback: socket connect failure,(socketId=%d),errCode=%d-->\r\n",socketId,errCode);
        Ql_SOC_Close(socketId);
		Ql_Timer_Start(TCP_TIMER_ID, TCP_TIMER_PERIOD, TRUE);//开启TCP链接初始化定时器
        m_tcp_state = STATE_SOC_CREATE;
		ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);
/*		APP_DEBUG("<--connect_fail_cnt=%d-->\r\n",connect_fail_cnt);
		
        if(RIL_AT_SUCCESS == RIL_NW_GetSignalQuality((u32*)&csq, (u32*)&bit_err))
		{
			if((csq > 10)&&(csq <= 31))
			{
				if(connect_fail_cnt++ > 100)
				{
					connect_fail_cnt = 0;
					APP_DEBUG("<--100次你都连不上还是重启吧,connect_fail_cnt=%d-->\r\n",connect_fail_cnt);
					Blind_UpdateCirQueueInfo();
					Report_RtcWakeUpOffsetSave();
					Ql_Reset(0);
					//Ql_RIL_SendATCmd("AT+CFUN=1", 9, NULL, NULL, 0);
				}
			}
		}*/
    }
}

//TCP链接关闭回调函数
void socket_close_callback(s32 socketId, s32 errCode, void* customParam )
{
    m_nSentLen  = 0;
    ClearTerminalAuthorizationFlag(CHANNEL_DATA_1); 
	Ql_Timer_Start(TCP_TIMER_ID, TCP_TIMER_PERIOD, TRUE);//开启TCP链接初始化定时器
	
    if (errCode == SOC_SUCCESS)
    {
        APP_DEBUG("<--CallBack: close socket successfully.-->\r\n"); 
    }else if(errCode == SOC_BEARER_FAIL)
    {   
        m_tcp_state = STATE_GPRS_DEACTIVATE;
        APP_DEBUG("<--CallBack: close socket failure,(socketId=%d,error_cause=%d)-->\r\n",socketId,errCode); 
    }else
    {
        m_tcp_state = STATE_GPRS_GET_DNSADDRESS;
        APP_DEBUG("<--CallBack: close socket failure,(socketId=%d,error_cause=%d)-->\r\n",socketId,errCode); 
    }
}

void socket_accept_callback(s32 listenSocketId, s32 errCode, void* customParam )
{  
}


//TCP收到服务端数据回调函数，当收到平台的数据的时候会调用该函数
void socket_read_callback(s32 socketId, s32 errCode, void* customParam )
{
    s32 ret;
    if(errCode)
    {
        APP_DEBUG("<--CallBack: socket read failure,(sock=%d,error=%d)-->\r\n",socketId,errCode);
        APP_DEBUG("<-- Close socket.-->\r\n");
        Ql_SOC_Close(socketId);
		Ql_Timer_Start(TCP_TIMER_ID, TCP_TIMER_PERIOD, TRUE);//开启TCP链接初始化定时器
        m_socketid = -1;
        if(errCode == SOC_BEARER_FAIL)  
        {
            m_tcp_state = STATE_GPRS_DEACTIVATE;
        }
        else
        {
            m_tcp_state = STATE_GPRS_GET_DNSADDRESS;
        }  
        return;
    }


    Ql_memset(m_recv_buf, 0, RECV_BUFFER_LEN);
    do
    {
        ret = Ql_SOC_Recv(socketId, m_recv_buf, RECV_BUFFER_LEN);

        if((ret < 0) && (ret != -2))
        //if(ret < 0)
        {
            APP_DEBUG("<-- Receive data failure,ret=%d.-->\r\n",ret);
            APP_DEBUG("<-- Close socket.-->\r\n");
            Ql_SOC_Close(socketId); //you can close this socket  
			Ql_Timer_Start(TCP_TIMER_ID, TCP_TIMER_PERIOD, TRUE);//开启TCP链接初始化定时器
            m_socketid = -1;
            m_tcp_state = STATE_SOC_CREATE;
            break;
        }
        else if(ret == -2)
        {
            //wait next CallBack_socket_read
            break;
        }
        else if(ret < RECV_BUFFER_LEN)
        {
            APP_DEBUG("<--Receive data from sock(%d),len(%d):%s\r\n",socketId,ret,m_recv_buf);
            break;
        }else if(ret == RECV_BUFFER_LEN)
        {
            APP_DEBUG("<--Receive data from sock(%d),len(%d):%s\r\n",socketId,ret,m_recv_buf);
        }
    }while(1);
	
	if((ret>0))
	{
		RadioProtocol_CheckRecvData(m_recv_buf,ret);
	}
}



//发送回调函数，当缓存中一次数据没有发送完成，会调用该函数
void socket_write_callback(s32 socketId, s32 errCode, void* customParam )
{
    s32 ret;

    if(errCode)
    {
        APP_DEBUG("<--CallBack: socket write failure,(sock=%d,error=%d)-->\r\n",socketId,errCode);
        APP_DEBUG("<-- Close socket.-->\r\n");
        Ql_SOC_Close(socketId);
        m_socketid = -1;
        
		Ql_Timer_Start(TCP_TIMER_ID, TCP_TIMER_PERIOD, TRUE);//开启TCP链接初始化定时器
		
        if(errCode == SOC_BEARER_FAIL)  
        {
            m_tcp_state = STATE_GPRS_DEACTIVATE;
        }
        else
        {
            m_tcp_state = STATE_GPRS_GET_DNSADDRESS;
        }  
        return;
    }

    do
    {
        ret = Ql_SOC_Send(m_socketid, m_pCurrentPos, m_remain_len);
        APP_DEBUG("<--CallBack: Send data,socketid=%d,number of bytes sent=%d-->\r\n",m_socketid,ret);

        if(ret == m_remain_len)//send compelete
        {
            m_remain_len = 0;
            m_pCurrentPos = NULL;
            m_nSentLen += ret;
            //m_tcp_state = STATE_SOC_ACK;
            break;
         }
         else if((ret < 0) && (ret == SOC_WOULDBLOCK)) 
         {
            //you must wait CallBack_socket_write, then send data;     
            break;
         }
         else if(ret < 0)
         {
              APP_DEBUG("<--Send data failure,ret=%d.-->\r\n",ret);
              APP_DEBUG("<-- Close socket.-->\r\n");
              Ql_SOC_Close(socketId);//error , Ql_SOC_Close
              
			  Ql_Timer_Start(TCP_TIMER_ID, TCP_TIMER_PERIOD, TRUE);//开启TCP链接初始化定时器
			  
              m_socketid = -1;

              m_remain_len = 0;
              m_pCurrentPos = NULL; 
              if(ret == SOC_BEARER_FAIL)  
              {
                  m_tcp_state = STATE_GPRS_DEACTIVATE;
              }
              else
              {
                  m_tcp_state = STATE_GPRS_GET_DNSADDRESS;
              }       
              break;
			  
        }
        else if(ret < m_remain_len)//continue send, do not send all data
        {
            m_remain_len -= ret;
            m_pCurrentPos += ret; 
            m_nSentLen += ret;
        }
     }while(1);
}

void GPRS_Deactived_callback(u8 contextId, s32 errCode, void* customParam )
{
	u8 updateFlg = GetUpdateFlag();
	if(updateFlg)return;
	//ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);  
    if (errCode == SOC_SUCCESS)
    {
        APP_DEBUG("<--CallBack: deactived GPRS successfully.-->\r\n"); 
		//if(ReadSleepFlag()!=2)
			{
				ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);  
				Ql_Timer_Start(TCP_TIMER_ID, TCP_TIMER_PERIOD, TRUE);//开启TCP链接初始化定时器
			    m_tcp_state = STATE_NW_GET_SIMSTATE;
			}
			/*APP_DEBUG("<--deactived count is %d.-->\r\n",gprs_deact_cnt);
			gprs_deact_cnt++;
			if(gprs_deact_cnt >= 50){
			Blind_UpdateCirQueueInfo();
			Report_RtcWakeUpOffsetSave();
			Ql_Reset(0);
			}*/
    }else
    {
        APP_DEBUG("<--CallBack: deactived GPRS failure,(contexid=%d,error_cause=%d)-->\r\n",contextId,errCode); 
    }
}

void GsmDrop_callback(void)
{
	u8 updateFlg = GetUpdateFlag();
	if(updateFlg)return;
	ClearTerminalAuthorizationFlag(CHANNEL_DATA_1); 
	if(m_tcp_state == STATE_SOC_CLOSE)
		{
      		 APP_DEBUG("\r\n<--GsmDrop CallBack_GsmDrop-->\r\n"); 
			ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);  
			Ql_Timer_Start(TCP_TIMER_ID, TCP_TIMER_PERIOD, TRUE);//开启TCP链接初始化定时器
			m_tcp_state = STATE_NW_GET_SIMSTATE;
		}

}
extern s32 m_pwrOnReason;
//////////////////////////////////////////////////////////////////////////////////////
//鉴权相关
//////////////////////////////////////////////////////////////////////////////////////
/**
  * @brief  连接登录，主要是触发注册鉴权任务
  * @param  type: 保留 
            state: 下一步执行的状态
  * @retval None
  */
void Net_First_Login( void)
{
	ConnectInfo.ipConnectFailureCnt=0;
    ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);  

	SetLoginAuthorizationState(LOGIN_READ_AUTHORIZATION_CODE);
	//不走注册鉴权流程
	//SetTimerTask(TIME_LINK1_LOGIN, SYSTICK_1SECOND); 
	SetTerminalAuthorizationFlag(CHANNEL_DATA_1);
	//显示鉴权成功
	RIL_GetPowerSupply(&battery_capacity, &battery_voltage);
	if(RTCPWRON != m_pwrOnReason)//如果不是闹钟唤醒就上报一条位置信息
	{
		//Report_UploadPositionInfo();
		Report_ClearReportTimeCount();
		Report_UpdatePram();
		//设置闹钟
		ST_Time dateTime;
		u64 mkTime;
		u32 interval,temp;
		interval = Report_GetDefaultReportInterval();//获取上报间隔
		if(interval >= 30*60)
		{
			Ql_GetLocalTime(&dateTime);//获取实时时间
			mkTime = Ql_Mktime(&dateTime);			
			temp = mkTime%interval;//距离下次需要上报的时间
			if(temp > 10*60)//大于10分钟就设置闹钟睡觉关机
			{
				mkTime += temp;
				mkTime -= 60;//设置闹钟提前60s开机
				Ql_MKTime2CalendarTime(mkTime, &dateTime);
				dateTime.year -= 2000;
				RIL_Alarm_Create(&dateTime,0);
				//关机前喂一次狗
				GpioOutInit(WATCHDOG,PINLEVEL_HIGH);//配置成输出
				HARD_FEED_DOG();
				Ql_Sleep(200);
				Ql_PowerDown(1);//关机模式
			}
		}
	}
#if	( !configUSE_GNSS )
	Report_SetReportStatus(ReportStatus_Init);
#endif
	APP_DEBUG("\r\n<--!!!链接成功回调!!!!-->\r\n");
	Blind_GiveSemaphore();
	APP_DEBUG("\r\n<--!!!Blind_GiveSemaphore!!!!-->\r\n");

}

//初始化TCP相关定时器
void init_tcp(void)
{
	Net_First_Value(MAIN_SERVER);//初始化TCP端口相关参数
    //register & start timer 
    Ql_Timer_Register(TCP_TIMER_ID, Timer_Callback, NULL);
    Ql_Timer_Start(TCP_TIMER_ID, TCP_TIMER_PERIOD, TRUE);

    Ql_Timer_Register(TIMEOUT_90S_TIMER_ID, Timer_Callback, NULL);
	
    timeout_90S_monitor = FALSE;
		ConnectInfo.ipConnectFailureCnt=0;
}

//鉴权过程
FunctionalState  Link1Login_TimeTask(void)
{
  u8  ucTmp[30];
  static u8 errorcount = 0;	
#ifdef MODEM_APP_SECOND_H
  if(CHANNEL_DATA_1 == (CHANNEL_DATA_1&GetTerminalAuthorizationFlag()))
#else
    if(GetTerminalAuthorizationFlag())
#endif
    {
      APP_DEBUG("<--!!!鉴权成功关闭任务!!!!-->\r\n");
      Link1LoginAuthorizationState = LOGIN_CLOSE_TASK;
    }
  //////////////
  switch(Link1LoginAuthorizationState)
  {
  case LOGIN_READ_AUTHORIZATION_CODE://读取鉴权码
    {
      Link1LoginCount = 0;
      errorcount = 0;
      Link1AuthorizationCount = 0;
      Link1LoginAuthorizationLastState = 0;
      Link1LoginAuthorizationDelayCount = 0;
      if(0 == EepromPram_ReadPram(E2_LOGINGPRS_PASSWORD_ID, ucTmp))//失败
      {
        Link1LoginAuthorizationState = LOGIN_SEND_LOGIN_CMD;
		APP_DEBUG("<-- 读取鉴权码失败-->\r\n");
      }
      else//成功
      {
        Link1LoginAuthorizationState = LOGIN_SEND_AUTHORIZATION_CMD;
		APP_DEBUG("<-- 读取鉴权码成功，下一步发送鉴权命令 -->\r\n");
      }
      break;
    }
  case LOGIN_SEND_LOGIN_CMD://发送注册命令
    {
		APP_DEBUG("<-- 发送注册命令 -->\r\n");
      if(ACK_OK == RadioProtocol_TerminalRegister())
      {
      
	  APP_DEBUG("<-- 发送注册命令成功-->\r\n");
       // Net_Other_Printf(PRTF_REGISTER);
        Link1LoginAuthorizationDelayCount = 0;
        Link1LoginAuthorizationDelayTime = 30;//注册鉴权延时时间
        Link1LoginAuthorizationLastState = LOGIN_SEND_LOGIN_CMD;//上一次状态
        Link1LoginCount++;
        if(Link1LoginCount >= 3)
        {
          Link1LoginAuthorizationState = LOGIN_CLOSE_GPRS;
        }
        else
        {
          Link1LoginAuthorizationState = LOGIN_DELAY;
        }
        errorcount = 0;
      }
      else
      {
      
	  APP_DEBUG("<-- 发送注册命令失败-->\r\n");
        errorcount++;
        if(errorcount >= 200)
        {
          Link1LoginAuthorizationState = LOGIN_CLOSE_TASK;
        }
      }
      break;
    }
  case LOGIN_SEND_AUTHORIZATION_CMD://发送鉴权命令
    {
	APP_DEBUG("<-- 发送鉴权命令-->\r\n");	
      if(ACK_OK == RadioProtocol_TerminalQualify())
      {
      
	    APP_DEBUG("<-- 发送鉴权命令成功-->\r\n");
        //Net_Other_Printf(PRTF_AUTHORIZATION);
        Link1LoginAuthorizationDelayCount = 0;//注册流程延时计数器清零
        Link1LoginAuthorizationDelayTime = 30;//注册鉴权延时时间,给1秒做网络延时
        Link1LoginAuthorizationLastState = LOGIN_SEND_AUTHORIZATION_CMD;//上一次状态
        Link1AuthorizationCount++;
        if(Link1AuthorizationCount >= 3)
        {
          Link1LoginAuthorizationState = LOGIN_SEND_LOGIN_CMD;
        }
        else
        {
          Link1LoginAuthorizationState = LOGIN_DELAY;
        }
        errorcount = 0;
      }
      else
      {
      
	  	APP_DEBUG("<-- 发送鉴权命令失败-->\r\n");
        errorcount++;
        if(errorcount >= 200)
        {
          Link1LoginAuthorizationState = LOGIN_CLOSE_TASK;
        }
      }
      break;
    }
  case LOGIN_CLOSE_GPRS://断开网络
    {
	  APP_DEBUG("<-- 鉴权失败断开网络 -->\r\n");
      Net_First_Close();
      Link1ManualCloseFlag = 1;
      ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);//鉴权标志清0
      ClearTerminalAuthorizationCode(CHANNEL_DATA_1);
      Link1LoginAuthorizationDelayCount = 0;
      Link1LoginAuthorizationDelayTime = 10*60;//注册鉴权延时时间，真服务器延时10分钟
      Link1LoginAuthorizationLastState = LOGIN_RECONNECT_GPRS;
      Link1LoginAuthorizationState = LOGIN_DELAY;	
      break;
      
    }
  case LOGIN_DELAY://延时
    {
      Link1LoginAuthorizationDelayCount++;
      if(Link1LoginAuthorizationDelayCount >= Link1LoginAuthorizationDelayTime)
      {
        Link1LoginAuthorizationDelayCount = 0;
        Link1LoginAuthorizationState = Link1LoginAuthorizationLastState;
      }
      break;
    }
  case LOGIN_RECONNECT_GPRS://重连网络
    {
	APP_DEBUG("<-- 重连网络 -->\r\n");
      Link1LoginCount = 0;
      errorcount = 0;
      Link1AuthorizationCount = 0;
      Link1LoginAuthorizationLastState = 0;
      Link1LoginAuthorizationState = 0;
      Link1LoginAuthorizationDelayCount = 0; 
      Net_First_Open();
      Link1ManualCloseFlag = 0;
      Link1LoginAuthorizationState = LOGIN_CLOSE_TASK;
      break;
    }
  case LOGIN_CLOSE_TASK://关闭任务
    {
	  /*
      if(1 == Link1ManualCloseFlag)
      {
        Net_First_Open();
        Link1ManualCloseFlag = 0;
      }
      */
      Link1LoginCount = 0;
      errorcount = 0;
      Link1AuthorizationCount = 0;
      Link1LoginAuthorizationLastState = 0;
      Link1LoginAuthorizationState = 0;
      Link1LoginAuthorizationDelayCount = 0; 
	  //ClrTimerTask(Msg_Link1Login_TimeTask);
	  return DISABLE;
      break;
    }
  default:
    {
      Link1LoginAuthorizationState = LOGIN_CLOSE_TASK;
    }
    break;
  }
  return ENABLE;
}

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
void ClearTerminalAuthorizationCode(u8 channel)
{   
/*
    #ifdef MODEM_APP_SECOND_H
    if(CHANNEL_DATA_1 == (channel&CHANNEL_DATA_1))
    {
         System_ClearLoginGprsPassword();
    }
    //////////////
    if(CHANNEL_DATA_2 == (channel&CHANNEL_DATA_2))
    {
        for(i=0; i<20; i++)
        {
            E2prom_WriteByte(E2_SECOND_LOGINGPRS_PASSWORD_ID_ADDR+i,0xff);
        }
    }
    #else
    */
    System_ClearLoginGprsPassword();
    //#endif
}

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
void System_ClearLoginGprsPassword(void)
{
    unsigned char buffer[20];
    memset(buffer,0xff,20);
    E2prom_WriteBytes(PRAM_FILE_NAME,E2_LOGINGPRS_PASSWORD_ID_ADDR,buffer,20);
}
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
u8 GetTerminalAuthorizationFlag(void)
{
	return 	TerminalAuthorizationFlag;
}
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
void SetTerminalAuthorizationFlag(u8 channel)
{

    #ifdef MODEM_APP_SECOND_H
    if(CHANNEL_DATA_1 == (channel&CHANNEL_DATA_1))
    {
        TerminalAuthorizationFlag = TerminalAuthorizationFlag | CHANNEL_DATA_1;
    }
    
    if(CHANNEL_DATA_2 == (channel&CHANNEL_DATA_2))
    {
        TerminalAuthorizationFlag = TerminalAuthorizationFlag | CHANNEL_DATA_2;
    }
    #else
    TerminalAuthorizationFlag = 1;
   // g_stModemStatus.curOffLineTimeCnt = 0;
    #endif
	
}
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
void ClearTerminalAuthorizationFlag(u8 channel)
{
    #ifdef MODEM_APP_SECOND_H
    if(CHANNEL_DATA_1 == (channel&CHANNEL_DATA_1))
    {
        TerminalAuthorizationFlag = TerminalAuthorizationFlag & 0xfd;
    }
    if(CHANNEL_DATA_2 == (channel&CHANNEL_DATA_2))
    {
        TerminalAuthorizationFlag = TerminalAuthorizationFlag & 0xfb;
    }
    #else
    TerminalAuthorizationFlag = 0;
    #endif
}

/*******************************************************************************
* Function Name  : SetLoginAuthorizationState
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetLoginAuthorizationState(u8 State)
{
    Link1LoginCount                 = 0;
    Link1AuthorizationCount         = 0;
    Link1LoginAuthorizationLastState= 0;
    Link1LoginAuthorizationState    = State;        
}

/**
  * @brief  发送缓冲数据到中心
  * @param    channel:数据通道
  * @param    srcdat :指向发送数据
  * @param    srclen :发送数据长度
  * @retval 返回发送结果
  *                              0 :失败  
  *                     等于srclen :成功
  */
u16 Communication_SndMsg2Svr(u8 channel, u8 * srcdat, u16 srclen)
{
    u16 len = 0;
	s32 ret;
	u64  ackNum = 0;
    if(channel == CHANNEL_SMS)
    {
        #ifdef EYE_USE_SMS
        len = srclen;
        //Modem_Api_SmsSend(SMS_PDU,NULL,srcdat,srclen);
        #endif
    }
//    #ifdef MODEM_APP_SECOND_H
//    else if(channel == CHANNEL_DATA_2)
//    {
//       	len = Net_Second_Send(srcdat,srclen);
//	  }
//    #endif
    else 
	if (m_socketid >= 0)
	///////////////////第一通道
    {
    
         ret = Ql_SOC_Send(m_socketid, (u8*)srcdat, srclen);
        if (ret == srclen)
        {
            APP_DEBUG("<-- Send socket data successfully. --> \r\n");
        }else{
            APP_DEBUG("<-- Fail to send socket data. --> \r\n");
            Ql_SOC_Close(m_socketid);
			m_socketid=-1;
            return 0;
        }
/*
        // Check ACK number
        do 
        {
            ret = Ql_SOC_GetAckNumber(m_SocketId, &ackNum);
            APP_DEBUG("<-- Current ACK Number:%llu/%d --> \r\n", ackNum, srclen);
            Ql_Sleep(500);
        } while (ackNum != srclen);
        */
        //APP_DEBUG("<-- Server has received all data --> \r\n");
        
    	return len=ret;
    }
	return 0;
}


/*
  * @brief  打开连接
  * @param  None
  * @retval None
  */
void Net_First_Open(void)
{

}
/*
  * @brief  关闭连接
  * @param  None
  * @retval None
  */
void Net_First_Close(void)
{
    s32 ret;
    //Close socket
    ret = Ql_SOC_Close(m_socketid);
    APP_DEBUG("<-- Close socket[%d], cause=%d --> \r\n", m_socketid, ret);
    //Deactivate GPRS
    APP_DEBUG("<-- Deactivating GPRS... -->\r\n");
	ret = Ql_GPRS_Deactivate(0);
    //ret = Ql_GPRS_DeactivateEx(pdpCntxtId, TRUE);
    APP_DEBUG("<-- Deactivated GPRS, cause=%d -->\r\n\r\n", ret);
}

/**
  * @brief  接获取参数
  * @param  type: 参数类型
  * @retval 返回参数结构 1->成功  0->非法数据
  */
u8 Net_First_Value(ServerType type )
{
    u8  tab[50]={0};
    u8  len;
    u32 id_ip;
    u16 temp;

	//////////////////////////////////////////////////////////////////鉴权标志清
    ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);  
	
	ConnectInfo.curIpType=type;
	//ConnectInfo.ipConnectFailureCnt=0;
	
    ////////////////////
    if(type==MAIN_SERVER)
    	{
	    len = EepromPram_ReadPram(E2_MAIN_SERVER_IP_ID, tab);    
	    if((0==len)||(len >= 30))
			{
			APP_DEBUG("<-- Net_First_Value Fail len=%d -->\r\n",len);
			return 0;
			}
	    memcpy(net_info.ip,tab,len); 
		net_info.ip[len]='\0';
		APP_DEBUG("<-- E2_MAIN_SERVER_IP_ID= %slen %d -->\r\n", net_info.ip,len);
    	}
	else
		{
	    len = EepromPram_ReadPram(E2_BACKUP_SERVER_IP_ID, tab);    
	    if((0==len)||(len >= 30))
			{
			APP_DEBUG("<-- Net_First_Value Fail len=%d -->\r\n",len);
			return 0;
			}
	    memcpy(net_info.ip,tab,len); 
		net_info.ip[len]='\0';
		APP_DEBUG("<-- E2_BACKUP_SERVER_IP_ID= %slen %d -->\r\n", net_info.ip,len);
		}
    //////////////////
    len = EepromPram_ReadPram(E2_MAIN_SERVER_TCP_PORT_ID, tab);    
    if((0==len)||(len >= 5))return 0;
    /////////////////////
    temp = Public_ConvertBufferToLong(tab); 
	/*
    #if 0
    if(temp)//modify by joneming
    {
        strcpy((char*)pstip->mode,"TCP");//模式
    }
    else
    { 
        if(EepromPram_ReadPram(E2_MAIN_SERVER_UDP_PORT_ID, tab))
        {
            temp = Public_ConvertBufferToLong(tab);
            if(temp)
            {
                strcpy((char*)pstip->mode,"UDP");//模式
            }
        }
    }
    #else
    strcpy((char*)pstip->mode,"TCP");//模式
    #endif
    */
    //Public_itoaEx(net_info->port,temp);
    net_info.port=(u32)temp;
    ////////////////////
    return 1;//参数可用
}


