/************************************************************************
//程序名称：Blind.c
//功能：该模块实现盲区补报功能。
//版本号：V0.1
//版权说明：版权属于深圳市伊爱高新技术开发有限公司
//开发人：dxl
//开发时间：2014.10
//版本记录：版本记录需包含版本、修改人、修改时间、修改原因、修改简要说明
//V0.1：每次把5条位置信息打包补报，只有收到平台应答后才会补报下一包，
//否则一直补报当前包，最多发送10次，每次20秒。若10次后仍然没有应答
//则会等待30分钟后再重发当前包。
*************************************************************************/

/********************文件包含*************************/
#include <stdio.h>
#include <string.h>
#include "Blind.h"
#include "Public.h"
#include "RadioProtocol.h"
#include "report.h"
#include "tcp_log.h"

/********************外部变量*************************/
extern u8 ShareBuffer[];

static u8 							reportAck = 0;
static CirQueueHandle_t pBlindCtrl = NULL;
static CirQueue_t 			BlindCtrlBackup;//副本
static int 							iSemaphoreId;
static u8  							blindProbe = 0;// 探针
static u8 							shamLinkCount = 0;

//更新节点信息
void Blind_UpdateCirQueueInfo(void);
QueueStatus Blind_IsCirQueueEmpty(CirQueueHandle_t xCirQueue);
static void Blind_NodeBackup(void);
static void Blind_NodeResume(void);

//盲区数据打包上报
QueueStatus Blind_DataPackUploading(CirQueueHandle_t xCirQueue,u32 timeout);
//初始化
void Blind_Init(void);

//盲区数据存储结构,盲区信息是固定存储地址，为检索而设定
/*

____________________________________________________________________________
|盲区信息|盲		区		数		据		循		环		存		储	|
|_____________|_____________________________________________________________|
|			   |																|
|_____________|_____________________________________________________________|


*/
void proc_blind_task(s32 taskId)
{
	QueueStatus upSta;
	u8 terminalAuthFlag;
	Blind_Init();
	while(1)
	{
		Blind_TakeSemaphore(TRUE);	
		
		//盲区上报
		terminalAuthFlag = GetTerminalAuthorizationFlag();
		APP_DEBUG("\r\n<-- terminalAuthFlag = %d -->\r\n",terminalAuthFlag);
		upSta = Blind_DataPackUploading(pBlindCtrl,BLIND_UP_OVERTIME);
		if(Q_SUCCESS == upSta)
		{
			APP_DEBUG("\r\n<-- 继续盲区上报 ....-->\r\n");
		}
		else
		{
			CirQueue_t * const pxCirQueue = (CirQueue_t *)pBlindCtrl;
			APP_DEBUG("\r\n...盲区上报完成 ...剩余盲区条数为:%d...head:%d...tail:%d\r\n", \
			pxCirQueue->queueLen.ulData,pxCirQueue->head.ulData,pxCirQueue->tail.ulData);
			//Blind_UpdateCirQueueInfo();
			Ql_Sleep(300000);//每过5分钟检测一次防止有新的盲区产生
			if(terminalAuthFlag)
			{
				Blind_GiveSemaphore();
			}
		}
	}
}

/*********************************************************************************************************
*                                          Blind_DataSave
*
* Description : 存储盲区数据
*
* Argument(s) : pBuffer--->数据指针
*			   Addr----->数据地址
*			   length---->数据长度
*
* Return(s)   : none
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************/
void Blind_DataSave(u8 *pBuffer, u32 Addr,u32 length)
{
	sFLASH_WriteBuffer(pBuffer,Addr,length);//写入flash
}
/*********************************************************************************************************
*                                          Blind_IsCirQueueEmpty
*
* Description : 队列判空，如果长度=0则为空，这时没有元素可以出队
*
* Argument(s) : 循环队列指针
*
* Return(s)   : 队列状态
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************/
QueueStatus Blind_IsCirQueueEmpty(CirQueueHandle_t xCirQueue)
{
	CirQueue_t * const pxCirQueue = (CirQueue_t *)xCirQueue;
	
  if (pxCirQueue->queueLen.ulData == 0)
  {
      return Q_SUCCESS;
  }
  return Q_FAILED;
}

