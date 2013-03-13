#ifndef __GRAP_PANEL_H__
#define __GRAP_PANEL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"



typedef S32 (*Panel_int)(void *pView);
typedef S32 (*InsertWidget)(void *pView, void *pWidget);
typedef S32 (*PanelTimer)(void *pView, GRAP_EVENT *pEvent);
typedef S32 (*PanelSysmsg)(void *pView, GRAP_EVENT *pEvent);
typedef S32 (*PanelPullmsg)(void *pView, GRAP_EVENT *pEvent);

typedef struct _tPANELPRIV
{
	U16			backgndPicId;
	U16			backgndColor;
	tGRAPVIEW	*pSubView;
	
	
}tPANELPRIV;


typedef struct _tPANEL
{
	tGrapViewAttr  view_attr; 

	HandleEvent		handle;
	Draw			draw;
	Destroy			destroy;
	
	struct tGrapView *pNext;	//���ӿؼ�insert��Desk��ʵ���������ӡ�
	struct tGrapView *pPrev;
	struct tGrapView *pParent;	//�ӿؼ�ָ��desk��
	
	
	struct tGrapView *pTail;
	tPANELPRIV		PanelPriv;
	
	Panel_int		panelinit;
	InsertWidget	panelInsert;
	PanelTimer		paneltimer;
	PanelSysmsg		panelsysmsg;
	PanelPullmsg	panelpullmsg;
	
	tGRAPZORDER		*pPanelZ;
	
	U32             *pPullChild;
	
}tPANEL;

tPANEL *Grap_InsertPanel(void *pView, tGrapViewAttr  *pview_attr, tPANELPRIV *pPanel_priv);
tPANEL *Grap_CreatePanel(tGrapViewAttr *pview_attr, tPANELPRIV *pPanel_priv);


#ifdef __cplusplus
}
#endif

#endif//__GRAP_PANEL_H__


