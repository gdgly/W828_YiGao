/*************************************************************************
 * INCLUDES
 *************************************************************************/
#include "hyTypes.h"
#include "hyErrors.h"
#include "hyhwTcc7901.h"
#include "hyhwGpio.h"
#include "hyhwDma.h"
#include "hyhwCkc.h"

/**************************************************************************
 * Local Types and defines:
**************************************************************************/
#include "hyhwRgb.h"

/*************************************************************************
 * DEFINES
 *************************************************************************/

//LCD_LPW+LCD_LSWC+W-1+LCD_LEWC

#define LCD_LPW		34   //��������
#define LCD_LSWC	31   //��ǰ��
#define LCD_LEWC	10   //�к��

//LCD_FPW+LCD_FSWC+H-1+LCD_FEWC

#define LCD_VDB		0   //
#define LCD_VDF		0    //
#define LCD_FPW		2    //֡������
#define LCD_FSWC	14    //֡ǰ��   ����ʱͼ�������ƶ�
#define LCD_FEWC	2    //֡���    


//#define	LCD_IMAGE0_BASE_ADDR						0x20100000
//#define	LCD_IMAGE1_BASE_ADDR						0x20200000
//#define	LCD_IMAGE2_BASE_ADDR						0x20300000

//#define PICTURE_RGBBUF_ADDR0				(0x20200000)
//#define PICTURE_RGBBUF_ADDR1				(0x20300000)

/*************************************************************************
 * Program
 *************************************************************************/
#if 1

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_clkSet()
* ����: ����lcd rgb ʱ�� 
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_clkSet( void )
{
#if 0
	//12M����
	TCC7901_CKC_PCK_LCD_REGISTER = 0x14000001;
	LCDC_CLK_DIVIDER_REGISTER = 0;
#else	
	//PLL1
	TCC7901_CKC_PCK_LCD_REGISTER = 0x11000000;
	LCDC_CLK_DIVIDER_REGISTER = 0x8;
#endif
	
}

