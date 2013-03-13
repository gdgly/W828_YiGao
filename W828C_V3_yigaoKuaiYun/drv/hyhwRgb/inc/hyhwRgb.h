/*************************************************************************
 * DEFINES
 *************************************************************************/
#define LCD_HSIZE_320	320
#define LCD_VSIZE_240	240
#define IMAGE0_HIGH		240
#define IMAGE0_WIDTH	320

#define LCD_HSIZE_800	800
#define LCD_HSIZE_720	720
#define LCD_VSIZE_480	480
#define LCD_VSIZE_576	576
#define LCD_HSIZE_768	768

//kong
#define LCD_HSIZE		320//��
#define LCD_VSIZE		240//��

/* register definition */
#define	LCDC_CTRL_REGISTER							TCC7901_LCDC_LCTRL_REGISTER			/* LCD Control Register */
	#define PIXEL_CLOCK_MASK_RGB					BIT23
	#define INVERTED_DATA_ENABLE					BIT15
	#define INVERTED_VERTICAL_SYNC					BIT14
	#define INVERTED_HORIZONTAL_SYNC				BIT13
	#define FALLING_EDGE_PIXEL_OUT					BIT12
	#define CLIPPING_ENABLE							BIT11
	#define RGB2YUV_ENABLE							BIT10
	
	#define PIXEL_DATA_WIDTH_FOR_OUTPUT_18BIT_RGB	(BIT18|BIT16)
	#define PIXEL_DATA_WIDTH_FOR_OUTPUT_16BIT_RGB	(BIT17|BIT16)
	#define PIXEL_DAT_WIDTH(X)						(X<<16)
	#define ONE_PIXEL_TWO_CLK						BIT9
	#define NON_INTERLACE							BIT8
	#define TV_MODE									BIT7
	#define TFT_LCD_MODE							BIT6
	#define STN_LCD_MODE							BIT5
	
	#define FETCH_ENABLE_2							BIT3
	#define FETCH_ENABLE_1							BIT2
	#define FETCH_ENABLE_0							BIT1
	#define LCD_BIT_LCD_CONTROLLER_ENABLE			BIT0
#define	LCDC_BACKGROUND_COLOR_REGISTER 				TCC7901_LCDC_LBC_REGISTER			/* LCD Background Color Register */
#define	LCDC_CLK_DIVIDER_REGISTER					TCC7901_LCDC_LCLKDIV_REGISTER		/* LCD Clock Divider Register */
#define	LCDC_HORIZONTAL_TIMING_REGISTER_1			TCC7901_LCDC_LHTIME1_REGISTER		/* LCD Horizontal Timing Register 1 */
#define	LCDC_HORIZONTAL_TIMING_REGISTER_2			TCC7901_LCDC_LHTIME2_REGISTER		/* LCD Horizontal Timing Register 2 */
#define	LCDC_VERTICAL_TIMING_REGISTER_1				TCC7901_LCDC_LVTIME1_REGISTER		/* LCD Vertical Timing Register 1 */
#define	LCDC_VERTICAL_TIMING_REGISTER_2				TCC7901_LCDC_LVTIME2_REGISTER		/* LCD Vertical Timing Register 2 */
#define	LCDC_VERTICAL_TIMING_REGISTER_3				TCC7901_LCDC_LVTIME3_REGISTER		/* LCD Vertical Timing Register 3 */
#define	LCDC_VERTICAL_TIMING_REGISTER_4				TCC7901_LCDC_LVTIME4_REGISTER		/* LCD Vertical Timing Register 4 */
#define	LCDC_LOOKUP_REGISTER_FOR_RED				TCC7901_LCDC_LLUTR_REGISTER			/* LCD Lookup Register for Red */
#define	LCDC_LOOKUP_REGISTER_FOR_GREEN				TCC7901_LCDC_LLUTG_REGISTER			/* LCD Lookup Register for Green */
#define	LCDC_LOOKUP_REGISTER_FOR_BLUE				TCC7901_LCDC_LLUTB_REGISTER			/* LCD Lookup Register for Blue */
#define	LCDC_MODULO_7_DITHERING_PATTERN_LOW			TCC7901_LCDC_LDP7L_REGISTER			/* LCD Modulo 7 Dithering Pattern (low) */
#define	LCDC_MODULO_7_DITHERING_PATTERN_HIGH		TCC7901_LCDC_LDP7H_REGISTER			/* LCD Modulo 7 Dithering Pattern (high) */
#define	LCDC_MODULO_5_DITHERING_PATTERN_REGISTER	TCC7901_LCDC_LDP5_REGISTER			/* LCD Modulo 5 Dithering Pattern Register */
#define	LCDC_MODULO_4_DITHERING_PATTERN_REGISTER	TCC7901_LCDC_LDP4_REGISTER			/* LCD Modulo 4 Dithering Pattern Register */
#define	LCDC_3_BIT_DITHERING_PATTERN_REGISTER		TCC7901_LCDC_LDP3_REGISTER			/* LCD 3-bit Dithering Pattern Register */
#define	LCDC_CLIPPING_REGISTER_1					TCC7901_LCDC_LCP1_REGISTER			/* LCD Clipping Register1 */
#define	LCDC_CLIPPING_REGISTER_2					TCC7901_LCDC_LCP2_REGISTER			/* LCD Clipping Register2 */
#define	LCDC_KEYING_REGISTER_1						TCC7901_LCDC_LK1_REGISTER			/* LCD Keying Register 1 */
#define	LCDC_KEYING_REGISTER_2						TCC7901_LCDC_LK2_REGISTER			/* LCD Keying Register 2 */
#define	LCDC_KEYING_MASK_REGISTER_1					TCC7901_LCDC_LKM1_REGISTER			/* LCD Keying Mask Register 1 */
#define	LCDC_KEYING_MASK_REGISTER_2					TCC7901_LCDC_LKM2_REGISTER			/* LCD Keying Mask Register 2 */
#define	LCDC_DISPLAY_SIZE_REGISTER					TCC7901_LCDC_LDS_REGISTER			/* LCD Display Size Register */
#define	LCDC_STATUS_REGISTER						TCC7901_LCDC_LSTATUS_REGISTER		/* LCD Status Register */
	#define LCDC_BIT_INT_CLEAR						BIT8								/* ���LCDC�жϱ�־��д��1�������ж� */
	#define LCDC_BIT_INT_ENABLE						BIT12
