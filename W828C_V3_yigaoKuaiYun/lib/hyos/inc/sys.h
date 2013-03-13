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

#ifndef _SYSH
#define _SYSH

#include "driver.h"
#include "uproc.h"
#include "signal.h"
#include "libs.h"


/* SIGNAL HANDLER STRUCTURE 	�źž���ṹ���� */
typedef struct sig_hand
{
	void		**volatile	which_wlist;	/* pointer to ewait list. */
											/* Signal handler sets which_wlist to 1 for E_INTR */
	sigset_t	volatile	pending;		/* pending signals */
	sigset_t	volatile	mask;			/* masked signals */
} SIG_HAND;


/*	SYSTEM PROCESS TABLE	ϵͳ���̵Ľṹ
	ע�⣬asmarm.s �У�Ĭ�ϵ�sproc�ṹ��˳��Ϊ����ṹ��˳���������ṹ�仯�ˣ�
	���ڻ���еĴ���Ҳ��Ҫ�仯  sizeof(SPROC) == 0xAC */
typedef struct sproc
{
	unsigned volatile	state;			/* 0x0	����״̬���������λ���� */
	pid_t				pid;			/* 0x4	����id */
	char				ptype;			/* 0x6	���̻����߳� */
	char	volatile	selecting;		/* 0x7	process is currently doing select */
	int					(*pentry)();	/* 0x8	������ں����ĵ�ַ */
	LNKLST				*plnklst;		/* 0xC	ptr to save area for ulib */
	short				epri;			/* 0x10	effective �������ȼ� */
	short				ipri;			/* 0x12	���̳�ʼ���ȼ� */
	unsigned			*stack;			/* 0x14	����ջ�Ķ������̽��������ֵ���ٸı� */
	unsigned *volatile	ustack;			/* 0x18	����ջָ�룬���Ž��̵����ж���̬�仯 */
	unsigned			*slimit;		/* 0x1C	����ջ�ĵף����̽��������ֵ���ٸı� */
	void	(*volatile	exitf)();		/* 0x20	ָ������˳�ʱ��Ҫ���õĵ�һ������������һЩ�ֳ������� 
												�μ�exit�����У��ڽ����˳�ʱ���˳������ǵݹ����
												���м���������Ҫ����ʱ��ָ���һ�������õĺ������������
												���ķ���ֵ�Ǻ���ָ�룬ָ��ڶ���Ҫ�����õĺ�����������*/
	FDTAB				*pfd;			/* 0x24	process fd table entry */
	short	volatile	per;			/* 0x28 ������������ڵر����õģ����������ʾʱ���� */
	short	volatile	due;			/* 0x2A	������ʱ����ʱ�� periodic schedule due time counter */
	short				sleep;			/* 0x2C	���̽���˯��״̬ʱ��˯��ʱ������� */
	unsigned short		nlocks;			/* 0x2E	no. of locked resources */
	void	*volatile	param;			/* 0x30	parameter to pass on periodic schedule */
	
	struct sproc *volatile wlist;		/* 0x34	�ȴ������������һ�����̵�wlistΪNULL����wlist_tail���ʹ�� */
	struct sproc *volatile clklst;		/* 0x38	ʱ����ƽ������� */
	
	short				cpri;			/* 0x3C	current priority saved for heir */
	short				cslice;			/* 0x3E	current timeslice value saved for heir */
	DVCFG				*pstdin;		/* 0x40	device to use for standard input */
	DVCFG				*pstdout;		/* 0x44	device to use for standard output */
	DVCFG				*pstderr;		/* 0x48	device to use for standard error */
	struct sproc *volatile heir;		/* 0x4C	process to whom priority is given, heir �̳��� 
												�����˺͵�ǰ����һ�����ȼ��Ľ��̣���������Ϊ�������ռ��
												����Դ��ͬʱ�����ȼ��Ľ������������Դ��������ʱ�����������ȼ� */
	struct sproc *volatile benefact;	/* 0x50	heir's benefactor ������ 
												���������̱���ʱ���������ȼ���������ȼ���������������
												���ڵ�ǰ�ϵ����ȼ��Ľ���ռ����benefact����Դ
												��ǰ���̾���heir���̣���ԭ�ȵ����ȼ��ڳ�Աcpri���棬ԭ�ȵ�
												ʱ��Ƭ��cslice����*/
	TMOUT				tmout;			/* 0x54	+ 0x1C process tmout structure */
	SIG_HAND			*psighand;		/* 0x70	pointer to signal handler struct */
	KERNLCK				*kernlock;		/* 0x74	pointer to kernel lock */
	unsigned short volatile time_left;	/* 0x78	remaining time for sleep/wait */
	unsigned short volatile slice_left;	/* 0x7A	remaining time for peer process */
	struct sproc		*peer_next;		/* 0x7C process of peer group */
	union
	{
		int volatile	proc;			/* 0x80 process return value */
		void volatile	*thread;		/* 		thread return value */
	} exit_status;
	void	*volatile	wchan;			/* 0x84 event waiting for in timed sleep */
	struct sproc *volatile pnext;		/* 0x88 pointer to next SPROC */
	char				*pname;			/* 0x8C �������� */
	clock_t 			tms_ptime;		/* 0x90 pointer to process times */
	struct sproc **volatile wlist_tail;	/* 0x94 ��¼�ȴ����������β��ָ�룬��wlist���ʹ�� */
	void	(*volatile	exlock)();		/* 0x98 saves onexit return in lock */
	void	(*volatile	extmout)();		/* 0x9C saves onexit return in tmout 
												��tmout����ʱ��Ҫ���õĺ���������Ҫ��tmout������д��������Ȼ�� */
	void				*ce_dos;		/* 0xA0 CE-DOS specific data */
	void				*ce_tcp;		/* 0xA4 CE-TCP specific data*/
										/* this is required by finit, which is not compiled for PSX */
	struct sproc		*main_thread;	/* 0xA8 main thread process */
} SPROC;

