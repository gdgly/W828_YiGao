#include "SysSetting.h"
#include "wmi.h"
#include "hy_unifi.h"

/*---------------------------------------------
*����:Set_FactorySetting
*����:�ָ�����ֵ
*����:
*����:
*-----------------------------------------------*/
void Set_FactorySetting(void *pDesk)
{
	U8 netSelected;
	tWMI_BECON_BUFF *pap;
	
	if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("ȷ�ϻָ���������?",0, pDesk, 3, NULL, NULL, DLG_ALLWAYS_SHOW))
	{
		if(gtHyc.NetSelected != 1)//֮ǰѡ��wifi
		{
			/*netSelected = gtHyc.NetSelected;
			gtHyc.NetSelected = 0xff;//ֵΪ�����л���
			if(0 == ComWidget_ChangeNetLogo(pDesk, netSelected, 1))
			{
				gtHyc.NetSelected = 1;
			}
			else
			{
				gtHyc.NetSelected = netSelected;
				
				Com_SpcDlgDeskbox("�ָ�����ֵʧ��,������!",0,pDesk,2,NULL,NULL,100);
				return ;
			}*/
			
			pap = hy_ap_get_connect_infor();
			if(hy_get_ap_link_status() == 1 && (pap->apStatus&0x01))
			{
				ComWidget_ApEvent(pDesk, "���ڶϿ�����,���Ժ�...", CMD_DISCONNECT, (void *)pap);
			}
		}
		
		netSelected = gtHyc.NetSelected;
		glbVariable_LoadDefaultValue();
		gtHyc.NetSelected = netSelected;
		hyhwLcd_lightSet(gtHyc.BackLight);		
		hyIE_clearDns();
		Net_AddEvt(NET_CLOSEPPP_EVT);
		glbVariable_SaveParameter();
		Com_SpcDlgDeskbox("���óɹ�!",0,pDesk,2,NULL,NULL,100);
	}
}
