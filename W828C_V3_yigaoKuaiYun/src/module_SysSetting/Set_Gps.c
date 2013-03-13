#include "SysSetting.h"


/*�ؼ�*/
const tGrapViewAttr Set_GpsAtrr[]=
{
	{COMM_DESKBOX_ID,   0, 0,240,320, 	0,0,0,0,    	  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/	
#else
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/	
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
#endif	
	//����lable
	{BUTTON_ID_3,      5,150,80,16,    	0,0,0,0,	0,0,0,0,1,0,0,0},/*GPS����:   */
	
	{BUTTON_ID_4,	   9,61,100,22,   	0,0,0,0,   	0,0,0,0,1,0,0,0},//
	{BUTTON_ID_5,	   213,61,20,22, 	0,0,0,0,   	0,0,0,0,1,0,0,0},//
	{BUTTON_ID_6,	   9,103,30,22,  	0,0,0,0,   	0,0,0,0,1,0,0,0},//
	{BUTTON_ID_7,	   213,103,20,22,	0,0,0,0,   	0,0,0,0,1,0,0,0},//
	
	{EDIT_ID_1,	172,61, 38,21,    172,61,210,82,   0,0,0xffff,0,1,0,1,0},/*����ɼ�����ʱ�����ı���*/
	{EDIT_ID_2,	172,103, 38,21,   172,103,210,124, 0,0,0xffff,0,1,0,1,0},/*�����ϴ�ʱ�����ı���*/
	
	
	{LISTER_ID_1,   50, 180, 160,60,    50, 180,210,240, 0, 0,0, 0,1,1,1,0},/*list   */
};

/*btn ����*/
const tGrapButtonPriv Set_GpsBtnAttr[]=
{
#ifdef W818C
	{"ȷ��",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//
	{"����",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},
#else
	{"����",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},	
	{"ȷ��",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//
#endif	
	{"GPS����:",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_RIGHT,0, TY_UNITY_BG1_EN,NULL},//
	{"�ɼ�ʱ����(1-60)",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_RIGHT,0, TY_UNITY_BG1_EN,NULL},//
	{"��",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_RIGHT,0, TY_UNITY_BG1_EN,NULL},//
	{"�ϴ�ʱ����(5-60)",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_RIGHT,0, TY_UNITY_BG1_EN,NULL},//
	{"��",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_RIGHT,0, TY_UNITY_BG1_EN,NULL},//
 
};

/*edt ����*/
const tGrapEditPriv	Set_GpsEditAttr []=
{
	{NULL,0x00,0,0,2,2,2,  1,2,0,0,1,0,0, 1,50,50,0,  0,0,1,0},
	
};

/*list*/

const tGrapListerPriv Set_GpsListAttr[]=
{
     {24, 2, 0xffff, COLOR_YELLOW,	0, 2, 0, TY_UNITY_BG1_EN, 0, 4, 3, 21, 0, 30, 30, 1, 0, 0, 2, 0,0},
};


/*��ť�¼�*/
static S32 Set_Gps_BtnEnter(void *pView,U16 state)
{
	tGRAPBUTTON  *pBtn ;
	tGRAPDESKBOX *pDesk;
	tGRAPEDIT    *pEdit;
	tGRAPLIST    *pList;
	U32  id;
	S32  rv = SUCCESS;
    int  getInterval, sendInterval;
    
	pBtn=(tGRAPBUTTON*)pView;
	pDesk=(tGRAPDESKBOX*)pBtn->pParent;
	id=pBtn->view_attr.id;
	if (state==STATE_NORMAL)
	{
		switch (id)
		{
		case BUTTON_ID_1:
			pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_1);
			getInterval = atoi((char *)pEdit->edit_privattr.digt);
						
			if(getInterval < 1 || getInterval > 60)
			{
				Com_SpcDlgDeskbox("�ɼ�ʱ����������Χ!",6,pDesk,2,NULL,NULL,200);	

				return RETURN_REDRAW;
			}
			
			pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_2);
			sendInterval = atoi((char *)pEdit->edit_privattr.digt);
			if(sendInterval < 5 || sendInterval > 60)
			{
				Com_SpcDlgDeskbox("�ϴ�ʱ����������Χ!",6,pDesk,2,NULL,NULL,200);	
				return RETURN_REDRAW;
			}
			
			pList=(tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
			gtHyc.gpsSwitch = pList->lst_privattr.focusItem;
			
			gtHyc.gpsGetInterval = getInterval;
			gtHyc.gpsSendInterval= sendInterval;
			
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

/**
 * lister ����б��ַ�
 */
static S32 Set_Gps_ListGetString(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	switch(i)
	{
	case 0:
		strcpy(str, "��");
		break;
	case 1:
		strcpy(str, "��");
		break;
	default:
		break;
	}

	return SUCCESS;
}

/****************************************************************
*����:Set_Gps_ListDrawIcon
*����:list ��ǰ��ĵ�ѡͼ
*����:
*����:
****************************************************************/
static S32 Set_Gps_ListDrawIcon(tGRAPLIST *pLister, U16 i)
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
	
	if (i == plst_privattr->focusItem)
	{//write select bar
         drvLcd_SetColor(COLOR_CIRBMP_SELECT,0xffff);
	}
	else
	{
		drvLcd_SetColor(COLOR_CIRBMP_UNSELECT,0xffff);
	}
	
	drvLcd_WriteBMPColor((const U8 *)gBmpCircleData,14,14,y1+6,x1+40);
	
	return SUCCESS;
}


// List �����ؼ��Ļ��ƺ�����
S32 Set_Gps_ListerDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLIST *pLister;
	U16 i;
	tGrapViewAttr *pview_attr;
	tGrapListerPriv *plst_privattr;

	pLister = (tGRAPLIST *)pView;
	pview_attr = &(pLister->view_attr);
	plst_privattr = &(pLister->lst_privattr);
	
	Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1+pview_attr->viewWidth), 
		(U16)(pview_attr->viewY1+pview_attr->viewHeight));
	
	if(plst_privattr->totalItem > plst_privattr->lstNum)
	{//��Ҫ��ʾ������
		if(plst_privattr->topItem > plst_privattr->totalItem - plst_privattr->lstNum)
		{
			plst_privattr->topItem = plst_privattr->totalItem - plst_privattr->lstNum;
		}
	}
	
	if(plst_privattr->totalItem <= plst_privattr->lstNum && plst_privattr->topItem > 0)
	{
		plst_privattr->topItem = 0;
	}
	if(plst_privattr->focusItem >= plst_privattr->totalItem && plst_privattr->totalItem > 0)
	{
		plst_privattr->focusItem = plst_privattr->totalItem-1;
	}
	
	for (i = 0; i < plst_privattr->lstNum; i++)
	{
		if(plst_privattr->topItem + i >= plst_privattr->totalItem)
			break;
		pLister->updateList(pLister, (U16)(plst_privattr->topItem + i));
	}
	
	//����Lister��ʾ��ǰ"�ڼ���/�ܹ�������" 
	if(plst_privattr->isShowCount==1 && plst_privattr->totalItem > 0)
	{//��ʾ����
		U8 buf[20];
		tStrDispInfo disp_info;
		
		
		memset(buf,0,20);
		sprintf(buf,"%d/%d",(plst_privattr->focusItem+1),plst_privattr->totalItem);
		
		disp_info.string = buf;
		if(pview_attr->curFocus == 1)
			disp_info.color = COLOR_RED;
		else
			disp_info.color = plst_privattr->lstColor;
		disp_info.font_size = plst_privattr->font_size;
		disp_info.font_type = plst_privattr->font_type;
		disp_info.flag = 2;
		disp_info.keyStr = NULL;
		Grap_WriteString(pview_attr->viewX1, pview_attr->viewY1+pview_attr->viewHeight-20, 
					pview_attr->viewX1+pview_attr->viewWidth,pview_attr->viewY1+pview_attr->viewHeight, &disp_info);
	}
	
	//����ǵ�ǰ����,�������滭�߿�
	Grap_DrawFocusRect(pview_attr, COLOR_RED);
	
	return SUCCESS;

}

/*---------------------------------------------
*����:Set_Gps
*����:GPS����
*����:
*����:
*-----------------------------------------------*/
void Set_Gps(void *pDeskFather)
{
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON *pBtn;
	tGRAPEDIT *pEdtColl;
	tGRAPLIST *pList;
	tGRAPBUTTON *plab;
	U8 i;
	int idex=0;
	
	pDesk=Grap_CreateDeskbox(&Set_GpsAtrr[idex++], "GPS����");
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
		pBtn=Grap_InsertButton(pDesk, &Set_GpsAtrr[idex++], &Set_GpsBtnAttr[i]);
		if (NULL==pBtn)
		{
			return;
		}
		if(i < 2)
		{
			pBtn->pressEnter = Set_Gps_BtnEnter;
		}
	}
	/*����༭��*/
	pEdtColl= Grap_InsertEdit(pDesk, &Set_GpsAtrr[idex++], &Set_GpsEditAttr[0]);
	pEdtColl->DrawInputCnt = Set_EditDrawInputCnt;
	if (NULL==pEdtColl)
	{
		return;
	}
	sprintf(pEdtColl->edit_privattr.digt, "%d", gtHyc.gpsGetInterval);
	
	pEdtColl= Grap_InsertEdit(pDesk, &Set_GpsAtrr[idex++], &Set_GpsEditAttr[0]);
	pEdtColl->DrawInputCnt = Set_EditDrawInputCnt;
	if (NULL==pEdtColl)
	{
		return;
	}
	sprintf(pEdtColl->edit_privattr.digt, "%d", gtHyc.gpsSendInterval);
	
	/*����List ��ѡ��*/
	pList =   Grap_InsertLister(pDesk, &Set_GpsAtrr[idex++], &Set_GpsListAttr[0]);
	if (pList == NULL) return;
	
	pList->getListString = Set_Gps_ListGetString;
	pList->drawIcon      = Set_Gps_ListDrawIcon;
	pList->draw = Set_Gps_ListerDraw;
	
	pList->lst_privattr.focusItem  = gtHyc.gpsSwitch;
	
	/*������Ϣѭ�����ͷ�*/
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);	
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}
