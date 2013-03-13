#include "hyTypes.h"
#include "hyErrors.h"

#include "hyhwCkc.h"
#include "hyhwSDIO.h"
#include "hymwSDIO.h"

#include "wmi.h"
#include "wifi_user.h"
#include "wifi_api.h"

#define SDIO_BUS_WIDTH		Bus_4_BitWideh

#define SEND_OP_COND_TIMEOUT                 10000
#define SEND_VOLTAGE_TIMEOUT    50000
#define SetLow16BitZero(X)      ( X &= 0xffff0000 )

extern void hyhwSDIO_DataTransferInit(tDataTransferParameter *pb);

/*-----------------------------------------------------------------------------
* ����:	hymw_udelay
* ����:	delay function
* ����:	delay time 
* ����:	error flag
*------------------------------------------------------------------------------*/
void hymw_udelay( U32 delayTime)
{
	U32 clk;
	clk = (SysClkRegister.mCpuClk)*delayTime;
	clk /= 3;
	while( clk --);	   
} 
/***************************************************************************
* ����:        	hymwSDIO_SendCommand_C52_R5

* ����:        	����ֱ�Ӷ�дIO�˿����� CMD52 ���õ�command data0 �����ߴ���

* ����:        	RwDirection    -- 1: read  0:write
                functionNumber -- SDIO �� 0~7 �� 8 ��function ģ��
                RegAddress     -- SDIO���ϴ������ļĴ�����ַ���� 17 bit
                WriteData      -- Ҫд��� unsigned char �����ݡ�
                                  ��Ϊ�������ֵΪ 0

* ����:        	�����־
****************************************************************************/
U32 hymwSDIO_SendCommand_C52_R5(U32 RwDirection, U32 functionNumber, 
                               U32 RegAddress, U8 *pDataTemp)
{
	U32 Argument, status, response,WriteData;
	if( RwDirection == SDIO_READ )
	{
		WriteData = 0;		
	}
	else
	{
		WriteData = *pDataTemp;
	}
	
	Argument = ARGUMENT_RW_BIT(RwDirection)|ARGUMENT_FN_BIT(functionNumber)|
	           ARGUNENT_ADDR_BIT(RegAddress)|ARGUMENT_DATA_BIT(WriteData);
	if(CCCR_IO_ABORT == RegAddress)
	{
	    status = hyhwSDIO_SendCommand(IO_RW_DIRECT_CMD52_R5, Argument, 
		                             Response_r1b_r5b_abrt, 0);
	}
	else
	{
	    status = hyhwSDIO_SendCommand(IO_RW_DIRECT_CMD52_R5, Argument, 
		                             Response_r1_r5, 0);
	}
	if(status == HY_ERROR)
	{
	    return HY_ERROR;
	}
	WriteData = hyhwSDIO_mGetResponse();
	if(0 != (WriteData & SDIO_CMD52_RESPONSE_CHECK_BIT) )
	{
		 return HY_ERROR;
	}
	*pDataTemp = WriteData;
	
	return HY_OK;
}

/***************************************************************************
* ����:        	SdioReset

* ����:        	SDIO target reset

* ����:        	��                   

* ����:        	��
****************************************************************************/
void SdioReset()
{
    U8 dataTemp = IO_ABORT_RESET;
    hymwSDIO_SendCommand_C52_R5(SDIO_WRITE, FUNCTION_0, CCCR_IO_ABORT, &dataTemp);
}

/***************************************************************************
* ����:        	FunctionNumberGet

* ����:        	�õ� SDIO target �õ��� Function Number ��ΧΪ 1~7

* ����:        	��                  

* ����:        	Function Number ��
****************************************************************************/
U32 FunctionNumberGet(U32 OCR)
{
	OCR = (OCR>>28)&(BIT2|BIT1|BIT0);
	return OCR;
}

