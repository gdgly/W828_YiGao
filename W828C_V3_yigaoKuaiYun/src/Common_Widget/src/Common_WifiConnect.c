/**************************************************
*Ŀǰ������GSMģ�龭����������,GPRSͨ��AT����Ӧ/GSM�ź���ʧ/����AT����
*��Ӧ��,�ý�����Ҫ��������GSM��������������GSMģ��
***************************************************/
#include "hyTypes.h"
#include "glbVariable.h"
#include "ppp_gprs.h"
#include "time.h"
#include "semaphore.h"
#include "std.h"
#include "sys.h"
#include "ospriority.h"
#include "appControlProcess.h"
#include "wmi.h"
#include "hy_unifi.h"

/////////////////////////////////////////////////////////////////////////////////
#define WIFIAUTOCONNECT_DELAY		18000	//3����

#define WIFI_KEEPCONNECT_DELAY		3000	//30S

#define WIFICONNECT_P_NAME		"WifiConnect_p"

typedef struct _aifi_last_t
{
	tWMI_BECON_BUFF	ap;
	U8				pw[64];
	
}tWIFI_LAST;


SEMAPHORE	*WifiConnect_p_Exit_sem = NULL;
int			*WifiConnectWait = NULL;

tWIFI_LAST	gtWifiLastConnAp;

U8 gu8WifiConnLock;//0:�������к�̨
U8 gu8WifiIsConnecting;

U32 gu32WifiConnDelay;//
U32 gu32WifiKeepConnect;
//////////////////////////////////////////////////////////////////////////
void WifiConnect_SaveApInfo(tWMI_BECON_BUFF *pAp, U8 *pPw)
{
hyUsbPrintf("++++++++++++++++++++save ap info 0x%x  name = %s  length = %d \r\n", (U32)pAp, pAp->ap.apName,pAp->ap.length);
	if(pAp != NULL && pAp != &(gtWifiLastConnAp.ap))
	{
		memset((char *)&gtWifiLastConnAp, 0, sizeof(tWIFI_LAST));
		memcpy((char *)&(gtWifiLastConnAp.ap), (char *)pAp, sizeof(tWMI_BECON_BUFF));
	hyUsbPrintf("ap.ap.apName = %s   length = %d \r\n", gtWifiLastConnAp.ap.ap.apName, gtWifiLastConnAp.ap.ap.length);
		if(pPw != NULL)
			strcpy(gtWifiLastConnAp.pw, pPw);
		
		gu32WifiKeepConnect = 0xFFFFFFFF;
	}
}

tWMI_BECON_BUFF* WifiConnect_GetCurAp()
{
hyUsbPrintf("ap.ap.apName = %s \r\n", gtWifiLastConnAp.ap.ap.apName);
	if(gtWifiLastConnAp.ap.ap.apName[0] != 0)
		return &(gtWifiLastConnAp.ap);
	else
		return NULL;
}

U32 WifiConnect_GetDelay()
{
	return gu32WifiConnDelay;
}

void WifiConnect_SetDelay(U32 delay)
{
	gu32WifiConnDelay = delay;
}

void WifiConnect_SetLock(lock)
{
//hyUsbPrintf("---------------------------------gu8WifiConnLock = %d \r\n",lock);
	gu8WifiConnLock = lock;
}

U8 WifiConnect_GetLock()
{
	return gu8WifiConnLock;
}

U8 WifiConnect_IfConnecting()
{
	return gu8WifiIsConnecting;
}

void WifiConnect_SetState(U8 state)
{
	gu8WifiIsConnecting = state;
}

/*-------------------------------
* ����:WifiConnect_WakeUp
* ����:���ѽ���
* ����:none
* ����:none
*-------------------------------*/
void WifiConnect_WakeUp()
{
	if(gu8WifiConnLock == 0 && WifiConnectWait != NULL)
	{
		ewakeup(&WifiConnectWait);
	}
}

int WifiConnect_FontConn(void *pDesk)
{
	if(gtHyc.NetSelected == 0 && hy_get_ap_link_status() == 0 && gtWifiLastConnAp.ap.ap.apName[0] != 0)
	{
		//�������ϴ����ӵ�AP,�������ʧ��,����������
		ComWidget_ApEvent(pDesk, "������������,���Ժ�...", CMD_CONNECT, (void *)&(gtWifiLastConnAp.ap));
		if(kernal_app_get_stat() != 1)//û�����ӳɹ�
		{
			WifiConnect_WakeUp();//���Ѻ�̨
			Com_SpcDlgDeskbox("��������ʧ��,���Ժ�����!",6,pDesk,2,NULL,NULL,500);
		}
	}
	if (gtHyc.NetSelected == 0 && hy_get_ap_link_status() == 1)//���ӳɹ�
	{
		WifiConnect_SetDelay(rawtime(NULL));
	}
}

