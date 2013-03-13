
#include "std.h"
#include "sys.h"
#include "ospriority.h"
#include "semaphore.h"

#include "appControlProcess.h"



int gSIM_pin_ready = 0;
int gsmcdma_client_exit = 0;
int gsmcdma_server_exit = 0;
SEMAPHORE *gsmcdma_client_exit_sem = NULL;
SEMAPHORE *gsmcdma_server_exit_sem = NULL;


/*---------------------------------------------------------
*����: SIM_set_pin_stat
*����: ����SIM����֤PIN��PUK���״̬
*����: stat	---	0:û��ͨ����֤	1:��֤OK
*����: none
*---------------------------------------------------------*/
void SIM_set_pin_stat(int stat)
{
	gSIM_pin_ready = stat;
}


/*---------------------------------------------------------
*����: SIM_get_pin_stat
*����: ��ѯSIM����֤PIN��PUK���Ƿ�ͨ��
*����: 
*����: 0:û��ͨ����֤		1:��֤OK
*---------------------------------------------------------*/
int SIM_get_pin_stat(void)
{
	return gSIM_pin_ready;
}

void gsmcdma_powerOffthread(void)
{
	gsmcdma_adapter_poweronoff(1);
	syssleep(50);
	gsmcdma_adapter_poweronoff(0);
}

void gsmcdma_powerOff_start(void)
{
	int pid, startValue;
	
	pid = createp((int (*)())gsmcdma_powerOffthread,
							"gsmPowerOff_p",
							128,
							PRIORITY_4_OSPP,
							NULL,NULL,NULL);
	startValue = pstart( pid, (void *)gsmcdma_powerOffthread );
}

void gsmcdma_powerOff_stop(void)
{
	int pid;
	
	pid = ntopid("gsmPowerOff_p");
	if (pid<0 || pid>30) return;
	
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	deletep(pid);
}
/*---------------------------------------------------------
*����: gsmcdma_stop_service
*����: �ر�gsm/cdma
*����: 
*����: none
*---------------------------------------------------------*/
void gsmcdma_stop_service(void)
{
#if 0
	gsmcdma_adapter_poweronoff(0);
#else
// 
	if (0 == AppCtrl_IsValidPro(GSMCDMA_SERVER_AN))	 return;
	
	//stop gsmcdma client thread
	gsmcdma_client_exit_sem = semaphore_init(0);
	gsmcdma_client_exit = 1;
	//semaphore_wait(gsmcdma_client_exit_sem);
	semaphore_timed_wait(gsmcdma_client_exit_sem,500);
	semaphore_destroy(gsmcdma_client_exit_sem,OS_DEL_NO_PEND);
	gsmcdma_client_exit_sem = NULL;
	gsmcdma_client_exit = 0;
	
	AppCtrl_DeletePro(GSMCDMA_CLIENT_AN);

	//stop gsmcdma server thread
	gsmcdma_server_exit_sem = semaphore_init(0);
	gsmcdma_server_exit = 1;
	//semaphore_wait(gsmcdma_server_exit_sem);
	semaphore_timed_wait(gsmcdma_server_exit_sem,500);
	semaphore_destroy(gsmcdma_server_exit_sem,OS_DEL_NO_PEND);
	gsmcdma_server_exit_sem = NULL;
	gsmcdma_server_exit = 0;
	
	AppCtrl_DeletePro(GSMCDMA_SERVER_AN);
#endif
}



extern tMSG_QUEUE *guser_reqmsg_que;
/*---------------------------------------------------------
*����: GsmCdmaServer_ThreadMethodInit
*����: ��ʼ��һЩ������overlay������
*����: 
*����: none
*---------------------------------------------------------*/
static void GsmCdmaServer_ThreadMethodInit(void)
{
	gsmcdma_server_exit = 0;
	guser_reqmsg_que = NULL;
	SIM_set_pin_stat(0);
	init_variable();

}



/*---------------------------------------------------------
*����: GsmCdmaServer_ThreadMethod
*����: GsmCdmaServer����
*����: 
*����: none
*---------------------------------------------------------*/
int GsmCdmaServer_ThreadMethod(void)
{
	GsmCdmaServer_ThreadMethodInit();
    //SIM_set_pin_stat(1);

	gsmcdma_server();

	return 1;
}


