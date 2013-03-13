#include "grap_view.h"
#include "grap_api.h"
#include "grap_base.h"
#include "grap_event.h"
#include "grap_deskbox.h"

/*===============================================================*/
/*----------------------------------------------
*����: Grap_GetViewByID
*����: ��ȡ��ǰ�����ռ���ָ��ID�Ŀؼ�
*����: view �����ؼ�ָ��
*����: id: Ҫ�ҵĿؼ�id
*----------------------------------------------*/
tGRAPVIEW *Grap_GetViewByIDFromComm(void *view, U16 id)
{
	tGRAPVIEW	*pView = NULL;
	
	pView = (tGRAPVIEW *)view;
	for ( ; pView != NULL; pView = pView->pNext)
	{
		if (pView->view_attr.id == id)
		{
			break;
		}
	}

	return pView;
}

/*----------------------------------------------
*����: Grap_GetViewByID
*����: ��ȡ��ǰ����ָ��ID�Ŀؼ�
*����: view �ؼ�ָ��   id: Ҫ�ҵĿؼ�id
*����: 
*----------------------------------------------*/
tGRAPVIEW *Grap_GetViewByID(void *view, U16 id)
{
	tGRAPVIEW	*pView = NULL;
	tPANEL		*pPanel;
	tGRAPVIEW	*pSub;
	tPANELPRIV	*pPriv;
	
	pView = ((tGRAPVIEW *)view)->pParent;
	for ( ; pView != NULL; pView = pView->pNext)
	{
		if (pView->view_attr.id == id)
		{
			break;
		}
		else if(Panel_IfPanel(pView))//panel�ؼ���ʶ
		{
			pPanel = (tPANEL *)pView;
			pPriv = &(pPanel->PanelPriv);
			pSub = pPriv->pSubView;
			while(pSub)
			{
				if(pSub->view_attr.id == id)
				{
					return pSub;
				}
				pSub = pSub->pNext;
			}
		}
	}

	return pView;
}

/*----------------------------------------------
*����: Grap_GetPanelViewByID
*����: ��panel�ϵõ�ָ��id�Ŀؼ�ָ��
*����: panelid: panel�ؼ���id��   widgetid: Ҫ���ҵĿؼ���id
*����: 
*----------------------------------------------*/
tGRAPVIEW *Grap_GetPanelViewByID(void *view, U16 panelid, U16 widgetid)
{
	tPANEL		*pPanel;
	tGRAPVIEW	*pSub;
	tPANELPRIV	*pPriv;
	
	//���ҵ�panel
	pPanel = (tPANEL *)Grap_GetViewByID(view, panelid);
	if(!pPanel)
	{
		return NULL;
	}
	
	//�ڸ�panel����ָ���ؼ�
	pPriv = &(pPanel->PanelPriv);
	pSub = pPriv->pSubView;
	
	while(pSub)
	{
		if(pSub->view_attr.id == widgetid)
		{
			break;
		}
		pSub = pSub->pNext;
	}

	return pSub;
}

/*----------------------------------------------
*����: Grap_GetCurFocus
*����: ��ȡ��ǰ���役��ؼ�,�����panel���������panel�еĿؼ�
*����: pView �ؼ�ָ�� pPanelid:�������ؼ���panel��,�򷵻�panel��id   ����Ҫ�ò���ʱ��NULL
*����: pFocus: ������ؼ�ָ��,���Ϊ�ҵ��������򷵻�NULL
*----------------------------------------------*/
tGRAPVIEW *Grap_GetCurFocus(void *pView, U16 *pPanelid)
{
	tGRAPVIEW	*pFocus;
	tPANEL		*pPanel;
	tGRAPVIEW	*pSub;
	tPANELPRIV	*pPriv;
	
	if(pPanelid != NULL)
	{
		*pPanelid = 0;
	}
	pFocus = ((tGRAPVIEW *)pView)->pParent;
	
	while(pFocus)
	{
		if(pFocus->view_attr.enable && Panel_IfPanel(pFocus))//panel�ؼ���ʶ
		{
			pPanel = (tPANEL *)pFocus;
			pPriv = &(pPanel->PanelPriv);
			pSub = pPriv->pSubView;
			while(pSub)
			{
				if(pSub->view_attr.curFocus == 1)
				{
					if(pPanelid != NULL)
					{
						*pPanelid = pFocus->view_attr.id;
					}
					return pSub;
				}
				pSub = pSub->pNext;
			}
		}
		else
		{
			if(pFocus->view_attr.curFocus == 1)
			{
				break;
			}
		}
		
		pFocus = pFocus->pNext;
	}
	
	return pFocus;
}

