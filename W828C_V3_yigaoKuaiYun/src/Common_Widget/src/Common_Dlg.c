/*----------------------�ļ�����--------------------------------
*��������: 08/02/06 
*����    : �ܵ�UI���Ƚ��̣����Դ������߻�������UIģ�����
*--------------------------------------------------------------*/

/*
*include standard files
*/
#include "hyTypes.h"
#include "LcdAPIFunc.h"

/*
*include local files
*/
#include "grap_api.h"
#include "base.h"

#include "Common_Dlg.h" 
#include "glbVariable_base.h"

#include "beeperCtrl.h"
#include "drvKeyArray.h"

//����ʾ��Ϊ����ʱ��ʾ�ĶԻ���
#define COM_DLG_BCK_X  50 //�Ի�����ʾ���ܵı��� 10, 83,
#define COM_DLG_BCK_Y  60

#define COM_DLG_TIPICON_X   57
#define COM_DLG_TIPICON_Y   65

#define COM_DLG_ONLYCONFORM_X   84
#define COM_DLG_ONLYCONFORM_Y   128


#define COM_DLG_CONFORM_X  59
#define COM_DLG_CONFORM_Y  127
 
#define COM_DLG_CANCEL_X  163
#define COM_DLG_CANCEL_Y  127

static eDLG_STAT gu8DlgState = DLG_CANCEL_EN;

static U32 gu32DlgTimeOutTotal = 0;
static U32 gu32DlgStartTime = 0;

int gs32PercentTotal=0,gs32PercentRecv=0;

/*
*���ؼ��Ĺ�������
*/
const tGrapViewAttr gtComDlgViewAttr[]=
{
 //     id                  viewX1         viewY1   viewWidth  viewHeight touchX1  touchY1      touchX2          touchX2               color                          clNormal                          clFocus            *pLangIndex      enable   curFocus  FocusEnable reserve
	
	{DLG_DESKBOX_ID, 10, 102, 220, 110, 0, 0, 0, 0,  TY_DIALOGBAX_BACKGROUND1_EN,TY_DIALOGBAX_BACKGROUND1_EN,0, 0, 1,0,0,0},//����ʾ�Ի���
	{DLG_DESKBOX_ID, 10, 102, 220, 110, 0, 0, 0, 0,  TY_DIALOGBAX_BACKGROUND2_EN,TY_DIALOGBAX_BACKGROUND2_EN,0, 0, 1,0,0,0},//������
	
	//ȡ������	 2
	{BUTTON_ID_1, 45, 180, 151, 24, 45, 180, 196, 204, TY_NOSELECT_AFFIRM_LONG_EN,TY_SELECT_AFFIRM_LONG_EN,0, 0, 1,0,0,0},//��һ��ť
  	
  	//���� 3
  	{LABEL_ID_1,  10, 107, 220, 16,   0,0,0,0,    0,0,0,0, 1,0,0,0},
  	//���� 4
  	{LABEL_ID_2, 30,133, 180, 32,   0,0,0,0,    0,0,0,0, 1,0,0,0},
#ifdef W818C
  	{BUTTON_ID_2,30, 180, 83, 24, 30, 180, 113, 204, TY_NOSELECT_AFFIRM_EN,TY_SELECT_AFFIRM_EN,0, 0, 1,0,0,0},
  	//ȡ�� 6
  	{BUTTON_ID_3,127, 180, 83, 24,127,180, 210, 204, TY_NOSELECT_CANCEL_EN,TY_SELECT_CANCEL_EN,0, 0, 1,0,0,0},
#else
  	//ȷ�� 5
  	{BUTTON_ID_2,127, 180, 83, 24,127,180, 210, 204, TY_NOSELECT_AFFIRM_EN,TY_SELECT_AFFIRM_EN,0, 0, 1,0,0,0},
  	//ȡ�� 6
  	{BUTTON_ID_3,30, 180, 83, 24, 30, 180, 113, 204, TY_NOSELECT_CANCEL_EN,TY_SELECT_CANCEL_EN,0, 0, 1,0,0,0},
#endif  	
  	//���� 7
  	{CARTOON_ID_1,102, 107, 36, 35, 0, 0, 0, 0, TY_SEARCH_AP1_EN,TY_SEARCH_AP1_EN,0, 0, 1,0,0,0},

  	//���� 8 ��ʾ����ʱ��
  	{LABEL_ID_2, 30,144, 180, 36,   0,0,0,0,    0,0,0,0, 1,0,0,0},
};