#define	LCDC_INTERRUPT_REGISTER						TCC7901_LCDC_LIM_REGISTER			/* LCD Interrupt Register */
#define	LCDC_IMAGE_0_CTRL_REGISTER					TCC7901_LCDC_LI0C_REGISTER			/* LCD Image 0 Control Register */
#define	LCDC_IMAGE_0_POSITION_REGISTER				TCC7901_LCDC_LI0P_REGISTER			/* LCD Image 0 Position Register */
#define	LCDC_IMAGE_0_SIZE_REGISTER					TCC7901_LCDC_LI0S_REGISTER			/* LCD Image 0 Size Register */
#define	LCDC_IMAGE_0_BASE_ADDRESS_0_REGISTER		TCC7901_LCDC_LI0BA0_REGISTER		/* LCD Image 0 Base Address 0 Register */
#define	LCDC_IMAGE_0_CURRENT_ADDRESS_REGISTER		TCC7901_LCDC_LI0CA_REGISTER			/* LCD Image 0 Current Address Register */
#define	LCDC_IMAGE_0_BASE_ADDRESS_1_REGISTER		TCC7901_LCDC_LI0BA1_REGISTER		/* LCD Image 0 Base Address 1 Register */
#define	LCDC_IMAGE_0_BASE_ADDRESS_2_REGISTER		TCC7901_LCDC_LI0BA2_REGISTER		/* LCD Image 0 Base Address 2 Register */
#define	LCDC_IMAGE_0_OFFSET_REGISTER				TCC7901_LCDC_LI0O_REGISTER			/* LCD Image 0 Offset Register */
#define	LCDC_IMAGE_0_SCALE_RATIO					TCC7901_LCDC_LI0SR_REGISTER			/* LCD Image 0 scale ratio */
#define	LCDC_IMAGE_1_CTRL_REGISTER					TCC7901_LCDC_LI1C_REGISTER			/* LCD Image 1 Control Register */
#define	LCDC_IMAGE_1_POSITION_REGISTER				TCC7901_LCDC_LI1P_REGISTER			/* LCD Image 1 Position Register */
#define	LCDC_IMAGE_1_SIZE_REGISTER					TCC7901_LCDC_LI1S_REGISTER			/* LCD Image 1 Size Register */
#define	LCDC_IMAGE_1_BASE_ADDRESS_0_REGISTER		TCC7901_LCDC_LI1BA0_REGISTER		/* LCD Image 1 Base Address 0 Register */
#define	LCDC_IMAGE_1_CURRENT_ADDRESS_REGISTER		TCC7901_LCDC_LI1CA_REGISTER			/* LCD Image 1 Current Address Register */
#define	LCDC_IMAGE_1_OFFSET_REGISTER				TCC7901_LCDC_LI1O_REGISTER			/* LCD Image 1 Offset Register */
#define	LCDC_IMAGE_1_SCALE_RATIO					TCC7901_LCDC_LI1SR_REGISTER			/* LCD Image 1 Scale ratio */
#define	LCDC_IMAGE_2_CTRL_REGISTER					TCC7901_LCDC_LI2C_REGISTER			/* LCD Image 2 Control Register */
#define	LCDC_IMAGE_2_POSITION_REGISTER				TCC7901_LCDC_LI2P_REGISTER			/* LCD Image 2 Position Register */
#define	LCDC_IMAGE_2_SIZE_REGISTER					TCC7901_LCDC_LI2S_REGISTER			/* LCD Image 2 Size Register */
#define	LCDC_IMAGE_2_BASE_ADDRESS_0_REGISTER		TCC7901_LCDC_LI2BA0_REGISTER		/* LCD Image 2 Base Address 0 Register */
#define	LCDC_IMAGE_2_CURRENT_ADDRESS_REGISTER		TCC7901_LCDC_LI2CA_REGISTER			/* LCD Image 2 Current Address Register. */
#define	LCDC_IMAGE_2_OFFSET_REGISTER				TCC7901_LCDC_LI2O_REGISTER			/* LCD Image 2 Offset Register */
#define	LCDC_IMAGE_2_SCALE_RATIO					TCC7901_LCDC_LI2SR_REGISTER			/* LCD Image 2 Scale ratio */

