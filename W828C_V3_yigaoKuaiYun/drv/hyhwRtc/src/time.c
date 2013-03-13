#include "hyhwRtc.h"
#include "string.h" 
#include "time.h"

/* format as "Sun Sep 16 01:03:52 1973\n\0" */
/* static const char format[] = "%.3s %.3s %2d %.2d:%.2d:%.2d %d\n"; */
static char tm_asc_rst[	         3+1+ 3+1+ 2+1+ 2+1+ 2+1+ 2+1+4+1 + 1 + 6];	/* ���asctime�����*/

static struct tm gm_use_tm;	/* ����ȫ�ֽṹ�壬�����洢��ʱ��Ϣ��gmtime/localtime/mktime/tm_getSystemTime���á�*/

static const char tm_week_name[7][10] =
{
	"Sunday",	/* Sunday */
	"monday",	/* monday */
	"tuesday",	/* tuesday */
	"Wednesday",/* Wednesday */
	"Thursday",	/* Thursday */
	"Friday",	/* Friday */
	"Saturday",	/* Saturday */
};
static const char tm_month_name[12][10] =
{
	"January",	/* January */
	"February",	/* February */
	"March",	/* March */
	"April",	/* April */
	"May",		/* May */
	"June",		/* June */
	"July",		/* July */
	"August",	/* August */
	"September",/* September */
	"October",	/* October */
	"November",	/* November */
	"December",	/* December */
};


// ����
static int tm_writeSpcStr(char **dst, const char **fmt, int rstlen, int *rlen, const struct tm *timeptr);
extern struct tm *tm_tz_convert (const time_t *timer, int use_localtime, struct tm *tp);
extern time_t tm_zt_convert (struct tm *tp, int use_localtime);
static char *tm_itos_len(char *dst, int value, int len);

/*-----------------------------------------------------------------------
* ����: clock
* ˵��: ȷ��������ʹ�õ�ʱ��.
* ����: none
* ����: ���Ƴ�������������ʹ�õĴ�����ʱ��. ���ص�ʱ�����Ժ���Ϊ��λ
* ע��: ����ֻ֧�ֵ�10���룬�ʶ�����ķ���ֵ��Ϊ10�ı���
*-----------------------------------------------------------------------*/
clock_t clock(void)
{
	return rawtime(NULL)*10;
}

/*-----------------------------------------------------------------------
* ����: difftime
* ˵��: ��������ʱ��Ĳ���: time1 - time0
* ����: time1: ʱ�䣬������
*		time0: ʱ�䣬����
* ����: ��double���ͱ�ʾ��ʱ������Ϊ��λ
*-----------------------------------------------------------------------*/
double difftime(time_t time1, time_t time0)
{
	/* ����time_t�Ѿ���unsigned int�ͣ����ٿ��������жϡ� */
	
	return (double)time1 - (double)time0;
}

/*-----------------------------------------------------------------------
* ����: mktime
* ˵��: �� tm ��ʽ��ʱ��ת��Ϊ time_t ��ʽ�Ĵ���
* 		�ṹ�е�tm_wday��tm_yday����������,
* ����: 
*		ptTime: ָ�� struct tm �Ľṹ��ָ��
* ����: time_t ���͵�ʱ�䡣��1970.1.1 00:00:00�� tm ʱ�������������
*-----------------------------------------------------------------------*/
time_t mktime(struct tm * ptTime)
{
	return tm_zt_convert(ptTime, 0);
}

/*-----------------------------------------------------------------------
* ����: tm_getSystemTime
* ˵��: �õ�ϵͳԭʼʱ��
* ����: 
*		ptTime: ָ�� struct tm �Ľṹ��ָ��
* ����: struct tm* ָ��
* ע��: �������p_tmΪNULL����ú�����gmtime�ȹ��ýṹ�壻
*		����ʹ�ô������Ĳ����ṹ��
*-----------------------------------------------------------------------*/
static struct tm *tm_getSystemTime(struct tm *p_tm)
{
	RtcTime_st rtcTime;
	struct tm *timer = &gm_use_tm;
	
	if (p_tm)
	{
		timer = p_tm;
	}
	
