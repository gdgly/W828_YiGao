#include "SysSetting.h"


/*---------------------------------------------
*����:Set_UpdateTime
*����:ʱ��ͬ��
*����:
*����:
*-----------------------------------------------*/
void Set_UpdateTime(void *pDesk)
{
	if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("ȷ��ͬ��ʱ��?",0, pDesk, 3, NULL, NULL, 3000))
	{
		Net_AddEvt(NET_UPDATE_TIME_EVT);//����Ϊ��Ҫͬ��ʱ��
	}
}
