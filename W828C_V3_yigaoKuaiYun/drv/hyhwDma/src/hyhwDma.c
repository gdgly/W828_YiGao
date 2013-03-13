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
#include "hyErrors.h"

/*---------------------------------------------------------------------------
   Project include files:
   --------------------------------------------------------------------------*/
#include "hyhwChip.h"
#include "hyhwDma.h"
#include "hyhwCkc.h"

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_Init
* ����:	��ʼ��DMA 1~8,dma0Ϊlcd_CPU DMA�����ӳ�ʼ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwDma_Init(void)
{
	U8 i;
	tDMA_REGISTER *ptDmaReg;
	
	hyhwCpm_busClkEnable( CKC_DMA ); 											// enable the dam clock
		
	for(i=1;i<DMA_TOTAL_CHANNEL_NUM;i++)
	{
		if(i < DMA_CHANNEL_NUM_PER_DMA) 										// channel 1-2
		{
			ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*i );   
		}	
		else if( i < 2*DMA_CHANNEL_NUM_PER_DMA) 								// channel 3-5							
		{
			ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(i-DMA_CHANNEL_NUM_PER_DMA) ); 
		}
		else																	// channel 6-8																	
		{
			ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(i-2*DMA_CHANNEL_NUM_PER_DMA) ); 
		}
		
		ptDmaReg->SOURCE_PARAMETER 		= DMA_PORT_TYPE_WORD; 					// DMAʱsource ����Ϊ1����
		ptDmaReg->DESTINATION_PARAMETER = DMA_PORT_TYPE_WORD; 					// DMAʱDestination ����Ϊ1����
		ptDmaReg->CHANNEL_CONTROL  		=(  CHCTRL_BIT_TRANSFER_TYPE_SOFTWARE 	// SW transfer
										  | CHCTRL_BIT_BURST_SIZE_1				// 1 burst transfer include 1 read and 1 write circles
										  | CHCTRL_BIT_WORD_SIZE1_32BIT			// WSIZE, Each cycle read or write 32bit data
										  | CHCTRL_BIT_DMA_DONE_FLAG			// FLAG , Represents that all hop of transfers are fulfilled.
										 );
	}
		
	// channel control register inialize						
	DMA0_CONFIG_REGISTER |= 	CHCONFIG_BIT_CH_PRIORITY_CYCLIC;				// set bit 0 of channel configuration register as fixed priority_mode
	DMA0_CONFIG_REGISTER &=		(~(0x7<<4 ));
	DMA0_CONFIG_REGISTER |=		CHCONFIG_BIT_CH_PRIORITY_012 << 4;				// default priority 0 > 1 > 2
	DMA0_CONFIG_REGISTER &= 	(~(0x7<<8 ));									// set initial state : not swap the data
	
	DMA1_CONFIG_REGISTER |=		CHCONFIG_BIT_CH_PRIORITY_CYCLIC;
	DMA1_CONFIG_REGISTER &=		(~(0x7<<4 ));
	DMA1_CONFIG_REGISTER |=		CHCONFIG_BIT_CH_PRIORITY_012 << 4;				// default priority 0 > 1 > 2
	DMA1_CONFIG_REGISTER &= 	(~(0x7<<8 ));									// set initial state : not swap the data
	
	DMA2_CONFIG_REGISTER |= 	CHCONFIG_BIT_CH_PRIORITY_CYCLIC;
	DMA2_CONFIG_REGISTER &=		(~(0x7<<4 ));
	DMA2_CONFIG_REGISTER |=		CHCONFIG_BIT_CH_PRIORITY_012 << 4;				// default priority 0 > 1 > 2
	DMA2_CONFIG_REGISTER &= 	(~(0x7<<8 ));									// set initial state : not swap the data
	
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_Init
* ����:	�ر�DMA��Դ���Խ�ʡ�õ�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
Void hyhwDma_DeInit(Void)
{
	//�ر�DMAʱ��
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_Config
* ����:	����DMA
* ����:	sa------����Դ��ַ
*		da------����Ŀ�ĵ�ַ
*		size--��Ҫ�����hop��( 32 bit word )
* ����:	���õ�ͨ���ţ��������ȼ��������ȿ��õ�ͨ��
*		HY_ERR_BUSY:�޿��õ�DMA
*----------------------------------------------------------------------------*/
U32 hyhwDma_Config(U32 sa, U32 da, U32 size)
{

	U8 ChannelId;
	tDMA_REGISTER *ptDmaReg;
	
	/* channel 0 �� LCD ռ��*/
	for( ChannelId = 1; ChannelId < DMA_TOTAL_CHANNEL_NUM; ChannelId ++ )
	{
		if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
		{
			ptDmaReg  = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
		}	
		else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
		{
			 ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );     
		}
		else
		{
			ptDmaReg  = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );     
		}
		
		if(( ptDmaReg->HOP_COUNT >> 16 ) == 0 )															/* ֻ���õ�ǰ�ļ�����ֵ�жϣ��������жϱ�־λ�ж�*/
		{ 
			ptDmaReg->SOURCE_START_ADDR 	 = sa;
			ptDmaReg->DESTINATION_START_ADDR = da;
			ptDmaReg->HOP_COUNT				 = size;
			ptDmaReg->CHANNEL_CONTROL 		|= ( CHCTRL_BIT_DMA_DONE_FLAG|CHCTRL_BIT_DMA_INT_ENABLE ); /* ���ж�,ʹ���ж� */
			ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;	
			return ChannelId;
		}
	}	
	return HY_ERR_BUSY;	
}

/*-----------------------------------------------------------------------------
* ����:	lcdCpuDmaStop
* ����:	ֹͣlcdCpuDma
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
Void lcdCpuDmaStop(Void)
{
	
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_Enable 
* ����:	����DMA
* ����:	ChannelId--ͨ����
*		
* ����:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note:  ChannelId�ķ�Χ0~8��0��2ʹ��DMA0��3��5ʹ��DMA1, 6~~8ʹ��DMA2����Ϊ�������
*----------------------------------------------------------------------------*/
U32 hyhwDma_Enable(U8 ChannelId)
{
	tDMA_REGISTER *ptDmaReg;
	
	//�������
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId ); 
	}	
	else if (ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );   
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
//	ptDmaReg->CHANNEL_CONTROL |= ( CHCTRL_BIT_DMA_DONE_FLAG|CHCTRL_BIT_DMA_INT_ENABLE ); /* ���ж�,ʹ���ж� */
	ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_START;
	
   	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_InterruptSet 
