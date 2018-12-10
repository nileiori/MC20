#ifndef _SLEEPCTR_H_
#define _SLEEPCTR_H_



//每次上电清除存取的休眠标志
void SleepInit(void);

//获取休眠标志
u8 ReadSleepFlag();


//写入休眠标志
void SetSleepFlag(u8 flag);


/*********************************************************************
//函数名称	:Sleep_TimeTask(void)
//功能		:休眠定时任务
//输入		:
//输出		:
//使用资源	:
//全局变量	:
//调用函数	:
//中断资源	:  
//返回		:
//备注		:
*********************************************************************/
FunctionalState Sleep_TimeTask(void);



#endif