#define	DUAL_LCD_CONTROL_REGISTER					TCC7901_LCDC_DLCTRL_REGISTER		/* Dual LCD Control Register */
#define	DUAL_LCD_CONFIGURATION_START_ADDRESS_0		TCC7901_LCDC_DLCSA0_REGISTER		/* Dual LCD Configuration Start Address 0 */
#define	DUAL_LCD_CONFIGURATION_START_ADDRESS_1		TCC7901_LCDC_DLCSA1_REGISTER		/* Dual LCD Configuration Start Address 1 */

#define	LCDC_YCbYc_TO_RGB_REGISTER					TCC7901_LCDC_Y2RC_REGISTER			/* YCbCr2RGB Control register */
#define LCDC_SCALER_TO_LCDC_REGISTER				TCC7901_LCDC_S2LC_REGISTER			/* Scaler to LCDC control register */
#define LCDC_PIP_CONTROL_REGISTE					TCC7901_LCDC_LPMCTRL_REGISTER		/* LCD PIP control register */
#define LCDC_PIP_WINDOW_REGISTER_0					TCC7901_LCDC_LPMW0_REGISTER			/* LCD PIP window register 0 */
#define LCDC_PIP_WINDOW_REGISTER_1					TCC7901_LCDC_LPMW1_REGISTER			/* LCD PIP window register 1 */
#define LCDC_PIP_HEIGHT_REGISTER					TCC7901_LCDC_LPMH_REGISTER			/* LCD PIP Height register */
#define LCDC_PIP_Chroma_Key_REGISTER				TCC7901_LCDC_LPMC_REGISTER			/* LCD PIP Chroma-keying register */
#define LCDC_PIP_Chroma_Key_MASK_REGISTER			TCC7901_LCDC_LPMMC_REGISTER			/* LCD PIP Chroma-keying Mask register */

#define LCDC_LOOKUP_TABLE							TCC7901_LCDC_LCDLUT_REGISTER		/* LCD lookup table */

/*************************************************************************
 * typedef
 *************************************************************************/

typedef enum 
{
	LCD_CH,
	IMG_CH0,	
	IMG_CH1,	
	IMG_CH2	
			
}eLCD_CH_TYPE;

typedef enum 
{
	PIXEL_1BBP,
	PIXEL_2BBP,
	PIXEL_4BBP,
	PIXEL_RGB332_OR_YCbCr,
	PIXEL_444BPP,
	PIXEL_565BPP,	
	PIXEL_555BPP,	
	PIXEL_888BPP,	
	PIXEL_666BPP		
}ePIXEL_INPUT;

typedef enum
{
	MDD_SOURCE_INT,// LSTATUS��DDλ�ж�Դ, disable done interrupt 
	MRU_SOURCE_INT,// LSTATUS��RUλ���ж�Դ, register update interrupt
	MFU_SOURCE_INT,// LSTATUS��FUλ���ж�Դ, FIFO underruninterrupt
	NO_SOURCE_INT  // �����жϾ����ε�
}eLCD_SOURCE_INT;

/*************************************************************************
 * macro DEFINES   
 *************************************************************************/

