#include "grap_api.h"
#include "grap_view.h"
#include "grap_base.h"
#include "grap_event.h"
#include "grap_deskbox.h"


U16  keyattr[][2] = 
{

	{Keyb_Code_1,		KEYEVENT_1},//1
	{Keyb_Code_2,		KEYEVENT_2},//2
	{Keyb_Code_3,		KEYEVENT_3},//3
	{Keyb_Code_4,		KEYEVENT_4},//4
	{Keyb_Code_5,		KEYEVENT_5},//5
	{Keyb_Code_6,		KEYEVENT_6},//6
	{Keyb_Code_7,		KEYEVENT_7},//7
	{Keyb_Code_8,		KEYEVENT_8},//8
	{Keyb_Code_9,		KEYEVENT_9},//9
	{Keyb_Code_Xing, 	KEYEVENT_STAR},//*
	{Keyb_Code_0, 		KEYEVENT_0},//0
	{Keyb_Code_Jing, 	KEYEVENT_SHARP},//#
	{Keyb_Code_Del,		KEYEVENT_DELETE},//ɾ��
	
	{Keyb_Code_Up, 		KEYEVENT_UP},//��
	{Keyb_Code_Down, 	KEYEVENT_DOWN},//��
	{Keyb_Code_Left, 	KEYEVENT_LEFT},//��
	{Keyb_Code_Right, 	KEYEVENT_RIGHT},//��
	{Keyb_Code_OK, 		KEYEVENT_OK},//OK
	{Keyb_Code_F1,	    KEYEVENT_F1},//
	{Keyb_Code_F2, 		KEYEVENT_F2},//
	{Keyb_Code_Tab, 	KEYEVENT_TAB},//
	{Keyb_Code_Esc,		KEYEVENT_ESC},//
	{Keyb_Code_ScanL,	KEYEVENT_SCAN},//ɨ���
	{Keyb_Code_ScanR,	KEYEVENT_SCAN},//ɨ���
	{Keyb_Code_ScanM,	KEYEVENT_SCAN},//ɨ���
	
	
	//{Keyb_Code_PowerLongPressed, 		cHyc_Act_PowerDown},//�ػ�
		
	{0xff, 				0xff}

} ;

static U32 Sys_KeyGet(Keyb_Code_e keKeyCode,U32 u32KeyEvent)
{
	U32 i = 0,key = 0xff;
	
	while(1)
	{
		if(keyattr[i][0] == 0xff)
			break;
		if(keyattr[i][0] == keKeyCode || (keKeyCode >= 32 && keyattr[i][0] == keKeyCode - 32))
		{
			key = keyattr[i][1];
			break;
		}
		i++;
	}
	
	return key;
}

/*---------------------------------------------------------
*����: GetKeyActionPhone
*����: //���ݲ�ͬ��desk��ָ�İ���������ɨ�谴��ֵ
*      //ע�ⶨ��keycodeʹ��Ҫʹ��1--10֮������֣�������Ϊϵͳ����
*      //���ָ�������������ϡ��¡����Ҽ�����Ӧָ���İ�����������Ӧϵͳ��
*����: none
*����: 0xff û�а��� ����Ϊ����ֵ
*---------------------------------------------------------*/
U32 GetKeyActionPhone(Keyb_Code_e keKeyCode,U32 u32KeyEvent,void *pView)
{
	U32 i = 0;
	U32 nKeyAction = 0xff;
	tGRAPDESKBOX *pDesk;
	U16 *pKeyEventArray;
	
	pDesk = (tGRAPDESKBOX *)pView;	
	
	if(NULL == pDesk ) return nKeyAction;
	
	pKeyEventArray =  pDesk->pKeyCodeArr;
	
	while(pKeyEventArray)
	{
		if(pKeyEventArray[i] == 0xff)
			break;
		if(pKeyEventArray[i] == keKeyCode || (keKeyCode >= 32 && pKeyEventArray[i] == keKeyCode - 32))
		{
			nKeyAction = pKeyEventArray[i+1];
			break;
		}
		i+=2;
	}
	
	if(0xff == nKeyAction)
	{
		nKeyAction = Sys_KeyGet(keKeyCode,u32KeyEvent);
	}
	
	return nKeyAction;
}



