/*                 md5.h           */
#ifndef _MMD5_H_
#define _MMD5_H_

#include "hyTypes.h"

/*
pData:����������
Len:���������ݳ���
pCode:���ܺ������
*/
void mMD5_Encode(U8 *pData,U32 Len,U32 *pCode);

#endif /* _MMD5_H_ */