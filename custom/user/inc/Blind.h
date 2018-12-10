#ifndef __BLIND_H
#define __BLIND_H

#include "include.h"


#define VERIFY_MODE_HASH	(0)
#define VERIFY_MODE_CRC32	(1)

#define configVerifyMode	(VERIFY_MODE_CRC32)

typedef struct
{
	u32	dataLen;				//节点数据长度盲区节点长度+盲区数据长度
	//u32	dataAddr;				//节点数据地址
}BLIND_NODE;
typedef union
{
	u32 ulData;
	u8	ucData[4];
}UN_BLIND_DATA;
typedef struct
{
	UN_BLIND_DATA	head;					//队列头
	UN_BLIND_DATA	tail;					//队列尾
	UN_BLIND_DATA	queueLen;				//队列长度
	UN_BLIND_DATA	queueCapacity;			//队列容量
	UN_BLIND_DATA	startAddr;			//存储起始地址
	UN_BLIND_DATA	dataSaveLen;			//盲区数据长度
	#if( VERIFY_MODE_CRC32 == configVerifyMode )
	UN_BLIND_DATA	crc32Verify;					//CRC32校验
	#elif( VERIFY_MODE_HASH == configVerifyMode )
	UN_BLIND_DATA	hashVerify;			//哈希校验
	#endif
}BLIND_SAVE_STU;

typedef BLIND_SAVE_STU	CirQueue_t;
typedef void * CirQueueHandle_t;

typedef enum {Q_SUCCESS = 0u,Q_FAILED = !Q_SUCCESS}QueueStatus;

#define BLIND_UP_OVERTIME					(15000)	// 盲区上报超时时间ms
#define	FLASH_BLIND_PACK_NUM				(5)//盲区打包数目
#define	FLASH_BLIND_AREA_SAVE_START_ADDR	(sizeof(BLIND_SAVE_STU))	//循环队列起始地址，前面预留的是存储节点信息的
#define	FLASH_BLIND_AREA_SAVE_END_ADDR		(FLASH_ONE_SECTOR_BYTES*FLASH_BLIND_END_SECTOR)
#define	FLASH_BLIND_AREA_SAVE_NUM			((FLASH_BLIND_AREA_SAVE_END_ADDR-FLASH_BLIND_AREA_SAVE_START_ADDR)/FLASH_BLIND_STEP_LEN-1)

void Blind_Init(void);

//创建一个信号量
int Blind_CreateSemaphore(void);
//给出信号量
void Blind_GiveSemaphore(void);
//获取信号量
void Blind_TakeSemaphore(bool wait);
//注册一个定时器用来控制盲区上报
void Blind_Timer_Register(void);
//更新节点信息
void Blind_UpdateCirQueueInfo(void);

//获取盲区上报状态
u8 Blind_GetReportState(void);

/**************************************************************************
//函数名：Blind_Link1Save
//功能：存储一条连接1的盲区数据
//输入：一条位置信息及长度
//输出：无
//返回值：0为成功，非0为失败，长度超出时会返回失败
//备注：一条盲区数据包括：时间4字节+属性1字节（预留）+校验和1字节+位置信息长度1字节+位置信息若干字节
***************************************************************************/
u8 Blind_Link1Save(u8 *pBuffer, u8 length, u8 attribute);

/**************************************************************************
//函数名：Blind_Link1ReportAck
//功能：连接1盲区补报应答处理
//输入：无
//输出：无
//返回值：无
//备注：收到连接1的盲区补报应答时需调用此函数
***************************************************************************/
void Blind_Link1ReportAck(void);

#endif
