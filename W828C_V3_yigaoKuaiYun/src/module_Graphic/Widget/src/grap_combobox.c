#include "grap_combobox.h"
#include "grap_event.h"
#include "grap_deskbox.h"


const U16 ComboboxScrollBarColor[24]=
{
	0xA75F,0xA75F,0x86FF,0x86FF,0x769F,0x769F,0x661F,0x661F,0x559F,0x453F,0x34BF,0x34BF,0x1C5E,
	0x39C,0x39C,0x35A,0x35A,0x318,0x318,0xB7A,0xB7A,0x3BB,0x3BB,0x45F 
};


S32 Grap_ComboboxRecvSysMsg(void *pView)
{

	return SUCCESS ;
}
/*
 * ������ string Ϊ�յ� ʱ��ſ��øĺ�����ȡ
 */
S32 Grap_ComboboxGetStr(void *pView,U16 i, U8 *pStr)
{

	return SUCCESS ;
}

void Grap_ComboxUpdateScr(void *pView)
{
	tGRAPCOMBOBOX	*pCombox;
	tGrapViewAttr	*pView_attr;
	tComboboxPriv	*pPriv;
	tComboboxScroll	*pScr;
	
	pCombox		= (tGRAPCOMBOBOX *)pView;
	pView_attr	= &(pCombox->view_attr);
	pPriv		= &(pCombox->comboPriv);
	pScr		= &(pCombox->comboScr);
	
	pScr->barH = (pScr->scrLen*pPriv->dis_num)/pPriv->total_num;
	if(pScr->barH < 10)	pScr->barH = 10;//��СΪ10�����ظ�
	
	pScr->ratio = (float)(pScr->scrLen-pScr->barH)/(pPriv->total_num - pPriv->dis_num);
	
	pScr->barStart = pScr->scrStartY + (U16)(pPriv->top_index*pScr->ratio);
	pScr->barEnd = pScr->barStart + pScr->barH;
	
	if(pPriv->top_index == pPriv->total_num - pPriv->dis_num)
	{
		pScr->barEnd = pScr->scrEndY;
		pScr->barStart = pScr->barEnd-pScr->barH;
	}
	
	return ;
}


/*
 *flag 0:�ƶ�top��  1:�ƶ�focus��
 */
S32 Grap_ComboboxLineUp(void *pView,U32 flag)
{
	tGRAPCOMBOBOX		*pCombobox;
	tComboboxPriv		*pPriv;
	tComboboxScroll		*pScr;
	tGrapViewAttr		*pview_attr;
	
    pCombobox = (tGRAPCOMBOBOX *)pView;
	pPriv = &(pCombobox->comboPriv);
	pScr = &(pCombobox->comboScr);
	pview_attr = &(pCombobox->view_attr);
	
	if(pPriv->total_num == 0)	return SUCCESS;
	
	if(flag == 1)
	{
		if(pPriv->opencur_index > 0)
		{
			pPriv->opencur_index--;
			if(pPriv->opencur_index < pPriv->top_index)
			{
				pPriv->top_index = pPriv->opencur_index;
				Grap_ComboxUpdateScr(pCombobox);
				pCombobox->draw(pCombobox, 0, 0);
			}
			else
			{
				pCombobox->comboxdrawone(pCombobox, (U16)(pPriv->opencur_index+1));
				pCombobox->comboxdrawone(pCombobox, pPriv->opencur_index);
			}
		}
		else//��һ��
		{
			pPriv->opencur_index = pPriv->total_num - 1;
			if(pPriv->total_num < pPriv->dis_num)
				pPriv->top_index = 0;
			else
				pPriv->top_index = pPriv->total_num - pPriv->dis_num;
			
			Grap_ComboxUpdateScr(pCombobox);
			pCombobox->draw(pCombobox, 0, 0);
		}
	}
	else if(flag == 0)
	{
		if(pPriv->top_index > 0)
		{
			pPriv->top_index --;
			Grap_ComboxUpdateScr(pCombobox);
			pCombobox->draw(pCombobox, 0, 0);
		}
	}
	
	
	return SUCCESS;
}
/*
 *flag 0:�ƶ�top��  1:�ƶ�focus��
 */
S32 Grap_ComboboxLineDown(void *pView,U32 flag)
{
	tGRAPCOMBOBOX		*pCombobox;
	tComboboxPriv		*pPriv;
	tComboboxScroll		*pScr;
	tGrapViewAttr		*pview_attr;
	
    pCombobox = (tGRAPCOMBOBOX *)pView;
	pPriv = &(pCombobox->comboPriv);
	pScr = &(pCombobox->comboScr);
	pview_attr = &(pCombobox->view_attr);
	
	if(pPriv->total_num == 0)	return SUCCESS;
	
	if(flag == 1)
	{
		if (pPriv->opencur_index < pPriv->total_num - 1)
		{
			pPriv->opencur_index++;
			if(pPriv->opencur_index >= pPriv->top_index + pPriv->dis_num)
			{
				pPriv->top_index++;
				Grap_ComboxUpdateScr(pCombobox);
				pCombobox->draw(pCombobox, 0, 0);
			}
			else
			{
				pCombobox->comboxdrawone(pCombobox, (U16)(pPriv->opencur_index-1));
				pCombobox->comboxdrawone(pCombobox, pPriv->opencur_index);
			}
		}
		else
		{
			pPriv->opencur_index = 0;
			pPriv->top_index = 0;
			Grap_ComboxUpdateScr(pCombobox);
			pCombobox->draw(pCombobox, 0, 0);
		}
	}
	else if(flag == 0)
	{
		if(pPriv->top_index+pPriv->dis_num < pPriv->total_num)
		{
			pPriv->top_index ++;
			Grap_ComboxUpdateScr(pCombobox);
			pCombobox->draw(pCombobox, 0, 0);
		}
	}
	
	
	return SUCCESS;
}

/*
 *distance �ƶ�����  ��direction == 0ʱֱ���ƶ����ô�
 *direction 0:���ƶ�,1:����, -1:����
 *���� -1:δ��ק 0:��ק
 */
