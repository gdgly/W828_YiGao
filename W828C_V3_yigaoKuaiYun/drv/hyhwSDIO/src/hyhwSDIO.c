#include "hyErrors.h"

#include "hyhwTcc7901.h"
#include "hyhwSDIO.h"
#include "hyhwGpio.h"

#include "wmi.h"
#include "wifi_user.h"


#define BLKSIZE_BLOCK_SIZE_BIT(X)       ( (X & 0x8fff) )
#define BLKSIZE_SDMA_BUF_BIT(X)         ( (X & 0x7000) )

#define TMODE_MULT_BIT(X)               ( (X & BIT0)<<5  )
#define TMODE_RW_BIT(X)                 ( (X & BIT0)<<4  )
#define TMODE_ACMD12_EN_BIT(X)          ( (X & BIT0)<<2  )
#define TMODE_BLKCOUNT_EN_BIT(X)        ( (X & BIT0)<<1  )
#define TMODE_DMA_EN_BIT(X)             ( (X & BIT0) )

#define HC_BUS_WIDTH_BIT(X)             ( ( (X & BIT0)<<1 ) )

#define HC_DMA_SEL_BIT(X)   \
( ( (0 == X) ? (X & BIT0) : ( (1 == X) ? ((X & BIT0)<<3) : ((X & BIT0)<<4) ) ) )

#define CMD_CMD_NUM_BIT(X)              ( ( (X & 0x3f)<<8 ) )
#define CMD_RESP_TYPE_BIT(X)            ( ( X & (BIT1|BIT0) ) )
#define CMD_DATSEL_BIT(X)               ( ( (X & BIT0)<<5 ) )

#define DATA_PRESENT_SELECT BIT5


/*-------- �궨����SD Controller �� buffer ����д unsigned int ������ ------*/
#define mWriteDataToBuffer(X)       (BUFFER_DATA_PORT_REGISTER = X)

/*-------- �궨���SD Controller �� buffer ����� unsigned int ������ ------*/
#define mReadDataFromBuffer(X)      (X = BUFFER_DATA_PORT_REGISTER)


/*-------- �궨����SD Controller �� buffer ����д unsigned char ������ ------*/
#define mWriteReg8DataToBuffer(X)       (BUFFER_DATA_PORT_REGISTER_REG8 = X)

/*-------- �궨���SD Controller �� buffer ����� unsigned char ������ ------*/
#define mReadReg8DataFromBuffer(X)      (X = BUFFER_DATA_PORT_REGISTER_REG8)



/*----------------- �궨��ȴ� SD Controller �� buffer ��д ---------------*/
#define mBufferWrEnableWaiting(timeOut)  \
    while(0 == (PRESENT_STATE_REGISTER & STATE_BIT_BURRER_WR_ENABLE)  && (timeOut--))
    

/*----------------- �궨��ȴ� SD Controller �� buffer �ɶ� ---------------*/   
#define mBufferRdEnableWaiting(timeOut)  \
    while(0 == (PRESENT_STATE_REGISTER & STATE_BIT_BURRER_RD_ENABLE) && (timeOut--))


