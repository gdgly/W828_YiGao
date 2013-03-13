#include "Rec_Store.h"
#include "Rec_Info.h"
#include "Rec_Queue.h"


/*=======================================================================*/
/*	ȫ�ֱ��� */
U32	appLockNandCtr = 0;

U8	*gpu8RecBuf;
static U8  gu8CurId[30];//��ǰ��¼Ա����ID
static U8  gu8CurRight; //��ǰ��¼Ա����Ȩ��



/*-----------------------------------------------------------
*����:	RecStore_SetId
*����:	�ѵ�ǰ��¼��id���ý���
*������ 
*����:	none
*------------------------------------------------------------*/
void RecStore_SetId(U8 *pId)
{
	memset(gu8CurId,0,30);
	strcpy(gu8CurId,pId);
}

/*-----------------------------------------------------------
*����:	RecStore_GetId
*����:	��ȡ��ǰ��¼��id
*������ 
*����:	ID
*------------------------------------------------------------*/
U8 *RecStore_GetId()
{
	return gu8CurId;
}

/*-----------------------------------------------------------
*����:	RecStore_SetRight
*����:	�ѵ�ǰ��¼��Ȩ�����ý���
*������ 
*����:	none
*------------------------------------------------------------*/
void RecStore_SetRight(U8 right)
{
	gu8CurRight = right;
}

/*-----------------------------------------------------------
*����:	RecStore_GetRight
*����:	��ȡ��ǰ��¼��Ȩ��
*������ 
*����:	right
*------------------------------------------------------------*/
U8 RecStore_GetRight()
{
	return gu8CurRight;
}

/*-----------------------------------------------------------
*����:	RecStore_Init
*����:	ģ���ʼ��.
*������.pRecBuf:�ⲿ�ṩbuf  
        bufSize:buf����  (>=140K)
*����:	�ɹ���HY_OK   ʧ�ܣ�HY_ERROR
*------------------------------------------------------------*/
int RecStore_Init(U8 *pRecBuf, U32 bufSize)
{
	int ret;
	
	if(bufSize < 140*1024)//�ⲿ�ṩ��buf����
	{
		return -1;
	}
	wlock((unsigned int)&appLockNandCtr);
	gpu8RecBuf = pRecBuf;
	ret = nandFile_Init(0);
	unlock((unsigned int)&appLockNandCtr);
	
	return ret;
}

/*-----------------------------------------------------------
*������RecStore_getFileHandle
*���ܣ�ͨ���ļ����ҵ��ļ�������file idx   
       ע��:���ܴ���ͬ���ļ�, ���һ��ļ�¼̫��,һ���ļ���Ų���,��Ҫʹ��2���ļ�,��2���ļ�����ͬ.
*������	name -- �ļ���
*		pagebuf	-- ��ʱbuf,һ��page ��С
*���أ��ɹ��������ҵ����ļ�����   ʧ�ܣ�<=0
*------------------------------------------------------------*/
int RecStore_getFileHandle(U8 *name, int *pFileIdx)
{
	return nandFile_open(name,pFileIdx);
}

/*-----------------------------------------------------------
*����:	RecStore_GetExistFileList
*����:	�õ�Ŀǰ���е��ļ������������б�.
*������ 
*		ppArr: �洢�����б�Ķ�ά����
*����:	�ļ�����.
*------------------------------------------------------------*/
int RecStore_GetFileNameList(int *pNodeArr)
{
	int i, j, k, fileNum,fileIdx;
	tFILE_ENTRY	*ptFile,*ptFile2;
	
	for(i = 0, j = 0; i<MAX_NAND_FILE; i++)
	{
		if(gtFileEntry[i].name != -1) 
		{
			pNodeArr[j] = i;
			j++;
		}
	}
	
	fileNum = j;
	//����Щ�ļ�����������  ������С������ǰ��
	for(i = 0; i < fileNum-1; i++)
	{
		k = 0;
		for(j = 0; j < fileNum-i-1; j++)
		{
			ptFile  = &gtFileEntry[pNodeArr[j]];
			ptFile2 = &gtFileEntry[pNodeArr[j+1]];
			if(ptFile->name > ptFile2->name)
			{
				fileIdx      = pNodeArr[j];
				pNodeArr[j]  = pNodeArr[j+1];
				pNodeArr[j+1]=fileIdx;
				k = 1;
			}
			else if(ptFile->name > ptFile2->name)//������ͬ���ļ��Ƚ�����
			{
				if(ptFile->dayIdx > ptFile2->dayIdx)
				{
					fileIdx      = pNodeArr[j];
					pNodeArr[j]  = pNodeArr[j+1];
					pNodeArr[j+1]=fileIdx;
					k = 1;
				}
			}
		}
		if(k == 0)
		{
			break;
		}
	}
	
	return fileNum;
}

/*-----------------------------------------------------------
*����:	RecStore_deleteFile
*����:	ɾ��ָ�����ļ�
*������ 
*����:	�ļ�����.
*------------------------------------------------------------*/
int RecStore_deleteFile(U8 *name)
{
	int ret;
	if (name == NULL)
		return HY_ERROR;
	
	wlock((unsigned int)&appLockNandCtr);
	ret = nandFile_delete(atoi(name));
	unlock((unsigned int)&appLockNandCtr);
	
	return ret;
}


/*-----------------------------------------------------------
*����:	nandFile_changeRecordStatus
*����:	��ָ���ļ���д��һ����¼.
*	  	������ļ�������,���Զ������ļ�.�������,ֱ��д��.
*������
*	    pDate_name:�ļ���. 6���ַ�
*		pNode:Ҫд������
*		recId:���ص�ID.��¼д���λ��.
*����:	�ɹ����ļ�idx   ʧ�ܣ�HY_ERROR
*------------------------------------------------------------*/
int RecStore_WriteRecord(U8 *pDate_name, tDATA_INFO *pNode, U32 *recId)
{
	tFILE_ENTRY	*pFile;
	int	fileIdx[MAX_NAND_FILE], useIdx;
	int i,cnt,ret;
	
	if(pDate_name == NULL || pNode == NULL)
	{
	hyUsbPrintf("pDate_name or pNode is NULL !\r\n");
		return HY_ERROR;
	}
	
	wlock((unsigned int)&appLockNandCtr);
	
	useIdx = -1;
	// ֱ�Ӵ�. ������ļ�������,����-1
	cnt = nandFile_open(pDate_name, fileIdx);
	if(cnt > 0)
	{
		//���ҵ�����Щ�ļ����ҳ�һ��û�д������ļ�
		for(i = 0; i < cnt; i++)
		{
			pFile = &gtFileEntry[fileIdx[i]];
			hyUsbPrintf("file id = %d  total = %d   name = %d  idx = %d \r\n", 
						fileIdx[i],pFile->totalRecords,pFile->name, pFile->dayIdx);
			if(pFile->totalRecords < MAX_RECORDS_PER_FILE)
			{
				useIdx = fileIdx[i];
				break;
			}
		}
	}
	
	if(useIdx == -1)//û���ҵ� �� ֮ǰ�Ǹ��ļ��Ѿ�����
	{
		useIdx = nandFile_create(pDate_name, cnt);
	}
	hyUsbPrintf("cur useidx = %d \r\n", useIdx);
	if(-1 != useIdx)
	{
		ret = nandFile_write(useIdx, (U8 *)pNode, sizeof(tDATA_INFO), pNode->tComm.state, recId);
		hyUsbPrintf("nandFile_write ret = %d \r\n", ret);
		if(ret == -1)
		{
			useIdx = -1;
		}
	}
	
	unlock((unsigned int)&appLockNandCtr);
	
	return useIdx;
}

/*-----------------------------------------------------------
*����:	RecStore_CmpId
*����:	�Աȼ�¼�е�ID�ź͵�ǰ��¼Ա����ID��
*������ pNode:����
*����:	�ɹ���HY_OK   ʧ�ܣ�HY_ERROR
*------------------------------------------------------------*/
int RecStore_CmpId(tDATA_INFO *pNode)
{
	int ret = HY_ERROR;
	
	if(strcmp(pNode->tComm.userId,gu8CurId) == 0)
	{
		ret = HY_OK;
	}
	
	return ret;
}

int default_cmp(tRECORD_STATUS *par, void *param)
{
	return 0;
}