S32 Grap_ComboboxMoveScrBar(tGRAPCOMBOBOX *pCombobox,U16 distance, S16 direction)
{
	tComboboxPriv		*pPriv;
	tComboboxScroll		*pScr;
	tGrapViewAttr		*pview_attr;
	U16 tempItem, tempDistance, Num;
	
	pPriv = &(pCombobox->comboPriv);
	pScr = &(pCombobox->comboScr);
	pview_attr = &(pCombobox->view_attr);
	
	if(direction == 0)
	{
		if(distance <= pScr->scrStartY || distance >= pScr->scrEndY)	return -1;
		if(distance >= pScr->barStart && distance <= pScr->barEnd)	return -1;

		if(distance > pScr->barEnd)//�����·�
		{
			tempDistance = distance - pScr->barEnd;
			Num = (U16)(tempDistance/pScr->ratio);
			if( (U16)(Num*pScr->ratio) < tempDistance)	Num++;
			
			pPriv->top_index += Num;
			
			pScr->barEnd += (U16)(Num*pScr->ratio);
			if(pScr->barEnd > pScr->scrEndY)
			{
				pScr->barEnd = pScr->scrEndY;
				pPriv->top_index = pPriv->total_num - pPriv->dis_num;
			}
			pScr->barStart = pScr->barEnd - pScr->barH;
		}
		else //�����Ϸ�
		{
			pScr->barStart = distance;
			pScr->barEnd = pScr->barStart+pScr->barH;
			
			distance = pScr->barStart - pScr->scrStartY;//��Թ������ľ���
		
			tempItem = (U16)(distance/pScr->ratio);
			
			if(tempItem == pPriv->top_index)	return -1;
			
			pPriv->top_index = tempItem;
		}
		
		if(pPriv->top_index+pPriv->dis_num >= pPriv->total_num)
		{
			pPriv->top_index = pPriv->total_num - pPriv->dis_num;
			
			pScr->barEnd = pScr->scrEndY;
			pScr->barStart = pScr->barEnd-pScr->barH;
		}
		else
		{
			pScr->barStart = pScr->scrStartY + (U16)(pPriv->top_index*pScr->ratio);
			pScr->barEnd = pScr->barStart+pScr->barH;
		}

		pCombobox->draw(pCombobox,0,0);
	}
	else if(direction == 1)//��
	{
		if(pScr->barEnd >= pScr->scrEndY)	return -1;
		
		Num = (U16)(distance/pScr->ratio);
		if( (U16)(Num*pScr->ratio) < distance)	Num++;
		
		pPriv->top_index += Num;
		
		pScr->barEnd += (U16)(Num*pScr->ratio);
		if(pScr->barEnd > pScr->scrEndY)
		{
			pScr->barEnd = pScr->scrEndY;
			pPriv->top_index = pPriv->total_num - pPriv->dis_num;
		}
		pScr->barStart = pScr->barEnd - pScr->barH;
			
		pCombobox->draw(pCombobox,0,0);
	}
	else if(direction == -1)//��
	{
		if(pScr->barStart <= pScr->scrStartY)	return -1;
		
		Num = (U16)(distance/pScr->ratio);
		if( (U16)(Num*pScr->ratio) < distance)	Num++;
		
		if(pPriv->top_index > Num)
		{
			pPriv->top_index -= Num;
			pScr->barStart -= (U16)(Num*pScr->ratio);
		}
		else
		{
			pPriv->top_index = 0;
			pScr->barStart = pScr->scrStartY;
		}

		pScr->barEnd = pScr->barStart + pScr->barH;
		pCombobox->draw(pCombobox,0,0);
	}
	
	return 0;
}

S32 Grap_ComboboxCheckRange(U16 x, U16 y, tGRAPCOMBOBOX *pCombox)
{
	if(x >= pCombox->openX1 && x <= pCombox->openX2 &&
	   y >= pCombox->openY1 && y <= pCombox->openY2)
	{
		return 1;
	}
	return 0;
}


//1:�ϼ�ͷ 2:�м� 3:�¼�ͷ  0:���ڸ�����
S32 Grap_ComboboxIfScr(U16 x, U16 y, tComboboxScroll *pScr, U16 needscr)
{
	
	if(needscr == 0)//����Ҫ������
	{
		return 0;
	}
	if(x >= pScr->viewX1 && x <= pScr->viewX2 &&
	   y >= pScr->viewY1 && y <= pScr->viewY2)
	{
		if(y >= pScr->arrow1_Y1 && y <= pScr->arrow1_Y2)//�ϼ�ͷ
		{
			return 1;
		}
		else if(y >= pScr->arrow2_Y1 && y <= pScr->arrow2_Y2)//�¼�ͷ
		{
			return 3;
		}
		else//�м�
		{
			return 2;
		}
	}
	
	return 0;
}


