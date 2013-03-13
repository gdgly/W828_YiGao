#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "AbstractFileSystem.h"
#include "Op_Common.h"
#include "Op_DownLoad.h"
#include "Op_Struct.h"
#include "Op_RequestInfo.h"
#include "Op_BarRuler.h"




int Express_OnLine(void *p);


/*================== �����ѯ ============================*/
const tGrapViewAttr Express_InquireViewAttr[] =
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//����
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,  20,282,88,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//ȷ��
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
#else
	{BUTTON_ID_2,	   15,282,73,34,  20,282,88,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//ȷ��
#endif	
	{BUTTON_ID_3,  10,40,220,16,  0,0,0,0,   0,	0,	0,0,1,0,0,0},//������Ҫ��ѯ�ĵ���
	
	{EDIT_ID_1,	  30, 65,186,24,  30, 65,220,90, 0,0,0xffff,0,1,1,1,0},//���뵥��
	
	{EDIT_ID_2,   5, 100,230, 175,      0,  100,  240,  276,   0,	0,	0x0,0,1,0,0,0},//��ʾ���
};

const tGrapButtonPriv  Express_InquireBtnAttr[]=
{
#ifdef W818C
	{(U8 *)"ȷ��",0xffff,0xffff,0,0,2,KEYEVENT_OK,TY_UNITY_BG1_EN, NULL},//ȷ��
	{(U8 *)"����",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//����
#else
	{(U8 *)"����",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//����
	{(U8 *)"ȷ��",0xffff,0xffff,0,0,2,KEYEVENT_OK,TY_UNITY_BG1_EN, NULL},//ȷ��
#endif	
	{(U8 *)"������Ҫ��ѯ�ĵ���",0xffff,0xffff,0,0,1,0,TY_UNITY_BG1_EN, NULL},//
	
};

//edit
const tGrapEditPriv Express_InquireEditPri[] = 
{
	{NULL,0x0,0,0,2,4,22,  1,BARCODE_LEN,0,0,2,0,0,1,  50,50,0,  0,1,1,1},
};

//edit2
const tGrapEdit2Priv  Express_InquireEdit2Attr[]=
{
 	{NULL,	0xffff,0,0,0,TY_UNITY_BG1_EN,0,0,1024,2,2,0, 0},//��ʾ��ѯ��Ϣ
};


S32 Express_Inquire_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	tGRAPDESKBOX 	*pDesk;
	tGRAPEDIT 	 	*pEdit;
	tGRAPEDIT2 		*pEdit2;
	U8				*pSend, *pSub;
	eDLG_STAT		kDlgState;
	tCARTOONPARA	tUp;
	int ret,len;
	
	if (state != STATE_NORMAL) return SUCCESS ;
	
	pBtn  = (tGRAPBUTTON *)pView;
	pDesk = (tGRAPDESKBOX *)pBtn->pParent;
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://��ѯ
		pEdit  = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_1);
		pEdit2 = (tGRAPEDIT2 *)Grap_GetViewByID(pDesk, EDIT_ID_2);
		ret = OpCom_CheckBarcode(pEdit->edit_privattr.digt,TYPE_BARCODE);//���жϹ���
		if(ret == 1)
		{
			if(-1 == Net_ChangeEvt(pDesk, NET_DOWNDATA_EVT))
			{
				break;
			}
			
			pSend = (U8 *)hsaSdram_UpgradeBuf();
			tUp.pdata   = pSend;
			tUp.datalen = hsaSdram_UpgradeSize();
			
			//�Ѳ�ѯ���ŷŽ�ȥ
			strcpy(pSend, pEdit->edit_privattr.digt);
			
			cartoon_app_entry(Express_OnLine,&tUp);
			AppCtrl_CreatPro(KERNAL_APP_AN, 0);
			kDlgState = Com_CtnDlgDeskbox(pDesk,2,"���ڲ�ѯ,���Ժ�...",NULL,NULL,ComWidget_LabTimerISR,50,DLG_ALLWAYS_SHOW);
			AppCtrl_DeletePro(KERNAL_APP_AN);
			
			Net_DelEvt(NET_FOREGROUND_EVT);
			
			if(kDlgState == DLG_CONFIRM_EN)
			{
				ret = kernal_app_get_stat();
				if(ret == 1)//�ɹ�
				{
					strcpy(pEdit2->edit_privattr.digt, pSend);
					pEdit2->get_default_str(pEdit2);
				}
				else if(ret == 2)//��¼������
				{
					Com_SpcDlgDeskbox("��¼������!",6,pDesk,1,NULL,NULL,150);
				}
				else if(ret == 3)//��������ʧ��
				{
					Com_SpcDlgDeskbox("��������ʧ��!",6,pDesk,2,NULL,NULL,150);
				}
				else if(ret == 4)//���ݴ���
				{
					Com_SpcDlgDeskbox("���ݴ���!",6,pDesk,2,NULL,NULL,150);
				}
				else
				{
					OpCom_ErrorRemind(pDesk);
				}
			}
			else
			{
				Com_SpcDlgDeskbox("ͨѶ��ʱ!",6,pDesk,2,NULL,NULL,150);
			}
			break;
		}

		Com_SpcDlgDeskbox("�����ʽ����!",0,pDesk,1,NULL,NULL,150);		
		break;
	case BUTTON_ID_2://����
		return RETURN_QUIT;
		break;
	default:
		break;
	}
	
	return RETURN_REDRAW;
}

