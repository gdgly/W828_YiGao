#include "Rec_Queue.h"
#include "Rec_Info.h"

/*===============================================================*/

tDATA_QUEUE	gtData_Que;	//���ݶ���

tMULTI_CODE	*gptMultiCode;

static U32 gu32QueueLock;

U8	gau8MainCode[32];


/*----------------------------------------------------------------*/

/*----------------------------------------------------------
* ����: Queue_Init
* ����: ���г�ʼ������
* ����: pQueBuf:�ⲿ�ṩbuf   
        bufsize:pQueBuf�Ĵ�С
        pMultiBuf:�ṩ��һƱ�����������ʹ��   
        multiBufSize:pMultiBuf ��С   >=250K
* ����: none 
------------------------------------------------------------*/
void Queue_Init(U8 *pQueBuf, int bufsize, U8 *pMultiBuf, int multiBufSize)
{
	int nodesize;
	
	nodesize = sizeof(tQUE_NODE);
	
	gtData_Que.rd 		= 0;
	gtData_Que.wr 		= 0;
	gtData_Que.delCnt	= 0;
	gtData_Que.size		= bufsize/nodesize;
	gtData_Que.pQueBuf	= (tQUE_NODE *)pQueBuf;
	
	gu32QueueLock = 0;
	
	gptMultiCode = pMultiBuf;
	gptMultiCode->ifOver   = 0;
	gptMultiCode->sendType = 0;
	gptMultiCode->total    = 0;
	gptMultiCode->maxNum   = (multiBufSize-600)/BARCODE_LEN;
	
	memset(gau8MainCode, 0, 32);
}

/*----------------------------------------------------------
* ����: Queue_GetMainCode
* ����: ��ȡһƱ�����������
* ����: 
* ����: 
------------------------------------------------------------*/
U8 *Queue_GetMainCode()
{
	return gau8MainCode;
}

/*----------------------------------------------------------
* ����: Queue_AddNode_Multi
* ����: ���һ���ӵ�
* ����: sendType:���ͷ�ʽ   0:��̨����    1:ǰ̨����
* ����: 0:�ɹ�   -1:ʧ�� 
------------------------------------------------------------*/
int Queue_AddNode_Multi(U8	*pMainCode, U8 *pSubCode, tDATA_INFO *pData, U8 sendType)
{
	
	//�����Ѿ����������Ŀ
	if(gptMultiCode->total >= MAX_SUBCODE_NUM)
	{
		return -1;
	}
	
	//֮ǰ�Ѿ��д洢,���ε������ź�֮ǰ��һ��,�򲻴洢����
	if(gu32MultiInNand >= 1)
	{
		//�����������,˵�����ڲ�ֹһ��һƱ���
		if(gptMultiCode->total > 0 && strcmp(gptMultiCode->mainCode, pMainCode) != 0)
		{
			gu32MultiInNand++;//����ֱ��ʹƱ����1,�������gu32MultiInNand>1  �Ͳ�����������,����gu32MultiInNand������ʵ�ʴ��ڵ�Ʊ��,
		  	                  //ֻ�Ƿ���֪�����滹����Ҫ���ص�Ʊ��
		}
		
		return 0;
	}
	
	//����ǵ�һ�����,����Ҫ�������ź���ϸ��Ϣ��ӽ���
	if(gptMultiCode->total == 0)
	{
		//memcpy(&(gptMultiCode->tData), pData, sizeof(tDATA_INFO));//2012-10-30 kong  ���һƱʱ������Щ��Ϣ�ȽϺ���
		//strcpy(gptMultiCode->mainCode, pMainCode);
		//gptMultiCode->sendType = sendType;//���ͷ�ʽ
	}
	
	
	if(strcmp(pMainCode, pSubCode) != 0)
	{
		strcpy(gptMultiCode->subCode[gptMultiCode->total], pSubCode);
		gptMultiCode->total++;
	}
	else
	{
		//������������ź��ӵ���һ��,��˵�����Ǳ�Ʊ�����һ��������Ϣ,������ӽ���
		memcpy(&(gptMultiCode->tData), pData, sizeof(tDATA_INFO));
		strcpy(gau8MainCode, pMainCode);
		strcpy(gptMultiCode->mainCode, pMainCode);
		gptMultiCode->sendType = sendType;//���ͷ�ʽ
		gptMultiCode->ifOver = 1;
		gu32MultiInNand++;//����һƱ
	}
	
	hyUsbPrintf("add multi total = %d \r\n", gptMultiCode->total);
	
	return 0;
}

