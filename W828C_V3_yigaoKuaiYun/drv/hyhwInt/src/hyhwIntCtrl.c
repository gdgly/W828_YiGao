
/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyErrors.h"

/*---------------------------------------------------------------------------
   Project include files:
   --------------------------------------------------------------------------*/
  
#include "hyhwChip.h"
#include "hyhwIntCtrl.h"

#ifdef __cplusplus
  /* Assume C declarations for C++ */
  extern "C" {
#endif

/*---------------------------------------------------------------------------
   Global variables:
   --------------------------------------------------------------------------*/


isr_function pisr_timer0[ISR_TIMER0_NUM];


/*  ����ֻ�� Audio Subsystem (codec) ��handler���ܻ�䣬����ֻ��һ������ָ�뼴��
	����Ƶ����£���Ƶ�Ľ���Ҳ��ͨ���жϴ�������Ҳ��һ������ָ�룬���������
	ָ�벻Ϊ��ʱ���ͽ�����Ƶ���� */
void (* isr_audio_codec ) ( void );
void (* isr_audio_decoder ) ( void );
void (* isr_audio_encoder )(void);
void (* isr_timer0)(void);
void (* isr_timer1)(void);
void (* isr_timer2)(void);
void (* isr_timer3)(void);
void (* isr_timer5)(void);
void (* isr_key_scan_timer0)(void);//����ɨ��

void (* isr_gpio)(void);
void (* isr_uart0)(void);
void (* isr_uart1)(void);
void (* isr_uart2)(void);
void (* isr_uart3)(void);
void (* isr_sdio0)(void);
void (* isr_sdio1)(void);
void (* isr_usb)(void);
void (* isr_enable_sdio)(void);

void (* isr_camera)(void);

void (* isr_ppp_time)(void);//pppЭ��Ķ�ʱ��

void (* isr_uartDma)(void);	// �����������õ�DMA����
void (* isr_spiDma)(void);
void (* isr_spi)(void);
void (* isr_rtc)(void);//rtc
void (* isr_adc)(void);//rtc
void (* isr_afs_check)(void);
void (* isr_spiTDBoot)(void);
void (* isr_spiTDPcm)(void);
void (* isr_spiGsmPcm)(void);
void (* isr_usbHost)(void);//usb host stack ISR 20100427
//void (* isr_lcdDmaShow)(void);//RGB lcd show

void (* isr_scanner_frame)(void);
void (* isr_scanner_data)(void);

static void hyhwInt_ISR_null ( void );


/*---------------------------------------------------------------------------
   exported function:
   --------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_Init
* ����:	��ʼ������ֹ�����ж�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_init( void )
{
	int i;
	
	/*��ʼ��ʱ�����е��жϽ�ֹ */
	INT_ENABLE_REGISTER	= 0;
	hyhwInt_clear(0xFFFFFFFF);
	
	/*��Ҫ���ܵ��ж�*/
		 
	 
	for(i=0; i<ISR_TIMER0_NUM; i++)
		pisr_timer0[i] = NULL;
	 
	 
	/*	Ϊ��ȫ�����Ϊisr_audio_codec ������ֵ */
	isr_audio_codec		= hyhwInt_ISR_null;
	
	/* ���Ҫ�ж��Ƿ��ǿղŵ��� */
	isr_audio_decoder	= NULL;
	isr_audio_encoder   = NULL;
	
	isr_timer1			= hyhwInt_ISR_null;
	isr_timer3			= hyhwInt_ISR_null;
	isr_key_scan_timer0	= NULL;
	isr_gpio			= NULL;
	isr_uart0			= NULL;
	isr_uart1			= NULL;
	isr_uart2			= NULL;
	isr_uart3			= NULL;
	isr_sdio0			= NULL;
	isr_sdio1			= NULL;
	isr_usb				= NULL;
	isr_enable_sdio     = NULL;
	isr_timer5          = NULL;
	
	isr_camera			= NULL;
	
	isr_ppp_time        = NULL;
	
	isr_spi				= NULL;
	isr_rtc               =NULL;
	isr_adc	            = NULL;
	isr_afs_check       = NULL;
	isr_uartDma		    = NULL;
	isr_spiDma			= NULL;
	
	isr_spiTDBoot		= NULL;
	isr_spiTDPcm        = NULL;
	isr_spiGsmPcm       = NULL;
	isr_usbHost           =NULL; //20100427
	
	isr_timer0			= NULL;
	
	//isr_lcdDmaShow      = NULL ;
	return ;
	
}


