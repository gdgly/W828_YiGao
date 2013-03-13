#ifndef _GBK2UNICODE_H_
#define _GBK2UNICODE_H_

#include "hyTypes.h"


#define   LITTLE_ENDIAN 0 //С�˱��뷽ʽ
#define   BIG_ENDIAN    1 //��˱��뷽ʽ

/*-----------------------------------------------------------
* ����:Hyc_Gbk2Unicode 
* ����:��gbk�ַ���ת����utf16�ַ��� ��β�� �����ֽڵ�NULL
* ����: pGbk gbk�ַ��� pUTF16�����ܵ�unicode��ַ�ռ� type:0:С�� 1����� �����uicode��ʽ,gbkһ����С�˷�ʽ��
* ����: unicode �ַ�����ռ�ĳ��ȣ�bytes�������β��
*-----------------------------------------------------------*/
U32 Hyc_Gbk2Unicode(char *pGbk, char *pUTF16, U32 type);

/*-----------------------------------------------------------
* ����:Hyc_Unicode2Gbk 
* ����:��unicode�ַ���ת����gbk�ַ��� ��β�� 1���ֽڵ�NULL
* ����: pUTF16��unicode�ַ��� pGbk: ���ܵ�gbk��ַ�ռ�  type:0:С�� 1����� ��unicode���뷽ʽ��gbk���ΪС�˷�ʽ��
* ����: gbk �ַ�����ռ�ĳ��ȣ�bytes�������β��
*-----------------------------------------------------------*/
U32 Hyc_Unicode2Gbk( char *pUTF16, char *pGbk, U32 type);

#endif