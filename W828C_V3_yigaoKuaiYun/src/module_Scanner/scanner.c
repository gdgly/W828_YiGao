/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"
/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "hyhwIntCtrl.h"

#include "AbstractFileSystem.h"
#include "LcdAPIFunc.h"
#include "glbVariable_base.h"

#include "scanner.h"


static U8 gu8ScannerType = SCANNER_TYPE_SYMBOL;

extern const U8 suffixCmd[][9];
extern const U8 Enable_ack[][9];
extern const U8 Disable_ack[][9];

void scannerUart_intIsr(void);
static void scanner_checkType(void);
static void scanner_set(void);


/*-----------------------------------------------------------------------------
* ����:	scanner_init
* ����:	��ʼ��ɨ��ͷ�ĵ�Դ���ƶ˿�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void scanner_init(void)
{
	hyhwScanner_powerCtrl(EnableOn);
	syssleep(1);
	
	hyhwCpm_busClkEnable(CKC_UART3);
	hyhwCpm_clearSoftReset(CKC_UART3);
	hyhwUart_Init(Uart_Baudrate_9600, Uart_DataBits_8, Uart_StopBits_1, 
					Uart_Parity_Disable, Uart_Fifo_Mode, SCANNER_UART,
					SCANNER_UART_PORT, FLOW_UNSUPPORT, DMA_UNSUPPORT);
	hyhwUart_rxIntDisable(SCANNER_UART);
	hyhwInt_disable(INT_BIT_UART);
	hyhwInt_ConfigUart_ISR(SCANNER_UART, scannerUart_intIsr);
	hyhwUart_rxIntEnable(SCANNER_UART);
	hyhwInt_setIRQ(INT_BIT_UART);
	hyhwInt_enable(INT_BIT_UART);
	
	barBuf_init();
	hyhwScanner_TrigInit();
	scanner_checkType();
	
	scanner_set();
}

/*-----------------------------------------------------------------------------
* ����:	scanner_reinit
* ����:	���³�ʼ��ɨ��ͷ�ĵ�Դ���ƶ˿�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void scanner_reinit(void)
{
	if (gu8ScannerType == SCANNER_TYPE_SYMBOL)
	{
		/* symbol ɨ��ͷ����*/
		hyhwScanner_powerCtrl(EnableOn);
		
		hyhwUart_Init(Uart_Baudrate_9600, Uart_DataBits_8, Uart_StopBits_1, 
						Uart_Parity_Disable, Uart_Fifo_Mode, SCANNER_UART,
						SCANNER_UART_PORT, FLOW_UNSUPPORT, DMA_UNSUPPORT);
		hyhwUart_rxIntDisable(SCANNER_UART);
		hyhwInt_disable(INT_BIT_UART);
		hyhwInt_ConfigUart_ISR(SCANNER_UART, scannerUart_intIsr);
		hyhwUart_rxIntEnable(SCANNER_UART);
		hyhwInt_setIRQ(INT_BIT_UART);
		hyhwInt_enable(INT_BIT_UART);
	}
	else //if (gu8ScannerType == SCANNER_TYPE_OPTION)
	{
		#if 0
		/* opticon ɨ��ͷ��102/102���룬����*/
		hyhwApi_stm102Wake();
		#else
		/* opticon ɨ��ͷ��7901���룬����*/
		#endif
	}
	
	barBuf_init();
	hyhwScanner_TrigInit();
}