#define hyhwmLcd_InterruptClear()	(LCDC_STATUS_REGISTER |= LCDC_BIT_INT_CLEAR)

 #if 1

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_clkSet()
* ����: ����lcd rgb ʱ�� 
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_clkSet( void );
void hyhwLcd_setVerticalTime(U32 vdb, U32 vdf, U32 fpw, U32 height, U32 fswc,  U32 fewc);
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setHorizontalTime
* ����:	Setting the Horizontal Timing Register
* ����:	pulseWidth	-- Horizontal Line Pulse Width
*		startCount	-- Horizontal Line Start Count
*		width		-- Width of the Displaying Window
*		endCount	-- Horizontal Line End Count
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setHorizontalTime(U32 lpw, U32 width, U32 lswc, U32 lewc);
/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_init
* ����:	��ʼ��
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_init( void );
/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_enableCtrler
* ����:	lcd ������ʹ��
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_enableCtrler(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_disableCtrler
* ����:	lcd ��������ʹ��
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_disableCtrler(void);
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setBackgroundColor
* ����:	Setting the back ground color 
* ����:	red		-- half word long input, the red grade 
*		green	-- half word long input, the green grade 
*		blue	-- half word long input, the blue grade 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setBackgroundColor( U8 red, U8 green, U8 blue );
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setSize
* ����:	Setting the LCD Size Information
* ����:	ch		-- Image Type or LCD Panel 
*		width	-- Horizontal Pixel Count
*		height	-- Vertical Line Count
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setSize( eLCD_CH_TYPE ch, U32 width, U32 height );
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_enableImage
* ����:	enable the image channel
* ����:	ch		-- Image Type or LCD Panel 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_enableImage( eLCD_CH_TYPE ch );
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
void hyhwLcd_setImage0Param( U16 imgX, U16 imgY, U16 width, U16 height, U32 baseAddress, ePIXEL_INPUT bitPerPixel, U32 scale);

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
void hyhwLcd_setImage1Param( U16 imgX, U16 imgY, U16 width, U16 height, U32 baseAddress, ePIXEL_INPUT bitPerPixel, U32 scale);
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
void hyhwLcd_setImage2Param( U16 imgX, U16 imgY, U16 width, U16 height, U32 baseAddress, ePIXEL_INPUT bitPerPixel, U32 scale);
#else

/*************************************************************************
 * function DEFINES
 ******************	*******************************************************/
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setHorizontalTime
* ����:	Setting the Horizontal Timing Register
* ����:	pulseWidth	-- Horizontal Line Pulse Width
*		startCount	-- Horizontal Line Start Count
*		width		-- Width of the Displaying Window
*		endCount	-- Horizontal Line End Count
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setHorizontalTime(U32 pulseWidth, U32 startCount, U32 width, U32 endCount);

/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_IntruptEnable();
* ����:	lcd �������ж�ʹ��
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_IntruptEnable(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_IntruptDisable();
* ����:	lcd �������ж�ʹ��
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_IntruptDisable(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_IntruptSource();
* ����:	lcd �������ж�Դѡ��
* ����:	sourceInt, �ж�Դ
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_IntruptSource(eLCD_SOURCE_INT sourceInt);

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
void hyhwLcd_setVerticalTime(U32 vdb, U32 vdf, U32 vSyncPulseWidth, U32 height, U32 frameStartWaitCycle,  U32 frameEndWaitCycle, U32 vSyncPulseWidth1,  U32 height1,U32 frameStartWaitCycle1, U32	frameEndWaitCycle1);

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
void hyhwLcd_setClipping(U32 uiUpperLimitG, U32 uiLowerLimitG, U32 uiUpperLimitR, U32 uiLowerLimitR,U32 uiUpperLimitB, U32 uiLowerLimitB );

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setSize
* ����:	Setting the LCD Size Information
* ����:	ch		-- Image Type or LCD Panel 
*		width	-- Horizontal Pixel Count
*		height	-- Vertical Line Count
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setSize( eLCD_CH_TYPE ch, U32 width, U32 height ); 

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setBackgroundColor
* ����:	Setting the back ground color 
* ����:	red		-- half word long input, the red grade 
*		green	-- half word long input, the green grade 
*		blue	-- half word long input, the blue grade 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setBackgroundColor( U8 red, U8 green, U8 blue );

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
void hyhwLcd_setImage0Param( U16 imgX, U16 imgY, U16 height, U16 width, U32 baseAddress, ePIXEL_INPUT bitPerPixel, U32 scale);

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
void hyhwLcd_setImage1Param( U16 imgX, U16 imgY, U16 height, U16 width, U32 baseAddress, ePIXEL_INPUT bitPerPixel, U32 scale);

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
void hyhwLcd_setImage2Param( U16 imgX, U16 imgY, U16 height, U16 width, U32 baseAddress, ePIXEL_INPUT bitPerPixel, U32 scale);

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_enableImage
* ����:	enable the image channel
* ����:	ch		-- Image Type or LCD Panel 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_enableImage( eLCD_CH_TYPE ch );

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_disableImage
* ����:	disable the image channel
* ����:	ch		-- Image Type or LCD Panel 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_disableImage( eLCD_CH_TYPE ch );

/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_enableCtrler
* ����:	lcd ������ʹ��
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_enableCtrler(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_disableCtrler
* ����:	lcd ��������ʹ��
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwLcd_disableCtrler(void);
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
void hyhwLcd_setImageColor( U16 * color, U16 imgX, U16 imgY , U16 width, U16 height, U32 size );

#endif