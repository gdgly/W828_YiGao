 /*----------------------�ļ�����--------------------------------
* ��������: 2008/05/29 
* ����    : ����ϵͳ�Ĺ��Ŀ��Ʋ���
*--------------------------------------------------------------*/

/*
*include files
*/
#include "hyTypes.h"

#include "hyhwGpio.h"
#include "hyhwIntCtrl.h"

#include "std.h"
#include "sys.h"
#include "ospriority.h"

#include "AppControlProcess.h"
#include "AppControlConsume.h"
#include "hardWare_Resource.h"

#include "hyhwTcc7901.h"
#include "Hyc_SysSleepMode.h"
#include "hyhwCkc.h"
#include "HycDeviceConfig.h"
#include "glbStatusDef.h"
#include "glbVariable.h"

volatile U32 gu32WakeMode = 0;  //����ģʽ
volatile U32 gu32PwnDCnt = 0;


U8  gu8GprsMode;//gprs����ģʽ  0������  1���ϴ�  2������

U8	su8GpsWorkMode = 0;

volatile U32 gu32LastTicks = 0;	


U8 GpsIsRun()
{
	return su8GpsWorkMode;
}

void Gprs_setMode(U8 mode)
{
	if(0x0100 == mode)
	{
		gu8GprsMode = 0 ;
	}
	else if(mode > 3) /* ��Ϊ���ϴ���¼���ϴ�λ�÷ֱ�����Ϊ0x01��0x02 ������Ϊ���� */
	{
		gu8GprsMode = 2;
	}
	else
	{
		gu8GprsMode = mode;
	}
}

U8 Gprs_getMode(void)
{
	return gu8GprsMode;
}


//����ϵͳ�ϴε�ʱ��
void Grap_UpdataLastTick(void)
{
    gu32LastTicks = rawtime(NULL);    
}

U32 Grap_GetLastTick()
{
	return gu32LastTicks;
}

void Grap_SetLastTick(U32 tick)
{
	gu32LastTicks = tick;
}
static void PowerOnPeripherals(void)
{
	//hyhwAk4183_SleepAndIntOn();
	
	//�ָ����ڹ���
	hyhwUart_InitPort();
	//Test_LedLight();
	scanner_reinit();
	//Test_LcdLight();
	hyhwAdc_HwInit();
	
	hyhwTimer_countEnable(4);
}

static void PowerOffPeripherals(void)
{
	hyhwTimer_countDisable(4);
	hyhwAdc_PowerIdle();	
	//hyhwAk4183_PenDetectHi_z();
	/* ��RTC���� */
	hyhwCpm_busClkEnable(CKC_RTC);
	
	/* �ر�ɨ��ͷ */
	scanner_deinit();
	
	hyhwCmosSensor_powerCtrl(EnableOff);
}

/*---------------------------------------------------------------------------
* ����: ConSume_SetWakeMode
* ����: ����ϵͳ����ģʽ
* ����: none
* ����: 1: ����ϵͳ�󣬱��ⲻ���������Զ�����������û��ѷ�ʽ���û���֪��
*		2: ����ϵͳ�󣬿����⣬�û����Խ��в���
*		0������˯��
*	
* -----------------------------------------------------------------------------*/
U32 SysSleep_SetWakeMode(U32 mode)
{
    gu32WakeMode = mode;
    
    return mode;
}
/*---------------------------------------------------------------------------
 ������ConSume_CheckSdioWake
 ���ܣ��ж��Ƿ���sdio����ϵͳ���������ָ�ϵͳ
 ������none
 ���أ�1:sdio 2��key 0������˯��
* -----------------------------------------------------------------------------*/
U32 SysSleep_CheckWakeMode(void)
{
    return gu32WakeMode;
}

U32 USB_isRunning(void)
{
	int pid;
	
	pid = ntopid(USB_HANDLER_NAME);
	if ((pid > 0) && (PSPROC(pid)->state != P_INACT))
	{
		return 1;
	}
	
		return 0;
}

