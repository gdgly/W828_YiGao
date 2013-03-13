/******************** (C) COPYRIGHT 2010 hyco ********************
* File Name          : receive_barcode.c
* Author             : W302 Application Team
* Version            : V1.0
* Date               : 06/18/2010
* Description        : receive  binary barcode  data from scanner and decode
*******************************************************************************/

/*
	ʹ�������ⲿ�жϣ�һ��timer���ڼ�ʱ���ݶ�ʹ��1us����
		1. frame�жϣ���־һ֡��ʼ������data�жϣ��������ṹ��buf��
					��һ���ж�ʱ����¼��ǰ��ǰ֡������
		2. data�жϣ���־���ݵ����ȡdata��ƽ����¼timer����(����ƽ����ʱ��)��
					ע�⣺ÿ��data�жϼ�¼������һ�ε�ƽ����ƽ����ʱ�䣬
							���Ե�ƽ����ȡ��
	
	����data��ƽ�Ļ��䣬Լ����frame�źű�Ե��data����ֵԽ��
		��������ʱ�����뿼��data��ƽλ����frame�źŵľ���
		ɨ��ͷ�������ľ���Ҳ��Ӱ�����ֵ
		
*/


/* Includes ------------------------------------------------------------------*/
#include "hyTypes.h"

#include "hyhwGPIO.h"
#include "hyhwTimer.h"
#include "hyhwIntCtrl.h"

#include "barcode_api.h"
#include "scanner.h"
#include "hardWare_Resource.h"

//#define TEST_DECODE
#define BAR_DECODE_MULTI_1

//#define HONYWELL_MODULE			/* honywell��ɨ���ͷģ�� */

//#define TEST_WRITE

#include "AbstractFileSystem.h"
#ifdef TEST_WRITE
#define BAR_FILE	"C:/bar.txt"
int gbarfp;

#endif


/* Ԥ���� */
//#define TEST_GPIO
#ifdef TEST_GPIO
	#define TESTSCAN_PORT	PORT_E//PORT_F//
	#define TESTSCAN_GPIO	GPIO_12//GPIO_31//
#endif
/* IO�����ƶ��� */

#define BARAPP_READ_FRAME			hyhwGpio_Read(SCANNER_FRAME_PORT, SCANNER_FRAME_LINE)
#define BARAPP_READ_RAW_DATA		hyhwGpio_Read(SCANNER_DATA_PORT, SCANNER_DATA_LINE)

#define	BARAPP_LASER_ENABLE			hyhwGpio_setHigh(SCANNER_LASER_CTRL_PORT, SCANNER_LASER_CTRL_LINE)
#define	BARAPP_LASER_DISABLE		hyhwGpio_setLow(SCANNER_LASER_CTRL_PORT, SCANNER_LASER_CTRL_LINE)

#define	BARAPP_PENDULUM_ENABLE		hyhwGpio_setHigh(SCANNER_PENDULUM_CTRL_PORT, SCANNER_PENDULUM_CTRL_LINE)
#define	BARAPP_PENDULUM_DISABLE		hyhwGpio_setLow(SCANNER_PENDULUM_CTRL_PORT, SCANNER_PENDULUM_CTRL_LINE)



/* �жϿ���Ԥ���� */
#define BARAPP_FRAME_EXTINT_LINE		INT_BIT_EI0		//FRAMEʹ�õ��� GPIO_C30
#define BARAPP_FRAME_EXTINT_ENABLE		hyhwInt_enable(BARAPP_FRAME_EXTINT_LINE)
#define BARAPP_FRAME_EXTINT_DISABLE		hyhwInt_disable(BARAPP_FRAME_EXTINT_LINE)
#define BARAPP_FRAME_EXTINT_CLEAR		hyhwInt_clear(BARAPP_FRAME_EXTINT_LINE)

#define BARAPP_DATA_EXTINT_LINE			INT_BIT_EI1		//DATAʹ�õ���GPIO_A2
#define BARAPP_DATA_EXTINT_ENABLE		hyhwInt_enable(BARAPP_DATA_EXTINT_LINE)
#define BARAPP_DATA_EXTINT_DISABLE		hyhwInt_disable(BARAPP_DATA_EXTINT_LINE)
#define BARAPP_DATA_EXTINT_CLEAR		hyhwInt_clear(BARAPP_DATA_EXTINT_LINE)

#define BARAPP_TIMER_BASE				5
#define BARAPP_TIMER_ENABLE				hyhwTimer_countEnable(BARAPP_TIMER_BASE)
#define BARAPP_TIMER_DISABLE			hyhwTimer_countDisable(BARAPP_TIMER_BASE)
#define BARAPP_TIMER_COUNTER_CLEAR		hyhwTimer_setConter(BARAPP_TIMER_BASE, 0)
#define BARAPP_TIMER_COUNTER_READ(pCount)		hyhwTimer_readConter(BARAPP_TIMER_BASE, pCount)
#define BARAPP_TIMER_LOADVALUE			(30000)		/*	30ms TIMER������Χ��
														�ּٶ�һ��frame���ᳬ��30ms
													*/

#define BARAPP_INT_ALL_ENABLE		
#define BARAPP_INT_ALL_DISABLE		


#define BARAPP_FARME_STABLE_IDX		1//4//  /* ��Ϊ�����ȶ���֡����Ҫʵ�� */

#define BARAPP_MIN_SUCCESS_NUM		1//2//
#define BARAPP_LASER_LIGHT_TIME		300	/* ��������ʱ�䣬3�룬10msΪ��λ */

#define BAR_CODE_DATA_MAX_LEN		(512)		/* ����data��buf��󳤶�(S16) */
#define BARDATA_SIZE				(2)			/* һ�����ݵ��ֽ��� */
#define BARDATA_BUF_NUM				(2)//(5)//			/* ������������buf�ĸ��� */

#define BARDATA_VALID_NUM_MIN		20	/* �ɽ������Ч������ͷ�ֵ */
#define BARDATA_VALID_NUM_MAX		(BAR_CODE_DATA_MAX_LEN-50)	/* �ɽ������Ч������߷�ֵ */

#define BARDATA_MAX_LEN				(48)//(100)

#define SCAN_DIRECTION_POSITIVE		1


/* ����buf״̬��ö�� */
typedef enum bardata_buf_state_en
{
	BARDATA_BUF_IDLE = 0,				/* ����״̬��buf���������� */
	BARDATA_BUF_RECEIVING,				/* ����״̬��buf�����ݲ������ڽ��� */
	BARDATA_BUF_RECEIVE_OK,				/* �������״̬��buf�����ݿ����ڽ��� */
	BARDATA_BUF_DECODING				/* ����״̬��buf�������������ڽ���
											��ʱ���������ڽ����µ�����
											������ɺ󣬴���IDLE״̬
										*/
}BARDATA_BUF_STATE_en;

/* bar���ݽṹ */
typedef struct bardata_buf_st
{
	S16	data[BAR_CODE_DATA_MAX_LEN];	/* ɨ������buf */
	U8	scanDirect;						/* ɨ�跽��0---����1---���� */
	BARDATA_BUF_STATE_en satate;		/* ����buf��״̬��������ɺ���IDLE״̬ */
	U16	validNum;						/* ����buf�е���Ч���ݸ�����buf����IDLEʱ���ñ�����Ҫ���� */
	
}BARDATA_BUF_st;

