
#ifndef _HYHW_AK4183_H_
#define _HYHW_AK4183_H_

/*************************************************************************
 * INCLUDES
 *************************************************************************/
#include "hyTypes.h"

#if 1  //CAD0 = 0
#define AK4183_WR_Addr			(0x90)
#define AK4183_RD_Addr			(0x91)
#else  //CAD0 = 1
#define AK4183_WR_Addr			(0x92)
#define AK4183_RD_Addr			(0x93)
#endif

#endif

/*-----------------------------------------------------------------------------
* ����:	hyhwAk4183_sendCmd();
* ����:	send command to Ak4183
* ����:	none
* ����: none
*			
*----------------------------------------------------------------------------*/
//#define SOFT_I2C
U32 hyhwAk4183_sendCmd(unsigned short Cmd);

/*-----------------------------------------------------------------------------
* ����:	hyhwAk4183_readCmd();
* ����:	read command to Ak4183
* ����:	none
* ����: none
*			
*----------------------------------------------------------------------------*/
U32 hyhwAk4183_readCmd(unsigned char *dataBuff,unsigned short num);

/*-----------------------------------------------------------------------------
* ����:	hyhwAk4183_Initial(void);
* ����:	initial Ak4183
* ����:	none
* ����: none
*----------------------------------------------------------------------------*/
void  hyhwAk4183_Initial(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwAk4183_PendDetect(void);
* ����:	detect pend interrupt
* ����:	none
* ����: none
*----------------------------------------------------------------------------*/
U32 hyhwAk4183_PenDetect(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwAk4183_PenDetectHi_z
* ����:	��ak4183 PENIRQ��Ϊ����̬
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwAk4183_PenDetectHi_z(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwAk4183_SleepAndIntOn
* ����:	��ak4183Ϊ�ڵ�ģʽ��������PENIRQ�����д���ʱ�õ�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwAk4183_SleepAndIntOn(void);

/************************************************************************
 * Function            : hyhwAk4183_TSC_12bitout
 * Description         : write to ADC
 * parameter:		   : CMD which to ADC,Over and PD
 * Return 			   : 12bit result
*************************************************************/
U16 hyhwAk4183_TSC_12bitout(unsigned char cmd,unsigned char pd);

/************************************************************************
 * Function            : hyhwAk4183_TSC_8bitout
 * Description         : write to ADC
 * parameter:		   : CMD which to ADC,Over and PD
 * Return 			   : 8bit result
*************************************************************/
U8 hyhwAk4183_TSC_8bitout(unsigned char cmd,unsigned char pd);
