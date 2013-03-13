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

#ifndef _HY_HWRTC_H_
#define _HY_HWRTC_H_

#ifdef __cplusplus
extern "C"
{
#endif


/**
* \defgroup tmhwRtc
* @brief  The timer driver allows user configure rtc timer and alarm interrupt and register interrupt 
*		  handlers for them.
*
* @{
*/

/**
 * @file   
 * @brief hereunder is the sequency to set timer and register timer handler
 *  - call tmhwTimerInit to initialise Timer registers
 *	- call tmhwRtcSetTime to set real time clock timer. It will generate interrupt every second
 *	- call tmhwRtcSetAlarm to set alarm. When alarm register value matchs time counters value, an 
 *	  interrupt is generated.
 */

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
//#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

/*************************************************************************
 * INCLUDES
 *************************************************************************/
/*************************************************************************
 * DEFINES
 *************************************************************************/
#define		RTC_CONTROL_REGISTER		TCC7901_RTC_RTCCON_REGISTER			/* RTC Control Register */
	#define RTC_BIT_WAKE_UP_EN			BIT7
	#define RTC_BIT_ALARM_INT_EN		BIT6
	#define RTC_BIT_OSC_EN				BIT5
	#define RTC_BIT_CLK_SET				BIT4
	#define RTC_BIT_WRITE_EN			BIT1
	#define RTC_BIT_HALT_EN				BIT0
	
#define		INT_CONTROL_REGISTER		TCC7901_RTC_INTCON_REGISTER			/* RTC Interrupt Control Register */
	#define INT_BIT_ENABLE				BIT0
	
#define		ALARM_CONTROL_REGISTER		TCC7901_RTC_RTCALM_REGISTER			/* RTC Alarm Control Register */
	#define ALARM_BIT_GLOBAL_EN			BIT7
	#define ALARM_BIT_YEAR_EN			BIT6
	#define ALARM_BIT_MONTH_EN			BIT5
	#define ALARM_BIT_WEEK_EN			BIT4
	#define ALARM_BIT_DATE_EN			BIT3
	#define ALARM_BIT_HOUR_EN			BIT2
	#define ALARM_BIT_MINUTE_EN			BIT1
	#define ALARM_BIT_SECOND_EN			BIT0
	
	#define ALARM_ALL_EN				(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6|BIT7)
	
#define		ALARM_SECOND_REGISTER		TCC7901_RTC_ALMSEC_REGISTER			/* Alarm Second Data Register */
#define		ALARM_MINUTE_REGISTER		TCC7901_RTC_ALMMIN_REGISTER			/* Alarm Minute Data Register */
#define		ALARM_HOUR_REGISTER			TCC7901_RTC_ALMHOUR_REGISTER		/* Alarm Hour Data Register */
#define		ALARM_DATE_REGISTER			TCC7901_RTC_ALMDATE_REGISTER		/* Alarm Date Data Register */
#define		ALARM_WEEK_REGISTER			TCC7901_RTC_ALMDAY_REGISTER			/* Alarm Day of Week Data Register */
#define		ALARM_MONTH_REGISTER		TCC7901_RTC_ALMMON_REGISTER			/* Alarm Month Data Register */
#define		ALARM_YEAR_REGISTER			TCC7901_RTC_ALMYEAR_REGISTER		/* Alarm Year Data Register */
#define		BCD_SECOND_REGISTER			TCC7901_RTC_BCDSEC_REGISTER			/* BCD Second Register */
#define		BCD_MINUTE_REGISTER			TCC7901_RTC_BCDMIN_REGISTER			/* BCD Minute Register1 */
#define		BCD_HOUR_REGISTER			TCC7901_RTC_BCDHOUR_REGISTER		/* BCD Hour Register */
#define		BCD_DATE_REGISTER			TCC7901_RTC_BCDDATE_REGISTER		/* BCD Date Register */
#define		BCD_WEEK_REGISTER			TCC7901_RTC_BCDDAY_REGISTER			/* BCD Day of Week Register */
#define		BCD_MONTH_REGISTER			TCC7901_RTC_BCDMON_REGISTER			/* BCD Month Register */
#define		BCD_YEAR_REGISTER			TCC7901_RTC_BCDYEAR_REGISTER		/* BCD Year Register */
#define		RTC_INT_MODE_REGISTER		TCC7901_RTC_RTCIM_REGISTER			/* RTC Interrupt Mode Register */
	#define INT_BIT_NORMAL				BIT3
	#define INT_BIT_PMWKUP_HIGH			BIT2
	#define INT_BIT_ALARM_DIS			~(BIT0|BIT1)
	#define INT_BIT_ALARM_EN			BIT1
	#define INT_BIT_MODE_EDGE			BIT1
	#define INT_BIT_MODE_LEVEL			(BIT0|BIT1)

#define		RTC_INT_PEND_REGISTER		TCC7901_RTC_RTCPEND_REGISTER		/* RTC Interrupt Pending Register */
	#define INTPEND_BIT_PENDING			BIT0



#define CALENDAR_START_YEAR				1950 
#define CALENDAR_END_YEAR				2085 
#define	CALENDAR_START_YEAR_FIRST_DATE	0    								// 1950��Ԫ��һ����������

#define CALENDAR_TOTAL_YEARS			136
// 1950 �굽2085��

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
                           

/************************************************************************
*	RTC Controller
************************************************************************/
typedef volatile struct _tRtcTime_st
{
	U8	second;		// (0 ~ 59)
	U8	minute;		// (0 ~ 59)
	U8	hour;		// (0 ~ 23)
	U8	week;		// Day of Week (SUN=0, MON, TUE, WED, THR, FRI, SAT=6)

	U8	day;		// (1 ~ 28,29,30,31)
	U8	month;		// (1 ~ 12)
	U16	year;
} RtcTime_st,*pRtcTime_st;


//����������
typedef struct RtcLunarDate_st
{
	UInt8  day;
	UInt8  month;
	//UInt16 chinese_era;//��ɵ�֧(��8λ:��ɣ���8λ:��֧)
	UInt8  chinese_eraT;//���
	UInt8  chinese_eraD;//��֧
}RtcLunarDate_st, *pRtcLunarDate_st;

//******************************************************************************************
//
//		Definition
//
//******************************************************************************************
#define COMMON_YEAR			0	/* ƽ�� */
#define LEAP_YEAR			1	/* ���� */


//******************************************************************************************
//		ENUM
//******************************************************************************************
typedef enum eNumOfDays
{
	RTC_DATE_SUNDAY = 0,
	RTC_DATE_MONDAY,
	RTC_DATE_TUESDAY,
	RTC_DATE_WEDNESDAY,
	RTC_DATE_THURSDAY,
	RTC_DATE_FRIDAY,
	RTC_DATE_SATURDAY
} eRTC_DATE_DAYS;	

typedef enum eNumOfMonths
{
	RTC_DATE_JANUARY = 1,
	RTC_DATE_FEBRUARY,
	RTC_DATE_MARCH,
	RTC_DATE_APRIL,
	RTC_DATE_MAY,
	RTC_DATE_JUNE,
	RTC_DATE_JULY,
	RTC_DATE_AUGUST,
	RTC_DATE_SEPTEMBER,
	RTC_DATE_OCTOBER,
	RTC_DATE_NOVEMBER,
	RTC_DATE_DECEMBER
} eRTC_DATE_MONTHS;

/*----------------------------------------------------------------------------
* ����:	hyhwRtc_getBcdTime
* ����:	
* ����:	 
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwRtc_getBcdTime(RtcTime_st *ptTime);

/*----------------------------------------------------------------------------
* ����:	hyhwRtc_IsValidTime
* ����:	
* ����:	 
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwRtc_IsValidTime(RtcTime_st *ptTimeTest);

/*----------------------------------------------------------------------------
* ����:	hyhwRtc_SetTime
* ����:	set timer
* ����:	 
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwRtc_SetTime(RtcTime_st *ptTime);

/*----------------------------------------------------------------------------
* ����:	hyhwRtc_GetTime
* ����:	get timer
* ����:	 
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwRtc_GetTime(RtcTime_st *ptTime);

/*----------------------------------------------------------------------------
* ����:	hyhwRtc_setBCDTime
* ����:	set BCD Timer
* ����:	
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwRtc_setBCDTime(RtcTime_st *ptTime);

/*----------------------------------------------------------------------------
* ����:	hyhwRtc_SetAlarm
* ����:	set alarm
* ����:	 
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwRtc_SetAlarm(RtcTime_st *ptTime);
 
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_setBcdAlarm
* ����:	set BCD Timer
* ����:	
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwRtc_setBcdAlarm(RtcTime_st *ptTime, U32 uCon);

/*----------------------------------------------------------------------------
* ����:	hyhwRtc_GetAlarm
* ����:	
* ����:	 
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwRtc_GetAlarm(RtcTime_st *ptTime);

/*----------------------------------------------------------------------------
* ����:	hyhwRtc_getBcdAlarm
* ����:	
* ����:	 
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwRtc_getBcdAlarm(RtcTime_st *ptTime);

/*----------------------------------------------------------------------------
* ����: hyhwRTC_readIntPending
* ����: read interrupt
* ����: none
* ����: 0----û�в���Alarm�ź�
*		1----��Alarm�źţ���ʱʱ�䵽
*----------------------------------------------------------------------------*/
U32 hyhwRTC_readIntPending(void);

/*----------------------------------------------------------------------------
* ����:	hyhwRTC_clearIntPending
* ����:	initial rtc
* ����:	 
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwRTC_clearIntPending(void);

/*----------------------------------------------------------------------------
* ����:	hyhwRTC_setWakeupOutput
* ����:	initial rtc
* ����:	 
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwRTC_setWakeupOutput(void);

/*----------------------------------------------------------------------------
* ����:	hyhwRtc_Init
* ����:	initial rtc
* ����:	 
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwRtc_Init(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwRtc_AlarmIntEnable
* ����:	ʹ�����ӹ��ܣ�ʹ�������ж�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_AlarmIntEnable(void );

/*-----------------------------------------------------------------------------
* ����:	hyhwRtc_AlarmIntDisable
* ����:	��ֹ�����ж�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_AlarmIntDisable(void );

/*-----------------------------------------------------------------------------
* ����:	hyhwRtc_EnableAlarm
* ����:	ʹ��RTCʱ�ӵ�����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_EnableAlarm( void );

/*-----------------------------------------------------------------------------
* ����:	hyhwRtc_DisableAlarm
* ����:	��ֹRTCʱ�ӵ�����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_DisableAlarm( void );

/*----------------------------------------------------------------------------
* ����:	hyhwRtc_Bcd2Dec
* ����:	��BCD�뻻��ʮ������ת
* ����:	nBCD	-- �����BCD��
* ����:	ʮ������
*----------------------------------------------------------------------------*/
U32  hyhwRtc_Bcd2Dec( U32 nBCD );

/*----------------------------------------------------------------------------
* ����:	hyhwRtc_Dec2Bcd
* ����:	��ʮ������ת����BCD��
* ����:	uDEC	-- �������
* ����:	BCD��
*----------------------------------------------------------------------------*/
U32 hyhwRtc_Dec2Bcd( U32 uDEC );

/*----------------------------------------------------------------------------
* ����:	hyhwRtc_getWeekDayOfToday
* ����:	����������ݡ��·ݡ��յõ�������
* ����:	nThisYear				-- ��������
*		nMonth					-- ������·�
*		nDay					-- ���������
* ����:	�����������
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwRtc_getWeekDayOfToday( U32 nThisYear, U32 nMonth, U32 nDay );

/*----------------------------------------------------------------------------
* ����:	hyhwRtc_getTotalDaysUntilTodayInThisYear
* ����:	����������ݵ�ĿǰΪֹ�ܵ�����
* ����:	nThisYear				-- ��������
*		nMonth					-- ������·�
* ����:	���������굽ĿǰΪֹ�ܵ�����
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwRtc_getTotalDaysUntilTodayInThisYear( U32 nThisYear, U32 nMonth, U32 nDay );

/*----------------------------------------------------------------------------
* ����:	hyhwRtc_getTotalDaysInMonth
* ����:	���������·ݵ�����
* ����:	nThisYear				-- ��������
*		nMonth					-- ������·�
* ����:	��������·ݵ�����
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwRtc_getTotalDaysInMonth( U32 nThisYear, U32 nMonth );

/*----------------------------------------------------------------------------
* ����:	hyhwRtc_getLeapYear
* ����:	�ж��Ƿ�������
* ����:	nThisYear		-- ��������
* ����:	LEAP_YEAR		-- ����
*		COMMON_YEAR		-- ƽ��
*----------------------------------------------------------------------------*/
U32 hyhwRtc_getLeapYear( U32 nThisYear );

/*-----------------------------------------------------------------------------
* ����:	hyhwRtc_ConvertToSeconds
* ����:	����ǰ��ʱ�䣬�������������ڣ�ʱ���룬 ת��Ϊ����
* ����:	pTime	-- ���룬 ʱ��ṹ��ָ��
* ����:	ת���õ�����
*----------------------------------------------------------------------------*/
U32 hyhwRtc_ConvertToSeconds( RtcTime_st *pTime );

/*-----------------------------------------------------------------------------
* ����:	hyhwRtc_GetCurrentMonthDays
* ����:	���ָ���������µ�������
* ����:	solarYear ----������
*		solarMonth ---������
* ����:	ָ�����µ�������
*----------------------------------------------------------------------------*/
U32 hyhwRtc_GetCurrentMonthDays(U32 solarYear, U32 solarMonth);

#ifdef __cplusplus
}
#endif

#endif //TM_HWRTC_H_