

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"
/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "hyhwSpi.h"
#include "hyhwGpio.h"
#include "hyhwCkc.h"
//#include "gsmd.h"
//#define spiMast

/* tg3 GPSB ӳ��˵�� 
   TDBOOT SPI �ӿ�, GPSB port7, �õ�DMA, ӳ��channle 0
   TD PCM����, GPSBͨ�� port8, ����DMA, ӳ�䵽channel 3

   GSB PCM����, GPSBͨ�� port6, ����DMA, ӳ�䵽channel 2
*/

/*----------------------------------------------------------------------------
   macro defines:
-----------------------------------------------------------------------------*/

#define SPI_FIFO_DEEP_8BIT		(8)
#define SPI_FIFO_DEEP_16BIT		(8)
#define SPI_FIFO_DEEP_32BIT		(4)

#define SPI_TX_TIMEOUT			(10000000)

// DMA2 ͨ��0, ��channel 6 ��ΪSPI��DMA����
#define SPI_DMA_CHANNEL			6

/*---------------------------------------------------------------------------
* ��  :	hyhwmSpi_DmaEnable 
* ����:	ʹ�� Spi Dma ����
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmSpi_DmaEnable()
{
	return hyhwDma_Enable(SPI_DMA_CHANNEL);	
}

/*--------------------------------------------------------------------------
* ��  :	hyhwmSpi_DmaDisable 
* ����:	disable Spi Dma ����
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwmSpi_DmaDisable()
{
	return hyhwDma_Disable(SPI_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* ����:	hyhwSpi_DmaIntClear
* ����:	���ָ��ͨ����DMA�ж�
* ����:	none
* ����:	HY_OK
*		HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwSpi_DmaIntClear()
{
	return hyhwDma_IntClear(SPI_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* ��  :	hyhwmSpi_DmaIntEnable 
* ����:	enable dma rxtx interrupt
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmSpi_DmaIntEnable()	
{
	return hyhwDma_IntEnable(SPI_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* ��  :	hyhwmSpi_DmaIntDisable 
* ����:	disable dma tx interrupt
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwmSpi_DmaIntDisable()
{
	return hyhwDma_IntDisable(SPI_DMA_CHANNEL);	
}
	
/*---------------------------------------------------------------------------
* ����:	hyhwSpi_DmacheckInterruptState
* ����:	�ж��Ƿ��ж�ʹ��
* ����:	none
* ����:	True �����Ѿ��ж�, FALSE ����δ�ж�
*--------------------------------------------------------------------------*/
BOOL hyhwSpi_DmacheckInterruptState()
{
	return hyhwDma_isDmaInterruptSet(SPI_DMA_CHANNEL);
}

/*---------------------------------------------------------------------------
* ����:	hyhwSpi_DmaGetStatus
* ����:	���ָ��ͨ����DMA��״̬������/busy��
*			�Ȳ鿴��ͨ���Ƿ�����---����
*			���ݴ������-----------����
*			�������ڴ���-----------������
* ����:	none
* ����:	HY_OK
*		HY_ERR_BUSY
*		HY_ERR_BAD_PARAMETER
*--------------------------------------------------------------------------*/
U32 hyhwSpi_DmaGetStatus()
{
	return hyhwDma_GetStatus(SPI_DMA_CHANNEL);
}

/*-----------------------------------------------------------------------------
* ����:	hyhwSpi_clearRXFifoCounter
* ����:	��ս��ջ�����
* ����:	Channel ѡ��ӳ���GPSB ͨ��
* ����:	HY_OK ��ȷ
*		HY_ERR_BAD_PARAMETER ��������
*------------------------------------------------------------------------------*/
U32 hyhwSpi_clearRXFifoCounter(U32 Channel)
{
	tSPI_REGISTER *ptSpiReg;
	
	//������
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER *)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	ptSpiReg->MODE &= SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER;

	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwSpi_clearRXFifoCounter1
