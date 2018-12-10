#ifndef	__WATCHDOG_H
#define	__WATCHDOG_H

//*************文件包含***************

//*************宏定义****************
//*************函数声明**************

#define	WATCHDOG_MODE_HARD	0
#define	WATCHDOG_MODE_SOFT	1

#define	WATCHDOG_MODE		WATCHDOG_MODE_HARD


/**************************************************************
* watchdog setup
***************************************************************/
/*
#define	BIT0	0x01
#define	BIT1	0x02
#define	BIT2	0x04
#define	BIT3	0x08
#define	BIT4	0x10
#define	BIT5	0x20
#define	BIT6	0x40
#define	BIT7	0x80
#define BIT_WDT	BIT0|BIT1|BIT2|BIT3|BIT4|BIT5

void SoftWdg_Feed(u8 bitSta);
*/
void WDG_Hard_Init(void);




#endif