/*----------------------------------------------
*����: Grap_ChangeInputMode
*����: �л����뷨ģʽ
*����: pEdit:�ؼ�ָ��
*����: none
*----------------------------------------------*/
void Grap_ChangeInputMode(tGRAPEDIT *pEdit)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT2		*pEdit2;
	U8	inputMode;
	U16	id;
	
	pDesk = (tGRAPDESKBOX *)(pEdit->pParent);
	inputMode = pDesk->inputMode;
	id = pEdit->view_attr.id;
hyUsbPrintf("inputMode  =  %d  input = %d reserve = 0x%x \r\n",inputMode, pEdit->edit_privattr.input,pEdit->view_attr.reserve);	
	if(inputMode > 0 && inputMode <= 5 && id >= pDesk->editMinId && id <= pDesk->editMaxId)
	{
		if((pEdit->view_attr.reserve&0xFF) != 0xFF)//����edit2
		{
			/*if(inputMode == pEdit->edit_privattr.input)
			{
				return ;
			}*/
			if(inputMode == 4)//��ǰ����д,��ر�
			{
				edit_change_input_mode(pDesk,0,0);
			}
			pDesk->inputMode = pEdit->edit_privattr.input;
		}
		else
		{
			pEdit2 = (tGRAPEDIT2 *)pEdit;
			/*if(inputMode == pEdit2->edit_privattr.input)
			{
				return ;
			}*/
			if(inputMode == 4)//��ǰ����д,��ر�
			{
				edit_change_input_mode(pDesk,0,0);
			}
			pDesk->inputMode = pEdit2->edit_privattr.input;
		}
		if(pDesk->inputMode == 4)//�л�������д, ���
		{
			edit_change_input_mode(pDesk,0,1);
		}
		
		t9_closeshow(pDesk);
		ReDraw(pDesk, 0, 0);
	}
}

/*----------------------------------------------
*����: Grap_ChangeFocusEnable
*����: �л�������
*����: pView:�ؼ�ָ��
*����: none
*----------------------------------------------*/
void Grap_ChangeFocusEnable(tGRAPVIEW *pView)
{
    tGRAPVIEW	*pTempView;
    tPANEL		*pPanel;
	tGRAPVIEW	*pSub;
	tPANELPRIV	*pPriv;
	U16 id;
hyUsbPrintf("FocusEnable = %d   curFocus = %d \r\n",pView->view_attr.FocusEnable, pView->view_attr.curFocus);	
    if(pView->view_attr.FocusEnable)
    {
    	if(!pView->view_attr.curFocus)
    	{
    		pTempView = Grap_GetCurFocus(pView->pParent, NULL);
    		if(pTempView)
    		{
    			pTempView->view_attr.curFocus = 0;
				/*if(pTempView->view_attr.enable)
    			{
    				pTempView->draw(pTempView, 0, 0);
    			}*/
    		}
    		pView->view_attr.curFocus = 1;
    		//pView->draw(pView, 0, 0);
    		//����Ǳ༭��,�����ÿ���༭��ָ�������뷨�����л����뷨
    		id = pView->view_attr.id;
    	hyUsbPrintf("changed focus id = %d \r\n", id);
    		if(id > EDIT_ID_BASE && id < BUTTON_ID_BASE)
    		{
    			Grap_ChangeInputMode((tGRAPEDIT *)pView);
    		}
    		t9_closeshow(pView->pParent);
    		ReDraw(pView->pParent, 0, 0);
    	}
    }    
}

