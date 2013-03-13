
/*
*include local files
*/
#include "std.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "glbVariable_base.h"
#include "glbVariable.h"
#include "Battery.h"
#include "LcdModule_BackLight.h"
#include "TcpIp_p.h"

#include "app_access_sim.h"
#include "Net_Evt.h"

/**************************************** ���涥�������ؼ� *******************************************/
tGRAPLABEL *pgtComFirstChd=NULL;

static U8  	gsu8GprsStep;
static U8 	su8ComCount,su8Comflag;
static U8	gsu8DateShowType;//��ǰ��ʾ�������ڻ���ʱ��

static U32 gsm_sigq_level = 0;

static const U8 keyBmp[][30] =  /* 20*10 */
{
	{0x00,0x00,0x00,0x18,0x71,0xC0,0x38,0xFB,0xE0,0x18,0x18,0x60,0x18,0x19,0xC0,0x18,
	0x31,0xC0,0x18,0x60,0x60,0x3C,0xFB,0xE0,0x7E,0xFB,0xC0,0x00,0x00,0x00},//123
	{0x00,0x00,0x00,0x3E,0x00,0x00,0x7F,0x00,0x00,0x03,0x00,0x00,0x1F,0x00,0x00,0x3F,
	0x00,0x00,0x73,0x00,0x00,0x7F,0x00,0x00,0x3F,0x80,0x00,0x00,0x00,0x00},//a
	{0x00,0x00,0x00,0x0C,0x00,0x00,0x1E,0x00,0x00,0x1E,0x00,0x00,0x33,0x00,0x00,0x33,
	0x00,0x00,0x3F,0x00,0x00,0x7F,0x80,0x00,0x61,0x80,0x00,0x00,0x00,0x00},//A
	{0x7F,0xC0,0x00,0x50,0x40,0x00,0x1F,0x00,0x00,0x10,0x00,0x00,0x1F,0xC0,0x00,0x00,
	0xC0,0x00,0x7F,0xC0,0x00,0x00,0xC0,0x00,0x00,0xC0,0x00,0x01,0x80,0x00},//д
	{0x22,0x40,0x00,0x22,0x80,0x00,0x77,0xE0,0x00,0x22,0x40,0x00,0x22,0x40,0x00,0xF7,
	0xF0,0x00,0x22,0x40,0x00,0x22,0x40,0x00,0x24,0x40,0x00,0x68,0x40,0x00},//ƴ
};

const U8 Gprs_BMP[][9] = //Gprs  8X9
{
	0x18,0x18,0xFF,0x7E,0x3C,0x18,0x00,0x00,0x00,//����1
	0x00,0x00,0x00,0x18,0x18,0xFF,0x7E,0x3C,0x18,//����2
	
	0x00,0x00,0x00,0x18,0x3C,0x7E,0xFF,0x18,0x18,//�ϴ�1
	0x18,0x3C,0x7E,0xFF,0x18,0x18,0x00,0x00,0x00,//�ϴ�2
};

/*
logo(���¼����Ϣ)   ��¼����+GPRS������ͷ+GPRS��ǰ�¼�    ����ʱ��   GSM�ź�   WIFI�ź�  ���    ���뷨
*/
const tGrapViewAttr gtComView[]=
{
	
	{COM_LOGO_ID,	LOGO_X,	LOGO_Y,	LOGO_W,	LOGO_H,	0,0,0,0,	TY_YUNDA_LOGO_EN,TY_YUNDA_LOGO_EN, 0,0, 1,0,0,0},//LOGO
	{COM_GPRSEVT_ID,GPRS_X,	GPRS_Y,	GPRS_W,	GPRS_H,	0,0,0,0,	0,0,	0,0,	1,0,0,0},//GPRS�¼�
	{COM_DATE_ID,	DATE_X,	DATE_Y,	DATE_W,	DATE_H,	0,0,0,0,	0,0,	0,0,	1,0,0,0},//����ʱ��
	{COM_GSMSIG_ID,	GSM_X,	GSM_Y,	GSM_W,	GSM_H,	GSMWIFI_X1,GSMWIFI_Y1,GSMWIFI_X2,GSMWIFI_Y2,	TY_G_NO_SIGNAL_EN,TY_G_NO_SIGNAL_EN,	0,0,	1,0,0,0},//gsm�ź�
	{COM_WIFISIG_ID,WIFI_X,	WIFI_Y,	WIFI_W,	WIFI_H,	GSMWIFI_X1,GSMWIFI_Y1,GSMWIFI_X2,GSMWIFI_Y2,	TY_W_NO_SIGNAL_EN,TY_W_NO_SIGNAL_EN,	0,0,	1,0,0,0},//wifi�ź�
	{COM_BAT_ID,	BAT_X,	BAT_Y,	BAT_W,	BAT_H,	0,0,0,0,	0,0,	0,0,	1,0,0,0},//���
	{COM_INPUT_ID,	INPUT_X,INPUT_Y,INPUT_W,INPUT_H,0,0,0,0,	0,0,	0,0,	1,0,0,0},//���뷨
	
};

