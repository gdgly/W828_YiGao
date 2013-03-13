#ifndef __GRAP_EDIT2_H__
#define __GRAP_EDIT2_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"

//��ͷ��ɫ   0x3186
#define ARROWCOLOR			(0x000)//0x3186 0x320
#define SCROLLBARHEIGHT		10//��ͷ�����
#define SCROLLBARWIDTH		16//��ͷ�����

#define EDIT2_DATA_SIZE    12288//2048


typedef S32 (*getDefaultStrInfo)(void *pView);
typedef S32 (*Edit2GetFocus)    (void *pView); 
typedef U32 (*Edit2KeyInput)(void *pView, U32 *keycode);
typedef S32 (*Edit2Enter)(void *pView);
typedef S32 (*Edit2DrawOneLine)(void *pView, U32 line_idx, U16 x_start, U16 y_start);


typedef struct tEdit2Scroll
{
	U16 barScrollLen ;//���黬��������ĳ���
	U16 bar_start_y;//y������ʼ����������
	U16 bar_end_y;
	U16 bar_start_x;//x������ʼ����������
	U16 bar_end_x;
	U16 start_y_pos;//��һ�ι�����������λ��(���ڴ�ֱ��)
	U16 end_y_pos;
	
	U16 arrow1_areaX1;//�������Ϸ���ͷ����
	U16 arrow1_areaY1;
	U16 arrow1_areaX2;
	U16 arrow1_areaY2;
	U16 arrow2_areaX1;//�������·���ͷ����
	U16 arrow2_areaY1;
	U16 arrow2_areaX2;
	U16 arrow2_areaY2;
	U16 need;
	U16 barBGColor ;//������ɫ

}tEdit2Scroll;

typedef struct tGrapEdit2Priv
{
	U8 	*digt;     		// size - 2k
	U16 color;			//������ɫ 
	U8  font_size;		//����ֻ��16��16��24��24��32��32��������
	U8  font_type;
	U8  position;       //�ַ�����ʼ��ʾ��λ��
	U16 coverPicId;		//������Ҫˢ�µı���ͼƬID,�����Ҫ��ɫ��д��ֱ��д��ɫ

	U8	cursor_en;		//����Ƿ����
	U16 TimerTot;		//�����˸һ���ܼ��ʱ��
	U32 containLen;		//��������ĳ���(�ܳ���)

	U8  xoffset;
	U8  yoffset;		//��Ա������Ͻ�ƫ��
	U8  mallocFlag;     //�Լ����ٿռ仹��ʹ��ָ���Ŀռ� 0:�Լ����� 1:ʹ���Զ���
						/* ע�� : ʹ��ָ���Ŀռ� Ҫ�ȿ�����ĳ��ȴ�2���ֽڣ������� */
	U8  input;    /* ���뷨���� 0:�ر�  1:123  2:abc  3:ABC  4:��д  5:ƴ��*/
	
}tGrapEdit2Priv;

typedef struct tCharPos
{
	U32 firstCharPos;//�������ַ���λ��
	U16 LineNo;//�����ǵڼ��� ��0��
	struct tCharPos *pNext;
	
}tCHARPOS;

//�ÿؼ��ڲ�ʹ�õĿ�����Ϣ,����Ҫ�ⲿ����
typedef struct tGrapEdit2Ctrl
{
	//line info
	U32 topLine_idx;   //��ǰ���ĵ�һ�����������е�λ��
	U32 totLine;       //������
	//U32 *line_pos;     //�洢ÿ�����ַ���buff�е�λ�õ����飬��СΪLINE_MAX*4
	U16 line_height;   //ÿ�е��и�
	U16 line_width;    //ÿ�е��п� ҪС��viewWidth
	U16 line_screen;   //��ǰһ�������ɵ�����
	U16 char_line;     //һ�������ɵ��ַ���

	//cursor info
	U32	cursor_pos;		//cursor����Ӧ�����ַ����е�λ��
	U32 cursor_line;    //������ڵ���
	U8	showState;		//��굱ǰ����˸״̬��0: not show out; 1: show out
	U16 TimerLft;		//�����˸ʱ�������ʱ��
	
	tCHARPOS  *pLineInfoH;//ͷָ��
	tCHARPOS  *pLineInfoT;//βָ��
	
}tGRAPEDIT2CTRL;


typedef struct tGrapEdit2
{
	// view start 
	tGrapViewAttr view_attr;   //��ʾ����Ӧ�ø����и�(20)�ı��������� ��Ϊ16�ı��� take care

	HandleEvent handle;
	Draw draw;
	Destroy destroy;

	struct tGrapView *pNext;
	struct tGrapView *pPrev;
	struct tGrapView *pParent;
	// view end

	tGrapEdit2Priv edit_privattr;
	tGRAPEDIT2CTRL edit_ctrl;
	getDefaultStrInfo get_default_str;
	Edit2GetFocus     get_focus; //�����㷢���ı��ʱ����Ҫ�����뷨����ʾ������ʱ��������ظú���
	RecvSysMsg recvSysMsg;
	
	RecvPullMsg recvPullMsg;//������ק��Ϣ
	Edit2KeyInput	edit2KeyInput;
	tEdit2Scroll  edit2Scr;
	
	Edit2Enter		edit2Enter;
	Edit2DrawOneLine edit2DrawOneLine;
	
			
}tGRAPEDIT2;


tGRAPEDIT2 *Grap_InsertEdit2(void *pView, tGrapViewAttr *pview_attr, tGrapEdit2Priv *pedit_privattr, U16 barBgColor);
tGRAPEDIT2 *Grap_CreateEdit2(tGrapViewAttr  *pview_attr, tGrapEdit2Priv *pedit_privattr, U16 barBgColor);
S32 Grap_Edit2GetDefaultStr(void *pView);

extern const U16 Edit2ScrollBarColor[16];

extern const U8 UpArrow[16];//w=16 h=8
extern const U8 DownArrow[16];//w=16 h=8


#ifdef __cplusplus
}
#endif

#endif