/*-----------------------------------------------------------
*����:	RecStoresub_SearchNextRecord
*����:	��ָ���ļ��а��������ʹ�ָ��λ��������һ����¼.
*������
*	    fileIdx:�ļ���gtFileEntry�е�idx
*		datacmpFunc: �ص�����,�Ա������Ƿ����
*		typecmpFunc: �ص�����,�Ա�״̬�Ƿ���ͬ
*		datacmpPara: datacmpFuncҪ����ȥ�Ĳ���
*		cmpfuncPara: typecmpFuncҪ����ȥ�Ĳ���
*		recId:���ص�ID.��¼�ҵ���¼��λ��.
*����:	�ɹ���HY_OK   ʧ�ܣ�HY_ERROR
*ע��:	����һ�ε���ʱ,�����Ҫ��ͷ����,*recId��������Ϊ0.
*       ���û��״̬�ԱȺ���,��ֱ�ӽ����������; �����״̬�ԱȺ���,���ȶԱ�״̬,�ٶԱ�����.
*		ģ���Ӻ���,�Ƕ���ӿ�.
*------------------------------------------------------------*/
int RecStoresub_SearchNextRecord(int fileIdx, DATA_CMP datacmpFunc, TYPE_CMP typecmpFunc, void *datacmpPara, void *cmpfuncPara, U32 *recId)
{
	int	i, j, ret, rc;
	int pageOffset, tmpIdx;
	int absNum,blk,isEnd = 0;
	tRECORD_STATUS *pagebuf;
	tDATA_INFO node;
	tFILE_ENTRY	*ptFile;
	
	rc = HY_ERROR;
	if (fileIdx >= MAX_NAND_FILE || datacmpFunc == NULL)
		return HY_ERROR;
	if (typecmpFunc == NULL)
		typecmpFunc = default_cmp;
	
	ptFile	= &gtFileEntry[fileIdx];
	if (*recId >= ptFile->totalRecords)	//���������Ѿ��������ļ����洢����, ����.
	{
		return HY_ERROR;
	}
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	
	//ֻ��ɾ��δ�ϴ��ļ�¼
	if(ptFile->reportedRecords + ptFile->deletedRecords >= ptFile->totalRecords)
	{//û��δ�ϴ��ļ�¼
		return HY_ERROR;
	}
	
	absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
	if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
	{
		absNum++;
	}
	
	blk			= (*recId) / ABSTRACT_NUM_PER_PAGE;
	pageOffset  = (*recId) % ABSTRACT_NUM_PER_PAGE;
	
	absNum -= blk;
	
	//��ȡ��Ч��ժҪ��Ϣ
	absNum = nandFile_readFileAbstract(fileIdx, (char *)pagebuf, blk, absNum);
	
	if(absNum == HY_ERROR)//��ȡʧ��
	{
		return HY_ERROR;
	}
	
	for(i = 0; i < absNum; i++)
	{
		for(j = 0; j < ABSTRACT_NUM_PER_PAGE; j++)//ÿ��ժҪ���������
		{
			tmpIdx = i*ABSTRACT_NUM_PER_PAGE+j;
			if(tmpIdx+blk*ABSTRACT_NUM_PER_PAGE >= ptFile->totalRecords)//���������Ѿ��������ļ����洢����, ����.
			{
				isEnd = 1;
				break;
			}
			
			if (typecmpFunc(&pagebuf[tmpIdx], cmpfuncPara) != 0)
			{
				continue;
			}
			
			rc = nandFile_read(fileIdx, tmpIdx, (U8 *)&node, sizeof(tDATA_INFO));
			if (rc == HY_ERROR)
			{
				continue;
			}
			
			rc = HY_ERROR;
			if (datacmpFunc(&node, datacmpPara) == 0)	//����ƥ��
			{
				*recId = tmpIdx;
				if(gu8CurRight != 0)
				{
					rc = RecStore_CmpId(&node);
					if(rc == HY_OK)
					{
						isEnd = 1;
						break;
					}
				}
				else
				{
					isEnd = 1;
					rc = HY_OK;
					break;
				}
			}
		}
		
		if(isEnd)
		{
			break;
		}
	}
		
	return rc;
}


// ��Ը�RecStore_DeleteRecord�������޸ĺ���  ��״̬Ϊ "��ɾ"
int CB_modifyState_DeleteRecord(tRECORD_STATUS *pStatus)
{
	pStatus->recState = 2;

	return pStatus->recState;
}
/*-----------------------------------------------------------
*������RecStore_DeleteRecord
*���ܣ�ɾ�� ĳһָ����.(ָ���ļ����ļ�������)
*������fileIdx:�ļ�����   
	   recordIdx:�ļ��ڼ�¼����
*���أ�ʧ�ܣ�-1    �ɹ���0
*------------------------------------------------------------*/
int RecStore_DeleteRecord(int fileIdx, int recordIdx)
{
	int ret;
	
	wlock((unsigned int)&appLockNandCtr);
	ret = nandFile_changeRecordStatus(fileIdx, recordIdx, CB_modifyState_DeleteRecord);
	unlock((unsigned int)&appLockNandCtr);
	
	return ret;
}

// ��Ը�RecStore_DeleteRecord�������޸ĺ���  ��״̬Ϊ "�Ѵ�"
int CB_modifyState_repedRecord(tRECORD_STATUS *pStatus)
{
	int ret = 1;
	
	if(pStatus->recState == 0)
	{
		pStatus->recState = 1;
		ret = 1;
	}
	else
	{
		ret = 0;
	}
	return ret;
}


/*-----------------------------------------------------------
*������RecStore_repedRecord
*���ܣ��޸�ĳһָ����Ϊ�ϴ�״̬.(ָ���ļ����ļ�������)
*������fileIdx:�ļ�����   
	   recordIdx:�ļ��ڼ�¼����
*���أ�ʧ�ܣ�-1    �ɹ���0
*------------------------------------------------------------*/
int RecStore_repedRecord(int fileIdx, int recordIdx)
{
	int ret;

	wlock((unsigned int)&appLockNandCtr);
	ret = nandFile_changeRecordStatus(fileIdx, recordIdx, CB_modifyState_repedRecord);
	unlock((unsigned int)&appLockNandCtr);
	
	return ret;
}

/*-----------------------------------------------------------
*������RecStore_repedRecordMulti
*���ܣ��޸Ķ�����¼״̬Ϊ�ϴ�״̬.(ָ���ļ����ļ�������)
*������fileIdx:�ļ�����   
	   recordIdx:�ļ��ڼ�¼����
*���أ�ʧ�ܣ�=0    �ɹ���>0  (�ɹ��޸ļ�¼�ĸ���)
*------------------------------------------------------------*/
int RecStore_repedRecordMulti(int fileIdx, int *recordIdx, int num)
{
	int ret,i,j,total;
	int reccnt,blk;
	
	total  = 0;
	reccnt = 0;
	blk    = -1;
	
	for(i = 0, j = 0; i < num; i++)
	{
		if(blk == -1)
		{
			blk = (recordIdx[i]/ABSTRACT_NUM_PER_PAGE);
		}
		else if(blk != recordIdx[i]/ABSTRACT_NUM_PER_PAGE)
		{
			blk = (recordIdx[i]/ABSTRACT_NUM_PER_PAGE);
			
			j += reccnt;
			
			wlock((unsigned int)&appLockNandCtr);
			ret = nandFile_changeRecordStatusMulti(fileIdx, &recordIdx[j-reccnt], &reccnt, CB_modifyState_repedRecord);
			unlock((unsigned int)&appLockNandCtr);
			
			if(ret == HY_OK)
			{
				total += reccnt;
			}
			
			reccnt = 0;
		}
		
		reccnt++;
		
		if(i+1 == num)
		{
			j += reccnt;
			
			wlock((unsigned int)&appLockNandCtr);
			ret = nandFile_changeRecordStatusMulti(fileIdx, &recordIdx[j-reccnt], &reccnt, CB_modifyState_repedRecord);
			unlock((unsigned int)&appLockNandCtr);
			
			if(ret == HY_OK)
			{
				total += reccnt;
			}
			reccnt = 0;
		}
	}
	
	return total;
}