void hyhwLcd_setVerticalTime(U32 vdb, U32 vdf, U32 fpw, U32 height, U32 fswc,  U32 fewc)
{
	LCDC_VERTICAL_TIMING_REGISTER_1 = (vdb<<27) | (vdf<<22) | (fpw<<16) | (height-1);
	LCDC_VERTICAL_TIMING_REGISTER_2 = (fswc<<16) | fewc;
	LCDC_VERTICAL_TIMING_REGISTER_3 = (fpw<<16) | (height-1);
	LCDC_VERTICAL_TIMING_REGISTER_4 = (fswc<<16) | fewc;
	
	return ;
}

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setHorizontalTime
* ����:	Setting the Horizontal Timing Register
* ����:	
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setHorizontalTime(U32 lpw, U32 width, U32 lswc, U32 lewc)
{
	LCDC_HORIZONTAL_TIMING_REGISTER_1 = (lpw << 16) | (width - 1);
	LCDC_HORIZONTAL_TIMING_REGISTER_2 = (lswc << 16) | lewc;
	
	return ;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_lcdRgbConfig
* ����:	����lcd ��DMA
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwDma_lcdRgbConfig( eLCDBUSTYPE_EN  busType, U32 addr)
{
	tDMA_REGISTER	*ptDmaReg;

	ptDmaReg = (tDMA_REGISTER *)LCD_DMA_REGISTER_ADDR;
	
	ptDmaReg->SOURCE_PARAMETER = DMA_PORT_TYPE_HALF_WORD; 	/*DMAʱsource ����Ϊ1*/
	ptDmaReg->DESTINATION_PARAMETER = DMA_PORT_TYPE_HALF_WORD; /*DMAʱDestination ����Ϊ0*/		
	
	ptDmaReg->HOP_COUNT	= 0x19200;
	
	if( busType == LcdBusType16 )
	{
		ptDmaReg->CHANNEL_CONTROL   = (  CHCTRL_BIT_TRANSFER_TYPE_SOFTWARE
									   | CHCTRL_BIT_BURST_SIZE_4
									   | CHCTRL_BIT_WORD_SIZE_16BIT	
									  );
	}
	else
	{
		ptDmaReg->CHANNEL_CONTROL   = (  CHCTRL_BIT_TRANSFER_TYPE_SOFTWARE
									   | CHCTRL_BIT_BURST_SIZE_4
									   | CHCTRL_BIT_WORD_SIZE1_32BIT	
									  );
	}
	ptDmaReg->DESTINATION_START_ADDR= addr;

}

void hyhwDma_lcdRgb_Disable(void)
{
	if ((LCDC_CTRL_REGISTER&LCD_BIT_LCD_CONTROLLER_ENABLE) != 0)
	{
		LCDC_CTRL_REGISTER &= (~LCD_BIT_LCD_CONTROLLER_ENABLE);
		//�ȴ���ǰ֡��ɣ��������Ҫ�ȴ�һ֡ʱ�䣻��Ҫ������һ֡��ˢ���ٶ�
		//���ڹ����Լ15ms
		while((LCDC_STATUS_REGISTER&BIT6));
	}
}
void hyhwDma_lcdRgb_Enable(void)
{
	if ((LCDC_CTRL_REGISTER&LCD_BIT_LCD_CONTROLLER_ENABLE) == 0)
	{
		LCDC_CTRL_REGISTER |= LCD_BIT_LCD_CONTROLLER_ENABLE;
	}
}
/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_init
* ����:	��ʼ��
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_init( void )
{}

/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_enableCtrler
* ����:	lcd ������ʹ��
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_enableCtrler(void)
{
	LCDC_CTRL_REGISTER |= LCD_BIT_LCD_CONTROLLER_ENABLE; //enable lcd controller
}

/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_disableCtrler
* ����:	lcd ��������ʹ��
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_disableCtrler(void)
{
	LCDC_CTRL_REGISTER &= ~LCD_BIT_LCD_CONTROLLER_ENABLE; //enable lcd controller
}

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setBackgroundColor
* ����:	Setting the back ground color 
* ����:	red		-- half word long input, the red grade 
*		green	-- half word long input, the green grade 
*		blue	-- half word long input, the blue grade 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setBackgroundColor( U8 red, U8 green, U8 blue )
{
	LCDC_BACKGROUND_COLOR_REGISTER = ( ( blue << 16 )|( green << 8 )| red );
	
	return ;
}

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setSize
* ����:	Setting the LCD Size Information
* ����:	ch		-- Image Type or LCD Panel 
*		width	-- Horizontal Pixel Count
*		height	-- Vertical Line Count
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setSize( eLCD_CH_TYPE ch, U32 width, U32 height ) 
{
	switch(ch)
	{
		case	LCD_CH:
			LCDC_DISPLAY_SIZE_REGISTER = (height << 16) | width;
			break;
		case	IMG_CH0:
			LCDC_IMAGE_0_SIZE_REGISTER = (height << 16) | width;
			break;
		case	IMG_CH1:
			LCDC_IMAGE_1_SIZE_REGISTER = (height << 16) | width;
			break;
		case	IMG_CH2:
			LCDC_IMAGE_2_SIZE_REGISTER = (height << 16) | width;
			break;
		default :
			break;	
	}
	
	
	return ;
}

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_enableImage
* ����:	enable the image channel
* ����:	ch		-- Image Type or LCD Panel 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_enableImage( eLCD_CH_TYPE ch )
{
	LCDC_CTRL_REGISTER |= 1<< ch;
}

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setImage0Param
* ����:	Setting the image 0 parameter
* ����:	imgX		-- image 0 start x position 
*		imgY		-- image 0 start y position 
*		width		-- image 0 width 
*		height		-- image 0 height
*		baseAddress	-- image 0 buffer start address
*		bitPerPixel	-- how many bits per pixel
*		scale		-- the buffer hop num
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setImage0Param( U16 imgX, U16 imgY, U16 width, U16 height, U32 baseAddress, ePIXEL_INPUT bitPerPixel, U32 scale)
{
	LCDC_IMAGE_0_POSITION_REGISTER = ( imgY<<16 )|imgX;
	LCDC_IMAGE_0_SIZE_REGISTER = ( height <<16 )|width;
	LCDC_IMAGE_0_BASE_ADDRESS_0_REGISTER = baseAddress;
	LCDC_IMAGE_0_CTRL_REGISTER = (bitPerPixel);
	LCDC_IMAGE_0_SCALE_RATIO = scale;
}

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setImage1Param
* ����:	Setting the image 1 parameter
* ����:	imgX		-- image 1 start x position 
*		imgY		-- image 1 start y position 
*		width		-- image 1 width 
*		height		-- image 1 height
*		baseAddress	-- image 1 buffer start address
*		bitPerPixel	-- how many bits per pixel
*		scale		-- the buffer hop num
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setImage1Param( U16 imgX, U16 imgY, U16 width, U16 height, U32 baseAddress, ePIXEL_INPUT bitPerPixel, U32 scale)
{
	LCDC_IMAGE_1_POSITION_REGISTER = ( imgY<<16 )|imgX;
	LCDC_IMAGE_1_SIZE_REGISTER = ( height <<16 )|width;
	LCDC_IMAGE_1_BASE_ADDRESS_0_REGISTER = baseAddress;
	LCDC_IMAGE_1_CTRL_REGISTER = (bitPerPixel);
	LCDC_IMAGE_1_SCALE_RATIO = scale;
}

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setImage2Param
* ����:	Setting the image 2 parameter
* ����:	imgX		-- image 2 start x position 
*		imgY		-- image 2 start y position 
*		width		-- image 2 width 
*		height		-- image 2 height
*		baseAddress	-- image 2 buffer start address
*		bitPerPixel	-- how many bits per pixel
*		scale		-- the buffer hop num
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setImage2Param( U16 imgX, U16 imgY, U16 width, U16 height, U32 baseAddress, ePIXEL_INPUT bitPerPixel, U32 scale)
{
	LCDC_IMAGE_2_POSITION_REGISTER = ( imgY<<16 )|imgX;
	LCDC_IMAGE_2_SIZE_REGISTER = ( height <<16 )|width;
	LCDC_IMAGE_2_BASE_ADDRESS_0_REGISTER = baseAddress;
	LCDC_IMAGE_2_CTRL_REGISTER = (bitPerPixel);
	LCDC_IMAGE_2_SCALE_RATIO = scale;
}

#else

#define NTSC_MODE_ENABLE_MY
//extern U8 gImage_b[781920];//238280781920];
extern U8* pimage0;
/*----------------------------------------------------------------------------
* ����:	hyhwLcdRgb_GpioSet()
* ����:	GPIO�˿ڸ�����������Ϊrgb lcd 
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcdRgb_GpioSet()
{
	hyhwGpio_setLcdrgbFunction();
}

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_clkSet()
* ����: ����lcd rgb ʱ�� 
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_clkSet( void )
{
	hyhwCpm_setLcdClk();	  			//���÷�Ƶֵ����lcdʱ��
	LCDC_CLK_DIVIDER_REGISTER = 0x0;	//2Mhz pxclk
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_lcdRgbConfig
* ����:	����lcd ��DMA
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwDma_lcdRgbConfig( eLCDBUSTYPE_EN  busType, U32 addr)
{
	
	tDMA_REGISTER	*ptDmaReg;

	ptDmaReg = (tDMA_REGISTER *)LCD_DMA_REGISTER_ADDR;
	
	ptDmaReg->SOURCE_PARAMETER = DMA_PORT_TYPE_HALF_WORD; 	/*DMAʱsource ����Ϊ1*/
	ptDmaReg->DESTINATION_PARAMETER = DMA_PORT_TYPE_STATIC; /*DMAʱDestination ����Ϊ0*/		
	if( busType == LcdBusType16 )
	{
		ptDmaReg->CHANNEL_CONTROL   = (  CHCTRL_BIT_TRANSFER_TYPE_SOFTWARE
									   | CHCTRL_BIT_BURST_SIZE_4
									   | CHCTRL_BIT_WORD_SIZE_16BIT	
									  );
	}
	else
	{
		ptDmaReg->CHANNEL_CONTROL   = (  CHCTRL_BIT_TRANSFER_TYPE_SOFTWARE
									   | CHCTRL_BIT_BURST_SIZE_4
									   | CHCTRL_BIT_WORD_SIZE1_32BIT	
									  );
	}
	ptDmaReg->DESTINATION_START_ADDR= addr;

}