/*----------------------------------------------------------
* ����: Queue_ClearMulti
* ����: �������
* ����: none
* ����: none 
------------------------------------------------------------*/
void Queue_ClearMulti()
{
	gptMultiCode->ifOver= 0;
	gptMultiCode->sendType = 0;
	gptMultiCode->total = 0;
	memset(gau8MainCode, 0, 32);
}


/*----------------------------------------------------------
* ����: Queue_GetMultiCodeCnt
* ����: ��ȡ���ݸ���
* ����: none
* ����: ���� 
------------------------------------------------------------*/
U16 Queue_GetMultiCodeCnt()
{
	return gptMultiCode->total;
}

/*--------------------------------------
*������Rec_GetMultiSendType
*���ܣ���ȡ����һƱ����ķ��ͷ�ʽ
*������none
*���أ�
*--------------------------------------*/
U8 Rec_GetMultiSendType()
{
	return gptMultiCode->sendType;
}


/*----------------------------------------------------------
* ����: Queue_GetMultiBuf
* ����: ��ȡָ��
* ����: none
* ����:  
------------------------------------------------------------*/
tMULTI_CODE *Queue_GetMultiBuf()
{
	return gptMultiCode;
}

/*----------------------------------------------------------
* ����: Queue_Clear
* ����: �������
* ����: none
* ����: none 
------------------------------------------------------------*/
void Queue_Clear()
{
	gtData_Que.rd 		= 0;
	gtData_Que.wr 		= 0;
	gtData_Que.delCnt	= 0;
}

/*----------------------------------------------------------
* ����: Queue_DelNodes
* ����: ɾ�����  ��rd����ʼ,  ��֧�����λ��ɾ��
* ����: changeFlag:�Ƿ�ͬʱ�ı��¼��nand�ϵ�״̬ 0:��  1:��
        cnt:����
* ����: 0:�ɹ�   -1:ʧ�� 
------------------------------------------------------------*/
int Queue_DelNodes(U8 changeFlag, int cnt)
{
	int		ret;
	U32		i,fileid = 0xFFFFFFFF, *recIdx;
	U16		opType,ifimage=0;
	
	wlock(&gu32QueueLock);
	if(gtData_Que.rd == gtData_Que.wr)//���п�
	{
	hyUsbPrintf("���п�  \r\n");
		unlock(&gu32QueueLock);
		return -1;
	}
//hyUsbPrintf("1111 del rd = %d  wr = %d \r\n", gtData_Que.rd, gtData_Que.wr);	
	recIdx = (U32 *)hsaSdram_RecSendBuf();
	
	for(i = 0; i < cnt;)
	{
		if(2 == gtData_Que.pQueBuf[gtData_Que.rd].delFlag)//�Ѿ�ɾ���ļ�¼
		{
			if(gtData_Que.delCnt > 0) gtData_Que.delCnt--;
			gtData_Que.rd++;
			if(gtData_Que.rd >= gtData_Que.size)
			{
				gtData_Que.rd = 0;
			}
			continue;
		}
		
		//��û�ж�λ�ļ��Ͳ�������
		if(fileid == 0xffffffff)
		{
			fileid = gtData_Que.pQueBuf[gtData_Que.rd].fileId;
			opType = gtData_Que.pQueBuf[gtData_Que.rd].opType;
			ifimage= gtData_Que.pQueBuf[gtData_Que.rd].tData.tComm.ifimage;
		}
		//��¼��ǰ��¼����
		recIdx[i] = gtData_Que.pQueBuf[gtData_Que.rd].recIdx;
		
		gtData_Que.rd++ ;
		if(gtData_Que.rd >= gtData_Que.size)
		{
			gtData_Que.rd = 0;
		}
		i++;
	}
	
//hyUsbPrintf("del rd = %d  wr = %d delflag = %d \r\n", gtData_Que.rd, gtData_Que.wr,gtData_Que.pQueBuf[gtData_Que.rd].delFlag);	
	//�Ƿ���Ҫͬʱ�޸ļ�¼��nand�ϵ�״̬
	if(changeFlag == 1)
	{
		//�ı��¼��״̬
		ret = RecStore_repedRecordMulti(fileid,recIdx,cnt);
	//hyUsbPrintf("ret %d gu32RecUnSendTotal = %d \r\n",ret,gu32RecUnSendTotal);	
		//����ʵ�ʸı�ĸ�����������Ϣ
		
		if(ifimage == 1)//ͼƬ
		{
			gu32RecUnSendImage = (gu32RecUnSendImage >= ret)? (gu32RecUnSendImage-ret):0;
		}
		else
		{
			gu32RecUnSendTotal = (gu32RecUnSendTotal >= ret)? (gu32RecUnSendTotal-ret):0;
		}
		
		/*for(i = 0; i < ret; i++)
		{
			if(gu32RecUnSendTotal <= Rec_GetExistMax())
			{
				//Rec_DelExistCnt();
			}
			
			if(gu32RecUnSendTotal > 0) gu32RecUnSendTotal--;
			//hyUsbPrintf("gu32RecUnSendTotal  = %d \r\n",gu32RecUnSendTotal);
		}*/
	}
	
	unlock(&gu32QueueLock);
	
	return 0;
}


