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
#include "hyhwDai.h"
#include "hyhwCkc.h"
#include "hyhwDma.h"

/*---------------------------------------------------------------------------
   extern function ����
----------------------------------------------------------------------------*/
//defined by GPIO ģ��
extern void hyhwGpio_setDaiFunction(void);

#define CODEC_PLAY_DMA_LCHANNEL 3
#define CODEC_PLAY_DMA_RCHANNEL 4
#define CODEC_RECORD_DMA_CHANNEL 5
/*---------------------------------------------------------------------------
* ��  :	hyhwmDai_DmaEnable 
* ����:	ʹ�� Dai Dma ����
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmDai_DmaEnable(eDAI_TRANS_TYPE daiTransferType)	
{
	// DMA1 ͨ��0/1 ��ΪDAI DMA ͨ������Ӧͨ����Ϊ3/4
	//if(hyhwDma_Enable(4) == HY_OK)
	if(daiTransferType == DaiTxType)
	{
		if(hyhwDma_Enable(CODEC_PLAY_DMA_RCHANNEL) == HY_OK)
		{
			return hyhwDma_Enable(CODEC_PLAY_DMA_LCHANNEL);	
		}	
	}
	else if(daiTransferType == DaiRxType)
	{
		return hyhwDma_Enable(CODEC_RECORD_DMA_CHANNEL);	
	}
}

/*--------------------------------------------------------------------------
* ��  :	hyhwmDai_DmaDisable 
* ����:	disable Dai Dma ����
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwmDai_DmaDisable(eDAI_TRANS_TYPE daiTransferType)
{
	// DMA1 ͨ��0 ��ΪDAI DMA ͨ������Ӧͨ����Ϊ3/4
	//if(hyhwDma_Disable(4) == HY_OK)
	if(daiTransferType == DaiTxType)
	{
		if(hyhwDma_Disable(CODEC_PLAY_DMA_RCHANNEL) == HY_OK)
		{
			return hyhwDma_Disable(CODEC_PLAY_DMA_LCHANNEL);
		}	
	}
	else if(daiTransferType == DaiRxType)
	{
		return hyhwDma_Disable(CODEC_RECORD_DMA_CHANNEL);
	}	
}

/*---------------------------------------------------------------------------
* ����:	hyhwDai_DmaIntClear
* ����:	���ָ��ͨ����DMA�ж�
* ����:	ChannelId--ͨ����
* ����:	HY_OK
*		HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwDai_DmaIntClear(eDAI_TRANS_TYPE daiTransferType)
{
	if(daiTransferType == DaiTxType)
	{
		return hyhwDma_IntClear(CODEC_PLAY_DMA_LCHANNEL);
	}
	else if(daiTransferType == DaiRxType)
	{
		return hyhwDma_IntClear(CODEC_RECORD_DMA_CHANNEL);
	}
}

/*---------------------------------------------------------------------------
* ��  :	hyhwmDai_DmaIntEnable 
* ����:	enable dma rxtx interrupt
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/		
U32 hyhwmDai_DmaIntEnable(eDAI_TRANS_TYPE daiTransferType)	
{
	// DMA1 ͨ��0 ��ΪDAI DMA ͨ������Ӧͨ����Ϊ3
	//if(hyhwDma_IntEnable(4))
	if(daiTransferType == DaiTxType)
	{
		return hyhwDma_IntEnable(CODEC_PLAY_DMA_LCHANNEL);
	}
	else if(daiTransferType == DaiRxType)
	{
		return hyhwDma_IntEnable(CODEC_RECORD_DMA_CHANNEL);
	}

}

/*---------------------------------------------------------------------------
* ��  :	hyhwmDai_DmaIntDisable 
* ����:	disable dma tx interrupt
* ����:	none
* ����: HY_OK, or HY_ERR_BAD_PARAMETER
*---------------------------------------------------------------------------*/
U32 hyhwmDai_DmaIntDisable(eDAI_TRANS_TYPE daiTransferType)
{
	// DMA1 ͨ��0 ��ΪDAI DMA ͨ������Ӧͨ����Ϊ3
	if(daiTransferType == DaiTxType)
	{
		return hyhwDma_IntDisable(CODEC_PLAY_DMA_LCHANNEL);
	}
	else if(daiTransferType == DaiRxType)
	{
		return hyhwDma_IntDisable(CODEC_RECORD_DMA_CHANNEL);
	}
	
}
	
