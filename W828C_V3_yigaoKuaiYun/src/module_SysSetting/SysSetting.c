/*
/*
����12������ֻ�ڹ���Ա�����У�ֻ�й���Ա�ſ������á�
1.	����������((��������ַ)(�������˿�)(������DNS))
2.	SIM����
3.	�������
4.	��ǹ��� 
5.	GPS����
6.	�������
7.	ϵͳ����
8.	����ѡ��(WIFI(WIFI�����ѡ��)��GPRS(GPRS������ѡ��))
9.	�Զ��ϴ�����
10.	��ӡ��������
11.	��¼��ѯ
12.	����Ԥ��
13: ���̸�ʽ��


ϵͳ����ÿһ����ͨ�û������Խ�������
ϵͳ����
1.	��������
2.	��������
3.	������У��
4.	��������
5.	ʱ��ͬ��
6.	�������(��������ĵȴ��������ɾ�ȦȦ�ĵȴ�ģʽ)
7.	ʹ��˵��
8.	�汾��Ϣ(���к�)�汾��Ϣ���Ѱ������кţ�����ʡ�����к�����
9.	��ǹ���
*/
#include "SysSetting.h"





/*---------------------------------------------
*����:Rec_SearchDesk
*����:��ѯ��¼������
*����:
*����:
*-----------------------------------------------*/
extern void Rec_SearchDesk(void *pDeskFather);



U32	gu32SettingTemp;
tSETTING_INFO	*gptSetInfo;


//����ԱȨ������
const tSETTING_INFO	setting_0[]=//����Ա
{
	//��ʾ�ַ�			ִ�к���
	{"����������",		Set_Url},
	{"SIM����",			Set_SIM},
	{"�������",		Set_CompanyID},
	{"��ǹ���",		Set_DeviceID},
	//{"GPS����",			Set_Gps},
	//{"��ӡ����",		Set_PrintTime},
	{"��¼��ѯ",		Rec_SearchDesk},
	{"����Ԥ��",		Set_FactorySetting},
	{"��ǹ���",		Set_RecClear},
	{"���̸�ʽ��",		Set_Format},
	{"ϵͳ����",		Set_SysSettingDesk},
	//{"ҵ��ѡ��",		Set_OperationsSelect}
};

//��ͨԱ��Ȩ������
const tSETTING_INFO	setting_1[]=//ϵͳ����
{
	//��ʾ�ַ�			ִ�к���
	{"��������",		Set_Sound},
	{"��������",		Set_BackLight},
	//{"������У��",	Set_ScreenVerify},
	{"��������",		Set_ScreenProtect},
	//{"��������",		Set_ParamConfig},
	{"�ϴ�����",		Set_AutoSend},
	{"ɨ��ģʽ",		Set_ScannerDesk},
	{"���������",		Set_GprsAccess},
	{"����������",		Set_WorkDay},
	{"�������",		Set_BT},
	{"WIFI����",		Set_WIFI},
	{"����ѡ��",		Set_NetSelect},
	{"ʱ��ͬ��",		Set_UpdateTime},
	{"�������",		Set_UpdateSoft},
	{"ʹ��˵��",		Set_Instructions},
	{"�汾��Ϣ",		Set_VersionInfo},
	{"��ǹ���",		Set_RecClear}
};

//Ȩ������   Ŀǰֻ����2��Ȩ��  ��������Ȩ��,�ڴ˴����Ӽ���,���������Ӧ������
const tSETTING_INFO	*setting_array[]=
{
	setting_0,setting_1
};


/*====================================����������================================*/
const tGrapViewAttr  SysSetViewAttr[] =
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//����
	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
};

