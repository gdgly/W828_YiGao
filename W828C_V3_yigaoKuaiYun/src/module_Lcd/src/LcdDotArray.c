/*************************************************************
%
% Filename     : LcdDotArray.c
% Project name : part of the LCD driver library of Hyctron
%
% Copyright 2003 Hyctron Electronic Design House,
% "Hyctron Electronic Design House" Shanghai, China.
% All rights are reserved. Reproduction in whole or in part is prohibited
% without the prior written consent of the copyright owner.
%
% Rev   Date    Author          Comments
%      (yymmdd)
% -------------------------------------------------------------
% 001   040106  HYCZJ		Primary version
% -------------------------------------------------------------
%
%  This source file contains the character dot array operation for LCD display
%
****************************************************************/
#include "HSA_API.h"

#include "glbVariable_base.h"

#include "LcdAPIFunc.h"
#include "LcdDotArray.h"
#include "AbstractFileSystem.h"


#define c1WORD_BYTES		32//һ��ȫ���ַ���ģ���ֿ�����ռ�õ��ֽ���
#define c1WORD_OCCUPY_BYTES	32//һ��ȫ���ַ���ģ���ֽ���

#define FONTTEMPLATE_FILENAME	"C:/TestFont/gbk_23.bin"

typedef enum _FontTemplate_en
{
	Template_In_Reserve = 0,	//��ģ��reserve����
	Template_In_File			//��ģ��ָ�����ļ���

} FontTemplate_en;

//extern volatile __align(128) unsigned char horAscii8x16[256][16];

/***********************************************************************************
 * Function: void hyc_UTF8toGB(char *string, int size)
 * Description: convert the 2-byte unicode string to 2-byte GB string
 *             ע�⣬unicode Ϊ2���ֽ�???�� gb2312Ϊ2���ֽ� ??? to confirm?
 * Parameter: *string --  string need to be convert, ע��һ��Ҫ�н�����'\0'
 * return: none
 ************************************************************************************/
void hyc_UTF8toGB(char *string)
{
	return;
}

