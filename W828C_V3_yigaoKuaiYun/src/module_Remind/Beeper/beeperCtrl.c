/*-------------------------------------------------------------------------
* 
* �ý��������������  �����ŷ������Ľӿ�
* ʹ�� ���� : ֱ�ӵ��� beeper_Start() ,������������������ ,��ɲ���,�������
*             ���Զ�ֹͣ,Ҳ��ǿ��ֹͣ ,�Լ����Ա༭���ַ���������,����������
*
*  ע��     : 1.���ȼ��ߵĽ��̵��û������� 
*             2.������С������syssleep(1) �� 10ms
* -------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
* inlcude
* -------------------------------------------------------------------------*/
#include "std.h"
#include "ospriority.h"
#include "sys.h"
#include "semaphore.h"
#include "LedCtl.h"
#include "beeperCtrl.h"
#include "appControlProcess.h"
#include "hardWare_Resource.h"

/*-------------------------------------------------------------------------
* macro
* -------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
* globe
* -------------------------------------------------------------------------*/
SEMAPHORE *beeperExit_sem = NULL;

BEEP_MUSIC_DATA_st *gpstBeepData = NULL;
S16 gBeepCnt;
U16 gRhythmCnt;

U8	gBeepEnable;
U8	gLedEnable;

/*-------------------------------------------------------------------------
* declear
* -------------------------------------------------------------------------*/
void beeper_Running(void) ;
void beeper_Stop(void);

/*-------------------------------------------------------------------------
* ����: beeper_PwmInit
* ˵��: ��ʼ�������÷�������Ƶ��
* ����: ��
* ����: ��
* -------------------------------------------------------------------------*/
void beeper_PwmInit(int period)
{
	hyhwGpio_setAsGpio(BUZZ_PWM_PORT, BUZZ_PWM1_GPIO);
	hyhwGpio_setAsPwmFunction(BUZZ_PWM_PORT, BUZZ_PWM1_GPIO);
	hyhwTimer_setPwm(BEEPER_PWM1_TIMER, 1000000/period, 50);
}
/*-------------------------------------------------------------------------
* ����: beeper_PwmDeinit
* ˵��: ��ʹ�ܷ�����
* ����: ��
* ����: ��
* -------------------------------------------------------------------------*/
void beeper_PwmDeinit(void)
{
	hyhwGpio_setAsGpio(BUZZ_PWM_PORT, BUZZ_PWM1_GPIO);
	hyhwGpio_setOut(BUZZ_PWM_PORT, BUZZ_PWM1_GPIO);
	hyhwGpio_setHigh(BUZZ_PWM_PORT, BUZZ_PWM1_GPIO);
	hyhwTimer_pwmDisable(BEEPER_PWM1_TIMER);
}
/*-------------------------------------------------------------------------
* ����: beeper_Init
* ˵��: ��ʼ��������
* ����: ��
* ����: ��
* -------------------------------------------------------------------------*/
void beeper_Init(BEEP_TYPE_en beep_type, int beepEnable, int ledEnable)
{	
	//gpstBeepData = pBeep_data;
	gpstBeepData = (BEEP_MUSIC_DATA_st *)beepData[beep_type];
	gBeepCnt = -1;
	/* ׼���������У����︳1���Ա����ն��н�������(PWM)���� */
	gRhythmCnt = 1;		//
	
	gBeepEnable = beepEnable;
	gLedEnable = ledEnable;
}

/*-------------------------------------------------------------------------
* ����: beeper_Start
* ˵��: ����������
* ����: 
* ����: ��
* -------------------------------------------------------------------------*/
int beeper_Start(BEEP_TYPE_en beep_type, int beepEnable, int ledEnable)
{
	int pid, startValue, pidStatus = HY_ERROR;
	int ret;
	
	if (gpstBeepData != NULL || beep_type == 0xff) return HY_ERROR;
	
	/*pid = ntopid(BEEPER_PRO_NAME);
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
	}*/
	beeper_Stop();
	beeperExit_sem = semaphore_init(0);
	
	if (pidStatus != HY_OK)
	{		
		beeper_Init(beep_type,beepEnable,ledEnable);
		//���̲����ڣ���Ҫ����
		pid = createp((int (*)())beeper_Running,
								BEEPER_PRO_NAME,
								BEEPER_SIZE,
								BEEPER_PRIOPITY,
								NULL,NULL,NULL);
	}
	startValue = pstart( pid, (void *)beeper_Running );

	return HY_OK;
	
}
/*-------------------------------------------------------------------------
* ����: beeper_Running
* ˵��: ���������������
* ����: ��
* ����: ��
* -------------------------------------------------------------------------*/
void beeper_Running(void)
{
	int tone;
	int ret ,powerState ;
	U32 start = rawtime(NULL);
	
	while(1)
	{		
		ret = semaphore_trywait(beeperExit_sem);
		if (ret == 0) /* �ֶ�ǿ���˳� */
		{			
			break;
		}
		
		
		/* ��������Ҫ���� */
		gRhythmCnt--;
		if (gRhythmCnt == 0)
		{
			/* ����ʱ�䵽��������һ���������� */
			gBeepCnt++;
			tone = gpstBeepData[gBeepCnt].tone;
			/* ��ȡ���� */
			if(tone == 0) /* ���ֲ����� �Զ��˳� */
			{
				break;
			}
			else if (tone == 0xFF)/* ��ֹ���������У�ͣPWM */
			{			
				beeper_PwmDeinit();
				gRhythmCnt = gpstBeepData[gBeepCnt].rhythm;
				if (gLedEnable != 0)
				{
					Led_ctrl(0);
				}
			}
			else /* ����,������PWMƵ�� */
			{			
				if (gBeepEnable != 0)
				{
					beeper_PwmInit(beepTone[tone]);
				}
				gRhythmCnt = gpstBeepData[gBeepCnt].rhythm;
				if (gLedEnable != 0)
				{
					Led_ctrl(LED_GREEN);
				}
			}			
		}		
		syssleep(1) ;		
	}
	Led_ctrl(0);
	gpstBeepData = NULL;
	beeper_PwmDeinit();
	
	semaphore_destroy(beeperExit_sem, OS_DEL_NO_PEND);
	beeperExit_sem = NULL;
	
}
/*-------------------------------------------------------------------------
* ����: beeper_Stop
* ˵��: ֹͣ������
* ����: ��
* ����: ��
* -------------------------------------------------------------------------*/
void beeper_Stop(void)
{
	int pid;
	
	pid = ntopid(BEEPER_PRO_NAME);
	if (pid<0 || pid>30) return;
	
	if (beeperExit_sem != NULL)
		semaphore_post(beeperExit_sem);
	
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	deletep(pid);
}

/*-------------------------------------------------------------------------
* ����: beeper_State
* ˵��: ������״̬
* ����: ��
* ����: 1---��������
*       0---û����
* -------------------------------------------------------------------------*/
int beeper_State(void)
{
	int pid;
	
	pid = ntopid(BEEPER_PRO_NAME);
	if ((pid > 0) && (PSPROC(pid)->state != P_INACT))
	{
		return 1;
	}
	
	return 0;
}


void beeper_test(void)
{
	beeper_Start(SETTING_ERR,1,0);
}

