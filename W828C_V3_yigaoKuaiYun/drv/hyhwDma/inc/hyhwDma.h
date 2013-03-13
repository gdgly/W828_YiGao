/**
 * \defgroup vhApiSdma
 * @brief Describes the vhApi for the DMA controller.
 *
 * The dma driver provides an interface to configure and enable the DMA 
 * engine. The services are implemented. This functionality can be
 * enhanced in order to make full use of the services provided by the DMA engine.
 * This is the only interface that upper layers may use in order to access the
 * services of the SDMA engine.
 **/


#ifndef _HYHW_DMA_H_
#define _HYHW_DMA_H_

#define CODEC_BUFF_MAX	0xFFFF0									/* DAIģ��ʹ�õı����ռ仺������С */
#define CODEC_BUFF_MIN	0x10

#define LCD_CPU_INTERFACE_ID		0xf0000818
#define LCD_DMA_REGISTER_ADDR		0xf0040000 					/* dma0 channel 0 */
#define DAI_INTERFACE_IDO			0xf0059020					/* DAI DADO ���ͼĴ����׵�ַ */
#define DAI_INTERFACE_IDOR			0xf0059024					/* �������׵�ַ */
#define DAI_INTERFACE_IDI			0xf0059000					/* DAI DADO ���ռĴ����׵�ַ */
#define DAI_INTERFACE_IDIR			0xf0059004					/* DAI DADO ���ռĴ����׵�ַ */
#define DAI_DMA_REGISTER_ADDR		0xf0040100 					/* dma1 channel 0 */
#define DAI_DMA_LEFT_REGISTER_ADDR	0xf0040100					/* dma1 channel 0 */
#define DAI_DMA_RIGHT_REGISTER_ADDR	0xf0040130					/* dma1 channel 1 */
#define DAI_DMA_RECORD_REGISTER_ADDR 0xf0040160

/*  20090805
	SPIͨ������, ѡ��DMAͨ��Ϊ DMA2��channel 0, 
	added by yanglijing, 20090805 
	------------------------------------------>
*/
#define SPI_DMA_REGISTER_ADDR		0xf0040200					/* ����SPIͨ��DMA�����ͨ�� dma2 channel 0 */
#define SPI_DMA_TX_DST_REGISTER		0xf0057000					/* GPSB��ͨ��0�����ݶ�д�˿ڵ�ַ */
#define SPI_DMA_RX_SRC_REGISTER		0xf0057000					/* GPSB��ͨ��0�����ݶ�д�˿ڵ�ַ */
/* <-------------------------------------------
	SPIͨ������, ѡ��DMAͨ��Ϊ DMA2��channel 0, 
	end added20090805 
*/

/*  20090806
	UARTͨ������, ѡ��DMAͨ��Ϊ DMA2��channel 1, 
	ѡ��UART channel 1 ӳ�䵽 port1����UART1
	added by yanglijing, 20090805 
	------------------------------------------>
*/
#define UART1_DMA_REGISTER_ADDR		0xf0040230					/* ����3M UART1ͨ��DMA�����ͨ�� dma2 channel 1 */
#define UART1_DMA_TX_DST_REGISTER	0xf0055100					/* UART1��д���ݼĴ�����ַ THR, CPUд��, ��������ȡ */
#define UART1_DMA_RX_SRC_REGISTER	0xf0055100					/* UART1�Ķ����ݼĴ�����ַ RBR, ��THR����һ���Ĵ��� */
/* <-------------------------------------------
	SPIͨ������, ѡ��DMAͨ��Ϊ DMA2��channel 0, 
	end added20090805 
*/


#define	DMA_TOTAL_CHANNEL_NUM		9
#define DMA_CHANNEL_NUM_PER_DMA		3
#define DMA_CHANNEL_OFFSET			0x30

#define DMA_DAI_LRBUF_OFFSET		8							/* DAI �Ĵ�����/��ͨ����ƫ����, ��ֵ��Ҫ��Ϊ��DAI/DMA�������� */
#define DMA_PORT_TYPE_WORD 			4
#define DMA_PORT_TYPE_HALF_WORD 	2
#define DMA_PORT_TYPE_BYTE 			1
#define DMA_PORT_TYPE_STATIC 		0