/***************************************************************************
* ����:			hyhwSDIO_SendCommand

* ����:        	ִ�з���Command�Ķ���

* ����:			cmdID  -- ���͵������
                commandArg -- д�� Argument �Ĵ���������
                eResponseType -- ��������ķ�������
                dataSelect -- �Ƿ�ռ�� data0 �����߷�������ı�־
                
* ����:        	HY_OK -- ��������ɹ�
                HY_ERROR  -- ��������ʧ��
****************************************************************************/
U32 hyhwSDIO_SendCommand(U16 cmdID, U32 commandArg, eResponseType respType,
                        Bool dataSelect)
{
	S32 timeout ;
	U32 intStatusReg;
    
    timeout = 100000;
	while( (0 != (PRESENT_STATE_REGISTER & STATE_BIT_CMDLINE_INUSE) )&&(--timeout) );
	if(0 == timeout)
	{
	    return HY_ERROR;
	}
	if(dataSelect)
	{
	    timeout = 100000;
	    while( (0 != (PRESENT_STATE_REGISTER & STATE_BIT_DATALINE_ACT)) && (--timeout) );
		if(0 == timeout)
		{
		    return HY_ERROR;
		}
	}
	ARGUMENT_REGISTER = commandArg;
	COMMAND_REGISTER  = CMD_INDEX(cmdID)|respType;

	#if 1
	//while( (0 == CHECK_COMMAND_COMPLETE)&&(0 == CHECK_COMMAND_TIMEOUT) );
	timeout = 100000;
	while( (0 == CHECK_COMMAND_COMPLETE)&&(0 == CHECK_COMMAND_TIMEOUT) && (--timeout));
	#else
	while(1)
	{
		intStatusReg = NORMAL_INTERRUPT_STATUS_REGISTER;
		if ((intStatusReg&NIS_BIT_COMMAND_COMPLETE) != 0 ||
			(intStatusReg&NIS_BIT_CMD_TIMEOUT_ERROR) != 0)
		{
			break;
		}
	}
	#endif

	if( 0 != CHECK_COMMAND_COMPLETE )
	{
	    NORMAL_INTERRUPT_STATUS_REGISTER |= NIS_BIT_COMMAND_COMPLETE;
	}
	else
	{
	    return HY_ERROR;
	}
    return HY_OK;
}

/***************************************************************************
* ����:			hyhwSDIO_SoftwareReset

* ����:        	Software Reset SD Controller

* ����:			��
                
* ����:        	��
****************************************************************************/
void hyhwSDIO_SoftwareReset(void)
{
    int i;
    SOFTWARE_RESET_TIMEOUT_REGISTER |= TIME_BIT_SOFTWARE_RESET_ALL;
    i = 700;    //�϶���������������� 2.67*5 us
    while(i--);
    while(SOFTWARE_RESET_TIMEOUT_REGISTER & TIME_BIT_SOFTWARE_RESET_ALL);
    return;
}

/***************************************************************************
* ����:			hyhwSDIO_SetBusasSdFunction

* ����:        	���������ó�SDģʽ

* ����:			��
                
* ����:        	��
****************************************************************************/
void hyhwSDIO_SetBusasSdFunction(void)
{
    //hyhwGpio_setSD2Function();
    hyhwGpio_setSD7Function();
}
/***************************************************************************
* ����:			hyhwSDIO_sdPortSelect

* ����:        	ѡ��SLOT0��SLOT1��Ӧ��SDIO��

* ����:			slot0��slot0
                
* ����:        	��
****************************************************************************/
void hyhwSDIO_sdPortSelect( ePortSelect slot0, ePortSelect slot1 )
{
	SD_PORT_CTRL_REGSITER = (SLOT0_PORT_SELECT(slot0)|SLOT1_PORT_SELECT(slot1));

}
/***************************************************************************
* ����:			ClockFrequencySelect

* ����:        	SDʱ��Ƶ��ѡ��

* ����:			ö���ͷ�Ƶ�ĳ��� ����MCLK��Ƶ
                               
* ����:        	��
****************************************************************************/
static void ClockFrequencySelect(tBaseClkDivisor freq)
{
    int i;
    
    CLOCK_CONTROL_REGISTER &= ~CLOCK_BIT_SD_CLOCK_ENABLE;
    CLOCK_CONTROL_REGISTER |= (freq | CLOCK_BIT_INTERNAL_CLOCK_ENABLE);
    i = 10000;
    while(i--);
    while( 0 == (CLOCK_CONTROL_REGISTER & CLOCK_BIT_INTERNAL_CLOCK_STABLE) );
    CLOCK_CONTROL_REGISTER |= CLOCK_BIT_SD_CLOCK_ENABLE;
}

void hyhwSDIO_ClockDisable(void)
{
	CLOCK_CONTROL_REGISTER &= ~CLOCK_BIT_SD_CLOCK_ENABLE;
}

/***************************************************************************
* ����:			WorkingVoltageSet

* ����:        	������ѹ���� �Ѿ����ó� 3.3V

* ����:			��
                               
* ����:        	��
****************************************************************************/
static void WorkingVoltageSet()
{
    POWER_HOST_CONTROL_REGISTER &= ~CONTROL_BIT_VOLTAGE_MASK;
    POWER_HOST_CONTROL_REGISTER |= CONTROL_BIT_VOLTAGE_30;
}

