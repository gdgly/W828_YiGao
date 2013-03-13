#ifndef _COMMON_DLG_H_
#define _COMMON_DLG_H_

#include "grap_view.h"
#include "grap_api.h"

#define DLG_ALLWAYS_SHOW  	 (0xFFFFFFFF)
#define DLG_DEFAULT_TIME 	 500
#define DLG_DEFAULT_TIME_3S  300

#define DLG_WIDGET_NUM		9


//����Ի��򷵻�״̬
typedef enum _eDLG_STAT
{
    DLG_CONFIRM_EN, //�Ի���ȷ��
    DLG_CANCEL_EN,   //�Ի���ȡ��
    DLG_TIMEROUT_EN //�����¼���ʱ�˳�
}eDLG_STAT;


/*===================================================================*/

int Com_GetPercentTotal();

void Com_SetPercentTotal(int total);

int Com_GetPercentRecv();

void Com_SetPercentRecv(int recvsize);

/*---------------------------------------------------------
*����: Com_DlgUpdataStat
*����: ���¶Ի���״̬
*����: stat �� �Ի���״̬
*����: none
*---------------------------------------------------------*/
void Com_DlgUpdataStat(eDLG_STAT stat);

U8 Com_DlgGetStat();

/*---------------------------------------------------------
*����: Com_DlgStartTime
*����: �Ի���ʼ��ʱ��
*����: none
*����: ��ʼʱ��
*---------------------------------------------------------*/
U32 Com_DlgStartTime(void);

/*---------------------------------------------------------
*����: Com_DlgTotalLiveTime
*����: �Ի��������ʱ��
*����: none
*����: ����ʱ��
*---------------------------------------------------------*/
U32 Com_DlgTotalLiveTime(void);

/*---------------------------------------------------------
*����: Com_DlgTimeOut
*����: �Ի����Ƿ�ʱ
*����: none
*����: 1: ��ʱ 0��δ��ʱ
*---------------------------------------------------------*/
int Com_DlgTimeOut(void);

/*---------------------------------------------------------
*����: ComWidget_LabTimerISR
*����: ����ź�ͼ��ʱ�������ж��¼�   
*����: pView �� lable point
*����: none 
*---------------------------------------------------------*/ 
S32 ComWidget_LabTimerISR(void *pView);

/*---------------------------------------------------------
*����: ComWidget_UpdataTimerMsg
*����: ����Ϣ�����е�timer��Ϣȥ�պ�    
*����: pDeskFather ����ָ��
*����: none 
*---------------------------------------------------------*/  
void ComWidget_UpdataTimerMsg(void *pDeskFather);

S32 ComWidget_LabTimerTimeOutISR(void *pView);

/*
*���ܣ�����Ի�����Ϣ
*����: pDeskFather:������ָ�� DlgType:�Ի������� 1��ʾ2����3ѯ�� id:��ʾ���ַ���id 
*      DlgPressEnter :ȷ����ť�ĺ���ָ�� DlgCancelEnter��ȡ����ť�ĺ���ָ��
*      DispTime : �Ի�����ʾ��ʱ��		 
*/
eDLG_STAT Com_SpcDlgDeskbox(U8 *str,U8 soundType,void *pDeskFather,U32 DlgType,
						PressEnter DlgPressEnter,PressEnter DlgCancelEnter,U32 DispTime);

/*---------------------------------------------------------
*����: Com_PercentLabTimerISR
*����: ������ʾ������ʾ�ٷֱ�  
*����: pView �� lable point
*����: none 
*---------------------------------------------------------*/ 
void Com_PercentLabTimerISR(void *pView);

/*
*���ܣ�����Ի�����Ϣ
*����: pDeskFather:������ָ�� DlgType:�Ի������� 1����ȡ�� id:��ʾ���ַ���id  
*      DlgPressEnter :ȷ����ť�ĺ���ָ�� DlgCancelEnter��ȡ����ť�ĺ���ָ��
*      DlgLabTimer: �����lableTimer�жϺ���ָ�� DispTime : lab�жϵ�ʱ��
*/
eDLG_STAT Com_CtnDlgDeskbox(void *pDeskFather,U32 DlgType,U8 *str,PressEnter DlgPressEnter,PressEnter DlgCancelEnter,
							Label_TimerISR DlgLabTimer,U32 DispTime , U32 TimeOut);


/*----------------------------------------------------------------------------------------------*/
/*---------------------------------------�ȴ����紦�����---------------------------------------*/
/*----------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------
*����: Com_ShutDownLabelTimerISR
*����: 
*����: none
*����: 
*---------------------------------------------------------*/
void Com_Usb_ShutDownLabelTimerISR(void *pView);


/*------------------yu---------------------------------------
* ����: Com_ShutDownDlgDeskbox
* ����: �Ի��򣬹رպ�̨������ ��Ҫ��ǿ�ƹػ���ǿ�ƽ���USBʱ�����
* ����: pDeskFather --- ������
* ����: 
------------------------------------------------------------*/
U8 Com_ShutDownDlgDeskbox(void *pDeskFather,U8 *str,Label_TimerISR DlgLabTimer,U32 timeOut);

#endif