/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_init
* ����:	��ʼ��
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_init( void )
{
//	hyhwDma_lcdRgbConfig(LcdBusType32, LCD_IMAGE1_BASE_ADDR );
	hyhwLcdRgb_GpioSet();//����GPIO����
	hyhwLcd_clkSet();
	
// ΪNTSC/PAL ����������, ����ΪNTSC	
#ifdef NTSC_MODE_ENABLE_MY
	//����LCD������������ʵ�ߴ��С��Ҳ���ԣֵ���ʵ�ߴ�
	hyhwLcd_setSize(LCD_CH, LCD_HSIZE_720, LCD_VSIZE_480);

	//����LHTIME ��LVTime�Ĵ���		
	hyhwLcd_setHorizontalTime( (LCD_HSIZE_720 << 1 ), 32, 212, 32 );
	hyhwLcd_setVerticalTime( 0, 0, LCD_VSIZE_480, 6,36, 3, LCD_VSIZE_480, 6,  37, 2 );
	
	//ΪNTSC����LCD���ƼĴ���
	LCDC_CTRL_REGISTER				  = (  TV_MODE | PIXEL_DAT_WIDTH(6) | FALLING_EDGE_PIXEL_OUT
										 | INVERTED_VERTICAL_SYNC | INVERTED_HORIZONTAL_SYNC | RGB2YUV_ENABLE
										);
#endif		

// ΪNTSC/PAL ����������, ����ΪPAL B/G/H/I,	
#ifdef PAL_MODE_ENABLE_MY		
	//����LCD������������ʵ�ߴ��С��Ҳ���ԣֵ���ʵ�ߴ�
	hyhwLcd_setSize( LCD_CH, LCD_HSIZE_720, LCD_VSIZE_576 );
	//����LHTIME ��LVTime�Ĵ���					
	hyhwLcd_setHorizontalTime((LCD_HSIZE_720 << 1 ), 2, 280, 6);					
	//hyhwLcd_setHorizontalTime(576 , 2, 140, 146);					
	hyhwLcd_setVerticalTime(0, 0, 576, 1, 43, 5, 576, 1, 44, 4);
	//hyhwLcd_setVerticalTime(0, 0, 0, 0, 0, 0, 288, 1, 22, 2);

	//ΪPAL����LCD���ƼĴ���
	LCDC_CTRL_REGISTER				  = (  TV_MODE | PIXEL_DAT_WIDTH(6) | FALLING_EDGE_PIXEL_OUT
										 | INVERTED_VERTICAL_SYNC | INVERTED_HORIZONTAL_SYNC | RGB2YUV_ENABLE
										);
			
#endif	
	hyhwLcd_setBackgroundColor( 0x0, 0x0, 0x0 );	//���ñ���ɫ
	hyhwLcd_setImage0Param( 0, 0, 720, 480,(U32)LCD_IMAGE0_BASE_ADDR,  PIXEL_565BPP, 0);/* set rgb 565 */
	
	//hyhwLcd_setImage1Param( 0, 0, LCD_HSIZE_720, LCD_VSIZE_480, LCD_IMAGE1_BASE_ADDR,  3, 0);
	//hyhwLcd_setImage2Param( 0, 0, LCD_HSIZE_720, LCD_VSIZE_480, LCD_IMAGE2_BASE_ADDR,  3, 0);
	//RGB 565
/*	LCDC_CTRL_REGISTER = ( 	PIXEL_CLOCK_MASK_RGB | PIXEL_DAT_WIDTH(6)|	\
							INVERTED_VERTICAL_SYNC | INVERTED_HORIZONTAL_SYNC | FALLING_EDGE_PIXEL_OUT | ONE_PIXEL_TWO_CLK	|\
							TV_MODE );
*/							
//	LCDC_SCALER_TO_LCDC_REGISTER = 1;
//	LCDC_PIP_CONTROL_REGISTE = 0; 			//disable PIP mode
	hyhwLcd_enableImage( IMG_CH0 );
	//hyhwLcd_enableImage( IMG_CH1 );
	//hyhwLcd_enableImage( IMG_CH2 );
	hyhwLcd_enableCtrler();					// LCDC������ʹ��
//	hyhwLcd_IntruptSource(NO_SOURCE_INT);	

}