* ����:	��ս��ջ�����
* ����:	Channel ѡ��ӳ���GPSB ͨ��
* ����:	HY_OK ��ȷ
*		HY_ERR_BAD_PARAMETER ��������
*------------------------------------------------------------------------------*/
U32 hyhwSpi_clearRXFifoCounter1(U32 Channel)
{
	tSPI_REGISTER *ptSpiReg;
	
	//������
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER *)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	ptSpiReg->MODE |= SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwSpi_init
* ����:	Map GPSB channel to numSPI 
* ����:	Channel ѡ��ӳ���GPSB ͨ��
*		numSPI	ѡ��ӳ���SPI�˿����	
* ����:	HY_OK ��ȷ
*		HY_ERR_BAD_PARAMETER ��������
*		HY_ERR_BYSY	����ͨ����˿�æ
* ʾ����channel 0 Remap as SPI7: hyhwSpi_init(0,7)
*------------------------------------------------------------------------------*/
U32 hyhwSpi_init( U32 Channel, U32 numSPI )
{
	//tSPI_REGISTER *ptSpiReg;
	U32 temp;
	U32 reg;
	
	//������
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	reg = SPI_CONFIG_REGISTER;
	switch(Channel)	//enable the clock
	{
		case 0:
			hyhwCpm_busClkEnable(CKC_GPSB0);
			hyhwCpm_setGpsb0Clk();			
			break;
		case 1:
			hyhwCpm_busClkEnable(CKC_GPSB1);
			hyhwCpm_setGpsb1Clk();			
			break;
		case 2:
			hyhwCpm_busClkEnable(CKC_GPSB2);
			hyhwCpm_setGpsb2Clk();			
			break;
		case 3:
			hyhwCpm_busClkEnable(CKC_GPSB3);
			hyhwCpm_setGpsb3Clk();			
			break;
		default:
			break;	
	}
	
	switch(numSPI) /* ����GPIOΪ�������� */
	{
		case 0:
			hyhwGpio_setGpsb0Function();		
			break;
		case 1:
			hyhwGpio_setGpsb1Function();		
			break;
		case 2:
			hyhwGpio_setGpsb2Function();		
			break;
		case 3:
			hyhwGpio_setGpsb3Function();		
			break;
		case 4:
			hyhwGpio_setGpsb4Function();		
			break;
		case 5:
			hyhwGpio_setGpsb5Function();		
			break;
		case 6:
			hyhwGpio_setGpsb6Function();		
			break;
		case 7:
			hyhwGpio_setGpsb7Function();		
			break;
		case 8:
			hyhwGpio_setGpsb8Function();		
			break;
		case 9:
			hyhwGpio_setGpsb9Function();		
			break;
		case 10:
			hyhwGpio_setGpsb10Function();		
			break;
		default:
			break;	
	}
	
	temp = numSPI << 8*Channel; // ���ö˿ڼĴ�����ͨ����ӳ��
	reg &= ~(0xff << 8*Channel);
	SPI_CONFIG_REGISTER = 	reg | temp;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwSpi_Master()
* ����:	set master spi mode 
* ����:	Channel ѡ��ӳ���GPSB ͨ��	
* ����:	HY_OK ��ȷ
*		HY_ERR_BYSY	����ͨ����˿�æ
* �޸ģ������bitWidth��������������λ�� ʹ��ʱ��Ҫע�� 2009.07.15
		��ѡ8/16/32λ��, �ú궨��ȡ����BIT16�ȱ�ʾ
*------------------------------------------------------------------------------*/
U32 hyhwSpi_Master( U32 Channel, U32 divider, U32 recovTime, 
					U32 holdTIme, U32 SetupTime, U32 bitWidth)
{
	tSPI_REGISTER *ptSpiReg;
	U32 spiRegModeValue;
	
	//������
	if(divider > 255)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER *)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );   

	spiRegModeValue  = ptSpiReg->MODE;
	// master mode
	spiRegModeValue &= ~SPI_MODE_BIT_SLV_SLAVE_MODE;	
		
	// set master relative time, �˴�Ĭ������
	spiRegModeValue &= ~( SPI_MODE_BIT_TRE_TIME_RECOVERY
						|SPI_MODE_BIT_THL_TIME_HOLD
						|SPI_MODE_BIT_TSU_TIME_SETUP
						);								
	
	// SPI �շ����ݼ�����SCK��������,ֻ��master ģʽ����
	spiRegModeValue |= ( SPI_MODE_BIT_PWD_POLARITY_SEND
					   |SPI_MODE_BIT_PRD_POLARITY_RECV // 
					   |SPI_MODE_BIT_PCK_POLARITY_PCK //SCK �Ŀ���Ϊ�ߵ�ƽ
					  ); 								
	
	// clear tx rx fifo counter
	spiRegModeValue |= SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER;
	spiRegModeValue |= SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER;		
	spiRegModeValue &= ~( SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER
						|SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER
					   );											
	
	spiRegModeValue &= ~SPI_MODE_BIT_SD_SHIFT_DIRECTION;				// shif left
	spiRegModeValue &= ~SPI_MODE_BIT_CTF_CONT_TRANSFER_ENABLE;		// CTF single mode
	spiRegModeValue |= SPI_MODE_BIT_EN_OPERATION_ENABLE;				// operation enable
	
	// ����SPIģʽ bit1| bit0,  SPI compatible ģʽ			
	spiRegModeValue &= ~SPI_MODE_BITMASK_MD_OPERATION;						
	spiRegModeValue |= SPI_MODE_BITMASK_MD_OPERATION_SPI;
	
	// ����SPI ͨ��λ��
	spiRegModeValue &= ~SPI_MODE_BITMASK_BWS_BITWIDTH;		
	spiRegModeValue |= ((bitWidth - 1) << SPI_MODE_OFFSITE_BITWIDTH);		
	
	// set ʱ�ӷ�Ƶ�� DIVLDV
	spiRegModeValue &= ~SPI_MODE_BITMASK_CLK_DIVIDER;					
	spiRegModeValue |= divider << SPI_MODE_OFFSITE_CLK_DIVIDER;
	
	ptSpiReg->MODE   = spiRegModeValue;
	
	
	return HY_OK;
}

