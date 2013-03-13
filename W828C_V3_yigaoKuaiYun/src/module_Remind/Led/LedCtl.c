#include "hyTypes.h"
#include "hyhwGPIO.h"
#include "LedCtl.h"
#include "hardWare_Resource.h"



#define LED_PORT				LED1_PORT
#define RED_LED_GPIO			LED2_GPIO
#define GREEN_LED_GPIO			LED3_GPIO
#define BLUE_LED_GPIO			LED1_GPIO

const U32 cLedCtrlIO_info[] = { LED_PORT , RED_LED_GPIO } ;

#define LED_KEY_PAD_TIMEOUT		(500)//5��
U32 gLedKeyPad_timeCtrl=0;
/*-------------------------------------------------------------------------
* ����: Led_init
* ˵��: ����LED����IO�ĳ�ʼ��
* ����: ��
* ����: ��
-------------------------------------------------------------------------*/
void Led_init(void)
{
	hyhwGpio_setAsGpio(LED_PORT, RED_LED_GPIO);
	hyhwGpio_setAsGpio(LED_PORT, GREEN_LED_GPIO);
	hyhwGpio_setAsGpio(LED_PORT, BLUE_LED_GPIO);
	hyhwGpio_setOut(LED_PORT, RED_LED_GPIO|GREEN_LED_GPIO|BLUE_LED_GPIO);
	hyhwGpio_setHigh(LED_PORT, RED_LED_GPIO|GREEN_LED_GPIO|BLUE_LED_GPIO);
	
	//���̵�
	hyhwGpio_setAsGpio(LED_KEYPAD_PORT, LED_KEYPAD_GPIO);
	hyhwGpio_setOut(LED_KEYPAD_PORT, LED_KEYPAD_GPIO);
	hyhwGpio_setLow(LED_KEYPAD_PORT, LED_KEYPAD_GPIO);
	return;
}

/*-------------------------------------------------------------------------
* ����: Led_ctrl
* ˵��: LED����
* ����: color��0----7	0�رգ�7��ɫ
* ����: ��
-------------------------------------------------------------------------*/
void Led_ctrl(int color)
{
	//RED
	if (color&LED_RED)
	{
		hyhwGpio_setLow(LED_PORT, RED_LED_GPIO);
	}
	else
	{
		hyhwGpio_setHigh(LED_PORT, RED_LED_GPIO);
	}
	//GREEN
	if (color&LED_GREEN)
	{
		hyhwGpio_setLow(LED_PORT, GREEN_LED_GPIO);
	}
	else
	{
		hyhwGpio_setHigh(LED_PORT, GREEN_LED_GPIO);
	}
	//BLUE
	if (color&LED_BLUE)
	{
		hyhwGpio_setLow(LED_PORT, BLUE_LED_GPIO);
	}
	else
	{
		hyhwGpio_setHigh(LED_PORT, BLUE_LED_GPIO);
	}
}

/*-------------------------------------------------------------------------
* ����: Led_keyPad_ctrl
* ˵��: ����LED����
* ����: enable: 0----�رգ������
* ����: ��
-------------------------------------------------------------------------*/
void Led_keyPad_ctrl(int enable)
{
	if (enable)
	{
		hyhwGpio_setHigh(LED_KEYPAD_PORT, LED_KEYPAD_GPIO);
		gLedKeyPad_timeCtrl = rawtime(NULL);
	}
	else
	{
		hyhwGpio_setLow(LED_KEYPAD_PORT, LED_KEYPAD_GPIO);
		gLedKeyPad_timeCtrl = 0xFFFFFFFF;
	}
}

/*-------------------------------------------------------------------------
* ����: Led_keyPad_timeCtrl
* ˵��: ����LEDʱ����ƣ���ʱ�ر�
* ����: ��
* ����: ��
-------------------------------------------------------------------------*/
void Led_keyPad_timeCtrl(void)
{
	U32 now;
	
	if (gLedKeyPad_timeCtrl != 0xFFFFFFFF)
	{
		now = rawtime(NULL);
		if ((now-gLedKeyPad_timeCtrl) >= LED_KEY_PAD_TIMEOUT)
		{
			Led_keyPad_ctrl(0);
		}
	}
}