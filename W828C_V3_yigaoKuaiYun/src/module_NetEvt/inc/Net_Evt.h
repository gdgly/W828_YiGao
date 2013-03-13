#ifndef _NET_EVT_H_
#define _NET_EVT_H_

/*===== include files =======*/
#include "hyTypes.h"


/*---------------------------------------------------*/
#define NET_FOREGROUND_EVT			0x80000000
#define NET_EVENT_MASK				0x7FFFFFFF

#define NET_LOGIN_EVT				0x00000001	//��¼
#define NET_UPDATE_SOFT_EVT			0x00000002	//��������
#define NET_DOWNDATA_EVT			0x00000004	//��������
#define NET_UPDATE_TIME_EVT			0x00000008	//ʱ��ͬ��
#define NET_SENDREC_EVT				0x00000010	//�ϴ���¼
#define NET_UPDATE_DATA_EVT			0x00000020	//��������
#define NET_CLOSESOCKET_EVT			0x00000040	//�ر�socket
#define NET_CLOSEPPP_EVT			0x00000080	//�ر�ppp
#define NET_CLEARREC_EVT			0x00000100	//������м�¼
#define NET_ADDREC					0x00000200	//��nand���ؼ�¼
#define NET_ADDMULTI_EVT			0x00000400	//��nand����һƱ�����¼
#define NET_SENDMULTI_EVT			0x00000800	//����һƱ�����¼
#define NET_SENDMULTI_F_EVT			0x00001000	//ǰ̨����һƱ�����¼
//#define NET_APPLYKEY_EVT			0x00002000	//����key
#define NET_CHECKSOFT_EVT			0x00002000	//�������汾

#define NET_STOP					0x40000000	//ֹͣ��̨����

extern U32 gu32NetEvt;


#endif //_NET_EVT_H_