/*-------------------------------------------------
* ����:Express_InquireDesk
* ����:���߲�ѯ����״̬
* ����:
* ����:
---------------------------------------------------*/
int Express_InquireDesk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT2		*pEdit2;
	tGRAPEDIT		*pEdit;
	int				i,idx=0;
	
	pDesk = Grap_CreateDeskbox(&Express_InquireViewAttr[idx++], "�����ѯ");
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	//��ť
	for(i = 0; i < 3; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &Express_InquireViewAttr[idx++], &Express_InquireBtnAttr[i]);
	    if(NULL == pBtn ) return -1;
	    if(i < 2)
	    {
	    	pBtn->pressEnter = Express_Inquire_BtnEnter;
	    }
    }
	
	pEdit = Grap_InsertEdit(pDesk, &Express_InquireViewAttr[idx++], &Express_InquireEditPri[0]);
	pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
	if(NULL == pEdit ) return -1;
	
	//��ʾ��ѯ��Ϣ
	pEdit2 = Grap_InsertEdit2(pDesk, &Express_InquireViewAttr[idx++], &Express_InquireEdit2Attr[0],0);
	if(NULL == pEdit2 ) return -1;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int Express_OnLine(void *p)
{
	struct protocol_http_request tRequest;
	tCARTOONPARA 	*pUp;
	int				ret = 1;
	int				bufLen,recvLen, len;
	U8  			*pSend,*pGbk,*pHead;
	U8				*pS, *pE, code[32];
	
	if(Net_ConnectNetWork(NULL) != 1)
	{
		OpCom_SetLastErrCode(ERR_CONNECTFAIL);//��������ʧ��
		return 3;
	}

	pUp    = (tCARTOONPARA *)p;
	pSend  = pUp->pdata;
	bufLen = pUp->datalen;
	pHead  = pSend;
	
	strcpy(code,pSend);
	sprintf(pSend,"<req op=\"%s\"><h><user>%s</user><stat>%s</stat><dev>%s</dev><flag>pda</flag></h><data><o><num>%s</num></o></data></req>",
				"getKinfo",Login_GetUserId(), glbVar_GetCompanyId(), glbVar_GetMachineId(),code);
		              
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	tRequest.method         = HTTP_POST_METHOD;
    tRequest.user_agent     = "hyco";
    tRequest.content_length = strlen(pSend);
    tRequest.content_data   = pSend;
    tRequest.referer        = NULL;
    tRequest.content_type   = "text/xml; charset=utf-8";
    tRequest.range          = NULL;
    tRequest.connection		= 1;
	tRequest.gzip			= 1;
	
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
			
			pS = strstr(pSend,"true");
			if(pS != NULL)
			{
				pS = strstr(pSend,"<data>");
				if(pS != NULL)
				{
					pS += 6;
					pE = strstr(pS,"</data>");
					if(NULL != pE)
					{
						len = pE - pS;
						if(len < 10)
						{
							strcpy(pSend,"���浥��Ϣ");
						}
						else
						{
							memcpy(pSend,pS,len);
							pSend[len] = 0;
						}
					}
					else
					{
						strcpy(pSend,"���浥��Ϣ");
					}
				}
				else
				{
					strcpy(pSend,"���浥��Ϣ");
				}
			}
			else
			{
				strcpy(pSend,"���浥��Ϣ");
			}
		}
		else
		{
			OpCom_SetLastErrCode(ERR_DATAERROR);
			return 12;
		}
	}
	else
	{
		ret = 3;
	}
	
	return ret;
}
