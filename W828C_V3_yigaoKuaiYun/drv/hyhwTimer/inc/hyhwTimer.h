/*******************************************************************************
*  (C) Copyright 2005 Shanghai Hyctron Electronic Design House, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of Shanghai Hyctron Electronic Design House and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  Hyctron and is confidential in nature.  Its use and dissemination by    
*  any party other than Hyctron is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/

#ifndef HY_HW_TIMER_H_
#define HY_HW_TIMER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define	TC32_BASE_FREQ	12000000

/*����Ĵ����жϴ������Ҫ�� �������ⶨ�� */
#define 	TIMER_INT_REQUEST_REGISTER		TCC7901_TIMER_TIREQ_REGISTER 		/* T/C Interrupt Request Register */
	#define TIMER_BIT_INT0					BIT0
	#define TIMER_BIT_INT1					BIT1
	#define TIMER_BIT_INT2					BIT2
	#define TIMER_BIT_INT3					BIT3
	#define TIMER_BIT_INT4					BIT4
	#define TIMER_BIT_INT5					BIT5
	#define WATCHDOG_BIT_INT				BIT6

	#define TIMER_BIT_FLAG0					BIT8
	#define TIMER_BIT_FLAG1					BIT9
	#define TIMER_BIT_FLAG2					BIT10
	#define TIMER_BIT_FLAG3					BIT11
	#define TIMER_BIT_FLAG4					BIT12
	#define TIMER_BIT_FLAG5					BIT13
	#define WATCHDOG_BIT_FLAG				BIT14
		
#define		TIMER_BASEADDR					TCC7901_TIMER_BASEADDR
		
//��5��ͨ�õ�Timer
#define TIMER_NUM		5
#define PWM_NUM			3	

#define TIMER0			0
#define TIMER1			1
#define TIMER2			2
#define TIMER3			3
#define TIMER4			4
#define TIMER5			5
#define WATCHhDOG		6	
	
/*	Timer �Ĵ����Ķ��壬ÿ��timerռ��4���Ĵ���  */
typedef volatile struct	_tTIMER_REGISTER
{
	U32 CONFIG;		  		/* Timer/Counter  Configuration Register		*/
	U32 COUNTER;			/* Timer/Counter  Counter Register				*/
	U32 REFERENCE;			/* Timer/Counter  Reference Register			*/
	U32 MID_REFERENCE;		/* Timer/Counter  Middle Reference Register		*/

}tTIMER_REGISTER;

/*---------------------------------------------------------------------------------------
* ��  :	hyhwmTimer_checkIntFlag
* ����:	check Timer �жϱ�־λ
* ����:	��
*---------------------------------------------------------------------------------------*/		
#define hyhwmTimer0_checkIntFlag()			(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_INT0)
#define hyhwmTimer1_checkIntFlag()			(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_INT1)
#define hyhwmTimer2_checkIntFlag()			(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_INT2)
#define hyhwmTimer3_checkIntFlag()			(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_INT3)
#define hyhwmTimer4_checkIntFlag()			(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_INT4)
#define hyhwmTimer5_checkIntFlag()			(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_INT5)
#define hyhwmWatchDog_checkIntFlag()		(TIMER_INT_REQUEST_REGISTER & WATCHDOG_BIT_INT)

/*----------------------------------------------------------------------------------------
* ��  :	hyhwmTimer_clearIntFlag
* ����:	clear Timer �жϱ�־λ
* ����:	��
*----------------------------------------------------------------------------------------*/		
#define hyhwmTimer0_clearIntFlag()			TIMER_INT_REQUEST_REGISTER  |=(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5) 
#define hyhwmWatchDog_clearIntFlag()		(TIMER_INT_REQUEST_REGISTER |= WATCHDOG_BIT_INT)

/*------------------------------------------------------------------------------------------
* ��  :	hyhwmTimer_checkCountFlag
* ����:	check Timer count flag
* ����:	��
*------------------------------------------------------------------------------------------*/		
#define hyhwmTimer0_checkCountFlag()		(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_FLAG0)
#define hyhwmTimer1_checkCountFlag()		(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_FLAG1)
#define hyhwmTimer2_checkCountFlag()		(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_FLAG2)
#define hyhwmTimer3_checkCountFlag()		(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_FLAG3)
#define hyhwmTimer4_checkCountFlag()		(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_FLAG4)
#define hyhwmTimer5_checkCountFlag()		(TIMER_INT_REQUEST_REGISTER & TIMER_BIT_FLAG5)
#define hyhwmWatchDog_checkCountFlag()		(TIMER_INT_REQUEST_REGISTER & WATCHDOG_BIT_FLAG)

	#define TIMER_BIT_COUNTER_ENABLE		BIT0
	#define TIMER_BIT_CONTINUE_ENABLE		BIT1
	#define TIMER_BIT_PWMMODEL_ENABLE		BIT2
	#define TIMER_BIT_INT_ENABLE	 	 	BIT3
	//#define TIMER_BIT_INT_ENABLE	 	 	BIT3
	#define TIMER_BIT_POLARITY_ENABLE	 	BIT7
	
	
	#define TIMER_BIT_COUNT_CLEAR_ENABLE	BIT8
	#define TIMER_BIT_COUNT_STOP_ENABLE	 	BIT9   								/* If TCNTn is equal to the TREFn, the TCNTn counter stop to increment */

				