void enableSpi(U32 Channel,U8 flag)
{
	tSPI_REGISTER *ptSpiReg;
	U32 spiRegModeValue;
	
	ptSpiReg = (tSPI_REGISTER *)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );   

	spiRegModeValue  = ptSpiReg->MODE;
	
	if(flag)
	{
		spiRegModeValue |= SPI_MODE_BIT_EN_OPERATION_ENABLE;
	}
	else
	{
		spiRegModeValue &= ~SPI_MODE_BIT_EN_OPERATION_ENABLE;
	}
	ptSpiReg->MODE = spiRegModeValue;
	
	return ;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwSpi_Slaver()
* ����:	set slave spi mode 
* ����:	Channel ѡ��ӳ���GPSB ͨ��
*		bitWidth----���ݿ��(8/16/32)
*		sendEnable--�Ƿ�������(0---��ֹ���ͣ�1---������)
* ����:	HY_OK ��ȷ
*		HY_ERR_BYSY	����ͨ����˿�æ
* �޸ģ������bitWidth��������������λ�� ʹ��ʱ��Ҫע�� 2009.07.15
		��ѡ8/16/32λ�� �ú궨��ȡ����BIT16�ȱ�ʾ
*------------------------------------------------------------------------------*/
U32 hyhwSpi_Slaver( U32 Channel, U32 bitWidth, int sendEnable)
{
	tSPI_REGISTER *ptSpiReg;
	U32 spiRegModeValue;
	
	//������
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER*)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	spiRegModeValue = ptSpiReg->MODE;
	// slave mode
	spiRegModeValue |= SPI_MODE_BIT_SLV_SLAVE_MODE;

	// operation enable
	spiRegModeValue |= SPI_MODE_BIT_EN_OPERATION_ENABLE;
	
	// ����SPIģʽ bit1| bit0,  SPI compatible ģʽ
	spiRegModeValue &= ~SPI_MODE_BITMASK_MD_OPERATION;
	spiRegModeValue |= SPI_MODE_BITMASK_MD_OPERATION_SPI;
	
	// clear tx rx fifo counter
	spiRegModeValue |= SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER;
	spiRegModeValue |= SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER;		
	spiRegModeValue &= ~( SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER
						|SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER
					   );
	// slave mod SDO output disable
	if (sendEnable == 0)
	{
		spiRegModeValue |= SPI_MODE_BIT_SDO_OUTPUT_DISABLE;
	}
	else
	{
		spiRegModeValue &= ~SPI_MODE_BIT_SDO_OUTPUT_DISABLE;
	}
	// SPI �շ����ݼ�����SCK��������,ֻ��master ģʽ����
	/*
	spiRegModeValue |= ( SPI_MODE_BIT_PWD_POLARITY_SEND
					   |SPI_MODE_BIT_PRD_POLARITY_RECV
					   |SPI_MODE_BIT_PCK_POLARITY_PCK
					  ); 								
	*/ //yanglijing ע�͵�
	//#define TIMING1
	#ifdef TIMING1 
	spiRegModeValue &= ~SPI_MODE_BIT_PCK_POLARITY_PCK; // �Ͻ��ز�������
	
	#else
	spiRegModeValue |= SPI_MODE_BIT_PCK_POLARITY_PCK;  // �����ز�������
	#endif
	// ����SPI ͨ��λ��
	spiRegModeValue &= ~SPI_MODE_BITMASK_BWS_BITWIDTH;
	spiRegModeValue |= ((bitWidth - 1) << SPI_MODE_OFFSITE_BITWIDTH);
	
	#if 0
	ptSpiReg->MODE = spiRegModeValue
					|SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER
					|SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER;
	#endif				
	ptSpiReg->MODE = spiRegModeValue;
	//ptSpiReg->MODE = 0x0007DF04;
	//	ptSpiReg->MODE = 0x00071F0C;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwSpi_Slaver()
* ����:	set slave spi mode 
* ����:	Channel ѡ��ӳ���GPSB ͨ��
*		bitWidth----���ݿ��(8/16/32)
*		sendEnable--�Ƿ�������(0---��ֹ���ͣ�1---������)
* ����:	HY_OK ��ȷ
*		HY_ERR_BYSY	����ͨ����˿�æ
* �޸ģ��������������Ҫ��Ϊ�˵���CHannel 3, PORT 8
		���ó�SSPģʽ���������շ�32bit����, ����channel 0, port 6������������ģʽ����������һ��������
*------------------------------------------------------------------------------*/
U32 hyhwSpi_Slaver_setSSP( U32 Channel, U32 bitWidth, int sendEnable)
{
	tSPI_REGISTER *ptSpiReg;
	U32 spiRegModeValue;
	
	//������
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER*)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	spiRegModeValue = ptSpiReg->MODE;
	// slave mode
	spiRegModeValue |= SPI_MODE_BIT_SLV_SLAVE_MODE;

	// operation enable
	spiRegModeValue |= SPI_MODE_BIT_EN_OPERATION_ENABLE;
	
	// ����SPIģʽ bit1| bit0,  SPI compatible ģʽ
	spiRegModeValue &= ~SPI_MODE_BITMASK_MD_OPERATION;
	spiRegModeValue |= SPI_MODE_BITMASK_MD_OPERATION_SPI;
	
	// clear tx rx fifo counter
	spiRegModeValue |= SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER;
	spiRegModeValue |= SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER;		
	spiRegModeValue &= ~( SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER
						|SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER
					   );
	// slave mod SDO output disable
	if (sendEnable == 0)
	{
		spiRegModeValue |= SPI_MODE_BIT_SDO_OUTPUT_DISABLE;
	}
	else
	{
		spiRegModeValue &= ~SPI_MODE_BIT_SDO_OUTPUT_DISABLE;
	}
	// SPI �շ����ݼ�����SCK��������,ֻ��master ģʽ����
	#if 0
	spiRegModeValue |= ( SPI_MODE_BIT_PWD_POLARITY_SEND
					   |SPI_MODE_BIT_PRD_POLARITY_RECV
					   |SPI_MODE_BIT_PCK_POLARITY_PCK
					  ); 								
	#else
	//#define TIMING1
	#ifdef TIMING1 
	spiRegModeValue &= ~SPI_MODE_BIT_PCK_POLARITY_PCK; // �Ͻ��ز�������
	
	#else
	spiRegModeValue |= SPI_MODE_BIT_PCK_POLARITY_PCK;  // �����ز�������
	#endif//#ifdef TIMING1
	#endif
	// ����SPI ͨ��λ��
	spiRegModeValue &= ~SPI_MODE_BITMASK_BWS_BITWIDTH;
	spiRegModeValue |= ((bitWidth - 1) << SPI_MODE_OFFSITE_BITWIDTH);
	
	// ����SSP ģʽ
	spiRegModeValue |= SPI_MODE_BITMASK_MD_OPERATION_SSP;
	
	#if 0
	ptSpiReg->MODE = spiRegModeValue
					|SPI_MODE_BIT_CRF_CLR_RECVFIFO_COUNTER
					|SPI_MODE_BIT_CWF_CLR_SENDFIFO_COUNTER;
	#endif
	ptSpiReg->MODE = spiRegModeValue;
	//ptSpiReg->MODE = 0x0007DF04;
	//	ptSpiReg->MODE = 0x00071F0C;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwSpi_Wait_TX_End()
* ����:	�ж��Ƿ�ȴ���ʱ
* ����:	Channel ѡ��ӳ���GPSB ͨ��	
* ����:	HY_OK ��ȷ
*		HY_ERR_BAD_PARAMETER	��������
*------------------------------------------------------------------------------*/
U32 hyhwSpi_Wait_TX_End( U32 Channel)
{
	tSPI_REGISTER *ptSpiReg;
	U32 i, status;
	
	//������
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	i		 = 0;
	ptSpiReg = (tSPI_REGISTER*)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );    
	status	 = ptSpiReg->STAT;
	
	while((status & BIT3)==0)
	{
		i++;
		if( i > SPI_TX_TIMEOUT )
		{
		  	return HY_ERR_TIMEOUT;
		}
		status	 = ptSpiReg->STAT;
	}
	
    return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwSpi_Wait_RX_DataNum()
* ����:	�ж��Ƿ�ȴ���ʱ
* ����:	dataNum ָ�����ն��ٸ��ֽڵ�����
*		Channel ѡ��ӳ���GPSB ͨ��	
*		deep	FIFO ���
* ����:	HY_OK ��ȷ
*		HY_ERR_BAD_PARAMETER	��������
* ע��: �ú����������ڲ����з��ַ��Ͳ�����8�������Ժ󣬵�9�����ݲ�������õ�������
*		ԭ����������ݷ��ͺͽ������ʵ�ƥ�䣬�������ɸ������޸ġ�
*------------------------------------------------------------------------------*/
U32 hyhwSpi_Wait_RX_DataNum(U32 dataNum, U32 Channel, U32 deep)
{
	tSPI_REGISTER *ptSpiReg;
	U32 i, status;
	
	//������
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	i		 = 0;
	ptSpiReg = (tSPI_REGISTER*)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel ); 
	status	 = ptSpiReg->STAT;
	
#if 0
	if (dataNum >= deep)
	{
		/*	����Ҫ�ȴ������ݸ������ڵ���FIFO���ʱ��
			ֱ���жϽ���fullλ
		*/
		while((status & BIT4)==0)
		{
			i++;
			if( i > SPI_TX_TIMEOUT )
			{
			  	return HY_ERR_TIMEOUT;
			}  	
		}
	}
#else
	if (dataNum > deep)
	{
		/*	����Ҫ�ȴ������ݸ������ڵ���FIFO���ʱ��
			���ش���
		*/
		return HY_ERROR;
	}
