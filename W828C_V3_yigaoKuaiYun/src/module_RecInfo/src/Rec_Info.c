#include "hyhwRtc.h"
#include "Rec_Info.h"

/*===============================================================*/
U32		gu32RecUnSendTotal;//δ�ϴ���¼����
U32		gu32RecUnSendImage;//δ�ϴ�ͼƬ����
U32		gu32RecInNandTotal;//δ�ϴ���¼��nand��δ�����ص�����
U32		gu32LastScanTime;//���һ��ɨ��ʱ��
U32		gu32MultiInNand;	//һƱ�����nand�ϴ洢������(��δ���ص�������)

tREC_CONFIG gtRecConfig;

tREC_EXIST	gtRecExist;
static 	U32 gu32LockExist;

/*--------------------------------------
*������Rec_InfoInit
*���ܣ���Ϣ��ʼ��
*������pStoreCfg:������Ϣ
*���أ�0:�ɹ�  ����:ʧ��
*--------------------------------------*/
int Rec_InfoInit(tSTORE_CFG *pStoreCfg)
{
	int ret = -1;
	
	gu32RecUnSendTotal = 0;
	gu32RecUnSendImage = 0;
	gu32RecInNandTotal = 0;
	gu32LastScanTime   = 0xFFFFFFFF;
	gu32MultiInNand    = 0;
	
	gu32LockExist = 0;
		
	memset((char *)&gtRecExist,0,sizeof(tREC_EXIST));
	gtRecExist.exist  = (tEXIST_CHECK *)(pStoreCfg->pExistBuf);
	gtRecExist.maxcnt = pStoreCfg->existBufSize/sizeof(tEXIST_CHECK);
	
	gtRecConfig.groupMax = pStoreCfg->groupMax;
	gtRecConfig.existMax = gtRecExist.maxcnt;
	gtRecConfig.delaySend= pStoreCfg->delaySecond*100;

	Queue_Init(pStoreCfg->pQueBuf, pStoreCfg->queBufSize, pStoreCfg->pMultiCodeBuf, pStoreCfg->multiBufSize);
	
	ret = RecStore_Init(pStoreCfg->pSmallRecBuf, pStoreCfg->smallBufSize);
	if(ret== 0)
	{
		ret= PicStore_Init(pStoreCfg->pLargeRecBuf, pStoreCfg->largeBufSize);
	}
	
	return ret;
}

void Rec_SetScanTime(U32 tim)
{
	gu32LastScanTime = tim;
}

/*--------------------------------------
*������Rec_CheckRecMaxSize
*���ܣ�����¼���ݽṹ�Ĵ�С
*������
*���أ���ǰ��С
*--------------------------------------*/
int Rec_CheckRecMaxSize()
{
	return sizeof(tDATA_INFO);
}

/*--------------------------------------
*������Rec_GetExistMax
*���ܣ���ȡ�ظ�ɨ���жϵ������
*������none
*���أ��ظ�ɨ���жϵ������
*--------------------------------------*/
U16 Rec_GetExistMax()
{
	return gtRecConfig.existMax;
}

/*--------------------------------------
*������Rec_GetExistMax
*���ܣ���ȡ������ϴ���
*������none
*���أ�������ϴ���
*--------------------------------------*/
U16 Rec_GetGroupMax()
{
	return gtRecConfig.groupMax;
}

/*--------------------------------------
*������Rec_GetDelaySend
*���ܣ���ȡ�ӳٷ��͵����ʱ��
*������none
*���أ��ӳٷ��͵����ʱ��
*--------------------------------------*/
U16 Rec_GetDelaySend()
{
	return gtRecConfig.delaySend;
}

/*--------------------------------------
*������Rec_GetUnSendCnt
*���ܣ���ȡ�ж�������¼δ�ϴ�   ����һƱ�����
*������none
*���أ�δ�ϴ��ļ�¼����
*--------------------------------------*/
U32 Rec_GetUnSendCnt()
{
	return gu32RecUnSendTotal+gu32MultiInNand;
}