/*---------------------------------------------------------------------------
* ����:	hyhwDma_checkInterruptState
* ����:	�ж��Ƿ��ж�ʹ��
* ����:	none
* ����:	True �����Ѿ��ж�, FALSE ����δ�ж�
*--------------------------------------------------------------------------*/
BOOL hyhwDai_DmacheckInterruptState(eDAI_TRANS_TYPE daiTransferType)
{
	// DMA1 ͨ��0 ��ΪDAI DMA ͨ������Ӧͨ����Ϊ3
	if(daiTransferType == DaiTxType)
	{
		return hyhwDma_isDmaInterruptSet(CODEC_PLAY_DMA_LCHANNEL);
	}
	else if(daiTransferType == DaiRxType)
	{
		return hyhwDma_isDmaInterruptSet(CODEC_RECORD_DMA_CHANNEL);
	}
}

/*---------------------------------------------------------------------------
* ����:	hyhwDma_checkInterruptState
* ����:	���ָ��ͨ����DMA��״̬������/busy��
*			�Ȳ鿴��ͨ���Ƿ�����---����
*			���ݴ������-----------����
*			�������ڴ���-----------������
* ����:	none
* ����:	HY_OK
*		HY_ERR_BUSY
*		HY_ERR_BAD_PARAMETER
*--------------------------------------------------------------------------*/
U32 hyhwDai_DmaGetStatus(eDAI_TRANS_TYPE daiTransferType)
{
	// DMA1 ͨ��0 ��ΪDAI DMA ͨ������Ӧͨ����Ϊ3
	if(daiTransferType == DaiTxType)
	{
		return hyhwDma_GetStatus(CODEC_PLAY_DMA_LCHANNEL);
	}
	else if(daiTransferType == DaiRxType)
	{
		return hyhwDma_GetStatus(CODEC_RECORD_DMA_CHANNEL);
	}
}

/*----------------------------------------------------------------------------
* ����:	hyhwDai_Init
* ����:	initial dai 
* ����:	none
* ����:	HY_OK					--	�ɹ�
		HY_ERR_BAD_PARAMETER 	--	�����������
*----------------------------------------------------------------------------*/
U32 hyhwDai_Init(void)
{
	//hyhwCpm_busClkEnable(CKC_DAI);		//enalbe the dai clk	
	hyhwGpio_setDaiFunction();
	//hyhwCpm_setDai0Clk();				//����DAI ʱ�ӣ���Ҫ����CKC_PCK_DAI_REGISTER
	
	return HY_OK;
}


/*----------------------------------------------------------------------------
* ����:	hyhwmDai_setMaster
* ����:	��DAI Set master mode
* ����:	none
* ����:	none 
*----------------------------------------------------------------------------*/
void hyhwDai_setMaster(void)
{
	DAI_DAVC_REGISTER  = DAVC_BIT_0dB;
	
	DAI_DAMR_REGISTER  = (//  DAI_DAMR_BIT_ENBLE		// DAI Master Enable
//						  | DAI_DAMR_BIT_TEN			// enable DAI transmitter
//						  | DAI_DAMR_BIT_REN			// enable Receiver
						    DAI_DAMR_BIT_SM				// 1 set that DAI system clock is generated by the clock generator block
						  | DAI_DAMR_BIT_BM				// bit clock is generated by dividing the system clock
						  | DAI_DAMR_BIT_FM				// frame clock is generated by dividing the bit clock
//						  | DAI_DAMR_BIT_BP				// bit clock polarity ,negtive edge effective 
//						  | DAI_DAMR_BIT_MM				// enable DAI monitor mode
//						  | DAI_DAMR_BIT_LB				// loopback mode enabled
						  | DAI_DAMR_BITMASK_BD16  		// DAI bit clock divider 4 (256->64)
						  | DAI_DAMR_BITMASK_FD32  		// DAI frame clock divider  00 select div 64 (64->1)
						  | DAI_DAMR_BITMASK_RXS_LSB16 	// DAI RX shift, LSB -16bit mode ����newcleus ����
						  | DAI_DAMR_BITMASK_TXS_LSB16 	// DAI TX shif, LSB-16bit mode 
						  
						 );
	// ����������������û������ģ�����ֻ��Ϊ��˵�������õ�����, ��Ϊ����ĸ�ֵ�����Ѿ�����Щλ����				 
	DAI_DAMR_REGISTER &= ~( DAI_DAMR_BIT_MS				// internal BCLK is used(Master mode )
						  | DAI_DAMR_BIT_PXE 			// disable (zero extension)
						  | DAI_DAMR_BIT_CC				// 0 disable CDIF clock master mode 
						  | DAI_DAMR_BIT_MD				// set dai bus as i2s bus mode
						  | DAI_DAMR_BIT_CM				// disable cdif monitor mode
						 );	
}