#define DMA0_BASEADDR			TCC7901_GDMA0_BASEADDR
#define DMA1_BASEADDR			TCC7901_GDMA1_BASEADDR
#define DMA2_BASEADDR			TCC7901_GDMA2_BASEADDR
#define DMA0_CONFIG_REGISTER	TCC7901_DMA0_CONFIG_REGISTR		//channel configuration register
#define DMA1_CONFIG_REGISTER	TCC7901_DMA1_CONFIG_REGISTR
#define DMA2_CONFIG_REGISTER	TCC7901_DMA2_CONFIG_REGISTR

//DMA control bit define 
	#define CHCTRL_BIT_ISSUE_CONTINU_TRANSFER					BIT15
	#define CHCTRL_BIT_ISSUE_STARTADDR_TRANSFER					~BIT15
	#define CHCTRL_BIT_DIFFERENTIAL_TRANSFER_MODE_ENABLE		BIT14
	#define CHCTRL_BIT_DIFFERENTIAL_TRANSFER_MODE_DISABLE		~BIT14
	#define CHCTRL_BIT_HW_REQUEST_SYNC_ENABLE					BIT13
	#define CHCTRL_BIT_HW_REQUEST_SYNC_DISABLE					~BIT13
	#define CHCTRL_BIT_HW_REQUEST_DIRECTION_WRITE				BIT12
	#define CHCTRL_BIT_HW_REQUEST_DIRECTION_READ				0
	#define CHCTRL_BIT_TRANSFER_LOCKED							BIT11
	#define CHCTRL_BIT_TRANSFER_UNLOCK							~BIT11
	#define CHCTRL_BIT_TRANSFER_BURST_MODE						BIT10
	#define CHCTRL_BIT_TRANSFER_ARBITRATION_MODE				~BIT10
	
	#define CHCTRL_BIT_TRANSFER_TYPE_SINGLE_EDGE				(0x0 << 8)
	#define CHCTRL_BIT_TRANSFER_TYPE_HARDWARE					(0x1 << 8)
	#define CHCTRL_BIT_TRANSFER_TYPE_SOFTWARE					(0x2 << 8)
	#define CHCTRL_BIT_TRANSFER_TYPE_SINGLE_LEVEL				(0x3 << 8)

	
	

	#define CHCTRL_BIT_BURST_SIZE_1								(0x0 << 6)
	#define CHCTRL_BIT_BURST_SIZE_2								(0x1 << 6)
	#define CHCTRL_BIT_BURST_SIZE_4								(0x2 << 6)
	#define CHCTRL_BIT_BURST_SIZE_8								(0x3 << 6)
	
	#define CHCTRL_BIT_WORD_SIZE_8BIT							(0x0 << 4)
	#define CHCTRL_BIT_WORD_SIZE_16BIT							(0x1 << 4)
	#define CHCTRL_BIT_WORD_SIZE1_32BIT							(0x2 << 4)
	#define CHCTRL_BIT_WORD_SIZE2_32BIT							(0x3 << 4)
	
	#define CHCTRL_BIT_DMA_DONE_FLAG							BIT3
	#define CHCTRL_BIT_DMA_INT_ENABLE							BIT2
	#define CHCTRL_BIT_DMA_INT_DISABLE							~BIT2
	#define CHCTRL_BIT_REPEAT_MODE_ENABLE						BIT1
	#define CHCTRL_BIT_REPEAT_MODE_DISABLE						~BIT1
	#define CHCTRL_BIT_DMA_START								BIT0
	#define CHCTRL_BIT_DMA_STOP									~BIT0

