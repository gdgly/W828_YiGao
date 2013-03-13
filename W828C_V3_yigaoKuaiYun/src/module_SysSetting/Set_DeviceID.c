#include "SysSetting.h"


/*�ؼ�*/
const tGrapViewAttr Set_DeviceIDAtrr[]=
{
	{COMM_DESKBOX_ID,   0,  0, 240,			320, 0,0,0,0, 		TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},
	{BUTTON_ID_1,  		83,282,73,34,  		83,282,156,316,   	YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},/*����*/
	{BUTTON_ID_2,		0, 100 ,230, 30,   	0,0,0,0,     		0,0,0,0,1,0,0,0},//����
	{EDIT_ID_1,			29,140, 181,24,     29,140,210,164, 	0,0,0xffff,0,1,0,0,0},/*�����ı���*/
					
	{EDIT_ID_2,   10, 175,210, 100,      10,  175,  220,  275,   0,	0,	0,0,1,0,0,0},			
};

/*btn ����*/
const tGrapButtonPriv Set_DeviceIDBtnAttr[]=
{
	{(U8*)"����",COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,TY_UNITY_BG1_EN,0},
	{(U8*)"��ǹ���", COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,FONTSIZE24X24,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN ,NULL},//

};
/*edt ����*/
const tGrapEditPriv	  Set_DeviceIDEditAttr []=
{
	{NULL,COLOR_YELLOW,0,0,4,6,16,  1,12,0,0,1,TY_UNITY_BG1_EN,0,1,   50,50,0,  0,0,1,0},
};

const tGrapEdit2Priv  Set_DeviceIDEdit2Attr[]=
{
 	{NULL,	0xffff,0,0,0,TY_UNITY_BG1_EN,0,0,256,1,1,0,0},//��ʾ
};

/*��ť�¼�*/
static S32 Set_DeviceID_BtnEnter(void *pView,U16 state)
{
	tGRAPBUTTON  *pBtn ;
	S32  rv = SUCCESS;

	pBtn=(tGRAPBUTTON*)pView;

	if (state==STATE_NORMAL)
	{
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1:
			rv = RETURN_QUIT;
			break;
		}
	}
	return rv;
}


/*---------------------------------------------
*����:Set_DeviceID
*����:��ǹ���
*����:
*����:
*-----------------------------------------------*/
void Set_DeviceID(void *pDeskFather)
{
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON *pBtn;
	tGRAPEDIT 	*pEdt;
	tGRAPEDIT2	*pEdit2;
	U8 i;
	int idx =0;
	
	pDesk=Grap_CreateDeskbox(&Set_DeviceIDAtrr[idx++], "��ǹ���");
	if (NULL==pDesk)
	{
		return;
	}
	Grap_Inherit_Public(pDeskFather,pDesk);/*��Ϣ�̳�*/
	
	/*���밴ť*/
	for (i=0;i<2;i++)
	{
		pBtn=Grap_InsertButton(pDesk, &Set_DeviceIDAtrr[idx++], &Set_DeviceIDBtnAttr[i]);
		if (NULL==pBtn)
		{
			return;
		}
		pBtn->pressEnter=Set_DeviceID_BtnEnter;
	}
	/*����༭��*/
	pEdt= Grap_InsertEdit(pDesk, &Set_DeviceIDAtrr[idx++], &Set_DeviceIDEditAttr[0]);
	pEdt->DrawInputCnt = Set_EditDrawInputCnt;
	if (NULL==pEdt)
	{
		return;
	}
	strcpy(pEdt->edit_privattr.digt,gtHyc.MachineCode);

	pEdit2 = Grap_InsertEdit2(pDesk, &Set_DeviceIDAtrr[idx++], &Set_DeviceIDEdit2Attr[0], 0);
	if(pEdit2 == NULL) return ;
	strcpy(pEdit2->edit_privattr.digt,"˵��:�ð�ǹ��Ψһ���,���ǹ��������к�һ��.");
	pEdit2->get_default_str(pEdit2);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);	
	Grap_DeskboxDestroy(pDesk);
	
	return;
}
