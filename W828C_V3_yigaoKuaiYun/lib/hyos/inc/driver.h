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
/******************************************************************************
* �ļ�:	driver.h 
* ����:	ZhouJie 
* ����:	2005 12 05 15:52
* ˵��:	������
*		1���ļ�������Ľṹ����
*		2��
* ��أ�1��dvcfg.h�� 
*		2��ioctl.h	 
*		3��select.h
*		4. signal.h
* ƽ̨:	ARM 32λ��С�˸�ʽ
*****************************************************************************/

#ifndef _DRIVER_H_
#define _DRIVER_H_

#include "dvcfg.h"
//#include "ioctl.h"
#include "signal.h"

/*	FILE DESCRIPTOR TABLE �ļ�������
	����ÿ�����̵�ÿ�����ܴ򿪵��ļ�����������ṹ����
	file descriptor value is equal to relative position within array
	There is one of these per process per possible open file.*/
typedef struct fdtab
{
	DVCFG	*pdvcfg;		/*	pointer to configuration info�������豸�����豸��������������Ϣ
								Ҳ����˵��ʹ��ʲô�������ļ� */
	void	*pfile;			/*	driver dependent pointer, �������ݸ�����Ϣ
								�ڴ��ļ�ʱ��ָ���ļ���ȫ·���ַ��� 
								���ļ�������ָʾ��ǰ�Ĵ��ļ��Ľṹ(OFILE)�ĵ�ָ��*/
	long long	volatile seek;	/*	current seek pointer, ��Ϊ��ֵ����ʾ��ǰ���� */
	short	mode;			/*	�ļ���ģʽλ���壬���庬����� */
} FDTAB;

/*	mode flags �ļ�ģʽ��λ���� */
#define M_EMPTY		0x0000
#define M_READ		0x0001
#define M_WRITE		0x0002
#define M_SYSIO		0x0004
#define M_CREATE	0x0008
#define M_REMOVE	0x0010
#define M_DIRECTORY	0x0020
#define M_RENAME	0x0040
#define M_NOCTTY	0x0080
#define M_TSOCKET	0x0100
#define M_APPEND	0x0200
#define M_EXCL		0x0400
#define M_TRUNC		0x0800
#define M_NONBLOCK	0x1000
#define M_UNLINK	0x2000
#define	M_ROOTDIRS	0x4000		/* �Ը�Ŀ¼��һ��Ŀ¼���в��� */
#define DUP_MASK	(M_READ|M_WRITE|M_DIRECTORY|M_NOCTTY|M_TSOCKET|M_NONBLOCK)


/*	I/O DATA BUFFER*/
#if 0
typedef struct buft
{
	char	*pbuf;			/* buffer address */
	char	* volatile get;	/* buffer get pointer */
	char	* volatile put;	/* buffer put pointer */
	char	*buflim;		/* buffer limit */
	short	volatile cnt;	/* no. characters */
	short	siz;			/* buffer size */
	short	lowater;		/* wakeup or XON low water level */
	short	hiwater;		/* process wait or XOFF high water level */
} BUFT;
#endif

/*  KERNEL LOCK STRUCTURE */
typedef struct kernlck
{
	void	*volatile locked;	/* process which has kernel resource locked */
	char	protect;			/* heir must run in a critical region */
} KERNLCK;



/*	I/O DRIVER FUNCTION CODES  */
#define	I_INIT		1
#define	I_OPEN		2
#define	I_READ		3
#define	I_WRITE		4
#define	I_CLOSE		5
#define	I_SGDEV		6
#define	I_DEVMSG	7
#define	I_DEVCNT	8
#define	I_SEEK		9
	#define I_SEEK_FROM_START      0
	#define I_SEEK_FROM_CURRENT    1
	#define I_SEEK_FROM_END        2