/*----------------------------------------------------------
* ����: Queue_AddNode
* ����: �����������һ����¼
* ����: pNode:�����ӵļ�¼��Ϣ
* ����: 0:�ɹ�   -1:ʧ�� 
------------------------------------------------------------*/
int Queue_AddNode(tQUE_NODE *pNode)
{
	U32		left;
	
	if(gu32RecInNandTotal > 0)//֮ǰ�м�¼û�м��ؽ���������Ķ����������м���
	{
//hyUsbPrintf("gu32RecUnSendTotal = %d   gu32RecInNandTotal = %d  \r\n",gu32RecUnSendTotal, gu32RecInNandTotal);	
		gu32RecUnSendTotal++;//�����Ƿ���ص�������,ȫ�ּ�������һ
		gu32RecInNandTotal++;
		return -1;
	}
	
	wlock(&gu32QueueLock);
	if(gtData_Que.rd > gtData_Que.wr)
	{
		left = gtData_Que.rd - gtData_Que.wr;
	}
	else
	{
		left = gtData_Que.size + gtData_Que.rd - gtData_Que.wr;
	}
	
	if(left > 1)//����һ���ս��  ���ó��ֺͳ�ʼ״̬һ�������
	{
		memcpy(&gtData_Que.pQueBuf[gtData_Que.wr++], pNode, sizeof(tQUE_NODE));
		
		if(gtData_Que.wr >= gtData_Que.size)
		{
			gtData_Que.wr = 0;
		}
		
		if(pNode->tData.tComm.ifimage == 1)//ͼƬ
		{
		hyUsbPrintf("add image ok \r\n");
			gu32RecUnSendImage++;
		}
		else
		{
			gu32RecUnSendTotal++;
		}
		
		unlock(&gu32QueueLock);
		return 0;
	}
	else
	{
	hyUsbPrintf("add que full ===================== \r\n");
		if(pNode->tData.tComm.ifimage == 1)//ͼƬ
		{
			gu32RecUnSendImage++;
		}
		else
		{
			gu32RecUnSendTotal++;//�����Ƿ���ص�������,ȫ�ּ�������һ
			gu32RecInNandTotal++;
		}
		unlock(&gu32QueueLock);
		return -1;
	}
}

/*----------------------------------------------------------
* ����: Queue_GetNodeCnt
* ����: ��ȡ��ǰ�����м�¼������
* ����: none
* ����: �����м�¼����
------------------------------------------------------------*/
U32 Queue_GetNodeCnt()
{
	U32	left;
	
	wlock(&gu32QueueLock);
	if(gtData_Que.wr >= gtData_Que.rd)
	{
		left = gtData_Que.wr - gtData_Que.rd;
	}
	else
	{
		left = gtData_Que.size + gtData_Que.wr - gtData_Que.rd;
	}
	
	left = (left >= gtData_Que.delCnt)?(left-gtData_Que.delCnt):0;
	
	unlock(&gu32QueueLock);
	
	return left;
}

