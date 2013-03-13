#ifndef MEMORY_API_H
#define MEMORY_API_H

#include "hyTypes.h"
#include "memory_buddy.h"


/* �ڴ�����ķ�ʽ, Ŀǰ��֧��buddy��ʽ */
#define MEMO_METHOD_BUDDY	1
#define MEMO_METHOD_HEAP	2

/* ��װ�õĽṹ�� */
typedef struct sys_memo_st
{
	U16	type;					// �ڴ����ʹ�õ������׺�����Ŀǰֻ����buddy�ķ�ʽ��
	buddy_memo buddymemo;		// buddy��ʽ����ʱ��Ҫ�Ľṹ��
	
	//other memory operate struct
	
}	sys_memo_st, *sys_memo_st_ptr;



/*----------------------------------------------------------------------
* ���ܣ��ڴ��ʼ��������sram��sdram���ڴ�ĳ�ʼ����		
* ������type ---   �ڴ���������ͣ�Ŀǰֻ��buddy��ʽһ�֣�
*		pram ---   �ڴ�����ʼ��ַ
*		ramlen --- �����ÿ���ڴ泤��
* ���أ�
*		NULL --- ��ʼ��ʧ��	
*		�ṹ��ָ��  --- �ɹ�
*-----------------------------------------------------------------------*/
void *memo_init( int type, U8 *pram, U32 ramlen );

/* ��Ӧ��free��������һ������Ϊmemo_initʱ�ķ���ֵ */
void memo_free( void *pmemo, void *pbuf );

/* ��Ӧ����malloc��������һ������Ϊmemo_initʱ�ķ���ֵ */
void *memo_alloc( void *pmemo, U32 size );

/* ��Ӧ����realloc��������һ������Ϊmemo_initʱ�ķ���ֵ */
void *memo_realloc( void *pmemo, void *memblock, U32 size_new );

/* �õ�ʣ���ڴ��Сֵ������Ϊmemo_initʱ�ķ���ֵ */
U32 memo_rest( void *pmemo );

/* ���������ڴ� */
void memo_destroy( U8 *pram );



#endif	//MEMORY_API_H