	hyhwRtc_GetTime(&rtcTime);
	timer->tm_sec = rtcTime.second;
	timer->tm_min = rtcTime.minute;
	timer->tm_hour = rtcTime.hour;
	//tmTime.tm_sec = rtcTime.week;
	timer->tm_mday = rtcTime.day;
	timer->tm_mon = rtcTime.month - 1;	/* for [1-12] => [0-11] */
	timer->tm_year = rtcTime.year - 1900;
	
	return timer;
}

/*-----------------------------------------------------------------------
* ����: time_time
* ˵��: �õ�ϵͳʱ�䡣����1970��1��1��00:00:00�����ڵ�������
* ����: 
*		timer: ������ŵõ���ʱ��
* ����: �õ�ϵͳʱ�䡣��ʵ�Ͳ�������һ�����Ǹ�ֵ�����ַ�����ʽ��
* �÷���1). long time_s = 0; time_s = time( NULL ); time_sΪĿ��ֵ��
*		2). long time_s = 0; time(&time_s); time_sΪĿ��ֵ��
*		3). long time_s_p = 0, time_s_r = 0; time_s_r = time( &time_s_p ); 
*				time_s_r��time_s_p��ΪĿ��ֵ����ͬ
*-----------------------------------------------------------------------*/
time_t time_time(time_t *timer)
{
#if 0
	if (!timer)
		*timer = rawtime(NULL) / 100;
	return rawtime(NULL) / 100;
#else

	time_t time_n;
	struct tm tmTime;
	
	time_n = mktime( tm_getSystemTime(&tmTime) );
	
	if (timer)
	{
		*timer = time_n;
	}
	return time_n;
	
#endif
}

/*-----------------------------------------------------------------------
* ����: asctime
* ˵��: ��tm�ṹ��ʱ��ת��Ϊ�ַ������͵�ʱ�䡣
* 		���ַ�����ʽΪ "Sun Sep 16 01:03:52 1973\n\0"��Ϊ24Сʱ�ơ�
* ����: 
*		ptTime: ָ�� struct tm �Ľṹ��ָ�� 
* ����: ָ��ת�����ִ���ָ��
*		�ִ���ʽ"Sun Sep 16 01:03:52 1973\n\0"
*-----------------------------------------------------------------------*/
char *asctime(const struct tm * tp)
{
	char tmpStr[36];
	int pos = 0;
	
	
	if (!tp)
		return NULL;
	//if ( ptTime->isdst <= 0 )		//no judge it
	//	return NULL;
	
	/* �����tm�ṹ���ڵ��������Ϸ����ж� */
	if (tp->tm_wday < 0 || tp->tm_wday >= 7)	//week
	{
		strcpy(tm_asc_rst, "???");	/* same as unix. */
	}
	else
	{
		strncpy(tm_asc_rst, tm_week_name[tp->tm_wday], 3);
	}
	pos += 3;
	
	/* blank */
	tm_asc_rst[pos++] = ' ';
	
	if (tp->tm_mon < 0 || tp->tm_mon >= 12)		/* month */
	{
		strncpy(tm_asc_rst + pos, "???", 3);
	}
	else
	{
		strncpy(tm_asc_rst + pos, tm_month_name[tp->tm_mon], 3);
	}
	pos += 3;
	
	/* blank */
	tm_asc_rst[pos++] = ' ';
	tm_asc_rst[pos++] = 0;	/* for bellow using 'strcat' */
	
	/* day */
	strcat(tm_asc_rst, _itoa(tp->tm_mday, tmpStr, 10));
	
	strcat(tm_asc_rst, " ");	/* blank */
	
	//hour
	if (tp->tm_hour > 23)
	{
		strcat(tm_asc_rst, "??");
	}
	else
	{
		strcat(tm_asc_rst, tm_itos_len(tmpStr, tp->tm_hour, 2));
	}
	
	strcat(tm_asc_rst, ":");	/* ":" */
	
	/* minute */
	if (tp->tm_min > 59)
	{
		strcat(tm_asc_rst, "??");
	}
	else
	{
		strcat(tm_asc_rst, tm_itos_len(tmpStr, tp->tm_min, 2));
	}
	
	strcat(tm_asc_rst, ":");	/* ":" */
	
	/* second */
	if (tp->tm_sec > 59)
	{
		strcat(tm_asc_rst, "??");
	}
	else
	{
		strcat(tm_asc_rst, tm_itos_len(tmpStr, tp->tm_sec, 2));
	}
	
	strcat(tm_asc_rst, " ");	/* blank */
	
	/* year */
	if (tp->tm_year + 1900 > 2350 || tp->tm_year < 70)
	{
		strcat(tm_asc_rst, "????");
	}
	else
	{
		strcat(tm_asc_rst, _itoa(tp->tm_year + 1900, tmpStr, 10));
	}
	
	strcat(tm_asc_rst, "\n");
	
	return tm_asc_rst;
}