/*-----------------------------------------
*����: Grap_ChangeFocus
*����: �ؼ��л�����
*����: type:�����л�   type�������л�   ��ǰ������
*����: 
*------------------------------------------*/
S32 Grap_ChangeFocus(tGRAPVIEW *pView, int type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPZORDER  	*pSubZorder ;
	tGRAPVIEW    	*pNextView, *pFocusView;
	tPANEL			*pPanel = NULL;
	tPANELPRIV		*pPriv;
	U16				panelId = 0;
	
	if(pView == NULL)
	{
		return RETURN_REDRAW;
	}
	pDesk = ((tGRAPDESKBOX *)pView)->pParent;
	pFocusView = Grap_GetCurFocus(pDesk, &panelId);
	if(pFocusView == NULL)
	{
		return RETURN_REDRAW;
	}
hyUsbPrintf("panel id = %d \r\n", panelId);	
	if(panelId != 0)//��ǰ������panel��, ���л�����ʱֻ�ڸ�panel�����䶯
	{
		pPanel = (tPANEL *)Grap_GetViewByID(pDesk, panelId);
		if(pPanel == NULL)
		{
			return RETURN_REDRAW;
		}
		
		pPriv = &(pPanel->PanelPriv);
	}
	
	if(1 == type)//�����л�
	{
		pNextView = pFocusView->pNext;
		
		while(pNextView)
		{
			if(pNextView->view_attr.enable && pNextView->view_attr.FocusEnable && !Panel_IfPanel(pNextView))//���Խ��ս���
			{
				break;
			}
			pNextView = pNextView->pNext;
		}
		
		if(!pNextView)//ֱ������β��û���ҵ�
		{
		hyUsbPrintf("change focus down panelId = %d  0x%x\r\n",panelId,(U32)pFocusView);
			//���´�ͷ��ʼ��
			if(panelId == 0)
			{
				pNextView = pDesk->pNext;
			}
			else
			{
				pNextView = pPriv->pSubView;
			}
			while(pNextView && pNextView != pFocusView)
			{
			//hyUsbPrintf("pNextView = %x pFocusView = %x\r\n", pNextView, pFocusView);
				if(pNextView->view_attr.enable && pNextView->view_attr.FocusEnable && !Panel_IfPanel(pNextView))//���Խ��ս���
				{
					break;
				}
				pNextView = pNextView->pNext;
			}
			if(pNextView == pFocusView)
			{
				pNextView = NULL;
			}
		}
	}
	else if(0 == type)//�����л�
	{
		pNextView = pFocusView->pPrev;
		
		if(panelId == 0)
		{
			while(pNextView != (tGRAPVIEW *)pDesk)
			{
				if(pNextView->view_attr.enable && pNextView->view_attr.FocusEnable && !Panel_IfPanel(pNextView))//���Խ��ս���
				{
					break;
				}
				pNextView = pNextView->pPrev;
			}
			
			//�ҵ�ͷҲû���ҵ�
			if(pNextView == (tGRAPVIEW *)pDesk)
			{
				//������β��ʼ��
				pNextView = pDesk->pTail;
				while(pNextView != pFocusView)
				{
					if(pNextView->view_attr.enable && pNextView->view_attr.FocusEnable && !Panel_IfPanel(pNextView))//���Խ��ս���
					{
						break;
					}
					pNextView = pNextView->pPrev;
				}
				
				if(pNextView == pFocusView)
				{
					pNextView = NULL;
				}
			}
		}
		else
		{
			while(pNextView != (tGRAPVIEW *)pPanel)
			{
				if(pNextView->view_attr.enable && pNextView->view_attr.FocusEnable && !Panel_IfPanel(pNextView))//���Խ��ս���
				{
					break;
				}
				pNextView = pNextView->pPrev;
			}
			
			//�ҵ�ͷҲû���ҵ�
			if(pNextView == (tGRAPVIEW *)pPanel)
			{
				//������β��ʼ��
				pNextView = pPanel->pTail;
				while(pNextView != pFocusView)
				{
					if(pNextView->view_attr.enable && pNextView->view_attr.FocusEnable && !Panel_IfPanel(pNextView))//���Խ��ս���
					{
						break;
					}
					pNextView = pNextView->pPrev;
				}
				if(pNextView == pFocusView)
				{
					pNextView = NULL;
				}
			}
		}
	}
	
	//�ҵ�
	if(pNextView)
	{
		pFocusView->view_attr.curFocus = 0;
		pFocusView->draw(pFocusView, 0, 0);
		pNextView->view_attr.curFocus = 1;
		pNextView->draw(pNextView, 0, 0);
		
		pFocusView = pNextView;
	hyUsbPrintf("========chaned focus id = %d \r\n", pFocusView->view_attr.id);	
		pSubZorder = pDesk->pZorderList ;
		while(pSubZorder)
		{
			pNextView = pSubZorder->pMember ;
			if (pNextView->view_attr.curFocus && pNextView->view_attr.FocusEnable && !Panel_IfPanel(pNextView))
			{
				Grap_UpdateZorder(pDesk, pSubZorder);
				break ;
			}
			pSubZorder = pSubZorder->pNext;
		}
		
		if(pFocusView->view_attr.id > EDIT_ID_BASE && pFocusView->view_attr.id < BUTTON_ID_BASE)
		{
			Grap_ChangeInputMode((tGRAPEDIT *)pFocusView);
		}
		else
		{
			if(pDesk->inputMode == 4)
			{
				edit_change_input_mode(pDesk,0,0);
			}
		}
	}
			
	return RETURN_REDRAW;
}