void key_wakeup_event(Keyb_Code_e eKeyCode,U32 keyEvent,tGRAPDESKBOX *pDesk)
{
	BackLight_EventTurnOnLcd(pDesk);
}

S32 Grap_DealEvent(void *pDesk,GRAP_EVENT *event,tMSG_BODY *pMsgBody)
{
    Keyb_Code_e	keKeyCode;
	U32			u32KeyEvent;
	U32			keycode = 0;
	U16 row,col;
	U32 act;
	uMSG *pMsg;
	
	memset(event, 0, sizeof(GRAP_EVENT));
	act = pMsgBody->id;
	pMsg = &pMsgBody->msg;
	
	keKeyCode = pMsg->Key.keycode;
	u32KeyEvent = pMsg->Key.keyevent;
	
	switch(act)
	{
	case  KEY_INPUT_EVENT:
		keycode = GetKeyActionPhone(keKeyCode, u32KeyEvent, pDesk);
		
		if (0xff == keycode) return SUCCESS; //NO Effect KeyCode
		
		event->message = VIEW_PRESS;
		event->lParam = keycode;
		event->wParam = u32KeyEvent ;					

		break;
	case TOUCH_KEY_EVENT:
	    /*if (1 == gtHyc.HOLD_Select)
	    {//��Ļ���Ѿ�����
	        event->message = VIEW_ANTICLICK;
		}
		else*/
		{
		    event->message = VIEW_CLICK;
		}
		event->lParam = (pMsg->Touchkey.row<<16)|pMsg->Touchkey.col;	
		break;
	case SYS_TIMER_EVENT:
		event->message = VIEW_TIMER;
		event->lParam = pMsg->Timer - Grap_GetLastTick();
		Grap_SetLastTick(pMsg->Timer);
		break;
	case SYS_REQ_QUIT_EVENT:
		//��Ӧ���յ�����Ϣ���������ֹͣ��ǰ�����취�˳���ǰ���̣�
		//����ý������з�����̣�������̱���Ҳdelete
		event->message = VIEW_REV_SYSMSG;
		event->lParam =  VIEW_QUITALL;
		break;
	case SYS_PHONE_EVENT://�绰��Ϣ���ͣ�ֻ�е绰���̲��ܽ��������Ϣ
		event->message = VIEW_REV_SYSMSG;
		event->lParam = pMsgBody->msg.PhoneMsg.PhoneEvent;
		event->wParam = pMsgBody->msg.PhoneMsg.Type;
		event->reserve = (U32)pMsgBody->msg.PhoneMsg.Buf;
		break;
	case KEY_PULL_EVENT://������ק��Ϣ	
		event->message = VIEW_REV_SYSMSG;
		break;
	case TOUCH_KEY_PULL_EVENT:
		/*if (1 == gtHyc.HOLD_Select)
		{//��Ļ���Ѿ�����
		event->message = VIEW_ANTICLICK;
		}
		else*/
		{
		event->message = VIEW_REV_PULLMSG;
		}
		event->lParam = (pMsg->PointKeyPull.y << 16)|pMsg->PointKeyPull.x;		    
		event->extParam = pMsg->PointKeyPull.endFlag;		   	    
		break;
	case KEY_LOCK_SCR:
	     event->message = VIEW_LOCK_SCR; 
	     break; 
	case KEY_UNLOCK_SCR://����
		 event->message = VIEW_UNLOCK_SCR;
		 break;
	case KEY_WAKEUP_SYS: //��������ϵͳ
	     key_wakeup_event(keKeyCode, u32KeyEvent, pDesk);
	     break;        	
	case SYS_FOCUS_EVENT:
	     //���뷨���л�ʱ�������⵽�ƻ���Ҫ���¼���
	     //edit_redraw_alpha_all_widget(pDesk); 
	     event->message = VIEW_REV_SYSMSG;//��Ϣ�������Լ�����Ϣ����ȡ�� 
	     break;
	case POP_UNLOCKDIALOG_EVENT:
		if(((tGRAPDESKBOX *)pDesk)->view_attr.id == MAIN_DESKBOX_ID)
		{
			event->message = VIEW_POPDIALOGUE;
		}
		else
		{
			event->message = VIEW_REV_SYSMSG;
		}
		break;            	
	default:
		event->message = VIEW_REV_SYSMSG;//��Ϣ�������Լ�����Ϣ����ȡ��
		break;					
	}

	return SUCCESS;
} 