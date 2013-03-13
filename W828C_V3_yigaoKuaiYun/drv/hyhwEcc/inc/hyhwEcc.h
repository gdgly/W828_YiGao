/*******************************************************************************
*  (C) Copyright 2005 Shanghai Hyctron Electronic Design House, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of Shanghai Hyctron Electronic Design House and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  Hyctron and is confidential in nature.  Its use and dissemination by    
*  any party other than Hyctron is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/
#ifndef ECC_DRV_H
#define ECC_DRV_H

typedef enum {
	ecc_no_error,		/* no error */
	ecc_1_symbol_error_occurred,
	ecc_2_symbol_error_occurred,
	ecc_3_symbol_error_occurred,
	ecc_4_symbol_error_occurred,
	ecc_symbol_error_occurred_large_than_4	/* uncorrectable error */
} eECC4_DIFF;

typedef enum 
{
    ecc_completed,
    ecc_uncompleted                      
} eECC_STATUS;

typedef volatile struct 
{
	U32 ecc4_0;		//0:15
	U32 ecc4_1;		//16:47
	U32 ecc4_2;		//48:79	

}tECC4_ARRAY;

/*-----------------------------------------------------------------------------
* ����:	hyhwDelay
* ����:	delay 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwDelay(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc4EnCode_Start
* ����:	start the ecc4 encode 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_ecc4EncodeStart(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc4EnCode_Stop
* ����:	stop the ecc4 encode 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_ecc4EncodeStop(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_Get
* ����:	get the ecc code from the ecc4 code register 
* ����:	ecc4_array
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_getEcc4( tECC4_ARRAY * ptEcc4Arry );
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc4_Set
* ����:	set the ecc code to the ecc4 code register 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_setEcc4( void );
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc4_Decode_Start
* ����:	start he ecc4 decode 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_ecc4DecodeStart(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc4_Decode_Stop
* ����:	stop he ecc4 decode 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_ecc4DecodeStop(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwSetEccBaseAddr
* ����:	set the ecc4 encode or decode base address 
* ����:	base,mask
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_setEccBaseAddr(U32 base, U32 mask );
/*-----------------------------------------------------------------------------
* ����:	hyhwMlcEcc4DecodingInterruptEnable
* ����:	set the ecc4 decode interrupt, & if the decode is complete,there is an interrupt be occure 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_mlcEcc4DecodingInterruptEnable(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwMlcEcc4EncodingInterruptEnable
* ����:	set the ecc4 encode interrupt, & if the encode is complete,there is an interrupt be occure 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_mlcEcc4EncodingInterruptEnable(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwEccWaitEncodingCompleted
* ����:	wait for encode complete 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
eECC_STATUS hyhwEcc_waitEncodingCompleted(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwEccDmaRequestEnable
* ����:	do ecc when data transfer use DMA 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_dmaRequestEnable(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwEccDmaRequestDisable
* ����:	disable ecc when data DMA transfer  complete 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_dmaRequestDisable(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwEccDataSize
* ����:	set the size of data which be decode or encode 
* ����:	size
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_dataSize(U32 size);
/*-----------------------------------------------------------------------------
* ����:	hyhwEccWaitDecodingCompleted
* ����:	waiting for decode complete 
* ����:	none
* ����:	apECC_status
*----------------------------------------------------------------------------*/
eECC_STATUS hyhwEcc_waitDecodingCompleted( void );
/*-----------------------------------------------------------------------------
* ����:	hyhwEccDecodingInterruptReqOcc
* ����:	to indicate that is there an decoding interrupt  occure 
* ����:	none
* ����:	apECC_status
*----------------------------------------------------------------------------*/
eECC_STATUS hyhwEcc_decodingInterruptReqOcc( void );
/*-----------------------------------------------------------------------------
* ����:	hyhwReadMlcEcc4ErrorCorrect
* ����:	to indicate that is it need error correct 
* ����:	* pdata
* ����:	eccdiff_t
*----------------------------------------------------------------------------*/
eECC4_DIFF hyhwEcc_readMlcEcc4ErrorCorrect( U8 * pdata );
/*-----------------------------------------------------------------------------
* ����:	hyhwEcc_rwNandMlcEcc4Set
* ����:	set the Ecc register before nand data read 
* ����:	nandRwPortAddr , eccDataSize
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwEcc_rwNandMlcEcc4Set( U32 nandRwPortAddr, U32 eccDataSize );

#endif //ECC_DRV_H