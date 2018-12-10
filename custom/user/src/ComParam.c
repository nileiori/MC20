#include "include.h"
//#include "ComParam.h"

#define PARAM_UART_PORT UART_PORT1//用于参数数据收发的串口
/*******************************************************************************
* Function Name  : ComParam_Error_Cmd
* Description    : 向串口发送数据错误时的应答帧数据
*                  送错误帧数据
* Input          : - Cmd  : 行驶记录仪命令字              
* Output         : None
* Return         : None
*******************************************************************************/
void ComParam_Error_Cmd(u8 Cmd)
{
    u8 Buffer[5];
    Buffer[0] = 0x55;
    Buffer[1] = 0x7a;   
    if(Cmd < 0x80)                     /*上载命令出错应答    */
    {
        Buffer[2] = 0xfa;        
    }
    else                                                /*下传命令出错应答    */
    {
        Buffer[2] = 0xfb;
    }
    Buffer[3] = 0;
    Buffer[4] = Public_GetXorVerify(Buffer, 4);
    Ql_UART_Write(PARAM_UART_PORT,Buffer,5);
}
/*******************************************************************************
* Function Name  : ComParam_Handle
* Description    : 处理行驶记录仪数据处理
* Input          : None              
* Output         : None
* Return         : None
*******************************************************************************/
void ComParam_DisposeParamCmd(u8 cmd ,u8 *pRec,u16 length)
{
    s16 nLen=0;                                      /*反馈据长度             */
    u8  verify;                                      /*反馈数据校验字节       */
    u8	*pSend;                                      /*反馈数据指针           */
    u8  buffer[200];
    u8 flag;
    flag=*(pRec+6);
    pSend = buffer;                                    /*初始化发送数据地址     */
    if(0 == flag)                       /*读参数                 */
    {
        E2ParamApp_DisposeReadParam(pSend+7, &nLen,pRec+7, length-1);
        nLen++;                              /*加上读标志             */
    }
    else //写参数
    {
        verify=E2ParamApp_DisposeWriteParam(pRec+7, length-1);
        if(verify)return;
        nLen = 1;
    }
    /////////////////////////////////////
    *pSend     = 0x55;
    *(pSend+1) = 0x7a;
    *(pSend+2) = cmd;
    *(pSend+3) = nLen >> 8;              /*长度                   */
    *(pSend+4) = nLen;
    *(pSend+5) = 0x12;
    *(pSend+6) = flag;                      /*读写标志                 */
    ///////////////
    verify = Public_GetXorVerify(pSend,nLen+6);
    /////////
    *(pSend+6+nLen) = verify;
    //ComParam_SendData(pSend, nLen+7);
    Ql_UART_Write(PARAM_UART_PORT,pSend,nLen+7);
}
/*******************************************************************************
* Function Name  : ComParam_DisposeRecvData
* Description    : 行驶记录仪数据分析
* Input          : - *pData  : 指向分析的数据
*                  - Length  : 分析数据长度  
* Output         : None
* Return         : - 收到一帧完整且正确数据返回真，否则返回假
*******************************************************************************/
unsigned char ComParam_DisposeRecvData(unsigned char *pRecvData,unsigned short Length)
{
    unsigned char verify,cmd;
    unsigned short len;
    if(Length<7)return 1;
    /////////////////
    if((0xAA != pRecvData[0])||(0x75 != pRecvData[1]))return 1;
    //////////////////
    cmd = pRecvData[2];
    ////////////
    len = Public_ConvertBufferToShort(&pRecvData[3]);
     ////////////////
    if((0xd0 != cmd)||(pRecvData[6] > 1))return 2;//不是参数读写
    ///////////
    if(len+7>Length)//长度不对
    {
        ComParam_Error_Cmd(cmd);                      /*向串口发送错误帧数据  */   
        return 3;
    }
    ///////////
    verify=Public_GetXorVerify(pRecvData, len+6);
    if(verify != pRecvData[len+6])//校验不对
    {
        ComParam_Error_Cmd(cmd);                      /*向串口发送错误帧数据  */   
        return 4;
    }  
    /////////////////
    ComParam_DisposeParamCmd(cmd,pRecvData,len);
    ///////////////////
    return 0;
}

