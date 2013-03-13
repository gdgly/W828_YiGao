/************************************************************************************
*  (C) Copyright 2005 Shanghai Hyctron Electronic Design House, All rights reserved	*              
*																					*
*  This source code and any compilation or derivative thereof is the sole      		*
*  property of Shanghai Hyctron Electronic Design House and is provided pursuant 	*
*  to a Software License Agreement.  This code is the proprietary information of    * 
*  Hyctron and is confidential in nature.  Its use and dissemination by    			*
*  any party other than Hyctron is strictly limited by the confidential information *
*  provisions of the Agreement referenced above. 									*
*************************************************************************************/

#ifndef JPEGAPI_H
#define JPEGAPI_H

/***example**************************************************************************

	tJPEG_API tJpeApi;
	
	if(����)
	{
		memset(&tJpegApi,0,sizeof(tJPEG_API));//ʹ��ǰӦ������ṹ�壡����
		
		tJpegApi.Func=Jpeg_Decode;
		tJpegApi.JpegFile=TRUE;
		tJpegApi.ReadFromFile=TRUE;
		tJpegApi.ScaleImage=�Ƿ����ţ�TRUE/FALSE;
		tJpegApi.PreViewImage=�Ƿ����Ƚ�Ԥ��ͼ��TRUE/FALSE;
		tJpegApi.pFileName=(U8 *)"�ļ���";
		tJpegApi.ReadOffset=0;
		tJpegApi.pOutputBuf=(U8 *)���ͼ�񻺳�����ַ;
		tJpegApi.OutputBufLen=���ͼ�񻺳������ȣ�������װ������ͼ��;
		tJpegApi.pTempBuf=(U8 *)SRAM������������ַ;
		tJpegApi.TempBufLen=SRAM�������������ȣ�16K Bytes;
		
		if(tJpegApi.ScaleImage==TRUE)
		{
			tJpegApi.pSdramBuf=(U8 *)Sdram������������ַ;
			tJpegApi.SdramBufLen=Sdram�������������ȣ�256K Bytes;		
			tJpegApi.ScrWidth=���ź���;
			tJpegApi.ScrHeight=���ź�߶�;
		}

		if(Jpeg_Codec(&tJpegApi)==Jpeg_Success)
		{
			//����ɹ�
			...
		}
		else
		{
			//������
			...	
		}
	}
	else if(����)
	{
		memset(&tJpegApi,0,sizeof(tJPEG_API));//ʹ��ǰӦ������ṹ�壡����
		
		tJpegApi.Func=Jpeg_Encode;
		tJpegApi.ImageLevel=ͼ��������5��������ʹ��Jpeg_High;
		tJpegApi.JpegFile=TRUE;
		tJpegApi.EncodeFormat=�������ݸ�ʽѡ��JPEG_FORMAT_RGB565/JPEG_FORMAT_YUV422;
		tJpegApi.ReadOffset=0;
		tJpegApi.pInputBuf=(U8 *)�������ݵ�ַ������������;
		tJpegApi.InputBufLen=�������ݳ���;
		tJpegApi.pOutputBuf=(U8 *)�����������ַ�����������;
		tJpegApi.OutputBufLen=�������������;
		tJpegApi.pTempBuf=(U8 *)SRAM������������ַ;
		tJpegApi.TempBufLen=SRAM�������������ȣ�24K Bytes;
		tJpegApi.pSdramBuf=(U8 *)Sdram������������ַ;
		tJpegApi.SdramBufLen=Sdram�������������ȣ�256K Bytes;
		tJpegApi.ImageWidth=����ͼ���ȣ�����Ϊ16�ı���;
		tJpegApi.ImageHeight=����ͼ��߶ȣ�����Ϊ16�ı���;

		if(Jpeg_Codec(&tJpegApi)==Jpeg_Success)
		{
			//����ɹ�
			//tJpegApi.OutputDataLenΪ�������������Ч���ݵĳ���
			fwrite(tJpegApi.pOutputBuf,1,tJpegApi.OutputDataLen,pfile);//�����ļ�
			...
		}
		else
		{
			//������
			...
		}
	}
		
****end of example*******************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/***variable definition**************************************************************/

