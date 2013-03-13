
/*************************************************************
%
% Filename     : DisplayMemory.c
% Project name : by use of displaymemory, part of the LCD driver library of Hyctron
%
% Copyright 2003 Hyctron Electronic Design House,
% "Hyctron Electronic Design House" Shanghai, China.
% All rights are reserved. Reproduction in whole or in part is prohibited
% without the prior written consent of the copyright owner.
%
% Rev   Date    Author          Comments
%      (yymmdd)
% -------------------------------------------------------------
% 001   060424  HYCJB		Primary version
% -------------------------------------------------------------
%
%  This source file contains low function of the Lcd Library test test you are the best
%
****************************************************************/

#include "HSA_API.h"
#include "string.h"

#include "glbVariable_base.h"
#include "LcdAPIFunc.h"
#include "LcdCPU.h"
#include "DisplayMemory.h"

#include "AbstractFileSystem.h"
#include "grap_api.h"
#include "resManage.h"

/*--------------------------------------------------------------------------
*DEFINES
--------------------------------------------------------------------------*/
#define TRANSFERDATA_USE_ASSEMBLY_LANGUAGE		//ʹ�û��ָ������ݵ���ʾ��


static MemoryType_e sRWMemoryType = MEMORYDISPLAY;	//����ͼ�洢�����Դ桢����洢��

static U16 sRow = 0;
static U16 sColumn = 0;
static U16 sHeight = 0;
static U16 sWidth = 0;
static U16 sTempColumn = 0;

static QUEUE_t sque;   

static U8  sLcd_DmaRefresh = 0;//0��lcd 1:dma