/*-----------------------------------------------------------
*������RecStore_GetAllUnreported
*���ܣ���������δ�ϴ��ļ�¼
*����:  usertype : Ȩ��
*���أ�ʧ�ܣ�-1    �ɹ���0
*------------------------------------------------------------*/
int RecStore_GetAllUnreported(U8 usertype)
{
	int i,j,k, fileNum, ret;
	int fileList[MAX_NAND_FILE];
	tRECORD_STATUS *pagebuf,recStat;
	tFILE_ENTRY	*ptFile;
	tDATA_INFO node;
	tQUE_NODE  tQue;
	int readCnt = 0,absNum,fileIdx;
	int quefull=0;
	U32 total,nand,image;
	
	wlock((unsigned int)&appLockNandCtr);
	
	total = gu32RecUnSendTotal;
	image = gu32RecUnSendImage;
	nand  = gu32RecInNandTotal;
hyUsbPrintf("��ǰδ������:%d,  ������δ������:%d  ͼƬ��: %d \r\n", total, nand,gu32RecUnSendImage);	
	gu32RecUnSendTotal = 0;
	gu32RecUnSendImage = 0;
	gu32RecInNandTotal = 0;
	gu32MultiInNand    = 0;
		
	fileNum = RecStore_GetFileNameList(fileList);
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
//hyUsbPrintf("��¼�ļ�����:%d \r\n", fileNum);
	for (i = 0; i < fileNum && quefull == 0; i++)//�ļ�ѭ�� start
	{
		fileIdx = fileList[i];
		ptFile  = &gtFileEntry[fileIdx];
		
		hyUsbPrintf("�ļ����:%d, ��¼����:%d,  ɾ����:%d,  �Ѵ���:%d\r\n",
				fileIdx, ptFile->totalRecords, ptFile->deletedRecords, ptFile->reportedRecords);
		/* kong 2011-05-24 ���жϸ��ļ����Ƿ���δ�ϴ��ļ�¼*/
		if(ptFile->reportedRecords + ptFile->deletedRecords >= ptFile->totalRecords)
		{//û��δ�ϴ��ļ�¼
			continue;
		}
		
		/* �ȶ�ȡ���ļ�������ժҪ��Ϣ */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileIdx, (char *)pagebuf, 0, absNum);
		//hyUsbPrintf("ժҪ����:%d\r\n", absNum);
		if(absNum == HY_ERROR)//��ȡʧ��
		{
		hyUsbPrintf("absNum  = %d \r\n");
			continue;
		}

		for(j = 0; j < ptFile->totalRecords && quefull == 0; j++)//ѭ������ʵ�ʶ�ȡ����ժҪ����
		{
			recStat = pagebuf[j];
		//hyUsbPrintf("��ǰ��¼����:%d, ״̬:%d\r\n", j, recStat.recState);	
			if(recStat.recState == 0)//0:δ�ϴ�
			{
				//�����ü�¼����ϸ��Ϣ���ص��ϴ�������
				ret = nandFile_read(fileIdx, j, (U8 *)&node, sizeof(tDATA_INFO));
				//hyUsbPrintf("��ȡ��ǰ��¼���(0:�ɹ�):%d\r\n", ret);
				if(ret == HY_OK)
				{
					if(node.tComm.ifimage == 1 && gu32RecUnSendTotal > 0)
					{
						gu32RecUnSendImage++;
					}
					else
					{
						tQue.fileId = fileIdx;
						tQue.recIdx = j;
						tQue.delFlag= 0;
						tQue.opType = node.tComm.opType;
						tQue.tData  = node;
						if(-1 == Queue_AddNode(&tQue))
						{
						hyUsbPrintf("�������ʧ��!! \r\n");
							quefull = 1;
							break;
						}
							hyUsbPrintf("add to queue success nand = %d  gu32RecUnSendImage = %d \r\n",nand, gu32RecUnSendImage);
							if(nand > 0 && node.tComm.ifimage == 0) nand--;

						readCnt++;
						if(nand > 0 && readCnt >= 1000)//Ϊ���ó�ʱ���ǰ̨д��¼  ÿ��ֻ����һ����
						{
							//syssleep(1);
							//readCnt = 0;
							quefull = 1;
							break;
						}
					}
				}
			}
			else if(recStat.recState == 4)//�ɴ�   ����״̬�ļ�¼����ʱ��Ҫ������
			{
				//��ȡ������¼��Ϣ
				ret = nandFile_read(fileIdx, j, (U8 *)&node, sizeof(tDATA_INFO));
				
				if(ret == HY_OK)
				{
					//�����������ж�
					Queue_AddNode_Multi(node.tComm.code, node.tComm.subCode, &node, 0);
				}
			}
		}
		
	}//�ļ�ѭ�� end
hyUsbPrintf("���غ����ʣ���¼:%d, ���غ�δ���ͼ�¼��:%d\r\n", nand, total);
//hyUsbPrintf("\r\nGetAll nand = %d gu32RecInNandTotal = %d total = %d gu32RecUnSendTotal = %d\r\n",nand,gu32RecInNandTotal,total,gu32RecUnSendTotal);
	if(nand > gu32RecInNandTotal && gu32RecInNandTotal > 0) gu32RecInNandTotal = nand;
	if(total > gu32RecUnSendTotal)   gu32RecUnSendTotal = total;
	if(image > gu32RecUnSendImage)   gu32RecUnSendImage = image;

	unlock((unsigned int)&appLockNandCtr);
	
	return HY_OK;
}


/*-----------------------------------------------------------
*������RecStore_GetUnreportedMulti
*���ܣ����ؿɴ�״̬��һƱ�����¼
*����: usertype : Ȩ��
       sendType:  ���ͷ�ʽ  0:��̨����    1:ǰ̨����
*���أ�ʧ�ܣ�-1    �ɹ���0
*------------------------------------------------------------*/
int RecStore_GetUnreportedMulti(U8 usertype, U8 sendType)
{
	int i,j,k, fileNum, ret;
	int fileList[MAX_NAND_FILE];
	tRECORD_STATUS *pagebuf,recStat;
	tFILE_ENTRY	*ptFile;
	tDATA_INFO node;
	tQUE_NODE  tQue;
	int readCnt = 0,absNum,fileIdx;
	int multiFlag = 0;
	
	wlock((unsigned int)&appLockNandCtr);
	
	gu32MultiInNand    = 0;
	fileNum = RecStore_GetFileNameList(fileList);
	
	k = 0;
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
//hyUsbPrintf("fileNum  = %d \r\n", fileNum);
	for (i = 0; i < fileNum; i++)//�ļ�ѭ�� start
	{
		fileIdx = fileList[i];
		ptFile  = &gtFileEntry[fileIdx];
		
		/* kong 2011-05-24 ���жϸ��ļ����Ƿ���δ�ϴ��ļ�¼*/
		if(ptFile->reportedRecords + ptFile->deletedRecords >= ptFile->totalRecords)
		{//û��δ�ϴ��ļ�¼
			continue;
		}
		
		/* �ȶ�ȡ���ļ�������ժҪ��Ϣ */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileIdx, (char *)pagebuf, 0, absNum);
		if(absNum == HY_ERROR)//��ȡʧ��
		{
			continue;
		}
	//hyUsbPrintf("fileIdx  = %d   total  = %d\r\n", i, ptFile->totalRecords);	
		for(j = 0; j < ptFile->totalRecords; j++)//ѭ������ʵ�ʶ�ȡ����ժҪ����
		{
			recStat = ((tRECORD_STATUS *)pagebuf)[j];
		hyUsbPrintf("load rec state = %d \r\n",recStat.recState);	
			if(recStat.recState == 4)//�ɴ�   ����״̬�ļ�¼����ʱ��Ҫ������
			{
				//��ȡ������¼��Ϣ
				ret = nandFile_read(fileIdx, j, (U8 *)&node, sizeof(tDATA_INFO));
				
				if(ret == HY_OK)
				{
					//�����������ж�
					if(-1 == Queue_AddNode_Multi(node.tComm.code, node.tComm.subCode, &node, sendType))
					{
						k = 1;//���Խ���
					}
				}
			}
		}
		
		if(k == 1)
		{
			break;
		}
		
	}//�ļ�ѭ�� end
	
	unlock((unsigned int)&appLockNandCtr);
	
	return HY_OK;
}