/***************************************************************************
* ����:        hymwSDIO_Initalize

* ����:        SDIO����ʼ��

* ����:        ��

* ����:        �����־
****************************************************************************/
U32 hymwSDIO_Initalize( void )
{
	U32 funcNum;
	U32 status = 0, sdio_rca, response, i;
	U8 dataTemp;
	//hyhwSDIO_SetBusasSdFunction();
	hyhwGpio_setSD1Function();
	hyhwSDIO_sdPortSelect( port0, port1 );
	
	
    hyhwSDIO_Initialize(Divisor_2, TmclkMult2Exp25);
	i = 1500000;
	while(i--);
	//CMD0
	status = hyhwSDIO_SendCommand(0, 0, Response_Nothing, 0);
    if(HY_ERROR == status)
    {
        return HY_ERROR;
    }
	status += hyhwSDIO_ErrorInterruptDetect();
	response = hyhwSDIO_mGetResponse();
	if( (i==SEND_OP_COND_TIMEOUT)||(0 != status) )
	{
		return HY_ERROR;
	}
	//CMD5
	for(i=0;i<SEND_OP_COND_TIMEOUT;i++)
	{
		status = hyhwSDIO_SendCommand(IO_SEND_OP_COND_CMD5_R4,0, Response_r3_r4, 0);
		    	/* ��ʼ��������õ�DAT�� */
        if(HY_ERROR == status)
        {
            return HY_ERROR;
        }
		status += hyhwSDIO_ErrorInterruptDetect();
		response = hyhwSDIO_mGetResponse();
		funcNum = FunctionNumberGet(response);
		if(funcNum > 0) break;
	}
	if( (i==SEND_OP_COND_TIMEOUT)||(0 != status) )
	{
		return HY_ERROR;
	}
	//CMD5 set 
	for(i=0;i<SEND_VOLTAGE_TIMEOUT;i++)
	{
		status = hyhwSDIO_SendCommand(IO_SEND_OP_COND_CMD5_R4, 0xfc0000, 
		                             Response_r3_r4,0);
		
		/*  response�п��Զ���SDIO֧�ֵ� total number of functions */
		response = hyhwSDIO_mGetResponse();
		if( 0 != (response & IO_READY) )
		{
		    break;		//test IORDY;
	    }
	}
	if( (i==SEND_VOLTAGE_TIMEOUT)||(0 != status) )
	{
		return HY_ERROR;
	}

#if 1
	status = hyhwSDIO_SendCommand(SEND_RELATIVE_ADDR_CMD3_R6, 0, 
	                              Response_r6_r7,0);
	status += hyhwSDIO_ErrorInterruptDetect();
	response = hyhwSDIO_mGetResponse();
	if( (response & SDIO_CMD3_RESPONSE_CHECK_BIT)||(0 != status) )
	{
		return HY_ERROR;
	}
	sdio_rca = SetLow16BitZero(response);
	
	hyhwSDIO_WorkingFrequenceSet(Divisor_1);
	
	status = hyhwSDIO_SendCommand(SELECT_DESELECT_CARD_CMD7_R1_R1b, sdio_rca, 
	                              Response_r1b_r5b, 1);		//go to CMD state
	status += hyhwSDIO_ErrorInterruptDetect();
	response = hyhwSDIO_mGetResponse(); 
	if( (response & SDIO_CMD7_RESPONSE_CHECK_BIT)||(0 != status) )
	{
		return HY_ERROR;
	}		
	dataTemp = 0;
	status = hymwSDIO_SendCommand_C52_R5(SDIO_READ, FUNCTION_0, 0, &dataTemp);
	        /*�õ�֧�ֵ� CCCR �汾*/
	dataTemp = Func_1_INT_ENABLE|INT_ENABLE_MASTER;
	status += hymwSDIO_SendCommand_C52_R5(SDIO_WRITE, 
	                                      FUNCTION_0,
										  CCCR_INTERRUPT_ENABLE,
										  &dataTemp); 
			/* ����Interrupt Enable x, ��Interrupt Enable Master, ���ж� */
#if 1
	if (SDIO_BUS_WIDTH == Bus_1_BitWideh)
		dataTemp = BUS_WIDTH_1_BIT;
	else	
	dataTemp = BUS_WIDTH_4_BIT;											 
	status += hymwSDIO_SendCommand_C52_R5(SDIO_WRITE,
										  FUNCTION_0,
										  CCCR_BUS_INTERFACE_CTRL,
										  &dataTemp);
            /* �������߿�� 4-bit mode */									  
#endif
	dataTemp = 0;										 
	status += hymwSDIO_SendCommand_C52_R5(SDIO_READ,		
										  FUNCTION_0,
										  CCCR_CARD_CAPABILITY,
										  &dataTemp);
			/* ��� target �Ƿ�֧�� 4-bit mode �µ��ж�*/						  

	if(dataTemp & SUPPORT_4_BIT_DAT_INT)						
	{
		dataTemp = ENABLE_4_BIT_DAT_INT;
		hymwSDIO_SendCommand_C52_R5(SDIO_WRITE, FUNCTION_0, CCCR_CARD_CAPABILITY, &dataTemp);
			/* ʹ���ж� */
	}
	dataTemp = 0;
	hymwSDIO_SendCommand_C52_R5(SDIO_READ, FUNCTION_0, CCCR_POWER_CTRL, &dataTemp);
	        /* ��� Power Control �Ĵ����Ƿ�֧��Master Power Control */
	if(dataTemp&SupportMasterPowerCtrl)					
	{
		dataTemp = EnableMasterPowerCtrl;
		status += hymwSDIO_SendCommand_C52_R5(SDIO_WRITE, FUNCTION_0, CCCR_POWER_CTRL, &dataTemp);
			/* ʹ�� Master Power Control */								  
	}
#if 0
	dataTemp = IO_DISABLE_FUNCTION_1;
	status += hymwSDIO_SendCommand_C52_R5(SDIO_WRITE,	FUNCTION_0,CCCR_IO_EN, &dataTemp);
			/* reset IO����1 (IO function1) ÿ�� reset ʱ���Ƚ�IOEx��0 */							  
	i = 5000;								  
	while(--i)
	{	
		dataTemp = 0;
		status += hymwSDIO_SendCommand_C52_R5(SDIO_READ, FUNCTION_0, CCCR_IO_READY, &dataTemp);
			/* �ȴ�IORx�� 0 */						  
		if(!(dataTemp & IO_FUNCTION_1_READY))
		{
			break;
		}
		if( (0 != status) || (0 == i) )
		{
		    return HY_ERROR;
		}
	}
	dataTemp = 0;
	hymwSDIO_SendCommand_C52_R5(SDIO_READ, FUNCTION_0, CCCR_IO_EN, &dataTemp);
	dataTemp = IO_ENABLE_FUNCTION_1;
	hymwSDIO_SendCommand_C52_R5(SDIO_WRITE, FUNCTION_0, CCCR_IO_EN, &dataTemp);
	dataTemp = 0;
	hymwSDIO_SendCommand_C52_R5(SDIO_READ, FUNCTION_0, CCCR_IO_EN, &dataTemp);
	
	    /* �� FBR ������ blockSize */
	dataTemp = 0x40;
	hymwSDIO_SendCommand_C52_R5(SDIO_WRITE, FUNCTION_0, FBR_BLOCK_SIZE_0, &dataTemp);   //modified by yangjian for mavell project!
	dataTemp = 0;									  
	hymwSDIO_SendCommand_C52_R5(SDIO_WRITE, FUNCTION_0, FBR_BLOCK_SIZE_1, &dataTemp);
	dataTemp = 0;
    hymwSDIO_SendCommand_C52_R5(SDIO_READ,  FUNCTION_0, FBR_BLOCK_SIZE_0, &dataTemp);
	dataTemp = 0;						  
	hymwSDIO_SendCommand_C52_R5(SDIO_READ,  FUNCTION_0, FBR_BLOCK_SIZE_1, &dataTemp);
#endif
	
			/* �� IOEx�� 1 */					  

	i=5000000;
	while(--i);
	/*������function0�Ĵ����󣬶�дWiFi�Ĵ���ǰ��Ҫ��1S��ʱ���Ա�֤�����ȶ���
	������Ϊ������function0�Ĵ�����WiFi��Ҫ1S���ȶ�ʱ�䡣*/									 
#endif
	
#if 0
	//test block
	{
		U32 hymwSdio_readBlock(U32 address, void *data, int length);
		U32 hymwSdio_writeBlock(U32 address, void *data, int length);

		char *testbuf_w = (char *)hsaSdramReusableMemoryPool();
		char *testbuf_r;
		testbuf_w += 1024*1024;
		testbuf_r = testbuf_w + 1024*1024;
		for (i=0; i<0x800; i++)
		{
			testbuf_w[i] = i;
		}
		status = hymwSdio_writeBlock(0x0001, testbuf_w, 0x800);//0x7c7);
		memset(testbuf_r, 0x00, 0x800);
		status = hymwSdio_readBlock(0x0001, testbuf_r, 0x800);//0x729);
		if (memcmp(testbuf_r, testbuf_w, 0x800) == 0)
		{
			memset(testbuf_r, 0x00, 0x800);
		}
	}
#endif
	return status;
}

