#include "Test_MainList.h"

static U8  gu8ChangeFlag;
static U32 gu32ChangeCnt, gu32ChangeFail2G, gu32ChangeFail2W;
/*=============================================�����л����Խ���===================================================*/
const tGrapViewAttr TestNetChange_ViewAttr[] =
{
	{COMM_DESKBOX_ID,    0,  0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//����
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//��ʼ/ֹͣ
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
#else	
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//��ʼ/ֹͣ
#endif	
	{BUTTON_ID_3,	   10,  100 ,220, 16,   0,0,0,0,   0,0,0,0,1,0,0,0},//���л�����
	
	{LABEL_ID_1, 		0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},//��ʱ��
};

const tGrapButtonPriv TestNetChange_BtnPriv[]=
{
#ifdef W818C
	{"��ʼ",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//ȷ��
	{"����",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//����
#else
	{"����",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//����
	{"��ʼ",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//ȷ��
#endif
	{NULL,		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//���л�����
};

const tGrapLabelPriv TestNetChange_LabPri[]=
{
	{NULL,0,0,0,0,0,0,0},
};

int TestNetChange_BtnEnter(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk =  (tGRAPDESKBOX *)pBtn->pParent;
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://��ʼ/ֹͣ
			if(gu8ChangeFlag == 0)
			{
				pBtn->btn_privattr.pName = "ֹͣ";
				gu8ChangeFlag = 1;
			}
			else
			{
				pBtn->btn_privattr.pName = "��ʼ";
				gu8ChangeFlag = 0;
			}
			break;
		case BUTTON_ID_2://����
			return RETURN_QUIT;
			break;
		default:
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return RETURN_CANCLE;
}

S32 TestNetChange_BtnDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tStrDispInfo disp_info;
	tGrapButtonBuffPic *pbtnpic;
	U16 viewX1,viewY1,viewX2,viewY2;
	char buf[50];
		
	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
	
	viewX1 = pview_attr->viewX1;
	viewY1 = pview_attr->viewY1;
	viewX2 = viewX1 + pview_attr->viewWidth;
	viewY2 = viewY1 + pview_attr->viewHeight; 
	
	Grap_ClearScreen(viewX1, viewY1, viewX2,viewY2);
	
	sprintf(buf, "�л�%d��/ʧ��%d��(W-G)/%d(G-W)", gu32ChangeCnt,gu32ChangeFail2G, gu32ChangeFail2W);	
	
	disp_info.string = buf;

	disp_info.color = pbtn_privattr->cl_normal;
	disp_info.font_size = pbtn_privattr->font_size;
	disp_info.font_type = pbtn_privattr->font_type;
	disp_info.flag = pbtn_privattr->position;
	disp_info.keyStr = NULL;
	Grap_WriteString(viewX1, viewY1, viewX2,viewY2, &disp_info);
	
	return SUCCESS;
}

void TestNetChange_LabTimer(void *pView)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPLABEL		*pWifi, *pGsm;
	static	U32		lastTime=0,now;
	U8				lastSelect;
	eDLG_STAT		kDlgState;
	int				ret;
	
	pDesk = ((tGRAPLABEL*)pView)->pParent;
	
	if(gu8ChangeFlag == 0)
	{
		lastTime = rawtime(NULL);
		return ;
	}
	
	if(ABS(rawtime(NULL) - lastTime) < 3000)
	{
		return ;
	}
	
	if(gtHyc.NetSelected == 1)//gsm--wifi
	{
		gu32ChangeCnt++;
		
		lastSelect = gtHyc.NetSelected;
		gtHyc.NetSelected = 0xff;//ֵΪ�����л���
		
		pWifi = (tGRAPLABEL *)Grap_GetViewByIDFromComm(pgtComFirstChd,COM_WIFISIG_ID);
		pGsm  = (tGRAPLABEL *)Grap_GetViewByIDFromComm(pgtComFirstChd,COM_GSMSIG_ID);
		
		Com_SpcDlgDeskbox("�����л�����,���Ժ�...",0,pDesk,1,NULL,NULL,10);
	
		ret = ComWidget_ChangeNet_StopTrans(lastSelect, 0);
		if (ret == HY_ERROR)
		{
			Com_SpcDlgDeskbox("�����л�ʧ��,������!",0,pDesk,1,NULL,NULL,150);
			gu32ChangeFail2W++;
			gtHyc.NetSelected = lastSelect;
			return ;
		}
		
		ret = ComWidget_ChangeNet_GprsToWifi();
		if (ret == HY_ERROR)
		{
			Com_SpcDlgDeskbox("�����л�ʧ��,������!",0,pDesk,1,NULL,NULL,150);
			gu32ChangeFail2W++;
			gtHyc.NetSelected = lastSelect;
			return ;
		}
		
		pWifi->view_attr.enable = 1;
		pGsm->view_attr.enable  = 0;
		
		//���WIFI�Զ����Ӽ���
		WifiConnect_SetDelay(0);
		//����WIFI��̨��������
		WifiConnect_SetLock(0);
		
		gtHyc.NetSelected = 0;
		
		SetUpDateState(0);
	}
	else if(gtHyc.NetSelected == 0)//wifi
	{
		lastSelect = gtHyc.NetSelected;
		gtHyc.NetSelected = 0xff;//ֵΪ�����л���
		if(0 == ComWidget_ChangeNetLogo(pDesk, lastSelect, 1))
		{
			gtHyc.NetSelected = 1;
			SetUpDateState(0);
		}
		else
		{
			gtHyc.NetSelected = lastSelect;
			gu32ChangeFail2G++;
		}
		gu32ChangeCnt++;
	}
	ReDraw(pDesk, 0, 0);
	
	lastTime = rawtime(NULL);
}

/*-------------------------------------------------------------
* ����:TestFun_NetChange
* ����:�����л��Զ�����
* ����:
* ����:
--------------------------------------------------------------*/
int TestFun_NetChange(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPLABEL		*pLabel;
	int i, idx = 0;
	
	gu8ChangeFlag = 0;
	gu32ChangeCnt = 0;
	gu32ChangeFail2G = 0;
	gu32ChangeFail2W = 0;
	
	pDesk = Grap_CreateDeskbox(&TestNetChange_ViewAttr[idx++], NULL);
	if(!pDesk)	return -1;
	Grap_Inherit_Public((tGRAPDESKBOX *)pDeskFather,pDesk);
	
	for(i = 0; i < 3; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &TestNetChange_ViewAttr[idx++], &TestNetChange_BtnPriv[i]);
		if (NULL == pBtn) return -1;
		if(i < 2)
		{
			pBtn->pressEnter = TestNetChange_BtnEnter;
		}
		else if(i == 2)
		{
			pBtn->draw = TestNetChange_BtnDraw;
		}
	}
	
	pLabel = Grap_InsertLabel(pDesk, &TestNetChange_ViewAttr[idx++], &TestNetChange_LabPri[0]);
	if (NULL == pLabel) return -1;
	pLabel->labelTimerISR = TestNetChange_LabTimer;
	pLabel->label_timer.enable = 1;
	pLabel->label_timer.TimerLft = 100;
	pLabel->label_timer.TimerTot = 100;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
		
	return 0;
}