const tGrapButtonPriv gtComDlgBtn[]=
{
	{(U8 *)"ȷ��",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,TY_DIALOGBAX_BACKGROUND1_EN,NULL},
	{(U8 *)"ȡ��",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,TY_DIALOGBAX_BACKGROUND1_EN,NULL},
};


const tGrapLabelPriv gtComDlgLabPri[]=
{
	//0xBDF7	
	//��ʾ ---����ť
	{(U8 *)"��ʾ",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,0,TY_DIALOGBAX_BACKGROUND1_EN},
	
	//����
	{(U8 *)"����",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,0,TY_DIALOGBAX_BACKGROUND1_EN},
	
	//ѯ��
	{(U8 *)"ѯ��",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,0,TY_DIALOGBAX_BACKGROUND1_EN},
	
	//����汾
	{(U8 *)"����汾",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,0,TY_DIALOGBAX_BACKGROUND1_EN},
	
	//������Ϣ
	{(U8 *)"������Ϣ",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,0,TY_DIALOGBAX_BACKGROUND1_EN},	
   
    //У��
	{(U8 *)"У��",COLOR_BLACK,COLOR_WHITE,0,0,SHOWSTRING_LIAN_MID,0,TY_DIALOGBAX_BACKGROUND1_EN},	
	
	//��ʾ��Ϣ  6
    {NULL,COLOR_BLACK,COLOR_BLACK,0,0,SHOWSTRING_LIAN_MID,0,TY_DIALOGBAX_BACKGROUND1_EN},
};


const U32 gu32ComDlgCtn[]=
{
    TY_SEARCH_AP8_EN,TY_SEARCH_AP7_EN,TY_SEARCH_AP6_EN,TY_SEARCH_AP5_EN,
    TY_SEARCH_AP4_EN,TY_SEARCH_AP3_EN,TY_SEARCH_AP2_EN,TY_SEARCH_AP1_EN   
};


const tGrapCartoonPriv gtComDlgCtn[]=
{
	{0,1,25,0,8,0,(U32 *)&gu32ComDlgCtn,TY_DIALOGBAX_BACKGROUND2_EN},
};



int Com_GetPercentTotal()
{
	return gs32PercentTotal;
}

void Com_SetPercentTotal(int total)
{
	gs32PercentTotal = total;
}

int Com_GetPercentRecv()
{
	return gs32PercentRecv;
}

void Com_SetPercentRecv(int recvsize)
{
	gs32PercentRecv = recvsize;
}

/*---------------------------------------------------------
*����: Com_DlgUpdataStat
*����: ���¶Ի���״̬
*����: stat �� �Ի���״̬
*����: none
*---------------------------------------------------------*/
void Com_DlgUpdataStat(eDLG_STAT stat)
{
    gu8DlgState = stat;     
}

U8 Com_DlgGetStat()
{
    return gu8DlgState;     
}

/*---------------------------------------------------------
*����: Com_DlgStartTime
*����: �Ի���ʼ��ʱ��
*����: none
*����: ��ʼʱ��
*---------------------------------------------------------*/
U32 Com_DlgStartTime(void)
{
    return gu32DlgStartTime;    
}

/*---------------------------------------------------------
*����: Com_DlgTotalLiveTime
*����: �Ի��������ʱ��
*����: none
*����: ����ʱ��
*---------------------------------------------------------*/
U32 Com_DlgTotalLiveTime(void)
{
    return gu32DlgTimeOutTotal;
}

/*---------------------------------------------------------
*����: Com_DlgTimeOut
*����: �Ի����Ƿ�ʱ
*����: none
*����: 1: ��ʱ 0��δ��ʱ
*---------------------------------------------------------*/
int Com_DlgTimeOut(void)
{
    int rc = FAIL;
    
    if (ABS(rawtime(NULL) - gu32DlgStartTime) >= gu32DlgTimeOutTotal) rc = SUCCESS;
    
    return rc;
}


/*---------------------------------------------------------
*����: ComWidget_LabTimerISR
*����: ����ź�ͼ��ʱ�������ж��¼�   
*����: pView �� lable point
*����: none 
*---------------------------------------------------------*/ 
S32 ComWidget_LabTimerISR(void *pView)
{
    S32 rc;
    tGRAPLABEL *pLable = (tGRAPLABEL *)pView;
    tGRAPDESKBOX *pDesk;
    
    pDesk = (tGRAPDESKBOX *)pLable->pParent;
    if ( 0xff != kernal_app_get_stat())
    {         
        Com_DlgUpdataStat(DLG_CONFIRM_EN);
        *pDesk->pQuitCnt = 1;       
    }
    else
    {
        if ( SUCCESS == Com_DlgTimeOut())
        {            
            Com_DlgUpdataStat(DLG_TIMEROUT_EN);
            *pDesk->pQuitCnt = 1;
        }
    }
    
    pDesk->kDlgState = Com_DlgGetStat();
    
    rc = SUCCESS;    
}


