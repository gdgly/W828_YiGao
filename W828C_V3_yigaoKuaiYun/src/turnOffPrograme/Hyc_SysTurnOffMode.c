#include "hyTypes.h"
#include "hyhwTcc7901.h"
#include "hyhwCkc.h"
#include "hyhwintCtrl.h"
#include "hyhwGpio.h"
#include "hyhwRtc.h"

#include "hyhwLcd.h"

#include "Hyc_SysTurnOffMode.h"
#include "HycRightConfig.h"


extern void Vector_Init_Block();
/*************************************************************************
 * DEFINES
 *************************************************************************/
//RTC
#define	RTC_CONTROL_REGISTER		TCC7901_RTC_RTCCON_REGISTER			/* RTC Control Register */
	#define RTC_BIT_WRITE_EN		BIT1
	
#define	RTC_INT_CONTROL_REGISTER	TCC7901_RTC_INTCON_REGISTER			/* RTC Interrupt Control Register */
	#define INT_BIT_ENABLE			BIT0
	
#define	BCD_SECOND_REGISTER			TCC7901_RTC_BCDSEC_REGISTER			/* BCD Second Register */

//RTC ALARM
#define	RTC_INT_PEND_REGISTER		TCC7901_RTC_RTCPEND_REGISTER		/* RTC Interrupt Pending Register */
	#define INTPEND_BIT_PENDING		BIT0


//USB
#define	INTERRUPT_REQUEST_REGISTER	TCC7901_PIC_STS_REGISTER
	#define INTREQ_BIT_VBUS			BIT8

//��Դ��PORT,GPIO,��Чֵ
#define POWER_KEY_PORT				(PORT_D)
#define POWER_KEY_GPIO				(BIT5)//(BIT6)//
#define POWER_KEY_VALID				(1)
#define POWER_KEY_LONG				(100)//100*10ms=1S


#define USB_DETECT_CNT				(10)
#define TURN_OFF_RESUME_12M_CNT		(100)//100*10ms=1S

/*	�ú��������еĳ��������overlay��Sram�У�
	�ڽ������ѭ��ʱ������Ҫ��ʹ�þֲ�������
	�������Ӻ�����������Sdram�ϵ��κ���Դ(�������ڴ�)
*/

static U32 bakWorkModeCtrlReg;

static volatile U32 *powerKey_Port;
static volatile U32 powerKey_Gpio;
static U32 gpioStatus;
static U32 powerKeyDetectCnt;	//��Դ��ɨ�����
static U32 usbDetectCnt;		//USB VBusɨ�����

static U32 turnOffResume12MCnt;

static U32 turnOnFlag;

static U32 turnOffAadVoltageFlag;
static U32 turnOffAad2VoltageFlag;

#ifdef TEST_USE_GPIO
static volatile U32 *test_Port;
static volatile U32 test_Gpio;
static U32 turnOffTestFlag;
#endif


