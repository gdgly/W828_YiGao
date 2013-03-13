
#ifndef BEEPER_CTRL_H_
#define BEEPER_CTRL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "hyTypes.h"
#include "hyErrors.h"

#include "hyhwTimer.h"
#include "hyhwGpio.h"


/*
ʹ��timer����������ʱ����ƣ�ʹ��timer6
*/

typedef struct beep_music_data_st
{
	U16	tone;			/* �������������pwm������ */
	U16	rhythm;			/* ���࣬��10msΪ��λ */
} BEEP_MUSIC_DATA_st;

typedef enum
{
	SCAN_OK = 0,
	SCANNER_POWERON_OK,
	WAIT_TRANS_DATA,
	LOW_POWER,
	LOW_POWER_DOWN,
	SETTING_OK,
	SETTING_ERR,
	KEY_SOUND,
	
	RESEVER1,
	RESEVER2,
	RESEVER3,
	RESEVER4
}BEEP_TYPE_en;


extern const U16 beepTone[];
extern const BEEP_MUSIC_DATA_st* const beepData[];


/*----------------------------------------------------------------------------
* ����:	beeper_start
* ����:	��ʼ����ָ������
* ����:	pBeep_data	----- ���е����ɽṹ����
* ���:	none
*-----------------------------------------------------------------------------*/
void beeper_start(BEEP_TYPE_en beep_type, int beepEnable, int ledEnable);

/*----------------------------------------------------------------------------
* ����:	beeper_stop
* ����:	ֹͣ��������
* ����:	none
* ���:	none
*-----------------------------------------------------------------------------*/
void beeper_stop(void);

/*----------------------------------------------------------------------------
* ����:	beeper_getState
* ����:	��ȡ����״̬
* ����:	none
* ���:	0	----- �����Ѿ����
*		1	----- ��������
*-----------------------------------------------------------------------------*/
int beeper_getState(void);

/*----------------------------------------------------------------------------
* ����:	beeper_ctrl_ISR
* ����:	���п��ƣ��ú�����timer�ж��е���(timer6)
* ����:	none
* ���:	none
*-----------------------------------------------------------------------------*/
void beeper_ctrl_ISR(void);


#endif	//#ifndef BEEPER_CTRL_H_