/*********************************************************************************************************
*                                          Blind_IsCirQueueFull
*
* Description : 队列判满，如果长度=容量，则此时不能有元素入队
*
* Argument(s) : 循环队列指针
*
* Return(s)   : 队列状态
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************/
QueueStatus Blind_IsCirQueueFull(CirQueueHandle_t xCirQueue)
{
	CirQueue_t * const pxCirQueue = (CirQueue_t *)xCirQueue;
	
  if (pxCirQueue->queueLen.ulData == pxCirQueue->queueCapacity.ulData)
  {
      return Q_SUCCESS;
  }
	return Q_FAILED;
}

/*********************************************************************************************************
*                                          Blind_GetCirQueueLength
*
* Description : 获取队列长度
*
* Argument(s) : 循环队列指针
*
* Return(s)   : 队列长度
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************/
int	Blind_GetCirQueueLength(CirQueueHandle_t xCirQueue)
{
	CirQueue_t * const pxCirQueue = (CirQueue_t *)xCirQueue;
	
	return	pxCirQueue->queueLen.ulData;
}

/*********************************************************************************************************
*                                          Blind_EnCirQueue
*
* Description : 多元素入队
*
* Argument(s) : pBuffer:待入队的数据指针 
*								BufferLen:入队数据长度
*								pQ:循环队列指针
*
* Return(s)   : 入队数据长度
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************/
QueueStatus Blind_EnCirQueue(u8 *pBuffer, u32 BufferLen,CirQueueHandle_t xCirQueue)
{
	u8* pBuf = pBuffer;
	CirQueue_t * const pxCirQueue = (CirQueue_t *)xCirQueue;

	if((0 == BufferLen)||(NULL == pBuf))
	{
		return Q_FAILED;
	}
	
    if (Q_FAILED == Blind_IsCirQueueEmpty(xCirQueue))//盲区不为空
	{
		if(pxCirQueue->tail.ulData == pxCirQueue->head.ulData)//如果追尾
		{
		//头部前进,盲区被新的数据覆盖了
			pxCirQueue->head.ulData++;
			pxCirQueue->head.ulData = pxCirQueue->head.ulData % pxCirQueue->queueCapacity.ulData;
		}
	}
	//存储数据
	pxCirQueue->startAddr.ulData = FLASH_BLIND_AREA_SAVE_START_ADDR + pxCirQueue->tail.ulData*FLASH_BLIND_STEP_LEN;
	pxCirQueue->dataSaveLen.ulData = BufferLen;
	Ql_memmove(pBuf+4,pBuf,BufferLen);
	Ql_memcpy(pBuf,pxCirQueue->dataSaveLen.ucData,4);//数据长度也要存储
	Blind_DataSave(pBuf,pxCirQueue->startAddr.ulData,BufferLen+4);
	APP_DEBUG("\r\n...写入盲区地址为:%d盲区长度为:%d\r\n", pxCirQueue->startAddr.ulData,pxCirQueue->dataSaveLen.ulData);
	pxCirQueue->tail.ulData++;//队列尾部递增
	//因为队列是环形，所以tail需要通过取模来实现转回到0位置
	pxCirQueue->tail.ulData = pxCirQueue->tail.ulData%pxCirQueue->queueCapacity.ulData;

	//如果队列没满就加1，否则就保持满吧
	if (Q_FAILED == Blind_IsCirQueueFull(xCirQueue))
	{
	  pxCirQueue->queueLen.ulData++;
	}
	APP_DEBUG("\r\n...当前盲区条数为:%d...head:%d...tail:%d\r\n", \
		pxCirQueue->queueLen.ulData,pxCirQueue->head.ulData,pxCirQueue->tail.ulData);
	//更新节点信息把节点信息写入Flash,这里是为了防止异常关机导致盲区丢失，毕竟防不胜防
	//if(0 == pxCirQueue->queueLen.ulData % 20)
	//Blind_UpdateCirQueueInfo();
	
	return Q_SUCCESS;
}

//保存盲区数据
u8 Blind_Link1Save(u8 *pBuffer, u8 length, u8 attribute)
{
	Blind_EnCirQueue(pBuffer, length,pBlindCtrl);
	if(blindProbe)
	{
		blindProbe = 0;
	}
}

