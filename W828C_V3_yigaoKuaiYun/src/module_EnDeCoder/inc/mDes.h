#ifndef _MDES_H
#define _MDES_H

#include "hyTypes.h"

/*
3des�ӽ���
�ýӿ����������ݷ�ʽΪ:
k = 8-(srclen%8);
memset(&src[srclen],k,k);

src:Դ����
srclen:Դ���ݳ���
dst:���ձ����������buf
key:��Կ
type:����  0:����   1:����
*/
int Des3(char *src, int srclen, char *dst, char *key, int type);

#endif