/*-----------------------------------------------------------------------
* ����: _strdate
* ˵��: �õ�ϵͳ���ڲ�ת��Ϊ�ִ�������datestr�У������ظ��ִ���ָ��
* ����: 
*		datestr: Ŀ��buffer
* ����: Ŀ��bufferָ��
* ע��: datestr buffer��������Ϊ9
* ע�⣺�����ʽΪ "12/05/99"
*-----------------------------------------------------------------------*/
char *_strdate( char *datestr )
{
	struct tm tmTime;
	char tmpStr2[4];
	
	tm_getSystemTime(&tmTime);
	strcpy(datestr, tm_itos_len(tmpStr2, tmTime.tm_mon + 1, 2));
	strcat(datestr, "/");
	strcat(datestr, tm_itos_len(tmpStr2, tmTime.tm_mday, 2));
	strcat(datestr, "/");
	strcat(datestr, tm_itos_len(tmpStr2, tmTime.tm_year % 100, 2));
	
	return datestr;
}

/*-----------------------------------------------------------------------
* ����: _strtime
* ˵��: �õ�ϵͳʱ�䲢ת��Ϊ�ִ�������datestr�У������ظ��ִ���ָ��
* ����: 
*		datestr: Ŀ��buffer
* ����: Ŀ��bufferָ��
* ע��: datestr buffer��������Ϊ9
* ע�⣺�����ʽΪ "13:40:40"
*-----------------------------------------------------------------------*/
char *_strtime( char *timestr )
{
	struct tm tmTime;
	char tmpStr2[4];
	
	tm_getSystemTime(&tmTime);
	strcpy(timestr, tm_itos_len(tmpStr2, tmTime.tm_hour, 2));
	strcat(timestr, ":");
	strcat(timestr, tm_itos_len(tmpStr2, tmTime.tm_min, 2));
	strcat(timestr, ":");
	strcat(timestr, tm_itos_len(tmpStr2, tmTime.tm_sec, 2));
	
	return timestr;
}

/*-----------------------------------------------------------------------
* ����: ctime
* ˵��: ��time_t�͵�timeת��Ϊtm�ṹ�����ʽ�����ִ���ʽ���
* ����: 
*		ptTime: ��� time_t �����ݵ�ָ��
* ����: ָ��ת�����ִ���ָ��
*		�ִ���ʽ"Sun Sep 16 01:03:52 1973\n\0"
* ע�⣺���ʱ��Ϊ����1970.1.1 00:00:00,�򷵻�NULL��
*-----------------------------------------------------------------------*/
char *ctime(const time_t * timer)
{
	struct tm * ptTime;
	
	ptTime = gmtime(timer);
	
	if (ptTime->tm_year < (1970 - 1900))
	{
		return NULL;
	}
	
	return asctime( ptTime );
}

/*-----------------------------------------------------------------------
* ����: gmtime
* ˵��: �� time_t ��ʽ��ʱ��ת��Ϊ�ṹ�� tm ���͵�ʱ�䡣
* ����: 
*		timer: ���Ҫת����time���ݵı�����ָ��
* ����: tm ���͵�ʱ��
* ע�⣺gmtime, mktime,��localtime ����һ���ṹ��"gm_use_tm"��װ�����ǵ�����
*-----------------------------------------------------------------------*/
struct tm *gmtime(const time_t * timer)
{
	return tm_tz_convert(timer, 0, &gm_use_tm);
}

/*-----------------------------------------------------------------------
* ����: localtime
* ˵��: �� time_t ��ʽ��ʱ��ת��Ϊ�ṹ�� tm ���͵�ʱ�䲢�����趨��ʱ����У����
* ����: 
*		timer: ���Ҫת����time���ݵı�����ָ��
* ����: tm ���͵�ʱ��
* ע��: localtime��gmtime�Ĳ�ͬ�����ڣ�localtime������У�Ե���ʱ���Ķ�����
*		���ù��������ݲ�֧�֡�
* ע�⣺gmtime, mktime,��localtime ����һ���ṹ��"gm_use_tm"��װ�����ǵ�����
*-----------------------------------------------------------------------*/
struct tm *localtime(const time_t * timer)
{
	return gmtime(timer);
}


