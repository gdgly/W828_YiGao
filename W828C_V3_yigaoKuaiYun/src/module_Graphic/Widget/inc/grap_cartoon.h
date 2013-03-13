#ifndef __GRAP_CARTOON_H__
#define __GRAP_CARTOON_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"

typedef S32 (*GetCartoonString)(void *pView, U32 id, U8 *str);

typedef struct tGrapCartoonPriv
{
	U32 flag;			// cartoon type
	U16 focus;
	U16 timeIntv;		//ʱ�������೤ʱ�䲥��һ��
	U16 lftTimeIntv;	//����೤ʱ����������һ��
	U16 totFrame;		//������������
	U16 curFrame;		//��ǰ���ŵ�����
	U32 *picArray;		//������ÿ���ͼƬid��������
	U16 backgndPicId;	//��Ҫ�Ա���ˢ�µ�ͼƬ
}tGrapCartoonPriv;

typedef struct tGrapCartoon
{
	// view start 
	tGrapViewAttr view_attr; 

	HandleEvent handle;
	Draw draw;
	Destroy destroy;

	struct tGrapView *pNext;
	struct tGrapView *pPrev;
	struct tGrapView *pParent;
	// view end

	tGrapCartoonPriv cartoon_privattr;
	
	GetCartoonString getCartString;
	PressEnter pressEnter;
	RecvSysMsg recvSysMsg;
	RecvPullMsg recvPullMsg;

}tGRAPCARTOON;

tGRAPCARTOON *Grap_InsertCartoon(void *pView, tGrapViewAttr *pview_attr, tGrapCartoonPriv *pcartoon_privattr);
//�����غ���
S32 Grap_GetCartoonString(void *pView, U32 id, U8 *str);


#define MAX_CARTOON_STR_LEN 80

#define TIMER_MOD 1	//�����ÿؼ�����timerʱ����С��Timer����ʱ�䵥λ�����룩


#ifdef __cplusplus
}
#endif

#endif


