/*******************************************************
* ---------��2009-12-22
*******************************************************/
#ifndef __GRAP_COMBOBOX_H__
#define __GRAP_COMBOBOX_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"


typedef S32 (*Combox_int)(void *pView);
typedef S32 (*Combox_drawone)(void *pView, U16 i);
typedef S32 (*Combox_enter)(void *pView, U16 i);
typedef S32 (*Combox_timer)(void *pView, U16 i);
typedef S32 (*Combox_drawscr)(void *pView);
typedef S32 (*Combox_getstr )(void *pView,U16 i, U8 *pStr);
typedef S32 (*ComboxGetFocus)(void *pView);

typedef struct _tComboBoxScr
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

}tComboboxScroll;


typedef struct tComboboxPriv
{
	U16 total_num;	//����Ŀ��
	U16 dis_num;	//һ����������ʾ������Ŀ��(����*�ؼ��� <= ����/2)
	U8  cur_index;	//��ǰ����������
	U8  opencur_index;	//չ���󽹵���
	U8  top_index;	//��ǰ����ʾ������Ŀ�е�һ����Ŀ������Ŀ�е�����
	U8  direction;	//չ��ʱ�ķ���(�� 0,  �� 1)
	
	U8	timerenable;//�ַ�����ʱ�Ƿ����
	U8  closewidth;	//�պ�ʱ������ʾ�ַ�����Ŀ��
	U8  closecharlen;//�պ�ʱһ�������ʾ���ַ���,����ʱ������ʾ
	U8  openwidth;	//չ��ʱ������ʾ�ַ����Ŀ��
	U8  opencharlen;//չ��ʱһ����������ʾ���ַ���,������ֵ������ʾ(ע���Ҳ��Ƿ��й�����)
	U16 firstcharpos;//�������ַ�λ��,��ֵΪ0
	U16 timerlft;		
	U16 timertot;
	
	U16 font_size;  //����Ĵ�С
	U16 font_type;  //�������� unicode��GB��
	
	U16 shortbar;	//���������,����
	U16 longbar;	//���������,չ����
	U16 bar_color;	//�������ɫ(�޷�����ͼƬʱʹ��)
	U16 clrnormal;	//�ǽ���ʱ��ɫ   �ַ���ɫ
	U16 clrfocus;	//����ʱ��ɫ     �ַ���ɫ
	U16 normalbgcolor;	//�ǽ����ǵ�ͼ��ɫ
	U16 openpicid;	//չ�����ͼƬ   ��ͼƬʱ��normalbgcolor���
	
	U16 start_x;	
	U16 start_y;	//��Ը������Ͻ�λ��
	U16 charhigh;	//�ַ���
	
	U16 backpicid;	//������Ҫˢ�µı���ͼƬID
	
	U16 needscr;	//�ÿؼ���������֪���Ƿ���Ҫ�Ҳ������,����ȷ��д
	//����3������Ҫ��ȷ��д,needscrΪ0ʱ���Բ�����д 
	U16 scrBarHeadId;	//�������Ϸ�ͼƬ ���ϼ�ͷ��ͼƬ
	U16 scrBarTailId;	//�·�			 ���¼�ͷ��ͼƬ
	U16 scrBarId;		//�м䲿��
	
}tComboboxPriv;

typedef struct tGrapCombobox
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

	//combo box  private
	tComboboxPriv  comboPriv;
	tComboboxScroll comboScr;
	
	Combox_int		comboxinit;
	Combox_drawone	comboxdrawone;
	Combox_enter	comboxenter;
	Combox_timer	comboxtimer;
	Combox_drawscr	combocdrawscr;
	Combox_getstr   comboxgetstr;
	ComboxGetFocus  combox_getFocus;
	
	RecvSysMsg		recvSysMsg;
	RecvPullMsg		recvPullMsg;
	
	U8 list_opened; // 1:�����б��Ѵ�
	
	U8  pullflag; //��¼���µ�����
	
	U16 openX1;
	U16 openX2;
	U16 openY1;
	U16 openY2;//չ����Ĵ�������
	
	U16 tempwidth;//��¼չ����Ŀ��,��ֹ�й�����ʱ�ı�˽�������е�ֵ

} tGRAPCOMBOBOX;

extern const U16 ComboboxScrollBarColor[24];

tGRAPCOMBOBOX *Grap_InsertCombobox(void *pView, tGrapViewAttr  *pview_attr, tComboboxPriv  *pcomboPriv);


#ifdef __cplusplus
}
#endif

#endif //__GRAP_COMBOBOX_H__ 