#endif
	else
	{
		/*	�ȴ����չ�ָ�����ݸ�����ֱ����ʱ */
		while((status & (0x0F<<16)) < (dataNum<<16))
		//while(((CPSB_STATUS>>16)&0x0F) < dataNum)
		{
			i++;
			if( i > SPI_TX_TIMEOUT )
			{
			  	return HY_ERR_TIMEOUT;
			} 	
		}
	}
	
    return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwSpi_Wait_RX_End()
* ����:	�ж��Ƿ�ȴ���ʱ
* ����:	Channel ѡ��ӳ���GPSB ͨ��		
* ����:	HY_OK ��ȷ
*		HY_ERR_BAD_PARAMETER	��������
*------------------------------------------------------------------------------*/
U32 hyhwSpi_Wait_RX_End(U32 Channel)
{
	tSPI_REGISTER *ptSpiReg;
	U32 i, status;
	
	//������
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	i		 = 0;
	ptSpiReg = (tSPI_REGISTER*)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	status	 = ptSpiReg->STAT;    
	
	while((status & BIT2)==0)
	{
		i++;
		if( i > SPI_TX_TIMEOUT )
		{
		  	return HY_ERR_TIMEOUT;
		}
		status	 = ptSpiReg->STAT;
	}
    return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwSpi_SendData()
* ����:	��������
* ����:	Channel ѡ��ӳ���GPSB ͨ��	
*		pdata ������
*		len   �����ֽ���
*		depp  fifo���
*		bitDeep λ��ȡֵ8,16,32, �ֱ����8/16/32λ��
* ����:	��ȷ���ؽ��յ�������
*		HY_ERR_BAD_PARAMETER	��������
*------------------------------------------------------------------------------*/
U32 hyhwSpi_SendData(U32 Channel, void* pdata, U32 len, U32 bitDeep)
{
	U32 i,j,k;
	tSPI_REGISTER *ptSpiReg;
	
	//������
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER*)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	
	//�ȴ��������
	hyhwSpi_Wait_TX_End(Channel);

	j=0;
	
	//8 λ��ȣ����ֽ��շ�
	if(bitDeep == 8)		
	{
		U8 *pU8SpiData = (U8 *)pdata;
		//enableSpi(0, 0);
		for (i=0; i<(len/8); i++)
		{
			for (k=0; k<8; k++)
			{
				ptSpiReg->DATAPORT = pU8SpiData[j++];
			}
			hyhwSpi_Wait_TX_End(Channel);
			//enableSpi(0, 1);
		}
		
		for (i=0; i<(len%8); i++)
		{
			ptSpiReg->DATAPORT = pU8SpiData[j++];
		}
		hyhwSpi_Wait_TX_End(Channel);
	}
	// 16 λ��ȣ��������շ�
	else if(bitDeep == 16)	
	{
		U16 *pU16SpiData = (U16 *)pdata;
	
		for (i=0; i<(len/8); i++)
		{
			for (k=0; k<8; k++)
			{
				ptSpiReg->DATAPORT = pU16SpiData[j++];
			}
			hyhwSpi_Wait_TX_End(Channel);
		}
	
		for (i=0; i<(len%8); i++)
		{
			ptSpiReg->DATAPORT = pU16SpiData[j++];
		}
		hyhwSpi_Wait_TX_End(Channel);
	}
	// 32 λ��ȣ������շ�
	else if(bitDeep == 32 || bitDeep == 24) 
	{
		
		U32 *pU32SpiData = (U32 *)pdata;
		
		for (i=0; i<(len/4); i++)
		{
			for (k=0; k<4; k++)
			{
				ptSpiReg->DATAPORT = pU32SpiData[j++];
			}
			hyhwSpi_Wait_TX_End(Channel);
		}
		
		for (i=0; i<(len%4); i++)
		{
			ptSpiReg->DATAPORT = pU32SpiData[j++];
		}
		hyhwSpi_Wait_TX_End(Channel);
	}
	
	return HY_OK;
}