/*---------------------------------------------------------
*����: ComWidget_UpdataTimerMsg
*����: ����Ϣ�����е�timer��Ϣȥ�պ�    
*����: pDeskFather ����ָ��
*����: none 
*---------------------------------------------------------*/  
void ComWidget_UpdataTimerMsg(void *pDeskFather)
{
    U32 len,i,meslen[10],totallen=0;
    U32 num = 0;
    U32 sendMsgStatus;
    tGRAPDESKBOX *pDesk;
    char *pTempBuf = (char *)malloc(1000);
    
    if (NULL == pTempBuf) return;
    
    pDesk = (tGRAPDESKBOX *)(((tGRAPVIEW *)pDeskFather)->pParent);
    
    for (i=0; i<10; i++)
    {
    
        sendMsgStatus = msg_queue_tryget(pDesk->pGetMsg, (char *)pDesk->ptGetMsgBody, &len);
    
        if (sendMsgStatus != HY_OK)
        {
            break;
        }
        
        if (SYS_TIMER_EVENT  != pDesk->ptGetMsgBody->id)
        {
            memcpy(pTempBuf+totallen,(char *)pDesk->ptGetMsgBody,len); 
            
            meslen[num] = len;
            num++; 
            totallen += len;
        }
    
    }
    
    totallen = 0;
    for(i=0; i<num; i++)
    {
        msg_queue_tryput(pDesk->pGetMsg, pTempBuf+totallen, meslen[i], 6);
        totallen += meslen[i];  
        //i++;
    }
    
    Grap_UpdataLastTick();
    
    free(pTempBuf);
}

/*---------------------------------------------------------
*����: Com_DlgButtonPress
*����: btn�������º�Ĳ���
*����: 
*����: none
*---------------------------------------------------------*/
static S32 Com_DlgButtonPress(void *pView, U16 state)
{
	tGRAPBUTTON *pBtn;
	tGRAPDESKBOX *pDesk;
	
	pBtn = (tGRAPBUTTON *)pView;
	
	if (state != STATE_PRESS)
	{
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		pDesk->kDlgState = DLG_CONFIRM_EN;
		*pDesk->pQuitCnt = 1;
	}
	
	return RETURN_CANCLE;
}

/*---------------------------------------------------------
*����: Com_DlgButtonCancel
*����: btn�������º�Ĳ���
*����: 
*����: none
*---------------------------------------------------------*/
static S32 Com_DlgButtonCancel(void *pView, U16 state)
{
	tGRAPBUTTON *pBtn;
	tGRAPDESKBOX *pDesk;
	
	pBtn = (tGRAPBUTTON *)pView;
	
	if (state != STATE_PRESS)
	{
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		pDesk->kDlgState = DLG_CANCEL_EN;
		*pDesk->pQuitCnt = 1;
	}
	
	return RETURN_CANCLE;
}

/*---------------------------------------------------------
*����: Com_DlgLabelDrawInfor
*����: ��ʾ��ʾ����
*����: none
*����: 
*---------------------------------------------------------*/
static S32 Com_DlgLabelDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLABEL *pLabel;
	tStrDispInfo disp_info;
	tGrapViewAttr  *pview_attr;
	tGrapLabelPriv *plabel_privattr;
	
	pLabel = (tGRAPLABEL *)pView;
	pview_attr = &(pLabel->view_attr);
	plabel_privattr = &(pLabel->label_privattr);	

	if (plabel_privattr->pStr)
	{
		disp_info.string = plabel_privattr->pStr;

		if(plabel_privattr->focus)
			disp_info.color = plabel_privattr->cl_focus;
		else
			disp_info.color = plabel_privattr->cl_normal;
		disp_info.font_size = plabel_privattr->font_size;
		disp_info.font_type = plabel_privattr->font_type;
		disp_info.flag = plabel_privattr->position;	
		
		disp_info.keyStr = NULL;
		Grap_WriteString(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight), &disp_info);
	}

	return SUCCESS;
}