S32 Grap_ComboboxRecvPullMsg(void *pView,GRAP_EVENT *pEvent)
{
    U32 extParam, message, wParam, lParam ;
	tGRAPCOMBOBOX		*pCombobox;
	tComboboxPriv		*pPriv;
	tComboboxScroll		*pScr;
	tGrapViewAttr		*pview_attr;
	S32 ret = FAIL;
	int retscr;
	int x, y, i;
	U32 xy, wh ;
	U16 tempH;
	U16 lastX,lastY;
	
    pCombobox = (tGRAPCOMBOBOX *)pView;
	pPriv = &(pCombobox->comboPriv);
	pScr = &(pCombobox->comboScr);
	pview_attr = &(pCombobox->view_attr);
  
    message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;

	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;
	
	lastY = HIWORD(pEvent->wParam);
	lastX = LOWORD(pEvent->wParam);
	
	if (VIEW_PULL_START == extParam)
	{//����ʼ��
		if(pCombobox->list_opened)//��
		{
			if(Grap_ComboboxCheckRange(x,y,pCombobox))
			{//����Ч��Χ��
				//hyUsbPrintf("combobox start \r\n");
				ret = VIEW_PULL_START;
			}
			else
			{
				pCombobox->list_opened = 0;//�ر�
		    	ReDraw(pCombobox, 0, 0);
	        	return FAIL;
			}
		}
		else 
		{
			if(GRAP_INWIDGET_RANGE(x,y,pCombobox))
		    {//����Ч��Χ��
		    	//hyUsbPrintf("combobox start 22\r\n");
		         ret = VIEW_PULL_START; 
		    }
		    else
		    {
		    	return FAIL;
		    }
		}
	}
	
	if (VIEW_PULL_START == extParam)
	{//down
		pPriv->firstcharpos = 0;
	//hyUsbPrintf("----------curFocus = %d \r\n",pCombobox->view_attr.curFocus);
		Grap_ChangeFocusEnable(pCombobox);
	//hyUsbPrintf("++++++++++curFocus = %d \r\n",pCombobox->view_attr.curFocus);
	//hyUsbPrintf("combobox VIEW_PULL_START  list_opened = %d \r\n",pCombobox->list_opened);
		if(pCombobox->list_opened)//��
		{
			retscr = Grap_ComboboxIfScr(x,y,pScr,pPriv->needscr);
			pCombobox->pullflag = retscr;
			if(retscr == 1)//�ϼ�ͷ
			{
				Grap_ComboboxLineUp(pCombobox,0);
			}
			else if(retscr == 3)//�¼�ͷ
			{
				Grap_ComboboxLineDown(pCombobox,0);
			}
			else if(retscr == 2)//�м�
			{
				Grap_ComboboxMoveScrBar(pCombobox,y,0);
			}
			else//���ڹ��������޹�����
			{
				if(pPriv->direction == 0)//��
				{
					for(i = 0; i < pPriv->dis_num; i++)
					{
						if(pPriv->top_index + i >= pPriv->total_num)
							break;
						if(y >= pCombobox->openY1+(i+1)*pview_attr->viewHeight &&
						   y <= pCombobox->openY1+(i+2)*pview_attr->viewHeight)
						{
							pPriv->opencur_index = i + pPriv->top_index;
							break ;
						}
					}
				}
				else//��
				{
					for(i = 0; i < pPriv->dis_num; i++)
					{
						if(pPriv->top_index + i >= pPriv->total_num)
							break;
						if(y >= pCombobox->openY1+i*pview_attr->viewHeight &&
						   y <= pCombobox->openY1+(i+1)*pview_attr->viewHeight)
						{
							pPriv->opencur_index = i + pPriv->top_index;
							break ;
						}
					}
				}
				pCombobox->draw(pView, 0, 0);
			}
		}
		else
		{
			pPriv->opencur_index = pPriv->cur_index;
		}
	}
	else if (VIEW_PULL_ING == extParam)
	{//��ק��
	//hyUsbPrintf("combobox VIEW_PULL_ING  list_opened = %d \r\n",pCombobox->list_opened);
		if(pCombobox->list_opened)//��
		{
			if(Grap_ComboboxCheckRange(x,y,pCombobox))
			{
				retscr = Grap_ComboboxIfScr(x,y,pScr,pPriv->needscr);
				if(retscr != pCombobox->pullflag)
				{
					return VIEW_PULL_ING;
				}
				if(pCombobox->pullflag == 1)//�ϼ�ͷ
				{
					Grap_ComboboxLineUp(pCombobox,0);
				}
				else if(pCombobox->pullflag == 3)//�¼�ͷ
				{
					Grap_ComboboxLineDown(pCombobox,0);
				}
				else if(pCombobox->pullflag == 2)//�м�
				{
					S16 temp,re;
						
					temp = y - lastY;

					if(temp > 0)
						re = Grap_ComboboxMoveScrBar(pCombobox,ABS(temp),1);//����
					else
						re = Grap_ComboboxMoveScrBar(pCombobox,ABS(temp),-1);//����
					
					if(re == 0)
					{
						return VIEW_PULL_ING;
					}
					else	return FAIL;
				}
				else//���ڹ��������޹�����
				{
					if(pPriv->direction == 0)//��
					{
						for(i = 0; i < pPriv->dis_num; i++)
						{
							if(pPriv->top_index + i >= pPriv->total_num)
								break;
							if(y >= pCombobox->openY1+(i+1)*pview_attr->viewHeight &&
							   y <= pCombobox->openY1+(i+2)*pview_attr->viewHeight)
							{
								pPriv->opencur_index = i + pPriv->top_index;
								break ;
							}
						}
					}
					else//��
					{
						for(i = 0; i < pPriv->dis_num; i++)
						{
							if(pPriv->top_index + i >= pPriv->total_num)
								break;
							if(y >= pCombobox->openY1+i*pview_attr->viewHeight &&
							   y <= pCombobox->openY1+(i+1)*pview_attr->viewHeight)
							{
								pPriv->opencur_index = i + pPriv->top_index;
								break ;
							}
						}
					}

					pCombobox->draw(pView, 0, 0);
				}
				ret = VIEW_PULL_ING;
			}
		}
		else
		{
			if(GRAP_INWIDGET_RANGE(x,y,pCombobox))   ret = VIEW_PULL_ING;
		}
	}
	else if (VIEW_PULL_END == extParam)
	{//up 
	//hyUsbPrintf("combobox VIEW_PULL_END  list_opened = %d \r\n",pCombobox->list_opened);    			
		if(pCombobox->list_opened)//��
		{
			if(!Grap_ComboboxCheckRange(x,y,pCombobox))
			{
				pCombobox->list_opened = 0;//�ر�
				ReDraw(pCombobox, 0, 0);

				return VIEW_PULL_END;
			}
		}
		else
		{
			if(!GRAP_INWIDGET_RANGE(x,y,pCombobox))
			{
				return VIEW_PULL_END;
			}
		}  
       
		if(pCombobox->list_opened)//��Ҫ���ǵ�����Ƿ�Ϊ������,������ʱδ��
		{
			retscr = Grap_ComboboxIfScr(x,y,pScr,pPriv->needscr);
			if(retscr == 1)//�ϼ�ͷ
			{
				//Grap_ComboboxLineUp(pCombobox,0);
			}
			else if(retscr == 3)//�¼�ͷ
			{
				//Grap_ComboboxLineDown(pCombobox,0);
			}
			else if(retscr == 2)//�м�
			{
				//Grap_ComboboxMoveScrBar(pCombobox,y,0);
			}
			else//���ڹ��������޹�����
			{
				if(y >= pview_attr->touchY1 && y <= pview_attr->touchY2)
				{//�ڵ�ǰ��ʾ����
					
				}
				else
				{//��������
					if(pPriv->direction == 0)//��
					{
						for(i = 0; i < pPriv->dis_num; i++)
						{
							if(pPriv->top_index + i >= pPriv->total_num)
								break;
							if(y >= pCombobox->openY1+(i+1)*pview_attr->viewHeight &&
							   y <= pCombobox->openY1+(i+2)*pview_attr->viewHeight)
							{
								pPriv->cur_index = i + pPriv->top_index;
								break ;
							}
						}
					}
					else//��
					{
						for(i = 0; i < pPriv->dis_num; i++)
						{
							if(pPriv->top_index + i >= pPriv->total_num)
								break;
							if(y >= pCombobox->openY1+i*pview_attr->viewHeight &&
							   y <= pCombobox->openY1+(i+1)*pview_attr->viewHeight)
							{
								pPriv->cur_index = i + pPriv->top_index;
								break ;
							}
						}
					}
				}
				
				pCombobox->comboxenter(pCombobox,pPriv->cur_index);
				pCombobox->list_opened = 0;
				//����Ҫredraw����ˢ�������������
				ReDraw(pCombobox, 0, 0); 
			}
		}
		else
		{
			pCombobox->list_opened = 1;
			//hyUsbPrintf("combox open = %d \r\n",pCombobox->list_opened);
			pCombobox->draw(pView, 0, 0);
		}

		return PARSED ;
	}
	else if (VIEW_PULL_TIMEOUT == extParam)
	{//��ʱ
	    ret = VIEW_PULL_TIMEOUT;
	}
	
	return ret; 
}

