#include "ringbuf.h"


/*--------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------
* ����:	RingBuf_init
* ����:	��ʼ��ring buffer�Ľṹ��ͬʱָ��buffer
* ����:	pRingBuf-----����ṹ
*		pBuf---------buffer
*		buflen-------buffer����
* ����:	none
*-----------------------------------------------------------------*/
void RingBuf_init(tRING_BUF *pRingBuf, U8 *pBuf, U16 buflen)
{
	memset((char *)pRingBuf, 0, sizeof(tRING_BUF));
	
	pRingBuf->size = buflen;
	pRingBuf->pBuf = pBuf;
	
	return ;
}

/*-----------------------------------------------------------------
* ����:RingBuf_Write
* ����:��ָ��ringbufд��ָ�����ȵ�����
* ����:
* ����:ʵ��д��ĳ���
*-----------------------------------------------------------------*/
U16 RingBuf_Write(tRING_BUF *pRingBuf, U8 *pWrite, U16 writeLen)
{
	U16		left,actWr=0;
	U16		rd,wr,size,len;
	U8		*pBuf;
	
	rd  = pRingBuf->rd;
	wr  = pRingBuf->wr;
	size= pRingBuf->size;
	len = pRingBuf->havelen;
	pBuf= pRingBuf->pBuf;
	
	if(len+writeLen >= size)//����д��ȫ��ʱ��һ��Ҳ��д
	{
		return 0;
	}
	
	left = size-len-1;
	actWr = (left >= writeLen)? writeLen:left;
	if(wr >= rd)
	{
		left = size - wr;
		if(left >= actWr)
		{
			memcpy(&pBuf[wr], pWrite, actWr);
		}
		else
		{
			memcpy(&pBuf[wr], pWrite, left);
			memcpy(&pBuf[0], &pWrite[left], actWr-left);
		}
	}
	else// if(wr < rd)
	{
		memcpy(&pBuf[wr], pWrite, actWr);
	}
	
	wr += actWr;
	if(wr >= size) wr -= size;
	pRingBuf->wr = wr;
	pRingBuf->havelen += actWr;
	
	return actWr;
}

/*-----------------------------------------------------------------
* ����:RingBuf_Read
* ����:��ָ��ringbuf��ȡָ�����ȵ�����
* ����:
* ����:ʵ�ʶ�ȡ�ĳ���
*-----------------------------------------------------------------*/
U16 RingBuf_Read(tRING_BUF *pRingBuf, U8 *pRead, U16 readLen)
{
	U16		left,actRd=0;
	U16		rd,wr,size,len;
	U8		*pBuf;
	U8 		intLev;
	
	//intLev = InterruptDisable();
	rd  = pRingBuf->rd;
	wr  = pRingBuf->wr;
	size= pRingBuf->size;
	len = pRingBuf->havelen;
	pBuf= pRingBuf->pBuf;
	//InterruptRestore(intLev);
	
	if(len == 0)
	{
		return 0;
	}
	actRd = (len >= readLen)?readLen:len;
	if(rd > wr)
	{
	  	left = size - rd;
		if(left >= actRd)
		{
			memcpy(&pRead[0], &pBuf[rd], actRd);
		}
		else
		{
			memcpy(&pRead[0], &pBuf[rd], left);
			memcpy(&pRead[left], &pBuf[0], actRd-left);
		}
		rd += actRd;
		if(rd >= size) rd -= size;
		pRingBuf->rd = rd;
		pRingBuf->havelen -= actRd;
	}
	else if(rd < wr)
	{
		memcpy(&pRead[0], &pBuf[rd], actRd);
		
		rd += actRd;
		if(rd >= size) rd -= size;
		pRingBuf->rd = rd;
		pRingBuf->havelen -= actRd;
	}
	
	return actRd;
}