/*	����ĽṹSPROC��state��Ա�Ķ���*/
#define	STATE_MASK	0x3ff
#define P_ACTIV		0x001
#define	P_INACT		0x002
#define P_SLEEP		0x004
#define P_WAIT		0x008
#define P_ZOMBI		0x010
#define P_SIGWAIT	0x040
#define P_SUSPEND	0x080
#define P_CONDWAIT	0x100
#define P_TRANS		0x200
#define P_SIG		BIT31		/*process flags: top bit is signal pending */

/* process types */
#define PT_PROCESS	0x00		/* standard process */
#define PT_THREAD	0x01		/* thread */
#define PT_ITHREAD	0x02		/* initial thread of multi-threaded process */

/* thread type macros
 */
#define IS_THREAD(a)		((a)->ptype == PT_THREAD)
#define IS_MAIN_THREAD(a)	((a)->ptype == PT_ITHREAD)
#define IN_THREAD_PROC(a)	((a)->ptype != PT_PROCESS)

/* priority compares*/
#define HI_PRI(a,b)	((a)->epri > (b)->epri)
#define EQ_PRI(a,b)	((a)->epri == (b)->epri)
#define LO_PRI(a,b)	((a)->epri < (b)->epri)

/*	process ID table entry 
	����ID��*/
union pidtab_ent
{
	SPROC *volatile psproc;
	union pidtab_ent *volatile pfree;
};

/* get pointer to SPROC for a specific pid */
#define PSPROC(a) (_pidtab[(a)].psproc)

/* validity check of pid */
#define VAL_PID(a)	(_pidtab[(a)].pfree > _pidtab_end)
#define INV_PID(a)	(_pidtab[(a)].pfree <= _pidtab_end)

/*  ���ȼ�����һ��˫�����������ȼ��Ӹߵ����������� 
	priority table entry */
typedef struct pritab_ent
{
	struct sproc *volatile prsproc;		/* ���������1: ��ǰ���ȼ��Ľ���*/
	struct sproc *volatile prtail;		/* ���������2: ������ͬ���ȼ���β���̣���Ϊ�˼ӿ��ٶȡ�
										   ��������ͨ��sproc�ṹ�еĳ�Աpeer_next��������
										   ��prtail��peer_next�У�ָ��գ���ʾ���� */
	struct pritab_ent *pr_next;			/* ��һ��������ȵ�ǰ�����ȼ��ͣ��ջ�, ������������ȼ���next�ȵ�ǰ��*/
	struct pritab_ent *pr_back;			/* ǰһ��������ȵ�ǰ�����ȼ��ߣ��ջ�������������ȼ���pr_back �ȵ�һ��������ȼ��ĵ�   */   
										/* ���� ���ȼ��б�Ϊ 11 - 8 - 5 -1  
											11.next = 8;   8.next= 5;  5.next = 1; 1.next = 11;
											1.back  = 5;   5.back = 8; 8.back = 11; 11.back = 1;*/
										
} PRITAB;

/* priority tests */
#define IS_PEER(a)   (_pritab[(a)->epri].prtail)
#define PRI_USED(a)  ((a)->prsproc)

/* empty priority table entry */
#define PRI_EMPTY (NULL)

/* scheduler_event states  �������¼�*/
#define SCH_NOEVENT		0		/* no event, normal advance to next process */
#define SCH_ISEVENT		1		/* event, go directly to highest active */
#define SCH_NOADVANCE	2		/* no event, do not preempt to next process */

