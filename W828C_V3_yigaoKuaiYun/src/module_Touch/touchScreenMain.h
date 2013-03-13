#ifndef __TOUCH_SCREEN_MAIN_H__
#define __TOUCH_SCREEN_MAIN_H__

#include "hyTypes.h"
//#include "appControlProcess.h"

#include "HycDeviceConfig.h"

//����ƫ�ƷǷ�ֵ
#define INVALID_OFFSET			(1000)
//������չϵ���Ƿ�ֵ
#define INVALID_TOUCH_COFF		(10)

// 1 ��������G22�� 
// 2 Yֵ������
//#define TOUCH_SCREEN_TYPE      (2)

//#if TOUCH_SCREEN_TYPE == 1
//�������ϵ��ֵ
#if	defined HYC_DEV_LCD_HX8347
    #define TOUCH_DEFAULT_X_COEF    (0.067606)
    #define TOUCH_DEFAULT_X_OFF     (-18)

    #define TOUCH_DEFAULT_Y_COEF    (0.092352)
    #define TOUCH_DEFAULT_Y_OFF     (-19)
//#elif TOUCH_SCREEN_TYPE == 2
#elif defined HYC_DEV_LCD_N2748TLY

	#define TOUCH_DEFAULT_X_COEF    (0.063291)//(0.066852)
    #define TOUCH_DEFAULT_X_OFF     -11//(-17)

    #define TOUCH_DEFAULT_Y_COEF    (0.086475)
    #define TOUCH_DEFAULT_Y_OFF     (-18)
#elif defined HYC_DEV_LCD_8877N_BYD

	#define TOUCH_DEFAULT_X_COEF    (-0.0659263)//(0.066666)
    #define TOUCH_DEFAULT_X_OFF     256//-15

    #define TOUCH_DEFAULT_Y_COEF    (0.0850271)//(0.086580)//(0.086475)
    #define TOUCH_DEFAULT_Y_OFF     -15//0//(-18)
#elif defined HYC_DEV_LCD_TIANMA

#elif defined HYC_DEV_LCD_R61505V
    #define TOUCH_DEFAULT_X_COEF    (0.066852)
    #define TOUCH_DEFAULT_X_OFF     (-17)

    #define TOUCH_DEFAULT_Y_COEF    (0.091954)
    #define TOUCH_DEFAULT_Y_OFF     (-22)     
#elif defined HYC_DEV_LCD_HXRGB
	#define TOUCH_DEFAULT_X_COEF    0.0846561//(0.066852)
    #define TOUCH_DEFAULT_X_OFF     -12//(-17)

    #define TOUCH_DEFAULT_Y_COEF    (0.0688468)
    #define TOUCH_DEFAULT_Y_OFF     (-23)
#endif

//��ק�������
#define PULL_RING_BUFFER_LEN	50	//������ק��Ч��ĸ���
#define PULL_POIONT_SETP        15   //x,y����ƫ����Сֵ�����ڸ�ֵ����Ϊ����Ч

//��ק��״̬
#define VIEW_PULL_FAIL       30
#define VIEW_PULL_START      31
#define VIEW_PULL_ING        32
#define VIEW_PULL_END        33
#define VIEW_PULL_TIMEOUT    34


//��Ч����ֵ
#define PULL_INVALID_POINT_X    0xFFFF        
#define PULL_INVALID_POINT_Y    0xFFFF        

//��ק����ֵ
#define PULL_END_POINT_X    0xFFFE        
#define PULL_END_POINT_Y    0xFFFE

//��ק��д��buf�ķ�ʽ
#define PULL_WRITE_NORMAL   1  //����д����ק��
#define PULL_WRITE_END      2  //д��̧���
#define PULL_WRITE_LONG     3  //д�볤����


//�����ʴ���idel,��ʼ������������
#define PEN_IDEL      (0)
#define PEN_START     (1)
#define PEN_WORKING   (2)

//BUF д״̬��־�������ж��Ƿ�Ϊ��һ����
#define PEN_WR_IDEL     (0)
#define PEN_WR_WORKING  (1)


//����״̬
#define PRESS_LONG_IDEL    (0)
#define PRESS_LONG_START   (1)
#define PRESS_LONG_WORKING (2)

//���������
#define PRESS_LONG_MAX_CNT  (30)

typedef struct _tPOINT
{
    unsigned short x;
    unsigned short y;
}tPOINT;

//��ק���㴥��
typedef struct _tPOINT_KEY_PULL
{
    U16 x;
    U16 y;
    U32 endFlag;
}tPOINT_KEY_PULL;

typedef struct _tTOUCH_CTR
{
     void  *pTouchEvent;
     volatile unsigned int lastTick; //�ϴ��д������ʱ���
     unsigned char  penFSM; //������״̬�����ڴ���̧�����ק
     unsigned char  status; //������ק��״̬
     unsigned char  wrBufState;//��¼����Ϣ״̬��־
     unsigned char  pointStep; //��Ч�Ĵ��������
     unsigned char  longPressFsm; //����״̬��־
     unsigned int   longPressTick; //������ʼ������tick��
     unsigned int   longPressCnt; //��������
     int   longPressMax; //����������
     
     volatile unsigned char  num; //�����ĵ����
     tPOINT  pointSaveBuf[PULL_RING_BUFFER_LEN];
     tPOINT  lastPoint;
     tPOINT  *pRead;
     tPOINT  *pWrite;
       
}tTOUCH_CTR;

typedef struct touchParameter_st
{
	S16 TouchOffsetX;//�������̶�ƫ��
	S16 TouchOffsetY;
	float TouchCoffX;//��������չϵ��
	float TouchCoffY;
} tTOUCH_PARAMETER;

/*-----------------------------------------------------------------------------
* ����:	ts_main(void);
* ����:	adc��ѭ��
* ����:	none
* ����: none
*----------------------------------------------------------------------------*/
void ts_main(void);

/*-----------------------------------------------------------------------------
* ����:	ts_wake_collect_position
* ����:	��ʱȥ�ɼ�����λ��
* ����:	none
* ����: none
*----------------------------------------------------------------------------*/
void ts_wake_collect_position(void);

/*---------------------------------------------------------------------------
 ������ts_get_touch_pull_position
 ���ܣ��ṩ������̨��ȡ��ק��Ľӿ�
 ������pKeyPullPiont: ��ȡָ��
 ���أ�HY_OK  OR  HY_ERROR
* -----------------------------------------------------------------------------*/
int ts_get_touch_pull_position(tPOINT_KEY_PULL *pKeyPullPiont);

/*---------------------------------------------------------------------------
 ������ts_get_point_num
 ���ܣ����ж��ٸ�������
 ������none
 ���أ�none
* -----------------------------------------------------------------------------*/
U8 ts_get_point_num(void);

#endif