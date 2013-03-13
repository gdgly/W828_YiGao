/************************************************************************************
*  (C) Copyright 2010 hyco, All rights reserved	                                    *              
*																					*
*  This source code and any compilation or derivative thereof is the sole      		*
*  property of hyco and is provided pursuant 	                                    *
*  to a Software License Agreement.  This code is the proprietary information of    * 
*  hyco and is confidential in nature.  Its use and dissemination by    			*
*  any party other than hyco is strictly limited by the confidential information *
*  provisions of the Agreement referenced above. 									*
*************************************************************************************/

/***file description*****************************************************************
*																					*
*	FileName:	comm_image.c														*
*	Plantform:	PC/S2100															*
*   ColorMode:	RGB565																*
*	Project:	video related project												*
*																					*
*	Description:																	*
*				1.scale the input image to the requested size	(W1xH1->W2xH2)      *
*				2.Zoom in the input image according to the zoom ratio				*
*               3.adjust the input image's light									*
*               4.adjust the input image's contrast gradient						*
*	File Component:																	*
*		        1.image scale subprogram											*
*				2.Digital_zoom function												*
*				3.Light_adjust function												*
*				4.Contrast_adjust function											*
*	Revision History:																*
* 	Rev   Author		Date		Changes        									*
*	----  ------------	----------	-------------------------------					*
*	1.0   Yangboqun     2007-5-15   original version								*
*	1.1	  Yangboqun		2007-5-28	����佹���������˷���ֵ						*
*									����˸�������Ƭ���ڰ�����������				*
*									�ԱȶȺ����иĹ̶�����Ϊ����Ӧ����				*
*	1.4   Yangboqun     2007-8-18   ����˺���Comm_Image_1280x1024to1600x1280_YUV	*
*									��֧��YUYV422���ݸ�ʽ����						*
****end of file description**********************************************************/

/***head file************************************************************************/

#include "comm_image.h"

/***end of head file*****************************************************************/


/***marco definition*****************************************************************/

#define FIX16_02 (65536/5)
#define FIX16_COMP (1<<15)

#define INSERT_RANGE	0x10	//��������Ͳ��뷨�ķֽ��
#define Next_RANGE    0x30      //��������ֽ��
#define Third_RANGE   0x50
#define Fourth_RANGE  0x70
#define Fifth_RANGE   0x90
#define Sixth_RANGE   0xb0
#define Seventh_RANGE 0xd0
#define Eighth_RANGE   0xf0

/***end of marco definition**********************************************************/

