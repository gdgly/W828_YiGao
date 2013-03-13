#include "SysSetting.h"



/*�ؼ�*/
const tGrapViewAttr Set_SIMViewAtrr[]=
{
	{COMM_DESKBOX_ID,   0,  0, 240,320, 0,0,0,0,   TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/	
#else
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},	/*����*/
#endif	
	{BUTTON_ID_3,	29,100,60, 30,	0,0,0,0 ,	    0,0,0,0,1,0,0,0},
	
	{EDIT_ID_1,		29,140,186,24,  29,100,215,164,   0,0,0xffff,0,1,1,1,0},/*�����ı���*/
					
};

/*btn ����*/
const tGrapButtonPriv Set_SIMBtnAttr[]=
{
#ifdef W818C
	{(U8 *)"ȷ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},
#else
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},	
	{(U8 *)"ȷ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},
#endif	
	{(U8 *)"SIM����", 0xffff,0x0,FONTSIZE24X24,0,2,0, TY_UNITY_BG1_EN,NULL},
};
/*edt ����*/
const tGrapEditPriv	  Set_SIMEdtAttr []=
{
	{NULL,COLOR_STR_BARSELECT,0,0,2,6,13,  1,11,0,0,1,0,0,1,   50,50,0,  0,1,1,1},
};




/*��ť�¼�*/
static S32 Set_SIM_BtnEnter(void *pView,U16 state)
{
	tGRAPBUTTON  *pBtn ;
	tGRAPDESKBOX *pDesk;
	tGRAPEDIT    *pEdit;
	U32  id;
	S32  rv = SUCCESS;

	pBtn  = (tGRAPBUTTON*)pView;
	pDesk = (tGRAPDESKBOX*)pBtn->pParent;
	
	id=pBtn->view_attr.id;
	
	if (state==STATE_NORMAL)
	{
		switch (id)
		{
		case BUTTON_ID_1:
			if(phone_onlyRead_simcardStatus() == 0)
			{
				Com_SpcDlgDeskbox("û��SIM��!",6,pDesk,1,NULL,NULL,100);
				break;
			}
			
			pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk, EDIT_ID_1);
			if(strlen(pEdit->edit_privattr.digt) == 11)
			{
				strcpy(gtHyc.simCard, pEdit->edit_privattr.digt);
				
				gsm_WriteSimCard(gtHyc.simCard);
				Comm_SetMachineInfo(gtHyc.simCard," ");
				
				glbVariable_SaveParameter();
				Com_SpcDlgDeskbox("���óɹ�!",0,pDesk,1,NULL,NULL,100);
				return RETURN_QUIT;
			}
			else
			{
				Com_SpcDlgDeskbox("λ������!",6,pDesk,1,NULL,NULL,100);
			}			
			return RETURN_QUIT;
			break;
		case BUTTON_ID_2:
			return RETURN_QUIT;
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return rv;
}



/*---------------------------------------------
*����:Set_SIM
*����:SIM��������
*����:
*����:
*-----------------------------------------------*/
void Set_SIM(void *pDeskFather)
{
	tGRAPDESKBOX 	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT 		*pEdt;
	U8 				i, idx=0;
	
	pDesk=Grap_CreateDeskbox(&Set_SIMViewAtrr[idx++], "SIM����");
	if (NULL==pDesk)
	{
		return;
	}
	Grap_Inherit_Public(pDeskFather,pDesk);/*��Ϣ�̳�*/
	pDesk->scanEnable = 1;
	pDesk->inputTypes = INPUT_TYPE_123;
	pDesk->inputMode = 1;//123
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	/*���밴ť*/
	for (i=0; i<3; i++)
	{
		pBtn=Grap_InsertButton(pDesk, &Set_SIMViewAtrr[idx++], &Set_SIMBtnAttr[i]);
		if (NULL == pBtn)
		{
			return;
		}
		pBtn->pressEnter = Set_SIM_BtnEnter;
	}
	/*����༭��*/
	pEdt= Grap_InsertEdit(pDesk, &Set_SIMViewAtrr[idx++], &Set_SIMEdtAttr[0]);
	pEdt->DrawInputCnt = Set_EditDrawInputCnt;
	if (NULL == pEdt)
	{
		return;
	}
	//pEdt->draw = Com_SimEditDraw;
	strcpy(pEdt->edit_privattr.digt, gtHyc.simCard);	

	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);	
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}
