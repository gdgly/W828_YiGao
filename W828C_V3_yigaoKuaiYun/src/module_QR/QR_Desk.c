#include "QR_Desk.h"
#include "hyhwCif.h"
#include "hyhwGPIO.h"
#include "hyhwCkc.h"
#include "hyhwDma.h" 
#include "grap_api.h"
#include "base.h"
#include "jpeg_api.h"
#include "AbstractFileSystem.h"
#include "hyhwIntCtrl.h"
#include "Common_Dlg.h"
#include "Common_Widget.h"
#include "qrcode_api.h"
#include "drvKeyArray.h"

HANDLE			ghqrc;
QRCODE_PIC		gpic;
QRCODE_CODE		gcode;


eQRSTATE eQRState;

U8	*gpQRcodeBuf;
U8  gu8QRRead,gu8QRWrite;
U8  gu8QRPrevDelayFrame;//Ԥ������֡��
U32 gu32QRQuitTime;//�޲���ʱ��ʱ�˳�

U8	*gpQRUseBuf;//�ⲿ�ṩ    ����2.1M
U32 gu32QRcodeLen;

/*�������ź�Ĵ洢λ��*/
U8 *gpQRQvga_y,*gpQRQvga_u,*gpQRQvga_v;
/*Ӧ�ý�����ʾʹ��*/
U8 *gpQRDisplayRgb;
/*����Ԥ����buf*/
U8 *gpQRPrev_y[QR_PREVIEW_BUF_NUM],*gpQRPrev_u[QR_PREVIEW_BUF_NUM],*gpQRPrev_v[QR_PREVIEW_BUF_NUM];

//���  92x71  δ�Խ���ɫΪ��ɫ  �Խ�����ɫΪ0X4c0
const U8 QR_Focus1[]=
{
	0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xF0,0xFF,0xFF,0x80,0x00,
	0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xF0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x30,0xDF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xB0,
	0xDF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xB0,0xD8,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,
	0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,
	0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,
	0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,
	0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,
	0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,
	0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,0xD8,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xB0,0xDF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,
	0x00,0x1F,0xFF,0xB0,0xDF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xB0,
	0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0xFF,0xFF,0x80,0x00,
	0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xF0,0xFF,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,
	0x00,0x1F,0xFF,0xF0
};

//�ڿ�   88x67  δ�Խ���ɫΪ0X679   �Խ���ɫΪ0X7e6
const U8 QR_Focus2[]=
{
	0xFF,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0xFF,0x80,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
	0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,
	0xFF
};


int QR_DecodeLibDeInit();
/*=========================================================================================*/
/*********************************************************************
* ������  : QR_SensorISR
* ����    : camera�жϴ�����
* ����    : none 
* ���    : none
* ����    :
**********************************************************************/
void QR_SensorISR()
{
	U8 rd,wr,wr2;

	rd = gu8QRRead;
	wr = gu8QRWrite;

	/* clear frame flag*/
	hyhwmCif_clearFrameFlag();
	wr2 = wr + 1;
	if(wr2 >= QR_PREVIEW_BUF_NUM)
	{
		wr2 -= QR_PREVIEW_BUF_NUM;
	}
	if(wr2 != rd)
	{
		wr = wr2;
	}
	/*if interrption coming��store the corresponding data to preview_y/u/v buffers*/
	hyhwCif_frameStart(	(U32)gpQRPrev_y[wr],(U32)gpQRPrev_u[wr],(U32)gpQRPrev_v[wr] );
	gu8QRWrite = wr;

	return;
}


static void QR_StopInterrupt()
{
	hyhwmCif_disable();
	hyhwInt_disable(INT_BIT_CAMERA);
	hyhwInt_clear(INT_BIT_CAMERA);
	hyhwmCif_intDisable();
	hyhwmCif_clearFrameFlag();
}

static int QR_HwDeInit()
{
	hyhwLcd_flashDisable();//�ر������
	
	hyhwmCif_disable();
	hyhwmCif_intDisable();
	hyhwInt_disable(INT_BIT_CAMERA);
	hyhwInt_ConfigCamera_ISR(NULL);
	hyhwmCif_clearFrameFlag();
	hyhwInt_clear(INT_BIT_CAMERA);
	hyhwInt_ConfigCodecISR(NULL);

	hyhwCmosSensor_standby_enable();

	/*�ر�Ӳ�������豸*/
	hyhwJpegG2d_Ctrl(0);
	hyhwCmosSensor_powerCtrl(EnableOff);
}