//start external DMA request register bit define
	#define	EXTREQ_BIT_UART1RX									BIT30	// UART channel 1 receiver
	#define	EXTREQ_BIT_UART1TX									BIT29	// UART channel 1 transmitter
	#define	EXTREQ_BIT_UART0RX									BIT27	// UART channel 0 receiver
	#define	EXTREQ_BIT_UART0TX									BIT26	// UART channel 0 transmitter
	#define	EXTREQ_BIT_I2C1										BIT25	// I2C channel 1
	#define EXTREQ_BIT_DAIRX									BIT24	// DAI Receiver
	#define EXTREQ_BIT_DAITX									BIT23	// DAI transmitter
	#define EXTREQ_BIT_CDIF										BIT22	// CD interface 
	#define EXTREQ_BIT_SPDIFU									BIT21	// SPDIF User data
	#define EXTREQ_BIT_SPDIFP									BIT20	// SPDIF Paceket(audio) data
	#define EXTREQ_BIT_I2C0										BIT19	// I2C channel 0
	#define EXTREQ_BIT_NFC										BIT18	// Nand Flash Controller
	#define EXTREQ_BIT_MSTICK									BIT17	// memory stick
	#define EXTREQ_BIT_ECC										BIT12	// ECC
	#define EXTREQ_BIT_UART3RX									BIT11	// UART channel 3 receiver
	#define EXTREQ_BIT_UART3TX									BIT10	// UART channel 3 transmitter
	#define EXTREQ_BIT_UART2RX									BIT9	// UART channel 2 receiver
	#define EXTREQ_BIT_UART2TX									BIT8	// UART channel 2 transmitter
	#define EXTREQ_BIT_GPSB3RX									BIT7	// GPSB channel 3 RX
	#define EXTREQ_BIT_GPSB2RX									BIT6	// GPSB channel 2 RX
	#define EXTREQ_BIT_GPSB1RX									BIT5	// GPSB channel 1 RX
	#define EXTREQ_BIT_GPSB0RX									BIT4	// GPSB channel 0 RX
	#define EXTREQ_BIT_GPSB3TX									BIT3	// GPSB channel 3 TX
	#define EXTREQ_BIT_GPSB2TX									BIT2	// GPSB channel 2 TX
	#define EXTREQ_BIT_GPSB1TX									BIT1	// GPSB channel 1 TX
	#define EXTREQ_BIT_GPSB0TX									BIT0	// GPSB channel 0 TX	 
// end external DMA request register bit define

// start DMA channel configuration register  bit define
	#define CHCONFIG_BIT_CH2_INT_STATUS							BIT22
	#define CHCONFIG_BIT_CH1_INT_STATUS							BIT21
	#define CHCONFIG_BIT_CH0_INT_STATUS							BIT20
	// ������status����������Щbitֻ����INT ENABLE �£�����Ч
	#define CHCONFIG_BIT_CH2_INT_MASKED_STATUS					BIT18
	#define CHCONFIG_BIT_CH1_INT_MASKED_STATUS					BIT17
	#define CHCONFIG_BIT_CH0_INT_MASKED_STATUS					BIT16 
	
	#define CHCONFIG_BIT_CH2_DATA_SWAP_ENABLE					BIT10
	#define CHCONFIG_BIT_CH1_DATA_SWAP_ENABLE					BIT9
	#define CHCONFIG_BIT_CH0_DATA_SWAP_ENABLE					BIT8
	
	#define CHCONFIG_BIT_CH_PRIORITY_012						(0x0 << 4)
	#define CHCONFIG_BIT_CH_PRIORITY_021						(0x1 << 4)
	#define CHCONFIG_BIT_CH_PRIORITY_102						(0x2 << 4)
	#define CHCONFIG_BIT_CH_PRIORITY_120						(0x3 << 4)
	#define CHCONFIG_BIT_CH_PRIORITY_210						(0x4 << 4)
	#define CHCONFIG_BIT_CH_PRIORITY_201						(0x5 << 4)
	
	#define CHCONFIG_BIT_CH_PRIORITY_FIXED						~BIT0
	#define CHCONFIG_BIT_CH_PRIORITY_CYCLIC						BIT0
// end 	DMA channel configuration register  bit define


// start DMA channel Repeat control �Ĵ��� bit ����	
	#define RPTCTRL_BIT_DRI_REPEAT_INT_DISABLE   BIT31
	#define RPTCTRL_BIT_EOT_REPEAT_INT_DISABLE   BIT30

// end DMA channel Repeat control �Ĵ��� bit ����	  


/***************************for test*********************************/	 
#define ST_SADR			0x00
#define SPARAM			0x04
#define C_SADR			0x0C
#define ST_DADR			0x10
#define DPARAM			0x14
#define C_DADR			0x1C
#define HCOUNT			0x20
#define CHCTRL			0x24
#define RPTCTRL			0x28
#define EXTREQ			0x2C

#define GDMA0_CH0		0xf0040000
#define GDMA0_CH1		0xf0040030
#define GDMA0_CH2		0xf0040060

#define GDMA1_CH0		0xf0040100
#define GDMA1_CH1		0xf0040130
#define GDMA1_CH2		0xf0040160

