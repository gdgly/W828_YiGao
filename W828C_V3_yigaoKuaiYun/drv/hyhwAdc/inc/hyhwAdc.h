#ifndef TM_HW_API_ADC_H_
#define TM_HW_API_ADC_H_

/*------------------------------------------------------------------------------
Standard include files:
------------------------------------------------------------------------------*/

#ifdef __cplusplus
	/*  Assume C declarations for C++ */
	extern "C" {
#endif

/*------------------------------------------------------------------------------
Project include files:
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
Types and defines:
------------------------------------------------------------------------------*/


#define	CONTROL0_REGISTER				TCC7901_ADC_ADCCON_REGISTER					/* ADC Control Register */
	#define	CON_BIT_STANDBY_MODEL		BIT4
	#define CON_BIT_IN_CH0				0x0
	#define CON_BIT_IN_CH1				0x1
	#define CON_BIT_IN_CH2				0x2
	#define CON_BIT_IN_CH3				0x3
	#define CON_BIT_IN_CH4				0x4
	#define CON_BIT_IN_CH5				0x5
	#define CON_BIT_IN_CH6				0x6
	#define CON_BIT_IN_CH7				0x7
	
	
#define	ADC_DATA_REGISTER				TCC7901_ADC_ADCDATA_REGISTER				/* ADC Data Register */
	#define DATA_BIT_STATUS_FLAG		BIT0
	
#define	CONTROL1_REGISTER				TCC7901_ADC_ADCCONA_REGISTER				/* ADC Control Register */
#define	STATUS_REGISTER					TCC7901_ADC_ADCSTATUS_REGISTER				/* ADC Status Register */
#define	CONFIG_REGISTER					TCC7901_ADC_ADCCFG_REGISTER					/* ADC Configuration Register */
	#define	CFGR_BIT_NEOC				~BIT7
	#define	CFGR_BIT_IRQE_EN			BIT3	
	#define	CFGR_BIT_R8_8BIT			BIT2
	#define	CFGR_BIT_APD_EN				BIT1
	#define	CFGR_BIT_SM_EN				BIT0	

typedef struct _tADC_CTRL
{
    unsigned char  converFlag; //״̬0 ����ʼת�� 1��ת�����
    unsigned char  curChl; //��ǰͨ��
    unsigned int   lastTick; //�ϴε�tick
    unsigned int   cvtTick;//��ʼת����tick��
    
}tADC_CTRL;


//ADC ͨ��
#define ADCNUM		8			

#define ADC_CH7		7
#define ADC_CH6		6
#define ADC_CH5		5
#define ADC_CH4		4
#define ADC_CH3		3
#define ADC_CH2		2
#define ADC_CH1		1
#define ADC_CH0		0

/*------------------------------------------------------------------------------
Exported Global variables:
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Exported Function protoypes:
------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
* ����:	hyhwAdc_Init
* ����:	��ʼ��A/D���ֵļĴ�����static����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwAdc_Init(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwAdc_PowerIdle();
* ����:	set adc power idle
* ����:	none
* ����: none
*			
*----------------------------------------------------------------------------*/
void hyhwAdc_PowerIdle(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwAdc_start
* ����:	����ADת��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwAdc_start(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwAdc_Read
* ����:	��ȡָ��ͨ����ADC��ÿ��ֻ�ܶ�ȡһ��ͨ����ֵ���������Ѿ�ת���õ�ֵ
*       ����û��ת�������Ѿ�������channel��ADֵΪADC_ERR_VALUE
* ����:	channel--ָ����ͨ��(ADC_CH0 ~ ADC_CH3)
* ����:	ADCֵ
*----------------------------------------------------------------------------*/
unsigned short hyhwAdc_Read(unsigned char channel);

#ifdef __cplusplus
}
#endif

#endif /* TM_HW_API_ADC_H_ */
