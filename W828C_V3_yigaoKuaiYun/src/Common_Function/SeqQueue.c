


/*
SeqQueue.c<ѭ������>
*/

#include "hyTypes.h"
#include "SeqQueue.h"



/*-----------------------------------------------------------------------------
* ����:	InitQueue()
* ����:	��ʼ��ѭ������
* ����:	
* ����:	none
*----------------------------------------------------------------------------*/
void InitQueue(tSeqQueue *Que, char *buffer, U32 size)
{
	//��*Q��ʼ��Ϊһ���յ�ѭ������
	Que->front=Que->rear=0;
    Que->element = buffer;
    Que->size = size;

    memset(buffer,0,size);
}

#if 1

/*-----------------------------------------------------------------------------
* ����:	EnterQueue()
* ����:	���
* ����:	
* ����:	rv	--	0��û�гɹ�	1���ɹ�
*----------------------------------------------------------------------------*/
U8 EnterQueue(tSeqQueue *Que, U8 *pData, U32 len)
{
	U8   rv=0;
	U32  rd,wr;
	U32  size,left;
	U8	 *pBuf;
	U8 	 IntValue;

	IntValue = InterruptDisable();
	rd = Que->front;
	wr = Que->rear;
	size = Que->size;
	pBuf = Que->element;
	InterruptRestore(IntValue);
	
	if(wr < rd)
	{
		left = rd-wr-1;
		if(left >= len)
		{
			memcpy(&pBuf[wr], pData, len);
			wr += len;
			rv = 1;
		}
	}
	else
	{
		left = size-wr+rd-1;
		if(left >= len)
		{
			left = size - wr;
			if(left >= len)
			{
				memcpy(&pBuf[wr], pData, len);
			}
			else
			{
				memcpy(&pBuf[wr], pData, left);
				memcpy(&pBuf[0], &pData[left], len-left);
			}
			wr += len;
			if(wr >= size) wr -= size;
			rv = 1;
		}
	}
	
	if(rv == 1)
	{
		IntValue = InterruptDisable();
		Que->rear = wr;
		InterruptRestore(IntValue);
	}
		
	return rv;
}

#else
/*-----------------------------------------------------------------------------
* ����:	EnterQueue()
* ����:	���
* ����:	
* ����:	rv	--	0��û�гɹ�	1���ɹ�
*----------------------------------------------------------------------------*/
U8 EnterQueue(tSeqQueue *Que, char x)
{
	U8   rv;
	U32  rear=Que->rear;
	U32  size=Que->size;


	//��Ԫ��x���
	//if((Que->rear+1)%(Que->size)==Que->front)  			//�����Ѿ�����
	if((rear+1==Que->front && rear+1<size)
		||(rear+1>=size && 0==Que->front))
	{
	     rv=0;
	     //hyUsbPrintf("\r\nkong--\r\n");
	}
	else
	{
		Que->element[rear]=x;
		//Que->rear=(Que->rear+1)%(Que->size);  		//�������ö�βָ��
		if(rear+1>=size)
		{
			rear=0;
		}
		else
		{
			rear=rear+1;
		}
		Que->rear=rear;
		rv=1;
	}

	return rv;
}
#endif

/*-----------------------------------------------------------------------------
* ����:	DeleteQueue()
* ����:	����
* ����:	
* ����:	rv	--	0��û�гɹ�	1���ɹ�
*----------------------------------------------------------------------------*/
U8 DeleteQueue(tSeqQueue *Que, char *x)
{
	U8	 rv;
	U32  front=Que->front;


	//ɾ�����еĶ�ͷԪ�أ���x������ֵ*/
    if(front==Que->rear)  	//����Ϊ��
	{
		rv=0;
	}
	else
	{
        *x=Que->element[front];
        //Que->front=(Que->front+1)%(Que->size);  //�������ö�ͷָ��
		if(front+1>=Que->size)
		{
			front=0;
		}
		else
		{
			front=front+1;
		}
		Que->front=front;
		rv=1;
	}
	
	return rv;
}


/*-----------------------------------------------------------------------------
* ����:	GetQueueFreeSpace()
* ����:	�õ����еĿ�Ԫ�ظ���
* ����:	
* ����:	len		--	��Ԫ�ظ���
*----------------------------------------------------------------------------*/
U32 GetQueueFreeSpace(tSeqQueue *Que)
{
	U32 len;

	if(Que->front<=Que->rear)
	{
		len=Que->size-(Que->rear-Que->front)-1;
	}
	else if(Que->front>Que->rear)
	{
		len=Que->front-Que->rear-1;
	}

	return len;
}


