#ifndef __GRAP_VIEW_H__
#define __GRAP_VIEW_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "hyTypes.h"
#include "glbVariable_base.h"
#include "LcdAPIFunc.h"
#include "appControlProcess.h"
#include "msgqueue.h"
#include "resManage.h"



#define PARSED	2
#define SUCCESS 1
#define FAIL 0


typedef struct tGrapEvent  GRAP_EVENT ;
typedef struct tGrapView tGRAPVIEW;

typedef S32 (*HandleEvent)(void *pView, GRAP_EVENT *pEvent);
typedef S32 (*Draw)(void *pView, U32 xy, U32 wh);
typedef S32 (*Destroy)(void *pView);
typedef S32 (*PressEnter)(void *pView, U16 state);
typedef S32 (*RecvSysMsg)(void *pView);
typedef S32 (*RecvPullMsg)(void *pView,GRAP_EVENT *pEvent);
typedef S32 (*Drawmore)(void *pView);

typedef struct tStrDispInfo
{
	U8  *string;
	U8	*keyStr;		//�ؼ��ַ���,����ʹ���ַ�����ɫ
	U16 color;			//�������ɫ
	U16 keyStrColor;	//�ؼ��ַ�����ɫ
	U8  font_size;		//����Ĵ�С
	U8  font_type;		//�������� unicode��GB��
	U8  flag;			//������ʾλ�� 1: ��  2:��  3:��
}tStrDispInfo;

typedef struct  tGrapViewAttr
{
	//U16	widgetType;	//�ؼ�����
	U16	id;			//�ؼ�ID
	
	U16 viewX1;		//�ؼ���ʼ�������
	U16 viewY1;
	U16	viewWidth;	//view�Ŀ�
	U16	viewHeight;	//view�ĸ�
	U16 touchX1;	//��������
	U16 touchY1;	//�������� ���
	U16 touchX2;	//�������� 
	U16 touchY2;	//�������� �յ�

	U16	clNormal;	//��������±���ͼƬ��id
	U16 clFocus;	//��������±���ͼƬ��id
	U16	color;		//ʹ��ָ����ɫ������
	U16 discolor;   //�ؼ���ɫ������ɫ
	U8	enable;		//0:�����ã������κδ���  1����ȫ����  2�����ɵ����ֻ��ʾ  (Ŀǰû�ж�1��2�����ִ���)
	U8  curFocus;	//�Ƿ�ǰ������ 0:��;1:��
	U8  FocusEnable;//�Ƿ����ӵ�н��� 1:����;0:������;������ӵ�н���Ŀؼ����л�����ʱ������

	U32 reserve;	//�����ֽڡ����ϲ㴫�ݲ�����ʹ�á�
}tGrapViewAttr;

typedef struct tGrapView
{
	tGrapViewAttr  view_attr; 

	HandleEvent handle;
	Draw draw;
	Destroy destroy;
	struct tGrapView *pNext;	//���ӿؼ�insert��Desk��ʵ���������ӡ�
	struct tGrapView *pPrev;
	struct tGrapView *pParent;	//�ӿؼ�ָ��desk��

}tGRAPVIEW;

//
typedef struct tGrapZorder
{
	tGRAPVIEW	*pMember;
	U16			zOrder ; //�ؼ��ı��,ֻ��Ϊ�˷����������

	struct tGrapZorder *pNext ;
}tGRAPZORDER;
//

// ������Ļ�ػ���

#define ReDraw(view, xy, wh)	\
    if (TRUE == *(((tGRAPDESKBOX *)(((tGRAPVIEW *)view)->pParent))->pUIFocus))  \
    {  \
       Grap_ReDrawFatherDesk(((tGRAPVIEW *)view)->pParent); \
       ( ((tGRAPVIEW *)view)->pParent->draw(((tGRAPVIEW *)view)->pParent, xy, wh) ); \
    }
    


#define GET_U16(p)		(* ( (U8 *)(p) ) | (*( (U8 *)(p) + 1 ) << 8) )


#define MAKEWORD(a, b)      ((U16)(((U8)(a)) | ((U16)((U8)(b))) << 8))
#define MAKELONG(a, b)      ((U32)(((U16)(a)) | ((U32)((U16)(b))) << 16))
#define LOWORD(l)           ((U16)(l))
#define HIWORD(l)           ((U16)(((U32)(l) >> 16) & 0xFFFF))
#define LOBYTE(w)           ((U8)(w))
#define HIBYTE(w)           ((U8)(((U16)(w) >> 8) & 0xFF))


#define SHOWSTRING_LIAN_LEFT	1
#define SHOWSTRING_LIAN_MID		2
#define SHOWSTRING_LIAN_RIGHT	4


#define STATE_NORMAL	0
#define STATE_PRESS		1
#define STATE_FOCUS		2
#define STATE_RELEASE	4

