/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"
/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "scanner.h"


/*	���������timer�ж��е��ã���timer�жϵ����ȼ����
	���������ݷ���һ��ring buf�У�����������ɨ���buf��
	��ȡ�����ַ����ݡ�
*/
#define BARCODE_BUF_SIZE		(128)//(256)//(1024)//
#define BARCODE_REMAIN_SIZE		(32)
typedef struct barcode_buf_st
{
	U8	buf[BARCODE_BUF_SIZE];
	U16	readPos;
	U16	writePos;
	U16	valideNum;
}BARCODE_BUF_st;

BARCODE_BUF_st gtbarCodeBuf;


/*-----------------------------------------------------------------
* ����: barBuf_writeBarCode
* ����: ��barBuf��д������
* ����: pData----����buf
*		len-----��Ҫд�����ݵĳ���
* ����: ʵ��д���len
*-----------------------------------------------------------------*/
int barBuf_writeBarCode(U8 *pData, int len)
{
	U16 readPos, writePos, valideNum;
	U8 *pbuf;
	
	if (len <= 0) return 0;
	
	//readPos		= gtbarCodeBuf.readPos;
	writePos	= gtbarCodeBuf.writePos;
	valideNum	= gtbarCodeBuf.valideNum;
	pbuf		= &gtbarCodeBuf.buf[0];
	
	if ((BARCODE_BUF_SIZE-valideNum) < BARCODE_REMAIN_SIZE)
	{
		/* �ռ䲻�㣬����0���� */
		return 0;
	}
	
	/* �ռ��㹻��copy���� */
	{
		int tempLen;
		tempLen = BARCODE_BUF_SIZE - writePos;
		/* ʣ��ռ䲻�������� */
		if (tempLen >= len)
		{
			/* buf writePos��˵Ŀռ��㹻 */
			memcpy(&pbuf[writePos], pData, len);
		}
		else
		{
			/* buf writePos��˵Ŀռ䲻�㣬��Ҫcopy���� */
			memcpy(&pbuf[writePos], pData, tempLen);
			memcpy(&pbuf[0], &pData[tempLen], len - tempLen);
		}
	}
	writePos += len;
	if (writePos >= BARCODE_BUF_SIZE)
	{
		writePos -= BARCODE_BUF_SIZE;
	}
	gtbarCodeBuf.writePos	= writePos;
	//gtbarCodeBuf.valideNum	= valideNum + len;
	gtbarCodeBuf.valideNum	+= len;
	return len;
}