int WifiConnect_BackConn()
{
	tWMI_BECON_BUFF *pBuf;
	
	if(gtHyc.NetSelected == 0 && hy_get_ap_link_status() == 0 && gtWifiLastConnAp.ap.ap.apName[0] != 0)
	{
		pBuf = (tWMI_BECON_BUFF *)&(gtWifiLastConnAp.ap);
		
		if(pBuf->wmiEncryptionType != WMI_CRYPTO_OPEN)
		{
			WifiPw_SetPwType(pBuf->wmiEncryptionType);
		hyUsbPrintf("name  pw = %s \r\n", gtWifiLastConnAp.pw);
			WifiPw_SetPw(gtWifiLastConnAp.pw);
		}
		hy_ap_setinfo(pBuf);
		hyUsbPrintf("name  connect ap ===== \r\n");	
		if(1 == ComWidget_ConnectAp(pBuf))
		{
			hyUsbPrintf("name  connect ok----- \r\n");
		}
		else
		{
			WifiConnect_WakeUp();//���Ѻ�̨
		}
	}
}

void WifiConnect_CheckState()
{	
	tWMI_BECON_BUFF *pAp;
	
	if(gu8WifiConnLock == 0)
	{
		if(hy_get_ap_link_status() == 0)
		{
		//hyUsbPrintf("Wifi_delay = %d   %d\r\n",delay,ABS(rawtime(NULL)-delay));
			if((gu32WifiConnDelay == 0 || ABS(rawtime(NULL)-gu32WifiConnDelay) >= WIFIAUTOCONNECT_DELAY) && 
			   (BackLight_HaveLight() == 1 || Rec_GetUnSendCnt() > 0))
			{
				//hyUsbPrintf("wakeup wifi p===\r\n");
				WifiConnect_WakeUp();
			}
		}
		else if(STATE_ASSOCIATED != hy_unifi_getConnectState())
		{
			hy_set_ap_link_status(0);
			WifiConnect_SetDelay(0);
			hyUsbPrintf("wifi set delay ===== \r\n");
		}
		/*else if(hy_get_ap_link_status() == 1)
		{
			//hyUsbPrintf("keep connect = %d   %d \r\n",gu32WifiKeepConnect, ABS(rawtime(NULL) - gu32WifiKeepConnect));
			if(gu32WifiKeepConnect == 0xFFFFFFFF)
			{
				gu32WifiKeepConnect = rawtime(NULL);
			}
			//����״̬���¼�,��ʱ�Ͽ�
			if(ABS(rawtime(NULL) - gu32WifiKeepConnect) >= WIFI_KEEPCONNECT_DELAY)
			{
			hyUsbPrintf("netevt = 0x%x   socket = %d \r\n", Net_GetEvt(), hyIE_GetHttpSocket());
				if(Net_GetEvt() == 0 && hyIE_GetHttpSocket() == -1)//û�������¼���socket�Ѿ��ر�
				{
					//�Ͽ�����,���汾�����ӵ�AP��Ϣ
					pAp = hy_ap_get_connect_infor();
					if(pAp != NULL)
					{
						//memcpy(&(gtWifiLastConnAp.ap), pAp, sizeof(tWMI_BECON_BUFF));
						//strcpy(gtWifiLastConnAp.pw, WifiPw_GetPw());
					}
					else
					{
						hyUsbPrintf("get ap info fail ====== \r\n");
					}
					ComWidget_DisConnectAp((void *)pAp);
					gu32WifiKeepConnect = 0xFFFFFFFF;
					gu32WifiConnDelay = rawtime(NULL);
					hyUsbPrintf("wifi disconnect======\r\n");
				}
				else
				{
					gu32WifiKeepConnect = rawtime(NULL);
				}
			}
		}*/
	}
}