/*-----------------------------------------------------------
*������RecStore_GetRecordStateNum
*���ܣ��õ�ָ���û����� ���� ���͵ļ�¼���ݵ�����  (����  ���ϴ�  ��ɾ��)
*������
	   usertype:�û�����
	   pDate_name:ָ��������   ���pEntry��ΪNULL 
	   type:ָ������
	   total: ����
	   rep �����ϴ�
	   unrep��δ�ϴ�
*���أ�ʧ�ܣ�-1    �ɹ���0
*------------------------------------------------------------*/
//int RecStore_GetRecordStateNum(U8 *pDate_name, U8 usertype, U8 type, U32 *total, U32 *rep, U32 *unrep)
int RecStore_GetRecordStateNum(U8 *pDate_name, U8 usertype, U8 type, tREC_INFO *pRecInfo)
{
	int ret, cnt,fileIdx[MAX_NAND_FILE], i, j;
	tFILE_ENTRY	*ptFile;
	tRECORD_STATUS *pagebuf,recStat;
	tDATA_INFO tNode;
	int     rc,absNum,typeCnt=0;
	
hyUsbPrintf("date = %s usertype = %d type = %d\r\n",pDate_name,usertype,type);	
	if (pDate_name == NULL)
		return HY_ERROR;
	
	wlock((unsigned int)&appLockNandCtr);
	
	//*total = *rep = *unrep = 0;	//��0
	pRecInfo->total = 0;
	pRecInfo->reported = 0;
	pRecInfo->unreported = 0;
	cnt = nandFile_open(pDate_name, fileIdx);
	if(cnt <= 0)//û���ҵ�
	{
		hyUsbPrintf("no file\r\n");
		unlock((unsigned int)&appLockNandCtr);
		return HY_ERROR;
	}
	
	for(j = 0; j < cnt; j++)
	{
		ptFile	= &gtFileEntry[fileIdx[j]];
		
		pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	hyUsbPrintf("file idx = %d  total = %d  rep = %d \r\n",fileIdx[j], ptFile->totalRecords, ptFile->reportedRecords);
		/* �ȶ�ȡ���ļ�������ժҪ��Ϣ */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileIdx[j], (char *)pagebuf, 0, absNum);
		if(absNum == HY_ERROR)//��ȡʧ��
		{
			unlock((unsigned int)&appLockNandCtr);
			return HY_ERROR;
		}
		hyUsbPrintf("absNum = %d  totalRecords = %d\r\n",absNum,ptFile->totalRecords);
		for(i = 0; i < ptFile->totalRecords; i++)
		{
			recStat = ((tRECORD_STATUS *)pagebuf)[i];
			
			if(pRecInfo->cnt < pRecInfo->max)
			{
				//��¼�¼�¼������
				for(typeCnt = 0; typeCnt < pRecInfo->cnt; typeCnt++)
				{
					if(pRecInfo->recType[typeCnt] == recStat.type)
					{
						break;
					}
				}
				if(typeCnt >= pRecInfo->cnt)
				{
					pRecInfo->recType[pRecInfo->cnt++] = recStat.type;
				}
			}
			
			//hyUsbPrintf("type = %d  userType = %d  recState = %d \r\n",recStat.type,recStat.userType,recStat.recState);
			/* �ҵ�ָ��Ȩ�ޡ�ɨ�����͡�״̬�ļ�¼ */
			if( (type == recStat.type) && (usertype == 0 || recStat.userType == 0 || usertype == recStat.userType)) /* ��6λΪȨ��  */
			{
				if(usertype != 0 && recStat.userType != 0)//���ǹ���Ա ��Ҫƥ���¼ID��
				{
					//hyUsbPrintf("fileidx = %d \r\n",fileIdx[j]);
					rc = nandFile_read(fileIdx[j], i, (U8 *)&tNode, sizeof(tDATA_INFO));
					//hyUsbPrintf("read rec = %d  %d\r\n",rc,i);
					if(rc == HY_OK)
					{
						rc = RecStore_CmpId(&tNode);
						//hyUsbPrintf("CmpId = %d\r\n",rc);
						if(rc == HY_OK)
						{
							//(*total)++;
							pRecInfo->total++;
							if(0 == recStat.recState || 3 == recStat.recState || 4 == recStat.recState)/* δ�ϴ� */
							{
								//(*unrep)++;
								pRecInfo->unreported++;
							}
							else if(1 == recStat.recState)/* ���ϴ� */
							{
								//(*rep)++;
								pRecInfo->reported++;
							}
						}
					}
				}
				else//����Ա
				{
					//(*total)++;
					pRecInfo->total++;	
					if(0 == recStat.recState || 3 == recStat.recState || 4 == recStat.recState)/* δ�ϴ� */
					{
					//hyUsbPrintf("δ����===\r\n");
						//(*unrep)++;
						pRecInfo->unreported++;
					}
					else if(1 == recStat.recState)/* ���ϴ� */
					{
					//hyUsbPrintf("�ѷ���===\r\n");
						//(*rep)++;
						pRecInfo->reported++;
					}
				}
			}
		}
	}
	unlock((unsigned int)&appLockNandCtr);
hyUsbPrintf("total = %d  unrep = %d rep = %d \r\n",pRecInfo->total,pRecInfo->unreported,pRecInfo->reported);	
	return HY_OK;
}

// ��Ը�nandFile_GetRecordInfo��ƥ�亯��
int CB_typeCheck_RecordInfo(tRECORD_STATUS *pState, tRECORD_STATUS *state)
{
	/* �ҵ�ָ��Ȩ�ޡ�ɨ�����͡�״̬�ļ�¼ */
hyUsbPrintf("type = %d  %d    user type = %d   %d \r\n", state->type, pState->type, state->userType, pState->userType);
	if( state->type == pState->type  
	    && (state->userType == pState->userType || 0 == state->userType || pState->userType == 0)) /* ��6λΪȨ��  */
	{
		return pState->recState;
	}
	else
	{
		return -1;
	}
}

/*-----------------------------------------------------------
*������RecStore_GetRecordInfo
*���ܣ��õ�ָ���û����� ���� ���͵ļ�¼���ݵ���ϸ��Ϣ   ��������ɾ���ļ�¼
*������pEntry:���֪���ļ���entry�򴫽���  ��֪�����Դ�NULL
	   usertype:�û�����
	   date:ָ��������   ���pEntry��ΪNULL  date���Դ�NULL
	   type:ָ������
	   pBuf�����ص�buf  tQUE_NODE�ṹ��
	   buflen��buf����
*���أ�ʧ�ܣ�-1    �ɹ������ؼ�¼����
*------------------------------------------------------------*/
int RecStore_GetRecordInfo(U8 *pDateName, U16 usertype, U8 type, U8 *pBuf, int buflen)
{
	int cnt,fileIdx[MAX_NAND_FILE], ret, total, len;
	int state,absNum;
	tDATA_INFO node;
	tQUE_NODE *pQue;
	tFILE_ENTRY	*ptFile;
	tRECORD_STATUS *pagebuf,recStat,fidRec;
	int  readsize,i,j;
	
	wlock((unsigned int)&appLockNandCtr);
	
	cnt = nandFile_open(pDateName, fileIdx);
	if(cnt <= 0)
	{		
		unlock((unsigned int)&appLockNandCtr);
		return HY_ERROR;
	}
	
	fidRec.type = type;
	fidRec.userType = usertype;
	pQue = (tQUE_NODE *)pBuf;
	total = len = 0;
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	for(j = 0; j < cnt; j++)
	{
		ptFile	= &gtFileEntry[fileIdx[j]];
				
		/* �ȶ�ȡ���ļ�������ժҪ��Ϣ */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileIdx[j], (char *)pagebuf, 0, absNum);
		if(absNum == HY_ERROR)//��ȡʧ��
		{
			unlock((unsigned int)&appLockNandCtr);
			return HY_ERROR;
		}
		
		for(i = 0; (i < ptFile->totalRecords) && (len < buflen); i++)
		{
			recStat = ((tRECORD_STATUS *)pagebuf)[i];
					
			state = CB_typeCheck_RecordInfo(&recStat, &fidRec);
			if(state >= 0)
			{
				ret = nandFile_read(fileIdx[j], i, (U8 *)&node, sizeof(tDATA_INFO));
				//hyUsbPrintf("recInfo read = %d  %d\r\n",ret,i);
				if(ret == HY_OK)
				{
					if(usertype != 0)//���ǹ���Ա
					{
						//��Ҫ��������¼�е�Ա�����ź͵�ǰ��¼Ա�����Ա�
						ret = RecStore_CmpId(&node);
					}
				}
				
				if(ret == HY_OK)
				{
					if(strlen(node.tComm.subCode) == 0 || (strcmp(node.tComm.code, node.tComm.subCode) == 0))
					{
						node.tComm.state = state;
						pQue[total].fileId = fileIdx[j];
						pQue[total].recIdx = i;
						pQue[total].delFlag= node.tComm.state;
						memcpy(&(pQue[total].tData), &node, sizeof(tDATA_INFO));
						total++;
						len += sizeof(tQUE_NODE);
					}
				}
			}
		}
	}

	unlock((unsigned int)&appLockNandCtr);

	return total;
}

