

#ifndef HY_HW_UART_H_
#define HY_HW_UART_H_

/*------------------------------------------------------------------------------
Standard include files:
------------------------------------------------------------------------------*/

#ifdef __cplusplus
/*  Assume C declarations for C++ */
extern "C" {
#endif

/*------------------------------------------------------------------------------
Project include files:
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Types and defines:
------------------------------------------------------------------------------*/
//��4��ͨ�õ�UART
#define UART_NUM		3
#define UART0			0
#define UART1			1
#define UART2			2
#define UART3			3

/* ����ӳ���UART �˿�Ϊ6������ͬʱֻ����4���˿��ڹ��� */
#define UART_PORT_NUM	6
#define UART_PORT_0		0
#define UART_PORT_1		1
#define UART_PORT_2		2
#define UART_PORT_3		3
#define UART_PORT_4		4
#define UART_PORT_5		5


#ifdef DEBUG //�������Ǵ���3
#define SCRATCH_UART   UART3
#else
#define SCRATCH_UART   UART0
#endif

#define	UART_BASEADDR		TCC7901_UART_BASEADDR
#define	UART_OFFESTADDR		TCC7901_UART_OFFSETADDR

#define TD_DATA_UART		UART1	// ���ؿ���,  ����td����ͨ��ʹ�õ�UART1, 3M�ٶ�ͨ��
#define TD_AT_STRUCT_UART 	UART2 // һ�㴮��

/* ����ͨ�ŵļ������ڵĻ���ַ���� */
#define GSM_UART_ADDR		 	TCC7901_UART_BASEADDR0 	
#define TD_DATA_UART_ADDR		TCC7901_UART_BASEADDR1
#define TD_AT_STRUCT_UART_ADDR 	TCC7901_UART_BASEADDR2


/*tcc 7901 added*/ 
#define	UART_MUX_BASEADDR	0xF0055400											/* port mux register base address */	
#define	UART_MUX_CHSEL		(*(volatile U32 *)(UART_MUX_BASEADDR + 0x00))		/* channel selection Register read/write*/
#define	UART_MUX_CHST		(*(volatile U32 *)(UART_MUX_BASEADDR + 0x04))		/* channel state Register read only */ 


typedef	volatile struct tuart_register
{
	U32	UART_RBR;
		#define	RECEIVE				UART_RBR
		#define	TRANSMIT			UART_RBR
		#define	DIVISOR_LATCH_LOW	UART_RBR
	U32	UART_IER;
		#define	INT_ENABLE 			UART_IER
		#define	DIVISOR_LATCH_HIGH	UART_IER
	U32	UART_IIR;
		#define	INT_IDENT			UART_IIR
		#define	FIFO_CONTROL		UART_IIR
	U32	UART_LCR;
		#define	LINE_CONRROL		UART_LCR
	U32	UART_MCR;
		#define	MODEM_CONTROL		UART_MCR
	U32	UART_LSR;
		#define	LINE_STATUS			UART_LSR
	U32	UART_MSR;
		#define	MODEM_STATUS		UART_MSR
	U32	UART_SCR;
		#define	SCRATCH				UART_SCR
	U32	UART_AFT;
		#define	AFC_TRIGGER			UART_AFT
	U32	UART_UCR;	
		#define	UART_CONTROL		UART_UCR
} tUART_REGISTER;

/**
 * @brief UART IP3106 baud rate enumeration
 *
 * The UART IP3106 can be configured for different baud rates.
 */
typedef enum euart_Baudrate
{
  Uart_Baudrate_3000000=0,
  Uart_Baudrate_1000000,
  Uart_Baudrate_500000,
  Uart_Baudrate_230400,
  Uart_Baudrate_115200,
  Uart_Baudrate_57600,
  Uart_Baudrate_38400,
  Uart_Baudrate_19200,
  Uart_Baudrate_9600,
  Uart_Baudrate_4800,
  //Uart_Baudrate_3600,
  Uart_Baudrate_2400
} eUART_BAUDRATE;

static const UInt16 sUartDivisor[] = 
{
	/* fuart = 3M */
//	39,      /* 4800 */

	/* fuart = 6M */
//	39,     /* 9600 */

	/* BaudRate = 12 Mhz / (16  * Divisor) */   
//	7,		/*  115200  baud */   clk tolerance is unbearable
//	13,		/*  57600  baud  */
//	20, 	/*  38400  baud  */	   clk tolerance is unbearable
//	39, 	/*  19200  baud  */
//	78, 	/*   9600  baud  */    ---

	/* BaudRate = 24 Mhz / (16  * Divisor) */   
//	13,		/*  115200  baud  */   
// 	26,		/*  57600  baud  */
//  39, 	/*  38400  baud  */
//  78, 	/*  19200  baud  */ --
//  156, 	/*   9600  baud  */ -- 
//  416, 	/*   4800  baud  */ --
//  625, 	/*   2400  baud  */

	/* BaudRate = 48 Mhz / (16  * Divisor) */   
	1,		/*	3000000	baud */
	3,		/*	1000000	baud */
	6,		/*	500000	baud */
	26,		/*  115200  baud */  //--
	52,		/*  57600  baud  */  //--
	78, 	/*  38400  baud  */ //--
	156, 	/*  19200  baud  */ //--
	313, 	/*   9600  baud  */ //--

};
#include "hyhwCkc.h"

typedef struct{
	ePERI_CLK  uart_clk;
	UInt16 sDivisor;
} tUART_BAUDCONFIG;


static const tUART_BAUDCONFIG g_arrayBaudRate[] = 
{
	/* �����޸Ķ��巽ʽ����ҪĿ����Ϊ�˷�Ƶʱ��ϵ���ӽ�����������ƫ�� */
	{PERI_CLK_48M,  1},
	{PERI_CLK_48M,  3},
	{PERI_CLK_48M,  6},
	{PERI_CLK_48M,  13},
	{PERI_CLK_24M,  13},
	{PERI_CLK_24M,  26},
	{PERI_CLK_24M,  39},
	{PERI_CLK_12M,  39},
	{PERI_CLK_6M,   39},
	{PERI_CLK_3M,   39},	
	{PERI_CLK_24M,  625},
};



/* ˵��: ����ı仯��Ҫ�ǻ��ڵ���ģ��Ƶ�ʵ�ѡ�� 
	     ��ѡ����Ƕ��ٵ�12M/24M/48M ������ѡ��Ĳ����ʷ�Ƶ��ֵ */
/**
 * @brief UART IP3106 data bits enumeration
 *
 * The UART IP3106 can be configured for different data bits length.
 */
typedef enum euart_data_bits
{
  Uart_DataBits_5 = 0x00, 
  Uart_DataBits_6 = 0x01, 
  Uart_DataBits_7 = 0x02, 
  Uart_DataBits_8 = 0x03
} eUART_DATA_BITS;

/**
 * @brief UART IP3106 stop bits enumeration
 *
 * The UART IP3106 can be configured for different stop bit length.
 */
typedef enum euart_stop_bits
{
  Uart_StopBits_1 = 0x00, 
  Uart_StopBits_2 = 0x04 
} eUART_STOP_BITS;

/**
 * @brief UART IP3106 parity enumeration
 *
 * The UART IP3106 has the ability to configure parity checking.
 */
typedef enum euart_parity
{
  Uart_Parity_Disable  = 0x00, /* parity checking disabled */
  Uart_Parity_Odd      = 0x08, /* data+parity bit will have and odd  numer of 1s.*/
  Uart_Parity_Even     = 0x18, /* data+parity bit will have and even numer of 1s.*/
  Uart_Parity_Mark     = 0x28, /* parity is always 1 */
  Uart_Parity_Space    = 0x38  /* parity is always 0 */  
} eUART_PARITY;

typedef enum euart_mode
{
  Uart_No_Fifo_Mode		= 0x00, /* UART Work is no fifo mode */
  Uart_Fifo_Mode		= 0x01  /* UART Work is fifo mode.*/
} eUart_Mode;

typedef enum
{
	FLOW_UNSUPPORT,
	FLOW_SUPPORT
}e_UARTFLOW_CTRL;

typedef enum
{
	DMA_UNSUPPORT,
	DMA_SUPPORT
}e_UARTDMA_CTRL;


/*------------------------------------------------------------------------------
Exported Global variables:
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Exported Function protoypes:
------------------------------------------------------------------------------*/
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
U32 hyhwUart_Init(eUART_BAUDRATE baudrate,
                           eUART_DATA_BITS	dataBits,
                           eUART_STOP_BITS	stopBits,
                           eUART_PARITY   	parity,
                           eUart_Mode	  	uartMode,
                           U8 uartId
						   );
#endif
/*-----------------------------------------------------------------------------
* ����:	hyhwUart_txIntEnable
* ����:	��Uart �ж�ʹ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_txIntEnable(U8 uartId);

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_rxIntEnable
* ����:	��Uart �ж�ʹ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_rxIntEnable(U8 uartId);

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_txIntDisable
* ����:	�ر�Uart �ж�ʹ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_txIntDisable(U8 uartId);

/*---------------------------------------------------------------------------
* ��  :	hyhwmUart1_DmaPause 
* ����:	��ͣ Uart1 Dma ����
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmUart1_DmaPause(void);

/*---------------------------------------------------------------------------
* ��  :	hyhwmUart1_Dmarestart 
* ����:	������ͣλ�ÿ�ʼ���� Uart1 Dma ����
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmUart1_Dmarestart(void);

/*---------------------------------------------------------------------------
* ��  :	hyhwmUart1_DmaisPause 
* ����:	�ж�DMA�Ƿ�����ͣ״̬
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
BOOL hyhwmUart1_DmaisPause(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_rxIntDisable
* ����:	�ر�Uart �ж�ʹ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwUart_rxIntDisable(U8 uartId);
/*-----------------------------------------------------------------------------
* ����:	hyhwUart_CheckRecDataReady(void)
* ����:	�ж��Ƿ������ݽ��յ�
* ����:	none
* ����:	HY_OK(�����ݣ���HY_ERROR(�����ݣ�
*----------------------------------------------------------------------------*/
U32 hyhwUart_CheckRecDataReady(U8 uartId);

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_ReadFifo
* ����:	��UART��fifo����һ���ֽ�
* ����:	none
* ����:	�������ֽ�
*----------------------------------------------------------------------------*/
U32 hyhwUart_ReadFifo(U8 * data,U8 uartId);

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_WriteFifo
* ����:	��UART��fifoд��һ���ֽ�
* ����:	д����ֽ�
* ����:	HY_OK		-- �ɹ�
*		HY_ERROR	-- ��ʱ
*----------------------------------------------------------------------------*/
U32 hyhwUart_WriteFifo(U8 data,U8 uartId);

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_PortSet(U32 port0, U32 port1, U32 port2, U32 port3)
* ����:	���ø������ڶ�Ӧ�Ķ˿ں�
* ����:	��
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwUart_PortSet(U32 port0, U32 port1, U32 port2, U32 port3);

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_StateRead(void)
* ����:	��ȡ״̬�Ĵ�����������
* ����:	��
* ����:	״̬�Ĵ����е�ֵ
*----------------------------------------------------------------------------*/
U32 hyhwUart_StateRead(void);


/*-----------------------------------------------------------------------------
* ����:	hyhwUart_sendData
* ����:	��������
* ����:	uartId------ע: ���id �Ǵ��ڶ˿�id, tg3Ӧ���п�ѡ����
					UART0/UART1/UART2, ����ͨ�����ò�����
		pdata ------���ݻ�����ָ��
		len   ------�������ݳ���
* ����: �������ݳ���	
*----------------------------------------------------------------------------*/
U32 hyhwUart_sendData(U8 uartId, void *pdata, U32 len);

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_recvData
* ����:	��������
* ����:	uartId------ע: ���id �Ǵ��ڶ˿�id, tg3Ӧ���п�ѡ����
					UART0/UART1/UART2, ����ͨ�����ò�����
		pdata ------���ݻ�����ָ��
		len   ------�������ݳ���
* ����: �������ݳ���	
*----------------------------------------------------------------------------*/
U32 hyhwUart_recvData(U8 uartId, void *pdata, U32 len);

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_recvData1
* ����:	�������ݣ��ú����ɸ����б�д��������ĺ����������ƣ��ɲ�ͬģ�����
* ����:	uartId------ע: ���id �Ǵ��ڶ˿�id, tg3Ӧ���п�ѡ����
					UART0/UART1/UART2, ����ͨ�����ò�����
		pdata ------���ݻ�����ָ��
		len   ------�������ݳ���
* ����: �������ݳ���	
*----------------------------------------------------------------------------*/
U32 hyhwUart_recvData1(U8 uartId, void *pdata, U32 len);

/*-----------------------------------------------------------------------------
* ����:	hyhwUart_Initx
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
                           U8 uartId, // ӳ���uart channnel
                           U8 port,    // ��Ӧ������Ķ˿�
                           e_UARTFLOW_CTRL isFlowSuppot, // �Ƿ�֧������
                           e_UARTDMA_CTRL isDmaSupport  // �Ƿ�֧��Dma
						   );

#ifdef __cplusplus
}
#endif

#endif /* TM_LLIC_API_UART_H_ */




