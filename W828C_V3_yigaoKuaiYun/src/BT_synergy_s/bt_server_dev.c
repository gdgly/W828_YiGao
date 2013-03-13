#include "ospriority.h"
#include "std.h"
#include "sys.h"
#include "hyTypes.h"
#include "base.h"
#include "bt_server_dev.h"
#include "ringbuf.h"
#include "glbVariable.h"
#include "btApi.h"
#include "grap_api.h"
#include "Common_Dlg.h"

#define BT_COUNT_MAX 14
#define BT_COUNT_MIN 8

extern int hycoBt_main(void);

#define SAVE_WEIGHT_MAX		2
typedef struct _bt_weight_t
{
	U8 rd;
	U8 wr;
	
	U8 weight[SAVE_WEIGHT_MAX][8];//�ô�����ÿ�������������Ϊ8,��Ϊ�ⲿ����������editҲ����Ϊ8,�����Ҫ�Ķ��ô�,��ͬʱ���½���������edit
	
}tBT_WEIGHT;

tBT_WEIGHT gtBtWeight;

SEMAPHORE *pBtSendSem = NULL;

U32 btSendLastTick = 0;

#define BT_SPP_RX_BUF_SIZE (64)
static __align(4) U8 btSppRxBuffer[BT_SPP_RX_BUF_SIZE];
static tRING_BUF tBtSppRxRingBuf;


//��������ĺϷ��� ����+.+������    �ϸ���,***.**
/*
��һ���ַ�����Ϊ���ֻ�-��
�������.��
.�ź��������2������
*/
//1:�Ϸ�   0:���Ϸ�
U8 hyco_check_weight(U8 *pData)
{
	int i,len;
	U8  ret = 1;
	
	len = strlen(pData);
	
	//�жϵ�һ���ַ��Ƿ������ֻ�+-��
	if(pData[0] != '+' && pData[0] != '-' && (pData[0] < '0' || pData[0] > '9'))
	{
		return 0;
	}
	
	//�ж��Ƿ���.��,.�ź����Ƿ�ֻ��2������
	if(pData[len-3] != '.')
	{
		return 0;
	}
	if(pData[len-2] < '0' || pData[len-2] > '9' || pData[len-1] < '0' || pData[len-1] > '9')
	{
		return 0;
	}
	
	//�жϴӵڶ����ַ���.��֮���Ƿ�������
	for(i = 1; i < len-3; i++)
	{
		if(pData[i] < '0' || pData[i] > '9')//������
		{
			ret = 0;
			break;
		}
	}
	
	return ret;
}


void bt_wr_weight(U8 *pData)
{
	U8 i,wr,len;
	
	//ȥ���ո�
	len = strlen(pData);
	for(i = 0; i < len; i++)
	{
		if(pData[i] == ' ')
		{
			len -= 1;
			memmove(&pData[i], &pData[i+1], len-i);
			pData[len] = 0;
		}
	}
	
	if(1 == hyco_check_weight(pData))
	{
		wr = gtBtWeight.wr;
		strcpy(gtBtWeight.weight[wr], pData);
		wr++;
		if(wr >= SAVE_WEIGHT_MAX) wr = 0;
		if(wr != gtBtWeight.rd) gtBtWeight.wr = wr;
	}
}

void bt_init_show(void)
{
	memset(&gtBtWeight, 0, sizeof(tBT_WEIGHT));
	bt_wr_weight("0.00");
}

int bt_create_process(void)
{
	int pid, startValue, pidStatus = HY_ERROR;
	
	if (pidStatus != HY_OK)
	{
		pid = createp(hycoBt_main,
		                BT_NAME,
		                BT_SIZE,
		                BT_PRIOPITY,
		                NULL,NULL,NULL);
	}
	
	RingBuf_init(&tBtSppRxRingBuf, btSppRxBuffer, BT_SPP_RX_BUF_SIZE);
	bt_init_show();
	pBtSendSem = semaphore_init(0);
	startValue = pstart(pid, (void *)hycoBt_main);
	
	AppCtrl_CreatPro_Virtual(BT_DRIVER_AN, pid);
	return startValue;
}