const tGrapButtonPriv   SysSetBtnAttr[]=
{
	{(U8 *)"����",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//����
};

/*const tGrapListerPriv  SysSetListAttr[]=
{   
	{24,9,0xffff,0x00,	0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,50,4,20,0,50,50,1,0,0,1,1,1},
};*/

const tSupListPriv SysSetSupListAttr[]=
{
	{NEED_TIMER|CONFIRMANDENTER, 0, 224,  24, 9, 0xffff, 0x0, 0,0,0,YD_SLIDER2_EN,YD_SLIDER1_EN,TY_UNITY_BG1_EN,
	 0,0,0,0,0,0,  50,4,20,0,50,50,0,0,1,YD_HD_S_EN,YD_HD_X_EN,YD_HD_Z_EN},
};

/*int SysSet_GetListStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	sprintf(str, "%2d:%s", i+1, gptSetInfo[i].pName);
	//sprintf(str, gptSetInfo[i].pName);
	return SUCCESS;
}

int SysSet_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	
	pDesk = (tGRAPLIST *)pLister->pParent;
	
	if(gptSetInfo[i].setfun != NULL)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		gptSetInfo[i].setfun(pDesk);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	}
	
	return RETURN_REDRAW;
}*/

int SysSet_SupListGetStr(void *pView, U16 itemNo, U8 *str, U16 maxLen)
{
	sprintf(str, "%2d:%s", itemNo+1, gptSetInfo[itemNo].pName);

	return SUCCESS;
}

int SysSet_SupListEnter(void *pView, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	tSUPPERLIST		*pSupList;
	
	pSupList = (tSUPPERLIST *)pView;
	pDesk  = (tGRAPDESKBOX *)pSupList->pParent;
	
	if(gptSetInfo[i].setfun != NULL)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		gptSetInfo[i].setfun(pDesk);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	}
	
	return RETURN_REDRAW;
}

int SysSet_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://�˳�
			return RETURN_QUIT;
			break;
		default:
			break;
		}
	}
	
	return RETURN_CANCLE;
}


/*---------------------------------------------
*����:Set_SysSettingDesk
*����:����������
*����:
*����:
*-----------------------------------------------*/
int Set_SysSettingDesk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tSUPPERLIST		*pSupList;
	int				i,idx=0;
	
	gptSetInfo = setting_array[1];
	
	pDesk = Grap_CreateDeskbox(& SysSetViewAttr[idx++], "ϵͳ����");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	//��ť
	pBtn = Grap_InsertButton(pDesk, & SysSetViewAttr[idx++], &SysSetBtnAttr[0]);
    if(NULL == pBtn ) return -1;
    pBtn->pressEnter =  SysSet_BtnEnter;

    /*pList = Grap_InsertLister(pDesk, &SysSetViewAttr[idx++], & SysSetListAttr[0]);
    if(pList == NULL) return -1;
    pList->lst_privattr.totalItem = sizeof(setting_1)/sizeof(tSETTING_INFO);
	pList->enter =  SysSet_ListEnter;
	pList->getListString =  SysSet_GetListStr;*/
	
	pSupList = Grap_InsertSupList(pDesk, &SysSetViewAttr[idx++], &SysSetSupListAttr[0]);
	if (pSupList == NULL) return;
	pSupList->SupListEnter = SysSet_SupListEnter;
	pSupList->SupListGetStr= SysSet_SupListGetStr;
	pSupList->SupList_priv.totalItem = sizeof(setting_1)/sizeof(tSETTING_INFO);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}


/*=============================================����Ա���ý���===================================================*/
const tGrapViewAttr  AdminSetViewAttr[] =
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//����
	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
};

const tGrapButtonPriv  AdminSetBtnAttr[]=
{
	{(U8 *)"����",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//����
};

/*const tGrapListerPriv  AdminSetListAttr[]=
{   
	{24,9,0xffff,0x00,	0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,50,4,20,0,50,50,1,0,0,1,1,1},
};*/

const tSupListPriv AdminSetSupListAttr[]=
{
	{NEED_TIMER|CONFIRMANDENTER, 0, 224,  24, 9, 0xffff, 0x0, 0,0,0,YD_SLIDER2_EN,YD_SLIDER1_EN,TY_UNITY_BG1_EN,
	 0,0,0,0,0,0,  50,4,20,0,50,50,0,0,1,YD_HD_S_EN,YD_HD_X_EN,YD_HD_Z_EN},
};


/*int AdminSet_GetListStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	sprintf(str, "%2d:%s", i+1, gptSetInfo[i].pName);
	//strcpy(str, gptSetInfo[i].pName);
	return SUCCESS;
}

int AdminSet_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	
	pDesk = (tGRAPLIST *)pLister->pParent;
	
	if(gptSetInfo[i].setfun != NULL)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		gptSetInfo[i].setfun(pDesk);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		
		gptSetInfo = setting_array[0];
	}
	
	return RETURN_REDRAW;
}*/

int AdminSet_SupListGetStr(void *pView, U16 itemNo, U8 *str, U16 maxLen)
{
	sprintf(str, "%2d:%s", itemNo+1, gptSetInfo[itemNo].pName);
	return SUCCESS;
}

int AdminSet_SupListEnter(void *pView, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	tSUPPERLIST		*pSupList;
	
	pSupList = (tSUPPERLIST *)pView;
	pDesk  = (tGRAPDESKBOX *)pSupList->pParent;
	
	if(gptSetInfo[i].setfun != NULL)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		gptSetInfo[i].setfun(pDesk);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		
		gptSetInfo = setting_array[0];
	}
	
	return RETURN_REDRAW;
}

