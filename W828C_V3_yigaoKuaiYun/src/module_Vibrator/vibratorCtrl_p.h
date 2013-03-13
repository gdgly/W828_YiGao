#ifndef HYC_VIBRATOR_CTRL_MODE
#define HYC_VIBRATOR_CTRL_MODE

#include "hyTypes.h"

#include "hyhwGpio.h"

/*---------------------------------------------------------------------------
 ����: VibratorCtrl_Start
 ����: ��������
 ����: millisecond----Ҫ������ʱ�䣬����Ϊ��λ��ʱ�䵽ʱ�������˳��ý���
 					 ����ֵΪ0ʱ���������˳���ֱ������VibratorCtrl_Stop()
 ����: HY_ERROR---�ý����������У���ǰ������Ч
 	   HY_OK------�����ɹ�
* -----------------------------------------------------------------------------*/
int VibratorCtrl_Start(U32 millisecond);

/*---------------------------------------------------------------------------
 ����: VibratorCtrl_Stop
 ����: ֹͣ����
 ����: ��
 ����: ��
* -----------------------------------------------------------------------------*/
void VibratorCtrl_Stop(void);


#endif //#ifndef HYC_VIBRATOR_CTRL_MODE