int bt_close(void)
{
	int pid;
	
	hyco_btExit();
	
	pid = ntopid(BT_NAME);
	if (pid<0 || pid>30) return;
	
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	semaphore_destroy(pBtSendSem, OS_DEL_NO_PEND);
	pBtSendSem = NULL;
	deletep(pid);
	AppCtrl_DeletePro(BT_DRIVER_AN);
}

/*
��ϴ��: TDI-200A1  ----------�������ݣ��Ż�Ӧ��������
ҫ��: X3190A1+       ----------�������ݣ��Ż�Ӧ��������
*/
U8 scalesSendData[]=
{
	0x02, 0x41, 0x44, 0x00, 0x05, 0x03
};

/*
���С��: TH168-W5-------------��������Ӧ����
��ϴ��: TDI-200A1  ----------�������ݣ��Ż�Ӧ��������
ҫ��: X3190A1+       ----------�������ݣ��Ż�Ӧ��������
0.00	02 41 44 2b 30 30 30 30 30 30 32 31 39 03 
0.44	02 41 44 2b 30 30 30 30 34 34 32 31 39 03 
0.78	02 41 44 2b 30 30 30 30 37 38 32 31 36 03
2.30	02 41 44 2b 30 30 30 32 33 30 32 31 38 03 
259.36	02 41 44 2b 30 32 35 39 33 36 32 31 32 03
-0.02	02 41 44 2d 30 30 30 30 30 32 32 31 42 03

�ǽ�_ҫ�����ݸ�ʽ
259.36	02 2b 30 32 35 39 33 36 32 31 32 03

ҫ��: X3190A7       ----------��������Ӧ����
56.87	"=78.6500"
*/