/*	�˽ṹ�������������Ӧ�õģ������������Ҫ�ı����Ͳɼ�������Ҫ�ı���  */
typedef struct barcode_app_st
{
	S8	decodeBuf_idx;					/* �������ݵ�buffer index */
	S8	recvBuf_idx;					/* �������ݵ�buffer index */
	
	U16	frameIdx;						/* frame ֡�źŵ���������ʼ��һЩ֡����Ҫ���� */
	
	U8	minSuccessNum;					/* ����ɹ��Ĵ���Ҫ����Ҫ����ɹ����Σ�������ɹ� */
	
	BARCODE_PIC tPic;					/* �������� */

}BARCODE_APP_st;

BARDATA_BUF_st	gtbarDataBuf[BARDATA_BUF_NUM];
BARCODE_APP_st	gtbarApp;
void *gbarAppDec_handle;

U8	barDecode[BARDATA_MAX_LEN];	/* ����Ҫ�Ľ���ɹ��Ĵ�������1ʱ����Ҫ���������� */
#ifndef BAR_DECODE_MULTI_1
U8	tempbarDecode[BARDATA_MAX_LEN];
U8	tempBarlength;
U8	tempScanDirect;
#endif
U8	barDecodeSuccNum;
U8	barDecodeFailNum;

//U8	barDecode_bak[BARDATA_MAX_LEN];
//U8	barLen_bak;
//U32	barDecodeOkTick;
//BARCODE_CODE_TYPE codeType_bak;

U8	gScanner_state = 0;			/* ɨ��ģ��״̬��0---�رգ�1---���� */
//U8	gScanKey_lastState;		/* ɨ�������һ��״̬��0---�ͷţ�1---���� */
U32	gLaserStartTime;		/* ���ⳬʱ�رյ���ʼʱ��(ticks) */
U32	gLaserTimeOut = BARAPP_LASER_LIGHT_TIME;			/* ���ⳬʱʱ��(ticks) */

#ifdef BAR_DECODE_MULTI_1
typedef struct barcode_bak_st
{
	U8 barCode[BARDATA_MAX_LEN];
	U8 barCodeLen;
	U8 okCnt;
}BARCODE_BAK_st;
#define BARCODE_OK_NUM		(2)//(4)////ʶ��ͬһ������Ŀ�ﵽʱ����Ϊʶ��ɹ�
#define BARCODE_BAK_NUM		(3)//(10)//(6)//
BARCODE_BAK_st tbarCodeBakBuff[BARCODE_BAK_NUM];
//U8 barCodeBakIndex;
#endif//BAR_DECODE_MULTI_1

#ifdef TEST_DECODE
BARCODE_CODE tTempcode;
#endif

#if 0
//for test
char *pTestBuf;
int  testSize;
#endif

//U32 scanKey_ok = 0;
//U32 barDecode_ok = 0;

void (* barDecode_WakeupDecoder)(void);


void barApp_dataExtInt_ISR(void);
static void barApp_bufRestore(void);

int barApp_getPowerStatus(void);

#ifdef TEST_DECODE
int testBarCode(char *pbarcode, int *pBarLen, BARCODE_CODE* tcode, BARDATA_BUF_st *ptbar);
#endif

#if 0
HANDLE Barcode_Open()
{
	return 0;
}
int Barcode_SetOpt(HANDLE hBc,BARCODE_OPT_TYPE eType,void *pParam)
{
	return BARCODE_ERROR;
}
int Barcode_Decode(HANDLE hBc,BARCODE_PIC *pPic,BARCODE_CODE *pCode)
{
	return BARCODE_ERROR;
}
void Barcode_Close(HANDLE hBc)
{
}
#endif

/*-------------------------------------------------------------------------
* ����:	barScannerHW_init
* ˵��:	ɨ��ģ�����IO��ʼ��
* ����:	��
* ����:	��
* -------------------------------------------------------------------------*/
void barScannerHW_init(void)
{
	
	/* ������ƿ� laser Ctrl */
	hyhwGpio_setAsGpio(SCANNER_LASER_CTRL_PORT, SCANNER_LASER_CTRL_LINE);
	hyhwGpio_setOut(SCANNER_LASER_CTRL_PORT, SCANNER_LASER_CTRL_LINE);
	//hyhwGpio_setHigh(SCANNER_LASER_CTRL_PORT, SCANNER_LASER_CTRL_LINE);
	hyhwGpio_setLow(SCANNER_LASER_CTRL_PORT, SCANNER_LASER_CTRL_LINE);
	/* ��Ƭ���ƿ� PENDULUM Ctrl */
	hyhwGpio_setAsGpio(SCANNER_PENDULUM_CTRL_PORT, SCANNER_PENDULUM_CTRL_LINE);
	hyhwGpio_setOut(SCANNER_PENDULUM_CTRL_PORT, SCANNER_PENDULUM_CTRL_LINE);
	//hyhwGpio_setHigh(SCANNER_PENDULUM_CTRL_PORT, SCANNER_PENDULUM_CTRL_LINE);
	hyhwGpio_setLow(SCANNER_PENDULUM_CTRL_PORT, SCANNER_PENDULUM_CTRL_LINE);

	/* ��ͷ֡�ź� */
	hyhwGpio_setAsGpio(SCANNER_FRAME_PORT, SCANNER_FRAME_LINE);
	hyhwGpio_setIn(SCANNER_FRAME_PORT, SCANNER_FRAME_LINE);
	hyhwGpio_setPullUp(SCANNER_FRAME_PORT, SCANNER_FRAME_LINE);
	/* ��ͷ�����ź� */
	hyhwGpio_setAsGpio(SCANNER_DATA_PORT, SCANNER_DATA_LINE);
	hyhwGpio_setIn(SCANNER_DATA_PORT, SCANNER_DATA_LINE);
	hyhwGpio_pullDisable(SCANNER_DATA_PORT, SCANNER_DATA_LINE);
	
	return;
}

void barApp_powerOnOff(int isEnable)
{
	hyhwScanner_powerCtrl(isEnable);
	//PhoneTrace(0, "scan power %d", isEnable);
}

#define POWER_ON			(BIT0)
#define POWER_OFF			(BIT1)

#define POWER_STATUS		(BIT4)

U8 scannerPowerFlag = 0;
/*-------------------------------------------------------------------------
* ����:	barApp_powerCtrl
* ˵��:	ɨ��ͷ��Դ���ƣ��������Դ����I2C����PMU���ƣ�
*		�������ж��е��ã��ú���ֻ����Ӧ���е���
* ����:	none
* ����:	��
* -------------------------------------------------------------------------*/
void barApp_powerCtrl(void)
{
	if ((scannerPowerFlag&POWER_ON) != 0)
	{
		if ((scannerPowerFlag&POWER_STATUS) == 0)
		{
			barApp_powerOnOff(1);
		}
		scannerPowerFlag = POWER_STATUS;//ɨ��ͷ��Դ�ѿ���
		//hyUsbPrintf("power_%x\r\n", scannerPowerFlag);
	}
	else if ((scannerPowerFlag&POWER_OFF) != 0)
	{
		if ((scannerPowerFlag&POWER_STATUS) != 0)
		{
			barApp_powerOnOff(0);
		}
		scannerPowerFlag = 0;
		//hyUsbPrintf("power_%x\r\n", scannerPowerFlag);
	}
}
/*-------------------------------------------------------------------------
* ����:	barApp_setPowerCtrlFlag
* ˵��:	����ɨ��ͷ��Դ����flag�������ж��е���
* ����:	isEnable
*			0---�رգ�����---����
* ����:	��
* -------------------------------------------------------------------------*/
void barApp_setPowerCtrlFlag(int isEnable)
{
	if (isEnable)
	{
		scannerPowerFlag |= POWER_ON;
	}
	else
	{
		scannerPowerFlag |= POWER_OFF;
	}
}