static int QR_DeInit()
{
	QR_HwDeInit();
	QR_DecodeLibDeInit();

	eQRState = QR_STATE_QUIT;
	
	return 0;
}

static int QR_ParaInit()
{
	eQRState	= QR_STATE_INIT;
	gu8QRRead	= 0;
	gu8QRWrite	= 0;
	gu32QRcodeLen = 0;
	
	gu8QRPrevDelayFrame	= 5;
	
	gu32QRQuitTime = rawtime(NULL);
	
	return 0;
}

static int QR_Alloc()
{
	U8  i;
	
	/*���ֽڶ���*/
    gpQRUseBuf = (char *)((unsigned)(gpQRUseBuf + 3) & 0xFFFFFFFC);
    
    //���ź������buf   115200
    gpQRQvga_y	= gpQRUseBuf;
	gpQRUseBuf	+= QR_PREVIEW_320_240_PIXELS;
	gpQRQvga_u	= gpQRUseBuf;
	gpQRUseBuf	+= (QR_PREVIEW_320_240_PIXELS>>2);
	gpQRQvga_v	= gpQRUseBuf;
	gpQRUseBuf	+= (QR_PREVIEW_320_240_PIXELS>>2);
	
	//���������ʾ�����ϵ�RGB����  153600
	gpQRDisplayRgb	= gpQRUseBuf;
	gpQRUseBuf	+= (QR_PREVIEW_320_240_PIXELS<<1);
	
	
	/*����Ԥ����buf   1843200*/
	for(i = 0; i < QR_PREVIEW_BUF_NUM; i++)
	{
		gpQRPrev_y[i] = gpQRUseBuf;
		gpQRUseBuf   += QR_PREVIEW_Y_SIZE;
		gpQRPrev_u[i] = gpQRUseBuf;
		gpQRUseBuf   += QR_PREVIEW_U_SIZE;
		gpQRPrev_v[i] = gpQRUseBuf;
		gpQRUseBuf   += QR_PREVIEW_V_SIZE;
	}
	
	gpQRcodeBuf = gpQRUseBuf; //1024
	//���� 2113024   (���2.1M)
	
	return 0;
}

static int QR_HwInit()
{
	hyhwCmosSensor_powerCtrl(EnableOn);
	//��Ӳ�������豸JPEG & 2D
	hyhwJpegG2d_Ctrl(1);
	//��cameraʱ��
	hyhwCpm_busClkEnable(CKC_CAM);
	//����camera���жϴ�����
	hyhwInt_ConfigCamera_ISR(QR_SensorISR);
	
	if(hyhwCmosSensor_Init() != 0)
	{
		return -1 ;
	}
	hyhwCif_init();
	//sleep(200);
	hyhwmCif_clearIntFlag();

#ifdef QR_VGA_PREVIEW	
	//����Ԥ��ģʽ
	hyhwCif_setVGAMode();   //640 x 480
	//hyhwCif_setSXGAMode(1);
#elif defined QR_QVGA_PREVIEW
	hyhwCif_setQVGAMode();
#endif
	hyhwmCif_clearFrameFlag();
	
	//ָ��INT_BIT_CAMERA�ж�ΪIRQ�ж�,��ʹ��
	hyhwInt_setIRQ(INT_BIT_CAMERA);
	hyhwInt_enable(INT_BIT_CAMERA);

	/*��ȡһ֡����*/
	hyhwCif_frameStart((U32)gpQRPrev_y[0],(U32)gpQRPrev_u[0],(U32)gpQRPrev_v[0]);

	hyhwLcd_flashDisable();//�ر������
	
	return 0;
}

void *hyQR_malloc( U32 size)
{
	return hy_malloc(size,QR_MEM_IDX);
}

void hyQR_free( void *p_memblock)
{
	hy_free(p_memblock,QR_MEM_IDX);
	
	return;
}

int QR_DecodeLibInit()
{
	QRCODE_PARAM tparam;
	
	//��ʼ���������ڴ���亯������ѡ��
	tparam.pMalloc = hyQR_malloc;
	tparam.pFree = hyQR_free;
	
	ghqrc = QRCode_OpenDec(&tparam);
	if(ghqrc == NULL)
	{//��ʼ��ʧ��
		return HY_ERROR;
	}
	return HY_OK;
}
int QR_DecodeLibDeInit()
{
	//�ͷž��
	if(ghqrc != NULL)
	{
		QRCode_Close(ghqrc);
	}
	return HY_OK;
}
static int QR_DecodeInit()
{
	QR_DecodeLibInit();
	if(ghqrc == NULL)
	{//��ʼ��ʧ��
		return HY_ERROR;
	}
	gpic.eType 	= QRCODE_PIC_TYPE_Y;	//Y������ʽ
	gpic.uWidth = QR_DECODE_DATA_W;		//ͼ����
	gpic.uHeight= QR_DECODE_DATA_H;		//ͼ��߶�
	//gpic.pData = pImage;				//ͼ������
	
	return 0;
}

