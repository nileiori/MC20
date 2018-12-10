#ifndef  _UPDATAFIRMWARE_H_
#define  _UPDATAFIRMWARE_H_

/*
********************************************************************************
*                                STRUCT
********************************************************************************
*/
typedef struct {
    u8  flag;//ftp忙标志，传数据时非0
    u8  ip[15];//ip地址
    u8  port[5];//端口
    u8  user[20];//服务器名称
    u8  pass[20];//服务器密码    
    u8  fpath[50];//文件路径
    u8  fname[50];//文件名
    u16 pageAddr;
    u32 address;
}APP_FTP;


/*
  * @brief  开始进行ftp文件下载
  * @param  None
  * @retval 返回是否执行升级, 1为升级，0为参数有问题不升级
  */
u8 Net_Other_FtpStart(void);
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
void Updata_SendResultNoteTimeTask(void);

u8 Net_Ftp_Parameter(u8 ParaType,u8 *tab,u8 tab_len);
void ftp_downfile_timer(void);
//读升级标志
u8 GetUpdateFlag(void);
//写升级标志
void SetUpdateFlag(u8 sta);

#endif

