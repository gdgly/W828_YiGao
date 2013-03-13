/*************************************************************
%
% Filename     : DrvKeybScan.c
% Project name : part of the lower key driver of GUI part of hyctron, manage the input key scanning 
%
% Copyright 2003 Hyctron Electronic Design House,
% "Hyctron Electronic Design House" Shanghai, China.
% All rights are reserved. Reproduction in whole or in part is prohibited
% without the prior written consent of the copyright owner.
%
% Rev   Date    	Author          Comments
%      (yymmdd)
% -------------------------------------------------------------
% 001   040128  HYCZJ		Primary version 
% -------------------------------------------------------------
%
%  This source file contains the key process part of the GUI   
% 
****************************************************************/
#include "glbStatusDef.h"
#include "hyhwGpio.h"
#include "drvKeyb.h"
#include "hyhwAdc.h"
#include "LedCtl.h"
#include "HycDeviceConfig.h"

#define CLOSE_AUTOSCAN_MAXTIME		(2*6000)//2����

U8   gu8AutoScan;//1:�����Զ�ɨ��   0:�ر�
U32	 gu32LastAutoScanTime;//�Զ�ɨ��ʱ���һ��ɨ�赽����ʱ��
static int gsScanKeyPushTime=0;

#define KEY_SCAN_L (BIT1)
#define KEY_SCAN_R (BIT4)
#define KEY_SCAN_M (BIT6)


/*-----------------------------------------
*ע�⣺*ע�⣺*ע�⣺*ע�⣺*ע�⣺*ע�⣺
*ע�⣺                             ע�⣺
*ע�⣺JTAG ���Ե�ʱ����ʹ�ð���  ע�⣺
*ע�⣺JTAG ���Ե�ʱ����ʹ�ð���  ע�⣺
*ע�⣺								ע�⣺
*ע�⣺*ע�⣺*ע�⣺*ע�⣺*ע�⣺*ע�⣺
-------------------------------------------*/

//#define JTAG_DEBUG

typedef struct _tKEYIOINPUT
{
	U8   port;  //�˿ں�
	U32  iobit; //bit
	U8   iook;  //��Ч��ʽ 0��1������ο�Ӳ������
}tKEYIOINPUT;

typedef struct _tKEYIOOUTPUT
{
	U8   port;  //�˿ں�
	U32  iobit; //bit
}tKEYIOOUTPUT;


#define KEY_IO_INPUT_NUM		4   
#define KEY_IO_OUTPUT_NUM		5
#define KEY_IO_INPUT_SINGLE_NUM 4  /* �Ǿ��󰴼� */
/* 
 * ˵��: ��20������ 4*5

 * ������̶��� 
W828
						     KEY_IN1	KEY_IN2		KEY_IN3		KEY_IN4
						     GPIO_F14	GPIO_D8		GPIO_D9 	GPIO_D10
KEY_OUT1		GPIO_A10		S2-0		S3-*		S4-7		S5-#
KEY_OUT2		GPIO_F15		S7-5		S8-DEL		S9-ESC		S10-6
KEY_OUT3		GPIO_A12		S11-DOWN	S12-R		S13-OK		S14-TAB
KEY_OUT4		GPIO_D6			S15-8		S16-4		S17-1		S18-9
KEY_OUT5		GPIO_D7			S19-2		S20-L		S21-UP		S22-3

 * ������Ϊ�������룬������Ϊ�������
 */
//��������˿�
const tKEYIOOUTPUT gtIOOutput[KEY_IO_OUTPUT_NUM]=
{
	{PORT_A, GPIO_10},
	{PORT_F, GPIO_15},
	{PORT_A, GPIO_12},
	{PORT_D, GPIO_6},
	{PORT_D, GPIO_7},
};

const tKEYIOINPUT  gtIOInput[KEY_IO_INPUT_NUM]=
{	
	{PORT_F, GPIO_14,	0},
	//{PORT_A, GPIO_2,	0},
	{PORT_E, GPIO_29,	0},
	{PORT_E, GPIO_30,	0},
	{PORT_D, GPIO_10,	0}
};

//�Ǿ������
const tKEYIOINPUT  gtIOInputSingle[KEY_IO_INPUT_SINGLE_NUM]=
{
	{PORT_A, GPIO_11,	1},	/* ������ */
	{PORT_D, GPIO_5,	0},	/* ɨ����� */
	{PORT_D, GPIO_12,	0},	/* ɨ����� */
	{PORT_F, GPIO_31,	0},	/* ���ܼ�F1 */
};


