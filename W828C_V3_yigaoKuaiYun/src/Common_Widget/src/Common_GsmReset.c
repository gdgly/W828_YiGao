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

/////////////////////////////////////////////////////////////////////////////////
#define GSMRESET_P_NAME		"GsmReset_p"

SEMAPHORE	*GsmReset_p_Exit_sem = NULL;
int			*GsmResetWait = NULL;

U8 gu8GsmIsReset;//GSM�Ƿ����ڸ�λ   1����   0����
U8 gsmReset_enable = 1;

//////////////////////////////////////////////////////////////////////////
/*-------------------------------
* ����:GsmReset_IfReset
* ����:��ȡ��ǰGSMģ��״̬(�Ƿ�������)
* ����:none
* ����:0����������   1��������
*-------------------------------*/
U8 GsmReset_IfReset()
{
	return gu8GsmIsReset;
}

void GsmReset_SetState(U8 state)
{
	gu8GsmIsReset = state;
}
/*-------------------------------
* ����:GsmReset_WakeUp_Ctrl
* ����:������ֹ����GSM RESET����
* ����:none
* ����:none
*-------------------------------*/
void GsmReset_WakeUp_Ctrl(U8 enable)
{
	gsmReset_enable = enable;
}

/*-------------------------------
* ����:GsmReset_WakeUp
* ����:���ѽ���
* ����:none
* ����:none
*-------------------------------*/
void GsmReset_WakeUp()
{
	if(GsmResetWait != NULL)
	{
		ewakeup(&GsmResetWait);
	}
}

/*-------------------------------
* ����:GsmReset_Process
* ����:GSMģ���������ʱ����ģ��
* ����:none
* ����:1
*-------------------------------*/
int GsmReset_Process(void)
{
	unsigned ilev;
	int ret;
	
	GsmReset_p_Exit_sem = semaphore_init(0);
	
	while(1)
	{
		GsmReset_SetState(0);
		
		ret = semaphore_trywait(GsmReset_p_Exit_sem);
		if(ret == 0)
		{
			break;
		}
		
		ilev = setil(_ioff);
		ewait(&GsmResetWait);
		(void) setil(ilev);
		
		ret = semaphore_trywait(GsmReset_p_Exit_sem);
		if(ret == 0)
		{
			break;
		}
		
		if (gsmReset_enable == 0)
		{
			//GSM RESET�Ѿ�����ֹ����ת
			continue;
		}
		
		//��GSMģ��״̬Ϊ����
		GsmReset_SetState(1);
		
		Ie_clear_connectfailcnt();
		//�ر�gprs
		ppp_close(); 
		ppp_clear_brg_flag();
		//�ȴ�gprs�ر�
		while((int *)ppp_get_eventppp() == NULL)
		//while(gprs_get_status() == 0)
		{
		//hyUsbMessage("close ppp\r\n");
			syssleep(5);
		}
		//hyUsbMessage("ppp exit\r\n");
		//ppp_p_Exit();
		
		ret = semaphore_trywait(GsmReset_p_Exit_sem);
		if(ret == 0)
		{
			break;
		}
		
		if(gtHyc.NetSelected == 1)
		{
			Ie_clear_connectfailcnt();
			netdev_close_tcpip();
		}
		
		gsmcdma_stop_service();
		
		if(gtHyc.NetSelected == 1)
		{
			syssleep(100);
			hyIE_resetSocket();
		}
		
		//��ʼ����ģ��
		
		ret = semaphore_trywait(GsmReset_p_Exit_sem);
		if(ret == 0)
		{
			break;
		}
		
		//hyUsbMessage("restart gsm\r\n");
		Gsm_InitToSdram_buf();
		gsm_SetPinCode(getPINCode());
		//hyUsbMessage("BBreset gsm\r\n");
		hyhwGsm_BBreset();
		syssleep(100);
		ret = semaphore_trywait(GsmReset_p_Exit_sem);
		if(ret == 0)
		{
			break;
		}
		
		AppCtrl_CreatPro(GSMCDMA_CLIENT_AN,0);
    	AppCtrl_CreatPro(GSMCDMA_SERVER_AN,0);
    	
    	AppCtrl_CreatPro(TCPIP_AN,0);
    	//hyUsbMessage("sleep gsm\r\n");
    	syssleep(100);
		SIM_set_pin_stat(1);
		//hyUsbMessage("set_pin\r\n");
		
		syssleep(600);
		
		Rec_SetScanTime(rawtime(NULL));
	}
	
	semaphore_destroy(GsmReset_p_Exit_sem, OS_DEL_NO_PEND);
	GsmReset_p_Exit_sem = NULL;
	GsmResetWait = NULL;
	
	return 1;
}

/*-------------------------------
* ����:GsmReset_Start
* ����:����GSM������
* ����:none
* ����:
*-------------------------------*/
int GsmReset_Start(void)
{
	int pid, startValue, pidStatus = HY_ERROR;
	
	pid = ntopid(GSMRESET_P_NAME);
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
		pid = createp((int (*)())GsmReset_Process,
								GSMRESET_P_NAME,
								1024,
								GSMRESET_PRIOPITY,
								NULL,NULL,NULL);
	}
	startValue = pstart( pid, (void *)GsmReset_Process );
	
	return HY_OK;
}

/*-------------------------------
* ����:GsmReset_p_Exit
* ����:�˳�GSM������
* ����:none
* ����:
*-------------------------------*/
void GsmReset_p_Exit(void)
{
	int pid;
	
	pid = ntopid(GSMRESET_P_NAME);
	if (pid<0 || pid>30) return;
	
	if (GsmReset_p_Exit_sem != NULL)
		semaphore_post(GsmReset_p_Exit_sem);
	
	ewakeup(&GsmResetWait);
	
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	deletep(pid);
}