S32 Grap_ComboboxEnter(void *pView, U16 i)
{
	
	return SUCCESS;
}

S32 Grap_ComboboxTimer(void *pView, U16 i)
{
	tGRAPCOMBOBOX	*pCombox;
	tGrapViewAttr	*pView_attr;
	tComboboxPriv	*pPriv;
	tComboboxScroll	*pScr;
	tStrDispInfo	disp_info;
	Character_Type_e kStr_Type;
	U8	tmpStr[50];
	U16 x1,x2,y1,y2;
	U16 len=0,lenstr ,widstr,limit,tempLimit;
	
	pCombox		= (tGRAPCOMBOBOX *)pView;
	pView_attr	= &(pCombox->view_attr);
	pPriv		= &(pCombox->comboPriv);
	pScr		= &(pCombox->comboScr);
	
	if((pCombox->list_opened == 1) && (i < pPriv->top_index || i > pPriv->top_index+pPriv->dis_num))
	{
		return SUCCESS;
	}
	
	memset(tmpStr,0,50);
	
	pCombox->comboxgetstr(pCombox,i,tmpStr);
	
	if(tmpStr[0] == 0) return FAIL;
	
	kStr_Type = pPriv->font_type;
	lenstr = Get_StringLength(kStr_Type,tmpStr);
	widstr = Get_StringWidth(kStr_Type,tmpStr);//�ַ����(UNICODE��ǵİ�1��)
	
	if(pCombox->list_opened)
	{
		limit = pPriv->opencharlen;
	}
	else
	{
		limit = pPriv->closecharlen;
	}
	
	if(widstr <= limit)//����Ҫ�ƶ���ʾ
	{
		return SUCCESS;
	}
	
	if(pCombox->list_opened)
	{
		i -= pPriv->top_index;
		if(pPriv->direction == 0)//��
		{
			y1 = pCombox->openY1+(i+1)*pView_attr->viewHeight;
			y2 = y1+pPriv->charhigh;
		}
		else//��
		{
			y1 = pCombox->openY1+i*pView_attr->viewHeight;
			y2 = y1+pPriv->charhigh;
		}
		
		
		
		x1 = pView_attr->viewX1;
		x2 = x1+pPriv->openwidth;
		//y1 = pCombox->openY1+i*pView_attr->viewHeight;
		//y2 = y1+pPriv->charhigh;
		
		if(pPriv->longbar)
		{
			Grap_ShowPicture(pPriv->longbar, x1, y1);
		}
		else
		{
			Grap_BrushScreen(x1+pPriv->start_x, y1+pPriv->start_y, 
					x2+pPriv->start_x, y2+pPriv->start_y, pPriv->bar_color);
		}
	}
	else
	{
		x1 = pView_attr->viewX1;
		x2 = x1+pPriv->closewidth;
		y1 = pView_attr->viewY1;
		y2 = y1+pPriv->charhigh;
		
		if(pPriv->shortbar)
		{
			Grap_ShowPicture(pPriv->shortbar, x1, y1);
		}
		else
		{
			Grap_BrushScreen(x1+pPriv->start_x, y1+pPriv->start_y, 
					x2+pPriv->start_x, y2+pPriv->start_y, pPriv->bar_color);
		}
	}
	if(kStr_Type == CHARACTER_LOCALCODE)
	{
		if(tmpStr[pPriv->firstcharpos]>=0x80)	pPriv->firstcharpos+=2;
		else	pPriv->firstcharpos++;
	}
	else
	{
		pPriv->firstcharpos+=2;
	}
	if(pPriv->firstcharpos >= lenstr) pPriv->firstcharpos = 0;
	
	disp_info.string	= &tmpStr[pPriv->firstcharpos];
	disp_info.color		= pPriv->clrfocus;
	disp_info.font_size = pPriv->font_size;
	disp_info.font_type = pPriv->font_type;
	disp_info.flag 		= 0;
	disp_info.keyStr = NULL;
	tempLimit = gu16Lcd_ColumnLimit;
	gu16Lcd_ColumnLimit = x2;
	Grap_WriteString(x1+pPriv->start_x, y1+pPriv->start_y, 
					x2+pPriv->start_x, y2+pPriv->start_y, &disp_info);
	gu16Lcd_ColumnLimit = tempLimit;
	
	return SUCCESS;
}