/*-------------------------------------------------------------------------
* ����:	barApp_getPowerStatus
* ˵��:	��ȡɨ��ͷ��Դ����״̬
* ����:	none
* ����:	��
*			0---�رգ�����---����
* -------------------------------------------------------------------------*/
int barApp_getPowerStatus(void)
{
	return (int)(scannerPowerFlag&POWER_STATUS);
}

/*-------------------------------------------------------------------------
* ����:	barApp_laserCtrl
* ˵��:	��ͷ�������
* ����:	isEnable
*			0---�رգ�����---����
* ����:	��
* -------------------------------------------------------------------------*/
void barApp_laserCtrl(int isEnable)
{
	if (isEnable)
	{
		BARAPP_LASER_ENABLE;
		gLaserStartTime = rawtime(NULL);//Fsk_Get_CurTime();
		
	#ifdef BAR_DECODE_MULTI_1
		//barCodeBakIndex = 0;
		memset(&tbarCodeBakBuff[0], 0, sizeof(BARCODE_BAK_st)*BARCODE_BAK_NUM);
	#endif
		
		BARAPP_INT_ALL_DISABLE;
		/* ���frame��data�жϱ�־����ֹ���ж� */
		BARAPP_FRAME_EXTINT_CLEAR;
		BARAPP_DATA_EXTINT_CLEAR;
		/* ����frame��data���ж� */
		BARAPP_FRAME_EXTINT_ENABLE;
		BARAPP_DATA_EXTINT_ENABLE;
		BARAPP_INT_ALL_ENABLE;
		
		/* timer��ʼ���� */
		BARAPP_TIMER_ENABLE;
	}
	else
	{
		BARAPP_LASER_DISABLE;
		gLaserStartTime = 0;
		
		barApp_bufRestore();
		
		BARAPP_INT_ALL_DISABLE;
		/* ��ֹframe��data�ж� */
		BARAPP_FRAME_EXTINT_DISABLE;
		BARAPP_DATA_EXTINT_DISABLE;
		/* ���frame��data�жϱ�־����ֹ���ж� */
		BARAPP_FRAME_EXTINT_CLEAR;
		BARAPP_DATA_EXTINT_CLEAR;
		BARAPP_INT_ALL_ENABLE;
		
		/* timerֹͣ���� */
		BARAPP_TIMER_DISABLE;
		
		barDecodeSuccNum = 0;
		barDecodeFailNum = 0;
	}
}

#if 1
/*-------------------------------------------------------------------------
* ����:	barApp_setLaserOutTime
* ˵��:	���ù�ͷ���ⳬʱʱ�䣬��ת��Ϊϵͳticks
* ����:	outTimeSecond---��ʱʱ��(����Ϊ��λ)
* ����:	��
* -------------------------------------------------------------------------*/
void barApp_setLaserOutTime(int outTimeSecond)
{
	gLaserTimeOut = outTimeSecond*100;//ת��Ϊϵͳticks
}
#if 0
/*-------------------------------------------------------------------------
* ����:	barApp_laserCloseTimeout
* ˵��:	��ͷ���ⳬʱ�رգ���ʱʱ��Ϊ BARAPP_LASER_LIGHT_TIME(3��)
* ����:	��
* ����:	��
* -------------------------------------------------------------------------*/
void barApp_laserCloseTimeout(void)
{
	U32 now;
	
	if (gLaserStartTime == 0) return;/* �Ѿ����ڹر�״̬ */
	now = rawtime(NULL);
	if ((now - gLaserStartTime) >= gLaserTimeOut)//BARAPP_LASER_LIGHT_TIME
	{
		/* �ر�ɨ��ģ�鼤�� */
		barApp_laserCtrl(0);
		//ͣ�ڹ�
		BARAPP_PENDULUM_DISABLE;
		//��ɨ��ͷ��Դ
		//barApp_powerOnOff(0);
		barApp_setPowerCtrlFlag(0);
		if (barDecode_WakeupDecoder != NULL)
		{
			barDecode_WakeupDecoder();
		}
		//hyUsbMessage("power Off_timeOut\r\n");
		
//		if (g_mcu_ctl.saveMsg.continueScan != 0)
//		{
//			gScanKey_lastState = 0;
//		}
		//test_scanTimeRestore();
	}
}
#endif
#endif


/*-------------------------------------------------------------------------
* ����:	barApp_getLaserState
* ˵��:	��ȡ��ͷ����״̬
* ����:	��
* ����:	0----�ر�״̬��1----����״̬
* -------------------------------------------------------------------------*/
int barApp_getLaserState(void)
{
	if (gLaserStartTime == 0)
	{
		//���ڹر�״̬
		return 0;
	}
	//���ڿ���״̬
	return 1;
}

#if 1
/*-------------------------------------------------------------------------
* ����:	barScanKey_laserCtrl
* ˵��:	ɨ������ͼ������
* ����:	scanKeyStatus --  �Ͳ�ɨ�������İ���״̬��0 Ϊ�����ͷţ�
*						  1-6000��ֵ����ʾ����״̬���Ҽ��Ϊ10ms��ֵ����1
* ����:	0----ɨ��������ͷ�״̬
*		1----ɨ������ڰ���״̬
* -------------------------------------------------------------------------*/
int barScanKey_laserCtrl(U32 scanKeyStatus)
{
	int rc;
	
	if (scanKeyStatus)	/* �������� */ 
	{
		if (scanKeyStatus <= gLaserTimeOut)	/* ����ʱ��С�ڳ�ʱ���� */ 
		{
			/* �ж��Ƿ�ʱ��һʱ�̵㣬��һʱ����Ҫ��ɨ�� */
			if (scanKeyStatus == gLaserTimeOut)
			{
				/* �ر�ɨ��ģ�鼤�� */
				barApp_laserCtrl(0);
				/*ͣ�ڹ�*/
				BARAPP_PENDULUM_DISABLE;
				/*��ɨ��ͷ��Դ*/
				
				//barApp_powerOnOff(0);
				
				barApp_setPowerCtrlFlag(0);
				if (barDecode_WakeupDecoder != NULL)
				{
					barDecode_WakeupDecoder();
					//_kqputlo(barDecode_WakeupDecoder, NULL);
				}
			}
			else if(scanKeyStatus == 1)
			{
				/*���¼���1�����㣬��ɨ��ͷ��Դ�Ͱڹ�*/
				
				//barApp_powerOnOff(1);
				//scanKey_ok++;
				barApp_setPowerCtrlFlag(1);
				if (barDecode_WakeupDecoder != NULL)
				{
					barDecode_WakeupDecoder();
					//_kqputlo(barDecode_WakeupDecoder, NULL);
				}
				
				/*���ڹ�*/
				BARAPP_PENDULUM_ENABLE;
				//hyUsbMessage("power On_Key\r\n");
			}
			else if(scanKeyStatus == 2)
			{
				/* ���¼���2�����㣬����ɨ��ģ�鼤�� */
				barApp_laserCtrl(1);
			}
		}
		
		rc = 1;
	}
	else
	{
		/* ɨ����ͷ� */
		//if (barApp_getLaserState() != 0)
		if (barApp_getPowerStatus() != 0)
		{
			/* �ر�ɨ��ģ�鼤�� */
			barApp_laserCtrl(0);
			//ͣ�ڹ�
			BARAPP_PENDULUM_DISABLE;
			//��ɨ��ͷ��Դ
			//barApp_powerOnOff(0);
			barApp_setPowerCtrlFlag(0);
			if (barDecode_WakeupDecoder != NULL)
			{
				barDecode_WakeupDecoder();
			}
			//hyUsbMessage("power Off_noKey\r\n");
		}

		rc = 0;
	}
	
	return rc;
}
#endif

