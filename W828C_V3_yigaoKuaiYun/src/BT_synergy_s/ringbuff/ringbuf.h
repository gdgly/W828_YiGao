#ifndef _RING_BUF_H_
#define _RING_BUF_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "hyTypes.h"

typedef struct _ringbuf_t
{
	U16	rd;
	U16	wr;
	U16	size;
	U16	havelen;
	U8	*pBuf;
	
}tRING_BUF;

/*-----------------------------------------------------------------
* ����:	RingBuf_init
* ����:	��ʼ��ring buffer�Ľṹ��ͬʱָ��buffer
* ����:	pRingBuf-----����ṹ
*		pBuf---------buffer
*		buflen-------buffer����
* ����:	none
*-----------------------------------------------------------------*/
void RingBuf_init(tRING_BUF *pRingBuf, U8 *pBuf, U16 buflen);
/*-----------------------------------------------------------------
* ����:RingBuf_Write
* ����:��ָ��ringbufд��ָ�����ȵ�����
* ����:
* ����:ʵ��д��ĳ���
*-----------------------------------------------------------------*/
U16 RingBuf_Write(tRING_BUF *pRingBuf, U8 *pWrite, U16 writeLen);
/*-----------------------------------------------------------------
* ����:RingBuf_Read
* ����:��ָ��ringbuf��ȡָ�����ȵ�����
* ����:
* ����:ʵ�ʶ�ȡ�ĳ���
*-----------------------------------------------------------------*/
U16 RingBuf_Read(tRING_BUF *pRingBuf, U8 *pRead, U16 readLen);
/*-----------------------------------------------------------------
* ����:RingBuf_PreRead
* ����:��ָ��ringbuf��ȡָ�����ȵ����ݣ�ֻ��ȡ�����ı�ring��ָ��
* ����:
* ����:ʵ�ʶ�ȡ�ĳ���
*-----------------------------------------------------------------*/
U16 RingBuf_PreRead(tRING_BUF *pRingBuf, U8 *pRead, U16 readLen);
/*-----------------------------------------------------------------
* ����:RingBuf_ConfirmRead
* ����:��ָ��ringbuf��ȡָ�����ȵ����ݣ�ֻ�ı�ָ�룬����ȡ���ݣ�
*		Ҫ��RingBuf_PreRead�������ʹ�ã�����ᵼ�¶�ȡ�������ݴ���
* ����:
* ����:ʵ�ʶ�ȡ�ĳ���
*-----------------------------------------------------------------*/
U16 RingBuf_ConfirmRead(tRING_BUF *pRingBuf, U16 readLen);
/*-----------------------------------------------------------------
* ����:RingBuf_getValidNum
* ����:��ָ��ringbuf�����Ч����
* ����:
* ����:��Ч���ݸ���
*-----------------------------------------------------------------*/
U16 RingBuf_getValidNum(tRING_BUF *pRingBuf);



#ifdef __cplusplus
}
#endif

#endif //_RING_BUF_H_