const tGrapLabelPriv gtComLabPriv[]=
{
	{NULL, 0xffff, 0xffff, 0, 0, 0, 0, 0},//logo
	{NULL, 0xffff, 0xffff, 0, 0, 0, 0, 0},//GPRS
	{NULL, COLOR_YELLOW, COLOR_YELLOW, FONTSIZE6X9, 0, 0, 0, 0},//����ʱ��
	{NULL, 0xffff, 0xffff, 0, 0, 0, 0, 0},//gsm
	{NULL, 0xffff, 0xffff, 0, 0, 0, 0, 0},//wifi
	{NULL, 0xffff, 0xffff, 0, 0, 0, 0, 0},//���
	{NULL, COLOR_YELLOW, COLOR_YELLOW, 0, 0, 0, 0, 0},//���뷨
};

/*---------------------------------------------------------
*����: ComWidget_Init
*����: 
*����: none
*����: 1
*---------------------------------------------------------*/
void ComWidget_Init(void)
{
	gsu8GprsStep = 0;
	gsu8DateShowType = 0;
	gsm_sigq_level = 0;
	SetUpDateState(0);
	
	su8ComCount = 0;
	su8Comflag = 1;	
}

/*---------------------------------------------------------
*����: Com_DlgButtonPress
*����: btn�������º�Ĳ���
*����: 
*����: none
*---------------------------------------------------------*/
static S32 ComWidget_EnterBtnPress(void *pView, U16 state)
{
	tGRAPBUTTON *pBtn;
	tGRAPDESKBOX *pDesk;
	
	pBtn = (tGRAPBUTTON *)pView;
	
	if (state != STATE_PRESS)
	{
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		Com_DlgUpdataStat(DLG_CONFIRM_EN);
		
		if(FALSE == hyudsDcd_Connected())
		{
			Com_DlgUpdataStat(DLG_CANCEL_EN);
		}
		
		pDesk->kDlgState = Com_DlgGetStat();
		
		return RETURN_QUIT;
	}
	
	return RETURN_CANCLE;
}