U32 SysSleep_isValid(void)
{
	U32 result = 1;
	U32 wakeUpMode, sleepFlag, now;
	
	
	if (BackLight_HaveLight() == 1)
	{
		/* δ�رձ��⣬������������ģʽ */
		return (0);//������
	}
	if(1 == Battery_DcInCheck()) //���ڳ�粻����
	{
		return (0);
	}

	if (1 != drvKeyb_PermitPowerSave())
	{   /*�Ƿ�����������*/
	    return (0);
	}	
	
	if (USBHandler_GetScsiFlag() != 0 ||
		USB_isRunning() != 0)
	{
		/* ���ڳ������USB��������������ģʽ */
		return (0);//������
	}	
	
    if (0 == Battery_Level())
    {
        /*090416�͵�����ʱ������ϵͳ����͹���״̬*/
        return (0);
    }    
	
	if (hyco_getConnectState() == TRUE)
	{
		//bt��������״̬�����������
		return 0;
	}
	
	if (gtHyc.NetSelected == 1)//ѡ���gsm����
    {
		if(modem_get_sleep_enable() == 0)
		{
			/*	��ֹ˯�ߣ����������͹���ģʽ */
			return (0);//������
		}
		
	    if((net_onlyRead_regStatus() >= 0 && gprs_get_status() >= 0 ) || GsmReset_IfReset() == 1)
	    {
	    	 /* gprs���ڴ��䣬������������ */
	    	return (0);
	    }
    }
    else//ѡ���wifi����
    {
		if ((hy_get_ap_link_status() == 1) &&				//wifi������
			(Gprs_getMode()!=0 || Net_GetEvt()!=0))	//�����緢���¼���׼������
		{
			return 0;
		}
    }
    
    if(0 != GpsIsRun())
    {
    	return (0);
    }
	
	/* �ж��Ƿ������������� */
	wakeUpMode = SysSleep_CheckWakeMode();
	if (wakeUpMode == 1)
	{
		now = rawtime(NULL);
		if (ABS(now - gu32PwnDCnt) < 5)
		{
			/* �������Ѻ�����ʱ��δ�������������� */
			return (0);//������
		}
	}
	else if (wakeUpMode == 2)
	{//�Ѿ��˳����ˣ������������̻�û�л�������
		now = rawtime(NULL);
		if (ABS(now - gu32PwnDCnt) < 200)
		{
			/* �������Ѻ�����ʱ��δ�������������� */
			return (0);//������
		}
	}
	
	return (1);//����������ģʽ
}