/*-----------------------------------------------------------
*������RecStore_GetRecordCode
*���ܣ��õ�ָ���û����� ���� ���͵ļ�¼���ݵ�������Ϣ   ������ɾ���ļ�¼
*������pEntry:���֪���ļ���entry�򴫽���  ��֪�����Դ�NULL
	   usertype:�û�����
	   date:ָ��������   ���pEntry��ΪNULL  date���Դ�NULL
	   type:ָ������
	   pBuf�����ص�buf
	   buflen��buf����
*���أ�ʧ�ܣ�-1    �ɹ������ؼ�¼����
*------------------------------------------------------------*/
int RecStore_GetRecordCode(U8 *pDateName, U16 usertype, U8 type, U8 *pBuf, int buflen)
{
	int cnt,fileIdx[MAX_NAND_FILE], ret, total, len;
	int state,absNum;
	tDATA_INFO node;
	tREC_SEARCH	*pSearch;
	tFILE_ENTRY	*ptFile;
	tRECORD_STATUS *pagebuf,recStat,fidRec;
	int  readsize,i,j;
	
	wlock((unsigned int)&appLockNandCtr);
	
	cnt = nandFile_open(pDateName, fileIdx);
hyUsbPrintf("get file cnt = %d \r\n", cnt);
	if(cnt <= 0)
	{		
		unlock((unsigned int)&appLockNandCtr);
		return HY_ERROR;
	}
	
	fidRec.type = type;
	fidRec.userType = usertype;
	pSearch = (tREC_SEARCH *)pBuf;
	total = len = 0;
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	for(j = 0; j < cnt; j++)
	{
		ptFile	= &gtFileEntry[fileIdx[j]];
	hyUsbPrintf("file id = %d  total = %d \r\n", fileIdx[j], ptFile->totalRecords);			
		/* �ȶ�ȡ���ļ�������ժҪ��Ϣ */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileIdx[j], (char *)pagebuf, 0, absNum);
		if(absNum == HY_ERROR)//��ȡʧ��
		{
			unlock((unsigned int)&appLockNandCtr);
			return HY_ERROR;
		}
		
		for(i = 0; (i < ptFile->totalRecords) && (len < buflen); i++)
		{
			recStat = ((tRECORD_STATUS *)pagebuf)[i];
					
			state = CB_typeCheck_RecordInfo(&recStat, &fidRec);
			if(state >= 0)
			{
				ret = nandFile_read(fileIdx[j], i, (U8 *)&node, sizeof(tDATA_INFO));
				//hyUsbPrintf("recInfo read = %d  %d\r\n",ret,i);
				if(ret == HY_OK)
				{
					if(usertype != 0)//���ǹ���Ա
					{
						//��Ҫ��������¼�е�Ա�����ź͵�ǰ��¼Ա�����Ա�
						ret = RecStore_CmpId(&node);
					}
				}
				
				if(ret == HY_OK)//ͼƬ��¼������
				{
					//�����һƱ���,��ֻ��������,
					if(strlen(node.tComm.subCode) == 0 || (strcmp(node.tComm.code, node.tComm.subCode) == 0))
					{
						memset(&pSearch[total], 0, sizeof(tREC_SEARCH));
						pSearch[total].fileId = fileIdx[j];
						pSearch[total].recIdx = i;
						pSearch[total].recState = state;
						pSearch[total].ifimage = node.tComm.ifimage;
						strcpy(pSearch[total].barCode, node.tComm.code);
						
						total++;
						len += sizeof(tREC_SEARCH);
					}
				}
			}
		}
	}

	unlock((unsigned int)&appLockNandCtr);

	return total;
}

/*-----------------------------------------------------------
*������RecStore_delLog
*���ܣ�ɾ��ָ������������ļ�
*����: dates ---- ָ��������
*���أ�ʧ�ܣ�-1    �ɹ���0
*------------------------------------------------------------*/
int RecStore_delLog(void *names)
{
	int i, fileNum;
	int fileList[MAX_NAND_FILE];
	int filename;
	
	filename = atoi(names);
	wlock((unsigned int)&appLockNandCtr);
	
	fileNum = RecStore_GetFileNameList(fileList);
	for (i = 0; i < fileNum; i++)
	{
		if (filename != gtFileEntry[fileList[i]].name)
		{
			nandFile_delete(gtFileEntry[fileList[i]].name);
		}
	}
	unlock((unsigned int)&appLockNandCtr);
	
	return HY_OK;
}

/*-----------------------------------------------------------
*������RecStore_Clear
*���ܣ�������м�¼��Ϣ
*����: none
*���أ�NULL
*------------------------------------------------------------*/
void RecStore_Clear()
{
	char *pagebuf;
	
	wlock((unsigned int)&appLockNandCtr);
	pagebuf = (char *)gpu8RecBuf;
	nandFile_format(0, pagebuf);
	unlock((unsigned int)&appLockNandCtr);
	
	return;
}

// ��Ը�nandFile_DelErrorScan������ƥ�亯��
int CB_typeCheck_DelErrorScan(tRECORD_STATUS *pState, tRECORD_STATUS *state)
{
	/* �ҵ�ָ��Ȩ�ޡ�δ�ϴ��ļ�¼ */
	if( state->type == pState->type      /* ɨ������ */
		&& 2 != pState->recState  /* 2:��ɾ�� */
		&& 1 != pState->recState  /* 1 ���ϴ�*/
	    && (state->userType == pState->userType || 0 == state->userType || pState->userType == 0) ) /* ��6λΪȨ��  */			       
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}

// ��Ը�nandFile_DelErrorScan������ƥ�亯��
int CB_dataCheck_DelErrorScan(tDATA_INFO *pNode, char *barCode)
{
	int ret = -1;
	
	if (pNode == NULL || barCode == NULL)
		return -1;
	
	if(strcmp(pNode->tComm.code, barCode) == 0 || strcmp(pNode->tComm.subCode, barCode) == 0)
	{
		ret = 0;
	}
	
	return ret;
}


/*-----------------------------------------------------------
*������RecStore_DelErrorScan
*���ܣ�ɾ�� ĳһ��¼ (����Ȩ��,ɨ������,ɨ����)
*������usertype:�û�����	   
	   type:ָ������
	   code:ɨ����  (������һƱһ��, ������һƱ����������Ż��ӵ���)
*���أ��ɹ�ɾ���ĸ���
*------------------------------------------------------------*/
int RecStore_DelErrorScan(U16 usertype, U8 type, U8 *pcode)
{
	int i, j, fileNum,recNum,ret;
	int recId,delCnt=0,isQuit=0;
	int fileList[MAX_NAND_FILE];
	int readCnt = 0,absNum,fileIdx;
	tRECORD_STATUS *pagebuf,state;
	tFILE_ENTRY	*ptFile;
	tDATA_INFO  node;
	
	if(NULL == pcode) 
		return HY_ERROR;
	
	ret = HY_ERROR;
	recId = 0;
	
	wlock((unsigned int)&appLockNandCtr);
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	fileNum = RecStore_GetFileNameList(fileList);
	for (i = 0; i < fileNum && isQuit == 0; i++)
	{
		fileIdx = fileList[i];
		ptFile  = &gtFileEntry[fileIdx];
		
		/* kong 2011-05-24 ���жϸ��ļ����Ƿ���δ�ϴ��ļ�¼*/
		if(ptFile->reportedRecords + ptFile->deletedRecords >= ptFile->totalRecords)
		{//û��δ�ϴ��ļ�¼
			continue;
		}
		
		/* �ȶ�ȡ���ļ�������ժҪ��Ϣ */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileIdx, (char *)pagebuf, 0, absNum);
		if(absNum == HY_ERROR)//��ȡʧ��
		{
			continue;
		}
		
		//ѭ��������ж�������ժҪ
		for(j = 0; j < absNum && isQuit == 0; j++)
		{
			for(recNum = 0; recNum < ABSTRACT_NUM_PER_PAGE && isQuit == 0; recNum++)//ѭ������ʵ�ʶ�ȡ����ժҪ����
			{
				recId = j*ABSTRACT_NUM_PER_PAGE+recNum;
				if(recId >= ptFile->totalRecords)
				{
					break;
				}
				
				state = pagebuf[recId];
				
				if(state.recState == 1 || state.recState == 2)//�Ѵ�����ɾ
				{
					continue;
				}
				
				if(state.type == type && (state.userType == usertype || usertype == 0))//������ͬ  �� Ȩ����ͬ���ǹ���Ա
				{
					//�Ƚ�����  --��Ҫ������¼
					if(HY_ERROR == nandFile_read(fileList[i], recId, (U8 *)&node, sizeof(tDATA_INFO)))
					{
						continue;
					}
					//������ͬ
					if(strcmp(pcode, node.tComm.code) == 0)
					{
						if(strlen(node.tComm.subCode) == 0)//û���ӵ���,˵����һƱһ��
						{
							ret = Queue_DelNode(fileIdx, recId);
							hyUsbPrintf("Queue_DelNode  ret = %d \r\n", ret);
							if (ret == HY_OK || ret == -1 || 0 == usertype) /* ����Ա���ܲ������δ�ϴ��ĵ����� */
							{
								pagebuf[recId].recState = 2;//�ĳ���ɾ
								ptFile->deletedRecords++;
								delCnt++;
							}
							isQuit = 1;
						}
						else//һƱ���
						{
							if(Net_IfSendMulti() && strcmp(Queue_GetMainCode(), pcode) == 0)//�Ѿ����ص��ϴ�������
							{
								isQuit = 1;
							}
							else
							{
								pagebuf[recId].recState = 2;//�ĳ���ɾ
								ptFile->deletedRecords++;
								delCnt++;
								if(strcmp(Queue_GetMainCode(), pcode) == 0)
								{
									//��ձ�Ʊ��Ϣ
									Queue_ClearMulti();
								}
								//Ʊ����1
								Rec_DelMultiCnt();
							}
						}
					}
					else if(strcmp(pcode, node.tComm.subCode) == 0)
					{
						//���ӵ���,��һƱ���
						if(Net_IfSendMulti() && strcmp(Queue_GetMainCode(), node.tComm.code) != 0)//δ���ص��ϴ�������
						{
							pagebuf[recId].recState = 2;//�ĳ���ɾ
							ptFile->deletedRecords++;
							delCnt++;
							if(strcmp(Queue_GetMainCode(), pcode) == 0)
							{
								//��ձ�Ʊ��Ϣ
								Queue_ClearMulti();
							}
							//Ʊ����1
							Rec_DelMultiCnt();
						}
						isQuit = 1;
					}
				}
			}
			
			//��ժҪblock������,��д
			recId = j*ABSTRACT_NUM_PER_PAGE;
			//hyUsbPrintf("write back abs  fileid = %d   recid = %d \r\n", fileList[i], recIdx);
			nandFile_WriteBackAbs(fileList[i], recId, (char *)pagebuf+j*NAND_PAGE_SIZE);
		}
	}
	
	unlock((unsigned int)&appLockNandCtr);
	
	return delCnt;
}