/*----------------------------------------------
*����: Grap_SendMessage
*����: ��ָ���ؼ�������Ϣ(������ʽ)   idΪ0��ʾ�����巢��
*����: pView:�ؼ�ָ��
       pEvent:��Ϣ
*����: none
*----------------------------------------------*/
int Grap_SendMessage(tGRAPVIEW *pView, GRAP_EVENT *pEvent)
{
	tGRAPDESKBOX *pDesk;
	tGRAPVIEW	*view;
	tPANEL		*pPanel;
	tGRAPVIEW	*pSub;
	tPANELPRIV	*pPriv;
	
	if (pView == NULL) 	return -1;
	
	pDesk = (tGRAPDESKBOX *)(pView->pParent);
	if (pEvent->id == 0)
	{
		return pDesk->handle(pDesk, pEvent);
	}
	else if(pEvent->id == pView->view_attr.id)
	{
	//hyUsbPrintf("view id = %d  handle \r\n", pView->view_attr.id);
		return pView->handle(pView, pEvent);
	}
	else
	{
		view = (tGRAPVIEW *)pDesk;
		while (view)
		{
			if(Panel_IfPanel(view))
			{
				pPanel = (tPANEL *)view;
				pPriv = &(pPanel->PanelPriv);
				pSub = pPriv->pSubView;
				while(pSub)
				{
					if(pSub->view_attr.id == pEvent->id)
					{
						return pSub->handle(pSub, pEvent);
					}
					pSub = pSub->pNext;
				}
			}
			else
			{
				if (view->view_attr.id == pEvent->id)
				{
					return view->handle(view, pEvent);
				}
			}
			view = view->pNext;
		}
		return 0;
	}
}

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
void Grap_WriteBMPColor(U16 x1, U16 y1, U16 width, U16 height, U8 *BmpColor, U16 colorP)
{
	drvLcd_SetColor(colorP,0x0000);
	drvLcd_WriteBMPColor(BmpColor,height,width,y1,x1);
	
	return ;
}

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
void Grap_BrushScreen(U16 x1, U16 y1, U16 x2, U16 y2, U16 colorP)
{
	drvLcd_SetColor(0x0000,colorP);
	drvLcd_SetDisplayAddress(Get_Display_DisplayMemory_Offset());
	drvLcd_ClearBox(y1, x1, y2-y1, x2-x1);
	return;
}

/*----------------------------------------------
*����: Grap_ClearScreen
*����: �õ�ǰ����ͼˢ��ָ������
*����: x1:��ʼ��
       y1:��ʼ��
       x2:������
       y2:������
*����: none
*----------------------------------------------*/
void Grap_ClearScreen(U16 x1, U16 y1, U16 x2, U16 y2)
{
	drvLcd_Background2DisplayMemory(y2-y1, x2-x1, y1, x1);
	
	return ;
}

/*----------------------------------------------
*����: Grap_ShowPicture
*����: ��ָ��λ����ʾָ��id�ŵ�ͼƬ
*����: picId:ͼƬö��,Ҫȷ������
	   posX:��ʼ��
       posY:��ʼ��
*����: none
*----------------------------------------------*/
void Grap_ShowPicture(U16 picId, U16 posX, U16 posY)
{
    if (picId > 0)
    {
    	res_load_bmp_to_memory(picId,posX,posY);
	}
	
	return;
}

/*----------------------------------------------
*����: Grap_Show_Part_BackPicture
*����: ��ʾָ��ͼƬ��һ����
*����: pView1:��ͼ���ڵĿؼ�(��һ���Ǵ�ı���ͼ)
	   pView2:��ǰҪˢ�µĿؼ�,����Ҫ��pView1��
*����: none
*----------------------------------------------*/
void Grap_Show_Part_BackPicture(void *pView1,void *pView2)
{
	tGRAPVIEW *ptView,*pview_attr;
	U32 x1,y1,x2,y2,h,w;
	U16 posX,posY,posH,posW;
	U32 picId;
	
    ptView = (tGRAPVIEW *)pView1;
    pview_attr = (tGRAPVIEW *)pView2;
    picId = ptView->view_attr.clNormal;
    
    h = ptView->view_attr.viewHeight;
    w = ptView->view_attr.viewWidth;
    posX = pview_attr->view_attr.viewX1;
    posY = pview_attr->view_attr.viewY1;
    posH = pview_attr->view_attr.viewHeight;
    posW = pview_attr->view_attr.viewWidth;
    
    if (picId > 0)
    {
    	x1 = ptView->view_attr.viewX1; //ͼƬ����ʼλ��
    	y1 = ptView->view_attr.viewY1;
    	x2 = posX - x1;    //���ͼƬ��λ��
    	y2 = posY - y1;
    	if(x2 < 0 || y2 < 0 || x2+posW > w || y2+posH > h)
    	{
    		return ; // ���򳬳�ͼƬ����
    	}

    	res_load_part_to_memory(picId,x2,y2,posX,posY,posH, posW);    	
	}
}