/*-----------------------------------------------------------------
* ����: barBuf_readBarCode
* ����: ��barBuf�ж�ȡ�������ݣ���BARCODE_END_CHARΪһ�����ݵ��б�����
* ����: pData----����buf
*		len-----��Ҫд�����ݵĳ���
* ����: 0----��ȡ�ɹ�
		-1---��ȡʧ��
*-----------------------------------------------------------------*/
int barBuf_readBarCode(U8 *pData, int *pLen)
{
	U16 readPos, tempReadPos, writePos, valideNum, len;
	U8	*pbuf, *pbufLimit;
	U8	flag;
	int ret = -1;
	
	if (pData==NULL || pLen==NULL)
	{
		return -1;
	}
	
	valideNum	= gtbarCodeBuf.valideNum;
	if (valideNum == 0)
	{
		*pLen = 0;
		return 0;
	}
	
	readPos		= gtbarCodeBuf.readPos;
	tempReadPos	= readPos;
	//writePos	= gtbarCodeBuf.writePos;
	pbuf		= &gtbarCodeBuf.buf[readPos];
	pbufLimit	= &gtbarCodeBuf.buf[BARCODE_BUF_SIZE];
	
	/* ����һ���������� */
	len = 0;
	flag = 0;
	while(valideNum--)
	{
		if (*pbuf == BARCODE_END_CHAR)
		{
		#if 0
			if (len == 0)
			{
				/* buf��һ���ַ���������������������� */
				tempReadPos++;
				pbuf++;
				continue;
			}
			else
		#endif
			{
				flag = 1;
				break;
			}
		}
		pbuf++;
		if (pbuf >= pbufLimit)
		{
			/* buf��ͷ���� */
			pbuf = &gtbarCodeBuf.buf[0];
		}
		
		len++;
	}
	if (flag == 1)
	{
		/* �ҵ�һ���������� */
		int tempLen;
		
		tempLen = BARCODE_BUF_SIZE - tempReadPos;
		if (tempLen >= len)
		{
			/* ����������һ�������� */
			memcpy(&pData[0], &gtbarCodeBuf.buf[tempReadPos], len);
		}
		else
		{
			/* ������������������ */
			memcpy(&pData[0], &gtbarCodeBuf.buf[tempReadPos], tempLen);
			memcpy(&pData[tempLen], &gtbarCodeBuf.buf[0], len-tempLen);
		}
		tempReadPos += (len + 1);	/* ������������� */
		if (tempReadPos >= BARCODE_BUF_SIZE)
		{
			tempReadPos -= BARCODE_BUF_SIZE;
		}
		
		gtbarCodeBuf.readPos	= tempReadPos;
		//gtbarCodeBuf.valideNum	= valideNum;
		gtbarCodeBuf.valideNum	-= (len + 1);
		*pLen = len;
		return 0;
	}
	else
	{
		/* û���ҵ�һ���������� */
		*pLen = 0;
		return -1;
	}
	return ret;
}

/*-----------------------------------------------------------------
* ����: barBuf_readData
* ����: ��barBuf�ж�ȡ����
* ����: pData----����buf
*		readLen--��Ҫ��ȡ�����ݳ���
* ����: ʵ�ʶ�ȡ��len
*-----------------------------------------------------------------*/
int barBuf_readData(U8 *pData, U16 readLen)
{
	U16 readPos, writePos, valideNum, left, actRd;
	U8 *pBuf;
	
	if (pData==NULL || readLen==0)
	{
		return -1;
	}
	
	valideNum	= gtbarCodeBuf.valideNum;
	if (valideNum == 0)
	{
		return 0;
	}
	
	readPos		= gtbarCodeBuf.readPos;
	writePos	= gtbarCodeBuf.writePos;
	pBuf		= &gtbarCodeBuf.buf[0];
	actRd		= 0;
	
	if(readPos > writePos)
	{
		left = BARCODE_BUF_SIZE - readPos;
		if(left >= readLen)
		{
			actRd = readLen;
			memcpy(&pData[0], &pBuf[readPos], actRd);
		}
		else
		{
			if((left+writePos) >= readLen)
			{
				actRd = readLen;
			}
			else
			{
				actRd = left + writePos;
			}
			memcpy(&pData[0], &pBuf[readPos], left);
			memcpy(&pData[left], &pBuf[0], actRd-left);
		}
		readPos += actRd;
		if(readPos >= BARCODE_BUF_SIZE) readPos -= BARCODE_BUF_SIZE;
		gtbarCodeBuf.readPos = readPos;
	}
	else if(readPos < writePos)
	{
		left = writePos - readPos;
		actRd = (left >= readLen)?readLen:left;
		memcpy(&pData[0], &pBuf[readPos], actRd);
		readPos += actRd;
		if(readPos >= BARCODE_BUF_SIZE) readPos -= BARCODE_BUF_SIZE;
		gtbarCodeBuf.readPos = readPos;
	}
	
	return actRd;
}


/*-----------------------------------------------------------------
* ����: barBuf_init
* ����: ��ʼ��barBuf
* ����: none
* ����: none
*-----------------------------------------------------------------*/
void barBuf_init(void)
{
	memset(&gtbarCodeBuf, 0x00, sizeof(BARCODE_BUF_st));
}