/*-----------------------------------------------------------------------------
* ����:	hyhwSpi_RecvData()
* ����:	set �������� 
* ����:	Channel ѡ��ӳ���GPSB ͨ��	
*		pdata ������
*		len   �����ֽ���
*		bitDeep λ��ȡֵ8,16,32, �ֱ����8/16/32λ��
* ����:	��ȷ���ؽ��յ�������
*		HY_ERR_BAD_PARAMETER	��������

*------------------------------------------------------------------------------*/
U32 hyhwSpi_RecvData(U32 Channel, void* pdata, U32 len, U32 bitDeep)
{
	U32 i,j,k,recvUnit;
	tSPI_REGISTER *ptSpiReg;
	
	//������
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER*)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	//�ȴ��������
	hyhwSpi_Wait_TX_End(Channel);

	j=0;
 
	if(bitDeep == 8)	//λ��Ϊ8
	{	
		U8 *pU8SpiData = (U8 *)pdata;
		recvUnit = 8;

		//���fifo������
		for(i=0; i<8; i++)
		{
			pU8SpiData[i] = ptSpiReg->DATAPORT;
		}
		
		for (i=0; i<(len/8); i++)							
		{
			for (k=0; k<8; k++)
			{
			ptSpiReg->DATAPORT = 0xFF;
			}
			
			hyhwSpi_Wait_TX_End(Channel);
			hyhwSpi_Wait_RX_DataNum(recvUnit , Channel, 8);		//�ȴ�������(len/8)���ֽ�����
			
			for (k=0; k<8; k++)
			{
				pU8SpiData[j++] = ptSpiReg->DATAPORT;
			}
				
		}
		for (i=0; i<(len%8); i++)
		{
			ptSpiReg->DATAPORT = 0xFF;
		}
		
		recvUnit = len%8;
		hyhwSpi_Wait_TX_End(Channel);
		hyhwSpi_Wait_RX_DataNum(recvUnit, Channel, 8);			//�ȴ�������(len%8)���ֽ�����
		
		for (i=0; i<(len%8); i++)
		{
			pU8SpiData[j++] = ptSpiReg->DATAPORT;
		}
	}
	else if(bitDeep == 16) //λ��Ϊ16
	{
		U16 *pU16SpiData = (U16 *)pdata;
		recvUnit 		 = 8;
			
		//���fifo������
		for(i=0; i<8; i++)
		{
			pU16SpiData[i] = ptSpiReg->DATAPORT;
		}
			
		for (i=0; i<(len/8); i++)
		{
			for (k=0; k<8; k++)
			{
				ptSpiReg->DATAPORT = 0xFFFF;
			}
			
			hyhwSpi_Wait_TX_End(Channel);
			hyhwSpi_Wait_RX_DataNum(recvUnit, Channel, 8);		//�ȴ�������(len/8)���ֽ�����
			
			for (k=0; k<8; k++)
			{
				pU16SpiData[j++] = ptSpiReg->DATAPORT;
			}
				
		}
		for (i=0; i<(len%8); i++)
		{
			ptSpiReg->DATAPORT = 0xFFFF;
		}
		
		recvUnit	= len%8;
		hyhwSpi_Wait_TX_End(Channel);
		hyhwSpi_Wait_RX_DataNum(recvUnit, Channel, 8);			//�ȴ�������(len%8)���ֽ�����				
		
		for (i=0; i<(len%8); i++)
		{
			pU16SpiData[j++] = ptSpiReg->DATAPORT;
		}
	}		
	else if(bitDeep == 32 || bitDeep == 24) //32 bit width tx
	{
		
		U32 *pU32SpiData = (U32 *)pdata;
		recvUnit		 = len/4;							
		
		//���fifo������
		for(i=0; i<4; i++)
		{
			pU32SpiData[i] = ptSpiReg->DATAPORT;
		}
		
		for (i=0; i<(len/4); i++)
		{
			for (k=0; k<4; k++)
			{
				ptSpiReg->DATAPORT = 0xFFFFFFFF;
			}
			
			hyhwSpi_Wait_TX_End(Channel);
			hyhwSpi_Wait_RX_DataNum(recvUnit, Channel, 4);		//�ȴ�������(len/4)���ֽ�����
			
			for (k=0; k<4; k++)
			{
				pU32SpiData[j++] = ptSpiReg->DATAPORT;
			}
			
		}
		for (i=0; i<(len%4); i++)
		{
			ptSpiReg->DATAPORT = 0xFFFFFFFF;
		}
		
		recvUnit	= len%4;
		hyhwSpi_Wait_TX_End(Channel);
		hyhwSpi_Wait_RX_DataNum(recvUnit, Channel, 4);			//�ȴ�������(len%4)���ֽ�����
		
		for (i=0; i<(len%4); i++)
		{
			pU32SpiData[j++]  = ptSpiReg->DATAPORT;
		}
	}
	
	i = 0;
	j = 0;
	k = 0;
	return HY_OK;
}

#if 0
/*-----------------------------------------------------------------------------
* ����:	hyhwSpi_ReturnState()
* ����:	set slave spi mode 
* ����:	Channel ѡ��ӳ���GPSB ͨ��	
* ����:	�������ڵĶ�д״̬
*------------------------------------------------------------------------------*/
U32 hyhwSpi_ReturnState(U32 Channel)
{
	U32 status;
	tSPI_REGISTER *ptSpiReg;
	
	//������
	if(Channel > 3)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	ptSpiReg = (tSPI_REGISTER *)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*Channel );
	status	 = ptSpiReg->STAT;
	
	if(status & BIT2)
	{
		return ReadNEmpty; 			//����������Ϊ��
	}
	else if(status & BIT3)
	{
		return WriteEmpty; 			//д������Ϊ�գ����Լ�����������
	}
	else
	{
		return NoneState; 			//������Ч״̬
	}
}
#endif


