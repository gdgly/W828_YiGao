#include "SysSetting.h"



/*�ؼ�*/
const tGrapViewAttr Set_UrlAtrr[]=
{
	{COMM_DESKBOX_ID,	0,0,240,320,  	0,0,0,0,         TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/	
#else
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/	
#endif	
	{BUTTON_ID_3,		0,90,230,40, 	0,0,0,0,         0, 0,0,0,1,0,0,0},//����
	{EDIT_ID_1,			15,140,210,24,  15,140,225,164,   0,0,0xffff,0,1,1,1,0},/*�����ı���*/
					
	{EDIT_ID_2,   10, 175,210, 100,      10,  175,  220,  275,   0,	0,	0,0,1,0,0,0},
};

/*btn ����*/
const tGrapButtonPriv Set_UrlBtnAttr[]=
{
#ifdef W818C
	{(U8 *)"ȷ��", 		0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,	TY_UNITY_BG1_EN,NULL},
	{(U8 *)"����", 		0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,	TY_UNITY_BG1_EN,NULL},
#else
	{(U8 *)"����", 		0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,	TY_UNITY_BG1_EN,NULL},
	{(U8 *)"ȷ��", 		0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,	TY_UNITY_BG1_EN,NULL},
#endif	
	{(U8 *)"��������ַ",0xffff,0x0,FONTSIZE24X24,0,2,0,  			TY_UNITY_BG1_EN,NULL},
 };
/*edt ����*/
const tGrapEditPriv	Set_UrlEditAttr[]=
{
	{NULL,COLOR_STR_BARSELECT,0,0,4,6,25,  1,256,0,0,0,0,0,1,   50,50,0,  0,0,1,0},
};

const tGrapEdit2Priv  Set_UrlEdit2Attr[]=
{
 	{NULL,	0xffff,0,0,0,TY_UNITY_BG1_EN,0,0,256,1,1,0,0},//��ʾ
};

/*��ť�¼�*/
static S32 Set_Url_BtnEnter(void *pView,U16 state)
{
	tGRAPBUTTON  *pBtn ;
	tGRAPDESKBOX *pDesk;
	tGRAPEDIT    *pEdit;
	U32  id;
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
			if(strcmp(pEdit->edit_privattr.digt, gtHyc.url) != 0)
			{
				strcpy(gtHyc.url, pEdit->edit_privattr.digt);
				glbVariable_SaveParameter();
				hyIE_clearDns();
			}
			
			Com_SpcDlgDeskbox("���óɹ�!",0,pDesk,1,NULL,NULL,150);
			rv = RETURN_QUIT;
			break;
		case BUTTON_ID_2:
			rv = RETURN_QUIT;
			break;
		}
	}
	return rv;
}


/*---------------------------------------------
*����:Set_Url
*����:����������
*����:
*����:
*-----------------------------------------------*/
void Set_Url(void *pDeskFather)
{
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON *pBtn;
	tGRAPEDIT 	*pEdt;
	tGRAPEDIT2	*pEdit2;
	U8 i;
	U32 rc;
	int idx =0; 
	
	pDesk=Grap_CreateDeskbox(&Set_UrlAtrr[idx++], "������");
	if (NULL==pDesk)
	{
		return;
	}
	Grap_Inherit_Public(pDeskFather,pDesk);/*��Ϣ�̳�*/
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 2;//abc
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	/*���밴ť*/
	for (i=0;i<3;i++)
	{
		pBtn=Grap_InsertButton(pDesk, &Set_UrlAtrr[idx++], &Set_UrlBtnAttr[i]);
		if (NULL==pBtn)
		{
			return;
		}
		pBtn->pressEnter=Set_Url_BtnEnter;
	}
	/*����༭��*/
	pEdt= Grap_InsertEdit(pDesk, &Set_UrlAtrr[idx++], &Set_UrlEditAttr[0]);
	pEdt->DrawInputCnt = Set_EditDrawInputCnt;
	if (NULL==pEdt)
	{
		return;
	}
	strcpy(pEdt->edit_privattr.digt, gtHyc.url);
	
	pEdit2 = Grap_InsertEdit2(pDesk, &Set_UrlAtrr[idx++], &Set_UrlEdit2Attr[0], 0);
	if(pEdit2 == NULL) return ;
	strcpy(pEdit2->edit_privattr.digt,"˵��:������¼�ϴ����������صĵ�ַ.\r\n����ȷ��д,����ҵ�����ݽ��޷���ȷ�ϴ�.");
	pEdit2->get_default_str(pEdit2);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);	
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}
