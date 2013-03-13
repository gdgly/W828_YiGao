/*--------------------------------------------------------------------------
Standard include files:
--------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

/*--------------------------------------------------------------------------
*INCLUDES
--------------------------------------------------------------------------*/
#include "hyhwLcd.h"
#include "hyhwGpio.h"
#include "glbVariable_base.h"
#include "glbVariable.h"
#include "HycDeviceConfig.h"


#define DISPLAYMODE_TWO

#ifdef DISPLAYMODE_TWO
U8 gScreen_Display_Mode = 0;	//��ʾ��ʽ
#endif
/*
version index	��ʾ������		����оƬ				device code
	0			����ʾ��			��						��
	1			������ʾ��		����оƬΪER61505V		0xB505
	2			���ǵ���			����оƬΪILI9325C		0x9325
*/
const U16 lcdDeviceCode[]=
{
	0xB505,		//������ʾ��	����оƬΪER61505V
	0x9325,		//���ǵ���	����оƬΪILI9325C
	0xFFFF
};
U8 lcdHwVersionIdx=0;



void drvLcd_ClearLcdRAM(void);
void LcdModulePowerOnOff(U8 u8Value);


void LcdModule_LightBack(void)
{
	LcdModulePowerOnOff(1);
}

/*-----------------------------------------------------------------------------
* ����:	Delay
* ����:	delay uint 1ms
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void Delay1(U32 Time)
{
	U32 i,j;
	for(i=0;i<Time;i++)
	{
		j=50000;
    	while(j--);
    }
}

/*-----------------------------------------------------------------------------
* ����:	LcdModulePowerOnOff
* ����:	LCDģ���Դ����
* ����:	u8Value----0:�ر�;1:���� 
* ����:	none
*----------------------------------------------------------------------------*/
/**/
void LcdModulePowerOnOff(U8 u8Value)
{
	if(u8Value == 1)
	{
		//hyhwLcd_lightSet(10);
		hyhwLcd_lightSet(gtHyc.BackLight);
	}
	else
	{
		hyhwLcd_PWMReset();
	}
}


/*-----------------------------------------------------------------------------
* ����:	drvLcd_LCDModulePowerOn
* ����:	��LCDģ���Դ
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_LCDModulePowerOn(void)
{
	LcdModulePowerOnOff(1);
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_LCDModulePowerOff
* ����:	�ر�LCDģ���Դ
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_LCDModulePowerOff(void)
{
	LcdModulePowerOnOff(0);
}

U8 drvLcd_GetVersionIndex(void)
{
	#if 0
	U16 deviceCode;
	U8  i, versionIndex;
	
	Delay1(18);
	drvLcd_WriteInstruct(0x0000);drvLcd_ReadData(&deviceCode);
	
	i = 0;
	versionIndex = 0;
	while(1)
	{
		if (lcdDeviceCode[i] == deviceCode)
		{
			versionIndex = (i+1);
			break;
		}
		else if (lcdDeviceCode[i] == 0xFFFF)
		{
			break;
		}
		i++;
	}
	
	return versionIndex;
	#else
	return 1;
	#endif
}

void drvLcd_InitLcdModuleInstructs(void)
{
	if (lcdHwVersionIdx == 1)
	{
		drvLcd_InitLcdModuleInstructs_0xB505();
	}
	else if (lcdHwVersionIdx == 2)
	{
		//drvLcd_InitLcdModuleInstructs_0x9325();
	}
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_InitLCDModule
* ����:	
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_InitLCDModule(void)
{
	hyhwLcd_lcdReset();
	LcdModulePowerOnOff(0);
	lcdHwVersionIdx = drvLcd_GetVersionIndex();
	drvLcd_InitLcdModuleInstructs();
	drvLcd_ClearLcdRAM();
}

#ifdef DISPLAYMODE_TWO
void drvLcd_176220DisplayMode(void)
{
	if (lcdHwVersionIdx == 1)
	{
		drvLcd_176220DisplayMode_0xB505();
	}
	else if (lcdHwVersionIdx == 2)
	{
		//drvLcd_176220DisplayMode_0x9325();
	}
}
void drvLcd_220176DisplayMode(void)
{
	if (lcdHwVersionIdx == 1)
	{
		drvLcd_220176DisplayMode_0xB505();
	}
	else if (lcdHwVersionIdx == 2)
	{
		//drvLcd_220176DisplayMode_0x9325();
	}
}
#endif

/*-----------------------------------------------------------------------------
* ����:	drvLcd_DisplayOn
* ����:	����ʾ
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_DisplayOn(void)
{
	if (lcdHwVersionIdx == 1)
	{
		drvLcd_DisplayOn_0xB505();
	}
	else if (lcdHwVersionIdx == 2)
	{
		//drvLcd_DisplayOn_0x9325();
	}
}
/*-----------------------------------------------------------------------------
* ����:	drvLcd_DisplayOff
* ����:	�ر���ʾ
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_DisplayOff(void)
{
	if (lcdHwVersionIdx == 1)
	{
		drvLcd_DisplayOff_0xB505();
	}
	else if (lcdHwVersionIdx == 2)
	{
		//drvLcd_DisplayOff_0x9325();
	}
}

/*-----------------------------------------------------------------------------
* ����:	drvLcd_SetRowColumn
* ����:	���ò�ɫ��ʾ������ʾ��ʼ��ַ����ַΪ����ź������row��Column,�Լ��߶�
*		�Ϳ��
* ����:	kRow 			- ����� , ��ʼ�У�0
*		kColumn 		- ����� ����ʼ�У�0
*		U16 kHeight 	- �߶�
*		U16 kWidth 		- ���
* ����:	none
*----------------------------------------------------------------------------*/
void drvLcd_SetRowColumn_Lcd(U16 kRow,U16 kColumn,U16 kHeight,U16 kWidth)
{
	if (lcdHwVersionIdx == 1)
	{
		drvLcd_SetRowColumn_Lcd_0xB505(kRow, kColumn, kHeight, kWidth);
	}
	else if (lcdHwVersionIdx == 2)
	{
		//drvLcd_SetRowColumn_Lcd_0x9325(kRow, kColumn, kHeight, kWidth);
	}
}