U32 hymwSDIO_CMD53ReadWrite(U32 Address, U8 *pBuffer, U32 Length, 
                            tCMD53Request *Request)
{
	U32 status;
	U32 h,Argument,BlockCount,BlockLength, blockSize, byte_block_count;
	tDataTransferParameter DataTransfer;
	tDataTransferParameter *pParameter = &DataTransfer;

    /* ��� command line / data line �Ƿ�Ϊfree״̬ */
    do
    {
    	status = hyhwSDIO_DatCmdLineFreeDetect();
    } while(0);
    if(status != HY_OK)
    {
    	return HY_ERROR;
    }

    /* ������ 4-bit mode �¶�д������Ҫ�õ��ļĴ��� */
    blockSize = 4;

    if( (CMD53_BLOCK_BASIS == Request->dmode) )
    {
        BlockCount = (Length)>>5;  // BlockCount/blockSize
        blockSize = 32;
        BlockLength = blockSize;
        byte_block_count = BlockCount;
    }
    else
    {
        BlockCount = 1;
        BlockLength = Length;
        byte_block_count = BlockLength;
    }
    
    #define FunctionNumber1     1
    Argument = ( ARGUMENT_RW_BIT(Request->direction) | 
                 ARGUMENT_FN_BIT(FunctionNumber1)   |
                 ARGUMENT_DMODE_BIT(Request->dmode) |
                 ARGUMENT_AMODE_BIT(Request->amode) | 
                 ARGUNENT_ADDR_BIT(Address) | 
                 ARGUMENT_DATA_BIT(byte_block_count) );
    #undef FunctionNumber1
    if(1 == BlockCount)
    {
        DataTransfer.MultSelect		= Single;
        DataTransfer.BlockCount_Enable	= BlockCountDisable;
    }
    else
    {
        DataTransfer.MultSelect		= Multiple;
        DataTransfer.BlockCount_Enable	= BlockCountEnable;
    }
    
    DataTransfer.ACMD12_Enable		= acmd12Disable;
    DataTransfer.commandNum			=IO_RW_EXTENDED_CMD53_R5;
    DataTransfer.BlockLength		=BlockLength;
    DataTransfer.DmaOperationSelect	=SDMA;
    DataTransfer.Dma_BufferSize		=0;
    DataTransfer.BlockCount			=BlockCount;
    DataTransfer.Argument			=Argument;
    if(Request->direction)
    {	
    	DataTransfer.Read_Select	=DataWrite;
    }
    else
    {
    	DataTransfer.Read_Select	=DataRead;
    }
    DataTransfer.DMA_Enable			=DmaDisable;
    DataTransfer.Bus_Width			=SDIO_BUS_WIDTH;
    DataTransfer.eResponse_Type		=Response_r1_r5;  
	
    /* �����ò������ݸ��ײ� SDIO controller */
	hyhwSDIO_DataTransferInit(pParameter);
	status = hyhwSDIO_CmdCompleteDetect();
    if( 0 != status )
	{
		return HY_ERROR;
	}	
	status = hyhwSDIO_ErrorCheck();
    if( 0 != status )
	{
		return HY_ERROR;
	}

    /* �������ݵĶ�д */
	for(h=0;h<BlockCount;h++)
	{
    	if(Request->direction)
    	{
    	    hyhwSDIO_DataWrite(BlockLength, pBuffer);
    	    pBuffer += BlockLength;
    	}
    	else
    	{
    	    hyhwSDIO_DataRead(BlockLength, pBuffer);
    	    pBuffer += BlockLength;
    	}
	}

	status += hyhwSDIO_DatCompeletDetect();
	status += hyhwSDIO_ErrorInterruptDetect();
	if( status!=0 )
	{
		return HY_ERROR;
	}
	status += hyhwSDIO_DatCmdLineFreeDetect();
	return (status);
}

