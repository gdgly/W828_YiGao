#include "SysSetting.h"



/*===============================ɨ��ģʽ���ý���===================================*/
const tGrapViewAttr Set_ScannerAttr[] =
{
	{COMM_DESKBOX_ID, 0,0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0x12345678},//����
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*ȷ��*/
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
#else
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*ȷ��*/
#endif

	{BUTTON_ID_3,	   0, 80 ,240, 32,   0,0,0,0,   0,0, 0,0,1,0,0,0},//ɨ��ģʽѡ��
	{BUTTON_ID_4,	   0, 130 ,240, 16,   0,0,0,0,   0,0, 0,0,1,0,0,0},//�������⿪��
	
	{LISTER_ID_1,  		5, 160, 230,60,   		5,  160,235,260,   		0,0,0,0,1,1,1,0},/*list   */
};

const tGrapButtonPriv Set_ScannerBtnAttr[]=
{
#ifdef W818C
	{(U8 *)"ȷ��",  COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,TY_UNITY_BG1_EN,NULL},
	{(U8 *)"����",  COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,TY_UNITY_BG1_EN,NULL},
#else
	{(U8 *)"����",  COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,TY_UNITY_BG1_EN,NULL},
	{(U8 *)"ȷ��",  COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,TY_UNITY_BG1_EN,NULL},
#endif	
	{"ɨ��ģʽѡ��",0xffff,0x0,FONTSIZE32X32,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},
	{"�������⿪��:�м��+*",COLOR_RED,COLOR_RED,0,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},
};

const tGrapListerPriv	Set_ScannerListAttr[]=
{
	{24, 2, 0xffff, COLOR_YELLOW,	0, 2, 0, TY_UNITY_BG1_EN, 0, 15, 4, 27, 0, 50, 50, 1, 0, 0, 1, 0,0},
};

int Set_Scanner_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPLIST 		*pList;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk =  (tGRAPDESKBOX *)pBtn->pParent;
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://ȷ��
			pList = (tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
			
			gtHyc.scanMode = pList->lst_privattr.focusItem;
			glbVariable_SaveParameter();
			
			Com_SpcDlgDeskbox("���óɹ�!",0,pDesk,1,NULL,NULL,100);
			return RETURN_QUIT;
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


S32 Set_Scanner_ListerGetString(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	if(i == 0)
	{
		strcpy(str, "��ס�����ɿ��ر�(�3-4��)");
	}
	else
	{
		strcpy(str, "��һ�γ�������3-4��");
	}	
	
	return SUCCESS;
}

S32 Set_Scanner_ListDrawIcon(tGRAPLIST *pLister, U16 i)
{
	tGrapViewAttr   *pview_attr    = &(pLister->view_attr);
	tGrapListerPriv *plst_privattr = &(pLister->lst_privattr);
	U16             x1,x2,y1,y2;
	
	if ( i >= plst_privattr->totalItem)
	{
		return SUCCESS;
	}
	
	x1 = pview_attr->viewX1;
	x2 = pview_attr->viewX1 + pview_attr->viewWidth;
	y1 = (i - plst_privattr->topItem) * plst_privattr->lstHeight + pview_attr->viewY1;
	y2 = y1 + plst_privattr->lstHeight;
	
	if (plst_privattr->focusItem == i)
	{//write select bar
         drvLcd_SetColor(COLOR_CIRBMP_SELECT,0xffff);
	}
	else
	{
		drvLcd_SetColor(COLOR_CIRBMP_UNSELECT,0xffff);
	}
    drvLcd_WriteBMPColor((const U8 *)gBmpCircleData,14,14,y1+6,x1);
	
	return SUCCESS;
}

/*---------------------------------------------
*����:Set_ScannerDesk
*����:ɨ��ģʽ
*����:
*����:
*-----------------------------------------------*/
void Set_ScannerDesk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPLIST     	*pList;
	int	i,idx=0;
		
	pDesk = Grap_CreateDeskbox(&Set_ScannerAttr[idx++], "ɨ��ģʽ");	
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	for(i = 0; i < 4; i ++)
	{
		pBtn = Grap_InsertButton(pDesk,&Set_ScannerAttr[idx++],&Set_ScannerBtnAttr[i]);
		if(pBtn == NULL) return ;
		if(i < 2)
		{
			pBtn->pressEnter = Set_Scanner_BtnPress;
		}
	}
	
	pList =   Grap_InsertLister(pDesk, &Set_ScannerAttr[idx++], &Set_ScannerListAttr[0]);
	if (pList == NULL) return;
	pList->getListString          = Set_Scanner_ListerGetString;
	pList->drawIcon               = Set_Scanner_ListDrawIcon;
	pList->lst_privattr.focusItem = gtHyc.scanMode;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);	
	Grap_DeskboxRun(pDesk);	
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}
