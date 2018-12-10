#ifndef __RIL_CELL_H__
#define __RIL_CELL_H__
#include "ql_type.h"
#include "ril_sim.h"

typedef struct 
{
 	char flg;
	u16 mcc; //国家编码
	u16 mnc; //运营商代码
	u16 lac; //基站代码
	u16 cellid; //小区号
	s8 rxlev;//以 dBm 为单位的接收信号等级
	u8 ta;//时间提前量，范围 0~63
}ST_QENG_info;



#endif
