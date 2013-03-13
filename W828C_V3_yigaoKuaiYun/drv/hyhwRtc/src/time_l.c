#include "time.h"

#define tm_isleap(year)	\
  ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))


static const unsigned short int tm_mon_yday[2][13] =
{
	/* Normal years.  */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* Leap years.  */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};


#define	SECS_PER_HOUR	(60 * 60)
#define	SECS_PER_DAY	(SECS_PER_HOUR * 24)

#define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))	/* y��ǰ���������������*/


/* Compute the `struct tm' representation of *T,
   offset OFFSET seconds east of UTC,
   and store year, yday, mon, mday, wday, hour, min, sec into *TP.
   Return nonzero if successful.  */
int tm_offtime (const time_t *t, long int offset, struct tm *tp)
{
	long int days, rem, y;
	const unsigned short int *ip;

	days = *t / SECS_PER_DAY;
	rem = *t % SECS_PER_DAY;
	
	/* ��offset. */
	rem += offset;
	
	/* ��������while��Ϊ+offset������� */
	while (rem < 0)
    {
		rem += SECS_PER_DAY;
		--days;
    }
	while (rem >= SECS_PER_DAY)
    {
		rem -= SECS_PER_DAY;
		++days;
    }
    
    /* ����hour */
	tp->tm_hour = rem / SECS_PER_HOUR;
	rem %= SECS_PER_HOUR;
	
	/* ����min��sec */
	tp->tm_min = rem / 60;
	tp->tm_sec = rem % 60;
	
	/* January 1, 1970 was a Thursday.  */
	tp->tm_wday = (4 + days) % 7;
	if (tp->tm_wday < 0)
		tp->tm_wday += 7;
	y = 1970;


	while (days < 0 || days >= (tm_isleap (y) ? 366 : 365))
    {
		/* Guess a corrected year, assuming 365 days per year.  */
		long int yg = y + days / 365 - (days % 365 < 0);

		/* Adjust DAYS and Y to match the guessed year.  */
		days -= ((yg - y) * 365
	       + LEAPS_THRU_END_OF (yg - 1)
	       - LEAPS_THRU_END_OF (y - 1));
		y = yg;
    }
	tp->tm_year = y - 1900;
	if (tp->tm_year != y - 1900)
    {
		/* The year cannot be represented due to overflow.  */
		return 0;
    }
	tp->tm_yday = days;
	
	ip = tm_mon_yday[tm_isleap(y)];
	for (y = 11; days < (long int) ip[y]; --y)
		continue;
	days -= ip[y];
	
	tp->tm_mon = y;
	
	tp->tm_mday = days + 1;
	
	return 1;
}

/*-----------------------------------------------------------------------
* ����: tm_zt_convert
* ˵��: ��time_t��ʽ��ʱ��ת��Ϊtm��ʽ��ʱ�䡣
* ����: 
*		timer: ʱ��ָ��
*		use_localtime: �Ƿ�Ϊ����ʱ��������Ŀǰ��֧��
*		tp: tm��ʽʱ���ָ��
* ����: �õ�ת�����ʱ��
* ˵��: �˺�����������WIN32-VC��ͬ��
*-----------------------------------------------------------------------*/
struct tm *
tm_tz_convert (const time_t *timer, int use_localtime, struct tm *tp)
{

	if (timer == NULL)
	{
		return NULL;
	}

	/* ����ʱ��Ϊ��8�� */
	if ( tm_offtime(timer, 8*3600, tp) == 0)
	{
		tp->tm_isdst = -1;
		tp = NULL;
	}
	else
	{
		tp->tm_isdst = 0;
	}

	return tp;
}

/*-----------------------------------------------------------------------
* ����: tm_zt_convert
* ˵��: ��tm��ʽ��ʱ��ת��Ϊtime_t��ʽ��ʱ�䡣
* ����: 
*		tp: ʱ��ָ�롣
*		use_localtime: �Ƿ�Ϊ����ʱ��������Ŀǰ��֧��
* ����: �õ�ת�����ʱ��
* ˵��: �˺�����������WIN32-VC��ͬ��
*-----------------------------------------------------------------------*/
time_t tm_zt_convert (struct tm *tp, int use_localtime)
{
	time_t res;
	int days, y;
	const unsigned short int *ip;
	
	if (tp == NULL || tp->tm_isdst == -1)
	{
		return 0;
	}
	y = 1970;
	tp->tm_year += 1900;
	
	/* guess day */
	days = ((tp->tm_year - y) * 365
		+ LEAPS_THRU_END_OF (tp->tm_year - 1)
		- LEAPS_THRU_END_OF (y - 1));
	       
	/* adjust days	//���Ǵ�7.1��ʼ�㣬���Ǵ�1.1��ʼ�㡣����1970.1.1 00:00:00��ʼ�㡣*/
	//ip = tm_mon_yday[tm_isleap(y)];
	//days -= ip[6];
	
	/* adjust days */
	ip = tm_mon_yday[tm_isleap(tp->tm_year)];
	days += ip[tp->tm_mon];
	
	/* adjust days */
	days += tp->tm_mday - 1;
	
	res = days * SECS_PER_DAY;
	
	res += (tp->tm_hour - 8) * SECS_PER_HOUR;	/* ����ʱ��Ϊ��8�� */
	res += tp->tm_min * 60 + tp->tm_sec;

	return res;
}