#define	I_SYNC		10
#define	I_MKFS		11
#define	I_IOCTL		12			/* perform function */
	/* ������ioctl�ľ���Ĳ����룬������չ */
	#define I_IOCTL_BLKSIZ		1	/* get disk block size as function return value */
	#define I_IOCTL_TABLE		2	/* get tty or queue table address (CE-VIEW only) */
									/* buf paramter points to where pointer is returned */
	#define I_IOCTL_DEVTYP		3	/* obtain device type as function return value */
			/* ��ѯdevice type�ķ���ֵ */
			#define I_IOCTL_DEVTYP_IS_TTY		1
			#define I_IOCTL_DEVTYP_IS_QUE		2
			#define I_IOCTL_DEVTYP_IS_DISK		3
			#define I_IOCTL_DEVTYP_IS_FILE		4
			#define I_IOCTL_DEVTYP_IS_NULL		5
			#define I_IOCTL_DEVTYP_IS_SOCKET		6
			#define I_IOCTL_DEVTYP_IS_FIFO		7
	#define I_IOCTL_FORMAT		4	/* format disk media */
	#define I_IOCTL_TIME_OUT	5	/* set TTY or queue read timeout */
	#define I_IOCTL_CAPACITY	6	/* get disk or contiguous file capacity in blocks. */
									/* buf points to a long. */
	#define I_IOCTL_FILE_SIZE	7	/* get current size of file (eof value) */
									/* buf points to a long. */
	#define I_IOCTL_FLUSH		8	/* flush a queue or a terminal input buffer */
	#define I_IOCTL_MIN_READ	9	/* set minimim number of TTY messages to read */
	#define I_IOCTL_MSGDEV		10	/* establish this TTY as the system message device */
	#define I_IOCTL_NERRS		11	/* get device error count as function value */
	#define I_IOCTL_L_ORDER		12	/* convert a long between file subsystem order */
									/* and host order.  buf is (long *) */
	#define I_IOCTL_DK_PARAMS	13	/* get disk parameters */
	#define I_IOCTL_CRT_CLR		14	/* clear crt screen */
	#define I_IOCTL_CRT_CUR		15	/* position crt cursor */
	#define I_IOCTL_S_ORDER		16	/* convert a short between file subsystem order */
									/* and host order.  buf is (long *) */
							
	#define	I_IOCTL_SET_FILE_ATTRIBUTE	17	
	#define I_IOCTL_UPDATE_FAT2			18							

	#define I_IOCTL_DEVPOLL		20	/* poll device */
	#define I_IOCTL_EXCEPTION	21	/* exception indicator for select */
	#define I_IOCTL_ASSIGN_SOCK	22	/* assign a socket */	
	#define I_IOCTL_FREE_SOCK	23	/* free a socket */
	#define I_IOCTL_CARD_DETECT 29  /* detect if physical memory card is present */
	#define I_IOCTL_READ_ID     30  /* read unique memory card ID */
	#define I_IOCTL_CARD_INIT   31  /* initialize card driver parameters */
	#define I_IOCTL_CARD_SIZE   32  /* gives card size */

#define I_RDTMO		13			/* read with timeout */
#define I_SETUP		14			/* set up driver data structures etc. */
#define	I_IRQACT	15			/* make interrupt request line active */
#define I_WRITEQHD	16			/* write to beginning of queue */
#define I_WRTMO		17			/* write with timeout */

#define I_TRUNCATE	18			/* ���ļ��ض�, 20070521 ���� */
#define I_SCANDIR	19			/* ɨ�����ļ���ʽ�򿪵�Ŀ¼���ӵ�ǰ������������һ���ļ�*/