/*-----------------------------------------------------------------------------
* ����:	scanner_deinit
* ����:	����ʼ��ɨ��ͷ���ر�ɨ��ͷ
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void scanner_deinit(void)
{
	hyhwScanner_TrigDisable();
	
	if (gu8ScannerType == SCANNER_TYPE_SYMBOL)
	{
		hyhwUart_rxIntDisable(SCANNER_UART);
		hyhwCpm_busClkDisable(CKC_UART3);
		hyhwInt_ConfigUart_ISR(SCANNER_UART, NULL);
		
		hyhwGpio_setAsGpio(SCANNER_UART_TX_PORT, SCANNER_UART_TX_GPIO);
		hyhwGpio_setIn(SCANNER_UART_TX_PORT, SCANNER_UART_TX_GPIO);
		hyhwGpio_setAsGpio(SCANNER_UART_RX_PORT, SCANNER_UART_RX_GPIO);
		hyhwGpio_setIn(SCANNER_UART_RX_PORT, SCANNER_UART_RX_GPIO);
		
		/* CTS �øߣ�����SE-955�������ݣ��ر�ʱ����Ϊ���븡��*/
		hyhwGpio_setAsGpio(SCANNER_UART_CTS_PORT, SCANNER_UART_CTS_GPIO);
		hyhwGpio_setIn(SCANNER_UART_CTS_PORT, SCANNER_UART_CTS_GPIO);
		hyhwGpio_pullDisable(SCANNER_UART_CTS_PORT, SCANNER_UART_CTS_GPIO);
	}
	else
	{
		#if 0
		hyhwApi_stm102ResetOn();
		hyhwApi_stm102HwInterfaceDeinit();
		#else
		#endif
	}
	hyhwScanner_powerCtrl(EnableOff);
}

/*-----------------------------------------------------------------------------
* ����:	scanner_checkType
* ����:	���ɨ��ͷ����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
static void scanner_checkType(void)
{
	int ret;
	scannerUart_sendwkup();
	syssleep(10);
	
	/* symbol�������� */
	ret = scannerUart_sendCmd(Enable_ack[0]);
	if (ret == HY_OK)
	{
		/* symbol����������Ӧ�ɹ� */
		gu8ScannerType = SCANNER_TYPE_SYMBOL;
	}
	else
	{
		/* symbol����������Ӧʧ�� */
		gu8ScannerType = SCANNER_TYPE_OPTICON;
	}
}

/*-----------------------------------------------------------------------------
* ����:	scanner_getType
* ����:	��ȡɨ��ͷ����
* ����:	none
* ����:	ɨ��ͷ����
*		0---symbol scanner(Ѷ��ɨ��ͷ)
*		1---option scanner(ŷ��ɨ��ͷ)
*----------------------------------------------------------------------------*/
int scanner_getType(void)
{
	return gu8ScannerType;
}

