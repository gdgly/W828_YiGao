/*******************************************************************************
*  (C) Copyright 2010 hyco, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of hyco and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  hyco and is confidential in nature.  Its use and dissemination by    
*  any party other than hyco is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"

/*----------------------------------------------------------------------------
 * INCLUDES
 *----------------------------------------------------------------------------*/
#include "hyhwTimer.h"
#include "hyhwCkc.h"

/*----------------------------------------------------------------------------
 * extern function ����
 *----------------------------------------------------------------------------*/
//defined in hyhwCkc.c
extern U32 hyhwCpm_getTimerClk(void);

/*----------------------------------------------------------------------------
 * �����Ǻ�������
 *----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_getIntStat
* ����:	get the timer32 int status
* ����:	none
* ����:	irq status
*----------------------------------------------------------------------------*/
U32 hyhwTimer32_getIntStat(void)
{
	return (TIMER32_INTCTRL_REGISTER & 0x0000001f);
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_disableTimer
* ����:	stop timer 32
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_disableTimer(void)
{
	TIMER32_INTCTRL_REGISTER	&= ~TC32EN_ENABLE_ON;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_enableTimer
* ����:	enable timer
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_enableTimer(void)
{
	TIMER32_ENABLE_REGISTER		|= TC32EN_ENABLE_ON;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_enableIrq
* ����:	enable the timer 32 irq
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_enableIrq(U32 irqSelect)
{
	TIMER32_INTCTRL_REGISTER |= irqSelect|0x60000000;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_disableIrq
* ����:	disable the timer 32 irq
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_disableIrq(U32 irqSelect)
{
	
	TIMER32_INTCTRL_REGISTER &= (~irqSelect);
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_setMatch0
* ����:	set match 0 value
* ����:	matchValue
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setMatch0(U32 matchValue)
{
	TIMER32_MATCH0_REGISTER  = matchValue;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_setMatch1
* ����:	set match 1 value
* ����:	matchValue
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setMatch1(U32 matchValue)
{
	TIMER32_MATCH1_REGISTER  = matchValue;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_setLoadValue
* ����:	set timer 32 load value
* ����:	LoadValue 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setLoadValue(U32 LoadValue)
{
	TIMER32_LOAD_REGISTER   = LoadValue;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_setPrescaleValue
* ����:	set prescale value
* ����:	preScaleValue
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setPrescaleValue(U32 preScaleValue)
{
	TIMER32_ENABLE_REGISTER = preScaleValue - 1;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_setFrequency
* ����:	����TIMER32���ж�Ƶ�ʣ�TIMER32�������Ƚ�����ʱ��ԴΪ12��
* ����:	cmp1Freq��cmp2FreqҪС��preScaleFreq��������Ҫ����������
		exp:hyhwTimer32_setFrequency(6, 2, 3);
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setFrequency(U32 preScaleFreq, U32 cmp1Freq, U32 cmp0Freq)
{
	hyhwTimer32_setPrescaleValue(TC32_BASE_FREQ / preScaleFreq);
	hyhwTimer32_setLoadValue(1);
	
	
	cmp1Freq = preScaleFreq / cmp1Freq;
	cmp0Freq = preScaleFreq / cmp0Freq;
	
	hyhwTimer32_setMatch1(cmp1Freq);
	hyhwTimer32_setMatch0(cmp0Freq);
	
	if (cmp1Freq)
	{
		hyhwTimer32_enableIrq(TC32IRQ_EN1);
	}
	else
	{
		hyhwTimer32_disableIrq(TC32IRQ_EN1);
	}
	
	if (cmp0Freq)
	{
		hyhwTimer32_enableIrq(TC32IRQ_EN0);
	}
	else
	{
		hyhwTimer32_disableIrq(TC32IRQ_EN0);
	}
	
	if (cmp1Freq > cmp0Freq)
	{
		TIMER32_ENABLE_REGISTER	&= ~(RELOAD_CMP0|RELOAD_CMP1);
		TIMER32_ENABLE_REGISTER |= RELOAD_CMP1;
	}
	else
	{
		TIMER32_ENABLE_REGISTER &= ~(RELOAD_CMP0|RELOAD_CMP1);
		TIMER32_ENABLE_REGISTER |= RELOAD_CMP0;
	}
	
	hyhwTimer32_enableTimer();
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_SetFrequency
* ����:	����timer ʱ��
* ����:	timerID		-- ͨ��index 
		uwFrequency	-- ����ʱ���жϵ�Ƶ��
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_SetFrequency(U8 timerID,U32 uwFrequency)
{
	U32 timerValue,timerClk;
	
	tTIMER_REGISTER	*ptTimerReg;
	
	/* ������ */
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		
	/* ���ͨ������������ */
//	timerClk	= (hyhwCpm_getTimerClk()>> 1);			/* ��Ϊtimer �ڲ���ƵĬ��Ϊ2��Ƶ  example timer = 1Mhz */
	timerClk 	= 750;	
	timerValue	= (timerClk *1000)/uwFrequency ;		/* ��Ϊtimer0\timer1\timer2 ��16λ���������������ʱ����1M��ô�����ʱ��65.536ms */	
	
	ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	
	ptTimerReg->CONFIG	   &= ~(TIMER_BIT_COUNTER_ENABLE|TIMER_BIT_PWMMODEL_ENABLE);
	ptTimerReg->REFERENCE	= timerValue;
	ptTimerReg->CONFIG     |= (TIMER_BIT_INT_ENABLE|TIMER_BIT_COUNTER_ENABLE|BIT5|BIT4);
							   
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_SetFreeFrequency
* ����:	����Timer���жϵ�Ƶ��
* ����:	timerUnitID	-- timer ��ţ�ϵͳ��������ʱ�ӣ����Ϊ0~5
*		frequency	-- ����ʱ���жϵ�Ƶ��, ��float�ͣ����mpeg4��֡��/��
* ����:	HY_OK		-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_SetFreeFrequency (U32 timerID, float frequency)
{
	float loadValue;
	U32 timerValue,timerClk;
	tTIMER_REGISTER	*ptTimerReg;
	
	/* ������ */
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
		

	//timerClk	= (hyhwCpm_getTimerClk()>> 1);			/* ��Ϊtimer �ڲ���ƵĬ��Ϊ2��Ƶ  example timer = 1Mhz */	  		
	timerClk 	= 750;
	timerValue	= (timerClk *1000)/frequency + 0.5;		
	timerValue	= (U32) loadValue;
	
	ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR) + timerID;
	
	ptTimerReg->CONFIG 	   &= ~(TIMER_BIT_COUNTER_ENABLE|TIMER_BIT_PWMMODEL_ENABLE);
	ptTimerReg->REFERENCE  	= timerValue;
	ptTimerReg->CONFIG 	   |= (TIMER_BIT_INT_ENABLE|TIMER_BIT_COUNTER_ENABLE|BIT5|BIT4);
							  
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_readConter
* ����:	��ȡָ��Timer�ļ���ֵ
* ����:	timerID	-- timer ���ΪTIMER0~TIMER5
*		*conter	-- ����ָ��timer�ļ���ֵ
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_readConter( U32 timerID, U32 *conter )
{
	tTIMER_REGISTER   *ptTimerReg ;
	U32 rc = HY_OK;
	
	if(timerID > TIMER_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
		*conter = ptTimerReg->COUNTER;
	}
	return rc;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_setConter
* ����:	����ָ��Timer�ļ���ֵ
* ����:	timerID	-- timer ���ΪTIMER0~TIMER5
*		conter	-- ָ��timer�ļ���ֵ
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_setConter( U32 timerID, U32 conter )
{
	tTIMER_REGISTER   *ptTimerReg;
	U32 rc = HY_OK;
	
	if(timerID > TIMER_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
		ptTimerReg->COUNTER = conter;
	}
	return rc;
}


/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_setPwm
* ����:	��ʼ��PWM���ֵļĴ�����static����
* ����:	pwmID		-- ͨ��index 
*		Pwm_period	-- ����Pwm outputƵ��
*		duty		-- Pwm output full duty (high level)
*					   PWM����ķ�ΧΪ��50HZ -- 10KHZ  ռ�ձȿɵ���Ϊ1/100
*					   ������PWM����ķ�Χ�����Ϊ low level
* ����:	HY_OK		-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_setPwm(U8 pwmID,U32 pwmPeriod, U16 duty)
{
	U32 timerClk,pwmValue,waitValue,holdValue;
	tTIMER_REGISTER	*ptTimerReg;
	
	//�����Ϸ��Լ��
	if(pwmID > PWM_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	//timerClk = (hyhwCpm_getTimerClk()>> 1);  			/*��Ϊtimer �ڲ���ƵĬ��Ϊ2��Ƶ  */       
	timerClk 	= 750;
	pwmValue  = (timerClk * 1000) / pwmPeriod;
	waitValue = pwmValue * duty / 100; 		
	holdValue = pwmValue;
	
	ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR ) + pwmID;
	
	ptTimerReg->CONFIG 		   &= ~(TIMER_BIT_COUNTER_ENABLE|TIMER_BIT_PWMMODEL_ENABLE);
	ptTimerReg->REFERENCE 	 	= holdValue;
	ptTimerReg->MID_REFERENCE	= waitValue;
	ptTimerReg->CONFIG 		   |= (TIMER_BIT_PWMMODEL_ENABLE|TIMER_BIT_COUNTER_ENABLE|BIT5|BIT4); 	

	return HY_OK;
}

U32 hyhwTimer_set2Pwm(U8 pwmID1,U8 pwmID2,U32 pwmPeriod, U16 duty)
{
	U32 timerClk,pwmValue,waitValue,holdValue;
	tTIMER_REGISTER	*ptTimerReg;
	U32 configReg1, configReg2;
	U32 *pConfigReg1, *pConfigReg2;
	
	//�����Ϸ��Լ��
	if(pwmID1>PWM_NUM || pwmID2>PWM_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	//timerClk = (hyhwCpm_getTimerClk()>> 1);  			/*��Ϊtimer �ڲ���ƵĬ��Ϊ2��Ƶ  */       
	timerClk 	= 750;
	pwmValue  = (timerClk * 1000) / pwmPeriod;
	waitValue = pwmValue * duty / 100;
	holdValue = pwmValue;
	
	//PWM ID1 config
	ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR) + pwmID1;
	ptTimerReg->CONFIG 		   &= ~(TIMER_BIT_COUNTER_ENABLE|TIMER_BIT_PWMMODEL_ENABLE);
	ptTimerReg->REFERENCE 	 	= holdValue;
	ptTimerReg->MID_REFERENCE	= waitValue;
	ptTimerReg->COUNTER			= 0;
	configReg1 = ptTimerReg->CONFIG;
	configReg1 |= (TIMER_BIT_PWMMODEL_ENABLE|TIMER_BIT_COUNTER_ENABLE|BIT5|BIT4);
	pConfigReg1 = &ptTimerReg->CONFIG;
	
	//PWM ID2 config
	ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR) + pwmID2;
	ptTimerReg->CONFIG 		   &= ~(TIMER_BIT_COUNTER_ENABLE|TIMER_BIT_PWMMODEL_ENABLE);
	ptTimerReg->REFERENCE 	 	= holdValue;
	ptTimerReg->MID_REFERENCE	= waitValue;
	ptTimerReg->COUNTER			= holdValue/2;
	configReg2 = ptTimerReg->CONFIG;
	configReg2 |= (TIMER_BIT_PWMMODEL_ENABLE|TIMER_BIT_COUNTER_ENABLE|BIT5|BIT4);
	pConfigReg2 = &ptTimerReg->CONFIG;
	
	//д�����üĴ���
	*pConfigReg1 = configReg1;
	*pConfigReg2 = configReg2;

	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_countEnable
* ����:	ʹ��Timer�ļ���
* ����:	timerID	-- timer ���ΪTIMER0~TIMER5
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/	
U32 hyhwTimer_countEnable(U32 timerID)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//�����Ϸ��Լ��
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptTimerReg 			 = (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->CONFIG	|= TIMER_BIT_COUNTER_ENABLE;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_countDisable
* ����:	disable Timer�ļ���
* ����:	timerID	-- timer ���ΪTIMER0~TIMER5
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_countDisable(U32 timerID)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//�����Ϸ��Լ��	
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptTimerReg			= (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->CONFIG &= ~TIMER_BIT_COUNTER_ENABLE;

	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_intEnable
* ����:	ʹ��Timer���ж�
* ����:	timerID	-- timer ���ΪTIMER0~TIMER5
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_intEnable(U32 timerID)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//�����Ϸ��Լ��
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}

	ptTimerReg			= (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->CONFIG |= TIMER_BIT_INT_ENABLE;

	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_intDisable
* ����:	disable Timer���ж�
* ����:	timerID	-- timer���Ϊ0~5
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_intDisable(U32 timerID)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//�����Ϸ��Լ��	
	if(timerID > TIMER_NUM)
	{
		return  HY_ERR_BAD_PARAMETER;
	}

	ptTimerReg			= (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->CONFIG &= ~TIMER_BIT_INT_ENABLE;


	return	HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_pwmEnable
* ����:	ʹ��Timer pwm model
* ����:	timerID	-- timer ���ΪTIMER0~TIMER3
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_pwmEnable(U32 timerID)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//�����Ϸ��Լ��	
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}

	ptTimerReg			= (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->CONFIG |= (TIMER_BIT_PWMMODEL_ENABLE|TIMER_BIT_COUNTER_ENABLE);

	return HY_OK;
}	

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_pwmDisable
* ����:	disable Timer model
* ����:	timerID	-- timer ���ΪTIMER0~TIMER3
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_pwmDisable(U32 timerID)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//�����Ϸ��Լ��
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}

	ptTimerReg			= (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->CONFIG &= ~(TIMER_BIT_PWMMODEL_ENABLE|TIMER_BIT_COUNTER_ENABLE);
							
	return HY_OK;
}					 						 
				 						 
/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_setReference
* ����:	����Timer�ļ���ֵ
* ����:	timerID	-- timer ���ΪTIMER0~TIMER5
*		load	-- Ҫ���ñȽ�����ƥ��ֵ
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_setReference(U32 timerID, U32 load)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//�����Ϸ��Լ��
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptTimerReg				= (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->REFERENCE	= load;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_setMidReference
* ����:	����Timer�ļ���ֵ
* ����:	timerID	-- timer ���ΪTIMER0~TIMER5
*		load	-- Ҫ���ñȽ�����ƥ��ֵ
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_setMidReference(U32 timerID, U32 load)
{
	tTIMER_REGISTER *ptTimerReg;
	
	//�����Ϸ��Լ��
	if(timerID > TIMER_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}

	ptTimerReg					= (tTIMER_REGISTER *)(TIMER_BASEADDR ) + timerID;
	ptTimerReg->MID_REFERENCE	= load;

	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwWatchdog_init
* ����:	initial watchdog  
* ����:	watchdog_time  ����Ϊ��λ������ܳ���357
*			max--357.9S  min--5.46mS
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwWatchdog_init(U32 watchdog_time)
{
	/*
		For example, 1 second Watchdog can be used with 183 WDTCNT value (12MHz clock).

		CountValue = WDTCNT * 2^16 = WDTCNT * 65536
		If you use 12MHz clock, 1 clock = 1/12000000 Second.

		1 sec. = 1/12000000 * WDTCNT * 65536
		WDTCNT = 1 sec. * 12000000 / 65536 = (about) 183
		
		max--357.9S  min--5.46mS
	BITCSET(HwWDTCTRL, 0xFFFF, usWDTCNT);
	BITSET(HwWDTCTRL, HwWDTCTRL_WS_1);
	BITSET(HwWDTCTRL, HwWDTCTRL_WE);
	 */
	U32 WDTCntReg;
	if (watchdog_time > 357) watchdog_time = 357;
	WDTCntReg = ((U32)watchdog_time * (1000 * EXTAL_CLK)) >> 16;
	if (WDTCntReg == 0) WDTCntReg = 1;//��֤һ����С���ã�12Mʱ��ʱ��С5.2ms
	CKC_WDTCTRL_REGISTER = WDTCntReg | WDOG_ENABLE;
	
	return 	HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwWatchdog_initMs
* ����:	initial watchdog  
* ����:	watchdog_timeMs  �Ժ���Ϊ��λ������ܳ���357913����
*			max--357.9S  min--5.46mS
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwWatchdog_initMs(U32 watchdog_timeMs)
{
	U32 WDTCntReg;
	if (watchdog_timeMs > 357913) watchdog_timeMs = 357913;
	WDTCntReg = ((U32)watchdog_timeMs * EXTAL_CLK) >> 16;
	if (WDTCntReg == 0) WDTCntReg = 1;//��֤һ����С���ã�12Mʱ��ʱ��С5.2ms
	CKC_WDTCTRL_REGISTER = WDTCntReg | WDOG_ENABLE;
	
	return 	HY_OK;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwWatchdog_enable
* ����:	enable watchdog
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_enable(void)
{
	CKC_WDTCTRL_REGISTER |= WDOG_ENABLE; 
}
/*-----------------------------------------------------------------------------
* ����:	hyhwWatchdog_disable
* ����:	disable watchdog
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_disable(void)
{
	CKC_WDTCTRL_REGISTER &= ~WDOG_ENABLE;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwWatchdog_intEnable
* ����:	enable watchdog interrupt
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_intEnable(void)
{
	//CKC_WDTCTRL_REGISTER |= WTCHDOG_BIT_IEN;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwWatchdog_intDisable
* ����:	disable watchdog interrupt
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_intDisable(void)
{
	//CKC_WDTCTRL_REGISTER &= ~WTCHDOG_BIT_IEN;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwWatchdog_feed
* ����:	feed watchdog
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_feed(void)
{
	CKC_WDTCTRL_REGISTER |= WDOG_CLEAR;	/*��������Ĵ���watchdog counter TIMER_WDCNT_REGISTER ������Ϊ0*/
	CKC_WDTCTRL_REGISTER &= ~WDOG_CLEAR;
}

/* end of file */