int AdminSet_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://�˳�
			return RETURN_QUIT;
			break;
		default:
			break;
		}
	}
	
	return RETURN_CANCLE;
}


/*---------------------------------------------
*����:AdminSettingDesk
*����:����Ա����������
*����:
*����:
*-----------------------------------------------*/
void AdminSettingDesk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tSUPPERLIST		*pSupList;
	int				i,idx=0;
	
	gptSetInfo = setting_array[0];
	
	pDesk = Grap_CreateDeskbox(&AdminSetViewAttr[idx++], NULL);
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	//��ť
	pBtn = Grap_InsertButton(pDesk, &AdminSetViewAttr[idx++], &AdminSetBtnAttr[0]);
    if(NULL == pBtn ) return;
    pBtn->pressEnter = AdminSet_BtnEnter;

    /*pList = Grap_InsertLister(pDesk, &AdminSetViewAttr[idx++], &AdminSetListAttr[0]);
    if(pList == NULL) return ;
    pList->lst_privattr.totalItem = sizeof(setting_0)/sizeof(tSETTING_INFO);
	pList->enter = AdminSet_ListEnter;
	pList->getListString = AdminSet_GetListStr;*/
	
	pSupList = Grap_InsertSupList(pDesk, &AdminSetViewAttr[idx++], &AdminSetSupListAttr[0]);
	if (pSupList == NULL) return;
	pSupList->SupListEnter = AdminSet_SupListEnter;
	pSupList->SupListGetStr= AdminSet_SupListGetStr;
	pSupList->SupList_priv.totalItem = sizeof(setting_0)/sizeof(tSETTING_INFO);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}


///////////////////////////////////////////////////////////////

S32 Set_EditDrawInputCnt(tGRAPEDIT *pEdit)
{
	tStrDispInfo disp_info;
	
	if(pEdit->view_attr.curFocus)
	{//��ʾ����
		U8 buf[8];
		
		Grap_ClearScreen(EDIT_X, EDIT_Y, EDIT_X+EDIT_W,EDIT_Y+EDIT_H);
		
		memset(buf,0,8);
		sprintf(buf,"%d/%d",strlen(pEdit->edit_privattr.digt),pEdit->edit_privattr.containLen);
		
		disp_info.string = buf;
		disp_info.color  = COLOR_RED;
		disp_info.font_size = 0;
		disp_info.font_type = 0;
		disp_info.flag = 2;
		disp_info.keyStr = NULL;
		Grap_WriteString(EDIT_X, EDIT_Y, EDIT_X+EDIT_W,EDIT_Y+EDIT_H, &disp_info);
	}
	
    return SUCCESS;
}

/*---------------------------------------------
*����:Help_GetInfo
*����:����
*����:pStr:��Ű�����Ϣ  
      limitSize:pStr�Ĵ�С
      pHelpName:�����ļ���
*����:
*-----------------------------------------------*/
void Help_GetInfo(U8 *pStr, int limitSize, U8 *pHelpName)
{
	int fp,size;
	char path[100];
	
	sprintf(path, "C:/ϵͳ/help/%s.txt", pHelpName);
	fp = AbstractFileSystem_Open(path, AFS_READ);
	if(fp >= 0)
	{
		size = AbstractFileSystem_FileSize(fp);
		if(size >= limitSize) size = limitSize-2;
		
		AbstractFileSystem_Seek(fp, 0, AFS_SEEK_OFFSET_FROM_START);
		AbstractFileSystem_Read(fp, pStr, size);
		AbstractFileSystem_Close(fp);
		
		pStr[size] = 0;
	}
	else
	{
		strcpy(pStr, "�Բ���,û��ʹ��˵���ļ�.");
	}
	
	return ;
}