// define message
#define VIEW_BASE_MACRO	0x20000000
#define VIEW_DRAW		(VIEW_BASE_MACRO+1)
#define VIEW_UPDATE		(VIEW_BASE_MACRO+2)
#define VIEW_LINEUP		(VIEW_BASE_MACRO+3)
#define VIEW_LINEDN		(VIEW_BASE_MACRO+4)
#define VIEW_PAGEUP		(VIEW_BASE_MACRO+5)
#define VIEW_PAGEDN		(VIEW_BASE_MACRO+6)
#define VIEW_SETLABELFOCUS	(VIEW_BASE_MACRO+7)
#define VIEW_ENTER		(VIEW_BASE_MACRO+8)
#define VIEW_BLANK		(VIEW_BASE_MACRO+9)	// default: empty
#define VIEW_ESC		(VIEW_BASE_MACRO+10)
#define VIEW_QUIT		(VIEW_BASE_MACRO+11)
#define VIEW_LOSELABELFOCUS	(VIEW_BASE_MACRO+12)
#define VIEW_LEFT		(VIEW_BASE_MACRO+13)
#define VIEW_RIGHT		(VIEW_BASE_MACRO+14)
#define VIEW_ENABLE		(VIEW_BASE_MACRO+15)
#define VIEW_DISABLE	(VIEW_BASE_MACRO+16)
#define VIEW_DIGIT		(VIEW_BASE_MACRO+17)
#define VIEW_DELETE		(VIEW_BASE_MACRO+18)
#define VIEW_QUITALL	(VIEW_BASE_MACRO+19)
#define VIEW_PRESS		(VIEW_BASE_MACRO+20)
#define VIEW_OVER		(VIEW_BASE_MACRO+21)
#define VIEW_TIMER		(VIEW_BASE_MACRO+22)
#define VIEW_SETLINEFOCUS  	(VIEW_BASE_MACRO+23)
#define VIEW_LABELFOCUS  (VIEW_BASE_MACRO+24)
#define VIEW_INPUT		(VIEW_BASE_MACRO+25)
#define VIEW_INIT		(VIEW_BASE_MACRO+26)

#define VIEW_TAB		(VIEW_BASE_MACRO+27)//add by kong
#define VIEW_CLICK		(VIEW_BASE_MACRO+28)
#define VIEW_BTNUP		(VIEW_BASE_MACRO+29)
#define VIEW_DATA_ARRIVAL	(VIEW_BASE_MACRO+30)
#define VIEW_REV_SYSMSG	(VIEW_BASE_MACRO+31)
#define VIEW_START		(VIEW_BASE_MACRO+32)
#define VIEW_PAUSE		(VIEW_BASE_MACRO+33)
#define VIEW_END		(VIEW_BASE_MACRO+34)
#define VIEW_SDPLUG		(VIEW_BASE_MACRO+35)
#define VIEW_SDUNPLUG	(VIEW_BASE_MACRO+36)
#define VIEW_MODIFY		(VIEW_BASE_MACRO+37)
#define VIEW_POWERDOWN	(VIEW_BASE_MACRO+38)
#define VIEW_ANTICLICK  (VIEW_BASE_MACRO+39)
#define VIEW_DELETEALL	(VIEW_BASE_MACRO+40)

#define VIEW_CONTENT_CLEAN	(VIEW_BASE_MACRO+41)//add by FredHe for reset RICHEDIT
#define VIEW_KEYPULL        (VIEW_BASE_MACRO+42)
#define VIEW_REV_PULLMSG    (VIEW_BASE_MACRO+43)
#define VIEW_LOCK_SCR    	(VIEW_BASE_MACRO+44) //������Ϣ
#define VIEW_UNLOCK_SCR    	(VIEW_BASE_MACRO+45) //������Ϣ

#define VIEW_SHOWWINDOW    	(VIEW_BASE_MACRO+46)

#define VIEW_POPDIALOGUE   	(VIEW_BASE_MACRO+47)

#define VIEW_KEY_MSG        (VIEW_BASE_MACRO+48)



// return type
#define RETURN_CANCLE	0
#define RETURN_REDRAW	11
#define RETURN_QUIT		12
#define RETURN_CLEAR	14
#define RETURN_DELETE	18
#define RETURN_ENTER	19
#define RETURN_FOCUSMSG 20
#define RETURN_PIERCE   21


//�����¼�
#define KEYEVENT_0			48
#define KEYEVENT_1			49
#define KEYEVENT_2			50
#define KEYEVENT_3			51
#define KEYEVENT_4			52
#define KEYEVENT_5			53
#define KEYEVENT_6			54
#define KEYEVENT_7			55
#define KEYEVENT_8			56
#define KEYEVENT_9			57

#define KEYEVENT_STAR		58
#define KEYEVENT_SHARP		59

#define KEYEVENT_UP			60
#define KEYEVENT_DOWN		61
#define KEYEVENT_LEFT		62
#define KEYEVENT_RIGHT		63

#define KEYEVENT_OK			64
#define KEYEVENT_ESC		65
#define KEYEVENT_DELETE		66

#define KEYEVENT_F1			67
#define KEYEVENT_F2			68
#define KEYEVENT_SCAN		69

#define KEYEVENT_TAB		70

#ifdef __cplusplus
}
#endif

#endif