/*--------------------------------------
*������Rec_GetUnSendImageCnt
*���ܣ���ȡ�ж�����ͼƬ��¼δ�ϴ�
*������none
*���أ�δ�ϴ���ͼƬ��¼����
*--------------------------------------*/
U32 Rec_GetUnSendImageCnt()
{
	return gu32RecUnSendImage;
}

/*--------------------------------------
*������Rec_GetMultiCnt
*���ܣ���ȡ�ж���ƱһƱ����ĵ���   !!!!����һ����׼ȷ����
*������none
*���أ�
*--------------------------------------*/
U32 Rec_GetMultiCnt()
{
	return gu32MultiInNand;
}

/*--------------------------------------
*������Rec_DelMultiCnt
*���ܣ�����һ��һƱ����ĵ���
*������none
*���أ�
*--------------------------------------*/
void Rec_DelMultiCnt()
{
	if(gu32MultiInNand > 0)
	{
		gu32MultiInNand--;
	}
}

/*--------------------------------------
*������Rec_GetInNandCnt
*���ܣ���ȡ�ж�������¼��nand��δ������
*������none
*���أ�δ�����صļ�¼����
*--------------------------------------*/
U32 Rec_GetInNandCnt()
{
	return gu32RecInNandTotal;
}

/*-----------------------------------------------------------
*������Rec_GetExistCnt
*���ܣ���ȡ�Ѿ���������ĸ���
*������
*���أ��Ѿ������ĸ���
*------------------------------------------------------------*/
int Rec_GetExistCnt()
{
	return gtRecExist.curcnt;
}

/*-----------------------------------------------------------
*������Rec_CheckExist
*���ܣ�����Ƿ������ͬ������
*������pCode:ɨ����
*���أ�-1 ���벻����    ==0 �����Ѵ���
*------------------------------------------------------------*/
int Rec_CheckExist(U8 *pCode)
{
	int i,t,ret = -1;
	
	if(NULL == pCode)
		return -1;
	
	/*if(ABS(rawtime(NULL) - gu32LastScanTime) > Rec_GetDelaySend())
	{
		return -1;
	}*/
	wlock(&gu32LockExist);
	
	hyUsbPrintf("have exist cnt = %d \r\n",gtRecExist.curcnt);
	for(i = 0,t = gtRecExist.rd; i < gtRecExist.curcnt; i++,t++)
	{
		if(t >= gtRecExist.maxcnt)
		{
			t -= gtRecExist.maxcnt;
		}
		hyUsbPrintf("bar = %s   cur = %s \r\n",pCode, gtRecExist.exist[t]);
		if(strcmp(gtRecExist.exist[t].code,pCode) == 0)//�Ѿ�����
		{
			ret = 0;
			//gu32LastScanTime = rawtime(NULL);
			break;
		}
	}
	
	unlock(&gu32LockExist);
	
	return ret;
}

/*-----------------------------------------------------------
*������Rec_AddExist
*���ܣ����һ������
*������pCode:ɨ����
*���أ�-1 ʧ��    ==0 �ɹ�
*------------------------------------------------------------*/
int Rec_AddExist(U8 *pCode, U8 *pOther)
{
	int ret = 0,len;
	
	if(NULL == pCode)
		return -1;
	
	wlock(&gu32LockExist);
	
	memset(&gtRecExist.exist[gtRecExist.wr], 0, sizeof(tEXIST_CHECK));
	strcpy(gtRecExist.exist[gtRecExist.wr].code, pCode);
	if(pOther != NULL)
	{
		len = strlen(pOther);
		if(len >= 32)
		{
			len = 30;
		}
		memcpy(gtRecExist.exist[gtRecExist.wr].other, pOther, len);
		gtRecExist.exist[gtRecExist.wr].other[len] = 0;
	}
	
	//hyUsbPrintf("111 AddExist rd = %d wr = %d  cnt = %d max = %d   code = %s\r\n",gtRecExist.rd,gtRecExist.wr,gtRecExist.curcnt,gtRecExist.maxcnt,gtRecExist.exist[gtRecExist.wr].code);
	
	gtRecExist.wr++;
	if(gtRecExist.curcnt >= gtRecExist.maxcnt)
	{
		gtRecExist.rd++;
	}
	else
	{
		gtRecExist.curcnt++;
	}
	
	if(gtRecExist.wr >= gtRecExist.maxcnt)
	{
		gtRecExist.wr -= gtRecExist.maxcnt;
	}
	
	if(gtRecExist.rd >= gtRecExist.maxcnt)
	{
		gtRecExist.rd -= gtRecExist.maxcnt;
	}
	//hyUsbPrintf("222 AddExist rd = %d wr = %d  cnt = %d max = %d\r\n",gtRecExist.rd,gtRecExist.wr,gtRecExist.curcnt,gtRecExist.maxcnt);
	unlock(&gu32LockExist);
	
	return ret;
}


