/* time.h: ANSI 'C' (X3J11 Oct 88) library header */

#ifndef __time_h
#define __time_h

#include "string.h"
#include "std.h"

//#ifndef __size_t
//#define size_t unsigned int
//#endif

#ifndef NULL
#define NULL (void *)0
#endif

//typedef unsigned int clock_t;    /* cpu time type */
//typedef unsigned int time_t;     /* date/time in unix secs past 1-Jan-70 */

/* ��������ʱ��Ľṹ����Ϊbroken-down time. ��� tm_isdstΪ������ʾDaylight Savings Time 
 * ��Ч��Ϊ���ʾDaylight Savings Time ��Ч, �������Ϣû�л�ȡ���� tm_isdstΪ�� */
struct tm {
  int tm_sec;   /* Seconds after minute (0 - 60) (1 leap second)
                   (0 - 60 allows for the occasional leap second) */
  int tm_min;   /* minutes after the hour (0 to 59) */
  int tm_hour;  /* Hours since midnight (0 - 23) */
  int tm_mday;  /* Day of month (1 - 31) */
  int tm_mon;   /* Month (0 - 11; January = 0) */
  int tm_year;  /* years since 1900 */
  int tm_wday;  /* Day of week (0 - 6; Sunday = 0) */
  int tm_yday;  /* Day of year (0 - 365; January 1 = 0) */
  int tm_isdst; /* DST   [-1/0/1] */
};


/*-----------------------------------------------------------------------
* ����: clock
* ˵��: ȷ��������ʹ�õ�ʱ��.
* ����: none
* ����: ���Ƴ�������������ʹ�õĴ�����ʱ��. ���ص�ʱ�����Ժ���Ϊ��λ
* ע��: ����ֻ֧�ֵ�10���룬�ʶ�����ķ���ֵ��Ϊ10�ı���
*-----------------------------------------------------------------------*/
clock_t clock(void);


/*-----------------------------------------------------------------------
* ����: difftime
* ˵��: ��������ʱ��Ĳ���: time1 - time0
* ����: time1: ʱ�䣬������
*		time0: ʱ�䣬����
* ����: ��double���ͱ�ʾ��ʱ������Ϊ��λ
*-----------------------------------------------------------------------*/
double difftime(time_t time1, time_t time0);


/*-----------------------------------------------------------------------
* ����: mktime
* ˵��: �� tm ��ʽ��ʱ��ת��Ϊ time_t ��ʽ�Ĵ���
* 		�ṹ�е�tm_wday��tm_yday����������,
* ����: 
*		ptTime: ָ�� struct tm �Ľṹ��ָ��
* ����: time_t ���͵�ʱ�䡣��1970.1.1 00:00:00�� tm ʱ�������������
*-----------------------------------------------------------------------*/
time_t mktime(struct tm * ptTime);


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
time_t time_time(time_t *timer);


/*-----------------------------------------------------------------------
* ����: asctime
* ˵��: ��tm�ṹ��ʱ��ת��Ϊ�ַ������͵�ʱ�䡣
* 		���ַ�����ʽΪ "Sun Sep 16 01:03:52 1973\n\0"��Ϊ24Сʱ�ơ�
* ����: 
*		ptTime: ָ�� struct tm �Ľṹ��ָ�� 
* ����: ָ��ת�����ִ���ָ��
*		�ִ���ʽ"Sun Sep 16 01:03:52 1973\n\0"
*-----------------------------------------------------------------------*/
char *asctime(const struct tm * tp);


/*-----------------------------------------------------------------------
* ����: _strdate
* ˵��: �õ�ϵͳ���ڲ�ת��Ϊ�ִ�������datestr�У������ظ��ִ���ָ��
* ����: 
*		datestr: Ŀ��buffer
* ����: Ŀ��bufferָ��
* ע��: datestr buffer��������Ϊ9
* ע�⣺�����ʽΪ "12/05/99"
*-----------------------------------------------------------------------*/
char *_strdate( char *datestr );


/*-----------------------------------------------------------------------
* ����: _strtime
* ˵��: �õ�ϵͳʱ�䲢ת��Ϊ�ִ�������datestr�У������ظ��ִ���ָ��
* ����: 
*		datestr: Ŀ��buffer
* ����: Ŀ��bufferָ��
* ע��: datestr buffer��������Ϊ9
* ע�⣺�����ʽΪ "13:40:40"
*-----------------------------------------------------------------------*/
char *_strtime( char *timestr );


/*-----------------------------------------------------------------------
* ����: ctime
* ˵��: ��time_t�͵�timeת��Ϊtm�ṹ�����ʽ�����ִ���ʽ���
* ����: 
*		ptTime: ��� time_t �����ݵ�ָ��
* ����: ָ��ת�����ִ���ָ��
*		�ִ���ʽ"Sun Sep 16 01:03:52 1973\n\0"
* ע�⣺���ʱ��Ϊ����1970.1.1 00:00:00,�򷵻�NULL��
*-----------------------------------------------------------------------*/
char *ctime(const time_t * timer);



/*-----------------------------------------------------------------------
* ����: gmtime
* ˵��: �� time_t ��ʽ��ʱ��ת��Ϊ�ṹ�� tm ���͵�ʱ�䡣
* ����: 
*		timer: ���Ҫת����time���ݵı�����ָ��
* ����: tm ���͵�ʱ��
* ע�⣺gmtime, mktime,��localtime ����һ���ṹ��"gm_use_tm"��װ�����ǵ�����
*-----------------------------------------------------------------------*/
struct tm *gmtime(const time_t * timer);


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
struct tm *localtime(const time_t * timer);


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
size_t strftime(char *s, size_t maxsize, const char *format, const struct tm *timeptr);


/* ע��: ���ļ��ڲ����ṩ _getsystime �� _setsystime ������ */
/*		 �����Ҫ����ֱ��ʹ�� hyhwRtc.h �е� hyhwRtc_GetTime hyhwRtc_SetTime �� */


#endif	//__time_h

/* end of time.h */