#define 	WATCHDOG_CONFIG_REGISTER 		TCC7901_TIMER_TWDCFG_REGISTER 		/* Watchdog Timer Configuration Register 	*/
#define 	WATCHDOG_CLEAR_REGISTER 		TCC7901_TIMER_TWDCLR_REGISTER 		/* Watchdog Timer Clear Register 			*/

#define 	TIMER32_ENABLE_REGISTER 		TCC7901_TIMER_TC32EN_REGISTER 		/* 32-bit Counter Enable / Pre-scale Value  */
	#define RELOAD_CMP1						BIT29
	#define RELOAD_CMP0						BIT28
	#define TC32EN_ENABLE_ON				BIT24
	
#define 	TIMER32_LOAD_REGISTER 			TCC7901_TIMER_TC32LDV_REGISTER 		/* 32-bit Counter Load Value				*/
#define 	TIMER32_MATCH0_REGISTER			TCC7901_TIMER_TC32CMP0_REGISTER 	/* 32-bit Counter Match Value 0 			*/
#define 	TIMER32_MATCH1_REGISTER 		TCC7901_TIMER_TC32CMP1_REGISTER 	/* 32-bit Counter Match Value 1 			*/
#define 	TIMER32_PRESCALE_VALUE_REGISTER	TCC7901_TIMER_TC32PCNT_REGISTER 	/* 32-bit Counter Current Value (pre-scale counter) */
#define 	TIMER32_MAIN_VALUE_REGISTER 	TCC7901_TIMER_TC32MCNT_REGISTER 	/* 32-bit Counter Current Value (main counter) 		*/
#define 	TIMER32_INTCTRL_REGISTER 		TCC7901_TIMER_TC32IRQ_REGISTER 		/* 32-bit Counter Interrupt Control 				*/
	#define	TC32IRQ_RSYNC_DIS				BIT30								// Disable Synchronization control
	#define	TC32IRQ_RSYNC_EN				~BIT30								// Enable Synchronization control
	#define TC32IRQ_SELECT_MASK				(BIT20|BIT19|BIT18|BIT17|BIT16)
	#define TC32IRQ_EN4						BIT20								// Enable Interrupt at the rising edge of a counter bit selected by BITSEL.
	#define TC32IRQ_EN3						BIT19								// Enable Interrupt at the end of pre-scale count
	#define TC32IRQ_EN2						BIT18								// Enable Interrupt at the end of count
	#define TC32IRQ_EN1						BIT17								// Enable Interrupt when the counter value matched with CMP1
	#define TC32IRQ_EN0						BIT16								// Enable Interrupt when the counter value matched with CMP0


#define hyhwmTimer32_clearInt()				(TIMER32_INTCTRL_REGISTER &= ~0x00001f00)

