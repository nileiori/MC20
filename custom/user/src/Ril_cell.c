#include "custom_feature_def.h"
#include "ril_network.h"
#include "ril.h"
#include "ril_util.h"
#include "ql_stdlib.h"
#include "ql_trace.h"
#include "ql_error.h"
#include "ql_system.h"
#include "ql_trace.h"
#include "Ril_cell.h"
#include "include.h"

ST_QENG_info Bsic_info[6]={ {0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0},
							{0,0,0,0,0,0,0}};//当前服务小区和邻近1~6个基站信息

//找出第n个c字符所在位置
u8 *findchar(u8 *buf, u16 cnt, u8 n, u8 c)
{
    u16 i;
	u8 find_cnt = 0;
	
	for(i=0;i<cnt; i++)
	{
		if(*buf == c)
		  find_cnt++;
		if(find_cnt == n)
		{
			return buf;  
		}
		
		buf++;
	}
	
	return NULL;
}
u8 get_num_len(u8 *buf, u16 cnt)
{
    u16 i=0;
    u8 num_len = 0;
	
	if(buf[i]=='-')
	  i++;
	
	for(;i<cnt; i++)
	{
	  	if ( buf[i]<='9' && buf[i]>='0')
		{
			num_len++;
		}
		else if(buf[i]<='F' && buf[i]>='A')
		{
			num_len++;
		}
		else if(buf[i]<='f' && buf[i]>='a')
		{
			num_len++;
		}
		else
			break;
	}
	
	return num_len;
}

u8 getnum(u8 *buf, u16 cnt, u8 n, u8 flg, int *num)
{
	s8 err;
	int tmp;
	
	buf = findchar(buf, cnt, n, ',');
	buf++;
	cnt =  get_num_len(buf, 10);
	if(cnt>0 && cnt <= 8)
	{
	    if(flg)
			tmp = asctohex(buf, cnt, &err);
		else
		  	tmp = asctodec(buf, cnt, &err);
		if(!err)
		{
			*num = tmp;
			return 1;
		}
	}
	
	return 0;
}

//获取服务基站信息
void SynServingCellQeng(u8 *pt,u16 cnt)
{
	int tmp;
	if(getnum(pt, cnt, 1, 0, &tmp))
	{
		Bsic_info[0].mcc = tmp;  
		if(getnum(pt, cnt, 2, 0, &tmp))
		{
			Bsic_info[0].mnc = tmp;  
			
			if(getnum(pt, cnt, 3, 1, &tmp))
			{
				Bsic_info[0].lac = tmp;  
				if(getnum(pt, cnt, 4, 1, &tmp))
				{
					Bsic_info[0].cellid= tmp;  
					
					if(getnum(pt, cnt, 7, 0, &tmp))
						Bsic_info[0].rxlev= tmp;  
					if(getnum(pt, cnt, 16, 0, &tmp))
						Bsic_info[0].ta = tmp;  
					Bsic_info[0].flg = 1;
					//APP_DEBUG("\r\n<-- QENG SynServingCellQeng :%d~%d~%x~%x~%d~%d -->\r\n",Bsic_info[0].mcc,Bsic_info[0].mnc,
		  			//		Bsic_info[0].lac,Bsic_info[0].cellid,Bsic_info[0].rxlev,Bsic_info[0].ta);
				}
			}
			
		}
	}

}
//获取附件6个基站信息
void SynNeighboringCellQeng(u8 * pt,u16 cnt)
{
	int tmp;
	u8 flg = 0;	
	u8 i;
    ST_QENG_info bsic_info;

	for(i=0;i<6;i++)
	{	
		flg = 0;
		if(getnum(pt, cnt, 3 + i*10, 0, &tmp)) //dbm
		{
			bsic_info.rxlev= tmp;  
			if(getnum(pt, cnt, 7 + i*10, 0, &tmp))
			{
				bsic_info.mcc = tmp;  
				
				if(getnum(pt, cnt, 8 + i*10, 0, &tmp))
				{
					bsic_info.mnc = tmp;  
					if(getnum(pt, cnt, 9 + i*10, 1, &tmp))
					{
						bsic_info.lac = tmp;  
						
						if(getnum(pt, cnt, 10 + i*10, 1, &tmp))
						{
							bsic_info.cellid= tmp; 
							bsic_info.flg = 1;
							Bsic_info[1+i] = bsic_info;
							flg = 1;
						}
					}
				}
				
			}
		}  
		
		if(flg==0)
		  break;
	}
/*
	for(i=0;i<7;i++)
		{
		APP_DEBUG("\r\n<-- QENG SynServingCellQeng :%d~%d~%x~%x~%d~%d -->\r\n",Bsic_info[i].mcc,Bsic_info[i].mnc,
	Bsic_info[i].lac,Bsic_info[i].cellid,Bsic_info[i].rxlev,Bsic_info[i].ta);
		}
*/

}
//将一个数从小端模式转换成大端模式的数组
u8 tobigendianarray(u32 val, u8 *array, u8 bytes)
{
	u8 i;

	for(i = bytes; i > 0 ; i--)
	{
		if(i>4)
			*array++ = 0;
		else
			*array++ = (u8)(val >> (8*(i-1)));  	
	}

	return bytes;
}

