/*******************************************************************************
*  (C) Copyright 2010 hyco, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of hyco and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  hyco and is confidential in nature.  Its use and dissemination by    
*  any party other than hyco is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"
/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "hyhwCkc.h"
#include "hyhwUart.h"
#include "hyhwGpio.h"



// Register and relative bits definition

/*UART INTERRUPT ENABLE REGISTER */	
	#define IER_BIT_EMSI            	BIT3
	#define IER_BIT_ELSI            	BIT2
	#define IER_BIT_ETXI            	BIT1
	#define IER_BIT_ERXI            	BIT0

/*UART INTERRUPT STATUS REGISTER */
	#define ISR_BIT_IID_MODEM       	~(BIT1|BIT2|BIT3)
	#define ISR_BIT_IID_TRAN       		BIT1
	#define ISR_BIT_IID_REC         	BIT2
	#define ISR_BIT_IID_ERR         	BIT1|BIT2
	#define ISR_BIT_IID_TIME        	BIT2|BIT3
	
	#define ISR_BIT_IP_PEND         	~BIT0
	#define ISR_BIT_IP_NPEND        	BIT0

/*UART_FIFO_CONTROL_REGISTER */	
	#define FCR_BITMASK_RX_TRIGGRE_LEVEL_MASK   ~(BIT6|BIT7)
	#define FCR_BITMASK_RX_TRIGGRE_LEVEL_1	    0
	
	#define FCR_BITMASK_RX_TRIGGRE_LEVEL_4		BIT6
	#define FCR_BITMASK_RX_TRIGGRE_LEVEL_8      BIT7
	#define FCR_BITMASK_RX_TRIGGRE_LEVEL_14     BIT6|BIT7
	
	#define FCR_BITMASK_TX_TRIGGRE_LEVEL_MASK   ~(BIT4|BIT5)
	#define FCR_BITMASK_TX_TRIGGRE_LEVEL_1		BIT4|BIT5
	#define FCR_BITMASK_TX_TRIGGRE_LEVEL_4		BIT5
	#define FCR_BITMASK_TX_TRIGGRE_LEVEL_8      BIT4
	#define FCR_BITMASK_TX_TRIGGRE_LEVEL_16     0
	
	#define FCR_BIT_DMA_MODE_NODEPEND_DISABLE   ~BIT3  /* DMA ����������RxDe��TxDe */
	#define FCR_BIT_DMA_MODE_NODEPEND_ENABLE	BIT3   /* DMA ����ʹ��, ����RxDe��TxDe��ֵ */
	#define FCR_BIT_TX_FIFO_RESET               BIT2
	#define FCR_BIT_RX_FIFO_RESET               BIT1
	#define FCR_BIT_FIFO_ENABLE                 BIT0
	
/*UART LINE CONTROL REGISTER */		
	#define LCR_BIT_LATCH_DLAB      	BIT7
	#define LCR_BIT_SET_BREAK      	 	BIT6
	#define LCR_BIT_STICK_PARITY    	BIT5
	#define LCR_BIT_PARITY_ODD			~BIT4
	#define LCR_BIT_PARITYEVEN      	BIT4
	#define LCR_BIT_PARITY_EN       	BIT3
	#define LCR_BIT_STOP				BIT2
				
/*UART LINE STATUS REGISTER */	
	#define LSR_BIT_ERF            		BIT7
	#define LSR_BIT_TEMT     			BIT6	/* 1: Transmit FIFO and shift registers are all empty */
	#define LSR_BIT_THRE   				BIT5	/* 1: fifo has less then half data */
	#define LSR_BIT_BREAK             	BIT4
		
	#define LSR_BIT_FER             	BIT3
	#define LSR_BIT_PER             	BIT2
	#define LSR_BIT_OER            		BIT1
	#define LSR_BIT_DR              	BIT0
	
/* UART Control Register */
	#define UCR_BIT_RX_WORDACCESS_DISABLE			~BIT3   /* Rx FIFO access to word disable (1 bytes) */
	#define UCR_BIT_RX_WORDACCESS_ENABLE			BIT3	/* Rx FIFO access to word enable  (4 bytes) */
	#define UCR_BIT_TX_WORDACCESS_DISABLE			~BIT2	/* Tx FIFO access to word disable (1 bytes) */
	#define UCR_BIT_TX_WORDACCESS_ENABLE			BIT2	/* Tx FIFO access to word enable  (4 bytes) */
	#define UCR_BIT_RX_DMA_DISABLE					~BIT1	/* Rx DMA Disable */
	#define UCR_BIT_RX_DMA_ENABLE					BIT1	/* Rx DMA Enable */
	#define UCR_BIT_TX_DMA_DISABLE					~BIT0	/* Tx DMA Disable */
	#define UCR_BIT_TX_DMA_ENABLE					BIT0	/* Tx DMA Enable */
	
/* UART Modem Control Register(MCR) */

	#define MCR_BIT_RW_RTS_DISABLE_MASK		~BIT6		// RTS Deassert Condition Control Bit ����
	#define MCR_BIT_RW_RTS_DISABLE_RXSTOP	~BIT6		// nRTS is de-asserted at the Rx Stop Condition
	#define MCR_BIT_RW_RTS_DISABLE_RXSTART	BIT6        // nRTS is de-asserted at the Rx Start Condition
	
	#define MCR_BIT_RW_AFC_MASK				~BIT5		// Hardware Auto Flow control Enable bit mask
	#define MCR_BIT_RW_AFC_DISABLE			~BIT5		// Disable auto flow contrl 
	#define MCR_BIT_RW_AFC_ENABLE			BIT5		// Enable auto flow contrl 
	
	#define MCR_BIT_RW_LOOP_MASK			~BIT4		// loop back mode enable mask
	#define MCR_BIT_RW_LOOP_DISABLE			~BIT4		// disable local loop back feature
	#define MCR_BIT_RW_LOOP_ENABLE			BIT4		// enable local loop back feature
	
	#define MCR_BIT_RW_RTS_MASK				~BIT1		// request to send signal flag mask
	#define MCR_BIT_RW_RTS_HIGH				~BIT1		// set nRTS line to high state, ��Ч״̬
	#define MCR_BIT_RW_RTS_LOW				BIT1		// reset  the nRTS line to low state
	
	#define MCR_BITMASK_FCC_ACTIVE_MASK		~(BIT1|BIT5) // flow control configuration
	#define MCR_BITMASK_FCC_ACTIVE_BOTH		BIT1|BIT5	 // both nCTS and nRTS automatic flow control is activated 
	#define MCR_BITMASK_FCC_ACTIVE_CTS		BIT5		 // only nCTS flow control is activated
	#define MCR_BITMASK_FCC_ACTIVE_NONE     			 // both unactived

/* UART	AFC �Ĵ��� */
	#define AFC_RTS_ASSERT_TRIGGER_LEVEL(i)       (i << 4)      
	#define AFC_RTS_DEASSERT_TRIGGER_LEVEL(i)      i
	
	
