#include "SysSetting.h"

int Set_RecClearing(void *p)
{
	while(Net_GetEvt()&NET_CLEARREC_EVT)
	{//�ȴ���̨��ռ�¼
	
		Net_WakeUp();
		syssleep(50);
	}
	
	return 1;
}


/*---------------------------------------------
*����:Set_RecClear
*����:��¼���
*����:
*����:
*-----------------------------------------------*/
void Set_RecClear(void *pDesk)
{
	if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�Ƿ�������м�¼?",0, pDesk, 3, NULL, NULL, DLG_ALLWAYS_SHOW))
	{
		Net_AddEvt(NET_CLEARREC_EVT);
		cartoon_app_entry(Set_RecClearing,NULL);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		Com_CtnDlgDeskbox(pDesk,2,"   ���Ժ�...",NULL,NULL,ComWidget_LabTimerISR,50,DLG_ALLWAYS_SHOW);
		AppCtrl_DeletePro(KERNAL_APP_AN);
		
		Com_SpcDlgDeskbox("��¼�����!",0,pDesk,1,NULL,NULL,100);
	}
}