/*---------------------------------------------------------
*����: Com_DlgLabelDrawInfor
*����: ��ʾ��ʾ��Ϣ����
*����: none
*����: 
*---------------------------------------------------------*/
static S32 Com_DlgLabelDrawInfor(void *pView, U32 xy, U32 wh)
{
	tGRAPLABEL *pLabel;
	tStrDispInfo disp_info;
	tGrapViewAttr  *pview_attr;
	tGrapLabelPriv *plabel_privattr;
	
	pLabel = (tGRAPLABEL *)pView;
	pview_attr = &(pLabel->view_attr);
	plabel_privattr = &(pLabel->label_privattr);
	
	if(plabel_privattr->focus)
		disp_info.color = plabel_privattr->cl_focus;
	else
		disp_info.color = plabel_privattr->cl_normal;

	disp_info.font_size = plabel_privattr->font_size;
	disp_info.font_type = plabel_privattr->font_type;
	disp_info.flag = plabel_privattr->position;	
	
	disp_info.string = pLabel->label_privattr.pStr ;
	
	disp_info.keyStr = NULL;
	Grap_WriteStringMulti(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
					pview_attr->viewY1+48, &disp_info);
	
	return SUCCESS;
}

S32 ComWidget_LabTimerTimeOutISR(void *pView)
{
    S32 rc;
    tGRAPLABEL *pLable = (tGRAPLABEL *)pView;
    tGRAPDESKBOX *pDesk;
    
    pDesk = (tGRAPDESKBOX *)pLable->pParent;
   
    if ( SUCCESS == Com_DlgTimeOut())
    {            
        Com_DlgUpdataStat(DLG_TIMEROUT_EN);
        *pDesk->pQuitCnt = 1;   
    }
    
    pDesk->kDlgState = Com_DlgGetStat();
    
    rc = SUCCESS;    
}

static void Com_DlgLabelTimerISR(void *pView)
{
	tGRAPDESKBOX *pDesk;
	
	pDesk = (tGRAPDESKBOX *)((tGRAPLABEL *)pView)->pParent;
	pDesk->kDlgState = DLG_TIMEROUT_EN;
	*pDesk->pQuitCnt = 1;
	
	return;
}


/*---------------------------------------------------------
*����: Com_DlgLabelCheckUSBTimerISR
*����: pView�� ����Ƿ��Ѿ��Ͽ�USB
*����: none
*����: none
*---------------------------------------------------------*/
static void Com_DlgLabelCheckUSBTimerISR(void *pView)
{
	if (FALSE == hyudsDcd_Connected())
	{
	    *((tGRAPDESKBOX *)((tGRAPLABEL *)pView)->pParent)->pQuitCnt = 1;
	}

	return;
}

/*---------------------------------------------------------
*����: Com_DlgSrcLabelHandle
*����: ��Ҫ���ڼ���Ƿ�������ͼ��λ��
*����: none
*����: none
*---------------------------------------------------------*/
static S32 Com_DlgSrcLabelHandle(void *pView, GRAP_EVENT *pEvent)
{
	U32 message, wParam, lParam ;
	tGRAPLABEL *pLabel;
	tGrapLabelPriv *plabel_privattr;
	tLabelTimer *plabel_timer;
	int ret ;

	pLabel = (tGRAPLABEL *)pView;
	plabel_privattr = &(pLabel->label_privattr);
	plabel_timer = &(pLabel->label_timer);
	
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;

	switch(message)
	{
	case VIEW_DRAW:
	case VIEW_UPDATE:
		pLabel->draw(pLabel, 0, 0);
		break;
	case VIEW_SETLABELFOCUS:
		plabel_privattr->focus = 1;
		pLabel->draw(pLabel, 0, 0);
		break;
	case VIEW_LOSELABELFOCUS:
		plabel_privattr->focus = 0;
		pLabel->draw(pLabel, 0, 0);
		break; 	
	case VIEW_TIMER:
		if (pLabel->label_timer.enable == 1)
		{
			if (plabel_timer->TimerLft == 0)
			{
				plabel_timer->TimerLft = plabel_timer->TimerTot;
				pLabel->labelTimerISR(pLabel);
			}
			else
			{
				S32 timeleft;
				
				timeleft =  plabel_timer->TimerLft-lParam;
				if (timeleft < 0) plabel_timer->TimerLft = 0;
				else plabel_timer->TimerLft = timeleft;
			}
		}
		break;		
	case VIEW_REV_SYSMSG:
		pLabel->recvSysMsg(pView);
		break;
	case VIEW_ANTICLICK:
	    break;	
	}

	return SUCCESS;
}

