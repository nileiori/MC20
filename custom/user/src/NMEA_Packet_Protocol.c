#include "NMEA_Packet_Protocol.h"

//Notice : You can also check the documentation of "Quectel_MC20_GNSS_Protocol_Specification_V1.0.pdf"

#if	configUSE_MTK_NMEA	

static Enum_SerialPort m_gnssUartPort = UART_PORT3;

/*************************************************************
** 函数名称: Public_GetXorVerify
** 功能描述: 对数据块进行异或，只取低字节
** 入口参数: Pdata数据块首地址,datalen长度       
** 出口参数: 
** 返回参数: 校验码
** 全局变量: 无
** 调用模块: 无
*************************************************************/
unsigned char GetXorVerify(const unsigned char *Pdata,unsigned short datalen)
{
	//对数据块进行异或，只取低字节
	unsigned char Verify ;
	unsigned short	i;
	Verify= Pdata[0];

	for(i = 1; i < datalen; i++)
	{
		Verify ^= Pdata[i];
	}
	return Verify;	  
}

bool MTK_SendBinCmd(u8 *strCmd)
{
        int len = 0,ret = 0;
        unsigned char outBuffer[256];
    
	// At initial, the protocol setting of the communication UART is supposed to be PMTK protocol.
	// Since EPO data are transferred using MTK Binary Packet, you have to change the protocol setting to MTK Binary Protocol
	// before starting EPO Transfer Protocol. You can use PMTK command 253 to change the UART protocol setting.
	// Please refer to 7.1 for the details of PMTK command 253.

        Ql_memset(outBuffer,0,sizeof(outBuffer));                
        Ql_memcpy((void *)outBuffer, strCmd, Ql_strlen(strCmd));
        len = Ql_strlen(outBuffer);
        outBuffer[len++] = 0x0D;
        outBuffer[len++] = 0x0A;
        outBuffer[len] = '\0';   
   
	/*send the data of outBuffer to the GPS module example L70 ect. */
        ret = Ql_UART_Write(m_gnssUartPort,outBuffer,len);
        if (ret < len)
        {
            APP_DEBUG("SendMtkBinCmd Only part of bytes are written, %d/%d \r\n", ret, len);
            return FALSE;
        }
        
        return TRUE;	

}
//Packet Type: 010 PMTK_SYS_MSG
//This message is used to automatically output system messages through GNSS module
bool MTK_SendCmd_Packet_Type_010(const char *message)
{
	char strCmdBuf[20] = {'\0'};
	char tmp[2];
	u8 verifyCode;
	bool ret = FALSE;
	
	Ql_strcpy(strCmdBuf,"PMTK010,");
	Ql_strcat(strCmdBuf,message);
	verifyCode = GetXorVerify(strCmdBuf,Ql_strlen(strCmdBuf));//计算校验码
	Ql_sprintf(tmp, "%02x", verifyCode);//hex格式化成hex字符串
	//数据组包
	Ql_memmove(strCmdBuf+1,strCmdBuf,Ql_strlen(strCmdBuf));
	*strCmdBuf = '$';
	Ql_strcat(strCmdBuf,"*");
	Ql_strcat(strCmdBuf,tmp);
	APP_DEBUG("\r\n...%s...\r\n",strCmdBuf);
	//需要的时候打开
	//ret = Mtk_SendBinCmd(strCmdBuf);
	return ret;
}

//Packet Type: 011 PMTK_TXT_MSG
bool MTK_SendCmd_Packet_Type_011(void)
{
	char strCmdBuf[20] = {'\0'};
	char tmp[2];
	u8 verifyCode;
	bool ret = FALSE;
	
	Ql_strcpy(strCmdBuf,"PMTK011,MTKGPS");
	verifyCode = GetXorVerify(strCmdBuf,Ql_strlen(strCmdBuf));//计算校验码
	Ql_sprintf(tmp, "%02x", verifyCode);//hex格式化成hex字符串
	//数据组包
	Ql_memmove(strCmdBuf+1,strCmdBuf,Ql_strlen(strCmdBuf));
	*strCmdBuf = '$';
	Ql_strcat(strCmdBuf,"*");
	Ql_strcat(strCmdBuf,tmp);
	APP_DEBUG("\r\n...%s...\r\n",strCmdBuf);
	//需要的时候打开
	//ret = Mtk_SendBinCmd(strCmdBuf);
	return ret;
}

