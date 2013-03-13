/**************************************************************************
 1:��֮ǰ��list�ؼ�������ͬ,�ÿؼ��Դ�������,�������λ���Ǹ��ݵ�ǰ
  ���е�һ���λ�õ�����(PC�ϵĹ����������϶������),֮ǰ�Ĺ�������
  ���ݽ������λ�õ�����.
 2:�ÿؼ���������ڵ�ǰ����.
 3:��������СΪ10�����ظ�
 
 4:'+'����ѡ��  ��ѡ�������ڲ�����ק
                ----------------------
               | + ȫ��      0011/0022|  <-----------������
               |----------------------|
               | +  ��ʾ�ַ�       |  |
               |-------------------|��|
               | +  ��ʾ�ַ�       |  |
 list��Ŀ��--> |-------------------|��|  <-----------������
               | +  ��ʾ�ַ�       |  |
               |-------------------|��|
               | +  ��ʾ�ַ�       |  |
               |-------------------|��|
               | +  ��ʾ�ַ�       |  |
               |-------------------|--|


***************************************************************************/

#ifndef __GRAP_SUPPERLIST_H__
#define __GRAP_SUPPERLIST_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"


#define 	NEED_TITLE			0x01	//��Ҫ������
#define		CHECK_BOX			0x02	//��ѡ��
#define		NEED_TIMER			0x04	//�ַ�����ʱ��Ҫtimer�ƶ�
#define		SELECT_ALL			0x08	//ѡ����ȫ��
#define		NEED_SCRBAR			0x10	//Ҫ��������
#define		CONFIRMANDENTER		0x20	//��һ�ε���ͽ���


#define		FIRSTPOINT_INTITLEICON		1//��һ�ε�����ڱ����е�Сͼ����
#define		FIRSTPOINT_INSCRBAR			2//��һ�ε�����ڹ�����
#define		FIRSTPOINT_INLIST			4//��һ�ε������list��
#define		FIRSTPOINT_INLISTICON		8//��һ�ε������list����Сͼ����
#define		HAS_PULLED					128//������ק


typedef S32 (*SupList_int)(void *pView);
typedef S32 (*SupList_update)(void *pView);
typedef S32 (*SupList_timer)(void *pView, U16 itemNo);
typedef S32 (*SupList_lineup)(void *pView,U32 flag);
typedef S32 (*SupList_linedown)(void *pView,U32 flag);
typedef S32 (*SupList_pageup)(void *pView);
typedef S32 (*SupList_pagedown)(void *pView);

typedef S32 (*SupList_getstr)(void *pView, U16 itemNo, U8 *str, U16 maxLen);
typedef S32 (*SupList_drawIcon)(void *pView, U16 itemNo);
typedef S32 (*SupList_drawone)(void *pView, U16 itemNo);
typedef S32 (*SupList_drawsrc)(void *pView);
typedef S32 (*SupList_drawtitle)(void *pView);
typedef S32 (*SupList_enter)(void *pView, U16 itemNo);
typedef S32 (*SupList_delete)(void *pView);
typedef S32 (*SupList_delopen)(void *pView, U16 itemNo);
typedef S32 (*SupList_updatebytotal)(void *pView);
typedef S32 (*SupListKeyInput)(void *pSupLister, U16 keycode);//��������,��Ҫ������ݼ�����(1,2,3,4.....)

typedef struct tItemInfo
{
	U16 isSelect;  //�����Ƿ�ѡ��
	U16 curItemNo; //��ǰ��

	struct tItemInfo  *pNext;
	
}tItemInfo;