/*
*���ܣ�����Ի�����Ϣ
*����: pDeskFather:������ָ�� DlgType:�Ի������� 1��ʾ2����3ѯ�� id:��ʾ���ַ���id 
*      DlgPressEnter :ȷ����ť�ĺ���ָ�� DlgCancelEnter��ȡ����ť�ĺ���ָ��
*      DispTime : �Ի�����ʾ��ʱ��		 
*/
eDLG_STAT Com_SpcDlgDeskbox(U8 *str,U8 soundType,void *pDeskFather,U32 DlgType,PressEnter DlgPressEnter,PressEnter DlgCancelEnter,U32 DispTime)
{
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON *pBtn;
	tGRAPLABEL *pLable;
	U16 	lastValue;
	U16 index;

	index = 0;
  
    gu8DlgState = DLG_CANCEL_EN;  

	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&gtComDlgViewAttr[index+0], NULL);
	if (NULL == pDesk ) return DLG_CANCEL_EN;	
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	ComWidget_UpdataTimerMsg(pDesk);	
	
	switch(DlgType)
	{
	case 1://��ʾ			

		pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+4],(tGrapLabelPriv *)&gtComDlgLabPri[6]);
		if (NULL == pLable ) return;
		pLable->draw = Com_DlgLabelDrawInfor;
		pLable->label_privattr.pStr = str; 
		
		pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+3],(tGrapLabelPriv *)&gtComDlgLabPri[0]);
		if (NULL == pLable ) return;
		pLable->draw = Com_DlgLabelDraw;
		break;
		
	case 2://����
		//pDesk->pKeyCodeArr = keyComArray;
	    pBtn = Grap_InsertButton(pDesk, (tGrapViewAttr *)&gtComDlgViewAttr[index+2], (tGrapButtonPriv *)&gtComDlgBtn[0]);
		if (NULL == pBtn ) return;
		
		if(DlgPressEnter != NULL)	pBtn->pressEnter = DlgPressEnter;
		else	pBtn->pressEnter = Com_DlgButtonPress;
		
		pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+4],(tGrapLabelPriv *)&gtComDlgLabPri[6]);
		if (NULL == pLable ) return;
		pLable->draw = Com_DlgLabelDrawInfor;
		pLable->label_privattr.pStr = str; 
		
		pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+3],(tGrapLabelPriv *)&gtComDlgLabPri[1]);
		if (NULL == pLable ) return;
		pLable->draw = Com_DlgLabelDraw;
		
		break;
	case 3://3ѯ��
		pBtn = Grap_InsertButton(pDesk, (tGrapViewAttr *)&gtComDlgViewAttr[index+5], (tGrapButtonPriv *)&gtComDlgBtn[0]);
		if (NULL == pBtn ) return;			
		if(DlgPressEnter != NULL)	pBtn->pressEnter = DlgPressEnter;
		else	pBtn->pressEnter = Com_DlgButtonPress;
		
		pBtn = Grap_InsertButton(pDesk, (tGrapViewAttr *)&gtComDlgViewAttr[index+6], (tGrapButtonPriv *)&gtComDlgBtn[1]);
		if (NULL == pBtn ) return;			
		if(DlgCancelEnter != NULL)	pBtn->pressEnter = DlgCancelEnter;
		else	pBtn->pressEnter = Com_DlgButtonCancel;			
			
		pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+4],(tGrapLabelPriv *)&gtComDlgLabPri[6]);
		if (NULL == pLable ) return;
		pLable->draw = Com_DlgLabelDrawInfor;
		pLable->label_privattr.pStr = str; 
		
		pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+3],(tGrapLabelPriv *)&gtComDlgLabPri[2]);
		if (NULL == pLable ) return;
		pLable->draw = Com_DlgLabelDraw;
		break;
	default:
		break;		
	}
	
	Remind_Fun(soundType, 0);
		
	if (DLG_ALLWAYS_SHOW != DispTime)
	{
    	pLable->label_timer.enable = 1;
    	pLable->label_timer.TimerLft = DispTime;
    	pLable->label_timer.TimerTot = DispTime;
    	pLable->labelTimerISR = Com_DlgLabelTimerISR;
    }

	lastValue = BackLight_CloseScreenProtect();
	
    pDesk->kDlgState = gu8DlgState;
    
	Grap_DeskboxRun(pDesk); 
	gu8DlgState = pDesk->kDlgState;
	Grap_DeskboxDestroy(pDesk);
	
	BackLight_SetScreenProtect(lastValue);

	return gu8DlgState;
}

