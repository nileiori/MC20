#ifndef __RADIO_PROTOCOL_H
#define __RADIO_PROTOCOL_H

//*********文件包含************
#include "E2ParamApp.h"

typedef enum {
    CHANNEL_SMS    = 0x01,//短信通道,0值保留
    CHANNEL_DATA_1 = 0x02,//数据通道1
    CHANNEL_DATA_2 = 0x04,//数据通道2
}SND_TYPE;//发送数据的类型

//协议与软件监控中心应答类型
typedef struct{
	u16 MessageID;//消息ID
	u16 Attribute;//消息体属性
	u8  PhoneNum[6];//手机号,BCD码
	u16 SerialNum;//消息流水号
	//u16 TotalPackage;//消息总包数
	//u16 CurrentPackage;//当前包序号，从1开始
}MESSAGE_HEAD;


//********宏定义****************
#define	RADIOPROTOCOL_BUFFER_SIZE	1024
#define DEVICE_LOGIN_PASSWORD_LEN       20
#define LOGIN_PASSWORD_LEN              22
extern u8 ShareBuffer[RADIOPROTOCOL_BUFFER_SIZE];

/*********************************************************************
//函数名称	:RadioProtocol_WritePram
//功能		:写参数
//备注		:无线写参数跟有线写参数公用此函数，该函数被0x8103命令调用
*********************************************************************/
#define RadioProtocol_WritePram(pBuffer,BufferLen)  E2ParamApp_DisposeWriteParam(pBuffer,BufferLen)
/*********************************************************************
//函数名称	:RadioProtocol_ReadPram
//功能		:读参数
//输入		:把需要读的参数ID号放入缓冲pIDBuffer中，每个ID四字节，高在前
//返回		:读取的参数总个数，*pDstBuffer按ID号，ID长度，ID内容的顺序存放，*DstBufferLen总的读取长度，不包括参数总个数字节
//备注		:ReadType:0为有线(串口)，每次只能读550字节；1为无线（GPRS），每次可读1000字节
*********************************************************************/
#define RadioProtocol_ReadPram(pDstBuffer,DstBufferLen,pIDBuffer,IDBufferLen)    E2ParamApp_DisposeReadParam(pDstBuffer,DstBufferLen,pIDBuffer,IDBufferLen)


/*********************************************************************
//函数名称	:RadioProtocol_TerminalGeneralAck(u16 SerialNum, u16 MessageID, u8 Result)
//功能		:终端通用应答
//输入		:Result:应答结果,0为成功，1为失败，2消息有误，3不支持
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:成功,返回ACK_OK;失败返回ACK_ERROR;
//备注		:指令ID：0x0001
*********************************************************************/
ProtocolACK RadioProtocol_TerminalGeneralAck(u8 AckResult);
/*********************************************************************
//函数名称	:RadioProtocol_PostionInformationReport(u8 *pBuffer, u16 length)
//功能		:位置信息汇报
//备注		:指令ID：0x0200
*********************************************************************/
ProtocolACK  RadioProtocol_PostionInformationReport(u8 *pBuffer, u16 BufferLen);