/*-----------------------------------------------------------------------------
* ����:	Get_GBKCurLocalCharBMP_Offset
* ����:	�õ����ر�������(��)��ģ����ģ���е�offset
* ����:	kQu---����
*		kWei--λ��
* ����:	��ģ����ģ���е�offset
*----------------------------------------------------------------------------*/
static U32 Get_GBKCurLocalCharBMP_Offset(U8 kQu,U8 kWei,FontTemplate_en * pGBK_FontTemplate)
{
	int offset;
/*
	// ʹ��GBK���˳�����е��ֿ⣬
	if(kQu>=0xA1 && kQu<=0xF7 && kWei>=0xA1)
	{
		// GBK/1: GB2312�Ǻ��ַ��� GBK/2: GB2312 ����
		offset=((kQu-0xA1)*94 +(kWei-0xA1))*c1WORD_BYTES;
	}
	else if(kQu >=0x81 && kQu<= 0xA0)
	{
		// GBK/3: ���人��
		offset=((kQu-0x81)*191 +(kWei-0x40)+17954)*c1WORD_BYTES;
	}
	else if(kQu >= 0xAA && kQu <=0xFE && kWei<0xA1)
	{
		// GBK/4: ���人��
		offset=((kQu-0xAA)*97 +(kWei-0x40)+9709)*c1WORD_BYTES;
	}
	else
	{
		//kQu = 0xA3;
		//kWei = 0xBF;
		//����ʶ���ַ�����ʾ"��"��ȫ�ǣ�
		offset=0x1B40;
	}
	return offset;
*/
	*pGBK_FontTemplate = Template_In_Reserve;
	// ʹ��GBK���˳�����е��ֿ⣬
	if(kQu>=0xA1 && kQu<=0xF7 && kWei>=0xA1)
	{
		// GBK/1: GB2312�Ǻ��ַ��� GBK/2: GB2312 ����
		offset=((kQu-0xA1)*94 +(kWei-0xA1))*c1WORD_BYTES;
	}
	else if((kQu >=0x81 && kQu<= 0xA0) && (kWei>=0x40))
	{
		// GBK/3: ���人��
		offset=((kQu-0x81)*191 +(kWei-0x40)+17954)*c1WORD_BYTES;
	}
	else if(kQu >= 0xAA && kQu <=0xFE && kWei<0xA1)
	{
		// GBK/4: ���人��
		offset=((kQu-0xAA)*97 +(kWei-0x40)+9709)*c1WORD_BYTES;
	}
	else if(kQu >= 0xA8 && kQu <=0xA9 && kWei<0xA1)
	{	// GBK/5: ����Ǻ���
		offset=0x1B40;//����ʶ���ַ�����ʾ"��"��ȫ�ǣ�
	}
	else if((kQu>=0xAA && kQu<=0xAF) && ( kWei>=0xA1))
	{	// �û��Զ����� (1)
		*pGBK_FontTemplate = Template_In_File;
		offset = ((kQu-0xAA)*94 +(kWei-0xA1))*c1WORD_BYTES;
	}
	else if((kQu>=0xF8 && kQu<=0xFE) && ( kWei>=0xA1))
	{	// �û��Զ����� (2)
		*pGBK_FontTemplate = Template_In_File;
		offset = ((kQu-0xF8)*94 +(kWei-0xA1)+564)*c1WORD_BYTES;
	}
	else if((kQu>=0xA1 && kQu<=0xA7) && (kWei>=0x40 && kWei<0xA1))
	{	// GBK �û��Զ����� (3)
		*pGBK_FontTemplate = Template_In_File;
		offset = ((kQu-0xA1)*97 + (kWei-0x40)+564+658) * c1WORD_BYTES;
	}
	else if((kQu >=0x81) &&(kWei>=0x20) && (kWei<0x40))
	{	//˫�ֽڵ�����û�5�� Code in ��Ӣ�����Ͱ�
		*pGBK_FontTemplate = Template_In_File;
		offset = ((kQu-0x81)*32 + (kWei-0x20)+564+658+679) * c1WORD_BYTES;
	}
	else
	{
		//kQu = 0xA3;
		//kWei = 0xBF;
		//����ʶ���ַ�����ʾ"��"��ȫ�ǣ�
		offset = 0x1B40;
	}
	return offset;
}

/*-----------------------------------------------------------------------------
* ����:	Get_KOREANCurLocalCharBMP_Offset
* ����:	�õ����ر��뺫����ģ����ģ���е�offset
* ����:	kQu---����
*		kWei--λ��
* ����:	��ģ����ģ���е�offset
*----------------------------------------------------------------------------*/
static U32 Get_KOREANCurLocalCharBMP_Offset(U8 kQu,U8 kWei)
{
//	���ģ�
//	��0X8141��ʼ:
//	1���������ֲ��֡��ַ������֣�
//	�ӵ�0X81����ʼ��0xC8����
//	ÿ������0XXX41--0XXXFE
//	2�������ֲ��֣�
//	�ӵ�0XCA����0XFD����
//	ÿ������0XXXA0--0XXXFE
	int offset;

	if(kQu>=0xA1 && kQu<=0xFD&& kQu!=0xC9 && kWei>=0xA1)
	{
		offset=((kQu-0xA1)*94 +(kWei-0xA1))*c1WORD_BYTES;
	}
//	else if(kQu >= 0xA1 && kQu <=0xC8 && kWei<0xA1)
//	{
//		offset=((kQu-0xA1)*97 +(kWei-0x40)+8742)*c1WORD_BYTES;
//	}
//	else if(kQu >=0x81 && kQu<= 0xA0)
//	{
//		offset=((kQu-0x81)*191 +(kWei-0x40)+11961)*c1WORD_BYTES;
//	}
	else
	{
		//kQu = 0xA3;
		//kUnicode_Low = 0xBF;
		//����ʶ���ַ�����ʾ"��"��ȫ�ǣ�
		offset=0x1B40;
	}

	return offset;
}

