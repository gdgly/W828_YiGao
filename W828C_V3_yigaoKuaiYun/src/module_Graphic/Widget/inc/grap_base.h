#ifndef __GRAP_BASE_H__
#define __GRAP_BASE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "grap_view.h"



/*=========================================================================*/
/*----------------------------------------------
*����: Grap_GetViewByID
*����: ��ȡ��ǰ����ָ��ID�Ŀؼ�
*����: view �ؼ�ָ��
*����: id: Ҫ�ҵĿؼ�id
*----------------------------------------------*/
tGRAPVIEW *Grap_GetViewByID(void *view, U16 id);

/*----------------------------------------------
*����: Grap_GetCurFocus
*����: ��ȡ��ǰ���役��ؼ�,�����panel���������panel�еĿؼ�
*����: pView �ؼ�ָ�� pPanelid:�������ؼ���panel��,�򷵻�panel��id   ����Ҫ�ò���ʱ��NULL
*����: pFocus: ������ؼ�ָ��,���Ϊ�ҵ��������򷵻�NULL
*----------------------------------------------*/
tGRAPVIEW *Grap_GetCurFocus(void *pView, U16 *pPanelid);

/*----------------------------------------------
*����: Grap_WriteBMPColor
*����: ��ָ������ɫ��ȡģ����
*����: x1:��
       y1:��
       width:ȡģ���ݿ�
       height:ȡģ���ݸ�
       BmpColor:ȡģ����
       colorP:��ɫ
*����: none
*----------------------------------------------*/
void Grap_WriteBMPColor(U16 x1, U16 y1, U16 width, U16 height, U8 *BmpColor, U16 colorP);
/*----------------------------------------------
*����: Grap_BrushScreen
*����: ��ָ������ɫ���ָ������
*����: x1:��ʼ��
       y1:��ʼ��
       x2:������
       y2:������
       colorP:��ɫ
*����: none
*----------------------------------------------*/
void Grap_BrushScreen(U16 x1, U16 y1, U16 x2, U16 y2, U16 colorP);
/*----------------------------------------------
*����: Grap_ClearScreen
*����: �õ�ǰ����ͼˢ��ָ������
*����: x1:��ʼ��
       y1:��ʼ��
       x2:������
       y2:������
*����: none
*----------------------------------------------*/
void Grap_ClearScreen(U16 x1, U16 y1, U16 x2, U16 y2);
/*----------------------------------------------
*����: Grap_ShowPicture
*����: ��ָ��λ����ʾָ��id�ŵ�ͼƬ
*����: picId:ͼƬö��,Ҫȷ������
	   posX:��ʼ��
       posY:��ʼ��
*����: none
*----------------------------------------------*/
void Grap_ShowPicture(U16 picId, U16 posX, U16 posY);
/*----------------------------------------------
*����: Grap_Show_Part_BackPicture
*����: ��ʾָ��ͼƬ��һ����
*����: pView1:��ͼ���ڵĿؼ�(��һ���Ǵ�ı���ͼ)
	   pView2:��ǰҪˢ�µĿؼ�,����Ҫ��pView1��
*����: none
*----------------------------------------------*/
void Grap_Show_Part_BackPicture(void *pView1,void *pView2);
/*----------------------------------------------
*����: Grap_WriteString
*����: ��ָ��λ����ʾ�ַ���
*����: 
	   
*����: none
*----------------------------------------------*/
void Grap_WriteString(U16 x1, U16 y1, U16 x2, U16 y2, tStrDispInfo *disp_info);
/*----------------------------------------------
*����: Grap_WriteStringMulti
*����: ��ָ����������ʾ�����ַ�,�����Զ�����;
*����: 
	   
*����: none
*----------------------------------------------*/
void Grap_WriteStringMulti(U16 x1, U16 y1, U16 x2, U16 y2, tStrDispInfo *disp_info);
/*----------------------------------------------
*����: Grap_ChangeFocusEnable
*����: �л�������
*����: pView:�ؼ�ָ��
*����: none
*----------------------------------------------*/
void Grap_ChangeFocusEnable(tGRAPVIEW *pView);
/*----------------------------------------------
*����: Grap_SendMessage
*����: ��ָ���ؼ�������Ϣ(������ʽ)   idΪ0��ʾ�����巢��
*����: pView:�ؼ�ָ��
       pEvent:��Ϣ
*����: none
*----------------------------------------------*/
int Grap_SendMessage(tGRAPVIEW *pView, GRAP_EVENT *pEvent);

#ifdef __cplusplus
}
#endif

#endif

