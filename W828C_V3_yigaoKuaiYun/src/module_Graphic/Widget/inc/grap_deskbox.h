#ifndef __GRAP_DESKBOX_H__
#define __GRAP_DESKBOX_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"



//���и߿�
typedef struct _tGRAP_RECT
{
    U16 row; 
    U16 col;
    U16 hig;
    U16 wid;
}tGRAP_RECT;



#define RETURN_TOSWITCHMODE		200

/* ����ṹ*/
typedef struct tGrapDeskBox
{
	// view start 
	tGrapViewAttr	view_attr; 	/* ����Ĺ������� */

	HandleEvent		handle;
	Draw			draw;
	Destroy			destroy;
	struct tGrapView *pNext;
	struct tGrapView *pPrev;
	struct tGrapView *pParent;
	// view end
	
	struct tGrapView *pTail;//����������һ���ؼ�
	
	tGRAPZORDER		*pZorderList;	/* ���ؼ�����*/


	tMSG_QUEUE		*pGetMsg;
	tMSG_QUEUE		**pPutMsg;
	
	tMSG_BODY		*ptGetMsgBody;
	tMSG_PUT_PARAM	*ptPutMsgParam;
	
	U32             *pPullChild;
	U32             lastPoint;
    U32             pullTimeOut;
    
	U8				*pKeyCodeArr;//ָ�򰴼�����
	U8				*pUIFocus;	/* */
	U8				*pQuitCnt;	//�����˳�������������ֵΪ�����˳����㣩��Ϊ0xffʱ��ʾȫ���˳�
	
    U8				kDlgState;//��¼�Ի�����״̬
    U8				inputTypes;//��ǰ�������뷨����; bit0(123)   bit1(abc)  bit2(ABC)  bit3(��д)  bit4(ƴ��)  ��ӦbitΪ1ʱ���ʾ�ô������ʹ�ö�Ӧ�����뷨,����û��; �����ֵΪ0,�򲻺����뷨
    U8				inputMode;//��ǰ�������뷨  0: �ر�  1:123   2:abc   3:ABC  4:��д  5:ƴ��
    U8				scanEnable;//��ǰ�����Ƿ����ɨ��  0:��   1:��
    U16				editMinId;	//��ǰ������ϣ���л��������Сedit�ؼ�id
    U16				editMaxId;	//
    
    void            *pdata;
 	void			*pT9;
    void            (*threadCallback)(void *pDesk);//���̻ص�����ָ�룬�ڽ��̳�ʼ��ʱ��ֵ���ɼ̳�
   	char 			*pDisName;//��ʾ�����ϵ�����
   	
    struct tGrapDeskBox *pFather; //�������ָ��
    
}tGRAPDESKBOX;

//��desk�������ٴ����е�ʱ����Ҫ���صĺ���ָ��, pViewΪ��ǰdeskָ��
typedef S32 (*DeskSpecialAction)(tGRAPDESKBOX *pDesk,GRAP_EVENT *pEvent);
//ֱ�ӷ���������
typedef S32 (*ReturnToSwitchMode)(tGRAPDESKBOX *pDesk,U8 Pid_Flag);//kong
extern ReturnToSwitchMode Grap_ReturnToSwitchMode;//kong

S32 Grap_DeskboxHandle(void *pView, GRAP_EVENT *pEvent);
tGRAPDESKBOX *Grap_CreateDeskbox(tGrapViewAttr *pview_attr, U8 *pDisplayName);
tGRAPDESKBOX *Grap_CreateDeskboxNoCom(tGrapViewAttr *pview_attr, U8 *pDisplayName);
S32 Grap_DeskboxRun(tGRAPDESKBOX *pDesk);
S32 Grap_DeskboxDestroy(void *pView);
S32 Grap_DeskboxDraw(void *pView, U32 xy, U32 wh);


S32 Grap_UpdateZorder(void *pView, tGRAPZORDER *pZorder) ;
S32 Grap_ZorderListDestroy(void *pView);

extern DeskSpecialAction Grap_DeskSpecialAction;

//ֱ�ӷ��ص�������
S32 Grap_Return2Switch(tGRAPDESKBOX *pDesk,U8 Pid_Flag);


#define GRAP_INWIDGET_RANGE(x,y,pView) (x >= pView->view_attr.touchX1 && x < pView->view_attr.touchX2 && \
                		            y >= pView->view_attr.touchY1 && y < pView->view_attr.touchY2)
 

#ifdef __cplusplus
}
#endif

#endif


