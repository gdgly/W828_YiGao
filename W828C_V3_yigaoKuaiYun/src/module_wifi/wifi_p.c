#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "std.h"
#include "hyhwCkc.h"
#include "hyhwChip.h"
#include "hyhwIntCtrl.h"
#include "hyhwSDIO.h"
#include "hymwSDIO.h"
#include "semaphore.h"
#include "wmi.h"
#include "wifi_user.h"
#include "wifi_api.h"
#include "TcpIp_p.h"
#include "ospriority.h"
#include "appControlProcess.h"
#include "semaphore.h"
#include "sys.h"

SEMAPHORE	*Wifi_Exit_sem = NULL;
void 	*pWifiThreadWait = NULL;
S8 wifi_init_state = 0;

void wifi_ap_list_intial(void)
{
    INIT_LLIST_HEAD(&tApList);
}

void wifi_Sdio_ISR(void)
{ 	
	if(hyhwSDIO_HwIntDetect() == 1 )//��ʾ��WiFi�ж�
	{
	//	ConSume_WifiChangeSpeedTimeOut(60);		
		ewakeup(&pWifiThreadWait);
	}
}

//flag =0:����Ƶ��   =1:���Ƶ��
void WiFiDriver_UpdataSpeedDirect(int flag)
{
/*	int wifiCoef = 0;
	
	if (flag)
	{
		wifiCoef = 64;
	}
	ConSume_UpdataSpeedDirect(WIFI_DRIVER_AN, wifiCoef);
*/
}

//0---δ��ʼ��
//1---��ʼ���ɹ�
//-1--��ʼ��ʧ��
int WiFiDriver_getInitState(void)
{
	return (int)wifi_init_state;
}

int WiFiDriver_ThreadMethod(void)
{
	int ret;
	unsigned int ilev;

	ConSume_UpdataAppCoef(WIFI_DRIVER_AN, 120);
	hyhwCpm_busClkEnable(CKC_SDMMC);
	hyhwCpm_clearSoftReset(CKC_SDMMC);
	
	wifi_ap_list_intial();
	
	//hyUsbPrintf("+++++++++++++++++++++++++++++++++����wifi����  clear list\r\n");
	hy_ap_del_list_all();
	
	wifi_init_state = 0;
	ret = mvWlan_wifiInit();
	if (ret == HY_ERROR)
	{
		//wifi��ʼ��ʧ�ܣ�����
		WiFi_Power_Disable();
		syssleep(10);
		ret = mvWlan_wifiInit();
		if (ret == HY_ERROR)
		{
			//wifi��ʼ������ʧ�ܣ����̵ȴ��˳�
			wifi_init_state = -1;
			Wifi_Exit_sem = semaphore_init(0);
			while(1)
			{
				ret = semaphore_trywait(Wifi_Exit_sem);
				if(ret == 0)
				{
					//�˳�
					semaphore_destroy(Wifi_Exit_sem, OS_DEL_NO_PEND);
					Wifi_Exit_sem = NULL;
					WiFi_Power_Disable();
					//hyhwCpm_setSoftReset(CKC_SDMMC);
					hyhwCpm_busClkDisable(CKC_SDMMC);
					wifi_init_state = 0;
					
					return 1;
				}
				syssleep(1);
				#if 0
				{
					static U32 testLastTick=0;
					U32 now = rawtime(NULL);
					if ((now-testLastTick) > 200)
					{
						PhoneTrace(1, "wifi init err");
						hyUsbMessage("wifi init err\r\n");
						testLastTick = now;
					}
				}
				#endif
			}
		}
	}
	wifi_init_state = 1;
	
#if 0
	/* ʹ��host��SDIO�ж� */
	hyhwInt_clear(INT_BIT_SD);

	hyhwInt_setIRQ(INT_BIT_SD);
	hyhwInt_setTriggerLevel(INT_BIT_SD);
	hyhwInt_enable(INT_BIT_SD);

	hyhwInt_ConfigSdio1_ISR(wifi_Sdio_ISR);
#endif

	hyhwSDIO_mEnCardIntSignal();
	hyhwSDIO_mEnCardIntStatus();
	
	ConSume_UpdataAppCoef(WIFI_DRIVER_AN, 0);
	Wifi_Exit_sem = semaphore_init(0);
//hyUsbPrintf("wifi state = %d \r\n", hy_unifi_getState());	
	while(1)
	{
		//Sdio�жϷ�ʽ
		//ilev = setil(_ioff);
		//hyhwSDIO_mEnCardIntStatus();
		//ewait(&pWifiThreadWait);
		//(void) setil(ilev);
		
		hy_unifi_handler();
		syssleep(1);
		
		
		ret = semaphore_trywait(Wifi_Exit_sem);
		if(ret == 0)
		{
			break;
		}
	}
	semaphore_destroy(Wifi_Exit_sem, OS_DEL_NO_PEND);
	Wifi_Exit_sem = NULL;
	hy_ap_del_list_all();
	hy_unifi_deinit();
	WiFi_Power_Disable();
	
	//hyhwCpm_setSoftReset(CKC_SDMMC);
	hyhwCpm_busClkDisable(CKC_SDMMC);
	
//	netdev_close_tcpip();
//	syssleep(50);
//	AppCtrl_CreatPro(TCPIP_AN,0);

	wifi_init_state = 0;
}

int WifiDriver_Start(void)
{
	int pid, startValue, pidStatus = HY_ERROR;
	
	pid = ntopid(WIFI_DRIVER_NAME);
	if (pid>0 && pid<30)
	{
		//���̴��ڣ��ж�״̬
		if (PSPROC(pid)->state == P_INACT)
		{
			//deletep(pid);//ɾ�����ٴ���
			pidStatus = HY_OK;//���̴����Ҵ���exit״̬����ֱ������
		}
		else
		{
			return HY_ERROR;//���̻������У�ֱ���˳�����ǰ������Ч
		}
	}
//	hyUsbPrintf("#########pidStatus == %d \r\n", pidStatus);
	if (pidStatus != HY_OK)
	{
		//���̲����ڣ���Ҫ����
		pid = createp((int (*)())WiFiDriver_ThreadMethod,
								WIFI_DRIVER_NAME,
								WIFI_DRIVER_SIZE,
								WIFI_DRIVER_PRIOPITY,
								NULL,NULL,NULL);
	}
	startValue = pstart( pid, (void *)WiFiDriver_ThreadMethod );
	
	AppCtrl_CreatPro_Virtual(WIFI_DRIVER_AN, pid);
	return HY_OK;
}

void WifiDriver_Exit(void)
{
	int pid;
		
//hyUsbPrintf("+++++++++++++++++++++++++++++++++�˳�wifi����  clear list\r\n");
	
	pid = ntopid(WIFI_DRIVER_NAME);
	if (pid<0 || pid>30) return;
	
	if (Wifi_Exit_sem != NULL)
		semaphore_post(Wifi_Exit_sem);
		
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	deletep(pid);
	AppCtrl_DeletePro(WIFI_DRIVER_AN);
	
//	hyUsbPrintf("WifiDriver_Exit  \r\n");
}