S32 Grap_ComboboxDrawOne(void *pView, U16 i)
{
	tGRAPCOMBOBOX	*pCombox;
	tGrapViewAttr	*pView_attr;
	tComboboxPriv	*pPriv;
	tComboboxScroll	*pScr;
	tStrDispInfo	disp_info;
	U8	tmpStr[50];
	U16 x1,x2,y1,y2,tempLimit;
	
	pCombox		= (tGRAPCOMBOBOX *)pView;
	pView_attr	= &(pCombox->view_attr);
	pPriv		= &(pCombox->comboPriv);
	pScr		= &(pCombox->comboScr);
	
	if(i >= pPriv->total_num)
	{
		return SUCCESS;
	}

	if(i < pPriv->top_index || i >= pPriv->top_index+pPriv->dis_num)
	{
		return SUCCESS;
	}
	
	x1 = pView_attr->viewX1;
	x2 = x1+pPriv->openwidth;
	i -= pPriv->top_index;
	if(pPriv->direction == 0)//��
	{
		y1 = pCombox->openY1+(i+1)*pView_attr->viewHeight;
		y2 = y1+pPriv->charhigh;
	}
	else//��
	{
		y1 = pCombox->openY1+i*pView_attr->viewHeight;
		y2 = y1+pPriv->charhigh;
	}
	i += pPriv->top_index;
	if(i == pPriv->opencur_index)//�������跴��
	{
		if(pPriv->longbar)
		{
			Grap_ShowPicture(pPriv->longbar, x1, y1);
		}
		else
		{
			Grap_BrushScreen(x1+pPriv->start_x, y1+pPriv->start_y, 
					x2+pPriv->start_x, y2+pPriv->start_y, pPriv->bar_color);
		}
		disp_info.color  = pPriv->clrfocus;
	}
	else
	{
		disp_info.color  = 0x0;//pPriv->clrnormal;
	}
	//��ʾ�ַ�
	memset(tmpStr,0,50);
	pCombox->comboxgetstr(pCombox,i,tmpStr);	
	
	if(tmpStr[0] == 0) return FAIL;
	
	disp_info.string = tmpStr;
	disp_info.font_size = pPriv->font_size;
	disp_info.font_type = pPriv->font_type;
	disp_info.flag      = 0;
	disp_info.keyStr = NULL;
	tempLimit = gu16Lcd_ColumnLimit;
	gu16Lcd_ColumnLimit = x2;
	Grap_WriteString(x1+pPriv->start_x, y1+pPriv->start_y, 
					x2+pPriv->start_x, y2+pPriv->start_y, &disp_info);
	gu16Lcd_ColumnLimit = tempLimit;
	
	return SUCCESS;
}

const U8  gBmpUpData[]=//18*16
{
	0x00,0x00,0x00,0x00,0x80,0x00,0x01,0xC0,0x00,0x01,0xC0,0x00,0x03,0xE0,0x00,0x03,
	0xE0,0x00,0x07,0xF0,0x00,0x07,0xF0,0x00,0x0F,0xF8,0x00,0x0F,0xF8,0x00,0x1F,0xFC,
	0x00,0x1F,0xFC,0x00,0x3F,0xFE,0x00,0x3F,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

const U8  gBmpDownData[]=//18*16
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFE,0x00,0x3F,0xFE,0x00,0x1F,0xFC,0x00,0x1F,
	0xFC,0x00,0x0F,0xF8,0x00,0x0F,0xF8,0x00,0x07,0xF0,0x00,0x07,0xF0,0x00,0x03,0xE0,
	0x00,0x03,0xE0,0x00,0x01,0xC0,0x00,0x01,0xC0,0x00,0x00,0x80,0x00,0x00,0x00,0x00
};

S32 Grap_ComboboxDrawScr(void *pView)
{
	tGRAPCOMBOBOX	*pCombox;
	tGrapViewAttr	*pView_attr;
	tComboboxPriv	*pPriv;
	tComboboxScroll	*pScr;
	U16 		x1,x2,y1,y2;
	U16 row,column,width,height,i,j;
	tRES_PARA   tRes;
	U32 *pClr;
	
	pCombox		= (tGRAPCOMBOBOX *)pView;
	pView_attr	= &(pCombox->view_attr);
	pPriv		= &(pCombox->comboPriv);
	pScr		= &(pCombox->comboScr);
	
	if(pPriv->scrBarHeadId == 0 || pPriv->scrBarTailId == 0 || pPriv->scrBarId == 0)//û�й�����
	{
		x1 = pScr->viewX1+1;
		x2 = pScr->viewX2;
		
		y1 = pScr->viewY1;
		y2 = pScr->scrStartY;
		Grap_WriteBMPColor(x1, y1, 18,16, (U8*)gBmpUpData, 0x00);//��
		
		y1 = pScr->scrEndY;
		y2 = pScr->viewY2;
		Grap_WriteBMPColor(x1, y1, 18, 16, (U8*)gBmpDownData, 0x00);//��
		
		//��������
		row = pScr->barStart;
		column = pScr->viewX1+2;
		width = 16;
		height = pScr->barH;
		for(i=0; i < width; i++)
		{
			drvLcd_SetColor(COLOR_BLACK, COLOR_BLACK);
			Memory_SetRowColumn(row, column++, height, 1);
			
			for(j=0; j < height; j++)
			{
				if(j == 0 || j == height-1)	drvLcd_WritePoint(0);
				else drvLcd_WritePoint(gFrontColor);
			}
		}
	}
	else
	{
		if (res_load_bmp_info(pPriv->scrBarId, &tRes) < 0) return FAIL;
		
		x1 = pScr->viewX1;
		x2 = pScr->viewX2;
		
		y1 = pScr->viewY1;
		y2 = pScr->scrStartY;
		Grap_ShowPicture(pPriv->scrBarHeadId, x1, y1);//��һ����
		
		y1 = pScr->scrStartY;
		y2 = pScr->scrEndY;

		while(y1+tRes.high < y2)
		{
			Grap_ShowPicture(pPriv->scrBarId, x1, y1);//�м䲿��
			y1 = y1+tRes.high;
		}
		y1 = y2 - tRes.high;
		Grap_ShowPicture(pPriv->scrBarId, x1, y1);//�м䲿��
		
		y1 = pScr->scrEndY;
		y2 = pScr->viewY2;
		Grap_ShowPicture(pPriv->scrBarTailId, x1, y1);//�ڶ�����
		
		//��������
		row = pScr->barStart;
		column = pScr->viewX1+1;
		width = tRes.width-2;
		height = pScr->barH;
		for(i=0; i < width; i++)
		{
			drvLcd_SetColor(ComboboxScrollBarColor[i], COLOR_BLACK);
			Memory_SetRowColumn(row, column++, height, 1);
			
			for(j=0; j < height; j++)
			{
				if(j == 0 || j == height-1)	drvLcd_WritePoint(0);
				else drvLcd_WritePoint(gFrontColor);
			}
		}
	}
	
	return SUCCESS;
}