/*-----------------------------------------------------------------
* ����:RingBuf_PreRead
* ����:��ָ��ringbuf��ȡָ�����ȵ����ݣ�ֻ��ȡ�����ı�ring��ָ��
* ����:
* ����:ʵ�ʶ�ȡ�ĳ���
*-----------------------------------------------------------------*/
U16 RingBuf_PreRead(tRING_BUF *pRingBuf, U8 *pRead, U16 readLen)
{
	U16		left,actRd=0;
	U16		rd,wr,size,len;
	U8		*pBuf;
	U8 		intLev;
	
	//intLev = InterruptDisable();
	rd  = pRingBuf->rd;
	wr  = pRingBuf->wr;
	size= pRingBuf->size;
	len = pRingBuf->havelen;
	pBuf= pRingBuf->pBuf;
	//InterruptRestore(intLev);
	
	if(len == 0)
	{
		return 0;
	}
	actRd = (len >= readLen)?readLen:len;
	if(rd > wr)
	{
	  	left = size - rd;
		if(left >= actRd)
		{
			memcpy(&pRead[0], &pBuf[rd], actRd);
		}
		else
		{
			memcpy(&pRead[0], &pBuf[rd], left);
			memcpy(&pRead[left], &pBuf[0], actRd-left);
		}
		rd += actRd;
		if(rd >= size) rd -= size;
		//pRingBuf->rd = rd;
		//pRingBuf->havelen -= actRd;
	}
	else if(rd < wr)
	{
		memcpy(&pRead[0], &pBuf[rd], actRd);
		
		rd += actRd;
		if(rd >= size) rd -= size;
		//pRingBuf->rd = rd;
		//pRingBuf->havelen -= actRd;
	}
	
	return actRd;
}

/*-----------------------------------------------------------------
* ����:RingBuf_ConfirmRead
* ����:��ָ��ringbuf��ȡָ�����ȵ����ݣ�ֻ�ı�ָ�룬����ȡ���ݣ�
*		Ҫ��RingBuf_PreRead�������ʹ�ã�����ᵼ�¶�ȡ�������ݴ���
* ����:
* ����:ʵ�ʶ�ȡ�ĳ���
*-----------------------------------------------------------------*/
U16 RingBuf_ConfirmRead(tRING_BUF *pRingBuf, U16 readLen)
{
	U16		actRd=0;
	U16		rd,wr,size,len;
	U8		*pBuf;
	U8 		intLev;
	
	//intLev = InterruptDisable();
	rd  = pRingBuf->rd;
	wr  = pRingBuf->wr;
	size= pRingBuf->size;
	len = pRingBuf->havelen;
	pBuf= pRingBuf->pBuf;
	//InterruptRestore(intLev);
	
#if 1
	if(readLen > len)
	{
		pRingBuf->havelen = 0;
		pRingBuf->rd = pRingBuf->wr;
	}
	else
	{
		len -= readLen;
		rd += readLen;
		if(rd >= size) rd -= size;
		pRingBuf->havelen = len;
		pRingBuf->rd = rd;
	}
#else
	//ֻ����rdֵ
	rd += readLen;
	if(rd >= size) rd -= size;
	//intLev = InterruptDisable();
	pRingBuf->rd = rd;
	//InterruptRestore(intLev);
#endif	
	return actRd;	
}

/*-----------------------------------------------------------------
* ����:RingBuf_getValidNum
* ����:��ָ��ringbuf�����Ч����
* ����:
* ����:��Ч���ݸ���
*-----------------------------------------------------------------*/
U16 RingBuf_getValidNum(tRING_BUF *pRingBuf)
{
/*	U16	left = 0;
	U16	rd,wr,size;
	U8 intLev;
	
	//intLev = InterruptDisable();
	rd  = pRingBuf->rd;
	wr  = pRingBuf->wr;
	size= pRingBuf->size;
	//InterruptRestore(intLev);
	
	if(rd > wr)
	{
		left = size - rd;
	}
	else if(rd < wr)
	{
		left = wr - rd;
	}*/
	
	return pRingBuf->havelen;
}