#define UART2_COMMUNICATION_TIMEOUT		1000//10000

// DMA2 ͨ��1, ��channel 7 ��ΪUART��DMA����
#define UART1_DMA_CHANNEL			7

/*---------------------------------------------------------------------------
* ��  :	hyhwmUart1_DmaPause 
* ����:	��ͣ Uart1 Dma ����
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmUart1_DmaPause(void)
{
	return hyhwDma_Pause(UART1_DMA_CHANNEL);
}
/*---------------------------------------------------------------------------
* ��  :	hyhwmUart1_Dmarestart 
* ����:	������ͣλ�ÿ�ʼ���� Uart1 Dma ����
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmUart1_Dmarestart(void)
{
	return hyhwDma_restart(UART1_DMA_CHANNEL);
}
/*---------------------------------------------------------------------------
* ��  :	hyhwmUart1_DmaisPause 
* ����:	�ж�DMA�Ƿ�����ͣ״̬
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
BOOL hyhwmUart1_DmaisPause(void)
{
	hyhwDma_isPause(UART1_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* ��  :	hyhwmUart1_DmaEnable 
* ����:	ʹ�� Uart1 Dma ����
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmUart1_DmaEnable(void)	
{
	return hyhwDma_Enable(UART1_DMA_CHANNEL);	
}

/*--------------------------------------------------------------------------
* ��  :	hyhwmUart1_DmaDisable 
* ����:	disable Uart1 Dma ����
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwmUart1_DmaDisable(void)
{
	return hyhwDma_Disable(UART1_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* ����:	hyhwUart1_DmaIntClear
* ����:	���ָ��ͨ����DMA�ж�
* ����:	none
* ����:	HY_OK
*		HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwUart1_DmaIntClear(void)
{
	return hyhwDma_IntClear(UART1_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* ��  :	hyhwmUart1_DmaIntEnable 
* ����:	enable dma rxtx interrupt
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmUart1_DmaIntEnable(void)	
{
	return hyhwDma_IntEnable(UART1_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* ��  :	hyhwmUart1_DmaIntDisable 
* ����:	disable dma tx interrupt
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwmUart1_DmaIntDisable(void)
{
	return hyhwDma_IntDisable(UART1_DMA_CHANNEL);	
}
	
/*---------------------------------------------------------------------------
* ����:	hyhwUart1_DmacheckInterruptState
* ����:	�ж��Ƿ��ж�ʹ��
* ����:	none
* ����:	True �����Ѿ��ж�, FALSE ����δ�ж�
*--------------------------------------------------------------------------*/
BOOL hyhwUart1_DmacheckInterruptState(void)
{
	return hyhwDma_isDmaInterruptSet(UART1_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* ����:	hyhwUart1_DmaGetStatus
* ����:	���ָ��ͨ����DMA��״̬������/busy��
*			�Ȳ鿴��ͨ���Ƿ�����---����
*			���ݴ������-----------����
*			�������ڴ���-----------������
* ����:	none
* ����:	HY_OK
*		HY_ERR_BUSY
*		HY_ERR_BAD_PARAMETER
*--------------------------------------------------------------------------*/
U32 hyhwUart1_DmaGetStatus(void)
{
	return hyhwDma_GetStatus(UART1_DMA_CHANNEL);
}
#if 0
/*-----------------------------------------------------------------------------
* ����:	hyhwUart_Init
* ����:	��ʼ��UART, Ӧ�ò�������ò�ͬ��UART����
* ����:	baudrate	-- UART�Ĳ�����
*		databits	-- ����λ��
*		stopbits	-- ֹͣλ��
*		parity		-- ��żУ������
* ����:	HY_OK		-- �ɹ�
*		HY_ERROR	-- ��ʱ
*----------------------------------------------------------------------------*/
UInt32 hyhwUart_Init(eUART_BAUDRATE baudrate,
                           eUART_DATA_BITS	dataBits,
                           eUART_STOP_BITS	stopBits,
                           eUART_PARITY   	parity,
                           eUart_Mode	  	uartMode,
                           U8 uartId
						   )
{
	tUART_REGISTER *ptUart_Reg;
	U32 divisor;
	
	if(uartId >UART_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	else
	{
		switch (uartId)
		{
			case 0:
				hyhwCpm_busClkEnable(CKC_UART0);
				hyhwCpm_clearSoftReset(CKC_UART0);
				hyhwGpio_setUart0Function(0); 	/* set uart0 port0, 4�����ش��� */
				//hyhwUart_portMap(uartId, GSM_UART_PORT);
				break;
			case 1:
				hyhwCpm_busClkEnable(CKC_UART1);
				hyhwCpm_clearSoftReset(CKC_UART1);
				hyhwGpio_setUart1Function(0); 	/* set uart1 port1,  2�ߴ��� */
				//hyhwUart_portMap(uartId, BT_UART_PORT);
				break;
			case 2:
				hyhwCpm_busClkEnable(CKC_UART2);
				hyhwCpm_clearSoftReset(CKC_UART2);
				hyhwGpio_setUart2Function(); 	/* set uart2 port2, 2�ߴ��� */
				//hyhwUart_portMap(uartId, GPS_UART_PORT);
				break;
			case 3:
				hyhwCpm_busClkEnable(CKC_UART3);
				hyhwCpm_clearSoftReset(CKC_UART3);
				hyhwGpio_setUart3Function(); 	/* set uart3 port5, 2�ߴ��� */
				//hyhwUart_portMap(uartId, STM102_UART_PORT);
				break;
		}
		
		hyhwCpm_setUartClk(uartId, PERI_CLK_48M); 				/* set uart clk */
			
			
		/* set baudrate  */
		/* Set the `Divisor Latch Access Bit` and enable so the DLL/DLM access*/
		/* Initialise the `Divisor latch LSB` and `Divisor latch MSB` registers */
		
		divisor		= sUartDivisor[baudrate];
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		
		ptUart_Reg->LINE_CONRROL 	  |= LCR_BIT_LATCH_DLAB;
		ptUart_Reg->DIVISOR_LATCH_LOW  = (divisor & 0x00FF);
		ptUart_Reg->DIVISOR_LATCH_HIGH = (divisor & 0xFF00);
		ptUart_Reg->LINE_CONRROL 	  |= dataBits|stopBits|parity;/*8n1*/
		
	//	ptUart_Reg->UART_CONTROL 	  |= (UCR_BIT_RX_DMA_ENABLE|UCR_BIT_TX_DMA_ENABLE);
		
		if(uartMode == Uart_Fifo_Mode)
		{	
			 ptUart_Reg->FIFO_CONTROL = FCR_BIT_FIFO_ENABLE;   // ʹ��FIFOλ
			//ptUart_Reg->FIFO_CONTROL &= ~FCR_BIT_FIFO_ENABLE;   // ʹ��FIFOλ
			
		}	 
	
		ptUart_Reg->FIFO_CONTROL    &= 	FCR_BIT_DMA_MODE_NODEPEND_DISABLE;
		
#if 0
		/* 2009.7.8 Zeet added for enable UART0 (CTS) auto flow control function Start */
		
	//	ptUart_Reg->MODEM_CONTROL 	  |= BIT1;				// Reset the nRTS line to low state
	//	ptUart_Reg->MODEM_CONTROL 	  |= BIT5;				// Auto Flow Control Enable
//		ptUart_Reg->MODEM_CONTROL 	  &= ~BIT4;				// Disable LoopBack

		ptUart_Reg->FIFO_CONTROL 	  |= (BIT7|BIT4);		// TX FIFO Trigger Level(8 Bytes)
		ptUart_Reg->FIFO_CONTROL 	  &= ~(BIT3|BIT6|BIT5);
		
		ptUart_Reg->UART_CONTROL 	  &= ~BIT3;				// Rx FIFO access to byte
		ptUart_Reg->UART_CONTROL 	  &= ~BIT2;				// Tx FIFO access to byte
//		ptUart_Reg->UART_CONTROL 	  |= BIT1;				// Rx DMA enable
//		ptUart_Reg->UART_CONTROL 	  |= BIT0;				// Tx DMA enable
		/*- 2009.7.8 Zeet added for enable UART0 (CTS) auto flow control function End -*/

#else 
		/* 2009.7.8 Zeet added for enable UART0 (CTS) auto flow control function Start */
		/*
		AFE		RTS			Flow Control Configuration
		1		1		:	Both nCTS and nRTS automatic flow control is activated
		1		0		:	Only nCTS automatic flow control is activated
		0		X		:	Both nCTS and nRTS automatic flow control is de-activated
							(Same as TCC77x series, only support for S/W Flow Control)
		*/
		
		/* ֻ��uart0 �� uart1��Ҫ����, ����ڴ˴���Ҫ�������� */
		
		if(uartId == UART0 || uartId == UART1)
		{
			// ��Ҫ���ؿ���
			// enable both CTS and RTS auto flow control
			ptUart_Reg->MODEM_CONTROL 	  &= MCR_BITMASK_FCC_ACTIVE_MASK;	// λ�������
			ptUart_Reg->MODEM_CONTROL     |= MCR_BITMASK_FCC_ACTIVE_BOTH;   // ��ֵ����, ʹ�ܷ��ͺͽ�������
			
			/* ˵��: ��ֵ��дʲôʱ��������Ч, ������˵, �����������, 
			   �����4��14ʱ, ��dataready�Ժ�,�������ź��ø�(��Чʱ)
			   ����ȥ��ȡ������������, �����Զ���15���ֽ�, ����ȡ���ݵ�4ʱ�����ٴ���Ч 
			   �����ó�0��8ʱ, dataready�Ժ�, ��������Ϊ��Ч, �����Զ���9���ֽ�, 
			   ��ȡ����0���ֽ�ʱ, ���زŻ���Ч 
			   �����������0/0����˼, ������һ���ֽھ�������Ч, ���ٷ��͵ڶ�������, �ȴ�������ݶ����Ժ�
			   ���ز��Զ���Ч, ���������DMAʱ�Ƿǳ���Ч�� */
			
			// disable loopback mode
			ptUart_Reg->MODEM_CONTROL     &= MCR_BIT_RW_LOOP_MASK;			// λ�������
			ptUart_Reg->MODEM_CONTROL 	  &= MCR_BIT_RW_LOOP_DISABLE;		// ��ֵ����

			if(uartId == UART0)
			{
				// һ�����ش���ʹ��, ����ULC2, ����14���ֽ��ж�
				ptUart_Reg->FIFO_CONTROL = ( FCR_BIT_FIFO_ENABLE             // FIFO ģʽ
				                            |FCR_BITMASK_RX_TRIGGRE_LEVEL_8  // triggerlevel -- ������ٸ��ֽڽ���DMA����
				                            |FCR_BITMASK_TX_TRIGGRE_LEVEL_8
				                            |FCR_BIT_TX_FIFO_RESET
				                            |FCR_BIT_RX_FIFO_RESET
				                            );
				
				
				ptUart_Reg->UART_CONTROL 	  |= UCR_BIT_RX_DMA_ENABLE;				// Rx DMA ��ʹ��
				ptUart_Reg->UART_CONTROL 	  |= UCR_BIT_TX_DMA_ENABLE;				// Tx DMA ��ʹ��
			}
			else
			{
				// UART1 ����tg3 3M ͨ��, ����UART�����жϴ���Ϊ1���ֽ�
				// RX FIFO trigger level / Tx FIFO trigger level set, ����һ���ֽڴ���
				/*
				ptUart_Reg->FIFO_CONTROL 	  &= FCR_BITMASK_RX_TRIGGRE_LEVEL_MASK;
				ptUart_Reg->FIFO_CONTROL 	  |= FCR_BITMASK_RX_TRIGGRE_LEVEL_1;
				
				ptUart_Reg->FIFO_CONTROL 	  &= FCR_BITMASK_TX_TRIGGRE_LEVEL_MASK;
				ptUart_Reg->FIFO_CONTROL 	  |= FCR_BITMASK_TX_TRIGGRE_LEVEL_1;
				
				*/
				ptUart_Reg->AFC_TRIGGER        = AFC_RTS_ASSERT_TRIGGER_LEVEL(0)
			                                | AFC_RTS_DEASSERT_TRIGGER_LEVEL(0);  // ���ô������ص�triggerlevelΪ14
				// һ�����ش���ʹ��, ����ULC2, ����14���ֽ��ж�
				ptUart_Reg->FIFO_CONTROL = ( FCR_BIT_FIFO_ENABLE             // FIFO ģʽ
				                            |FCR_BITMASK_RX_TRIGGRE_LEVEL_1  // triggerlevel -- ������ٸ��ֽڽ���DMA����
				                            |FCR_BITMASK_TX_TRIGGRE_LEVEL_1
				                            |FCR_BIT_TX_FIFO_RESET
				                            |FCR_BIT_RX_FIFO_RESET
				                            );
				
				ptUart_Reg->UART_CONTROL 	  |= UCR_BIT_RX_DMA_ENABLE;				// Rx DMA ʹ��
				ptUart_Reg->UART_CONTROL 	  |= UCR_BIT_TX_DMA_ENABLE;				// Tx DMA ʹ��
			}
			//ptUart_Reg->FIFO_CONTROL 	  &= FCR_BIT_TX_FIFO_RESET;
			//ptUart_Reg->FIFO_CONTROL 	  &= FCR_BIT_RX_FIFO_RESET;
		}	
		else
		{
			// ����Ҫ����	// RX FIFO trigger level / Tx FIFO trigger level set, ����8���ֽڴ���
			ptUart_Reg->FIFO_CONTROL 	  &= FCR_BITMASK_RX_TRIGGRE_LEVEL_MASK;
			ptUart_Reg->FIFO_CONTROL 	  |= FCR_BITMASK_RX_TRIGGRE_LEVEL_14;
			ptUart_Reg->FIFO_CONTROL 	  &= FCR_BITMASK_TX_TRIGGRE_LEVEL_MASK;
			ptUart_Reg->FIFO_CONTROL 	  |= FCR_BITMASK_TX_TRIGGRE_LEVEL_8;
			
			ptUart_Reg->FIFO_CONTROL 	  &= FCR_BIT_DMA_MODE_NODEPEND_DISABLE; // DMA ����ʹ��, ��������RxDe �� TxDe
			
			
		}
		
		ptUart_Reg->UART_CONTROL 	  &= UCR_BIT_RX_WORDACCESS_DISABLE;				// Rx FIFO access to byte
		ptUart_Reg->UART_CONTROL 	  &= UCR_BIT_TX_WORDACCESS_DISABLE;				// Tx FIFO access to byte
#endif
		// wait the setting is OK
	  	while(ptUart_Reg->LINE_STATUS & LSR_BIT_THRE == 0);
		    
		/* Line Control register, preparing for transmit and receive data
		   Clear the `Divisor Latch Access Bit (DLAB)*/	
		ptUart_Reg->LINE_CONRROL 	  &= ~LCR_BIT_LATCH_DLAB;
	} 
			
	return HY_OK;	

}
#endif
void hyhwUart_flowCtrl(U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
	ptUart_Reg->MODEM_CONTROL 	  |= (BIT1|BIT5);
}

void hyhwUart_DmaEffect(U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
	ptUart_Reg->FIFO_CONTROL 	  &= 0;
	
	ptUart_Reg->FIFO_CONTROL 	  |= 3;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwUart_txIntEnable
* ����:	��Uart tx �ж�ʹ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_txIntEnable(U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	
	if(uartId >UART_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
	
		ptUart_Reg->INT_ENABLE|=(IER_BIT_ETXI);
	}
	return HY_OK;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwUart_rxIntEnable
* ����:	��Uart rx �ж�ʹ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_rxIntEnable(U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	U32 rc = HY_OK;
	
	if(uartId >UART_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
	
		ptUart_Reg->INT_ENABLE|=(IER_BIT_ERXI);
	}
	return rc;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwUart_txIntDisable
* ����:	�ر�Uart tx �ж�ʹ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_txIntDisable(U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	U32 rc = HY_OK;
	
	if(uartId >UART_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		
		ptUart_Reg->INT_ENABLE &=~(IER_BIT_ETXI);
	}
	return rc;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwUart_rxIntDisable
* ����:	�ر�Uart rx �ж�ʹ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_rxIntDisable(U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	U32 rc = HY_OK;
	
	if(uartId >UART_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		
		ptUart_Reg->INT_ENABLE &=~(IER_BIT_ERXI);
	}
	return rc;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_CheckRecDataReady(void)
* ����:	�ж��Ƿ������ݽ��յ�
* ����:	none
* ����:	HY_OK(�����ݣ���HY_ERROR(�����ݣ�
*----------------------------------------------------------------------------*/
U32 hyhwUart_CheckRecDataReady(U8 uartId)
{
	/* read the character */
	tUART_REGISTER *ptUart_Reg;
	U32 rc = HY_OK;
	
	if(uartId >UART_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);

		if(ptUart_Reg->LINE_STATUS & LSR_BIT_DR)
		{
			rc = HY_OK;
		}
		else
		{
			rc = HY_ERROR;
		}	
	}
	return rc;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwUart_ReadFifo
* ����:	��UART��fifo����һ���ֽ�
* ����:	none
* ����:	�������ֽ�
*----------------------------------------------------------------------------*/
U32 hyhwUart_ReadFifo(U8 *data,U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	U32 rc = HY_OK;
	
	if(uartId >UART_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
	ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);

	*data =(ptUart_Reg->RECEIVE);
	}
	return rc;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_WriteFifo
* ����:	��UART��fifoд��һ���ֽ�
* ����:	д����ֽ�
* ����:	HY_OK		-- �ɹ�
*		HY_ERROR	-- ��ʱ
*----------------------------------------------------------------------------*/
U32 hyhwUart_WriteFifo(U8 data,U8 uartId)
{
	/* wait until the transmit fifo has less than half data	 */	
	tUART_REGISTER *ptUart_Reg;
	U32 rc = HY_OK;
	
	if(uartId >UART_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		
		while(!(ptUart_Reg->LINE_STATUS & LSR_BIT_THRE));
		ptUart_Reg->TRANSMIT	=	data ;	
	}
	return rc;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_PortSet(U32 port0, U32 port1, U32 port2, U32 port3)
* ����:	���ø������ڶ�Ӧ�Ķ˿ں�
* ����:	��
* ����:	HY_OK		-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
* ע��: 4���������������
*----------------------------------------------------------------------------*/
U32 hyhwUart_PortSet(U32 port0, U32 port1, U32 port2, U32 port3)
{
	U32 regBuffer;

	if(port0 > 5 || port1 > 5|| port2 > 5|| port3 > 5)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	if(port0 == port1 || port0 == port2 || port0 == port3
		|| port1 == port2 || port1 == port3 || port2 == port3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	regBuffer = (port0 
				| port1 << 4
				| port2 << 8
				| port3 << 12
				);
				
	UART_MUX_CHSEL = regBuffer;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_PortSet(U32 port0, U32 port1, U32 port2, U32 port3)
* ����:	���ø������ڶ�Ӧ�Ķ˿ں�
* ����:	��
* ����:	HY_OK		-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
* ע��: �Ĵ����е�4��channelӳ��ֵ��������ͬ
*----------------------------------------------------------------------------*/
U32 hyhwUart_portMap(U32 uart, U32 port)
{
	U32 regBuffer;
	U32 offset;
	
	regBuffer = UART_MUX_CHSEL;
	offset = 4*uart;
	
	regBuffer &= ~(0x7<<offset);
	regBuffer |= port << offset;
				
	UART_MUX_CHSEL = regBuffer;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_Init
* ����:	��ʼ��UART, Ӧ�ò�������ò�ͬ��UART����
* ����:	baudrate	-- UART�Ĳ�����
*		databits	-- ����λ��
*		stopbits	-- ֹͣλ��
*		parity		-- ��żУ������
* ����:	HY_OK		-- �ɹ�
*		HY_ERROR	-- ��ʱ
*----------------------------------------------------------------------------*/
UInt32 hyhwUart_Init(eUART_BAUDRATE baudrate,
                           eUART_DATA_BITS	dataBits,
                           eUART_STOP_BITS	stopBits,
                           eUART_PARITY   	parity,
                           eUart_Mode	  	uartMode,
                           U8 uartId, 					 // ӳ���uart channnel
                           U8 port,   					 // ��Ӧ������Ķ˿�
                           e_UARTFLOW_CTRL isFlowSuppot, // �Ƿ�֧������
                           e_UARTDMA_CTRL isDmaSupport  // �Ƿ�֧��Dma
						   )
{
	tUART_REGISTER *ptUart_Reg;
	U32 divisor;
	
	if(uartId >UART_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	else
	{
		switch (uartId)
		{
			case 0:
				hyhwCpm_busClkEnable(CKC_UART0);
				hyhwCpm_clearSoftReset(CKC_UART0);
				break;
			case 1:
				hyhwCpm_busClkEnable(CKC_UART1);
				hyhwCpm_clearSoftReset(CKC_UART1);
				break;
			case 2:
				hyhwCpm_busClkEnable(CKC_UART2);
				hyhwCpm_clearSoftReset(CKC_UART2);
				break;
			case 3:
				hyhwCpm_busClkEnable(CKC_UART3);
				hyhwCpm_clearSoftReset(CKC_UART3);
				break;
		}
		switch (port)
		{
			case 0:
				hyhwGpio_setUart0Function(0); 	/* set uart0 port0, 4�����ش��� */
				break;
			case 1:
				hyhwGpio_setUart1Function(0);	/* set uart0 port0, 4�����ش��� */
				break;
			case 2:
				hyhwGpio_setUart2Function(); 	/* set uart2 port2, 2�ߴ��� */
				break;
			case 3:
				hyhwGpio_setUart3Function(); 	/* set uart3 port3, 2�ߴ��� */
				break;
			case 4:
				hyhwGpio_setUart4Function(); 	/* set uart4 port4, 2�ߴ��� */
				break;
			case 5:
				hyhwGpio_setUart5Function(); 	/* set uart5 port5, 2�ߴ��� */
				break;
		}
		
		hyhwCpm_setUartClk(uartId, g_arrayBaudRate[baudrate].uart_clk); 	/* set uart clk */
		hyhwUart_portMap(uartId, port);				/* �Ѷ�Ӧ�Ŀ�����channelӳ�䵽��Ӧ�Ķ˿� */
			
	
		divisor = g_arrayBaudRate[baudrate].sDivisor;
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		
		ptUart_Reg->LINE_CONRROL 	  |= LCR_BIT_LATCH_DLAB;
		ptUart_Reg->DIVISOR_LATCH_LOW  = (divisor & 0x00FF);
		ptUart_Reg->DIVISOR_LATCH_HIGH = (divisor & 0xFF00);
		ptUart_Reg->LINE_CONRROL 	  |= dataBits|stopBits|parity;/*8n1*/
		
		if(uartMode == Uart_Fifo_Mode)
		{	
			 ptUart_Reg->FIFO_CONTROL  = FCR_BIT_FIFO_ENABLE;   // ʹ��FIFOλ	
			 ptUart_Reg->FIFO_CONTROL |=  ( FCR_BITMASK_RX_TRIGGRE_LEVEL_8//FCR_BITMASK_RX_TRIGGRE_LEVEL_14  // triggerlevel -- ������ٸ��ֽڽ���DMA����
				                            |FCR_BITMASK_TX_TRIGGRE_LEVEL_8
				                            |FCR_BIT_TX_FIFO_RESET
				                            |FCR_BIT_RX_FIFO_RESET
				                            );		
		}	 
		
		// disable loopback mode
		ptUart_Reg->MODEM_CONTROL     &= MCR_BIT_RW_LOOP_MASK;			// λ�������
		ptUart_Reg->MODEM_CONTROL 	  &= MCR_BIT_RW_LOOP_DISABLE;		// ��ֵ����
		
		// ��������
		ptUart_Reg->MODEM_CONTROL 	  	  &= MCR_BITMASK_FCC_ACTIVE_MASK;	// λ�������
		if( isFlowSuppot == FLOW_SUPPORT)
		{
			// enable both CTS and RTS auto flow control
			ptUart_Reg->MODEM_CONTROL     |= MCR_BITMASK_FCC_ACTIVE_BOTH;   // ��ֵ����, ʹ�ܷ��ͺͽ�������
			ptUart_Reg->AFC_TRIGGER        = AFC_RTS_ASSERT_TRIGGER_LEVEL(0)
			                                | AFC_RTS_DEASSERT_TRIGGER_LEVEL(0);  // ���ô������ص�triggerlevelΪ14
		}
		
		// DMA����
		ptUart_Reg->FIFO_CONTROL   		  &= FCR_BIT_DMA_MODE_NODEPEND_DISABLE;
		if( isDmaSupport == DMA_SUPPORT)
		{
			ptUart_Reg->UART_CONTROL 	  |= UCR_BIT_RX_DMA_ENABLE;				// Rx DMA ʹ��
			ptUart_Reg->UART_CONTROL 	  |= UCR_BIT_TX_DMA_ENABLE;				// Tx DMA ʹ��
		}
		
		// Rx FIFO access to byte // Tx FIFO access to byte
		ptUart_Reg->UART_CONTROL 	  &= (UCR_BIT_RX_WORDACCESS_DISABLE |UCR_BIT_TX_WORDACCESS_DISABLE);
				

		// wait the setting is OK
	  	while(ptUart_Reg->LINE_STATUS & LSR_BIT_THRE == 0);
		    
		/* Line Control register, preparing for transmit and receive data
		   Clear the `Divisor Latch Access Bit (DLAB) */	
		ptUart_Reg->LINE_CONRROL 	  &= ~LCR_BIT_LATCH_DLAB;
	} 
			
	return HY_OK;	

}

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_StateRead()
* ����:	��ȡ״̬�Ĵ�����������
* ����:	��
* ����:	״̬�Ĵ����е�ֵ
*----------------------------------------------------------------------------*/
U32 hyhwUart_StateRead(void)
{
	return UART_MUX_CHST;
}


/*-----------------------------------------------------------------------------
* ����:	hyhwUart_CheckTransmitFifoEmpty
* ����:	���UART�ķ���fifo�Ƿ�Ϊ��
* ����:	д�봮��id
* ����:	Fifo��״̬  
*			1	-- fifoΪ�գ� 
*			0	-- fifo��Ϊ�գ�
*			HY_ERR_BAD_PARAMETER -- ��������
*----------------------------------------------------------------------------*/
U32 hyhwUart_CheckTransmitFifoEmpty(U8 uartId)
{
	tUART_REGISTER *ptUart_Reg;
	
	if(uartId >UART_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
	}
	
	return (ptUart_Reg->LINE_STATUS & LSR_BIT_TEMT);
}	


#if 0
/*-----------------------------------------------------------------------------
* ����:	hyhwUart_Ch1Config
* ����:	����UART1�����ؿ���, ���ڴ�������
* ����:	none
* ����: none
* ��ע: uart1 3M,ʹ��DMA���գ�ͬʱ����uart ����1��byte�жϷ�ʽ���жϷ������
		��ֹͣuart�жϣ�������һ��timer��ʱ�ж�(�ݶ�5ms)��������ʱ�ж�ʱ����
		ȡDMA��ǰ��дλ�ã���DMA�Ѿ����յ�������copy����������������DMA��ֹͣ
		��timer���������DAM�жϣ�����������timerͣ��������������1byte��uart
		�жϡ�DMA���ÿ���Ϊ��8k bytes�ڴ�ѭ��ʹ�ã�2k�����жϡ�
*----------------------------------------------------------------------------*/
U32 hyhwUart_Ch1Config(
						   eUART_BAUDRATE   baudrate,
                           eUART_DATA_BITS	dataBits,
                           eUART_STOP_BITS	stopBits,
                           eUART_PARITY   	parity,
                           eUart_Mode	  	uartMode
                      )
{

	tUART_REGISTER *ptUart_Reg;
	U32 divisor;
	U32 uartReg;
	
	/* 
	
		���ô������ؿ���, �����ò������Լ���������
	*/
	hyhwGpio_setUart0Function(); 	/* set uart1 port */
	hyhwCpm_setUart0Clk();
			
	/* set baudrate  */
	/* Set the `Divisor Latch Access Bit` and enable so the DLL/DLM access*/
	/* Initialise the `Divisor latch LSB` and `Divisor latch MSB` registers */
	
	divisor		= sUartDivisor[baudrate];
	ptUart_Reg 	= (tUART_REGISTER*)UART1_3M_TDADDR;
	
	ptUart_Reg->LINE_CONRROL 	  |= LCR_BIT_LATCH_DLAB;
	ptUart_Reg->DIVISOR_LATCH_LOW  = (divisor & 0x00FF);
	ptUart_Reg->DIVISOR_LATCH_HIGH = (divisor & 0xFF00);
	ptUart_Reg->LINE_CONRROL 	  |= dataBits|stopBits|parity;/*8n1*/
	
	// ���û��������ƼĴ���, ����UART�����ж�Ϊ1���ֽ��ж�
	uartReg    = ptUart_Reg->FIFO_CONTROL;
	uartReg   |= (  //FCR_BIT_FIFO_ENABLE       // FIFO ����ʹ�ܣ���ΪҪ����DMA����
	             // | FCR_BIT_TXRX_ENABLE       // ����DMA������RxDe/TxDe�Ĺ�ϵ
	               FCR_BIT_TXFIFO_RES        // reset FIFO�������ͻ�����
	              | FCR_BIT_RXFIFO_RES
	             );
//	uartReg   &= FCR_BIT_RXTL_1;              // RxFIFO trigger level Ϊ1���ֽ�
	uartReg   |= FCR_BIT_RXTL_8;              // RxFIFO trigger level Ϊ1���ֽ�
	ptUart_Reg->FIFO_CONTROL       = uartReg; 

	
	// ��������
/*	ptUart_Reg->MODEM_CONTROL 	  |= BIT5;				// Auto Flow Control Enable
	ptUart_Reg->MODEM_CONTROL 	  &= ~BIT4;				// Disable LoopBack
	ptUart_Reg->MODEM_CONTROL 	  &= ~BIT1;
    
	ptUart_Reg->FIFO_CONTROL 	  |= (BIT7|BIT4);		// TX FIFO Trigger Level(8 Bytes)
	ptUart_Reg->FIFO_CONTROL 	  &= ~(BIT3|BIT6|BIT5);
	
	uartReg    = ptUart_Reg->UART_CONTROL;
	uartReg	  &= ~BIT3;				// Rx FIFO access to word (1 bytes)
	uartReg   &= ~BIT2;				// Tx FIFO access to word (1 bytes)
	uartReg   |= BIT1;				// Rx DMA enable
	uartReg	  |= BIT0;				// Tx DMA enable
	ptUart_Reg->UART_CONTROL      = uartReg;
*/	
	// wait the setting is OK
	while(ptUart_Reg->LINE_STATUS & LSR_BIT_THRE == 0);
	    
	/* Line Control register, preparing for transmit and receive data
	   Clear the `Divisor Latch Access Bit (DLAB)*/	
	ptUart_Reg->LINE_CONRROL 	  &= ~LCR_BIT_LATCH_DLAB;

return HY_OK;	
}
#endif
/*-----------------------------------------------------------------------------
* ����:	hyhwUart_CheckInterrupt
* ����:	�ж�ָ��uartId�Ƿ�����ж�
* ����:	uartId, ע: ���id �Ǵ��ڶ˿�id, UART0/1/2/3
* ����: none
* ʾ��: hyhwUart_CheckInterrupt(UART0);	
*----------------------------------------------------------------------------*/
BOOL hyhwUart_CheckInterrupt(U8 uartId)
{
	U32 tempVal;
	
	tempVal = UART_MUX_CHST;
	if(tempVal & (1<<uartId)) // bit1 ΪUART1���жϱ�־λ, ����Ĭ��chanle1 ӳ�䵽UART1
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_sendData
* ����:	��������
* ����:	uartId------ע: ���id �Ǵ��ڶ˿�id, tg3Ӧ���п�ѡ����
					UART0/UART1/UART2, ����ͨ�����ò�����
		pdata ------���ݻ�����ָ��
		len   ------�������ݳ���
* ����: �������ݳ���	
*----------------------------------------------------------------------------*/
U32 hyhwUart_sendData(U8 uartId, void *pdata, U32 len)
{
	/* wait until the transmit fifo has less than half data	 */	
	tUART_REGISTER *ptUart_Reg;
	U32 rc = HY_OK;
	U32 i;
	U8 *pDataPnt;
	U32 timeOut = 0;
	volatile U32 *pStatus, *pTransmit;
	
	pDataPnt = (U8 *)pdata;
	
	if(uartId >UART_NUM)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		pStatus = (volatile U32 *)(&ptUart_Reg->LINE_STATUS);
		pTransmit = (volatile U32 *)(&ptUart_Reg->TRANSMIT);
		
		for(i = 0; i<len; i++)
		{
			timeOut = 0;
			while(!(*pStatus & LSR_BIT_THRE))
			{
				timeOut++;
				if(timeOut >= UART2_COMMUNICATION_TIMEOUT)
				{
					return HY_ERROR;
					//break;
				}
			}
			
			*pTransmit = pDataPnt[i];
		}
	}
	return rc;

}

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_recvData
* ����:	��������
* ����:	uartId------ע: ���id �Ǵ��ڶ˿�id, tg3Ӧ���п�ѡ����
					UART0/UART1/UART2, ����ͨ�����ò�����
		pdata ------���ݻ�����ָ��
		len   ------�������ݳ���
* ����: �������ݳ���	
*----------------------------------------------------------------------------*/
U32 hyhwUart_recvData(U8 uartId, void *pdata, U32 len)
{
	tUART_REGISTER *ptUart_Reg;
	//U32 rc = HY_OK;
	U32 i;
	U8 *pDataPnt;
	U32 timeOut;
	volatile U32 *pStatus, *pTransmit;

	
	if(uartId >UART_NUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	else
	{
		pDataPnt = (U8 *)pdata;
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		pStatus = (volatile U32 *)(&ptUart_Reg->LINE_STATUS);
		pTransmit = (volatile U32 *)(&ptUart_Reg->TRANSMIT);

		for(i = 0; i<len; i++)
		{
			timeOut = 0;
			while(!(*pStatus & LSR_BIT_DR))
			{
				timeOut++;
				if(timeOut >= UART2_COMMUNICATION_TIMEOUT)
				{
					return HY_ERROR;
					//break;
				}
			}
			pDataPnt[i] = *pTransmit;
		}
	}
	return HY_OK;

}

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_recvData1
* ����:	�������ݣ��ú����ɸ����б�д��������ĺ����������ƣ��ɲ�ͬģ�����
* ����:	uartId------ע: ���id �Ǵ��ڶ˿�id, tg3Ӧ���п�ѡ����
					UART0/UART1/UART2, ����ͨ�����ò�����
		pdata ------���ݻ�����ָ��
		len   ------�������ݳ���
* ����: �������ݳ���	
*----------------------------------------------------------------------------*/
U32 hyhwUart_recvData1(U8 uartId, void *pdata, U32 len)
{
	tUART_REGISTER *ptUart_Reg;
	//U32 rc = HY_OK;
	U32 i;
	U8 *pDataPnt;
	U32 timeOut;
	volatile U32 *pStatus, *pTransmit;

	
	if(uartId >UART_NUM)
	{
		return (-HY_ERR_BAD_PARAMETER);
	}
	else
	{
		pDataPnt = (U8 *)pdata;
		ptUart_Reg 	= (tUART_REGISTER*)(UART_BASEADDR +  UART_OFFESTADDR * uartId);
		pStatus = (volatile U32 *)(&ptUart_Reg->LINE_STATUS);
		pTransmit = (volatile U32 *)(&ptUart_Reg->TRANSMIT);

		for(i = 0; i<len; i++)
		{
			timeOut = 0;
			while(!(*pStatus & LSR_BIT_DR))
			{
				timeOut++;
				if(timeOut >= UART2_COMMUNICATION_TIMEOUT)
				{
					return i;
					//return HY_ERROR;
					//break;
				}
			}
			pDataPnt[i] = *pTransmit;
		}
	}
	return len;

}

/* ����ĳ���Ϊ���Գ�����ʱ���ţ�����ɾ�� */

#if 0

#define BUFFSIZE	(8*1024) // UART1 DMA ��������С 8k
#define INTBUFFSIZE (2*1024) // UART1 DMA �жϻ�������С2k

U32 lastIntDstAddr = 0;
U32 lastIntSrcAddr = 0;

U32 writepos	   = 0;
U32 readpos        = 0;
U32 countBytes     = 0; // �����ʱ�������е���Ч�ֽ���
U32 lastDealedBytes = 0;
U32 readAddr;
U8 *pdataBufAddr;
U8 *pDmaRecvBufAddr;

U8 *data;
/* UART1���жϷ����� */
void isr_Uart1(void)
{
	
	// ���ж�, ����ɶ������Զ�����жϱ��, ����
	//hyhwUart_rxIntDisable(0);			// ��ֹUART1�����ж�
	//hyhwTimer_SetFrequency(2, 5000); 	// ���ö�ʱ��, 5ms �ж�һ��, ������ʱ�ж�
	//hyhwDma_Uart1DmaEnable();	        // ����UART1 DMA����
	hyhwUart_rxIntDisable(0);
	hyhwUart_ReadFifo(&data, 0);
	hyhwUart_WriteFifo('a', 0);
	hyhwUart_rxIntEnable(0);
	
}

/* ���溯�������õĶ�ʱ���ж� */
void isr_Timer2_count(void)
{	
	U32 wAddr = 0;
	hyhwmTimer0_clearIntFlag();
	// ��ȡDMA��ǰ����λ��
	hyhwmUart1_DmaDisable();
	wAddr = hyhwDma_GetCurrentDstAddr(UART1_DMA_CHANNEL);
	// �������ݵ�Ŀ�ĵ�ַ, �ϴ�DMA�ж�ʱ�ĵ�ַ��wAddr�����ַ������
	
	if(wAddr > lastIntDstAddr)
	{
		memcpy(pdataBufAddr, (U8 *)lastIntDstAddr, ((U32)wAddr-(U32)lastIntDstAddr));
	}
	else
	{
		U32 count = (U32)pDmaRecvBufAddr - (U32)lastIntDstAddr;
		memcpy(pdataBufAddr, (U8 *)lastIntDstAddr, count);
		memcpy(pdataBufAddr+count, (U8 *)pDmaRecvBufAddr, (U32)wAddr-(U32)pDmaRecvBufAddr);
	}

}

// DMA �ж�����
void isr_Dma_Uart1(void)
{
	lastDealedBytes = INTBUFFSIZE;
/*	hyhwUart1_DmaIntClear();   // ����жϱ�־
	hyhwTimer_intDisable(2);    // �رն�ʱ���ж�
	hyhwmUart1_DmaDisable();   // ͣ��DMA����
#if 0	
	hyhwUart_rxIntDisable(1);  // ����UART1�ĵ��ֽڽ����ж�
#else
	hyhwUart_rxIntDisable(0);  // ����UART1�ĵ��ֽڽ����ж�	
#endif	
	memcpy(pdataBufAddr, (U8 *)lastIntDstAddr, INTBUFFSIZE);
	AdjustBufferParameters();*/
}

void InitBufferParameters(U32 pBufAddr, U32 pDmaBufAddr)
{
	pdataBufAddr   = (U8 *)pBufAddr;
	pDmaRecvBufAddr= (U8 *)pDmaBufAddr;
	lastIntDstAddr = 0;
//	lastIntSrcAddr = 0;
	
	writepos	   = 0;
	readpos        = 0;
//	countBytes     = 0; // �����ʱ�������е���Ч�ֽ���
	
	lastDealedBytes = 0;
	
}
void AdjustBufferParameters(void)
{
	writepos += lastDealedBytes;
	if(writepos> BUFFSIZE)
	{
		writepos -= BUFFSIZE;
	}
	lastIntSrcAddr  = pDmaRecvBufAddr + writepos;
	
	readpos  += lastDealedBytes;
	if(readpos > BUFFSIZE)
	{	
		readpos -= BUFFSIZE;
	}
	
	lastIntDstAddr  = pDmaRecvBufAddr + readpos;
}

/*----------------------------------------------------------------------------

*-----------------------------------------------------------------------------*/
void hyhwDmaSubsys_Init(U8 *buffer, U32 bufSizeInSamples)
{
	/* 
	U8	*pBuffer;		
	U16	writePos;		/*
	U16	readPos; 		/*
							
	U32	sizeInBytes;	/*
	U16	curBytes;		/*
	U16	maxInputSamples;/*
	U16	lastDealedSamples;*/
/*    _tDMA_ROLL_BUFF_MANAGER  *pDmaBuffer;
	pDmaBuffer = &rxBuffer;
	
	 Initialise play buffers 
	pDmaBuffer->pBuffer			= buffer;
	
	pDmaBuffer->sizeInBytes	    = bufSizeInSamples;
	pDmaBuffer->writePos			= 0;
	pDmaBuffer->readPos			= 0;
	pDmaBuffer->maxInputSamples	= (U16)(bufSizeInSamples/4);
	pDmaBuffer->curBytes		= 0;
	pDmaBuffer->lastDealedSamples = 0;*/
	
	/*	��ʼ�� COI (codec interface) Ӳ�� 
	  	����CODEC fifo ģʽ��AD/DA�л�ʱ�������buffer�� ������ͬʱdisable TX ��RX 
	  	ͬʱ��������loop λ����λ����ʱ��fifo��disable�����ж�Ҳ����ֹ*/
	
	/*	��ֹ�����жϵĲ��� */
	

}


// ���Գ�������, for UART1 ����
void test_Uart1_TDMOD(void)
{

	U8 dataSend;
	U8 dataRecv;
	U32 i;
	char *dataSend1 = "I Love family home ";


	hyhwUart_Init( Uart_Baudrate_115200,//Uart_Baudrate_3000000,
	                    Uart_DataBits_8,
	                    Uart_StopBits_1,
	                    Uart_Parity_Disable,
	                    Uart_Fifo_Mode,
	                    0
				      );
	


	// ���ô��ڵĴ�������
	hyhwUart_Ch1Config( Uart_Baudrate_115200,//Uart_Baudrate_3000000,
	                    Uart_DataBits_8,
	                    Uart_StopBits_1,
	                    Uart_Parity_Disable,
	                    Uart_Fifo_Mode
				      );
	hyhwInt_ConfigUart1_ISR(isr_Uart1);		
	hyhwInt_ConfigTimer2_ISR(isr_Timer2_count);	
	
	hyhwmUart1_DmaIntDisable();
	hyhwUart1_DmaIntClear();
#if 0	
	hyhwUart_txIntDisable(1);
	hyhwUart_rxIntDisable(1);
#else
	hyhwUart_txIntDisable(0);
	hyhwUart_rxIntDisable(0);
#endif	
	hyhwmUart1_DmaDisable();

/* �����ж� */	
	hyhwInt_setTriggerLevel(INT_BIT_UART);
	hyhwInt_setTriggerLevel(INT_BIT_DMA);
	hyhwInt_setIRQ(INT_BIT_UART);
	hyhwInt_setIRQ(INT_BIT_DMA); 
	hyhwInt_enable(INT_BIT_UART);
	hyhwInt_enable(INT_BIT_DMA);
	     
	// ����UART�ж�	
#if 0	
	hyhwUart_rxIntEnable(1);
#else
	hyhwUart_rxIntEnable(0);
#endif	
	// DMA����, DMA �ж�
	// ��ʼ��������
	InitBufferParameters((U8 *)0x20300000, (U8 *)0x20100000);
	hyhwDma_Uart1Config(1, BUFFSIZE, 0x20100000, INTBUFFSIZE);
#if 0	
	// ����һ�����������ж�, �Լ��շ�
	for(i=0; i<10; i++)
	if( HY_OK == hyhwUart_WriteFifo(dataSend, 1))
	{
		hyhwUart_ReadFifo(&dataRecv, 1);	
	}
#else
/*	// ����һ�����������ж�, �Լ��շ�
	for(i=0; i<20; i++)
	{
		dataSend = dataSend1[i];
		hyhwUart_WriteFifo(dataSend, 0);
	}
*/	
	//hyhwUart_ReadFifo(&dataRecv, 0);
#endif		
	while(1);
	hyhwUart_ReadFifo(&dataRecv, 0);;


}
#endif


/*-----------------------------------------------------------------------------
*	�޸ļ�¼ 1 :
20090817	������
����:
	1\����ǰ�湤���ķ����Դ��ڲ��ֽ��������޸�
	�����
	#define TD_DATA_UART		UART1	// ���ؿ���,  ����td����ͨ��ʹ�õ�UART1, 3M�ٶ�ͨ��
	#define TD_AT_STRUCT_UART 	UART2 // һ�㴮��
	#define GSM_UART			UART0   // ֧������    

	#define GSM_UART_ADDR		 	TCC7901_UART_BASEADDR0 	
	#define TD_DATA_UART_ADDR		TCC7901_UART_BASEADDR1
	#define TD_AT_STRUCT_UART_ADDR 	TCC7901_UART_BASEADDR2
	ɾ����
	UART1_3M_TDADDR���壬���ں�������ʱ������Ӧ�޸�
	2\���ո�����Ҫ���޸��˳�ʼ����ʼ������
-------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
*	�޸ļ�¼ 2 :
20090819	������
	����˺���:
	hyhwmUart1_Dmarestart;hyhwmUart1_DmaisPause;
	��Ҫ��Ϊ����UART1 ��DMAʱ�ܹ���ͣ��������ֹͣDMA�������ã���ͣ������������
	���Խ���ԭ���ĵط����д��䡣
	
	���ڳ�ʼ������û�и��Ľӿڣ�������Ӳ����·�����˷ֱ�����, ����ǰ���������˴������غ�DMA�Ƿ�ʹ�ܿ���
	
	����Ϊ���ڲ��ָ����е�д�ĺ�������ע�͡�
-------------------------------------------------------------------------------*/
/* 20100513
	���±�д���ڳ�ʼ����������Ϊ102ʹ�õĴ�����Ҫ�õ��˿�5,��uart5��Ϊ�˷����Ժ���ģ����±�д�˳�ʼ������
	������3��������ȷ���Ƿ�֧��mda��flow ctrl. ע��uart1��uart5�ǹ���cts��rts���ŵģ����GPIO������Ҳ�����޸�
	���е��ó�ʼ�������ĵط���Ҫ���޸ģ���ȷ�Ƿ���Ҫ���غ�DMA
*//* 20100513
	���±�д���ڳ�ʼ����������Ϊ102ʹ�õĴ�����Ҫ�õ��˿�5,��uart5��Ϊ�˷����Ժ���ģ����±�д�˳�ʼ������
	������3��������ȷ���Ƿ�֧��mda��flow ctrl. ע��uart1��uart5�ǹ���cts��rts���ŵģ����GPIO������Ҳ�����޸�
	���е��ó�ʼ�������ĵط���Ҫ���޸ģ���ȷ�Ƿ���Ҫ���غ�DMA
*/