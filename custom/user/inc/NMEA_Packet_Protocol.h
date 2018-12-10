#ifndef __NMEA_PACKET_PROTOCOL__H
#define __NMEA_PACKET_PROTOCOL__H

#include "string.h"
#include "ql_type.h"
#include "ql_uart.h"
#include "ql_fs.h"
#include "ql_error.h"
#include "ql_system.h"
#include "ql_stdlib.h"
#include "custom_user_cfg.h"


//Notice : You can also check the documentation of "Quectel_MC20_GNSS_Protocol_Specification_V1.0.pdf"

#if	configUSE_MTK_NMEA	

//Packet Type: 010 PMTK_SYS_MSG
//This message is used to automatically output system messages through GNSS module
bool MTK_SendCmd_Packet_Type_010(const char *message);
//Packet Type: 011 PMTK_TXT_MSG
bool MTK_SendCmd_Packet_Type_011(void);
//Packet Type: 001 PMTK_ACK
//Acknowledgement of PMTK Command. In order to inform the sender whether the receiver has received
//the packet, and an acknowledge packet PMTK_ACK should be returned after the receiver receives a packet.
bool MTK_SendCmd_Packet_Type_001(const char *strCmd,const char *strFlag);
//Packet Type: 101 PMTK_CMD_HOT_START
//This message is used to achieve the hot start of the GNSS module (use all available data in the NV store).
//Normally hot start means the GNSS module was powered down for less than 3 hours (RTC must be alive)
//and its ephemeris is still valid. As there is no need for downloading ephemeris, it is the fastest startup
//method
bool MTK_SendCmd_Packet_Type_101(void);
//Packet Type: 102 PMTK_CMD_WARM_START
//This message is used to achieve the warm start of the GNSS module. Warm start means the GNSS
//module has approximate information on time, position and coarse data of satellite positions. But it needs
//to download ephemeris until it can get a fix. Using this message will force the GNSS warm restart without
//using the ephemeris data in NV
bool MTK_SendCmd_Packet_Type_102(void);
//Packet Type: 103 PMTK_CMD_COLD_START
//This message is used to achieve cold start of the GNSS module. Using this message will force the GNSS
//cold restart without using any prior location information, which includes time, position, almanacs and
//ephemeris data
bool MTK_SendCmd_Packet_Type_103(void);
//Packet Type: 104 PMTK_CMD_FULL_COLD_START
//This message is essentially a cold restart, but additionally clear system and user configuration at re-start.
//That is, reset the GNSS module to the factory status. Full cold start means the GNSS module has no
//information on last location. It needs to search the full time and frequency space, and also all possible
//satellite numbers before it can get a fix
bool MTK_SendCmd_Packet_Type_104(void);
//Packet Type: 161 PMTK_CMD_STANDBY_MODE
//This message is used to enter into standby mode for power saving
bool MTK_SendCmd_Packet_Type_161(const char *message);
//Packet Type: 183 PMTK_LOCUS_QUERY_STATUS
//This message is used to query logging status and is not supported in all-in-one solution
bool MTK_SendCmd_Packet_Type_183(void);
//Packet Type: 184 PMTK_LOCUS_ERASE_FLASH
//This message is used to erase logger flash and is not supported in all-in-one solution
bool MTK_SendCmd_Packet_Type_184(const char *message);
bool MTK_SendCmd_Packet_Type_185(const char *message);
bool SDK_PQEPE_SendCmdToWrite(const char *mode,const char *save);
bool SDK_PQGLP_SendCmdToRead(void);
bool SDK_PQGLP_SendCmdToWrite(const char *mode,const char *save);


#endif

#endif