void Comm_Image_ScaleSub_666(COMM_IMAGE_SCALE *pScaleInfo,U16 *pTempBuf1,U8 *pTempBuf2)
{
	U32 *pInBuf,*pOutBuf;
	U32 Xscale, Yscale, Xratio2, Yratio2;	//���ű�����Զ������
	U32 width1, width2, height1, height2;
	U32 sourX, sourY, dstX, dstY,tempsourY;
	U32 r,g,b,r1,g1,b1,r2,g2,b2,r3,g3,b3,r4,g4,b4,r5,g5,b5,r6,g6,b6;
	U32 Souryaddr=0,Dstxaddr=0,Dstyaddr=0,Souraddr,Dstaddr;//Դͼ�������ڴ��ַ��Ŀ��ͼ�������ڴ��ַ��ʼ��
	U32 tmpValue = 0;
	U8	XInsertFlag,YInsertFlag;	//���ݵ�ǰ���X,Y ratioֵ���ò����������뻹�ǲ�ֵ
	U32 bufwidth1,bufwidth2,bufheight1,bufheight2;

	//U16 posArrX[MAX_IMAGE_WIDTH];	//store position in X-coordiarte
	//U8	scalArrX[MAX_IMAGE_WIDTH];	//left scale

	U16 *posArrX;
	U8 *scalArrX;

	posArrX		=pTempBuf1;
	scalArrX	=pTempBuf2;

	width1		= pScaleInfo->inwidth;
	height1		= pScaleInfo->inheight;
	width2		= pScaleInfo->owidth;
	height2		= pScaleInfo->oheight;
	bufwidth1	= pScaleInfo->inbufwidth;
	bufwidth2	= pScaleInfo->obufwidth;
	bufheight1	= pScaleInfo->inbufheight;
	bufheight2	= pScaleInfo->obufheight;
	pInBuf		= (U32 *)pScaleInfo->inputbuf;
	pOutBuf		= (U32 *)pScaleInfo->oputbuf;

	Xscale		= (width1 << 16) / width2;
	Yscale		= (height1 << 16) / height2;
	
	//create the two arrays data
	sourX	= 0;
	for (dstX = 0; dstX < width2; dstX++,sourX+=Xscale)
	{
		scalArrX[dstX]	= (U8)((sourX & 0xffff) >> 8);
		posArrX[dstX]	= (U16)(sourX >> 16);
	}	
	//create end

	if(pScaleInfo->out_rotate)//��ʼ�������ʼ��ַ
	{
		Dstxaddr= pScaleInfo->obufaddr;
		Dstyaddr= pScaleInfo->obufwidth*(width2-1);
	}
	else //Dstyaddr=obufaddr;
	{
		Dstyaddr= pScaleInfo->obufaddr;
	}
	
	Souryaddr=pScaleInfo->inbufaddr;

	sourX		= 0;
	sourY		= 0;
	tempsourY	= 0;//��ر�����ʼ��
	
	for(dstY = 0; dstY < height2; )
	{
		Yratio2 	= (tempsourY & 0xffff) >> 8;
		YInsertFlag = (Yratio2 < INSERT_RANGE || Yratio2 > Eighth_RANGE) ? 0 : 1;//set flag 1

		Souraddr	= Souryaddr;
		Dstaddr		= Dstyaddr+Dstxaddr;
		//ȡ1,2,3,4������,1,2��������ݲ��ᳬ�磬ֱ��ȡ��
		tmpValue=pInBuf[Souraddr];
		r1 = (tmpValue>>12);
		g1 = ((tmpValue &0x0fc0)>>6);
		b1 = (tmpValue & 0x3f);
		
		tmpValue= pInBuf[Souraddr+1];
		r2 = (tmpValue>>12);
		g2 = ((tmpValue &0x0fc0)>>6);
		b2 = (tmpValue & 0x3f);
		
		if(sourY + 1 < height1)
		{
			tmpValue=pInBuf[Souraddr+bufwidth1];
			r3 = (tmpValue>>12);
			g3 = ((tmpValue &0x0fc0)>>6);
			b3 = (tmpValue & 0x3f);
			tmpValue=pInBuf[Souraddr+bufwidth1+1];
			r4 = (tmpValue>>12);
			g4 = ((tmpValue &0x0fc0)>>6);
			b4 = (tmpValue & 0x3f);
		}
		else
		{
			r3 = r1;
			g3 = g1;
			b3 = b1;
			r4 = r2;
			g4 = g2;
			b4 = b2;
		}	
		//end
		for(dstX = 0; dstX < width2; )
		{
			Xratio2 = scalArrX[dstX];	
			XInsertFlag = (Xratio2 < INSERT_RANGE || Xratio2 > Eighth_RANGE) ? 0 : 1;//set flag 2

			if (0 == YInsertFlag )	
			{
				if(0 == XInsertFlag)//ֱ����������
				{			
					//judge which point(in around 4 points) value to use
					if (Xratio2 < INSERT_RANGE)
					{
						if (Yratio2 < INSERT_RANGE)
							pOutBuf[Dstaddr]= ((r1<<12) | (g1<<6) | b1);
						else
							pOutBuf[Dstaddr]= ((r3<<12) | (g3<<6) | b3);
					}
					else
					{
						if (Yratio2 < INSERT_RANGE)
							pOutBuf[Dstaddr]= ((r2<<12) | (g2<<6) | b2);
						else
							pOutBuf[Dstaddr]= ((r4<<12) | (g4<<6) | b4);
					}
				}
				else //����X���ϵ������ֵ
				{
					if (Yratio2 < INSERT_RANGE)	//ƫ��1�ߵ���������
					{							
						if (Xratio2<Next_RANGE)//���1��ܽ�r=(r1*7+r2)/8
						{
							r=((r1<<3)+r2-r1)>>3;
							g=((g1<<3)+g2-g1)>>3;
							b=((b1<<3)+b2-b1)>>3;
						}
						else if(Xratio2>Seventh_RANGE)//���2��ܽ�r=(r2*7+r1)/8
						{
							r=((r2<<3)+r1-r2)>>3;
							g=((g2<<3)+g1-g2)>>3;
							b=((b2<<3)+b1-b2)>>3;
						}
						else if(Xratio2<Third_RANGE)//���1��Ͻ�r=(r1*3+r2)/4
						{
							r=((r1<<1)+r1+r2)>>2;
							g=((g1<<1)+g1+g2)>>2;
							b=((b1<<1)+b1+b2)>>2;
						}
						else if (Xratio2>Sixth_RANGE)//���2��Ͻ�r=(r2*3+r1)/4
						{	
							r=((r2<<1)+r2+r1)>>2;
							g=((g2<<1)+g2+g1)>>2;
							b=((b2<<1)+b2+b1)>>2;
						}
						else if (Xratio2<Fourth_RANGE)//ƫ���1��r=(r1*5+r2*3)/8
						{
							r=(((r1+r2)<<2)+r1-r2)>>3;
							g=(((g1+g2)<<2)+g1-g2)>>3;
							b=(((b1+b2)<<2)+b1-b2)>>3;
						}
						else if (Xratio2>Fifth_RANGE)//ƫ���2��r=(r1*3+r2*5)/8
						{	
							r=(((r1+r2)<<2)+r2-r1)>>3;
							g=(((g1+g2)<<2)+g2-g1)>>3;
							b=(((b1+b2)<<2)+b2-b1)>>3;
						}
						else//ȡƽ��ֵ
						{
							r=(r1+r2)>>1;
							g=(g1+g2)>>1;
							b=(b1+b2)>>1;
						}
					
					}
					else	//ƫ��2�ߵ���������
					{						
						if (Xratio2<Next_RANGE)//���3��ܽ�r=(r3*7+r4)/8
						{
							r=((r3<<3)+r4-r3)>>3;
							g=((g3<<3)+g4-g3)>>3;
							b=((b3<<3)+b4-b3)>>3;
						}
						else if(Xratio2>Seventh_RANGE)//���4��ܽ�r=(r4*7+r3)/8
						{
							r=((r4<<3)+r3-r4)>>3;
							g=((g4<<3)+g3-g4)>>3;
							b=((b4<<3)+b3-b4)>>3;
						}
						else if(Xratio2<Third_RANGE)//���3��Ͻ�r=(r3*3+r4)/4
						{
							r=((r3<<1)+r3+r4)>>2;
							g=((g3<<1)+g3+g4)>>2;
							b=((b3<<1)+b3+b4)>>2;
						}
						else if (Xratio2>Sixth_RANGE)//���4��Ͻ�r=(r4*3+r3)/4
						{	
							r=((r4<<1)+r4+r3)>>2;
							g=((g4<<1)+g4+g3)>>2;
							b=((b4<<1)+b4+b3)>>2;
						}
						else if (Xratio2<Fourth_RANGE)//ƫ���3��r=(r3*5+r4*3)/8
						{
							r=(((r3+r4)<<2)+r3-r4)>>3;
							g=(((g3+g4)<<2)+g3-g4)>>3;
							b=(((b3+b4)<<2)+b3-b4)>>3;
						}
						else if (Xratio2>Fifth_RANGE)//ƫ���4��r=(r3*3+r4*5)/8
						{	
							r=(((r3+r4)<<2)+r4-r3)>>3;
							g=(((g3+g4)<<2)+g4-g3)>>3;
							b=(((b3+b4)<<2)+b4-b3)>>3;
						}
						else//ȡƽ��ֵ
						{
							r=(r3+r4)>>1;
							g=(g3+g4)>>1;
							b=(b3+b4)>>1;
						}					
					}
					pOutBuf[Dstaddr]=((r << 12) | (g << 6) | b );
				}
			}
			else
			{
				if (0 == XInsertFlag)//����Y���ϵ������ֵ
				{
					if (Xratio2 < INSERT_RANGE)//ƫ����ߵ���������
					{						
						if (Yratio2<Next_RANGE)//���1��ܽ�r=(r1*7+r3)/8
						{
							r=((r1<<3)+r3-r1)>>3;
							g=((g1<<3)+g3-g1)>>3;
							b=((b1<<3)+b3-b1)>>3;
						}
						else if(Yratio2>Seventh_RANGE)//���3��ܽ�r=(r3*7+r1)/8
						{
							r=((r3<<3)+r1-r3)>>3;
							g=((g3<<3)+g1-g3)>>3;
							b=((b3<<3)+b1-b3)>>3;
						}
						else if (Yratio2<Third_RANGE)//���1��Ͻ�r=(r1*3+r3)/4
						{
							r=((r1<<1)+r1+r3)>>2;
							g=((g1<<1)+g1+g3)>>2;
							b=((b1<<1)+b1+b3)>>2;
						}
						else if (Yratio2>Sixth_RANGE)//���3��Ͻ�r=(r3*3+r1)/4
						{
							r=((r3<<1)+r3+r1)>>2;
							g=((g3<<1)+g3+g1)>>2;
							b=((b3<<1)+b3+b1)>>2;
						}
						else if (Yratio2<Fourth_RANGE)//ƫ���1��r=(r1*5+r3*3)/8
						{
							r=(((r1+r3)<<2)+r1-r3)>>3;
							g=(((g1+g3)<<2)+g1-g3)>>3;
							b=(((b1+b3)<<2)+b1-b3)>>3;
						}
						else if (Yratio2>Fifth_RANGE)//ƫ���3��r=(r1*3+r3*5)/8
						{
							r=(((r1+r3)<<2)+r3-r1)>>3;
							g=(((g1+g3)<<2)+g3-g1)>>3;
							b=(((b1+b3)<<2)+b3-b1)>>3;
						}
						else//ȡƽ��ֵ
						{
							r=(r1+r3)>>1;
							g=(g1+g3)>>1;
							b=(b1+b3)>>1;
						}
					}
					else	//ƫ���ұߵ���������
					{
						if (Yratio2<Next_RANGE)//���2��ܽ�r=(r2*7+r4)/8
						{
							r=((r2<<3)+r4-r2)>>3;
							g=((g2<<3)+g4-g2)>>3;
							b=((b2<<3)+b4-b2)>>3;
						}
						else if(Yratio2>Seventh_RANGE)//���4��ܽ�r=(r4*7+r2)/8
						{
							r=((r4<<3)+r2-r4)>>3;
							g=((g4<<3)+g2-g4)>>3;
							b=((b4<<3)+b2-b4)>>3;
						}
						else if (Yratio2<Third_RANGE)//���2��Ͻ�r=(r2*3+r4)/4
						{
							r=((r2<<1)+r2+r4)>>2;
							g=((g2<<1)+g2+g4)>>2;
							b=((b2<<1)+b2+b4)>>2;
						}
						else if (Yratio2>Sixth_RANGE)//���4��Ͻ�r=(r4*3+r2)/4
						{
							r=((r4<<1)+r4+r2)>>2;
							g=((g4<<1)+g4+g2)>>2;
							b=((b4<<1)+b4+b2)>>2;
						}
						else if (Yratio2<Fourth_RANGE)//ƫ���2��r=(r2*5+r4*3)/8
						{
							r=(((r2+r4)<<2)+r2-r4)>>3;
							g=(((g2+g4)<<2)+g2-g4)>>3;
							b=(((b2+b4)<<2)+b2-b4)>>3;
						}
						else if (Yratio2>Fifth_RANGE)//ƫ���4��r=(r2*3+r4*5)/8
						{
							r=(((r2+r4)<<2)+r4-r2)>>3;
							g=(((g2+g4)<<2)+g4-g2)>>3;
							b=(((b2+b4)<<2)+b4-b2)>>3;
						}
						else//ȡƽ��ֵ
						{
							r=(r2+r4)>>1;
							g=(g2+g4)>>1;
							b=(b2+b4)>>1;
						}
					}
				}
				else	//4��Ķ��β�ֵ
				{
					//1��2�����ֵ�����������r5�У�3��4�����ֵ�����������r6��
					if (Xratio2<Next_RANGE)//���1��3��ܽ�r5=(r1*7+r2)/8
					{
						r5=((r1<<3)+r2-r1)>>3;
						g5=((g1<<3)+g2-g1)>>3;
						b5=((b1<<3)+b2-b1)>>3;
						r6=((r3<<3)+r4-r3)>>3;
						g6=((g3<<3)+g4-g3)>>3;
						b6=((b3<<3)+b4-b3)>>3;
					}
					else if(Xratio2>Seventh_RANGE)//���2��4��ܽ�r5=(r2*7+r1)/8
					{
						r5=((r2<<3)+r1-r2)>>3;
						g5=((g2<<3)+g1-g2)>>3;
						b5=((b2<<3)+b1-b2)>>3;
						r6=((r4<<3)+r3-r4)>>3;
						g6=((g4<<3)+g3-g4)>>3;
						b6=((b4<<3)+b3-b4)>>3;
					}
					else if(Xratio2<Third_RANGE)//���1��3��Ͻ�r5=(r1*3+r2)/4
					{
						r5=((r1<<1)+r1+r2)>>2;
						g5=((g1<<1)+g1+g2)>>2;
						b5=((b1<<1)+b1+b2)>>2;
						r6=((r3<<1)+r3+r4)>>2;
						g6=((g3<<1)+g3+g4)>>2;
						b6=((b3<<1)+b3+b4)>>2;
					}
					else if (Xratio2>Sixth_RANGE)//���2��4��Ͻ�r5=(r2*3+r1)/4
					{	
						r5=((r2<<1)+r2+r1)>>2;
						g5=((g2<<1)+g2+g1)>>2;
						b5=((b2<<1)+b2+b1)>>2;
						r6=((r4<<1)+r4+r3)>>2;
						g6=((g4<<1)+g4+g3)>>2;
						b6=((b4<<1)+b4+b3)>>2;
					}
					else if (Xratio2<Fourth_RANGE)//ƫ���1��3��r5=(r1*5+r2*3)/8
					{
						r5=(((r1+r2)<<2)+r1-r2)>>3;
						g5=(((g1+g2)<<2)+g1-g2)>>3;
						b5=(((b1+b2)<<2)+b1-b2)>>3;
						r6=(((r3+r4)<<2)+r3-r4)>>3;
						g6=(((g3+g4)<<2)+g3-g4)>>3;
						b6=(((b3+b4)<<2)+b3-b4)>>3;
					}
					else if (Xratio2>Fifth_RANGE)//ƫ���2��4��r5=(r1*3+r2*5)/8
					{	
						r5=(((r1+r2)<<2)+r2-r1)>>3;
						g5=(((g1+g2)<<2)+g2-g1)>>3;
						b5=(((b1+b2)<<2)+b2-b1)>>3;
						r6=(((r3+r4)<<2)+r4-r3)>>3;
						g6=(((g3+g4)<<2)+g4-g3)>>3;
						b6=(((b3+b4)<<2)+b4-b3)>>3;
					}
					else//ȡƽ��ֵ
					{
						r5=(r1+r2)>>1;
						g5=(g1+g2)>>1;
						b5=(b1+b2)>>1;
						r6=(r3+r4)>>1;
						g6=(g3+g4)>>1;
						b6=(b3+b4)>>1;
					}
					//�µ�5��6�����ֵ���õ����ս��
					if (Yratio2<Next_RANGE)//���5��ܽ�r=(r5*7+r6)/8
					{
						r=((r5<<3)+r6-r5)>>3;
						g=((g5<<3)+g6-g5)>>3;
						b=((b5<<3)+b6-b5)>>3;
					}
					else if(Yratio2>Seventh_RANGE)//���6��ܽ�r=(r6*7+r5)/8
					{
						r=((r6<<3)+r5-r6)>>3;
						g=((g6<<3)+g5-g6)>>3;
						b=((b6<<3)+b5-b6)>>3;
					}
					else if (Yratio2<Third_RANGE)//���5��Ͻ�r=(r5*3+r6)/4
					{
						r=((r5<<1)+r5+r6)>>2;
						g=((g5<<1)+g5+g6)>>2;
						b=((b5<<1)+b5+b6)>>2;
					}
					else if (Yratio2>Sixth_RANGE)//���6��Ͻ�r=(r6*3+r5)/4
					{
						r=((r6<<1)+r6+r5)>>2;
						g=((g6<<1)+g6+g5)>>2;
						b=((b6<<1)+b6+b5)>>2;
					}
					else if (Yratio2<Fourth_RANGE)//ƫ���5��r=(r5*5+r6*3)/8
					{
						r=(((r5+r6)<<2)+r5-r6)>>3;
						g=(((g5+g6)<<2)+g5-g6)>>3;
						b=(((b5+b6)<<2)+b5-b6)>>3;
					}
					else if (Yratio2>Fifth_RANGE)//ƫ���6��r=(r5*3+r6*5)/8
					{
						r=(((r5+r6)<<2)+r6-r5)>>3;
						g=(((g5+g6)<<2)+g6-g5)>>3;
						b=(((b5+b6)<<2)+b6-b5)>>3;
					}
					else//ȡƽ��ֵ
					{
						r=(r5+r6)>>1;
						g=(g5+g6)>>1;
						b=(b5+b6)>>1;
					}										
				}
				pOutBuf[Dstaddr]=((r << 12) | (g << 6) | b );
			}
			dstX++;
			//if(pJpeg_Param->ImageRotate) Dstaddr-=pJpeg_Param->ScreenWidth;
			//else Dstaddr++;
			if(pScaleInfo->out_rotate) Dstaddr-=bufwidth2;
			else Dstaddr++;

			if (dstX < width2)
			{
				//
				if((posArrX[dstX]-sourX)>=2) //1,2,3,4����������
				{
					//��������
					for(;sourX<posArrX[dstX];)
					{
						sourX++;
						Souraddr++;
					}
					//ȡ1,2,3,4��ֵ
					tmpValue=pInBuf[Souraddr];
					r1 = (tmpValue>>12);
					g1 = ((tmpValue &0x0fc0)>>6);
					b1 = (tmpValue & 0x3f);
				
					if(sourX + 1 < width1)
					{
						tmpValue=pInBuf[Souraddr+1];
						r2 = (tmpValue>>12);
						g2 = ((tmpValue &0x0fc0)>>6);
						b2 = (tmpValue  &0x3f);
						if(sourY + 1 < height1)
						{
						tmpValue=pInBuf[Souraddr+bufwidth1];
						r3 = (tmpValue>>12);
						g3 = ((tmpValue &0x0fc0)>>6);
						b3 = (tmpValue  &0x3f);
						tmpValue=pInBuf[Souraddr+bufwidth1+1];
						r4 = (tmpValue>>12);
						g4 = ((tmpValue &0x0fc0)>>6);
						b4 = (tmpValue &0x3f);
						}
						else
						{
						r3 = r1;
						g3 = g1;
						b3 = b1;
						r4 = r2;
						g4 = g2;
						b4 = b2;
						}
					}
					else
					{
						r2 = r1;
						g2 = g1;
						b2 = b1;
						if(sourY + 1 < height1)
						{
						tmpValue=pInBuf[Souraddr+bufwidth1];
						r3 = (tmpValue>>12);
						g3 = ((tmpValue &0x0fc0)>>6);
						b3 = (tmpValue &0x3f);
						}
						else
						{
						r3 = r1;
						g3 = g1;
						b3 = b1;
						}
						r4 = r3;
						g4 = g3;
						b4 = b3;
					}
				}
				else if((posArrX[dstX]-sourX)>=1)
				//
				//if((posArrX[dstX]-sourX)>=1)
				{	//��������
					sourX++;
					Souraddr++;
					//��һ�ε�1��3������ڵ�ǰ��2��4����ֵ
					r1=r2;
					g1=g2;
					b1=b2;
					r3=r4;
					g3=g4;
					b3=b4;
				    //ȡ��һ�εĵ�2��4����
					if(sourX + 1 < width1)
					{
						tmpValue=pInBuf[Souraddr+1];
						r2 = (tmpValue>>12);
						g2 = ((tmpValue &0x0fc0)>>6);
						b2 = (tmpValue &0x3f);
						if( sourY + 1 < height1)
						{
							tmpValue=pInBuf[Souraddr+bufwidth1+1];
							r4 = (tmpValue>>12);
							g4 = ((tmpValue &0x0fc0)>>6);
							b4 = (tmpValue  &0x3f);
						}
						else
						{
							r4 = r2;
							g4 = g2;
							b4 = b2;
						}
					}
					else
					{
						r2 = r1;
						g2 = g1;
						b2 = b1;
						r4 = r3;
						g4 = g3;
						b4 = b3;
					}
				}
			}
		}

		if(pScaleInfo->out_rotate) Dstxaddr++;
		else Dstyaddr+=bufwidth2;

		sourX=0;//���³�ʼ��,�µ�һ�п�ʼ
		dstY++;	
		tempsourY+=Yscale;
		for (;sourY<(tempsourY>>16);)
		{
			sourY++;
			Souryaddr+=bufwidth1;
		}
	}
	return ;
}