/*********************************************************************
//函数名称	:RadioProtocol_SendMessage(MESSAGE_HEAD	head, u8 *SrcData, u16 SrcLen, u8 SendCh)
//功能		:发送消息
//输入		:MESSAGE_HEAD	head:消息头;SrcData:指向消息体的指针;
//		:SrcLen:消息体长度;SendCh:发送通道,0为GPRS,1为短信;
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:解析完一条指令后，通常需要应答，应答时需调用该函数；
//		:主动上发一条消息命令时也需要调用该函数。
*********************************************************************/
ErrorStatus RadioProtocol_SendCmd(MESSAGE_HEAD	head, u8 *SrcData, u16 SrcLen, u8 SendCh );
/*********************************************************************
//函数名称	:RadioProtocol_SendData
//函数功能	:
//备注		:
*********************************************************************/
ProtocolACK RadioProtocol_SendData(u16 cmd, u8 *SrcData, u16 SrcLen, u8 SendCh);
/*********************************************************************
//函数名称	:RadioProtocol_ProtocolParse(u8 *pBuffer, u16 BufferLen)
//功能		:协议解析,解析一条指令
//输入		:pBuffer:指向数据(待解析的数据)的指针;BufferLen:数据的长度
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:成功,返回SUCCESS;失败,返回ERROR
//备注		:平台下发的所有命令都是由该函数解析，该函数的输入参数
//		:是7E开头,7E结尾的一串字节序列
*********************************************************************/
ErrorStatus RadioProtocol_ProtocolParse(u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//函数名称	:unTransMean
//功能		:实现反转义功能
//		:0x7e----->0x7d后跟一个0x02;0x7d----->0x7d后跟一个0x01
//输入		:解析前字节流，解析前字节流长度
//输出		:解析后字节流
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:转义解析后字节流长度
//备注		:协议解析函数需调用此函数，先进行反转义，然后再解析。
*********************************************************************/
/*********************************************************************
//函数名称	:RadioProtocol_CheckRecvData
//功能		:将接受到平台的数据分离出标准部标数据帧，标准部标数据帧7E开头7E结尾
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:接受到平台数据后调用该函数
*********************************************************************/
void RadioProtocol_CheckRecvData(unsigned char *recvData,unsigned short recvDataLen);
u16 unTransMean(u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//函数名称	:TransMean
//功能		:实现打包时的转义处理功能
//输入		:转义前字节流，转义前字节流长度
//输出		:转义后字节流
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:转义后字节流长度
//备注		:0x7e----->0x7d后跟一个0x02;0x7d----->0x7d后跟一个0x01
*********************************************************************/
u16 TransMean(u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//函数名称	:RadioProtocol_HeadtoBytes(u8 *pBuffer, MESSAGE_HEAD head)
//功能		:把消息头变成字节流
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:
*********************************************************************/
u8 RadioProtocol_HeadtoBytes(u8 *pBuffer, MESSAGE_HEAD head);

/*********************************************************************
//函数名称	:RadioProtocol_ntohs(u16 value)
//功能		:网络字节序转主机字节序
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:
*********************************************************************/
u16 RadioProtocol_ntohs(u16 value);
/*********************************************************************
//函数名称	:RadioProtocol_ntoh(u32 value)
//功能		:网络字节序转主机字节序
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:
*********************************************************************/
u32 RadioProtocol_ntoh(u32 value);
/*********************************************************************
//函数名称	:RadioProtocol_htons(u16 value)
//功能		:主机字节序转网络字节序
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:
*********************************************************************/
u16 RadioProtocol_htons(u16 value);
/*********************************************************************
//函数名称	:RadioProtocol_hton(u32 value)
//功能		:主机字节序转网络字节序
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:
*********************************************************************/
u32 RadioProtocol_hton(u32 value);

/*********************************************************************
//函数名称	:NullOperate(u8 *pBuffer, u16 BufferLen)
//功能		:空操作,不干任何事
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:成功,返回ACK_OK;失败返回ACK_ERROR;
//备注		:
*********************************************************************/
ProtocolACK NullOperate(u8 *pBuffer, u16 BufferLen);
/*********************************************************************
//函数名称	:RadioProtocol_ReadDeviceID
//功能		:平台读取终端ID
//备注		:
*********************************************************************/
unsigned char RadioProtocol_ReadDeviceID(unsigned char *data);



//查询终端属性
ProtocolACK RadioProtocol_TerminalAttribute(u8 *pBuffer, u16 BufferLen);



/*********************************************************************
//函数名称	:RadioProtocol_GetCurRecvSerialNum
//功能		:取得当前接收流水号
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:
//备注		:
*********************************************************************/
unsigned short RadioProtocol_GetCurRecvSerialNum(void) ;

/*********************************************************************
//函数名称	:RadioProtocol_GetUpdataResultNoteAckFlag
//功能		:取得升级结果通知应答标志
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:
//返回		:1为收到平台的通用应答了
//备注		:
*********************************************************************/
unsigned char RadioProtocol_GetUpdataResultNoteAckFlag(void) ;

#endif

