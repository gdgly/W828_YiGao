/*
��̨�¼���������
*/
#include "grap_api.h"
#include "glbVariable.h"
#include "Net_Evt.h"
#include "Rec_Info.h"
#include "hy_unifi.h"
#include "Op_Config.h"


#ifdef VER_CONFIG_HYCO

#define	CHECK_COMPANY_TIME		(4*60*60*100)


U8	gu8IfUpdateCompany;	//�Ƿ���Ҫ���¹�˾   0:����Ҫ   1:��Ҫ   0xff:��Ҫ��������
U8	gu32LastUpdateCompany;//�����¹�˾��Ϣ��ʱ��
U8	gu8EvtWtVol_Abnormal;	//����������쳣���͸���   0:�������   1:ֻ�����������   2:ֻ�����쳣����   3:2�߶�����

U8 gu8BtErrorFlag;
U8 BtTmkdStr[50];

void Evt_VarInit()
{
	gu8IfUpdateCompany = 0;
	gu32LastUpdateCompany = 0xFFFFFFFF;
	
	gu8EvtWtVol_Abnormal = 0;
}

/*-----------------------------------------------
* ����:Evt_SetUpdateCompanyLast
* ����:�������һ�θ��¹�˾��Ϣ��ʱ��
* ����:
* ����:
------------------------------------------------*/
void Evt_SetUpdateCompanyLast(U32 tick)
{
	gu32LastUpdateCompany = tick;
}

/*-----------------------------------------------
* ����:Evt_GetUpdateCompanyFlag
* ����:��ȡ���¹�˾�¼�
* ����:
* ����:
------------------------------------------------*/
U8 Evt_GetUpdateCompanyFlag()
{
	return gu8IfUpdateCompany;
}

/*-----------------------------------------------
* ����:Evt_SetUpdateCompanyFlag
* ����:���ø��¹�˾�¼�
* ����:
* ����:
------------------------------------------------*/
void Evt_SetUpdateCompanyFlag(U8 flag)
{
	gu8IfUpdateCompany = flag;
}

/*-----------------------------------------------
* ����:Evt_GetEvtWtVol_AbnormalFlag
* ����:��ȡ����������쳣������״̬
* ����:
* ����:
------------------------------------------------*/
U8 Evt_GetEvtWtVol_AbnormalFlag()
{
	return gu8EvtWtVol_Abnormal;
}

/*-----------------------------------------------
* ����:Evt_SetEvtWtVol_AbnormalFlag
* ����:��������������쳣������״̬
* ����:
* ����:
------------------------------------------------*/
void Evt_SetEvtWtVol_AbnormalFlag(U8 flag)
{
	gu8EvtWtVol_Abnormal = flag;
}

/*-----------------------------------------------
* ����:Evt_Event_Remind
* ����:�¼�����
* ����:
* ����:
------------------------------------------------*/

/*-----------------------------------------------
* ����:Evt_Event_Remind
* ����:�¼�����
* ����:
* ����:
------------------------------------------------*/
void Evt_Event_Remind(tGRAPDESKBOX *pDesk)
{
	U8 type;
	
	if(GetUpDateState() == 2)//�ɹ�
	{
		SetUpDateState(0x82);//�Ѿ���ʾ��
		Com_SpcDlgDeskbox("ʱ��ͬ�����",0,pDesk,1,NULL,NULL,100);
		ReDraw(pDesk,0,0);
	}
	else if(GetUpDateState() == 3)//��ʾʧ��ԭ��
	{
		SetUpDateState(0x83);//�Ѿ���ʾ��
		Com_SpcDlgDeskbox(UpDateTime_GetFailReason(),6,pDesk,2,NULL,NULL,0xFFFFFFFF);
		ReDraw(pDesk,0,0);
	}
	
	if(gu8BtErrorFlag == 1)
	{
		gu8BtErrorFlag = 0;
		Com_SpcDlgDeskbox(BtTmkdStr,6,pDesk,1,NULL,NULL,200);
		ReDraw(pDesk,0,0);
	}
	
	//�Ƿ���Ҫ�������ع�˾��Ϣ
	if(Evt_GetUpdateCompanyFlag() == 0xff)
	{
		Evt_SetUpdateCompanyFlag(0);
		Com_SpcDlgDeskbox("��˾�б��нϴ�䶯,�뼰ʱ����!",6,pDesk,2,NULL,NULL,300);
		ReDraw(pDesk,0,0);
	}
}

/*----------------------------------------------------------
* ����: Evt_CloseSocket_Check
* ����: ����Ƿ���Ҫ�Ͽ�socket��ppp������   
* ����: ��
* ����: 
------------------------------------------------------------*/
void Evt_CloseSocket_Check()
{
	static U8 	count = 0;
	static U32	lastTime = 0;
	U32 now;
	
	now = rawtime(NULL);
	if(ABS(now - lastTime) >= 200)
	{
		if(gprs_get_status() == HY_OK && Net_GetTcpFlag() == 1)
		{
			if (gtHyc.NetSelected == 1)//ѡ�����gsm
			{
				//ppp������״̬,�� TCP�����˳�,��ʱ��Ҫ�ر�ppp
				Net_AddEvt(NET_CLOSEPPP_EVT);
			}
			count = 0;
		}
		
		if(gtHyc.NetSelected == 0)//ѡ�����wifi
		{
			if((Net_GetEvt()&~NET_CLOSESOCKET_EVT) || (STATE_ASSOCIATED != hy_unifi_getConnectState()))
			{
				count = 0;
			}
		}
		else if (gtHyc.NetSelected == 1)//ѡ�����gsm
		{
			if((Net_GetEvt()&~NET_CLOSESOCKET_EVT) || ((int *)ppp_get_eventppp() != NULL))
			{
				count = 0;
			}
		}
		
		if(count == 5)
		{
			if(hyIE_GetHttpSocket() != -1)
		    {
		    	Net_AddEvt(NET_CLOSESOCKET_EVT);
		    }
		    count++;
	    }
	    else if(count == 6)
	    {
	    	if (gtHyc.NetSelected == 1)//ѡ�����gsm
	    	{
	    		Net_AddEvt(NET_CLOSEPPP_EVT);
	    	}
		    count = 0;
	    }
	    else
	    {
	    	count++;
	    }
		
		lastTime = rawtime(NULL);
	}
}