/*----------------------------------------------------------------------------
* ����:	hyhwDai_setSlave
* ����:	��DAI Set slave mode 
* ����:	none
* ����:	none 
*----------------------------------------------------------------------------*/
void hyhwDai_setSlave(void)
{
	DAI_DAVC_REGISTER = DAVC_BIT_0dB;
	
	DAI_DAMR_REGISTER  = (  DAI_DAMR_BIT_MS				// 1 External BCLK used , Slave Mode 
						  | DAI_DAMR_BITMASK_RXS_LSB24 	// DAI RX shift, LSB -24bit mode
						  | DAI_DAMR_BITMASK_TXS_LSB16 	// DAI TX shif, LSB-16bit mode 
						  | DAI_DAMR_BIT_TEN			// enable DAI transmitter
						  | DAI_DAMR_BIT_REN			// enable Receiver
						  | DAI_DAMR_BITMASK_BD16  		// DAI bit clock divider 16 (512->32)
						  | DAI_DAMR_BITMASK_FD32  		// DAI frame clock divider  00 select div 32 (32->1)
						  | DAI_DAMR_BIT_BP				// bit clock polarity ,negtive edge effective
						  | DAI_DAMR_BIT_MM				// enable DAI monitor mode
						  | DAI_DAMR_BIT_LB				// loopback mode enabled
						 );
	// ����������������û������ģ�����ֻ��Ϊ��˵�������õ�����, ��Ϊ����ĸ�ֵ�����Ѿ�����Щλ����
	DAI_DAMR_REGISTER &= ~( DAI_DAMR_BIT_PXE 			// disable (zero extension)
						  & DAI_DAMR_BIT_ENBLE			// DAI Master Enable
						  & DAI_DAMR_BIT_MD				// set dai bus as i2s bus mode
						  & DAI_DAMR_BIT_SM				// 1 set that DAI system clock from extern
						  & DAI_DAMR_BIT_BM				// bit clock from extern
						  & DAI_DAMR_BIT_FM				// frame clock  from extern
						  & DAI_DAMR_BIT_CC				// 0 disable CDIF clock master mode 
						  & DAI_DAMR_BIT_CM				// disable cdif monitor mode
						 );			 
}

/*-----------------------------------------------------------------------------
* ����:	U32 hyhwAudioCfg_SetSamplingRate( hyhwAudioCfg_SamplingRate_en)
* ����:	���ò�����
* ����:	samplingRate	--	ö�٣����¼��ֲ�����
*							hz_8000,
*							hz_11025,
*							hz_16000,
*							hz_22050,
*							hz_32000,
*							hz_44100,
*							hz_48000,
* ����:	HY_OK,
*		HY_ERR_BAD_PARAMETER
*----------------------------------------------------------------------------*/
U32 hyhwDai_SetSamplingRate(eHYHWAUDIOCFG_SAMPLINGRATE_EN samplingRate)
{
	/* ע�͵����������Ҫԭ�������Ӧ�ó��򲿷֣������޸�.
	   ��Ӧ�ó����һ�ε��øú�����ǰ, �и�lcdcpu��DMA����, 
	   ������︴λ�Ժ�, DMA���þ�ʧЧ�ˡ����³�����DMA����.
	   ��Ȼ���Ӧ�ó�����ͼ����ʾǰ�ٵ���һ��DMA���ú���Ҳ���ԡ�
	   20090911, yanglijing
	 */  
	//hyhwCpm_setSoftReset(CKC_DMA);
	__asm{ nop };
	__asm{ nop };
	__asm{ nop };
	__asm{ nop };
	//hyhwCpm_clearSoftReset(CKC_DMA);

//	hyhwCpm_setDaiClk(512*samplingRate);			/* set dai system clk */
	hyhwCpm_setDaiDcoClk(512*samplingRate);			/* set dai system clk */
	return HY_OK;
}