/*-----------------------------------------------------------------------
* ����: strftime
* ˵��: ��ʱ�䰴�չ涨�ĸ�ʽ��ʽ��Ϊ�ִ�
* ����: 
*		s: Ŀ��buffer
*		maxsize: Ŀ��buffer����
*		format: ��ʽ����Ҫ��
*		timeptr: tm ָ��
* ����: �ִ��������ܳ��ȣ�������βNULL��һ���ֽڡ�
*		����ó��ȳ���buffer�ܳ��ȣ��򷵻�ֵΪ0����ʱbuffer�����á�
* ˵��: ����ʹ�÷�����ο�WIN32-VC�� 
*		Ŀǰ֧�ֵĸ�ʽ����ΪVC��֧�ֵ�ȫ������: %a,%A,%b,%B,%c,%d,%H,%I,%j
*		%m,%M,%p,%S,%U,%w,%W,%x,%X,%y,%Y,%z,%Z,%%,
*		%#a, %#A, %#b, %#B, %#p, %#X, %#z, %#Z, %#%, %#c, %#x, %#d, %#H, 
*		%#I, %#j, %#m, %#M, %#S, %#U, %#w, %#W, %#y, %#Y
*		�������弰ʹ����ο�VC��
*		��Ŀǰz/Z���������Ч�ִ�����Ϊ���ִ�Ϊ��ʾʱ���������ִ���
*-----------------------------------------------------------------------*/
size_t strftime(char *s, size_t maxsize, const char *format, const struct tm *timeptr)
{
	int len, overflow, rlen;
	char *dst;
	
	len = 0;
	dst = s;
	
	while ( *format )
	{
		if (*format != '%')	/* normal */
		{
			*dst++ = *format++;
			len++;
			if (len >= maxsize) overflow = 1;
		}
		else	/* special string */
		{
			overflow = tm_writeSpcStr(&dst, &format, maxsize-len, &rlen, timeptr);
			len + rlen;
		}
		
		if (overflow == 1)
			break;
	}
	
	if (overflow == 1)	/* memo-out */
	{
		return 0;
	}
	else
	{
		*dst = 0;	/* ��NULL��β */
		return len + 1;	/* ���ش�NULL�ĳ��� */
	}

}

/*-----------------------------------------------------------------------
* ����: tm_itos_len
* ˵��: ��int������ת��Ϊ�ִ��������len���ȣ��������С��len����ǰ�߲��ַ�'0'��
* ����: 
*		dst: ��������ŵĵ�ַ
*		value: Ҫת������
*		len: ��Ҫ�ĳ���
* ����: ��������ŵĵ�ַ
* ע�⣺ֻ����������ֻת��Ϊ10���Ƶ�ת��
*-----------------------------------------------------------------------*/
static char *tm_itos_len(char *dst, int value, int len)
{
	int res, i;
	
	dst[len] = 0;
	for (i = len - 1; i >= 0; i--)
	{
		res = value % 10;
		value /= 10;
		dst[i] = res + '0';
	}
	
	return dst;
}

