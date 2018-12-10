#ifndef __SYSCTRL__H_
#define __SYSCTRL__H_


////////////////////////////////////////////////////////////////////IP参数结构体
typedef struct
{
    ST_GprsConfig m_GprsConfig;//GPRS PDP context     
    u8  mode[4];//TCP或UDP    
    u8  ip[30]; //IP或域名
    u32  port;//端口
}SOC_IPVAL;//IP参数结构体



/**
  * @brief  发送缓冲数据到中心
  * @param    channel:数据通道
  * @param    srcdat :指向发送数据
  * @param    srclen :发送数据长度
  * @retval 返回发送结果
  *                              0 :失败  
  *                     等于srclen :成功
  */
u16 Communication_SndMsg2Svr(u8 channel, u8 * srcdat, u16 srclen);

/*
  * @brief  打开连接
  * @param  None
  * @retval None
  */
void Net_First_Open(void);
/*
  * @brief  关闭连接
  * @param  None
  * @retval None
  */
void Net_First_Close(void);




#endif