#define GDMA2_CH0		0xf0040200
#define GDMA2_CH1		0xf0040230
#define GDMA2_CH2		0xf0040260


#define DMA_REG(X,Y)		*( volatile U32 *)(X+Y)

/*---------------------------------------------------------------------------
   Types and defines:
   --------------------------------------------------------------------------*/
typedef struct _tDMA_REGISTER 
{
  U32 SOURCE_START_ADDR ;
  U32 SOURCE_PARAMETER ;
  U32 _SOURCE_PARAMETER ;
  U32 SOURCE_CURRENT_ADDR;
  U32 DESTINATION_START_ADDR;
  U32 DESTINATION_PARAMETER;
  U32 _DESTINATION_PARAMETER;
  U32 DESTINATION_CURRENT_ADDR;
  U32 HOP_COUNT;
  U32 CHANNEL_CONTROL;
  U32 REPEAT_CONTROL;
  U32 EXTERNAL_DMA_REQUEST;
}tDMA_REGISTER;

typedef struct  
{
  tDMA_REGISTER CH0;
  tDMA_REGISTER CH1;
  tDMA_REGISTER CH2;
}tDMA_CH;

typedef struct  
{
  tDMA_REGISTER CH0;
  tDMA_REGISTER CH1;
  tDMA_REGISTER CH2;
}tDMA;

typedef enum 
{
	channle0,
	channle1,
	channle2,
	channle3,
	channle4,
	channle5,
	channle6,
	channle7,
	channle8
	
} eDMA_CHANNEL;


typedef enum _eDmaPortWidth_en
{
	DmaPortWidth32bits,
	DmaPortWidth08bits,
	DmaPortWidth16bits,
	DmaPortWidthReserved	//����Ķ˿ڿ��
	
} eDMAPORTWIDTH_EN;

typedef enum eDmaHardwareModule_en
{
	UT1_RX	=	BIT30	,	// UART channel 1 receiver
	UT1_TX	=	BIT29	,	// UART channel 1 transmitter
	UT0_RX	=	BIT27	,	// UART channel 0 receiver
	UT0_TX	=	BIT26	,	// UART channel 0 transmitter
	I2C_1	=	BIT25	,	// I2C channel 1
	DAI_RX	=	BIT24	,   // DAI Receiver
	DAI_TX	=	BIT23	,   // DAI transmitter
	CD_IF	=	BIT22	,	// CD interface 
	SPDIF_U	=	BIT21	,   // SPDIF User data
	SPDIF_P	=	BIT20	,   // SPDIF Paceket(audio) data
	I2C_0	=	BIT19	,	// I2C channel 0
	NFC_N	=	BIT18	,	// Nand Flash Controller
	MS_TICK	=	BIT17	,   // memory stick
	ECC_E	=	BIT12	,	// ECC
	UT3_RX	=	BIT11	,	// UART channel 3 receiver
	UT3_TX	=	BIT10	,	// UART channel 3 transmitter
	UT2_RX	=	BIT9	,	// UART channel 2 receiver
	UT2_TX	=	BIT8	,	// UART channel 2 transmitter
	GSB3_RX	=	BIT7	,	// GPSB channel 3 RX
	GSB2_Rx	=	BIT6	,	// GPSB channel 2 RX
	GSB1_RX	=	BIT5	,	// GPSB channel 1 RX
	GSB0_RX	=	BIT4	,	// GPSB channel 0 RX
	GSB3_TX	=	BIT3	,	// GPSB channel 3 TX
	GSB2_TX	=	BIT2	,	// GPSB channel 2 TX
	GSB1_TX	=	BIT1	,	// GPSB channel 1 TX
	GSB0_TX	=	BIT0		// GPSB channel 0 TX	 
	
} eDMAHARDWAREMODULE_EN;


typedef enum 
{
	dmaPortTypeStatic,
	dmaPortTypeByte,
	dmaPortTypeHalfWord,
	dmaPortTypeWord = 4
} eDMAPORTTYPE;

typedef enum eDmaHardwareModuleRW_en
{
	HardwareRead,
	HardwareWrite
} eDMAHARDWAREMODULERW_EN;

typedef enum eLcdBusType_en
{
	LcdBusType16,
	LcdBusType32

}eLCDBUSTYPE_EN;

