#ifndef COMM_IMAGE_API_H
#define COMM_IMAGE_API_H

#ifdef __cplusplus
extern "C"
{
#endif

/***example**************************************************************************

	//DCЧ��ʹ��˵��
	U16 *pBuf,*pCmos,*pOut;
	int i;
	
	//ʹ��ǰ�����ȳ�ʼ�����ݱ�pBufָ��128k bytes������
	Comm_Image_ProcInit(pBuf,0,0,COMM_IMAGE_NORMAL);
	
	
	//pCmosΪCmos Sensor���ͼ�񻺳���
	//pOutΪͼ���ŵ�ַ
	//WidthΪ��׽��ͼ���ȣ�HeightΪͼ��߶�
	
	if(Ԥ��ģʽ)
	{
		//Ԥ��ͼΪ640x240
		for(i=0;i<240;i++)
		{
			Comm_Image_640x1_to_320x1(pCmos+i*640,pOut+i*320);
		}
	}
	else(����ģʽ)
	{
		Comm_Image_Process(pCmos,Width,Height);
	}
	
	if(�ı�Ч��)
	{
		Comm_Image_ProcInit(pBuf,���ȵ���,�Աȶȵ���,Ч��ֵ);
	}
		
****end of example*******************************************************************/


/***macro definition*****************************************************************/

//Comm_Image_ProcInit.Type
//DCЧ��
#define COMM_IMAGE_NORMAL 0
#define COMM_IMAGE_OLD_PHOTO 1
#define COMM_IMAGE_GRAY 2
#define COMM_IMAGE_RED 3
#define COMM_IMAGE_GREEN 4
#define COMM_IMAGE_BLUE 5
#define COMM_IMAGE_NEGATIVE 6

/***end of macro definition**********************************************************/


/***variable definition**************************************************************/
typedef struct comm_image_scale
{
	U16        *inputbuf;   //����ͼ�񻺳���ָ��
	U16        inbufwidth;  //����ͼ�񻺳������
	U16        inbufheight; //����ͼ�񻺳����߶�
	U16        inwidth;     //����ͼ����
	U16        inheight;    //����ͼ��߶�
	U32        inbufaddr;   //����ͼ���ڻ���������ʼλ��
	U16        *oputbuf;    //���ͼ�񻺳���ָ��
	U16        obufwidth;   //���ͼ�񻺳������
	U16        obufheight;  //���ͼ�񻺳����߶�
	U16        owidth;      //���ͼ����
	U16        oheight;     //���ͼ��߶�
	U32        obufaddr;    //���ͼ���ڻ���������ʼλ��(����ת��־�й�)
	BOOL       out_rotate;  //���ͼ����ת��־
}COMM_IMAGE_SCALE;
/***end of variable definition*******************************************************/


/***function definition**************************************************************/

/************************************************************************************
*function:		Comm_Image_1280x1024to1600x1280_YUV            
*
*description:	scale image from 1280x1024 to 1600x1280
*				Transfer 4 input lines from 1280x1 to 1600x1 firstly, and store temporary
*				data in SRAM. Then transfer 1600x4 data block to 1600x5 and save data
*				in output buffer. Loop until all input data are done.
*
*				Be Attention, input buffer address should larger than ouput buffer
*				address 1440K bytes at least, otherwise input image will be currupted !!!
*
*parameter:		pInput		1280x1024, YUYV442
*				pOutput		1600x1280, YUYV442
*				pTempBuf	5*1600*2 bytes, store temporary data 
*
*return value:	0			success
*				-1			error			
*************************************************************************************/
int Comm_Image_1280x1024to1600x1280_YUV(U16 *pInput,U16 *pOutput,U16 *pTempBuf);

/************************************************************************************
*function:		Comm_Image_1280x1024to1600x1280            
*
*description:	scale image from 1280x1024 to 1600x1280
*				Transfer 4 input lines from 1280x1 to 1600x1 firstly, and store temporary
*				data in SRAM. Then transfer 1600x4 data block to 1600x5 and save data
*				in output buffer. Loop until all input data are done.
*
*				Be Attention, input buffer address should larger than ouput buffer
*				address 1440K bytes at least, otherwise input image will be currupted !!!
*
*parameter:		pInput		1280x1024, RGB565
*				pOutput		1600x1280, RGB565
*				pTempBuf	5*1600*2 bytes, store temporary data 
*
*return value:	0			success
*				-1			error			
*************************************************************************************/
int Comm_Image_1280x1024to1600x1280(U16 *pInput,U16 *pOutput,U16 *pTempBuf);

/************************************************************************************
*function:		Comm_Image_ScaleSub            
*
*description:	scale image data block according to required size
*
*parameter:		pScaleInfo		reference the struct define of comm_image_scale
*				pTempBuf1		image width * sizeof(U16)	
*				pTempBuf2		image width * sizeof(U8)
*				
*return value:	none
*notice:       output image size >=input image size/2
*************************************************************************************/
void Comm_Image_ScaleSub(COMM_IMAGE_SCALE *pScaleInfo,U16 *pTempBuf1,U8 *pTempBuf2);

/************************************************************************************
*function:		Comm_Image_Zoom                                                    
*                                                                                  
*description:	Zoom in the input image according to the zoom ratio					
*																					
*parameter:		pInputBuf	input image data block									
*				pOutputBuf	output image data block									
*               pTempBuf	temp buffer for	Jpeg_ScaleImage_sub,the size is width*3									*
*				Ratio	    zoom ratio(2,3,4,5,6)								
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	0    Success
*               -1   Error(�����佹��Χ,���buffer��û������,����buffer�е�������Ч)																
*************************************************************************************/
int Comm_Image_Zoom(U16 *pInputBuf,U16 *pOutputBuf,U8 *pTempBuf,U8 Ratio,U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_ProcInit                                                   
*                                                                                   
*description:	compute the image process table							
*																					
*parameter:		pBuf        point to table buffer, 128k bytes
*				LRatio	    light adjust ratio(-4,-3,-2,-1,0,1,2,3,4),default is 0		
*				CRatio      contrast adjust ratio(-4,-3,-2,-1,0,1,2,3,4),default is 0								
*	            Type        reference the macro definition
*							
*return value:	none																
*************************************************************************************/
void Comm_Image_ProcInit(U16 *pBuf,S16 LRatio,S16 CRatio, U16 Type);

/************************************************************************************
*function:		Comm_Image_640x1_to_320x1                                                   
*                                                                                   
*description:	scale and process image
*																					
*parameter:		pInput      input image buffer
*				pOutput	    output image buffer
*							
*return value:	none																
*************************************************************************************/
void Comm_Image_640x1_to_320x1(U16 *pInput,U16 *pOutput);

/************************************************************************************
*function:		Comm_Image_Process                                                   
*                                                                                   
*description:	process image
*																					
*parameter:		pBuf      point to image buffer
*				Width	  width of image
*				Height	  height of image
*							
*return value:	none																
*************************************************************************************/
void Comm_Image_Process(U16 *pBuf,U32 Width,U32 Height);

/************************************************************************************
*function:		Comm_Image_LightAdjust                                                        
*                                                                                   
*description:	Adjust the input image's light										
*																					
*parameter:		pBuf		input(output) buffer
*				Ratio	    adjust ratio(-4,-3,-2,-1,0,1,2,3,4),default is 0		
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_LightAdjust(U16 *pBuf,S16 Ratio,U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_ContrastAdjust                                                     
*                                                                                   
*description:	Adjust the input image's contrast gradient							
*																					
*parameter:		pBuf        input(output) buffer								
*				Ratio	    adjust ratio(-4,-3,-2,-1,0,1,2,3,4),default is 0		
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_ContrastAdjust(U16 *pBuf,S16 Ratio,U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_Emboss                                                    
*                                                                                   
*description:	Emboss the input image							
*																					
*parameter:		pBuf        input(output) buffer										
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_Emboss(U16 *pBuf,U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_Oldphoto                                                    
*                                                                                   
*description:	Aging the input image to old 							
*																					
*parameter:		pBuf        input(output) buffer										
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_Oldphoto(U16 *pBuf,U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_Grayimage                                                    
*                                                                                   
*description:	transform the RGB565 image into gray image							
*																					
*parameter:		pBuf        input(output) buffer										
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_Grayimage(U16 *pBuf,U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_normal                                                     
*                                                                                   
*description:	Adjust the input image's contrast gradient							
*																					
*parameter:		pBuf        input(output) buffer								
*				Ratio	    adjust ratio(-4,-3,-2,-1,0,1,2,3,4),default is 0		
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_normal(U16 *pBuf,S16 LRatio,S16 CRatio, U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_old                                                     
*                                                                                   
*description:	Adjust the input image's contrast gradient							
*																					
*parameter:		pBuf        input(output) buffer								
*				Ratio	    adjust ratio(-4,-3,-2,-1,0,1,2,3,4),default is 0		
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_old(U16 *pBuf,S16 LRatio,S16 CRatio, U16 Width,U16 Height);

/************************************************************************************
*function:		Comm_Image_gray                                                     
*                                                                                   
*description:	Adjust the input image's contrast gradient							
*																					
*parameter:		pBuf        input(output) buffer								
*				Ratio	    adjust ratio(-4,-3,-2,-1,0,1,2,3,4),default is 0		
*				Width       input(output) image width								
*               Height      input(output) image height								
*return value:	none																
*************************************************************************************/
//void Comm_Image_gray(U16 *pBuf,S16 LRatio,S16 CRatio, U16 Width,U16 Height);

/***end of function definition*******************************************************/


#ifdef __cplusplus
}
#endif

#endif