/* benefactor/heir region states */
#define RUN_HEIR		1		/* heir process to be run with no preemption */
#define RUN_BENEFACTOR	2		/* return to benefactor with no preemption */

/* �ں��ӻ������Ĳ�����KERNEL QUEUES */
union kqitem
{
	void *p;
	unsigned long l;
};


/*	��¼��Ҫִ�е��ж��ӻ�������������Ľṹ�����磬ewakeup���������ж��е��ã�����ʵ����
	���ж��в�������ִ�У����ǰ������뵽ϵͳ���ж��ӻ����������У��˳��жϺ���ִ��
	����������ж��ӻ��������д��3��ֵ��Ҫִ�еĺ����ĵ�ַ���ú����Ĳ���������֧��
	�������������*/
typedef struct kqentry
{
	void	(*func)();
	union kqitem *p1;
	union kqitem *p2;
}KQUE;

/* kernel process options
 * NOTE: These bits values are encoded in the kqueXXX.asm modules! */
#define K_HI		(1U << (BITS_INT - 1))
#define K_CLKLST	0x0001
#define K_KQUEHI	0x0002
#define K_KQUELO	0x0004
#define K_TMOUT		0x0008
#define K_TIMSLIC	0x0010		/* asynchronous, peer timeslice expired */


/*	RESOURCE COORDINATOR TABLE */
typedef struct rstab
{
	unsigned	volatile	rsource;	/* ��������Դ��ֵ */
	pid_t		volatile	rspid;		/* ������ڵ���0����ʾ������Դ�Ľ���id�ţ����С��0����ʾ��Դ״̬��������Ԥ���� */
	void		*volatile	rswait;		/* ָ��ȴ���Դ�Ľ���������׽��̣�������ͨ�����̽ṹ������ */
} RSTAB;

/*	resource states ����rspid��¼*/
#define R_EMPTY	(-1)	/* ��ʾ��Դ�Ľṹ���λ�û�û��ʹ�ù�*/
#define R_NBUSY	(-2)	/* ��ʾ��Դ�Ľṹ���λ��ʹ�ù��������ڿ��� */


/*	fast form of entreg, entsig, _entsys
 */
#define _entreg() ++_noswit
#define _entsig() ++_nosig
#define _entsys() {++_nosig;++_noswit;}

/* fast form of lvreg
 */
#define _lvreg() \
	{ \
	if (!--_noswit && (_klev_flags || _event)) \
		__lvreg(); \
	}

/* fast form of lvsig
 */
#define _lvsig() \
	{ \
	lvsig(); \
	}

/* fast form of _lvsys
 */
#define _lvsys() \
	{ \
	_lvreg(); lvsig(); \
	}

#define tsleep(x)	_tsleep(x)

/* function prototypes
 */
short _chgepri(short pri);
short _chgpri(pid_t pid, short priority);
int _close(register FDTAB *pfd);
int _cmpbuf(char *s1, char *s2, int n);
int _cmpstr(char *s1, char *s2);
void _creatclk(SPROC *p);
void _deletep(SPROC *pu);
void _doclk(unsigned nunits);
void _entp(void *param);
int _ewait(void **list, int sig_ign);
void _ewait_tmo(TMOUT *tm);
int _ewaittm(register void **list, unsigned tout, int sig_ign);
void _extract_proc(SPROC *p);
unsigned _fill(char *buf, unsigned n, char c);
void _ientp(void *param);
void _initproc(char *mem, char *name, unsigned ssize, unsigned thread,
	char *sin, char *sout, char *serr);
void _insert_proc(SPROC *pu);
void _isignal(pid_t);
int _is_peer(void);
void _istart(SPROC *, void *);
char *_kalloc(register unsigned need);
int _kewait(void **list, int tmoflag);
int _kfree(char *p);
void _klev_hi(void);
int _kqgethi(void (**func)(), union kqitem **item1, union kqitem **item2);
int _kqgetlo(void (**func)(), union kqitem **item1, union kqitem **item2);
void _kqputhi(void (*func)(), ...);
void _kqputlo(void (*func)(), ...);
unsigned _lenstr(char *);
int *_limit(void);
extern SPROC *_lo_proc;	
void __lvreg(void);
int _open(DVCFG *pdev, int mode, FDTAB *pfd, long size);
void _peer_add_head(PRITAB *pt, SPROC *p);
void _peer_add_tail(PRITAB *pt, SPROC *p);
void _peer_delete(PRITAB *pt, SPROC *p);
void _peer_delete_head(PRITAB *pt, SPROC *p);
void _peer_done(void);
void _peer_raise(void);
void _putclk(SPROC *p);
void _rellock(KERNLCK *plck);
int _send_sig(SPROC *p, int signo);
int _setlock(KERNLCK *plck, int tmoflag);
void _signal(int sig, short p);
char *_strcpy(register char *s1, register char *s2);
void _unsched_peer(void);
void _utos(void);
void _wakeup(void ** list); /* 20071122*/