* ����:	ֹͣDMA
* ����:	ChannelId--ͨ����
*	
* ����:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note:  ChannelId�ķ�Χ0~8��0��2ʹ��DMA0��3��5ʹ��DMA1, 6~~8ʹ��DMA2����Ϊ�������
*----------------------------------------------------------------------------*/
U32 hyhwDma_Disable(U8 ChannelId)
{
	tDMA_REGISTER *ptDmaReg;
	
	//�������
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if (ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	ptDmaReg->HOP_COUNT        = 0;
	ptDmaReg->CHANNEL_CONTROL &= ~CHCTRL_BIT_DMA_START;
	
   	return HY_OK;
	
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_Pause 
* ����:	��ͣDMA���䣬���ٴο�ʼ����ʱ����Ҫ��������DMA������hyhw_restart��������
* ����:	ChannelId--ͨ����
*	
* ����:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note:  ChannelId�ķ�Χ0~8��0��2ʹ��DMA0��3��5ʹ��DMA1, 6~~8ʹ��DMA2����Ϊ�������
*----------------------------------------------------------------------------*/
U32 hyhwDma_Pause(U8 ChannelId)
{
	tDMA_REGISTER *ptDmaReg;
	
	//�������
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if (ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
//	ptDmaReg->HOP_COUNT        = 0;
	ptDmaReg->CHANNEL_CONTROL &= ~CHCTRL_BIT_DMA_START;
	
   	return HY_OK;
	
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_restart 
* ����:	��ͣDMA���䣬���ٴο�ʼ����ʱ����Ҫ��������DMA��
* ����:	ChannelId--ͨ����
*	
* ����:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note:  ChannelId�ķ�Χ0~8��0��2ʹ��DMA0��3��5ʹ��DMA1, 6~~8ʹ��DMA2����Ϊ�������
*----------------------------------------------------------------------------*/
U32 hyhwDma_restart(U8 ChannelId)
{
	tDMA_REGISTER *ptDmaReg;
	
	//�������
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if (ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
//	ptDmaReg->HOP_COUNT        = 0;
	ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_START;
	
   	return HY_OK;
	
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_isPause 
* ����:	�ж�DMA�Ƿ�����ͣ״̬
* ����:	ChannelId--ͨ����
*	
* ����:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note:  ChannelId�ķ�Χ0~8��0��2ʹ��DMA0��3��5ʹ��DMA1, 6~~8ʹ��DMA2����Ϊ�������
*----------------------------------------------------------------------------*/
BOOL hyhwDma_isPause(U8 ChannelId)
{
	U32 temp;
	tDMA_REGISTER *ptDmaReg;
	
	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if (ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	temp = ptDmaReg->CHANNEL_CONTROL;
	if(temp & CHCTRL_BIT_DMA_START)
	{
		return FALSE;
	}
	
	if(ptDmaReg->HOP_COUNT == 0)
	{
		return FALSE;
	}
	
   	return TRUE;
	
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_IntClear
* ����:	���ָ��ͨ����DMA�ж�
* ����:	ChannelId--ͨ����
* ����:	HY_OK
*		HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDma_IntClear(U8 ChannelId)
{
	tDMA_REGISTER	*ptDmaReg;
	
	//����ͨ��ѡ����ƼĴ���
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
//	ptDmaReg->CHANNEL_CONTROL &= ~CHCTRL_BIT_DMA_DONE_FLAG;
	ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_DONE_FLAG;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_IntEnable
* ����:	ʹ��ָ��ͨ����DMA�ж�
* ����:	ChannelId--ͨ����
* ����:	HY_OK
*		HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDma_IntEnable(U8 ChannelId)
{
	tDMA_REGISTER	*ptDmaReg;
	
	//����ͨ��ѡ����ƼĴ���
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_INT_ENABLE;
	
	return HY_OK;
}



/*-----------------------------------------------------------------------------
* ����:	hyhwDma_IntDisable
* ����:	��ָֹ��ͨ����DMA�ж�
* ����:	ChannelId--ͨ����
* ����:	HY_OK
*		HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDma_IntDisable(U8 ChannelId)
{
	tDMA_REGISTER	*ptDmaReg;
	
	//����ͨ��ѡ����ƼĴ���
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	ptDmaReg->CHANNEL_CONTROL &= CHCTRL_BIT_DMA_INT_DISABLE;
	
	return HY_OK;
}


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
U32 hyhwDma_GetStatus(U8 ChannelId)
{
	tDMA_REGISTER *ptDmaReg;
	
	//�����Ϸ��Լ��
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	if( ( ptDmaReg->HOP_COUNT >> 16) == 0 )
	{ 
		ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_DONE_FLAG;
		return HY_OK;
	}
	else
	{
		return HY_ERR_BUSY;	
	}
}


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
						 	)
{
	tDMA_REGISTER *ptDmaReg;
	
	//�����Ϸ��Լ��
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM ) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	
	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	ptDmaReg->SOURCE_PARAMETER 		= (SourceAddrMask<<4) & (~0xff) + (U8)(SourceIncreaseStep);
	ptDmaReg->DESTINATION_PARAMETER = (DestinationAddrMask<<4) & (~0xff)  + (U8)(DestinationIncreaseStep);
	
	return HY_OK;
}


/*-----------------------------------------------------------------------------
* ����:	hyhwDma_InterruptSet 
* ����:	����dma�ж�ʹ��
* ����:	ChannelId--ͨ����
*		Enable_en IntCtrl--EnableOff �жϽ�ֹ
*						 --EnableOn  �ж�����
* ����:	HY_OK
*		HY_ERR_BAD_PARAMETER
*note:  ChannelId�ķ�Χ0~5��0��2ʹ��DMA0��3��5ʹ��DMA1,����Ϊ�������
*----------------------------------------------------------------------------*/
/*U32 hyhwDma_InterruptSet(U8 ChannelId, eENABLE IntCtrl)
{
	tDMA_REGISTER	*ptDmaReg;
	if(	 ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	if(ChannelId >= DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );   
	}	
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );   
	}
	if(IntCtrl == EnableOff)
	{
		ptDmaReg->CHANNEL_CONTROL &= ~CHCTRL_BIT_DMA_INT_ENABLE;
	}
	else
	{
		ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_INT_ENABLE;
	}
	return HY_OK;
}
*/
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
						)
{
	tDMA_REGISTER *ptDmaReg;
	
	//�����Ϸ��Լ��
	if(ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	if(ReadWrite == HardwareRead)
	{
		ptDmaReg->CHANNEL_CONTROL 	&= ~CHCTRL_BIT_HW_REQUEST_DIRECTION_WRITE;
	}
	else
	{
		ptDmaReg->CHANNEL_CONTROL 	|= CHCTRL_BIT_HW_REQUEST_DIRECTION_WRITE;
	}
	
	ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_HW_REQUEST_SYNC_ENABLE;
	
	ptDmaReg->CHANNEL_CONTROL 		&= ~(0x3 << 8);
	ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_TRANSFER_TYPE_HARDWARE;
	
	ptDmaReg->EXTERNAL_DMA_REQUEST 	 = 0; 									//clear
	ptDmaReg->EXTERNAL_DMA_REQUEST  |= HardwareType;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_uart0TxConfig
* ����:	����DMA uart0 Tx DMA ʹ�ܴ���
* ����:	ChannelId------ѡ�����ڴ����DMAͨ��
*		sa------����Դ��ַ��ַ
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart0TxConfig(U32 ChannelId, U32 sa)
{
	tDMA_REGISTER *ptDmaReg;
	
	// ������� ȷ��ͨ�����ڿ����ҿ���״̬
	if(HY_OK != hyhwDma_GetStatus(ChannelId))
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	// ����ͨ��ѡ����ƼĴ���
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	
	ptDmaReg->SOURCE_START_ADDR 		= sa;
	ptDmaReg->SOURCE_PARAMETER 			= 1;								// address added step is 1, every time when operation is finished ,address += 1;
	ptDmaReg->DESTINATION_START_ADDR 	= 0xf0055000; 						// UART0 THR ���ͱ��ּĴ���
	ptDmaReg->DESTINATION_PARAMETER 	= 0;
	ptDmaReg->HOP_COUNT 				= 16;								// or it may be 15 better
	ptDmaReg->REPEAT_CONTROL 			= (BIT31|BIT30); 					/* DMA interrupt is occurred when the last DMA Repeated DMA operation */
	ptDmaReg->EXTERNAL_DMA_REQUEST 		= BIT26;				 			/* UART channel 0 transmitter DMA requeset */
	ptDmaReg->CHANNEL_CONTROL 		   &= ~(BIT9|BIT8|BIT5|BIT4); 			/* every cycle read /write 8 bit data */
	ptDmaReg->CHANNEL_CONTROL 		   |= (BIT8|BIT1);						/* enable HW transfer, and the next request transfer mode is the same as the last */
	ptDmaReg->CHANNEL_CONTROL 		   |= (BIT0); 							/* UART��interrupt flag generate DMA requeset */

	ptDmaReg->CHANNEL_CONTROL 		   |= ( CHCTRL_BIT_DMA_DONE_FLAG|CHCTRL_BIT_DMA_INT_ENABLE ); /* ���ж�,ʹ���ж� */
	ptDmaReg->CHANNEL_CONTROL 		   |= CHCTRL_BIT_DMA_START;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_uart0RxConfig
* ����:	����DMA uart0 Rx DMA ʹ�ܴ���
* ����:	ChannelId------ѡ�����ڴ����DMAͨ��
*		sa------����Դ��ַ��ַ
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart0RxConfig(U32 ChannelId, U32 sa)
{
	tDMA_REGISTER *ptDmaReg;
	
	//������� ȷ��ͨ�����ڿ����ҿ���״̬
	if(HY_OK != hyhwDma_GetStatus(ChannelId))
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	//����ͨ��ѡ����ƼĴ���
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	
	ptDmaReg->SOURCE_START_ADDR 	 = 0xf0055000;	 						// UART0 RBR ���ջ������Ĵ���;
	ptDmaReg->SOURCE_PARAMETER 		 = 0;									// address added step is 0, every time when operation is finished ,address += 0;
	ptDmaReg->DESTINATION_START_ADDR = sa;
	ptDmaReg->DESTINATION_PARAMETER  = 1;
	ptDmaReg->HOP_COUNT 			 = 16;									// or it may be 15 better
	ptDmaReg->REPEAT_CONTROL 		 = (BIT31|BIT30); 						/* DMA interrupt is occurred when the last DMA Repeated DMA operation */
	ptDmaReg->EXTERNAL_DMA_REQUEST   = BIT27;				 				/* UART channel 0 receiver DMA requeset */
	ptDmaReg->CHANNEL_CONTROL 		&= ~(BIT9|BIT8|BIT5|BIT4); 				/* every cycle read /write 8 bit data */
	ptDmaReg->CHANNEL_CONTROL 		|= (BIT8|BIT1);							/* enable HW transfer, and the next request transfer mode is the same as the last */
	ptDmaReg->CHANNEL_CONTROL 		|= (BIT0); 								/* UART��interrupt flag generate DMA requeset */

	ptDmaReg->CHANNEL_CONTROL 		|= ( CHCTRL_BIT_DMA_DONE_FLAG|CHCTRL_BIT_DMA_INT_ENABLE ); /* ���ж�,ʹ���ж� */
	ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_uart2TxConfig
* ����:	����DMA uart2 Tx DMA ʹ�ܴ���
* ����:	ChannelId------ѡ�����ڴ����DMAͨ��
*		sa------����Դ��ַ��ַ
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart2TxConfig(U32 ChannelId, U32 sa)
{
	tDMA_REGISTER *ptDmaReg;
	
	//������� ȷ��ͨ�����ڿ����ҿ���״̬
	if(HY_OK != hyhwDma_GetStatus(ChannelId))
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	//����ͨ��ѡ����ƼĴ���
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	
	ptDmaReg->SOURCE_START_ADDR 	 = sa;
	ptDmaReg->SOURCE_PARAMETER 		 = 1;									 // address added step is 1, every time when operation is finished ,address += 1;
	ptDmaReg->DESTINATION_START_ADDR = 0xf0055200; 	 					 	 // UART2 THR ���ͱ��ּĴ���
	ptDmaReg->DESTINATION_PARAMETER  = 0;
	ptDmaReg->HOP_COUNT 			 = 16;									 // or it may be 15 better
	ptDmaReg->REPEAT_CONTROL 	     = (BIT31|BIT30); 	 				 	 /* DMA interrupt is occurred when the last DMA Repeated DMA operation */
	ptDmaReg->EXTERNAL_DMA_REQUEST   = BIT8;				 				 /* UART channel 2 transmitter DMA requeset */
	ptDmaReg->CHANNEL_CONTROL 		&= ~(BIT9|BIT8|BIT5|BIT4);				 /* every cycle read /write 8 bit data */
	ptDmaReg->CHANNEL_CONTROL 		|= (BIT8|BIT1);							 /* enable HW transfer, and the next request transfer mode is the same as the last */
	ptDmaReg->CHANNEL_CONTROL 		|= (BIT0); 								 /* UART��interrupt flag generate DMA requeset */

	ptDmaReg->CHANNEL_CONTROL 		|= ( CHCTRL_BIT_DMA_DONE_FLAG|CHCTRL_BIT_DMA_INT_ENABLE ); /* ���ж�,ʹ���ж� */
	ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_uart2RxConfig
* ����:	����DMA uart2 Rx DMA ʹ�ܴ���
* ����:	ChannelId------ѡ�����ڴ����DMAͨ��
*		sa------����Դ��ַ��ַ
*----------------------------------------------------------------------------*/
U32 hyhwDma_uart2RxConfig(U32 ChannelId, U32 sa)
{
	tDMA_REGISTER *ptDmaReg;
	
	//������� ȷ��ͨ�����ڿ����ҿ���״̬
	if(HY_OK != hyhwDma_GetStatus(ChannelId))
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	//����ͨ��ѡ����ƼĴ���
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	
	ptDmaReg->SOURCE_START_ADDR 	 = 0xf0055200; 							// UART2 RBR ���ջ������Ĵ���
	ptDmaReg->SOURCE_PARAMETER 		 = 0;									// address added step is 0, every time when operation is finished ,address += 0;
	ptDmaReg->DESTINATION_START_ADDR = sa;
	ptDmaReg->DESTINATION_PARAMETER  = 1;
	ptDmaReg->HOP_COUNT 			 = 16;									// or it may be 15 better
	ptDmaReg->REPEAT_CONTROL 		 = (BIT31|BIT30); 						/* DMA interrupt is occurred when the last DMA Repeated DMA operation */
	ptDmaReg->EXTERNAL_DMA_REQUEST   = BIT9;				 				/* UART channel 2 receiver DMA requeset */
	ptDmaReg->CHANNEL_CONTROL 		&= ~(BIT9|BIT8|BIT5|BIT4); 				/* every cycle read /write 8 bit data */
	ptDmaReg->CHANNEL_CONTROL 		|= (BIT8|BIT1);							/* enable HW transfer, and the next request transfer mode is the same as the last */
	ptDmaReg->CHANNEL_CONTROL 		|= (BIT0); 								/* UART��interrupt flag generate DMA requeset */
	
	ptDmaReg->CHANNEL_CONTROL 		|= ( CHCTRL_BIT_DMA_DONE_FLAG|CHCTRL_BIT_DMA_INT_ENABLE ); /* ���ж�,ʹ���ж� */
	ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;

	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_lcdCpuConfig
* ����:	����lcd ��DMA
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
Void hyhwDma_lcdCpuConfig(eLCDBUSTYPE_EN  busType)
{
	
	tDMA_REGISTER *ptDmaReg;
	U32 buffAddr;
	ptDmaReg = (tDMA_REGISTER *)LCD_DMA_REGISTER_ADDR;
	
//	buffAddr = ~(((262144)-1)<<4);
//	buffAddr = ~(((1048576)-1)<<4);	//1M
//	ptDmaReg->SOURCE_PARAMETER	 = ((0xffffff00 & buffAddr) | DMA_PORT_TYPE_HALF_WORD);
	ptDmaReg->SOURCE_PARAMETER	  	= DMA_PORT_TYPE_HALF_WORD;

	ptDmaReg->DESTINATION_PARAMETER = DMA_PORT_TYPE_STATIC; 				/* DMAʱDestination ����Ϊ0 */	
		
	if( busType == LcdBusType16 )
	{
		ptDmaReg->CHANNEL_CONTROL   = (  CHCTRL_BIT_TRANSFER_TYPE_SOFTWARE
									   | CHCTRL_BIT_BURST_SIZE_4
									   | CHCTRL_BIT_WORD_SIZE_16BIT	
					//				   | CHCTRL_BIT_REPEAT_MODE_ENABLE
					//				   | CHCTRL_BIT_ISSUE_CONTINU_TRANSFER
									  );
	}
	else
	{
		ptDmaReg->CHANNEL_CONTROL   = (  CHCTRL_BIT_TRANSFER_TYPE_SOFTWARE
									   | CHCTRL_BIT_BURST_SIZE_4
									   | CHCTRL_BIT_WORD_SIZE1_32BIT	
									  );
	}
	
//	ptDmaReg->REPEAT_CONTROL		= 0;
	ptDmaReg->DESTINATION_START_ADDR= LCD_CPU_INTERFACE_ID;

}

/*-----------------------------------------------------------------------------
* ����:	lcdCpuEnable
* ����:	ʹ��lcd ��DMA
* ����:	sa		--	����Դ��ַ
*		size	--	��Ҫ����İ�����
* ����:	none		
*----------------------------------------------------------------------------*/
U32 lcdCpuEnable(U16 * pdata, U32 size)
{
	tDMA_REGISTER *ptDmaReg;

	ptDmaReg = (tDMA_REGISTER *)LCD_DMA_REGISTER_ADDR;
	
	ptDmaReg->SOURCE_START_ADDR = ( U32 )pdata;

	// Ӧ���д�����ǰ��ֵĸ���, ���������2λ
	ptDmaReg->HOP_COUNT			= size>>2;
	
	/* 
	  ��ͼ��ʱ, ÿ�ζ���һ֡, ����Ҫ�ظ�ģʽ, Ҳ���ô����ж�
	  ������ﲻ������ж�
	*/ 
	//ptDmaReg->CHANNEL_CONTROL  |= CHCTRL_BIT_DMA_INT_ENABLE;
	
	ptDmaReg->CHANNEL_CONTROL  |= CHCTRL_BIT_DMA_START;
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	lcdCpuDmaOK
* ����:	�ж�LCD DMA�Ƿ���ɣ������������жϣ�����TRUE�����򷵻�FAULSE
* ����:	none
* ����:	TRUE or FALSE		
*----------------------------------------------------------------------------*/
BOOL lcdCpuDmaOK(Void)
{
	tDMA_REGISTER *ptDmaReg;

	ptDmaReg = (tDMA_REGISTER *)LCD_DMA_REGISTER_ADDR;
	
	if((ptDmaReg->HOP_COUNT >> 16) == 0)
	{
		/* 
		  ��ͼ��ʱ, ÿ�ζ���һ֡, ����Ҫ�ظ�ģʽ, Ҳ���ô����ж�
		  ������ﲻ������ж�
		*/   
	//	ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_DONE_FLAG;
		return TRUE;
		
	}
	else 
	{
		return FALSE;
	}
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_isDmaInterruptSet
* ����:	�ж��Ƿ��ж�ʹ��
* ����:	ChannelId
*		ChannelId--ͨ����
* ����:	True for False
*----------------------------------------------------------------------------*/
BOOL hyhwDma_isDmaInterruptSet(U8 ChannelId)
{
	
	tDMA_REGISTER *ptDmaReg;
	
	if(ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId );
	}	
	else if(ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );      
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER*)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}

	if( ptDmaReg->CHANNEL_CONTROL & CHCTRL_BIT_DMA_DONE_FLAG )
	{
		//ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_DONE_FLAG;
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_DaiDmaConfig
* ����:	����DMA DAI DMA ʹ�ܴ���, ָ���̶�ͨ��DAI_DMA_REGISTER_ADDR
*		�ú�������һ��������ָ���ض�ͨ������DMA����, ��ǰ�ĺ���̫���鷳������3��)
*		�ú����������,�����Ķ���Ҫ�ǻ��ں�codecģ������ͨ���Ĵ��䣬����2��DMA
*		,�ڸ����а�����ʵ��, �������������¼��DMAͨ��
* ����:	daiTransferType------ָ����������, ��ѡֵ(0,1): 0��ʾ���ͣ� 1 ��ʾ����
*		size------��������С, ����ָ��DMA����Ļ�������С,�û��������û�������
*		pBufLeftAddr---------��Ƶ������������ָ�룬������DMA������׵�ַ
*		pBufRightAddr--------��Ƶ������������ָ�룬������DMA������׵�ַ
*		sampleCount----------�����Ӹ������ò�������HOPCOUNTֵ
*----------------------------------------------------------------------------*/
void hyhwDma_DaiDmaConfig(eDAI_TRANS_TYPE daiTransferType, U32 size, 
							U16 *pBufLeftAddr, U16 *pBufRightAddr, U32 sampleCount)
{
	if(daiTransferType == DaiTxType)								// DAI DMA����
	{
		tDMA_REGISTER *ptDmaRegLeft;
		tDMA_REGISTER *ptDmaRegRight;
		ptDmaRegLeft  = ( tDMA_REGISTER * )DAI_DMA_LEFT_REGISTER_ADDR;
		ptDmaRegRight = ( tDMA_REGISTER * )DAI_DMA_RIGHT_REGISTER_ADDR;
		
		
		/* ���������������������ͬ���������ж�ֻʹ��һ��DMAͨ�� */
		ptDmaRegLeft->CHANNEL_CONTROL     = ( CHCTRL_BIT_ISSUE_CONTINU_TRANSFER
											| CHCTRL_BIT_TRANSFER_TYPE_SINGLE_EDGE
											| CHCTRL_BIT_HW_REQUEST_DIRECTION_READ
											| CHCTRL_BIT_REPEAT_MODE_ENABLE
											| CHCTRL_BIT_HW_REQUEST_SYNC_ENABLE
											| CHCTRL_BIT_BURST_SIZE_4
											| CHCTRL_BIT_WORD_SIZE_16BIT
											| CHCTRL_BIT_DMA_INT_ENABLE
										    );
										   
		ptDmaRegRight->CHANNEL_CONTROL    = ( CHCTRL_BIT_ISSUE_CONTINU_TRANSFER
											| CHCTRL_BIT_TRANSFER_TYPE_SINGLE_EDGE
											| CHCTRL_BIT_HW_REQUEST_DIRECTION_READ
											| CHCTRL_BIT_REPEAT_MODE_ENABLE
											| CHCTRL_BIT_HW_REQUEST_SYNC_ENABLE
											| CHCTRL_BIT_BURST_SIZE_4
											| CHCTRL_BIT_WORD_SIZE_16BIT
										    );
										   								   
		/* 
			Hopcount���㷽���� sampleCount(���ֽڼ�)/(burstsize*bitWidth/8) 
			1 sampleCount == 2 bytes, burstsize = 4, bitWidth = 16 bits 
			Hopcount = sampleCount/16
		*/								   
		ptDmaRegLeft->HOP_COUNT				  = (sampleCount >> 3);
		ptDmaRegRight->HOP_COUNT			  = (sampleCount >> 3);
	
	 // left :
	 	// ������Դ��ַ������, �Լ�DMAԴ��ַ�����Ĵ�������
	 	ptDmaRegLeft->SOURCE_START_ADDR  	  = (U32)pBufLeftAddr;
		ptDmaRegLeft->SOURCE_PARAMETER 		  = ((~((size-1)<<4))&0xffffff00)|DMA_PORT_TYPE_HALF_WORD; 
		
		ptDmaRegLeft->DESTINATION_START_ADDR  = DAI_INTERFACE_IDO;
		ptDmaRegLeft->DESTINATION_PARAMETER   = (0xfffffe00|DMA_DAI_LRBUF_OFFSET);
		
		ptDmaRegLeft->EXTERNAL_DMA_REQUEST	  = EXTREQ_BIT_DAITX;
		
	 // right:	
	 	ptDmaRegRight->SOURCE_START_ADDR  	  = (U32)pBufRightAddr;
	 	ptDmaRegRight->SOURCE_PARAMETER 	  = ((~((size-1)<<4))&0xffffff00)|DMA_PORT_TYPE_HALF_WORD;  
		
		ptDmaRegRight->DESTINATION_START_ADDR = DAI_INTERFACE_IDOR;
		ptDmaRegRight->DESTINATION_PARAMETER  = (0xfffffe00|DMA_DAI_LRBUF_OFFSET);
		
		ptDmaRegRight->EXTERNAL_DMA_REQUEST	  = EXTREQ_BIT_DAITX;
	}
	else if(daiTransferType == DaiRxType)					 // DAI DMA����, ¼�����
	/* ��Ϊ�ڴ�绰ʱ����ͬʱ����¼���Ͳ��ţ�����޸Ĵ���Ϊ¼�����򵥶���һ��DMAͨ������¼�� */
	{
		tDMA_REGISTER *ptDmaRegRecord;
		ptDmaRegRecord = ( tDMA_REGISTER * )DAI_DMA_RECORD_REGISTER_ADDR;
		ptDmaRegRecord->CHANNEL_CONTROL   = ( CHCTRL_BIT_ISSUE_CONTINU_TRANSFER
											| CHCTRL_BIT_TRANSFER_TYPE_SINGLE_EDGE
											| CHCTRL_BIT_HW_REQUEST_DIRECTION_READ
											| CHCTRL_BIT_REPEAT_MODE_ENABLE
											| CHCTRL_BIT_HW_REQUEST_SYNC_ENABLE
											| CHCTRL_BIT_BURST_SIZE_4
											| CHCTRL_BIT_WORD_SIZE_16BIT
											| CHCTRL_BIT_DMA_INT_ENABLE
										    );
		ptDmaRegRecord->HOP_COUNT			    = (sampleCount >> 3);

		/*if (0!=Test_TellingChl(0) || 0!=Test_TellingChl(1))
		{   
			// ��绰״̬�� �ú������ڴ�绰ʱ�п��ܲ�ζ���, ��Ҫ�޸��ⲿ�ִ��� 20090910, yanglijing
			if(skr_soft_detect_earphone() == 1)//����mic����ʱ
			{
				ptDmaRegRecord->SOURCE_START_ADDR  	= DAI_INTERFACE_IDIR;
				ptDmaRegRecord->DESTINATION_START_ADDR  = (U32)pBufRightAddr;
				
			}
			else // ��������ʱ
			{
				ptDmaRegRecord->SOURCE_START_ADDR  	= DAI_INTERFACE_IDI;
				ptDmaRegRecord->DESTINATION_START_ADDR  = (U32)pBufLeftAddr;		
			}
			ptDmaRegRecord->SOURCE_PARAMETER   	    = (0xfffffe00|DMA_DAI_LRBUF_OFFSET);
			ptDmaRegRecord->DESTINATION_PARAMETER   = ((~((size-1)<<4))&0xffffff00)|DMA_PORT_TYPE_HALF_WORD;

			ptDmaRegRecord->EXTERNAL_DMA_REQUEST    = EXTREQ_BIT_DAIRX;	
		}
		else*/
		{
			// ����¼��ʱ, �ú������ڴ�绰ʱ�п��ܲ�ζ���, ��Ҫ�޸��ⲿ�ִ��� 20090910, yanglijing
			ptDmaRegRecord->SOURCE_START_ADDR  	= DAI_INTERFACE_IDI;
			ptDmaRegRecord->DESTINATION_START_ADDR  = (U32)pBufLeftAddr;		
			
			ptDmaRegRecord->SOURCE_PARAMETER   	    = (0xfffffe00|DMA_DAI_LRBUF_OFFSET);
			ptDmaRegRecord->DESTINATION_PARAMETER   = ((~((size-1)<<4))&0xffffff00)|DMA_PORT_TYPE_HALF_WORD;

			ptDmaRegRecord->EXTERNAL_DMA_REQUEST    = EXTREQ_BIT_DAIRX;	
		}	
	}
	
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_DaiDmaConfig
* ����:	����DMA DAI DMA ʹ�ܴ���, ָ���̶�ͨ��DAI_DMA_REGISTER_ADDR
*		�ú�������һ��������ָ���ض�ͨ������DMA����, ��ǰ�ĺ���̫���鷳������3��)
*		�ú����������,�����Ķ���Ҫ�ǻ��ں�codecģ������ͨ���Ĵ��䣬����2��DMA
*		,�ڸ����а�����ʵ��, �������������¼��DMAͨ��
* ����:	daiTransferType------ָ����������, ��ѡֵ(0,1): 0��ʾ���ͣ� 1 ��ʾ����
*		size------��������С, ����ָ��DMA����Ļ�������С,�û��������û�������
*		pBufLeftAddr---------��Ƶ������������ָ�룬������DMA������׵�ַ
*		pBufRightAddr--------��Ƶ������������ָ�룬������DMA������׵�ַ
*		sampleCount----------�����Ӹ������ò�������HOPCOUNTֵ
*----------------------------------------------------------------------------*/
void hyhwDma_DaiDmaReConfig(eDAI_TRANS_TYPE daiTransferType, U32 size, 
							U16 *pBufLeftAddr, U16 *pBufRightAddr, U32 sampleCount)
{
	if(daiTransferType == DaiTxType)								// DAI DMA����
	{
		tDMA_REGISTER *ptDmaRegLeft;
		tDMA_REGISTER *ptDmaRegRight;
		ptDmaRegLeft  = ( tDMA_REGISTER * )DAI_DMA_LEFT_REGISTER_ADDR;
		ptDmaRegRight = ( tDMA_REGISTER * )DAI_DMA_RIGHT_REGISTER_ADDR;
						   
		ptDmaRegLeft->HOP_COUNT				  = (sampleCount >> 3);
		ptDmaRegRight->HOP_COUNT			  = (sampleCount >> 3);
	}
	else
	{
		tDMA_REGISTER *ptDmaRegRecord;
		ptDmaRegRecord = ( tDMA_REGISTER * )DAI_DMA_RECORD_REGISTER_ADDR;
		
		ptDmaRegRecord->HOP_COUNT			    = (sampleCount >> 3);

		/*if (0!=Test_TellingChl(0) || 0!=Test_TellingChl(1))
		{   
			// ��绰״̬�� �ú������ڴ�绰ʱ�п��ܲ�ζ���, ��Ҫ�޸��ⲿ�ִ��� 20090910, yanglijing
			if(skr_soft_detect_earphone() == 1)//����mic����ʱ
			{
				ptDmaRegRecord->SOURCE_START_ADDR  	= DAI_INTERFACE_IDIR;
				ptDmaRegRecord->DESTINATION_START_ADDR  = (U32)pBufRightAddr;
				
			}
			else // ��������ʱ
			{
				ptDmaRegRecord->SOURCE_START_ADDR  	= DAI_INTERFACE_IDI;
				ptDmaRegRecord->DESTINATION_START_ADDR  = (U32)pBufLeftAddr;		
			}
			ptDmaRegRecord->SOURCE_PARAMETER   	    = (0xfffffe00|DMA_DAI_LRBUF_OFFSET);
			ptDmaRegRecord->DESTINATION_PARAMETER   = ((~((size-1)<<4))&0xffffff00)|DMA_PORT_TYPE_HALF_WORD;

			ptDmaRegRecord->EXTERNAL_DMA_REQUEST    = EXTREQ_BIT_DAIRX;	
		}*/
	}
	
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_DaiDmaEnable
* ����:	����DAI DMA��DMA����
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwDma_DaiDmaEnable(eDAI_TRANS_TYPE daiTransferType)
{
	tDMA_REGISTER *ptDmaReg;
	if(daiTransferType == DaiTxType)
	{
		ptDmaReg = ( tDMA_REGISTER * )DAI_DMA_LEFT_REGISTER_ADDR;
		ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;

		ptDmaReg = ( tDMA_REGISTER * )DAI_DMA_RIGHT_REGISTER_ADDR;
		ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;
	}
	else
	{
		ptDmaReg = ( tDMA_REGISTER * )DAI_DMA_RECORD_REGISTER_ADDR;
		ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;
	}	
}

/*-----------------------------------------------------------------------------
* ����:	isDaiDmaDmaOK
* ����:	�ж�dai DMA�Ƿ���ɣ������������жϣ�����TRUE�����򷵻�FAULSE
* ����:	none
* ����:	TRUE or FALSE		
*----------------------------------------------------------------------------*/
BOOL isDaiDmaDmaOK(eDAI_TRANS_TYPE daiTransferType)
{
	tDMA_REGISTER	*ptDmaReg;
	if(daiTransferType == DaiTxType)
	{	
		ptDmaReg = (tDMA_REGISTER *)DAI_DMA_REGISTER_ADDR;
		
		if(  ( ptDmaReg->HOP_COUNT >> 16) == 0  )
		{
			ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_DONE_FLAG;
			return TRUE;
		}
		else 
		{
			return FALSE;
		}
	}
	else if(daiTransferType == DaiRxType)
	{
		ptDmaReg = (tDMA_REGISTER *)DAI_DMA_RECORD_REGISTER_ADDR;
		
		if(  ( ptDmaReg->HOP_COUNT >> 16) == 0  )
		{
			ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_DONE_FLAG;
			return TRUE;
		}
		else 
		{
			return FALSE;
		}
	}	
}

/*----------------------------------------------------------------------------
* ����:	hyhwDai_setTxSampleCount
* ����:	 
* ����:	sampleCount				--  Tx sample count 
* ����:	HY_OK					--	�ɹ�
		HY_ERR_BAD_PARAMETER 	--	�����������
*----------------------------------------------------------------------------*/
U32 hyhwDma_SetDaiSampleCount(U32 sampleCount, eDAI_TRANS_TYPE daiTransferType)
{
	tDMA_REGISTER *ptDmaReg;
	
	if((sampleCount > CODEC_BUFF_MAX)||(sampleCount < CODEC_BUFF_MIN))	
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
	if(daiTransferType == DaiTxType)
	{
		ptDmaReg = ( tDMA_REGISTER * )DAI_DMA_REGISTER_ADDR;
		ptDmaReg->HOP_COUNT = ((sampleCount)>>0x03);
		
		ptDmaReg = ( tDMA_REGISTER * )DAI_DMA_RIGHT_REGISTER_ADDR;
		ptDmaReg->HOP_COUNT = ((sampleCount)>>0x03);
	}	
	else if(daiTransferType == DaiRxType)
	{
		ptDmaReg = ( tDMA_REGISTER * )DAI_DMA_RECORD_REGISTER_ADDR;
		ptDmaReg->HOP_COUNT = ((sampleCount)>>0x03); 
	}
	
	
	return HY_OK;	
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_DaiDmaConfig	added by yanglijing, 2009-08-05
* ����:	����DMA SPI DMA ʹ�ܴ���, ָ���̶�ͨ��SPI_DMA_REGISTER_ADDR
* ����:	daiTransferType------ָ����������, ��ѡֵ(0,1): 0��ʾ���ͣ� 1 ��ʾ����
*		size-----------------��������С
*		pBufAddr-------------DMA�����׵�ַ
*		count----------------�����ж���ֵ, �ò�������HOPCOUNTֵ, �ֽڱ�ʾ
*----------------------------------------------------------------------------*/
void hyhwDma_SpiDmaConfig(eSPI_TRANS_TYPE TransferType, U32 size, U32 *pBufAddr, U32 Count)
{
	tDMA_REGISTER *ptDmaReg;
	
	ptDmaReg = ( tDMA_REGISTER * )SPI_DMA_REGISTER_ADDR;
	
	/* SPI DMA ���ƼĴ������� */
	ptDmaReg->CHANNEL_CONTROL     = ( CHCTRL_BIT_ISSUE_CONTINU_TRANSFER
									| CHCTRL_BIT_TRANSFER_TYPE_SINGLE_EDGE
									| CHCTRL_BIT_HW_REQUEST_DIRECTION_READ
									| CHCTRL_BIT_REPEAT_MODE_ENABLE
									| CHCTRL_BIT_HW_REQUEST_SYNC_ENABLE
									| CHCTRL_BIT_BURST_SIZE_4
									| CHCTRL_BIT_WORD_SIZE1_32BIT
									| CHCTRL_BIT_DMA_INT_ENABLE
									);
									   
	/* 
		ע��: SPI ��������ϵͳ����Ϊ32λ, ���������Ϊ4, Ҳ����DMAһ�����������ֽ���
			��16, Ϊ��ͬ������ļĴ������ó�: �����Ϊ4, λ��32bit.
	/* 
		Hopcount���㷽���� count ֵ����ָ�ֽ���, ���Դ�DMA�жϴ�����ֽ���, hopCount = count / 16
	*/								   
	ptDmaReg->HOP_COUNT				  	  = (Count >> 4);
	
	if(TransferType == TxType)						// SPI DMA����
	{
		/*  // ���䷽��, ��Ҫд�뵽���ͼĴ�����ֵ���ڴ��ַ�xд���Ĵ���
			����Դ��ַӦ���Ǵ���Ļ������׵�ַ, ��ֵ����λ�����洢, 32Bitϵͳ����
			32Bit���ݴ洢�ٶȻ�ȽϿ�, ��SPI���õ�λ��һ��
		*/	
		ptDmaReg->SOURCE_START_ADDR       = (U32)pBufAddr;
		ptDmaReg->SOURCE_PARAMETER        = ((~((size-1)<<4))&0xffffff00) |DMA_PORT_TYPE_WORD;
		
		/*
	 		����Ŀ�ĵ�ַӦ����SPI�����ݼĴ���, ����ƫ����ΪDMA_PORT_TYPE_STATIC
	 		DMA ����ʱԴ��ַ���������仯 
	 	*/	
	 	ptDmaReg->DESTINATION_START_ADDR  = (U32)SPI_DMA_TX_DST_REGISTER;
		ptDmaReg->DESTINATION_PARAMETER   = DMA_PORT_TYPE_STATIC;
		
		ptDmaReg->EXTERNAL_DMA_REQUEST	  = EXTREQ_BIT_GPSB0TX;
		
	 
	}
	else if(TransferType == RxType)					 // SPI DMA����
	{
		/*
	 		����ʱԴ��ַӦ����SPI�����ݼĴ���, ����ƫ����ΪDMA_PORT_TYPE_STATIC
	 		DMA ����ʱԴ��ַ���������仯 
	 	*/	
		ptDmaReg->SOURCE_START_ADDR       = SPI_DMA_RX_SRC_REGISTER;
		ptDmaReg->SOURCE_PARAMETER        = DMA_PORT_TYPE_STATIC;
		
		/*
			����ʱĿ�ĵ�ַӦ���Ǵ���Ļ������׵�ַ, ��ֵ����λ�����洢, 32Bitϵͳ����
			32Bit���ݴ洢�ٶȻ�ȽϿ�, ��SPI���õ�λ��һ��
		*/	
	 	ptDmaReg->DESTINATION_START_ADDR  = (U32)pBufAddr;
	 	ptDmaReg->DESTINATION_PARAMETER   = ((~((size-1)<<4))&0xffffff00) |DMA_PORT_TYPE_WORD;
	 	
	 	
		
		ptDmaReg->EXTERNAL_DMA_REQUEST    = EXTREQ_BIT_GPSB0RX;
	}	
}							

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_SpiDmaEnable
* ����:	����SPI DMA��DMA����
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwDma_SpiDmaEnable()
{
	tDMA_REGISTER *ptDmaReg;
	ptDmaReg = ( tDMA_REGISTER * )SPI_DMA_REGISTER_ADDR;
	ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;
}


/*---------------------------------------------------------------------------->
added start by yanglijing , 20090806
/*-----------------------------------------------------------------------------
* ����:	hyhwDma_uart1Config
* ����:	����DMA uart1 Tx DMA ʹ�ܴ���, ����TD
* ����:	transType---------------DMA���䷽��, ��ѡ����TxType/RxType
*		buffsize----------------���λ�������С, ���ֽڼ�
*		sourceAddr--------------����Դ��ַ��ַ
*		bytesCount--------------DMAһ���жϵ��ֽ���
* ����: none
* ����: hyhwDma_uart1Confit(TxType, 512, 0x2010000, 128)
*		���ô���DMAͨ������Ϊ���λ�������С512�ֽڣ�����ģʽ��ÿ128�ֽ�һ��DMA�ж�
*		���մ��ڲ��ֽ������޸ģ�BSIZE ��Ϊile1����ÿ��DMA����1���ֽ�
*----------------------------------------------------------------------------*/
void hyhwDma_Uart1Config(e_TRANS_TYPE transType, U32 buffsize, U32 sourceAddr, U32 bytesCount)
{
	tDMA_REGISTER *ptDmaReg;
	
	ptDmaReg = UART1_DMA_REGISTER_ADDR;
	
	/* UART1 DMA ���ƼĴ������� */
	ptDmaReg->CHANNEL_CONTROL     = ( CHCTRL_BIT_ISSUE_CONTINU_TRANSFER
									| CHCTRL_BIT_TRANSFER_TYPE_HARDWARE//CHCTRL_BIT_TRANSFER_TYPE_SINGLE_EDGECHCTRL_BIT_TRANSFER_TYPE_HARDWARE//|CHCTRL_BIT_TRANSFER_TYPE_SINGLE_EDGE
									| CHCTRL_BIT_HW_REQUEST_DIRECTION_READ
									| CHCTRL_BIT_REPEAT_MODE_ENABLE
									| CHCTRL_BIT_HW_REQUEST_SYNC_ENABLE
									| CHCTRL_BIT_BURST_SIZE_1
									| CHCTRL_BIT_WORD_SIZE_8BIT
									| CHCTRL_BIT_DMA_INT_ENABLE
									);
									   
	/* 
		ע��: UART1 ��������ϵͳ����Ϊ8λ, ���������Ϊ16, ��һ�������8����д����, ���һ����ഫ��
			��8���ֽ�
	/* 
		Hopcount���㷽���� count ֵ����ָ�ֽ���, ���Դ�DMA�жϴ�����ֽ���, hopCount = count / 8
	*/								   
	ptDmaReg->HOP_COUNT				  	  = (1);
	ptDmaReg->REPEAT_CONTROL              = (  bytesCount 
	                                         | RPTCTRL_BIT_DRI_REPEAT_INT_DISABLE
	                                         | RPTCTRL_BIT_DRI_REPEAT_INT_DISABLE
	                                        );
	if(transType == TxType)						// SPI DMA����
	{
	 	/*	// ���䷽��, ��Ҫд�뵽���ͼĴ�����ֵ���ڴ��ַ�xд���Ĵ���
			����Դ��ַӦ���Ǵ���Ļ������׵�ַ, ��ֵ����λ�����洢, UART�Ĵ������Ϊ8bit
	 	*/	
	 	ptDmaReg->SOURCE_START_ADDR  	  = (U32)sourceAddr;
		ptDmaReg->SOURCE_PARAMETER 		  = ((~((buffsize-1)<<4))&0xffffff00) |DMA_PORT_TYPE_BYTE;
		
		/*
			����Ŀ�ĵ�ַӦ��UART��THT���ݼĴ���, ����ƫ����ΪDMA_PORT_TYPE_STATIC
	 		DMA ����ʱԴ��ַ���������仯 
		*/	
		ptDmaReg->DESTINATION_START_ADDR  = (U32)UART1_DMA_TX_DST_REGISTER;
		ptDmaReg->DESTINATION_PARAMETER   = DMA_PORT_TYPE_STATIC;
		
		ptDmaReg->EXTERNAL_DMA_REQUEST	  = EXTREQ_BIT_UART1TX;
		
	 
	}
	else if(transType == RxType)					 // SPI DMA����
	{
		/*
			����Դ��ַӦ��UART��RBR���ݼĴ���, ����ƫ����ΪDMA_PORT_TYPE_STATIC
	 		DMA ����ʱԴ��ַ���������仯
		*/	
	 	ptDmaReg->SOURCE_START_ADDR  	  = (U32)UART1_DMA_RX_SRC_REGISTER;
	 	ptDmaReg->SOURCE_PARAMETER   	  = DMA_PORT_TYPE_STATIC;
	 		
	 	/*
	 		����ʱĿ�ĵ�ַӦ���Ǵ���Ļ������׵�ַ, ��ֵ����λ�����洢, UART�Ĵ������Ϊ8bit
	 	*/	
		ptDmaReg->DESTINATION_START_ADDR  = (U32)sourceAddr;
		ptDmaReg->DESTINATION_PARAMETER   = ((~((buffsize-1)<<4))&0xffffff00) |DMA_PORT_TYPE_BYTE;
		
		ptDmaReg->EXTERNAL_DMA_REQUEST    = EXTREQ_BIT_UART1RX;
		
	}	
}
/*added end, by yanglijing , 20090806
<-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_SpiDmaEnable
* ����:	����UART1 DMA��DMA����
* ����:	none
* ����:	none		
*----------------------------------------------------------------------------*/
void hyhwDma_Uart1DmaEnable()
{
	tDMA_REGISTER *ptDmaReg;
	ptDmaReg = ( tDMA_REGISTER * )UART1_DMA_REGISTER_ADDR;
	ptDmaReg->CHANNEL_CONTROL 		|= CHCTRL_BIT_DMA_START;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_GetCurrentSrcAddr 
* ����:	��ѯ��ǰDMAͨ����ǰ�����Դ��ַ
* ����:	ChannelId--ͨ����
* ����:	��ǰDMA����Դ��ֵַ/ ����HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDma_GetCurrentSrcAddr(U32 ChannelId)
{
	tDMA_REGISTER *ptDmaReg;
	
	//�������
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId ); 
	}	
	else if (ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );   
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	return ptDmaReg->SOURCE_CURRENT_ADDR;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwDma_GetCurrentDstAddr 
* ����:	��ѯ��ǰDMAͨ����ǰ����Ŀ�ĵ�ַ
* ����:	ChannelId--ͨ����
* ����:	��ǰDMA����Ŀ�ĵ�ֵַ
*----------------------------------------------------------------------------*/
U32 hyhwDma_GetCurrentDstAddr(U32 ChannelId)
{
	tDMA_REGISTER *ptDmaReg;
	
	//�������
	if (ChannelId >= DMA_TOTAL_CHANNEL_NUM) 
	{
		return HY_ERR_BAD_PARAMETER;
	}	
	if (ChannelId < DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA0_BASEADDR + DMA_CHANNEL_OFFSET*ChannelId ); 
	}	
	else if (ChannelId < 2*DMA_CHANNEL_NUM_PER_DMA)
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA1_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-DMA_CHANNEL_NUM_PER_DMA) );   
	}
	else
	{
		ptDmaReg = (tDMA_REGISTER *)(DMA2_BASEADDR + DMA_CHANNEL_OFFSET*(ChannelId-2*DMA_CHANNEL_NUM_PER_DMA) );   
	}
	
	return ptDmaReg->DESTINATION_CURRENT_ADDR;
}


/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_RGBEnable
* ����:	Setting the  800*480 color image,
* ����:	* color	--  the data point
* ����:	none		
*----------------------------------------------------------------------------*/
U32 hyhwLcd_RGBEnable( U32 sorceAddr, U32 destAddr, U32 size )
{
	tDMA_REGISTER	*ptDmaReg;
	ptDmaReg = ( tDMA_REGISTER * )LCD_DMA_REGISTER_ADDR;
	
	ptDmaReg->SOURCE_PARAMETER = DMA_PORT_TYPE_HALF_WORD; 	/*DMAʱsource ����Ϊ1*/
	ptDmaReg->DESTINATION_PARAMETER = DMA_PORT_TYPE_HALF_WORD; /*DMAʱDestination ����Ϊ0*/		
	
	ptDmaReg->SOURCE_START_ADDR = ( U32 )sorceAddr;
	ptDmaReg->DESTINATION_START_ADDR = ( U32 )destAddr;
	
	ptDmaReg->CHANNEL_CONTROL   = (  CHCTRL_BIT_TRANSFER_TYPE_SOFTWARE
									   | CHCTRL_BIT_BURST_SIZE_4
									   | CHCTRL_BIT_WORD_SIZE_16BIT	
									  );
	ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_INT_ENABLE;
	ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_START;
	ptDmaReg->HOP_COUNT	= (size>>1);
	
	
	return HY_OK;
}
/*-----------------------------------------------------------------------------
* ����:	lcdCpuDmaOK
* ����:	�ж�RGB DMA�Ƿ���ɣ������������жϣ�����TRUE�����򷵻�FAULSE
* ����:	none
* ����:	TRUE or FALSE		
*----------------------------------------------------------------------------*/
BOOL hyhwLcd_RGBDmaOK( void )
{
	tDMA_REGISTER	*ptDmaReg;
	( U32 * )ptDmaReg = ( U32 * )LCD_DMA_REGISTER_ADDR;
	if(( ptDmaReg->HOP_COUNT >> 16) == 0 )
	{
		ptDmaReg->CHANNEL_CONTROL |= CHCTRL_BIT_DMA_DONE_FLAG;
		return TRUE;
	}
	else 
	{
		return FALSE;
	}
}

/*-----------------------------------------------------------------------------
*	�޸ļ�¼ 1 :
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
	
// ɾ������ǰ�������Ѿ���#if 0 ע���ĺ�������, 	
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
*	�޸ļ�¼ 2 :
2009 08 06	������
����:
	��Ҫ����UART1Ϊ��������DMA����, ����TD���ݵĴ��䣬���Ǻ�����, 3M���ں�����
	uart1 3M,ʹ��DMA���գ�ͬʱ����uart ����1��byte�жϷ�ʽ���жϷ��������ֹͣuart�ж�
	��������һ��timer��ʱ�ж�(�ݶ�5ms)��������ʱ�ж�ʱ����ȡDMA��ǰ��дλ�ã���DMA�Ѿ����յ�������copy����
	������������DMA��ֹͣ��timer���������DAM�жϣ�����������timerͣ��������������1byte��uart�жϡ�DMA��
	�ÿ���Ϊ��8k bytes�ڴ�ѭ��ʹ�ã�2k�����жϡ�
����:
	��hyhwDma�ļ���ģ�����������������hyhwDma_Uart1Config �� hyhwDma_Uart1DmaEnable
	, һ��������UART1��DMA���õ�, һ������ʹ��DMA����, ������˼����궨��, ��hyhwDma.h�ļ�
�޸�λ��:
	1\hyhwDma.h ��������
		hyhwDma_Uart1Config �� hyhwDma_Uart1DmaEnable
	2\hyhwDma.c hyhwDma_Uart1Config �� hyhwDma_Uart1DmaEnable ��������
*//*-----------------------------------------------------------------------------
*	�޸ļ�¼ 3 :
2009 08 19	������
����:
	��Ҫ��DMA��ͣ����������������DMA���ɴ�ԭ������ͣ��λ�ûָ�����
����:
	
�޸�λ��:
	ͷ�ļ������Ӻ���ԭ������, Դ�ļ�����Ӻ�������
	U32 hyhwDma_Pause(U8 ChannelId)
	U32 hyhwDma_restart(U8 ChannelId)
	BOOL hyhwDma_isPause(U8 ChannelId)
	����������Ϊ����ӵ�, ��hyhwDma_isPause����TRUEʱ������hyhwDma_restart���������
-------------------------------------------------------------------------------*//* 

*/