// ��Ը�RecStore_ChangeState������ƥ�亯��
int CB_typeCheck_ChangeState(tRECORD_STATUS *pState, tRECORD_STATUS *state)
{
	/* �ҵ�ָ��Ȩ�ޡ�δ�ϴ��ļ�¼ */
	if( state->type == pState->type      /* ɨ������ */
		&& 2 != pState->recState  /* 2:��ɾ�� */
		&& 1 != pState->recState  /* 1 ���ϴ�*/
	    && (state->userType == pState->userType || 0 == state->userType || pState->userType == 0) ) /* ��6λΪȨ��  */			       
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}

// ��Ը�RecStore_ChangeState������ƥ�亯��
int CB_dataCheck_ChangeState(tDATA_INFO *pNode, char *barCode)
{
	int ret = -1;
	
	if (pNode == NULL || barCode == NULL)
		return -1;
	
	if(strcmp(pNode->tComm.code, barCode) == 0 || strcmp(pNode->tComm.subCode, barCode) == 0)
	{
		ret = 0;
	}
	
	return ret;
}

// ��Ը�RecStore_ChangeState�������޸ĺ���  ��״̬Ϊ "����"
int CB_modifyState_ChangeState_Disable(tRECORD_STATUS *pStatus)
{
	pStatus->recState = 3;

	return pStatus->recState;
}

// ��Ը�RecStore_ChangeState�������޸ĺ���  ��״̬Ϊ "�ɴ�"
int CB_modifyState_ChangeState_Enable(tRECORD_STATUS *pStatus)
{
	pStatus->recState = 4;

	return pStatus->recState;
}

/*-----------------------------------------------------------
*������RecStore_ChangeState
*���ܣ��ı�ָ���û�����  ҵ������  ����    ��״̬
*������usertype:�û�����	   
	   type:ָ������
	   code:ɨ����
	   changeState:��Ҫ�ı�ɵ�״̬
*���أ�ʧ�ܣ�-1,  �����ϴ�: -2   ���ϴ�: -3    �ɹ���0
*------------------------------------------------------------*/
int RecStore_ChangeState(U16 usertype, U8 type, U8 *pcode, U8 changeState)
{
	int i, fileNum, ret;
	int recId;
	int fileList[MAX_NAND_FILE];
	tRECORD_STATUS state;
	MODIFY_STATUE_FUNC pFun;
	
	if(NULL == pcode || changeState == 0 || changeState > 4) 
		return HY_ERROR;
	
	ret = HY_ERROR;
	recId = 0;
	state.type     = type;
	state.userType = usertype;
	
	wlock((unsigned int)&appLockNandCtr);
	
	fileNum = RecStore_GetFileNameList(fileList);

	for (i = 0; i < fileNum; i++)
	{
		recId = 0;
		
		ret = RecStoresub_SearchNextRecord(fileList[i], CB_dataCheck_ChangeState, CB_typeCheck_ChangeState, pcode, &state, &recId);
		if (ret == HY_OK)	//ƥ��ɹ�.
		{
			ret = Queue_DelNode(fileList[i], recId);
			
			if (ret == HY_OK || ret == -1 || 0 == usertype) /* ����Ա���ܲ������δ�ϴ��ĵ����� */
			{
				switch(changeState)
				{
				case 1:
					pFun = CB_modifyState_repedRecord;
					break;
				case 2:
					pFun = CB_modifyState_DeleteRecord;
					break;
				case 3:
					pFun = CB_modifyState_ChangeState_Disable;
					break;
				case 4:
					pFun = CB_modifyState_ChangeState_Enable;
					break;
				default:
					pFun = NULL;
					break;
				}
				nandFile_changeRecordStatus(fileList[i], recId, pFun);
				if(ret == -1)
				{//nand�ϴ��ڸü�¼ ��δ���ؽ��ϴ�����
					ret = HY_OK;
				}
			}
			break;	//���������,�����ļ���ֻ��һ�������ļ�¼. ��ɾ������ֱ��break.
		}
	}

	unlock((unsigned int)&appLockNandCtr);
	
	return ret;
}


/*-----------------------------------------------------------
*������RecStore_ChangeMultiCodeState
*���ܣ��ı�ָ���û�����  ҵ������  ����    ��״̬   (һƱ���ʹ��)    ���ܵ��л�:1:����-->�ɴ�   2:����-->ɾ��  3:�ɴ�-->�Ѵ�   4:�ɴ�-->ɾ��
*������usertype:�û�����	   
	   type:ָ������
	   code:ɨ����(������)
	   changeState:��Ҫ�ı�ɵ�״̬
*���أ�ʧ�ܣ�-1,  �����ϴ�: -2   ���ϴ�: -3    �ɹ���0
*------------------------------------------------------------*/
int RecStore_ChangeMultiCodeState(U16 usertype, U8 type, U8 *pcode, U8 changeState)
{
	int i, j, fileNum, ret,rc;
	int recIdx,recNum,absNum;
	int fileList[MAX_NAND_FILE];
	tDATA_INFO  node;
	tFILE_ENTRY	*ptFile;
	tRECORD_STATUS *pagebuf;
	tRECORD_STATUS state;
	MODIFY_STATUE_FUNC pFun;
	
	if(NULL == pcode || changeState == 0 || changeState > 4) 
		return HY_ERROR;
	
	ret = HY_ERROR;
	state.type     = type;
	state.userType = usertype;
	
	wlock((unsigned int)&appLockNandCtr);
	
	fileNum = RecStore_GetFileNameList(fileList);
hyUsbPrintf("filenum = %d  state = %d  \r\n", fileNum, changeState);	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	for (i = 0; i < fileNum; i++)
	{
		ptFile	= &gtFileEntry[fileList[i]];
		if(ptFile->reportedRecords + ptFile->deletedRecords >= ptFile->totalRecords)
		{//û��δ�ϴ��ļ�¼
			continue;
		}
		
		//���ļ��м���ժҪ
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		
		//��ͷ��ȡ��Ч��ժҪ��Ϣ
		absNum = nandFile_readFileAbstract(fileList[i], (char *)pagebuf, 0, absNum);
		
		if(absNum == HY_ERROR)//��ȡʧ��
		{
			continue;
		}
	hyUsbPrintf("absNum = %d  \r\n", absNum);
		//ѭ��������ж�������ժҪ
		for(j = 0; j < absNum; j++)
		{
			for(recNum = 0; recNum < ABSTRACT_NUM_PER_PAGE; recNum++)
			{
				recIdx = j*ABSTRACT_NUM_PER_PAGE+recNum;
				if(recIdx >= ptFile->totalRecords)
				{
					break;
				}
		hyUsbPrintf("abs recState = %d  userType = %d   type = %d \r\n", pagebuf[recIdx].recState, pagebuf[recIdx].userType, pagebuf[recIdx].type);
				if(pagebuf[recIdx].recState != 3 && pagebuf[recIdx].recState != 4)//ֻ�ܸı���� �� �ɴ�״̬�ļ�¼
				{
					continue;
				}
				
				//Ȩ��Ϊ����Ա  ��Ȩ����ͬ       ɨ��������ͬ
				if( (usertype == 0 || usertype == pagebuf[recIdx].userType) && (pagebuf[recIdx].type == type))
				{
					//�Ƚ�����  --��Ҫ������¼
					rc = nandFile_read(fileList[i], recIdx, (U8 *)&node, sizeof(tDATA_INFO));
					if(rc == HY_ERROR)
					{
						continue;
					}
					
					hyUsbPrintf("code = %s   %s   %s\r\n",pcode, node.tComm.code, node.tComm.subCode);
					//������ͬ
					if(strcmp(pcode, node.tComm.code) == 0)
					{
						//�ı�״̬
						pagebuf[recIdx].recState = changeState;
						if(changeState == 1)//�Ѵ�
						{
							ptFile->reportedRecords++;
						}
						else if(changeState == 2)//��ɾ
						{
							ptFile->deletedRecords++;
						}
					}
				}
			}
			
			//��ժҪblock������,��д
			recIdx = j*ABSTRACT_NUM_PER_PAGE;
			hyUsbPrintf("write back abs  fileid = %d   recid = %d \r\n", fileList[i], recIdx);
			nandFile_WriteBackAbs(fileList[i], recIdx, (char *)pagebuf+j*NAND_PAGE_SIZE);
		}
	}

	unlock((unsigned int)&appLockNandCtr);
	
	return ret;
}

