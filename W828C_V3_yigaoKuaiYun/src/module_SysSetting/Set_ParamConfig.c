#include "SysSetting.h"


/*
1:�Ƿ������������
2:�����Ƿ�����Ϊ��
3:ǩ���Ƿ��������
4:
*/


const tGrapViewAttr Set_ParamConfigAtrr[]=
{
	{COMM_DESKBOX_ID,0,  0, 240, 320, 0,0,0,0,         	  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/	
#else
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/	
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
#endif	
	{BUTTON_ID_3,	10, 50 ,210, 20,    10,50,220,70,   SZ_BX_K_EN,SZ_XZ_K_EN,0,0,1,0,0,0},//�Ƿ������������
	{BUTTON_ID_4,	10, 80 ,210, 20,    10,80,220,100,   SZ_BX_K_EN,SZ_XZ_K_EN,0,0,1,0,0,0},//�����Ƿ�����Ϊ��
	{BUTTON_ID_5,	10, 110 ,210, 20,   10,100,220,130,  SZ_BX_K_EN,SZ_XZ_K_EN,0,0,1,0,0,0},//ǩ���Ƿ��������
	
};

/*btn ����*/
const tGrapButtonPriv Set_ParamConfigBtnAttr[]=
{
#ifdef W818C
	{(U8 *)"ȷ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  	TY_UNITY_BG1_EN,NULL},
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,    TY_UNITY_BG1_EN,NULL},
#else
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,    TY_UNITY_BG1_EN,NULL},
	{(U8 *)"ȷ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  	TY_UNITY_BG1_EN,NULL},
#endif	
	{(U8 *)"������������", 0xffff,COLOR_YELLOW,0,0,1,0, TY_UNITY_BG1_EN,NULL},
	{(U8 *)"��������Ϊ��", 0xffff,COLOR_YELLOW,0,0,1,0, TY_UNITY_BG1_EN,NULL},
	{(U8 *)"ǩ�ձ�������", 0xffff,COLOR_YELLOW,0,0,1,0, TY_UNITY_BG1_EN,NULL},
};

S32 Set_ParamConfig_BtnEnter(void *pView,U16 state)
{
	tGRAPBUTTON *pBtn;
	tGRAPDESKBOX *pDesk;
	U32  id,val,bit;

	pBtn=(tGRAPBUTTON*)pView;
	pDesk=(tGRAPDESKBOX*)pBtn->pParent;
	id=pBtn->view_attr.id;
	if (state==STATE_NORMAL)
	{
		switch (id)
		{
		case BUTTON_ID_1:
			if(gu32SettingTemp != gtHyc.ParamCfg)	
			{
				gtHyc.ParamCfg = gu32SettingTemp;
				
				glbAllVariable_SaveParameter();
				Com_SpcDlgDeskbox("���óɹ�!",0,pDesk,1,NULL,NULL,100);
				return RETURN_QUIT;
			}
			break;
		case BUTTON_ID_2:
			return RETURN_QUIT;
			break;
		case BUTTON_ID_3://�Ƿ������������
		case BUTTON_ID_4://�����Ƿ�����Ϊ��
		case BUTTON_ID_5://ǩ���Ƿ��������
			bit = id-BUTTON_ID_3;
			val = 0x01<<bit;
			gu32SettingTemp ^= val;
			break;
		default:
			break;
		}
		
		return RETURN_REDRAW;
	}
	return SUCCESS;
}

S32 Set_ParamConfig_BtnDraw(void *pView, U32 xy, U32 wh)
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
	
	if( (gu32SettingTemp>>(pview_attr->id-BUTTON_ID_3)) & 0x01)
	{
		Grap_ShowPicture(SZ_XZ_K_EN, viewX1, viewY1);
	}
	else
	{
		Grap_ShowPicture(SZ_BX_K_EN, viewX1, viewY1);
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
*����:Set_ParamConfig
*����:ϵͳ��������
*����:
*����:
*-----------------------------------------------*/
void Set_ParamConfig(void *pDeskFather)
{
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON *pBtn;
	U8 i;
	int indx = 0;
	
	gu32SettingTemp = gtHyc.ParamCfg;
	
	pDesk=Grap_CreateDeskbox(&Set_ParamConfigAtrr[indx++],"��������");
	if (NULL==pDesk)
	{
		return;
	}
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	for(i = 0;i < 5;i++)
	{
		pBtn=Grap_InsertButton(pDesk, &Set_ParamConfigAtrr[indx++], &Set_ParamConfigBtnAttr[i]);
		if (NULL==pBtn)
		{
			return;
		}
		pBtn->pressEnter=Set_ParamConfig_BtnEnter;
		if(i >= 2)
		{
			pBtn->draw = Set_ParamConfig_BtnDraw;
		}
	}
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);	
	Grap_DeskboxDestroy(pDesk);
}
