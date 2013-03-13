#ifndef _HYHW_AT_BUFFER_H_
#define _HYHW_AT_BUFFER_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include "hyTypes.h"



typedef struct _tSeqQueue
{
   U32  front;
   U32  rear;
   U32	size;
   char	*element;  //���е�Ԫ�ؿռ�
}tSeqQueue;




#ifdef __cplusplus
}
#endif

#endif 