//根据部标打包基站信息
u8 PackgQENG_Info(u8 *buff)
{
	u8 *pt=buff;
	u16 i=0, j = 0, cnt,tmp;
	if(0 == Io_ReadStatusBit(STATUS_BIT_NAVIGATION))//只有在不导航的时候才上传基站
	{
		if(Bsic_info[0].flg==1)
		{
			//基站定位信息
			pt[i++] = 0xE7; //附加信息ID号
			
			cnt = i;
			pt[i++] = 0; //附加信息数据长度

			for(tmp=0;tmp<7;tmp++)
			{
				if(Bsic_info[tmp].flg == 1)
				{
					if(tmp==0)
					{
						tobigendianarray(Bsic_info[tmp].mcc, &pt[i], 2); i+=2; //国家编码
						tobigendianarray(Bsic_info[tmp].mnc, &pt[i], 2); i+=2; //运营商编码
					}
					tobigendianarray(Bsic_info[tmp].lac, &pt[i], 2); i+=2; //大区号
					tobigendianarray(Bsic_info[tmp].cellid, &pt[i], 2); i+=2; //小区号	
					pt[i++] = Bsic_info[tmp].rxlev; //接受信号等级
					
					if(tmp==0)
						pt[i++] = Bsic_info[tmp].ta; //提前到达时间
				}
				
			}
			pt[cnt] = i - cnt - 1;
			return pt[cnt]+2;
		}
	}
	return 0;
}


//基站信息查询回调函数
static s32 ATResponse_QENG_Handler(char* line, u32 len, void* userdata)
{
	//APP_DEBUG("\r\n<-- QENG LEN=%d, DATA= :%s  -->\r\n",len,line);

    ST_QENG_info *QENG_Reponse = (ST_QENG_info*)userdata;

    char *head = Ql_RIL_FindString(line, len, "+QENG:"); //continue wait
    if(head)
    {
      if(len<10)
        {
        	//Ql_sscanf(head,"%*[^ ]%d,%d,%[^\r\n]",&QENG_Reponse->rssi,&QENG_Reponse->ber);
		}
	  else if(len<80)
	  	{
	  	SynServingCellQeng(head,len);	
	  	}
	  	else
  		{
  		SynNeighboringCellQeng(head,len);	
        	//Ql_sscanf(head,"%*[^ ]%d,%d,%[^\r\n]",&QENG_Reponse->rssi,&QENG_Reponse->ber);
  		}

		return  RIL_ATRSP_CONTINUE;
    }
	

    head = Ql_RIL_FindLine(line, len, "OK"); // find <CR><LF>OK<CR><LF>, <CR>OK<CR>，<LF>OK<LF>
    if(head)
    {  
        return  RIL_ATRSP_SUCCESS;
    }

    head = Ql_RIL_FindLine(line, len, "ERROR");// find <CR><LF>ERROR<CR><LF>, <CR>ERROR<CR>，<LF>ERROR<LF>
    if(head)
    {  
        return  RIL_ATRSP_FAILED;
    } 

    head = Ql_RIL_FindString(line, len, "+CME ERROR:");//fail
    if(head)
    {
        return  RIL_ATRSP_FAILED;
    }


    return RIL_ATRSP_CONTINUE; //continue wait
}
//查询基站信息
s32  RIL_NW_GetCell(void)
{
    s32 retRes = 0;
	char strat2[]="AT+QENG?\r\n\0";
    char strAT[] = "AT+QENG=1,1\0";
    ST_QENG_info pQENG_Reponse;
    retRes = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT), ATResponse_QENG_Handler,(void*)&pQENG_Reponse,0);
	Ql_Sleep(300);
    retRes = Ql_RIL_SendATCmd(strat2,Ql_strlen(strat2), ATResponse_QENG_Handler,(void*)&pQENG_Reponse,0);
    if(RIL_AT_SUCCESS == retRes)
    {

    }
 
    return retRes;
}


