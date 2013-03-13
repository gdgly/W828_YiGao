#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "hyhwChip.h"
#include "hyhwIntCtrl.h"
#include "hyhwGpio.h"

#include "std.h"
#include "sys.h"
#include "ospriority.h"

#include "semaphore.h"
#include "barDecode_p.h"
#include "glbVariable.h"
#include "appControlProcess.h"

#define BARCODE_P_NAME		"bar_decode_p"


SEMAPHORE *barDecode_p_Exit_sem = NULL;
int *EventBarDecode = NULL;

U32 barDecode_runningTicks = 0;
U8	barDecodeEnable = 0;

U32 gu32barLastScanTime;


////////////////////////////////////////////////////////////////////
int barDecode_Process(void);

/*---------------------------------------------------------------------------
 ����: barDecode_p_Start
 ����: ����ɨ����̣�������������
 ����: 
 ����: HY_ERROR---�ý����������У���ǰ������Ч
 	   HY_OK------�����ɹ�
* -----------------------------------------------------------------------------*/
int barDecode_p_Start(void)
{
	int pid, startValue, pidStatus = HY_ERROR;
	
	pid = ntopid(BARCODE_P_NAME);
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
	barDecode_p_Exit_sem = semaphore_init(0);
	barDecode_runningTicks = rawtime(NULL);

	if (pidStatus != HY_OK)
	{
		//���̲����ڣ���Ҫ����
		pid = createp((int (*)())barDecode_Process,
								BARCODE_P_NAME,
								1024,
								BARCODE_PRIOPITY,
								NULL,NULL,NULL);
	}
	startValue = pstart( pid, (void *)barDecode_Process );
	return HY_OK;
}

/*---------------------------------------------------------------------------
* ����:	barDecode_Enable
* ����:	ʹ��ɨ�蹦�ܣ����ɨ��ɹ�������Ϩ�𼤹⣻���򣬳�ʱϨ�𼤹�
* ����:	enableSecond----ɨ�輤�����ʱ��(��)
* ����:	none
* -----------------------------------------------------------------------------*/
void barDecode_Enable(U32 enableSecond)
{
#if 0
	// �������û�����У�������
	//barDecode_p_Start();
	barApp_powerOnOff(1);
	barDecode_runningTicks = enableSecond*100+rawtime(NULL);
//	ewakeup(&EventBarDecode);
	barApp_laserCtrl(1);
#endif
}

/*---------------------------------------------------------------------------
* ����:	barDecode_Enable
* ����:	��ֹɨ�蹦�ܣ��������ٽ���
* ����:	none
* ����:	none
* -----------------------------------------------------------------------------*/
void barDecode_Disable(void)
{
	//barDecode_runningTicks = rawtime(NULL);
	barApp_laserCtrl(0);
}

/*---------------------------------------------------------------------------
 ����: barDecode_p_Exit
 ����: ֹͣɨ�蹦��
 ����: ��
 ����: ��
* -----------------------------------------------------------------------------*/
void barDecode_p_Exit(void)
{
	int pid;
	
	pid = ntopid(BARCODE_P_NAME);
	if (pid<0 || pid>30) return;
	
	if (barDecode_p_Exit_sem != NULL)
		semaphore_post(barDecode_p_Exit_sem);
	
	ewakeup(&EventBarDecode);
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	deletep(pid);
}

/*---------------------------------------------------------
* ����: barDecode_p_IsRunnning
* ����: ɨ������Ƿ�������
* ����: none
* ����: =1----��������
*		=0-----û������
*---------------------------------------------------------*/
int barDecode_p_IsRunnning(void)
{
	int pid;
	
	pid = ntopid(BARCODE_P_NAME);
	if ((pid > 0) && (PSPROC(pid)->state != P_INACT))
	{
		return 1;
	}
	
	return 0;
}

void barDecode_Wakeup(void)
{
	ewakeup(&EventBarDecode);
}
/*---------------------------------------------------------------------------
 ����: barDecode_Process
 ����: ɨ�����
 ����: ��
 ����: ���Ƿ���1������ϵͳҪ��
* -----------------------------------------------------------------------------*/
int barDecode_Process(void)
{
	int ret;
	unsigned ilev;

	barScanner_init();
	barDecode_Wakeup_Config(barDecode_Wakeup);
	barApp_powerOnOff(0);
	while(1)
	{
		ilev = setil(_ioff);
		ewait(&EventBarDecode);
		//ewaittm(&EventBarDecode, 2);
		(void) setil(ilev);
	//	syssleep(1);
		barApp_powerCtrl();
		
		ret = semaphore_trywait(barDecode_p_Exit_sem);
		if (ret == 0)
		{
			//��Ӧ�˳�Ҫ���˳�����
			barApp_laserCtrl(0);
			break;
		}
		#if 0
		if (barDecode_runningTicks!=0 &&
			rawtime(NULL)>=barDecode_runningTicks)
		{
			gu32barLastScanTime = rawtime(NULL);
			//����ʱ�䵽��ֹͣ����
			barApp_laserCtrl(0);
			barApp_powerOnOff(0);
			continue;
		}
		#endif
		
		if(barApp_decodeBarcode() > 0)
		{
			//barApp_powerOnOff(0);
		}
		barApp_powerCtrl();
		
		#if 0
		{
			static U32 lastTick=0;
			U32 now = rawtime(NULL);
			
			if ((now-lastTick) >= 50)
			{
				PhoneTrace(0, "barApp_RUN...");
				lastTick = now;
			}
		}
		#endif
	} /* while(1) */

	barScanner_close();
	
	semaphore_destroy(barDecode_p_Exit_sem, OS_DEL_NO_PEND);
	barDecode_p_Exit_sem = NULL;
	EventBarDecode = NULL;
	
	return 1;
}

void bar_updatelasttime(U32 last)
{
	gu32barLastScanTime = last;
}

void bar_ifstartscan()
{
	U32 delay;

#if 0//��ʱȡ���Զ���������
	if(barGetScanType() == 1 && gtHyc.ScanTrigger == 1 && barDecode_p_IsRunnning() == 1)
	{
		delay = ABS(rawtime(NULL)-gu32barLastScanTime);
		
		if(delay >= 100 && barApp_getLaserState() == 0)
		{
			//�����Զ�����ʱ��
	    	cls_updata_lock_time(rawtime(NULL));

			barDecode_Enable(gtHyc.ScanLightTime);
		}
	}
#endif
	return ;
}

#if 0
int total = 0;
int okCnt = 0;
int errCnt = 0;
void testScan(void)
{

	barDecode_Enable(5);
	while(1)
	{	//�˳��l����ɨ��ɹ�����ʱ
		int len;U8 recvBuf[50];
		barBuf_readBarCode(&recvBuf[0], &len);
		if (len > 0)
		{
			//break;
			syssleep(10);
			barDecode_Enable(5);
		}
		else if (barApp_getLaserState() == 0)
		{
			//break;
			syssleep(50);
			barDecode_Enable(5);
		}
		syssleep(1);
	}

}
#endif