typedef enum eDaiTransType
{
	DaiTxType,	// DAI ����
	DaiRxType	// DAI ����
}eDAI_TRANS_TYPE;


typedef enum eSpiTransType
{
	TxType,	// DMA ����
	RxType	// DMA ����
}eSPI_TRANS_TYPE, e_TRANS_TYPE;

/*---------------------------------------------------------------------------
   Function prototypes:
   --------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
* ����:	hyhwDma_Init
* ����:	��ʼ��DMA
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwDma_Init(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_Init
* ����:	�ر�DMA��Դ���Խ�ʡ�õ�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwDma_DeInit(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_Config
* ����:	����DMA
* ����:	sa------����Դ��ַ
*		da------����Ŀ�ĵ�ַ
*		size--��Ҫ���������( 32 bit word )
* ����:	���õ�ͨ���ţ��������ȼ��������ȿ��õ�ͨ��
*		HY_ERR_BUSY:�޿��õ�DMA
*----------------------------------------------------------------------------*/
U32 hyhwDma_Config(U32 sa, U32 da, U32 size);

/*-----------------------------------------------------------------------------
* ��  :	hyhwmDai_DmaIntEnable 
* ����:	ʹ�� Dai Dma ����
* ����:	ChannelId DMAͨ��
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*-----------------------------------------------------------------------------*/		
U32 hyhwDma_Enable(U8 ChannelId);

/*----------------------------------------------------------------------------
* ��  :	hyhwmDai_DmaIntDisable 
* ����:	disable Dai Dma ����
* ����:	ChannelId DMAͨ��
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*-----------------------------------------------------------------------------*/
U32 hyhwDma_Disable(U8 ChannelId);

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_IntClear
* ����:	���ָ��ͨ����DMA�ж�
* ����:	ChannelId--ͨ����
* ����:	HY_OK
*		HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDma_IntClear(U8 ChannelId);

/*-----------------------------------------------------------------------------
* ��  :	hyhwmDai_DmaIntEnable 
* ����:	enable dma rxtx interrupt
* ����:	ChannelId DMAͨ��
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*-----------------------------------------------------------------------------*/		
U32 hyhwDma_IntEnable(U8 ChannelId);

/*----------------------------------------------------------------------------
* ��  :	hyhwmDai_DmaIntDisable 
* ����:	disable dma tx interrupt
* ����:	ChannelId DMAͨ��
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*-----------------------------------------------------------------------------*/
U32 hyhwDma_IntDisable(U8 ChannelId);

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_GetStatus
* ����:	���ָ��ͨ����DMA��״̬������/busy��
*			�Ȳ鿴��ͨ���Ƿ�����---����
*			���ݴ������-----------����
*			�������ڴ���-----------������
* ����:	ChannelId--ͨ����
* ����:	HY_OK
*		HY_ERR_BUSY
*		HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDma_GetStatus(U8 ChannelId);	
/*-----------------------------------------------------------------------------
* ����:	hyhwDma_isDmaInterruptSet
* ����:	�ж��Ƿ��ж�ʹ��
* ����:	ChannelId
*		ChannelId--ͨ����
* ����:	True for False
*----------------------------------------------------------------------------*/
BOOL hyhwDma_isDmaInterruptSet(U8 ChannelId);

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_ParameterSet 
* ����:	��������ָ��ͨ���Ĳ������������ó�ѭ��DMA��DMAÿ������/��С�ĵ�ַ�������ã�
*       ��ַҲ���Թ̶�
* ����:	ChannelId--ͨ����
*		SourceAddrMask--DMA��������Щsource��ַ�߹̶����䣬�������ô����Խ���ѭ��/����DMA
*		SourceIncreaseStep--DMA������source��ַ�ı仯���ã�8λ�з��ţ�bit7��ʾ��/��
*		DestinationAddrMask--DMA��������ЩDestination��ַ�߹̶����䣬�������ô����Խ���ѭ��/����DMA
*		DestinationIncreaseStep--DMA������Destination��ַ�ı仯���ã�8λ�з��ţ�bit7��ʾ��/��
* ����:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note:	ChannelId�ķ�Χ0~8��0��2ʹ��DMA0��3��5ʹ��DMA1, 6~~8ʹ��DMA2����Ϊ�������
*----------------------------------------------------------------------------*/
U32 hyhwDma_ParameterSet(
						 	U8 ChannelId,
						 	U32 SourceAddrMask,
						 	S8 SourceIncreaseStep,
						 	U32 DestinationAddrMask,
						 	S8 DestinationIncreaseStep
						 	);

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_InterruptSet 
* ����:	����dma�ж�ʹ��
* ����:	ChannelId--ͨ����
*		Enable_en IntCtrl--EnableOff �жϽ�ֹ
*						 --EnableOn  �ж�����
* ����:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note: 	ChannelId�ķ�Χ0~8��0��2ʹ��DMA0��3��5ʹ��DMA1, 6~~8ʹ��DMA2����Ϊ�������
*----------------------------------------------------------------------------*/
U32 hyhwDma_HWModeSet(
							U8 ChannelId,
							eDMAHARDWAREMODULERW_EN ReadWrite,
							eDMAHARDWAREMODULE_EN HardwareType
						);
	
