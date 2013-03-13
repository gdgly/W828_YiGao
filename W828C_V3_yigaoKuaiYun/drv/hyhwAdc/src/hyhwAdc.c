/*******************************************************************************
*  (C) Copyright 2010 hyco, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of hyco and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  hyco and is confidential in nature.  Its use and dissemination by    
*  any party other than hyco is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/

/**
 * \addtogroup hyhwAdc
 * @{
 */

/** 
 * @file hyhwAdc.c
 * @brief The source code for the ADC (hardware layer).
 *
 * The source code for the ADC (hardware layer).
 * 
 */

/*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"

/*------------------------------------------------------------------------------
Project include files:
------------------------------------------------------------------------------*/
#include "hyhwCkc.h"
#include "hyhwAdc.h"
#include "hyhwGpio.h"


/*------------------------------------------------------------------------------
Local Types and defines:
------------------------------------------------------------------------------*/

static volatile U16 sgADC_Value[8];			//���ڱ������ͨ����ADCֵ
static tADC_CTRL gtAdcCtrl;

#define ADC_COMPULSORYREAD_LIMIT		100000
#define ADC_ERR_VALUE				    0x3ff//�����ADCֵ
/*------------------------------------------------------------------------------
Global variables:
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Static prototypes:
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Exported functions:
------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
* ����:	hyhwAdc_HwInit
* ����:	��ʼ��A/D���ֵļĴ���
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwAdc_HwInit(void)
{
	/* ��ADCģ�鲢����ADC clk =2.0M */
	hyhwCpm_setAdcClk();
	CONTROL0_REGISTER |= CON_BIT_STANDBY_MODEL;
	CONFIG_REGISTER   |= (0x00000f00|CFGR_BIT_APD_EN|CFGR_BIT_SM_EN);
}
/*-----------------------------------------------------------------------------
* ����:	hyhwAdc_Init
* ����:	��ʼ��A/D���ֵļĴ�����static����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwAdc_Init(void)
{
	U8 i;

	for(i=0;i<8;i++)
	{
		sgADC_Value[i] = ADC_ERR_VALUE; 
	}
	/* ��ADCģ�鲢����ADC clk =2.0M */
	hyhwCpm_setAdcClk();
	CONTROL0_REGISTER |= CON_BIT_STANDBY_MODEL;
	CONFIG_REGISTER   |= (0x00000f00|CFGR_BIT_APD_EN|CFGR_BIT_SM_EN);
	
	memset(&gtAdcCtrl,0,sizeof(tADC_CTRL));
}

/*-----------------------------------------------------------------------------
* ����:	hyhwAdc_PowerIdle();
* ����:	set adc power idle
* ����:	none
* ����: none
*			
*----------------------------------------------------------------------------*/
void hyhwAdc_PowerIdle(void)
{
	CONTROL0_REGISTER |= CON_BIT_STANDBY_MODEL;
	//�ر�ADCʱ��
	CKC_PCK_ADC_REGISTER &= (~MODEL_BIT_ENABLE);
}
/*-----------------------------------------------------------------------------
* ����:	hyhwAdc_start
* ����:	����ADת��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwAdc_start(void)
{
	U32 rc = HY_OK;
	U32 i,j,reg;
	U32 stausReg;
	U32 curTick;
	S32 index;
	//static U8 tempCnt = 0;
	
	if (0 == gtAdcCtrl.curChl && 0 == gtAdcCtrl.converFlag)
	{
	    CONFIG_REGISTER   &= ~(CFGR_BIT_APD_EN|CFGR_BIT_SM_EN);
	}
	
	curTick = rawtime(NULL);
	
	if (gtAdcCtrl.lastTick != curTick && gtAdcCtrl.converFlag == 0)
	{		 
	    CONTROL0_REGISTER = gtAdcCtrl.curChl;
    	gtAdcCtrl.converFlag = 1;
    	gtAdcCtrl.cvtTick = curTick;
    	return;
	}
	
    if (!(ADC_DATA_REGISTER & DATA_BIT_STATUS_FLAG)) 
    {
        S32 diff;
        
        diff = (curTick>=gtAdcCtrl.cvtTick) ? (curTick-gtAdcCtrl.cvtTick):(gtAdcCtrl.cvtTick-curTick);
        
        if (ABS(diff) > 20)
        {
        	 hyhwAdc_Init();
        }
                
        return;    
	}
	
	stausReg = STATUS_REGISTER;
	
	index = (stausReg>>16) & 0x0f; 
	
	if (index < ADCNUM)
	{
	    sgADC_Value[index] = stausReg&0x3ff;
	    
	    //if (2 == index && tempCnt == 0) 
	    //PhoneTrace(0,"chl:%x",sgADC_Value[index]);
	    
	    //tempCnt++;
	    //if (tempCnt > 100) tempCnt = 0;
	}
	
	gtAdcCtrl.lastTick = rawtime(NULL);	

    gtAdcCtrl.curChl = 0;//(gtAdcCtrl.curChl==0) ? 7:0;

	gtAdcCtrl.converFlag = 0;
	
	//PhoneTrace(0,"---%x,  %x",sgADC_Value[0],sgADC_Value[7]);

	//return rc;
}
/*-----------------------------------------------------------------------------
* ����:	hyhwAdc_Read
* ����:	��ȡָ��ͨ����ADC��ÿ��ֻ�ܶ�ȡһ��ͨ����ֵ���������Ѿ�ת���õ�ֵ
*       ����û��ת�������Ѿ�������channel��ADֵΪADC_ERR_VALUE
* ����:	channel--ָ����ͨ��(ADC_CH0 ~ ADC_CH3)
* ����:	ADCֵ
*----------------------------------------------------------------------------*/
U16 hyhwAdc_Read(U8 channel)
{
	//�������		
	if(channel > ADCNUM)
	{
		return HY_ERR_BAD_PARAMETER;
	}
	
//	CONTROL0_REGISTER |= CON_BIT_STANDBY_MODEL;
//	CONFIG_REGISTER   |= (0x00000F00|CFGR_BIT_APD_EN|CFGR_BIT_SM_EN);
	//sgADC_Value[channel] = ADC_ERR_VALUE; 
	
	return (sgADC_Value[channel] & 0x3ff);
}

/*-----------------------------------------------------------------------------
* ����:	hyhwAdc_SetDefault
* ����:	��ָ����ͨ�����ó�Ĭ�ϵ�ֵ
* ����:	channel--ָ����ͨ��(ADC_CH0 ~ ADC_CH3)
* ����:	ADCֵ
*----------------------------------------------------------------------------*/
void hyhwAdc_SetDefault(U8 channel)
{
     sgADC_Value[channel] = 0x3ff;
}



