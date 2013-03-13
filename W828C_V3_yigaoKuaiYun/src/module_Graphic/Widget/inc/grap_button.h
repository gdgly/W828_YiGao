#ifndef __GRAP_BUTTON_H__
#define __GRAP_BUTTON_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"

#define MAX_BUTTON_NAME_LEN	16	//button�������ִ����ȡ�

#define LEFT_POSITION      1
#define CENTER_POSITION    2
#define RIGHT_POSITION     4

//����ṹ����֧��button����ʾbuffͼƬ����ʽ��
// button�ϱ����ڿ�����ʾ3�ֶ�����pic(ID��ʽ)��pic(buff��ʽ)������
typedef struct tGrapButtonBuffPicInfo_st
{
	U16 off_x1;		//ͼƬҪ��ʾ�ĵط������button���Ͻǵ�ƫ��
	U16 off_y1;
	U16 width;		//ͼƬ�Ŀ�͸�
	U16 height;
	U8  *rgb;		//ͼƬbuffer

}tGrapButtonBuffPic;


typedef struct tGrapButtonPriv
{
	U8  *pName;
	U16 cl_normal;     //�ǰ���״̬��ɫ
	U16 cl_focus;	   //����״̬��ɫ
	U8  font_size;     //����Ĵ�С
	U8  font_type;     //�������� unicode��GB��
	U8  position;      //��ʾλ��  ���п���������
	U8 	keycode;	//��button�󶨵İ�����ֵ��Buttonֻ��Ӧ���������
	U16 backgndPicId;	//��Ҫ�Ա���ˢ�µ�ͼƬ
	tGrapButtonBuffPic *pBufPic;	//��buff��ʽ��ͼƬʱ�õ�buff
	
}tGrapButtonPriv;

typedef struct tGrapButton
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
	
	tGrapButtonPriv btn_privattr;  //name �ַ�������ʾ����
	PressEnter pressEnter;
	RecvSysMsg recvSysMsg;
	RecvPullMsg recvPullMsg;

	U8	state;	// now has only two state: normal and press. later maybe add focus. 0: normal, 1: press.

}tGRAPBUTTON;

tGRAPBUTTON *Grap_InsertButton(void *pView, tGrapViewAttr  *pview_attr, tGrapButtonPriv *pbtn_privattr);

#ifdef __cplusplus
}
#endif

#endif