/*-----------------------------------------------------------------------------
* ����:	hyhwDma_uart0TxConfig
* ����:	����DMA uart0 Tx DMA ʹ�ܴ���
* ����:	ChannelId------ѡ�����ڴ����DMAͨ��
*		sa------����Դ��ַ��ַ
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart0TxConfig(U32 ChannelId, U32 sa);

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_uart0RxConfig
* ����:	����DMA uart0 Rx DMA ʹ�ܴ���
* ����:	ChannelId------ѡ�����ڴ����DMAͨ��
*		sa------����Դ��ַ��ַ
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart0RxConfig(U32 ChannelId, U32 sa);

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_uart2TxConfig
* ����:	����DMA uart2 Tx DMA ʹ�ܴ���
* ����:	ChannelId------ѡ�����ڴ����DMAͨ��
*		sa------����Դ��ַ��ַ
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart2TxConfig(U32 ChannelId, U32 sa);

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_uart2RxConfig
* ����:	����DMA uart2 Rx DMA ʹ�ܴ���
* ����:	ChannelId------ѡ�����ڴ����DMAͨ��
*		sa------����Դ��ַ��ַ
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart2RxConfig(U32 ChannelId, U32 sa);

/*-----------------------------------------------------------------------------
* ����:	isDaiDmaDmaOK
* ����:	�ж�dai DMA�Ƿ���ɣ������������жϣ�����TRUE�����򷵻�FAULSE
* ����:	none
* ����:	TRUE or FALSE		
*----------------------------------------------------------------------------*/
BOOL isDaiDmaDmaOK(eDAI_TRANS_TYPE daiTransferType);

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_lcdCpuConfig
* ����:	����lcd ��DMA
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwDma_lcdCpuConfig(eLCDBUSTYPE_EN  busType);

/*-----------------------------------------------------------------------------
* ����:	lcdCpuEnable
* ����:	ʹ��lcd ��DMA
* ����:	sa		--	����Դ��ַ
*		size	--	��Ҫ����İ�����
* ����:	none		
*----------------------------------------------------------------------------*/
U32 lcdCpuEnable(U16 * pdata, U32 size);

/*-----------------------------------------------------------------------------
* ����:	lcdCpuDmaOK
* ����:	�ж�LCD DMA�Ƿ���ɣ������������жϣ�����TRUE�����򷵻�FAULSE
* ����:	none
* ����:	TRUE or FALSE		
*----------------------------------------------------------------------------*/
BOOL lcdCpuDmaOK(void);		

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_isDmaInterruptSet
* ����:	�ж��Ƿ��ж�ʹ��
* ����:	ChannelId
*		ChannelId--ͨ����
* ����:	True for False
*----------------------------------------------------------------------------*/
BOOL hyhwDma_isDmaInterruptSet(U8 ChannelId);


			
/*-----------------------------------------------------------------------------
* ����:	hyhwDma_Config
* ����:	����DMA
* ����:	sa------����Դ��ַ
*		da------����Ŀ�ĵ�ַ
*		size--��Ҫ������ֽ���
* ����:	HY_OK 
*		HY_ERR_BUSY:�޿��õ�DMA
*----------------------------------------------------------------------------*/
U32 hyhwDma_Config(U32 sa, U32 da, U32 size);


