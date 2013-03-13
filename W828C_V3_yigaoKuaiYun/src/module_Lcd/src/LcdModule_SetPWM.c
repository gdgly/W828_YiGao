#include "hyTypes.h"
#include "hyhwTimer.h"
#include "hyhwGpio.h"
#include "hyErrors.h"
#include "HycDeviceConfig.h"
#include "hardWare_Resource.h"

#ifdef ACT6311
	#define PWM_TIMERID			TIMER0
	#define PWM_PORT			LCD_PWM_PORT
	#define PWM_BIT				LCD_PWM_GPIO
#endif  // ACT6311
#ifdef CP2142MM
	#define PWM_COUNTER_ID 		TIMER1
	#define PWM_TIMERID			TIMER0
	#define PWM_PORT			LCD_PWM_PORT
	#define PWM_BIT				LCD_PWM_GPIO
#endif


/*-----------------------------------------------------------------------------
* ����:	void hyhwLcd_lightSet(U8 grade)
* ����:	test pwm function
* ����:	grade--���뱳��ǿ�����ô�(0 �� 31) grade =0 ��������20mA,
*										   grade =8	������15mA
*										   grade =16������10mA
*										   grade =24������5mA
*										   grade =31����ر�
*----------------------------------------------------------------------------*/
U8 gu8LastGrade = 0xFF;
#ifdef ACT6311
void hyhwLcd_lightSet(U8 grade)
{
	if(grade >68 || grade < 40) grade = 40 ;
	
	if(grade == gu8LastGrade) return ;
	gu8LastGrade = grade;
	
	hyhwGpio_setAsGpio(PWM_PORT, PWM_BIT);
	hyhwGpio_setPwmFunction();
	hyhwTimer_setPwm(PWM_TIMERID,1000,grade); 
}

void hyhwLcd_lightOff()
{
	hyhwGpio_setAsGpio(PWM_PORT, PWM_BIT);
	hyhwGpio_setOut(PWM_PORT, PWM_BIT);
	hyhwGpio_setLow(PWM_PORT, PWM_BIT);
}

void hyhwLcd_lightReset(void)
{
	hyhwGpio_setAsGpio(PWM_PORT, PWM_BIT);
	hyhwGpio_setOut(PWM_PORT, PWM_BIT);
	hyhwGpio_setLow(PWM_PORT, PWM_BIT);
	gu8LastGrade = 0xFF; 
}
#endif
#ifdef CP2142MM
static void hyhwLcd_isr(void)
{
/* ��PWM��������Ժ�Ӧ�������ź����øߵ�ƽ���ֵ�������, ������Ч */
	hyhwTimer_pwmDisable(PWM_TIMERID);
	
	hyhwGpio_setAsGpio(PWM_PORT, PWM_BIT);
	hyhwGpio_setOut(PWM_PORT, PWM_BIT);
	hyhwGpio_setHigh(PWM_PORT, PWM_BIT);
	
	hyhwTimer_countDisable(PWM_COUNTER_ID);
	hyhwTimer_intDisable(PWM_COUNTER_ID);
}
void hyhwLcd_PWMReset(void)
{
	hyhwGpio_setAsGpio(PWM_PORT, PWM_BIT);
	hyhwGpio_setOut(PWM_PORT, PWM_BIT);
	hyhwGpio_setLow(PWM_PORT, PWM_BIT);
	gu8LastGrade = 0xFF; 
}
void hyhwLcd_lightSet(U8 grade)
{
	U32 timePeriod;
	U8  tempGrade;
	
	if(grade>16)
		grade = 1;
	
	if (0 == grade) return;
		
	if (gu8LastGrade == 0xFF)
	{
		tempGrade = grade;
	}
	else
	{
		if (grade > gu8LastGrade)
		{
			tempGrade = grade-gu8LastGrade;
		}
		else if (grade < gu8LastGrade)
		{
			tempGrade = 16-gu8LastGrade+grade;
		}
		else
		{
			return;
		}
	}
	gu8LastGrade = grade;
	
	timePeriod = 1000000/(50*tempGrade);

	hyhwTimer_pwmDisable(PWM_TIMERID);
	hyhwGpio_setAsGpio(PWM_PORT, PWM_BIT);
	hyhwGpio_setAsPwmFunction(PWM_PORT, PWM_BIT);
	
	hyhwInt_ConfigTimer1_ISR(hyhwLcd_isr);
	hyhwTimer_SetFrequency(PWM_COUNTER_ID, timePeriod);
	hyhwTimer_countDisable(PWM_COUNTER_ID);
	hyhwTimer_intEnable(PWM_COUNTER_ID);
	
	hyhwTimer_setConter(PWM_TIMERID, 0);
	hyhwTimer_setPwm(PWM_TIMERID, 20000, 70);
	
	hyhwTimer_setConter(PWM_COUNTER_ID, 0);
	hyhwTimer_countEnable(PWM_COUNTER_ID);
}
void hyhwLcd_lightSet_test(U8 cnt)
{
	U32 timePeriod;
	U8  tempGrade;
	
	//timePeriod = 1000000/(50*cnt);

	hyhwTimer_pwmDisable(PWM_TIMERID);
	hyhwGpio_setAsGpio(PWM_PORT, PWM_BIT);
	hyhwGpio_setPwmFunction();
	
	hyhwInt_ConfigTimer1_ISR(hyhwLcd_isr);
	hyhwTimer_SetFrequency(PWM_COUNTER_ID, timePeriod);
	hyhwTimer_countDisable(PWM_COUNTER_ID);
	hyhwTimer_intEnable(PWM_COUNTER_ID);
	
	hyhwTimer_setPwm(PWM_TIMERID, 20000, 50);
	hyhwTimer_countEnable(PWM_COUNTER_ID);
}
#endif

