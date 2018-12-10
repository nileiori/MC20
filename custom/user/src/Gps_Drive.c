#include"include.h"


/*******************************************************************************************************
EPOTM 运行流程（二）
部分应用场景可能对功耗要求的较为严格，可以在网络注册成功，时间同步完成的情况下，再去开启
GNSS 定位。在定位完成后， GNSS 断电，以节约功耗。

1) 模块开机后， 配置 PDP context 和 APN。目前 EPOTM的相应操作仅支持在 PDP context 2 下进行；
2) 确认网络是否注册成功；
3) 确认时间同步完成。模块开机后会利用 NITZ 自动更新本地时间。 若某地区的运营商不支持此功能，则
需要手动利用 NTP 进行时间的同步；
4) 使能 EPOTM功能；
5) 发送 AT+QGNSSC=1 命令开启 GNSS 功能；
6) 获取 NMEA 信息；
7) 发送 AT+QGNSSC=0 命令关闭 GNSS 功能

********************************************************************************************************/
//网络同步时间ip、端口
#define NTP_SERVER   	"202.120.2.101"
#define NTP_PORT        (u16)123
static s8 gNtpRestult= -1;//网络同步时间标志


u8 m_gps_state = STATE_WAIT_GPRS_NET;//GPS定时器的 初始化状态


static bool epo_flag = FALSE;		//判断EPO  (GNSS)是否使能
static ST_LocInfo locinfo={0};//基站获取的经纬度

#define RBUF_SIZE  1024
static u8 GpsTempBuffer[RBUF_SIZE];
static u16 GpsTempCount = 0;

u8 GprmcBuffer[GPRMC_BUFFER_SIZE];
u8 GprmcBufferBusyFlag = 0;
u8 GpggaBuffer[GPGGA_BUFFER_SIZE];
u8 GpggaBufferBusyFlag = 0;

extern GPS_STRUCT	gPosition;//当前有效位置
extern GPS_STRUCT	 Position;//临时解析用

static void Callback_NTP(char *strURC);	//时间同步返回函数
void Callback_Location(s32 result, ST_LocInfo* loc_info);			//基站返回函数
void Callback_Timer_GPS(u32 timeId,void *param);
static void GpsReciveData(u8 *pBuffer, u16 length);
void Gps_NMEA_Hdlr(u8 *multi_nmea, u16 len, void *customizePara);



//GPS定位相关定时器初始化
void init_GPStimer(void)
{ 	
	s32 ret;
	//注册GPS定位定时器
	ret = Ql_Timer_Register(TIME_GPS_ID, Callback_Timer_GPS, NULL);
	if(ret <0)
    {
		//保存错误代号
        APP_DEBUG("\r\n<--failed!!,init_GPStimer Ql_Timer_Register: timer(%d) fail ,ret = %d -->\r\n",TIME_GPS_ID,ret);
    }
	else
	{
		APP_DEBUG("\r\n<--init_GPStimer OK -->\r\n");
	}
	//注册GPS超时定时器
	ret = Ql_Timer_Register(TIMEOUT_TIME_GPS_ID, Callback_Timer_GPS , NULL); 
	if(ret <0)
    {
		//保存错误代号
        APP_DEBUG("\r\n<--failed!!,init_GPStimer Ql_Timer_Register: timer(%d) fail ,ret = %d -->\r\n",TIMEOUT_TIME_GPS_ID,ret);
    }
	else
	{
		APP_DEBUG("\r\n<--init_GPStimer TimeOut OK -->\r\n");
	}
	
	if(ReadSleepFlag()==2)
		{
			GpsOnOffFlag=0;
		}
	else
		{
			GpsOnOffFlag=1;
			JO_OpenGPSTimer();
		}
}
//开启GPS定位定时器
void JO_OpenGPSTimer(void)
{
	s32 ret;

	m_gps_state = STATE_WAIT_GPRS_NET;//GPS定时器的 初始化状态
	ret=Ql_Timer_Start(TIME_GPS_ID, GPS_TIMER_PERIOD, TRUE);
	if(ret <0)
    {
		//保存错误代号
        APP_DEBUG("\r\n<--failed!!,OpenGPSTimer: timer(%d) fail ,ret = %d -->\r\n",TIME_GPS_ID,ret);
    }
	else
	{
		APP_DEBUG("\r\n<--OpenGPSTimer Success! -->\r\n");
	}
	
	JO_OpenGPSOutTimer();//开启超时定时器
} 
void SentGnssOnMsgToTask(void)
{
	Ql_OS_SendMessage (main_task_id, MSG_ID_GNSS_CTR, MSG_GNSS_ON,0);

}
	