/*********************************************************************************************************
*                                          Blind_DeCirQueue
*
* Description : 元素出队
*
* Argument(s) : pBuffer:出队的数据指针 
*			   xCirQueue:循环队列指针
*
* Return(s)   : 出队列的数据长度(数据体长度( 2字节)+ 数据体的长度)
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************/
u32 Blind_DeCirQueue(u8 *pBuffer,CirQueueHandle_t xCirQueue)
{
	BLIND_SAVE_STU temp;
	u8* pBuf = pBuffer;
	CirQueue_t * const pxCirQueue = (CirQueue_t *)xCirQueue;
	if (Q_SUCCESS == Blind_IsCirQueueEmpty(xCirQueue))
	{
		//强制清零
		//pxCirQueue->head.ulData = 0;
		//pxCirQueue->tail.ulData = 0;
	  	return 0;
	}
	
	//寻址
	pxCirQueue->startAddr.ulData = FLASH_BLIND_AREA_SAVE_START_ADDR + pxCirQueue->head.ulData*FLASH_BLIND_STEP_LEN;
	//读取盲区数据长度
	sFLASH_ReadBuffer(temp.dataSaveLen.ucData,pxCirQueue->startAddr.ulData,4);
	//APP_DEBUG("\r\n...读取盲区地址为:%d盲区长度为:%d\r\n", pxCirQueue->startAddr.ulData,temp.dataSaveLen.ulData);
	//读取盲区数据
	*pBuf++ = temp.dataSaveLen.ucData[1];
	*pBuf++ = temp.dataSaveLen.ucData[0];
	sFLASH_ReadBuffer(pBuf,pxCirQueue->startAddr.ulData+4,temp.dataSaveLen.ulData);
	pxCirQueue->head.ulData++;
	//因为队列是环形，所以head需要通过取模来实现转回到0位置
	pxCirQueue->head.ulData = pxCirQueue->head.ulData % pxCirQueue->queueCapacity.ulData;
	pxCirQueue->queueLen.ulData--; 
	
	return ( temp.dataSaveLen.ulData+2);//加上两个字节的数据长度

}

typedef struct
{
	u16	itemNum;
	u32	blindDataLens;
}BLIND_PACK_INFO;
/*********************************************************************************************************
*                                          Blind_DataPack
*
* Description : 盲区数据打包
*
* Argument(s) : pBuffer:出队的数据指针 
*			   xCirQueue:循环队列指针
*
* Return(s)   : 数据项个数和数据项长度的结构体
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************/
BLIND_PACK_INFO Blind_DataPack(u8 *pBuffer,CirQueueHandle_t xCirQueue)
{
	u8 i;
	u8* pBuf = pBuffer;
	u32 dataLen=0;
	BLIND_PACK_INFO upInfo={0,0};
	//s32 ret;
	CirQueue_t * const pxCirQueue = (CirQueue_t *)xCirQueue;
	
	for(i = 0;i < FLASH_BLIND_PACK_NUM;i++)
	{
		dataLen = Blind_DeCirQueue(pBuf,xCirQueue);
		pBuf += dataLen;
		if(dataLen)
		{
			upInfo.itemNum++;
		}
		upInfo.blindDataLens += dataLen;
	}
	
	return upInfo;
}


/*********************************************************************************************************
*                                          Blind_DataPackUploading
*
* Description : 盲区数据打包
*
* Argument(s) : timeout:盲区定时器回调间隔
*			   xCirQueue:循环队列指针
*
* Return(s)   : none
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************/
QueueStatus Blind_DataPackUploading(CirQueueHandle_t xCirQueue,u32 timeout)
{
	BLIND_PACK_INFO upInfo;
	s32	ret;
	CirQueue_t * const pxCirQueue = (CirQueue_t *)xCirQueue;

	if (Q_SUCCESS == Blind_IsCirQueueEmpty(xCirQueue))
	{
		
	  	return Q_FAILED;
	}

	//backup
	Blind_NodeBackup();
	
	upInfo = Blind_DataPack(ShareBuffer+3,xCirQueue);
	
	ShareBuffer[0] = (u8)(upInfo.itemNum>>8);//两个字节表示项数
	ShareBuffer[1] = (u8)upInfo.itemNum;
	ShareBuffer[2] = 1;//0:正常的位置批量汇报;1:盲区补报
	//ShareBuffer[3] = (u8)(upInfo.blindDataLens>>8);//两个字节位置汇报信息长度
	//ShareBuffer[4] = (u8)upInfo.blindDataLens;
	APP_DEBUG("\r\n<-- Blind_Report %d   %d-->\r\n",upInfo.itemNum,upInfo.blindDataLens);
	RadioProtocol_PostionInformation_BulkUpTrans(ShareBuffer,upInfo.blindDataLens+3);

	APP_DEBUG("\r\n...剩余盲区条数为:%d...head:%d...tail:%d\r\n", \
		pxCirQueue->queueLen.ulData,pxCirQueue->head.ulData,pxCirQueue->tail.ulData);
	// 启动盲区上报回调函数定时器,单次
    //ret = Ql_Timer_Start(LOGIC_BLIND_REPORT_TMR_ID, timeout,FALSE);
	Ql_OS_SendMessage(main_task_id, MSG_ID_BLIND_CALLBACK, timeout, 0);
	blindProbe = 1;// 探针启动
	return Q_SUCCESS;
}