//���յ�������
//���С��TH168-W5����ϴ��TDI-200A1��ҫ��X3190A1+
//���������ݽ���
void hyco_dataParse_scales(char *buf, int len)
{
	int i, j, realLen, needLen;
	char recvData[15];
	char tempData[15], data;
	int sign, dot;
	char frameFlag;
	char *pValidData;//��Ч����ָ�룬�������ſ�ʼ
	int weightFlag = 0;
	
//	hyUsbPrintf("bt app recv len=%d\r\n", len);
//	hyUsbMessageByLen(buf, len);
//	hyUsbMessage("\r\nbt recv end\r\n");
	
	bt_write_log(buf, len);
	
	RingBuf_Write(&tBtSppRxRingBuf, buf, len);
	
	frameFlag = 0;
	while(1)
	{
		realLen = RingBuf_PreRead(&tBtSppRxRingBuf, &recvData[0], BT_COUNT_MAX);
		if(realLen >= BT_COUNT_MIN)//���ʵ�ʶ�ȡ���������Ѿ����ڵ�����С����,����Խ����ж����ݸ�ʽ
		{
			if (realLen>=8 && recvData[0]=='=')
			{
				realLen = 8;
				//�ҵ�һ֡����---8�ֽ�֡
				frameFlag = 2;
				pValidData = (char *)&recvData[1];
			}
			else if (realLen>=12 && recvData[0]==0x02 && recvData[11]==0x03)
			{
				realLen = 12;
				//�ҵ�һ֡����---12�ֽ�֡
				frameFlag = 1;
				pValidData = (char *)&recvData[1];
			}
			else if (realLen>=14 && recvData[0]==0x02 && recvData[13]==0x03)
			{
				realLen = 14;
				//�ҵ�һ֡����---14�ֽ�֡
				frameFlag = 1;
				pValidData = (char *)&recvData[3];
			}
			else
			{
				//����8��12��14�ֽ�֡����ô����һ���ֽ�
				if (realLen >= BT_COUNT_MAX)
				{
						//�������len�Ѿ��������֡��len�������ĵ�һ���ֽ�-----gao 2012-08-22
					RingBuf_ConfirmRead(&tBtSppRxRingBuf, 1);
					continue;
				}
				else
				{
					//���ݲ��㣬�˳�ѭ��
					break;
				}
			}
		
		}
		else
		{
			//���ݲ��㣬�˳�ѭ��
			break;
		}
		
		if (frameFlag == 1)
		{
			//�ҵ�һ֡����
			sign = 0;
			if (pValidData[0] == '-')
			{
				sign = 1;//����
			}
			//�����ݲ���
			memcpy(tempData, &pValidData[1], 6);
			tempData[7] = 0;
			//����С����
			dot = 0x36-pValidData[7];//dot = 6-(pValidData[7]-0x30);
			if (dot>0 && dot<6)
			{
				//��С����
				data = tempData[dot];//�ݴ�
				tempData[dot] = '.';
				for (i=5; i>dot; i--)
				{
					tempData[i+1] = tempData[i];
				}
				tempData[i+1] = data;
			}
			else if (dot == 0)
			{
				//ȫ��ΪС��
				tempData[0] = '0';
				tempData[1] = '.';
				//�����ݲ���
				memcpy(&tempData[2], &pValidData[1], 6);
				tempData[8] = 0;
			}
			//ȥ������ǰ���'0'
			//0002.22--->2.22; 0032.00--->32.00
			for (i=0; i<6; i++)
			{
				if (tempData[i] != '0' ||
					tempData[i+1] == '.')
				{
					break;
				}
			}
			j = 0;
			if (sign == 1)
			{
				//����realLen��dot�ֲ�����
				realLen = strlen(tempData);
				for (dot=0; dot<realLen; dot++)
				{
					if (tempData[dot]!='0' && tempData[dot] != '.')
					{
						break;
					}
				}
				//���ֵΪ0�������'-'��
				if (dot != realLen)
				{
					recvData[j++] = '-';
				}
			}
			strcpy(&recvData[j], &tempData[i]);
			
			//д������
			bt_wr_weight(recvData);
			
			weightFlag = 1;
			//����6���ֽ�
			RingBuf_ConfirmRead(&tBtSppRxRingBuf, realLen);
			frameFlag = 0;
			continue;
		}
		else if (frameFlag == 2)
		{
			char ch;
			
			sign = 0;
			memset(tempData,0,8);
			memcpy(tempData,pValidData,7);
			for(i=0;i<7/2;i++)
			{
				ch = tempData[i];
				tempData[i] = tempData[6-i];
				tempData[6-i] = ch;
			}
			realLen = strlen(tempData);
			for (i=0; i<realLen; i++)//add
			{
				if (tempData[i] == ' ')
				{
					realLen -= 1;
					memmove(&tempData[i], &tempData[i+1], realLen-i);
					tempData[realLen] = 0;
				}
			}
			
			//pValidData = &tempData[0];
			//�ж��Ƿ�Ϊ����
			if (tempData[0] == '-')
			{
				sign = 1;//����
				//pValidData = &tempData[1];
			}
			//ȥ������ǰ���'0'
			//0002.22--->2.22; 0032.00--->32.00
			for (i=sign; i<6; i++)
			{
				if (tempData[i] != '0' ||
					tempData[i+1] == '.')
				{
					break;
				}
			}
			j = 0;
			if (sign == 1)
			{
				//����realLen��dot�ֲ�����
				realLen = strlen(tempData);
				for (dot=sign; dot<realLen; dot++)
				{
					if (tempData[dot]!='0' && tempData[dot] != '.')
					{
						break;
					}
				}
				//���ֵΪ0�������'-'��
				if (dot != realLen)
				{
					recvData[j++] = '-';
				}
			}
			strcpy(&recvData[j], &tempData[i]);
			
			//д������
			bt_wr_weight(recvData);
			
			weightFlag = 1;
			RingBuf_ConfirmRead(&tBtSppRxRingBuf, realLen);
			frameFlag = 0;
			continue;
		}
	}
	if (weightFlag)
	{
		//������ȡ�ɹ���postһ���ź���
		semaphore_post(pBtSendSem);
	}
}


