#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "hyhwGpio.h"

#include "std.h"
#include "sys.h"
#include "ospriority.h"

#include "semaphore.h"
#include "vibratorCtrl_p.h"
#include "hardWare_Resource.h"
#include "appControlProcess.h"


#define VIBRATOR_CTRL_RUN_TIME		(100)//1�룬��ʱ��
#define VIBRATOR_CTRL_STOP_TIME		(100)//1�룬ֹͣʱ��



/*
                            RUN_TIME                   RUN_TIME
run ----------             ------------              ------------
stop          |___________|            |____________|            |_______
               STOP_TIME                STOP_TIME                 STOP_TIME
*/

SEMAPHORE *vibratorCtrlExit_sem = NULL;
U32 vibratorCtrlRunningTicks;

int VibratorCtrl_Process(void);


void VibratorCtrl_Init(void)
{
	hyhwGpio_setAsGpio(VIBRATOR_CTRL_PORT, VIBRATOR_CTRL_GPIO);
	hyhwGpio_setOut(VIBRATOR_CTRL_PORT, VIBRATOR_CTRL_GPIO);
	hyhwGpio_setLow(VIBRATOR_CTRL_PORT, VIBRATOR_CTRL_GPIO);
}

void VibratorCtrl_RunTime(int millisecond)
{
	VibratorCtrl_Start(millisecond);
	sleep(millisecond/10);
	VibratorCtrl_Stop();
}


/*---------------------------------------------------------------------------
 ����: VibratorCtrl_Start
 ����: ��������
 ����: millisecond----Ҫ������ʱ�䣬����Ϊ��λ��ʱ�䵽ʱ�������˳��ý���
 					 ����ֵΪ0ʱ���������˳���ֱ������VibratorCtrl_Stop()
 ����: HY_ERROR---�ý����������У���ǰ������Ч
 	   HY_OK------�����ɹ�
* -----------------------------------------------------------------------------*/
int VibratorCtrl_Start(U32 millisecond)
{
	int pid, startValue, pidStatus = HY_ERROR;
	int ret;
	
	VibratorCtrl_Stop();
	pid = ntopid(VIBRATOR_CTRL_NAME);
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
		vibratorCtrlRunningTicks = millisecond/10;//��msת��Ϊϵͳticks
		vibratorCtrlExit_sem = semaphore_init(0);
	
		//���̲����ڣ���Ҫ����
		pid = createp((int (*)())VibratorCtrl_Process,
								VIBRATOR_CTRL_NAME,
								VIBRATOR_SIZE,
								VIBRATOR_PRIOPITY,
								NULL,NULL,NULL);
	}
	startValue = pstart( pid, (void *)VibratorCtrl_Process );
	return HY_OK;
}

/*---------------------------------------------------------------------------
 ����: VibratorCtrl_Stop
 ����: ֹͣ����
 ����: ��
 ����: ��
* -----------------------------------------------------------------------------*/
void VibratorCtrl_Stop(void)
{
	int pid;
	
	pid = ntopid(VIBRATOR_CTRL_NAME);
	if (pid<0 || pid>30) return;
	
	if (vibratorCtrlExit_sem != NULL)
		semaphore_post(vibratorCtrlExit_sem);
	
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	deletep(pid);
}

/*---------------------------------------------------------
* ����: VibratorCtrl_IsRunnning
* ����: �������ƽ����Ƿ�������
* ����: none
* ����: =1----��������
*		=0-----û������
*---------------------------------------------------------*/
int VibratorCtrl_IsRunnning(void)
{
	int pid;
	
	pid = ntopid(VIBRATOR_CTRL_NAME);
	if ((pid > 0) && (PSPROC(pid)->state != P_INACT))
	{
		return 1;
	}
	
	return 0;
}


/*---------------------------------------------------------------------------
 ����: VibratorCtrl_Process
 ����: �������ƽ���
 ����: ��
 ����: ���Ƿ���1������ϵͳҪ��
* -----------------------------------------------------------------------------*/
int VibratorCtrl_Process(void)
{
	int ret, vibratorAct;
	U32 runningTicks, ticks_cnt;
	
	VibratorCtrl_Init();
	
	runningTicks = 0;
	ticks_cnt = 0;
	vibratorAct = 1;//һ��ʼ����������
	while(1)
	{
		ret = semaphore_trywait(vibratorCtrlExit_sem);
		if (ret == 0)
		{
			//��Ӧ�˳�Ҫ��ֹͣ�������˳�����
			hyhwGpio_setLow(VIBRATOR_CTRL_PORT, VIBRATOR_CTRL_GPIO);
			break;
		}
		if (vibratorCtrlRunningTicks!=0 &&
			runningTicks>=vibratorCtrlRunningTicks)
		{
			//����ʱ�䵽��ֹͣ�������˳�����
			hyhwGpio_setLow(VIBRATOR_CTRL_PORT, VIBRATOR_CTRL_GPIO);
			break;
		}
		
		if (vibratorAct == 1)
		{
			if (ticks_cnt == 0)
			{
				//��������
				hyhwGpio_setHigh(VIBRATOR_CTRL_PORT, VIBRATOR_CTRL_GPIO);
			}
			else if (ticks_cnt >= VIBRATOR_CTRL_RUN_TIME)
			{
				//��ʱ�䵽����ֹͣ����
				vibratorAct = 0;
				ticks_cnt = 0;
				continue;
			}
		}
		else
		{
			if (ticks_cnt == 0)
			{
				//ֹͣ��
				hyhwGpio_setLow(VIBRATOR_CTRL_PORT, VIBRATOR_CTRL_GPIO);
			}
			else if (ticks_cnt >= VIBRATOR_CTRL_STOP_TIME)
			{
				//ֹͣ��ʱ�䵽��������
				vibratorAct = 1;
				ticks_cnt = 0;
				continue;
			}
		}
		ticks_cnt++;
		runningTicks++;
		syssleep(1);
		
	} /* while(1) */

	semaphore_destroy(vibratorCtrlExit_sem, OS_DEL_NO_PEND);
	vibratorCtrlExit_sem = NULL;
	
	return 1;
}