//开启GPS超时定时器
void JO_OpenGPSOutTimer(void)
{
	s32 ret;

	ret=Ql_Timer_Start(TIMEOUT_TIME_GPS_ID, TIMEOUT_30S_PERIOD, FALSE);

	if(ret <0)
	{
		//保存错误代号
		APP_DEBUG("\r\n<--failed!!,OpenGPSOutTimer: timer(%d) fail ,ret = %d -->\r\n",TIME_GPS_ID,ret);
	}
	else
	{
	 	APP_DEBUG("\r\n<--OpenGPSOutTimer Success! -->\r\n");
	}

	
}

void CtrGnssOn(void)
{
	m_gps_state = STATE_EPO_START;
	
}

//外部关闭GNSS，调用该函数，GNSS关闭，同时控制GNSS状态定时器会关闭
void CtrGnssOFF(void)
{
	m_gps_state = STATE_GNSS_OFF;
	
}
//返回gnss epo状态
bool Gnss_ReadStatus(void)
{
	return epo_flag;
}
/******************************************************************************
*	类别:定时器返回函数
*	功能:  <1> 获取GPS数据超时后的处理
			<2> 获取GPS数据流程
*	参数:定时Id
*	返回:无
******************************************************************************/
void Callback_Timer_GPS(u32 timeId,void *param)
{	
	//GPS  定位超时
	if(TIMEOUT_TIME_GPS_ID==  timeId)
	{	
		APP_DEBUG("\r\n<--TIMEOUT_TIME_GPS m_gps_state=%d-->\r\n",m_gps_state);
		s32 ret;
		if(m_gps_state==m_gps_state)//网络注册时间超时直接开启GNSS
			{
				m_gps_state = STATE_EPO_START;
			}
		else if(m_gps_state==STATE_WAIT_LOC)//获取基站位置超时
			{
				m_gps_state = STATE_EPO_START;
			}
		Ql_Timer_Stop(TIMEOUT_TIME_GPS_ID);
	}
	//GPS 秒 定
	else  if(TIME_GPS_ID == timeId)
	{
	
		//APP_DEBUG("\r\n<--TIME_GPS_ID m_gps_state=%d-->\r\n",m_gps_state);	
		switch(m_gps_state)
		{
			case STATE_WAIT_GPRS_NET://等待MC20注册到网络
				{
					s32 creg = 0,cgreg = 0;
	                s32 ret;
	                //Ql_NW_GetNetworkState(&creg, &cgreg);
	                ret = RIL_NW_GetGSMState(&creg);
	                ret = RIL_NW_GetGPRSState(&cgreg);
	                APP_DEBUG("<--Network State:creg=%d,cgreg=%d-->\r\n",creg,cgreg);
	                if((cgreg == NW_STAT_REGISTERED)||(cgreg == NW_STAT_REGISTERED_ROAMING))
	                {
						Ql_Timer_Stop(TIMEOUT_TIME_GPS_ID);
	                    m_gps_state = STATE_TIME_SYNC;
	                }					
				}
				break;
			case STATE_TIME_SYNC://检查时间同步
				{
					u8 status = 0; 
					s32 ret;
					ret = RIL_GPS_Read_TimeSync_Status(&status);//检测网络是否同步成功
					APP_DEBUG("<--netSync=%d status=%d-->\r\n",ret,status);
					if(ret == RIL_ATRSP_SUCCESS)
					{
						if(status == 1)//已经同步
						{
							m_gps_state = STATE_GPS_NET;
						} 
						else
						{
							ret = RIL_NTP_START(NTP_SERVER, NTP_PORT, Callback_NTP);//未同步，网络同步
							if(ret == RIL_ATRSP_SUCCESS)
							{
								m_gps_state = STATE_NTP_WAIT;
							}
							else
							{
								m_gps_state = STATE_GPS_NET;
							}
						}
					}
					else//同步失败，直接打开GNSS
					{
						m_gps_state = STATE_GPS_NET;
					}
					break;
				}
			case STATE_NTP_WAIT://等待网络同步时间
				{
					if(gNtpRestult == -1)
					{
						APP_DEBUG("<--Waiting for NTP time sync result.-->\r\n");
					}
					else if(gNtpRestult == 0)			//NTP sync successful
					{	
						m_gps_state = STATE_GPS_NET;
						APP_DEBUG("<--NTP time sync successful.-->\r\n");
					}
					else
					{
						m_gps_state = STATE_EPO_START;
						APP_DEBUG("<--NTP time sync failed(%d), try to read nmea.-->\r\n", gNtpRestult);
					}
					break;
				}
			case STATE_GPS_NET:
				{
					s32 ret;
					s32 pdpCntxtId=0;
					// Set PDP context
						ret = Ql_GPRS_GetPDPContextId();
						APP_DEBUG("<-- The GPS PDP context id available is: %d (can be 0 or 1)-->\r\n", ret);
						if (ret >= 0)
						{
							pdpCntxtId = (u8)ret;
						} else {
							APP_DEBUG("<--GPS Fail to get PDP context id, try to use PDP id(0) -->\r\n");
							pdpCntxtId = 0;
						}
					
						ret = RIL_NW_SetGPRSContext(pdpCntxtId);
						APP_DEBUG("<--GPS Set PDP context id to %d -->\r\n", pdpCntxtId);
						if (ret != RIL_AT_SUCCESS)
						{
							APP_DEBUG("<--STATE_GPS_NET Ql_RIL_SendATCmd error  ret=%d-->\r\n",ret );
						}
					
						// Set APN
						APP_DEBUG("<-- GPS Set APN -->\r\n");
						ret = RIL_NW_SetAPN(1, "CMNET\0", "", "");
					m_gps_state = STATE_GET_LOC;	
				}
			case STATE_GET_LOC://获取基站位置
				{
					s32 ret=RIL_GetLocation(Callback_Location);
					if(RIL_AT_SUCCESS == ret)
					{
						APP_DEBUG("<--RIL_GetLocation success ret=%d-->\r\n",ret);
						m_gps_state = STATE_WAIT_LOC;
						JO_OpenGPSOutTimer();//开启超时定时器
					}
					break;
				}
			case STATE_WAIT_LOC://得到基站位置，设置基站位置
				{
					s32 ret; 
					if((locinfo.latitude !=0) && (locinfo.longitude!=0))
					{
						APP_DEBUG("\r\n<-- Module location: latitude=%f, longitude=%f -->\r\n",locinfo.latitude,locinfo.longitude);
						ret = reference_location();//秒定设置
						APP_DEBUG("\r\n<--reference_location, ret = %d.\r\n-->", ret);
						m_gps_state = STATE_EPO_START;
						Ql_Timer_Stop(TIMEOUT_TIME_GPS_ID);
					}
					break;	
				}
			case STATE_EPO_START://打开EPO，GNSS
				{
					if(!epo_flag)
					{
						s32 ret = RIL_GPS_EPO_Enable(1);
						if(RIL_AT_SUCCESS != ret)
						{
							APP_DEBUG("Set EPO status to %d fail\r\n", ret);
						}	
						else
						{
							APP_DEBUG("Set EPO status to %d successful\r\n", ret);
							#ifdef __GNSS_NEW_API__
			                ret = Ql_GNSS_PowerOn(RMC_EN | GGA_EN, Gps_NMEA_Hdlr, &Position);  // Also can use ALL_NMEA_EN to enable receiving all NMEA sentences.
			                if(RIL_AT_SUCCESS == ret) 
			                {
			                    epo_flag=TRUE;
								ret=Ql_Timer_Stop(TIME_GPS_ID);
								if(ret <0)
								{
									//保存错误代号
									APP_DEBUG("\r\n<--failed!!,STOP_GPSTimer: timer(%d) fail ,ret = %d -->\r\n",TIME_GPS_ID,ret);
								}
								APP_DEBUG("\r\n<--STOP_GPSTimer Success! -->\r\n");
			                }
			                #else
			                ret = RIL_GPS_Open(1);
							if(RIL_AT_SUCCESS == ret )
							{
								APP_DEBUG("Set GNSS status to  successful, ret = %d.\r\n",ret);
								epo_flag=TRUE;
								m_gps_state = STATE_CHECK_FIX;
							}
			                #endif	
						}
							
					}
					break;			
				}
			case STATE_CHECK_FIX://获取gps定位数据
				{
					RIL_GPS_Read("ALL", GpsTempBuffer);
					GpsTempCount=Ql_strlen(GpsTempBuffer);//获取GPS数据区长度
					GpsReciveData(GpsTempBuffer,GpsTempCount);//提取GPS数据
					break;
				}	
			case STATE_GNSS_OFF://关闭EPO，GNSS
				{
					if(epo_flag)
					{
						s32 ret;
						#ifdef __GNSS_NEW_API__
		                ret = Ql_GNSS_PowerDown();
		                #else
		                ret = RIL_GPS_Open(0);
		                #endif
		                if(RIL_AT_SUCCESS != ret) 
		                {
		                    APP_DEBUG("Power off GPS fail, iRet = %d.\r\n", ret);
		                    break;
		                }	
						else
						{
							RIL_GPS_EPO_Enable(0);			
							ret=Ql_Timer_Stop(TIME_GPS_ID);
							if(ret <0)
							{
								//保存错误代号
								APP_DEBUG("\r\n<--failed!!,STOP_GPSTimer: timer(%d) fail ,ret = %d -->\r\n",TIME_GPS_ID,ret);
							}
							APP_DEBUG("\r\n<--STOP_GPSTimer Success! -->\r\n");
							epo_flag=FALSE;
							APP_DEBUG("<--GNSS OFF-->\r\n");
						}		
					}	
					break;
				}
			default:
					break;
			}
	}
 }