/*----------------------------------------------------------------
*������RecStore_DeleteOldRecord
*���ܣ�������ڵļ�¼.(ÿ���½ʱ,����Ƿ��й��ڵļ�¼,�����,��ɾ��֮.)
*������pDateName: ����֮ǰ����Ϊ�Ǿɵļ�¼(��������).
*���أ�0.
*-----------------------------------------------------------------*/
int RecStore_DeleteOldRecord(U8 *pDateName)
{
	int i, fileNum;
	int fileList[MAX_NAND_FILE];
	int filename;
	
	filename = atoi(pDateName);
	wlock((unsigned int)&appLockNandCtr);
	fileNum = RecStore_GetFileNameList(fileList);
	
	for (i = 0; i < fileNum; i++)
	{
		if (filename >= gtFileEntry[fileList[i]].name)
		{
			nandFile_delete(gtFileEntry[fileList[i]].name);
		}
	}
	unlock((unsigned int)&appLockNandCtr);
	
	return HY_OK;
}


// ��ȡ����.
int nandFile_CallBack_GetStatueValue(U8 *data, int len, int *param1, int *param2)
{
	tSEQUECE_PAGE	*pagebuf;
	tDATA_INFO      *ptRec;
	U8  type, usertype;
	
	pagebuf = (U8 *)gpu8RecBuf;
	
	memcpy((U8 *)pagebuf,data,len);	
	
	ptRec = (tDATA_INFO *)data ;  /* δ��ֵ����ȷ���ж� */	
	type = ptRec->tComm.opType ;
	 
	usertype = ptRec->tComm.right ;
//hyUsbPrintf("GetStatueValue opType = %d  right  = %d \r\n ",type, usertype);	
	*param1 = type;
	*param2 = usertype;
	
	return 1;
}

/*-----------------------------------------------------------
*������RecStore_LoadAllSubcode
*���ܣ���Ҫ���һƱ���,���������ӵ�
*������pBuf:���ص����ڴ�
       pCode:������
*���أ�ʧ�ܣ�-1    �ɹ������ؼ�¼����
*------------------------------------------------------------*/
int RecStore_LoadAllSubcode(U8 *pBuf, U8 *pCode)
{
	return -1;//��δʵ��
}

int CB_dataCheck_SearchByCode(tDATA_INFO *pNode, char *barCode)
{
	int ret = -1;
	
	if (pNode == NULL || barCode == NULL)
		return -1;
	
	if(strcmp(pNode->tComm.code, barCode) == 0 || strcmp(pNode->tComm.subCode, barCode) == 0)
	{
		ret = 0;
	}
	
	return ret;
}

int CB_typeCheck_SearchByCode(tRECORD_STATUS *pState, tRECORD_STATUS *state)
{
	/* �ҵ�ָ��Ȩ�ޡ�δ�ϴ��ļ�¼ */
	if( state->type == pState->type      /* ɨ������ */
	    && (state->userType == pState->userType || 0 == state->userType || pState->userType == 0) ) /* ��6λΪȨ��  */			       
	{
		return 0;
	}
	else 
	{
		return -1;
	}
}

/*-----------------------------------------------------------
*������RecStore_SearchByCode
*���ܣ�ͨ���û�����  ��������  ����  ������Ϣ
*������right:�û�����	   
	   type:ָ������
	   pDate:ָ������   ΪNULLʱ��ѯȫ��
	   code:ɨ���� (�����һƱ���  �����ӵ���)
	   pBuf:���ز鵽������
*���أ�ʧ�ܣ�-1,  �����ϴ�: -2   ���ϴ�: -3    �ɹ���>0
*------------------------------------------------------------*/
int RecStore_SearchByCode(U16 right, U8 type, U8 *pDate, U8 *pcode, U8 *pBuf, int buflen)
{
	int i, fileIdx, fileNum, ret, total, len;
	int recId,state,absNum;
	int fileList[MAX_NAND_FILE];
	tRECORD_STATUS fidRec, recStat, *pagebuf;
	tFILE_ENTRY	*ptFile;
	tDATA_INFO *pData;
	tDATA_INFO node;
	U8  searchCode[32];
	
	if(NULL == pcode) 
		return HY_ERROR;
	
	strcpy(searchCode, pcode);
	ret = HY_ERROR;

	wlock((unsigned int)&appLockNandCtr);
//hyUsbPrintf("right  = %d  type = %d code = %s \r\n", right, type, searchCode);	
	total = len = 0;
	
	fidRec.type = type;
	fidRec.userType = right;
//hyUsbPrintf("buf addr = %x  \r\n",(U32)pBuf);
	pData = (tDATA_INFO *)pBuf;
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	
	if(pDate == NULL)//��ѯȫ��
	{
		fileNum = RecStore_GetFileNameList(fileList);
	}
	else
	{
		fileNum = nandFile_open(pDate, fileList);
	}
	//hyUsbPrintf("filenum = %d \r\n", fileNum);
	for (fileIdx = 0; fileIdx < fileNum; fileIdx++)
	{
		ptFile	= &gtFileEntry[fileList[fileIdx]];
		/* �ȶ�ȡ���ļ�������ժҪ��Ϣ */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileList[fileIdx], (char *)pagebuf, 0, absNum);
	//hyUsbPrintf("absNum = %d \r\n",absNum);
		if(absNum == HY_ERROR)//��ȡʧ��
		{
			unlock((unsigned int)&appLockNandCtr);
			return HY_ERROR;
		}
	//hyUsbPrintf("totalRecords = %d \r\n",ptFile->totalRecords);
		for(i = 0; (i < ptFile->totalRecords) && (len < buflen); i++)
		{
			recStat = ((tRECORD_STATUS *)pagebuf)[i];
		//hyUsbPrintf("rectype = %d   fidRectype  = %d \r\n",recStat.type,fidRec.type);			
			state = CB_typeCheck_RecordInfo(&recStat, &fidRec);
			if(state >= 0)
			{
				ret = nandFile_read(fileList[fileIdx], i, (U8 *)&node, sizeof(tDATA_INFO));
			//hyUsbPrintf("recInfo read optype = %d   i = %d \r\n",node.tComm.opType,i);
				if(ret == HY_OK)
				{
					if(gu8CurRight != 0)//���ǹ���Ա
					{
						//��Ҫ��������¼�е�Ա�����ź͵�ǰ��¼Ա�����Ա�
						ret = RecStore_CmpId(&node);
					}
				}
			//hyUsbPrintf("ret = %d \r\n",ret);	
				if(ret == HY_OK)
				{
					//�Ƚϵ���
					if(strcmp(searchCode, node.tComm.subCode) == 0)
					{
						if(strcmp(searchCode, node.tComm.code) == 0)
						{
							//hyUsbPrintf("total  = %d  size = %d \r\n",total,sizeof(tDATA_INFO));
						hyUsbPrintf("00opType = %d searchCode = %s \r\n", node.tComm.opType,node.tComm.code);
							
						//hyUsbPrintf("pData addr = %x  \r\n",(U32)&pData[total]);
							memcpy((char *)&pData[total], (char *)&node, sizeof(tDATA_INFO));
						
						//hyUsbPrintf("11opType = %d searchCode = %s \r\n", pData[total].tComm.opType,pData[total].tComm.code);
							pData[total].tComm.state = recStat.recState;
						//hyUsbPrintf("22opType = %d searchCode = %s \r\n", pData[total].tComm.opType,pData[total].tComm.code);
							total++;
							len += sizeof(tDATA_INFO);
						}
						else
						{
							strcpy(searchCode, node.tComm.code);
						}
					}
					else if(strcmp(searchCode, node.tComm.code) == 0 && node.tComm.subCode[0] == 0)
					{
						//hyUsbPrintf("total  = %d  size = %d \r\n",total,sizeof(tDATA_INFO));
					hyUsbPrintf("00opType = %d searchCode = %s \r\n", node.tComm.opType,node.tComm.code);
						
					//hyUsbPrintf("pData addr = %x  \r\n",(U32)&pData[total]);
						memcpy((char *)&pData[total], (char *)&node, sizeof(tDATA_INFO));
					
					//hyUsbPrintf("11opType = %d searchCode = %s \r\n", pData[total].tComm.opType,pData[total].tComm.code);
						pData[total].tComm.state = recStat.recState;
					//hyUsbPrintf("22opType = %d searchCode = %s \r\n", pData[total].tComm.opType,pData[total].tComm.code);
						total++;
						len += sizeof(tDATA_INFO);
					}
				}
			}
		}
	}
	
	unlock((unsigned int)&appLockNandCtr);
	
	//hyUsbPrintf("search optype = %d  code = %s  \r\n", pData[0].tComm.opType,pData[0].tComm.code);
	
	return total;
}


