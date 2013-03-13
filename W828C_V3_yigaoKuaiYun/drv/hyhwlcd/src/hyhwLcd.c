
/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"
/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "hyhwLcd.h"
#include "hyhwGpio.h"
#include "hyhwCkc.h"

/*----------------------------------------------------------------------------
* ����:	void hyhwLcd_GpiopinSet(void)
* ����:	����GPIO���Ź���Ϊcpu lcd
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/

void hyhwLcd_GpiopinSet(void)
{
	hyhwGpio_setLcdcpuFunction(); 	
}								 

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_onChipInit
* ����:	��ʼ�� on chip lcd ����
* ����:	inMode0--ѡ��lcdΪrgb ���� cpu
*		outMode--ѡ�����Ϊ68ϵͳ�ӿڻ���80ϵͳ�ӿ�
*		busWidth--ѡ�����߿��8/16/32
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_onChipInit(eLCD_INMODEL inMode, eLCD_OUTMODEL outMode, eLCD_BUSWIDTH busWidth)  
{
	hyhwCpm_busClkEnable(CKC_LCD);
	hyhwCpm_setLcdClk();					/* ��ʼ��lcd ʱ�� */
	hyhwLcd_GpiopinSet(); 					/* initial lcd port */
	
	//��������ģʽ
	if(inMode == LCD_CPU)
	{
		LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_IMCPU;	// ����Ϊcpu lcd ��
	}
	else
	{
		LCD_LCDSI_CTRL0_REGISTER |= LCDSI_CTRL0_BIT_IMRGB;	// ����Ϊcpu rgb ��
	}
	
	//�����������
	if(outMode == LCD_MODEL_80)
	{
		LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_OM80;	// ����Ϊ80 system interface
	}		
	else
	{
		LCD_LCDSI_CTRL0_REGISTER |= LCDSI_CTRL0_BIT_OM68;	// ����Ϊ68 system interface
	}
	
	//�������߿��
	if(busWidth == LCD_BUS_WIDTH_8)
	{
		LCD_LCDSI_CTRL1_REGISTER &= LCDSI_BITMASK_WIDTH_8;	// �������߿��8
		LCD_LCDSI_CTRL2_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		LCD_LCDSI_CTRL3_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		LCD_LCDSI_CTRL4_REGISTER &= LCDSI_BITMASK_WIDTH_8;
	}
	else if(busWidth == LCD_BUS_WIDTH_16)
	{
		LCD_LCDSI_CTRL1_REGISTER &= LCDSI_BITMASK_WIDTH_8;	// �������λ
		LCD_LCDSI_CTRL2_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		LCD_LCDSI_CTRL3_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		LCD_LCDSI_CTRL4_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		
		LCD_LCDSI_CTRL1_REGISTER |= LCDSI_BITMASK_WIDTH_16;	// �������߿��16
		LCD_LCDSI_CTRL2_REGISTER |= LCDSI_BITMASK_WIDTH_16;
		LCD_LCDSI_CTRL3_REGISTER |= LCDSI_BITMASK_WIDTH_16;
		LCD_LCDSI_CTRL4_REGISTER |= LCDSI_BITMASK_WIDTH_16;
		
	}
	else
	{
		LCD_LCDSI_CTRL1_REGISTER &= LCDSI_BITMASK_WIDTH_8;	// �������λ
		LCD_LCDSI_CTRL2_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		LCD_LCDSI_CTRL3_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		LCD_LCDSI_CTRL4_REGISTER &= LCDSI_BITMASK_WIDTH_8;
		
		LCD_LCDSI_CTRL1_REGISTER |= LCDSI_BITMASK_WIDTH_32;	// �������߿��32
		LCD_LCDSI_CTRL2_REGISTER |= LCDSI_BITMASK_WIDTH_32;
		LCD_LCDSI_CTRL3_REGISTER |= LCDSI_BITMASK_WIDTH_32;
		LCD_LCDSI_CTRL4_REGISTER |= LCDSI_BITMASK_WIDTH_32;
		
	}
}	

/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setWritSetupTime
* ����:	set lcd_bus setup time  
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritSetupTime(U32 setupTime)  
{
	U32 lcdClk,clkTime,setupValue;
	
	LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_IMCPU;	
	
	lcdClk  = hyhwCpm_Get_LcdClk();                 		/* ���LCD CLK */
	setupValue = (lcdClk /1000) * setupTime ;				/* ��� clk = 1Mhz ,time = 1us ��ô setupValue =1 */	
	
	if(setupValue > 7)	setupValue = 7;
	
#ifdef LCD_CS0
	
	LCD_LCDSI_CTRL1_REGISTER = (setupValue << 28);
	LCD_LCDSI_CTRL2_REGISTER = (setupValue << 28);
	
#elif defined LCD_CS1
	LCD_LCDSI_CTRL3_REGISTER = (setupValue << 28);	
	LCD_LCDSI_CTRL3_REGISTER = (setupValue << 28);
#endif
}
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setWritHoldTime
* ����: set lcd_bus hold time  ע��hold_time ʱ�䵥λΪ us
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritHoldTime(U32 holdTime)  
{
	U32 lcdClk,clkTime,holdValue;
	
	LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_IMCPU;	
	
	lcdClk  = hyhwCpm_Get_LcdClk();                 		/* ���LCD CLK */
	holdValue = (lcdClk /1000) * holdTime ;		 			/* ��� clk = 1Mhz ,time = 1us ��ô holdValue =1 */	
	
	if(holdValue > 7)	holdValue = 7;
	
#ifdef LCD_CS0
	
	LCD_LCDSI_CTRL1_REGISTER = (holdValue << 16);
	LCD_LCDSI_CTRL2_REGISTER = (holdValue << 16);
	
#elif defined LCD_CS1
	LCD_LCDSI_CTRL3_REGISTER = (holdValue << 16);	
	LCD_LCDSI_CTRL4_REGISTER = (holdValue << 16);
#endif
}
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setWritPulseWidth
* ����:	set lcd_bus pulse width   ʱ�䵥λΪ us
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritPulseWidth(U32 pulseWidth)
{
	U32 lcdClk,clkTime,pulseValue;
	
	LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_IMCPU;	
	
	lcdClk  = hyhwCpm_Get_LcdClk();                  		/* ���LCD CLK */
	pulseValue = (lcdClk /1000) * pulseWidth ;		 		/* ��� clk = 1Mhz ,time = 1ms ��ô pulseValue =1000 */	
	
	if(pulseValue > 0x1ff)	pulseValue = 0x1ff;
	
#ifdef LCD_CS0
	
	LCD_LCDSI_CTRL1_REGISTER = (pulseValue << 19);
	LCD_LCDSI_CTRL2_REGISTER = (pulseValue << 19);
	
#elif defined LCD_CS1
	LCD_LCDSI_CTRL3_REGISTER = (pulseValue << 19);	
	LCD_LCDSI_CTRL4_REGISTER = (pulseValue << 19);
#endif

}



