/******************************************************************************
*  (C) Copyright 2005 Shanghai Hyctron Electronic Design House, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of Shanghai Hyctron Electronic Design House and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  Hyctron and is confidential in nature.  Its use and dissemination by    
*  any party other than Hyctron is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/

#ifndef HY_HWLCD_H_
#define HY_HWLCD_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define	LCD_LCDSI_CTRL0_REGISTER 	TCC7901_LCD_LCDSI_CTRL0_REGISTER	/* Control register for LCDSI */
	#define	LCDSI_CTRL0_BIT_IMCPU	~BIT0								/* Input mode ,use cpu lcd, set this bit to 0, converts on-chip cpu signal to 68/80 interaface signal */
	#define LCDSI_CTRL0_BIT_IMRGB	BIT0								/* Input mode ,use rgb lcd, set this bit to 1 */
	#define LCDSI_CTRL0_BIT_OM80	~BIT1								/* Output mode, 0 output 80-system interface signal, 1 output 68-system interface signal */
	#define LCDSI_CTRL0_BIT_OM68	BIT1								/* Output mode, 0 output 80-system interface signal, 1 output 68-system interface signal */

#define	LCD_LCDSI_CTRL1_REGISTER 	TCC7901_LCD_LCDSI_CTRL1_REGISTER    /* Control register for LCSN[0] when LXA=0 (for core access) */
#define	LCD_LCDSI_CTRL2_REGISTER    TCC7901_LCD_LCDSI_CTRL2_REGISTER    /* Control register for LCSN[0] when LXA=1 (for core access) */
#define	LCD_LCDSI_CTRL3_REGISTER    TCC7901_LCD_LCDSI_CTRL3_REGISTER    /* Control register for LCSN[1] when LXA=0 (for core access) */
#define	LCD_LCDSI_CTRL4_REGISTER    TCC7901_LCD_LCDSI_CTRL4_REGISTER    /* Control register for LCSN[1] when LXA=1 (for core access) */
//	#define LCDSI_BITMASK_WIDTH_32		(1<<31)						/* ��λΪ1������ 32bit memory mapped I/O devices */
//	#define LCDSI_BITMASK_WIDTH_16		((0x1<<15)|(0x0<<31)) 		/* ���� 16bit memory mapped I/O devices */
//	#define LCDSI_BITMASK_WIDTH_8		((0x0<<15)|(0x0<<31)) 		/* ���� 8bit memory mapped I/O devices */
	#define LCDSI_BITMASK_WIDTH_32		BIT31 		   				/* ��λΪ1������ 32bit memory mapped I/O devices */
	#define LCDSI_BITMASK_WIDTH_16		BIT15 						/* ���� 16bit memory mapped I/O devices */
	#define LCDSI_BITMASK_WIDTH_8		~(0x80008000)				/* ���� 8bit memory mapped I/O devices */
	
	#define LCDSI_BITMASK_WSETUP_TIME		(0x7 << 28)		/* д ��������ʱ�� 3 bits mask */
	#define LCDSI_BITMASK_WPULSE_WIDTH		(0x1ff << 19)	/* д����������  9 bits mask */
	#define	LCDSI_BITMASK_HOLD_TIME			(0x7 << 16)		/* д��������ʱ��  3 bits mask */
	#define LCDSI_BIT_READ_SETUP_TIME		(0x7 << 12)		/* �� ��������ʱ�� 3 bits mask */ 
	#define LCDSI_BIT_READ_PULSE_WIDTH		(0x1ff << 3)	/* ������������  9 bits mask */
	#define	LCDSI_BIT_READ_HOLD_TIME		0x7				/* ����������ʱ��  3 bits mask */


#if 1//RGB��

	#define LCD_LCDSI_LCSN0													// ����Ӳ����·ͼ�豸cs Ƭѡ�źŽӵ�LCSN0 ������
	#ifdef LCD_LCDSI_LCSN0	
		#define 	LCD_CMD		*((volatile U16 *)(0xf0000810))             // дLCD����
	#define		drvLcd_WriteInstruct(u16Instruct)	(LCD_CMD = u16Instruct)
#elif defined LCD_LCDSI_LCSN1
	#define 	LCD_CMD		*((volatile U16 *)(0xf0000820))
	#define		drvLcd_WriteInstruct(u16Instruct)	(LCD_CMD = u16Instruct)
#endif

#ifdef LCD_LCDSI_LCSN0	
		#define 	LCD_DATA	*((volatile U16 *)(0xf0000818))				// дLCD ����
	#define		drvLcd_WriteData(u16Data)			(LCD_DATA = u16Data)
	#define		drvLcd_ReadData(pu16Data)			(*pu16Data = LCD_DATA)
#elif defined LCD_LCDSI_LCSN1
	#define 	LCD_DATA	*((volatile U16 *)(0xf0000828))
	#define		drvLcd_WriteData(u16Data)			(LCD_DATA = u16Data)
	#define		drvLcd_ReadData(pu16Data)			(*pu16Data = LCD_DATA)
#endif	

#ifdef LCD_LCDSI_LCSN0	
	#define		LCD_DATA_ADDRESS		    	(0xf0000818)
#elif defined LCD_LCDSI_LCSN1
	#define		LCD_DATA_ADDRESS		    	(0xf0000828)
#endif	

#endif//RGB��

typedef enum elcd_inmodel
{
	LCD_CPU = 0,
	LCD_RGB
}eLCD_INMODEL;

typedef enum elcd_outmodel
{
	LCD_MODEL_80= 0,
	LCD_MODEL_68

}eLCD_OUTMODEL; 

typedef enum elcd_buswidth
{
	LCD_BUS_WIDTH_8 = 0,
	LCD_BUS_WIDTH_16,
	LCD_BUS_WIDTH_32

}eLCD_BUSWIDTH;
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_onChipInit
* ����:	��ʼ�� on chip lcd ����
* ����:	inMode0--ѡ��lcdΪrgb ���� cpu
*		outMode--ѡ�����Ϊ68ϵͳ�ӿڻ���80ϵͳ�ӿ�
*		busWidth--ѡ�����߿��8/16/32
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_onChipInit(eLCD_INMODEL inMode, eLCD_OUTMODEL outMode, eLCD_BUSWIDTH busWidth);
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_Set_Writ_SetupTime
* ����:	set lcd_bus setup time  
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritSetupTime(U32 setupTime);

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_Set_Writ_HoldTime
* ����: set lcd_bus hold time  
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritHoldTime(U32 holdTime); 

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_Set_Writ_PulseWidth
* ����:	set lcd_bus pulse width   
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritPulseWidth(U32 pulseWidth);

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setReadSetupTime
* ����:	set lcd_bus setup time  
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritReadTime(U32 setupTime);

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setReadHoldTime
* ����: set lcd_bus hold time  ע��hold_time ʱ�䵥λΪ us
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setReadHoldTime(U32 holdTime);

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setReadPulseWidth
* ����:	set lcd_bus pulse width   ʱ�䵥λΪ us
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setReadPulseWidth(U32 pulseWidth);

#ifdef __cplusplus
}
#endif

#endif //HY_HWLCD_H_