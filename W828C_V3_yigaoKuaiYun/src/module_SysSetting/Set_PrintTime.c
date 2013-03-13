#include "SysSetting.h"


/*�ؼ�*/
const tGrapViewAttr Set_PrintTimeAtrr[]=
{
	{COMM_DESKBOX_ID,  0,  0, 240,320, 0,0,0,0,         TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/	
#else
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/	
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
#endif	
	{BUTTON_ID_3,	   0,90,230,40, 	0,0,0,0,         0,  0,0,0,1,0,0,0},//����
	{EDIT_ID_1,		   15,140,210,24,  15,140,225,164,   0,0,0xffff,0,1,1,1,0},/*�����ı���*/
};

/*btn ����*/
const tGrapButtonPriv Set_PrintTimeBtnAttr[]=
{
#ifdef W818C
	{(U8 *)"ȷ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},
#else
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},
	{(U8 *)"ȷ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},
#endif
	{(U8 *)"��ӡ����", 0xffff,0x0,FONTSIZE24X24,0,2,0, TY_UNITY_BG1_EN,NULL},
 };
/*edt ����*/
const tGrapEditPriv	Set_PrintTimeEditAttr []=
{
	{NULL,COLOR_STR_BARSELECT,0,0,2,4,8,  1,3,0,0,1,0,0,1,   50,50,0,  0,0,1,0},
};


/*��ť�¼�*/
static S32 Set_PrintTime_BtnEnter(void *pView,U16 state)
{
	tGRAPBUTTON  *pBtn ;
	tGRAPDESKBOX *pDesk;
	tGRAPEDIT    *pEdit;
	U32  id,cnt;
	S32  rv = SUCCESS;

	pBtn=(tGRAPBUTTON*)pView;
	pDesk=(tGRAPDESKBOX*)pBtn->pParent;
	id=pBtn->view_attr.id;
	
	if(state==STATE_NORMAL)
	{
		switch (id)
		{
		case BUTTON_ID_1:
			pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk, EDIT_ID_1);
			cnt = atoi(pEdit->edit_privattr.digt);
			
			if(cnt < 1 || cnt > 10)
			{
				Com_SpcDlgDeskbox("������Χ!",0,pDesk,1,NULL,NULL,150);
				rv = RETURN_REDRAW;
			}
			else
			{
				gtHyc.printTimes = cnt;
				glbVariable_SaveParameter();
				Com_SpcDlgDeskbox("���óɹ�!",0,pDesk,1,NULL,NULL,150);
				rv = RETURN_QUIT;
			}
			break;
		case BUTTON_ID_2:
			rv = RETURN_QUIT;
			break;
		}
	}
	return rv;
}


/*---------------------------------------------
*����:Set_PrintTime
*����:��ӡ����
*����:
*����:
*-----------------------------------------------*/
void Set_PrintTime(void *pDeskFather)
{
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON *pBtn;
	tGRAPEDIT *pEdt;
	U8 i;
	U32 rc;
	U8  str[8];
	int dex =0;
	
	memset(str,0,8);
	
	pDesk=Grap_CreateDeskbox(&Set_PrintTimeAtrr[dex++], "��ӡ����");
	if (NULL==pDesk)
	{
		return;
	}
	Grap_Inherit_Public(pDeskFather,pDesk);/*��Ϣ�̳�*/
	pDesk->inputTypes = INPUT_TYPE_123;
	pDesk->inputMode = 1;//123
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	/*���밴ť*/
	for (i=0;i<3;i++)
	{
		pBtn=Grap_InsertButton(pDesk, &Set_PrintTimeAtrr[dex++], &Set_PrintTimeBtnAttr[i]);
		if (NULL==pBtn)
		{
			return;
		}
		pBtn->pressEnter=Set_PrintTime_BtnEnter;
	}
	/*����༭��*/
	pEdt= Grap_InsertEdit(pDesk, &Set_PrintTimeAtrr[dex++], &Set_PrintTimeEditAttr[0]);
	pEdt->DrawInputCnt = Set_EditDrawInputCnt;
	if (NULL==pEdt)
	{
		return;
	}
	sprintf(pEdt->edit_privattr.digt, "%d", gtHyc.printTimes);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);	
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}
