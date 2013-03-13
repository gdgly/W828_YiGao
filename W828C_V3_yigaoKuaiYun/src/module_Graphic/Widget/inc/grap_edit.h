#ifndef __GRAP_EDIT_H__
#define __GRAP_EDIT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"

#define MAX_DIGITEDIT_LEN 	300

#define CONTAIN_ANY		0
#define	CONTAIN_NUM		1
#define	CONTAIN_CHAR	2
#define CONTAIN_NUM1    4

typedef struct tGrapEdit tGRAPEDIT;
typedef S32 (*GetFocus)(tGRAPEDIT *pView); 
typedef S32 (*EditEnter)(void *pView);
typedef U32 (*EditKeyInput)(void *pView, U32 *keycode);
typedef U32 (*EditKeyEnd)(void *pView, U8 type);//�����ɾ������  type:0 ����   1:ɾ��
typedef S32 (*EditTimer)(void *pView);
typedef S32 (*EditDropListGetStr)(tGRAPEDIT *pView, U16 i, U8 *str);
typedef S32 (*EditDropListDrawScroll)(tGRAPEDIT *pView);
typedef S32 (*EditDropListDrawOne)(tGRAPEDIT *pView, U16 i);
typedef S32 (*EditDrawInputCnt)(tGRAPEDIT *pView); 
typedef S32 (*EditDropListEnter)(tGRAPEDIT *pView, U16 i); 

typedef struct tGrapEditPriv
{
	U8 	*digt;
	U16 color;			//������ɫ 
	U8  font_size;
	U8  font_type;		//gbk,unicode,utf-8
	U8  xoffset;
	U8  yoffset;		//��Կؼ����Ͻ�ƫ��
	U8  len;			//��ǰ�༭���ܹ���ʾ���ַ�����(��Ϊ�Ǽ򵥵ı༭��,ֻ�е�һ����ַ�)
	U8	position;		//���䷽ʽ
	U16 containLen;		//��������ĳ���(�ܳ���)
	U16	Pos;			//����Ӧ�����ַ����е�λ��
	U16 firstCharPos;	//�ô��ڵ�һ����ʾ�ַ����ַ�����λ��
	U8  containFlag;	//��������(0:������,���Խ����κ��ַ�;1:ֻ����������;2:�������뺺��������ַ�,4:���ֺ�.)

	U16 coverPicId;			//������Ҫˢ�µı���ͼƬID
	
	U8	showState;
	U8	enable;			//����Ƿ����
	U16 TimerLft;		//�����˸ʱ�������ʱ��
	U16 TimerTot;		//�����˸һ���ܼ��ʱ��
	
	U8  view_pwd;       //��ʾ��ʽ 0:�ַ���ʾ 1:������ʾ��ʽ  **************

	U8  mallocFlag;     //�ؼ��Զ����ٿռ仹��ʹ��ָ���Ŀռ� 0:�ؼ��Զ����� 1:ʹ���Լ��ƶ���(��digt���ʹ��)
						/* ע�� : ʹ��ָ���Ŀռ� Ҫ�ȿ�����ĳ��ȴ�2���ֽڣ������� */	

	U8  scanEnable;     // �Ƿ����ɨ������

	U8  input ;    /* ���뷨���� 0:�ر�  1:123  2:abc  3:ABC  4:��д  5:ƴ��*/

	U8	sendOK;		//ɨ��ɹ����Ƿ��Զ�����һ��OK����Ϣ  0:������   1:����

}tGrapEditPriv;


//edit������������Ϣ
typedef struct _drop_list_t
{
	U8	state;	//0:δչ��   1:չ��
	U8	laststate;	//�ϴ�״̬
	U8	direction;	//0:����  1:����
	U8	high;	//չ����ĸ߶�
	U8	listnum;	//չ����һ��������ʾ������
	U8	total;	//������
	U8	focus;	//��ǰ������ ��0��ʼ����  0xff��ʾû�н�����
	U8	top;
	U16	backColor;	//չ��ʱ��ͼ��ɫ
	U16 focusColor;	//��������ɫ
	
}tDROPLIST;

typedef struct tGrapEdit//����ģ�����ҹ���.�·���ʾ������ѯ���ִ� (��δʵ��)
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

	tGrapEditPriv edit_privattr;

	RecvSysMsg recvSysMsg;
	GetFocus   get_focus; //�����㷢���ı��ʱ����Ҫ�����뷨����ʾ������ʱ��������ظú���  		
	RecvPullMsg recvPullMsg;

	EditEnter	editenter;
	EditKeyInput	editKeyInput;
	EditKeyEnd		editKeyEnd;
	EditTimer		editTimer;
	EditDropListGetStr dropListGetStr;
	EditDropListDrawScroll dropListDrawScroll;
	EditDropListDrawOne    dropListDrawOne;
	EditDrawInputCnt	   DrawInputCnt;
	EditDropListEnter	   dropListEnter;
	
	tDROPLIST	dropList;
	
	U8  pullflag;		//��ק��־ bit0:0:δ��������ק 1:��������ק bit1:0:��ǰ��ק 1:�����ק
	U8  charh;
	U8  charw;
	U16 disLenBack;//��Щ���뷨չ������Ҫ����editλ��,���ܵ���˽�������е�len�б䶯,������д�䶯���ֵ
	
}tGRAPEDIT;

tGRAPEDIT *Grap_InsertEdit(void *pView, tGrapViewAttr *pview_attr, tGrapEditPriv *pedit_privattr);

#ifdef __cplusplus
}
#endif

#endif


/*
2012-09-12 ������  ������������,��������������������,������������Ӧ����ʾ������;
������Ĭ������,�����·�����չ��,Ĭ��������չ���������ʾ4����Ϣ,����4����Ҫ����;
������չ�����ͨ�����°���ѡ��;
*/