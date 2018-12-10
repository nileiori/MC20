#ifndef _BLINDFILE_H
#define _BLINDFILE_H

#define BLINDFILENAME   "BlindFile.txt"

#define	FLASH_ONE_SECTOR_BYTES	4096//每个扇区4096字节
#define	FLASH_BLIND_START_SECTOR            0//盲区,18扇区
#define FLASH_BLIND_END_SECTOR              8//500条盲区,预留1扇区以备存储参数
#define FLASH_BLIND_STEP_LEN                124


//创建盲区存取文件
void CreatBlindFile(void);


//写入指定地址若干字节
void sFLASH_ReadBuffer(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);

//从指定地址读取若干字节
void sFLASH_WriteBuffer(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);

//擦除块
void sFLASH_EraseSector(u32 SectorAddr);



#endif