/*-----------------------------------------------------------------------------
* ����:	hymwSdio_readIoReg
* ����:	sido read reg 
* ����:	func:	function num
		reg	:	register	
		pData:	the value of register 
* ����:	error flag
*------------------------------------------------------------------------------*/
int hymwSdio_readIoReg( U8 func, U32 reg, U8 * pDataTemp)
{
	if( hymwSDIO_SendCommand_C52_R5(SDIO_READ, func, reg, pDataTemp))
	{
		return HY_ERROR;
	}
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hymwSdio_writeIoReg
* ����:	sido read reg 
* ����:	func:	function num
		reg	:	register	
		dat:	the value of register 
* ����:	error flag
*------------------------------------------------------------------------------*/
U32 hymwSdio_writeIoReg( U8 func, U32 reg,  U8 dat)
{
	if( hymwSDIO_SendCommand_C52_R5(SDIO_WRITE, func, reg, &dat))
	{
		return HY_ERROR;
	}
	return HY_OK;
}


#define SDIO_BLOCK_SIZE		(64)

U32 hymwSdio_readBlock(U32 address, void *data, int length)
{
	U32 i,j,Argument,BlockCount,BlockLength, blockSize, byte_block_count;
	U32 bytes, tempData, timeOut;
	U8 *pBuff;

	if(hyhwSDIO_DatCmdLineFreeDetect() )
	{
		adapter.mvWlanStatus = WLAN_STATUS_NOT_CONNECT;
		return HY_ERROR;
	}
	
	BlockCount = length / SDIO_BLOCK_SIZE;
	bytes  = length % SDIO_BLOCK_SIZE;
    BlockLength = SDIO_BLOCK_SIZE;
    
#if 0	//unifi ����Ҫ����2k bytes����д��Ŀǰ�����ϲ㲹��2k bytes
    if (bytes > 0)
    {
    	BlockCount++;
    	bytes = 0;
    }
#endif
    byte_block_count = BlockCount;
    
    Argument = ( ARGUMENT_RW_BIT(CMD53_READ) | 
                 ARGUMENT_FN_BIT(1)   |
                 ARGUMENT_DMODE_BIT(CMD53_BLOCK_BASIS) |
                 ARGUMENT_AMODE_BIT(CMD53_FIXED_ADDRESS) | 
                 //ARGUNENT_ADDR_BIT(SDIO_IO_PORT) | 
                 ARGUNENT_ADDR_BIT(address) | 
                 ARGUMENT_DATA_BIT(byte_block_count) );
	

	BLOCK_SIZE_REGISTER = ( BLKSIZE_BLOCK_SIZE_BIT(BlockLength) |
	                        BLKSIZE_SDMA_BUF_BIT(0) );
	BLOCK_COUNT_REGISTER   = BlockCount;
	ARGUMENT_REGISTER	   = Argument;

	TRANSFER_MODE_REGISTER = ( TMODE_MULT_BIT(Multiple) |
	                           TMODE_RW_BIT(!CMD53_READ) |
	                           TMODE_ACMD12_EN_BIT(acmd12Disable) |
	                           TMODE_BLKCOUNT_EN_BIT(1) |
	                           TMODE_DMA_EN_BIT(DmaDisable) );

	POWER_HOST_CONTROL_REGISTER  = ( HC_BUS_WIDTH_BIT(SDIO_BUS_WIDTH) |
	                                 HC_DMA_SEL_BIT(SDMA) );                          
	                              
    COMMAND_REGISTER       = ( CMD_CMD_NUM_BIT(IO_RW_EXTENDED_CMD53_R5) |
                               CMD_RESP_TYPE_BIT(Response_r1_r5) |
                               DATA_PRESENT_SELECT );

    if( hyhwSDIO_CmdCompleteDetect() )
	{
		return HY_ERROR;
	}	

    if( hyhwSDIO_ErrorCheck() )
	{
		return HY_ERROR;
	}

    /* �������ݵĶ� */
    pBuff = (U8 *)data;
	for(i=0;i<BlockCount;i++)
	{
	    //��32bit��
	    for( j = 0; j < SDIO_BLOCK_SIZE/4; j ++ )
		{
			timeOut = 10;
			while(timeOut --);
			timeOut = 10000;
			while(0 == (PRESENT_STATE_REGISTER & STATE_BIT_BURRER_RD_ENABLE) && (timeOut--));
			tempData = BUFFER_DATA_PORT_REGISTER;
			memcpy(pBuff, &tempData, 4);//��ֹbuf�����뵼�����ݽ��մ���
	    	pBuff += 4;
	    }
	}
	if( hyhwSDIO_DatCompeletDetect() )
	{
		return HY_ERROR;
	}

	if( hyhwSDIO_ErrorInterruptDetect() )
	{
		return HY_ERROR;
	}
	
	if(hyhwSDIO_DatCmdLineFreeDetect() )
	{
		adapter.mvWlanStatus = WLAN_STATUS_NOT_CONNECT;
		return HY_ERROR;
	}

#if 0
	/* ��ȡʣ���ֽ� */
	if (bytes > 0)
	{
	    //���ֽڶ�ȡ
	    byte_block_count = bytes;
	    Argument = ( ARGUMENT_RW_BIT(CMD53_READ) | 
	                 ARGUMENT_FN_BIT(1)   |
	                 ARGUMENT_DMODE_BIT(CMD53_BYTE_BASIS) |
	                 ARGUMENT_AMODE_BIT(CMD53_FIXED_ADDRESS) | 
	                 //ARGUNENT_ADDR_BIT(SDIO_IO_PORT) | 
	                 ARGUNENT_ADDR_BIT(address) | 
	                 ARGUMENT_DATA_BIT(byte_block_count) );
		

		BLOCK_SIZE_REGISTER = ( BLKSIZE_BLOCK_SIZE_BIT(bytes) |
		                        BLKSIZE_SDMA_BUF_BIT(0) );
		BLOCK_COUNT_REGISTER   = 1;
		ARGUMENT_REGISTER	   = Argument;
		
		TRANSFER_MODE_REGISTER = ( TMODE_MULT_BIT(Single) |
		                           TMODE_RW_BIT(!CMD53_READ) |
		                           TMODE_ACMD12_EN_BIT(acmd12Disable) |
		                           TMODE_BLKCOUNT_EN_BIT(1) |
		                           TMODE_DMA_EN_BIT(DmaDisable) );

		POWER_HOST_CONTROL_REGISTER  = ( HC_BUS_WIDTH_BIT(SDIO_BUS_WIDTH) |
		                                 HC_DMA_SEL_BIT(SDMA) );                          
		                              
	    COMMAND_REGISTER       = ( CMD_CMD_NUM_BIT(IO_RW_EXTENDED_CMD53_R5) |
	                               CMD_RESP_TYPE_BIT(Response_r1_r5) |
	                               DATA_PRESENT_SELECT );
			
	    if( hyhwSDIO_CmdCompleteDetect() )
		{
			return HY_ERROR;
		}	

	    if( hyhwSDIO_ErrorCheck() )
		{
			return HY_ERROR;
		}
		
	    for (j=0; j<bytes; j++)
		{
			timeOut = 10000;
			while(0 == (PRESENT_STATE_REGISTER & STATE_BIT_BURRER_RD_ENABLE) && (timeOut--));
			*pBuff = BUFFER_DATA_PORT_REGISTER_REG8;
	    	pBuff++;
	    }

		if( hyhwSDIO_DatCompeletDetect() )
		{
			return HY_ERROR;
		}
		if( hyhwSDIO_ErrorInterruptDetect() )
		{
			return HY_ERROR;
		}
		if(hyhwSDIO_DatCmdLineFreeDetect() )
		{
			adapter.mvWlanStatus = WLAN_STATUS_NOT_CONNECT;
			return HY_ERROR;
		}
	}//if (bytes > 0)
#endif
	
	return HY_OK;
}

U32 hymwSdio_writeBlock(U32 address, void *data, int length)
{
	U32 i,j,Argument,BlockCount,BlockLength, blockSize, byte_block_count;
	U32 bytes, tempData, timeOut;
	U8 *pBuff;

	if(hyhwSDIO_DatCmdLineFreeDetect() )
	{
		adapter.mvWlanStatus = WLAN_STATUS_NOT_CONNECT;
	//	hy_ap_updata_ap_list(AP_LIST_REFRESH);//090217
		//wifi_ChkLink_Set_Autodisconnect();
		return HY_ERROR;
	}
	
	BlockCount = length / SDIO_BLOCK_SIZE;
	bytes  = length % SDIO_BLOCK_SIZE;
    BlockLength = SDIO_BLOCK_SIZE;
    
#if 0	//unifi ����Ҫ����2k bytes����д��Ŀǰ�����ϲ㲹��2k bytes
    if (bytes > 0)
    {
    	BlockCount++;
    	bytes = 0;
    }
#endif
    byte_block_count = BlockCount;
    
    
    Argument = ( ARGUMENT_RW_BIT(CMD53_WRITE) | 
                 ARGUMENT_FN_BIT(1)   |
                 ARGUMENT_DMODE_BIT(CMD53_BLOCK_BASIS) |
                 ARGUMENT_AMODE_BIT(CMD53_FIXED_ADDRESS) | 
                 //ARGUNENT_ADDR_BIT(SDIO_IO_PORT) | 
                 ARGUNENT_ADDR_BIT(address) | 
                 ARGUMENT_DATA_BIT(byte_block_count) );
	

	BLOCK_SIZE_REGISTER = ( BLKSIZE_BLOCK_SIZE_BIT(BlockLength) |
	                        BLKSIZE_SDMA_BUF_BIT(0) );
	BLOCK_COUNT_REGISTER   = BlockCount;
	ARGUMENT_REGISTER	   = Argument;

	TRANSFER_MODE_REGISTER = ( TMODE_MULT_BIT(Multiple) |
	                           TMODE_RW_BIT(!CMD53_WRITE) |
	                           TMODE_ACMD12_EN_BIT(acmd12Disable) |
	                           TMODE_BLKCOUNT_EN_BIT(1) |
	                           TMODE_DMA_EN_BIT(DmaDisable) );

	POWER_HOST_CONTROL_REGISTER  = ( HC_BUS_WIDTH_BIT(SDIO_BUS_WIDTH) |
	                                 HC_DMA_SEL_BIT(SDMA) );                          
	                              
    COMMAND_REGISTER       = ( CMD_CMD_NUM_BIT(IO_RW_EXTENDED_CMD53_R5) |
                               CMD_RESP_TYPE_BIT(Response_r1_r5) |
                               DATA_PRESENT_SELECT );

    if( hyhwSDIO_CmdCompleteDetect() )
	{
		return HY_ERROR;
	}

    if( hyhwSDIO_ErrorCheck() )
	{
		return HY_ERROR;
	}

    /* �������ݵ�д */
    pBuff = (U8 *)data;
	for(i=0;i<BlockCount;i++)
	{
	    //��32bitд
		for( j = 0; j < SDIO_BLOCK_SIZE/4; j ++ )
		{
			timeOut = 20;
			while(timeOut --);
			timeOut = 10000;
			while(0 == (PRESENT_STATE_REGISTER & STATE_BIT_BURRER_WR_ENABLE) && (timeOut--));
			memcpy(&tempData, pBuff, 4);//��ֹbuf�����뵼�����ݷ��ʹ���
			BUFFER_DATA_PORT_REGISTER = tempData;
	    	pBuff	+= 4;
	    }
	}

	if( hyhwSDIO_DatCompeletDetect() )
	{
		return HY_ERROR;
	}

	if( hyhwSDIO_ErrorInterruptDetect() )
	{
		return HY_ERROR;
	}
	
	if(hyhwSDIO_DatCmdLineFreeDetect() )
	{
		adapter.mvWlanStatus = WLAN_STATUS_NOT_CONNECT;
		return HY_ERROR;
	}
	
#if 0
	/* д��ʣ���ֽ� */
	if (bytes > 0)
	{
	    //���ֽ�д��
	    byte_block_count = bytes;
	    Argument = ( ARGUMENT_RW_BIT(CMD53_WRITE) | 
	                 ARGUMENT_FN_BIT(1)   |
	                 ARGUMENT_DMODE_BIT(CMD53_BYTE_BASIS) |
	                 ARGUMENT_AMODE_BIT(CMD53_FIXED_ADDRESS) | 
	                 //ARGUNENT_ADDR_BIT(SDIO_IO_PORT) | 
	                 ARGUNENT_ADDR_BIT(address) | 
	                 ARGUMENT_DATA_BIT(byte_block_count) );
		

		BLOCK_SIZE_REGISTER = ( BLKSIZE_BLOCK_SIZE_BIT(bytes) |
		                        BLKSIZE_SDMA_BUF_BIT(0) );
		BLOCK_COUNT_REGISTER   = 1;
		ARGUMENT_REGISTER	   = Argument;

		TRANSFER_MODE_REGISTER = ( TMODE_MULT_BIT(Single) |
		                           TMODE_RW_BIT(!CMD53_WRITE) |
		                           TMODE_ACMD12_EN_BIT(acmd12Disable) |
		                           TMODE_BLKCOUNT_EN_BIT(1) |
		                           TMODE_DMA_EN_BIT(DmaDisable) );

		POWER_HOST_CONTROL_REGISTER  = ( HC_BUS_WIDTH_BIT(SDIO_BUS_WIDTH) |
		                                 HC_DMA_SEL_BIT(SDMA) );                          
		
	    COMMAND_REGISTER       = ( CMD_CMD_NUM_BIT(IO_RW_EXTENDED_CMD53_R5) |
	                               CMD_RESP_TYPE_BIT(Response_r1_r5) |
	                               DATA_PRESENT_SELECT );

	    if( hyhwSDIO_CmdCompleteDetect() )
		{
			return HY_ERROR;
		}

	    if( hyhwSDIO_ErrorCheck() )
		{
			return HY_ERROR;
		}
	    for (j=0; j<bytes; j++)
		{
			timeOut = 10000;
			while(0 == (PRESENT_STATE_REGISTER & STATE_BIT_BURRER_WR_ENABLE) && (timeOut--));
			BUFFER_DATA_PORT_REGISTER_REG8 = *pBuff;
	    	pBuff++;
	    }

		if( hyhwSDIO_DatCompeletDetect() )
		{
			return HY_ERROR;
		}
		if( hyhwSDIO_ErrorInterruptDetect() )
		{
			return HY_ERROR;
		}
		if(hyhwSDIO_DatCmdLineFreeDetect() )
		{
			adapter.mvWlanStatus = WLAN_STATUS_NOT_CONNECT;
			return HY_ERROR;
		}
	}//if (bytes > 0)
#endif

	return HY_OK;
	
}