/*++++++++++++++++++++++++++++++++++++++++++���Ͷ���++++++++++++++++++++++++++++++++++++++++++++++*/
/*-----------------------------------------------------------------------------
* ����:	EnQueue
* ����:	���Ӷ���Ԫ��,ˢ�½���
* ����: que-----����
* ����:	TRUE----�ɹ�	FALSE---ʧ��
*----------------------------------------------------------------------------*/
static void EnQueue(QUEUE_t *que)
{
	int i,length;
	AREA_t *pArea;
	int row,col,endRow,endCol;
	
	if(sHeight*sWidth >= gLcd_Total_Row*gLcd_Total_Column || que->length >= QUESIZE)
	{
		gRefreshNeedQue = FALSE;
		que->length = 0;
		return;
	}
	
	if(que->length < 0)
	{
		que->length = 0;
	}

	length = que->length;
	for (i=0; i<length; i++)
	{
	     pArea = &que->data[i];
	     
	     row = pArea->kRow;
	     col = pArea->kColumn;
	     endRow = pArea->kRow+pArea->kHigh;
	     endCol = pArea->kColumn+pArea->kWidth;
	     
	     if (sRow >= row && sColumn>= col && sRow+sHeight <= endRow && sColumn+sWidth <= endCol)
	     {
	         return;
	     }
	     else if (sRow <=row && sColumn <= col && sRow+sHeight >= endRow && sColumn+sWidth >= endCol)
	     {
	         pArea->kRow = sRow;
	         pArea->kColumn = sColumn;
	         pArea->kHigh = sHeight;
	         pArea->kWidth = sWidth;
	         return;
	     }
	}

	que->data[que->length].kRow = sRow;
	que->data[que->length].kColumn = sColumn;
	que->data[que->length].kHigh = sHeight;
	que->data[que->length].kWidth = sWidth;

	que->length++;
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_GetQueList
* ����:	�����������ָ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
QUEUE_t *drvLcd_GetQueList(void)
{
    return &sque;
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_IsUseQueue
* ����:	�Ƿ�ʹ�ö���
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_IsUseQueue(int type)
{
	if (1 == type)
	{
		gRefreshNeedQue = TRUE;
	}
	else
	{
		gRefreshNeedQue = FALSE;
	}
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_ClearQueue
* ����:	�����ʾ����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_ClearQueue(void)
{
	sque.length = 0;
}

/*++++++++++++++++++++++++++++++++++++++++++�ڴ�++++++++++++++++++++++++++++++++++++++++++++++*/
/*-----------------------------------------------------------------------------
* ����:	�õ��ļ����������,�Լ���С
*----------------------------------------------------------------------------*/
U32 Get_Display_Buffer_Size(void)
{
	return ((U32)hsaDisplayMemoryPool_GetBufferSize()/6);
}

/*
* ��ȡ��ǰ����������ʾ���ǿ�RGBbuf�ĵ�ַ
*/
U32 Get_Display_RgbBuf_Offset()//kong rgb  ��ʾ�����õ�ַ
{
	U32 addess;

	addess = (U32)hsaDisplayMemoryPool_GetBuffer();
	
	return (U32)hsaConvertUnCacheMemory((char *)addess);
}

U32 Get_Display_RgbReversal_Offset(void)//����rgb����
{
	return ((U32)hsaDisplayMemoryPool_GetBuffer()+Get_Display_Buffer_Size());
}

U32 Get_Display_DisplayMemory_Offset(void)//�ϲ������buf
{
	return ((U32)hsaDisplayMemoryPool_GetBuffer()+Get_Display_Buffer_Size()*2);
}

U32 Get_Display_DisplayBuffer_Offset(void)
{
	return ((U32)hsaDisplayMemoryPool_GetBuffer()+Get_Display_Buffer_Size()*3);
}

U32 Get_Display_BackGround_Offset(void)
{
    return ((U32)hsaDisplayMemoryPool_GetBuffer()+Get_Display_Buffer_Size()*4);
}

U32 Get_Uncompress_Buffer_Offset(void)
{
    return ((U32)hsaDisplayMemoryPool_GetBuffer()+Get_Display_Buffer_Size()*5);
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_GetMemoryOffset
* ����:	�õ�memory��offset
* ����: x,y memory������
* ����:	none
*----------------------------------------------------------------------------*/
char *drvLcd_GetMemoryOffset(U16 x, U16 y, U32 type)
{
	char *pOff;

	switch(type)
	{
	case MEMORYBACKGROUND:        
		pOff = (char *)((Get_Display_BackGround_Offset() + (y*gLcd_Total_Column+x)*c1POINT_OCCUPY_BYTES));
		break;
	case MEMORYDISPLAY:
		pOff = (char *)((Get_Display_DisplayMemory_Offset() + (y*gLcd_Total_Column+x)*c1POINT_OCCUPY_BYTES));
		break;
	case MEMORYBUFFER:
		pOff = (char *)((Get_Display_DisplayBuffer_Offset() + (y*gLcd_Total_Column+x)*c1POINT_OCCUPY_BYTES));
		break;
	default:
		pOff = NULL;
		break;             
	}

	return pOff;
}

/*-----------------------------------------------------------------------------
* ����:	GetSDRAMOffset
* ����:	����к�����SDRAM�еĵ�ַ
* ����:	gMemory-----����ͼ�洢�����Դ�
*		kRow--------��Ļ��ʾ��
*		kColumn-----��Ļ��ʾ��
* ����:	offset------SDRAM��ַ
*----------------------------------------------------------------------------*/
U32 GetSDRAMOffset(U16 kRow, U16 kColumn)
{
	U32 offset;

	if (sRWMemoryType == MEMORYBACKGROUND)
	{
		offset = (U32)(Get_Display_BackGround_Offset() + (kRow*gLcd_Total_Column+kColumn)*c1POINT_OCCUPY_BYTES);
	}
	else if (sRWMemoryType == MEMORYDISPLAY)
	{
		offset = (U32)(Get_Display_DisplayMemory_Offset() + (kRow*gLcd_Total_Column+kColumn)*c1POINT_OCCUPY_BYTES);
	}
	else if(sRWMemoryType == MEMORYBUFFER)
	{
		offset = (U32)(Get_Display_DisplayBuffer_Offset() + (kRow*sWidth+kColumn)*c1POINT_OCCUPY_BYTES);
	}
	else if (sRWMemoryType == ALPHABACKGROUND)
	{
	    offset = (U32)(ALPHA_MEMORY_OFFSET + (kRow*gLcd_Total_Column+kColumn)*c1POINT_OCCUPY_BYTES); 
	}
	else if (sRWMemoryType == ALPHADISPLAY)
	{
	    offset = (U32)(ALPHA_MEMORY_OFFSET+ALPHA_BLOCK_SIZE + (kRow*gLcd_Total_Column+kColumn)*c1POINT_OCCUPY_BYTES); 
	}
	else if (sRWMemoryType == ALPHABUFFER)
	{
	    offset = (U32)(ALPHA_MEMORY_OFFSET+2*ALPHA_BLOCK_SIZE + (kRow*gLcd_Total_Column+kColumn)*c1POINT_OCCUPY_BYTES);
	}

	return offset;
}


/*-----------------------------------------------------------------------------
* ����:	drvLcd_SetDisplayMemoryArea
* ����:	����SDRAM�Դ��ж�Ӧλ�õ���Ϣ
* ����: kRow------��Ļ��ʾ��
*		kColumn---��Ļ��ʾ��
*   	kHigh-----��Ļ�߶�
*       kWidth----��Ļ���
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_GetDisplayMemoryArea(U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn)
{
	if(kRow > gLcd_Total_Row || kRow+kHigh > gLcd_Total_Row 
	   || kColumn > gLcd_Total_Column || kColumn+kWidth > gLcd_Total_Column)
	{
		return;
	} 
	
	Memory_SetRowColumn(kRow, kColumn, kHigh, kWidth);
	if(gRefreshNeedQue)	EnQueue(&sque);
	sRWMemoryType = MEMORYDISPLAY;
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_SetDisplayArea
* ����:	������ʾ��ַ
* ����: DisplayOffset------��ʾ
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_SetDisplayAddress(U32 DisplayOffset)
{	
	if(DisplayOffset==Get_Display_BackGround_Offset())
	{
		sRWMemoryType = MEMORYBACKGROUND;
	}
	else if(DisplayOffset==Get_Display_DisplayMemory_Offset())
	{
		sRWMemoryType = MEMORYDISPLAY;
	}
	else if(DisplayOffset==Get_Display_DisplayBuffer_Offset())
	{
		sRWMemoryType = MEMORYBUFFER;
	}	
}

/******************************************************
* ����: hyhwRgb_GetAddressOffset
* ����: ����к�����RGBbuf�еĵ�ַ
* ����: kRow:��    kColumn:��
* ����: ��ַ
*******************************************************/
U32 hyhwRgb_GetAddressOffset(U16 kRow, U16 kColumn)
{
	U32 offset;

	offset = (U32)(Get_Display_RgbBuf_Offset() + (kRow*gLcd_Total_Row+kColumn)*c1POINT_OCCUPY_BYTES);	
	
	return offset;
}

/***********************************
* ����:hyhwRgb_AdjustBuf
* ����:������ʾbuf �����ת������
* ����:pSrc:Դ  pDst:Ŀ��  flag:0:pSrc�е�����Ϊ����������(���ǰ���Ļλ�ô��)  1:��Ӧ��Ļλ�ô�ŵ�����
* ����:none
***********************************/
void hyhwRgb_AdjustBuf(U16 *pSrc, U16 *pDst, U16 kRow, U16 kCol, U16 kHight, U16 kWidth, U8 flag)
{
	int i,j,displaymode;
	U16 endRow,endCol,dataWidth;
	
	if(flag == 0)
	{
		dataWidth = kWidth;
	}
	else
	{
		dataWidth = gLcd_Total_Column;
	}
	displaymode = 0;//LcdModule_Get_ShowType();

	endRow = kRow+kHight;
	endCol = kCol+kWidth;
	if(displaymode == 0)//����
	{
		for(j = endCol-1; j >= kCol; j--)
		{
			for(i = kRow; i < endRow; i++)
			{
				*pDst++ = pSrc[i*dataWidth+j];
			}
		}
	}
	else //��ԭʼˢ������һ�� ���Բ��õ���
	{
		memcpy((U8 *)pDst, (U8 *)pSrc, gLcd_Total_Row*gLcd_Total_Column*c1POINT_OCCUPY_BYTES);
	}
	
	return ;
}

/*++++++++++++++++++++++++++++++++++++++����++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*-----------------------------------------------------------------------------
* ����:	drvLcd_SetRowColumn
* ����:	���ò�ɫ��ʾ������ʾ��ʼ��ַ,��ַΪ����ź������row��Column,�Լ��߶ȺͿ��
* ����:	kRow------����� kColumn---�����  kHeight---�߶�  kWidth----���
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_SetRowColumn(U16 kRow,U16 kColumn,U16 kHeight,U16 kWidth)
{
	sRow = kRow;
	sColumn = kColumn;
	sTempColumn = 0;
}

void Memory_SetRowColumn(U16 kRow, U16 kColumn, U16 kHeight, U16 kWidth)
{
	sRow = kRow;
	sColumn = kColumn;
	sHeight = kHeight;
	sWidth = kWidth;
	sTempColumn = 0;

	if(kHeight == gLcd_Total_Row && kWidth == gLcd_Total_Column)
	{
		sRWMemoryType = MEMORYDISPLAY;
	}
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_DMARefreshEnable
* ����:	ʹ��DMA����ʱ����������ӵ�
* ����:	1������DMA���� 0:cpu ����
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_DMARefreshEnable(U32 flag)
{
    if (1 == flag)   sLcd_DmaRefresh = 1;
    else sLcd_DmaRefresh = 0;   
}


void Set_DispMemory2Queue(U16 h, U16 w, U16 y, U16 x)
{
	sRWMemoryType = MEMORYDISPLAY;
	Memory_SetRowColumn(y, x, h, w);	
	if(gRefreshNeedQue)	EnQueue(&sque);
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_AddDispRect
* ����:	����ʾ���������У��Ա��ܹ���������
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void  drvLcd_AddDispRect(U16 y, U16 x, U16 h, U16 w)
{
	Memory_SetRowColumn(y, x, h, w);
	
	if(gRefreshNeedQue)	EnQueue(&sque);
}	


/*-----------------------------------------------------------------------------
* ����:	alpha_set_mem_type
* ����:	���û�ͼ��buf����
* ����:	type��buf����
* ����:	none
*----------------------------------------------------------------------------*/
void alpha_set_mem_type(U32 type)
{
     if (type >ALPHABUFFER) return;
          
     sRWMemoryType = type;
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_ClearBox
* ����:	���ָ�����򣬲���ʱ���Ա������
* ����:	kPage----ҳ��ţ�����ʱΪ����ţ�
*		kColumn--�����
*		kHeight--�߶ȣ���ɫ��ʱΪҳ�߶ȣ�����ʱΪ�и߶ȣ�
*		kWidth---�п��
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_ClearBox(U16 kPage, U16 kColumn, U16 kHeight, U16 kWidth)
{
	U16 i, j;
	U16 kRow;
	U32 EndRow,EndCol;

	kRow = kPage;
	
	EndRow = kHeight + kRow;
	EndCol = kColumn+kWidth;

	if(kRow > gLcd_Total_Row || EndRow > gLcd_Total_Row 
	   || kColumn > gLcd_Total_Column || EndCol > gLcd_Total_Column)
	{
		return;
	}   

	for(i=kRow;i<EndRow;i++)
	{
		for(j=kColumn;j<EndCol;j++)
		{
			drvLcd_SetRowColumn(i,j,1,kWidth);
			drvLcd_WritePoint(gBackColor);
		}
	}
	
	Memory_SetRowColumn(kRow, kColumn, kHeight, kWidth);
	
	if(gRefreshNeedQue)	EnQueue(&sque);
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_ClearBox
* ����:	��ָ������ɫ��ͼ
* ����:	kPage----ҳ��ţ�����ʱΪ����ţ�
*		kColumn--�����
*		kHeight--�߶ȣ���ɫ��ʱΪҳ�߶ȣ�����ʱΪ�и߶ȣ�
*		kWidth---�п��
*       pcolor---��ɫ����
*       ע�⣺��ɫ����Ĵ�Сһ��Ҫ>=kWidth����������������øú�����С��ʹ��
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_ClearBoxUseColor(U16 kPage, U16 kColumn, U16 kHeight, U16 kWidth, const U16 *pcolor)
{
	U16 i, j,k;
	U16 kRow;
	U32 EndRow,EndCol;

	kRow = kPage;
	
	EndRow = kHeight + kRow;
	EndCol = kColumn+kWidth;
	
	if(kRow > gLcd_Total_Row || EndRow > gLcd_Total_Row 
	   || kColumn > gLcd_Total_Column || EndCol > gLcd_Total_Column)
	{
		return;
	}   
	
	for(i=kRow;i<EndRow;i++)
	{
		for(k=0,j=kColumn;j<EndCol;j++,k++)
		{
			drvLcd_SetRowColumn(i,j,1,kWidth);
			drvLcd_WritePoint(pcolor[k]);
		}
	}
	
	Memory_SetRowColumn(kRow, kColumn, kHeight, kWidth);
	
	if(gRefreshNeedQue)	EnQueue(&sque);
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_DrawRectUseColor
* ����:	��ָ������ɫ��ͼ
* ����:	pRect����ͼ�ķ�Χ  pcolor����ɫָ�� mode 0��ˮƽ�� 1:��ֱ��
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_DrawRectUseColor(RECT *pRect, const U16 *pcolor, U32 mode)
{
	U16 i, j,k;
	U16 h,w,r,c;
	
	h = pRect->h;
	w = pRect->w;
	r = pRect->y;
	c = pRect->x;
	
	if(r+h > gLcd_Total_Row || c+w > gLcd_Total_Column)
	{
		return;
	}
	
	if (1 == mode)
	{
	    for (k=0,i=r; i<r+h; i++,k++)
	    {
	        for (j=c; j<c+w; j++)
	        {
	            drvLcd_SetRowColumn(i,j,1,1);
			    drvLcd_WritePoint(pcolor[k]);    
	        }
	    }    
	}
	else
	{
	    for (i=r; i<r+h; i++)
	    {
	        for (k=0,j=c; j<c+w; j++,k++)
	        {
	            drvLcd_SetRowColumn(i,j,1,1);
			    drvLcd_WritePoint(pcolor[k]);    
	        }
	    }
	}   
	
	Memory_SetRowColumn(r,c,h,w);
	
	if(gRefreshNeedQue)	EnQueue(&sque);
}

/*-----------------------------------------------------------------------------
* ����:	WritePoint2SDRAM
* ����:	дһ����ɫ�ĵ㵽SDRAM
* ����:	u16Data-----16λ��ɫ����
* ����:	none
*----------------------------------------------------------------------------*/
void WritePoint2SDRAM(U16 u16Data)
{
	U16 *offset;

	offset = (U16 *)GetSDRAMOffset(sRow, (U16)(sColumn+sTempColumn));
	*offset = (U16)u16Data;

	if (sTempColumn>=sWidth-1)
	{
		sRow++;
		sTempColumn = 0;
	}
	else
	{
		sTempColumn++;
	}
}

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
void drvLcd_Memory2Memory(char *pSrc, char *pDest,U16 kBMPHigh, U16 kBMPWidth, U16 kRow, U16 kColumn)
{
    U16 i;
	U32 realWid;

    realWid = kBMPWidth<<1;
   
	for(i=0; i<kBMPHigh; i++)
	{
		memcpy(pDest+(((i+kRow)*gLcd_Total_Column+kColumn)<<1),pSrc,realWid );
		pSrc += realWid;
	}	
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_BMPFromDisplayMemory2Screen
* ����:	SDRAM�Դ�����DMA��ʽֱ��������ʾ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_BMPFromDisplayMemory2Screen(void)
{	
	U32 size; 
	int i;
	U16 data;
	U16 *pu16Sdram;
	U16 *pRgb,*pTemp;

	drvLcd_SetRowColumn_Lcd(0, 0, gLcd_Total_Row, gLcd_Total_Column);

	size = gLcd_Total_Row*gLcd_Total_Column;
	pu16Sdram = (U16 *)Get_Display_DisplayMemory_Offset(); 
	
	
#ifdef TRANSFERDATA_USE_ASSEMBLY_LANGUAGE
	__asm
	{
		mov		r0, size
		mov		r1, #LCD_DATA_ADDRESS
		mov		r2, pu16Sdram
		
	Lcd_loop:
		ldrh	r3, [r2], #2
		subs	r0, r0, #1
		strh	r3, [r1]
		bne		Lcd_loop
	}
#else
	for (i = 0;i < size;i++)
	{
		drvLcd_WriteData(*p16Sdram++);
	}
#endif
	
	return ;
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_Background2DisplayMemory
* ����:	��SDRAM����ͼ�����ж�ȡ��Ҫˢ��λ�õ����ݵ�SDRAM�Դ��ж�Ӧλ��
* ����: kRow------��Ļ��ʾ��
*		kColumn---��Ļ��ʾ��
*   	kHigh-----��Ļ�߶�
*       kWidth----��Ļ���
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_Background2DisplayMemory(U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn)
{
	U16	*offsetbg, *offsetDisplay;
	int i;

	if(kHigh==gLcd_Total_Row && kWidth==gLcd_Total_Column)
	{
		gRefreshNeedQue = FALSE;
	}
	
	if((kColumn+kWidth)>gLcd_Total_Column)
	{
		kWidth=gLcd_Total_Column-kColumn;
	}
	
	if((kRow+kHigh)>gLcd_Total_Row)
	{
		kHigh=gLcd_Total_Row-kRow;
	}
	
	for(i=0; i<kHigh; i++)
	{
		sRWMemoryType = MEMORYBACKGROUND;
		offsetbg = (U16 *)GetSDRAMOffset((U16)(kRow+i), kColumn);

		sRWMemoryType = MEMORYDISPLAY;
		offsetDisplay = (U16 *)GetSDRAMOffset((U16)(kRow+i), kColumn);

		memcpy((char *)offsetDisplay, (char *)offsetbg, kWidth*c1POINT_OCCUPY_BYTES);
		offsetDisplay += kWidth;
		offsetbg += kWidth;
	}

	Memory_SetRowColumn(kRow, kColumn, kHigh, kWidth);

	if(gRefreshNeedQue)	EnQueue(&sque);
}

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
void drvLcd_Memory2DisplayMemory(void *pBuf, U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn)
{
	U16	*offsetBuffer, *offsetDisplay;
	int i;
	U32 cpySize;


	offsetBuffer = (U16 *)pBuf;
	if(kHigh==gLcd_Total_Row && kWidth==gLcd_Total_Column)
	{
		gRefreshNeedQue = FALSE;
	}
	
	if((kColumn+kWidth)>gLcd_Total_Column)
	{
		kWidth=gLcd_Total_Column-kColumn;
	}
	
	if((kRow+kHigh)>gLcd_Total_Row)
	{
		kHigh=gLcd_Total_Row-kRow;
	}
	
	cpySize = kWidth*c1POINT_OCCUPY_BYTES;
	sRWMemoryType = MEMORYDISPLAY;
	
	for(i=0; i<kHigh; i++)
	{
		offsetDisplay = (U16 *)GetSDRAMOffset((U16)(kRow+i), kColumn);

		memcpy((char *)offsetDisplay, (char *)offsetBuffer, cpySize);
		offsetBuffer += kWidth;
	}

	Memory_SetRowColumn(kRow, kColumn, kHigh, kWidth);

	if(gRefreshNeedQue)	EnQueue(&sque);
}


/*-----------------------------------------------------------------------------
* ����:	drvLcd_PartMemory2DisplayMemory
* ����:	��SDRAM���������ж�ȡ�������ݵ�SDRAM�Դ���ָ��λ��
* ����:	pBuf------����Դ��ַ
*		klcdRow------�����ϵ���ʾλ��
*		klcdColumn
*   	kfactHigh-----����ʵ�ʸ߶�
*       kfactWidth----����ʵ�ʿ��

*		kdisRow------�Ӹ��п�ʼȡ����--����ڸ�����
*		kdisColumn---�Ӹ��п�ʼȡ����
*   	kdisHigh-----����Ӧȡ���ݵĸ�
*       kdisWidth----����Ӧȡ���ݵĿ�
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_PartMemory2DisplayMemory(void *pBuf, U16 kfactHigh, U16 kfactWidth, U16 klcdRow, U16 klcdColumn,
									U16 kdisHigh, U16 kdisWidth, U16 kdisRow, U16 kdisColumn)
{
	U16	*offsetBuffer, *offsetDisplay;
	int i, offset_y;

	offsetBuffer = (U16 *)pBuf;
	
	if(kdisHigh == 0 || kdisWidth == 0)
		return ;
	if(kdisHigh==gLcd_Total_Row && kdisWidth==gLcd_Total_Column)
	{
		gRefreshNeedQue = FALSE;
	}
	
	if((klcdColumn+kdisWidth)>gLcd_Total_Column)
	{
		kdisWidth=gLcd_Total_Column-klcdColumn;
	}
	
	if((klcdRow+kdisHigh)>gLcd_Total_Row)
	{
		kdisHigh=gLcd_Total_Row-klcdRow;
	}
	
	offsetBuffer += kdisRow*kfactWidth;
	for(i=0; i<kdisHigh; i++)
	{
		sRWMemoryType = MEMORYDISPLAY;
		offsetDisplay = (U16 *)GetSDRAMOffset((U16)(klcdRow+i), klcdColumn);

		memcpy((char *)offsetDisplay, (char *)(offsetBuffer+kdisColumn), kdisWidth*c1POINT_OCCUPY_BYTES);
		offsetBuffer += kfactWidth;
	}

	Memory_SetRowColumn(klcdRow, klcdColumn, kdisHigh, kdisWidth);

	if(gRefreshNeedQue)	EnQueue(&sque);
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_InDisplayMemoryMove
* ����:	��SDRAM���ƶ�����
* ����: Offset_dest-----Ŀ�ĵ�ַ
*		Offset_src------Դ��ַ
*		size------------���ݴ�С
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_InDisplayMemoryMove(U32 Offset_dest, U32 Offset_src, U32 size)
{
	memcpy((char*)(Get_Display_DisplayMemory_Offset()+Offset_dest), (char*)(Get_Display_DisplayMemory_Offset()+Offset_src), size);
	sRWMemoryType = MEMORYDISPLAY;
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_JPGBackground2DisplayMemory
* ����:	��SDRAM����ͼ�����ж�ȡ��Ҫˢ�µ����ݵ�SDRAM�Դ��е�λ��
* ����: kRow------��Ļ��ʾ��
*		kColumn---��Ļ��ʾ��
*   	kHigh-----��Ļ�߶�
*       kWidth----��Ļ���
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_JPGBackground2DisplayMemory(U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn)
{
	U16	*offsetbg, *offsetDisplay;
	int i;

	if(kHigh==gLcd_Total_Row && kWidth==gLcd_Total_Column)
	{
		gRefreshNeedQue = FALSE;
	}
	
	offsetbg=(U16 *)Get_Display_BackGround_Offset();
	
	for(i=0; i<kHigh; i++)
	{
		sRWMemoryType = MEMORYDISPLAY;
		offsetDisplay = (U16 *)GetSDRAMOffset((U16)(kRow+i), kColumn);

		memcpy((char *)offsetDisplay, (char *)offsetbg, kWidth*c1POINT_OCCUPY_BYTES);
		offsetDisplay += kWidth;
		offsetbg += kWidth;
	}

	Memory_SetRowColumn(kRow, kColumn, kHigh, kWidth);

	if(gRefreshNeedQue)	EnQueue(&sque);
}


/*-----------------------------------------------------------------------------
* ����:	drvLcd_DisplayMemory2DisplayBuffer
* ����:	����ˢ�µ����ݴ�SDRAM�Դ浽SDRAM���ݻ�����
* ����: kRow------��Ļ��ʾ��
*		kColumn---��Ļ��ʾ��
*   	kHigh-----��Ļ�߶�
*       kWidth----��Ļ���
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_DisplayMemory2DisplayBuffer(U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn)
{
	U16 *offsetDisplay, *offsetBuffer;
	int i;

	if(kHigh*kWidth > Get_Display_Buffer_Size())	//����BUFFER����ʾһ����
	{
		kHigh=Get_Display_Buffer_Size()/(kWidth*c1POINT_OCCUPY_BYTES);
	}

	sRWMemoryType = MEMORYBUFFER;
	offsetBuffer = (U16 *)GetSDRAMOffset(0, 0);

	for(i=0; i<kHigh; i++)
	{
		sRWMemoryType = MEMORYDISPLAY;
		offsetDisplay = (U16 *)GetSDRAMOffset((U16)(kRow+i), kColumn);

		memcpy((char *)offsetBuffer, (char *)offsetDisplay, kWidth*c1POINT_OCCUPY_BYTES);
		offsetBuffer += kWidth;
		offsetDisplay += kWidth;
	}

	sRWMemoryType = MEMORYBUFFER;
	Memory_SetRowColumn(kRow, kColumn, kHigh, kWidth);
	
	if(1 == gu8EnableRefreshSrc)
	{
		drvLcd_SetRowColumn_Lcd(kRow, kColumn, kHigh, kWidth);
	}
}


/*-----------------------------------------------------------------------------
* ����:	drvLcd_BMPFromDisplayBuffer2Screen
* ����:	SDRAM��������DMA��ʽֱ��������ʾ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_BMPFromDisplayBuffer2Screen(void)
{
	U16* pu16Sdram,*pTemp;
	U32  u16size,i;

    if (1 != gu8EnableRefreshSrc) return;
    
#ifdef TRANSFERDATA_USE_ASSEMBLY_LANGUAGE
	//ʹ��16bit������
	u16size = sHeight*sWidth;//16bit size
	if(u16size == 0) return; //��
	pu16Sdram = (U16*)Get_Display_DisplayBuffer_Offset();
	__asm
	{
		mov		r0, u16size
		mov		r1, #LCD_DATA_ADDRESS
		mov		r2, pu16Sdram
		
	sendLcd_loop:
		ldrh	r3, [r2], #2
		subs	r0, r0, #1
		strh	r3, [r1]
		bne		sendLcd_loop
	}
#else
	u16size = sHeight*sWidth;//16bit size
	if(u16size == 0) return; //��
	pu16Sdram = (U16*)Get_Display_DisplayBuffer_Offset();
	for (i = 0;i < u16size;i++)
	{
		drvLcd_WriteData(*pu16Sdram++);
	}
#endif
}


/*-----------------------------------------------------------------------------
* ����:	drvLcd_DisplayMemory2DisplayBufferAndScreen
* ����:	���Ҷ��У��Ƿ���ڲ���ˢ�µ����ݣ�
*		�����ڲ���ˢ�µ����ݣ���SDRAM�Դ��в���ˢ�µ����ݵ�SDRAM���ݻ����У�
*		��ɺ�SDRAM��������DMA��ʽ����ʾ���Ķ�Ӧλ��
*		ֱ������Ϊ��
* ����: que-----����
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_DisplayMemory2DisplayBufferAndScreen(QUEUE_t *que)
{
	int i;
	U32 hig,wid,row,col;

	if(que->length == 0)
		return;
		
	if (1 == sLcd_DmaRefresh)
	{//DMA ����
	    
	    while(lcdCpuDmaOK() != TRUE) syssleep(1);
        drvLcd_SetRowColumn_Lcd(0,0,gLcd_Total_Row,gLcd_Total_Column);
	    lcdCpuEnable((char *)Get_Display_DisplayMemory_Offset(), gLcd_Total_Row*gLcd_Total_Column);
	    
	    que->length = 0;	    
	    return;    
	} 	

	for	(i=0; i<que->length; i++)
	{
		hig = que->data[i].kHigh;
		wid = que->data[i].kWidth;
		row = que->data[i].kRow;
		col = que->data[i].kColumn;
				
		if ( hig*wid == 0) continue;
		
		drvLcd_DisplayMemory2DisplayBuffer(hig, wid, row, col);
		drvLcd_BMPFromDisplayBuffer2Screen();
	}

	que->length = 0;
}


/*-----------------------------------------------------------------------------
* ����:	drvLcd_ClearDisplayMemory
* ����:	�����ʾ��������
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_ClearDisplayMemory(U16 kRow, U16 kColumn, U16 kHeight, U16 kWidth)
{
	U16 i,j;

	if(kHeight==gLcd_Total_Row && kWidth==gLcd_Total_Column)
	{
		gRefreshNeedQue = FALSE;
	}

	if(gRefreshNeedQue)	EnQueue(&sque);

	sRWMemoryType = MEMORYDISPLAY;
	Memory_SetRowColumn(kRow, kColumn, kHeight, kWidth);

	for(i=0;i<kHeight;i++)
	{
		for(j=0;j<kWidth;j++)
		{
			drvLcd_SetRowColumn(kRow+i,kColumn+j,1,1);
			drvLcd_WritePoint(gFrontColor);
		}
	}
}    


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
void drvLcd_Memory2DisplayMemoryClarity(void *pBuf, U16 kHigh, U16 kWidth, U16 kRow, U16 kColumn, U16 startH, U16 startW, U16 filterColor)
{
	U16	*offsetBuffer, *offsetDisplay;
	U16 dispColor;
	int i,j;

	offsetBuffer = (U16 *)pBuf;
	if(kHigh==gLcd_Total_Row && kWidth==gLcd_Total_Column)
	{
		gRefreshNeedQue = FALSE;
	}
	
	if((kColumn+kWidth)>gLcd_Total_Column)
	{
		kWidth=gLcd_Total_Column-kColumn;
	}
	
	if((kRow+kHigh)>gLcd_Total_Row)
	{
		kHigh=gLcd_Total_Row-kRow;
	}
	
	for(i=0; i<kHigh; i++)
	{
		sRWMemoryType = MEMORYDISPLAY;
		offsetDisplay = (U16 *)GetSDRAMOffset((U16)(kRow+i), kColumn);
		
		for (j=0; j<kWidth; j++)
		{ 		    
		    
		    if ((j<startW || j+startW > kWidth) || (i<startH || i+startH > kHigh))		    
		    {
		         if (*offsetBuffer == filterColor)
		         {
		             //*offsetDisplay =  *offsetDisplay;
		         }
		         else  
		         {
		             *offsetDisplay = *offsetBuffer;
		         }    
		    }
		    else
		    {
		       *offsetDisplay = *offsetBuffer; 
		    }		    
		   
		    offsetDisplay++;
		    offsetBuffer++;
		}		
	}

	Memory_SetRowColumn(kRow, kColumn, kHigh, kWidth);

	if(gRefreshNeedQue)	EnQueue(&sque);
}

/*-----------------------------------------------------------------
* ����: res_write_to_bgdisp_memory
* ����: ����ʾ�������뵽bg and disp memory ��
* ����: 
* ����: 
-------------------------------------------------------------------*/
void drvLcd_write_to_bgdisp_memory(U16 *pbuf, tRES_PARA *pRes, int posX, int posY)
{
	int i,j,high,width;
	U32 *pData;
	int bufSize;
	
	high  = pRes->high;
	width = pRes->width;
	
	bufSize = high*width*c1POINT_OCCUPY_BYTES;
	
	//buf ��ʾ
    if (high == gLcd_Total_Row && width == gLcd_Total_Column)
    {
    	 
    	 memcpy((char*)Get_Display_BackGround_Offset(),    pbuf,  bufSize);
    	 memcpy((char*)Get_Display_DisplayMemory_Offset(), pbuf,  bufSize);
    	 
    	 gRefreshNeedQue = FALSE;
    }
    else
    {
        Memory_SetRowColumn(posY, posX, high, width);
        sRWMemoryType = MEMORYDISPLAY;
        for(i=0; i<high; i++)
    	{
    		for(j=0; j<width; j++)
    		{
    			pData = pbuf+i*width+j;
    			drvLcd_WritePoint(*pData);
    		}
    	}
    	
    	Memory_SetRowColumn(posY, posX, high, width);
    	
    	if(gRefreshNeedQue)	EnQueue(&sque);
    }  
}

void drvLcd_write_to_bganddisp_memory(U16 *pbuf, tRES_PARA *pRes, int posX, int posY)
{
	
	int i,j,high,width;
	U32 *pData;
	int bufSize;
	
	high  = pRes->high;
	width = pRes->width;
	
	bufSize = high*width*c1POINT_OCCUPY_BYTES;
	
	//buf ��ʾ
    if (high == gLcd_Total_Row && width == gLcd_Total_Column)
    {
    	 
    	 memcpy((char*)Get_Display_BackGround_Offset(),    pbuf,  bufSize);
    	 memcpy((char*)Get_Display_DisplayMemory_Offset(), pbuf,  bufSize);
    	 
    	 gRefreshNeedQue = FALSE;
    }
    else
    {
        Memory_SetRowColumn(posY, posX, high, width);
    	sRWMemoryType = MEMORYBACKGROUND;
        for(i=0; i<high; i++)
    	{
    		for(j=0; j<width; j++)
    		{
    			pData = pbuf+i*width+j;
    			drvLcd_WritePoint(*pData);
    		}
    	}
        
        
        Memory_SetRowColumn(posY, posX, high, width);
        sRWMemoryType = MEMORYDISPLAY;
        for(i=0; i<high; i++)
    	{
    		for(j=0; j<width; j++)
    		{
    			pData = pbuf+i*width+j;
    			drvLcd_WritePoint(*pData);
    		}
    	}    	
    	
    	Memory_SetRowColumn(posY, posX, high, width);
    	
    	if(gRefreshNeedQue)	EnQueue(&sque);
    }
}

/*----------------------------------------------------------------------------
* ����:	DisplayData2Screen
* ����:	���Դ���������ʾ���������Ƿ�dma�������ж�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void DisplayData2Screen(void)
{
	if(gRefreshNeedQue)
	{
		drvLcd_DisplayMemory2DisplayBufferAndScreen(&sque);
	}
	else
	{
		if (1 == sLcd_DmaRefresh)
		{
		    while(lcdCpuDmaOK() != TRUE)
		    {
		    	syssleep(1);
		    }
            drvLcd_SetRowColumn_Lcd(0,0,gLcd_Total_Row,gLcd_Total_Column);
    	    lcdCpuEnable((char *)Get_Display_DisplayMemory_Offset(), gLcd_Total_Row*gLcd_Total_Column*(c1POINT_OCCUPY_BYTES>>1));
		}
		else if (1 == gu8EnableRefreshSrc)
		{
    	    drvLcd_BMPFromDisplayMemory2Screen();
		}
		
		gRefreshNeedQue = TRUE;
	}
}


/*++++++++++++++++++++++++++++++++++++++ͼ�λ���++++++++++++++++++++++++++++++++++++++++++++++++++*/

//��ֱ������������ʾ����д��sdram�к�ͳһ��������
void scratch_sdram_draw_line(U16 y1,U16 x1,U16 y2,U16 x2, U16 wid, U32 ColorN)
{
	U16 u1,v1,u2,v2;
	S16 deltaX=x2-x1;
	S16 deltaY=y2-y1;
	int dx=ABS(x1-x2),dy=ABS(y1-y2);
	unsigned char m;

	S16 du,dv;
	S16 u,v,uEnd,p;
	S16 twoDv,twoDvDu;
/////////////////
    U8  i;
    U32 totalpix;
    U16 row,col;
////////////////////    

	if((deltaX*deltaY)>=0) 
	{
		if(dx>=dy)
		{
			m=1; //б����[0,1] ��Χ��
			u1=x1;v1=y1;u2=x2;v2=y2;
		}
		else
		{
			m=2; //б����(1, ��) ��Χ��
			u1=y1;v1=x1;u2=y2;v2=x2;
		}
	}
	else
	{
		if(dx>=dy)
		{
			m=3; //б����[?1,0) ��Χ��
			u1=x1;v1=y1;u2=x2;v2=2*y1-y2;
		}
		else
		{
			m=4; //б����(?��,?1) ��Χ��
			u1=y1;v1=x1;u2=2*y1-y2;v2=x2;
		}
	}

	du=ABS(u1-u2);
	dv=ABS(v1-v2);
	p=2*dv-du;
	twoDv=2*dv;
	twoDvDu=2*(dv-du);

	if(u1>u2)
	{
		u=u2;
		v=v2;
		uEnd=u1;
	}
	else
	{
		u=u1;
		v=v1;
		uEnd=u2;
	}

	switch (m)
	{
	case 1:
		row = v;
		col = u;
		break;
	case 2:
  		row = u;
		col = v;
		break;
	case 3:
  		row = 2*y1-v;
		col = u;
		break;
	case 4:
	    row = 2*y1-u;
		col = v;
		break;
	default:
	    break;
	}
	
	if (1 == m || 2 == m || 3 == m || 4== m)
	{
	    Memory_SetRowColumn(row, col, wid, wid);
	    totalpix = wid*wid;
	    
	    if (col + totalpix >= gLcd_Total_Column)
	    {
	         totalpix = gLcd_Total_Column-1-col;
	    }
	    
	    while(totalpix--)   drvLcd_WritePoint(ColorN);   
	}
	
	while(u<uEnd)
	{
		u++;
		if(p<0)
		{
			p+=twoDv;
		}
		else
		{
			v++;
			p+=twoDvDu;
		}		
		
    	switch (m)
    	{
    	case 1:
    		row = v;
    		col = u;
    		break;
    	case 2:
      		row = u;
    		col = v;
    		break;
    	case 3:
      		row = 2*y1-v;
    		col = u;
    		break;
    	case 4:
    	    row = 2*y1-u;
    		col = v;
    		break;
    	default:
    	    break;
    	}
		
		if (1 == m || 2 == m || 3 == m || 4== m)
        {
        	Memory_SetRowColumn(row, col, wid, wid);
        	totalpix = wid*wid;
        	
        	//add 090414
        	if (col + totalpix >= gLcd_Total_Column)
    	    {
    	         totalpix = gLcd_Total_Column-1-col;
    	    }
            	
        	while(totalpix--)   drvLcd_WritePoint(ColorN);   
        }
	}
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_drawLine()
* ����:	����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_drawLine(U16 y1,U16 x1,U16 y2,U16 x2)
{
	U16 u1,v1,u2,v2;
	S16 deltaX=x2-x1;
	S16 deltaY=y2-y1;
	int dx=ABS(x1-x2),dy=ABS(y1-y2);
	unsigned char m;

	S16 du,dv;
	S16 u,v,uEnd,p;
	S16 twoDv,twoDvDu;


	if((deltaX*deltaY)>=0)
	{
		if(dx>=dy)
		{
			m=1; //б����[0,1] ��Χ��
			u1=x1;v1=y1;u2=x2;v2=y2;
		}
		else
		{
			m=2; //б����(1, ��) ��Χ��
			u1=y1;v1=x1;u2=y2;v2=x2;
		}
	}
	else
	{
		if(dx>=dy)
		{
			m=3; //б����[?1,0) ��Χ��
			u1=x1;v1=y1;u2=x2;v2=2*y1-y2;
		}
		else
		{
			m=4; //б����(?��,?1) ��Χ��
			u1=y1;v1=x1;u2=2*y1-y2;v2=x2;
		}
	}

	du=ABS(u1-u2);
	dv=ABS(v1-v2);
	p=2*dv-du;
	twoDv=2*dv;
	twoDvDu=2*(dv-du);

	if(u1>u2)
	{
		u=u2;
		v=v2;
		uEnd=u1;
	}
	else
	{
		u=u1;
		v=v1;
		uEnd=u2;
	}
	switch (m)
	{
	case 1:
		drvLcd_ClearBox(v,u,1,1);
		break;
	case 2:
		drvLcd_ClearBox(u,v,1,1);
		break;
	case 3:
		drvLcd_ClearBox(2*y1-v,u,1,1);
		break;
	case 4:
		drvLcd_ClearBox(2*y1-u,v,1,1);
		break;
	default:break;
	}
	while(u<uEnd)
	{
		u++;
		if(p<0)
		{
			p+=twoDv;
		}
		else
		{
			v++;
			p+=twoDvDu;
		}
		switch (m)
		{
		case 1:
			drvLcd_ClearBox(v,u,1,1);
			break;
		case 2:
			drvLcd_ClearBox(u,v,1,1);
			break;
		case 3:
			drvLcd_ClearBox(2*y1-v,u,1,1);
			break;
		case 4:
			drvLcd_ClearBox(2*y1-u,v,1,1);
			break;
		default:
			break;
		}
	}
}


/*-----------------------------------------------------------------------------
* ����:	drvLcd_Rectangle
* ����:	������
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_Rectangle(U16 kRow, U16 kColumn, U16 kHeight, U16 kWidth, U16 FrameWidth)
{
	U16 i,j;

	sRWMemoryType = MEMORYDISPLAY;
	Memory_SetRowColumn(kRow, kColumn, kHeight, kWidth);

	for(i=0;i<FrameWidth;i++)
	{
		for(j=0;j<kWidth;j++)
		{
			drvLcd_SetRowColumn(kRow+i,kColumn+j,1,kWidth);//-
			drvLcd_WritePoint(gFrontColor);

			drvLcd_SetRowColumn(kRow+kHeight-1-i,kColumn+j,1,kWidth);//-
			drvLcd_WritePoint(gFrontColor);
		}
	}

	for(i=0;i<FrameWidth;i++)
	{
		for(j=0;j<kHeight;j++)
		{
			drvLcd_SetRowColumn(kRow+j,kColumn+i,kHeight,1);//|
			drvLcd_WritePoint(gFrontColor);

			drvLcd_SetRowColumn(kRow+j,kColumn+kWidth-1-i,kHeight,1);//|
			drvLcd_WritePoint(gFrontColor);
		}
	}

	Memory_SetRowColumn(kRow, kColumn, kHeight, kWidth);

	if(gRefreshNeedQue)	EnQueue(&sque);
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_WriteBMPColor
* ����:	ֻ��Բ�ɫ��������ɫͼƬ����ǰ��ɫ�ͱ���ɫ����
*		������ǵ�ɫ��������drvLcd_WriteBMP()��
* ����:	*pkBMPPointer--��ɫͼƬ����
*		kBMPHigh-------ͼƬ�߶�
*		kBMPWidth------ͼƬ���
*		kRow-----------�����
*		kColumn--------�����
* ����:	none
*----------------------------------------------------------------------------*/
//ʹ�ú��ŵĲ�ɫbmpͼ��bit7...bit0, bit7���� ��չ����ͼ
void drvLcd_WriteBMPColor(const U8 *pkBMPPointer,U16 kBMPHigh,U16 kBMPWidth,U16 kRow,U16 kColumn)
{
	int i,j,m,n;
	U16 tempData,tempBMPWidth;
	U8 u8nBit;
	
	tempBMPWidth =(U16)(kBMPWidth/8);
	if (kBMPWidth % 8) tempBMPWidth++;

	for(i=0;i<kBMPHigh;i++)
	{
		drvLcd_SetRowColumn((U16)(kRow+i),kColumn,1,kBMPWidth);
		for(j=0;j<tempBMPWidth;j++)
		{
			tempData=*(pkBMPPointer+i*tempBMPWidth+j);
			if((kBMPWidth % 8)&&(j==(tempBMPWidth-1)))
			{
				u8nBit = (U8)(kBMPWidth % 8);
			}
			else
			{
				u8nBit = 8;
			}

			//��͸��ͼƬ(ֻдǰ��ɫ)
			m=0;
			for(n=7 ; n>=(8-u8nBit) ; n--)
			{
				if((tempData>>n)&(0x01))
				{
					drvLcd_SetRowColumn((U16)(kRow+i),(U16)(kColumn+j*8+m),1,1);
					drvLcd_WritePoint(gFrontColor);
				}
				m++;
			}
		}
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*-----------------------------------------------------------------------------
* ����:	drvLcd_GetFontHighWidth
* ����:	���������С���õ�������ʾ�ĸߡ���
* ����:	fontsize:�����С *BMPHigh *BMPWidth:ȡ�ַ���ʾ�ĸߡ���
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_GetFontHighWidth(U16 fontsize,U16 *BMPHigh,U16 *BMPWidth)
{
	switch(fontsize)
	{
	case FONTSIZE8X16:
		*BMPWidth = 8;
		*BMPHigh = 16;
		break;
	case FONTSIZE24X24:
		*BMPWidth = 12;
		*BMPHigh = 24;
		break;	
	case FONTSIZE32X32:
		*BMPWidth = 16;
		*BMPHigh = 32;
		break;	
	default:
		*BMPWidth = 8;
		*BMPHigh = 16;
		break;			
	}
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_DispString
* ����:	��ʾ�ַ���
* ����:	pos----��ʾλ��
*		kDisplayFlag--��ʾ��ʽ
*		*pkString-----����ʾ���ַ���
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_DispString(TEXTOUT *text)
{
	U32 WidthStr;
	U16 BMPWidth;
	U16 BMPHigh;
	Character_Type_e kStr_Type;	
	
	if(text->pkString == NULL) return;//����ǿ�ָ����ֱ�ӷ���
	
	drvLcd_GetFontHighWidth(text->kDisplayFlag & FONTSIZE_BIT,&BMPHigh,&BMPWidth);

	kStr_Type = text->kDisplayFlag>>8;
	WidthStr = BMPWidth*Get_StringLength(kStr_Type,(char*) text->pkString);	
	
	if(text->x +WidthStr > gu16Lcd_ColumnLimit ) WidthStr = gu16Lcd_ColumnLimit-text->x;	
	
	drvLcd_SetColor(text->FrontColor,text->BackColor);
	switch(text->EraseBack)
	{
	case 0:	
		sRWMemoryType = MEMORYDISPLAY;
		break;
	case 1:
		if(text->prect != NULL)
		drvLcd_Background2DisplayMemory(text->prect->h,text->prect->w,text->prect->y,text->prect->x);
		else
		drvLcd_Background2DisplayMemory(BMPHigh,WidthStr,text->y,text->x);
		break;
	case 2:
		if(text->prect == NULL) return;
		drvLcd_ClearBox(text->prect->y,text->prect->x,text->prect->h,text->prect->w);
		break;	
	case 3:
	    sRWMemoryType = ALPHADISPLAY;	
		break;	    		
	default:
		sRWMemoryType = MEMORYDISPLAY;
		break;		
	}
	
	drvLcd_DisplayString(text->y, text->x, text->kDisplayFlag, text->pkString, text->pKeyStr, text->keyStrColor);	

	if (text->EraseBack != 3)
	{
		Memory_SetRowColumn(text->y, text->x, BMPHigh, WidthStr);

		if(gRefreshNeedQue)	EnQueue(&sque);
	}
	else
	{
		alpha_set_push_queue(text->y, text->x, BMPHigh, WidthStr); 
	}
}