/*-----------------------------------------------------------------------
* ����: tm_writeSpcStr
* ˵��: ���ת���ִ���
* ����: 
*		dst: ��������ŵĵط�
*		fmt: Ҫת����ִ�
*		rstlen: Ŀǰbuffer��ʣ��ĳ��ȣ������ж�bufferԽ��
*		rlen: ת������ʵʹ�õĳ���
*		timeptr: time�ṹ��
* ����: �Ƿ�Խ�硣1: Խ�磻0: û��Խ�硣
* ע�⣺�����ش��������ת���ִ���֧�֣�����Ը��ִ���
*-----------------------------------------------------------------------*/
static int tm_writeSpcStr(char **dst, const char **fmt, int rstlen, int *rlen, const struct tm *timeptr)
{
	int len, overflow = 0, meaning = 0, tmpData;
	char tmpStr[50], tmpStr2[6], *str;
	const char *sourFmt;
	
	sourFmt = *fmt;
	sourFmt++;	/* skip '%'. */
	
	if (*sourFmt == '#')
	{
		sourFmt++;
		if (*sourFmt == 'a' || *sourFmt == 'A' || *sourFmt == 'b' || *sourFmt == 'B' || 
			*sourFmt == 'p' || *sourFmt == 'X' || *sourFmt == 'z' || *sourFmt == 'Z' || *sourFmt == '%')
		{
			/* do nothing. ignore '#' */
		}
		else
		{
			meaning = 1;
		}
	}
	
	switch(*sourFmt)
	{
	case 'a':	/* Abbreviated weekday name */
		len = 3;
		str = (char *)tm_week_name[timeptr->tm_wday];
		break;
	case 'A':	/* Full weekday name */
		str = (char *)tm_week_name[timeptr->tm_wday];
		len = strlen(str);
		break;
	case 'b':	/* Abbreviated month name */
		len = 3;
		str = (char *)tm_month_name[timeptr->tm_mon];
		break;
	case 'B':	/* Full month name */
		str = (char *)tm_month_name[timeptr->tm_mon];
		len = strlen(str);
		break;
	case 'c':	/* Date and time representation appropriate for locale */
		if (meaning)
		{
			/* format: "Thursday, July 03, 2008 15:02:34" (month/date/year) */
			strcpy(tmpStr, tm_week_name[timeptr->tm_wday]);
			strcat(tmpStr, ", ");
			strcat(tmpStr, tm_month_name[timeptr->tm_mon]);
			strcat(tmpStr, " ");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_mday, 2));
			strcat(tmpStr, ", ");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_year + 1900, 4));
			strcat(tmpStr, " ");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_hour, 2));
			strcat(tmpStr, ":");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_min, 2));
			strcat(tmpStr, ":");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_sec, 2));
		}
		else
		{	/* format: "07/03/08 15:02:34" (month/date/year) */
			strcpy(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_mon + 1, 2));
			strcat(tmpStr, "/");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_mday, 2));
			strcat(tmpStr, "/");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_year % 100, 2));
			strcat(tmpStr, " ");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_hour, 2));
			strcat(tmpStr, ":");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_min, 2));
			strcat(tmpStr, ":");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_sec, 2));
		}
		str = tmpStr;
		len = strlen(str);
		break;
	case 'd':	/* Day of month as decimal number (01 - 31) */
		if (meaning)
		{
			str = _itoa(timeptr->tm_mday, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 2;
			str = tm_itos_len(tmpStr, timeptr->tm_mday, 2);
		}
 		break;
	case 'H':	/* Hour in 24-hour format (00 - 23) */
		if (meaning)
		{
			str = _itoa(timeptr->tm_hour, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 2;
			str = tm_itos_len(tmpStr, timeptr->tm_hour, 2);
		}
		break;
	case 'I':	/* Hour in 12-hour format (01 - 12) */
		tmpData = (timeptr->tm_hour>12)?(timeptr->tm_hour-12):(timeptr->tm_hour);
		if (meaning)
		{
			str = _itoa(tmpData, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 2;
			str = tm_itos_len(tmpStr, tmpData, 2);
		}
		break;
	case 'j':	/* Day of year as decimal number (001 - 366) */
		if (meaning)
		{
			str = _itoa(timeptr->tm_yday + 1, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 3;
			str = tm_itos_len(tmpStr, timeptr->tm_yday + 1, 3);
		}
		break;
	case 'm':	/* Month as decimal number (01 - 12) */
		if (meaning)
		{
			str = _itoa(timeptr->tm_mon + 1, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 2;
			str = tm_itos_len(tmpStr, timeptr->tm_mon + 1, 2);
		}
		break;
	case 'M':	/* Minute as decimal number (00 - 59) */
		if (meaning)
		{
			str = _itoa(timeptr->tm_min, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 2;
			str = tm_itos_len(tmpStr, timeptr->tm_min, 2);
		}
		break;
	case 'p':	/* Current locale��s A.M./P.M. indicator for 12-hour clock */
		len = 2;
		if (timeptr->tm_hour > 12) strcpy(tmpStr, "PM");
		else strcpy(tmpStr, "AM");
		str = tmpStr;
		break;
	case 'S':	/* Second as decimal number (00 - 59) */
		if (meaning)
		{
			str = _itoa(timeptr->tm_sec, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 2;
			str = tm_itos_len(tmpStr, timeptr->tm_sec, 2);
		}
		break;
	case 'U':	/* Week of year as decimal number, with Sunday as first day of week (00 - 53) */
		if (meaning)
		{
			if (timeptr->tm_yday % 7 < timeptr->tm_wday)
				str = _itoa(timeptr->tm_yday / 7, tmpStr, 10);
			else
				str = _itoa(timeptr->tm_yday / 7 + 1, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			if (timeptr->tm_yday % 7 < timeptr->tm_wday)
				str = tm_itos_len(tmpStr, timeptr->tm_yday / 7, 2);
			else
				str = tm_itos_len(tmpStr, timeptr->tm_yday / 7 + 1,2);
			len = strlen(str);
		}
		break;
	case 'w':	/* Weekday as decimal number (0 - 6; Sunday is 0) */
		/*  ignore 'meaning' value */
		len = 1;
		str = tm_itos_len(tmpStr, timeptr->tm_wday, 1);
		break;
	case 'W':	/* Week of year as decimal number, with Monday as first day of week (00 - 53) */
		if (meaning)
		{	/* for sunday belongs to pre-week */
			if (timeptr->tm_yday % 7 < ((timeptr->tm_wday == 0)?(timeptr->tm_wday + 7):(timeptr->tm_wday)))
				str = _itoa(timeptr->tm_yday / 7, tmpStr, 10);
			else
				str = _itoa(timeptr->tm_yday / 7 + 1, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			if (timeptr->tm_yday % 7 < ((timeptr->tm_wday == 0)?(timeptr->tm_wday + 7):(timeptr->tm_wday)))
				str = tm_itos_len(tmpStr, timeptr->tm_yday / 7, 2);
			else
				str = tm_itos_len(tmpStr, timeptr->tm_yday / 7 + 1, 2);
			len = strlen(str);
		}
		break;
	case 'x':	/* Date representation for current locale */
		if (meaning)
		{
			/* format: "Thursday, July 03, 2008" */
			strcpy(tmpStr, tm_week_name[timeptr->tm_wday]);
			strcat(tmpStr, ", ");
			strcat(tmpStr, tm_month_name[timeptr->tm_mon]);
			strcat(tmpStr, " ");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_mday, 2));
			strcat(tmpStr, ", ");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_year + 1900, 4));
		}
		else
		{
			/* format: "07/03/08" */
			strcpy(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_mon + 1, 2));
			strcat(tmpStr, "/");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_mday, 2));
			strcat(tmpStr, "/");
			strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_year % 100, 2));
		}
		str = tmpStr;
		len = strlen(str);
		break;
	case 'X':	/* Time representation for current locale */
				/* format: "15:02:34" */
		strcpy(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_hour, 2));
		strcat(tmpStr, ":");
		strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_min, 2));
		strcat(tmpStr, ":");
		strcat(tmpStr, tm_itos_len(tmpStr2, timeptr->tm_sec, 2));
		str = tmpStr;
		len = strlen(str);
		break;
	case 'y':	/* Year without century, as decimal number (00 - 99) */
		if (meaning)
		{
			str = _itoa(timeptr->tm_year % 100, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 2;
			str = tm_itos_len(tmpStr, timeptr->tm_year % 100, 2);
		}
		break;
	case 'Y':	/* Year with century, as decimal number */
		if (meaning)
		{
			str = _itoa(timeptr->tm_year+1900, tmpStr, 10);
			len = strlen(str);
		}
		else
		{
			len = 4;
			str = tm_itos_len(tmpStr, timeptr->tm_year+1900, 4);
		}
		break;
	case 'z':	/* Time-zone name or abbreviation; no characters if time zone is unknown */
	case 'Z':
				/* should put out "�й���׼ʱ��", we ignore now */
		len = 0;
		str = tmpStr;
		break;
	case '%':	/* Percent sign */
		len = 1;
		tmpStr[0] = '%';
		str = tmpStr;
		break;
	default:
		len = 0;
		str = tmpStr;
		break;
	}
	
	if (len >= rstlen)
	{
		overflow = 1;
	}
	else
	{
		strncpy(*dst, str, len);
		*dst += len;
		**dst = 0;	/* Note: not *dst++ */
	}
	
	sourFmt++;	/* skip the checked character */
	*fmt = sourFmt;
	*rlen = len;
	
	return overflow;
	
}


