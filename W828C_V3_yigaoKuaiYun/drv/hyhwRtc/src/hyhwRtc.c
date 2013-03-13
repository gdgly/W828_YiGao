

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"

/*------------------------------------------------------------------------------
Project include files:
------------------------------------------------------------------------------*/
#include "hyhwRtc.h"
#include "hyhwCkc.h"

/*************************************************************************
 * DEFINES
 *************************************************************************/

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/

/*************************************************************************
 * EXTERNAL
 *************************************************************************/

/*************************************************************************
 * STATICS
 *************************************************************************/
 //==============================================================
//
//		Internal Variable
//
//==============================================================
const U8 MonthDays[13] =
{
	0,		// Reserved
	31,		// January
	28,		// February	[ Ignore leap Month ]
	31,		// March
	30,		// April
	31,		// May
	30,		// June
	31,		// July
	31,		// August
	30,		// September
	31,		// October
	30,		// November
	31		// December
};

static U8  su8TimeSetLock ; /* �����дʱ��ͬʱ�����Ĵ��� */


/*----------------------------------------------------------------------------
* ����:	hyhwRtc_IsValidTime
* ����:	
* ����:	 
* ����:	IsNeedMending == 0	-- ValidTime
*		IsNeedMending == 1	-- NeedMending
*----------------------------------------------------------------------------*/

