
#ifndef __HY_MEM_MANAGE_API_H__
#define __HY_MEM_MANAGE_API_H__


////////////////////////////	INCLUDE		//////////////////////////////

//#include "../basic_type.h"
#include "hyTypes.h"


#ifdef __cplusplus
extern "C"{
#endif


////////////////////////////	MACRO	//////////////////////////////////

#define GETBUFFER		hy_malloc
#define REGETBUFFER		hy_realloc

#define FREEBUFFER( Ptr ,idx)	\
{								\
	if	( (Ptr) != NULL )		\
	{							\
		hy_free( (Ptr) ,idx);	\
		(Ptr) = NULL;			\
	}							\
}


////////////////////////////	PROTOTYPE	//////////////////////////////

//------------------------------------------------------------------------
//	���к���������hy_mem_create, hy_mem_load_blocks, hy_mem_add_block
//	֮��, hy_mem_release ֮ǰ��������ĵ���.
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// name:	hy_malloc - public	
// desc:	�ڴ���亯�� ͬ pc �ϵ� malloc
// params:	
// return:	HBOOL, TRUE to indicate success
//------------------------------------------------------------------------
void* hy_malloc( U32 size, U8 idx);


//------------------------------------------------------------------------
//	prototype:	hy_realloc( void *memblock, U32 size_new )
//	function:	same as realloc on PC
//				����һ���µ��ڴ����������ڴ����Ķ���������ȥ��
//	parameter:	void *memblock	���ڴ����ĵ�ַ
//				U32 size_new	���ڴ����ĳߴ�

//	return:		������ٳɹ������ɹ��������¿����ڴ�ε�ַ
//				��������ڴ���׵�ַΪNULL����Ϊ�俪��һ���µ��ڴ�
//				����¿����ڴ�ʧ�ܣ��򷵻�NULL
//------------------------------------------------------------------------
void* hy_realloc( void *p_memblock, U32 size_new, U8 idx);


//------------------------------------------------------------------------
//	prototype:	void hy_free( void* memblock )
//	function:	same as free() on PC
//	parameter:	void *p_memblock:	address of the memblock you wanna release
//	copyright:	Hyctron
//------------------------------------------------------------------------
void hy_free( void *p_memblock, U8 idx);


//------------------------------------------------------------------------
//	debug func
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// name:	hy_mem_inspect - public
// desc:	������, �б���, ����������ʱ�̵���
//			���ú�, ��֪���ڴ���Ƿ��ƻ�. 
// params:	NULL
// return:	���� TURE ���ʾ��Ȼ���! ������ƻ��򷵻�False, ���߸ɴ���������.
//------------------------------------------------------------------------
BOOL hy_mem_inspect(U8 idx);


//------------------------------------------------------------------------
// name:	buddy_rest - public
// desc:	return the quantity ( size in bite ) of the usable memory
//			ʣ���ڴ�鿴. ����������ʱ�̵���
// params:	NULL
// return:	size of the remain memory
//------------------------------------------------------------------------
S32 hy_mem_rest(U8 idx);




#ifdef __cplusplus
}
#endif


#endif	//	__HY_MEM_MANAGE_API_H__