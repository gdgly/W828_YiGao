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

#ifndef _LIBSH
#define _LIBSH

/*	HEAP MANAGEMENT STRUCTURE
	ÿ��������ڴ�ѵ�ͷ��������ṹ��ͷ8���ֽھ���HEAP�ṹ��������ڴ������
	������ڴ��С���ǿ��Ը��û�ʹ�õ��ڴ� 
	
	���ڻ��չ����ڴ棬��kalloc ��kfreeά�����յ��ڴ����� ��һ���ͷŹ����ڴ��
	�ĵ�ַ������alldat (��kalloc.c���еĳ�Աavail��
	
	���ڷ�����ڴ棬���ϵͳҪ������˳�ʱȫ���ͷ��ڴ棬������ά��ĳ�����̵�
	������ڴ�ѣ���nalloc ��_free ά��	*/
typedef struct heap
{
	struct heap *volatile next;
	union
	{
		unsigned size;		/* number of bytes following the header */
		void *p;			/* assure HEAP is rounded to size of pointer */
	} u;
} HEAP;


/*	�����ڴ�ѵĶ��ĵ�ַ�����������ڴ�ѵĵ�ַ�ļ�� */
#define	topofheap(pheap)	((unsigned)(pheap) + sizeof(HEAP) + pheap->u.size)


/*	DATA STRUCTURE FOR alloc AND free
	This structure exists only once and holds static data for alloc and free */
typedef struct
{
	HEAP avail;				/* dummy pointing to first free chunk of heap��ָ���ͷŵ��ڴ�� */
	HEAP *volatile bottom;	/* lowest valid heap address for free */
	HEAP *volatile top;		/* current top of heap */
	void *volatile plock;	/* wait list for heap management */
	volatile int inuse;		/* heap management in use , heap �Ƿ����ڽ��з�����ͷ�*/
} ALLDAT;


/*	PROCESS LINK LIST
 *	This structure saves onexit returns and linked list pointers.
 *	If anything is added to this structure which is not a pointer,
 *	function istart will have to be changed.
 */
typedef struct lnklst
{
	void /*FIO*/*volatile pfio;				/* 0x0 buffer-flush link */
	HEAP	*volatile alloc;			/* 0x4 ptr to alloced heap chunks */
	void	(*volatile exalloc)();		/* 0x8 place to save onexit return in alloc */
	void	(*volatile exfinit)();		/* 0xc place to save onexit return in finit */
	void	(*volatile extsleep)();		/* 0x10 place to save onexit return in tsleep */
	void /*FIO*/		*volatile stdin;			/* 0x14 address of input control buffer */
	void /*FIO*/		*volatile stdout;			/* 0x18 address of output control buffer */
	void /*FIO*/		*volatile stderr;			/* 0x1c address of std. error control buffer */
} LNKLST;

#endif
