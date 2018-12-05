#include "qtso.h"



#define QTSO_DATA_MAX 10

struct QTSO_DATA
{
	char head[4];
	u32 size;
	float data[QTSO_DATA_MAX];
	u8 uCRC;
}static QTSO_DAT={'Q','T','S','O',0};

u8 QTSO_SetDAT(u32 size,float * dat)
{
	int i=0;
	if(size == 0 || size>10)
		return 0;
	QTSO_DAT.size=size;
	for(i=0;i<size;i++)
	{
		QTSO_DAT.data[i]=dat[i];
	}
	QTSO_DAT.uCRC=0;
	for(i=8;i<8+QTSO_DAT.size*4;i++)
	{
		QTSO_DAT.uCRC^=((u8 *)(&QTSO_DAT))[i];
	}
	return 1;
}

u8 QTSO_Send(u32 size,float * dat)
{
	int i;
	if(QTSO_SetDAT(size,dat)==0)
		return 0;
	for(i=0;i<8+QTSO_DAT.size*4;i++)
	{
		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
			USART1->DR =((u8 *)(&QTSO_DAT))[i];
	}
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
			USART1->DR =QTSO_DAT.uCRC;
	return 1;
}