/*---------------------------------------------------------
*����: ComWidget_RevUSBMsg
*����: ʹ�ù����ռ����USB��Ϣ       
*����: none
*����: 
*---------------------------------------------------------*/ 
void ComWidget_RevUSBMsg(void *pView)
{
    tGRAPLABEL *pLabel = (tGRAPLABEL *)pView;
    tGRAPDESKBOX *pDesk;
    tMSG_BODY *pMsgBody;
    U8 tempdata[40]={0};
    tMSG_PUT_PARAM	*ptPutMsg = tempdata;
    
    pDesk = pLabel->pParent;
    
    pMsgBody = pDesk->ptGetMsgBody;    
    
    if (USE_USB_IS_ACT == pMsgBody->id)
    {
        ptPutMsg->body.id = USE_USB_IGNORE;         
        
        BackLight_EventTurnOnLcd(pDesk); 
	#if 0
		if(Gprs_getMode() > 0 && 0 == (YDget_gprsevent()&YD_GPRS_FOREGROUND)) /* ֻ�Ǻ�̨�¼���ȥ�ȴ� */
		{	
	        if(DLG_CONFIRM_EN == Com_ShutDownDlgDeskbox(pDesk,"�������ڴ���,��ɱ��δ����ǹ���Զ�����USB!", Com_Usb_ShutDownLabelTimerISR, DLG_ALLWAYS_SHOW))
	        {
	        	YDreset_gprsevent();
	            ptPutMsg->body.id = USE_USB_ENTER_ACT;
	            pLabel->view_attr.enable = 0 ;  /* �رո�comGolb�ؼ��������labelTimerISR */
	        }
        }
        else 
    #endif
       	{
       		if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�Ƿ�����USB?",0, pDesk, 3,ComWidget_EnterBtnPress,NULL,DLG_DEFAULT_TIME))
	        //if (DLG_CONFIRM_EN == Com_DlgDeskbox(pDesk,8,cDIALOG_IS_ENTER_USB,ComWidget_EnterBtnPress,NULL,DLG_DEFAULT_TIME))
	        {        	
	            ptPutMsg->body.id = USE_USB_ENTER_ACT;
	        }
        } 
        ptPutMsg->length  = sizeof(eMSG_ID);
        ptPutMsg->priority = 10;
       
       	//ͼƬģ��δ����
		ReDraw(pDesk,0,0);
		
        msg_queue_put(ptMsg_controlDesk, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
    }
	
	return ;
}


/*---------------------------------------------------------
*����: ComWidget_CheckObjAttar
*����: ����źź͵绰�ȹ���ͼ�꣬������     
*����: pDesk ����ָ��
*����: 
*---------------------------------------------------------*/
void ComWidget_CheckObjAttar(void *pView)
{
	tGRAPLABEL 		*pLab_Gsm,*pLab_Wifi,*pLab_Bat;
	tGRAPDESKBOX	*pDesk; 
	U32 lev,idx;
    
    pDesk = ((tGRAPVIEW *)pView)->pParent;
    
    pLab_Gsm	= Grap_GetViewByID(pDesk,COM_GSMSIG_ID);
    pLab_Wifi	= Grap_GetViewByID(pDesk,COM_WIFISIG_ID);
    pLab_Bat	= Grap_GetViewByID(pDesk,COM_BAT_ID);

    if(NULL == pLab_Gsm || NULL == pLab_Wifi || NULL == pLab_Bat)
    {
    	return ;
    }
	
	if(gtHyc.NetSelected == 1)//ѡ���gsm
	{
		//gsm
		if(BackLight_HaveLight() == 1)
		{//û������ʱ
			pLab_Gsm->view_attr.clNormal = TY_G_NO_SIGNAL_EN + gsm_sigq_level;
			pLab_Gsm->view_attr.clFocus  = TY_G_NO_SIGNAL_EN + gsm_sigq_level;
		}
	}	
	else
	{
		//wifi  ����Ѿ�����  ��Ҫ�ж��ź�
		if(SUCCESS == netdev_get_tcpip_status())
		{
			idx = hy_get_ap_rssi_range(hy_get_connected_ap_rssi());  			 

			if (idx >= 4) idx = TY_W_SIGNAL4_EN;
			else idx = TY_W_NO_SIGNAL_EN+idx;
			
			pLab_Wifi->view_attr.clNormal = idx;
			pLab_Wifi->view_attr.clFocus  = idx;
		}
		else
		{
			pLab_Wifi->view_attr.clNormal = TY_W_NO_SIGNAL_EN;
			pLab_Wifi->view_attr.clFocus  = TY_W_NO_SIGNAL_EN;
		}
	}
	
	//battery
	lev = Battery_Level();
	if(lev > 4) lev = 4;
	
	if(1 == Battery_DcInCheck())
	{
	    if (0 == usb_need_charge())
	    {
            pLab_Bat->view_attr.clNormal = TY_BATTERY5_EN;
            pLab_Bat->view_attr.clFocus  = TY_BATTERY5_EN;
	    }
	    else
	    {
    	    pLab_Bat->view_attr.clNormal = TY_BATTERY1_CHARGE_EN+lev;
    	    pLab_Bat->view_attr.clFocus =  TY_BATTERY1_CHARGE_EN+lev;
	    }
	}
	else
	{
	    pLab_Bat->view_attr.clNormal = TY_BATTERY1_EN+lev;
	    pLab_Bat->view_attr.clFocus  = TY_BATTERY1_EN+lev;
	}

	return ;
}

/*---------------------------------------------------------
*����: ComWidget_LableEnter
*����: �������ͼ�굱����ݼ���������ܽ���       
*����: none
*����:  
*---------------------------------------------------------*/ 
int ComWidget_LableEnter(void *pView, U16 viewX, U16 viewY)
{
	tGRAPDESKBOX 	*pDesk;
	tGRAPLABEL 		*pLable;
	eDLG_STAT		kDlgState;
	U16				id1,id2;
	
	pLable = (tGRAPLABEL *)pView;
	switch(pLable->view_attr.id)
	{
	case COM_LOGO_ID://LOGO
        break;
    case COM_GPRSEVT_ID://GPRS�¼�
        break;
    case COM_DATE_ID://����ʱ��
        break;
    case COM_GSMSIG_ID://GSM�ź�
    case COM_WIFISIG_ID://WIFI�ź�
    	//��������ѡ��
        pDesk = (tGRAPDESKBOX *)(pLable->pParent);
        
    	if(0x12345678 == pDesk->view_attr.reserve)//wifi��ؽ���
    	{
    		break;
    	}
    	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		Set_NetSelect(pDesk);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
        break;
    case COM_BAT_ID://���
        break;
    default:
        break;
	}
    
    return RETURN_REDRAW;
}

/*---------------------------------------------------------
*����: ComWidget_LabelRecvPullMsg
*����: ��ק������       
*����: none
*����: 
*---------------------------------------------------------*/
int ComWidget_LabelRecvPullMsg(void *pView,GRAP_EVENT *pEvent)
{
    U32 extParam, message, wParam, lParam ;
	tGRAPLABEL *pLable;
	tGrapLabelPriv *pLable_privattr;
	tLabelTimer *pLable_timer;
	tGRAPVIEW *pTempView;
	S32 ret = FAIL;
	int x, y ;
	U32 father_id;
	U32 id;
  
	pLable = (tGRAPLABEL *)pView;
	pLable_privattr = &(pLable->label_privattr);
	pLable_timer = &(pLable->label_timer);

    message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;

	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;
	
	id = pLable->view_attr.id;
	
	if (VIEW_PULL_START == extParam)
	{//����ʼ��

		if(GRAP_INWIDGET_RANGE(x,y,pLable))
	    {//����Ч��Χ��
	         
	         ret = VIEW_PULL_START; 
	    }
	    else
	    {
	        return FAIL;
	    }
	}
	
	if (VIEW_PULL_START == extParam)
	{//down 
	}
	else if (VIEW_PULL_ING == extParam)
	{//��ק��
	     ret = VIEW_PULL_ING;
	}
	else if (VIEW_PULL_END == extParam)
	{//up 	    			

        if(!GRAP_INWIDGET_RANGE(x,y,pLable)) 
        {
             return VIEW_PULL_END;
        }
        
        father_id = ((tGRAPDESKBOX *)pLable->pParent)->view_attr.id;
		
		ret = pLable->labelEnter(pLable,x,y);
		if ( ret== RETURN_REDRAW)
		{
			ReDraw(pLable, 0, 0);    				
		}
		else if(ret == RETURN_QUIT)
		{
			*((tGRAPDESKBOX *)(pLable->pParent))->pQuitCnt = 1;
		}
		else
		{
			pLable->draw(pLable, 0, 0);
		}
		ret = VIEW_PULL_END;

	}
	else if (VIEW_PULL_TIMEOUT == extParam)
	{//��ʱ
	    ret = VIEW_PULL_TIMEOUT;  
	}
	
	return ret;
}


/*--------------------------------------
logo �ؼ�ˢ��  �е�¼��Ϣ��Ҫ��ʾʱ��ʾ��¼��Ϣ  ������ʾlogo
----------------------------------------*/
int ComWidget_LogoDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPLABEL		*pLable;
	tStrDispInfo	disp_info;
	tGrapViewAttr	*pview_attr;
	tGrapLabelPriv	*pLable_privattr;
	U8	disBuf[10];
	
	pLable = (tGRAPLABEL *)pView;
	pview_attr = &(pLable->view_attr);
	pLable_privattr = &(pLable->label_privattr);
	
	pDesk = (tGRAPDESKBOX *)(pLable->pParent);
	
	//��ʾ��ҳ������
	disp_info.string = pDesk->pDisName;
	if(disp_info.string == NULL)
	{//4,  3, 51, 21
		if(pview_attr->clNormal)
		{
			Grap_ShowPicture(pview_attr->clNormal, 4, 3);
		}
		else
		{
			Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
		}
	}
	else//�е�¼��Ϣ
	{
		if(strlen(disp_info.string) > 8)
		{
			memcpy(disBuf,disp_info.string, 8);
			disBuf[8] = 0;
			disp_info.string = disBuf;
		}
		
		Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
		
		disp_info.color = pLable_privattr->cl_normal;
		disp_info.font_size = pLable_privattr->font_size;
		disp_info.font_type = pLable_privattr->font_type;
		disp_info.flag = pLable_privattr->position;
		disp_info.keyStr = NULL;
		Grap_WriteString(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight), &disp_info);
	}
	
	return SUCCESS;
}

