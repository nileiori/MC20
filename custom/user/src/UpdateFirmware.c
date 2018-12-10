#include"include.h"

#define URL_LEN 512
#define APN      "cmnet"
#define USERID   ""
#define PASSWD   ""

static APP_FTP         NetOther_FtpVal;//FTP升级参数
static u8 m_URL_Buffer[URL_LEN];

// 启动开始下载文件
void ftp_downfile_timer(void)
{

    s32 strLen;
    ST_GprsConfig apnCfg;
    Ql_memcpy(apnCfg.apnName,   APN, Ql_strlen(APN));
    Ql_memcpy(apnCfg.apnUserId, USERID, Ql_strlen(USERID));
    Ql_memcpy(apnCfg.apnPasswd, PASSWD, Ql_strlen(PASSWD));

    //ftp://hostname/filePath/fileName:port@username:password
    strLen = Ql_sprintf(m_URL_Buffer, "ftp://%s%s%s:%s@%s:%s",NetOther_FtpVal.ip, NetOther_FtpVal.fpath, NetOther_FtpVal.fname,
    														  NetOther_FtpVal.port, NetOther_FtpVal.user, NetOther_FtpVal.pass);
    APP_DEBUG("\r\n<-- URL:%s -->\r\n",m_URL_Buffer);
    
    strLen=Ql_FOTA_StartUpgrade(m_URL_Buffer, &apnCfg, NULL);
	if(strLen==-1)
		{
			Ql_Reset(0);
		}
}

/*
  * @brief  开始进行ftp文件下载
  * @param  None
  * @retval 返回是否执行升级, 1为升级，0为参数有问题不升级
  */
u8 Net_Other_FtpStart(void)
{
    if(NetOther_FtpVal.flag == 0)
    {
    /*
        ClrTimerTask(TIME_CORNER);
    	ClrTimerTask(TIME_POSITION);
        ClrTimerTask(TIME_IO_MONITOR);
        ClrTimerTask(TIME_GPSREC);
     */
     //Stop_TaskScheduleTimer();
	// CtrGnssOFF();
        NetOther_FtpVal.flag = 1;//置位ftp升级标志
        return 1;
    }
    return 0;
}
//读升级标志
u8 GetUpdateFlag(void)
{
	return NetOther_FtpVal.flag;
}

//写升级标志
void SetUpdateFlag(u8 sta)
{
	NetOther_FtpVal.flag = sta;
}

/*********************************************************************
//函数名称	:Updata_SendResultNoteTimeTask
//功能		:发送升级结果通知到平台
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:默认是开启的,1秒调度1次
*********************************************************************/
void Updata_SendResultNoteTimeTask(void)
{
    static u8 sucUpdataAckResult = 0;
    static u8 sucUpdataState = 0;
    static u8 sucUpdatacount = 0;    
    /////////////////////////
    if(0xff==sucUpdataState)return;
    /////////////////////////////
    switch(sucUpdataState)
    {
        case 0://读取结果信息
        {
            u8 Buffer[3] = {0};
            if(FRAM_BufferRead(Buffer,FRAM_FIRMWARE_UPDATA_FLAG_LEN,FRAM_FIRMWARE_UPDATA_FLAG_ADDR))
            {
            
				APP_DEBUG("\r\n<-- FRAM_FIRMWARE_UPDATA_FLAG :%d -->\r\n",Buffer[0]);
                //////////////
                sucUpdataState = 1;//继续下一步
                //////////////
                if((1 == Buffer[0])||(3 == Buffer[0]))//1:表示平台下发过远程升级指令,2:升级成功,3:升级失败
                {
                    sucUpdataAckResult = 1;
                }
                else 
                if(2 == Buffer[0])
                {
                    sucUpdataAckResult = 0;
                }
                else
                {
                    sucUpdataState = 0xff;
                }                
            } 
            break;
        }
        case 1://检测是否上线 
        {
            if(GetTerminalAuthorizationFlag())
            {
                sucUpdatacount = 0;
                sucUpdataState++;
            }
            else
            {
                sucUpdatacount++;
                if(sucUpdatacount>120)
                {
                    sucUpdataState = 0xff;
                }
            }
            break;
        }
        case 2://发送升级结果通知给平台
        {
            sucUpdataState++;
			APP_DEBUG("\r\n<-- FRAM_FIRMWARE_UPDATA_RESULT -->\r\n");
            RadioProtocol_FirmwareUpdataResultNote(0,sucUpdataAckResult);
            return ;
        }
        case 3://
        case 4://
        case 5://
            sucUpdataState++;//发送完了，等待3秒等平台答应
            break;
        case 6://检测应答
        {
            if(0==RadioProtocol_GetUpdataResultNoteAckFlag())
            {
                sucUpdataState = 2;
                sucUpdatacount++;
                if(sucUpdatacount < 3)break;
            }
            sucUpdatacount = 0;
            FRAM_BufferWrite(FRAM_FIRMWARE_UPDATA_FLAG_ADDR,&sucUpdatacount,1);//写入铁电下次就不会发送结果通知了 dxl,2017.2.23,待完善
            sucUpdataState = 0xff;
            break;
        }
        default:
            sucUpdataState = 0xff;
            break;;
    }
    return ;
}