static int QR_Init()
{
	if(-1 == QR_DecodeInit())
	{
		return -1;
	}
	
	QR_ParaInit();
	QR_Alloc();
	if(-1 == QR_HwInit())
	{
		return -1 ;
	}
	
	return 0;
}


/*=========================================================================================*/
const tGrapViewAttr QRViewAttr[]=
{
	{COMM_DESKBOX_ID, 0,0,240,320,0,0,0,0,TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//����
#ifdef W818C
	{BUTTON_ID_1,	  9,282, 73,34,     9,282, 82,316,    YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_2,	158,282, 73,34,   158,282,231,316,    YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
#else	
	{BUTTON_ID_2,	  9,282, 73,34,     9,282, 82,316,    YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_1,	158,282, 73,34,   158,282,231,316,    YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//ʶ��
#endif
};

const tGrapButtonPriv QRBtnPrv[]=
{
#ifdef W818C
	{(U8 *)"ʶ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK, 0,  NULL},//
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,0,  NULL},//
#else
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,0,  NULL},//
	{(U8 *)"ʶ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK, 0,  NULL},//
#endif
};

void drvLcd_WriteBMPToShowBuff( U16 *pdata,const U8 *pkBMPPointer,U16 kBMPHigh,U16 kBMPWidth,U16 kRow,U16 kColumn,U16 Fcolor)
{
	U8 u8nBit;
	U16 tempData,tempBMPWidth;
	U16 *offset;
	int i,j,m,n;

	tempBMPWidth =(U16)(kBMPWidth>>3);
	if (kBMPWidth % 8) tempBMPWidth++;

	for(i=0;i<kBMPHigh;i++)
	{
		for(j=0;j<tempBMPWidth;j++)
		{
			tempData=*(pkBMPPointer+i*tempBMPWidth+j);
			if((kBMPWidth % 8)&&(j==(tempBMPWidth-1)))
			{
				u8nBit = (U8)(kBMPWidth % 8);
			}
			else
			{
				u8nBit = 8;
			}

			//��͸��ͼƬ(ֻдǰ��ɫ)
			m=0;
			for(n=7 ; n>=(8-u8nBit) ; n--)
			{
				if((tempData>>n)&(0x01))
				{
					offset = (U16 *)((U8*)pdata+((kRow+i)*240+kColumn+j*8+m)*2);
					*offset = (U16)Fcolor;	
				}
				m++;
			}			
		}
	}
}
#define LINE_LEN	(25)
void drawPreviewFrame(U16 *pdata, U16 kBMPHigh, U16 kBMPWidth, U16 kRow, U16 kColumn, U16 Fcolor)
{
	int i, j;
	U16 *pOffset;
	
	//���Ϻ���
	for (i=0; i<LINE_LEN; i++)
	{
		pOffset = (U16 *)((U8*)pdata+(kRow*240+kColumn+i)*2);
		*pOffset = Fcolor;
	}
	//���Ϻ���
	for (i=0; i<LINE_LEN; i++)
	{
		pOffset = (U16 *)((U8*)pdata+(kRow*240+(kColumn+kBMPWidth-LINE_LEN)+i)*2);
		*pOffset = Fcolor;
	}
	//���º���
	for (i=0; i<LINE_LEN; i++)
	{
		pOffset = (U16 *)((U8*)pdata+((kRow+kBMPHigh)*240+kColumn+i)*2);
		*pOffset = Fcolor;
	}
	//���º���
	for (i=0; i<LINE_LEN; i++)
	{
		pOffset = (U16 *)((U8*)pdata+((kRow+kBMPHigh)*240+(kColumn+kBMPWidth-LINE_LEN)+i)*2);
		*pOffset = Fcolor;
	}
	
	//��������
	for (i=0; i<LINE_LEN; i++)
	{
		pOffset = (U16 *)((U8*)pdata+((kRow+i)*240+kColumn)*2);
		*pOffset = Fcolor;
	}
	//��������
	for (i=0; i<LINE_LEN; i++)
	{
		pOffset = (U16 *)((U8*)pdata+((kRow+i)*240+kColumn+kBMPWidth)*2);
		*pOffset = Fcolor;
	}
	//��������
	for (i=0; i<LINE_LEN; i++)
	{
		pOffset = (U16 *)((U8*)pdata+((kRow+kBMPHigh-LINE_LEN+i)*240+kColumn)*2);
		*pOffset = Fcolor;
	}
	//��������
	for (i=0; i<LINE_LEN; i++)
	{
		pOffset = (U16 *)((U8*)pdata+((kRow+kBMPHigh-LINE_LEN+i)*240+kColumn+kBMPWidth)*2);
		*pOffset = Fcolor;
	}
}

void QR_Frame(U16 *ShowBuffer, U16 fcolor1, U16 fcolor2, U8 flag)
{
	U16  *p1,*p2;
	U32 len;
	U16 leftS;
	U16 i,j,alpha;
	U16 r,g,b, t,t1,t2;
	
	if(flag == 1)
	{
		drawPreviewFrame(ShowBuffer,120,120,90,60,0x0E70);
	}
}


//����Ԥ������
void QR_LoadDcData(U16 kRow, U16 kCol, U16 kHigh, U16 kWidth, U8 *pBuf)
{
	U8  *pMemory,*pTemp,*pRgb;
	U16 *pD,*pS;
	U32 off,size;
	int i,j;
	
	pMemory = (U8 *)Get_Display_DisplayMemory_Offset();

#ifdef W818C
	off  = (kRow*gLcd_Total_Column+kCol)*2;//���Դ��е�ƫ��
	pD   = (U16 *)(pMemory+off);
	pS = (U16 *)pBuf ;
	
	for(i = 0 ; i < kWidth; i++)
	{
		for(j = 0 ; j < kHigh ; j++)
		{
			pD[j] = pS[kWidth * (kHigh-1-j) + i] ;
		}
		pD += gLcd_Total_Column;
	}
#else
	size = kHigh*kWidth;
	off  = (kRow*gLcd_Total_Column+kCol)*2;//���Դ��е�ƫ��
	pD   = (U16 *)(pMemory+off);
	memcpy((char *)pD,pBuf,size*2);
#endif	
	
	QR_Frame((U16 *)pMemory,0x0, 0x679, 1);
	
	drvLcd_BMPFromDisplayMemory2Screen();
	return ;
}

int QR_Preview()
{
	U8	rd,wr;
	U8	intLev;
	U8	*pTemp;
	int tQRret, frames;
	
	rd = gu8QRRead;
	wr = gu8QRWrite;
	
	if(rd != wr)
	{
		if(gu8QRPrevDelayFrame > 0)
		{
			gu8QRPrevDelayFrame--;
		}
		else// if(hyhwLcd_RGBDmaOK() == TRUE)
		{
		#ifdef QR_VGA_PREVIEW	
			/*Ԥ��ͼ�����ųɿ�����ʾ��ͼ��*/
			Comm_HWScaler_YUV420_t((U32*)gpQRPrev_y[rd],(U32*)gpQRPrev_u[rd],(U32*)gpQRPrev_v[rd],
						 (U32*)gpQRQvga_y,(U32*)gpQRQvga_u,(U32*)gpQRQvga_v,
						 (QR_PREVIEW_H_SIZE<<16)|QR_PREVIEW_W_SIZE,(QR_PREVIEW_DISPLAY_HEIGHT<<16)|QR_PREVIEW_DISPLAY_WIDTH );
			
			/*YUV-->RGB, gpQRDisplayRgb 320 x 240 x 2*/
			Color_YUV420toRGB565_Buf_t((U32*)gpQRQvga_y,(U32*)gpQRQvga_u,(U32*)gpQRQvga_v,
    		                         (U32*)gpQRDisplayRgb,
    		                         (QR_PREVIEW_DISPLAY_HEIGHT<<16)|QR_PREVIEW_DISPLAY_WIDTH,0,
    		                         (QR_PREVIEW_DISPLAY_HEIGHT<<16)|QR_PREVIEW_DISPLAY_WIDTH,
    		                         (QR_PREVIEW_DISPLAY_HEIGHT<<16)|QR_PREVIEW_DISPLAY_WIDTH,0);
		#elif defined QR_QVGA_PREVIEW
			/*YUV-->RGB, gpQRDisplayRgb 320 x 240 x 2*/
			Color_YUV420toRGB565_Buf((U32*)gpQRPrev_y[rd],(U32*)gpQRPrev_u[rd],(U32*)gpQRPrev_v[rd],
    		                         (U32*)gpQRDisplayRgb,
    		                         (QR_PREVIEW_DISPLAY_HEIGHT<<16)|QR_PREVIEW_DISPLAY_WIDTH,0,
    		                         (QR_PREVIEW_DISPLAY_HEIGHT<<16)|QR_PREVIEW_DISPLAY_WIDTH,
    		                         (QR_PREVIEW_DISPLAY_HEIGHT<<16)|QR_PREVIEW_DISPLAY_WIDTH,0);
		#endif

        	QR_LoadDcData(QR_DISPLAY_START_ROW,QR_DISPLAY_START_COL,QR_PREVIEW_DISPLAY_HEIGHT,QR_PREVIEW_DISPLAY_WIDTH,gpQRDisplayRgb);

    		if(eQRState == QR_STATE_START)
    		{
    			eQRState = QR_STATE_INIT;

				//������2֡ͼ��
				for (frames=0; frames<2; frames++)
				{
        		#ifdef QR_QVGA_RECOGNISE
        			gpic.pData = gpQRQvga_y;//320*240ʶ��
        		#else
        			gpic.pData = gpQRPrev_y[rd];//640*480ʶ��
        		#endif
        			memset((char *)&gcode,0,sizeof(QRCODE_CODE));
        			tQRret = QRCode_Decode(ghqrc,&gpic,&gcode);
        			if(tQRret == QRCODE_SUCCESS)
					{//����ɹ�
						memcpy(gpQRcodeBuf,gcode.pBuf,gcode.uLen);
						gu32QRcodeLen = gcode.uLen;
						if(gu32QRcodeLen > 0)
							eQRState = QR_STATE_GETOK;
						
						gpQRcodeBuf[gu32QRcodeLen] = 0;
						
						break;//����ɹ����˳�ѭ��
					}
					else// if(tQRret == QRCODE_UNKNOWN_CODE)
					{//����֧�ֵ�����(��ʱ����ʶ�������Ҳ�᷵�ظ�ֵ)
						gu32QRcodeLen = 0;
						gpQRcodeBuf[gu32QRcodeLen] = 0;
					}
					rd++;
					if(rd >= QR_PREVIEW_BUF_NUM)
					{
						rd -= QR_PREVIEW_BUF_NUM;
					}
					if (rd == gu8QRWrite)
					{
						rd--;
						break;
					}
				}
				
        	}
        	
        	rd++;
			if(rd >= QR_PREVIEW_BUF_NUM)
			{
				rd -= QR_PREVIEW_BUF_NUM;
			}
			gu8QRRead = rd;
		}
	}
	
	if(gu8QRPrevDelayFrame > 0)
	{
		return 0;
	}
	
	/*������ƣ��Զ��������*/
	if(eQRState == QR_STATE_INIT)
	{
		hyhwLcd_flashEnable();
	}
	
	return 0;
}


int QR_DeskboxHandle(void *pView, GRAP_EVENT *pEvent)
{
	tGRAPDESKBOX	*pDesk;
	U32				delay;
	U32				message, wParam, lParam;
	int				ret = -1;
	tGRAPVIEW		*pSubView;
	tGRAPZORDER		*pSubZorder ;
	
	pDesk	= (tGRAPDESKBOX *)pView;
	message = pEvent->message;
	wParam  = pEvent->wParam;
	lParam  = pEvent->lParam;
	
	switch(message)
	{
	case VIEW_DRAW:		  
		pDesk->draw(pDesk, 0, 0); 
		if( (TRUE == *pDesk->pUIFocus) && !(*pDesk->pQuitCnt))
		{
			DisplayData2Screen();
		}
		return SUCCESS;
	    break;
	case VIEW_QUIT:
	case VIEW_ESC:
		*pDesk->pQuitCnt = 1;
		break;
	case VIEW_QUITALL:
		*pDesk->pQuitCnt = 0xff;
		break;
	case VIEW_PRESS:
		break;
	case VIEW_REV_PULLMSG:
	    if (pDesk->pPullChild == NULL)
        {//��һ��    
                        
            if (VIEW_PULL_END == pEvent->extParam) return SUCCESS;
            
            pEvent->extParam = VIEW_PULL_START;       
            
            pSubZorder = pDesk->pZorderList;

			while (pSubZorder)
			{
				pSubView = pSubZorder->pMember;
				
				if (pSubView->view_attr.enable == 1 && VIEW_PULL_START == pSubView->handle(pSubView,pEvent))
                {
                    pDesk->pPullChild = pSubView;
                    pDesk->lastPoint = lParam;
                    pDesk->pullTimeOut = rawtime(NULL);
                    break;    
                }
                
				pSubZorder = pSubZorder->pNext;
			}
        }
        else
        {//������
            int rv;
            
            pEvent->wParam = pDesk->lastPoint;
            
            pSubView = pDesk->pPullChild;
            rv = pSubView->handle(pSubView,pEvent);
            	        
            if (VIEW_PULL_END == rv)
            {
                pDesk->pPullChild = NULL;	        
            }
            else if (VIEW_PULL_ING == rv)
            {
                pDesk->lastPoint = lParam;
            }
            
            //���ⷵ��ֵ
            if(rv == PARSED )
    		{
    	        pSubZorder = pDesk->pZorderList ;
    	        while(pSubZorder)
				{
					if(pSubView == pSubZorder->pMember)
					{
						break;
					}
					pSubZorder = pSubZorder->pNext;
				}
    			Grap_UpdateZorder(((tGRAPVIEW *)pView)->pParent, pSubZorder);	
    			pDesk->pPullChild = NULL;			
    		}
    		else if(RETURN_FOCUSMSG == rv)
    		{
    			pDesk->pPullChild = NULL;
    			return RETURN_FOCUSMSG;
    		}
    		else if(RETURN_QUIT == rv)
    		{
    			pDesk->pPullChild = NULL;
    			return RETURN_QUIT;
    		}
        }	    
        pDesk->pullTimeOut = rawtime(NULL);
	    break;
	default:
		break;
	}
	
	if(message != VIEW_QUIT && message != VIEW_ESC && message != VIEW_REV_PULLMSG && message != VIEW_KEY_MSG)
	{
		tGRAPZORDER  *pSubZorder ;
		S32		ret = 0 ;
		pSubZorder = pDesk->pZorderList ;

		while (pSubZorder)	// deskbox��������ӿؼ���handle�ĵ��á�
		{
			pSubView = pSubZorder->pMember ;

			if( pSubView->view_attr.enable || message == VIEW_INIT)
			{
				ret = pSubView->handle(pSubView, pEvent);
				if(ret == PARSED )
				{
					Grap_UpdateZorder(((tGRAPVIEW *)pView)->pParent, pSubZorder);
					break ;
				}
				else if(RETURN_FOCUSMSG == ret)
				{
					return RETURN_FOCUSMSG;
				}
				else if(RETURN_QUIT == ret)
				{
					return RETURN_QUIT;
				}
			}
			pSubZorder = pSubZorder->pNext;
		}
	}
	
	if(eQRState == QR_STATE_INIT || eQRState == QR_STATE_START)
	{
		delay = ABS(rawtime(NULL) - gu32QRQuitTime);
		if(delay >= 18000)//3 min
		{//�޲��� ��ʱ�˳�
			*pDesk->pQuitCnt = 1;
		}
		else
		{
			/*ͼ��Ԥ��*/
			QR_Preview();
			if(eQRState == QR_STATE_GETOK)//ʶ��ɹ�
			{
				QR_StopInterrupt();
				hyhwLcd_flashDisable();
				
				*pDesk->pQuitCnt = 1;
				beeper_Start(0,1,0);
				/*Com_SpcDlgDeskbox(gpQRcodeBuf,0,pDesk,1,NULL,NULL,DLG_ALLWAYS_SHOW);
				
				QR_ParaInit();

				hyhwCif_frameStart((U32)gpQRPrev_y[0],(U32)gpQRPrev_u[0],(U32)gpQRPrev_v[0]);
			
				hyhwmCif_intEnable();
				hyhwInt_enable(INT_BIT_CAMERA);*/
			}
		}
	}
	
	
	return SUCCESS;
}


S32 QR_ButtonRecvPullMsg(void *pView,GRAP_EVENT *pEvent)
{
    U32 extParam, message, wParam, lParam ;
    tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tGrapButtonBuffPic *pbtnpic;
	tGRAPVIEW *pTempView;
	S32 ret = FAIL;
	int x, y ;

	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
    
    message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;

	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;
	
	if (VIEW_PULL_START == extParam)
	{//����ʼ��

		if(GRAP_INWIDGET_RANGE(x,y,pButton))
	     {//����Ч��Χ��
	         ret = VIEW_PULL_START;
	     }
	     else
	     {
	        return FAIL;
	     }
	}
	
	if (VIEW_PULL_START == extParam)
	{//down
		Grap_ChangeFocusEnable(pButton);

	    pButton->state = STATE_PRESS;
		pButton->draw(pButton, 0, 0);
	    DisplayData2Screen();
	}
	else if (VIEW_PULL_ING == extParam)
	{//��ק��
	    if(GRAP_INWIDGET_RANGE(x,y,pButton))
	     {//����Ч��Χ��
	         pButton->state = STATE_PRESS;
		     pButton->draw(pButton, 0, 0);
		     
		     ret = pButton->pressEnter(pView, pButton->state) ;
             if ( ret== RETURN_REDRAW)
             {
            	 ReDraw(pButton, 0, 0);
             }
             else if(ret == RETURN_QUIT)
             {
            	 *((tGRAPDESKBOX *)(pButton->pParent))->pQuitCnt = 1;
             }
             else if (ret == RETURN_FOCUSMSG)
             {
            	 return ret;
             }			
		     ret = VIEW_PULL_ING; 
	     }
	     else
	     {
	        pButton->state = STATE_NORMAL;
		    pButton->draw(pButton,0,0);
	     }

	    
	}
	else if (VIEW_PULL_END == extParam)
	{//up
	    pButton->state = STATE_NORMAL;
		pButton->draw(pButton,0,0);				
		DisplayData2Screen();

        if(!GRAP_INWIDGET_RANGE(x,y,pButton))
        {
            return VIEW_PULL_END;
        }    
        
		ret = pButton->pressEnter(pView, pButton->state) ;
		if ( ret== RETURN_REDRAW)
		{
			ReDraw(pButton, 0, 0);
			ret = VIEW_PULL_END;
		}
		else if(ret == RETURN_QUIT)
		{
			*((tGRAPDESKBOX *)(pButton->pParent))->pQuitCnt = 1;
		}
		else if (ret == RETURN_FOCUSMSG)
		{
			return ret;
		}			
		return PARSED;
	}
	else if (VIEW_PULL_TIMEOUT == extParam)
	{//��ʱ
	    pButton->state = STATE_NORMAL;
		pButton->draw(pButton,0,0); 
		
		ret = VIEW_PULL_TIMEOUT;  
	}
	
	return ret;  
 
}

S32 QR_ButtonHandle(void *pView, GRAP_EVENT *pEvent)
{
	tGRAPBUTTON *pButton;
	U32 message, wParam, lParam ;
	int ret = SUCCESS;

	pButton = (tGRAPBUTTON *)pView;
	
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	
	switch(message)
	{
	case VIEW_ENABLE:
		pButton->view_attr.enable = 1;
		pButton->draw(pButton, 0, 0);
		break;
	case VIEW_DISABLE:
		pButton->view_attr.enable = 0;
		break;
	case VIEW_DRAW:
	case VIEW_UPDATE:
		pButton->draw(pButton, 0, 0);
		break;
	case VIEW_PRESS:
		if ((U16)lParam == pButton->btn_privattr.keycode)
		{
			tGRAPVIEW *pTempView;
			
			pButton->view_attr.clNormal = pButton->view_attr.clFocus;
			pButton->btn_privattr.cl_normal = pButton->btn_privattr.cl_focus;
			
			if(pButton->view_attr.FocusEnable)
			{
				if(!pButton->view_attr.curFocus)
				{
					pTempView = pButton->pParent;
					while(pTempView)
					{
						if(pTempView->view_attr.curFocus)
						{
							pTempView->view_attr.curFocus = 0;
							pTempView->draw(pTempView, 0, 0);
						}						
						pTempView = pTempView->pNext ;
					}						
					pButton->view_attr.curFocus = 1;
				}
			}
				
			pButton->state = STATE_PRESS;
			pButton->draw(pButton,0,0);
			DisplayData2Screen();
			syssleep(15);
			pButton->state = STATE_NORMAL;
			pButton->draw(pButton,0,0);
			DisplayData2Screen();
			ret = pButton->pressEnter(pView, pButton->state) ;
			if ( ret== RETURN_REDRAW)
			{
				ReDraw(pButton, 0, 0);
				return PARSED;
			}
			else if(ret == RETURN_QUIT)
			{
				*((tGRAPDESKBOX *)(pButton->pParent))->pQuitCnt = 1;
			}
			else if (ret == RETURN_FOCUSMSG)
			{
				return ret;
			}	
		}
		break;
	case VIEW_CLICK:
	case VIEW_BTNUP:
		break ;	          
	     	
	case VIEW_QUIT:
		break;
	case VIEW_REV_SYSMSG:
		pButton->recvSysMsg(pView);
		break;
	case VIEW_REV_PULLMSG:
	    ret = pButton->recvPullMsg(pButton,pEvent);
	    break;	
	}
	return ret;
}


int QR_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;

	if (state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk =  (tGRAPDESKBOX *)pBtn->pParent;
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1:
			gu32QRQuitTime = rawtime(NULL);
			eQRState = QR_STATE_START;
			break;
		case BUTTON_ID_2:
			*pDesk->pQuitCnt = 1;
			return RETURN_QUIT;
			break;
		default:
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return RETURN_CANCLE;
}

/*-----------------------------------------------------
* ����:QR_Desk
* ����:����
* ����:pWorkBuf:�ⲿ�ṩ����buf  ����2.1M
       pQRbuf:QRʶ��ʹ�õ�buf   ��Ҫ7M
       QRlen:(in)pQRbuf�ĳ���  (out)����ʶ�𵽵�QR��ĳ���  0:��ʾû��ʶ��
* ����:ʶ�𵽵�QR��
------------------------------------------------------*/
U8 *QR_Desk(tGRAPDESKBOX *pFather, U8 *pWorkBuf, U8 *pQRbuf, U32 *QRlen)
{
	int				i,ret = 1;
	tGRAPDESKBOX	*pDesk	= NULL;
	tGRAPBUTTON		*pBtn;
	int				malloclen;
	U16 			lastValue;
	U8				index=0;
	
	ConSume_UpdataSpeedDirect(SHELL_AN, 192);
	
	malloclen  = *QRlen;
	gpQRUseBuf = pWorkBuf;
	*QRlen = 0;
	
	pDesk = Grap_CreateDeskbox(&QRViewAttr[index++], "QRʶ��");
	if (NULL == pDesk)
	{
		goto QR_BAD;
	}
	Grap_Inherit_Public(pFather, pDesk);
	pDesk->handle = QR_DeskboxHandle;
	pDesk->pKeyCodeArr = keyComArray;
	
	Grap_ShowPicture(pDesk->view_attr.clNormal, 0, 0);
	drvLcd_SetColor(0xffff, 0);
	drvLcd_SetDisplayAddress(Get_Display_DisplayMemory_Offset());
	drvLcd_DisplayString(100,84,0,"���Ե�...", NULL, 0);
	DisplayData2Screen();
	
	for (i=0; i<2; i++)
	{
		pBtn = Grap_InsertButton(pDesk,&QRViewAttr[index++],&QRBtnPrv[i]);
		if (NULL == pBtn)	goto QR_BAD;
		pBtn->pressEnter = QR_BtnPress;
		pBtn->recvPullMsg = QR_ButtonRecvPullMsg;
		pBtn->handle = QR_ButtonHandle;
	}

	InputRange_CloseFd(); //�رռ������뷢��ʹ�� �ǵ��˳���Ҫ���³�ʼ��	
	hy_mem_create(QR_MEM_IDX);
	hy_mem_load_blocks(pQRbuf, malloclen, QR_MEM_IDX);

	if(0 != QR_Init())
	{
		Com_SpcDlgDeskbox("�������豸!",6,pDesk,2,NULL,NULL,100);
		Grap_DeskboxDestroy(pDesk);
		goto QR_BAD;
	}
	
	lastValue = BackLight_CloseScreenProtect();//��¼������Ϣ �˳�ʱ��ԭ
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);

	BackLight_SetScreenProtect(lastValue);

	QR_DeInit();
	
	hy_mem_release(QR_MEM_IDX);
	InputRange_Init(); /* ���³�ʼ�����뷨 */

	drvLcd_IsUseQueue(0);
	drvLcd_ClearQueue();
	
	*QRlen = gu32QRcodeLen;
	
	ConSume_UpdataSpeedDirect(SHELL_AN, 0);
	
	return gpQRcodeBuf;
	
QR_BAD:
	
	*QRlen = 0;
	QR_DeInit();
	hy_mem_release(QR_MEM_IDX);
	InputRange_Init(); /* ���³�ʼ�����뷨 */
	drvLcd_IsUseQueue(0);
	drvLcd_ClearQueue();

	ConSume_UpdataSpeedDirect(SHELL_AN, 0);
	
	return NULL;
}