/*-----------------------------------------------------------
*������RecStore_GetSubCodeByMainCode
*���ܣ�ͨ���û�����  ��������  ������ ��ȡ���������е��ӵ���
*������right:�û�����	   
	   type:ָ������
	   pDate:ָ������   ΪNULLʱ��ѯȫ��
	   pMainCode:������
	   pBuf:���ز鵽������
*���أ�ʧ�ܣ�-1,  �����ϴ�: -2   ���ϴ�: -3    �ɹ���0
*------------------------------------------------------------*/
int RecStore_GetSubCodeByMainCode(U16 right, U8 type, U8 *pDate, U8 *pMainCode, U8 *pBuf, int buflen)
{
	int i, fileIdx, fileNum, ret, total, len;
	int recId,state,absNum;
	int fileList[MAX_NAND_FILE];
	tRECORD_STATUS fidRec, recStat, *pagebuf;
	tFILE_ENTRY	*ptFile;
	tDATA_INFO *pData;
	tDATA_INFO node;
	
	if(NULL == pMainCode) 
		return HY_ERROR;
	
	ret = HY_ERROR;

	wlock((unsigned int)&appLockNandCtr);
//hyUsbPrintf("right  = %d  type = %d code = %s \r\n", right, type, pcode);	
	total = len = 0;
	
	fidRec.type = type;
	fidRec.userType = right;
//hyUsbPrintf("buf addr = %x  \r\n",(U32)pBuf);
	pData = (tDATA_INFO *)pBuf;
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	
	if(pDate == NULL)//��ѯȫ��
	{
		fileNum = RecStore_GetFileNameList(fileList);
	}
	else
	{
		fileNum = nandFile_open(pDate, fileList);
	}
	//hyUsbPrintf("filenum = %d \r\n", fileNum);
	for (fileIdx = 0; fileIdx < fileNum; fileIdx++)
	{
		ptFile	= &gtFileEntry[fileList[fileIdx]];
		/* �ȶ�ȡ���ļ�������ժҪ��Ϣ */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileList[fileIdx], (char *)pagebuf, 0, absNum);
	//hyUsbPrintf("absNum = %d \r\n",absNum);
		if(absNum == HY_ERROR)//��ȡʧ��
		{
			unlock((unsigned int)&appLockNandCtr);
			return HY_ERROR;
		}
	hyUsbPrintf("totalRecords = %d \r\n",ptFile->totalRecords);
		for(i = 0; (i < ptFile->totalRecords) && (len < buflen); i++)
		{
			recStat = ((tRECORD_STATUS *)pagebuf)[i];
		hyUsbPrintf("rectype = %d   fidRectype  = %d \r\n",recStat.type,fidRec.type);			
			state = CB_typeCheck_RecordInfo(&recStat, &fidRec);
			if(state >= 0)
			{
				ret = nandFile_read(fileList[fileIdx], i, (U8 *)&node, sizeof(tDATA_INFO));
			hyUsbPrintf("recInfo read optype = %d   i = %d \r\n",node.tComm.opType,i);
				if(ret == HY_OK)
				{
					if(gu8CurRight != 0)//���ǹ���Ա
					{
						//��Ҫ��������¼�е�Ա�����ź͵�ǰ��¼Ա�����Ա�
						ret = RecStore_CmpId(&node);
					}
				}
			hyUsbPrintf("ret = %d \r\n",ret);	
				if(ret == HY_OK)
				{
					//�Ƚ�������
					if(strcmp(pMainCode, node.tComm.code) == 0)
					{
					//hyUsbPrintf("total  = %d  size = %d \r\n",total,sizeof(tDATA_INFO));
					hyUsbPrintf("00opType = %d pcode = %s \r\n", node.tComm.opType,node.tComm.code);
						
					//hyUsbPrintf("pData addr = %x  \r\n",(U32)&pData[total]);
						memcpy((char *)&pData[total], (char *)&node, sizeof(tDATA_INFO));
					
					hyUsbPrintf("11opType = %d pcode = %s \r\n", pData[total].tComm.opType,pData[total].tComm.code);
						pData[total].tComm.state = recStat.recState;
					//hyUsbPrintf("22opType = %d pcode = %s \r\n", pData[total].tComm.opType,pData[total].tComm.code);
						total++;
						len += sizeof(tDATA_INFO);
					}
				}
			}
		}
	}
	
	unlock((unsigned int)&appLockNandCtr);
	
	//hyUsbPrintf("search optype = %d  code = %s  \r\n", pData[0].tComm.opType,pData[0].tComm.code);
	
	return total;
}

//ͨ���ļ������ͼ�¼�������Ҽ�¼
int RecStore_SearchByIndex(int fileIdx, int recIdx, tDATA_INFO *pData)
{
	int i, ret;
	int state,absStart;
	tRECORD_STATUS recStat, *pagebuf;
	tFILE_ENTRY	*ptFile;
	
	ret = HY_ERROR;

	wlock((unsigned int)&appLockNandCtr);
		
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);

	ptFile	= &gtFileEntry[fileIdx];

	absStart = recIdx/ABSTRACT_NUM_PER_PAGE;

	if(nandFile_readFileAbstract(fileIdx, (char *)pagebuf, absStart, 1) == HY_ERROR)//��ȡʧ��
	{
		unlock((unsigned int)&appLockNandCtr);
		return HY_ERROR;
	}

	recStat = ((tRECORD_STATUS *)pagebuf)[recIdx%ABSTRACT_NUM_PER_PAGE];
	nandFile_read(fileIdx, recIdx, (U8 *)pData, sizeof(tDATA_INFO));
	pData->tComm.state = recStat.recState;
	
	unlock((unsigned int)&appLockNandCtr);
		
	return 1;
}

/*-----------------------------------------------------------
*������RecStore_LoadInfo
*���ܣ��õ�ָ���û����� ���� ���͵ļ�¼���ݵ���ϸ��Ϣ   ��������ɾ���ļ�¼
*������pEntry:���֪���ļ���entry�򴫽���  ��֪�����Դ�NULL
	   usertype:�û�����
	   date:ָ��������   ���pEntry��ΪNULL  date���Դ�NULL
	   type:ָ������
	   pBuf�����ص�buf  tQUEnode�ṹ��
	   buflen��buf����
*���أ�ʧ�ܣ�-1    �ɹ������ؼ�¼����
*------------------------------------------------------------*/
int RecStore_LoadInfo(U8 *pDateName, U8 *pBuf, int buflen)
{
	int fileIdx[MAX_NAND_FILE], ret, total, len;
	int absNum,cnt;
	tDATA_INFO node;
	tQUE_NODE  *pQue;
	tFILE_ENTRY	*ptFile;
	tRECORD_STATUS *pagebuf,recStat;
	int  readsize,i,j;
	
	wlock((unsigned)&appLockNandCtr);
	
	cnt = nandFile_open(pDateName, fileIdx);
	if(cnt <= 0)
	{		
		unlock((unsigned int)&appLockNandCtr);
		return HY_ERROR;
	}
	
	pQue = (tQUE_NODE *)pBuf;
	total = len = 0;
	
	pagebuf = (tRECORD_STATUS *)(gpu8RecBuf + NAND_PAGE_SIZE);
	
	for(j = 0; j < cnt; j++)
	{
		ptFile	= &gtFileEntry[fileIdx[j]];
		
		/* �ȶ�ȡ���ļ�������ժҪ��Ϣ */
		absNum = ptFile->totalRecords/ABSTRACT_NUM_PER_PAGE;
		if(ptFile->totalRecords%ABSTRACT_NUM_PER_PAGE)
		{
			absNum++;
		}
		absNum = nandFile_readFileAbstract(fileIdx[j], (char *)pagebuf, 0, absNum);
		if(absNum == HY_ERROR)//��ȡʧ��
		{
			unlock((unsigned)&appLockNandCtr);
			return HY_ERROR;
		}
		
		for(i = 0; (i < ptFile->totalRecords) && (len < buflen); i++)
		{
			recStat = ((tRECORD_STATUS *)pagebuf)[i];

			ret = nandFile_read(fileIdx[j], i, (U8 *)&node, sizeof(tDATA_INFO));
			
			if(ret == HY_OK)
			{
				node.tComm.state = recStat.recState;
				pQue[total].fileId = fileIdx[j];
				pQue[total].recIdx = i;
				pQue[total].delFlag= node.tComm.state;
				pQue[total].opType = node.tComm.opType;
				memcpy(&(pQue[total].tData), &node, sizeof(tDATA_INFO));
				total++;
				len += sizeof(tQUE_NODE);
			}
		}
	}

	unlock((unsigned)&appLockNandCtr);

	return total;
}