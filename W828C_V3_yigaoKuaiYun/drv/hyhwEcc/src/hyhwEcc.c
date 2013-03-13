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
/*************************************************************************
 * INCLUDES
 *************************************************************************/
#include "hyTypes.h"
#include "hyErrors.h"
#include "hyhwTCC7901.h"
#include "hyhwEcc.h"
#include "hyhwNandFlash.h"
/*************************************************************************
 * DEFINES
 *************************************************************************/
#define ECC_CONTROL_REGISTER								TCC7901_ECC_CTRL_REGISTER
	#define ECC_BIT_MLC_ECC8_DECODING_INTERRUPT_ENABLE	 	BIT31
	#define ECC_BIT_MLC_ECC4_DECODING_INTERRUPT_ENABLE	 	BIT30	
	#define ECC_BIT_ECC_DMA_REQUEST_ENABLE	 				BIT29	
	#define ECC_BIT_ECC_DATA_SIZE_MASK	 					(~0x07FF0000)		/* BIT[26:16] -- DATASIZE */ 			
	#define ECC_DATA_SIZE_SHIFT	 							16 			
	#define ECC_BIT_ECC_DATA_SIZE(X)	 					(X<<ECC_DATA_SIZE_SHIFT) 			
	#define ECC_BIT_ECC_COUNTER_VALLUE_MASK					0x0000FFF8			/*read-only register*/
	#define ECC_BIT_ECC_ENABLE_CTRL_MASK	 				~(BIT2|BIT1|BIT0)
	#define ECC_BIT_ECC_DISABLE	 							~(BIT2|BIT1|BIT0)
	#define ECC_BIT_SLC_ECC_ENCODING_ENABLE	 				2	
	#define ECC_BIT_SLC_ECC_DECODING_ENABLE	 				3	
	#define ECC_BIT_MLC4_ECC_ENCODING_ENABLE	 			4
	#define ECC_BIT_MLC4_ECC_DECODING_ENABLE	 			5
	#define ECC_BIT_MLC8_ECC_ENCODING_ENABLE	 			6
	#define ECC_BIT_MLC8_ECC_DECODING_ENABLE	 			7
#define ECC_BASE_ADDRESS_REGISTER							TCC7901_ECC_BASE_REGISTER
	#define BASE_ADDRESS_OFFSET								0
#define ECC_ADDRESS_MASK_REGISTER							TCC7901_ECC_MASK_REGISTER
	#define ADDRESS_MASK_OFFSET								0
#define ECC_CLEAR_REGISTER									TCC7901_ECC_CLEAR_REGISTER

#define ECC_CLEAR_REGISTER									TCC7901_ECC_CLEAR_REGISTER
	#define REGISTER_BIT_CLEAR								0x00000000
#define ECC_SLC_ECC_CODE_REGISTER							TCC7901_ECC_CLEAR_REGISTER
#define MLC_ECC4_CODE_REGISTER_0 							TCC7901_ECC_MECC4_0_REGISTER
#define MLC_ECC4_CODE_REGISTER_1 							TCC7901_ECC_MECC4_1_REGISTER
#define MLC_ECC4_CODE_REGISTER_2 							TCC7901_ECC_MECC4_2_REGISTER
#define MLC_ECC4_ERROR_NUMBER_REGISTER						TCC7901_ECC_ERRNUM_REGISTER
#define ECC_INT_REQ_REGISTER								TCC7901_ECC_IREQ_REGISTER
	#define ECC_BIT_SLC_ECC_ENCODING_FLAG_MASK				BIT21
	#define	ECC_BIT_SLC_ECC_DECODING_FLAG_MASK				BIT20
	#define ECC_BIT_MLC_ECC4_ENCODING_FLAG_MASK				BIT19
	#define	ECC_BIT_MLC_ECC4_DECODING_FLAG_MASK				BIT18
	#define ECC_BIT_MLC_ECC8_ENCODING_FLAG_MASK				BIT17
	#define	ECC_BIT_MLC_ECC8_DECODING_FLAG_MASK				BIT16
	#define ECC_BIT_MLC_ECC4_DECODING_INTERRUPT_REQ_MASK	BIT2
	#define	ECC_BIT_MLC_ECC8_DECODING_INTERRUPT_REQ_MASK	BIT0