/*-----------------------------------------------------------
*������Rec_GetExistCodeByIdx
*���ܣ�ͨ�������ҵ�����   �������   �����洢��һ�����0��
*������idx����  ��0��ʼ   
*���أ�
*------------------------------------------------------------*/
tEXIST_CHECK *Rec_GetExistCodeByIdx(int idx)
{
	int i;
//hyUsbPrintf("idx = %d  curcnt = %d \r\n", idx, gtRecExist.curcnt);	
	if(idx >= gtRecExist.curcnt)
	{
		return NULL;
	}
	
	i = gtRecExist.wr-1-idx;
	if(i < 0) i = gtRecExist.maxcnt+i;
//hyUsbPrintf("exist i = %d \r\n", i);	
	return &gtRecExist.exist[i];
}


/*-----------------------------------------------------------
*������Rec_DelExist
*���ܣ�ɾ��һ���Ѵ�������
*������pCode:ɨ����
*���أ�-1 ʧ��    ==0 �ɹ�
*------------------------------------------------------------*/
int Rec_DelExist(U8 *pCode)
{
	int i,t,ret = -1;
	
	if(NULL == pCode)
		return ret;
	
	wlock(&gu32LockExist);
	
	if(gtRecExist.curcnt > 0)
	{
		for(i = 0,t = gtRecExist.rd; i < gtRecExist.curcnt; i++,t++)
		{
			if(t >= gtRecExist.maxcnt)
			{
				t -= gtRecExist.maxcnt;
			}
			//hyUsbPrintf("DelExist bar = %s   cur = %s \r\n",pCode, gtRecExist.exist[t]);
			if(strcmp(gtRecExist.exist[t].code,pCode) == 0)//�Ѿ�����
			{
				//hyUsbPrintf("del cnt = %d   %s\r\n",t,pCode);
				ret = 0;
				break;
			}
		}
		//hyUsbPrintf("DelExist rd = %d wr = %d  cnt = %d max = %d\r\n",gtRecExist.rd,gtRecExist.wr,gtRecExist.curcnt,gtRecExist.maxcnt);
		if(ret == 0)//�ҵ�
		{
			if(gtRecExist.curcnt == 1)
			{
				gtRecExist.rd = 0;
				gtRecExist.wr = 0;
				gtRecExist.curcnt = 0;
				//hyUsbPrintf("clear all  end \r\n");
			}
			else if(gtRecExist.curcnt > 1)
			{
				if(t == gtRecExist.rd)
				{
					gtRecExist.rd++;
				}
				else if(t > gtRecExist.rd)
				{
					memmove(&gtRecExist.exist[gtRecExist.rd+1],&gtRecExist.exist[gtRecExist.rd],sizeof(tEXIST_CHECK)*(t-gtRecExist.rd));
					gtRecExist.rd++;
				}
				else//t < gtRecExist.rd
				{
					memmove(&gtRecExist.exist[1],&gtRecExist.exist[0],sizeof(tEXIST_CHECK)*t);
					if(gtRecExist.rd == gtRecExist.maxcnt-1)
					{
						memcpy(&gtRecExist.exist[0],&gtRecExist.exist[gtRecExist.rd],sizeof(tEXIST_CHECK));
					}
					else
					{
						memcpy(&gtRecExist.exist[0],&gtRecExist.exist[gtRecExist.maxcnt-1],sizeof(tEXIST_CHECK));
						memmove(&gtRecExist.exist[gtRecExist.rd+1],&gtRecExist.exist[gtRecExist.rd],sizeof(tEXIST_CHECK)*(gtRecExist.maxcnt-1-gtRecExist.rd));
					}
					gtRecExist.rd++;
				}
				
				if(gtRecExist.rd >= gtRecExist.maxcnt)
				{
					gtRecExist.rd -= gtRecExist.maxcnt;
				}
				gtRecExist.curcnt--;
			}
		}
	}
	
	unlock(&gu32LockExist);
	
	return ret;
}