/*---------------------------------------------------------
*����: Com_PercentLabTimerISR
*����: ������ʾ������ʾ�ٷֱ�  
*����: pView �� lable point
*����: none 
*---------------------------------------------------------*/ 
void Com_PercentLabTimerISR(void *pView)
{
	tGRAPLABEL 		*pLabel;
	tStrDispInfo 	disp_info;
	tGrapViewAttr  	*pview_attr;
	tGrapLabelPriv 	*plabel_privattr;
	tGRAPDESKBOX	*pDesk;
	int total,percent;
	U8  buf[6];
	U8  req ;
	
    pLabel = (tGRAPLABEL *)pView;
	pview_attr = &(pLabel->view_attr);
	plabel_privattr = &(pLabel->label_privattr);   
    pDesk = (tGRAPDESKBOX *)pLabel->pParent;    
        
    if (SUCCESS == Com_DlgTimeOut())
    {            
        pDesk->kDlgState = DLG_TIMEROUT_EN;
        *pDesk->pQuitCnt = 1;
    }
    else if( 0xff != kernal_app_get_stat() ) /* �������־λ�Ѵ�����ϱ���� */
    {
    	pDesk->kDlgState = DLG_CONFIRM_EN;
    	 *pDesk->pQuitCnt = 1;
    }
    else
    {
		Grap_Show_Part_BackPicture(pLabel->pParent,pLabel);
		
		total   = Com_GetPercentTotal();
		
		memset(buf,0,6);
		if(total > 0)
		{
			percent = (Com_GetPercentRecv()*100)/total ;
			if(percent >= 0 && percent <= 100)
			{
				sprintf(buf,"%d%s",percent,"%");		
			}
		}
		else
		{
			strcpy(buf,"0%");
		}
		
		disp_info.string = buf;
		disp_info.color = 0;
		disp_info.font_size = plabel_privattr->font_size;;
		disp_info.font_type = plabel_privattr->font_type;;
		disp_info.flag = 2;	
		disp_info.keyStr = NULL;
		Grap_WriteString(pview_attr->viewX1, pview_attr->viewY1+12, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
			(U16)(pview_attr->viewY1+12 + pview_attr->viewHeight), &disp_info);

		pLabel->draw(pLabel,0,0);		
    }	
}