/*-----------------------------------------------------------------------------
* ����:	Get_JAPANCurLocalCharBMP_Offset
* ����:	�õ����ر���������ģ����ģ���е�offset
* ����:	kQu---����
*		kWei--λ��
* ����:	��ģ����ģ���е�offset
*----------------------------------------------------------------------------*/
static U32 Get_JAPANCurLocalCharBMP_Offset(U8 kQu,U8 kWei)
{
//	1����ÿ����188����
//	2����ÿ�����м��0x007Fû���ã�ע����û���ã�������Ϊ�գ����ԣ�
//		ÿ������0X40-0X7eλ������ƫ�������ǣ�λ-0X40��������0x80-0xfcλ
//		������ƫ�������ǣ�λ-0X40-1����
//	3).0XA0��----0XBF����û����
//
//ƫ�������㣺
//	��0x8140-0x9FFC
//	1.�����λ��С��0X7F����
//		((��-0x81)*188+(λ-0x40))
//	2.�����λ�����0X7F����
//		((��-0x81)*188+(λ-0x40-1))
//	��0xE040-0xEAFC
//	3.�����λ��С��0X7F,��
//		(((��-0XE0)+(0XA0-0x81))*188+(λ-0x40))
//	4.�����λ�����0X7F����
//		(((��-0XE0)+(0XA0-0x81))*188+(λ-0x40-1))
//	������ı������ڱ���ģ����û����ģ��

//	��0X8140��ʼ:
//	ÿ������0X40---0XFC��188��,ÿ����������0xXX7Fû���ã�����ÿ������188��
//	1�������Ų��֣�
//		һ����0X8140----0X81FC
//		���ַ��ţ��磺�������š������ŵȵȡ�
//	2).������ĸ���֣�
//		������0X8240----0X82FC��0X8340----0X83FC��0X8440----0X84FC��0X8540----0X85FC��0X8640----0X86FC��0X8740----0X87FC
//		���֡�Ӣ����ĸ��������ĸ��
//		���У�
//		0X85��0X86������Ϊ�գ�ȫΪ�ո񣩡�
//	3).��������
//		��0X8840----0X82FC��0X8940----0X89FC��.....0XFB40----0XFBFC��0XFC40----0XFCFC��
//		���У�
//		��0X8840----0X88FC����ʵ�ʴ�0X889F��ʼ������ǰ����95��0X9F-0X40�����ո�
//		��0X9840----0X98FC������0X9873----0X989EΪ�գ�
//		��0XA0��----0XBF����û���ã�ע�⣺��û��ʹ�ã�������Ϊ�ո�
//		���ԣ�0X9F������Ϊ0XE0����
//		��0XEB��----0XEC����0xEF����û����
//		��0XFC������0XFC4C----0XFCFCΪ�ա�

	int offset;

	//1
	if(kQu>=0x81 && kQu<=0x9F && kWei>=0x40 && kWei<=0x7F)//���ڰ�7F����,jemy,07-06-04
	{
		offset=((kQu-0x81)*188 +(kWei-0x40))*c1WORD_BYTES;
	}
	//2
	else if(kQu>=0x81 && kQu<=0x9F &&  kWei>0x7F)
	{
		offset=((kQu-0x81)*188 +(kWei-0x41))*c1WORD_BYTES;
	}
	//3
	else if(kQu >= 0xE0 && kQu <=0xEA && kWei>=0x40 && kWei<0x7F)
	{
		offset=(((kQu-0xe0)+(0xA0-0x80))*188 +(kWei-0x40))*c1WORD_BYTES;
	}
	//4
	else if(kQu >= 0xE0 && kQu <=0xEA && kWei>0x7F)
	{
		offset=(((kQu-0xe0)+(0xA0-0x80))*188 +(kWei-0x41))*c1WORD_BYTES;
	}
	//����ʶ��
	else
	{
		//kQu = 0x81;
		//kWei = 0x48;
		//����ʶ���ַ�����ʾ"��"��ȫ�ǣ�
		offset=0x0100;
	}

	return offset;
}