/******************************************************************************
*	类型 :回调函数
*	功能:	判断时间是否同步
*	返回:	无
******************************************************************************/
void Callback_NTP(char *strURC)
{
 char *p1 = NULL;
 char *p2 = NULL;
 char strResult[10] = {0};
 char ntpResult = -1;
 APP_DEBUG("I am in Callback_NTP");
 p1 = Ql_strstr(strURC, ":");
 p2 = Ql_strstr(p1, "\r\n");
 if(p1 && p2)
 {
	 Ql_memset(strResult, 0x0, sizeof(strResult));
	 Ql_strncpy(strResult, p1 + 1, p2 - p1 - 1);
	 gNtpRestult = Ql_atoi(strResult);
 }
}

/******************************************************************************
* 类型:	基站回调函数
* 功能: 返回当前基站的经纬度;
* 返回:	无
******************************************************************************/
 void Callback_Location(s32 result, ST_LocInfo* loc_info)//基站返回函数
{
	locinfo.latitude=loc_info->latitude;
	locinfo.longitude=loc_info->longitude;
	APP_DEBUG("\r\n<-- Module location: latitude=%f, longitude=%f -->\r\n",locinfo.latitude,locinfo.longitude);
}
/******************************************************************************
* 功能 :发送AT指令，设置基站;
* 参数:	无;
* 返回:	基站是否设置成功;
******************************************************************************/
 s32 reference_location()
{
	u8 strAT[50]={0};
	s32 len = Ql_sprintf(strAT,"AT+QGREFLOC=%f,%f",locinfo.latitude,locinfo.longitude);
	// APP_DEBUG("\r\n<-- Module strAT    ===%s \r\n", strAT);
	return Ql_RIL_SendATCmd(strAT, len, NULL, NULL, 0);
}