/*-----------------------------------------------------------
*������Rec_DelExistCnt
*���ܣ����ټ���
*������
*���أ�none
*------------------------------------------------------------*/
void Rec_DelExistCnt()
{
	wlock(&gu32LockExist);
	
	if(gtRecExist.curcnt > 0)
	{
		gtRecExist.curcnt--;
		gtRecExist.rd++;
		if(gtRecExist.rd >= gtRecExist.maxcnt)
		{
			gtRecExist.rd -= gtRecExist.maxcnt;
		}
	}
	
	unlock(&gu32LockExist);
}

/*-----------------------------------------------------------
*������Rec_ClearExist
*���ܣ�����ظ������жϼ���
*������
*���أ�none
*------------------------------------------------------------*/
void Rec_ClearExist()
{
	wlock(&gu32LockExist);
	
	gtRecExist.rd = 0;
	gtRecExist.wr = 0;
	gtRecExist.curcnt = 0;
	
	unlock(&gu32LockExist);
}

/*--------------------------------------
*������Rec_WriteNewRecode
*���ܣ�д���¼�¼
*������pData��Ҫд�������
*���أ��ɹ���0   ʧ�ܣ�-1
*--------------------------------------*/
int Rec_WriteNewRecode(tDATA_INFO *pdata, U8 *pOther)
{
	U32         idx;
	U8			buf[8], code[32];
	int         fileIdx ;
	int         ret ,i;
	U16	        opType;
	
	code[0] = 0;
	if(strlen(pdata->tComm.subCode) == 0)//û���ӵ���,��һƱһ����
	{
		strcpy(code, pdata->tComm.code);
	}
	else
	{
		strcpy(code, pdata->tComm.subCode);
	}
	opType = pdata->tComm.opType;
	
	OpCom_GetCurDay(buf,8);
	fileIdx = RecStore_WriteRecord(buf, pdata, &idx);
	if (fileIdx != HY_ERROR)
	{
		if(pdata->tComm.ifimage == 1)
		{
			//ͼƬ����+1
			gu32RecUnSendImage++;
		}
		else
		{
			if(pdata->tComm.state == 0)//����ֱ����ӵ������ϴ���
			{
				Queue_Add2Que(fileIdx,idx,opType,pdata);
			}
			if(code[0] != 0)
			{
				Rec_AddExist(code, pOther);
			}
		}
		ret = 0;
	}
	else
	{
		if(pdata->tComm.ifimage == 1)
		{
			//ͼƬ����+1
			gu32RecUnSendImage++;
		}
		else
		{
			if(pdata->tComm.state == 0)//����ֱ����ӵ������ϴ���
			{
				Queue_Add2Que(100,0,opType,pdata);
			}
			if(code[0] != 0)
			{
				Rec_AddExist(code, pOther);
			}
		}
		ret = -1;
	}
//hyUsbPrintf("wr gu32RecInNandTotal = %d \r\n",gu32RecInNandTotal);	
	//if(gu32RecInNandTotal == 0)
		gu32LastScanTime = rawtime(NULL);

	return ret;
}