/*-------------------------------
* ����:WifiConnect_Process
* ����:
* ����:none
* ����:1
*-------------------------------*/
int WifiConnect_Process(void)
{
	tWMI_BECON_BUFF  *pBuf = NULL;
	tAPINFO  *pAp;
	unsigned ilev;
	int i,cnt,ret;
	
	gu8WifiConnLock = 0;
	gu32WifiConnDelay = 0;
	gu32WifiKeepConnect = 0xFFFFFFFF;
	
	memset(&gtWifiLastConnAp, 0, sizeof(tWIFI_LAST));
	
	WifiConnect_p_Exit_sem = semaphore_init(0);
	while(1)
	{
		WifiConnect_SetState(0);
		
		ret = semaphore_trywait(WifiConnect_p_Exit_sem);
		if(ret == 0)
		{
			break;
		}
		
		ilev = setil(_ioff);
		ewait(&WifiConnectWait);
		(void) setil(ilev);
		
		ret = semaphore_trywait(WifiConnect_p_Exit_sem);
		if(ret == 0)
		{
			break;
		}
		
		WifiConnect_SetState(1);		
		gu32WifiConnDelay = 0x0;
		
		if(hy_unifi_getState() < CMD_STA_INITED || gtHyc.NetSelected != 0 || hy_get_ap_link_status() == 1)
		{
			continue;
		}
		
	hyUsbPrintf("NetSelected = %d \r\n",gtHyc.NetSelected);	
		Ie_clear_connectfailcnt();
		//netdev_stop_tcpip();
		//netdev_remove_wifi_dev();
		hyIE_resetSocket();

		pAp = Wifi_GetLastAp(0);
		if (pAp == NULL)
		{
			//��¼��û��Ap��Ϣ����ִ��ɨ�趯��
			hyUsbPrintf("no ap info ===== \r\n");
			gu32WifiConnDelay = rawtime(NULL);
			continue;
		}
	//hyUsbPrintf("search ap  ===== \r\n");	
		if(1 == ComWidget_SearchApList(NULL))//ɨ��AP
		{
		hyUsbPrintf("search ap ok  ==== \r\n");
			cnt = 0;
			while(1)
			{
				for(i = cnt; i < WIFI_SAVEPW_MAX; i++)
				{
					pAp = Wifi_GetLastAp(i);
				//hyUsbPrintf("wifi info  %x\r\n",(U32)pAp);
					if(pAp != NULL)
					{
						cnt = i;
						break;
					}
				}
				hyUsbPrintf("wifi connect cnt = %d \r\n",i);
				if(i >= WIFI_SAVEPW_MAX)//���û���κ����Ӽ�¼,��������
				{
					gu32WifiConnDelay = rawtime(NULL);
					hyUsbPrintf("gu32WifiConnDelay = %d \r\n",gu32WifiConnDelay);
					break;
				}
				
				pBuf = hy_get_ap_info_bymac(pAp->mac);//�ҵ�֮ǰ���ӹ���ap
				if(pBuf != NULL)
				{
					if(pAp->pwType != WMI_CRYPTO_OPEN)
					{
						WifiPw_SetPwType(pAp->pwType);
					hyUsbPrintf("pw = %s \r\n", pAp->pw);
						WifiPw_SetPw(pAp->pw);
					}
					hy_ap_setinfo(pBuf);
					hyUsbPrintf("connect ap ===== \r\n");	
					if(1 == ComWidget_ConnectAp(pBuf))
					{
					hyUsbPrintf("connect ok----- \r\n");
						break;
					}
				}
		
				cnt++;
				if(cnt >= WIFI_SAVEPW_MAX)
				{
					break;
				}
			}//while(1)
		}//if(1 == ComWidget_SearchApList(NULL))//ɨ��AP
	}//while(1)
	
	semaphore_destroy(WifiConnect_p_Exit_sem, OS_DEL_NO_PEND);
	WifiConnect_p_Exit_sem = NULL;
	WifiConnectWait = NULL;
	
	return 1;
}


/*-------------------------------
* ����:WifiConnect_Start
* ����:����������
* ����:none
* ����:
*-------------------------------*/
int WifiConnect_Start(void)
{
	int pid, startValue, pidStatus = HY_ERROR;
	
	pid = ntopid(WIFICONNECT_P_NAME);
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
	
	if (pidStatus != HY_OK)
	{
		//���̲����ڣ���Ҫ����
		pid = createp((int (*)())WifiConnect_Process,
								WIFICONNECT_P_NAME,
								1024,
								WIFICONNECT_PRIOPITY,
								NULL,NULL,NULL);
	}
	startValue = pstart( pid, (void *)WifiConnect_Process );
	
	return HY_OK;
}

/*-------------------------------
* ����:WifiConnect_p_Exit
* ����:�˳�������
* ����:none
* ����:
*-------------------------------*/
void WifiConnect_p_Exit(void)
{
	int pid;
	
	pid = ntopid(WIFICONNECT_P_NAME);
	if (pid<0 || pid>30) return;
	
	if (WifiConnect_p_Exit_sem != NULL)
		semaphore_post(WifiConnect_p_Exit_sem);
	
	ewakeup(&WifiConnectWait);
	
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	deletep(pid);
}