/*
*���ܣ�����Ի�����Ϣ
*����: pDeskFather:������ָ�� DlgType:�Ի������� 1����ȡ�� id:��ʾ���ַ���id  
*      DlgPressEnter :ȷ����ť�ĺ���ָ�� DlgCancelEnter��ȡ����ť�ĺ���ָ��
*      DlgLabTimer: �����lableTimer�жϺ���ָ�� DispTime : lab�жϵ�ʱ��
*/
eDLG_STAT Com_CtnDlgDeskbox(void *pDeskFather,U32 DlgType,U8 *str,
							PressEnter DlgPressEnter,PressEnter DlgCancelEnter,
							Label_TimerISR DlgLabTimer,U32 DispTime , U32 TimeOut)
{
    
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON *pBtn;
	tGRAPLABEL *pLable;
	tGRAPCARTOON *pCtn;
	U16 	lastValue;
	U16 index;

	index = 0;
        
    gu8DlgState = DLG_CANCEL_EN;
    
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&gtComDlgViewAttr[index+1], NULL);
	if (NULL == pDesk ) return;	
	Grap_Inherit_Public(pDeskFather,pDesk);
	//pDesk->pKeyCodeArr = keyComArray;
	
	ComWidget_UpdataTimerMsg(pDesk);
	
    switch(DlgType)
    {
    case 1:
        pCtn = Grap_InsertCartoon(pDesk,(tGrapViewAttr*)&gtComDlgViewAttr[index+7],(tGrapCartoonPriv*)&gtComDlgCtn[0]);
    	if(NULL == pCtn) return;
    	
	    pBtn = Grap_InsertButton(pDesk, (tGrapViewAttr *)&gtComDlgViewAttr[index+2], (tGrapButtonPriv *)&gtComDlgBtn[1]);
		if (NULL == pBtn ) return;
		if(DlgCancelEnter != NULL)	pBtn->pressEnter = DlgCancelEnter;
		else	pBtn->pressEnter = Com_DlgButtonCancel;

    	
    	pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+8],(tGrapLabelPriv *)&gtComDlgLabPri[6]);
    	if (NULL == pLable ) return;
    	pLable->draw = Com_DlgLabelDrawInfor;
    	pLable->label_privattr.pStr = str; 
    	
    	if (DLG_ALLWAYS_SHOW != DispTime)
    	{
    		pLable->label_timer.enable = 1;
    	}
    	
    	if (NULL != DlgLabTimer)
    	{
        	pLable->label_timer.TimerLft = 0;
        	pLable->label_timer.TimerTot = DispTime;
    	    pLable->labelTimerISR = DlgLabTimer;
    	}
    	else
    	{
    	    pLable->label_timer.TimerLft = TimeOut;
        	pLable->label_timer.TimerTot = TimeOut;
    	    pLable->labelTimerISR = Com_DlgLabelTimerISR;    
    	}    
    	break;
    case 2: //�ް���
        pCtn = Grap_InsertCartoon(pDesk,(tGrapViewAttr*)&gtComDlgViewAttr[index+7],(tGrapCartoonPriv*)&gtComDlgCtn[0]);
    	if(NULL == pCtn) return;   	
 	
    	pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+8],(tGrapLabelPriv *)&gtComDlgLabPri[6]);
    	if (NULL == pLable ) return;
    	pLable->draw = Com_DlgLabelDrawInfor;
    	pLable->label_privattr.pStr = str; 
    	if (DLG_ALLWAYS_SHOW != DispTime)
    	{
    		pLable->label_timer.enable = 1;
    	}
    	
    	if (NULL != DlgLabTimer)
    	{
        	pLable->label_timer.TimerLft = 0;
        	pLable->label_timer.TimerTot = DispTime;
    	    pLable->labelTimerISR = DlgLabTimer;
    	}
    	else
    	{
    	    pLable->label_timer.TimerLft = TimeOut;
        	pLable->label_timer.TimerTot = TimeOut;
    	    pLable->labelTimerISR = Com_DlgLabelTimerISR;    
    	}    
    	break;	
	}
	
    gu32DlgStartTime = rawtime(NULL);
    gu32DlgTimeOutTotal = TimeOut;

	lastValue = BackLight_CloseScreenProtect();
	
    pDesk->kDlgState = gu8DlgState;
	
	Grap_DeskboxRun(pDesk); 
	gu8DlgState = pDesk->kDlgState;
	Grap_DeskboxDestroy(pDesk); 
	
	BackLight_SetScreenProtect(lastValue);
	 
    return gu8DlgState;    
}


/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/*---------------------------------------�ȴ����紦�����---------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
static U8 gu8ShutDownDglWait ;


void Com_ShutDown_SetWait(U8 set)
{
	gu8ShutDownDglWait = set ;
}

/*---------------------------------------------------------
*����: Com_ShutDownLabelTimerISR
*����: 
*����: none
*����: 
*---------------------------------------------------------*/
static void Com_ShutDownLabelTimerISR(void *pView)
{
    U8  req ;
    tGRAPLABEL *pLable = (tGRAPLABEL *)pView;
    tGRAPDESKBOX *pDesk;
  
    pDesk = (tGRAPDESKBOX *)pLable->pParent;
        /* ��Ϊ�öԻ���ĳ��ֲ���ComGlob */
    if(1 == gu8ShutDownDglWait || gsm_getmodestate() == 1) /* ��־���û�GSM�쳣�˳� */
    {
    	pDesk->kDlgState = DLG_CONFIRM_EN;
        *pDesk->pQuitCnt = 1;
    }
    else if (SUCCESS == Com_DlgTimeOut())
    {            
        pDesk->kDlgState = DLG_TIMEROUT_EN;
        *pDesk->pQuitCnt = 1;
    }
       
}


/*---------------------------------------------------------
*����: Com_ShutDownLabelTimerISR
*����: 
*����: none
*����: 
*---------------------------------------------------------*/
void Com_Usb_ShutDownLabelTimerISR(void *pView)
{
    U8  req ;
    tGRAPLABEL *pLable = (tGRAPLABEL *)pView;
    tGRAPDESKBOX *pDesk;
  
    pDesk = (tGRAPDESKBOX *)pLable->pParent;
        
    App_SetVbsInvalidTime();
    
    if(1 == gu8ShutDownDglWait)
    {
    	pDesk->kDlgState = DLG_CONFIRM_EN;
        *pDesk->pQuitCnt = 1;
    }
    else if(FALSE == hyudsDcd_Connected())
	{
		pDesk->kDlgState = DLG_CANCEL_EN;
        *pDesk->pQuitCnt = 1;
	}
    else if (SUCCESS == Com_DlgTimeOut())
    {            
        pDesk->kDlgState = DLG_TIMEROUT_EN;
        *pDesk->pQuitCnt = 1;
    }
       
}