//Packet Type: 001 PMTK_ACK
//Acknowledgement of PMTK Command. In order to inform the sender whether the receiver has received
//the packet, and an acknowledge packet PMTK_ACK should be returned after the receiver receives a packet.
bool MTK_SendCmd_Packet_Type_001(const char *strCmd,const char *strFlag)
{
	char strCmdBuf[20] = {'\0'};
	char tmp[2];
	u8 verifyCode;
	bool ret = FALSE;
	
	Ql_strcpy(strCmdBuf,"PMTK001,");
	Ql_strcat(strCmdBuf,strCmd);
	Ql_strcat(strCmdBuf,",");
	Ql_strcat(strCmdBuf,strFlag);
	verifyCode = GetXorVerify(strCmdBuf,Ql_strlen(strCmdBuf));//计算校验码
	Ql_sprintf(tmp, "%02x", verifyCode);//hex格式化成hex字符串
	//数据组包
	Ql_memmove(strCmdBuf+1,strCmdBuf,Ql_strlen(strCmdBuf));
	*strCmdBuf = '$';
	Ql_strcat(strCmdBuf,"*");
	Ql_strcat(strCmdBuf,tmp);
	APP_DEBUG("\r\n...%s...\r\n",strCmdBuf);
	//需要的时候打开
	//ret = Mtk_SendBinCmd(strCmdBuf);
	return ret;
}

//Packet Type: 101 PMTK_CMD_HOT_START
//This message is used to achieve the hot start of the GNSS module (use all available data in the NV store).
//Normally hot start means the GNSS module was powered down for less than 3 hours (RTC must be alive)
//and its ephemeris is still valid. As there is no need for downloading ephemeris, it is the fastest startup
//method
bool MTK_SendCmd_Packet_Type_101(void)
{
	bool ret = FALSE;

	//需要的时候打开
	//ret = Mtk_SendBinCmd("$PMTK101*32");
	return ret;
}

//Packet Type: 102 PMTK_CMD_WARM_START
//This message is used to achieve the warm start of the GNSS module. Warm start means the GNSS
//module has approximate information on time, position and coarse data of satellite positions. But it needs
//to download ephemeris until it can get a fix. Using this message will force the GNSS warm restart without
//using the ephemeris data in NV
bool MTK_SendCmd_Packet_Type_102(void)
{
	bool ret = FALSE;

	//需要的时候打开
	//ret = Mtk_SendBinCmd("$PMTK102*31");
	return ret;
}

//Packet Type: 103 PMTK_CMD_COLD_START
//This message is used to achieve cold start of the GNSS module. Using this message will force the GNSS
//cold restart without using any prior location information, which includes time, position, almanacs and
//ephemeris data
bool MTK_SendCmd_Packet_Type_103(void)
{
	bool ret = FALSE;

	//需要的时候打开
	//ret = Mtk_SendBinCmd("$PMTK103*30");
	return ret;
}

//Packet Type: 104 PMTK_CMD_FULL_COLD_START
//This message is essentially a cold restart, but additionally clear system and user configuration at re-start.
//That is, reset the GNSS module to the factory status. Full cold start means the GNSS module has no
//information on last location. It needs to search the full time and frequency space, and also all possible
//satellite numbers before it can get a fix
bool MTK_SendCmd_Packet_Type_104(void)
{
	bool ret = FALSE;

	//需要的时候打开
	//ret = Mtk_SendBinCmd("$PMTK104*37");
	return ret;
}

//Packet Type: 161 PMTK_CMD_STANDBY_MODE
//This message is used to enter into standby mode for power saving
bool MTK_SendCmd_Packet_Type_161(const char *message)
{
	char strCmdBuf[20] = {'\0'};
	char tmp[2];
	u8 verifyCode;
	bool ret = FALSE;
	
	Ql_strcpy(strCmdBuf,"PMTK161,");
	Ql_strcat(strCmdBuf,message);
	verifyCode = GetXorVerify(strCmdBuf,Ql_strlen(strCmdBuf));//计算校验码
	Ql_sprintf(tmp, "%02x", verifyCode);//hex格式化成hex字符串
	//数据组包
	Ql_memmove(strCmdBuf+1,strCmdBuf,Ql_strlen(strCmdBuf));
	*strCmdBuf = '$';
	Ql_strcat(strCmdBuf,"*");
	Ql_strcat(strCmdBuf,tmp);
	APP_DEBUG("\r\n...%s...\r\n",strCmdBuf);
	//需要的时候打开
	//ret = Mtk_SendBinCmd(strCmdBuf);
	return ret;
}

//Packet Type: 183 PMTK_LOCUS_QUERY_STATUS
//This message is used to query logging status and is not supported in all-in-one solution
bool MTK_SendCmd_Packet_Type_183(void)
{
	bool ret = FALSE;

	//需要的时候打开
	//ret = Mtk_SendBinCmd("$PMTK183*38");
	return ret;
}