void barDecode_Wakeup_Config(void(*pHandle)(void))
{
	barDecode_WakeupDecoder = pHandle;
}
/*-------------------------------------------------------------------------
* ����:	barApp_frameExtInt_ISR
* ˵��:	��ͷ֡�ź��ⲿ�жϵĴ�����(���ж�)������������У�����ʹ������buffer��
*		һ��buf���ڽ��ղɼ������ݣ���һ�����ڽ���
* ����:	��
* ����:	��
* -------------------------------------------------------------------------*/
void barApp_frameExtInt_ISR(void)
{
	int i;
	BARDATA_BUF_st	*ptbarDataBuf;
	S8 recvBuf_idx;
	U16	frameIdx, validNum;
	U8 scanDirect;
	
	recvBuf_idx = gtbarApp.recvBuf_idx;
	
	gtbarApp.frameIdx++;
	frameIdx = gtbarApp.frameIdx;
	
	/* ���ǵ�������ȶ���ǰ�漸֡���� */
	/* frame���жϼ���ǰһ֡�Ľ�����Ҳ�ǵ�ǰ֡�Ŀ�ʼ */
	if(frameIdx > BARAPP_FARME_STABLE_IDX)
	{
		/* ��ȡframe�ڣ��Լ�¼ɨ�跽�� */
		scanDirect = BARAPP_READ_FRAME;
		/* ������һ֡ */
		ptbarDataBuf = &gtbarDataBuf[recvBuf_idx];
		if (ptbarDataBuf->satate == BARDATA_BUF_RECEIVING)
		{
			/* ��¼���һ����ƽ�ĳ���ʱ�� */
			barApp_dataExtInt_ISR();
			//�������һ����ƽ�ļ�¼
			validNum = ptbarDataBuf->validNum;
			if (validNum>0 && validNum<=BARDATA_VALID_NUM_MAX)
			{
				S16 barData;
				validNum -= 1;
				barData = 0-ptbarDataBuf->data[validNum];
				ptbarDataBuf->data[validNum] = barData;
			}
			ptbarDataBuf->satate = BARDATA_BUF_RECEIVE_OK;
			/*if (barDecode_WakeupDecoder != NULL)
			{
				barDecode_WakeupDecoder();
			}*/
		}
		
		/* �µ�һ֡�����¿�ʼ���� */
		BARAPP_TIMER_COUNTER_CLEAR;
		
		/* ���ҿ����ڽ������ݵ�buf */
		recvBuf_idx++;
		if (recvBuf_idx >= BARDATA_BUF_NUM)
		{
			recvBuf_idx = 0;
		}
		for (i=0; i<BARDATA_BUF_NUM; i++)
		{
			if (gtbarDataBuf[recvBuf_idx].satate == BARDATA_BUF_IDLE)
			{
				break;
			}
			recvBuf_idx++;
			if (recvBuf_idx >= BARDATA_BUF_NUM)
			{
				recvBuf_idx = 0;
			}
		}
		if (i == BARDATA_BUF_NUM)
		{
			/* û�п����ڽ������ݵ�buf���´��ж����� */
		#if 0
			GPIO_ResetBits(GPIOB, GPIO_Pin_2);
			BARAPP_FRAME_EXTINT_DISABLE;
			BARAPP_DATA_EXTINT_DISABLE;
		#endif
			return;
		}
		
		/* �ҵ�һ֡�ɽ������ݣ���¼��index */
		gtbarApp.recvBuf_idx = recvBuf_idx;
		
		ptbarDataBuf = &gtbarDataBuf[recvBuf_idx];
		ptbarDataBuf->satate = BARDATA_BUF_RECEIVING;
		ptbarDataBuf->validNum = 0;
		if (scanDirect == SCAN_DIRECTION_POSITIVE)
		{
			scanDirect = 1;
		}
		else
		{
			scanDirect = 0;
		}
		ptbarDataBuf->scanDirect = scanDirect;
		
		BARAPP_DATA_EXTINT_ENABLE;
	}
	else if(frameIdx == BARAPP_FARME_STABLE_IDX)
	{
		/* ��һ�βɼ����ݵĿ�ʼ */
		/* ����data�ⲿ�жϣ���ʼ�ɼ����� */
		//BARAPP_DATA_EXTINT_ENABLE;
	}
}

/*-------------------------------------------------------------------------
* ����:	barApp_dataExtInt_ISR
* ˵��:	��ͷ�������ź��ⲿ�жϵĴ�����(���ж�)��
*		����������У���¼���ж�֮ǰ�ĵ�ƽ����ƽ����ʱ��
* ����:	��
* ����:	��
* -------------------------------------------------------------------------*/
void barApp_dataExtInt_ISR(void)
{
	U8	data, flag;
	U32 count;
	S16	barData, tempData=0;
	U16 validNum;
	BARDATA_BUF_st	*ptbarDataBuf;
	
	/* �õ��ϴε�ƽ����ʱ�估��ǰ��ƽ(�ϴε�ƽ�뵱ǰ��ƽ�෴) */
	BARAPP_TIMER_COUNTER_READ(&count);
	BARAPP_TIMER_COUNTER_CLEAR;
	data = BARAPP_READ_RAW_DATA;
	
	ptbarDataBuf = &gtbarDataBuf[gtbarApp.recvBuf_idx];
	validNum = ptbarDataBuf->validNum;
	
	if (ptbarDataBuf->satate == BARDATA_BUF_RECEIVING)
	{
		/* ����д��buf */
		barData = 0;
		/* �ϴε�ƽ�뵱ǰ��ƽ�෴����ֵ��ʾ�͵�ƽ������ֵ��ʾ����ʱ�� */
		if (data)
		{
			barData = (S16)count;
		}
		else
		{
			barData = (S16)(0 - count);
		}
		#if 1
		if (validNum >= 1)
		{
			flag = 0;
			if (barData>0 && ptbarDataBuf->data[validNum-1]>0)
			{
				flag = 1;
				tempData = -600;
			}
			else if (barData<0 && ptbarDataBuf->data[validNum-1]<0)
			{
				flag = 1;
				tempData = 600;
			}
			if (flag == 1)
			{
				if (validNum < BAR_CODE_DATA_MAX_LEN)
				{
					ptbarDataBuf->data[validNum] = tempData;
					ptbarDataBuf->validNum++;
					validNum++;
				}
			}
		}
		#endif
		
		if (validNum < BAR_CODE_DATA_MAX_LEN)
		{
			ptbarDataBuf->data[validNum] = barData;
			ptbarDataBuf->validNum++;
		}
	}
	
	return;
}

#if 0
/*-------------------------------------------------------------------------
* ����:	barApp_timerInt_ISR
* ˵��:	
* ����:	��
* ����:	��
* -------------------------------------------------------------------------*/
void barApp_timerInt_ISR(void)
{
	if (barScanKey_laserCtrl() != 0)
	{
		/* ɨ�������ʱ�����ػ� */
		//Sys_ClearShutDownTimer();
	}
}
#endif

