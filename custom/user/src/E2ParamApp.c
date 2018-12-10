#include "include.h"
#include "FCT_SelfCheck.h"


/*******************************************************************************/
////////////////////////////////////////////
#define NEED_CONVERT_OLD_SELF_DEFINE_ID     //如果定义,则转换之前自定义参数ID为0x0200开始的参数
/////////////////////////////////////
//////////////////////////////////////////
extern const u8 EepromPramLength[];
extern const unsigned short c_usEepromPramID[];

/*************************************************************
** 函数名称: E2ParamApp_ConvertPramID
** 功能描述: 转换旧的自定义参数ID(0x0200开始)
** 入口参数: 参数ID
** 出口参数: 
** 返回参数: 我们新自定义的ID(0xF200开始)
** 全局变量: 
** 调用模块: 
*************************************************************/

unsigned int E2ParamApp_ConvertPramID(unsigned int usParamID)
{
    unsigned int paramID;
    paramID=usParamID;
    //////////////////////
    if(0x0200<=usParamID&&usParamID<=0x23B)//
    {
        paramID=usParamID+0xF000;
    }
    ////////////////////////
    return paramID;
}

/*************************************************************
** 函数名称: E2ParamApp_ReadSpecialParam
** 功能描述: 读特殊参数
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 
** 调用模块: 
*************************************************************/	
unsigned char E2ParamApp_ReadSpecialParam(unsigned short usParamID,unsigned char *data)
{
    if(E2_SIM_ICCID_NUMBER_ID == usParamID)//SIM卡的ICCID号码
    {
        //Modem_Api_CCID(data);
        return 10;
    }
    return 0;
}

