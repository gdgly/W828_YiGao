#ifndef MEMO_BUDDY_H
#define MEMO_BUDDY_H

#include "hyTypes.h"

//////////	CONSTANT  //////////////////////////////////////
#define EMPTY
#define power_max		27
#define sz_min_page		0x10	//	size of page
#define sz_page_head	0x10	//	size of page head


/*========	buddy memory node  ========*/
typedef struct bm_node_tag *bm_node_ptr;

typedef struct bm_node_tag	
{
	S32			kval;		/*	2^kval */
	bm_node_ptr	mstart;		/*	starting address */
	bm_node_ptr	l_link;		/*	prior */
	bm_node_ptr	r_link;		/*	next */
} bm_node, *bm_node_ptr;

typedef struct bm_head_node_tag	
{
	U32			size;		/*	��������п��С */
	bm_node_ptr	first;
} bm_head_node, *bm_head_node_ptr;

typedef struct buddy_memo_t 
{
	U8			inuse;		/* ��Ǹö��ڴ��Ƿ����ڱ�ĳ������ʹ�� */
	void		*plock;		/* ����öν������ڱ�ĳ������ʹ�ã����������Ľ��̷���������� */
	bm_head_node nodehead[32];	/* buddy�õ����ڴ�����ͷ������� */
} buddy_memo;

/*----------------------------------------------------------------------
* ����: buddy_init
* ˵��: ��ʼ���ڴ����head
* ����: pbuddymemo:	�ڴ����ṹ���ָ��
*		pmem:	�ö��ڴ����ʼ��ַ
*		len:	�ö��ڴ�ĳ���
* ����: none
* ע�⣺�ڶ��ڴ�ֿ�ʱ�ɴӴ�С�ֺʹ�С����ֵ���������ʹ�õ��ǵڶ���
*-----------------------------------------------------------------------*/
void buddy_init( buddy_memo *pbuddymemo, void *pmem, U32 len );


/*----------------------------------------------------------------------
* ����: buddy_alloc
* ˵��: ��phead���ڴ���з������һ����СΪsize���ڴ�
* ����: pbuddymemo:	�ڴ����ṹ���ָ��
*		size:		������ڴ��Ĵ�С
* ����: NULL or pointer
*-----------------------------------------------------------------------*/
void* buddy_alloc( buddy_memo *pbuddymemo, U32 size );


/*----------------------------------------------------------------------
* ����: buddy_free
* ˵��: ��phead���ڴ�����ͷ�p_memblockָ����ڴ��
* ����: pbuddymemo:	�ڴ����ṹ���ָ��
*		p_memblock:	Ҫ�ͷŵ��ڴ��
* ����: none
*-----------------------------------------------------------------------*/
void buddy_free( buddy_memo *pbuddymemo, void* p_memblock );


/*----------------------------------------------------------------------
* ����: buddy_realloc
* ˵��: �õ�phead���ڴ������ʣ����ܿռ��С
* ����: pbuddymemo:	�ڴ����ṹ���ָ��
*		memblock:	���ڴ�ĵ�ַ
*		size_new:	���ڴ�Ĵ�С
* ����: ʣ���ܿռ�Ĵ�С
* ����: ����һ���µ��ڴ����������ڴ����Ķ���������ȥ
*-----------------------------------------------------------------------*/
void* buddy_realloc( buddy_memo *pbuddymemo, void *memblock, U32 size_new );


/*----------------------------------------------------------------------
* ����: buddy_rest
* ˵��: �õ�phead���ڴ������ʣ����ܿռ��С
* ����: pbuddymemo:		�ڴ����ṹ���ָ��
* ����: ʣ���ܿռ�Ĵ�С
*-----------------------------------------------------------------------*/
U32	buddy_rest( buddy_memo *pbuddymemo );



#endif //MEMO_BUDDY_H