#if 0
void barApp_softInt_ISR(void)
{
	int	len, flag;
	U32 now;
	BARCODE_CODE_TYPE codeType;
	U8 continueScan;
	
	len = 0;
	flag = 0;
	codeType = CODE_TYPE_MAX;
	//continueScan = g_mcu_ctl.saveMsg.continueScan;
	continueScan = 0;
	now = rawtime(NULL);//Fsk_Get_CurTime();
	
	/* ��������ɨ������ */
	barApp_getBarcode(&barDecode[0], &len, &codeType);
	if (len > 0)
	{
		if (continueScan != 0)
		{
			/*	����ɨ��ģʽ�£���Ҫ�Ա�����ʶ�������Ƿ�һ�£�
				��ͬ���������ͬ�����
			*/
			if (memcmp(&barDecode_bak[0], &barDecode[0], len) == 0 &&	//������ͬ
				len == barLen_bak &&									//������ͬ
				codeType == codeType_bak)								//������ͬ
			{
				//������ͬ���������
				flag = 0;
			}
			else
			{
				//���벻ͬ�����
				memcpy(&barDecode_bak[0], &barDecode[0], len);
				barLen_bak = len;
				codeType_bak = codeType;
				flag = 1;
			}
		}
		else
		{
			barLen_bak = 0;
			flag = 1;
		}
		
		if (flag != 0)
		{
			//if (barDecode[0] != CODE_CTRL_FNC3)
			{
			//	beeper_start(SCAN_OK, g_mcu_ctl.saveMsg.beeperEn, 1);
			}
			barBuf_writeBarCode(&barDecode[0], len);
		}
		barDecodeOkTick = now;
		//test_scanTimeRestore();
	}
	else
	{
		if (continueScan != 0)
		{
			if ((now-barDecodeOkTick) > 50)
			{
				barDecodeOkTick = now;
				barLen_bak = 0;
			}
		}
	}
}
#else
int barApp_decodeBarcode(void)
{
	int	len;
	BARCODE_CODE_TYPE codeType;
	
	barApp_getBarcode(&barDecode[0], &len, &codeType);
	if (len > 0)
	{
		barBuf_writeBarCode(&barDecode[0], len);
	}
	return len;
}
#endif

#if 0
void barApp_fullBakInfo(void)
{
	barLen_bak = 8;
	codeType_bak = CODE_TYPE_CODE128;
	barDecode_bak[0] = CODE_CTRL_FNC3;
	memcpy(&barDecode_bak[1], "010501", 7);
}
#endif
/*-------------------------------------------------------------------------
* ����:	barData_FrameInt_init
* ˵��:	��ʼ��frame�ڵ��ⲿ�жϣ����ж�
* ����:	��
* ����:	��
* -------------------------------------------------------------------------*/
void barData_FrameInt_init(void)
{
	//hyhwGpio_IntenGpioc30(EINT0SEL);			/* ����ָ�� GPIO Ϊ�жϹ��� GPIO_C30 ExternInt0 */
//	hyhwGpio_IntenHpcsn_l(EINT0SEL);			/* ����ָ�� GPIO Ϊ�жϹ��� GPIO_F24 ExternInt0 */
	hyhwGpio_IntenHpxd3(EINT0SEL);				/* ����ָ�� GPIO Ϊ�жϹ��� GPIO_F3 ExternInt0 */
	//hyhwExtern5_setTriggerFallingEdge();		/* ExternInt0 ���жϳ������� */
 	INT_TRIGGER_MODE_REGISTER &= (~INT_BIT_EI0);/* �ش��� */
 	INT_MODEA_REGISTER |= INT_BIT_EI0;			/* ������&�½��ش��� */
 	INT_INTMASK_REGISTER |= INT_BIT_EI0;
 	
 	hyhwInt_setFIQ(INT_BIT_EI0);				/* �ж�ΪFIQ���� */
	//hyhwInt_enable(INT_BIT_EI0);				/* ʹ�� */
	
	hyhwInt_ConfigScannerFrame(barApp_frameExtInt_ISR);
	BARAPP_FRAME_EXTINT_DISABLE;
	BARAPP_FRAME_EXTINT_CLEAR;
}

/*-------------------------------------------------------------------------
* ����:	barData_DataInt_init
* ˵��:	��ʼ��data�ڵ��ⲿ�жϣ����ж�
* ����:	��
* ����:	��
* -------------------------------------------------------------------------*/
void barData_DataInt_init(void)
{
	//hyhwGpio_IntenGpioa2(EINT1SEL);			/* ����ָ�� GPIO Ϊ�жϹ��� GPIO_A2 ExternInt1 */
//	hyhwGpio_IntenHpxd3(EINT1SEL);				/* ����ָ�� GPIO Ϊ�жϹ��� GPIO_F3 ExternInt1 */
	hyhwGpio_IntenHpcsn_l(EINT1SEL);			/* ����ָ�� GPIO Ϊ�жϹ��� GPIO_F24 ExternInt1 */
	//hyhwExtern5_setTriggerFallingEdge();		/* ExternInt1 ���жϳ������� */
 	INT_TRIGGER_MODE_REGISTER &= (~INT_BIT_EI1);/* �ش��� */
 	INT_MODEA_REGISTER |= INT_BIT_EI1;			/* ������&�½��ش��� */
 	INT_INTMASK_REGISTER |= INT_BIT_EI1;
 	
 	hyhwInt_setFIQ(INT_BIT_EI1);				/* �ж�ΪFIQ���� */
	//hyhwInt_enable(INT_BIT_EI1);				/* ʹ�� */
	
	hyhwInt_ConfigScannerData(barApp_dataExtInt_ISR);
	BARAPP_DATA_EXTINT_DISABLE;
	BARAPP_DATA_EXTINT_CLEAR;
}

/*-------------------------------------------------------------------------
* ����:	barData_Timer_init
* ˵��:	��ʼ��timer����
* ����:	��
* ����:	��
* -------------------------------------------------------------------------*/
void barData_Timer_init(void)
{
	tTIMER_REGISTER	*ptTimerReg;
	
	ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR ) + BARAPP_TIMER_BASE;
	
	ptTimerReg->REFERENCE	= BARAPP_TIMER_LOADVALUE;
	ptTimerReg->CONFIG		= 0;			//��1/6us����
	
	hyhwTimer_countDisable(BARAPP_TIMER_BASE);
	hyhwTimer_intDisable(BARAPP_TIMER_BASE);
}

#define BARAPP_TEST_TIMER		(0)
#define BARAPP_TEST_LOADVALUE	(50000)
void barData_Timer_init_test(void)
{
	tTIMER_REGISTER	*ptTimerReg;
	
	ptTimerReg  = (tTIMER_REGISTER *)(TIMER_BASEADDR ) + BARAPP_TEST_TIMER;
	
	ptTimerReg->REFERENCE	= BARAPP_TEST_LOADVALUE;
	ptTimerReg->CONFIG		= 0;			//��1/6us����
	
	//hyhwTimer_countDisable(BARAPP_TEST_TIMER);
	hyhwTimer_countEnable(BARAPP_TEST_TIMER);
	hyhwTimer_intDisable(BARAPP_TEST_TIMER);
}

/*-------------------------------------------------------------------------
* ����:	barData_Init
* ˵��:	��ʼ��frame/data�ڵ��ⲿ�жϡ���timer������
* ����:	��
* ����:	��
* -------------------------------------------------------------------------*/
void barData_Init(void)
{
	barScannerHW_init();
	barData_FrameInt_init();
	barData_DataInt_init();
	barData_Timer_init();

	/* ��ʼ�����ݽṹ */
	memset(&gtbarDataBuf[0], 0x00, sizeof(BARDATA_BUF_st)*BARDATA_BUF_NUM);
	
}