void hyco_dataParse(char *buf, int len)
{
	if(0 == gtHyc.bt_devenable)
	{
		bt_wr_weight("0.00");
		return ;
	}
	
	if (1 == gtHyc.bt_devenable ||	//��ϴ��TDI-200A1
		2 == gtHyc.bt_devenable ||	//���С��TH168-W5
		3 == gtHyc.bt_devenable)	//ҫ��X3190A1+
	{
		hyco_dataParse_scales(buf, len);
	}
}

#define BT_SEND_TIMEOUT		(20)//200ms//	(50)//500ms//	
//���С�Ʋ���Ҫ��������
void hyco_sendDataBt(void)
{
	int ret, now;
	
	if(0 == gtHyc.bt_devenable) return ;
	
	now = rawtime(NULL);
	ret = semaphore_trywait(pBtSendSem);
	if (ret<0 && (now-btSendLastTick)<BT_SEND_TIMEOUT)
	{
		//û�н��յ���������û�г�ʱ(200ms)��ֱ�ӷ���
		return;
	}
	#if 0	//for test
	if (ret<0 && (now-btSendLastTick)>=BT_SEND_TIMEOUT)
	{
		extern U32 btRecvDataCnt;
		//��ʱ��û���ź�������������200ms
	//	hyUsbPrintf("recv timeOut!! T=%d\r\n", rawtime(NULL));
	}
	#endif
	btSendLastTick = now;
	
	if (1 == gtHyc.bt_devenable ||
		3 == gtHyc.bt_devenable)
	{
		int ret;
		//��ϴ��TDI-200A1��ҫ��X3190A1+����Ҫ���ݽ���
		cond_bt_wakeup();
		ret = hyco_btSendData(scalesSendData, sizeof(scalesSendData));
	//	hyUsbPrintf("send r=%d!! T=%d\r\n", ret, rawtime(NULL));
	}
}

void bt_get_show(U8 *data)
{
	if(gtBtWeight.wr != gtBtWeight.rd)
	{
		strcpy(data, gtBtWeight.weight[gtBtWeight.rd]);
		gtBtWeight.rd++;
		if(gtBtWeight.rd >= SAVE_WEIGHT_MAX)
			gtBtWeight.rd = 0;
	}
	
	hyco_sendDataBt();
	
	return;
}

//type 0:addr to str    1:str to addr
void BT_AddrToStr(tBdAddress *pAddr, U8 *pStr, U8 type)
{
	U8  val;
	int i,len,temp;
	
	if(type == 0)
	{
		sprintf(pStr, "%04x%02x%06x", pAddr->nap, pAddr->uap, pAddr->lap);
	}
	else
	{
		len = strlen(pStr);
		temp = 0;
		for(i = 0; i < 4; i++)
		{
			val = pStr[i];
			if(val >= '0' && val <= '9')
			{
				temp = temp*16+(val-'0');
			}
			else if(val >= 'a' && val <= 'z')
			{
				temp = temp*16+(val-'a'+10);
			}
			else if(val >= 'A' && val <= 'Z')
			{
				temp = temp*16+(val-'A'+10);
			}
		}
		pAddr->nap = temp;
		
		temp = 0;
		for(i = 4; i < 6; i++)
		{
			val = pStr[i];
			if(val >= '0' && val <= '9')
			{
				temp = temp*16+(val-'0');
			}
			else if(val >= 'a' && val <= 'z')
			{
				temp = temp*16+(val-'a'+10);
			}
			else if(val >= 'A' && val <= 'Z')
			{
				temp = temp*16+(val-'A'+10);
			}
		}
		pAddr->uap = temp;
		
		temp = 0;
		for(i = 6; i < 12; i++)
		{
			val = pStr[i];
			if(val >= '0' && val <= '9')
			{
				temp = temp*16+(val-'0');
			}
			else if(val >= 'a' && val <= 'z')
			{
				temp = temp*16+(val-'a'+10);
			}
			else if(val >= 'A' && val <= 'Z')
			{
				temp = temp*16+(val-'A'+10);
			}
		}
		pAddr->lap = temp;

		hyUsbPrintf("%s  nap = %x  uap = %x  lap = %x \r\n",pStr,pAddr->nap, pAddr->uap,pAddr->lap);
	}
}

