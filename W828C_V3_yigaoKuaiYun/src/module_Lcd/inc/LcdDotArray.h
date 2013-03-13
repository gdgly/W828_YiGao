
#ifndef _LCDDOTARRAY_H_ 
#define _LCDDOTARRAY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "LcdAPIFunc.h"
 

typedef struct _StrAttribute
{
	U32 LanguOffset; //���ִ�С�ַ����ֿ��е�offset
	U16 FontSize;        //�������ԣ���С 8*16 ��24*24 32*32��
	U16 BytesOccupy;    //�������ֿ���ռ�ݵĴ�С
	U8  *pStr;          //�ַ�����ָ��
	U8  *pBuf;          //��ģָ��
	Character_Type_e  	Type; //��ǰ�ַ����ֿ��е�����(Unicode ,Local GBK , ���� ������)
	
}StrAttribute,*PStrAttribute;

/*-----------------------------------------------------------------------------
* ����:	drvLcd_GetSBCCaseCharBMP
* ����:	�õ���������������ģ
* ����:	kStr_Type------�ַ�����
*			CHARACTER_UNICODE---unicode�ַ���
*			CHARACTER_LOCALCODE-�������ַ���
*		kUnicode_High--unicode��8λ�����ر���ʱΪ���룩
*		kUnicode_Low---unicode��8λ�����ر���ʱΪλ�룩
*		*buff----------��ģ����
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_GetSBCCaseCharBMP(Character_Type_e kStr_Type,U8 kUnicode_High,U8 kUnicode_Low,U8 *buff,U16 bytes);

/*-----------------------------------------------------------------------------
* ����:	drvLcd_GetSBCCaseCharBMP
* ����:	�õ���������������ģ
* ����:	kStr_Type------�ַ�����
*			CHARACTER_UNICODE---unicode�ַ���
*			CHARACTER_LOCALCODE-�������ַ���
*		kUnicode_High--unicode��8λ�����ر���ʱΪ���룩
*		kUnicode_Low---unicode��8λ�����ر���ʱΪλ�룩
*		*buff----------��ģ����
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_GetDBCCharBMP(StrAttribute *AttriStr);

/*-----------------------------------------------------------------------------
* ����:	drvLcd_GetDBCCaseCharBMP_Special
* ����:	��ȡ�����ַ�����ģ��>0x80��
* ����:	kChar----------�ַ���>0x80��
*		kLanguageType--��������
*		*buff----------��ģ����
* ����:	none
*----------------------------------------------------------------------------*/
//void drvLcd_GetDBCCaseCharBMP_Special(U8 kChar, U16 kLanguageType,U8 *buff);


#ifdef __cplusplus
}
#endif

#endif // _LCDDOTARRAY_H_

/*============================ History List ================================
 1. Rev 0.1					 20040107             HYCZJ
========================================================================*/