FDTAB *_chkfd(int fd);

#if 0
void _abort_proc(SPROC *p);
SPROC *_add_peer(SPROC *pu);
void _init_thread(SPROC *p);
unsigned _ltob(char *s, long n, short base);
void _mrelease(void);
void _private_destroy(void);
void _putstr(int file, ...);
int _sig_thread(SPROC *p, int signo);
void selwakeup(SPROC *p, int collision);
void _trelease(void);
void _thread_cancel_test(void);
void _thread_exit(void);
int _tsleep(char *pchan, int pri, char msg[], int tval);
void _twakeup(char *pchan);
void _wakepid(int status);
void _del_peer(SPROC *pu);
#endif

/* data declarations*/

extern ALLDAT alldat;
extern char	volatile _benereg;	
extern clock_t _clk_tck;
extern SPROC *volatile _clklst;
extern SPROC *_cproc;
extern char *_def_fs;
extern char	volatile _event;	
extern int	 _freex;	
extern KERNLCK _heaplck;
extern unsigned volatile _hi_ticks;
extern unsigned volatile _hi_timun;
extern char volatile _input_wake;
extern char	*_hptop;
extern char	_in_klev;
extern char	_in_idle;
extern char	_in_nest;
extern char	_in_sched;
extern char	_in_state;
extern char	volatile _intsig;
extern char	_is_init;
extern char	_keep_main;
extern unsigned volatile _klev_clk;
extern unsigned volatile _klev_flags;
extern char	_klevhipri;
extern unsigned volatile _klev_ticks;
extern unsigned volatile _klev_timun;
extern unsigned volatile _kqcnthi;
extern unsigned volatile _kqcntlo;
extern int volatile _kqovf_hi;
extern int volatile _kqovf_lo;
extern KQUE *_kque_hi;
extern KQUE *_kque_lo;
extern RSTAB *volatile _lprstab;	
extern short _maxdev;
extern short _maxfd;	
extern unsigned _maxkqhi;
extern unsigned _maxkqlo;
extern pid_t _maxpids;
extern short _maxpri;
extern int _nkqhi;
extern int _nkqlo;
extern char	volatile _nosig;	
extern char	volatile _noswit;
extern int volatile _nproc;	
extern short _nres;		
extern KQUE *_pgethi;
extern KQUE *_pgetlo;
extern char	*volatile _pheap;	
extern union pidtab_ent *_pidfree_hd;
extern union pidtab_ent *_pidfree_tail;
extern union pidtab_ent *_pidtab;
extern union pidtab_ent *_pidtab_end;
extern KQUE *_pmaxhi;
extern KQUE *_pmaxlo;
extern int _prempt;	
extern PRITAB *volatile _pri_cur;
extern PRITAB *volatile _pri_head;
extern PRITAB *_pritab;
extern PRITAB *_pritab_end;
extern RSTAB *_prstab;	
extern void (*_pdrv_dos)();
extern void (*_pdrv_tcp)();
extern KQUE *_pputhi; 
extern KQUE *_pputlo; 
extern short _psiz_dos;
extern short _psiz_tcp;
extern char _psx;
extern USRCFG *_pusrcfg;	
extern UPROC *_puproc;	
extern KERNLCK _reslck;	
extern clock_t volatile _seconds;	/* seconds for date & time */
extern clock_t volatile _sec_cnt;	/* partial second measured in clock ticks */
extern SPROC *volatile selwait;	
extern unsigned short _slice_val;
extern char	*_sstack;	
extern char	_stack_clr;
extern char	volatile _state;		
extern int *sused;	
extern char	_svflt;		
extern unsigned volatile _tck_cnt;		
extern unsigned _tcks_unit;		
extern clock_t _tick;		
extern clock_t	volatile _timer;		
extern unsigned short volatile _timeslice;
extern clock_t	*_tms_utime;
extern void	*volatile _tsleep_hd;
extern TMOUT *volatile _tlist;	
extern char	*tzname[2];

extern CONST char *_err_tab[];
extern CONST int _max_errs;
extern unsigned _ndoub;
extern unsigned _nreg;
extern USRCFG * (*pulink)();

extern char	_environ[];			/* environment string */

extern char	**environ;			/* environment list */
extern SPROC *volatile _twaitlst;	
extern SPROC *volatile _zwaitlst;	

#endif