U32 hyhwRtc_IsValidTime(RtcTime_st *ptTimeTest)
{
	
	if(NULL == ptTimeTest) return 1 ;
	
	hyhwRtc_getBcdTime(ptTimeTest);

	/* Conversion to Dec and Check validity */

	/* Second */
	ptTimeTest->second	= hyhwRtc_Bcd2Dec( ptTimeTest->second );
	if (ptTimeTest->second > 59)
	{
		return 1;
	}
	/* Minute */
	ptTimeTest->minute 	= hyhwRtc_Bcd2Dec( ptTimeTest->minute );
	if ( ptTimeTest->minute > 59 )
	{
		return 1;
	}
	/* Hour */
	ptTimeTest->hour 	= hyhwRtc_Bcd2Dec( ptTimeTest->hour );
	if ( ptTimeTest->hour > 23 )
	{
		return 1;
	}
	
	/* Date */
	ptTimeTest->day 	= hyhwRtc_Bcd2Dec( ptTimeTest->day );
	if ( ptTimeTest->day > 31 )
	{
		return 1;
	}
	
	/* Day */
	ptTimeTest->week 	= hyhwRtc_Bcd2Dec( ptTimeTest->week );
	if ( ptTimeTest->week > 6 )
	{	    
	//	return 1;
	}
	
	/* Month */

	ptTimeTest->month 	= hyhwRtc_Bcd2Dec( ptTimeTest->month );
	if ( ( ptTimeTest->month < 1 ) || ( ptTimeTest->month > 12 ) )
	{
		return 1;
	}
	
	/* Year */
	ptTimeTest->year 	= hyhwRtc_Bcd2Dec( ptTimeTest->year );
	if ( ( ptTimeTest->year < 1950 ) || ( ptTimeTest->year > 2080) )
	{
		return 1;
	}
	
	return	0;
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_SetTime
* ����:	set timer
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_SetTime(RtcTime_st *ptTime)
{
	RtcTime_st	sBCDTime;

	/* Second */
	if ( ptTime->second > 59 )
	{
		sBCDTime.second	= 0;
	}	
	else
	{
		sBCDTime.second	= hyhwRtc_Dec2Bcd( ptTime->second );
	}
	
	/* Minute */
	if ( ptTime->minute > 59 )
	{
		sBCDTime.minute	= 0;
	}
	else
	{
		sBCDTime.minute	= hyhwRtc_Dec2Bcd( ptTime->minute );
	}
	
	/* Hour */
	if ( ptTime->hour   > 23 )
	{
		sBCDTime.hour	= 0;
	}		
	else
	{
		sBCDTime.hour	= hyhwRtc_Dec2Bcd( ptTime->hour );
	}
	
	/* date */
	if ( ptTime->day > 31 || ptTime->day < 1 )
	{
		sBCDTime.day	= 1;
	}
	else
	{
		sBCDTime.day	= hyhwRtc_Dec2Bcd( ptTime->day );
	}
	
	/* year */
	if ( ptTime->year > 2080 || ptTime->year < 1950 )
	{
		sBCDTime.year	= 0x1950;
	}
	else
	{
		sBCDTime.year	= hyhwRtc_Dec2Bcd( ptTime->year );
	}
	
	/* month */
	if ( ptTime->month > 12 || ptTime->month < 1 )
	{
		sBCDTime.month	= 1;
	}
	else
	{
		sBCDTime.month	= hyhwRtc_Dec2Bcd( ptTime->month );
	}
	
	/* weekdays */
	sBCDTime.week		= hyhwRtc_getWeekDayOfToday( ptTime->year, ptTime->month, ptTime->day );

	wlock(&su8TimeSetLock) ;
	hyhwRtc_setBCDTime(&sBCDTime);
	unlock(&su8TimeSetLock);
	
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_setBCDTime
* ����:	set BCD Timer
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_setBCDTime(RtcTime_st *ptTime)
{
	hyhwCpm_busClkEnable(CKC_RTC);
	
	RTC_CONTROL_REGISTER |= (RTC_BIT_CLK_SET|RTC_BIT_HALT_EN);
	RTC_CONTROL_REGISTER |= RTC_BIT_WRITE_EN; 
	INT_CONTROL_REGISTER |= INT_BIT_ENABLE;
	RTC_CONTROL_REGISTER &= ~RTC_BIT_CLK_SET;

	BCD_SECOND_REGISTER	  = ptTime->second;
	BCD_MINUTE_REGISTER	  = ptTime->minute;
	BCD_HOUR_REGISTER	  = ptTime->hour;
	BCD_DATE_REGISTER	  = ptTime->day;
	BCD_WEEK_REGISTER	  = ptTime->week;
	BCD_MONTH_REGISTER	  = ptTime->month;
	BCD_YEAR_REGISTER	  = ptTime->year;
	
	RTC_CONTROL_REGISTER &= ~RTC_BIT_HALT_EN;
	INT_CONTROL_REGISTER &= ~INT_BIT_ENABLE;
	RTC_CONTROL_REGISTER &= ~RTC_BIT_WRITE_EN; 
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_GetTime
* ����:	get rtc timer
* ����:	 
* ����:	 0	-- �ɹ�
* 		 1	-- ʧ��
*----------------------------------------------------------------------------*/
U32 hyhwRtc_GetTime(RtcTime_st *ptTime)
{
	wlock(&su8TimeSetLock) ;
	hyhwRtc_getBcdTime(ptTime);
	unlock(&su8TimeSetLock);
	
	/* Conversion to Dec and Check validity */
	
	/* Second */
	ptTime->second		= hyhwRtc_Bcd2Dec( ptTime->second );
	if (ptTime->second > 59)
	{
		ptTime->second	= 0;
		return 1;
	}

	/* Minute */
	ptTime->minute = hyhwRtc_Bcd2Dec( ptTime->minute );
	if ( ptTime->minute > 59 )
	{
		ptTime->minute	= 0;
		return 1;
	}

	/* Hour */
	ptTime->hour = hyhwRtc_Bcd2Dec( ptTime->hour );
	if ( ptTime->hour > 23 )
	{
		ptTime->hour	= 0;
		return 1;
	}

	/* Date */
	ptTime->day = hyhwRtc_Bcd2Dec( ptTime->day );
	if ( ptTime->day > 31 )
	{
		ptTime->day		= 1;
		return 1;
	}

	/* Day */
	ptTime->week = hyhwRtc_Bcd2Dec( ptTime->week );
	if ( ( ptTime->week > 6 ) )
	{
		ptTime->week	= 0;
		return 1;
	}

	/* Month */
	ptTime->month	= hyhwRtc_Bcd2Dec( ptTime->month );
	if ( ( ptTime->month < 1 ) || ( ptTime->month > 12 ) )
	{
		ptTime->month	= 1;
		return 1;
	}

	/* Year */
	ptTime->year	= hyhwRtc_Bcd2Dec( ptTime->year );
	if ( ( ptTime->year < 1950 ) || ( ptTime->year > 2080 ) )
	{
		ptTime->year	= 1950;
		return 1;
	}

	return 0;
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_getBcdTime
* ����:	get rtc bcd time
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_getBcdTime(RtcTime_st *ptTime)
{
	hyhwCpm_busClkEnable(CKC_RTC);
	
	RTC_CONTROL_REGISTER |= RTC_BIT_WRITE_EN; 
	INT_CONTROL_REGISTER |= INT_BIT_ENABLE;
	
	ptTime->second		  = BCD_SECOND_REGISTER;
	ptTime->minute		  = BCD_MINUTE_REGISTER;
	ptTime->hour		  = BCD_HOUR_REGISTER;
	ptTime->week		  = BCD_WEEK_REGISTER;
	ptTime->day			  = BCD_DATE_REGISTER;
	ptTime->month		  = BCD_MONTH_REGISTER;
	ptTime->year		  = BCD_YEAR_REGISTER;

	INT_CONTROL_REGISTER &= ~INT_BIT_ENABLE;
	RTC_CONTROL_REGISTER &= ~RTC_BIT_WRITE_EN; 
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_SetAlarm
* ����:	set alarm time 
* ����:	 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_SetAlarm(RtcTime_st *ptTime)
{
	U32 uCon;
	RtcTime_st	sTime;

	uCon	= ALARM_ALL_EN;

	/* Second */
	if ( ptTime->second > 59 )
	{
		uCon		&= ~ALARM_BIT_SECOND_EN;
	}	
	else
	{
		sTime.second = hyhwRtc_Dec2Bcd( ptTime->second );
	}
	
	/* Minute */
	if ( ptTime->minute > 59 )
	{
		uCon 	    &= ~ALARM_BIT_MINUTE_EN;
	}	
	else
	{
		sTime.minute = hyhwRtc_Dec2Bcd( ptTime->minute );
	}
	
	/* Hour */
	if ( ptTime->hour > 23 )
	{
		uCon 	   	&= ~ALARM_BIT_HOUR_EN;
	}	
	else
	{
		sTime.hour	= hyhwRtc_Dec2Bcd( ptTime->hour );
	}
	
	/* date */
	if ( ptTime->day > 31 || ptTime->day < 1 )
	{
		uCon 	   &= ~ALARM_BIT_DATE_EN;
	}
	else
	{
		sTime.day	= hyhwRtc_Dec2Bcd( ptTime->day );
	}
	
	/* year */
	if ( ptTime->year > 2080 || ptTime->year < 1950 )
	{
		uCon 	   &= ~ALARM_BIT_YEAR_EN;
	}
	else
	{
		sTime.year	= hyhwRtc_Dec2Bcd( ptTime->year );
	}
	
	/* month */
	if ( ptTime->month > 12 || ptTime->month < 1 )
	{
		uCon 	   &= ~ALARM_BIT_MONTH_EN;
	}	
	else
	{
		sTime.month	= hyhwRtc_Dec2Bcd( ptTime->month );
	}
		
	/* weekdays */
	if ( ptTime->week > 6)
	{
		uCon 	   &= ~ALARM_BIT_WEEK_EN;
	}	
	else	
	{
	sTime.week	= hyhwRtc_getWeekDayOfToday( ptTime->year, ptTime->month, ptTime->day );
	}

	hyhwRtc_setBcdAlarm(&sTime,uCon);
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_setBcdAlarm
* ����:	set alarm BCD Time
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_setBcdAlarm(RtcTime_st *ptTime, U32 uCon)
{
	hyhwCpm_busClkEnable(CKC_RTC);

	RTC_CONTROL_REGISTER   |= RTC_BIT_WRITE_EN|RTC_BIT_ALARM_INT_EN; 
	INT_CONTROL_REGISTER   |= INT_BIT_ENABLE;
	
	RTC_INT_MODE_REGISTER  &= ~(BIT0|BIT1|BIT2|BIT3); 
	//RTC_INT_MODE_REGISTER  |= (BIT0|BIT1|BIT2);       //20100608 ��δ������
	RTC_INT_MODE_REGISTER  |= (BIT0|BIT1);
	ALARM_CONTROL_REGISTER  = uCon;							/* note */

	ALARM_SECOND_REGISTER	= ptTime->second;
	ALARM_MINUTE_REGISTER	= ptTime->minute;
	ALARM_HOUR_REGISTER		= ptTime->hour;
	ALARM_DATE_REGISTER		= ptTime->day;
	ALARM_WEEK_REGISTER		= ptTime->week;
	ALARM_MONTH_REGISTER	= ptTime->month;
	ALARM_YEAR_REGISTER		= ptTime->year;
	
	RTC_INT_PEND_REGISTER  &= ~INTPEND_BIT_PENDING;
	
	INT_CONTROL_REGISTER   &= ~INT_BIT_ENABLE;
	RTC_CONTROL_REGISTER   &= ~RTC_BIT_WRITE_EN; 
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_GetAlarm
* ����:	get alarm time
* ����:	 
* ����:	alarm time
*----------------------------------------------------------------------------*/
U32 hyhwRtc_GetAlarm(RtcTime_st *ptTime)
{
	U32	uCon;

	hyhwCpm_busClkEnable(CKC_RTC);

	uCon	= hyhwRtc_getBcdAlarm(ptTime);

	/* Second */
	if ((uCon & ALARM_BIT_SECOND_EN) == 0)
	{
		ptTime->second	= (U8) -1;
	}	
	else
	{
		ptTime->second	= hyhwRtc_Bcd2Dec( ptTime->second );
	}

	/* Minute */
	if ((uCon & ALARM_BIT_MINUTE_EN) == 0)
	{
		ptTime->minute	= (U8) -1;
	}	
	else
	{
		ptTime->minute	= hyhwRtc_Bcd2Dec( ptTime->minute );
	}	

	/* Hour */
	if ((uCon & ALARM_BIT_HOUR_EN) == 0)
	{
		ptTime->hour	= (U8) -1;
	}	
	else
	{
		ptTime->hour	= hyhwRtc_Bcd2Dec( ptTime->hour );
	}	

	/* date */
	if ((uCon & ALARM_BIT_DATE_EN) == 0)
	{
		ptTime->day		= (U8) -1;
	}	
	else
	{
		ptTime->day		= hyhwRtc_Bcd2Dec( ptTime->day );
	}	

	/* month */
	if ((uCon & ALARM_BIT_MONTH_EN) == 0)
	{
		ptTime->month	= (U8) -1;
	}	
	else
	{
		ptTime->month	= hyhwRtc_Bcd2Dec( ptTime->month );
	}

	/* year */
	if ((uCon  &ALARM_BIT_YEAR_EN) == 0)
	{
		ptTime->year	= (U16) -1;
	}	
	else
	{
		ptTime->year	= hyhwRtc_Bcd2Dec( ptTime->year );
	}

	/* weekdays */
	if ((uCon & ALARM_BIT_WEEK_EN) == 0)
	{
		ptTime->week	= 1;
	}
	else
	{
		ptTime->week 	= hyhwRtc_Bcd2Dec( ptTime->week );
	}

	return	uCon;
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_getBcdAlarm
* ����:	get alarm bcd time
* ����:	 
* ����:alarm bcd time
*----------------------------------------------------------------------------*/
U32 hyhwRtc_getBcdAlarm(RtcTime_st *ptTime)
{
	U32 rtcAlarm;
	
	hyhwCpm_busClkEnable(CKC_RTC);
	
	RTC_CONTROL_REGISTER |= RTC_BIT_WRITE_EN; 
	INT_CONTROL_REGISTER |= INT_BIT_ENABLE;
	
	ptTime->second		  = ALARM_SECOND_REGISTER;
	ptTime->minute		  = ALARM_MINUTE_REGISTER;
	ptTime->hour		  = ALARM_HOUR_REGISTER;
	ptTime->day			  = ALARM_DATE_REGISTER;
	ptTime->week		  = ALARM_WEEK_REGISTER;
	ptTime->month		  = ALARM_MONTH_REGISTER;
	ptTime->year          = ALARM_YEAR_REGISTER;

	rtcAlarm 			  = ALARM_CONTROL_REGISTER;
	
	INT_CONTROL_REGISTER &= ~INT_BIT_ENABLE;
	RTC_CONTROL_REGISTER &= ~RTC_BIT_WRITE_EN; 
	
	return rtcAlarm;

}

/*----------------------------------------------------------------------------
* ����: hyhwRTC_readIntPending
* ����: read interrupt
* ����: none
* ����: 0----û�в���Alarm�ź�
*		1----��Alarm�źţ���ʱʱ�䵽
*----------------------------------------------------------------------------*/
U32 hyhwRTC_readIntPending(void)
{
	RTC_CONTROL_REGISTER  |= RTC_BIT_WRITE_EN; 
	INT_CONTROL_REGISTER  |= INT_BIT_ENABLE;

	if(RTC_INT_PEND_REGISTER &INTPEND_BIT_PENDING)
	{
		return 1;
	}
	INT_CONTROL_REGISTER  &= ~INT_BIT_ENABLE;
	RTC_CONTROL_REGISTER  &= ~RTC_BIT_WRITE_EN; 

	return 0;
}
/*----------------------------------------------------------------------------
* ����:	hyhwRTC_clearIntPending
* ����:	initial rtc
* ����:	 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRTC_clearIntPending(void)
{
	RTC_CONTROL_REGISTER  |= RTC_BIT_WRITE_EN; 
	INT_CONTROL_REGISTER  |= INT_BIT_ENABLE;
	
	RTC_INT_PEND_REGISTER &= ~INTPEND_BIT_PENDING;
	
	INT_CONTROL_REGISTER  &= ~INT_BIT_ENABLE;
	RTC_CONTROL_REGISTER  &= ~RTC_BIT_WRITE_EN; 
}
/*----------------------------------------------------------------------------
* ����:	hyhwRTC_setWakeupOutput
* ����:	initial rtc
* ����:	 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRTC_setWakeupOutput(void)
{
	RTC_CONTROL_REGISTER  |= RTC_BIT_WRITE_EN; 
	INT_CONTROL_REGISTER  |= INT_BIT_ENABLE;
	
	/* set in normal operation mode 
	   and PMWKUP active high */
	RTC_INT_MODE_REGISTER |= INT_BIT_PMWKUP_HIGH|INT_BIT_NORMAL;
	
	INT_CONTROL_REGISTER  &= ~INT_BIT_ENABLE;
	RTC_CONTROL_REGISTER  &= ~RTC_BIT_WRITE_EN; 
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_AlarmIntEnable
* ����:	alarm int enable 
* ����:	 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_AlarmIntEnable(void)
{
	RTC_CONTROL_REGISTER   |= RTC_BIT_ALARM_INT_EN;
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_AlarmIntDisable
* ����:	alarm int disable 
* ����:	 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_AlarmIntDisable(void)
{
	RTC_CONTROL_REGISTER   &= ~RTC_BIT_ALARM_INT_EN; 
}
/*-----------------------------------------------------------------------------
* ����:	hyhwRtc_EnableAlarm
* ����:	ʹ��RTCʱ�ӵ�����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_EnableAlarm(void)
{
	RTC_CONTROL_REGISTER   |= RTC_BIT_WRITE_EN; 
	INT_CONTROL_REGISTER   |= INT_BIT_ENABLE;
	
	ALARM_CONTROL_REGISTER |= ALARM_ALL_EN;
	
	INT_CONTROL_REGISTER   &= ~INT_BIT_ENABLE;
	RTC_CONTROL_REGISTER   &= ~RTC_BIT_WRITE_EN; 		
}
/*-----------------------------------------------------------------------------
* ����:	hyhwRtc_DisableAlarm
* ����:	��ֹRTCʱ�ӵ�����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_DisableAlarm(void)
{
	RTC_CONTROL_REGISTER   |= RTC_BIT_WRITE_EN; 
	INT_CONTROL_REGISTER   |= INT_BIT_ENABLE;
	
	ALARM_CONTROL_REGISTER &= ~ALARM_ALL_EN;
	
	INT_CONTROL_REGISTER   &= ~INT_BIT_ENABLE;
	RTC_CONTROL_REGISTER   &= ~RTC_BIT_WRITE_EN; 		
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_Init
* ����:	initial rtc
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwRtc_Init(void)
{
	RtcTime_st sDateTime;
		
	if(hyhwRtc_IsValidTime(&sDateTime)!=0)
	{
		hyhwCpm_busClkEnable(CKC_RTC);

		sDateTime.second 	= 0;
		sDateTime.minute	= 0;
		sDateTime.hour		= 0;
		sDateTime.day		= 1;
		sDateTime.month		= 1;
		sDateTime.year 		= 2012;
		hyhwRtc_SetTime( &sDateTime );
		
		sDateTime.year 		= 1980;
		hyhwRtc_SetAlarm( &sDateTime );
		hyhwRtc_DisableAlarm();
		hyhwRTC_clearIntPending();
		hyhwRtc_AlarmIntDisable();
	}
	else
	{
		hyhwCpm_busClkEnable(CKC_RTC);
		
		hyhwRtc_SetTime( &sDateTime );
		
		sDateTime.year 		= 1980;
		hyhwRtc_SetAlarm( &sDateTime );
		hyhwRtc_DisableAlarm();
		hyhwRTC_clearIntPending();
		hyhwRtc_AlarmIntDisable();
	}
	
	su8TimeSetLock = 0 ;
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_Bcd2Dec
* ����:	��BCD�뻻��ʮ������ת
* ����:	nBCD	-- �����BCD��
* ����:	ʮ������
*----------------------------------------------------------------------------*/
U32  hyhwRtc_Bcd2Dec( U32 nBCD )
{
	return	(
				((nBCD & 0xF000) >> 12) * 1000 +
				((nBCD & 0xF00)  >> 8) * 100   +
				((nBCD & 0xF0) 	 >> 4) * 10    +
				(nBCD & 0x0F)
			);
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_Dec2Bcd
* ����:	��ʮ������ת����BCD��
* ����:	uDEC	-- �������
* ����:	BCD��
*----------------------------------------------------------------------------*/
U32 hyhwRtc_Dec2Bcd(U32 uDEC)
{
	U32	uBCD;

	uBCD	 = (uDEC / 1000) << 12;
	uBCD	|= (((uDEC %= 1000) / 100) << 8);
	uBCD	|= (((uDEC %= 100) / 10) << 4);
	uBCD	|= (((uDEC %= 10) / 1) << 0);

	return uBCD;
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_getWeekDayOfToday
* ����:	����������ݡ��·ݡ��յõ�������
* ����:	nThisYear				-- ��������
*		nMonth					-- ������·�
*		nDay					-- ���������
* ����:	�����������
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwRtc_getWeekDayOfToday(U32 nThisYear, U32 nMonth, U32 nDay)
{
	U32 dwTotalDays;

	/* ������������Ƿ���ȷ */
	if ( nThisYear < 1 )
	{
		return HY_ERR_BAD_PARAMETER;
	}

	/* 1year 1month 1day -> Monday */
	dwTotalDays =	( nThisYear-1 )*365 +
					( nThisYear-1 )/4 -
					( nThisYear-1 )/100 +
					( nThisYear-1 )/400;

	dwTotalDays += hyhwRtc_getTotalDaysUntilTodayInThisYear( nThisYear, nMonth, nDay );

	return ( dwTotalDays % 7 );
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_getTotalDaysUntilTodayInThisYear
* ����:	����������ݵ�ĿǰΪֹ�ܵ�����
* ����:	nThisYear				-- ��������
*		nMonth					-- ������·�
* ����:	���������굽ĿǰΪֹ�ܵ�����
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwRtc_getTotalDaysUntilTodayInThisYear(U32 nThisYear, U32 nMonth, U32 nDay)
{
	U32	i;
	U32 dwTotalDays;
	
	/* ���������·��Ƿ���ȷ */
	if ( nMonth < 1 || nMonth > 12 )
	{
		return HY_ERR_BAD_PARAMETER;
	}

	if ( nDay > hyhwRtc_getTotalDaysInMonth( nThisYear, nMonth ) )
	{
		return HY_ERR_BAD_PARAMETER;
	}

	dwTotalDays = nDay;

	for ( i = 1; i < nMonth; ++i )
	{
		dwTotalDays += hyhwRtc_getTotalDaysInMonth( nThisYear, i );
	}

	return 	dwTotalDays;
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_getTotalDaysInMonth
* ����:	���������·ݵ�����
* ����:	nThisYear				-- ��������
*		nMonth					-- ������·�
* ����:	��������·ݵ�����
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwRtc_getTotalDaysInMonth(U32 nThisYear, U32 nMonth)
{

	/* ���������·��Ƿ���ȷ */
	if ( nMonth < 1 || nMonth > 12 )
	{
		return HY_ERR_BAD_PARAMETER;
	}

	if ( nMonth == RTC_DATE_FEBRUARY )
	{
		/* �жϵ����Ƿ������꣬�������ô2�·ݽ���29�� */
		if ( hyhwRtc_getLeapYear( nThisYear ) == LEAP_YEAR )
		{
			return MonthDays[nMonth] + 1;
		}
		else
		{	
			return MonthDays[nMonth];
		}
	}
	
	return MonthDays[nMonth];
}
/*----------------------------------------------------------------------------
* ����:	hyhwRtc_getLeapYear
* ����:	�ж��Ƿ�������
* ����:	nThisYear		-- ��������
* ����:	LEAP_YEAR		-- ����
*		COMMON_YEAR		-- ƽ��
*----------------------------------------------------------------------------*/
U32 hyhwRtc_getLeapYear(U32 nThisYear)
{
	if ( ( ( nThisYear % 4 ) == 0 && ( nThisYear % 100 ) != 0 ) || ( nThisYear % 400 == 0 ) )
	{
		return LEAP_YEAR;
	}

	return COMMON_YEAR;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwRtc_ConvertToSeconds
* ����:	����ǰ��ʱ�䣬�������������ڣ�ʱ���룬 ת��Ϊ����
* ����:	pTime	-- ���룬 ʱ��ṹ��ָ��
* ����:	ת���õ�����
*----------------------------------------------------------------------------*/
U32 hyhwRtc_ConvertToSeconds(RtcTime_st *pTime)
{
	U32 i;
	U32 totalVal;
	U32 totalDays;
	U32 DaysOfYear;
	U32 curMonthDays;
	U32 Total;
	U32 thisyear;
	
	totalDays = pTime->day;
	thisyear = pTime->year;
	for( i = CALENDAR_START_YEAR; i< thisyear; i++)
	{		
		
		DaysOfYear	 = ((i%4)==0)? 366: 365;		
		totalDays	+=DaysOfYear;
	}
	
	Total			 = pTime->month;
	for(i = 1;i <Total;i++)
	{
		curMonthDays = hyhwRtc_GetCurrentMonthDays(thisyear, i);
		totalDays 	+= curMonthDays;
	}	
	
	totalVal		 = ((totalDays*24 + pTime->hour)*60+ pTime->minute)* 60 + pTime->second;
	
	return totalVal;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwRtc_GetCurrentMonthDays
* ����:	���ָ���������µ�������
* ����:	solarYear ----������
*		solarMonth ---������
* ����:	ָ�����µ�������
*----------------------------------------------------------------------------*/
U32 hyhwRtc_GetCurrentMonthDays(U32 solarYear, U32 solarMonth)
{
	U32 curMonthDays;
	
	switch(solarMonth)
	{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
				curMonthDays	= 31;
				break;
		case 2:
				curMonthDays	= ((solarYear%4)== 0)? 29:28;				
				break;
		case 4:
		case 6:
		case 9:
		case 11:
				curMonthDays	= 30;
				break;
		default:
			break;
	}
	
	return curMonthDays;
}
