#ifndef __PUBLIC_H_
#define __PUBLIC_H_


#define PUBLICSECS(x)	(unsigned long)((x) * SYSTICK_1SECOND)  //

#define STEP_VAL    (float)(10.0)
#define STEP_FLT    (float)(0.1)
#define SIGN_FLG    (float)(-1.0)
#define PUBLIC_CHECKBIT(X,N) ((X) & (1ul << (N)))	   //测试某一位
#define PUBLIC_SETBIT(X,N) (X)=((X) | (1ul<<(N)))       //设置某一位
#define PUBLIC_CLRBIT(X,N) (X)=((X) & (~(1ul<<(N))))    //清除某一位
////////////////////////////////////
#define PUBLIC_CHECKBIT_EX(X,N) (PUBLIC_CHECKBIT(X,N)?1:0)	   //测试某一位,返回值只有0和1

/*************************************************************OK
** 函数名称: Public_GetSumVerify
** 功能描述: 对数据块累加求和，只取低字节
** 入口参数: Pdata数据块首地址,datalen长度		 
** 出口参数: 
** 返回参数: 校验码
** 全局变量: 无
** 调用模块: 无
*************************************************************/
unsigned char Public_GetSumVerify(const unsigned char *Pdata,unsigned short datalen);
/*************************************************************
** 函数名称: Public_ConvertLongToBuffer
** 功能描述: 把unsigned long转成4位unsigned char,高位在前
** 入口参数: value:需要转换的数据
** 出口参数: buf:转换结果的首地址
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/	
void Public_ConvertLongToBuffer(unsigned long value,unsigned char *buf);
/*************************************************************
** 函数名称: Public_ConvertAscToBCD
** 功能描述: 把asciii码转成BCD码,(把两个字节的asciii码转成一个字节的BCD码)
** 入口参数: destin目标数据的首地址,source源数据的首地址,len需要转换的源数据长度
** 出口参数: destin目标数据的首地址
** 返回参数: 无
** 全局变量: destin使用时注意destin的空间分配
** 调用模块: 
** 例    子: 0x31 0x32 0x33 0x34 0x35 0x36 =>0x12 34 56
*************************************************************/	
void Public_ConvertAscToBCD(unsigned char * destin,const unsigned char* source, unsigned char len);
/*************************************************************
** 函数名称: Public_ConvertBcdToAsc
** 功能描述: 将BCD码字符串每一个字节转换成它的AscII码的字符串
** 入口参数: pAsc转换之后的数据首地址,pBcd需要转换的BCD数据首地址，BcdLen:需要转换的数据长度
** 出口参数: pAsc转换之后的数据首地址
** 返回参数: 无
** 全局变量: 
** 调用模块: 注意空间分配
** 例    子: 0x12 34 56=>0x31 0x32 0x33 0x34 0x35 0x36
*************************************************************/	
void Public_ConvertBcdToAsc(unsigned char *pAsc, const unsigned char *pBcd, unsigned char BcdLen);
/*************************************************************
** 函数名称: Public_ConvertBcdToValue
** 功能描述: 将BCD码字符串转换成它的值的字符串
** 入口参数: pValue转换之后的数据首地址,pBcd需要转换的BCD数据首地址，BcdLen:需要转换的数据长度
** 出口参数: pValue转换之后的数据首地址
** 返回参数: 无
** 全局变量: 
** 调用模块: 注意空间分配
** 例    子: 0x12 34 56=>12 34 56
*************************************************************/	
void Public_ConvertBcdToValue(unsigned char *pValue, const unsigned char *pBcd, unsigned char BcdLen);
/*************************************************************
** 函数名称: Public_ConvertBufferToLong
** 功能描述: 把4位unsigned char,高位在前的数据转成unsigned long;
             (与Public_ConvertLongToBuffer功能相反)
** 入口参数: buf:需要转换的数据的首地址
** 出口参数: 
** 返回参数: 转换结果
** 全局变量: 
** 调用模块: 
*************************************************************/
unsigned long Public_ConvertBufferToLong(unsigned char *buf);
/*************************************************************
** 函数名称: Public_ConvertBufferToShort
** 功能描述: 把2位unsigned char,高位在前的数据转成unsigned short;
** 入口参数: buf:需要转换的数据的首地址
** 出口参数: 
** 返回参数: 转换结果
** 全局变量: 
** 调用模块: 
*************************************************************/
unsigned short Public_ConvertBufferToShort(unsigned char *buf);