/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigUsbHost_ISR  20100427
* ����:	���� usb Host ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUsbHost_ISR(  void (*pIntVector) (void))
{
	isr_usbHost = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_LcdDmaShow_ISR
* ����:	���� RGB���� ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
/*void hyhwInt_LcdDmaShow_ISR(  void (*pIntVector) (void))
{
	isr_lcdDmaShow = pIntVector;
}*/


/*-----------------------------------------------------------------------------
* ����:	hyhwInt_disableAll
* ����:	��ֹ�����ж�
* ����:	group			--  A���жϻ���B���ж�
*		intSourceMask	--  ��Ӧ���ж�λ�������Ƕ���жϵĻ�ֵ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_disableAll( void )
{
	/*  �����ģ��Ƿ�HwMREQ ��MREQA����Ҫ��ʵ ʲô��˼ 
	unsigned long __HwMREQ
	
	HwIEN	= 0 ;

		__HwMREQ = HwMREQ;				// Flush the interrupt Queue
		HwIEN	 = 0x0;		//&= ~HwINT_MEN;		
		HwMREQ	 = __HwMREQ;
	}  */

}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigCodecISR
* ����:	��������Audio Subsubsystem �ڲ��š�¼��������в�ͬ��ISR���������ң�����
*		�����ͬ��ISRҲ���в�ͬ�����ԣ���Ҫ����AudioSubsytem��ISR������
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigCodecISR(  void (*pIntVector) (void))
{
	isr_audio_codec = pIntVector;
}