/*-----------------------------------------------------------------------------
* ����:	hyhwSpi_CheckInterrupt
* ����:	�ж��Ƿ�����ж�
* ����:	channel Id
* ����: none
* ��ע: ����spi�˿�Ӧ�õ�����Ҫ��GPSB��ͨ����, tg3 ���õ���ͨ���ŷֱ���
		: TD_BOOT_SPI_CTRL/ GSM_PCM_SPI_CTRL/TD_PCM_SPI_CTRL , 
* ʾ��: hyhwSpi_CheckInterrupt();	
	
*----------------------------------------------------------------------------*/
BOOL hyhwSpi_CheckInterrupt(U8 channelId)
{
	U32 tempVal;
	
	tempVal = SPI_IRQ_STATUS;
	if(tempVal & (1<<(2*channelId))) 
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*-----------------------------------------------------------------------------
* ����:	hyhwSpi_IntEnable
* ����:	ʹ���ж�
* ����:	channel Id
		IntSource ��ʾʹ�ܵ��ж�Դ, ע��: ������ж�Դ���ÿ����Ƕ���ж�Դ�Ļ�ֵ
		IntSource ��BIT0-BIT8�Ļ�ֵ
* ����: none
* ��ע: ����spi�˿�Ӧ�õ�����Ҫ��GPSB��ͨ����, tg3 ���õ���ͨ���ŷֱ���
		: TD_BOOT_SPI_CTRL/ GSM_PCM_SPI_CTRL/TD_PCM_SPI_CTRL, 
		����ļĴ���ֵֻ��������9���ж�, ������ǰ���DMA��ص����á�
* ʾ��: hyhwSpi_CheckInterrupt(GSM_PCM_SPI_CTRL, RXFIFO_FULL);// �򿪽����ж�
*----------------------------------------------------------------------------*/
void hyhwSpi_IntEnable(U8 channelId, eSPI_INT_SRC IntSource)
{
	tSPI_REGISTER *ptSpiReg;
	
	ptSpiReg        = (tSPI_REGISTER *)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*channelId );
	ptSpiReg->INTEN |= IntSource;
}