/*************************************************************
** 函数名称: Public_ConvertShortToBuffer
** 功能描述: 把unsigned short转成2位unsigned char,高位在前
** 入口参数: value:需要转换的数据
** 出口参数: buf:转换结果的首地址
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/	
void Public_ConvertShortToBuffer(unsigned short value,unsigned char *buf);
/*************************************************************
** 函数名称: hextostr
** 功能描述: 把unsigned int转成字符串形式
** 入口参数: hex:需要转换的数，len:转换的长度，_str转换后的存放指针
** 出口参数: _str:转换结果的首地址，转换后的长度
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/	
unsigned char HexToStr(unsigned int hex,unsigned char len, void *_str);
/*************************************************************
** 函数名称: Public_CheckArrayValIsEqual
** 功能描述: 判断两个数组是否相等,
** 入口参数: array需要判断的数组1首地址,array1需要判断的数组2首地址,len需要判断的长度
** 出口参数: 无
** 返回参数: 0相等,1不相等;
** 全局变量: 
** 调用模块: 
*************************************************************/	
unsigned char Public_CheckArrayValIsEqual(const unsigned char *array,const unsigned char *array1,unsigned short len);
/*************************************************************
** 函数名称: Public_GetXorVerify
** 功能描述: 对数据块进行异或，只取低字节
** 入口参数: Pdata数据块首地址,datalen长度		 
** 出口参数: 
** 返回参数: 校验码
** 全局变量: 无
** 调用模块: 无
*************************************************************/
unsigned char Public_GetXorVerify(const unsigned char *Pdata,unsigned short datalen);
/*************************************************************
** 函数名称: Public_atof
** 功能描述: 把字符串转成数值
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
float Public_atof(const char*str); 
/*************************************************************
** 函数名称: Public_itoa
** 功能描述: 把数值转成字符串(不考虑负数)
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块: 无
*************************************************************/
/*********************************************************************
** 函 数 名：
**********************************************************************/

unsigned char dectostr(unsigned int decnum,void *_str,unsigned char _bit);

unsigned char Public_itoa(unsigned char *str,unsigned long val,unsigned char n);

#define Public_itoaEx(str,val)  Public_itoa(str,val,0)
/*************************************************************
** 函数名称: Public_BCD2HEX
** 功能描述: BCD转换为Byte        
** 入口参数: val:需要转换的数据
** 出口参数: 无
** 返回参数: 转换结果
** 全局变量: 
** 调用模块: 
*************************************************************/
unsigned char Public_BCD2HEX(unsigned char val);    //BCD转换为Byte
/*************************************************************
** 函数名称: asctodec
** 功能描述: 
** 入口参数: 
** 出口参数:
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/
int asctodec(unsigned char *asc, unsigned char  len, signed char *_err);

/*************************************************************
** 函数名称: hextostr
** 功能描述: 把unsigned int转成字符串形式
** 入口参数: hex:需要转换的数，len:转换的长度，_str转换后的存放指针
** 出口参数: _str:转换结果的首地址，转换后的长度
** 返回参数: 无
** 全局变量: 
** 调用模块: 
*************************************************************/
unsigned long asctohex(unsigned char *asc, unsigned char len, signed char *_err);

/*************************************************************
** 函数名称: Public_HEX2BCD
** 功能描述: Byte码转换为BCD码    
** 入口参数: val:需要转换的数据
** 出口参数: 无
** 返回参数: 转换结果
** 全局变量: 
** 调用模块: 
*************************************************************/
unsigned char Public_HEX2BCD(unsigned char val);    //Byte码转换为BCD码
/*************************************************************
** 函数名称: Public_Mymemcpy
** 功能描述: src的数据拷到des中,n为拷贝长度,flag为方向(1:倒序,0:顺序)
** 入口参数: 
** 出口参数: 
** 返回参数: 
** 全局变量: 无
** 调用模块:
*************************************************************/
void *Public_Mymemcpy(void *des,  const void *src, unsigned short len, unsigned char flag);

unsigned long Public_CRC_32( unsigned char * aData, unsigned long aSize );

#endif