/*--------------------------------------
gprs �¼��ؼ�ˢ��  ���� ��¼����  ��ͷ  ��ǰ�¼�
----------------------------------------*/
int ComWidget_GprsEvtDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLABEL		*pLable;
	tStrDispInfo	disp_info;
	tGrapViewAttr	*pview_attr;
	tGrapLabelPriv	*pLable_privattr;
	U32				recTotal, gprsEvt;
	U8				direct,disBuf[20];
	
	pLable = (tGRAPLABEL *)pView;
	pview_attr = &(pLable->view_attr);
	pLable_privattr = &(pLable->label_privattr);
	
	Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight*2));
	
	disp_info.string 	= disBuf;
	disp_info.font_size = 0;
	disp_info.font_type = 0;
	disp_info.flag = 1;
	
	//��ȡ��ǰλ�ϴ���¼����  �����ʾ4λ
	recTotal = Rec_GetUnSendCnt();
	//if(recTotal > 0)
	{
		disp_info.color = pLable_privattr->cl_normal;
		if(recTotal > 9999)
		{
			recTotal = 9999;
			disp_info.color = COLOR_RED;
		}
		sprintf(disBuf, "��¼%d", recTotal);
		
		disp_info.keyStr = NULL;
		Grap_WriteString(pview_attr->viewX1+8, pview_attr->viewY1, 
					pview_attr->viewX1+pview_attr->viewWidth, pview_attr->viewY1+pview_attr->viewHeight, &disp_info);
	}
	recTotal = Rec_GetUnSendImageCnt();
	//if(recTotal > 0)
	{
		disp_info.color = COLOR_YELLOW;
		if(recTotal > 9999)
		{
			recTotal = 9999;
			disp_info.color = COLOR_RED;
		}
		sprintf(disBuf, "ͼƬ%d", recTotal);
		
		disp_info.keyStr = NULL;
		Grap_WriteString(pview_attr->viewX1+8, pview_attr->viewY1+pview_attr->viewHeight, 
					pview_attr->viewX1+pview_attr->viewWidth, pview_attr->viewY1+pview_attr->viewHeight*2, &disp_info);
	}
	
	//��ȡGPRS��ǰ�¼� ���¼����Ϳ���֪����ͷ����
	Net_GetEvtStr(disBuf, &direct);
	if(disBuf[0] != 0 && (direct == 0 || direct == 1))//���¼�
	{
		direct *= 2;
		//gprs ��ͷ  104
		drvLcd_SetColor(0xffff,0xffff);    
    	drvLcd_WriteBMPColor((const U8 *)Gprs_BMP[direct+gsu8GprsStep],9,8,pview_attr->viewY1,pview_attr->viewX1);
		gsu8GprsStep = !gsu8GprsStep;
		
		//�¼�
//		disp_info.color  	= pLable_privattr->cl_normal;
//		Grap_WriteString(117, 3, 181, 19, &disp_info);
	}
	
	return SUCCESS;
}