#if 0
#define IMAGE_SCANNER_FILE		("C:/image102.hyc")
#define UPDATE_SCANNER_STR		("Updating scanner...")
/*-----------------------------------------------------------------------------
* ����:	scanner_update_init_102
* ����:	��������ʼ��102����(ŷ��ɨ��ͷ����102)
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void scanner_update_init_102(void)
{
	int resourceHandle, len;
	
	resourceHandle = AbstractFileSystem_Open(IMAGE_SCANNER_FILE, AFS_READ);
	if(resourceHandle >= 0)
	{
		AbstractFileSystem_Close(resourceHandle);
		
		len = strlen(UPDATE_SCANNER_STR);
		//drvLcd_SetColor(COLOR_WHITE,COLOR_BLACK);
		//drvLcd_ClearDisplayMemory(0, 0, gLcd_Total_Row, gLcd_Total_Column);
		//drvLcd_Background2DisplayMemory(16,len*8,140,50);
		drvLcd_Background2DisplayMemory(78, 196, 93, 24);
		drvLcd_SetColor(COLOR_WHITE,COLOR_BLACK);
		drvLcd_DisplayString(140,50,0,(U8*)UPDATE_SCANNER_STR);
		DisplayData2Screen();

		LcdModulePowerOnOff(1);//Ϊ����������Ϊ����ֵ
		
		hyhwUart_stm32FlashLoader(IMAGE_SCANNER_FILE);
	}
	hyhwApi_CommInit();
}
#endif

/*-----------------------------------------------------------------------------
* ����:	scanner_set
* ����:	����ɨ��ͷ
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
static void scanner_set(void)
{
	if (gu8ScannerType == SCANNER_TYPE_SYMBOL)
	{
		/* symbol ɨ��ͷ����*/
		scanner_symbol_set();
		/* ��ʼ������IO */
		/* CTS �øߣ�����SE-955�������� */
		hyhwGpio_setAsGpio(SCANNER_UART_CTS_PORT, SCANNER_UART_CTS_GPIO);
		hyhwGpio_setOut(SCANNER_UART_CTS_PORT, SCANNER_UART_CTS_GPIO);
		hyhwGpio_setHigh(SCANNER_UART_CTS_PORT, SCANNER_UART_CTS_GPIO);
		/* RTS ��Ϊ���븡�� */
		hyhwGpio_setAsGpio(SCANNER_UART_RTS_PORT, SCANNER_UART_RTS_GPIO);
		hyhwGpio_setIn(SCANNER_UART_RTS_PORT, SCANNER_UART_RTS_GPIO);
		hyhwGpio_pullDisable(SCANNER_UART_RTS_PORT, SCANNER_UART_RTS_GPIO);
		/* PWRDWN ��Ϊ���븡�� */
		hyhwGpio_setAsGpio(SCANNER_PWRDWN_PORT, SCANNER_PWRDWN_GPIO);
		hyhwGpio_setIn(SCANNER_PWRDWN_PORT, SCANNER_PWRDWN_GPIO);
		hyhwGpio_pullDisable(SCANNER_PWRDWN_PORT, SCANNER_PWRDWN_GPIO);
		/* Beeper ��Ϊ���븡�� */
		hyhwGpio_setAsGpio(SCANNER_BPR_PORT, SCANNER_BPR_GPIO);
		hyhwGpio_setIn(SCANNER_BPR_PORT, SCANNER_BPR_GPIO);
		hyhwGpio_pullDisable(SCANNER_BPR_PORT, SCANNER_BPR_GPIO);
		/* AIM_WAKE ��Ϊ���븡�� */
		hyhwGpio_setAsGpio(SCANNER_AIM_WAKE_PORT, SCANNER_AIM_WAKE_GPIO);
		hyhwGpio_setIn(SCANNER_AIM_WAKE_PORT, SCANNER_AIM_WAKE_GPIO);
		hyhwGpio_pullDisable(SCANNER_AIM_WAKE_PORT, SCANNER_AIM_WAKE_GPIO);
	}
	else //if (gu8ScannerType == SCANNER_TYPE_OPTICON)
	{
		/* opticon ɨ��ͷ��7901���룬����*/
		hyhwInt_disable(INT_BIT_UART);
		hyhwUart_rxIntDisable(SCANNER_UART);
		hyhwCpm_busClkDisable(CKC_UART3);
		/* ��ʼ������IO */
		/* MCU_RESET �õ� */
		hyhwGpio_setAsGpio(SCANNER_MCU_RESET_PORT, SCANNER_MCU_RESET_GPIO);
		hyhwGpio_setOut(SCANNER_MCU_RESET_PORT, SCANNER_MCU_RESET_GPIO);
		hyhwGpio_setLow(SCANNER_MCU_RESET_PORT, SCANNER_MCU_RESET_GPIO);
		/* MCU_BOOT_CTRL �õ� */
		hyhwGpio_setAsGpio(SCANNER_MCU_BOOT_CTRL_PORT, SCANNER_MCU_BOOT_CTRL_GPIO);
		hyhwGpio_setOut(SCANNER_MCU_BOOT_CTRL_PORT, SCANNER_MCU_BOOT_CTRL_GPIO);
		hyhwGpio_setLow(SCANNER_MCU_BOOT_CTRL_PORT, SCANNER_MCU_BOOT_CTRL_GPIO);
		/* MCU_WAKEUP �õ� */
		hyhwGpio_setAsGpio(SCANNER_MCU_WAKEUP_PORT, SCANNER_MCU_WAKEUP_GPIO);
		hyhwGpio_setOut(SCANNER_MCU_WAKEUP_PORT, SCANNER_MCU_WAKEUP_GPIO);
		hyhwGpio_setLow(SCANNER_MCU_WAKEUP_PORT, SCANNER_MCU_WAKEUP_GPIO);
		/* TRIG �õ� */
		hyhwGpio_setAsGpio(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
		hyhwGpio_setOut(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
		hyhwGpio_setLow(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
		
		barDecode_p_Start();
	}
	/* ���³�ʼ��barBuf */
	barBuf_init();
}

void scan_clear(void)
{
}