//提取GPS数据
static void GpsReciveData(u8 *pBuffer, u16 length)
{
     u16 i,j;
     u8 *p=NULL;
     u8 count = 0;
     
     p=pBuffer;
     
     for(i=0; i<length; i++)
     {
         if(*(p+i) == '$')
         {
             if(0 == strncmp("RMC",(char const *)(p+i+3),3))
             {
                 for(j=i+1;j<length;j++)
                 {
                     if((*(p+j) == 0x0d)||(*(p+j) == 0x0a))
                     {
                        j++;
                        break;
                     }
                 }
                 if((j-i) < GPRMC_BUFFER_SIZE)
                 {
                     memcpy(GprmcBuffer,p+i,j-i);
                     count++;
                     i += (j-i-1);//跳过这些字节
                 }
                 
                 
             }
             else if(0 == strncmp("GGA",(char const *)(p+i+3),3))
             {
                 for(j=i+1;j<length;j++)
                 {
                     if((*(p+j) == 0x0d)||(*(p+j) == 0x0a))
                     {
                        j++;
                        break;
                     }
                 }
                 if((j-i) < GPGGA_BUFFER_SIZE)
                 {
                     memcpy(GpggaBuffer,p+i,j-i);
                     count++;
                     i += (j-i-1);//跳过这些字节
                 }
             }
         }
         
         if(count >= 2)
         {
        	 s32 ret;
             SetEvTask(MSG_Gps_EvTask); ///SetEvTask(EV_GPS_PARSE); 
	 		//ret =  Ql_OS_SendMessage (main_task_id, MSG_ID_EV_USER_FUNCTION, REMOTE_SET,0);//发送gps数据解析消息
	 		//ret =  Ql_OS_SendMessage (sysCtrl_task_id, MSG_ID_EV_USER_FUNCTION, REMOTE_SET,0);//发送gps数据解析消息
			//if(OS_SUCCESS!=ret)
			//{
			//	APP_DEBUG("\r\ send GPS DATA MESS  error!  ret = %d  \r\n",  ret);
			//}
             break;
         }
     }
}
#ifdef __GNSS_NEW_API__
/*****************************************************************
* Function:     isXdigit 
* 
* Description:
*               This function is used to check whether the 
*               input character number is a uppercase hexadecimal digital.
*
* Parameters:
*               [in]ch:   
*                       A character num.
*
* Return:        
*               TRUE  - The input character is a uppercase hexadecimal digital.
*               FALSE - The input character is not a uppercase hexadecimal digital.
*
*****************************************************************/
bool isXdigit(char ch)
{
    if((ch>='0' && ch<='9'
) || (ch >= 'A' && ch<='F'))
    {
        return TRUE;
    }

    return FALSE;
}