/*--------------------------------------
����ʱ�� �ؼ�ˢ��
----------------------------------------*/
int ComWidget_DateDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPLABEL		*pLable;
	tStrDispInfo	disp_info;
	tGrapViewAttr	*pview_attr;
	tGrapLabelPriv	*pLable_privattr;
	U8				disBuf[20];
	
	pLable = (tGRAPLABEL *)pView;
	pview_attr = &(pLable->view_attr);
	pLable_privattr = &(pLable->label_privattr);
	
	pDesk = (tGRAPDESKBOX *)(pLable->pParent);
	
	Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
				
	//��λ������������Ľ������ʾ�����������ַ����������ַ���,������ʾ��¼������
	if(pDesk->inputMode != 0)//�����뷨
	{
		//�˴���������,�ɸ��ؼ��Լ�����draw��������
		//Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
		//	(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
	}
	/*else
	{
		Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight+18));
		
		disp_info.string = Login_GetName();
		if(disp_info.string != NULL)
		{
			if(strlen(disp_info.string) > 8)
			{
				memcpy(disBuf,disp_info.string, 8);
				disBuf[8] = 0;
				disp_info.string = disBuf;
			}
			
			disp_info.color = 0xffff;
			disp_info.font_size = 0;
			disp_info.font_type = 0;
			disp_info.flag = 2;
			
			disp_info.keyStr = NULL;
			Grap_WriteString(pview_attr->viewX1, pview_attr->viewY1 + pview_attr->viewHeight+1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight+17), &disp_info);
		}
	}*/
		
	if(gsu8DateShowType == 0)//��ʾ����
	{
		//sprintf(disBuf, "%02d-%02d-%02d", (gtRtc_solar.year%100), gtRtc_solar.month, gtRtc_solar.day);
		sprintf(disBuf, "%02d-%02d", gtRtc_solar.month, gtRtc_solar.day);
	}
	else if(gsu8DateShowType == 1)//��ʾʱ��
	{
		sprintf(disBuf, "%02d:%02d", gtRtc_solar.hour, gtRtc_solar.minute);
	}
	
	disp_info.string 	= disBuf;
	disp_info.color  	= pLable_privattr->cl_normal;
	disp_info.font_size = pLable_privattr->font_size;
	disp_info.font_type = 0;
	disp_info.flag = 2;
	disp_info.keyStr = NULL;
	Grap_WriteString(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight), &disp_info);
	
	return SUCCESS;
}

/*--------------------------------------
gsm �ź� �ؼ�ˢ��
----------------------------------------*/
int ComWidget_GsmSigDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLABEL		*pLable;
	tGrapViewAttr	*pview_attr;
	
	pLable = (tGRAPLABEL *)pView;
	pview_attr = &(pLable->view_attr);
	
	Grap_ShowPicture(pview_attr->clFocus, pview_attr->viewX1, pview_attr->viewY1);
	
	return SUCCESS;
}

