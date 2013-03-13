#ifndef __GRAP_EVENT_H__
#define __GRAP_EVENT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"

// ��Ϣ�ṹ��
typedef struct tGrapEvent 
{
	void *pView;
	U32  id;
	U32	 message;
	U32	 wParam;
	U32  lParam;
	U32	 reserve;
	U32  extParam; //��Ϣ����
}GRAP_EVENT;

//��Ϣ����һ����Ϣ���飨����ΪEVENTNUMINPOOL����EventHead�н���¼:
//	struct tEvent *pEventTop: ��Ϣ�صĿ�ʼ��Ϣ��ַ(ʵ�������ַ)
//	struct tEvent *pEventBtm: ��Ϣ�ص������Ϣ��ַ����һ��(ʵ�������ַ)
//	struct tEvent *pEventCur: ��Ϣ���е�ǰ��Ϣ�ĵ�һ����Ϣ
//	struct tEvent *pEventEnd: ��Ϣ���е�ǰ��Ϣ�����һ����Ϣ
//pEventTop��pEventBtm��Ϊ���γ�ѭ���������ģ�pEventCur��pEventEnd�������õ�����Ϣλ�á�
//��������Ӧ�ö��Ǳ���װ�����ģ�ʹ�ò��ǲ��ÿ��ǵģ�e

S32 Grap_SendMessage(tGRAPVIEW *pView, GRAP_EVENT *pEvent);

#ifdef __cplusplus
}
#endif

#endif