typedef struct tSupperListPriv
{
	U8  ListInfo;		//bit0:�Ƿ���Ҫ������; 1:��Ҫ bit1:�Ǹ�ѡ���ǵ�ѡ;1:��ѡ 
						//bit2:�Ƿ���Ҫtimer;1:Ҫ  
						//bit3:�Ƿ�ѡ����ȫ��;1:�� (��ֵһ��Ϊ0) bit4:��ʱ�Ƿ���Ҫ��ʾ�Ҳ������ 1:��Ҫ (��ʼ��ʱ��������)
						//bit5:�Ƿ��һ�ε���ͽ��� 1:��
	//��Ҫ������ʱ������
	U8  titleH;			//�����еĸ�--�������Ҫ������,����������0
	U16 titleW;			//��---�������ؼ��Ŀ�һ��			

	//
	U16 itemH;			//ÿһ��ĸ�
	U16 itemScreenNum;	//һ��������ʾ����ĸ��� (������������)
	U16 fontNoFocusColor;//�ǽ���ʱ�ַ���ɫ
	U16 fontFocusColor;	//����ʱ�ַ���ɫ
	U16 topItemNo;		//��ǰһ����һ��ı��
	U16 focusItemNo;	//��������
	U16 totalItem;		//������ (������������)
	U16 longCoverPicId;	//����������ID,--����Ҫ��ʾ���Ҳ�Ĺ�����ʱ��
	U16 shortCoverPicId;//�̷�������ID,--��Ҫ��ʾ���Ҳ�Ĺ�����ʱ��
	U16 backPicId;		//�ÿؼ�û��ָ��ͼƬʱ��Ҫ�ñ���ˢ��,����ͼƬ��ID
	
	U16 iconX;			//ÿ��Сͼ�����ʾλ��---����ڸ���viewX1��ƫ��
	U16 iconY;			//ÿ��Сͼ�����ʾλ��---����ڸ���viewY1��ƫ��
	U16 iconW;
	U16 iconH;
	U16 icon_NoSelectId;  //δѡ��ʱ��ͼ��
	U16 icon_SelectId;    //ѡ��ʱ��ͼ��
	
	U16 strX;
	U16 strY;			//�ַ���ʼ��ʾ��λ�� ��Сͼ��һ��,������ڸ�����ʼ���λ��
	U16 screenStrLen;	//һ��������ʾ���ֽ���(Ҫ��ʾ���ַ���������ʱ������ʾ)
	U16 firstCharPos;	//��ʾ��LIST��ͷ���ַ��������ַ����е�λ��,��ֵΪ0
	
	U16 TimerLft;		
	U16 TimerTot;		//��Ҫtimerʱ����Ч
	U8  font_size;      //����Ĵ�С
	U8  font_type;      //�������� unicode��GB��
	U8  position;      //��ʾλ��  ���п���������
	
	//����3������Ҫ��ȷ��д,��ʹ����Ҫ������ҲҪ��д 
	U16 scrBarHeadId;	//�������Ϸ�ͼƬ ���ϼ�ͷ��ͼƬ
	U16 scrBarTailId;	//�·�			 ���¼�ͷ��ͼƬ
	U16 scrBarId;		//�м䲿��
	
}tSupListPriv;


typedef struct tListScrBar
{
	U16 viewX1;
	U16 viewY1;
	U16 viewX2;
	U16 viewY2;			//��������������
	
	U16 scrLen;			//�������򳤶�
	U16 scrStartY;		//���������
	U16 scrEndY;		//�������յ�
	
	U16 barStart;		//�����ϴ����
	U16 barEnd;			//�����ϴ��յ�
	
	U16 arrow1_Y1;		
	U16 arrow1_Y2;
	U16 arrow2_Y1;
	U16 arrow2_Y2;		//��ͷ��Ϣ
	
	U16 barH;//������ĸ�
	float	ratio;//��ֵ
		
}tListScr;

typedef struct tSupperList
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
	
	tSupListPriv	SupList_priv;
	tListScr		ListScr;
	tItemInfo		*pItemInfoH;//head
	tItemInfo		*pItemInfoT;//tail
	
	RecvSysMsg		recvSysMsg;
	RecvPullMsg		recvPullMsg;
	
	SupList_int			SupListInt;
	SupList_update		SupListUpDate;
	SupList_timer		SupListTimer;
	SupList_lineup		SupListLineUp;
	SupList_linedown	SupListLineDown;
	SupList_pageup		SupListPageUp;
	SupList_pagedown	SupListPageDown;
	SupList_getstr		SupListGetStr;
	SupList_drawIcon	SupListDrawIcon;
	SupList_drawone		SupListDrawOne; 
	SupList_drawsrc		SupListDrawSrc;
	SupList_drawtitle	SupListDrawTitle;
	SupList_enter		SupListEnter;
	SupList_delete		SupListDelete;
	SupList_delopen		SupListDelOpen;
	SupList_updatebytotal	SupListUpDateByTotal;
	SupListKeyInput		SupkeyInput;
	
	U8 timerEn;
	U8 lastKey;	//�ϴΰ���ֵ  (bit7��¼֮ǰ��timer�Ƿ�enable)
	U8 pullFlag;//bit7Ϊ1��ʾ�϶���
	
}tSUPPERLIST;

tSUPPERLIST *Grap_InsertSupList(void *pView, tGrapViewAttr *pview_attr, tSupListPriv *plst_privattr);


extern U16 gFrontColor;
extern const U8 Suspension_points[];
extern int Grap_GetDispStr(Character_Type_e kStr_Type,U16 factwid,U8 *tmpStr);
extern const U8 *const titalStr[];

#ifdef __cplusplus
}
#endif

#endif