extern U32 gTime;
void SysSleepHandler(void)
{
	U32 wakeupFlag;
	U8  intLevel;
	U32 inPutPort, inPutGpio;
	U32 enterTime, leaveTime, i;
	
	if (SysSleep_isValid() == 0)
	{
		return;
	}
	
	intLevel = InterruptDisable();
	
	PowerOffPeripherals();	//�ر�����
	//Battery_WriteInfo("sys sleep in!\r\n");
	
	#if 1
	if (gtHyc.NetSelected == 1)//ѡ���gsm����
	{
		/*	����CTS GSMģ������������
			CTS���ָߵ�ƽ��֪ͨGSMģ��ϵͳ���������ݡ�
			CTS���ֵ͵�ƽ��֪ͨGSMģ��ϵͳ���Խ������ݡ�*/
		hyhwGpio_setHigh(GSM_UART0_CTS_PORT,GSM_UART0_CTS_GPIO);

	}
	#endif

	enterTime = gTime;
	Hyc_SysSleepOverlay();
	Hyc_Set_netFlag(gtHyc.NetSelected);
	//�Ƚ�Ƶ��12MHz��Ϊ����core��ѹ���ϵ�
	hyhwSysClk_Set(ICLK12M_AHB6M, EXTAL_CLK);

#ifdef HYC_DEV_USE_WATCHDOG
	hyhwWatchdog_disable();
#endif	
	
	//LedRed_statusCtrl(1);
	//wakeupFlag = Hyc_SysSleepMain();
	hyhwArm926e_writeBackDcache();
	hyhwArm926ejs_drainWriteBuffer();
	hyhwArm926ejs_invalidIcacheDcache();
	
	hyhwArm926ejs_disableIcache();
	hyhwArm926ejs_disableDcache();
	wakeupFlag = Hyc_SysSleepMain();
	
	//hyhwSysClk_Set(ICLK192M_AHB96M, EXTAL_CLK);
	hyhwSysClk_Set(ICLK96M_AHB48M, EXTAL_CLK);
	/* ϵͳ��n������û�����У������ﲹ��(����ϵͳʱ��) */
	leaveTime = gTime;
	_clock_s(ABS(leaveTime-enterTime));
	
	ConSume_ChangeMode();
	if ((wakeupFlag & (UART1_WAKEUP|KEY_WAKEUP|LOW_POWER_WAKEUP|USB_VBUS_WAKEUP|POWER_OFF_WAKEUP|G_KEY_WAKEUP|W_KEY_WAKEUP)) != 0)
	{	
		//hyhwApi_stm102Wake();
		PowerOnPeripherals();						//������		
		//InterruptRestore(intLevel);
		enable_IF();
	#ifdef HYC_DEV_USE_WATCHDOG	
		hyhwWatchdog_feed();
		hyhwWatchdog_enable();
	#endif	
		
		#if 1
		if (gtHyc.NetSelected == 1)//ѡ���gsm����
    	{
			/* ����CTS ����GSMģ����CPU�������� */
			hyhwGpio_setLow(GSM_UART0_CTS_PORT,GSM_UART0_CTS_GPIO);
		}
		#endif
			
		syssleep(10);
		drvKeyb_ClearKey();
		
		//ConSume_ChangeCoefTemp(120,0);
		USB_InitMemory_buf();
		
		SysSleep_SetWakeMode(2);
	    gu32PwnDCnt = rawtime(NULL);//ticks
	    
		if ((wakeupFlag&KEY_WAKEUP))
		{
			//PhoneTrace(0,"power key");
			
			//AppCtrl_WakeupPutGWKey(2);
			drvKeyb_SetEvent(Keyb_Code_Power, cKEYB_EVENT_RELEASE);
		}
		else
		{
			//AppCtrl_WakeupPutGWKey(2);
		}
		
		
		//����timerʱ��
		Grap_UpdataLastTick();
		
		if ((wakeupFlag & POWER_OFF_WAKEUP) != 0)
		{
			/*	�ڴ���ģʽ�£�����power�����ػ� */
			
		}
	}
	else
	{
		/*	SDIO_WAKEUP  UART1_WAKEUP  TIME_30S_WAKEUP ����
			���Ϸ�ʽ����ʱ���������򿪣�ֻ���ں�̨�������û���֪��
		*/
		PowerOnPeripherals();
		//InterruptRestore(intLevel);
		enable_IF();
	#ifdef HYC_DEV_USE_WATCHDOG	
		hyhwWatchdog_feed();
		hyhwWatchdog_enable();
	#endif	
		
		syssleep(1);
		drvKeyb_ClearKey();
		USB_InitMemory_buf();
		
		SysSleep_SetWakeMode(1);
		gu32PwnDCnt = rawtime(NULL);//ticks
		//add
		//ConSume_ChangeCoefTemp(360);
		//����timerʱ�䣬���ܼӻ�Ӱ��alarmʱ��
		Grap_UpdataLastTick();
				
		#if 1
		if (gtHyc.NetSelected == 1)//ѡ���gsm����
    	{
			/* ����CTS ����GSMģ����CPU�������� */
			hyhwGpio_setLow(GSM_UART0_CTS_PORT,GSM_UART0_CTS_GPIO);
		}
		#endif
		
	}
	
	barBuf_init();
	//Battery_WriteInfo("sys sleep out!\r\n");
}

/* end of file */
/*---------------------------------------------------------------------
090821 : jiezhang  ���ӵ͹����˳�USB overlay USB_InitMemory(); 

----------------------------------------------------------------------*/