static void Blind_NodeBackup(void)
{
	CirQueue_t * const pxCirQueue = (CirQueue_t *)pBlindCtrl;

	Ql_memcpy((void*)&BlindCtrlBackup.head,(void*)&pxCirQueue->head,sizeof(BLIND_SAVE_STU));
}

static void Blind_NodeResume(void)
{
	CirQueue_t * const pxCirQueue = (CirQueue_t *)pBlindCtrl;

	Ql_memcpy((void*)&pxCirQueue->head,(void*)&BlindCtrlBackup.head,sizeof(BLIND_SAVE_STU));
	//在盲区上报期间又写过盲区要特殊处理
	if(0 == blindProbe)
	{
		if(pxCirQueue->tail.ulData == pxCirQueue->head.ulData)//如果追尾
		{
		//头部前进,盲区被新的数据覆盖了
			pxCirQueue->head.ulData++;
			pxCirQueue->head.ulData = pxCirQueue->head.ulData % pxCirQueue->queueCapacity.ulData;
		}
		pxCirQueue->tail.ulData++;//队列尾部递增
		//因为队列是环形，所以tail需要通过取模来实现转回到0位置
		pxCirQueue->tail.ulData = pxCirQueue->tail.ulData%pxCirQueue->queueCapacity.ulData;

		//如果队列没满就加1，否则就保持满吧
		if (Q_FAILED == Blind_IsCirQueueFull(pBlindCtrl))
		{
		  pxCirQueue->queueLen.ulData++;
		}					
	}
}
//更新节点信息
void Blind_UpdateCirQueueInfo(void)
{
	CirQueue_t * const pxCirQueue = (CirQueue_t *)pBlindCtrl;

	pxCirQueue->crc32Verify.ulData = Public_CRC_32( pxCirQueue->head.ucData, FLASH_BLIND_AREA_SAVE_START_ADDR - 4 );
	sFLASH_WriteBuffer(pxCirQueue->head.ucData,0,FLASH_BLIND_AREA_SAVE_START_ADDR);
}

/**************************************************************************
//函数名：Blind_Link1ReportAck
//功能：连接1盲区补报应答处理
//输入：无
//输出：无
//返回值：无
//备注：收到连接1的盲区补报应答时需调用此函数
***************************************************************************/
void Blind_Link1ReportAck(void)
{
	reportAck = 1;
}

u8 Blind_GetLink1ReportAck(void)
{
	return reportAck;
}

void Blind_ClrLink1ReportAck(void)
{
	reportAck = 0;
	shamLinkCount = 0;
}

//创建一个信号量
int Blind_CreateSemaphore(void)
{
	return Ql_OS_CreateSemaphore("MySemaphore", 1);
}
//给出信号量
void Blind_GiveSemaphore(void)
{
	Ql_OS_GiveSemaphore(iSemaphoreId);
}
//获取信号量
void Blind_TakeSemaphore(bool wait)
{
	Ql_OS_TakeSemaphore(iSemaphoreId,wait);
}