/*---------------------------------------------------------------
*����: bt_connect_auto
*����: ֮ǰ�������ñ�������Զ�����
*����: ��
*����: 10 ���ӳɹ� 11 :��ʼ��δ��ɻ�ʧ��  12 :�������¼ 13 :����ʧ��  14 :������֤ʧ�� 15 :����δ����
---------------------------------------------------------------*/
int bt_connect_auto(void)
{
	int ret = 11 ,i;
	int retValue;
	U8 btret;
	tBdAddress btaddr;
	
	if(0 == gtHyc.bt_devenable) return 15 ;
	//1.��ʼ��״̬�ж�
	if(hyco_getBtInitState() == TRUE)
	{
		ret++ ;
		//2.��ʼ��OK ��֮ǰ�б�������Ӽ�¼�ͽ����Զ�����
		for(i = 0 ; i < 8 ; i++)
		{
			if(gtHyc.bt_addr2[i] != 0) break ;
		}
		if(i < 8)
		{
			BT_AddrToStr(&btaddr, gtHyc.bt_addr2, 1);
			//memcpy(&btaddr, gtHyc.bt_addr2, sizeof(tBdAddress));
			hyco_releaseSearchList();
			hyco_defConnectBdAddr(&btaddr);
			ConSume_UpdataAppCoef(BT_DRIVER_AN, 96);
			retValue = hyco_startConnecting(0);
			ret++ ;
			//3.���ӳɹ�
			if (retValue == TRUE)
			{
				//4.��Ҫ������֤
				if (hyco_getConnectState() == FALSE)
				{
					ret++;
					//5.����������֤
					
					//PhoneTrace4(0,"pwd:%s",gtHyc.bt_pwd);
					if(hyco_startPair(gtHyc.bt_pwd, strlen(gtHyc.bt_pwd)) == TRUE)
					{
						for(i = 0; i < 40; i++)
						{
							if(hyco_getConnectState() == TRUE)
							{
								break;
							}
							syssleep(5);
						}
						return 10 ; //SUCCESS
					}
				}
				else
				{
					return 10 ;//SUCCESS
				}
			}
			else
			{
				ConSume_UpdataAppCoef(BT_DRIVER_AN, 0);
			}
		}
		
	}
	return ret ;
}

extern S32 ComWidget_LabTimerISR(void *pView) ;
// 0 fail 1 success
int bt_dev_connect(tGRAPDESKBOX *pDesk)
{
	eDLG_STAT		kDlgState;
	int             err,ret = 0 ;
	
	cartoon_app_entry(bt_connect_auto,NULL);
	AppCtrl_CreatPro(KERNAL_APP_AN, 0);
	kDlgState = Com_CtnDlgDeskbox(pDesk,2,"�������������豸...",NULL,NULL,ComWidget_LabTimerISR,50,82000);
	AppCtrl_DeletePro(KERNAL_APP_AN);
	if(DLG_CONFIRM_EN == kDlgState) 
	{
		err = kernal_app_get_stat();
		if(10 == err)
		{
			ret = 1 ;
			Com_SpcDlgDeskbox("���ӳɹ�!",0,pDesk,1,NULL,NULL,100);
		}
		else if(11 == err)
		{
			Com_SpcDlgDeskbox("������ʼ��δ���!",6,pDesk,2,NULL,NULL,400);
		}
		else if(12 == err)
		{
			Com_SpcDlgDeskbox("�����豸δ����!",6,pDesk,2,NULL,NULL,400);
		}
		else if(13 == err)
		{
			Com_SpcDlgDeskbox("�豸����ʧ��!",6,pDesk,2,NULL,NULL,400);
		}
		else if(14 == err)
		{
			Com_SpcDlgDeskbox("������֤ʧ��!",6,pDesk,2,NULL,NULL,400);
		}
		else if(15 == err)
		{
			Com_SpcDlgDeskbox("������δ����!",6,pDesk,2,NULL,NULL,400);
		}
		
	}
	
	return  ret ;
}