/***************************************************************************
* ����:			 DataTimeoutValueSet

* ����:        	 �������ʱʱ��

* ����:			 timeout -- ���ʱʱ�䡣��ΧΪ 0~14
				 
				 14 - TMCLK * 2^27
				 ------------------
				 ------------------
				 1 - TMCLK * 2^14
                 0 - TMCLK * 2^13
                                
* ����:        	��
****************************************************************************/
static void DataTimeoutValueSet(eTimeoutValue timeout)
{
    SOFTWARE_RESET_TIMEOUT_REGISTER &= ~TIME_BIT_TIMEOUT_VALUE_MASK;
    SOFTWARE_RESET_TIMEOUT_REGISTER |= TIMEOUT_VALUE(timeout);
}

/***************************************************************************
* ����:			InterruptStateEnable

* ����:        	ʹ�� normal interrupt 

* ����:			��
                               
* ����:        	��
****************************************************************************/
static void InterruptStateEnable()
{
    NORMAL_INTERRUPT_STATUS_ENABLE_REGISTER = ALL_INTERRUPT_ENABLE;
    NORMAL_INTERRUPT_SIGNAL_ENABLE_REGISTER = 0;
}

/***************************************************************************
* ����:			hyhwSDIO_Initialize

* ����:        	��ʼ�� host �˿�����

* ����:			freq -- SD controller ʱ��Ƶ��
                timeout -- ���ó�ʱʱ�䣬���÷�ΧΪ 0~14
                
                14 - TMCLK * 2^27
                ------------------
                ------------------
                1 - TMCLK * 2^14
                0 - TMCLK * 2^13
                
* ����:        	��
****************************************************************************/  
void hyhwSDIO_Initialize(tBaseClkDivisor freq, eTimeoutValue timeout)
{
    hyhwSDIO_SoftwareReset();
    ClockFrequencySelect(freq);
    WorkingVoltageSet();
    DataTimeoutValueSet(timeout);
    InterruptStateEnable();
}

/***************************************************************************
* ����:			hyhwSDIO_WorkingFrequenceSet

* ����:        	����SDIO������Ƶ��

* ����:			��
                               
* ����:        	��
                
****************************************************************************/
void hyhwSDIO_WorkingFrequenceSet(tBaseClkDivisor freq)
{
    U16 temp;
    temp = CLOCK_CONTROL_REGISTER;
    CLOCK_CONTROL_REGISTER = (temp &0xff)|(freq);
}

/***************************************************************************
* ����:			hyhwSDIO_ErrorInterruptDetect

* ����:        	����Ƿ��д����жϲ���

* ����:			��
                               
* ����:        	HY_OK -- �����������ж�
                HY_ERROR -- ���������ж�
****************************************************************************/
U32 hyhwSDIO_ErrorInterruptDetect( void )
{
    U32 testBit;
    testBit = (NORMAL_INTERRUPT_STATUS_REGISTER & NIS_ERROR_INTERRUPT_BIT_MASK);
    if( 0 != testBit)
    {
        return HY_ERROR;
    }
    else
    {
        return HY_OK;
    }
}

/***************************************************************************
* ����:			hyhwSDIO_DatCmdLineFreeDetect

* ����:        	���command line �� data line �Ƿ�Ϊfree

* ����:			��
                               
* ����:        	HY_OK -- command line �� data line��Ϊfree
                HY_ERROR -- command line �� data line����һ���߲�Ϊfree
****************************************************************************/
U32 hyhwSDIO_DatCmdLineFreeDetect()
{

	U32 i=1000;
	while(DAT_CMD_LINE_ACT)
	{
		i --;
		if(i == 0)
		{
			return HY_ERROR;
		}
	
	}
	return HY_OK;		
}