/*************************************************************************
Packet Type: 184 PMTK_LOCUS_ERASE_FLASH
This message is used to erase logger flash and is not supported in all-in-one solution

Data Field:
$PMTK184,Type
Example:
$PMTK184,1*22<CR><LF>
Response:
$PMTK001,184,3*3D<CR><LF>
Field Description

$ Each NMEA message starts with '$'
PMTK MTK proprietary message
Packet Type 184
Type '1'=Erase all logger internal flash data
* End character of data field
Checksum Hexadecimal checksum
<CR><LF> Each of the messages

***************************************************************************/
bool MTK_SendCmd_Packet_Type_184(const char *message)
{
	char strCmdBuf[20] = {'\0'};
	char tmp[2];
	u8 verifyCode;
	bool ret = FALSE;
	
	Ql_strcpy(strCmdBuf,"PMTK184,");
	Ql_strcat(strCmdBuf,message);
	verifyCode = GetXorVerify(strCmdBuf,Ql_strlen(strCmdBuf));//计算校验码
	Ql_sprintf(tmp, "%02x", verifyCode);//hex格式化成hex字符串
	//数据组包
	Ql_memmove(strCmdBuf+1,strCmdBuf,Ql_strlen(strCmdBuf));
	*strCmdBuf = '$';
	Ql_strcat(strCmdBuf,"*");
	Ql_strcat(strCmdBuf,tmp);
	APP_DEBUG("\r\n...%s...\r\n",strCmdBuf);
	//需要的时候打开
	//ret = Mtk_SendBinCmd(strCmdBuf);
	return ret;
}

bool MTK_SendCmd_Packet_Type_185(const char *message)
{
	char strCmdBuf[20] = {'\0'};
	char tmp[2];
	u8 verifyCode;
	bool ret = FALSE;
	
	Ql_strcpy(strCmdBuf,"PMTK185,");
	Ql_strcat(strCmdBuf,message);
	verifyCode = GetXorVerify(strCmdBuf,Ql_strlen(strCmdBuf));//计算校验码
	Ql_sprintf(tmp, "%02x", verifyCode);//hex格式化成hex字符串
	//数据组包
	Ql_memmove(strCmdBuf+1,strCmdBuf,Ql_strlen(strCmdBuf));
	*strCmdBuf = '$';
	Ql_strcat(strCmdBuf,"*");
	Ql_strcat(strCmdBuf,tmp);
	APP_DEBUG("\r\n...%s...\r\n",strCmdBuf);
	//需要的时候打开
	//ret = Mtk_SendBinCmd(strCmdBuf);
	return ret;
}

bool SDK_PQEPE_SendCmdToWrite(const char *mode,const char *save)
{
	char strCmdBuf[20] = {'\0'};
	char tmp[2];
	u8 verifyCode;
	bool ret = FALSE;
	
	Ql_strcpy(strCmdBuf,"PQEPE,W,");
	Ql_strcat(strCmdBuf,mode);
	Ql_strcat(strCmdBuf,",");
	Ql_strcat(strCmdBuf,save);
	verifyCode = GetXorVerify(strCmdBuf,Ql_strlen(strCmdBuf));//计算校验码
	Ql_sprintf(tmp, "%02x", verifyCode);//hex格式化成hex字符串
	//数据组包
	Ql_memmove(strCmdBuf+1,strCmdBuf,Ql_strlen(strCmdBuf));
	*strCmdBuf = '$';
	Ql_strcat(strCmdBuf,"*");
	Ql_strcat(strCmdBuf,tmp);
	APP_DEBUG("\r\n...%s...\r\n",strCmdBuf);
	//需要的时候打开
	//ret = Mtk_SendBinCmd(strCmdBuf);
	return ret;
}

bool SDK_PQGLP_SendCmdToRead(void)
{
	bool ret = FALSE;

	//需要的时候打开
	//ret = Mtk_SendBinCmd("$PQGLP,R*24");
	return ret;
}

bool SDK_PQGLP_SendCmdToWrite(const char *mode,const char *save)
{
	char strCmdBuf[20] = {'\0'};
	char tmp[2];
	u8 verifyCode;
	bool ret = FALSE;
	
	Ql_strcpy(strCmdBuf,"PQGLP,W,");
	Ql_strcat(strCmdBuf,mode);
	Ql_strcat(strCmdBuf,",");
	Ql_strcat(strCmdBuf,save);
	verifyCode = GetXorVerify(strCmdBuf,Ql_strlen(strCmdBuf));//计算校验码
	Ql_sprintf(tmp, "%02x", verifyCode);//hex格式化成hex字符串
	//数据组包
	Ql_memmove(strCmdBuf+1,strCmdBuf,Ql_strlen(strCmdBuf));
	*strCmdBuf = '$';
	Ql_strcat(strCmdBuf,"*");
	Ql_strcat(strCmdBuf,tmp);
	APP_DEBUG("\r\n...%s...\r\n",strCmdBuf);
	//需要的时候打开
	//ret = Mtk_SendBinCmd(strCmdBuf);
	return ret;
}


#endif