void Hyc_SysTurnOffMain(void)
{

#ifdef TEST_USE_GPIO
	hyhwGpio_setAsGpio(TEST_PORT, TEST_GPIO);
	hyhwGpio_setOut(TEST_PORT, TEST_GPIO);
	hyhwGpio_setLow(TEST_PORT, TEST_GPIO);
	
	test_Port = (volatile U32 *)(GPIO_GROUP_REGISTER_BASEADDR + (TEST_PORT*sizeof(tGPIO_REGISTER)));
	test_Gpio = TEST_GPIO;
#endif

	powerKey_Port	= (volatile U32 *)(GPIO_GROUP_REGISTER_BASEADDR + (POWER_KEY_PORT*sizeof(tGPIO_REGISTER)));
	powerKey_Gpio	= POWER_KEY_GPIO;

/*------------------------------------------
	ADJ			ADJ2		��ѹ
 	0			0			0.9V
 	0			1			1.1V
 	1			0			1.3V
 	1			1			1.5V
--------------------------------------------*/	
//���CORE��ǰ�ĵ�ѹ״̬����¼
#ifdef TCC7901
	if (hyhwmGpio_ReadADJ() != 0)	//ADJ
		turnOffAadVoltageFlag = 1;
	else
		turnOffAadVoltageFlag = 0;
		
	if (hyhwmGpio_ReadADJ2() != 0)	//ADJ2
		turnOffAad2VoltageFlag = 1;
	else
		turnOffAad2VoltageFlag = 0;
#endif	
	
	bakWorkModeCtrlReg = CKC_MODECTR_REGISTER;
#if 1
	/*	Disable PLLs steps: */
	/*	1.set HwPLLCFG's S=3 to make stable when system clock is changed
	*/
	CKC_PLL0CFG_REGISTER |= 0x00030000;
	CKC_PLL1CFG_REGISTER |= 0x00030000;
	
	/*	2.clear CPU0 source, HwCLKCTRL setting bit0~2
		clear bus divider, Fbus=Fxindiv/2
	*/
	//CLK_DIVIDE_REGISTER |= 0x0000EF00;
	//CKC_CLKCTRL_REGISTER |= (CKC_CLKCTRL_REGISTER & ~0x0F) | (BIT2|BIT0);

	/* ʹ��32768������Ϊϵͳʱ�� */
	CKC_CLKCTRL_REGISTER |= MCPUCLK_BIT_DIV_MASK;
	CKC_CLKCTRL_REGISTER |= (CKC_CLKCTRL_REGISTER & ~0x0F) | (BIT2|BIT1);
	
	/* ��ֹ PLL */
	CKC_PLL0CFG_REGISTER |= BIT31;
	CKC_PLL1CFG_REGISTER |= BIT31;
	
 #if  defined(G20_V5)||defined(G22_V1)
	//set core voltage to 0.9V	
#ifdef TCC7901
	hyhwmGpio_SetADJ_Low(); 	//ADJ
	hyhwmGpio_SetADJ2_Low();	//ADJ2
#endif
	#endif
#endif
	
	//clear all IRQ
	INT_CLEAR_REGISTER  = 0xffffffff;
//	INTB_CLEAR_REGISTER  = 0x0001ffff;
	
	#if  defined(G20_V5)||defined(G22_V1)
	//enter power down mode
	CKC_MODECTR_REGISTER &= ~BIT9;
	CKC_MODECTR_REGISTER |= (BIT9|BIT8);
	#endif
	
	#ifdef TEST_USE_GPIO
	turnOffTestFlag = 0;
	#endif
	
	powerKeyDetectCnt = 0;
	usbDetectCnt = 0;
	
	turnOffResume12MCnt = 0;
	RTC_CONTROL_REGISTER |= RTC_BIT_WRITE_EN;
	RTC_INT_CONTROL_REGISTER |= INT_BIT_ENABLE;
	while(1)
	{
		turnOnFlag = 0;
		#ifdef TEST_USE_GPIO
		if (sysSleepTestFlag == 0)
		{
			(*test_Port) &= ~test_Gpio;//low
			turnOffTestFlag = 1;
		}
		else
		{
			(*test_Port) |= test_Gpio;//high
			turnOffTestFlag = 0;
		}
		#endif
		
	/* ɨ���Դ��������ǵ�Դ������������ */
		if ((*powerKey_Port) & powerKey_Gpio)
		{
			/* �ߵ�ƽ */
			gpioStatus = 1;
		}
		else
		{
			/* �͵�ƽ */
			gpioStatus = 0;
		}
		if (gpioStatus == POWER_KEY_VALID)
		{
			powerKeyDetectCnt++;
			if (powerKeyDetectCnt >= POWER_KEY_LONG)
			{
				/* ������Դ��������ϵͳ���� */
				turnOnFlag |= POWER_ON_TURNON;
			}
		}
		else
		{
			/* �����Դkey������ */
			powerKeyDetectCnt = 0;
		}
		
	/*	��ȡRTC��ֱ�Ӷ�ȡALARM�Ĵ��� */
		if (RTC_INT_PEND_REGISTER & INTPEND_BIT_PENDING)
		{
			turnOnFlag |= ALARM_TURNON;
		}
		
	/* ��� USB �Ƿ����(�п�����������) */
		if (INTERRUPT_REQUEST_REGISTER & INTREQ_BIT_VBUS)
		{
			usbDetectCnt++;
			if (usbDetectCnt >= USB_DETECT_CNT)
			{
				turnOnFlag |= USB_VBUS_TURNON;
				usbDetectCnt = 0;
			}
		}
		else
		{
			/* ���USB VBUS������ */
			usbDetectCnt = 0;
		}
		
		turnOffResume12MCnt++;
		//����1����һ��
		if(turnOffResume12MCnt >= TURN_OFF_RESUME_12M_CNT)
		{
			CKC_MODECTR_REGISTER = BIT9; // �ָ�12Mʱ��
			turnOffResume12MCnt = 0;
		}
		else if (turnOffResume12MCnt == 3)
		{
			CKC_MODECTR_REGISTER = BIT8;//ֹͣ12Mʱ�ӣ�
		}
		
		/* ����ϵͳ */
		if (turnOnFlag != 0)
		{
			break;
		}
	}
	CKC_MODECTR_REGISTER = bakWorkModeCtrlReg;
	
	/*	�������Ƶ�� */
	hyhwSysClk_Set(ICLK96M_AHB48M, EXTAL_CLK);
	
	#if  defined(G20_V5)||defined(G22_V1)
	// exit power down mode	
	CKC_MODECTR_REGISTER &= ~BIT8;
	CKC_MODECTR_REGISTER |= (BIT9);
	#endif
	
	#if 1
	{
		int i, color;
		
		LcdModule_LightBack();
		color = 0;
		if (turnOnFlag&POWER_ON_TURNON)
		{
			color |= 0x001F;
		}
		if (turnOnFlag&USB_VBUS_TURNON)
		{
			color |= 0x07E0;
		}
		if (turnOnFlag&ALARM_TURNON)
		{
			color |= 0xF800;
		}
		drvLcd_SetRowColumn_Lcd(100,0,10,10);
		for (i=0; i<100; i++)
		{
			drvLcd_WriteData(color);
		}
		
		i=100000;
		while(i--);
	}
	#endif
	/*	ֱ�Ӹ�λ���� */
	Vector_Init_Block();
}