/* ÿ��bit��Ӧ�İ��� */
const U8 gdrvKeyb_u8KeycodeTable[] = 
{
	0,
	Keyb_Code_Power,  /* bit 0 */
	Keyb_Code_ScanL,  /* bit 1 */
	Keyb_Code_ScanR,
	Keyb_Code_F1,
	
	Keyb_Code_0,
	Keyb_Code_Xing,
	Keyb_Code_7,
	Keyb_Code_Jing,
	Keyb_Code_5,
	Keyb_Code_Del,
	Keyb_Code_Esc,
	Keyb_Code_6,
	Keyb_Code_Down,
	Keyb_Code_Right,
	Keyb_Code_OK,
	Keyb_Code_Tab,
	Keyb_Code_8,
	Keyb_Code_4,
	Keyb_Code_1,
	Keyb_Code_9,
	Keyb_Code_2,
	Keyb_Code_Left,
	Keyb_Code_Up,
	Keyb_Code_3,
	
};

//���B6�Ƿ�Ϊ��
const U32 cWakeUpSysKeyInfo[25] = 
{
	//power key
	0,			/* out put port */
	0,			/* out put gpio */
	PORT_A,		/* in  put port */
	GPIO_11,	/* in  put port */
	1,			/* ��Чֵ */
	
	//ɨ�����
	0,			/* out put port */
	0,			/* out put gpio */
	PORT_D,		/* in  put port */
	GPIO_5,		/* in  put port */
	0,			/* ��Чֵ */

	//ɨ�����
	0,			/* out put port */
	0,			/* out put gpio */
	PORT_D,		/* in  put port */
	GPIO_12,	/* in  put port */
	0,			/* ��Чֵ */
	
#if 1
	//ok key
	0,		/* out put port */
	0,		/* out put gpio */
	PORT_F,		/* in  put port */
	GPIO_31,	/* in  put port */
	0,			/* ��Чֵ */
#endif
};


void drvKey_ClearScanTime()
{
	if(gu8AutoScan == 1)//�Զ�ɨ��
	{
		hyhwScanner_TrigCtrl(0);
		gsScanKeyPushTime = 150;
		gu32LastAutoScanTime = rawtime(NULL);
	}
}

/*-------------------------------------------------------
* ����: drvKeyb_ScanInitialize
* ����: Initialize the key matrix scan module
* ����: cSSA_OK, initialization is successful 
* ����:      
---------------------------------------------------------*/
U32 drvKeyb_ScanInitialize( void )
{
	U32 i;
	U32 port, iobit;
	
	#if 1
	hyhwGpio_setAsGpio(PORT_A, GPIO_2);
	hyhwGpio_pullDisable(PORT_A, GPIO_2);
	hyhwGpio_setIn(PORT_A, GPIO_2);
	#endif
	
	//power_key ��Ϊ����
	hyhwGpio_setAsGpio(gtIOInputSingle[0].port, gtIOInputSingle[0].iobit);
	hyhwGpio_pullDisable(gtIOInputSingle[0].port, gtIOInputSingle[0].iobit);
	hyhwGpio_setIn(gtIOInputSingle[0].port, gtIOInputSingle[0].iobit);
	
	//����IO����Ϊ��������
	for(i=1; i < KEY_IO_INPUT_SINGLE_NUM; i++)
	{
		port = gtIOInputSingle[i].port;
		iobit = gtIOInputSingle[i].iobit;
		hyhwGpio_setAsGpio(port, iobit);
		hyhwGpio_setPullUp(port, iobit);
		hyhwGpio_setIn(port, iobit);
	}

#ifndef JTAG_DEBUG
	//����ڳ�ʼ��
	for(i=0; i<KEY_IO_INPUT_NUM; i++)
	{
		port = gtIOInput[i].port;
		iobit = gtIOInput[i].iobit;
		hyhwGpio_setAsGpio(port, iobit);
		hyhwGpio_setPullUp(port, iobit);
		hyhwGpio_setIn(port, iobit);
	}
	
	//����ڳ�ʼ��
	for(i=0; i<KEY_IO_OUTPUT_NUM; i++)
	{
		port = gtIOOutput[i].port;
		iobit = gtIOOutput[i].iobit;
		hyhwGpio_setAsGpio(port, iobit);
		hyhwGpio_setOut(port, iobit);
		hyhwGpio_setHigh(port, iobit);
	}
#endif
	
	gu8AutoScan = 0;
	return cSSA_OK;
}


/*------------------------------------------------------------------------
* ����: drvKeyb_IsLocked
* ����: To determine whether the local keypad should be locked.
* ����: TRUE, the local keys are locked & no events will be generated.
        FALSE, the local keys are operated normally.
* ����: 
--------------------------------------------------------------------------*/
U32 drvKeyb_IsLocked( void )
{
	return 0;
}


/*------------------------------------------------------------------------
* ����: drvKeybPowerOnKeyPressed
* ����: ��⿪�����Ƿ���
* ����: none
* ����: 1------������
*		0------��û�а���
-------------------------------------------------------------------------*/
U32 drvKeybIOinputSingleKeyPressed(U8 index)
{
	U32 rc = 0;

	if (hyhwGpio_Read(gtIOInputSingle[index].port, gtIOInputSingle[index].iobit) == gtIOInputSingle[index].iook)
	{
	 	rc = 1;
	}

	return rc;
}
/*------------------------------------------------------------------------
* ����: drvKeybPowerOnKeyPressed
* ����: ��⿪�����Ƿ���
* ����: none
* ����: 1------����������
*		0------������û�а���
-------------------------------------------------------------------------*/
U32 drvKeybPowerOnKeyPressed(void)
{
	U32 rc = 0;
	U32 powerKeyIndex;

	powerKeyIndex = 0;
	if (hyhwGpio_Read(gtIOInputSingle[powerKeyIndex].port, gtIOInputSingle[powerKeyIndex].iobit) == gtIOInputSingle[powerKeyIndex].iook)
	{
	 	rc = 1;
	}

	return rc;
}

