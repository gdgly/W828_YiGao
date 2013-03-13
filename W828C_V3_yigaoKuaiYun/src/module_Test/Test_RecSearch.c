#include "Test_MainList.h"



/*======================================================================================================*/
const U8* const opType_Test[]=//����ҵ��
{
	"״̬:",
	
	"��������:",
	"ɨ��ص�:",
	"Ա�����:",
	"����:",
	"ɨ��ʱ��:",
	"����:",
	"�ӵ���",
	
	"��һվ:",
};

/*---------------------------------------------------------------
*����: Op_Test_SearchDisplay
*����: ��ʾ��¼��ѯ�������ַ���
*����: pData:��ѯ���Ľ��
       total:��ѯ��������
       pDispStr:(out)��ʾ��buf
       dispBufLen:pDispStr����󳤶�
*����: 
----------------------------------------------------------------*/
int Op_Test_SearchDisplay(tDATA_INFO *pData, int total, U8 *pDispStr, int dispBufLen)
{
	U8 *str[]={"δ��", "�Ѵ�", "��ɾ", "����", "�ɴ�"};
	int i, j, cnt,stat,len=0;
	tDATA_COMM 	*pComm;
	tTESTOP		*pTestOp;
	
	for(i = 0; i < total; i++)
	{
		pComm = &(pData[i].tComm);
		stat = pComm->state;
		pTestOp = &pData[i].uData.testOp;
		
		sprintf(&pDispStr[len], "%s%s\r\n%s%d\r\n%s%s\r\n%s%s\r\n%s%d\r\n%s%s\r\n%s%s\r\n%s%s\r\n-----------------------\r\n",
							opType_Test[0], str[stat],
							opType_Test[1], pComm->opType,
							opType_Test[2], pComm->scanStat,
							opType_Test[3], pComm->userId,
							opType_Test[4], pComm->groupId,
							opType_Test[5], pComm->scanTime,
							opType_Test[6], pComm->code,
							opType_Test[7], pComm->subCode,
							
							opType_Test[8], pTestOp->Station
							);
		
		len += strlen(&pDispStr[len]);
		
		//�����ж�
		if(len+1024 >= dispBufLen)
		{
			strcpy(&pDispStr[len], "�ڴ治��,������Ϣ�޷���ʾ.");
			break;
		}
	}
	
	return 0;
}