/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_IntruptEnable();
* ����:	lcd �������ж�ʹ��
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_IntruptEnable(void)
{
	LCDC_STATUS_REGISTER = LCDC_BIT_INT_ENABLE;
}	

/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_IntruptDisable();
* ����:	lcd �������ж�ʹ��
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_IntruptDisable(void)
{
	LCDC_STATUS_REGISTER &= ~LCDC_BIT_INT_ENABLE;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_IntruptSource();
* ����:	lcd �������ж�Դѡ��
* ����:	sourceInt, �ж�Դ
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_IntruptSource(eLCD_SOURCE_INT sourceInt)
{
	if(sourceInt == MDD_SOURCE_INT)				// LSTATUS��DDλ�ж�Դ, disable done interrupt
	{
		LCDC_INTERRUPT_REGISTER  = 0xf;
	}else if(sourceInt == MRU_SOURCE_INT)		// LSTATUS��RUλ���ж�Դ, register update interrupt
	{
		LCDC_INTERRUPT_REGISTER = 0x11;	
	}
	else if(sourceInt == MFU_SOURCE_INT)		// LSTATUS��FUλ���ж�Դ, FIFO underruninterrupt
	{
		LCDC_INTERRUPT_REGISTER = 0x18;	
	}
	else
	{
		LCDC_INTERRUPT_REGISTER = 0x1f;			// �����жϾ����ε�	
	}
}

/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_enableCtrler
* ����:	lcd ������ʹ��
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_enableCtrler(void)
{
	LCDC_CTRL_REGISTER |= LCD_BIT_LCD_CONTROLLER_ENABLE; //enable lcd controller
}

/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_disableCtrler
* ����:	lcd ��������ʹ��
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_disableCtrler(void)
{
	LCDC_CTRL_REGISTER &= ~LCD_BIT_LCD_CONTROLLER_ENABLE; //enable lcd controller
}

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_enableImage
* ����:	enable the image channel
* ����:	ch		-- Image Type or LCD Panel 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_enableImage( eLCD_CH_TYPE ch )
{
	LCDC_CTRL_REGISTER |= 1<< ch;
}
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_disableImage
* ����:	disable the image channel
* ����:	ch		-- Image Type or LCD Panel 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_disableImage( eLCD_CH_TYPE ch )
{
	LCDC_CTRL_REGISTER &= ~(1<< ch);

}
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setSize
* ����:	Setting the LCD Size Information
* ����:	ch		-- Image Type or LCD Panel 
*		width	-- Horizontal Pixel Count
*		height	-- Vertical Line Count
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setSize( eLCD_CH_TYPE ch, U32 width, U32 height ) 
{
	switch(ch)
	{
		case	LCD_CH:
			LCDC_DISPLAY_SIZE_REGISTER = (height << 16) | width;
			break;
		case	IMG_CH0:
			LCDC_IMAGE_0_SIZE_REGISTER = (height << 16) | width;
			break;
		case	IMG_CH1:
			LCDC_IMAGE_1_SIZE_REGISTER = (height << 16) | width;
			break;
		case	IMG_CH2:
			LCDC_IMAGE_2_SIZE_REGISTER = (height << 16) | width;
			break;
		default :
			break;	
	}
	
	
	return ;
}
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setHorizontalTime
* ����:	Setting the Horizontal Timing Register
* ����:	pulseWidth	-- Horizontal Line Pulse Width
*		startCount	-- Horizontal Line Start Count
*		width		-- Width of the Displaying Window
*		endCount	-- Horizontal Line End Count
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setHorizontalTime(U32 pulseWidth, U32 startCount, U32 width, U32 endCount)
{
	LCDC_HORIZONTAL_TIMING_REGISTER_1 = ((width-1) << 16) | (pulseWidth - 1);
	
	LCDC_HORIZONTAL_TIMING_REGISTER_2 = ((startCount-1) << 16) | (endCount-1);
}

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setVerticalTime
* ����:	Setting the Vertical Timing Register
* ����:	vSyncPulseWidth		-- Number of Clock Pulse Width for VSYNC
*		height				-- Height of the Displaying Window
*		frameStartWaitCycle	-- Number of Wait Cycles after Starting the New Frame
*		frameEndWaitCycle	-- Number of Wait Cycles before Starting the New Frame
*		vSyncPulseWidth1	-- Number of Clock Pulse Width for VSYNC
*		height1				-- Height of the Displaying Window
*		frameStartWaitCycle1-- Number of Wait Cycles after Starting the New Frame
*		frameEndWaitCycle1	-- Number of Wait Cycles before Starting the New Frame
*		vSyncDelay			-- Number of VSYNC Delay after HSYNC		
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setVerticalTime(U32 vdb, U32 vdf, U32 vSyncPulseWidth, U32 height, U32 frameStartWaitCycle,  U32 frameEndWaitCycle, U32 vSyncPulseWidth1,  U32 height1,U32 frameStartWaitCycle1, U32	frameEndWaitCycle1)
{
	
	
	LCDC_VERTICAL_TIMING_REGISTER_1 = ((height-1) << 16) | ( vSyncPulseWidth- 1);
	LCDC_VERTICAL_TIMING_REGISTER_2 = ((frameStartWaitCycle-1) << 16) | (frameEndWaitCycle-1);
	LCDC_VERTICAL_TIMING_REGISTER_3 = ((height1-1) << 16) | (vSyncPulseWidth1 - 1);
	LCDC_VERTICAL_TIMING_REGISTER_4 = ((frameStartWaitCycle1-1) << 16) | (frameEndWaitCycle1-1);

}
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setClipping
* ����:	Setting the Clipping Informations
* ����:	uiUpperLimitG	-- green color up limit
*		uiLowerLimitG	-- green color lower limit
*		uiUpperLimitR	-- red color up limit 
*		uiLowerLimitR	-- red color lower limit
*		uiUpperLimitB	-- blue color up limit
*		uiLowerLimitB	-- blue color lower limit
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setClipping(U32 uiUpperLimitG, U32 uiLowerLimitG, U32 uiUpperLimitR, U32 uiLowerLimitR,U32 uiUpperLimitB, U32 uiLowerLimitB )
{
	LCDC_CLIPPING_REGISTER_1 = ((uiLowerLimitG << 24)|(uiUpperLimitG << 16)|(uiLowerLimitR << 8)|uiUpperLimitR);
	LCDC_CLIPPING_REGISTER_2 = ((uiLowerLimitB << 8)|uiUpperLimitB);

}
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setBackgroundColor
* ����:	Setting the back ground color 
* ����:	red		-- half word long input, the red grade 
*		green	-- half word long input, the green grade 
*		blue	-- half word long input, the blue grade 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setBackgroundColor( U8 red, U8 green, U8 blue )
{
	LCDC_BACKGROUND_COLOR_REGISTER = ( ( blue << 16 )|( green << 8 )| red );

}

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setImage0Param
* ����:	Setting the image 0 parameter
* ����:	imgX		-- image 0 start x position 
*		imgY		-- image 0 start y position 
*		width		-- image 0 width 
*		height		-- image 0 height
*		baseAddress	-- image 0 buffer start address
*		bitPerPixel	-- how many bits per pixel
*		scale		-- the buffer hop num
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setImage0Param( U16 imgX, U16 imgY, U16 width, U16 height, U32 baseAddress, ePIXEL_INPUT bitPerPixel, U32 scale)
{
	LCDC_IMAGE_0_POSITION_REGISTER = ( imgY<<16 )|imgX;
	LCDC_IMAGE_0_SIZE_REGISTER = ( height <<16 )|width;
	LCDC_IMAGE_0_BASE_ADDRESS_0_REGISTER = baseAddress;
	LCDC_IMAGE_0_CTRL_REGISTER = (bitPerPixel);
	LCDC_IMAGE_0_SCALE_RATIO = scale;
}
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setImage1Param
* ����:	Setting the image 1 parameter
* ����:	imgX		-- image 1 start x position 
*		imgY		-- image 1 start y position 
*		width		-- image 1 width 
*		height		-- image 1 height
*		baseAddress	-- image 1 buffer start address
*		bitPerPixel	-- how many bits per pixel
*		scale		-- the buffer hop num
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setImage1Param( U16 imgX, U16 imgY, U16 width, U16 height, U32 baseAddress, ePIXEL_INPUT bitPerPixel, U32 scale)
{
	LCDC_IMAGE_1_POSITION_REGISTER = ( imgY<<16 )|imgX;
	LCDC_IMAGE_1_SIZE_REGISTER = ( height <<16 )|width;
	LCDC_IMAGE_1_BASE_ADDRESS_0_REGISTER = baseAddress;
	LCDC_IMAGE_1_CTRL_REGISTER = (bitPerPixel);
	LCDC_IMAGE_1_SCALE_RATIO = scale;

}
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setImage2Param
* ����:	Setting the image 2 parameter
* ����:	imgX		-- image 2 start x position 
*		imgY		-- image 2 start y position 
*		width		-- image 2 width 
*		height		-- image 2 height
*		baseAddress	-- image 2 buffer start address
*		bitPerPixel	-- how many bits per pixel
*		scale		-- the buffer hop num
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setImage2Param( U16 imgX, U16 imgY, U16 width, U16 height, U32 baseAddress, ePIXEL_INPUT bitPerPixel, U32 scale)
{
	LCDC_IMAGE_2_POSITION_REGISTER = ( imgY<<16 )|imgX;
	LCDC_IMAGE_2_SIZE_REGISTER = ( height <<16 )|width;
	LCDC_IMAGE_2_BASE_ADDRESS_0_REGISTER = baseAddress;
	LCDC_IMAGE_2_CTRL_REGISTER = (bitPerPixel);
	LCDC_IMAGE_2_SCALE_RATIO = scale;
}

void hyhwLcd_setImage0Color( U32 color, U16 imgX, U16 imgY )
{
	U32 height,width;
	U32 *reg = (U32 *)(LCDC_IMAGE_1_BASE_ADDRESS_0_REGISTER);
	height = LCDC_IMAGE_1_SIZE_REGISTER;
	width = height & 0x0000ffff;
	height >>= 16;
	reg += (imgX )+( (height-1) - imgY )*width;
	*reg = (color);
}

void hyhwLcd_setImage1Color( U32 color, U16 imgX, U16 imgY )
{
	
	U32 height,width;
	U32 *reg = (U32 *)(LCDC_IMAGE_1_BASE_ADDRESS_0_REGISTER);
	height = LCDC_IMAGE_1_SIZE_REGISTER;
	width = height & 0x0000ffff;
	height >>= 16;
	reg += (imgX )+( (height-1) - imgY )*width;
	*reg = (color);
	
	
}
void hyhwLcd_setImage2Color( U32 color, U16 imgX, U16 imgY )
{
	U32 height,width;
	U16 *reg = (U16 *)(LCDC_IMAGE_2_BASE_ADDRESS_0_REGISTER);
	height = LCDC_IMAGE_2_SIZE_REGISTER;
	width = height & 0x0000ffff;
	height >>= 16;
	reg += (imgX )+( (height-1) - imgY )*width;
	*reg = (color);
}

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setImageColor
* ����:	Setting the image color
* ����:	imgX		-- image start x position ( 0 <= x <=799 )
*		imgY		-- image start y position (	0 <= y <=479 )
*		width		-- image width 
*		height		-- image height
*		*color		-- image buffer start address
*		size		-- the image size
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setImageColor( U16 * color, U16 imgX, U16 imgY , U16 width, U16 height, U32 size )
{
	U32 i;
	U16 *reg = (U16 *)(LCDC_IMAGE_2_BASE_ADDRESS_0_REGISTER);
	reg += (imgX )+( (480-1) - imgY )*800;
	
	while( size >= width )
	{	
		for( i = 0; i < width; i++, reg ++,	color ++ )
		{		
			* reg = * color;
		}
		size -= width;
		reg += ( 800 - width );
	}
	
	for( i = 0; i < size; i++, reg ++,	color ++ )
	{		
		* reg = * color;
	}
}
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_RGB666
* ����:	��RGB565����д��RGB666 Ȼ��DMA ������
* ����:	imgX		-- image start x position ( 0 <= x <=799 )
*		imgY		-- image start y position (	0 <= y <=479 )
*		width		-- image width 
*		height		-- image height
*		*color		-- image buffer start address
*		size		-- the image size
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_RGB666Refresh( U16 * color, U32 pictureAddr0, U32 pictureAddr1)
{

	U32 i,data;
	U32 *rgb666Buff;//rgb buff!!!
	static U32 j = 0; 
	
//	U32 * rgb666Buff_2 = (U32 *)0x25100000;
//	(U16 *)(LCDC_IMAGE_2_BASE_ADDRESS_0_REGISTER);
//	U16 *reg = (U16 *)(LCDC_IMAGE_2_BASE_ADDRESS_0_REGISTER);
#if 1
	if( j == 0 )
	{
		j = 1;
		rgb666Buff = (U32 *)pictureAddr0;
	}
	else
	{
		j = 0;
		rgb666Buff = (U32 *)pictureAddr1;
	}	
#endif
	
	for( i = 0; i < 384000; i ++)
	{
		data =  color[i];
		rgb666Buff[i] = (((data&0x1f000) <<2) | ((data & 0x7ff)<<1));
	}
//	syssleep(10);
#if 0
	if( j == 0 )
	{
		TCC7901_LCDC_LI1BA0_REGISTER = pictureAddr0;
	}
	else
	{
		TCC7901_LCDC_LI1BA0_REGISTER = pictureAddr1;
	}
#else
	TCC7901_LCDC_LI1BA0_REGISTER = (U32)rgb666Buff;
#endif
		

//	TCC7901_LCDC_LI1BA0_REGISTER = 0x20c00000;
//	TCC7901_LCDC_LCTRL_REGISTER |= (BIT2);  //open
	
/*	{
		int i;
		
		rgb666Buff = (U32 *)0x20F00000;
		for (i=0;i<800*480;i++)
		{
			*rgb666Buff++ = 0x0003F000;//red
			
		}
	}
*/	
//	rgb666Buff = (U32 *)0x20F00000;//rgb buff!!!
//	hyhwLcd_RGBEnable( ( U32 )0x24f00000, ( ) );


}
void test_RGB(void)
{
	

//	U32 x,y;
//	U16* RGB565bufer;
	hyhwLcd_init();
	
	//hyhwLcd_setImageColor( (U16 *)0xf800, 320, 480, 320, 240, 320*240 );
}
#endif