/*----------------------------------------------------------
* ����: Queue_DelNode
* ����: �Ӷ�����ɾ��һ�����
* ����: fileid:��¼�����ļ�����
        recIdx:��¼����
* ����: 0:ɾ���ɹ�  -2:���ڷ��Ͳ���ɾ��  -1: û�ҵ���node
------------------------------------------------------------*/
int Queue_DelNode(U32 fileid, U32 recIdx)
{
	U32		i,left;
	
	wlock(&gu32QueueLock);
	if(gtData_Que.wr >= gtData_Que.rd)
	{
		left = gtData_Que.wr - gtData_Que.rd;
	}
	else
	{
		left = gtData_Que.size + gtData_Que.wr - gtData_Que.rd;
	}
	
	left = (left >= gtData_Que.delCnt)?(left-gtData_Que.delCnt):0;
//hyUsbPrintf("left = %d  rd = %d  wr = %d \r\n", left, gtData_Que.rd, gtData_Que.wr);	
	if(left == 0)
	{
		unlock(&gu32QueueLock);
		return -1;
	}
	
	i = gtData_Que.rd;
	while(i != gtData_Que.wr)
	{
		if(fileid == gtData_Que.pQueBuf[i].fileId && recIdx == gtData_Que.pQueBuf[i].recIdx)
		{
			break;
		}
		i++;
		if(i >= gtData_Que.size)
		{
			i = 0;
		}
	}
	if(i == gtData_Que.wr)//û���ҵ�
	{
		unlock(&gu32QueueLock);
		return -1;
	}
	else
	{
		gtData_Que.pQueBuf[i].delFlag = 2;//���Ϊɾ��
		gtData_Que.delCnt++;
		
		if(gtData_Que.pQueBuf[i].tData.tComm.ifimage == 1)//ͼƬ
		{
			if(0 == PicStore_Delete(gtData_Que.pQueBuf[i].tData.tComm.code))
			{
				if(gu32RecUnSendImage > 0) gu32RecUnSendImage--;
			}
		}
		else
		{
			if(gu32RecUnSendTotal > 0) gu32RecUnSendTotal--;
		}
	hyUsbPrintf("gu32RecUnSendTotal  = %d \r\n",gu32RecUnSendTotal);	
		unlock(&gu32QueueLock);
		return 0;
	}
}

/*----------------------------------------------------------
* ����: Queue_GetNodesDate
* ����: ��ȡ�����н�����Ϣ
* ����: tQUE_NODE:��Ż�ȡ���Ľ����Ϣ
        cnt:ϣ����ȡ������
* ����: ʵ�ʻ�ȡ���Ľ����
------------------------------------------------------------*/
U32 Queue_GetNodesDate(tQUE_NODE *pQue, int cnt)
{
	U32 i,j,find;
	U16 opType=0xFFFF;
	U32 fileid = 0xFFFFFFFF;
	
	wlock(&gu32QueueLock);
	
	i = gtData_Que.rd;
	if(i + cnt < gtData_Que.size)
	{
		memcpy(&pQue[0], &gtData_Que.pQueBuf[i], cnt*sizeof(tQUE_NODE));
	}
	else
	{
		j = gtData_Que.size-i;
		memcpy(&pQue[0], &gtData_Que.pQueBuf[i], j*sizeof(tQUE_NODE));
		memcpy(&pQue[j], &gtData_Que.pQueBuf[0], (cnt-j)*sizeof(tQUE_NODE));
	}
	
	find = 0;
	for(i = 0; i < cnt;)
	{
		//���ж��Ƿ�ɾ��
		if(pQue[i].delFlag == 2)
		{
			if(find == 0)//֮ǰ��û���ҵ���Ч��¼
			{
				if(gtData_Que.delCnt > 0) gtData_Que.delCnt--;
				gtData_Que.rd++;
				if(gtData_Que.rd >= gtData_Que.size)
				{
					gtData_Que.rd = 0;
				}
			}
			cnt--;
			if(i != cnt)
			{
				memcpy(&pQue[i], &pQue[i+1], (cnt-i)*sizeof(tQUE_NODE));
			}
			continue;
		}
		else if(opType == 0xFFFF)
		{
			find = 1;
			fileid	= pQue[i].fileId;
			opType	= pQue[i].opType;
			if(pQue[i].tData.tComm.nopacket == 1)//�����ǰ��¼�ǲ���������,��ֱ������
			{
				break;
			}
		}
		else if(opType != pQue[i].opType || fileid != pQue[i].fileId || pQue[i].tData.tComm.nopacket == 1)//���Ͳ�һ�»���ͬһ���ļ�ʱ����ĵ���
		{
		hyUsbPrintf("opType = (%d  %d) fileid  = (%d  %d)\r\n",opType, pQue[i].opType, fileid, pQue[i].fileId);
			break;
		}
		else
		{
			find++;
		}
		i++;
	}
	
	unlock(&gu32QueueLock);
	
	return find;
}

/*----------------------------------------------------------
* ����: Queue_Add2Que
* ����: �����������һ����¼
* ����: 
* ����: 0:�ɹ�   -1:ʧ��
------------------------------------------------------------*/
int Queue_Add2Que(U32 entry, U32 idx, U16 type, tDATA_INFO *pdata)
{
	tQUE_NODE tQue;
	
	tQue.fileId = entry;
	tQue.recIdx = idx;
	tQue.delFlag= 0;
	tQue.opType = type;
	
	memcpy(&tQue.tData, pdata,sizeof(tDATA_INFO));
	
	return Queue_AddNode(&tQue);
}