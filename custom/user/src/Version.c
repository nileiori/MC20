/*******************************************************************************
 * File Name:           Version.c
 * Function Describe:   
 * Relate Module:		Version.h
 * Writer:              
 * Date:                
 * Rewriter:            
 * 修改时间             
 ******************************************************************************/


/*******************************************************************************

 ******************************************************************************/

#include "include.h"

//////////////////////////
#define SOFTVERSION                 02//版本号:两位:表示当前版本号
/////////////////////////////////////////
#define VER_FIRMWARE_TIME           "201807221540"//编译时间
///////////////////////////////////////////////////
///////////////////////////////////////////////
#define HARDWARE_VERSION            "Ver:1.0"//硬件版本号
//////////////////////////////////////////////////
#define NETWORK_TYPE                01//表示系列,大版本
#define NETWORK_MODE_NAME           "G"
#define PRODUCT_SERISE_NAME         "VTKG-22G"//产品系列名称:8个字节//
/////////////////////////////////
//////////以下宏不需要修改///////
//////////////////////////////////////////////
#ifdef OPEN_DEBUG_TEST_MODE        //调试版本,不用修改,防止忘了开看门狗
#define SOFTVERSIONS                STR(01)//版本号:两位:表示当前版本号
#else
#define SOFTVERSIONS                STR(SOFTVERSION)
#endif
#define NETWORK_TYPES               STR(NETWORK_TYPE)
#define FIRMWARE_VERSION            "4"NETWORK_TYPES""SOFTVERSIONS//版本号共5位数字，//固件版本号,2表示702,01表示G网通用版本,02表示C网通用版(华为模块),04表示C通用版(中兴模块)
///////////////////////////////
#define FIRMWARE_INFORMATION        PRODUCT_SERISE_NAME"_Ver:"FIRMWARE_VERSION"_"VER_FIRMWARE_TIME//固件信息(包括版本号及日期)
///////////////////////////////////
#define HARDWARE_INFORMATION        HARDWARE_VERSION"_"NETWORK_MODE_NAME
//////////////////////////////////////////////
#define EQUIPMENT_COMPANY           "EI"
#define EQUIPMENT_SOFT_VER          "0"FIRMWARE_VERSION
#define EQUIPMENT_MANUFACTURER      "MANUFACTURER:"EQUIPMENT_COMPANY
#define EQUIPMENT_PRODUCT_MODEL     "PRODUCT MODEL:"PRODUCT_SERISE_NAME//8个字节
#define EQUIPMENT_PCB_VERSION       "PCB VERSION:A "//含1个空格
#define EQUIPMENT_SOFTWARE_VERSION  "SOFTWARE VERSION:"EQUIPMENT_SOFT_VER
#ifndef FIRMWARE_TIME
unsigned char FIRMWARE_TIME[]={VER_FIRMWARE_TIME};
#endif
/********************************************
 * Function: Version_UpdateSoftVersion
 * Describe: 更新当前软件版本号
**********************************************/
void Version_UpdateSoftVersion(void)
{
    //为编译时不把MANUFACTURER常量数组优化掉（去掉了）而增加以下代码
    ////////////////////
    if(strncmp(EQUIPMENT_MANUFACTURER,EQUIPMENT_PRODUCT_MODEL,6)||strncmp(EQUIPMENT_PCB_VERSION,EQUIPMENT_SOFTWARE_VERSION,6))//一定运行
    {
        //Public_SetOperateTimer(SysCtrl_DisposeTrackResetSystem,PUBLICSECS(20));//重新后，防止RTC出错，需要等待20秒更新了系统时钟再判断
    }
}
/********************************************
 * Function: Version_GetFirmwareInformation
 * Describe: 获取当前固件信息(版本号+编译日期)
 * 返回    : 长度
**********************************************/
unsigned char Version_GetFirmwareInformation(unsigned char *buffer)
{
    strcpy((char *)buffer,FIRMWARE_INFORMATION);
    return strlen(FIRMWARE_INFORMATION);
}
/********************************************
 * Function: Version_GetFirmwareInformation
 * Describe: 获取当前硬件信息(版本号)
 * 返回    : 长度
**********************************************/
unsigned char Version_GetHardwareInformation(unsigned char *buffer)
{
    strcpy((char *)buffer,HARDWARE_VERSION);
    return strlen(HARDWARE_VERSION);
}
/********************************************
 * Function: Version_GetProductSeriesName
 * Describe: 获取当前产品系列名称
 * 返回    : 长度
**********************************************/
unsigned char Version_GetProductSeriesName(char *buffer)
{
    strcpy((char *)buffer,PRODUCT_SERISE_NAME);
    return strlen(PRODUCT_SERISE_NAME);
}
/********************************************
 * Function: Version_GetEquipmentCompany
 * Describe: 获取当前设备公司简称
 * 返回    : 长度
**********************************************/
unsigned char Version_GetEquipmentCompany(char *buffer)
{
    strcpy((char *)buffer,EQUIPMENT_COMPANY);
    return strlen(EQUIPMENT_COMPANY);
}
/********************************************
 * Function: Version_GetEquipmentSoftVer
 * Describe: 获取当前设备版本号
 * 返回    : 长度
**********************************************/
unsigned char Version_GetEquipmentSoftVer(char *buffer)
{
    strcpy((char *)buffer,EQUIPMENT_SOFT_VER);
    return strlen(EQUIPMENT_SOFT_VER);
}
/******************************************************************************
								EndModule
******************************************************************************/