/*---------------------------------------------
gsm �ź� timer
----------------------------------------------*/
void ComWidget_GsmSigTimer(void *pView)
{
	U32 sigq = 0;
	tGRAPLABEL *pLable;
	U16 sigq_id;
	int simcard=1,regStat;
	static U8  hasCard = 0;
	static U16 sigq_id_last = 0;
	static U32 sigqCnt=0,nocard = 0,nosigg=0,noreg = 0;
	
	if(hasCard == 0 && strlen(gsm_ReadSimCard()) > 0)
	{
		hasCard = 1;
	}
	
	if ((BackLight_HaveLight() == 0 && Rec_GetUnSendCnt() == 0 && 0 == Net_GetEvt()) || GsmReset_IfReset() == 1)
	{
		return ;
	}
	    
	pLable = (tGRAPLABEL *)pView;

	//����Ƿ���SIM��
	simcard = phone_detect_simcard_nrt();	
	if(simcard == 1)
	{		
		nocard = 0;
		//��ѯע��״̬�ɹ�
		regStat = net_query_reg_nrt();
		if(regStat == 1)
		{
			noreg = 0;
			sigq = net_query_sigq_nrt();
			sigq_id = (U16)(sigq>>16);

			if(sigq_id_last != sigq_id)
				sigq_id_last = sigq_id;
			
			sigq = (U32)(sigq & 0x0000FFFF);
		    if(sigq > 31)
		    {
		    	sigqCnt++;
		    	if(sigqCnt>=3)
		    	{
		    		sigq=0;
		    		sigqCnt=0;
		    	}
		    	else
		    	{
		    	    sigq = gsm_sigq_level;//return FAIL;
		    	}
		    }
		    else
		    {
		    	if(sigq==0)sigq = 0;
				else if(sigq < 11)sigq = 1;
				else if(sigq < 16)sigq = 2;
				else if(sigq < 21)sigq = 3;
				else sigq = 4;
				sigqCnt = 0;
			}
			
			if(sigq == 0)
			{
				nosigg++;
				if(nosigg >= 15)
				{
					nosigg = 0;
					PhoneTrace(0, "sigq");
					GsmReset_WakeUp();
				}
			}
			else
			{
				nosigg = 0;
			}
		}
		else
		{
			noreg++;
			if(noreg >= 15)
			{
				noreg = 0;
				PhoneTrace(0, "noreg");
				GsmReset_WakeUp();
			}
			sigq = 0;
		}
		
	}
	else
	{
		noreg = 0;
		nosigg= 0;
		sigq = 0;
		if(hasCard == 1)
		{
			nocard++;
			if(nocard >= 20)
			{
				nocard = 0;
				PhoneTrace(0, "nocard");
				GsmReset_WakeUp();
			}
		}
	}
	gsm_sigq_level = sigq;
	
	pLable->view_attr.clNormal = TY_G_NO_SIGNAL_EN + gsm_sigq_level;
	pLable->view_attr.clFocus  = TY_G_NO_SIGNAL_EN + gsm_sigq_level;
	
	pLable->draw(pLable,0,0);

	return ;
}

/*--------------------------------------
wifi �ź� �ؼ�ˢ��
----------------------------------------*/
int ComWidget_WifiSigDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLABEL		*pLable;
	tGrapViewAttr	*pview_attr;
	
	pLable = (tGRAPLABEL *)pView;
	pview_attr = &(pLable->view_attr);
	
	Grap_ShowPicture(pview_attr->clFocus, pview_attr->viewX1, pview_attr->viewY1);
	
	return SUCCESS;
}

/*---------------------------------------------
wifi �ź� timer
----------------------------------------------*/
void ComWidget_WifiSigTimer(void *pView)
{
	tGRAPLABEL		*pLable;
	U32 idx;
	
	pLable = (tGRAPLABEL *)pView;
	if (SUCCESS == netdev_get_tcpip_status())
	{
		idx = hy_get_ap_rssi_range(hy_get_connected_ap_rssi());  			 

		if (idx >= 4) idx = TY_W_SIGNAL4_EN;
		else idx = TY_W_NO_SIGNAL_EN+idx;
		
		pLable->view_attr.clNormal = idx;
		pLable->view_attr.clFocus  = idx;
	}
	else
	{
		pLable->view_attr.clNormal = TY_W_NO_SIGNAL_EN;
		pLable->view_attr.clFocus  = TY_W_NO_SIGNAL_EN;
	}
	
	pLable->draw(pLable,0,0);
}

/*--------------------------------------
��� �ؼ�ˢ��
----------------------------------------*/
int ComWidget_BatDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLABEL		*pLable;
	tGrapViewAttr	*pview_attr;
	
	pLable = (tGRAPLABEL *)pView;
	pview_attr = &(pLable->view_attr);
	
	Grap_ShowPicture(pview_attr->clFocus, pview_attr->viewX1, pview_attr->viewY1);
	
	return SUCCESS;
}

/*---------------------------------------------
Bat �ź� timer
----------------------------------------------*/
void ComWidget_BatTimer(void *pView)
{
	tGRAPLABEL		*pLable;
	U32 			lev, batIndex;
	static U8 lastframe = 0;
	static U8 batterLowCnt = 10;
	
	pLable = (tGRAPLABEL *)pView;
	
	lev = Battery_Level();
	if (lev > 4) lev = 4;
	if (1 == Battery_DcInCheck())
	{
        if(1 == usb_need_charge())
        {
	        lastframe++;
	        if (lastframe >= 5)
	        {
	            if(lev == 4) lastframe = 3;
	            else  lastframe = lev;
	        }
	        
	        batIndex = TY_BATTERY1_CHARGE_EN+lastframe;
		    
		    pLable->view_attr.clNormal = batIndex;
		    pLable->view_attr.clFocus  = batIndex;
		}
	}
	else
	{
    	pLable->view_attr.clNormal = TY_BATTERY1_EN+lev;
    	pLable->view_attr.clFocus  = TY_BATTERY1_EN+lev;
	}
	
	pLable->draw(pLable,0,0);
}

