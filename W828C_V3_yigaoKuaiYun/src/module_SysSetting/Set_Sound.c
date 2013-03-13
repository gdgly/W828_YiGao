#include "SysSetting.h"

//��ʾ������
U8 gu8RemindType;

//������
U8 gu8KeySound;
			
//ɨ����
U8 gu8ScanSound;
/*=============================================================================================*/
const tGrapViewAttr    gtViewSetSoundAttr[] = 
{	
	{COMM_DESKBOX_ID,  	0,0,240,320,   		0,  0,  0,  0, 		TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},/*����   */
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
#else
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
#endif	
	{BUTTON_ID_3,	30, 50 ,200, 20,    30,50,230,70,   SZ_BX_K_EN,SZ_XZ_K_EN,0,0,1,0,0,0},//��ʾ������
	{BUTTON_ID_4,	30, 80 ,200, 20,    30,80,230,100,   SZ_BX_K_EN,SZ_XZ_K_EN,0,0,1,0,0,0},//ɨ��������
	{BUTTON_ID_5,	30, 110 ,200, 20,   30,100,230,130,  SZ_BX_K_EN,SZ_XZ_K_EN,0,0,1,0,0,0},//����������
	
};

const tGrapButtonPriv gtButnSetSoundAttr []=
{
#ifdef W818C
	{(U8 *)"ȷ��",  COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,TY_UNITY_BG1_EN,NULL},
	{(U8 *)"����",  COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,TY_UNITY_BG1_EN,NULL},
#else
	{(U8 *)"����",  COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,TY_UNITY_BG1_EN,NULL},
	{(U8 *)"ȷ��",  COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,TY_UNITY_BG1_EN,NULL},
#endif	
	{(U8 *)"��ʾ����",COLOR_STR_UNSELECT,COLOR_RED,0,0,0,0,TY_UNITY_BG1_EN,NULL},
	{(U8 *)"ɨ������",COLOR_STR_UNSELECT,COLOR_RED,0,0,0,0,TY_UNITY_BG1_EN,NULL},
	{(U8 *)"��������",COLOR_STR_UNSELECT,COLOR_RED,0,0,0,0,TY_UNITY_BG1_EN,NULL},
};



static S32 SetSound_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON		*pBtn;
	U32             id;
	S32             rv = SUCCESS;
	tGRAPDESKBOX *pDesk;

	pBtn 	= (tGRAPBUTTON *)pView;		
	id		= pBtn->view_attr.id;
	pDesk = (tGRAPDESKBOX *)pBtn->pParent;

	if (state == STATE_NORMAL)
	{
		switch( id )
		{
		case BUTTON_ID_1:/* ȷ�� */
			if(gtHyc.RemindType != gu8RemindType || gtHyc.KeySound != gu8KeySound || gtHyc.ScanSound != gu8ScanSound)
			{
				gtHyc.RemindType = gu8RemindType;
				gtHyc.KeySound   = gu8KeySound;
				gtHyc.ScanSound  = gu8ScanSound;
				
				glbVariable_SaveParameter();
			}
			Com_SpcDlgDeskbox("���óɹ�!",0,pDesk,1,NULL,NULL,100);
			rv = RETURN_QUIT;	
			break;
		case BUTTON_ID_2:/* ���� */
			rv = RETURN_QUIT;				
			break;
		case BUTTON_ID_3://��ʾ������
			gu8RemindType = !gu8RemindType;
			//hyUsbPrintf("gu8RemindType  = %d \r\n",gu8RemindType);
			rv = RETURN_REDRAW;
			break;
		case BUTTON_ID_4://ɨ��������
			gu8ScanSound = !gu8ScanSound;
			//hyUsbPrintf("gu8ScanSound  = %d \r\n",gu8ScanSound);
			rv = RETURN_REDRAW;
			break;
		case BUTTON_ID_5://����������
			gu8KeySound = !gu8KeySound;
			//hyUsbPrintf("gu8KeySound  = %d \r\n",gu8KeySound);
			rv = RETURN_REDRAW;
			break;
		default:
			break;
		}	
	}
   
	return rv ;	
}