static const BARCODE_OPT_DEC gtDecOption[2] = 
{
  	/* 0 -- BARCODE_CODE_TYPE_EAN128 */
	{
		BARCODE_CODE_TYPE_EAN128,
		BARCODE_DEC_MODE_COMBINED_COMBINED,
		BARCODE_DEC_MODE_COMBINED_COMBINED,
		BARCODE_DEC_RELIABLE_LOW,
		//BARCODE_DEC_RELIABLE_MIDDLE,
		1,
		1,
		2,
		50,
		BARCODE_OPT_DEC_FLAG_ENABLE
	},
	/* 2 -- BARCODE_CODE_TYPE_CODE39 */
	{
		BARCODE_CODE_TYPE_CODE39,
		BARCODE_DEC_MODE_BINARY_SINGLE,
		BARCODE_DEC_MODE_BINARY_SINGLE,
		BARCODE_DEC_RELIABLE_LOW,
		//BARCODE_DEC_RELIABLE_HIGH,
		1,
		1,
		2,
		50,
		BARCODE_OPT_DEC_FLAG_ENABLE|BARCODE_OPT_DEC_FLAG_FULL_ASCII|BARCODE_OPT_DEC_FLAG_OUTPUT_ECC
	},
};

/*-------------------------------------------------------------------------
* ����:	barcode_setOption
* ˵��:	��������ʶ�������
* ����:	barDec_handle--����handle
*		pbarcodeOpt----��ȡ���ԵĽṹָ��
* ����:	0----���óɹ���-1----������Ч
* -------------------------------------------------------------------------*/
int barcode_setOption(HANDLE barDec_handle)
{
	int ret;
	int i;
        
	BARCODE_OPT bopt;
	BARCODE_OPT_SCAN *pbscan;

	bopt.eType = BARCODE_OPT_TYPE_SCAN;
	pbscan = &bopt.tParam.tOptScan;
	
	//����˫��ɨ��
	pbscan->uFlag = BARCODE_OPT_SCAN_FLAG_BIDIRECT;
	
	//��˳�����CODE39��CODE128��2�����루�������Ͳ���⣩
	pbscan->uScanSeqNum = 2;
	pbscan->eScanSeq[0] = BARCODE_SCAN_TYPE_CODE39;
	pbscan->eScanSeq[1] = BARCODE_SCAN_TYPE_CODE128;
	
	//�������ýӿڣ�ע��pbscan�ṹ�������г�Ա��Ҫ��ȷ��ֵ
	if(Barcode_SetOpt(barDec_handle,&bopt) != BARCODE_SUCCESS)
	{
		return -1;
	}
	
	/* ʵ������ */
	bopt.eType = BARCODE_OPT_TYPE_DEC;
	for(i=0; i<2; i++)
	{
		/* ��Ͽ�ʹ�õĽṹ, ��������ò����Ľṹ*/
		memcpy((char *)&bopt.tParam.tOptDec, (char *)&gtDecOption[i], sizeof(BARCODE_OPT_DEC));
		/*�������ýӿڣ�ע��pbdec�ṹ�������г�Ա��Ҫ��ȷ��ֵ */
		ret = Barcode_SetOpt(barDec_handle, &bopt);
		if (ret != BARCODE_SUCCESS)
		{
			return -1;
		}
	}
	return 0;
}



/*-------------------------------------------------------------------------
* ����:	barApp_init
* ˵��:	�ṩ�������ߵĽӿڣ�����������У����б�Ҫ�ĳ�ʼ��
* ����:	��
* ����:	0	-- ��ʼ���ɹ�
*		-1	-- ��ʼ��ʧ��
* -------------------------------------------------------------------------*/
int barApp_init(void)
{
	int rc = 0;
	
	/* ��ʼ������˳����Ҫ�Ĳ���*/
	BARCODE_PIC			*ptPic;
	
	#if 0
	//for test
	pTestBuf = hsaSdram_UpgradeBuf();
	testSize = 0;
	#endif
	
	/* �ж���Դ��ʼ��*/
	barData_Init();
	
    /* ȫ�ֱ�����ʼ�� */
    memset((U8*)&gtbarApp, 0, sizeof(BARCODE_APP_st));
	gbarAppDec_handle = NULL;

	gtbarApp.minSuccessNum	= BARAPP_MIN_SUCCESS_NUM;
	gtbarApp.frameIdx = 0;
	
	ptPic = &gtbarApp.tPic;
	ptPic->eType					= BARCODE_PIC_TYPE_STRIPE;		/* һάY������ʽ  */
	ptPic->tData.tBImage.uWidth		= BAR_CODE_DATA_MAX_LEN;		/* ͼ���� */
	ptPic->tData.tBImage.uHeight	= 1;							/* ͼ��߶ȣ�ӦΪ1 */
	ptPic->tData.tBImage.uStartX	= 0;							/* ��ʼ��x���� */
	ptPic->tData.tBImage.uStartY	= 0;							/* ��ʼ��y���꣬ӦΪ0 */
	ptPic->tData.tBImage.uEndX		= BAR_CODE_DATA_MAX_LEN-1;		/* ��ֹ��x���� */
	ptPic->tData.tBImage.uEndY		= 0;							/* ��ֹ��y���꣬ӦΪ0 */
	ptPic->tData.tBImage.pData		= &gtbarDataBuf[0].data[0];		/* ͼ������ */

	/* ������ʼ�� */
	gbarAppDec_handle = Barcode_OpenDec(NULL);
	//hyUsbPrintf("handle === %x\r\n",gbarAppDec_handle);
	if(gbarAppDec_handle)
	{
		rc = barcode_setOption(gbarAppDec_handle);
		if (rc != 0)
		{
		   Barcode_Close(gbarAppDec_handle);
		   rc =  -1;
		}
	}
	else
	{
		rc = -1;
	}
	
#ifdef TEST_WRITE
	gbarfp = AbstractFileSystem_Open(BAR_FILE,AFS_RDWR);
	if(gbarfp >= 0)
	{
		AbstractFileSystem_Seek(gbarfp,0,AFS_SEEK_OFFSET_FROM_END);
	}
	else
	{
		gbarfp = AbstractFileSystem_Create(BAR_FILE);
	}
	if (gbarfp >= 0)
	{
		#define TIME_M	"time;"##__DATE__##" "##__TIME__
		char str[50];
		char *pTime = (char *)TIME_M;
		strcpy(str, "����ɨ��,");
		strcat(str, pTime);
		strcat(str, "\r\n");
		AbstractFileSystem_Write(gbarfp,str,strlen(str));
	}
#endif
	
	return rc;
}

/*-------------------------------------------------------------------------
* ����:	barApp_bufRestore
* ˵��:	����buf�ָ���ʼ��ֵ
* ����:	��
* ����:	��
* -------------------------------------------------------------------------*/
static void barApp_bufRestore(void)
{
	int i;
	
	for (i=0; i<BARDATA_BUF_NUM; i++)
	{
		gtbarDataBuf[i].satate = BARDATA_BUF_IDLE;
		gtbarDataBuf[i].validNum = 0;
	}
	gtbarApp.decodeBuf_idx = 0;
	gtbarApp.recvBuf_idx = 0;
	gtbarApp.frameIdx = 0;
}

