#include "SysSetting.h"



/*�ؼ�*/
const tGrapViewAttr Set_AutoSendAtrr[]=
{
	{COMM_DESKBOX_ID,   0,  0, 240,320, 0,0,0,0, 		TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},
#ifdef W818C
	{BUTTON_ID_1,	    15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
	{BUTTON_ID_2,	    152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/	
#else
	{BUTTON_ID_2,	    15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
	{BUTTON_ID_1,	    152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/	
#endif		
	{BUTTON_ID_3,	  8,  40,96,16,   0,0,0,0,   0,  0,0,0,0,0,0,0},//�����ϴ���ʽ
	{BUTTON_ID_4,	  8,  70,140,22,   0,0,0,0,   0,  0,0,0,1,0,0,0},
	{BUTTON_ID_5,	  188,70 ,30,22,   0,0,0,0,   0,  0,0,0,1,0,0,0},
	{BUTTON_ID_6,	  8, 110 ,140,22,   0,0,0,0,   0,  0,0,0,1,0,0,0},
	{BUTTON_ID_7,	  188,110,30, 22,   0,0,0,0,   0,  0,0,0,1,0,0,0},
	
	{EDIT_ID_1,	148,70, 38,21,   148,70,186,91,    	0,0,0xffff,0,1,1,1,0},/*�Զ��ϴ�������ֵ�ı���*/
	{EDIT_ID_2,	148,110, 38,21,   148,110,186,131, 		0,0,0xffff,0,1,0,1,0},/*�ϴ�ʱ�������ı���*/
	
	{EDIT_ID_3,   10, 135,210, 145,      10,  135,  220,  280,   0,	0,	0,0,1,0,0,0},
	
	{COMBOBOX_ID_1,	   110, 38,100,22,   110,38,210,60, 0,0,0xffff,0,0,0,0,0},//�����ϴ���ʽ
};

/*btn ����*/
const tGrapButtonPriv Set_AutoSendBtnAttr[]=
{
#ifdef W818C
   	{(U8 *)"ȷ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  	TY_UNITY_BG1_EN,NULL},
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,	TY_UNITY_BG1_EN,NULL},
#else
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,	TY_UNITY_BG1_EN,NULL},
   	{(U8 *)"ȷ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  	TY_UNITY_BG1_EN,NULL},
#endif
	{(U8 *)"�����ϴ���ʽ",0xffff,0xffff,FONTSIZE8X16,0,2,0,				TY_UNITY_BG1_EN,NULL},	
	{(U8 *)"������ֵ(20-200)", 0xffff,0x0,FONTSIZE8X16,0,1,0, 	TY_UNITY_BG1_EN,NULL},
	{(U8 *)"��",0xffff,0xffff,FONTSIZE8X16,0,2,0,				TY_UNITY_BG1_EN,NULL},	
	{(U8 *)"�ӳ�ʱ��(1-30)", 0xffff,0xffff,FONTSIZE8X16,0,1,0,TY_UNITY_BG1_EN,NULL},
	{(U8 *)"��",0xffff,0xffff,FONTSIZE8X16,0,2,0,        		TY_UNITY_BG1_EN,NULL},
};

/*edt ����*/
const tGrapEditPriv	Set_AutoSendEditAttr []=
{
	{NULL,COLOR_STR_BARSELECT,0,0,2,4,4,  1,4,0,0,1,0,0,1,   50,50,0,  0,0,1,0},
	{NULL,COLOR_STR_BARSELECT,0,0,2,4,2,  1,2,0,0,1,0,0,1,   50,50,0,  0,0,1,0},
};

const tGrapEdit2Priv  Set_AutoSendEdit2Attr[]=
{
 	{NULL,	COLOR_YELLOW,0,0,0,TY_UNITY_BG1_EN,0,0,256,1,1,0,0},//��ʾ
};

const tComboboxPriv Set_AutoSendComAttr[]=
{
	{0,2,0,0,0,0,    1,80,10,90,11,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,YD_HD_S_EN,YD_HD_X_EN,YD_HD_Z_EN},//���䷽ʽ
};

/*��ť�¼�*/
static S32 Set_AutoSend_BtnEnter(void *pView,U16 state)
{
	tGRAPBUTTON  *pBtn ;
	tGRAPDESKBOX *pDesk;
	tGRAPEDIT    *pEdit;
	tGRAPCOMBOBOX	*pComBox;
	U32  id;
	S32  rv = SUCCESS;
	U32  edtTime,edtNum;

	pBtn=(tGRAPBUTTON*)pView;
	pDesk=(tGRAPDESKBOX*)pBtn->pParent;
	id=pBtn->view_attr.id;
	if (state==STATE_NORMAL)
	{
		switch (id)
		{
		case BUTTON_ID_1:
			pComBox = (tGRAPCOMBOBOX *)Grap_GetViewByID(pDesk,COMBOBOX_ID_1);
			pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_1);
			edtNum= atoi((char *)pEdit->edit_privattr.digt);			
			if(edtNum < 20 || edtNum > 200)
			{
				Com_SpcDlgDeskbox("������ֵ������Χ(20-200)!",6,pDesk,2,NULL,NULL,200);	
				return RETURN_REDRAW;
			}
			
			pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_2);
			
			edtTime = atoi((char *)pEdit->edit_privattr.digt);			
			if(edtTime < 1 || edtTime > 30)
			{
				Com_SpcDlgDeskbox("�ӳ�ʱ�䳬�˷�Χ(1-30)!",6,pDesk,2,NULL,NULL,200);	
				return RETURN_REDRAW;
			}
			
			gtHyc.sendType     = pComBox->comboPriv.cur_index;
			gtHyc.autoSendNum  = edtNum;
			gtHyc.autoSendTime = edtTime;			
			
			glbVariable_SaveParameter();
			
			Com_SpcDlgDeskbox("���óɹ�!",0,pDesk,1,NULL,NULL,100);		
			rv = RETURN_QUIT;
			break;
		case BUTTON_ID_2:
			rv = RETURN_QUIT;
			break;
		default:
			break;
		}
	}
	return rv;
}


S32 Set_AutoSendComboboxGetStr(void *pView,U16 i, U8 *pStr)
{
	if(i == 0)
	{
		strcpy(pStr, "�Զ�");
	}
	else
	{
	#ifdef W818C
		strcpy(pStr, "�ֶ�-��F2");
	#else
		strcpy(pStr, "�ֶ�");
	#endif
	}
	
	return SUCCESS ;
}

S32 Set_AutoSendComboxEnter(void *pView, U16 i)
{
	tGRAPCOMBOBOX	*pCombox;
	tGRAPEDIT  	    *pEdit;
	
	pCombox = (tGRAPCOMBOBOX *)pView;
	
	if(pCombox->list_opened == 1)
	{
		pCombox->view_attr.curFocus = 0;
		pEdit = (tGRAPEDIT *)Grap_GetViewByID(pCombox,EDIT_ID_1);
		pEdit->view_attr.curFocus = 1;
	}
	
	return SUCCESS;
}

/*---------------------------------------------
*����:Set_AutoSend
*����:�Զ�����
*����:
*����:
*-----------------------------------------------*/
void Set_AutoSend(void *pDeskFather)
{
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON *pBtn;
	tGRAPEDIT2	*pEdit2;
	tGRAPEDIT   *pEdtColl;
	tGRAPCOMBOBOX	*pComBox;
	U8 i;
	int idex=0;

	pDesk=Grap_CreateDeskbox(&Set_AutoSendAtrr[idex++], "�ϴ�����");
	if (NULL==pDesk)
	{
		return;
	}
	Grap_Inherit_Public(pDeskFather,pDesk);/*��Ϣ�̳�*/
	pDesk->inputTypes = INPUT_TYPE_123;
	pDesk->inputMode = 1;//123
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_2;
	/*���밴ť*/
	for (i=0;i<7;i++)
	{
		pBtn=Grap_InsertButton(pDesk, &Set_AutoSendAtrr[idex++], &Set_AutoSendBtnAttr[i]);
		if (NULL==pBtn)
		{
			return;
		}
		
		pBtn->pressEnter=Set_AutoSend_BtnEnter;
	}
	/*����༭��*/
	pEdtColl= Grap_InsertEdit(pDesk, &Set_AutoSendAtrr[idex++], &Set_AutoSendEditAttr[0]);
	pEdtColl->DrawInputCnt = Set_EditDrawInputCnt;
	if (NULL==pEdtColl)
	{
		return;
	}
	sprintf(pEdtColl->edit_privattr.digt,"%d",gtHyc.autoSendNum);
	
	pEdtColl= Grap_InsertEdit(pDesk, &Set_AutoSendAtrr[idex++], &Set_AutoSendEditAttr[1]);
	pEdtColl->DrawInputCnt = Set_EditDrawInputCnt;
	if (NULL==pEdtColl)
	{
		return;
	}
	sprintf(pEdtColl->edit_privattr.digt,"%d",gtHyc.autoSendTime);

	pEdit2 = Grap_InsertEdit2(pDesk, &Set_AutoSendAtrr[idex++], &Set_AutoSendEdit2Attr[0], 0);
	if(pEdit2 == NULL) return ;

#ifdef W818C
	strcpy(pEdit2->edit_privattr.digt,"˵��:ѡ��'�Զ�',��ǹ�����'������ֵ'��'�ӳ�ʱ��'�Զ����ͼ�¼(δ����¼����������ֵ������ɨ��ʱ�䳬���ӳ�ʱ��);ѡ��'�ֶ�'�����ֶ���F2,��ǹ�ſ�ʼ���ͼ�¼.");
#else
//	strcpy(pEdit2->edit_privattr.digt,"˵��:ѡ��[�Զ�],��ǹ�����[������ֵ]��[�ӳ�ʱ��]�Զ����ͼ�¼(δ����¼����������ֵ������ɨ��ʱ�䳬���ӳ�ʱ��);ѡ��[�ֶ�]�����ֶ������������Ҽ���,��ǹ�����ſ�ʼ���ͼ�¼.");
	strcpy(pEdit2->edit_privattr.digt,"˵��:��ǹ�����[������ֵ]��[�ӳ�ʱ��]�Զ��������ͼ�¼(δ����¼����������ֵ������ɨ��ʱ�䳬���ӳ�ʱ��); ���輫���ϴ�,�ɡ����������Ҽ���");
#endif
	pEdit2->get_default_str(pEdit2);
	
	pComBox = Grap_InsertCombobox(pDesk, &Set_AutoSendAtrr[idex++], &Set_AutoSendComAttr[0]);
	if(NULL == pComBox ) return ;
	pComBox->comboPriv.total_num = 2;
	pComBox->comboPriv.cur_index = gtHyc.sendType;
	pComBox->comboxgetstr = Set_AutoSendComboboxGetStr;
	pComBox->comboxenter  = Set_AutoSendComboxEnter;
	
	/*������Ϣѭ�����ͷ�*/
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);	
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}
