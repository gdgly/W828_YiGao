#ifndef _SEL_WORD_H_
#define _SEL_WORD_H_


//���Ŀ�ʼ����
#define  SEL_PANEL_WR_ROW  14
#define  SEL_PANEL_WR_COL  7
#define  SEL_PANEL_IM_ROW  24
#define  SEL_PANEL_IM_COL  7

#define  SEL_PANEL_HIG  176
#define  SEL_PANEL_WID  228


/*---------------------------------------------------------
*����: sel_if_enable_selword
*����: �رջ�򿪱�ѡ��
*����: none
*����: none
*--------------------------------------------------------*/
int sel_if_enable_selword(tGRAPDESKBOX *pDesk);

/*---------------------------------------------------------
*����: sel_have_selword
*����: �Ƿ���б�ѡ��
*����: none
*����: -1
*--------------------------------------------------------*/
int sel_have_selword(tGRAPDESKBOX *pDesk);

#endif