#define MLC_ECC4_ERROR_ADDRESS_REGISTER_0					TCC7901_ECC_MECC4_EADDR0_REGISTER  		
#define MLC_ECC4_ERROR_ADDRESS_REGISTER_1					TCC7901_ECC_MECC4_EADDR1_REGISTER  		
#define MLC_ECC4_ERROR_ADDRESS_REGISTER_2					TCC7901_ECC_MECC4_EADDR2_REGISTER  		
#define MLC_ECC4_ERROR_ADDRESS_REGISTER_3					TCC7901_ECC_MECC4_EADDR3_REGISTER 
 		
#define MLC_ECC4_ERROR_DATA_REGISTER_0						TCC7901_ECC_MECC4_EDATA0_REGISTER  		
#define MLC_ECC4_ERROR_DATA_REGISTER_1						TCC7901_ECC_MECC4_EDATA1_REGISTER  		
#define MLC_ECC4_ERROR_DATA_REGISTER_2						TCC7901_ECC_MECC4_EDATA2_REGISTER  		
#define MLC_ECC4_ERROR_DATA_REGISTER_3						TCC7901_ECC_MECC4_EDATA3_REGISTER  		

#define NFC_WORD_DATA_REGISTER			TCC7901_NFC_WDATA_REGISTER

#define TRYTIME 	250

#define SCFG_ECC_MONITOR_BUS_SELECTION_REGISTER		*((volatile U32 *)(0xf005000c))
	#define SCFG_BIT_ECC_SEL_BUS_MASK				~(BIT16|BIT17)	
	#define SCFG_BIT_ECC_SEL_STORAGE_BUS			(0<<16)
	#define SCFG_BIT_ECC_SEL_IO_BUS					(1<<16)
	#define SCFG_BIT_ECC_SEL_PRUPOSE_SRAM_BUS		(2<<16)
	#define SCFG_BIT_ECC_SEL_MAIN_PORC_DATA_BUS		(3<<16)