// btindex : �����������豸index, δ0xff �ǶϿ�
int bt_connect_enable(int *btindex)
{
	int ret = TRUE ;
	
	if(0xff == *btindex)
	{
		hyco_startDebond();
		hyco_startDisconnect();
		ConSume_UpdataAppCoef(BT_DRIVER_AN, 0);
	}
	else
	{
		ConSume_UpdataAppCoef(BT_DRIVER_AN, 96);
		ret = hyco_startConnecting(*btindex) ;
	}
	
	return ret ;
}

int bt_connect_checkpwd(U8 *pwd)
{
	int ret = TRUE ;
	
	ret = hyco_startPair(pwd, strlen(pwd));
	
	return ret ;
}
void bt_disconnect(void *pDesk)
{
	int dev = 0xff;
	cartoon_app_entry(bt_connect_enable,&dev);
	AppCtrl_CreatPro(KERNAL_APP_AN, 0);
	Com_CtnDlgDeskbox(pDesk,2,"���ڶϿ������豸",NULL,NULL,ComWidget_LabTimerISR,50,8000);
	AppCtrl_DeletePro(KERNAL_APP_AN);
}


int BT_WeightEditTimer(void *pView)
{
	tGRAPVIEW 		*pFocus;
	tGRAPCOMBOBOX 	*pCom;
	tGRAPEDIT 		*pEdit;
	tGrapViewAttr	*pview_attr;
	tGrapEditPriv	*pedit_privattr;

	pEdit = (tGRAPEDIT *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	
	pFocus = Grap_GetCurFocus(pEdit, NULL);
	if(pFocus->view_attr.id > COMBOBOX_ID_BASE && pFocus->view_attr.id < PICTURE_ID_BASE)
	{
		pCom = (tGRAPCOMBOBOX *)pFocus;
		if(pCom->list_opened == 1)
		{
			return FAIL;
		}
	}
	
	if(1 == pview_attr->reserve)
	{
		if (hyco_getConnectState() != TRUE)
		{
			if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�����豸�ѶϿ�,�Ƿ���������?",6, pEdit->pParent, 3, NULL, NULL, 6000))
			{
				pview_attr->reserve = 0 ;
			}
			else
			{
				pview_attr->reserve = 2 ;
			}
			ReDraw(pEdit, 0, 0);					
		}
		else
		{
			bt_get_show(pEdit->edit_privattr.digt);
			pEdit->draw(pEdit, 0, 0);
		}
	}
	else if(0 == pview_attr->reserve) // ��¼����״̬ 0 ��Ҫ����, 1 ���ӳɹ�, 2����ʧ��
	{
		if((gtHyc.ParamCfg&0x01) == 0)//�ж��Ƿ������������,������Ǳ���,��������������
		{
			pview_attr->reserve = 2;
			//��������ʧ��,�޸�edit����
			pview_attr->FocusEnable = 1;
			pview_attr->color=0xffff;
			pedit_privattr->TimerTot = 50;
			
			ReDraw(pEdit, 0, 0);
			return SUCCESS;
		}
		if(1 == bt_dev_connect(pEdit->pParent))
		{
			pview_attr->reserve = 1;
		}
		else
		{
			pview_attr->reserve = 2;
			
			if((gtHyc.ParamCfg&0x01) == 0)
			{
				//��������ʧ��,�޸�edit����
				pview_attr->FocusEnable = 1;
				pview_attr->color=0xffff;
				pedit_privattr->TimerTot = 50;
			}
		}
		ReDraw(pEdit, 0, 0);
	}
	else if(2 == pview_attr->reserve)//��������ʧ��  �����ֹ���������
	{
		Grap_EditCursorShow(pEdit);
	}
	
	return SUCCESS;
}
