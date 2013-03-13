/*******************************************************************************
*  (C) Copyright 2009 Shanghai Hyctron Electronic Ltd, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of Shanghai Hyctron Electronic Design House and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  Hyctron and is confidential in nature.  Its use and dissemination by    
*  any party other than Hyctron is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*  ˵����telechip TCC7901 оƬ���жϿ�������������
*******************************************************************************/

#ifndef HYHW_INTA_CTRL_H_
#define HYHW_INTA_CTRL_H_

#ifdef __cplusplus
extern "C" {	/*  Assume C declarations for C++ */
#endif



#define ISR_TIMER0_NUM	5

typedef void (* isr_function)(void);		//uart1ɨ��


extern isr_function pisr_timer0[];



/*  ����ֻ�� Audio Subsystem (codec) ��handler���ܻ�䣬����ֻ��һ������ָ�뼴��
	����Ƶ����£���Ƶ�Ľ���Ҳ��ͨ���жϴ�������Ҳ��һ������ָ�룬���������
	ָ�벻Ϊ��ʱ���ͽ�����Ƶ���� */
extern void (* isr_audio_codec ) ( void );

extern void (* isr_audio_decoder ) ( void );
extern void (* isr_timer1)(void);
extern void (* isr_timer2)(void);
extern void (* isr_timer3)(void);
extern void (* isr_timer5)(void); 
extern void (* isr_key_scan_timer0)(void);//����ɨ��

extern void (* isr_gpio)(void);
extern void (* isr_uart0)(void);
extern void (* isr_uart1)(void);
extern void (* isr_uart2)(void);
extern void (* isr_uart3)(void);
extern void (* isr_sdio0)(void);
extern void (* isr_sdio1)(void);
extern void (* isr_usb)(void);

extern void (* isr_camera)(void);
extern void (* isr_enable_sdio)(void);


extern void (* isr_ppp_time)(void);//ppp��ʱ��

extern void (* isr_uartDma)(void);
extern void (* isr_spiDma)(void);

extern void (* isr_spiTDBoot)(void);
extern void (* isr_spiTDPcm)(void);
extern void (* isr_spiGsmPcm)(void);

extern void (* isr_spi)(void);
extern void (* isr_rtc)(void);//rtc�ж� alarmclock RTC
extern void (* isr_adc)(void);
extern void (* isr_afs_check)(void);

extern void (* isr_scanner_frame)(void);
extern void (* isr_scanner_data)(void);
	
/*	PIC(programble Interrupt control)�жϼĴ��� */
#define	INT_ENABLE_REGISTER			TCC7901_PIC_IEN_REGISTER	/*	�ж�ʹ�ܼĴ��� 	R/W, Interrupt Enable Register */			
	/*	ͨ�õ�λ���壬���������µļĴ��������������������
	�ж�ʹ�ܼĴ���(IEN)			bit��1ʹ���ж�		bit��0��ֹ�ж� 
	�ж�����Ĵ���(CLR)			bit��1����ж�
	�ж�״̬�Ĵ���(STS)			bit��1�жϼ���
	�ж�ѡ��Ĵ���(SEL)			bit��1ѡ��IRQ		bit��0ѡ��FIQ 
	�ж�Դ״̬�Ĵ���(SRC)		���ж�����ͼ��ԼĴ���XOR ����ʾ�ж�Դ��״̬
	����״̬�Ĵ���(MSTS)		��ʹ�ܵ��ж�Դ����ʾ�ж�״̬�����ڽ�ֹ���жϣ���Ӧλ��0 
	�����жϲ����Ĵ���(TST)		����SRC�Ĵ�������1���ж�Դ�����˼Ĵ�����Ӧλд1���������ж�
	�жϼ��ԼĴ���(POL)			bit��1��ʾ����Ч��bit��0��ʾ����Ч
	IRQԭʼ״̬�Ĵ���(IRQ)
	FIQԭʼ״̬�Ĵ���(FIQ)
	����IRQ״̬�Ĵ���(MIRQ) 	IRQ�Ĵ���������״̬
	����FIQ״̬�Ĵ���(MFIQ) 	FIQ�Ĵ���������״̬
	�жϴ���ģʽ�Ĵ���(MODE)	bit��1��ʾѡ���ƽ������bit��0��ʾѡ��edge����
	ͬ��ʹ�ܼĴ���(SYNC)		bit��1��ʾ�ж�Դ��busʱ��HCLKͬ��
	����ʹ�ܼĴ���(WKEN) 		bit��0��ʾCPU clock�� bus clock�ɱ���Ӧ���жϻ���
	˫�߻򵥱߼Ĵ���(MODEA)		����edge����ģʽ�£�bit��0��ʾ�ж���single edgeʱ����FIQ��IRQ��bit��1��ʾ
								both edge ʱ����IRQ��FIQ
	�ж�������μĴ���(INTMSK)	bit��1��ʾ��Ӧ���ж�ת����IRQ��FIQ	*/						
	#define INT_BIT_SELECT4		BIT31	/* refer to IRQSEL3 				*/
	#define	INT_BIT_ADC			BIT30	/* ADC interrupt enable				*/		
	#define	INT_BIT_LCD			BIT29   /* LCD controller interrupt enable 	*/		
	#define	INT_BIT_CAMERA		BIT28   /* Camera interrupt enable 			*/		
	#define	INT_BIT_SD		    BIT27   /* SD/MMC interrupt enable 			*/		
	#define	INT_BIT_NANDFLASH   BIT26   /* Nandflash controller Interrupt enable*/		
	#define	INT_BIT_SELECT3    	BIT25   /* refer to IRQSEL3  				*/		
	#define	INT_BIT_HDD	  	    BIT24   /* HDD contrller Interrupt enable	*/	
	#define	INT_BIT_DMA		    BIT23   /* DMA contrller interrupt enable 	*/		
	#define	INT_BIT_USB_HOST	BIT22   /* USB 1.1 host interrupt enable 	*/	
	#define	INT_BIT_USB_DEVICE	BIT21   /* USB 2.0 device interrupt enable 	*/		
	#define	INT_BIT_GRAPHIC_2D	BIT20   /* Graphic 2D Interrupt enable, ע�� POL�Ĵ�����ӦλӦ��Ϊ1 */		
	#define	INT_BIT_SELECT2		BIT19   /* refer to IRQSEL2  				*/		
	#define	INT_BIT_UART		BIT18   /* UART interrupt enable 			*/		
	#define	INT_BIT_HPI			BIT17   /* EHI Channel 0 interrupt enable 	*/		
	#define	INT_BIT_CDRX		BIT16   /* CDIF receive interrupt enable	*/		
	#define	INT_BIT_DAITX		BIT15   /* DAI transmit interrupt enable 	*/		
	#define	INT_BIT_DAIRX		BIT14   /* DAI receive interrupt enable 	*/		
	#define	INT_BIT_I2C			BIT13   /* I2C interrupt enable				*/		
	#define	INT_BIT_SC		    BIT12   /* Mem-to Mem scalerinterrupt enable*/		
	#define	INT_BIT_SELECT1	    BIT11   /* refer to IRQSEL1 				*/		
	#define	INT_BIT_SELECT0	    BIT10   /* refer to IRQSEL0 				*/		
	#define	INT_BIT_SPD_TX	    BIT9    /* SPDIF Transmitter interrupt enable*/		
	#define	INT_BIT_SCORE	   	BIT8    /* Sub Processor Interrupt enable	*/		
	#define	INT_BIT_TIMER1		BIT7    /* Timer 1 interrupt enable		*/		
	#define	INT_BIT_TIMER0		BIT6    /* Timer 0 interrupt enable		*/		
	#define	INT_BIT_GPSB		BIT5    /* GPSB interrupt enable 		*/		
	#define	INT_BIT_RTC			BIT4	/* RTC interrupt enable 		*/		
	#define	INT_BIT_EI3			BIT3	/* External Interrupt 3 enable	*/ 											
	#define	INT_BIT_EI2			BIT2	/* External Interrupt 2 enable	*/ 											
	#define	INT_BIT_EI1			BIT1	/* External Interrupt 1enable	*/ 		
	#define	INT_BIT_EI0			BIT0	/* External Interrupt 0 enable	*/ 							
#define	INT_CLEAR_REGISTER			TCC7901_PIC_CLR_REGISTER	/*	�ж�����Ĵ���  	R/W, Interrupt Clear Register*/
#define	INT_STATUS_REGISTER			TCC7901_PIC_STS_REGISTER	/*	�ж�״̬�Ĵ���  	R, Interrupt Status Register */
#define	INT_IRQFIQ_SELECT_REGISTER	TCC7901_PIC_SEL_REGISTER	/*	IRQ/FIQѡ��Ĵ���	R/W, IRQ/FIQ Selection Register */
#define	INT_SOURCE_REGISTER			TCC7901_PIC_SRC_REGISTER	/*	Դ�ж�״̬�Ĵ���	R, Source Interrupt Status Register */
#define	INT_MASKED_STATUS_REGISTER	TCC7901_PIC_MSTS_REGISTER	/*	����״̬�Ĵ��� 		R, Masked Status Register */		
#define	INT_TEST_REGISTER			TCC7901_PIC_TIG_REGISTER	/*	�����жϲ����Ĵ���	R/W, Test Interrupt Generation Register*/		
#define	INT_POLARITY_REGISTER		TCC7901_PIC_POL_REGISTER	/*	�жϼ��ԼĴ���		R/W, Interrupt Polarity Register*/	 
#define	INT_RAWIRQ_REGISTER			TCC7901_PIC_IRQ_REGISTER	/*	IRQԭʼ״̬�Ĵ���	R, IRQ Raw Status Register*/	
#define	INT_RAWFIQ_REGISTER			TCC7901_PIC_FIQ_REGISTER	/*	FIQԭʼ״̬�Ĵ���  	R, FIQ Status Register*/	
#define	INT_MASKIRQ_REGISTER		TCC7901_PIC_MIRQ_REGISTER	/*	����IRQ״̬�Ĵ���	R, Masked IRQ Status Register*/	 
#define	INT_MASKFIQ_REGISTER		TCC7901_PIC_MFIQ_REGISTER	/*	����FIQ״̬�Ĵ���	R, Masked FIQ Status Register*/	 
#define	INT_TRIGGER_MODE_REGISTER	TCC7901_PIC_MODE_REGISTER	/*	�жϴ���ģʽ�Ĵ���	R/W, Trigger Mode Register 0:edge, 1:level */	    
#define	INT_SYNC_REGISTER			TCC7901_PIC_SYNC_REGISTER	/*	ͬ��ʹ�ܼĴ���  	R/W, Synchronizer Enable Register  */  
#define	INT_WAKEUP_REGISTER			TCC7901_PIC_WKEN_REGISTER	/*	����ʹ�ܼĴ���  	R/W, Wakeup Enable Register  */	
#define	INT_MODEA_REGISTER			TCC7901_PIC_MODEA_REGISTER	/*	˫�߻򵥱߼Ĵ���	R/W, Both Edge or Single Edge Register */	  
#define	INT_INTMASK_REGISTER		TCC7901_PIC_INTMSK_REGISTER	/*	�ж�������μĴ���	R/W, Interrupt Output Masking Register */
#define	INT_ALLMASK_REGISTER		TCC7901_PIC_ALLMSK_REGISTER	/*	�ж�ȫ���μĴ���	R/W, All Mask Register */
	#define INT_BIT_ALLMASK_FIQ		BIT1	/* ��0��ʾ IRQ�жϱ�����	*/
	#define	INT_BIT_ALLMASK_IRQ		BIT0	/* ��0��ʾ FIQ�жϱ�����	*/		
	

/* VIC��Vectored Interrupt Controller�� �Ĵ���*/									
#define	INT_IRQ_VECTOR_REGISTER		TCC7901_VIC_VAIRQ_REGISTER/*	IRQ�����Ĵ���		R, IRQ Vector Register */		
	/* 4��������������Ŀ�Ĵ���ͨ��bit���壬*/
	#define INT_BIT_VECTOR_INV			BIT31	/* 1/0 - invalid/valid for current Interrupt Source */
	#define	INT_BITMASK_VECTOR_VAVN		(BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)		/* interrupt Vector Address Offset in word-addrss, ���� 0��4��8	*/		
#define	INT_FIQ_VECTOR_REGISTER		TCC7901_VIC_VAFIQ_REGISTER/*	FIQ�����Ĵ���		R, FIQ Vector Register*/	 
#define	INT_IRQ_VECTOR_NUM_REGISTER	TCC7901_VIC_VNIRQ_REGISTER/*	IRQ���������Ĵ���	R, IRQ Vector Number Register */	  
#define	INT_FIQ_VECTOR_NUM_REGISTER	TCC7901_VIC_VNFIQ_REGISTER/*	FIQ���������Ĵ���	R, FIQ Vector Number Register */	  

#define	INT_VECTOR_CONTROL_REGISTER	TCC7901_VIC_VCTRL_REGISTER/*	�������ƼĴ���		R/W, Vector control Register */  
	#define INT_BIT_VEC_CTRL_RCL			BIT31	
	#define INT_BIT_VEC_CTRL_FPOL			BIT30	
	#define INT_BIT_VEC_CTRL_FFLG			BIT29	
	#define INT_BIT_VEC_CTRL_IFLG			BIT28	
	#define INT_BIT_VEC_CTRL_FHD			BIT27	
	#define INT_BIT_VEC_CTRL_IHD			BIT26	

#define	INT_PRIORITY0_3_REGISTER	TCC7901_VIC_PRI04_REGISTER/*	�ж�0-3�����ȼ�  	R/W, Priorityes for Interrupt 0~3*/	 
	/* 8�����ȼ��Ĵ�����ͨ��λ���� */
	#define	INT_BITMASK_PRIORITY_PRI3		(BIT28|BIT27|BIT26|BIT25|BIT24)				
	#define	INT_BITMASK_PRIORITY_PRI2		(BIT20|BIT19|BIT18|BIT17|BIT16)				
	#define	INT_BITMASK_PRIORITY_PRI1		(BIT12|BIT11|BIT10|BIT9 |BIT8 )				
	#define	INT_BITMASK_PRIORITY_PRI0		(BIT4 |BIT3 |BIT2 |BIT1 |BIT0)				
#define	INT_PRIORITY4_7_REGISTER	TCC7901_VIC_PRI08_REGISTER/*	�ж�4-7�����ȼ�  	R/W, Priorityes for Interrupt 4~7*/	 
#define	INT_PRIORITY8_11_REGISTER	TCC7901_VIC_PRI12_REGISTER/*	�ж�8-11�����ȼ�  	R/W, Priorityes for Interrupt 8~11*/	  
#define	INT_PRIORITY12_15_REGISTER	TCC7901_VIC_PRI16_REGISTER/*	�ж�12-15�����ȼ�  	R/W, Priorityes for Interrupt 12~15*/	  
#define	INT_PRIORITY16_19_REGISTER	TCC7901_VIC_PRI20_REGISTER/*	�ж�16-19�����ȼ�  	R/W, Priorityes for Interrupt 16~19*/	  
#define	INT_PRIORITY20_23_REGISTER	TCC7901_VIC_PRI24_REGISTER/*	�ж�20-23�����ȼ�  	R/W, Priorityes for Interrupt 20~23*/	  
#define	INT_PRIORITY24_27_REGISTER	TCC7901_VIC_PRI28_REGISTER/*	�ж�24-27�����ȼ�  	R/W, Priorityes for Interrupt 24~27*/	   
#define	INT_PRIORITY28_31_REGISTER	TCC7901_VIC_PRI32_REGISTER/*	�ж�28-31�����ȼ�  	R/W, Priorityes for Interrupt 28~31*/	   


/* PORT�ġ����ж���صļĴ��� */
#define	INT_EXTERNEL_SELECT0_REGISTER	TCC7901_PORT_EINTSEL0_REGISTER	/*	�ⲿ�ж�ѡ��Ĵ���0�������ⲿ�ж�1-3��Դ��ѡ��	R, External Interrupt select0 Register */
	#define	INT_BITMASK_EXT_INT0		(BIT4 |BIT3 |BIT2 |BIT1 |BIT0 )		
	#define	INT_BITMASK_EXT_INT1		(BIT12|BIT11|BIT10|BIT9 |BIT8 )		
	#define	INT_BITMASK_EXT_INT2		(BIT20|BIT19|BIT18|BIT17|BIT16)		
	#define	INT_BITMASK_EXT_INT3		(BIT28|BIT27|BIT26|BIT25|BIT24)		
#define	INT_EXTERNEL_SELECT1_REGISTER	TCC7901_PORT_EINTSEL1_REGISTER	/*	�ⲿ�ж�ѡ��Ĵ���1�������ⲿ�ж�4-7��Դ��ѡ��	R, External Interrupt select0 Register */
	#define	INT_BITMASK_EXT_INT4		(BIT4 |BIT3 |BIT2 |BIT1 |BIT0 )		
	#define	INT_BITMASK_EXT_INT5		(BIT12|BIT11|BIT10|BIT9 |BIT8 )		
	#define	INT_BITMASK_EXT_INT6		(BIT20|BIT19|BIT18|BIT17|BIT16)		
	#define	INT_BITMASK_EXT_INT7		(BIT28|BIT27|BIT26|BIT25|BIT24)		
	
	/* �ⲿ�жϵľ����ֵ,���赽2���ⲿѡ���жϼĴ����е������0-7���ⲿѡ��λ�õľ����ֵ */
	#define	INT_EXT_SELECT_USBVB_OFF		31	/*external int�� Ϊż����int��Ϊ0-7*/		
	#define	INT_EXT_SELECT_USBVB_ON			31	/*external int�� Ϊ������int��Ϊ0-7*/	
	#define	INT_EXT_SELECT_PMKUP			30		
	#define	INT_EXT_SELECT_LPD23			29		
	#define	INT_EXT_SELECT_LPD18			28		
	#define	INT_EXT_SELECT_GPIOC30			27		
	#define	INT_EXT_SELECT_GPIOC27			26		
	#define	INT_EXT_SELECT_GPIOC26			25		
	#define	INT_EXT_SELECT_HPCSN_L			24		
	#define	INT_EXT_SELECT_HPRDN			23		
	#define	INT_EXT_SELECT_HPXD11			22		
	#define	INT_EXT_SELECT_HPXD3			21		
	#define	INT_EXT_SELECT_SCMD0			20	
	#define	INT_EXT_SELECT_SDO0				19	
	#define	INT_EXT_SELECT_GPIOA2			18		
	#define	INT_EXT_SELECT_GPIOA3			17		
	#define	INT_EXT_SELECT_GPIOA5			16		
	#define	INT_EXT_SELECT_GPIOB0			15		
	#define	INT_EXT_SELECT_GPIOB3			14		
	#define	INT_EXT_SELECT_GPIOB4			13		
	#define	INT_EXT_SELECT_GPIOB13			12		
	#define	INT_EXT_SELECT_GPIOB14			11		
	#define	INT_EXT_SELECT_GPIOB15			10		
	#define	INT_EXT_SELECT_SCMD1			9	
	#define	INT_EXT_SELECT_SDO1				8	
	#define	INT_EXT_SELECT_GPIOA6			7		
	#define	INT_EXT_SELECT_GPIOA7			6		
	#define	INT_EXT_SELECT_GPIOA11			5		
	#define	INT_EXT_SELECT_AIN0				4		
	#define	INT_EXT_SELECT_AIN1				3		
	#define	INT_EXT_SELECT_AIN7				2		
	#define	INT_EXT_SELECT_UTXD2			1		
	#define	INT_EXT_SELECT_URXD3			0		
	
#define	INT_IRQ_SELECT_REGISTER			TCC7901_PORT_IRQSEL_REGISTER	/*	IRQѡ��Ĵ�����R/W, External Interrupt select0 Register */
	#define	INT_BITMASK_IRQSEL_0		(BIT1|BIT0)		
		/* IRQ SELECT0 �����ֵ */	
		#define	INT_IRQSEL0_VIDEO_CORE		0		
		#define	INT_IRQSEL0_EHICH0_CTRL		1		
		#define	INT_IRQSEL0_EI4				2		
	#define	INT_BITMASK_IRQSEL_1		(BIT3|BIT2)	
		/* IRQ SELECT1 �����ֵ */	
		#define	INT_IRQSEL1_GPSB			0		
		#define	INT_IRQSEL1_EHICH1_CTRL		1		
		#define	INT_IRQSEL1_EI5				2		
	#define	INT_BITMASK_IRQSEL_2		(BIT5|BIT4)	
		/* IRQ SELECT2 �����ֵ */	
		#define	INT_IRQSEL2_UART			0		
		#define	INT_IRQSEL2_EHICH1_INTERNAL	1		
	#define	INT_BITMASK_IRQSEL_3		(BIT7|BIT6)		
		/* IRQ SELECT3 �����ֵ */	
		#define	INT_IRQSEL3_MEM_STICK		0		
		#define	INT_IRQSEL3_EI6				1		
	#define	INT_BITMASK_IRQSEL_4		(BIT9|BIT8)	
		/* IRQ SELECT4 �����ֵ */	
		#define	INT_IRQSEL4_GPSB			0		
		#define	INT_IRQSEL4_ECC				1		
		#define	INT_IRQSEL4_EI7				2		
		#define	INT_IRQSEL4_USB20_DMA		3		

/*-----------------------------------------------------------------------------------*/

/*  ����ֻ�� Audio Subsystem (codec) ��handler���ܻ�䣬����ֻ��һ������ָ�뼴��
	����Ƶ����£���Ƶ�Ľ���Ҳ��ͨ���жϴ�������Ҳ��һ������ָ�룬���������
	ָ�벻Ϊ��ʱ���ͽ�����Ƶ���� */
extern void (* isr_audio_codec ) ( void );

extern void (* isr_audio_decoder ) ( void );
extern void (* isr_timer0)(void);
extern void (* isr_timer1)(void);
extern void (* isr_timer2)(void);
extern void (* isr_key_scan_timer0)(void);//����ɨ��

extern void (* isr_gpio)(void);
extern void (* isr_uart0)(void);
extern void (* isr_uart1)(void);
extern void (* isr_uart2)(void);
extern void (* isr_uart3)(void);
extern void (* isr_sdio0)(void);
extern void (* isr_sdio1)(void);


extern void (* isr_spi)(void);
extern void (* isr_camera)(void);
extern void (* isr_usb)(void);
extern void (* isr_usbHost)(void);//usb host stack 20100427
//extern void (* isr_lcdDmaShow)(void);//RGB lcd Show dma intp

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_init
* ����:	��ʼ������ֹ�����ж�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_init ( void );

/*-----------------------------------------------------------------------------
* ��  :	hyhwInt_enable
* ����:	ʹ�ܵ�ָ��Դ���ж�
* ����:	intSourceMask--�ж�Դ������,�����Ƕ���жϵĻ�ֵ
*----------------------------------------------------------------------------*/
#define	hyhwInt_enable(intSourceMask) 	INT_ENABLE_REGISTER |= intSourceMask;

/*-----------------------------------------------------------------------------
* ��  :	hyhwInt_disable
* ����:	��ָֹ��Դ���ж�
* ����:	intSourceMask--�ж�Դ������,�����Ƕ���жϵĻ�ֵ
*----------------------------------------------------------------------------*/
#define	hyhwInt_disable(intSourceMask)	INT_ENABLE_REGISTER &= ~(intSourceMask);

/*-----------------------------------------------------------------------------
* ��  :	hyhwInt_clear
* ����:	���ָ��Դ���жϱ��
* ����:	intSourceMask--�ж�Դ������,�����Ƕ���жϵĻ�ֵ
*----------------------------------------------------------------------------*/
#define	hyhwInt_clear(intSourceMask)	INT_CLEAR_REGISTER |= intSourceMask;

/*-----------------------------------------------------------------------------
* ��  :	hyhwInt_getStatus
* ����:	����ָ��Դ���жϵ�����
* ����:	intSourceMask--�ж�Դ������,�����Ƕ���жϵĻ�ֵ
*----------------------------------------------------------------------------*/
#define	hyhwInt_getStatus(intSourceMask)	(INT_STATUS_REGISTER & intSourceMask);


/*-----------------------------------------------------------------------------
* ��  :	hyhwInt_setIRQ
*		hyhwInt_setFIQ
* ����:	����ָ��Դ���жϵ�ΪIRQ/FIQ����
* ����:	intSourceMask--�ж�Դ������,�����Ƕ���жϵĻ�ֵ
*----------------------------------------------------------------------------*/
#define	hyhwInt_setIRQ(intSourceMask)	INT_IRQFIQ_SELECT_REGISTER |= intSourceMask;
#define	hyhwInt_setFIQ(intSourceMask)	INT_IRQFIQ_SELECT_REGISTER &= ~(intSourceMask);

/*-----------------------------------------------------------------------------
* ��  :	hyhwInt_getMaskedRequest
* ����:	����ָ��Դ���жϵ�����
* ����:	intSourceMask--�ж�Դ������,�����Ƕ���жϵĻ�ֵ
*----------------------------------------------------------------------------*/
#define	hyhwIntA_getMaskedStatus(intSourceMask)		(INT_MASKED_STATUS_REGISTER & intSourceMask)

/*-----------------------------------------------------------------------------
* ��  :	hyhwInt_setTriggerLevel
*		hyhwInt_setTriggerEdge
* ����:	����ָ��Դ���жϵĴ�������Ϊedge/level����
* ����:	intSourceMask--�ж�Դ������,�����Ƕ���жϵĻ�ֵ
*----------------------------------------------------------------------------*/
#define	hyhwInt_setTriggerLevel(intSourceMask)	INT_TRIGGER_MODE_REGISTER |= intSourceMask
#define	hyhwInt_setTriggerEdge(intSourceMask)	INT_TRIGGER_MODE_REGISTER &= ~(intSourceMask)

/*-----------------------------------------------------------------------------
* ��  :	hyhwInt_setActiveLow
*		hyhwInt_setActiveHigh
* ����:	����A��/B���ָ��Դ���жϵ���Ч����Ϊlow/high���ͣ�Ĭ��Ϊactive high
* ����:	intSourceMask--�ж�Դ������,�����Ƕ���жϵĻ�ֵ
*----------------------------------------------------------------------------*/
#define	hyhwInt_setActiveLow(intSourceMask)		INT_POLARITY_REGISTER |= intSourceMask
#define	hyhwInt_setActiveHigh(intSourceMask)	INT_POLARITY_REGISTER &= ~(intSourceMask)

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigCodecISR
* ����:	��������Audio Subsubsystem �ڲ��š�¼��������в�ͬ��ISR���������ң�����
*		�����ͬ��ISRҲ���в�ͬ�����ԣ���Ҫ����AudioSubsytem��ISR������
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigCodecISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigAudioDecISR
* ����:	���Ž��������жϺ���
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigAudioDecISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigTimer1_ISR
* ����:	���� timer1 ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigTimer1_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigKeyScanTimer0_ISR
* ����:	���� KeyScanTimer0 ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigKeyScanTimer0_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigGpio_ISR
* ����:	���� Gpio ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigGpio_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigUart0_ISR
* ����:	���� Uart ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart0_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigUart1_ISR
* ����:	���� Uart ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart1_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigUart2_ISR
* ����:	���� Uart ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart2_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigUart_ISR
* ����:	���� Uart ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart_ISR(U8 id,  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigSdio0_ISR
* ����:	���� Sdio0 ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSdio0_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigSdio1_ISR
* ����:	���� Sdio1 ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSdio1_ISR(  void (*pIntVector) (void));

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigCamera_ISR
* ����:	���� Sdio1 ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigCamera_ISR(  void (*pIntVector) (void));

#ifdef __cplusplus
}
#endif

/*@}*/

#endif /* HYHW_INTA_CTRL_H_ */