/*-----------------------------------------------------------------------------
* ����:	hyhwDelay
* ����:	delay 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwDelay(void)
{
	U32 i = 50;
	while( i -- ); 

}
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_ecc4EncodeStart
* ����:	start the ecc4 encode 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_ecc4EncodeStart(void)					/*mlc ecc4*/
{
	ECC_CLEAR_REGISTER	= REGISTER_BIT_CLEAR;
	ECC_CONTROL_REGISTER |=	ECC_BIT_MLC4_ECC_ENCODING_ENABLE;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_ecc4EncodeStop
* ����:	stop the ecc4 encode 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
#define hyhwEcc_ecc4EncodeStop	ECC_CONTROL_REGISTER &=	(~ECC_BIT_MLC4_ECC_ENCODING_ENABLE)

/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_getEcc4
* ����:	get the ecc code from the ecc4 code register 
* ����:	ecc4_array
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_getEcc4( tECC4_ARRAY	* ptEcc4Array )
{
	ptEcc4Array->ecc4_0 = MLC_ECC4_CODE_REGISTER_0;
	ptEcc4Array->ecc4_1 = MLC_ECC4_CODE_REGISTER_1;
	ptEcc4Array->ecc4_2 = MLC_ECC4_CODE_REGISTER_2;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_setEcc4
* ����:	set the ecc code to the ecc4 code register 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
#define NFC_SINGLE_DATA_REGISTER			TCC7901_NFC_SDATA_REGISTER 
void hyhwEcc_setEcc4( void )
{
	tECC4_ARRAY	tEcc4Array;
	ECC_INT_REQ_REGISTER |= ECC_BIT_MLC_ECC4_DECODING_FLAG_MASK;		/*clear flag*/
		
		/*set ecc data*/												
	tEcc4Array.ecc4_0 |= NFC_SINGLE_DATA_REGISTER;
	tEcc4Array.ecc4_0 <<=8;
	tEcc4Array.ecc4_0 |= NFC_SINGLE_DATA_REGISTER;
		
	tEcc4Array.ecc4_1 |= NFC_SINGLE_DATA_REGISTER;
	tEcc4Array.ecc4_1 <<= 8;
	tEcc4Array.ecc4_1 |= NFC_SINGLE_DATA_REGISTER;
	tEcc4Array.ecc4_1 <<= 8;
	tEcc4Array.ecc4_1 |= NFC_SINGLE_DATA_REGISTER;
	tEcc4Array.ecc4_1 <<= 8;
	tEcc4Array.ecc4_1 |= NFC_SINGLE_DATA_REGISTER;
		
	tEcc4Array.ecc4_2 |= NFC_SINGLE_DATA_REGISTER;
	tEcc4Array.ecc4_2 <<= 8;
	tEcc4Array.ecc4_2 |= NFC_SINGLE_DATA_REGISTER;
	tEcc4Array.ecc4_2 <<= 8;
	tEcc4Array.ecc4_2 |= NFC_SINGLE_DATA_REGISTER;
	tEcc4Array.ecc4_2 <<= 8;
	tEcc4Array.ecc4_2 |= NFC_SINGLE_DATA_REGISTER;
	MLC_ECC4_CODE_REGISTER_0 = tEcc4Array.ecc4_0;
	MLC_ECC4_CODE_REGISTER_1 = tEcc4Array.ecc4_1;
	MLC_ECC4_CODE_REGISTER_2 = tEcc4Array.ecc4_2;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_ecc4DecodeStart
* ����:	start he ecc4 decode 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_ecc4DecodeStart(void)
{
	ECC_CLEAR_REGISTER	= REGISTER_BIT_CLEAR;
	ECC_CONTROL_REGISTER |=	ECC_BIT_MLC4_ECC_DECODING_ENABLE;//start for decode
}

/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_setEccBaseAddr
* ����:	set the ecc4 encode or decode base address 
* ����:	base,mask
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_setEccBaseAddr(U32 base, U32 mask )
{
	ECC_CLEAR_REGISTER	= REGISTER_BIT_CLEAR;
	ECC_BASE_ADDRESS_REGISTER	=	base<<BASE_ADDRESS_OFFSET;	
	ECC_ADDRESS_MASK_REGISTER	=	mask<<ADDRESS_MASK_OFFSET;	
	SCFG_ECC_MONITOR_BUS_SELECTION_REGISTER = SCFG_BIT_ECC_SEL_MAIN_PORC_DATA_BUS;				//ECC bus select	
	
}
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_mlcEcc4DecodingInterruptEnable
* ����:	set the ecc4 decode interrupt, & if the decode is complete,there is an interrupt be occure 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_mlcEcc4DecodingInterruptEnable(void)
{
	ECC_CONTROL_REGISTER	|=	ECC_BIT_MLC_ECC4_DECODING_INTERRUPT_ENABLE;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_mlcEcc4EncodingInterruptEnable
* ����:	set the ecc4 encode interrupt, & if the encode is complete,there is an interrupt be occure 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_mlcEcc4EncodingInterruptEnable(void)
{
	SCFG_ECC_MONITOR_BUS_SELECTION_REGISTER = SCFG_BIT_ECC_SEL_MAIN_PORC_DATA_BUS;
	ECC_INT_REQ_REGISTER |= ECC_BIT_MLC_ECC4_ENCODING_FLAG_MASK;			//clear the interrupt musk bit
	ECC_CONTROL_REGISTER |= ECC_BIT_MLC4_ECC_ENCODING_ENABLE;
	ECC_CLEAR_REGISTER	= REGISTER_BIT_CLEAR;								/* Clear ECC Block		*/
}
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_waitEncodingCompleted
* ����:	wait for encode complete 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
eECC_STATUS hyhwEcc_waitEncodingCompleted(void)
{	
	U32 time = TRYTIME;
//	tECC4_ARRAY tEcc4Array;
	while(time --)
	{	
		if( ECC_INT_REQ_REGISTER & ECC_BIT_MLC_ECC4_ENCODING_FLAG_MASK )
		{
			__asm
		    {
		       	mov r0, #0xf005b010	
		    	ldmia r0!, {r6-r8} 
		    	
		    	ldr r0,[0xf005b000]
		    	bic r0,r0,#7
				str r0,[0xf005b000]

		    	
		        str r6, [0xf00b0010] /* ����Ӳ����˵���൱��д��4��bytes */
		        str r7, [0xf00b0010]
		        str r8, [0xf00b0010]
				str r8, [0xf00b0010]

		    }
			
//			tEcc4Array.ecc4_0 = MLC_ECC4_CODE_REGISTER_0;
//			tEcc4Array.ecc4_1 = MLC_ECC4_CODE_REGISTER_1;
//			tEcc4Array.ecc4_2 = MLC_ECC4_CODE_REGISTER_2;
//			ECC_CONTROL_REGISTER	&=	~(BIT0|BIT1|BIT2);//	hyhwEcc_ecc4EncodeStop;
//			NFC_WORD_DATA_REGISTER	=	tEcc4Array.ecc4_0;
//			NFC_WORD_DATA_REGISTER	=	tEcc4Array.ecc4_1;	
//			NFC_WORD_DATA_REGISTER	=	tEcc4Array.ecc4_2;	
//			NFC_WORD_DATA_REGISTER	=	0;
			return	ecc_completed;
		}
		hyhwDelay();
	}
	
	return	ecc_uncompleted;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_dmaRequestEnable
* ����:	do ecc when data transfer use DMA 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/

#define hyhwEcc_dmaRequestEnable 	ECC_CONTROL_REGISTER|=ECC_BIT_ECC_DMA_REQUEST_ENABLE

/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_dmaRequestDisable
* ����:	disable ecc when data DMA transfer  complete 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/

#define hyhwEcc_dmaRequestDisable	ECC_CONTROL_REGISTER &=	(~ECC_BIT_ECC_DMA_REQUEST_ENABLE)

/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_dataSize
* ����:	set the size of data which be decode or encode 
* ����:	size
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_dataSize(U32 size)
{
	ECC_CONTROL_REGISTER &= ECC_BIT_ECC_DATA_SIZE_MASK;
	ECC_CONTROL_REGISTER |= ECC_BIT_ECC_DATA_SIZE(size);
}
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_waitDecodingCompleted
* ����:	waiting for decode complete 
* ����:	none
* ����:	apECC_status
*----------------------------------------------------------------------------*/

eECC_STATUS hyhwEcc_waitDecodingCompleted( void )
{
	U32 time = TRYTIME;
	
	ECC_CONTROL_REGISTER |= ECC_BIT_MLC_ECC4_DECODING_INTERRUPT_ENABLE;
	MLC_ECC4_CODE_REGISTER_0 = NFC_WORD_DATA_REGISTER;
	MLC_ECC4_CODE_REGISTER_1 = NFC_WORD_DATA_REGISTER;
	MLC_ECC4_CODE_REGISTER_2 = NFC_WORD_DATA_REGISTER;
	while(time --)
	{	
		if( ECC_INT_REQ_REGISTER & ECC_BIT_MLC_ECC4_DECODING_FLAG_MASK )
		{
			return	ecc_completed;
		}
		hyhwDelay();
	}
	/*���жϱ�־*/
//	ECC_INT_REQ_REGISTER = REGISTER_BIT_CLEAR;
	return	ecc_uncompleted;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_decodingInterruptReqOcc
* ����:	to indicate that is there an decoding interrupt  occure 
* ����:	none
* ����:	apECC_status
*----------------------------------------------------------------------------*/
eECC_STATUS hyhwEcc_decodingInterruptReqOcc( void )
{
	if( ECC_INT_REQ_REGISTER & ECC_BIT_MLC_ECC4_DECODING_INTERRUPT_REQ_MASK )
	{	
		return	ecc_completed;
	}
	else
	{
		return	ecc_uncompleted;
	}	
}

/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_readMlcEcc4ErrorCorrect
* ����:	to indicate that is it need error correct 
* ����:	* pdata
* ����:	eccdiff_t
*----------------------------------------------------------------------------*/
eECC4_DIFF hyhwEcc_readMlcEcc4ErrorCorrect( U8 * pdata )
{
	U32 errorNum,errorAddr,errorData,i;
	U8 * ptemp,data;
		
	errorNum = MLC_ECC4_ERROR_NUMBER_REGISTER;

	if( errorNum > 4 )
	{
		return  ecc_symbol_error_occurred_large_than_4;
	}
	
	for( i = 0; i < errorNum; i ++ )
	{
		
		errorAddr = *((volatile U32 *)((U32)&MLC_ECC4_ERROR_ADDRESS_REGISTER_0 + 4*i));
		errorData =	*((volatile U32 *)((U32)&MLC_ECC4_ERROR_DATA_REGISTER_0 + 4*i));
		//Modified by ICE on 2008.12.08 ,for some lage pagesize nandflash, pagesize = 4K,but we always use 512 ecc
		//ptemp = pdata+((gtnandInfo.pageSize/4)-errorAddr+7);
		ptemp = pdata+(512-errorAddr+7);
		data = *ptemp; 
		*ptemp = data^errorData;
	}
	
	
	return errorNum;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_rwNandMlcEcc4Set
* ����:	set the Ecc register before nand data read 
* ����:	nandRwPortAddr , eccDataSize
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_rwNandMlcEcc4Set( U32 nandRwPortAddr, U32 eccDataSize )
{
	SCFG_ECC_MONITOR_BUS_SELECTION_REGISTER = SCFG_BIT_ECC_SEL_MAIN_PORC_DATA_BUS;	//ECC bus select
	ECC_BASE_ADDRESS_REGISTER	=	nandRwPortAddr;
	ECC_ADDRESS_MASK_REGISTER   = 	REGISTER_BIT_CLEAR;
	ECC_CONTROL_REGISTER = ECC_BIT_ECC_DATA_SIZE( eccDataSize );
}
