#include "SysSetting.h"


extern int Comm_UpgradeSoft(void *p);
extern int Comm_GetSoftInfo();


/*---------------------------------------------
*����:Set_UpdateSoft
*����:�������
*����:
*����:
*-----------------------------------------------*/
void Set_UpdateSoft(void *pDesk)
{
	U64 	totalspace,freespace = 0;
	int		ret,upgrade,lastValue;
	eDLG_STAT		kDlgState;
	
	AbstractFileSystem_VolumeSpace( "C", &totalspace, &freespace);
	freespace >>= 20;
	if(freespace < 2)//M
	{
		Com_SpcDlgDeskbox("���̿ռ䲻��,��2M.",0,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
		return;
	}
	
	if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�Ƿ�ȷ������?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
	{
		if(Net_ChangeEvt(pDesk,NET_UPDATE_SOFT_EVT) < 0) return ;
		
		lastValue = BackLight_CloseScreenProtect();//�ر�����
		
		ConSume_UpdataSpeedDirect(SHELL_AN, 192);
		
		cartoon_app_entry(Comm_GetSoftInfo,NULL);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		kDlgState = Com_CtnDlgDeskbox(pDesk,2,"���ڻ�ȡ��Ϣ,���Ժ�!",NULL,NULL,ComWidget_LabTimerISR,50,12000);
		AppCtrl_DeletePro(KERNAL_APP_AN);
		
		if(DLG_CONFIRM_EN == kDlgState)
		{
			ret = kernal_app_get_stat();
			if(6 == ret)//��������ʧ��
			{
				Com_SpcDlgDeskbox("�������,���Ե�1��������!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
			}
			else if(7 == ret || 8 == ret)//����ʧ��
			{
				Com_SpcDlgDeskbox("�������,���Ե�1��������!",6, pDesk, 2,NULL, NULL,  0x8FFFFFFF);
			}
			else if(9 == ret)//�������°汾
			{
				if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�������°汾,�Ƿ�ǿ������?",0, pDesk, 3, NULL, NULL, 0x8FFFFFFF))
				{
					ret = 5;
				}
			}
			else if(10 == ret)
			{
				Com_SpcDlgDeskbox("û�г����ļ�!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
			}
			
			//
			if(5 == ret)
			{
				cartoon_app_entry(Comm_UpgradeSoft,NULL);
				AppCtrl_CreatPro(KERNAL_APP_AN, 0);
				kDlgState = Com_CtnDlgDeskbox(pDesk,2,"��������,���Ե�!",NULL,NULL,Com_PercentLabTimerISR,4, 0x8FFFFFFF);
				AppCtrl_DeletePro(KERNAL_APP_AN);
				
				Comm_SetUpgradeState(0);
				
				if (kDlgState == DLG_CONFIRM_EN)
				{
					ret = kernal_app_get_stat();
				
					if(5 == ret)
					{
						if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�������,��ػ����ٴο�������.",0, pDesk, 2, NULL, NULL, 0x8FFFFFFF))				
						{
							AppCtrl_PowerOffProc();
						}
					}
					else if(7 == ret)
					{
						Com_SpcDlgDeskbox("�ļ�����,������!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
					}
					else if(6 == ret)
					{
						Com_SpcDlgDeskbox("�������,���Ե�1��������!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
					}
					else
					{
						Com_SpcDlgDeskbox("�������,���Ե�1��������!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
					}
				}
				else
				{
					Com_SpcDlgDeskbox("�������,���Ե�1��������!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
				}
			}
		}
		else
		{
			Com_SpcDlgDeskbox("��������ʧ��!",0,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
		}
		
		Net_ClearEvt(NET_FOREGROUND_EVT);
		BackLight_SetScreenProtect(lastValue);//������
		
		ConSume_UpdataSpeedDirect(SHELL_AN, 0);
	}
}
