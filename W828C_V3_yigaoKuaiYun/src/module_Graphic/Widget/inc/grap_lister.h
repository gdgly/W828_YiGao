#ifndef __GRAP_LISTER_H__
#define __GRAP_LISTER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"
	
typedef struct tGrapLister tGRAPLIST;


typedef S32 (*UpdateList)(tGRAPLIST *pLister, U16 i);
typedef S32 (*GetListStr)(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen);
typedef S32 (*EnterLister)(tGRAPLIST *pLister, U16 i);
typedef S32 (*ListerTimer)(tGRAPLIST *pLister, U16 i);
typedef S32 (*ListDrawIcon)(tGRAPLIST *pLister,U16 i);//��Сͼ��
typedef S32 (*ChangeListItem)(tGRAPLIST *pLister,U16 i);//���ı�focus itemʱ���غ���
typedef S32 (*ListKeyInput)(tGRAPLIST *pLister, U16 keycode);//��������,��Ҫ������ݼ�����(1,2,3,4.....)

typedef struct tGrapListerPriv
{
	U8  lstHeight;		//listÿ�е��и�
	U8  lstNum;			//һ����list�ĸ���
	U16	lstColor;		//list��������ɫ
	U16	lstFocusColor;	//list���ױ���ɫ
	U16 focusItem;		//��ǰ��������idx
	U16	totalItem;		//�ܼӽ���������
	U16	topItem;		//��ǰ�����ϱ�һ����idx��(֧��list���Զ�����,��һ�ν���ʱ��ʼĬ��Ϊ0)
	U16 backgndPicId;	//��Ҫ����ˢ�µı���ͼID
	U16 scrbarPicId;	//����ͼƬID(���������)
	
	U8  x_start;		
	U8  y_start;		//��ʼ��ʾ�ַ���λ��, ����ڱ������Ͻ�
	U8  curLen;			//����һ��������ʾ���ַ���(Ҫ��ʾ���ַ���������ʱ������ʾ)		
	U16 firstCharPos;	//��ʾ��LIST��ͷ���ַ��������ַ����е�λ��,��ֵΪ0
	U16 TimerLft;		
	U16 TimerTot;
	U8	enable;			//�ַ�����ʱ�Ƿ����
	U8  font_size;     //����Ĵ�С
	U8  font_type;     //�������� unicode��GB��
	U8  position;      //��ʾλ��  ���п���������
	U8  isShowCount;   //�Ƿ���ʾ "��ǰ��/�ܼ���"  ����Ҫ��ʾ����ؼ��߶������ҪԤ��20���ظ�
	U8	numkeyenable;	//�Ƿ�ʹ�����ֿ�ݼ�  1:ʹ�� 0:��ʹ��   ʹ�����ֿ�ݽ�,�����ּ����Խ����Ӧ��ŵ���

}tGrapListerPriv;

typedef struct tGrapLister
{
	// view start.
	tGrapViewAttr view_attr;

	HandleEvent handle;
	Draw draw;
	Destroy destroy;

	struct tGrapView *pNext;
	struct tGrapView *pPrev;
	struct tGrapView *pParent;
	// view end.

	// bellow is Lister property
	tGrapListerPriv lst_privattr;

	UpdateList updateList;
	GetListStr getListString;	// ��������������أ���ΪĬ�Ϻ�����Ϊ��ֱ�ӷ��ء������������ִ���
	EnterLister enter;			// ��list���水�س���ʱӦ�����ĸ����档�������Ҳ�������أ���Ȼ�޷������²㺯����
	ListerTimer	listtimer;
	ListDrawIcon	drawIcon;
	ChangeListItem   changeItem;
	ListKeyInput	keyInput;
	RecvSysMsg recvSysMsg;
	RecvPullMsg recvPullMsg;
 	
 	U8	ConfirmAndEnter;		//1:�����ֱ�ӽ���.0:��һ��ѡ��,�ڶ��ν���
	U8	lastKey;	//�ϴΰ���ֵ  (bit7��¼֮ǰ��timer�Ƿ�enable)
	U32 startPos; //��קʱ��һ���λ��
	
}tGRAPLIST;

// ���ų����Ľӿڡ�
tGRAPLIST *Grap_InsertLister(void *pView, tGrapViewAttr *pview_attr, tGrapListerPriv *plst_privattr);

// ���º�����Ϊ���ϲ�������ؿ��ŵ�
S32 Grap_ListerUpdateItem(tGRAPLIST *pLister, U16 i);
S32 Grap_ListerEnterItem(tGRAPLIST *pLister, U16 i);
S32 Grap_ListerGetString(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen);
S32 Grap_ListerDraw(void *pView, U32 xy, U32 wh);
S32 Grap_ListerHandle(void *pView, GRAP_EVENT *pEvent);
S32 Grap_ListerTimer(tGRAPLIST *pLister, U16 i);
S32 Grap_ListDrawIcon(tGRAPLIST *pLister, U16 i);

#define LISTSTR_MAXLEN 	100


#ifdef __cplusplus
}
#endif

#endif