S32 Grap_ComboboxDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPCOMBOBOX	*pCombox;
	tGrapViewAttr	*pView_attr;
	tComboboxPriv	*pPriv;
	tComboboxScroll	*pScr;
	tStrDispInfo	disp_info;
	U8	tmpStr[50];
	U16 x1,x2,y1,y2;
	U16 i,tempLimit;
	
	pCombox		= (tGRAPCOMBOBOX *)pView;
	pView_attr	= &(pCombox->view_attr);
	pPriv		= &(pCombox->comboPriv);
	pScr		= &(pCombox->comboScr);
	
	x1 = pView_attr->viewX1+pPriv->start_x;
	x2 = x1+pPriv->closewidth;
	y1 = pView_attr->viewY1+pPriv->start_y;
	y2 = y1+pPriv->charhigh;
	
	if(pView_attr->curFocus)
	{//������ɫ
		if(pView_attr->clFocus)
		{
			Grap_ShowPicture(pView_attr->clFocus, pView_attr->viewX1, pView_attr->viewY1);
		}
		else if(pPriv->backpicid)
		{
			Grap_ClearScreen(pView_attr->viewX1, pView_attr->viewY1, (U16)(pView_attr->viewX1 + pView_attr->viewWidth),
					(U16)(pView_attr->viewY1 + pView_attr->viewHeight));
		}
		else
		{
			Grap_BrushScreen(pView_attr->viewX1, pView_attr->viewY1, (U16)(pView_attr->viewX1 + pView_attr->viewWidth),
					(U16)(pView_attr->viewY1 + pView_attr->viewHeight), pPriv->normalbgcolor);
		}
		
		if(pPriv->shortbar)
		{
			Grap_ShowPicture(pPriv->shortbar, pView_attr->viewX1, pView_attr->viewY1);
		}
		else
		{
			Grap_BrushScreen(x1, y1, x2, y2, pPriv->bar_color);
		}
		disp_info.color  = pPriv->clrfocus;
	}
	else
	{
		if(pView_attr->clNormal)
		{
			Grap_ShowPicture(pView_attr->clNormal, pView_attr->viewX1, pView_attr->viewY1);
		}
		else if(pPriv->backpicid)
		{
			Grap_ClearScreen(pView_attr->viewX1, pView_attr->viewY1, (U16)(pView_attr->viewX1 + pView_attr->viewWidth),
					(U16)(pView_attr->viewY1 + pView_attr->viewHeight));
		}
		else
		{
			Grap_BrushScreen(pView_attr->viewX1, pView_attr->viewY1, (U16)(pView_attr->viewX1 + pView_attr->viewWidth),
					(U16)(pView_attr->viewY1 + pView_attr->viewHeight), pPriv->normalbgcolor);
		}
		
		{
			Grap_BrushScreen(x1, y1, x2, y2, pPriv->normalbgcolor);
		}
		disp_info.color  = pPriv->clrnormal;
	}
	
	//��ʾ�ַ�
	memset(tmpStr,0,50);
	pCombox->comboxgetstr(pCombox,pPriv->cur_index,tmpStr);	
	
	if(tmpStr[0] == 0) return FAIL;
	disp_info.string = tmpStr;
	disp_info.font_size = pPriv->font_size;
	disp_info.font_type = pPriv->font_type;
	disp_info.flag      = 0;
	disp_info.keyStr = NULL;
	tempLimit = gu16Lcd_ColumnLimit;
	gu16Lcd_ColumnLimit = x2;
	Grap_WriteString(x1, y1, x2, y2, &disp_info);
	gu16Lcd_ColumnLimit = tempLimit;

	Grap_WriteBMPColor(pView_attr->viewX1+pView_attr->viewWidth-18, pView_attr->viewY1+2, 18, 16, (U8*)gBmpDownData, 0x00);
	
	if(pCombox->list_opened == 1)
	{//չ��״̬
	//hyUsbPrintf("curFocus = %d \r\n",pView_attr->curFocus);
		if(pView_attr->curFocus)
		{
			x1 = pView_attr->viewX1;
			x2 = x1+pCombox->tempwidth;
			if(pPriv->direction == 0)//��
			{
				y1 = pView_attr->touchY2;
				y2 = pCombox->openY2;
			}
			else//��
			{
				y1 = pCombox->openY1;
				y2 = pView_attr->touchY1;
			}
			if(pPriv->openpicid)
			{//��ָ��ͼƬ
				Grap_ShowPicture(pPriv->openpicid, x1, y1);
			}
			else
			{//����ɫ���
				drvLcd_SetColor(pPriv->bar_color,pPriv->bar_color);
				drvLcd_Rectangle(y1-1,x1-1,y2-y1+2,x2-x1+2,1);
				Grap_BrushScreen(x1,y1,x2,y2, pPriv->normalbgcolor);
			}
			
			if(pPriv->needscr == 1)//��Ҫ��������
			{
				Grap_ComboboxDrawScr(pCombox);
			}

			for(i = 0; i < pPriv->dis_num; i++)
			{
				if(pPriv->top_index + i >= pPriv->total_num)
					break;
				pCombox->comboxdrawone(pView, pPriv->top_index + i);
			}
		}
		else
		{
			pCombox->list_opened = 0;
			ReDraw(pCombox,0,0);
		}
	}
	
	//����ǵ�ǰ����,�������滭�߿�
	Grap_DrawFocusRect(pView_attr, COLOR_RED);
	
	return SUCCESS;
}