/*******************************************************************************
** 函数名称: E2ParamApp_CheckPhoneFormatData
** 函数功能: 判断数据是否为手机号格式,BCD[6]
** 入口参数: data数据首地址,datalen数据长度
** 返回参数: 0是手机号格式,1不是手机号格式
** 全局变量: 无
** 调用模块: 无
*******************************************************************************/
unsigned char E2ParamApp_CheckPhoneFormatData(unsigned char *data,unsigned char datalen)
{
    u8 *p = NULL;
    u8  High4Bit = 0;//高4位
    u8  Low4Bit = 0;//低4位
    u8  i;
    if(NULL == p)
    {
        if(6 != datalen)
        {
            return 1;//长度有误
        }
        p = data;
        for(i=0; i<datalen; i++)
        {
            High4Bit = (*p&0xf0) >> 4;
            Low4Bit = *p&0x0f;
            if((High4Bit > 9)||(Low4Bit > 9))
            {
                return 1;//格式有误
            }
            p++;
        }
        return 0;
        
    }
    else
    {
        return 1;//返回错误
    }
}
extern u16 DelayAckCommand;
/*************************************************************
** 函数名称: E2ParamApp_DisposeWriteParam
** 功能描述: 处理协议写参数
** 入口参数: pBuffer数据首地址,BufferLen数据长度
** 出口参数: 
** 返回参数: 处理结果
** 全局变量: 
** 调用模块: 
*************************************************************/	
ProtocolACK E2ParamApp_DisposeWriteParam(u8 *pBuffer, u16 BufferLen)
{
    u8	*p;
    u8	Buffer[30];
    u8	ReadLen;
    u8	PramLen;
    s16	length;
    u32	PramID,prePramID;
    u16	index;

    p = pBuffer;
    length = BufferLen;

    //先只考虑单值的情况
    while(length > 0)
    {
        //读参数ID
        prePramID=Public_ConvertBufferToLong(p);
		if(0x0000f238 == prePramID)//库存模式特殊处理
		{
			DelayAckCommand = 12;//关机
        	//触发延时任务
        	SetTimerTask(TIME_DELAY_TRIG,3*SECOND);
			return ACK_OK;
		}
		if(0 == FCT_ReadSelfCheckFlg())//正常模式下切换到FCT模式
		{
			if(0x0001ffff == prePramID)
			{
				FCT_DEBUG("\r\nSYSTEM RESET INTO FCTMODE\r\n");
				Buffer[0] = 0; 
				FRAM_BufferWrite(FRAM_FCT_SELF_CHECK_ADDR,Buffer,FRAM_FCT_SELF_CHECK_LEN);
				DelayAckCommand = 5;//system reset
	        	//触发延时任务
	        	SetTimerTask(TIME_DELAY_TRIG,3*SECOND);
				return NOACK;
			}
		}
        p += 4;
        ////////////////////////
        PramID = E2ParamApp_ConvertPramID(prePramID);
        /////////////////////////
        index = EepromPram_ConvertParamIDToIndex(PramID);
        ///////////////
        //读取参数长度
        PramLen = *p++;
		
        //////////////
        if(index)//处理ID已识别的,其它跳过
        {
            //检查参数ID和参数长度是否正确
            if(EepromPramLength[index] >= PramLen)
            {
                //检查一些特定的参数是否正确
                
                if((E2_SIM_ICCID_NUMBER_ID == PramID)//SIM卡的ICCID号码,不用保存,直接读
                 ||(E2_FIRMWARE_VERSION_ID == PramID))
                {
                    //不需要保存
                }
                else
                {      
                  if((E2_MAIN_SERVER_IP_ID == PramID)||                    
                    (E2_BACKUP_SERVER_IP_ID == PramID)||
                    (E2_MAIN_SERVER_TCP_PORT_ID == PramID)||
                    (E2_CAR_PROVINCE_ID == PramID)||
                    (E2_CAR_CITY_ID == PramID)||
                    (E2_CAR_PLATE_NUM_ID == PramID)||
                    (E2_CAR_PLATE_COLOR_ID == PramID)||
					(E2_DEVICE_PHONE_ID == PramID)||
                    (E2_DEVICE_ID == PramID))
                    {
                    	
                        //擦除鉴权码,下次上线重新发注册指令
                        //ClearTerminalAuthorizationCode(CHANNEL_DATA_1);
						//SetAccLineFlag(0);
						//Net_First_Value(MAIN_SERVER);
						//Socket_Restart();
						//Ql_Timer_Start(TCP_TIMER_ID, TCP_TIMER_PERIOD, TRUE);//开启TCP链接初始化定时器
                    }
				  	//APP_DEBUG("长度...%d..%d..%04x",index,PramLen,PramID);
                    EepromPram_WritePramEx(index, p, PramLen);
                    //////////////////////// 
                    if(E2_DEVICE_ID == PramID)//写入备份区域,终端ID
                    {
                        EepromPram_WritePram(E2_DEVICE_BACKUP_ID, p, PramLen);
						//EepromPram_WritePram(E2_DEVICE_PHONE_ID, p+1, E2_DEVICE_PHONE_ID_LEN);
                    }
					if(E2_SLEEP_REPORT_TIME_ID == PramID)
                    {
                        extern s32 m_pwrOnReason;
                    	m_pwrOnReason = PWRKEYPWRON;
                    }
                    //读出刚写入的参数                   
                    if(PramLen < 30)
                    { 
                        ReadLen = EepromPram_ReadPramEx(PramID,index, Buffer);
                        if(Public_CheckArrayValIsEqual(Buffer,p,ReadLen))//写出错
                        {
                            return ACK_ERROR;//写出错
                        }
                    }
                    //更新相应的变量
                    EepromPram_UpdateVariable(PramID); 
                }
            }
            else//跳过长度不正确的
            {
                //return 0;
            }
        }
        else//跳过ID不认识的
        {
            return NOACK;;
        }
        ////////////////////
        length = length - 4 - 1 - PramLen;
        p += PramLen;
    }
    return ACK_OK;
}
/*************************************************************
** 函数名称: E2ParamApp_ReadOneParam
** 功能描述: 处理协议读参数
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 
** 调用模块: 
*************************************************************/	
unsigned char E2ParamApp_ReadOneParam(unsigned short index,unsigned int PramID,unsigned char *data)
{
    u8 PramLen = 0;
    u8 Buffer[50];
	char ch[30] = {'\0'};
    if(E2_SIM_ICCID_NUMBER_ID == PramID)//SIM卡的ICCID号码
    {
        //写参数ID
        Public_ConvertLongToBuffer(PramID,data);
        data[4] = 10;//写参数长度
        Modem_Api_CCID(&data[5]);
        return 15;
    }
    else
    if((E2_FIRMWARE_VERSION_ID&0x0FFF)==(PramID&0x0FFF))//版本号
    {
        Public_ConvertLongToBuffer(PramID,data);
		//data="V1.01";
        Version_GetEquipmentSoftVer((char *)&data[5]);
        data[4] = 6;//
        return 11;
    }
	else
    if(E2_DEFINE_HARD_VER_ID==PramID)//硬件版本号
    {
        Public_ConvertLongToBuffer(PramID,data);
        PramLen = Version_GetHardwareInformation((char *)&data[5]);
        data[4] = PramLen;//
        return (5+PramLen);
    }
	else
    if(0x0001F010==PramID)//IMEI 0001F010
    {
        Public_ConvertLongToBuffer(PramID,data);
		if(RIL_AT_SUCCESS == RIL_GetIMEI(ch))
		{
			PramLen = Ql_strlen(ch)-2;//减去0D  0A 回车换行
	        Ql_strcpy(&data[5],ch+2);
        }
		data[4] = PramLen;//
        return (5+PramLen);
    }
	else
    if(0x0001F011==PramID)//IMSI 0001F011
    {
        Public_ConvertLongToBuffer(PramID,data);
		if(RIL_AT_SUCCESS == RIL_SIM_GetIMSI(ch))
        {
	        PramLen = Ql_strlen(ch)-2;//减去0D  0A 回车换行
	        Ql_strcpy(&data[5],ch+2);
        }
		data[4] = PramLen;//
        return (5+PramLen);
    }
	else
    if(0x0001F012==PramID)//SN 0001F012
    {
        Public_ConvertLongToBuffer(PramID,data);
		PramLen = Ql_GetUID(Buffer, sizeof(Buffer));
        Ql_memcpy(&data[5],Buffer,PramLen);
        data[4] = PramLen;//
        return (5+PramLen);
    }
	else
	if(0x0000f238 == PramID)//库存模式特殊处理
	{
		Public_ConvertLongToBuffer(PramID,data);//都关机了还读个毛线
		PramLen = 1;        
        data[4] = PramLen;
		data[5] = 0;
        return (5+1);
	}
    //////////////////////////
    PramLen = EepromPram_ReadPramEx(PramID,index, Buffer);
	//APP_DEBUG("\r\n哈哈哈哈...%d\r\n",PramLen)
    if(PramLen)
    {
        //写参数ID
        Public_ConvertLongToBuffer(PramID,data);
        //写参数长度
        data[4] = PramLen;
        //写参数内容
        memcpy(&data[5],Buffer,PramLen);
        return PramLen+5;
    }
    return 0;
}
/*************************************************************
** 函数名称: E2ParamApp_DisposeReadAllParam
** 功能描述: 处理协议读参数
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 
** 调用模块: 
** 备    注: 读取的参数总个数，*pDstBuffer按ID号，ID长度，ID内容的顺序存放，*DstBufferLen总的读取长度，不包括参数总个数字节
           : ReadType:0为有线(串口)，每次只能读550字节；1为无线（GPRS），每次可读1000字节
*************************************************************/	
unsigned char E2ParamApp_DisposeReadAllParam(unsigned char *pDstBuffer,s16 *DstBufferLen)
{
    unsigned char *des;
    unsigned char paramNum,len;
    signed short length;
    unsigned short maxlen,index;
    unsigned long PramID;  
    length =0;
    paramNum =0;    
    des = pDstBuffer;
    maxlen = 256;
    //**************平台指定的参数*****************
    for(index=0; index<EepromPram_GetLenEepromPramID(); index++)
    {
        PramID = c_usEepromPramID[index];
        len = E2ParamApp_ReadOneParam(index,PramID,des);
        if(len)
        {
            if(length+len>maxlen)break;
            des += len;
            length+=len;
            paramNum++;
        }
    }     
    ////////////////////
    *DstBufferLen=length;
    ////////////////
    return paramNum;
}
/*************************************************************
** 函数名称: E2ParamApp_DisposeReadParam
** 功能描述: 处理协议读参数
** 入口参数: 把需要读的参数ID号放入缓冲pIDBuffer中，每个ID四字节，高在前，IDBufferLen为4的整数倍
** 出口参数: 
** 返回参数: 
** 全局变量: 
** 调用模块: 
** 备    注: 读取的参数总个数，*pDstBuffer按ID号，ID长度，ID内容的顺序存放，*DstBufferLen总的读取长度，不包括参数总个数字节
           : ReadType:0为有线(串口)，每次只能读550字节；1为无线（GPRS），每次可读1000字节
*************************************************************/	
unsigned char E2ParamApp_DisposeReadParam(unsigned char *pDstBuffer,s16 *DstBufferLen,unsigned char *pIDBuffer,signed short IDBufferLen)
{
    unsigned char *des,*src;
    unsigned char paramNum,len;
    signed short length,datalen;
    unsigned short maxlen,index;
    unsigned long PramID,readParamID;    
    paramNum =0;
    length =0;
    des = pDstBuffer;    
    src = pIDBuffer;//    
    datalen =IDBufferLen;
    maxlen = 256;
    //////////////////
    while(datalen > 0)
    {
        PramID=Public_ConvertBufferToLong(src);        
        readParamID = E2ParamApp_ConvertPramID(PramID);
        index = EepromPram_ConvertParamIDToIndex(readParamID);
        //读参数
        len = E2ParamApp_ReadOneParam(index,PramID,des);
        if(len)
        {
            if(length+len>maxlen)break;
            des += len;
            length+=len;
            paramNum++;
        }
        src += 4;
        datalen -=4;
    }
    ////////////////////
    *DstBufferLen=length;
    ////////////////
    return paramNum;
}


