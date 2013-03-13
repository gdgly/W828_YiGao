#ifndef _TCPIP_P_H_
#define _TCPIP_P_H_

/*************************************************************************
 * INCLUDES
 *************************************************************************/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

//tcpipЭ��ջ���е�״̬��������TCPIP_INI״̬ʱ����û�з���ip address ���ܽ�������������
typedef enum {
TCPIP_INI = 0,
TCPIP_RUN,
TCPIP_FAIL //һ���������ip����ʧ����ɵ�
}eTCPIP_STATE;

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
//��ʼ��TcpIp����
//void TcpIp_Initial(void);

/*---------------------------------
*����: TcpIp_GetRunState
*����: �õ�TcpIp����״̬
*����: none
*����: 0:����wait 1:run
---------------------------------*/
//eTCPIP_STATE TcpIp_GetRunState(void);

int TcpIp_ThreadMethod(void);

#endif //_TCPIP_P_H_
