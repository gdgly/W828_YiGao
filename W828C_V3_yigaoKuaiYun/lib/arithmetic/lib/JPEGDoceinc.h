#ifndef _JPEGDOCEINC_
#define _JPEGDOCEINC_

/*
*standard include files
*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"


typedef enum  
{
	JpegDecodeReturn_Success  =	0,
	JpegDecodeReturn_error,
	JpegDecodeReturn_Nofile,
	JpegDecodeReturn_ReadfileError,
	JpegDecodeReturn_NotJpegfile,	
	JpegDecodeReturn_UnSupportJpegfile,
	JpegDecodeReturn_BadJpegfile,
	JpegDecodeReturn_BadQuantilizationID,
	JpegDecodeReturn_BadDataPrecision,
	JpegDecodeReturn_BadImageWidthorHeight,
	JpegDecodeReturn_BadComponentInfo,
	JpegDecodeReturn_BadHuffmanTable,
	JpegDecodeReturn_InsufficientMemory	,
	JpegDecodeReturn_ComponentNot3,
	JpegDecodeReturn_UserBreak,
	JpegDecodeReturn_NoThumbnail
} JpegDecodeReturn_en;

extern U32 mathdiv(U32 a,U32 b);
extern U32 mathunsignmulti(U32 a,U32 b);
extern int mathsignmulti(int a,int b);

typedef char *LPCSTR;


/*************************************************************************
 * ����:	Jpeg_DecodeFile            
 *
 * ˵��:	Jpeg�����ں����������Ź���
 *
 * ����:	FileName 	�ļ���
 *			pTemp		���ݻ������׵�ַ
 *			TempLen		���������ȣ��������12k��
 *			LcdW		���ź�ͼ�ο��
 *			LcdH		���ź�ͼ��߶�
 *			pOutBuf		ͼ������������׵�ַ
 *
 * ����:	JpegDecodeReturn_en
 **************************************************************************/
JpegDecodeReturn_en Jpeg_DecodeFile(char *FileName,U8 *pTemp,U32 TempLen,U16 LcdW,U16 LcdH,U8 *pOutBuf);

// idct.c ��ʵ��
void JPEGDec_idctInt (int* coef_block,U8 *output_buf); 

// JpegDisplay.c  ��ʵ��                             
void displayImageInformation(void);

#endif