u8 Net_Ftp_Parameter(u8 ParaType,u8 *tab,u8 tab_len)
{
    u8  *p=NULL;
    u8 i;
    u8 len;//计算端口等长度
    u8  num=0;//","个数
    u8  numaddr[5]={0};//","的地址
    u8  addr=0;
    switch(ParaType)
    {
    case 1://URL地址
      {
        for(i=0;i<tab_len;i++)
        {
          if(tab[i] == ',')
          {
            numaddr[num] = i;
            num++;
          }
        }
        if(num != 3)
        {
          return 1;
        }
        //ftp文件路径
        p = NetOther_FtpVal.fpath;
        len = sizeof(NetOther_FtpVal.fpath);
        memset(p,0,len); 
        memcpy(NetOther_FtpVal.fpath,tab,numaddr[0]); 
		APP_DEBUG("\r\n<-- NetOther_FtpVal fpath :%s -->\r\n",NetOther_FtpVal.fpath);
		
        //ftp文件名
        p = NetOther_FtpVal.fname;
        len = sizeof(NetOther_FtpVal.fname);
        memset(p,0,len);
        addr = numaddr[0] + 1;
        len  = numaddr[1] - numaddr[0] - 1;
        memcpy(p,tab+addr,len); 
		APP_DEBUG("\r\n<-- NetOther_FtpVal fname :%s -->\r\n",NetOther_FtpVal.fname);
		
        //ftp用户名
        p = NetOther_FtpVal.user;
        len = sizeof(NetOther_FtpVal.user);
        memset(p,0,len);
        addr = numaddr[1] + 1;
        len  = numaddr[2] - numaddr[1] - 1;    
        memcpy(p,tab+addr,len); 
		APP_DEBUG("\r\n<-- NetOther_FtpVal user :%s -->\r\n",NetOther_FtpVal.user);

        //ftp登陆密码
        p = NetOther_FtpVal.pass;
        len = sizeof(NetOther_FtpVal.pass);
        memset(p,0,len);
        addr = numaddr[2] + 1;
        len  = tab_len - addr;    
        memcpy(p,tab+addr,len);  
		APP_DEBUG("\r\n<-- NetOther_FtpVal pass :%s -->\r\n",NetOther_FtpVal.pass);
        break;
      }
    case 2://拨号点名称
      {
         break;
      }
    case 3://拨号用户名
      {
        break;
      }
    case 4://拨号密码
      {
        
        break;
      }
    case 5://ftp的ip地址
      {
        p = NetOther_FtpVal.ip;
        len = sizeof(NetOther_FtpVal.ip);
        memset(p,0,len);
        memcpy(NetOther_FtpVal.ip,tab,tab_len);
		APP_DEBUG("\r\n<-- NetOther_FtpVal ip :%s -->\r\n",NetOther_FtpVal.ip);
        break;
      }
    case 6://TCP端口
      {
        p = NetOther_FtpVal.port;
        len = sizeof(NetOther_FtpVal.port);
        memset(p,0,len);
        //sprintf((char*)NetOther_FtpVal.port,"%d",Public_ConvertBufferToShort(tab));
        Ql_sprintf((char*)NetOther_FtpVal.port,"%d",Public_ConvertBufferToShort(tab));
		//Ql_sprintf(m_URL_Buffer, "ftp://%s%s%s:%s@%s:%s",FTP_SVR_ADDR, FTP_SVR_PATH, FTP_FILENAME, FTP_SVR_PORT, FTP_USER_NAME, FTP_PASSWORD);
		//NetOther_FtpVal.port=((u32)tab[0]<<8)|(u32)tab[1];

		
		APP_DEBUG("\r\n<-- NetOther_FtpVal port :%s -->\r\n",NetOther_FtpVal.port);
		Ql_sprintf(m_URL_Buffer, "ftp://%s%s%s:%s@%s:%s",NetOther_FtpVal.ip, NetOther_FtpVal.fpath, NetOther_FtpVal.fname,
																	  NetOther_FtpVal.port, NetOther_FtpVal.user, NetOther_FtpVal.pass);
			APP_DEBUG("\r\n<-- URL:%s -->\r\n",m_URL_Buffer);

        break;
      }      
    }
    return 0;
}

