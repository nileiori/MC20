/*******************************************************************************
 * File Name    :	Version.h
 * File Describe:	版本声明及与版本相关定义
 * Relate Module:	Version.h
 * Writer		:   joneming
 * Date			:   2014-04-11
**********************************************************************************/
#ifndef __VERSION_H_
#define __VERSION_H_
/******************************************************************************/
/****************************************************/
/*                                                  */
/* Definitions                                      */
/*                                                  */
/****************************************************/
#define _STR(s)             #s					    //# 运算符能将宏的参数转换为带双引号的字符串，
#define STR(s)              _STR(s)
/****************************************************/
/*                                                  */
/* Function declarations                            */
/*                                                  */
/****************************************************/
#ifndef FIRMWARE_TIME
extern unsigned char FIRMWARE_TIME[];
#endif
/********************************************
 * Function: Version_UpdateSoftVersion
 * Describe: 更新当前软件版本号
**********************************************/
void Version_UpdateSoftVersion(void);
/********************************************
 * Function: Version_GetFirmwareInformation
 * Describe: 获取当前固件信息(版本号+编译日期)
 * 返回    : 长度
**********************************************/
unsigned char Version_GetFirmwareInformation(unsigned char *buffer);
/********************************************
 * Function: Version_GetFirmwareInformation
 * Describe: 获取当前硬件信息(版本号)
 * 返回    : 长度
**********************************************/
unsigned char Version_GetHardwareInformation(unsigned char *buffer);
/********************************************
 * Function: Version_GetProductSeriesName
 * Describe: 获取当前产品系列名称
 * 返回    : 长度
**********************************************/
unsigned char Version_GetProductSeriesName(char *buffer);
/********************************************
 * Function: Version_GetEquipmentCompany
 * Describe: 获取当前设备公司简称
 * 返回    : 长度
**********************************************/
unsigned char Version_GetEquipmentCompany(char *buffer);
/********************************************
 * Function: Version_GetEquipmentSoftVer
 * Describe: 获取当前设备版本号
 * 返回    : 长度
**********************************************/
unsigned char Version_GetEquipmentSoftVer(char *buffer);
#endif//__VERSION_H_
/*******************************************************************************
 *                             end of module
 *******************************************************************************/
 