//��̬�ı����Ժ����init����
S32 Grap_ComboboxInit(void *pView)
{
	tGRAPCOMBOBOX	*pCombox;
	tGrapViewAttr	*pView_attr;
	tComboboxPriv	*pPriv;
	tComboboxScroll	*pScr;
	tRES_PARA       tRes;
	U16 y1,y2,h;
	
	pCombox		= (tGRAPCOMBOBOX *)pView;
	pView_attr	= &(pCombox->view_attr);
	pPriv		= &(pCombox->comboPriv);
	pScr		= &(pCombox->comboScr);
	
	/*pPriv->cur_index 		= 0;
	pPriv->opencur_index 	= 0;
	pPriv->top_index 		= 0;*/
	pPriv->firstcharpos 	= 0;
	pPriv->timerlft 		= pPriv->timertot;
	
	if(pPriv->cur_index >= pPriv->total_num)
	{
		pPriv->cur_index = pPriv->total_num-1 <= 0 ? 0 : pPriv->total_num-1;
	}
	pPriv->opencur_index = pPriv->cur_index;
	if(pPriv->top_index+pPriv->dis_num <= pPriv->cur_index)
	{
		pPriv->top_index = pPriv->cur_index+1 - pPriv->dis_num;
	}
	else if(pPriv->top_index > pPriv->cur_index)
	{
		pPriv->top_index = pPriv->cur_index;
	}
	
	if(pPriv->dis_num <= pPriv->total_num && pPriv->top_index+pPriv->dis_num >= pPriv->total_num)
	{
		pPriv->top_index = pPriv->total_num-pPriv->dis_num;
	}
	
	if(pPriv->dis_num < pPriv->total_num)
	{
		h = pPriv->dis_num * pView_attr->viewHeight;
		pPriv->needscr = 1;
	}
	else
	{
		h = pPriv->total_num * pView_attr->viewHeight;
		pPriv->needscr = 0;
	}

	if(pView_attr->viewY1+h+pView_attr->viewHeight > cLCD_TOTAL_ROW)
	{//��Ҫ����չ��
		pPriv->direction = 1;
	}
	else
	{
		pPriv->direction = 0;
	}
	
	pCombox->openX1 = pView_attr->touchX1;
	pCombox->openX2 = pView_attr->touchX2;
	if(pPriv->direction == 0)//��
	{
		pCombox->openY1 = pView_attr->touchY1+1;
		pCombox->openY2 = pCombox->openY1+h+pView_attr->viewHeight;
	}
	else//��
	{
		pCombox->openY1 = pView_attr->touchY1-h-1;
		pCombox->openY2 = pView_attr->touchY2;
	}
	
	pPriv->openwidth = pCombox->tempwidth;
//hyUsbPrintf("combox needSrc = %d \r\n",pPriv->needscr);
	if(pPriv->needscr == 1)
	{//�������������
		if(pPriv->scrBarHeadId > 0)
		{
			if(res_load_bmp_info(pPriv->scrBarHeadId, &tRes) < 0)
			{
				hyUsbPrintf("load bmp fail \r\n");
				return FAIL;
			}
		}
		else
		{
			tRes.width = 20;
			tRes.high  = 16;
		}
		pPriv->openwidth -= tRes.width;
		
		pScr->viewX1 = pView_attr->viewX1+pPriv->openwidth;
		pScr->viewX2 = pView_attr->viewX1+pCombox->tempwidth;
		if(pPriv->direction == 0)//��
		{
			pScr->viewY1 = pView_attr->touchY2;
			pScr->viewY2 = pCombox->openY2;
		}
		else//��
		{
			pScr->viewY1 = pCombox->openY1;
			pScr->viewY2 = pView_attr->touchY1;
		}
		
		pScr->scrStartY 	= pScr->viewY1 + tRes.high;
		pScr->scrEndY 		= pScr->viewY2 - tRes.high;
		pScr->scrLen		= pScr->scrEndY - pScr->scrStartY;
		
		pScr->arrow1_Y1 	= pScr->viewY1;
		pScr->arrow1_Y2 	= pScr->viewY1+tRes.high-3;
		pScr->arrow2_Y2 	= pScr->viewY2;
		pScr->arrow2_Y1 	= pScr->arrow2_Y2-tRes.high+3;
		
		
		pCombox->openX2 = pScr->viewX2;
		
		Grap_ComboxUpdateScr(pCombox);
	}
	
	return SUCCESS;
}

S32 Grap_ComboboxHandle(void *pView, GRAP_EVENT *pEvent)
{
	U32 message, wParam, lParam ;
	tGRAPCOMBOBOX *pCombobox;
	tComboboxPriv  *pPriv;
	tComboboxScroll  *pScr;
	tGrapViewAttr *pview_attr;
	S32 ret = SUCCESS;

	pCombobox = (tGRAPCOMBOBOX *)pView;
	pPriv = &(pCombobox->comboPriv);
	pScr = &(pCombobox->comboScr);
	pview_attr = &(pCombobox->view_attr);
	
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	
	switch(message)
	{
	case VIEW_INIT:
		pCombobox->comboxinit(pView);
		break;
	case VIEW_ENABLE:
		pCombobox->view_attr.enable = 1;
		pCombobox->draw(pCombobox, 0, 0);
		break;
	case VIEW_DISABLE:
		pCombobox->view_attr.enable = 0;
		break;
	case VIEW_DRAW:
	case VIEW_UPDATE:
		pCombobox->draw(pView, 0, 0);
		break;
	case VIEW_TIMER:
		if(pview_attr->curFocus && pPriv->timerenable)
		{
			if(pPriv->timerlft == 0)
			{
				pPriv->timerlft = pPriv->timertot;
				if(pCombobox->list_opened == 0)
				{
					pCombobox->comboxtimer(pCombobox,pPriv->cur_index);
				}
				else
				{
					pCombobox->comboxtimer(pCombobox,pPriv->opencur_index);
				}
				//����ǵ�ǰ����,�������滭�߿�
				Grap_DrawFocusRect(&(pCombobox->view_attr), COLOR_RED);
			}
			else
			{
				S32 timeleft;
				
				timeleft = pPriv->timerlft-lParam;
				
				if (timeleft < 0)  pPriv->timerlft = 0;
				else pPriv->timerlft = timeleft;				
			}
		}
		break;
	case VIEW_LINEUP:
		break;
	case VIEW_LINEDN:
		break;
	case VIEW_PRESS:
	//hyUsbPrintf("combox focus = %d  lparam  = %d  wparam = %d  \r\n",
	//		pCombobox->view_attr.curFocus, lParam, wParam);
		if((pCombobox->view_attr.curFocus == 1 || pEvent->reserve == 1))//����ʱ��ӦOK��
		{
			switch(lParam)
			{
			case KEYEVENT_OK:
				if(cKEYB_EVENT_RELEASE == wParam)
				{
					if(pCombobox->list_opened)
					{
						pPriv->cur_index = pPriv->opencur_index;
						pCombobox->comboxenter(pCombobox,pPriv->opencur_index);
						pCombobox->list_opened = 0;
						//����Ҫredraw����ˢ�������������
						ReDraw(pCombobox, 0, 0); 
					}
					else
					{
						pCombobox->list_opened = 1;
						pCombobox->draw(pView, 0, 0);
					}
				}
				return PARSED;
				break;
			case KEYEVENT_UP:
				if(pCombobox->list_opened)
				{
					if(pPriv->opencur_index > 0)
					{
						pPriv->opencur_index--;
					}
					if(pPriv->opencur_index < pPriv->top_index)
					{
						pPriv->top_index = pPriv->opencur_index;
					}
					Grap_ComboboxLineUp(pCombobox,0);
					pCombobox->draw(pView, 0, 0);
				}
				else
				{
					Grap_ChangeFocus(pCombobox, 0);
				}
				return PARSED;
				break;
			case KEYEVENT_DOWN:
				if(pCombobox->list_opened)
				{
					if(pPriv->opencur_index+1 < pPriv->total_num)
					{
						pPriv->opencur_index++;
					}
					
					if(pPriv->opencur_index >= pPriv->top_index+pPriv->dis_num)
					{
						pPriv->top_index = pPriv->opencur_index;
					}
					Grap_ComboboxLineDown(pCombobox,0);
					pCombobox->draw(pView, 0, 0);
				}
				else
				{
					Grap_ChangeFocus(pCombobox, 1);//�����л�����
				}
				return PARSED;
				break;
			default:
				break;
			}
		}
		break;
	case VIEW_ENTER: //�˴��������������Ĵ���Ҫ����
		break;
	case VIEW_CLICK:
		break;
	case VIEW_QUIT:
		break;
	case VIEW_REV_SYSMSG:
		pCombobox->recvSysMsg(pView);
		break;
	case VIEW_REV_PULLMSG:
	    ret = pCombobox->recvPullMsg(pCombobox,pEvent);
	    break;	
	default:
		break;
	}
	
	return ret;
}