/*-----------------------------------------------------------------------------
* �����Ǻ�����������
*------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_getIntStat
* ����:	get the timer32 int status
* ����:	none
* ����:	irq status
*----------------------------------------------------------------------------*/
U32 hyhwTimer32_getIntStat(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_disableTimer
* ����:	stop timer 32
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_disableTimer(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_enableTimer
* ����:	enable timer
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_enableTimer(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_enableIrq
* ����:	enable the timer 32 irq
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_enableIrq(U32 irqSelect);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_disableIrq
* ����:	disable the timer 32 irq
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_disableIrq(U32 irqSelect);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_setMatch0
* ����:	set match 0 value
* ����:	matchValue
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setMatch0(U32 matchValue);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_setMatch1
* ����:	set match 1 value
* ����:	matchValue
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setMatch1(U32 matchValue);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_setLoadValue
* ����:	set timer 32 load value
* ����:	LoadValue 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setLoadValue(U32 LoadValue);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_setPrescaleValue
* ����:	set prescale value
* ����:	preScaleValue
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setPrescaleValue(U32 preScaleValue);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer32_setFrequency
* ����:	����TIMER32���ж�Ƶ�ʣ�TIMER32�������Ƚ�����ʱ��ԴΪ12��
* ����:	cmp1Freq��cmp2FreqҪС��preScaleFreq��������Ҫ����������
		exp:hyhwTimer32_setFrequency(6, 2, 3);
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwTimer32_setFrequency(U32 preScaleFreq, U32 cmp1Freq, U32 cmp0Freq);
	
/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_SetFrequency
* ����:	����Timer���жϵ�Ƶ�ʡ�Timer0 ��Timer5 ���������������
*		���ʹ����control/status�Ĵ������ж�ʹ�ܣ��ͻ��Զ�����
*		����timer0 �̶�ѡ�õ���APBʱ�ӵ�1/4��Ϊʱ�ӣ����ԣ�
*		���ÿ�θı���APB����Ҫ���µ�������������Ա㱣֤timer0ÿ���жϵ�ʱ���
*		��һ��
* ����:	timerUnitID	-- timer ��ţ�ϵͳ��������ʱ�ӣ����Ϊ0~5
*		uwFrequency	-- ����ʱ���жϵ�Ƶ��
* ����:	HY_OK		-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_SetFrequency(U8 timerID, U32 time);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_SetFreeFrequency
* ����:	����Timer���жϵ�Ƶ��
* ����:	timerUnitID	-- timer ��ţ�ϵͳ��������ʱ�ӣ����Ϊ0~5
*		frequency	-- ����ʱ���жϵ�Ƶ��, ��float�ͣ����mpeg4��֡��/��
* ����:	HY_OK		-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_SetFreeFrequency (U32 timerUnitID, Float frequency);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_setPwm
* ����:	��ʼ��PWM���ֵļĴ�����static����
* ����:	channel		-- ͨ��index 
*		Pwm_period	-- ����Pwm outputƵ��
*		duty		-- Pwm output full duty (high level)
*		PWM����ķ�ΧΪ��50HZ -- 10KHZ  ռ�ձȿɵ���Ϊ1/100
*		������PWM����ķ�Χ�����Ϊ low level
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_setPwm(U8 channel,U32 pwm_period, U16 duty);		

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_countEnable
* ����:	ʹ��Timer�ļ���
* ����:	timerID	-- timer ���ΪTIMER0~TIMER5
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_countEnable(U32 timerID);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_countDisable
* ����:	disable Timer�ļ���
* ����:	timerID	-- timer ���ΪTIMER0~TIMER5
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_countDisable(U32 timerID);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_intEnable
* ����:	ʹ��Timer���ж�
* ����:	timerID	-- timer ���ΪTIMER0~TIMER5
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_intEnable(U32 timerID);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_IntEnable
* ����:	disable Timer���ж�
* ����:	timerID	-- timer ���ΪTIMER0~TIMER5
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_intDisable(U32 timerID);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_pwmEnable
* ����:	ʹ��Timer pwm model
* ����:	timerID	-- timer ���ΪTIMER0~TIMER5
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_pwmEnable(U32 timerID);
	
/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_IntEnable
* ����:	disable Timer model
* ����:	timerID	-- timer ���ΪTIMER0~TIMER5
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_pwmDisable(U32 timerID);
					 						 
/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_setReference
* ����:	����Timer�ļ���ֵ
* ����:	timerID	-- timer ���ΪTIMER0~TIMER5
*		load	-- Ҫ���ñȽ�����ƥ��ֵ
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_setReference(U32 timerID, U32 load);

/*-----------------------------------------------------------------------------
* ����:	hyhwTimer_setMidReference
* ����:	����Timer�ļ���ֵ
* ����:	timerID	-- timer ���ΪTIMER0~TIMER5
*		load	-- Ҫ���ñȽ�����ƥ��ֵ
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwTimer_setMidReference(U32 timerID, U32 load);

/*-----------------------------------------------------------------------------
* ����:	hyhwWatchdog_init
* ����:	initial watchdog
* ����:	watchdog_time  ����Ϊ��λ������ܳ���357
*			max--357.9S  min--5.46mS
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwWatchdog_init(U32 watchdog_time);

/*-----------------------------------------------------------------------------
* ����:	hyhwWatchdog_initMs
* ����:	initial watchdog
* ����:	watchdog_timeMs  �Ժ���Ϊ��λ������ܳ���357913����
*			max--357.9S  min--5.46mS
* ����:	HY_OK				-- �ɹ�
*		HY_ERR_BAD_PARAMETER-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwWatchdog_initMs(U32 watchdog_timeMs);

/*-----------------------------------------------------------------------------
* ����:	hyhwWatchdog_enable
* ����:	enable watchdog
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_enable(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwWatchdog_disable
* ����:	disable watchdog
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_disable(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwWatchdog_intEnable
* ����:	enable watchdog interrupt
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_intEnable(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwWatchdog_intDisable
* ����:	disable watchdog interrupt
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_intDisable(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwWatchdog_feed
* ����:	feed watchdog
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwWatchdog_feed(void);

#ifdef __cplusplus
}
#endif

#endif //TM_HWTIMER_H_