#include "SysSetting.h"



/*�ؼ�*/
const tGrapViewAttr Set_WorkDayAtrr[]=
{
	{COMM_DESKBOX_ID,   0,  0, 240,320, 0,0,0,0, 		TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},
#ifdef W818C
	{BUTTON_ID_1,	    15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
	{BUTTON_ID_2,	    152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/	
#else
	{BUTTON_ID_2,	    15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
	{BUTTON_ID_1,	    152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/	
#endif		
	{BUTTON_ID_3,	  10,  40,220,16,   0,0,0,0,   0,  0,0,0,1,0,0,0},//����������
	{BUTTON_ID_4,	  8,  70,146,22,   0,0,0,0,   0,  0,0,0,1,0,0,0},//���뿪ʼʱ��(0-23)
	
	{EDIT_ID_1,	155,70, 40,22,   148,70,186,91,    	0,0,0xffff,0,1,1,1,0},//ʱ��(0-23)
	
	{EDIT_ID_2,   10, 100,210, 180,      10,  100,  220,  280,   0,	0,	0,0,1,0,1,0},
};

/*btn ����*/
const tGrapButtonPriv Set_WorkDayBtnAttr[]=
{
#ifdef W818C
   	{(U8 *)"ȷ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  	TY_UNITY_BG1_EN,NULL},
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,	TY_UNITY_BG1_EN,NULL},
#else
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,	TY_UNITY_BG1_EN,NULL},
   	{(U8 *)"ȷ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  	TY_UNITY_BG1_EN,NULL},
#endif
	{(U8 *)"����������",0xffff,0xffff,FONTSIZE8X16,0,2,0,		TY_UNITY_BG1_EN,NULL},	
	{(U8 *)"���뿪ʼʱ��(0-23)", 0xffff,0x0,FONTSIZE8X16,0,1,0, 	TY_UNITY_BG1_EN,NULL},
};

/*edt ����*/
const tGrapEditPriv	Set_WorkDayEditAttr []=
{
	{NULL,COLOR_STR_BARSELECT,0,0,2,4,3,  1,2,0,0,1,0,0,1,   50,50,0,  0,0,1,0},
};

const tGrapEdit2Priv  Set_WorkDayEdit2Attr[]=
{
 	{NULL,	COLOR_YELLOW,0,0,0,TY_UNITY_BG1_EN,0,0,512,1,1,0,0},//��ʾ
};

/*��ť�¼�*/
static S32 Set_WorkDay_BtnEnter(void *pView,U16 state)
{
	tGRAPBUTTON  *pBtn ;
	tGRAPDESKBOX *pDesk;
	tGRAPEDIT    *pEdit;
	U32  id;
	S32  rv = SUCCESS;
	int			value;

	pBtn=(tGRAPBUTTON*)pView;
	pDesk=(tGRAPDESKBOX*)pBtn->pParent;
	id=pBtn->view_attr.id;
	if (state==STATE_NORMAL)
	{
		switch (id)
		{
		case BUTTON_ID_1:
			pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_1);
			value= atoi((char *)pEdit->edit_privattr.digt);			
			if(value > 23)
			{
				Com_SpcDlgDeskbox("ʱ���������  (��Χ0-23)!",6,pDesk,2,NULL,NULL,100);	
				return RETURN_REDRAW;
			}
			
			gtHyc.workDay = value;
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

/*---------------------------------------------
*����:Set_WorkDay
*����:����������
*����:
*����:
*-----------------------------------------------*/
void Set_WorkDay(void *pDeskFather)
{
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON *pBtn;
	tGRAPEDIT2	*pEdit2;
	tGRAPEDIT   *pEdt;
	U8 i;
	int idex=0;

	pDesk=Grap_CreateDeskbox(&Set_WorkDayAtrr[idex++], "����������");
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
	for (i=0;i<4;i++)
	{
		pBtn=Grap_InsertButton(pDesk, &Set_WorkDayAtrr[idex++], &Set_WorkDayBtnAttr[i]);
		if (NULL==pBtn)
		{
			return;
		}
		if(i < 2)
		{
			pBtn->pressEnter=Set_WorkDay_BtnEnter;
		}
	}
	/*����༭��*/
	pEdt= Grap_InsertEdit(pDesk, &Set_WorkDayAtrr[idex++], &Set_WorkDayEditAttr[0]);
	pEdt->DrawInputCnt = Set_EditDrawInputCnt;
	if (NULL==pEdt)
	{
		return;
	}
	sprintf(pEdt->edit_privattr.digt,"%d",gtHyc.workDay);

	pEdit2 = Grap_InsertEdit2(pDesk, &Set_WorkDayAtrr[idex++], &Set_WorkDayEdit2Attr[0], 0);
	if(pEdit2 == NULL) return ;
	strcpy(pEdit2->edit_privattr.digt,"˵��:����һ��Ŀ�ʼʱ���.����Ϊ00:00Ϊһ��Ŀ�ʼʱ��,�������ǵĹ���ʱ����ܻ��Խ00:00(�繤��ʱ��:19:00--05:00),\
�����00:00��Ϊ��һ��Ŀ�ʼʱ��,���ǵĹ����ͱ���������������,��ҵ����ͳ�Ʋ�����.Ϊ�˸������ͳ��ҵ��,����������һ��Ŀ�ʼʱ��,�繤��ʱ��:19:00--05:00,����������һ��Ŀ�ʼʱ��Ϊ:12:00.\
���ú�,�������һ��19:00��ʼ�������ܶ���05:00,��ͳ��ҵ��ʱ��ͳ��Ϊ�ܶ���ҵ��.");
	pEdit2->get_default_str(pEdit2);
	
	/*������Ϣѭ�����ͷ�*/
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);	
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}
