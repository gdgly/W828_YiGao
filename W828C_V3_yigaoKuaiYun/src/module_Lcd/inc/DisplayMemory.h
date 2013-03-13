/*************************************************************
%
% Filename     : DisplayMemory.h
% Project name : by use of displaymemory, part of the LCD driver library of Hyctron
%
% Copyright 2003-2004 Hyctron Electronic Design House,
% "Hyctron Electronic Design House" Shanghai, China.
% All rights are reserved. Reproduction in whole or in part is prohibited
% without the prior written consent of the copyright owner.
%
% Rev   Author        Begin        Complete      Comments
%                    (yymmdd)     (yymmdd)
% -------------------------------------------------------------
% 001   HYCJB   	 060421                    Primary version
% -------------------------------------------------------------
%
%  ��Դ�ļ�������320*240����ʾ�������
****************************************************************/

#ifndef _DISPLAYMEMORY_H_ 
#define _DISPLAYMEMORY_H_

#include "HSA_API.h"
#include "LcdAPIFunc.h"
#include "AlphaDisplay.h"


#ifdef __cplusplus
extern "C"
{
#endif

//NandFlash��ԴͼƬ:
//ÿ����ԴͼƬ��Offset��High��Width

#define QUESIZE				15	//������󳤶�

typedef struct area_t
{
	U16 kRow; 		//��Ļ��ʾ��
	U16 kColumn;	//��Ļ��ʾ��
	U16 kHigh;		//��Ļ�߶�
	U16 kWidth;		//��Ļ���
}AREA_t;

typedef struct tqueue
{
	int length;				//���г���
	AREA_t data[QUESIZE]; 	//��������
}QUEUE_t;


typedef struct _RECT
{
	U16 y;      //��   �����������
	U16 x;      //��
	U16 h;      //��
	U16 w;      //��
}RECT;

typedef struct _POS
{
	U16 x;      //��   ��������λ��
	U16 y;      //��
}POS;


typedef struct _TEXTOUT
{
	U16 y;//��
	U16 x;//��
	U16 kDisplayFlag;//�������� | ��С���Ƿ�Ϊ�ļ�����
	U16 FrontColor;  //�����ǰ��ɫ
	U16 BackColor;   //����ı���ɫ
	U16 keyStrColor;	//�ؼ��ַ���ɫ
	U8  *pKeyStr;		//�ؼ��ַ���
	U8  *pkString;    //��ʾ�ַ�ָ��
	U8  EraseBack;    //��������ɫģʽ 0�������� 1��ֻ��ʹ��͸��ɫ 2��ʹ��BackColor��ɫ����
	                 //ע��:�����������СΪprect��ָ������
	RECT *prect; //���EraseBack��=1����Ҫˢ��rect����ĵ�ɫ	
	
}TEXTOUT;

typedef enum
{
	MEMORYBACKGROUND=0,
	MEMORYDISPLAY,
	MEMORYBUFFER,
	ALPHABACKGROUND,
	ALPHADISPLAY,
	ALPHABUFFER
}MemoryType_e;	  


//////////////////////////////////////////////////////////////////////////////////////////////////
/*-----------------------------------------------------------------------------
* ����:	�õ��ļ����������,�Լ���С
*----------------------------------------------------------------------------*/
U32 Get_Display_DisplayMemory_Offset(void);

U32 Get_Display_Buffer_Size(void);

U32 Get_Display_DisplayBuffer_Offset(void);

U32 Get_Display_BackGround_Offset(void);

/*-----------------------------------------------------------------------------
* ����:	GetSDRAMOffset
* ����:	����к�����SDRAM�еĵ�ַ
* ����:	gMemory-----����ͼ�洢�����Դ�
*		kRow--------��Ļ��ʾ��
*		kColumn-----��Ļ��ʾ��
* ����:	offset------SDRAM��ַ
*----------------------------------------------------------------------------*/
U32 GetSDRAMOffset(U16 kRow, U16 kColumn);

void Memory_SetRowColumn(U16 kRow, U16 kColumn, U16 kHeight, U16 kWidth);

/*-----------------------------------------------------------------------------
* ����:	drvLcd_IsUseQueue
* ����:	�Ƿ�ʹ�ö���
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_IsUseQueue(int type);

/*-----------------------------------------------------------------------------
* ����:	drvLcd_ClearQueue
* ����:	�����ʾ����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_ClearQueue(void);

/*-----------------------------------------------------------------------------
* ����:	drvLcd_GetMemoryOffset
* ����:	�õ�memory��offset
* ����: x,y memory������
* ����:	none
*----------------------------------------------------------------------------*/
char *drvLcd_GetMemoryOffset(U16 x, U16 y, U32 type);

/*-----------------------------------------------------------------------------
* ����:	drvLcd_Background2DisplayMemory
* ����:	��SDRAM����ͼ�����ж�ȡ��Ҫˢ��λ�õ����ݵ�SDRAM�Դ��ж�Ӧλ��
* ����: kRow------��Ļ��ʾ��
*		kColumn---��Ļ��ʾ��
*   	kHigh-----��Ļ�߶�
*       kWidth----��Ļ���
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_Background2DisplayMemory(U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn);

/*-----------------------------------------------------------------------------
* ����:	drvLcd_ClearDisplayMemory
* ����:	�����ʾ��������
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_ClearDisplayMemory(U16 kRow, U16 kColumn, U16 kHeight, U16 kWidth);

/*-----------------------------------------------------------------------------
* ����:	drvLcd_Memory2Memory
* ����:	��NandFlash�ж�ȡ�����ϵ�ͼƬд��ָ����SDRAM�Դ�
* ����:	BMPOffsetInNand---����ͼƬ��NandFlash��ԴͼƬ�е�ƫ�Ƶ�ַ
*		kBMPHigh----------ͼƬ�߶�
*       kBMPWidth---------ͼƬ���
*       kRow--------------��Ļ��ʾ��
*		kColumn-----------��Ļ��ʾ��
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_Memory2Memory(char *pSrc, char *pDest,U16 kBMPHigh, U16 kBMPWidth, U16 kRow, U16 kColumn);

/*-----------------------------------------------------------------------------
* ����:	drvLcd_Memory2DisplayMemory
* ����:	��SDRAM���������ж�ȡ���ݵ�SDRAM�Դ���ָ��λ��
* ����:	pBuf------����Դ��ַ
*		kRow------��Ļ��ʾ��
*		kColumn---��Ļ��ʾ��
*   	kHigh-----��Ļ�߶�
*       kWidth----��Ļ���
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_Memory2DisplayMemory(void *pBuf, U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn);


/*-----------------------------------------------------------------------------
* ����:	drvLcd_Memory2DisplayMemoryClarity
* ����:	��SDRAM���������ж�ȡ���ݵ�SDRAM�Դ���ָ��λ��
* ����:	pBuf------����Դ��ַ
*		kRow------��Ļ��ʾ��
*		kColumn---��Ļ��ʾ��
*   	kHigh-----��Ļ�߶�
*       kWidth----��Ļ���
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_Memory2DisplayMemoryClarity(void *pBuf, U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn, U16 startH, U16 startW, U16 filterColor);

/*-----------------------------------------------------------------------------
* ����:	Set_DispMemory2Queue
* ����:	��ָ�����Դ�������
* ����:	 h w y x :��\��\��\��
* ����:	none
*----------------------------------------------------------------------------*/
void Set_DispMemory2Queue(U16 h, U16 w, U16 y, U16 x);

/*-----------------------------------------------------------------------------
* ����:	drvLcd_DispString
* ����:	��ʾ�ַ���
* ����:	pos----��ʾλ��
*		kDisplayFlag--��ʾ��ʽ
*		*pkString-----����ʾ���ַ���
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_DispString(TEXTOUT *text);

/*-----------------------------------------------------------------------------
* ����:	DisplayData2Screen
* ����:	���Դ���������ʾ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void DisplayData2Screen(void);

/*-----------------------------------------------------------------------------
* ����:	drvLcd_SetDisplayMemoryArea
* ����:	����SDRAM�Դ��ж�Ӧλ�õ���Ϣ
* ����: kRow------��Ļ��ʾ��
*		kColumn---��Ļ��ʾ��
*   	kHigh-----��Ļ�߶�
*       kWidth----��Ļ���
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_GetDisplayMemoryArea(U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn);

/*-----------------------------------------------------------------------------
* ����:	drvLcd_SetDisplayArea
* ����:	������ʾ��ַ
* ����: DisplayOffset------��ʾ
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_SetDisplayAddress(U32 DisplayOffset);

/*-----------------------------------------------------------------------------
* ����:	drvLcd_Rectangle
* ����:	������
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_Rectangle(U16 kRow, U16 kColumn, U16 kHeight, U16 kWidth, U16 FrameWidth);

/*-----------------------------------------------------------------------------
* ����:	BMPFromSDRAMDisplayMemory2Screen
* ����:	SDRAM�Դ����������ݣ�������ֱ��������ʾ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_BMPFromDisplayMemory2Screen(void);

/*-----------------------------------------------------------------------------
* ����:	drvLcd_BMPFromDisplayBuffer2Screen
* ����:	SDRAM�������������ݣ�������ֱ��������ʾ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_BMPFromDisplayBuffer2Screen(void);

/*-----------------------------------------------------------------------------
* ����:	WritePoint2SDRAM
* ����:	дһ����ɫ�ĵ㵽SDRAM���������桢�Դ桢BUffer���п��ܣ�
* ����:	u16Data-----16λ��ɫ����
* ����:	none
*----------------------------------------------------------------------------*/
void WritePoint2SDRAM(U16 u16Data);

/*-----------------------------------------------------------------------------
* ����:	drvLcd_AddDispRect
* ����:	����ʾ���������У��Ա��ܹ���������
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void  drvLcd_AddDispRect(U16 y, U16 x, U16 h, U16 w);
#ifdef __cplusplus
}
#endif

#endif 	//_DISPLAYMEMORY_H_