//���뷨
int ComWidget_InputDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPLABEL		*pLable;
	tGrapViewAttr	*pview_attr;
	
	pLable = (tGRAPLABEL *)pView;
	pview_attr = &(pLable->view_attr);

	pDesk = (tGRAPDESKBOX *)(pLable->pParent);
	
	Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1,  (U16)(pview_attr->viewX1 + pview_attr->viewWidth), 
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
	
	if(pDesk->inputMode > 0 && pDesk->inputMode <= 5)
	{
		drvLcd_SetColor(COLOR_YELLOW,COLOR_YELLOW);
		drvLcd_WriteBMPColor(keyBmp[pDesk->inputMode-1],10,20,pview_attr->viewY1,pview_attr->viewX1);
	}
	
	return SUCCESS;
}

//�����ؼ���timer��ʱ������
void ComWidget_TimerISR(void *pView)
{
	tGRAPLABEL *pLable;
	
	pLable = (tGRAPLABEL *)pView;
	
	switch(pLable->view_attr.id)
	{
	case COM_LOGO_ID://LOGO
		pLable->draw(pLable, 0, 0);
        break;
    case COM_GPRSEVT_ID://GPRS�¼�(��¼����  ��ͷ  ��ǰ�¼�)
        pLable->draw(pLable, 0, 0);
        break;
    case COM_DATE_ID://����ʱ��
        pLable->draw(pLable, 0, 0);
        gsu8DateShowType++;
		if(gsu8DateShowType > 1)
		{
			gsu8DateShowType = 0;
		}
        break;
    case COM_GSMSIG_ID://GSM�ź�
        ComWidget_GsmSigTimer(pLable);
        break;
    case COM_WIFISIG_ID://WIFI�ź�
        ComWidget_WifiSigTimer(pLable);
        break;
    case COM_BAT_ID://���
        ComWidget_BatTimer(pLable);
        break;
    default:
        break;
	}
}

/*---------------------------------------------------------
*����: ComWidget_ObjHandle
*����: ���������handle�����������������������       
*����: none
*����: 
*---------------------------------------------------------*/
int ComWidget_ObjHandle(void *pView, GRAP_EVENT *pEvent)
{
	tGRAPLABEL 		*pLable;
	tGrapLabelPriv 	*pLable_privattr;
	tGRAPDESKBOX	*pDesk;
	U32 id;
    S32 ret = SUCCESS;
	
	pLable = (tGRAPLABEL *)pView;
	pLable_privattr = &(pLable->label_privattr);
	
	pDesk = (tGRAPDESKBOX *)(pLable->pParent);
	
	id = pLable->view_attr.id;
	
	switch(pEvent->message)
	{
	case VIEW_DRAW:
	case VIEW_UPDATE:
		ComWidget_CheckObjAttar(pLable);
		pLable->draw(pLable, 0, 0);
		break;	
	case VIEW_TIMER:
		if (pLable->label_timer.enable == 1)
		{
			tLabelTimer *pLable_timer = &pLable->label_timer;
			
			if (pLable_timer->TimerLft == 0)
			{
				pLable_timer->TimerLft = pLable_timer->TimerTot;
				pLable->labelTimerISR(pLable);
			}
			else
			{
				S32 timeleft;
				
				timeleft = pLable_timer->TimerLft - pEvent->lParam;
				
				if (timeleft < 0) pLable_timer->TimerLft = 0;
				else  pLable_timer->TimerLft = timeleft;
			}
		}
		break;
	case VIEW_REV_SYSMSG:    
	    break;
	case VIEW_REV_PULLMSG:
	    ret = pLable->recvPullMsg(pLable,pEvent);
	    break;	
	default:
		break;
	}	
	
	return ret;
}

/*---------------------------------------------------------
*����: ComWidget_DestroyObj
*����: �ͷŶ���ָ�룬��û�������ͷŶ�����Ϊ����ȫ�ֵĶ���
*      ָ�룬������������һֱפ�����ڴ���       
*����: none
*����: 1
*---------------------------------------------------------*/
static S32 ComWidget_DestroyObj(void *pView)
{
	return SUCCESS;
}