/***************************************************************************
* ����:			hyhwSDIO_CmdCompleteDetect

* ����:        	���command �Ƿ������

* ����:			��
                               
* ����:        	HY_OK -- ��command ������ɣ������Ѿ��յ�response
                HY_ERROR -- û��command ������ɡ�
****************************************************************************/  
U32 hyhwSDIO_CmdCompleteDetect( void )
{
	//while((0 == CHECK_COMMAND_COMPLETE)&&(0 == CHECK_COMMAND_TIMEOUT));
	U32 timeOut = 100000;
	while((0 == CHECK_COMMAND_COMPLETE)&&(0 == CHECK_COMMAND_TIMEOUT) && (timeOut--));
	
	if( 0 != CHECK_COMMAND_COMPLETE )
	{
	    NORMAL_INTERRUPT_STATUS_REGISTER |= NIS_BIT_COMMAND_COMPLETE;
	}
	else
	{
	    return HY_ERROR;
	}
	return HY_OK;   
}

/***************************************************************************
* ����:			hyhwSDIO_DatCompeletDetect

* ����:        	���data �Ƿ������

* ����:			��
                               
* ����:        	HY_OK -- ��data �������
                HY_ERROR -- û��data ������ɡ�
****************************************************************************/ 
U32 hyhwSDIO_DatCompeletDetect( void )
{
    //while( ( 0 == CHECK_TRANSFER_COMPLETE )&& ( 0 == CHECK_DATA_TIMEOUT ) );
    U32 timeOut=100000;
    while(1)
    {
    	U32 status;
    	status = NORMAL_INTERRUPT_STATUS_REGISTER;
    	if (status&(NIS_BIT_TRANSFER_COMPLETE|NIS_BIT_DAT_TIMEOUT_ERROR))
    	{
    		break;
    	}
    	timeOut--;
    	if (timeOut == 0)
    	{
    		return HY_ERROR;
    	}
    }
   
    if( 0 != CHECK_TRANSFER_COMPLETE )
    {
        NORMAL_INTERRUPT_STATUS_REGISTER |= NIS_BIT_TRANSFER_COMPLETE;
    }
    else
    {
        return HY_ERROR;
    }
    return HY_OK;
}

/***************************************************************************
* ����:			hyhwSDIO_ErrorCheck

* ����:        	�����command����������Ƿ��������

* ����:			��
                               
* ����:        	HY_OK -- ��command���������û�в�������
                HY_ERROR -- ��command����������д���
****************************************************************************/
U32 hyhwSDIO_ErrorCheck( void )
{
    if( 0 != (NORMAL_INTERRUPT_STATUS_REGISTER & NIS_ERROR_INTERRUPT_BIT_MASK) )
    {
        return HY_ERROR;
    }
    return HY_OK;
}


/***************************************************************************
* ����:			DataTransferInit

* ����:        	����������Ĳ����͸� host controller register

* ����:			pb -- ���ݲ�����ŵĽṹָ��

* ����:        	��
****************************************************************************/
void hyhwSDIO_DataTransferInit(tDataTransferParameter *pb)
{
	BLOCK_SIZE_REGISTER = ( BLKSIZE_BLOCK_SIZE_BIT(pb->BlockLength) |
	                        BLKSIZE_SDMA_BUF_BIT(pb->Dma_BufferSize) );
	BLOCK_COUNT_REGISTER   = pb->BlockCount;
	ARGUMENT_REGISTER	   = pb->Argument;

	TRANSFER_MODE_REGISTER = ( TMODE_MULT_BIT(pb->MultSelect) |
	                           TMODE_RW_BIT(pb->Read_Select) |
	                           TMODE_ACMD12_EN_BIT(pb->ACMD12_Enable) |
	                           TMODE_BLKCOUNT_EN_BIT(pb->BlockCount_Enable) |
	                           TMODE_DMA_EN_BIT(pb->DMA_Enable) );

	POWER_HOST_CONTROL_REGISTER  = ( HC_BUS_WIDTH_BIT(pb->Bus_Width) |
	                                 HC_DMA_SEL_BIT(pb->DmaOperationSelect) );                          
	                              
    COMMAND_REGISTER       = ( CMD_CMD_NUM_BIT(pb->commandNum) |
                               CMD_RESP_TYPE_BIT(pb->eResponse_Type) |
                               DATA_PRESENT_SELECT );
}

