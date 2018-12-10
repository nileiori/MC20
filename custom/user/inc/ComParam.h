/*******************************************************************************
 * File Name:			ComParam.c 
 * Function Describe:	
 * Relate Module:		测试相关协议。
 * Writer:				Joneming
 * Date:				2011-09-08
 * ReWriter:			
 * Date:				
 *******************************************************************************/
 
#ifndef __COMPARAM__H_
#define __COMPARAM__H_
/************************************************************
** 函数名称: ComParam_RecvOneData
** 功能描述: 数据接收中断
** 入口参数:
** 出口参数:
************************************************************/  
void ComParam_RecvOneData(unsigned char data);
/*******************************************************************************
* Function Name  : Recorder_DisposeRecvData
* Description    : 行驶记录仪数据分析
* Input          : - *pData  : 指向分析的数据
*                  - Length  : 分析数据长度  
* Output         : None
* Return         : - 收到一帧完整且正确数据返回真，否则返回假
*******************************************************************************/
unsigned char ComParam_DisposeRecvData(unsigned char *pRecvData,unsigned short Length);
#endif

/*******************************************************************************
 *                             end of module
 *******************************************************************************/


