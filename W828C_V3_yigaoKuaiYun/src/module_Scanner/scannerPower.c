/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"
/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "hyhwGpio.h"
#include "scanner.h"



/*-----------------------------------------------------------------------------
* ����:	hyhwScanner_TrigInit
* ����:	ɨ��ͷtrig IO��ʼ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwScanner_TrigInit(void)
{
	hyhwGpio_setAsGpio(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
	hyhwGpio_setOut(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
	hyhwGpio_setHigh(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
}
/*-----------------------------------------------------------------------------
* ����:	hyhwScanner_TrigDisable
* ����:	ɨ��ͷtrig IO��ʼ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwScanner_TrigDisable(void)
{
	hyhwGpio_setAsGpio(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
	hyhwGpio_setIn(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
	hyhwGpio_pullDisable(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
}

/*-----------------------------------------------------------------------------
* ����:	hyhwScanner_TrigCtrl
* ����:	ɨ��ͷtrig IO���ƣ��͵�ƽ��Ч
* ����:	isEnable----0,��ֹ����������
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwScanner_TrigCtrl(int isEnable)
{
	U8 scannerType;
	
	scannerType = scanner_getType();
	if (scannerType == SCANNER_TYPE_SYMBOL)
	{
		//symbol
		if (isEnable)
		{
			hyhwGpio_setAsGpio(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
			hyhwGpio_setOut(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
			hyhwGpio_setLow(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
		}
		else
		{
			hyhwGpio_setAsGpio(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
			hyhwGpio_setOut(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
			hyhwGpio_setHigh(SCANNER_TRIG_PORT, SCANNER_TRIG_GPIO);
		}
	}
	else
	{
		//opticon
		static U32 scanKeyCnt = 0;
		if (isEnable)
		{
			//ɨ�������
			scanKeyCnt++;
			if (scanKeyCnt > 6000) scanKeyCnt = 6000;//һ����
		}
		else
		{
			//ɨ���̧��
			scanKeyCnt = 0;
		}
		barScanKey_laserCtrl(scanKeyCnt);
	}
}