/*-----------------------------------------------------------------------------
* ����:	GetFontTemplateFromeFile
* ����:	��ָ�����ļ��л���������ģ
* ����:	data----�����ģ��buffer
*		offset--��ģ���ļ��е�ƫ��
*		size----��ģ���ݵĴ�С
* ����:	0-------�ɹ�
*		��0-------ʧ��
*----------------------------------------------------------------------------*/
static int GetFontTemplateFromeFile( char * data, int offset, int size )
{
	int fp,rc=-1;

	fp = AbstractFileSystem_Open((char*)FONTTEMPLATE_FILENAME,HSA_READ_FOM);
	if (fp>=0)
	{
		AbstractFileSystem_Seek( fp, offset, HSA_OFFSET_FROM_START_FSS );
		if (size == AbstractFileSystem_Read( fp, (char*)data, size ))
		{
			rc = 0;
		}
	}
	AbstractFileSystem_Close( fp);

	return rc;
}

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
void drvLcd_GetDBCCharBMP(StrAttribute *AttriStr)
{//ȫ���ַ�

	U32 offset;
	U8 HighByte;
	U8 LowByte;
	FontTemplate_en GBK_FontTemplate = Template_In_Reserve;	
	
	HighByte = *AttriStr->pStr;
	LowByte  = *(AttriStr->pStr+1);

	if (AttriStr->Type==CHARACTER_UNICODE)
	{//unicode	
		
		offset = ((HighByte<<8) +  LowByte) * (AttriStr->BytesOccupy);			
	}
	else
	{//localcode������
		
		if((HighByte>=0xAA && HighByte<=0xAF) && ( LowByte>=0xA1))
		{	// �û��Զ����� (1)
			GBK_FontTemplate = Template_In_File;
			offset = ((HighByte-0xAA)*94 +(LowByte-0xA1))*AttriStr->BytesOccupy;
		}
		else if((HighByte>=0xF8 && HighByte<=0xFE) && ( LowByte>=0xA1))
		{	// �û��Զ����� (2)
			GBK_FontTemplate = Template_In_File;
			offset = ((HighByte-0xF8)*94 +(LowByte-0xA1)+564)*AttriStr->BytesOccupy;
		}
		else if((HighByte>=0xA1 && HighByte<=0xA7) && (LowByte>=0x40 && LowByte<0xA1))
		{	// GBK �û��Զ����� (3)
			GBK_FontTemplate = Template_In_File;
			offset = ((HighByte-0xA1)*97 + (LowByte-0x40)+564+658) * AttriStr->BytesOccupy;
		}
		else if((HighByte >=0x81) &&(LowByte>=0x20) && (LowByte<0x40))
		{	//˫�ֽڵ�����û�5�� Code in ��Ӣ�����Ͱ�
			GBK_FontTemplate = Template_In_File;
			offset = ((HighByte-0x81)*32 + (LowByte-0x20)+564+658+679) * AttriStr->BytesOccupy;
		}
		else if(HighByte >= 0x81 && HighByte <= 0xFE && LowByte>=0x40)
		{//���ֿ��в�����ģ
			offset = ((HighByte - 0x81)*0xC0 + LowByte - 0x40)*AttriStr->BytesOccupy;
		}
		else
		{//���ڱ����������ʾ���������ַ���ʾ
			offset = ((0xA3 - 0x81)*0xC0 + 0xBF - 0x40)*AttriStr->BytesOccupy;
		}		
	}	
	
	offset += AttriStr->LanguOffset;

	if (GBK_FontTemplate == Template_In_Reserve)
	{		
		HSA_AccessReservedArea((char *)AttriStr->pBuf,offset,AttriStr->BytesOccupy);
	}
	else
	{//��ָ���ļ��л����ģ
		if (GetFontTemplateFromeFile((char *)AttriStr->pBuf,offset,AttriStr->BytesOccupy) != 0)
		{//�ļ��򿪻��ȡʧ�ܣ����ȡȫ���ʺ�"��"
			
			offset = ((0xA3 - 0x81)*0xC0 + 0xBF - 0x40)*AttriStr->BytesOccupy + AttriStr->LanguOffset;
			
			HSA_AccessReservedArea((char *)AttriStr->pBuf,offset,AttriStr->BytesOccupy);
		}
	}
}