/*-----------------------------------------------------------------------------
* ����:	hyhwDma_DaiDmaConfig
* ����:	����DMA DAI DMA ʹ�ܴ���, ָ���̶�ͨ��DAI_DMA_REGISTER_ADDR
*		�ú�������һ��������ָ���ض�ͨ������DMA����, ��ǰ�ĺ���̫���鷳������3��)
*		�ú����������,�����Ķ���Ҫ�ǻ��ں�codecģ������ͨ���Ĵ��䣬����2��DMA
*		,�ڸ����а�����ʵ�֡�
* ����:	daiTransferType------ָ����������, ��ѡֵ(0,1): 0��ʾ���ͣ� 1 ��ʾ����
*		size------��������С, ����ָ��DMA����Ļ�������С,�û��������û�������
*		pBufLeftAddr---------��Ƶ������������ָ�룬������DMA������׵�ַ
*		pBufRightAddr--------��Ƶ������������ָ�룬������DMA������׵�ַ
*		sampleCount----------�����Ӹ������ò�������HOPCOUNTֵ
*----------------------------------------------------------------------------*/
void hyhwDma_DaiDmaConfig(eDAI_TRANS_TYPE daiTransferType, U32 size, 
						U16 *pBufLeftAddr, U16 *pBufRightAddr, U32 sampleCount);

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_DaiDmaEnable
* ����:	����DAI DMA��DMA����
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwDma_DaiDmaEnable(eDAI_TRANS_TYPE daiTransferType);

/*----------------------------------------------------------------------------
* ����:	hyhwDma_SetDaiSampleCount
* ����:	 
* ����:	sampleCount				--  Tx/RX sample count 
* ����:	HY_OK					--	�ɹ�
		HY_ERR_BAD_PARAMETER 	--	�����������
*----------------------------------------------------------------------------*/
U32 hyhwDma_SetDaiSampleCount(U32 sampleCount, eDAI_TRANS_TYPE daiTransferType);

// ��ʼSPI DMA ���������ӿ�����
/*-----------------------------------------------------------------------------
* ����:	hyhwDma_DaiDmaConfig	added by yanglijing, 2009-08-05
* ����:	����DMA SPI DMA ʹ�ܴ���, ָ���̶�ͨ��SPI_DMA_REGISTER_ADDR
* ����:	daiTransferType------ָ����������, ��ѡֵ(0,1): 0��ʾ���ͣ� 1 ��ʾ����
*		size-----------------��������С
*		pBufAddr-------------DMA�����׵�ַ
*		count----------------�����ж���ֵ, �ò�������HOPCOUNTֵ, �ֽڱ�ʾ
*----------------------------------------------------------------------------*/
void hyhwDma_SpiDmaConfig(eSPI_TRANS_TYPE TransferType, U32 size, U32 *pBufAddr, U32 Count);						

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_SpiDmaEnable
* ����:	����SPI DMA��DMA����
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwDma_SpiDmaEnable();

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_uart1Config
* ����:	����DMA uart1 Tx DMA ʹ�ܴ���
* ����:	transType---------------DMA���䷽��, ��ѡ����TxType/RxType
*		buffsize----------------���λ�������С, ���ֽڼ�
*		sourceAddr--------------����Դ��ַ��ַ
*		bytesCount--------------DMAһ���жϵ��ֽ���
* ����: none
* ����: hyhwDma_uart1Confit(TxType, 512, 0x2010000, 128)
*		���ô���DMAͨ������Ϊ���λ�������С512�ֽڣ�����ģʽ��ÿ128�ֽ�һ��DMA�ж�
*----------------------------------------------------------------------------*/
void hyhwDma_Uart1Config(e_TRANS_TYPE transType, U32 buffsize, U32 sourceAddr, U32 bytesCount);

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_GetCurrentSrcAddr 
* ����:	��ѯ��ǰDMAͨ����ǰ�����Դ��ַ
* ����:	ChannelId--ͨ����
* ����:	��ǰDMA����Դ��ֵַ/ ����HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDma_GetCurrentSrcAddr(U32 ChannelId);

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_GetCurrentDstAddr 
* ����:	��ѯ��ǰDMAͨ����ǰ����Ŀ�ĵ�ַ
* ����:	ChannelId--ͨ����
* ����:	��ǰDMA����Ŀ�ĵ�ֵַ
*----------------------------------------------------------------------------*/
U32 hyhwDma_GetCurrentDstAddr();

#endif  //_HYHW_DMA_H_