S32 SetSound_BtnDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tStrDispInfo disp_info;
	tGrapButtonBuffPic *pbtnpic;
	U16 viewX1,viewY1,viewX2,viewY2;

	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
	
	viewX1 = pview_attr->viewX1;
	viewY1 = pview_attr->viewY1;
	viewX2 = viewX1 + pview_attr->viewWidth;
	viewY2 = viewY1 + pview_attr->viewHeight; 
	
	Grap_ClearScreen(viewX1, viewY1, viewX2,viewY2);
	
	if(pview_attr->id == BUTTON_ID_3)//��ʾ������
	{
	//hyUsbPrintf("gu8RemindType  = %d \r\n",gu8RemindType);
		if(gu8RemindType)
		{
			Grap_ShowPicture(SZ_XZ_K_EN, viewX1, viewY1);
		}
		else
		{
			Grap_ShowPicture(SZ_BX_K_EN, viewX1, viewY1);
		}
	}
	else if(pview_attr->id == BUTTON_ID_4)//ɨ��������
	{
	//hyUsbPrintf("gu8ScanSound  = %d \r\n",gu8ScanSound);
		if(gu8ScanSound)
		{
			Grap_ShowPicture(SZ_XZ_K_EN, viewX1, viewY1);
		}
		else
		{
			Grap_ShowPicture(SZ_BX_K_EN, viewX1, viewY1);
		}
	}
	else if(pview_attr->id == BUTTON_ID_5)//����������
	{
	//hyUsbPrintf("gu8KeySound  = %d \r\n",gu8KeySound);
		if(gu8KeySound)
		{
			Grap_ShowPicture(SZ_XZ_K_EN, viewX1, viewY1);
		}
		else
		{
			Grap_ShowPicture(SZ_BX_K_EN, viewX1, viewY1);
		}
	}

	if (pbtn_privattr->pName)//��ʾbutton�ϵ��ַ�
	{
		disp_info.string = pbtn_privattr->pName;

		if(pButton->state == STATE_NORMAL)
			disp_info.color = pbtn_privattr->cl_normal;
		else
			disp_info.color = pbtn_privattr->cl_focus;
		disp_info.font_size = pbtn_privattr->font_size;
		disp_info.font_type = pbtn_privattr->font_type;
		disp_info.flag = pbtn_privattr->position;
		disp_info.keyStr = NULL;
		Grap_WriteString(viewX1+25, viewY1+2, viewX2,viewY2, &disp_info);
	}
	
	return SUCCESS;
}

/*---------------------------------------------
*����:Set_Sound
*����:��������
*����:
*����:
*-----------------------------------------------*/
void Set_Sound(void *pDeskFather)
{
	tGRAPDESKBOX   *pDesk;
	tGRAPBUTTON    *pBtn;
	tGRAPLABEL     *pLabel;
	tGRAPLIST      *pList;
	tGRAPLIST      *pListScan;
	tGRAPLIST      *pListPress;
	U8             i;
	int idex=0;
	
	gu8RemindType = gtHyc.RemindType;
	gu8KeySound   = gtHyc.KeySound;
	gu8ScanSound  = gtHyc.ScanSound;
	
	pDesk = Grap_CreateDeskbox(&gtViewSetSoundAttr[idex++], "��������");
	if (NULL == pDesk ) return;
	Grap_Inherit_Public(pDeskFather,pDesk);

	/* ȷ��,���� */
	for(i = 0; i < 5; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &gtViewSetSoundAttr[idex++], &gtButnSetSoundAttr[i]);
		if (NULL == pBtn) return;
		pBtn->pressEnter = SetSound_BtnEnter;
		if(i >= 2)
		{
			pBtn->draw = SetSound_BtnDraw;
		}
	}
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}