// tg3��Ŀ��, ���º����ò��� .. // 20090902 yanglijing
/* GPIO_F BIT14���ź����ÿ��ܻ�Ӱ������ģ�飬��ʱ����ע�͵�, tg3��û������ƿ���
   ����Щ������Ϊ�պ��� */
/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_flashSet
* ����:	�������������
* ����: none
* ����:	none
*���ò�����0-16
*	RFSET(��)  IFLED=2880/RFSET  RFSET = 14.3K(IFLED =206mA Max)
*	
*	FENS Data	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	
*		
*	IFLX(%)	 	100	94	87	81	73	67	60	53	46	40	34	27	20	13	6	3
*----------------------------------------------------------------------------*/
#define FENS_Data_MAX 	16

U32 hyhwLcd_flashSet(U8 FENS_Data)
{
	U32 i,j;
	U8 intLev;

	if(FENS_Data > FENS_Data_MAX)
		return HY_ERR_BAD_PARAMETER;
	
	FENS_Data = (FENS_Data_MAX+1) - FENS_Data;
	
#if 0
	hyhwGpio_setAsGpio(LED_FLASH_PORT,LED_FLASH_GPIO);
	hyhwGpio_setOut(LED_FLASH_PORT,LED_FLASH_GPIO);
	
	intLev = InterruptDisable();
	for(i=0;i<FENS_Data;i++)
	{
		hyhwGpio_setHigh(LED_FLASH_PORT,LED_FLASH_GPIO);
		//delay 1us
		for(j=0;j<=20;j++);
		hyhwGpio_setLow(LED_FLASH_PORT,LED_FLASH_GPIO);
		//delay 1us
		for(j=0;j<=20;j++);
	}
	InterruptRestore(intLev);
#else
	hyhwGpio_setAsGpio(LED_FLASH_PORT,LED_FLASH_GPIO);
	hyhwGpio_setOut(LED_FLASH_PORT,LED_FLASH_GPIO);
	hyhwGpio_setHigh(LED_FLASH_PORT,LED_FLASH_GPIO);
#endif
	return HY_OK;
}



/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_flashEnable
* ����:	ʹ�������
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_flashEnable(void)
{
	hyhwGpio_setAsGpio(LED_FLASH_PORT,LED_FLASH_GPIO);
	hyhwGpio_setOut(LED_FLASH_PORT,LED_FLASH_GPIO);
	hyhwGpio_setHigh(LED_FLASH_PORT,LED_FLASH_GPIO);
	
}
/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_flashDisable
* ����:	�ر������  Flash����ƵĹرշ�ʽ��VDD_LEDFLASH_EN����GPIOA[7] ��
        ���Ϊ�͵�ƽ�Ǵ�����ƣ����Ϊ�ߵ�ƽʱ������ȫ�ر�����ƣ�
        ����ڶ�����ƹرտ���ʱ��Ҫ����I/O������Ϊ�������̬���С�
* ����: none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwLcd_flashDisable(void)
{
	hyhwGpio_setAsGpio(LED_FLASH_PORT,LED_FLASH_GPIO);
	hyhwGpio_setOut(LED_FLASH_PORT,LED_FLASH_GPIO);
	hyhwGpio_setLow(LED_FLASH_PORT,LED_FLASH_GPIO);
}
/*-----------------------------------------------------------------------------
* ����:	hyhwLcd_flashStatus
* ����:	�����״̬
* ����:	none
* ����:	0----�رգ�1----����
*----------------------------------------------------------------------------*/
int hyhwLcd_flashStatus(void)
{
	//return !hyhwGpio_Read(LED_FLASH_PORT,LED_FLASH_BIT);
}

void flash_test()
{
	static enable = 0  ;
	
	if(0 == enable){enable = 1 ; hyhwLcd_flashDisable();}
	else{enable = 0 ; hyhwLcd_flashEnable();}
	
}
