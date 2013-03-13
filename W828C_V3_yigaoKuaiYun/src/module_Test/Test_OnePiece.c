#include "Test_MainList.h"



/*=============================================һƱһ������===================================================*/

const tGrapViewAttr TestOnePiece_ViewAttr[] =
{
	{COMM_DESKBOX_ID,    0,  0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//����
#ifdef W818C
	{BUTTON_ID_1,	     9,282, 73,34,     9,282, 82,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//ȷ��
	{BUTTON_ID_2,	    83,282, 73,34,    83,282,156,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//ɾ��
	{BUTTON_ID_3,	   158,282, 73,34,   158,282,231,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
#else	
	{BUTTON_ID_3,	     9,282, 73,34,     9,282, 82,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_2,	    83,282, 73,34,    83,282,156,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//ɾ��
	{BUTTON_ID_1,	   158,282, 73,34,   158,282,231,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//ȷ��
#endif	
	{BUTTON_ID_10,	   5,  35 ,55, 20,   0,0,0,0,   0,0,0,0,1,0,0,0},//��һվ
	{BUTTON_ID_11,	   5,  60 ,55, 20,   0,0,0,0,   0,0,0,0,1,0,0,0},//�˵���
	
	{EDIT_ID_1,	   60, 35,170,22,   60,35,235,57, 0,0,0xffff,0,1,1,1,0},//��һվ
	{EDIT_ID_2,	   60, 60,170,22,   60,60,235,82, 0,0,0xffff,0,1,0,1,0},//�˵���
};

const tGrapButtonPriv TestOnePiece_BtnPriv[]=
{
#ifdef W818C
	{"ȷ��",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//ȷ��
	{"ɾ��",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, 			TY_UNITY_BG1_EN,NULL},//ɾ��
	{"����",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//����
#else
	{"����",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//����
	{"ɾ��",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_F1, 	TY_UNITY_BG1_EN,NULL},//ɾ��
	{"ȷ��",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//ȷ��
#endif	
	{"��һվ",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"�˵���",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
};

const tGrapEditPriv TestOnePiece_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,20,  1, COMPANY_ID_LEN-1,  0,0,2,0,0,1,  50,50,0,  0,1,1,1},//��һվ
	{NULL,0x0,0,0,2,4,20,  1, BARCODE_LEN-1,		0,0,2,0,0,1,  50,50,0,  0,1,1,1},//�˵���
};


int TestOnePiece_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tDATA_INFO 		Recd;
	int ret;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
		if (gpEdit_Stat2==NULL || gpEdit_Code==NULL)
		{
			Com_SpcDlgDeskbox("�ؼ���Ϣ�쳣!",0,pDesk,1,NULL,NULL,150);
			return RETURN_REDRAW;
		}
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://ȷ��
			if(gpEdit_Stat2->view_attr.curFocus == 1)//վ��
			{
				if(strlen(gpEdit_Stat2->edit_privattr.digt) == 0)
				{
					Com_SpcDlgDeskbox("������վ��!",0,pDesk,1,NULL,NULL,150);
				}
				else if(OpCom_CheckBarcode(gpEdit_Stat2->edit_privattr.digt,TYPE_COMPANYID) == 0)//վ��������ȷ
				{
					strcpy(gtOpInfoGlb.station2, gpEdit_Stat2->edit_privattr.digt);
					ret = Company_GetNameById(gtOpInfoGlb.station2, gpEdit_Stat2->edit_privattr.digt, gpEdit_Stat2->edit_privattr.containLen);
					if(ret == 0)
					{
						gpEdit_Stat2->view_attr.curFocus = 0;
						gpEdit_Code->view_attr.curFocus = 1;
					}
					else
					{
						Com_SpcDlgDeskbox("û�ж�Ӧվ��!",0,pDesk,1,NULL,NULL,150);
					}
				}
				else
				{
					Com_SpcDlgDeskbox("վ����Ϣ����!",0,pDesk,1,NULL,NULL,150);
				}
			}
			else if(gpEdit_Code->view_attr.curFocus == 1)//�˵�
			{
				if(strlen(gtOpInfoGlb.station2) == 0)
				{
					Com_SpcDlgDeskbox("������վ���!",0,pDesk,1,NULL,NULL,150);
				}
				else if(strlen(gpEdit_Code->edit_privattr.digt) == 0)
				{
					Com_SpcDlgDeskbox("�������˵���!",0,pDesk,1,NULL,NULL,150);
				}
				else if(OpCom_CheckBarcode(gpEdit_Code->edit_privattr.digt,TYPE_BARCODE) == 0)
				{
					//��鵥���Ƿ����
					//if(0 != Op_ComCheckExist(pDesk, gpEdit_Code->edit_privattr.digt,OpCom_GetCurOpType()))
					{
						//����¼�Ĺ�������
						OpCom_FillCommInfo(&Recd, gpEdit_Code->edit_privattr.digt, NULL, RECORD_UNSEND);
						
						//ҵ�񲿷�
						strcpy(Recd.uData.testOp.Station, gtOpInfoGlb.station2);
						
						//д��¼
						ret = Rec_WriteNewRecode(&Recd, NULL);
						if (ret == 0)
						{
							Op_CommonListShow_Add(gpEdit_Code->edit_privattr.digt);
							memset(gpEdit_Code->edit_privattr.digt, 0, gpEdit_Code->edit_privattr.containLen);
						}
						else
						{
							Com_SpcDlgDeskbox("��¼����ʧ��!",0,pDesk,1,NULL,NULL,150);
						}
					}
				}
				else
				{
					Com_SpcDlgDeskbox("�˵��Ÿ�ʽ����!",0,pDesk,1,NULL,NULL,150);
				}
			}
			break;
		case BUTTON_ID_2://ɾ��
			Op_ComDelCode(pDesk, OpCom_GetCurOpType());
			break;
		case BUTTON_ID_3://����
			if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�Ƿ��˳��ò���?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
			{
				return RETURN_QUIT;
			}
			break;
		default:
			break;
		}
	}
	
	return RETURN_REDRAW;
}

/*-----------------------------------------------
*����:TestFun_OnePiece
*����:һƱһ��
*����:
*����:
*-----------------------------------------------*/
int TestFun_OnePiece(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	int				i,idx=0;
	
	//���ñ��β�������
	OpCom_SetCurOpType(HYCO_OP_TEST);
	
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&TestOnePiece_ViewAttr[idx++], NULL);
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_2;
	
	for (i=0; i<5; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &TestOnePiece_ViewAttr[idx++], &TestOnePiece_BtnPriv[i]);
		if(NULL == pBtn ) return;
		if(i < 3)
		{
			pBtn->pressEnter = TestOnePiece_BtnPress;
		}
	}
	
	for (i=0; i<2; i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &TestOnePiece_ViewAttr[idx++], &TestOnePiece_EditPriv[i]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(NULL == pEdit ) return;
		if(i == 0)//��һվ
		{
			gpEdit_Stat2 = pEdit;
			Company_GetNameById(gtOpInfoGlb.station2, pEdit->edit_privattr.digt, pEdit->edit_privattr.containLen);
		}
		else if(i == 1)//�˵���
		{
			gpEdit_Code = pEdit;
		}
	}
	
	Op_CommonInsertListShow(pDesk, 10, 115, 220, 160);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}


