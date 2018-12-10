#include "include.h"

/************************************************************
** 函数名称: System_ClearAllUserRecord
** 功能描述: 
** 入口参数:
** 出口参数:
************************************************************/ 
void System_ClearAllUserRecord(void)
{
	/*
    unsigned char buffer[30];    
    Area_EraseAllArea();//清除矩形、圆形区域
    Polygon_ClearArea();//清除多边形区域
    ClrRoute();                   //清空线路
    TiredDrive_TiredDriverInfoInit();//超时驾驶相关驾驶员信息初始化   
    Report_EraseBlindArea();//擦除盲区区域   
    History_EraseTrackArea();//擦除历史轨迹区域    
    Public_ConvertLongToBuffer(0,buffer);
    EepromPram_WritePram(E2_CAR_TOTAL_MILE_ID, buffer,4);
    Temp_SaveDefaultParameter();
    EepromPram_UpdateVariable(E2_CAR_TOTAL_MILE_ID);
    FRAM_BufferWrite(FRAM_EMERGENCY_FLAG_ADDR, buffer, FRAM_EMERGENCY_FLAG_LEN);//清除铁电存储标志
    */
}
/*********************************************************************
//函数名称	:EepromPram_DefaultServerParameter
//功能		:
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
void EepromPram_DefaultServerParameter(void)
{
    unsigned char Buffer[30];
    unsigned char BufferLen;
    strcpy((char *)Buffer,"momo.e-eye.cn");
    BufferLen=strlen((char *)Buffer);
    //////////////
    if(0 == EepromPram_WritePram(E2_MAIN_SERVER_IP_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_MAIN_SERVER_IP_ID, Buffer, BufferLen);
    }
    //////////////////////
    if(0 == EepromPram_WritePram(E2_BACKUP_SERVER_IP_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_BACKUP_SERVER_IP_ID, Buffer, BufferLen);
    }
    //0x0010       主服务器APN
    strcpy((char *)Buffer,"cmnet");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_MAIN_SERVER_APN_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_MAIN_SERVER_APN_ID, Buffer, BufferLen);
    }   
    //0x0014 备份服务器APN
    strcpy((char *)Buffer,"cmnet");
    BufferLen = strlen((char const *)Buffer);
    if(0 == EepromPram_WritePram(E2_BACKUP_SERVER_APN_ID, Buffer, BufferLen))
    {
        EepromPram_WritePram(E2_BACKUP_SERVER_APN_ID, Buffer, BufferLen);
    }
    //主服务器TCP端口,默认为11111
    Public_ConvertLongToBuffer(11111, Buffer);
    if(0 == EepromPram_WritePram(E2_MAIN_SERVER_TCP_PORT_ID, Buffer, E2_MAIN_SERVER_TCP_PORT_LEN))
    {
        EepromPram_WritePram(E2_MAIN_SERVER_TCP_PORT_ID, Buffer, E2_MAIN_SERVER_TCP_PORT_LEN);
    }
    ///////////////
    //if(0 == EepromPram_WritePram(E2_MAIN_SERVER_UDP_PORT_ID, Buffer, E2_MAIN_SERVER_UDP_PORT_LEN))
    //{
    //    EepromPram_WritePram(E2_MAIN_SERVER_UDP_PORT_ID, Buffer, E2_MAIN_SERVER_UDP_PORT_LEN);
    //}
}

/*********************************************************************
//函数名称	:CheckFirstPowerOn
//功能		检测是否为第一次上电
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:返回值为1初次使用，0不是初次使用
//备注		:
*********************************************************************/
u8 CheckFirstPowerOn(void)
{
    s32 handle=-1;
    u32	temp=0;
	s32 ret = 0;
	//Enum_FSStorage storage = Ql_FS_UFS;

	handle = Ql_FS_Open(PRAM_FILE_NAME,QL_FS_READ_WRITE);
	APP_DEBUG("\r\n<-- Creat PRAM_FILE_NAME File %d-->\r\n",handle);
	if(handle<0)//不能读取到参数文件就认为是第一次使用
		{  
            // format 
            //Ql_FS_Format(storage);
			handle = Ql_FS_Open(PRAM_FILE_NAME,QL_FS_CREATE_ALWAYS); //if file does not exist ,creat it
			APP_DEBUG("\r\n<-- Creat PRAM_FILE_NAME File %d-->\r\n",handle);
			//Ql_memset(ShareBuffer,0xff, RADIOPROTOCOL_BUFFER_SIZE);

			//while(temp<E2_PRAM_END_ADDR)
			//	{
		    //      ret = Ql_FS_Seek(handle,0,QL_FS_FILE_END);  //seek end 
          	//	  ret = Ql_FS_Write(handle, ShareBuffer, E2_PRAM_END_ADDR,&temp);
			//	  Ql_FS_Flush(handle); //fflush强制缓存写入文件
				 // temp++;
			//	}

		 Ql_FS_Close(handle);//close the file
			 return 1;
		}
	Ql_FS_Close(handle);//close the file
	return 0;
}

/*********************************************************************
//函数名称	:PowerOnUpdata
//功能		:上电更新变量
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:更新的变量有：IO、GPS、位置汇报、摄像头
*********************************************************************/
void PowerOnUpdata(void)
{
	
    //读第一次使用标志
    if(CheckFirstPowerOn()==1)
    {
        EepromPram_DefaultSet();
        EepromPram_DefaultServerParameter();
		Gps_PowerOnClearPosition();
		//SleepInit();
		//SetAccLineFlag(0);
    }
	CreatBlindFile();//创建盲区存取文件
    ///////////////
    //MemoryChipSelfCheck();
    ////////////////////////////    
    //更新IO变量
    Io_UpdataPram();
	//GPS计算里程相关变量
	//GpsMile_UpdatePram();
	//MileMeter_ClearOldPoint();	 
	//更新GPS变量
	Gps_PowerOnUpdataPosition();

    //更新心跳包时间间隔
    HeartBeat_UpdatePram();

    //更新手机号码
    RadioProtocol_UpdateTerminalPhoneNum();    
    //////////////////


}

/*********************************************************************
//函数名称	:PeripheralInit
//功能		:外设初始化
//输入		:
//输出		:
//使用资源	:
//全局变量	:   
//调用函数	:
//中断资源	:  
//返回		:
//备注		:初始化的外设有：GPS、GPRS、FRAM、FLASH、EEPROM、摄像头
*********************************************************************/
void PeripheralInit(void)
{

	//初始IIC接口
	//Mc20_IIC_Init();
	//初始化BM250
	//init_bma250e();
    /////////////////
    //RadioProtocol_RecvQueueInit();
    /////////////////
    //////////////    
    //GPIO初始化,custom_gpio_cfg.h里面预先配置过了
    Io_Init(); 
	Led_Init();
	LowVol_UpdatePram();
	//打开电池供电
	//VBATTN_ON();
	//BATTERYCHARGE_ON();
    /////////////
    //SPI_FLASH_AllUnprotect();    
    //E2prom_InitIO();
    //AD初始化
    //Ad_Init(); 
	//检测是12V供电还是24V供电
    //Io_PowerOnDetectMainPower();
    /////////////////
    ///Communication_Init();
    //Sleep_Init();
    //////////////////
	init_TaskScheduleTimer();//开启任务扫描定时器
	
}


