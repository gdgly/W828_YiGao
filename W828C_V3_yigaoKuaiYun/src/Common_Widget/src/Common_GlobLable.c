#include "grap_api.h"
#include "glbVariable.h"
#include "Net_Evt.h"
#include "Rec_Info.h"


//#define RETURN_TO_SWITCH_TICKS		18000	//3����


//static U32 gsu32LastKeyorPull=0;

/*
 * const data
*/
const tGrapViewAttr gtComLabTime[]=
{
  //�����ID�ţ������ظ�
  {0xFFFD, 0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0},
};

const tGrapLabelPriv gtComLabTimePri[]=
{
	{NULL,0,0,0,0,0,0,0},
};

//���һ�ΰ��������¼�
/*void common_updatelast_keyorpull()
{
	gsu32LastKeyorPull = rawtime(NULL);
}

void common_check_lastevent(tGRAPDESKBOX *pDesk)
{
	hyUsbPrintf("rawtime = %d    last  = %d    %d    %d \r\n",
			rawtime(NULL),gsu32LastKeyorPull, ABS(rawtime(NULL)-gsu32LastKeyorPull), RETURN_TO_SWITCH_TICKS);
	if(ABS(rawtime(NULL)-gsu32LastKeyorPull) > RETURN_TO_SWITCH_TICKS)
	{
		if(pDesk->view_attr.id != MAIN_DESKBOX_ID)	
		{
			Grap_Return2Switch(pDesk, RETURN_TOSWITCHMODE);
		}
		else//���������������˺���Ϣ
		{
			Gqy_ClearInputData();
			ReDraw(pDesk, 0, 0);
		}
	}
}*/

/*---------------------------------------------------------
*����: common_glob_lab_draw
*����: �պ���������ˢ��   
*����: none
*����: none
*---------------------------------------------------------*/
static S32 common_glob_lab_draw(void *pView, U32 xy, U32 wh)
{
	 return SUCCESS;
}


/*---------------------------------------------------------
*����: common_glob_lab_timer_isr
*����: ��ʱ������������Ӧ�ÿ��Է���ĺ�������   
*����: none
*����: none
*---------------------------------------------------------*/
static void common_glob_lab_timer_isr(void *pView)
{
    tGRAPDESKBOX	*pDesk;
    static	U32		lastTime=0;
	U32				now;
	int				ret;
    
    pDesk = ((tGRAPLABEL*)pView)->pParent;
    
	bat_show_low_battery_deskbox(pDesk);
	
//	common_check_lastevent(pDesk);
	
	//�¼�����
	Evt_Event_Remind(pDesk);
	
	//���������Ͽ�,ֻ���ж�ʱ��
	Evt_CloseSocket_Check();
	
	//���wifi�Ƿ���Ϊ�źŻ�����ԭ�� ����Ϊ�Ͽ�
	if(gtHyc.NetSelected == 0)
	{
		WifiConnect_CheckState();
	}
	
	now = rawtime(NULL);
	if(ABS(now - lastTime) > 500)
	{
		//�������Ƿ���Ҫ����
		if(Login_GetLoginFlag() == 1)
		{
			//�����Ƿ���Ҫ����
			ret = Comm_GetUpFlag();
			if(ret == 1)//�г�����Ҫ����,����ǿ��
			{
				Comm_SetUpFlag(0x81);
				Com_SpcDlgDeskbox("�������°汾,�뵽[ϵͳ����]��������!",6,pDesk,2,NULL,NULL,500);
				ReDraw(pDesk,0,0);
				Comm_SetUpFlag(0);
			}
			else if(ret == 2)//ǿ�Ƴ�������
			{
				Comm_SetUpFlag(0x82);
				Com_SpcDlgDeskbox("��������Ҫ����,���������[ȷ��]����.",0, pDesk, 2, NULL, NULL, 0xFFFFFFFF);
				Common_NetUpgrade(3, pDesk);
				Comm_SetUpFlag(0);
			}
		}
		
		//����Ƿ��м�¼��Ҫ���ͻ����   ʱ���Ƿ�ͬ��
		Evt_RecLoadSend_Check();
		
		//��鹫˾��Ϣ�Ƿ���Ҫ����
		Evt_UpdateCompany_Check();
		
		//���gsmģ���Ƿ���Ҫ����
		Evt_GsmReset_Check();
		
		lastTime = now;
		
	}	
}

/*---------------------------------------------------------
*����: common_insert_glob_lab
*����: �ڴ����в���ÿؼ�   
*����: none
*����: none
*---------------------------------------------------------*/
static S32 common_recv_sys_msg(void *pView)
{
    tGRAPLABEL *pLable = (tGRAPLABEL *)pView;
    
    //���USB
    ComWidget_RevUSBMsg(pView);
    
    return SUCCESS;
} 


/*---------------------------------------------------------
*����: common_insert_glob_lab
*����: �ڴ����в���ÿؼ�   
*����: none
*����: none
*---------------------------------------------------------*/
void common_insert_glob_lab(tGRAPDESKBOX *pDesk) 	
{	
	tGRAPLABEL *pLable;
	int powerOn,powerDown;
	
	powerOn = AppCtrl_IsValidPro(POWER_ON_AN);
	powerDown = AppCtrl_IsValidPro(POWER_DOWN_AN);
	
	if (1 == powerOn || 1 == powerDown)
	{
	    return;
	}	
	
	pLable = Grap_InsertLabel(pDesk, &gtComLabTime[0], &gtComLabTimePri[0]);
	if(NULL == pLable) return;
	pLable->draw = common_glob_lab_draw;
	pLable->label_timer.enable = 1;
	pLable->label_timer.TimerLft = 10;
	pLable->label_timer.TimerTot = 10;
	pLable->labelTimerISR =  common_glob_lab_timer_isr;
	pLable->recvSysMsg = common_recv_sys_msg;
}	