/*---------------------------------------------------------
*����: Com_NetButtonCancel
*����: btn�������º�Ĳ���
*����: 
*����: none
*---------------------------------------------------------*/
static S32 Com_ShutDownButtonCancel(void *pView, U16 state)
{
	tGRAPBUTTON *pBtn;
	tGRAPDESKBOX *pDesk;
	
	pBtn = (tGRAPBUTTON *)pView;
	
	if (state != STATE_PRESS)
	{
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		pDesk->kDlgState = DLG_CANCEL_EN;
		*pDesk->pQuitCnt = 1;
	}
	
	return RETURN_CANCLE;
}

/*------------------yu---------------------------------------
* ����: Com_ShutDownDlgDeskbox
* ����: �Ի��򣬹رպ�̨������ ��Ҫ��ǿ�ƹػ���ǿ�ƽ���USBʱ�����
* ����: pDeskFather --- ������
* ����: 
------------------------------------------------------------*/
U8 Com_ShutDownDlgDeskbox(void *pDeskFather,U8 *str,Label_TimerISR DlgLabTimer,U32 timeOut)
{
	tGRAPDESKBOX *pDesk;
	tGRAPBUTTON  *pBtn;
	tGRAPLABEL   *pLable;
	tGRAPCARTOON *pCtn;
	int          ret  ;
	U16          index = 0;
	
    /* ���������ؼ��� */
	pDesk = Grap_CreateDeskboxNoCom((tGrapViewAttr*)&gtComDlgViewAttr[index+1], NULL);
	if (NULL == pDesk ) return;
	
	pDesk->kDlgState = DLG_CONFIRM_EN;
	
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	ComWidget_UpdataTimerMsg(pDesk);
	
    pCtn = Grap_InsertCartoon(pDesk,(tGrapViewAttr*)&gtComDlgViewAttr[index+7],(tGrapCartoonPriv*)&gtComDlgCtn[0]);
	if(NULL == pCtn) return;
	
    pBtn = Grap_InsertButton(pDesk, (tGrapViewAttr *)&gtComDlgViewAttr[index+2], (tGrapButtonPriv *)&gtComDlgBtn[1]);
	if (NULL == pBtn ) return;
	pBtn->pressEnter = Com_ShutDownButtonCancel;

	
	pLable = Grap_InsertLabel(pDesk,(tGrapViewAttr *)&gtComDlgViewAttr[index+8],(tGrapLabelPriv *)&gtComDlgLabPri[6]);
	if (NULL == pLable ) return;
	pLable->draw = Com_DlgLabelDrawInfor;
	pLable->label_privattr.pStr = str;//"�������ڴ���,��ɱ��δ����ǹ���Զ��ػ�!"; 
	//pLable->label_privattr.pStr = "�������ڴ���,��ɱ��δ����ǹ";
	pLable->label_timer.enable = 1;
    
    pLable->label_timer.TimerLft = 0;
	pLable->label_timer.TimerTot = 20;
	if(NULL == DlgLabTimer)
	{
		pLable->labelTimerISR = Com_ShutDownLabelTimerISR ;
	}
	else
	{
    	pLable->labelTimerISR = DlgLabTimer;
    }

	
    gu32DlgStartTime = rawtime(NULL);
    gu32DlgTimeOutTotal = timeOut;
    
	gu8ShutDownDglWait = 0;
		
#if 0
	if(!(YDget_gprsevent()&YD_GPRS_FOREGROUND)) /* û��ǰ̨�¼�,�о�ֱ���˳� */
	{
		YDclear_gprsevent(YD_GPRS_SENDREC); /* �ϴ���־����,���ϴ�������ʱ���¼���ʱ��,
		                     �Ի������һֱ�ȴ�gu8ShutDownDglWait����1 ����Ҫ�����ϴ�����*/
		YDupdata_Wakeup();
		Grap_DeskboxRun(pDesk);
	}
#endif
	ret = pDesk->kDlgState ;
	Grap_DeskboxDestroy(pDesk);
    
    return ret;    
}

//////////////////////////////////////////////////////////////////////////////////////////////////

/*-------------------�޸ļ�¼------------------------------------
090216  �� �޸�id index���Ե�Bug
090309  :  �޸ĶԻ��򳤶ȼ�¼
---------------------------------------------------------------*/