/*	TIMEOUT STRUCTURE ����*/
typedef struct tmout
{
	struct tmout	* volatile tnext;		/* ָ���������һ��TMOUT�ṹ��������_tmoutʹ�ã�sproc�ṹ
											   �еĳ�ʱ�ṹ���һ������ȫ�ֱ���_tlistָ�������һ��
											   ��ʱ�ṹ����������һ��ָ��ա��������������ܸı����ָ��*/
	unsigned		volatile ttime;			/* time-out�¼�������ʣ���ʱ�䣬��λ��tick����������TMOUT
											   �ṹ�Ѿ���active list�У� ttime���ܻᱻ��Ϊһ���µ�timoutֵ��
											   ���Ҫֹͣһ��time-out�����ttimeӦ��Ϊ0�����ttime��Ϊ�㣬
											   ����ÿ��ϵͳ�ж�ʱ������1��ֱ������ֵΪ0��*/
	void			(* volatile tfunc)();	/* ָ��time-out����ʱ��Ҫ���õĺ���������ttime����0ʱ������
											   �ĺ����������������ʱ��pt(TMOUT�ṹָ�룩��Ϊ���������tfunc
											   ָ�����Ϊ�գ���ʾ��time-out����ʱ������Ҫ�����κκ�����*/
	union									/* tfuncָ��ĺ��������õ��Ĳ��� */
	{
			int tint;						/* targ.tint �� integer �͵Ĳ��� */
			void *tptr;						/* targ.tptr �� pointer �͵Ĳ��� */
	} volatile targ;						
	char 			volatile tdone;			/* ʱ�䵽�ڱ��  0 - ��ʱ��ʼ��1 -- ��ʱʱ�䵽��2-- ��ʱ����ʧЧ
												(��������ѱ�wakeup���� )
											   ��һ��TMOUT�ṹ����ʱ������_tmout�������㡣��ttime��Ϊ0ʱ������
											   _tmout������1����һ���Ѿ����е�TMOUT�ṹ��ʼһ���µ�timout
											   ����ʱ���û�Ӧ��tdone���㡣*/
	char 			volatile inton;			/* �ж�������
											   �������tfunc����ʱ�������жϷ���������������Ϊ1��
											   ������_tmoutʹ�ã��������������ܸı����ָ�� */
	void			* volatile tproc;		/* process which activated timer 
											   �����Ϊ�գ�ָ������TMOUT�ṹ���еĽ��̣������ú���_tmout�Ľ���
											   ���Ϊ�գ���ʾTMOUT��������������ʼ����������ġ�
											   ������_tmoutʹ�ã��������������ܸı����ָ�� */
	void			* volatile towner;		/* process which owns timer 
											   ��ʾTMOUT�ṹ������������̵Ľṹ�ģ�SPROC�ṹ����TMOUT�ṹ 
											   ���ΪNULL����ʾ��ָ���û��Լ������TMOUT�ṹʵ��*/
} TMOUT;

/* timer creation options */
#define TM_CREATOR		1			/* owned by creator; same as YES */		
#define TM_MAIN_THREAD	2			/* owned by main_thread */		
#define TM_PERMANENT	3			/* permanent timer */		

/* SELDAT STRUCTURE */
#if 0
typedef struct seldat
{
	char	volatile	collision;		/* 0x0 select collision */
	void	 *volatile	rselect;		/* 0x4 selecting process for read */
	void	 *volatile	wselect;		/* 0x8 selecting process for write */
	void	 *volatile	eselect;		/* 0xC selecting process for exception */
	unsigned volatile	exception;		/* 0x10 exception indicator */
} SELDAT;

/* select collision states */
#define RCOLLIS		0x0001	/* select collision on read */
#define WCOLLIS		0x0002	/* select collision on write */
#define ECOLLIS		0x0004	/* select collision on except */
#endif

/* profiler definitions */
#define TMS_ISR_1	(_maxpids)		/* stats for ISR not nested */
#define TMS_ISR_2	(_maxpids+1)	/* stats for ISR nested */
#define TMS_KLEVEL	(_maxpids+2)	/* stats for deferred ISR kernel level */
#define TMS_SCHED	(_maxpids+3)	/* stats for scheduling time */
#define TMS_IDLE	(_maxpids+4)	/* stats for idle scheduler time */
#define TMS_SAMPLE	(_maxpids+5)	/* sample count */
#define TMS_BUFSIZE	(_maxpids+6)	/* size of profile stats buffer in longs */

/* function prototypes */
void		_kill(pid_t pid, int signo);
FDTAB		*_chkfd(int fd);
void		_clock(void);
char		*_sbreak(unsigned n);
void		_tmout(TMOUT *t, int add);
void		_tmouton(TMOUT *t, int add);
void		_inc_prof(void);

#if 0
void		_bflush(BUFT *pb);
int			ibuf(int size, BUFT *b);
void		_ioctl_exc(int type, void *pd, char c);
int			_ioctl_poll(int type, void *pd, char c);

int			one_of(int n, ...);
void		_selclose(SELDAT *pe);
void		_selinit(SELDAT *pe);
void		_selwaker(SELDAT *pe);
void		_selwakew(SELDAT *pe);
void		_selwakee(SELDAT *pe);
int			tgetc(BUFT *pb);
int			tputc(BUFT *pb, char c);
#endif

/* data declarations */
extern char _enaclk;
//extern char _enaio;


#endif