/*-------------------------------------------------------------------------
* ����:	barApp_close
* ˵��:	�ṩ�������ߵĽӿڣ��رս����
* ����:	��
* ����:	��
* -------------------------------------------------------------------------*/
void barApp_close(void)
{
	Barcode_Close(gbarAppDec_handle);
}


/*-------------------------------------------------------------------------
* ����:	barScanner_Init
* ˵��:	��ʼ��ɨ��ͷ����ʼ������������⡢����ɨ��ͷ��Դ
* ����:	none
* ����:	0	- ��ʼ���ɹ�
*		-1  - ��ʼ��ʧ�ܣ���ʱû�п���ɨ��ͷ��Դ
* -------------------------------------------------------------------------*/
int barScanner_init(void)
{
	int rc;
	
	rc = barApp_init();
	if (rc == 0)
	{
		gScanner_state = 1;		//ɨ��ģ�鿪��
		//barApp_powerOnOff(1);
	}
	barApp_laserCtrl(0);
	//gScanKey_lastState = 0;
	barDecode_WakeupDecoder = NULL;
	
	barBuf_init();
	
	//barData_Timer_init_test();
	//beeper_start(SCANNER_POWERON_OK, 1, 1);
	//while(beeper_getState() == 1);
	
	//test_scanInitOk();
	#ifdef TEST_GPIO
	hyhwGpio_setAsGpio(TESTSCAN_PORT, TESTSCAN_GPIO);
	hyhwGpio_setOut(TESTSCAN_PORT, TESTSCAN_GPIO);
	hyhwGpio_setLow(TESTSCAN_PORT, TESTSCAN_GPIO);
	#endif
	
	return rc;
}

/*-------------------------------------------------------------------------
* ����:	barScanner_Init
* ˵��:	��ʼ��ɨ��ͷ����ʼ������������⡢����ɨ��ͷ��Դ
* ����:	none
* ����:	0	- ��ʼ���ɹ�
*		-1  - ��ʼ��ʧ�ܣ���ʱû�п���ɨ��ͷ��Դ
* -------------------------------------------------------------------------*/
int barScanner_close(void)
{
	Barcode_Close(gbarAppDec_handle);
	BARAPP_LASER_DISABLE;
	BARAPP_PENDULUM_DISABLE;	//��ֹ�ڹ�Ƭ�ڶ�
	//gScanKey_lastState = 0;
	gScanner_state = 0;
	barApp_powerOnOff(0);
#ifdef TEST_WRITE
	if(gbarfp >= 0)
	{
		extern int okCnt;
		extern int errCnt;
		char str[50];
		sprintf(str, "ɨ��ʶ����ȷ����:%d\r\n", okCnt);
		AbstractFileSystem_Write(gbarfp,str,strlen(str));
		sprintf(str, "ɨ��ʶ���������:%d\r\n", errCnt);
		AbstractFileSystem_Write(gbarfp,str,strlen(str));
		AbstractFileSystem_Close(gbarfp);
	}
#endif

}

/*-------------------------------------------------------------------------
* ����:	barScanner_checkValidFrame
* ˵��:	����Ƿ�����Ч��֡���ݣ��������Ч֡����ֱ�ӻ���
*		�ú�����IRQ[user_irq_handler()]�ж��е���
* ����:	none
* ����:	none
* -------------------------------------------------------------------------*/
void barScanner_checkValidFrame(void)
{
	int i, flag = 0;
	
	if (scanner_getType() != SCANNER_TYPE_OPTICON)
	{
		/* ����ŷ��ɨ��ͷ���˳� */
		return;
	}
	
	/* ŷ��ɨ��ͷ�����Ƿ��ڿ���״̬ */
	if (barApp_getLaserState() == 0)
	{
		/* �����ѹر� */
		return;
	}
	for (i=0; i<BARDATA_BUF_NUM; i++)
	{
		if (gtbarDataBuf[i].satate == BARDATA_BUF_RECEIVE_OK)
		{
			if (barDecode_WakeupDecoder != NULL)
			{
				barDecode_WakeupDecoder();
			}
			break;
		}
	}
	
	return;
}

/*-------------------------------------------------------------------------
* ����:	barScanner_getState
* ˵��:	���ɨ��ģ��״̬
* ����:	none
* ����:	0	- �ر�״̬
*		1	- ����״̬
* -------------------------------------------------------------------------*/
int barScanner_getState(void)
{
	return gScanner_state;
}

void barTestWrite(U8 *pBar, U16 *pDat, int len)
{
#ifdef TEST_WRITE
	int i,j;
	U16  ch;
	U8  *pWrite;
	
	pWrite = (U8 *)hsaSdram_UpgradeBuf();
	
	j = 0;
	
	if(gbarfp >= 0)
	{
		//д����
		AbstractFileSystem_Write(gbarfp,pBar,strlen(pBar));
		AbstractFileSystem_Write(gbarfp,"\r\n",strlen("\r\n"));
		
		//hyUsbPrintf("len == %d\r\n",len);
		//д����
		for(i = 0; i < len; i++)
		{
			ch = pDat[i];
			sprintf(&pWrite[j],"%04x ",ch);
			j+=5;
		}
		pWrite[j] = 0;
		AbstractFileSystem_Write(gbarfp,pWrite,j);
		AbstractFileSystem_Write(gbarfp,"\r\n",strlen("\r\n"));
	}
#else
	
	int i,j,fp;
	U16  ch;
	char path[50];
	U8  *pWrite;
	
	pWrite = (U8 *)hsaSdram_UpgradeBuf();
	
	j = 0;
	
	sprintf(path,"C:/%d.txt",rawtime(NULL));
	fp = AbstractFileSystem_Create(path);
	
	if(fp >= 0)
	{
		//д����
		AbstractFileSystem_Write(fp,pBar,strlen(pBar));
		AbstractFileSystem_Write(fp,"\r\n",strlen("\r\n"));
		
		//hyUsbPrintf("len == %d\r\n",len);
		//д����
		for(i = 0; i < len; i++)
		{
			ch = pDat[i];
			sprintf(&pWrite[j],"%04x ",ch);
			j+=5;
		}
		pWrite[j] = 0;
		AbstractFileSystem_Write(fp,pWrite,j);
		AbstractFileSystem_Write(fp,"\r\n",strlen("\r\n"));
		
		AbstractFileSystem_Close(fp);
	}
	
#endif
}