/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigAudioDecISR
* ����:	���Ž��������жϺ���
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigAudioDecISR(  void (*pIntVector) (void))
{
	isr_audio_decoder = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigAudioEncISR
* ����:	���ű��������жϺ���
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigAudioEncISR(  void (*pIntVector) (void))
{
	isr_audio_encoder = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigTimer0Hw_ISR
* ����:	���� timer1 ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigTimer0Hw_ISR(  void (*pIntVector) (void))
{
	isr_timer0 = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigTimer1_ISR
* ����:	���� timer1 ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigTimer1_ISR(  void (*pIntVector) (void))
{
	isr_timer1 = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigTimer2_ISR
* ����:	���� timer2 ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigTimer2_ISR(  void (*pIntVector) (void))
{
	isr_timer2 = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigTimer3_ISR
* ����:	���� timer2 ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigTimer3_ISR(  void (*pIntVector) (void))
{
	isr_timer3 = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigTimer5_ISR
* ����:	���� timer5 ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigTimer5_ISR(  void (*pIntVector) (void))
{
	isr_timer5 = pIntVector;
}


/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigKeyScanTimer0_ISR
* ����:	���� KeyScanTimer0 ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigKeyScanTimer0_ISR(  void (*pIntVector) (void))
{
	isr_key_scan_timer0 = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigGpio_ISR
* ����:	���� Gpio ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigGpio_ISR(  void (*pIntVector) (void))
{
	isr_gpio = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigUart0_ISR
* ����:	���� Uart ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart0_ISR(  void (*pIntVector) (void))
{
	isr_uart0 = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigUart1_ISR
* ����:	���� Uart ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart1_ISR(  void (*pIntVector) (void))
{
	isr_uart1 = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigUart2_ISR
* ����:	���� Uart ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart2_ISR(  void (*pIntVector) (void))
{
	isr_uart2 = pIntVector;
}


#include "hyhwUart.h"
/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigUart_ISR
* ����:	���� Uart ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUart_ISR(U8 id,  void (*pIntVector) (void))
{
	switch(id)
	{
	case UART0:
		isr_uart0 = pIntVector;
		break;
	case UART1:
		isr_uart1 = pIntVector;
		break;
	case UART2:
		isr_uart2 = pIntVector;
		break;
	case UART3:
		isr_uart3 = pIntVector;
		break;
	default:
		break;
	}
}



/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigSdio0_ISR
* ����:	���� Sdio0 ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSdio0_ISR(  void (*pIntVector) (void))
{
	isr_sdio0 = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigSdio1_ISR
* ����:	���� Sdio1 ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSdio1_ISR(  void (*pIntVector) (void))
{
	isr_sdio1 = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigUsb_ISR
* ����:	���� usb ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUsb_ISR(  void (*pIntVector) (void))
{
	isr_usb = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigCamera_ISR
* ����:	���� Sdio1 ���жϺ�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigCamera_ISR(  void (*pIntVector) (void))
{
	isr_camera = pIntVector;
}


/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigTimer0_ISR
* ����:	
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigTimer0_ISR(U8 id, void (*pIntVector) (void))
{
	if(id < ISR_TIMER0_NUM)
		pisr_timer0[id] = pIntVector;
}


/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigEnableSdio
* ����:	����sdio�ж�
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigEnableSdio(void (*pIntVector) (void))
{
    isr_enable_sdio = pIntVector;
}


/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigPppTime
* ����:	ע��ppp��ʱ��
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigPppTime(void (*pIntVector) (void))
{
    isr_ppp_time = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ISR_null
* ����:	Ϊcodec ��ISR�ĺ���ָ�����Ŀպ�����Ϊisr_audio_codec����ָ�븳��ֵʹ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
static void hyhwInt_ISR_null ( void )
{
   return;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigSpi
* ����:	ע��spi�жϺ���
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSpi(void (*pIntVector) (void))
{
    isr_spi = pIntVector;
}
 
/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigUart1Scan
* ����:	�õ�TRC�ж�,�����¼�
* ����:	pIntVector	-- �����RTC ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_RTC_intermit(void (*pIntVector) (void))
{
    isr_rtc = pIntVector;
}
 
/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigADC
* ����:	ע��ADCɨ�躯��
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigADC(void (*pIntVector) (void))
{
    isr_adc = pIntVector;
}
 
/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigSpiDma
* ����:	����spiDMA���ж�
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSpiDma(void (*pIntVector) (void))
{
    isr_spiDma = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigUartDma
* ����:	uart����DMA�ж�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigUartDma(void (*pIntVector) (void))
{
    isr_uartDma = pIntVector;
}


/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigUartDma
* ����:	spi6 Gsm Pcm �����ж�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSpiGsmPcm(void (*pIntVector) (void))
{
    isr_spiGsmPcm = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigSpiDma
* ����:	spi8 TD Pcm �����ж�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSpiTDPcm(void (*pIntVector) (void))
{
    isr_spiTDPcm = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigUartDma
* ����:	spi7 TD-BOOT �����ж�����
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigSpiTDBoot(void (*pIntVector) (void))
{
    isr_spiTDBoot = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigScannerFrame
* ����:	ɨ��frame�жϷ���������
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigScannerFrame(void (*pIntVector) (void))
{
    isr_scanner_frame = pIntVector;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwInt_ConfigScannerData
* ����:	ɨ��data�жϷ���������
* ����:	pIntVector	-- �����ISR ����������ڵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwInt_ConfigScannerData(void (*pIntVector) (void))
{
    isr_scanner_data = pIntVector;
}

/* 
�޸ļ�¼: 
1\ �����
 hyhwInt_ConfigSpiDma
 hyhwInt_ConfigUartDma
 ��������������ͨ��DMA�����
 yanglijing, 20090817
*/ 
 
#ifdef __cplusplus
  }
#endif
