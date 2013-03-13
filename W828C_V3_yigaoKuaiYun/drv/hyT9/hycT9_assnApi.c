#include "hycT9.h"
#include "AbstractFileSystem.h"




#define T9_COMPRESS_CH_FILE_NAME		("C:/ϵͳ/EI/dictionary.bin")

#define T9_COMPRESS_CH_MAX_OUT_NUM		(30)
#define T9_COMPRESS_CH_READ_SIZE		(T9_COMPRESS_CH_MAX_OUT_NUM*2)//35������
/*---------------------------------------------------------------------------
* ����: t9_getNextWord
* ����: ���������GBK���֣��ҵ���ú�������ɴ���ĺ���
* 		��󷵻�T9_COMPRESS_CH_MAX_OUT_NUM�����֣�T9_COMPRESS_CH_READ_SIZE�ֽ�
* ����: InValue-------GBK����code
*		pOutValue-----��ȡ������غ����ַ�
* ���أ�<=0------ʧ��
*		>0------ʵ�ʶ�ȡ���ĺ����ַ���*2
* -----------------------------------------------------------------------------*/
//��Ҫ�޸ġ�090506����һ�ζ�ȡ���������Ժ��ȡ��������
static int t9_getNextWord(char *InStr, char *pOutStr, int times)
{
	char *pOut = pOutStr;
	int fileHandle, fileSize, readSize, ChNum;
	int offset, w_offset;
	int high, low, i, len, ret;
	int willReadSize;
	__align(4) char pbuffer[T9_COMPRESS_CH_READ_SIZE+11];

	fileHandle = AbstractFileSystem_Open(T9_COMPRESS_CH_FILE_NAME, AFS_READ);
	if (fileHandle < 0) return -1;

	low = (InStr[1] & 0xFF) - 0x40;
	high = (InStr[0] & 0xFF) - 0x81;
	//hyUsbPrintf("low = %d  high = %d\r\n",low,high);
	if ((low<0x00) || (low>0xBF) || (high<0x00) || (high>0x7E))
	{
		AbstractFileSystem_Close(fileHandle);
		return -1;
	}
	offset = (low + high * (0xFF - 0x40)) * 6;

	AbstractFileSystem_Seek(fileHandle, offset, AFS_SEEK_OFFSET_FROM_START);
	readSize = AbstractFileSystem_Read(fileHandle, pbuffer, 6);
	w_offset = *(int *)pbuffer;
	ChNum = *(short *)(pbuffer+4);
	
	if (times == 1)
	{
	    ret = ChNum;
    }
    else
    {
        ret = ChNum - (T9_COMPRESS_CH_MAX_OUT_NUM+5)-(times - 2) * T9_COMPRESS_CH_MAX_OUT_NUM;

    }
    
	if (ret > 0)
	{
		readSize = MIN(ret * 2, T9_COMPRESS_CH_READ_SIZE);
		
		if (times != 1)
		{
		     w_offset = w_offset + (T9_COMPRESS_CH_READ_SIZE + 10) + (times - 2) * T9_COMPRESS_CH_READ_SIZE;	
		} 		
		
		AbstractFileSystem_Seek(fileHandle, w_offset, AFS_SEEK_OFFSET_FROM_START);
		readSize = AbstractFileSystem_Read(fileHandle, pOutStr, readSize);
		pOut += readSize;
		ret = readSize;
		
		if (times == 1)
		{
		    readSize = T9_COMPRESS_CH_READ_SIZE+10 - readSize; 
		}
		else
		{
		    readSize = T9_COMPRESS_CH_READ_SIZE - readSize; 
		}		
		
		w_offset = 0;
	}
	else
	{
		if (times == 1)
		{
		    readSize = T9_COMPRESS_CH_READ_SIZE+10; 
		}
		else
		{
		    readSize = T9_COMPRESS_CH_READ_SIZE;
		}
		  		
		w_offset = -ret * 2;
		ret = 0;
	}
	
	if (readSize > 0)
	{
		AbstractFileSystem_Seek(fileHandle, 24066 * 6, AFS_SEEK_OFFSET_FROM_START);
		AbstractFileSystem_Read(fileHandle, pbuffer, 4);
		w_offset += *(int *)pbuffer;
		AbstractFileSystem_Seek(fileHandle, w_offset, AFS_SEEK_OFFSET_FROM_START);
		memset(pOut, 0, readSize + 1);
		len = AbstractFileSystem_Read(fileHandle, pOut, readSize);
		ret += len;
	}
	
	AbstractFileSystem_Close(fileHandle);

	return ret;
}


/*---------------------------------------------------------------------------
* ����: t9_getAssociateWord
* ����: ��ȡ�����ֿ�
* ����: pAssn-------��ȡ��������1��ʼ
* ���أ����� 0  ���� -1
* -----------------------------------------------------------------------------*/
int t9_getAssociateWord(tT9ASSN_CTR *pAssn, char *lastWord)
{
    int rv = 0;
    int len;
	
	pAssn->assntimes++;
	//hyUsbPrintf("times = %d  str = %s\r\n",pAssn->assntimes,lastWord);
    len = t9_getNextWord(lastWord, pAssn->assnbuf,pAssn->assntimes);
    //hyUsbPrintf("len = %d\r\n",len);
    if (len > 0)
    {
         pAssn->assntotal = len/2;
         pAssn->assnselect = 0;
         //hyUsbPrintf("assn = %s\r\n",pAssn->assnbuf);
    }
    else
    {
         pAssn->assntotal = 0;
         rv = -1;
    }
    
    return rv;
    
}