S32 Grap_ComboboxDestroy(void *pView)
{
	tGRAPCOMBOBOX *pCombobox;
	pCombobox = (tGRAPCOMBOBOX *)pView;

	free((void *)pCombobox);
	return SUCCESS;
}

tGRAPCOMBOBOX *Grap_CreateCombobox(tGrapViewAttr  *pview_attr, tComboboxPriv  *pcomboPriv)
{
	tGRAPCOMBOBOX *pCombobox;

	pCombobox = (tGRAPCOMBOBOX *)malloc(sizeof(tGRAPCOMBOBOX));
	if (!pCombobox)
	{
		return NULL;
	}
	memset(pCombobox,0,sizeof(tGRAPCOMBOBOX));
	
	pCombobox->comboPriv.cur_index 		= 0;
	pCombobox->comboPriv.opencur_index 	= 0;
	pCombobox->comboPriv.top_index 		= 0;
	
	pCombobox->view_attr = *pview_attr;
	pCombobox->comboPriv = *pcomboPriv;
	
	pCombobox->tempwidth = pCombobox->comboPriv.openwidth;
	
	if(pCombobox->comboPriv.dis_num < pCombobox->comboPriv.total_num)
	{//��ֹ������������д����
		pCombobox->comboPriv.needscr = 1;
	}
	else
	{
		pCombobox->comboPriv.needscr = 0;
	}
	
	pCombobox->draw			= Grap_ComboboxDraw;
	pCombobox->handle		= Grap_ComboboxHandle;
	pCombobox->destroy		= Grap_ComboboxDestroy;
	pCombobox->recvSysMsg	= Grap_ComboboxRecvSysMsg;
    pCombobox->recvPullMsg	= Grap_ComboboxRecvPullMsg;
	pCombobox->comboxinit	= Grap_ComboboxInit;
	pCombobox->comboxdrawone= Grap_ComboboxDrawOne;
	pCombobox->comboxenter	= Grap_ComboboxEnter;
	pCombobox->comboxtimer	= Grap_ComboboxTimer;
	pCombobox->combocdrawscr= Grap_ComboboxDrawScr;
	pCombobox->comboxgetstr = Grap_ComboboxGetStr;

	
	return pCombobox;
}

tGRAPCOMBOBOX *Grap_InsertCombobox(void *pView, tGrapViewAttr  *pview_attr, tComboboxPriv  *pcomboPriv)
{
	tGRAPDESKBOX *pParentView;
	tGRAPCOMBOBOX *pCombobox;

	pParentView = (tGRAPDESKBOX *)pView;
	if (!pParentView) return NULL;
	pCombobox = (tGRAPCOMBOBOX *)malloc(sizeof(tGRAPCOMBOBOX));
	if (!pCombobox)
	{
		return NULL;
	}
	memset(pCombobox,0,sizeof(tGRAPCOMBOBOX));
	pCombobox->pPrev   = pParentView->pTail;
	pParentView->pTail->pNext = (tGRAPVIEW *)pCombobox;
	pParentView->pTail = (tGRAPVIEW *)pCombobox;
	pCombobox->pParent = pParentView;
	
	//pCombobox->pNext = pParentView->pNext;
	//pParentView->pNext = (tGRAPVIEW *)pCombobox;
	//pCombobox->pParent = pParentView;
	pCombobox->comboPriv = *pcomboPriv ;
	
	pCombobox->comboPriv.cur_index 		= 0;
	pCombobox->comboPriv.opencur_index 	= 0;
	pCombobox->comboPriv.top_index 		= 0;
	
	pCombobox->view_attr = *pview_attr;
	pCombobox->comboPriv = *pcomboPriv;
	
	pCombobox->tempwidth = pCombobox->comboPriv.openwidth;
	
	if(pCombobox->comboPriv.dis_num < pCombobox->comboPriv.total_num)
	{//��ֹ������������д����
		pCombobox->comboPriv.needscr = 1;
	}
	else
	{
		pCombobox->comboPriv.needscr = 0;
	}

	pCombobox->draw			= Grap_ComboboxDraw;
	pCombobox->handle		= Grap_ComboboxHandle;
	pCombobox->destroy		= Grap_ComboboxDestroy;
	pCombobox->recvSysMsg	= Grap_ComboboxRecvSysMsg;
    pCombobox->recvPullMsg	= Grap_ComboboxRecvPullMsg;
	pCombobox->comboxinit	= Grap_ComboboxInit;
	pCombobox->comboxdrawone= Grap_ComboboxDrawOne;
	pCombobox->comboxenter	= Grap_ComboboxEnter;
	pCombobox->comboxtimer	= Grap_ComboboxTimer;
	pCombobox->combocdrawscr= Grap_ComboboxDrawScr;
	pCombobox->comboxgetstr = Grap_ComboboxGetStr;

	
	return pCombobox;
}
