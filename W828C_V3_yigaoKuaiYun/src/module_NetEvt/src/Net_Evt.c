#include "ospriority.h"
#include "hyhwCkc.h"
#include "std.h"
#include "sys.h"
#include "net_api.h"
#include "wmi.h"
#include "semaphore.h"
#include "glbVariable.h"
#include "Common_Dlg.h"
#include "Common_AppKernal.h"
#include "Net_Evt.h"
#include "Rec_Queue.h"
#include "Op_Config.h"
#include "grap_api.h"
#include "http.h"
#include "hy_unifi.h"







extern S32 ComWidget_LabTimerISR(void *pView);
/*===============================================================*/
#define NET_BACKEVT_NAME			"gprs_evt"
#define NET_BACKEVT_SIZE			4096
#define NET_BACKEVT_PRIOPITY		PRIORITY_6_OSPP


U32 		gu32NetEvt = 0;
U8			*gpNetSendBuf;//�����������ͨѶ  ���ͼ�¼

void 		*pNetThreadWait = NULL;
SEMAPHORE 	*pNetThread_ExitSem = NULL;



/*------------------------------------------------------------------*/
/*----------------------------------------
*����:Net_GsmReset
*����:��ʹ��GSM����ʱ,GSMģ����������򷵻�1
*����:none
*����:1:GSM������
*------------------------------------------*/
U8 Net_GsmReset()
{
	if(GsmReset_IfReset() == 1 && gtHyc.NetSelected == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*----------------------------------------
*����:Net_WakeUp
*����:���������̨�¼��������
*����:none
*����:
*------------------------------------------*/
void Net_WakeUp()
{
	if(pNetThreadWait != NULL)
	{
		ewakeup(&pNetThreadWait);
	}
}

/*----------------------------------------
*����:Net_ClearEvt
*����:��������¼�
*����:none
*����:none
*------------------------------------------*/
void Net_ClearEvt()
{
	gu32NetEvt = 0;
}

/*----------------------------------------
*����:Net_GetEvt
*����:��ȡ�����¼�
*����:none
*����:�����¼�
*------------------------------------------*/
U32 Net_GetEvt()
{
	return gu32NetEvt;
}

/*----------------------------------------
*����:Net_GetEvtStr
*����:��ȡ�����¼��ַ���
*����:direction:(out)��Ӧ�÷��������ͷ����   0:��  1:��
*����:�����¼��ַ���
*------------------------------------------*/
void Net_GetEvtStr(U8 *pDisBuf, U8 *direction)
{
	pDisBuf[0] = 0;
	if(pNetThreadWait) // ��̨���̽���wait״̬
	{
		return ;
	}
	if(gu32NetEvt&NET_LOGIN_EVT)//��¼
	{
		*direction = 1;
		strcpy(pDisBuf, "��¼");
	}
	else if(gu32NetEvt&NET_UPDATE_SOFT_EVT)//��������
	{
		*direction = 0;
		strcpy(pDisBuf, "��������");
	}
	else if(gu32NetEvt&NET_DOWNDATA_EVT)//��������
	{
		*direction = 0;
		strcpy(pDisBuf, "��������");
	}
	else if(gu32NetEvt&NET_UPDATE_TIME_EVT)//ʱ��ͬ��
	{
		*direction = 0;
		strcpy(pDisBuf, "ʱ��ͬ��");
	}
	else if( (gu32NetEvt&NET_SENDREC_EVT) || (gu32NetEvt&NET_SENDMULTI_EVT) )//�ϴ���¼
	{
		*direction = 1;
		strcpy(pDisBuf, "�ϴ���¼");
	}
	else if(gu32NetEvt&NET_UPDATE_DATA_EVT)//��������
	{
		*direction = 0;
		strcpy(pDisBuf, "��������");
	}
	else if(gu32NetEvt&NET_CLEARREC_EVT)//��ռ�¼
	{
		*direction = 0;
		strcpy(pDisBuf, "��ռ�¼");
	}
	
	return ;
}

/*----------------------------------------
*����:Net_AddEvt
*����:����ָ�����͵������¼�
*����:event�����ӵ��¼�
*����:none
*------------------------------------------*/
void Net_AddEvt(U32 event)
{
	gu32NetEvt |= event;//�����¼�
	
	//����������ӵ���ǰ̨�¼�,�Ȱ�֮ǰ��ǰ̨��־ȥ��
	if(event == NET_LOGIN_EVT ||
	   event == NET_UPDATE_SOFT_EVT ||
	   event == NET_DOWNDATA_EVT ||
	   event == NET_SENDMULTI_F_EVT)
	{
		gu32NetEvt &= ~NET_FOREGROUND_EVT;
	}
	
	//����������ӵĲ���ǰ̨��־λ ��֮ǰû��ǰ̨��־λ, ����gprs��̨����
	if(event != NET_FOREGROUND_EVT && !(gu32NetEvt&NET_FOREGROUND_EVT))
	{
		Net_WakeUp();
	}
}

/*----------------------------------------
*����:Net_AddSendEvt
*����:��Ӽ�¼�����¼�
*����:
*����:none
*------------------------------------------*/
void Net_AddSendEvt()
{
	gu32LastScanTime = 0x7fffffff;
	if(0 == (Net_GetEvt()&NET_SENDMULTI_EVT))
	{
		if(Rec_GetMultiCnt() > 0 && Rec_GetMultiSendType() == 0)//��һƱ���Ҫ����  ���Ǻ�̨����
		{
			Net_AddEvt(NET_SENDMULTI_EVT);
		}
	}
	if(Queue_GetNodeCnt() > 0 && 0 == (Net_GetEvt()&NET_SENDREC_EVT))//û���ϴ���¼�ı��
	{
		Net_AddEvt(NET_SENDREC_EVT);//�����ϴ����
	}
	else
	{
		Net_WakeUp();
	}
}

/*----------------------------------------
*����:Net_DelEvt
*����:ɾ��ָ�����͵������¼�
*����:event��ɾ�����¼�
*����:none
*------------------------------------------*/
void Net_DelEvt(U32 event)
{
	gu32NetEvt &= ~event;//ɾ��ָ���¼�
	
	//���ɾ������ǰ̨��־λ,���ѽ���
	if(event == NET_FOREGROUND_EVT)
	{
		Net_WakeUp();
	}
}


/*-----------------------------------------
* ����: Net_WaitEvt
* ����: �ȴ��л������¼�
* ����: 
* ����: 1:�ɹ�   -1:ʧ��
*-----------------------------------------*/
int Net_WaitEvt(void *p)
{
	tCARTOONPARA	*pUp;
	int ret = 1;
	U16 event;
	
	pUp = (tCARTOONPARA *)p;
	
	event = pUp->datalen;

	while(Net_GetEvt()&event)//�ȴ���̨���̴����¼�
	{
		Net_WakeUp();
		syssleep(10);
		if(Net_GsmReset() == 1)
		{
			ret = 2;
			break;
		}
	}
	
	return ret;
}

/*-----------------------------------------
* ����: Net_ChangeEvt
* ����: �л������¼�
* ����: 
* ����: 0:�ɹ�   -1:ʧ��
*-----------------------------------------*/
int Net_ChangeEvt(tGRAPDESKBOX *pDesk, U32 event)
{
	tCARTOONPARA	tUp;
	
	if(Net_GetEvt() != 0)//��������ʹ��
	{
		Net_AddEvt(event);//���������¼�

		tUp.datalen = event;
		tUp.pdata   = NULL;
		cartoon_app_entry(Net_WaitEvt,&tUp);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		Com_CtnDlgDeskbox(pDesk,2,"�豸��ռ��,���Ժ�...",NULL,NULL,ComWidget_LabTimerISR,50,DLG_ALLWAYS_SHOW);
		AppCtrl_DeletePro(KERNAL_APP_AN);

		if(kernal_app_get_stat() != 1)//ʧ��
		{
			Net_DelEvt(event);
			Com_SpcDlgDeskbox("��������ʧ��,���Ժ�����!",6,pDesk,2,NULL,NULL,500);
			return -1;
		}
	}
	else//����ʹ����,ֻ������Ϊ��ǰ̨�¼�����
	{
		Net_AddEvt(NET_FOREGROUND_EVT);
	}
	
	return 0;
}

/*----------------------------------------
*����:Net_IfSendMulti
*����:�Ƿ��ڷ���һƱ���
*����:none
*����:0:��  !=0 ��
*------------------------------------------*/
int Net_IfSendMulti()
{
	return (gu32NetEvt&NET_SENDMULTI_EVT);
}

/*----------------------------------------
*����:Net_StartGprs
*����:����gprs
*����:none
*����:1:�ɹ�
*------------------------------------------*/
int Net_StartGprs(void *p)
{
	int		ret = 1, count = 0;
	
	//gsm������ʱ��������
	if(GsmReset_IfReset() == 1)
	{
		return 11;
	}
	
	//������ʱ��������
	/*if(Battery_PermitRun() == 0)
	{
		return 11;
	}*/
	
	//�Ƴ�wifi����
	netdev_remove_wifi_dev();
	
	//�Ѿ�������״̬ʱ ��������
	if(NULL == (int *)ppp_get_eventppp() && gprs_get_status() == HY_OK)
	{
		return 1;
	}
	
	while(count++ < 2)
	{
		ret = gprs_start_connect();
		if(ret >= 0)
		{
			break;
		}
		syssleep(50);
	}
	
	if(ret >= 0 && ret != 2 && ret != 3)
	{
		ret = 1;
	}
	else
	{
		ret = -1;
	}
	
	return ret;
}

/*----------------------------------------------------------
*����: Net_ConnectNetWork
*����: ����ѡ��ʽ���Ӳ�ͬ����
*����: pDesk:��ǰ����
*����: 1:�ɹ�,-1:ʧ��
------------------------------------------------------------*/
int Net_ConnectNetWork(tGRAPDESKBOX *pDesk)
{
	int ret = -1;
	
	if(gtHyc.NetSelected == 0)//wifi
	{
		if(netdev_get_tcpip_status())
		{
			//wifi������
			ret = 1;
		}
	}
	else if(gtHyc.NetSelected == 1)//ʹ��gprs
	{
		if(1 == Net_StartGprs(NULL))
		{
			ret = 1;
		}
	}

	return ret;
}

/*----------------------------------------------------------
*����: Net_GetNetState
*����: �жϵ�ǰ����״̬
*����: 
*����: 
------------------------------------------------------------*/
U8 Net_GetNetState()
{
	U8 state = 0;
	
	//hyUsbPrintf("gtHyc.NetSelected = %d \r\n", gtHyc.NetSelected);
	
	if(gtHyc.NetSelected == 0)//wifi
	{
		if(netdev_get_tcpip_status())
		{
			state = 1;
		}
		//hyUsbPrintf("wifi state = %d \r\n", state);
	}
	else if(gtHyc.NetSelected == 1)//gsm
	{
		if(GsmReset_IfReset() == 0)
		{
			state = net_onlyRead_regStatus();
		}
		else
		{
			state = 0;
		}
		//hyUsbPrintf("gsm state = %d \r\n", state);
	}
	
	return state;
}


//�������ݳ���     <=0 ʧ��
int Net_DecodeData(U8 *pNetData, int len, U8 *pGbk)
{
	int ret = 0;
	int gzip=0;
	U8	*pGzip,*pUnic;
	
	if(http_GzipFlag() == 1)
	{
		while(1)
		{
			//0x1f 0x8b  ��gzipѹ�����ݵ�ͷ
			if(*pNetData == 0x1f && *(pNetData+1) == 0x8b)
			{
				break;
			}
			pNetData++;
			len--;
			if(len <= 0)
			{
			//hyUsbPrintf("gizp flag err!!\r\n");
				return 0;
			}
		}
		gzip   = 0;
		pGzip  = (U8 *)hsaSdramReusableMemoryPool();//����gzip��ѹ
		Gzip_Uncompress(pNetData,len,pGzip,&gzip,1);
		//hyUsbPrintf("gizp decode len = %d\r\n",gzip);
		pGzip[gzip] = 0;
		pGzip[gzip+1] = 0;
	}
	else
	{
		pGzip = pNetData;
		gzip  = len;
	}

	if(gzip > 0)
	{
		if(hyIE_GetEnType() == CHARSET_UTF8)
		{
			//hyUsbPrintf("%s \r\n",pGzip);
			pUnic = (U8 *)hsaSdramReusableMemoryPool()+0x100000;
			//utf-8 ת unicode
			DecodeUTF8String(CHARACTER_UNICODE,pGzip,pUnic,0x100000);
			ret = Hyc_Unicode2Gbk(pUnic, pGbk, 1);
		}
		else
		{
			memcpy(pGbk, pGzip, gzip);
			ret = gzip;
		}
		
		pGbk[ret] = 0;
		pGbk[ret+1] = 0;
		//hyUsbPrintf("ret = %d GBK = %s \r\n",ret, pGbk);
	}
	else
	{
		ret = 0;
	}
hyUsbPrintf("DecodeData ret__%d \r\n",ret);
	return ret;
}

/*----------------------------------------
*����:Net_SendMulti
*����:����һƱ���
*����:
*����:0:�ɹ�   ����:����
*------------------------------------------*/
int Net_SendMulti()
{
	int				ret = 1;
	tOP_CALLBACK	tOpCb;
	tMULTI_CODE	*pMulti;
	
	pMulti = Queue_GetMultiBuf();
	
	gpNetSendBuf = (U8 *)hsaSdram_RecSendBuf();
	
	memset((char *)&tOpCb, 0, sizeof(tOP_CALLBACK));
	tOpCb.right = 0;
	tOpCb.funType = 5;
	tOpCb.opType = pMulti->tData.tComm.opType;
	tOpCb.uOpcfg.tSend.pSend = gpNetSendBuf;
	tOpCb.uOpcfg.tSend.bufLen = hsaSdram_RecSendSize();
	tOpCb.uOpcfg.tSend.pNode  = (U8 *)pMulti;
	tOpCb.uOpcfg.tSend.cnt    = 1;
	tOpCb.uOpcfg.tSend.pUrl   = gtHyc.url;
	ret = Op_Config_CallBack(&tOpCb);
	
	if(0 == ret)
	{
		//���ͳɹ�,�ı�״̬   Ϊ  �Ѵ�
		RecStore_ChangeMultiCodeState(pMulti->tData.tComm.right, pMulti->tData.tComm.opType, pMulti->tData.tComm.code, RECORD_SEND);
		//��ձ�Ʊ��Ϣ
		Queue_ClearMulti();
		//Ʊ����1
		Rec_DelMultiCnt();
	}
	
	return ret;
}

/*----------------------------------------
*����:Net_SendRequest_F
*����:��ǰ̨��������ʹ��
*����:
*����:1:�ɹ�   >1:���� (2:����ʧ��   3:��������ʧ��   4:���յ��������ʹ���)
*------------------------------------------*/
int Net_SendRequest_F(void *p)
{
	tCARTOONPARA 	*pUp;
	int				ret = 1;
	int				bufLen,recvLen;
	U8  			*pSend,*pGbk;
	struct protocol_http_request tRequest;
	
	if(Net_ConnectNetWork(NULL) != 1)
	{
		return 3;
	}

	pUp    = (tCARTOONPARA *)p;
	pSend  = pUp->pdata;
	bufLen = pUp->datalen;
		
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	tRequest.method         = HTTP_POST_METHOD;
    tRequest.user_agent     = "hyco";
    tRequest.content_length = strlen(pSend);
    tRequest.content_data   = pSend;
    tRequest.referer        = NULL;
    tRequest.content_type   = "text/xml; charset=utf-8";
    //tRequest.content_type   = "multipart/form-data;charset=UTF8";
    tRequest.range          = NULL;
    tRequest.connection		= 1;
	tRequest.gzip			= 1;
	
	hyUsbPrintf("gtHyc.url = %s  \r\n send (front) = %s \r\n", gtHyc.url, pSend);
	recvLen = http_send_request(gtHyc.url, bufLen, &tRequest);
	if(recvLen > 0)
	{
		pGbk = (U8 *)hsaSdram_DecodeBuf();
		recvLen = Net_DecodeData(pSend,recvLen,pGbk);
		if(recvLen > 0)
		{
			memcpy(pSend,pGbk,recvLen);
			pSend[recvLen] = 0;
			pSend[recvLen+1] = 0;
			
			hyUsbPrintf("recv (front)\r\n");
			hyUsbMessageByLen(pSend, recvLen);
			hyUsbPrintf("\r\n");
		}
		else
		{
			ret = 4;
		}
	}
	else
	{
		ret = 3;
	}
	
	return ret;
}

/*----------------------------------------
*����:Net_SendRequest
*����:��������
*����:
*����:0:�ɹ�   >0:����
*------------------------------------------*/
int Net_SendRequest(tQUE_NODE *pNode, int cnt)
{
	int				ret = 1;
	tOP_CALLBACK	tOpCb;
	
	gpNetSendBuf = (U8 *)hsaSdram_RecSendBuf();
	
	memset((char *)&tOpCb, 0, sizeof(tOP_CALLBACK));
	tOpCb.right = 0;
	tOpCb.funType = 1;
	tOpCb.opType = pNode[0].opType;
	tOpCb.uOpcfg.tSend.pSend = gpNetSendBuf;
	tOpCb.uOpcfg.tSend.bufLen = hsaSdram_RecSendSize();
	tOpCb.uOpcfg.tSend.pNode  = (U8 *)pNode;
	tOpCb.uOpcfg.tSend.cnt    = cnt;
	tOpCb.uOpcfg.tSend.pUrl   = gtHyc.url;
	ret = Op_Config_CallBack(&tOpCb);
	
	return ret;
}

/*----------------------------------------
*����:Net_LoadFromNand
*����:��nand�ϼ���δ�ϴ��ļ�¼
*����:none
*����:
*------------------------------------------*/
void Net_LoadFromNand()
{
	if(Queue_GetNodeCnt() == 0)
	{
		Queue_Clear();
		RecStore_GetAllUnreported(Login_GetRight());
		if(Queue_GetNodeCnt() == 0)
		{
			gu32RecUnSendTotal = 0;
		}
	}
}

int Net_AutoConnect()
{
	int ret = 0;
	
	if(gtHyc.NetSelected == 0)
	{
		WifiConnect_BackConn();
	}
	else
	{
		if(phone_detect_simcard_nrt() != 1 || net_onlyRead_regStatus() != 1)//GSM û������
		{
		hyUsbPrintf("gsm not net \r\n");
			gu32NetEvt = 0;//�������¼����
			ret = -1;
		}
	}
	
	return ret;
}

/*----------------------------------------
*����:Net_ThreadMain
*����:�����̨�¼��������
*����:none
*����:
*------------------------------------------*/
int Net_ThreadMain(void)
{
	int 		ret,cnt;
	U32			intLevel;
	U32			kQueCnt, sendCnt;
	tQUE_NODE	*pNodes;
	tSTORE_CFG  tStoreCfg;
	tCARTOONPARA tUp;
	
	gu32NetEvt	= 0;
	pNetThreadWait = NULL;
	gu32RecUnSendTotal = 0;
	gu32RecInNandTotal = 0;
	
	gpNetSendBuf = (U8 *)hsaSdram_RecSendBuf();
	
	tStoreCfg.groupMax		= 50;//����buf������,��Ҫ����50,�����������ʱ,���Ӧ����hsaSdram_RecGroupBuf()��С
	tStoreCfg.delaySecond	= 28;
	tStoreCfg.pSmallRecBuf	= (U8 *)hsaSdram_SmallRecBuf();
	tStoreCfg.smallBufSize	= hsaSdram_SmallRecSize();
	tStoreCfg.pLargeRecBuf	= (U8 *)hsaSdram_LargeRecBuf();
	tStoreCfg.largeBufSize	= hsaSdram_LargeRecSize();
	tStoreCfg.pQueBuf		= (U8 *)hsaSdram_RecQueBuf();
	tStoreCfg.queBufSize	= hsaSdram_RecQueSize();
	tStoreCfg.pMultiCodeBuf	= (U8 *)hsaSdram_MultiRecBuf();
	tStoreCfg.multiBufSize	= hsaSdram_MultiRecSize();
	tStoreCfg.pExistBuf	    = (U8 *)hsaSdram_BarCompareBuf();
	tStoreCfg.existBufSize	= hsaSdram_BarCompareSize();
	if(-1 == Rec_InfoInit(&tStoreCfg))
	{
		return -1;
	}
	
	//�������м�¼
	RecStore_GetAllUnreported(Login_GetRight());
	if(Queue_GetNodeCnt() == 0)
	{
		gu32RecUnSendTotal = 0;
	}
	
	//���dns
	hyIE_clearDns();
	
	pNodes = (tQUE_NODE *)hsaSdram_RecGroupBuf();//������ϴ�ʱʹ��
	
	pNetThread_ExitSem = semaphore_init(0);
	
	while(1)
	{
		ret = semaphore_trywait(pNetThread_ExitSem);
		if (ret == 0)
		{
			//��Ӧ�˳�Ҫ���˳�����
			break;
		}
		
		if(gu32NetEvt&NET_STOP)//ֹͣ��̨�ϴ�����
		{
		hyUsbPrintf("stop upload proc \r\n");
			gu32NetEvt = NET_STOP;//���������־,ֻ���� ֹͣ��־
		}
	hyUsbPrintf("wait -----gu32NetEvt = %d \r\n",gu32NetEvt);
		if( Net_GetNetState() == 0 ||	//û������
		   (gu32NetEvt&NET_STOP) != 0 || //Ҫ��ֹͣ��̨����
		   (gu32NetEvt&NET_EVENT_MASK) == 0 ||	//û���κ��¼�
		   (gu32NetEvt&NET_FOREGROUND_EVT) != 0	//��ʱ��ǰ̨��ʹ��GPRS,  ��̨��Ҫ����wait״̬
		   )
		{
			ConSume_UpdataAppCoef(UPLOAD_APP_AN, 0);
			
			intLevel = setil(_ioff);
			(void) ewait(&pNetThreadWait);
			(void) setil(intLevel);
			
			ConSume_UpdataAppCoef(UPLOAD_APP_AN, 120);
		}
	hyUsbPrintf("wakeup cur gu32NetEvt = %x \r\n",gu32NetEvt);	
		ret = semaphore_trywait(pNetThread_ExitSem);
		if (ret == 0)
		{
			//��Ӧ�˳�Ҫ���˳�����
			break;
		}

		//�����ȼ����GPRS�¼�
		if(gu32NetEvt&NET_STOP)//ֹͣ��̨�ϴ�����
		{
		hyUsbPrintf("stop upload proc \r\n");
			gu32NetEvt = NET_STOP;//���������־,ֻ���� ֹͣ��־
		}
		else if(gu32NetEvt&NET_CLOSEPPP_EVT)//�ر�ppp
		{
			ppp_close(); 
	    	ppp_clear_brg_flag();
	    	Net_DelEvt(NET_CLOSESOCKET_EVT);
	    	Net_DelEvt(NET_CLOSEPPP_EVT);
		}
		else if(gu32NetEvt&NET_CLOSESOCKET_EVT)//�ر�socket
		{
			Evt_CloseHttpSocket();
	    	Net_DelEvt(NET_CLOSESOCKET_EVT);
		}
		else if(gu32NetEvt&NET_LOGIN_EVT)//��¼
		{
			if(0 != Net_AutoConnect())
			{
				continue;
			}
			
			Net_AddEvt(NET_FOREGROUND_EVT);
			Net_DelEvt(NET_LOGIN_EVT);
		}
		else if(gu32NetEvt&NET_UPDATE_SOFT_EVT)//��������
		{
			if(0 != Net_AutoConnect())
			{
				continue;
			}
			Net_AddEvt(NET_FOREGROUND_EVT);
			Net_DelEvt(NET_UPDATE_SOFT_EVT);
		}
		else if(gu32NetEvt&NET_DOWNDATA_EVT)//��������
		{
			if(0 != Net_AutoConnect())
			{
				continue;
			}
			Net_AddEvt(NET_FOREGROUND_EVT);
			Net_DelEvt(NET_DOWNDATA_EVT);
		}
		else if(gu32NetEvt&NET_ADDREC)//��nand���ؼ�¼
		{
			Net_LoadFromNand();
			Net_DelEvt(NET_ADDREC);
		}
		else if(gu32NetEvt&NET_UPDATE_TIME_EVT)//ʱ��ͬ��
		{
			if(0 != Net_AutoConnect())
			{
				continue;
			}
			
			/*if(Battery_PermitRun() == 0)
			{//��������,����ʹ������!
				
			}
			else*/
			{
				ret = Net_ConnectNetWork(NULL);//����GPRS
				if(ret == 1)
				{
					SetUpDateState(1);//����Ϊ����ͬ��
					ret = SetSyn_NetTime();//2:�ɹ�   3:ʧ��(��Ҫ��ʾʧ��ԭ��)   0:����ԭ��ʧ��
				hyUsbPrintf("SetSyn_NetTime   ret  = %d \r\n", ret);
					if(ret == 2 || ret == 3)
					{
						SetUpDateState(ret);
						syssleep(50);
					}
					else
					{
						SetUpDateState(0);//���ͬ������ԭ��ʧ��,���޸�Ϊδͬ��
					}
				}
			}
			Net_DelEvt(NET_UPDATE_TIME_EVT);
		}
		else if(gu32NetEvt&NET_UPDATE_DATA_EVT)//��������
		{
			U8 flag;
			
			if(0 != Net_AutoConnect())
			{
				continue;
			}
			
			/*if(Battery_PermitRun() == 0)
			{//��������,����ʹ������!
				
			}
			else*/
			{
				ret = Net_ConnectNetWork(NULL);//����GPRS
				if(ret == 1)
				{
					tUp.pdata   = (U8 *)hsaSdram_UpgradeBuf();
					tUp.datalen = hsaSdram_UpgradeSize();
					
					//�Ƿ���Ҫ���¹�˾��Ϣ
					if(Evt_GetUpdateCompanyFlag() == 1)
					{
						if(DownLoad_Company_GetTotal() == 0)//��Ҫ�������ع�˾��Ϣ
						{
							Evt_SetUpdateCompanyFlag(0xff);
						}
						else
						{
							//���¹�˾��Ϣ
							if(1 == DownLoad_DownCompany(&tUp))
							{//��Ҫȫ�����¹�˾�б�
								Evt_SetUpdateCompanyFlag(0xff);
							}
							else
							{
								Evt_SetUpdateCompanyFlag(0);
							}
						}
					}
					
					//�Ƿ���Ҫ��������������쳣��
					flag = Evt_GetEvtWtVol_AbnormalFlag();
					Evt_SetEvtWtVol_AbnormalFlag(0);
					ret = 0x80;
					if(flag&0x01)//�����������
					{
						if(1 == DownLoad_DownWeiVol(&tUp))
						{
							//�ɹ�
							ret |= 0x01;
						}
					}
					if(flag&0x2)//�����쳣��
					{
						if(1 == DownLoad_DownAbnormal(&tUp))
						{
							//�ɹ�
							ret |= 0x02;
						}
					}
					
					Evt_SetEvtWtVol_AbnormalFlag(ret);
				}
				
				//������汾
				if(Comm_GetUpFlag() == 0)
				{
					ret = Net_ConnectNetWork(NULL);
					if(ret == 1)
					{
						Common_NetUpgrade(1, NULL);
					}
				}
			}
			Net_DelEvt(NET_UPDATE_DATA_EVT);
		}
		else if(gu32NetEvt&NET_CLEARREC_EVT)//��ռ�¼
		{
			gu32RecUnSendTotal = 0;
			gu32RecUnSendImage = 0;
			gu32RecInNandTotal = 0;
			gu32MultiInNand    = 0;
			Net_DelEvt(NET_SENDREC_EVT);//ɾ�����ͼ�¼�¼�
			
			RecStore_Clear();//���nand�м�¼
			PicStore_Format();//ͼƬ
			Queue_Clear();//��ն�����Ϣ
			Queue_ClearMulti();
			Net_DelEvt(NET_CLEARREC_EVT);//ɾ����ռ�¼�¼�
			hyUsbPrintf("gu32RecUnSendTotal = %d  gu32RecUnSendImage = %d  gu32RecInNandTotal = %d \r\n", 
					gu32RecUnSendTotal, gu32RecUnSendImage, gu32RecInNandTotal);
		}
		else if(gu32NetEvt&NET_ADDMULTI_EVT)//����һƱ���
		{
			RecStore_GetUnreportedMulti(Login_GetRight(), 0);//��̨����
			
			Net_DelEvt(NET_ADDMULTI_EVT);
		}
		else if(gu32NetEvt&NET_SENDMULTI_EVT)//����һƱ���
		{
			if(0 != Net_AutoConnect())
			{
				continue;
			}
			
			Net_SendMulti();
			
			Net_DelEvt(NET_SENDMULTI_EVT);
		}
		else if(gu32NetEvt&NET_SENDREC_EVT)//���ͼ�¼
		{
			if(0 != Net_AutoConnect())
			{
				continue;
			}
			
			if(Login_HasInfo() == 0)//û�е�¼��Ϣ
			{
				Net_DelEvt(NET_SENDREC_EVT);
				continue;
			}
			//�жϵ���
			/*if(Battery_PermitRun() == 0)
			{
				syssleep(500);
			}
			else*/
			{
				kQueCnt = Queue_GetNodeCnt();
				if(kQueCnt == 0)//û�м�¼��Ҫ�ϴ�
				{
					Net_DelEvt(NET_SENDREC_EVT);
					continue;
				}
				
				//if(/*glbVar_GetSendType() == 1 || */kQueCnt >= gblVar_GetSendCnt() || 
				//   ABS(rawtime(NULL) - gu32LastScanTime) >= gblVar_GetSendDelay())	//���������ӳ�ʱ��û���ٴ�ɨ��
				
				//if(kQueCnt > 5 || ABS(rawtime(NULL) - gu32LastScanTime) >= gblVar_GetSendDelay())
				{
					gu32LastScanTime = 0x7fffffff;
					ret = Net_ConnectNetWork(NULL);//������·
					if(ret == 1)//���ӳɹ�
					{
						/*
						if(gtHyc.NetSelected == 0)//wifi
						{
							sendCnt = (kQueCnt >= Rec_GetGroupMax())? Rec_GetGroupMax():kQueCnt;
						}
						else if(gtHyc.NetSelected == 1)//gsm
						{
							sendCnt = (kQueCnt >= (Rec_GetGroupMax()/2))? (Rec_GetGroupMax()/2):kQueCnt;
						}
						else
						{
							syssleep(100);
							continue;
						}
						
						if(sendCnt == 0) sendCnt = 1;
						*/
						sendCnt = 1;
						cnt = Queue_GetNodesDate(pNodes, sendCnt);
						hyUsbPrintf("  cnt = %d que cnt = %d  send cnt = %d  cnt = %d\r\n",cnt,kQueCnt, sendCnt, cnt);
						
						//cnt = 2;
						if(cnt > 0)
						{
							ret = Net_SendRequest(pNodes, cnt);//��������
							if(ret == -1)//����ͨѶ����
							{
								syssleep(50);
							}
							else if(ret == 0)//�ɹ�
							{
								Queue_DelNodes(1,cnt);
								syssleep(20);
							}
							else//������������������
							{
								Queue_DelNodes(0,1);//�ȴӶ�����ɾ��һ��
								syssleep(20);
							}
						}
						else//û�дӶ�����ȡ������
						{
							syssleep(100);
						}
						
						if(cnt >= kQueCnt)//�����е�����ȫ�����س���, ���ܴ�ʱ������ɨ���
						{
							hyUsbPrintf("sendcnt(%d) > quecnt(%d) claer \r\n",cnt,kQueCnt);
							Net_DelEvt(NET_SENDREC_EVT);
						}
					}
					else//gprs����ʧ��
					{
						syssleep(100);
					}
				}
				/*else
				{
					Net_DelEvt(NET_SENDREC_EVT);
				}*/
			}
		}
	}
	
	semaphore_destroy(pNetThread_ExitSem, OS_DEL_NO_PEND);
	pNetThread_ExitSem = NULL;
	pNetThreadWait = NULL;
	exit(1);
	return 1;	
}

/*----------------------------------------
*����:Net_BackEvt_CreateThread
*����:���������̨�¼��������
*����:none
*����:none
*------------------------------------------*/
int Net_BackEvt_CreateThread()
{
	int pid, startValue, pidStatus = HY_ERROR;
	
	pid = ntopid(NET_BACKEVT_NAME);
	if (pid>0 && pid<30)
	{
		//���̴��ڣ��ж�״̬
		if (PSPROC(pid)->state == P_INACT)
		{
			pidStatus = HY_OK;//���̴����Ҵ���exit״̬����ֱ������
		}
		else
		{
			return HY_ERROR;//���̻������У�ֱ���˳�����ǰ������Ч
		}
	}
	
	if (pidStatus != HY_OK)
	{
		pid = createp(	Net_ThreadMain,
		                NET_BACKEVT_NAME,
		                NET_BACKEVT_SIZE,
		                NET_BACKEVT_PRIOPITY,
		                NULL,NULL,NULL);
	}
	
	startValue = pstart(pid, (void *)Net_ThreadMain);
	AppCtrl_CreatPro_Virtual(UPLOAD_APP_AN, pid);
}

/*----------------------------------------
*����:Net_BackEvt_ExitThread
*����:���������̨�¼��������
*����:none
*����:none
*------------------------------------------*/
void Net_BackEvt_ExitThread(void)
{
	int pid;
	
	pid = ntopid(NET_BACKEVT_NAME);
	if (pid<0 || pid>30) return;
	
	if(pNetThread_ExitSem != NULL)
		semaphore_post(pNetThread_ExitSem);
	
	while(PSPROC(pid)->state != P_INACT)
	{
		if(pNetThreadWait)
			ewakeup(&pNetThreadWait);
	    
	    syssleep(10);
	}
	
	deletep(pid);
	AppCtrl_DeletePro(UPLOAD_APP_AN);
}