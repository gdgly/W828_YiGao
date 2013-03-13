#ifndef _APP_CONTROL_CONSUME_H_
#define _APP_CONTROL_CONSUME_H_

/*
*include files
*/
#include "hyTypes.h"

//ϵͳ���е����Ƶ��
#define CSM_MAX_CLK     (240)


//��ʱ�ı�Ƶ�ʳ�ʱ��ʱ��
//#define APP_CSM_ELAPSE_TIME   100
#define APP_CSM_ELAPSE_TIME   150

/*
*define types
*/
//ֻ������Ľ�������ʱ����Ҫ�������ϵ��������
//ÿ�����̶���һ������ϵ�������ﵽĳ��ϵ��ʱ��Ӧʵʩ��Ӧ�Ĳ���
typedef struct _tAPP_CTR_CSM
{
   eAPPLICATION_NO pid; //���̵�pid
   U32             coef;//�������е�ϵ��  
}tAPP_CTR_CSM;

/*---------------------------------------------------------------------------
 ������ConSume_CoefInitial
 ���ܣ���ʼ������
 ������none
 ���أ�none
* -----------------------------------------------------------------------------*/
void ConSume_CoefInitial(void);

/*---------------------------------------------------------------------------
 ������ConSume_ChangeCoefTemp
 ���ܣ���ʱ�ı�һ��Ƶ�ʣ���Ҫ�������ڽ����л���ˢ������ʱ������ʱ�ĸо�,�����ǰƵ�ʱ�Ҫ�����ĸ��򲻸ı�Ƶ��
 ������AppId:���̱��ö��  coef:��Ҫ�ı��ϵ��
 ���أ�none
* -----------------------------------------------------------------------------*/
void ConSume_ChangeCoefTemp(U32 coef, int type);

/*---------------------------------------------------------------------------
 ������ConSume_ChangeCoefTempElapseTime
 ���ܣ���ʱ�ı�һ��Ƶ�ʵ�ʱ���Ƿ�ʱ
 ������none
 ���أ�none
* -----------------------------------------------------------------------------*/
void ConSume_ChangeCoefTempElapseTime(void);

/*---------------------------------------------------------------------------
 ������ConSume_UpdataAppCoef
 ���ܣ�����Ӧ�ý��̵�ϵ��,���ϵ���Ƕ�̬�ı��
 ������AppId:���̱��ö��  coef:��Ҫ�ı��ϵ��
 ���أ�none
* -----------------------------------------------------------------------------*/
void ConSume_UpdataAppCoef(eAPPLICATION_NO AppId, U32 coef);

/*---------------------------------------------------------------------------
 ������ConSume_UpdataSpeedDirect
 ���ܣ�ֱ�Ӹ���Ƶ�ʣ�����Ҫ����̨ȥ�޸�Ƶ��
 ������AppId:���̱��ö��  coef:��Ҫ�ı��ϵ��
 ���أ�none
* -----------------------------------------------------------------------------*/
void ConSume_UpdataSpeedDirect(eAPPLICATION_NO AppId, U32 coef);

/*---------------------------------------------------------------------------
 ������ConSume_Get_PidSpeed
 ���ܣ��õ���ǰpid��Ƶ��
 ������AppId:���̱��ö��  coef:��Ҫ�ı��ϵ��
 ���أ�none
* -----------------------------------------------------------------------------*/
U16 ConSume_Get_PidSpeed(eAPPLICATION_NO AppId);

#if 0
/*---------------------------------------------------------------------------
 ������ConSume_WifiChangeSpeedTimeOut
 ���ܣ��ú���ֻ�ṩ��wifi����ʹ��������ʹ�ã��޸�Ƶ���г�ʱ����
 ������coef:��Ҫ���ӵ�ϵ��
 ���أ�none
* -----------------------------------------------------------------------------*/
void ConSume_WifiChangeSpeedTimeOut(U32 coef);

/*---------------------------------------------------------------------------
 ������ConSume_AppChangeSpeedTimeOut
 ���ܣ�ֱ������Ƶ�ʣ�����Ҫ����̨ȥ�޸�Ƶ��
 ������coef:��Ҫ���ӵ�ϵ��
 ���أ�none
* -----------------------------------------------------------------------------*/
void ConSume_WifiCheckTimeOut(void);
#endif

/*---------------------------------------------------------------------------
 ������ConSume_DetectCurCoef
 ���ܣ���̨����Ƶ��
 ������none
 ���أ�none
* -----------------------------------------------------------------------------*/
void ConSume_DetectCurCoef(void);

#endif