/*------------------------------------------------------------------------
* ����: drvKeyb_Scan()
* ����: ɨ�谴��״̬
* ����: �õ����µļ� ��bit����ʾĳ������
* ����: none
--------------------------------------------------------------------------*/
S32 drvKeyb_Scan( void )
{
	unsigned int status = 0;
	unsigned int bit = 1;
	unsigned int i, j;
	int scanKey=0;
	static int laser = 0;

	/* I/O����⣬power�� */
	for(i = 0 ; i < KEY_IO_INPUT_SINGLE_NUM; i++)
	{
		if(0 != drvKeybIOinputSingleKeyPressed(i))
		{
			status |= bit;
		}
		
		bit <<= 1;
	}
	
#ifndef JTAG_DEBUG
	#if 1
	hyhwGpio_setAsGpio(PORT_F, GPIO_14);
	hyhwGpio_setOut(PORT_F, GPIO_14);
	hyhwGpio_setHigh(PORT_F, GPIO_14);
	//hyhwGpio_setAsGpio(PORT_F, GPIO_14);
	hyhwGpio_setIn(PORT_F, GPIO_14);
	#endif
	/* �������ɨ�� */
	for(i=0; i<KEY_IO_OUTPUT_NUM; i++)
	{
		hyhwGpio_setLow(gtIOOutput[i].port, gtIOOutput[i].iobit);

		/* ��ȡKEY_INPUT_PORT����I/Oֵ */
		for (j=0; j<KEY_IO_INPUT_NUM; j++)
		{
			if(hyhwGpio_Read(gtIOInput[j].port, gtIOInput[j].iobit) == gtIOInput[j].iook)
			{
				status |= bit;
			}
			bit <<= 1;
		}
		
		hyhwGpio_setHigh(gtIOOutput[i].port, gtIOOutput[i].iobit);
	}
#endif
	
	//�Ƿ�����ɨ��
	if(Grap_GetCurDesk_ScanEnable() > 0)
	{
		//ɨ��trig IO����
		scanKey = (status&BIT1||status&BIT2||status&BIT3);
		if(gu8AutoScan == 0)//�Զ�ɨ���
		{
			if(laser == 0 && scanKey != 0)
			{
				hyhwScanner_TrigCtrl(1);
				laser = 1;
			}
			else if(laser == 1 && scanKey == 0)
			{
				hyhwScanner_TrigCtrl(0);
				laser = 0;
			}
			//if((status&BIT3) && scanKey)//F1+scan  ���Զ�ɨ��
			{
				laser = 0;
				gu8AutoScan = 2;
				gsScanKeyPushTime = 0;
				Led_ctrl(LED_RED);
				hyhwScanner_TrigCtrl(1);
			}
		}
		else if(gu8AutoScan == 2 && scanKey == 0)
		{
			gu8AutoScan = 1;
			gu32LastAutoScanTime = rawtime(NULL);
		}
		else if(gu8AutoScan == 3 && scanKey == 0)
		{
			gu8AutoScan = 0;
		}
		else if(gu8AutoScan == 1)//�Զ�ɨ��
		{
			if(gsScanKeyPushTime == 0)
			{
				gsScanKeyPushTime = 400;
				laser = 0;
			}
			else
			{
				//ģ��ɨ�谴������
				if(gsScanKeyPushTime > 0) gsScanKeyPushTime--;
				if (laser == 0 && gsScanKeyPushTime >= 150)
				{
					hyhwScanner_TrigCtrl(1);
					laser = 1;
				}
				else if(laser == 1 && gsScanKeyPushTime < 150)
				{
					hyhwScanner_TrigCtrl(0);
					laser = 0;
				}
			}
			
			if(ABS(rawtime(NULL)-gu32LastAutoScanTime) >= CLOSE_AUTOSCAN_MAXTIME || (status&BIT3) && scanKey) //���Զ�ɨ��
			{
				hyhwScanner_TrigCtrl(0);
				laser = 0;
				gu8AutoScan = 3;
				gsScanKeyPushTime = 0;
				Led_ctrl(LED_CLOSE);
			}
		}
		
		//���ɨ���
		status &= ~(BIT1|BIT2|BIT3);
	}
	else if(gu8AutoScan == 1)
	{
		hyhwScanner_TrigCtrl(0);
		laser = 0;
		gu8AutoScan = 0;
		gsScanKeyPushTime = 0;
		Led_ctrl(LED_CLOSE);
	}
	
    return status;
} 


