#ifndef __E2PARAM_APP__H
#define __E2PARAM_APP__H
////////////////////////////////

/////////////////////
#define USE_E2_PARAM_APP
/*************************************************************
** 函数名称: E2ParamApp_ConvertPramID
** 功能描述: 根据要求转成我们新自定义的ID
** 入口参数: 参数ID
** 出口参数: 
** 返回参数: 我们新自定义的ID
** 全局变量: 
** 调用模块: 
*************************************************************/	
unsigned int E2ParamApp_ConvertPramID(unsigned int usParamID);
/*************************************************************
** 函数名称: E2ParamApp_ConvertPramIDToGroupIndex
** 功能描述: 把参数ID转成相关数组下标
** 入口参数: 参数ID
** 出口参数: 
** 返回参数: 数组下标
** 全局变量: 
** 调用模块: 
*************************************************************/	
unsigned short E2ParamApp_ConvertPramIDToGroupIndex(unsigned short PramID);
/*************************************************************
** 函数名称: E2ParamApp_DisposeProtocolWriteParam
** 功能描述: 处理协议写参数
** 入口参数: pBuffer数据首地址,BufferLen数据长度
** 出口参数: 
** 返回参数: 处理结果
** 全局变量: 
** 调用模块: 
*************************************************************/	
ProtocolACK E2ParamApp_DisposeWriteParam(u8 *pBuffer, u16 BufferLen);
/*************************************************************
** 函数名称: E2ParamApp_ReadOneParam
** 功能描述: 处理协议读参数
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 
** 调用模块: 
*************************************************************/	
unsigned char E2ParamApp_ReadOneParam(unsigned short index,unsigned int PramID,unsigned char *data);
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
unsigned char E2ParamApp_DisposeReadParam(unsigned char *pDstBuffer,s16 *DstBufferLen,unsigned char *pIDBuffer,signed short IDBufferLen);
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
unsigned char E2ParamApp_DisposeReadAllParam(unsigned char *pDstBuffer,s16 *DstBufferLen);
/*************************************************************
** 函数名称: E2ParamApp_ReadSpecialParam
** 功能描述: 读特殊参数
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 
** 调用模块: 
*************************************************************/	
unsigned char E2ParamApp_ReadSpecialParam(unsigned short usParamID,unsigned char *data);
#endif