/*****************************************************************
* Function:     Ql_check_nmea_valid 
* 
* Description:
*               This function is used to check whether the 
*               input nmea is a valid nmea which contains '$' and <CR>.
*
* Parameters:
*               [in]nmea_in:   
*                       string of a sigle complete NMEA sentence
*
* Return:        
*               TRUE  - nmea_in is a valid NMEA sentence.
*               FALSE - nmea_in is an invalid NMEA sentence.
*
*****************************************************************/
bool Ql_check_nmea_valid(char *nmea_in)
{
    char *p=NULL, *q=NULL;
    
    u8 len = Ql_strlen(nmea_in);
    
    if(len > MAX_NMEA_LEN)                              // validate NMEA length
    {
        return FALSE;
    }
    
    p = nmea_in;
    if(*p == '$')                                       // validate header
    {
        q=Ql_strstr(p, "\r");                           // validate tail
        if(q)
        {
            if(isXdigit(*(q-1)) && isXdigit(*(q-2)))    // validate checksum is a hex digit
            {
                u8 cs_cal = 0;
                char cs_str[3]={0};
                while(*++p!='*')
                {
                    cs_cal ^= *p;
                }
                Ql_sprintf(cs_str, "%X", cs_cal);        // checksum is always uppercase.
                if(!Ql_strncmp(cs_str, q-2, 2))          // validate checksum(2 bits)
                {
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
    
    return FALSE;
}

/*****************************************************************
* Function:     Ql_get_nmea_segment_by_index 
* 
* Description:
*               This function is used to get the specified segment from a string
*               of a sigle complete NMEA sentence which contains '$' and <CR>.
*
* Parameters:
*               [in]nmea_in:   
*                       String of a sigle complete NMEA sentence
*               [in]index:     
*                       The index of the segment to get.
*               [out]segment: 
*                       The buffer to save the specified segment.
*               [in]max_len: 
*                       The length of the segment buffer.
*
* Return:        
*               >0 - Get segment successfully and it's the length of the segment.
*               =0 - Segment is empty.
*               <0 - Error number.
*
*****************************************************************/
char Ql_get_nmea_segment_by_index(char *nmea_in, u8 index, char *segment, u8 max_len)
{
    char *p=NULL, *q=NULL;
    u8 i=0;
    
    p = nmea_in;
    q = NULL;
    
    for(i=0; i<index; i++)
    {
        q=Ql_strstr(p, ",");
        if(q)
        {
            p = q+1;
        }
        else
        {
            break;
        }
    }
    
    q = NULL;
    if(i<index)
    {
        return -1;                  // Segment is not exist;
    }
    else
    {
        if((q=Ql_strstr(p, ",")) || (q=Ql_strstr(p, "*")))
        {
            u8 dest_len = q-p;
            if((*p==',') && (dest_len==1))
            {
                return 0;           // Segment is empty which is between ',' and ',' or ',' and '*'.
            }            
            else if(dest_len<max_len)
            {
                Ql_memset(segment, 0, max_len);
                Ql_memcpy(segment, p, dest_len);
                return dest_len;
            }
            else
            {
                return -2;          // Segment is longer than max_len!
            }
        }
        else
        {
            return -1;             // Segment is not exist;
        }
    }
    return -100;                   // Unknow error!
}

void Gps_NMEA_Hdlr(u8 *multi_nmea, u16 len, void *customizePara)
{
    u8 *p=NULL, *q=NULL;
    char one_nmea[MAX_NMEA_LEN+1];
    char dest_segment[32];
	u8 count = 0;
	GPS_STRUCT *pGps = (GPS_STRUCT *)customizePara;
	//Ql_memset(pGps,0x0,sizeof(GPS_STRUCT));
    //RMC
    p=NULL; q=NULL;
    p = Ql_strstr(multi_nmea, "$GNRMC");
    if(p)
    {
        q = Ql_strstr(p, "\r");
        if(q)
        {
            Ql_memset(one_nmea, 0, sizeof(one_nmea));
            Ql_memcpy(one_nmea, p, q-p+1);
            if(Ql_check_nmea_valid(one_nmea))
            {
            	//UTC Time Time hhmmss.sss - index 1
                char ret=-1;
                ret = Ql_get_nmea_segment_by_index(one_nmea, 1, dest_segment, sizeof(dest_segment));
                if(ret > 0)
                {
                	//解析时间,小数点后的秒抛弃
									pGps->Hour = (*dest_segment-0x30)*10+(*(dest_segment+1)-0x30);
									pGps->Hour += 8;
									pGps->Minute = (*(dest_segment+2)-0x30)*10+(*(dest_segment+3)-0x30);
									pGps->Second = (*(dest_segment+4)-0x30)*10+(*(dest_segment+5)-0x30);	
									//APP_DEBUG("\r\nUTC Time is :\r\n%02d/%02d/%02d \r\n",pGps->Hour, pGps->Minute, pGps->Second);
                }
				
                //fix status - index 2
                ret=-1;
                ret = Ql_get_nmea_segment_by_index(one_nmea, 2, dest_segment, sizeof(dest_segment));
                if(ret > 0)
                {
                    if(!Ql_strcmp(dest_segment, "A"))
                    {
                    	pGps->Status = 1;//有效定位
                        //APP_DEBUG("Fixed, Fix status: %s\r\n", dest_segment);
                    }
                    else
                    {
                    	pGps->Status = 0;
                        //APP_DEBUG("Unfixed, Fix status: %s\r\n", dest_segment);
                    }
                }
                
                //Latitude - index 3
                ret = -1;
                ret = Ql_get_nmea_segment_by_index(one_nmea, 3, dest_segment, sizeof(dest_segment));
                if(ret > 0)
                {
                	pGps->Latitue_D = (*dest_segment-0x30)*10+(*(dest_segment+1)-0x30);
					pGps->Latitue_F = (*(dest_segment+2)-0x30)*10+(*(dest_segment+3)-0x30);

					char *ptr = strchr(dest_segment,'.');//小数点
					pGps->Latitue_FX = (u16)Ql_atoi(ptr+1);
                    double dLat=0.0;
                    dLat = Ql_atof(dest_segment);
					pGps->Latitue = dLat;
                    //APP_DEBUG("Latitude: %0.4f %d %d %d\r\n", dLat,pGps->Latitue_D,pGps->Latitue_F,pGps->Latitue_FX);
                }

				//Latitude_direct - index 4
                ret = -1;
                ret = Ql_get_nmea_segment_by_index(one_nmea, 4, dest_segment, sizeof(dest_segment));
                if(ret > 0)
                {
                    char lat_direct = *dest_segment;
                    if('N' == lat_direct)
                    {
						pGps->North = 1;
                    }
					else if('S' == lat_direct)
                    {
						pGps->North = 0;
                    }
					//APP_DEBUG("lat_direct: %c\r\n", lat_direct);                 
                }
				
                //Longitude -index 5
                ret=-1;
                ret = Ql_get_nmea_segment_by_index(one_nmea, 5, dest_segment, sizeof(dest_segment));
                if(ret > 0)
                {
                	pGps->Longitue_D = (*dest_segment-0x30)*100+(*(dest_segment+1)-0x30)*10+(*(dest_segment+2)-0x30);
					pGps->Longitue_F = (*(dest_segment+3)-0x30)*10+(*(dest_segment+4)-0x30);

					char *ptr = strchr(dest_segment,'.');//小数点
					pGps->Longitue_FX = (u16)Ql_atoi(ptr+1);
                    double dLon=0.0;
                    dLon = Ql_atof(dest_segment);
					pGps->Longitue = dLon;
                    //APP_DEBUG("Longitude: %0.4f %d %d %d\r\n", dLon,pGps->Longitue_D,pGps->Longitue_F,pGps->Longitue_FX);
                }
				
				//Longitude_direct - index 6
				ret = -1;
				ret = Ql_get_nmea_segment_by_index(one_nmea, 6, dest_segment, sizeof(dest_segment));
				if(ret > 0)
				{
					char log_direct = *dest_segment;
					if('E' == log_direct)
					{
						pGps->East = 1;
					}
					else if('W' == log_direct)
					{
						pGps->East = 0;
					}
					//APP_DEBUG("log_direct: %c\r\n", log_direct);				 
				}
				
				//speed - index 7
				ret = -1;
				ret = Ql_get_nmea_segment_by_index(one_nmea, 7, dest_segment, sizeof(dest_segment));
				if(ret > 0)
				{
					double dSpeed=0.0;
                    dSpeed = Ql_atof(dest_segment);
					pGps->Speed_D = dSpeed;
					pGps->Speed = (u8)dSpeed;
					char *ptr = strchr(dest_segment,'.');//小数点
					pGps->SpeedX = (u8)Ql_atoi(ptr);;
					if(pGps->SpeedX >= 10)
					{
						pGps->SpeedX = 0;
					}
					//APP_DEBUG("Speed: %02d %02d\r\n", pGps->Speed,pGps->SpeedX);				 
				}

				//Course - index 8
				ret = -1;
				ret = Ql_get_nmea_segment_by_index(one_nmea, 8, dest_segment, sizeof(dest_segment));
				if(ret > 0)
				{
					double dCourse=0.0;
                    dCourse = Ql_atof(dest_segment);
					pGps->Course = (u16)dCourse;
					//APP_DEBUG("Course: %02d\r\n", pGps->Course);				 
				}

				//Data - index 9
				ret = -1;
				ret = Ql_get_nmea_segment_by_index(one_nmea, 9, dest_segment, sizeof(dest_segment));
				if(ret > 0)
				{
					pGps->Date = (*dest_segment-0x30)*10+(*(dest_segment+1)-0x30);
					pGps->Month = (*(dest_segment+2)-0x30)*10+(*(dest_segment+3)-0x30);
					pGps->Year = (*(dest_segment+4)-0x30)*10+(*(dest_segment+5)-0x30);	
					//APP_DEBUG("\r\nUTC Data is :\r\n%02d/%02d/%02d \r\n",pGps->Date, pGps->Month, pGps->Year);			 
				}

				count++;
            }
        }
    }

    //GGA
    p=NULL; q=NULL;
    p = Ql_strstr(multi_nmea, "$GNGGA");
    if(p)
    {
        q = Ql_strstr(p, "\r");
        if(q)
        {
            Ql_memset(one_nmea, 0, sizeof(one_nmea));
            Ql_memcpy(one_nmea, p, q-p+1);
            if(Ql_check_nmea_valid(one_nmea))
            {

				//Number of satellites - index 7
                char ret = -1;
                ret = Ql_get_nmea_segment_by_index(one_nmea, 7, dest_segment, sizeof(dest_segment));
                if(ret > 0)
                {
                    u8 ucSatCnt=0;
                    ucSatCnt = Ql_atoi(dest_segment);
					pGps->SatNum = ucSatCnt;
                    //APP_DEBUG("SatCnt: %d\r\n", ucSatCnt);
                }

				//HDOP - index 8
                ret = -1;
                ret = Ql_get_nmea_segment_by_index(one_nmea, 8, dest_segment, sizeof(dest_segment));
                if(ret > 0)
                {
                    double dHdop=0.0;
                    dHdop = Ql_atof(dest_segment);
					pGps->HDOP = (s16)dHdop;
                    //APP_DEBUG("HDOP: %0.1f\r\n", dHdop);
                }
				
                //Altitude - index 9
                ret = -1;
                ret = Ql_get_nmea_segment_by_index(one_nmea, 9, dest_segment, sizeof(dest_segment));
                if(ret > 0)
                {
                    double dAut=0.0;
                    dAut = Ql_atof(dest_segment);
					pGps->High = (s16)dAut;
                    //APP_DEBUG("Altitude: %0.1f\r\n", dAut);
                }

				//Geoid Separation - index 11
                ret = -1;
                ret = Ql_get_nmea_segment_by_index(one_nmea, 11, dest_segment, sizeof(dest_segment));
                if(ret > 0)
                {
                    double dHighOffset=0.0;
                    dHighOffset = Ql_atof(dest_segment);
					pGps->HighOffset = (s16)dHighOffset;
                    //APP_DEBUG("HighOffset: %0.1f\r\n", dHighOffset);
                }
				count++;
            }
        }
    }
	if(2 == count)
	{
		Gps_ClrParseErr();
		//Io_WriteAlarmBit(ALARM_BIT_GNSS_FAULT, RESET);
		//Gps_CopyToPosition(pGps);
		SetEvTask(MSG_Gps_EvTask);
	}	
	
}
#endif