void hyhwSpi_IntDisable(U8 channelId, eSPI_INT_SRC IntSource)
{
	tSPI_REGISTER *ptSpiReg;
	
	ptSpiReg        = (tSPI_REGISTER *)(TCC7901_GPSB_BASEADDR + TCC7901_GPSB_OFFSETADDR*channelId );
	ptSpiReg->INTEN &= ~IntSource;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwSpi_IntClear
* ����:	ʹ���ж�
* ����:	channel Id
* ����: none
* ��ע: ����spi�˿�Ӧ�õ�����Ҫ��GPSB��ͨ����, tg3 ���õ���ͨ���ŷֱ���
		: TD_BOOT_SPI_CTRL/ GSM_PCM_SPI_CTRL/TD_PCM_SPI_CTRL , 
* ʾ��: hyhwSpi_CheckInterrupt();	
	
*----------------------------------------------------------------------------*/
void hyhwSpi_IntClear(U8 channelID)
{
	// ���Ĵ����Ϳ�������ж�
	// �ú�������Ϊ��, ��ʼ����ͨ������һ��ȫ�ֱ���������״̬�Ĵ�����ֵ�Ϳ�������ж��ˡ�
}

#include "hyhwIntCtrl.h"
#if 0

U8 pData1[32];
void spi_rxInt_Isr(void)
{
	// ����ж�
	
	// �жϴ���
	U32 spiStatus , i, num;
	
	spiStatus = TCC8300_SPI0_SPSTS_REGISTER(0);
	num = ((spiStatus>>16) & 0x0f);
	//while( TCC8300_SPI0_SPSTS_REGISTER(pcmSourceSpiChn) & BIT4 ) //RX FIFO CNT IS 12
	{
 		for( i = 0; i < num; i ++ )
		{
			//PhoneTrace(0, "pcm data");

			pData1[i] = (SPI_PCM_DATA_SDI(0));//��spi�ж�ȡpcm����(��16bit��Ч)
		}
	}
	
}
#endif



U32 hyhwSpi_Send(U32 Channel, char* pdata, U32 len, U32 bitDeep)
{
	U32 i;
	for(i=0; i<len; i++)
	{
		while(0 != hyhwGpio_Read(PORT_E, BIT6));
		hyhwSpi_SendData(0, &pdata[i], 1, bitDeep);	
	
	}
	return 0;
}

/*mode 	����д��primary����secondary, 
  rwflag �����Ƕ�����д, 0 ��ʾ���յ����ݣ���, 1��ʾ�������ݣ�д
  rs �Ĵ���ѡ��, 1��ʾдָ��, 0��ʾ�����Ĵ�����д��״̬�Ĵ�����
����֮ǰע��Ҫ��Ƭѡ��һ�£��������ٷ���ԭ��״̬ */
void hyhwSpi_LcdSend(unsigned short Data, unsigned int mode
	,unsigned int rs, unsigned int rwflag)
{
	unsigned int data;
	if(mode == 0)
	{
		// primary register
		/* ��Ҫȷ��RS��RW��ֵ */
		data = Data | rs << 17 | rwflag << 16 | 0x16 << 18;
	}else
	{
		// secondary register
		data = Data | rs << 17 | rwflag << 16 | 0x17 << 18;
	}

	hyhwSpi_SendData(0, &data, 1, 24);
}

void hyhwSpi_LcdCmd(unsigned int cmd, unsigned int mode)
{
		// rw = 1, rs = 1
	hyhwSpi_LcdSend(cmd, mode, 1, 1);
}

void hyhwSpi_LcdData(unsigned int data, unsigned int mode)
{
	// rw = 1, rs = 0
	hyhwSpi_LcdSend(data, mode, 0, 1);
}

U32 hyhwSpi_LcdRecv(void)
{
	// can read state register use this function
	unsigned int data;
	hyhwSpi_LcdCmd(0x0, 0);
	hyhwSpi_RecvData(0, &data, 1, 24);
	return data;
}
/*-----------------------------------------------------------------------------
*	�޸ļ�¼ 1 :
2009 07 07 ������
*	���⣺
	1���������ʱ���⣺ �����Ͳ�����8���ֽ�����ʱ�����ܵ���9�����ݲ�Ϊ�գ�
	�Ҳ�����õ������ݣ������������hyhwSpi_Wait_RX_DataNum()�����������������
	����

*	�޸ĵط�:
	ͷ�ļ�����������Դ�ļ����ļ����壬�͸ú�����6������
*------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
*	�޸ļ�¼ 2 :
2009 07 15	������
����:
	1\slave ģʽ���ò�������ͨ��, ��BIT15�ȱ�ʾʱ���岻��ȷ
	2\hyhwSpi_clearRXFifoCounter()hyhwSpi_clearRXFifoCounter1(),�������궨���Ǹ�����ǰ�̶�ͨ��ʱд�ģ�
		Ϊ��ͨ�ã����ｫ�������Ժ�����ʽ����
����:
	����:	hyhwSpi_Master()��hyhwSpi_Slave()��
	�����bitWidth��������������λ�� ʹ��ʱ��Ҫע��
		��ѡ8/16/32λ��, �ú궨��ȡ����BIT16�ȱ�ʾ
�޸�λ��:
	1\ͷ�ļ���SPI_MODE�Ĵ����е�λ�ú���ж���
	2\ͷ�ļ��к�hyhwSpi_Master()��hyhwSpi_Slave()	��صĶ����������
		������������ؼĴ�������ֵ�����˲����޸ģ�
	3\ͷ�ļ���������hyhwSpi_clearRXFifoCounter()hyhwSpi_clearRXFifoCounter1()����������
		Դ�ļ���ע�͵��������꣬������ͬ���ĺ�����������˲�������Ҫ��ʹ��ʱע��
	
*------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
*	�޸ļ�¼ 3 :
2009 08 05	������
����:
	ͨ��SPI������������ʱ, �����Ҫ3s��ʱ��, ���ʱ����DMA����һ�ǿ�������ٶ�, ������
	CPU��������������, ���õȴ���������. ���ո�����Ҫ��������
����:
	��hyhwDma�ļ���ģ�����������������hyhwDma_SpiDmaConfig �� hyhwDma_SpiDmaEnable
	, һ��������SPI��DMA���õ�, һ������ʹ��DMA����, ������˼����궨��, ��hyhwDma.h�ļ�
	
	
�޸�λ��:
	1\hyhwDma.h ͷ�ļ��е���ӱ��20090805, ��Ҫ�Ǻ궨��, �� ��������
		hyhwDma_SpiDmaConfig �� hyhwDma_SpiDmaEnable
	2\hyhwDma.c hyhwDma_SpiDmaConfig �� hyhwDma_SpiDmaEnable ��������		
	3\hyhwSpi.h �е����º�����������Դ�ļ��к�������
	hyhwmSpi_DmaEnable,hyhwmSpi_DmaDisable,hyhwSpi_DmaIntClear,hyhwmSpi_DmaIntEnable,
	hyhwmSpi_DmaIntDisable,hyhwSpi_DmacheckInterruptState,hyhwSpi_DmaGetStatus;
	
����, �� Gao �����, �������г��ּĴ�����ֵ������ʱ, �����һ���м�������ı�Ч��
,hyhwSpi_Slaverģʽ����˲���sendEnable, ����Ҳ�������޸ġ�	
------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
*	�޸ļ�¼ 4 :
20090820
	���ո�����Ҫ�����SPI8 ��TDģ��Ĳ��ԣ��ڲ��Թ����з�����Щ���������⣬����
	��ǰ�漯�ɹ���ʱ��ӵĺ��������������ע��
	:
	hyhwSpi_init, hyhwSpi_Slaver�������޸�
	������˺���:  hyhwSpi_Slaver_setSSP/hyhwSpi_IntClear/hyhwSpi_IntEnable/
	hyhwSpi_CheckInterrupt
------------------------------------------------------------------------------*/