/***************************************************************************
* ����:			hyhwSDIO_DataRead

* ����:        	��SDIO controller�� buffer�ж�������

* ����:			dataLength  -- ׼�������ݵĳ���
                pReadBuffer -- ������ݵ�ָ��

* ����:        	��
****************************************************************************/
void hyhwSDIO_DataRead(U32 dataLength, U8 *pReadBuffer)
{
    U32 lengthPending, i;
    U32 *pRDBuf;
    if(1 == dataLength)
    {
        lengthPending = dataLength;
    }
    else
    {
        lengthPending = dataLength >> 2;
    }
    if(1 != dataLength)
    {
        pRDBuf = (U32 *)pReadBuffer;
    }
    
    for(i = 0; i < lengthPending; i++)
    {
        U32 j= 10;
        while(j--);
        j = 100000;
        mBufferRdEnableWaiting(j);
 
        if(1 == dataLength)
        {    
            mReadReg8DataFromBuffer( *pReadBuffer );
        }
        else
        {
            mReadDataFromBuffer( *pRDBuf++ );
        }
    }
}

/***************************************************************************
* ����:			hyhwSDIO_DataWrite

* ����:        	��SDIO controller�� buffer��д������

* ����:			dataLength  -- ׼��д���ݵĳ���
                pReadBuffer -- ������ݵ�ָ��

* ����:        	��
****************************************************************************/
void hyhwSDIO_DataWrite(U32 dataLength, U8 *pWriteBuffer)
{
    U32 lengthPending, i;
    U32 *pWRBuf;
    if(1 == dataLength)
    {
        lengthPending = dataLength;
    }
    else
    {
        lengthPending = dataLength >> 2;
    }
    if(1 != dataLength)
    {
        pWRBuf = (U32 *)pWriteBuffer;
    }
    
    for(i = 0; i < lengthPending; i++)
    {
        U32 j= 10;
        while(j--);
        j = 100000;
        mBufferWrEnableWaiting(j);
        
        if(1 == dataLength)
        {    
            mWriteReg8DataToBuffer( *pWriteBuffer );
        }
        else
        {
            mWriteDataToBuffer( *pWRBuf++ );
        }
    }
}

/***************************************************************************
* ����:			hyhwSDIO_DatCmdLineReset

* ����:        	reset data/cmd line

* ����:			��
                
* ����:        	��
****************************************************************************/
void hyhwSDIO_DatCmdLineReset( void )
{
    TCC7901_SDIO_TIME_REGISTER |= TIME_BIT_SOFTWARE_RESET_CMD;
    TCC7901_SDIO_TIME_REGISTER |= TIME_BIT_SOFTWARE_RESET_DAT;
    while(TCC7901_SDIO_TIME_REGISTER&TIME_BIT_SOFTWARE_RESET_CMD);
    while(TCC7901_SDIO_TIME_REGISTER&TIME_BIT_SOFTWARE_RESET_DAT);
}

/***************************************************************************
* ����:			hyhwSDIO_HwIntDetect

* ����:        	����Ƿ���SDIO�ж��źŲ���

* ����:			��
                
* ����:        	0 -- û���ж��źŲ���
                1 -- ���ж��źŲ���
****************************************************************************/
U8 hyhwSDIO_HwIntDetect( void )
{
    if(NORMAL_INTERRUPT_STATUS_LOW_REGISTER&NIS_BIT_CARD_INTERRUPT)
    {
        NORMAL_INTERRUPT_STATUS_ENABLE_REGISTER &= ~SIGEN_BIT_CARD_INTERRUPT_ENABLE;
        adapter.intReq = MV_INT_STATUS_INT_REQ;
        return 1;
    }
    else
    {
        return 0;
    }
}

void hyhwSDIO_mEnCardIntStatus( void ) 
{
	//if((adapter.intReq == MV_INT_STATUS_INT_REQ))//||(adapter.lastWorkType == workType_voip))
	{
    	NORMAL_INTERRUPT_STATUS_ENABLE_REGISTER |= SIGEN_BIT_CARD_INTERRUPT_ENABLE;
    	//adapter.intReq = MV_INT_STATUS_NONE;
    }
}