/*-----------------------------------------------
* ����:Evt_GsmReset_Check
* ����:gsmģ���Ƿ���Ҫ�������
* ����:
* ����:
------------------------------------------------*/
void Evt_GsmReset_Check()
{
	if(Ie_get_connectfailcnt() >= 5 && GsmReset_IfReset() == 0) /* IE��������5������ʧ�� ����GSM */
	{
		if(gtHyc.NetSelected == 1)
		{
			GsmReset_WakeUp();
		}
		else if(gtHyc.NetSelected == 0)
		{
			Ie_clear_connectfailcnt();
		}
	}
	
	if(gsm_getmodestate() == 1 && GsmReset_IfReset() == 0)  /* GSM ����ӦAT�� ����GSM */
	{//GSMģ������쳣  ������������
		GsmReset_WakeUp();
	}
}

/*-----------------------------------------------
* ����:Evt_RecLoadSend_Check
* ����:��¼����,���ͼ��
* ����:
* ����:
------------------------------------------------*/
void Evt_RecLoadSend_Check()
{
	int		reg;
	U32		kQueCnt;
	
//hyUsbPrintf("back Net_GetEvt = 0x%x \r\n", Net_GetEvt());
	
	reg = Net_GetNetState();
		
	kQueCnt = Queue_GetNodeCnt();//�ж϶������Ƿ��м�¼
	if(reg == 1)//��ע��
	{
		if(!(Net_GetEvt()&NET_FOREGROUND_EVT))
		{//û��ǰ̨�¼�

			if(GetUpDateState() == 0)
			{
				Net_AddEvt(NET_UPDATE_TIME_EVT);//����Ϊ��Ҫͬ��ʱ��
			}
			
			if(Login_HasInfo() == 1/* && glbVar_GetSendType() == 0*/)//�е�¼��Ϣ  ��ѡ������Զ�����
			{
				if(0 == (Net_GetEvt()&NET_SENDMULTI_EVT))
				{
					if(Rec_GetMultiCnt() > 0 && Rec_GetMultiSendType() == 0)//��һƱ���Ҫ����  ���Ǻ�̨����
					{
						Net_AddEvt(NET_SENDMULTI_EVT);
					}
				}
				if(0 == (Net_GetEvt()&NET_SENDREC_EVT))//û���ϴ���¼�ı��
				{
				//hyUsbPrintf("send cnt = %d  %d  %d \r\n", gblVar_GetSendCnt(), gu32LastScanTime, gblVar_GetSendDelay());
					//if(kQueCnt >= gblVar_GetSendCnt() || 
					//  (kQueCnt > 0 && ABS(rawtime(NULL) - gu32LastScanTime) >= gblVar_GetSendDelay()))//�������м�¼δ�ϴ�
					if(kQueCnt > 0)
					{
						Net_AddEvt(NET_SENDREC_EVT);//�����ϴ����
					}
				}
				else//���ϴ���¼��־  ��Ҫ���ѽ���
				{
					Net_WakeUp();
				}
			}
		}
	}
	
	if(Rec_GetMultiCnt() > 0 && Queue_GetMultiCodeCnt() == 0)
	{
		//�����ϻ���һƱ����ļ�¼,���������Ѿ�������,��Ҫ����
		Net_AddEvt(NET_ADDMULTI_EVT);
	}
	
	if((Rec_GetUnSendImageCnt() > 0 || Rec_GetInNandCnt() > 0 || Rec_GetUnSendCnt() > Rec_GetMultiCnt()) && kQueCnt == 0)//��nand�ϻ���δ�ϴ���¼�Ҷ���������Ϊ��
	{
		Net_AddEvt(NET_ADDREC);
	}
}

/*-----------------------------------------------
* ����:Evt_UpdateCompany_Event
* ����:ִ�й�˾�б����
* ����:
* ����:
------------------------------------------------*/


/*-----------------------------------------------
* ����:Evt_UpdateCompany_Check
* ����:��鹫˾�б����
* ����:
* ����:
------------------------------------------------*/
void Evt_UpdateCompany_Check()
{

}


/*-----------------------------------------------
* ����:Evt_UpdateWtVol_Check
* ����:��������������
* ����:
* ����:
------------------------------------------------*/
void Evt_UpdateWtVol_Check()
{

}


int Evt_CloseHttpSocket()
{
	struct protocol_http_request tRequest;
	U8  *pSend;
	int bufLen,sock;
	
	sock = hyIE_GetHttpSocket();
	if(sock != -1)
	{
	//hyUsbPrintf("close socket === \r\n");
		protocol_http_close(sock);
		hyIE_resetSocket();
	}
	
	return 0;
}


int Evt_FillErrStr(U8 *pData)
{
	U8 *pSub;
	
	pSub = strstr(pData, "False");
	if(pSub != NULL)
	{
		pSub += 5;
		if(*pSub == '\t')
		{
			pSub++;
		}
		else if(*pSub == '\r')
		{
			pSub+=2;
		}
		UpDateTime_SetFailReason(pSub);
	}
	else
	{
		UpDateTime_SetFailReason("��������!");
	}
}

#endif //#ifdef VER_CONFIG_HYCO