//盲区上报回调函数
void Blind_Report_Callback_OnTimer(u32 timerId, void* param)
{	
    if(LOGIC_BLIND_REPORT_TMR_ID == timerId)
	{
		if(0 == Blind_GetLink1ReportAck())//盲区上报没回应
		{
			//resume
			Blind_NodeResume();
			//如果在线,又老是不回应，说明是虚链接了特么的
			if(GetTerminalAuthorizationFlag())
			{
				APP_DEBUG("Blind_Report_Callback_OnTimer......虚链接...%d\r\n",shamLinkCount);
				if(shamLinkCount++ > 5)
				{				
					shamLinkCount = 0;
					ClearTerminalAuthorizationFlag(CHANNEL_DATA_1);
					//Socket_Restart();//重新走tcp初始化流程
					Net_First_Close();
					//Blind_UpdateCirQueueInfo();
					//Report_RtcWakeUpOffsetSave();
					//Ql_Reset(0);
				}
			}
		}
		else
		{			
			Blind_ClrLink1ReportAck();//清除盲区上报应答标志
		}
		
		//如果在线,给出信号量,继续盲区上报
		if(GetTerminalAuthorizationFlag())
		{
			if (Q_SUCCESS == Blind_IsCirQueueEmpty(pBlindCtrl))
			{
				//更新节点信息把节点信息写入Flash
				Blind_UpdateCirQueueInfo();
			}
			Blind_GiveSemaphore();//释放信号量
		}
		else
		{
			Blind_UpdateCirQueueInfo();
		}
		blindProbe = 0; //无论如何要归位
	}
}
/*
《蝶恋花·花褪残红青杏小》
花褪残红青杏小。燕子飞时，绿水人家绕。
枝上柳绵吹又少，天涯何处无芳草。
墙里秋千墙外道。墙外行人，墙里佳人笑。
笑渐不闻声渐悄，多情却被无情恼。
*/
//获取盲区上报状态
u8 Blind_GetReportState(void)
{
	return (u8)Blind_IsCirQueueEmpty(pBlindCtrl);	
}


/*********************************************************************************************************
*                                          Blind_CirQueueGenericCreate
*
* Description : 创建一个循环队列
*
* Argument(s) : uxQueueLength:队列的容量
*
* Return(s)   : 申请的队列指针
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************/
CirQueueHandle_t Blind_CirQueueGenericCreate(const u32 uxQueueLength)
{
	CirQueue_t * pxCirQueue;

	pxCirQueue = (CirQueue_t *)Ql_MEM_Alloc(sizeof(CirQueue_t));
	if( pxCirQueue == NULL )
	{
		return NULL;
	}
	Ql_memset(pxCirQueue->head.ucData,0,FLASH_BLIND_AREA_SAVE_START_ADDR);
	pxCirQueue->startAddr.ulData = FLASH_BLIND_AREA_SAVE_START_ADDR;
	pxCirQueue->queueCapacity.ulData = uxQueueLength;

	return pxCirQueue;
}
/*
《自题金山画像》
心似已灰之木，身如不系之舟。
问汝平生功业，黄州惠州儋州。
*/
//注册一个定时器用来控制盲区上报
void Blind_Timer_Register(void)
{	
	s32 ret = Ql_Timer_Register(LOGIC_BLIND_REPORT_TMR_ID, Blind_Report_Callback_OnTimer, NULL);
	APP_DEBUG("\r\nBlind_Timer_Register	%d......\r\n",ret);
}
/*********************************************************************************************************
*                                          Blind_Init
*
* Description : 盲区初始化
*
* Argument(s) : 
*
* Return(s)   : 
*
* Caller(s)   : none.
*
* Note(s)     : none.
*********************************************************************************************************/
void Blind_Init(void)
{
	BLIND_SAVE_STU temp;
	u32 crc32Verify;
	s32 ret;
	CirQueue_t * pxCirQueue;
	//创建一个信号量
	iSemaphoreId = Blind_CreateSemaphore();
	//创建一个互斥量
	//iMutexId = Ql_OS_CreateMutex("MyMutex");
	//申请节点空间
	pBlindCtrl = Blind_CirQueueGenericCreate(FLASH_BLIND_AREA_SAVE_NUM);
	if(NULL == pBlindCtrl)
	{
		APP_DEBUG("blind init failed,System is about to restart......\r\n");	
		Ql_Reset(0);
	}
	APP_DEBUG("blind init success......\r\n");
	pxCirQueue = (CirQueue_t *)pBlindCtrl;
	
	sFLASH_ReadBuffer(temp.head.ucData,0,FLASH_BLIND_AREA_SAVE_START_ADDR);
	crc32Verify = Public_CRC_32( temp.head.ucData, FLASH_BLIND_AREA_SAVE_START_ADDR - 4 );
	//CRC校验
	if(crc32Verify == temp.crc32Verify.ulData)
	{
		Ql_memcpy(pxCirQueue->head.ucData,temp.head.ucData,FLASH_BLIND_AREA_SAVE_START_ADDR);
		APP_DEBUG("Node check success......%04x    %04x...\r\n",crc32Verify,temp.crc32Verify.ulData);
	}
	else
	{
	//更新节点信息把节点信息写入Flash
		Blind_UpdateCirQueueInfo();
	}
	//保险一点
	pxCirQueue->queueCapacity.ulData = FLASH_BLIND_AREA_SAVE_NUM;
	//先释放信号量
	Blind_TakeSemaphore(FALSE);
}



