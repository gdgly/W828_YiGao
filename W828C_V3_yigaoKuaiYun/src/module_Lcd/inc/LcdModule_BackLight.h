#ifndef _LCDMODULE_BACKLIGHT_H_
#define _LCDMODULE_BACKLIGHT_H_

#include "HycDeviceConfig.h"


#ifdef ACT6311
	#define LCD_PWM_GRADE_1	   68
	#define LCD_PWM_GRADE_2	   65
	#define LCD_PWM_GRADE_3	   60
	#define LCD_PWM_GRADE_4	   50
	#define LCD_PWM_GRADE_5	   40 
#endif

#ifdef CP2142MM
	#define LCD_PWM_GRADE_1	   12//15
	#define LCD_PWM_GRADE_2	   8
	#define LCD_PWM_GRADE_3	   5
	#define LCD_PWM_GRADE_4	   3
	#define LCD_PWM_GRADE_5	   2 
#endif 
	
extern S32  gu32ScreenPro;
//lcd PWM������Ƽ���
extern const U8 gu8Lcd_BackLight[5];

/*---------------------------------------------------------------------------
 ������BackLight_EventTurnOnLcd
 ���ܣ����ڴ�������״̬�£������¼���Ҫ�������� ��:���绰�ˣ��������µ�
 ��������
 ���أ�1������ 0��ά��
* -----------------------------------------------------------------------------*/ 
U32  BackLight_EventTurnOnLcd(void *pView);

/*---------------------------------------------------------------------------
 ������BackLight_UpdataLightTime
 ���ܣ����±���ʱ��
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/ 
void BackLight_UpdataLightTime(U32 t);

/*---------------------------------------------------------------------------
 ������BackLight_CalculateTime
 ���ܣ��Ա����ʱ,��ʱΪ0ʱ��رձ���
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/ 
void  BackLight_CalculateTime(void);

/*---------------------------------------------------------------------------
 ������BackLight_AvoidThis
 ���ܣ��ڵ�ǰģʽ�����δ����
 ��������
 ���أ�1:���� 0:������
* -----------------------------------------------------------------------------*/ 
U32 BackLight_AvoidThis(void);

#endif