//ִ�й���
//tJPEG_API.Func
#define Jpeg_Decode 0
#define Jpeg_Encode 1
#define Jpeg_GetSize 2

//��������
//tJPEG_API.ImageLevel
#define Jpeg_Low 0
#define Jpeg_Standard 1
#define Jpeg_High 2
#define Jpeg_Vivid 3
#define Jpeg_Original 4

//�������ݸ�ʽ
//tJPEG_API.EncodeFormat
#define JPEG_FORMAT_RGB565 1
#define JPEG_FORMAT_YUV422 2
#define JPEG_FORMAT_YUV420 3
	
typedef enum  
{
	Jpeg_Success=0,
	Jpeg_error,
	Jpeg_Nofile,
	Jpeg_ReadfileError,
	Jpeg_NotJpegfile,	
	Jpeg_UnSupportJpegfile,
	Jpeg_BadJpegfile,
	Jpeg_BadQuantilizationID,
	Jpeg_BadDataPrecision,
	Jpeg_BadImageWidthorHeight,
	Jpeg_BadComponentInfo,
	Jpeg_BadHuffmanTable,
	Jpeg_InsufficientMemory	,
	Jpeg_ComponentNot3,
	Jpeg_UserBreak,
	Jpeg_NoThumbnail
}JpegReturn_en;


typedef struct tjpeg_api
{
	U8			Func;			//ִ�й���
	U8			ImageLevel;		//������������5������ع���:Encode
	U8			EncodeFormat;	//�����������ݸ�ʽ(RGB565/YUV422)
	BOOL		JpegFile;		//�Ƿ�Ϊjpeg�ļ�����ع���:Decode,Encode
	BOOL		ReadFromFile;	//�Ƿ���ļ���ȡ���ݣ���ع���:Decode,GetSize
	BOOL		ScaleImage;		//�Ƿ���Ҫͼ�����ţ���ع���:Decode
	BOOL		PreViewImage;	//�Ƿ�ֻ��Ԥ��ͼ��1Ϊֻ��Ԥ��ͼ��0Ϊֱ�ӽ��ͼ

	U8			*pFileName;		//�ļ�������ع���:JpegFile
#ifdef PLATFORM_PC
	FILE		*FileHandle;	//�ļ��������ع���:!JpegFile && ReadFromFile
#else
	int			FileHandle;		//�ļ��������ع���:!JpegFile && ReadFromFile
#endif

	U32			ReadOffset;		//��ƫ�Ƶ�ַ����ع���:Decode,Encode,GetSize

	U8			*pInputBuf;		//���뻺��������ع���:Encode || !ReadFromFile
	U32			InputBufLen;	//���뻺�������ȣ���ع���:Encode || !ReadFromFile
	U8			*pOutputBuf;	//�������������ع���:Decode,Encode
	U32			OutputBufLen;	//������������ȣ���ع���:Decode,Encode
	U32			OutputDataLen;	//������ݳ��ȣ���ع���:Encode
	U8			*pTempBuf;		//��������������ع���:Decode,Encode
	U32			TempBufLen;		//�������������ȣ���ع���:Decode,Encode
	U8			*pSdramBuf;
	U32			SdramBufLen;

	U16			ImageWidth;		//ͼ���ȣ���ع���:Encode || GetSize
	U16			ImageHeight;	//ͼ��߶ȣ���ع���:Encode || GetSize
	U16			ScrWidth;		//��Ļ��ȣ���ع���:ScaleImage
	U16			ScrHeight;		//��Ļ�߶ȣ���ع���:ScaleImage
}tJPEG_API;

/***end of variable definition*******************************************************/


/***function definition**************************************************************/

/************************************************************************************
*function:		Jpeg_Codec            
*
*description:	decode & encode interface method
*
*parameter:		pJpegApi	parameter struction
*
*return value:	JpegReturn_en
*************************************************************************************/
JpegReturn_en Jpeg_Codec(struct tjpeg_api *pJpegApi);
JpegReturn_en Jpeg_HWCodec(struct tjpeg_api *pJpegApi,U8 *pBuf,U32 BufLen);

/***end of function definition*******************************************************/

#ifdef __cplusplus
}
#endif

#endif