/*---------------------------------------------------------
*����: ComWidget_CreatAllObj
*����: ���������ĵ�ض�������      
*����: none
*����: ��һ�������ؼ���ָ��
*---------------------------------------------------------*/
tGRAPLABEL *ComWidget_CreatAllObj(void)
{
	tGRAPLABEL *pLastObj=NULL,*pCreatObj=NULL;
	U32 i;
//LOGO GPRS ����ʱ��  gsm  wifi  ���  ���뷨
	U32 timetotal[]={100,//logo				     
					 100,//GPRS
					 500,//ʱ�������				 
					 200,//gsm
					 200,//wifi
					 200,//���
					 0
					 };

	for(i=0; i<COM_WIDGET_NUM; i++)//
	{
		pCreatObj = Grap_CreateLabel((tGrapViewAttr*)&gtComView[i],(tGrapLabelPriv*)&gtComLabPriv[i]);
		
		if(pCreatObj != NULL)
		{						
			pCreatObj->destroy = ComWidget_DestroyObj;
			pCreatObj->handle  = ComWidget_ObjHandle;
			pCreatObj->labelEnter = ComWidget_LableEnter;
			pCreatObj->recvPullMsg= ComWidget_LabelRecvPullMsg;
			
			if(timetotal[i] > 0)
				pCreatObj->label_timer.enable = 1;
			else
				pCreatObj->label_timer.enable = 0;
			pCreatObj->label_timer.TimerLft = 0;
			pCreatObj->label_timer.TimerTot = timetotal[i];
			pCreatObj->labelTimerISR = ComWidget_TimerISR;
			
			pCreatObj->pNext = (tGRAPVIEW *)pLastObj;
			
			pLastObj = pCreatObj;
			
			switch(i)
			{
			case 0://logo
				pCreatObj->draw = ComWidget_LogoDraw;
				break;
			case 1://gprs
				pCreatObj->draw = ComWidget_GprsEvtDraw;
				break;
			case 2://date
				pCreatObj->draw = ComWidget_DateDraw;
				break;
			case 3://gsm
				if(gtHyc.NetSelected != 1)//δѡgsm����
				{
					pCreatObj->view_attr.enable = 0;
				}
				pCreatObj->draw = ComWidget_GsmSigDraw;
				break;
			case 4://wifi
				if(gtHyc.NetSelected != 0)//δѡwifi����
				{
					pCreatObj->view_attr.enable = 0;
				}
				pCreatObj->draw = ComWidget_WifiSigDraw;
				break;
			case 5://bat
				pCreatObj->draw = ComWidget_BatDraw;
				break;
			case 6://input
				pCreatObj->draw = ComWidget_InputDraw;
				break;
			default:
				break;
			}
		}
	}
	
	return pCreatObj;
}
/*---------------------------------------------------------
*����: ComWidget_DeleteAllObj
*����: �ͷŹ�����������,ֻ���ڹػ�ʱ������ʹ������������ͷ����й�������      
*����: none
*����: ��һ�������ؼ���ָ��
*---------------------------------------------------------*/
void ComWidget_DeleteAllObj(void *pView)
{
	tGRAPLABEL *pSubChi,*pSubLast;
	
	if(NULL == pView)
	{
		return ;
	}
	
	pSubChi = (tGRAPLABEL *)pView;
	pSubLast = pSubChi;
	while(pSubChi)
	{
		pSubLast = pSubChi;
		pSubChi = (tGRAPLABEL *)pSubChi->pNext;
		free((void *)pSubLast);
	}
	
	return ;	
}


/*---------------------------------------------------------
*����: ComWidget_Link2Desk
*����: �ѹ����ռ����ӵ�desk��      
*����: pDesk��Ҫ���ӵ�deskָ�� pFirstCom:��һ���ؼ���ָ��
*����: 
*---------------------------------------------------------*/
void ComWidget_Link2Desk(void *pDesk,void *pFirstCom)
{
	tGRAPVIEW *pParentView;
	tGRAPVIEW *pSubChi;
	
	if(NULL == pDesk || NULL == pFirstCom)
	{
		return ;
	}	
	
	pParentView = (tGRAPVIEW *)pDesk;	
	
	pSubChi = (tGRAPVIEW *)pFirstCom;
	while(pSubChi)
	{
		
		pSubChi->pParent = pParentView;
		
		if(pSubChi->pNext == NULL) 
		{	
			pSubChi->pNext = ((tGRAPDESKBOX *)pParentView)->pNext;
			break;
		}
		pSubChi = pSubChi->pNext; 
	}	
	
	((tGRAPDESKBOX *)pParentView)->pNext = (tGRAPVIEW *)pFirstCom;
	
	return ;
}

/*---------------------------------------------------------
*����: ComWidget_Link2Desk
*����: �ѹ����ռ����ӵ�desk��      
*����: pDesk��Ҫ���ӵ�deskָ�� pFirstCom:��һ���ؼ���ָ�� TotalChild:���봰��ؼ�����
*����: 
*---------------------------------------------------------*/
void ComWidget_LeftFromDesk(void *pDesk,void *pFirstCom,U32 TotalChild)
{
	tGRAPVIEW *pParentView;
	tGRAPLABEL *pSubChi;
	 
	U32 i;
	
	if(NULL == pDesk || NULL == pFirstCom || TotalChild == 0)
	{
		return ;
	}
		
	pParentView = (tGRAPVIEW *)pDesk;	
	pSubChi = (tGRAPLABEL *)pFirstCom;
	
	for(i=0; i<TotalChild; i++)
	{
		if(i == TotalChild-1) 
		{
			((tGRAPDESKBOX *)pParentView)->pNext = (tGRAPVIEW *)pSubChi->pNext;
			pSubChi->pNext = NULL;
			break;
		}
		pSubChi = (tGRAPLABEL *)pSubChi->pNext;
		
	}
	
	return ;
}