/*-------------------------------------------------------------------------
* ����:	barApp_getBarcode
* ˵��:	�������
* ����:	pbarcode	-- ���룬�����߸����Ĵ�������buffer
*		pbarLength	-- �������������ĳ���
* ����:	0	- �õ�һ����Ч������
*		-1  - ʧ��
* -------------------------------------------------------------------------*/
int barApp_getBarcode(U8 *pbarcode, int *pbarLength, BARCODE_CODE_TYPE *pCodeType)
{
	int	i, rc = -1;
	int decodeRet;
	int	successNum, barlength;//, tempBarlength;
	S8	decodeBuf_idx;
//	U8	tempBar[BARDATA_MAX_LEN];	/* ����Ҫ�Ľ���ɹ��Ĵ�������1ʱ����Ҫ���������� */
	BARCODE_CODE	tcode;		/* �������ص�������� */
	BARDATA_BUF_st	*ptbarDataBuf;
#ifdef BAR_DECODE_MULTI_1
	BARCODE_BAK_st	*ptbarBak;
#endif
#ifdef TEST_WRITE
	U16 valNum;
#endif
	
	successNum = 0;
	while(1)
	{
		/* ɨ��ͷ�����Ƿ��ڿ���״̬ */
		if (barApp_getLaserState() == 0)
		{
			/* �����ѹرգ����ؽ������ */
			*pbarLength = 0;
			return -1;
		}
		
		/* �ж��Ƿ��пɽ��������֡ */
		decodeBuf_idx = gtbarApp.decodeBuf_idx;
		decodeBuf_idx++;
		if (decodeBuf_idx >= BARDATA_BUF_NUM)
		{
			decodeBuf_idx = 0;
		}
		for (i=0; i<BARDATA_BUF_NUM; i++)
		{
			if (gtbarDataBuf[decodeBuf_idx].satate == BARDATA_BUF_RECEIVE_OK)
			{
				break;
			}
			decodeBuf_idx++;
			if (decodeBuf_idx >= BARDATA_BUF_NUM)
			{
				decodeBuf_idx = 0;
			}
		}
		if (i == BARDATA_BUF_NUM)
		{
			/* û�пɽ������ݣ����ؽ������ */
			*pbarLength = 0;
			return -1;
		}
		
		/* �ҵ�һ֡�ɽ������ݣ���¼��index */
		gtbarApp.decodeBuf_idx = decodeBuf_idx;
		ptbarDataBuf = &gtbarDataBuf[decodeBuf_idx];
		if (ptbarDataBuf->validNum < BARDATA_VALID_NUM_MIN ||
			ptbarDataBuf->validNum > BARDATA_VALID_NUM_MAX)
		{
			/* ��Ч����̫�ٻ�̫�ֱ࣬�ӷ��ش��� */
			ptbarDataBuf->satate = BARDATA_BUF_IDLE;
			//*pbarLength = 0;
			//return -1;
			continue;
		}
		/* ��֡�������ڽ��� */
		ptbarDataBuf->satate = BARDATA_BUF_DECODING;

		gtbarApp.tPic.tData.tBBuf.pData		= (void *)&ptbarDataBuf->data[0];
		gtbarApp.tPic.tData.tBBuf.uLen		= ptbarDataBuf->validNum*BARDATA_SIZE;
		
	#ifdef TEST_WRITE
		valNum = ptbarDataBuf->validNum;
	#endif
		if (/*ptbarDataBuf->scanDirect && */gtbarApp.frameIdx > 50)
		{
			//ptbarDataBuf->scanDirect = 1;
		}
		memset(&tcode, 0, sizeof(BARCODE_CODE));
		#ifdef TEST_GPIO
		hyhwGpio_setHigh(TESTSCAN_PORT, TESTSCAN_GPIO);
		#endif
		decodeRet = Barcode_Decode(gbarAppDec_handle, &gtbarApp.tPic, &tcode);
		//if (gtbarApp.frameIdx >= 100 && gtbarApp.frameIdx < 120)
		//{
		//	barTestWrite("TT", gtbarApp.tPic.tData.tBBuf.pData, ptbarDataBuf->validNum);
		//}
		//decodeRet = BARCODE_ERROR;
		#ifdef TEST_GPIO
		hyhwGpio_setLow(TESTSCAN_PORT, TESTSCAN_GPIO);
		#endif
		
		barDecodeSuccNum = 0;
		if (decodeRet == BARCODE_SUCCESS)
		{
			#if 1
			for (i=0; i<BARCODE_BAK_NUM; i++)
			{
				//���Ⱥ�������ͬ��������ȷ��
				if (tcode.uLen == tbarCodeBakBuff[i].barCodeLen &&
					(memcmp(tbarCodeBakBuff[i].barCode, 
							tcode.pBuf, 
							tcode.uLen) == 0))
				{
					tbarCodeBakBuff[i].okCnt++;
					if (tbarCodeBakBuff[i].okCnt >= BARCODE_OK_NUM)
					{
						//��ȷ��Ŀ�ﵽBARCODE_OK_NUM����Ϊ�ɹ�ʶ��
						barlength	= tbarCodeBakBuff[i].barCodeLen;
						memcpy(pbarcode, tbarCodeBakBuff[i].barCode, barlength);
						barDecodeSuccNum = BARAPP_MIN_SUCCESS_NUM;
					}
					break;
				}
			}
			if (i == BARCODE_BAK_NUM)
			{
				//û��ƥ��ģ��Ͱ��µ����������
				for (i=0; i<BARCODE_BAK_NUM; i++)
				{
					if (tbarCodeBakBuff[i].barCodeLen == 0)
					{
						memcpy(tbarCodeBakBuff[i].barCode, tcode.pBuf, tcode.uLen);
						tbarCodeBakBuff[i].barCodeLen = tcode.uLen;
						tbarCodeBakBuff[i].okCnt++;
						break;
					}
				}
			}
			#endif
		}
		/*else
		{
			barTestWrite("TT",gtbarApp.tPic.tData.tBBuf.pData,ptbarDataBuf->validNum);
		}*/
		/* ��֡����ʹ����� */
		ptbarDataBuf->satate = BARDATA_BUF_IDLE;
		/* �ɹ���������ﵽҪ�󣬷��سɹ� */
		if (barDecodeSuccNum >= gtbarApp.minSuccessNum)
		{
			U8 intLev;
			intLev = InterruptDisable();
			barApp_laserCtrl(0);
			//ͣ�ڹ�
			BARAPP_PENDULUM_DISABLE;
			InterruptRestore(intLev);
			
			//��ɨ��ͷ��Դ
			//barApp_powerOnOff(0);
			barApp_setPowerCtrlFlag(0);
			
			pbarcode[barlength++] = BARCODE_END_CHAR;
			*pbarLength = barlength;
			*pCodeType = tcode.eType;
		
			rc = 0;
			break;
		}
	}
	
	return rc;
}


#ifdef TEST_DECODE
int testBarCode(char *pbarcode, int *pBarLen, BARCODE_CODE* tcode, BARDATA_BUF_st *ptbar)
{
	BARCODE_CODE_TYPE type;
#if 1
	if (memcmp(&pbarcode[0], "977100707700515", *pBarLen) != 0 &&
		memcmp(&pbarcode[0], "9771007077005", *pBarLen) != 0 &&
		tcode->eType != CODE_TYPE_CODE128)
#endif
#if 0
	type = tcode->eType;
	if (type == CODE_TYPE_MATRIX25 ||
		type == CODE_TYPE_IATA25)
#endif
	{
		#if 1
		//uart ����������������
		{
			U16	validNum, strLen, i, j;
			S16	*pData;
			U8	str[30];
			
			validNum = ptbar->validNum;
			pData = &ptbar->data[0];
			for (i=0; i<validNum; i++)
			{
				sprintf(str, "0x%08X, ", (S16)*pData);
				pData++;
				if ((i>0 && ((i+1)%8)==0) ||
					(i == (validNum-1)))
				{
					strcat(str, "\r\n");
				}
				strLen = strlen(str);
				j=0;
				while(strLen--)
				{
					USART_SendData(USART1, str[j++]);
					while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
				}
			}
			//��ʾʶ������
			sprintf(str, "--barcode type: %d\r\n", tTempcode.eType);
			strLen = strlen(str);
			j=0;
			while(strLen--)
			{
				USART_SendData(USART1, str[j++]);
				while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
			}
		}
		#endif
		
		//strcat(&pbarcode[0], "_err");
		//*pBarLen += 4;
		
		return -1;
	}
	return 0;
}
#endif