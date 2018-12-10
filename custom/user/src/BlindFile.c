#include "include.h"

//创建盲区存取文件
void CreatBlindFile(void)
{
	s32 handle=-1;
	u32	temp=0;
	s32 ret = 0;
	u32 wrneum=0;
	handle = Ql_FS_Open(BLINDFILENAME,QL_FS_READ_WRITE);
	if(handle<0)//没有盲区存取文件，新建盲区存取文件，初始化的时候一定要写文件为0XFF
		{
			APP_DEBUG("\r\n<-- Creat CreatBlindFile -->\r\n");
			Ql_memset(ShareBuffer,0xff, RADIOPROTOCOL_BUFFER_SIZE);
			handle = Ql_FS_Open(BLINDFILENAME,QL_FS_CREATE_ALWAYS); //if file does not exist ,creat it
			while(temp<(FLASH_ONE_SECTOR_BYTES*(FLASH_BLIND_END_SECTOR+1)))
			{
			  ret = Ql_FS_Seek(handle,0,QL_FS_FILE_END);  //seek end 
			  ret = Ql_FS_Write(handle, ShareBuffer, RADIOPROTOCOL_BUFFER_SIZE,&wrneum );
			  Ql_FS_Flush(handle); //fflush强制缓存写入文件
			  temp=temp+RADIOPROTOCOL_BUFFER_SIZE;
			}
		 Ql_FS_Close(handle);//close the file
		 APP_DEBUG("\r\n<-- Creat CreatBlindFile Finsh  FileEndAddr=%d-->\r\n",temp);
			 return 1;
		}
	Ql_FS_Close(handle);//close the file
	return 0;
}


void BlindTextDebug(u8 *debugData,u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
		{
			APP_DEBUG("%x ",debugData[i]);
		}
		APP_DEBUG("\r\n");
}

//从指定地址读取若干字节
void sFLASH_ReadBuffer(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)
{
	u8 tmp;
	tmp=E2prom_ReadBytes(BLINDFILENAME,ReadAddr, pBuffer, NumByteToRead);
	if(tmp==0)
		{
			tmp=E2prom_ReadBytes(BLINDFILENAME,ReadAddr, pBuffer, NumByteToRead);
		}
	//APP_DEBUG("\r\n<-- BlindFile Read ReadAddr=%d,NumByteToRead=%d,ReadNum=%d,Content: -->\r\n",ReadAddr,NumByteToRead,tmp);
	//BlindTextDebug(pBuffer,NumByteToRead);

}

//写入指定地址若干字节
void sFLASH_WriteBuffer(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
	u8 tmp;
	tmp=E2prom_WriteBytes(BLINDFILENAME,WriteAddr, pBuffer,NumByteToWrite);
	if(tmp==0)
		{
			tmp=E2prom_WriteBytes(BLINDFILENAME,WriteAddr, pBuffer,NumByteToWrite);
		}
	APP_DEBUG("\r\n<-- Write WriteAddr=%d,NumByteToWrite=%d,WriteNum=%d,Content: -->\r\n",WriteAddr,NumByteToWrite,tmp);

	//调试代码
	BlindTextDebug(pBuffer,NumByteToWrite);

	
}
//擦除块
void sFLASH_EraseSector(u32 SectorAddr)
{
	//u8 tmp=0xFF,i=0;
	
	Ql_memset(ShareBuffer,0xff, RADIOPROTOCOL_BUFFER_SIZE);//盲区做擦除操作的时候一定要写为0XFF
	E2prom_WriteBytes(BLINDFILENAME,SectorAddr, ShareBuffer,RADIOPROTOCOL_BUFFER_SIZE);
	E2prom_WriteBytes(BLINDFILENAME,SectorAddr+RADIOPROTOCOL_BUFFER_SIZE, ShareBuffer,RADIOPROTOCOL_BUFFER_SIZE);
	
	E2prom_WriteBytes(BLINDFILENAME,SectorAddr+2*RADIOPROTOCOL_BUFFER_SIZE, ShareBuffer,RADIOPROTOCOL_BUFFER_SIZE);
	E2prom_WriteBytes(BLINDFILENAME,SectorAddr+3*RADIOPROTOCOL_BUFFER_SIZE, ShareBuffer,RADIOPROTOCOL_BUFFER_SIZE);
	//APP_DEBUG("\r\n<-- EraseBlindSector%d-%d-%d-%d -->\r\n",SectorAddr,SectorAddr+RADIOPROTOCOL_BUFFER_SIZE,SectorAddr+2*RADIOPROTOCOL_BUFFER_SIZE,SectorAddr+3*RADIOPROTOCOL_BUFFER_SIZE);

	//for(i=0;i<FLASH_ONE_SECTOR_BYTES;i++)
	//{
	//	E2prom_WriteBytes(BLINDFILENAME,SectorAddr+i, &tmp,1);
	//}
}