/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setReadSetupTime
* ����:	set lcd_bus setup time  
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setWritReadTime(U32 setupTime)  
{
	U32 lcdClk,clkTime,setupValue;
	
	LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_IMCPU;	
	
	lcdClk  = hyhwCpm_Get_LcdClk();                 		/* ���LCD CLK */
	setupValue = (lcdClk /1000) * setupTime ;				/* ��� clk = 1Mhz ,time = 1us ��ô setupValue =1 */	
	
	if(setupValue > 7)	setupValue = 7;
	
#ifdef LCD_CS0
	
	LCD_LCDSI_CTRL1_REGISTER = (setupValue << 12);
	LCD_LCDSI_CTRL2_REGISTER = (setupValue << 12);
	
#elif defined LCD_CS1
	LCD_LCDSI_CTRL3_REGISTER = (setupValue << 12);	
	LCD_LCDSI_CTRL3_REGISTER = (setupValue << 12);
#endif
}
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setReadHoldTime
* ����: set lcd_bus hold time  ע��hold_time ʱ�䵥λΪ us
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setReadHoldTime(U32 holdTime)  
{
	U32 lcdClk,clkTime,holdValue;
	
	LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_IMCPU;	
	
	lcdClk  = hyhwCpm_Get_LcdClk();                 		/* ���LCD CLK */
	holdValue = (lcdClk /1000) * holdTime ;		 			/* ��� clk = 1Mhz ,time = 1us ��ô holdValue =1 */	
	
	if(holdValue > 7)	holdValue = 7;
	
#ifdef LCD_CS0
	
	LCD_LCDSI_CTRL1_REGISTER = (holdValue << 0);
	LCD_LCDSI_CTRL2_REGISTER = (holdValue << 0);
	
#elif defined LCD_CS1
	LCD_LCDSI_CTRL3_REGISTER = (holdValue << 0);	
	LCD_LCDSI_CTRL4_REGISTER = (holdValue << 0);
#endif
}
/*----------------------------------------------------------------------------
* ����:	hyhwLcd_setReadPulseWidth
* ����:	set lcd_bus pulse width   ʱ�䵥λΪ us
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_setReadPulseWidth(U32 pulseWidth)
{
	U32 lcdClk,clkTime,pulseValue;
	
	LCD_LCDSI_CTRL0_REGISTER &= LCDSI_CTRL0_BIT_IMCPU;	
	
	lcdClk  = hyhwCpm_Get_LcdClk();                  		/* ���LCD CLK */
	pulseValue = (lcdClk /1000) * pulseWidth ;				/* ��� clk = 1Mhz ,time = 1ms ��ô pulseValue =1000 */	
	
	if(pulseValue > 0x1ff)	pulseValue = 0x1ff;
	
#ifdef LCD_CS0
	
	LCD_LCDSI_CTRL1_REGISTER = (pulseValue << 3);
	LCD_LCDSI_CTRL2_REGISTER = (pulseValue << 3);
	
#elif defined LCD_CS1
	LCD_LCDSI_CTRL3_REGISTER = (pulseValue << 3);	
	LCD_LCDSI_CTRL4_REGISTER = (pulseValue << 3);
#endif

}


void hyhwLcd_setBusTime( U32  hClk )
{
	
	if( hClk <= 48 )
	{
		//LCD_CONTROL_REGISTER_2 = 0x90099021;
		//LCD_LCDSI_CTRL1_REGISTER = 0xA0099021;
//			LCD_LCDSI_CTRL2_REGISTER = 0x20429021;
		LCD_LCDSI_CTRL2_REGISTER = 0x90099021;
	}
	else if( hClk <= 96 )
	{
		//LCD_CONTROL_REGISTER_2 = 0xa0399021;
		//LCD_LCDSI_CTRL1_REGISTER = 0xB0399021;
		LCD_LCDSI_CTRL2_REGISTER = 0xa0399021;
	}
	else
	{
		//LCD_CONTROL_REGISTER_2 = 0xb04a9021;0xA0089021
		//LCD_LCDSI_CTRL1_REGISTER = 0xC04a9021;
		LCD_LCDSI_CTRL2_REGISTER = 0xb04a9021;
	}	
}

/* end of file */	
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		