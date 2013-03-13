#include "grap_lister.h"
#include "grap_event.h"
#include "grap_deskbox.h"

const U8 Suspension_points[]={0x33,0x30,0x33,0x30};

S32 Grap_ChangeItem(tGRAPLIST *pLister, U16 i)
{
	return SUCCESS;
}

S32 Grap_ListDrawIcon(tGRAPLIST *pLister, U16 i)
{
	return SUCCESS;
}


//����1 ��ʾ����Ĵ�������key   0 ��ʾδ����
//��ʾ��1��ʼ   ��0���ʾ10�Ŀ�ݼ�        keycode:0xff��ʾ��timer�����
S32 Grap_ListKeyInput(tGRAPLIST *pLister, U16 keycode)
{
	int ret=0;
	U16 key;
	tGrapListerPriv *plst_privattr;

	plst_privattr = &(pLister->lst_privattr);
//hyUsbPrintf("keycode ==== %d  total = %d \r\n",keycode,plst_privattr->totalItem);
	if(keycode == 0xFF)//��timer����   �ڶ���������ʱδ��
	{
	//hyUsbPrintf("keycode = 0x%02x,  lastkey = 0x%02x,  total = %d \r\n", keycode, pLister->lastKey, plst_privattr->totalItem);
		//��ʱû�еȵ���������,��ֱ��ִ��֮ǰ�����Ķ���
		key = pLister->lastKey&0x7F;
		plst_privattr->enable = (pLister->lastKey>>7);
		pLister->lastKey = 0;
		if(key <= plst_privattr->totalItem)
		{
			plst_privattr->focusItem = key-1;
			if(plst_privattr->focusItem < plst_privattr->topItem)	// if delete last item(in the top of last screen),must change to pre-page.
			{
				 plst_privattr->topItem = plst_privattr->focusItem;
			}
			else if(plst_privattr->focusItem >= plst_privattr->topItem+plst_privattr->lstNum)
			{
				plst_privattr->topItem = plst_privattr->focusItem+1-plst_privattr->lstNum;
			}
			pLister->draw(pLister,0,0);
			DisplayData2Screen();
			syssleep(15);
			ret = pLister->enter(pLister, plst_privattr->focusItem);
			if(ret == RETURN_REDRAW)
			{
				ReDraw(pLister, 0, 0);
			}
		}
		
		return 1;
	}
	
	//ֻ����0--9���ּ�,�����ֻ֧��2λ(0--99)
	if(keycode < KEYEVENT_0 || keycode > KEYEVENT_9)
	{
	//hyUsbPrintf("keycode too big = %d  lastkey = %d  timer = %d \r\n",keycode,pLister->lastKey,plst_privattr->enable);
		if(pLister->lastKey > 0)
		{
			plst_privattr->enable = (pLister->lastKey>>7);
			pLister->lastKey = 0;
		}
		return 0;
	}
	
	keycode -= KEYEVENT_0;
	
	if(pLister->lastKey == 0)//��һ����
	{
		key = keycode;
		if(key == 0) key = 10;//����0ת����10
	//hyUsbPrintf("first key  = %d  key = %d \r\n",keycode ,key);
		if(key > plst_privattr->totalItem)
		{
			//����ֵ��������
			return 0;
		}
		
		if(key == 10)
		{
			plst_privattr->focusItem = key-1;
			if(plst_privattr->focusItem < plst_privattr->topItem)	// if delete last item(in the top of last screen),must change to pre-page.
			{
				 plst_privattr->topItem = plst_privattr->focusItem;
			}
			else if(plst_privattr->focusItem >= plst_privattr->topItem+plst_privattr->lstNum)
			{
				plst_privattr->topItem = plst_privattr->focusItem+1-plst_privattr->lstNum;
			}
			pLister->draw(pLister,0,0);
			DisplayData2Screen();
			syssleep(15);
			ret = pLister->enter(pLister, plst_privattr->focusItem);
			if(ret == RETURN_REDRAW)
			{
				ReDraw(pLister, 0, 0);
			}
			return 1;
		}
		else if(key*10 >= plst_privattr->totalItem)
		{
		//hyUsbPrintf("key*10  = %d \r\n", key*10);
			//��ͳ�2λ���󳬹�����,��ֻ�ܰ�һλ����
			plst_privattr->focusItem = key-1;
			if(plst_privattr->focusItem < plst_privattr->topItem)	// if delete last item(in the top of last screen),must change to pre-page.
			{
				 plst_privattr->topItem = plst_privattr->focusItem;
			}
			else if(plst_privattr->focusItem >= plst_privattr->topItem+plst_privattr->lstNum)
			{
				plst_privattr->topItem = plst_privattr->focusItem+1-plst_privattr->lstNum;
			}
			pLister->draw(pLister,0,0);
			DisplayData2Screen();
			syssleep(15);
			ret = pLister->enter(pLister, plst_privattr->focusItem);
			if(ret == RETURN_REDRAW)
			{
				ReDraw(pLister, 0, 0);
			}
			return 1;
		}
		else
		{
		//hyUsbPrintf(" init timer  = %d \r\n",plst_privattr->enable);
			//��Ҫ�ȴ��ڶ�������
			pLister->lastKey = key;
			if(plst_privattr->enable == 1)
			{
				pLister->lastKey |= 0x80;
			}
			
			//����timer
			plst_privattr->enable = 1;
			plst_privattr->TimerLft = 100;//�ȴ�1S
		//hyUsbPrintf("    timer  = %d \r\n",plst_privattr->enable);
		}
	}
	else //if(pLister->lastKey != 0)//֮ǰ�а���,�����Ǻ�����
	{
		key = (pLister->lastKey&0x7F);
		plst_privattr->enable = (pLister->lastKey>>7);
		pLister->lastKey = 0;
		
		key = key*10+keycode;
	//hyUsbPrintf("keycode = %d  key = %d \r\n",keycode, key);
		if(key <= plst_privattr->totalItem)
		{
			plst_privattr->focusItem = key-1;
			if(plst_privattr->focusItem < plst_privattr->topItem)	// if delete last item(in the top of last screen),must change to pre-page.
			{
				 plst_privattr->topItem = plst_privattr->focusItem;
			}
			else if(plst_privattr->focusItem >= plst_privattr->topItem+plst_privattr->lstNum)
			{
				plst_privattr->topItem = plst_privattr->focusItem+1-plst_privattr->lstNum;
			}
			pLister->draw(pLister,0,0);
			DisplayData2Screen();
			syssleep(15);
			ret = pLister->enter(pLister, plst_privattr->focusItem);
			if(ret == RETURN_REDRAW)
			{
				ReDraw(pLister, 0, 0);
			}
			return 1;
		}
		return 0;
	}
	
	return 0;
}

int Grap_GetDispStr(Character_Type_e kStr_Type,U16 factwid,U8 *tmpStr)
{
	int t=0;
	
	if(kStr_Type == CHARACTER_LOCALCODE)
	{
		while(factwid)
		{
			if(tmpStr[t]>=0x80)
			{
				if(factwid < 2)	break;
				else	factwid-=2;
				t+=2;
			}
			else
			{
				factwid--;
				t++;
			}
		}
	}
	else 
	{
		while(factwid)
		{
			if(tmpStr[t]>0x03)//ȫ��
			{
				if(factwid < 2)	break;
				else	factwid-=2;
			}
			else	factwid--;
			
			t+=2;
		}
	}
	
	return t;
}

S32 Grap_ListerTimer(tGRAPLIST *pLister, U16 i)//i:��ǰ����
{
	U16 x1, y1, x2, y2;	//list item range
	U8	tmpStr[LISTSTR_MAXLEN];
	U8  string[30], *pStr;
	U16 len=0,lenstr ,widstr;
	tStrDispInfo disp_info;
	Character_Type_e kStr_Type;
	
	tGrapViewAttr *pview_attr = &(pLister->view_attr);
	tGrapListerPriv *plst_privattr = &(pLister->lst_privattr);

	if ( i >= plst_privattr->totalItem )
	{
		return SUCCESS;
	}
	memset(tmpStr,0,LISTSTR_MAXLEN);
	memset(string,0,30);
	
	pLister->getListString(pLister, i, tmpStr, LISTSTR_MAXLEN);
	
	kStr_Type = plst_privattr->font_type;
	lenstr = Get_StringLength(kStr_Type,tmpStr);
	widstr = Get_StringWidth(kStr_Type,tmpStr);//�ַ����(UNICODE��ǵİ�1��)
	
	if(widstr <= plst_privattr->curLen)//����Ҫ�ƶ���ʾ
		return SUCCESS;

	x1 = pview_attr->viewX1;
	x2 = pview_attr->viewX1 + pview_attr->viewWidth;
	y1 = (i - plst_privattr->topItem) * plst_privattr->lstHeight + pview_attr->viewY1;
	y2 = y1 + plst_privattr->lstHeight;

	if(plst_privattr->scrbarPicId)
		Grap_ShowPicture(plst_privattr->scrbarPicId, x1, y1);
	else if(plst_privattr->backgndPicId)
		Grap_ClearScreen(x1, y1, x2, y2);
	else if(pview_attr->color != plst_privattr->lstFocusColor)//ѡ�����ͱ�����û��ʱ������ָ������ɫ���(���ָ����ɫ�ͽ���ɫһ������ˢ��)
		Grap_BrushScreen(x1, y1, x2, y2, pview_attr->color);
	
	if(kStr_Type == CHARACTER_LOCALCODE)
	{
		if(tmpStr[plst_privattr->firstCharPos]>=0x80)	plst_privattr->firstCharPos+=2;
		else	plst_privattr->firstCharPos++;
	}
	else
	{
		plst_privattr->firstCharPos+=2;
	}
	if(plst_privattr->firstCharPos >= lenstr) plst_privattr->firstCharPos = 0;
	
	//��ǰ���Сͼ��--��Ҫ��ͼ��Ҫ�Լ����ظú���
	pLister->drawIcon(pLister,i);
	
	{
		U16 templen,tempwid;
		int t;
		
		pStr = &tmpStr[plst_privattr->firstCharPos];
		templen = Get_StringLength(kStr_Type,pStr);
		tempwid = Get_StringWidth(kStr_Type,pStr);
		if(tempwid <= plst_privattr->curLen && tempwid+6 >= plst_privattr->curLen)
		{
			memcpy(string, pStr, templen);
			string[templen]=0;
			string[templen+1]=0;
		}
		else if(tempwid+6 < plst_privattr->curLen)
		{
			memcpy(string, pStr, templen);
			if(kStr_Type == CHARACTER_LOCALCODE)
			{
				for(t = 0;t < 6;t++)
				{
					string[templen++]=0x20;
				}
			}
			else
			{
				for(t = 0;t < 6;t++)
				{
					string[templen++]=0x0;
					string[templen++]=0x20;
				}
			}
			
			tempwid = plst_privattr->curLen - tempwid - 6;
			pStr = tmpStr;
			t = Grap_GetDispStr(kStr_Type,tempwid,pStr);
			memcpy(&string[templen], pStr, t);
			string[templen+t]  =0;
			string[templen+t+1]=0;
		}
		else if(tempwid > plst_privattr->curLen)
		{
			tempwid = plst_privattr->curLen;
			t = Grap_GetDispStr(kStr_Type,tempwid,pStr);
			memcpy(string, pStr, t);
			string[t]=0;
			string[t+1]=0;
		}
	}
	
	disp_info.string = string;
	disp_info.color = plst_privattr->lstFocusColor;
	disp_info.font_size = plst_privattr->font_size;
	disp_info.font_type = plst_privattr->font_type;
	disp_info.flag = 0;
	disp_info.keyStr = NULL;
	//��ǰ���Сͼ��--��Ҫ��ͼ��Ҫ�Լ����ظú���
	pLister->drawIcon(pLister,i);
	Grap_WriteString(x1+plst_privattr->x_start, y1+plst_privattr->y_start, x1+plst_privattr->x_start+8*plst_privattr->curLen, y2, &disp_info);

	return SUCCESS;
}

S32 Grap_ListerRecvSysMsg(void *pView)
{

	return SUCCESS;
}


S32 Grap_ListerRecvPullMsg(void *pView,GRAP_EVENT *pEvent)
{
    U32 extParam, message, wParam, lParam ;
	tGRAPLIST *pLister;
	tGrapViewAttr *pview_attr;
	tGrapListerPriv *plst_privattr;
	tGRAPVIEW *pTempView;
	S32 ret = FAIL;
	int x, y,i,x0,y0 ;
	GRAP_EVENT event;

  
	pLister = (tGRAPLIST *)pView;
	pview_attr = &(pLister->view_attr);
	plst_privattr = &(pLister->lst_privattr);

    message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;
	
	event.id = pLister->view_attr.reserve;//�����洢��������Ĺ�������ID
	event.message = VIEW_DRAW;
	event.wParam = 0;
	event.lParam = 0;

	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;
	
	y0 = HIWORD(wParam) ;
	x0 = LOWORD(wParam) ;
	
	
	if (VIEW_PULL_START == extParam)
	{//����ʼ��

		if(GRAP_INWIDGET_RANGE(x,y,pLister))
	    {//����Ч��Χ��
	         ret = VIEW_PULL_START;
	         pLister->startPos = (y<<16)|x; 
	    }
	    else
	    {
	        return FAIL;
	    }
	}
	
	if (VIEW_PULL_START == extParam)
	{//down
		Grap_ChangeFocusEnable(pLister);
		
		for(i = 0; i < plst_privattr->totalItem - plst_privattr->topItem; i++)
	    {
		    if(y >= pLister->view_attr.touchY1 + i*plst_privattr->lstHeight
			&& y <= pLister->view_attr.touchY1 + (i+1)*plst_privattr->lstHeight)
			{
        
                if (0 == pLister->ConfirmAndEnter && plst_privattr->focusItem == i+plst_privattr->topItem) 
				{
				    pLister->ConfirmAndEnter = 3;				    
				}
                
                if(plst_privattr->focusItem != i+plst_privattr->topItem) 
				{
					plst_privattr->focusItem = i+plst_privattr->topItem ;
					plst_privattr->TimerLft = plst_privattr->TimerTot;
					plst_privattr->firstCharPos = 0 ;
					pLister->draw(pLister, 0, 0);
					pLister->changeItem(pLister,plst_privattr->focusItem);
            		
            		if (1 != pLister->ConfirmAndEnter) pLister->ConfirmAndEnter = 0;
				}
				
				if (0 == pLister->ConfirmAndEnter) 
				{
				    pLister->ConfirmAndEnter = 2;				    
				}    
                
				break;
			}
		}			
	}
	else if (VIEW_PULL_ING == extParam)
	{//��ק��
	    
	    int diffy;
	    int direction;
	    int diffnum;
	    int lstnum;
	    U32 focusItem;
	    U32 topItem;
	    U32 totalItem;
	    U32 lstHeight;
	    
	    if (1 != pLister->ConfirmAndEnter) 
		{
		    pLister->ConfirmAndEnter = 0;				    
		}
    
	    if (!GRAP_INWIDGET_RANGE(x,y,pLister))
	    {
	        return FAIL;    
	    }
	    
	    diffy = y-y0;
	    
	    focusItem = plst_privattr->focusItem;
	    topItem = plst_privattr->topItem;
	    totalItem = plst_privattr->totalItem;
	    lstnum = plst_privattr->lstNum;
	    lstHeight =  plst_privattr->lstHeight;
	    
	    if (diffy > 0)
	    {//down
	    
	        diffnum = diffy/lstHeight;
 
	        if (topItem > 0 && diffnum > 0)
	        {
	            if (topItem-diffnum < 0)
	            {
	                topItem = plst_privattr->topItem = 0; 
	            }
	            else  
	            {
	                plst_privattr->topItem -= diffnum;
	                topItem =  plst_privattr->topItem;
	            } 
	            
                
                //plst_privattr->focusItem = plst_privattr->topItem;
                
                pLister->lst_privattr.TimerLft = pLister->lst_privattr.TimerTot;
			    plst_privattr->firstCharPos = 0 ;
                pLister->draw(pLister,0,0);    
	            
	            ret = VIEW_PULL_ING;    
	        } 
	    }
	    else
	    {//up
	        
	        diffy = ABS(diffy);
	        
	        diffnum = diffy/lstHeight;
	        
	        if (topItem+lstnum < totalItem && diffnum > 0)
	        {
	            if (topItem+diffnum >= totalItem)
	            {
	                
	            }
	            else  
	            {
	                plst_privattr->topItem += diffnum;
	                topItem =  plst_privattr->topItem;
	            }        
	                            
                //plst_privattr->focusItem = plst_privattr->topItem;
                
                pLister->lst_privattr.TimerLft = pLister->lst_privattr.TimerTot;
			    plst_privattr->firstCharPos = 0 ;
                pLister->draw(pLister,0,0);    
	            
	            ret = VIEW_PULL_ING;    
	        } 
	        
	    } 
	    	     
    
	}
	else if (VIEW_PULL_END == extParam)
	{//up 	    			
        
        int starty;
        
        if( !(GRAP_INWIDGET_RANGE(x,y,pLister) && y< (pLister->view_attr.touchY1+(plst_privattr->totalItem - plst_privattr->topItem)*plst_privattr->lstHeight))) 
        {
            return VIEW_PULL_END;
        }
        
        starty = HIWORD(pLister->startPos);
        
        if (ABS(y-starty) >= plst_privattr->lstHeight)  return VIEW_PULL_END;  
        
        for(i = 0; i < plst_privattr->totalItem - plst_privattr->topItem; i++)
	    {
		    if(y >= pLister->view_attr.touchY1 + i*plst_privattr->lstHeight
			&& y <= pLister->view_attr.touchY1 + (i+1)*plst_privattr->lstHeight)
			{        
                
                int flag = 0; 
				
				if(pLister->ConfirmAndEnter != 1)
				{
					if (pLister->ConfirmAndEnter == 2)
					{
					    pLister->ConfirmAndEnter = 3;    
					}
					else
					{
					    pLister->ConfirmAndEnter = 0;
					    flag = 1;
					}
				}
				else
				{
				    flag = 1;        
				}
				
				if (1 == flag )
				{
			        ret = pLister->enter(pLister,plst_privattr->focusItem);	
            		
					if (ret == RETURN_REDRAW)
					{
						ReDraw(pLister, 0, 0);
						ret = VIEW_PULL_END;
					}
					else if (ret == RETURN_FOCUSMSG)
					{
						return ret;
					}
					else if (RETURN_QUIT == ret)
					{
						*((tGRAPDESKBOX *)pLister->pParent)->pQuitCnt = 1;
						ret = VIEW_PULL_END;							
					}
					else if(ret == SUCCESS)
					{
						ret = VIEW_PULL_END;
					}    
				}
				
				break;
			}
		}	
	}
	else if (VIEW_PULL_TIMEOUT == extParam)
	{//��ʱ
	    ret = VIEW_PULL_TIMEOUT;  
	}
	
	return ret; 
 
}


// List ˢ����ʾ��i���ĺ�����(ˢ�±���+��ʾ�ִ�)
S32 Grap_ListerUpdateItem(tGRAPLIST *pLister, U16 i)
{
	U16 x1, y1, x2, y2;	//list item range
	U32 lenstr, widstr;
	U8	n,tmpStr[LISTSTR_MAXLEN];
	tGrapViewAttr *pview_attr = &(pLister->view_attr);
	tGrapListerPriv *plst_privattr = &(pLister->lst_privattr);

	if ( i >= plst_privattr->totalItem)
	{
		return SUCCESS;
	}
	memset(tmpStr,0,LISTSTR_MAXLEN);
	x1 = pview_attr->viewX1;
	x2 = pview_attr->viewX1 + pview_attr->viewWidth;
	y1 = (i - plst_privattr->topItem) * plst_privattr->lstHeight + pview_attr->viewY1;
	y2 = y1 + plst_privattr->lstHeight;

	if(plst_privattr->focusItem == i && plst_privattr->scrbarPicId)
		Grap_ShowPicture(plst_privattr->scrbarPicId, x1, y1);
	else if(plst_privattr->backgndPicId)
		Grap_ClearScreen(x1, y1, x2, y2);
	else if(pview_attr->color != plst_privattr->lstFocusColor)//ѡ�����ͱ�����û��ʱ������ָ������ɫ���(���ָ����ɫ�ͽ���ɫһ������ˢ��)
		Grap_BrushScreen(x1, y1, x2, y2, pview_attr->color);

	pLister->getListString(pLister, i, tmpStr, LISTSTR_MAXLEN);

	//��ǰ���Сͼ��--��Ҫ��ͼ��Ҫ�Լ����ظú���
	pLister->drawIcon(pLister,i);

	if(tmpStr[0]!=0 || tmpStr[1] != 0)
	{	
		U8 string[30];
		U16 len=0;
		tStrDispInfo disp_info;
		Character_Type_e kStr_Type; 
		
		kStr_Type = plst_privattr->font_type;//�ж�����
		lenstr = Get_StringLength(kStr_Type,tmpStr);//�ַ�����(UNICODE��ǵ�Ҳ��2��)
		widstr = Get_StringWidth(kStr_Type,tmpStr);//�ַ����(UNICODE��ǵİ�1��)

		if(widstr <= plst_privattr->curLen)
		{
			memcpy(string, tmpStr, lenstr);
			string[lenstr]=0;
			string[lenstr+1]=0;
		}
		else
		{
			lenstr = plst_privattr->curLen;
			memcpy(string, tmpStr, lenstr);
			for(n = 0; n < lenstr; n++)
			{
				if(string[n] < 0x80)
				{
					
				}
				else
				{
					if(n+1 < lenstr)
					{
						n++;
					}
					else
					{
						break;
					}
				}
			}
			
			
			string[n]=0;
		}
		/*else
		{
			int t,kRow,kColumn,Num;
			widstr = plst_privattr->curLen-2;//���ܳ��������ʾ���
			t = Grap_GetDispStr(kStr_Type,widstr,tmpStr);
			memcpy(string, tmpStr, t);
			string[t]=0;
			string[t+1]=0;
			
			if(plst_privattr->focusItem != i)//��ʡ�Ժ�
			{
				Num = Get_StringWidth(kStr_Type,string);//�ַ����(UNICODE��ǵİ�1��)
				kRow = y1+plst_privattr->y_start+12;
				kColumn = x1+plst_privattr->x_start+8*Num;
				drvLcd_SetColor(plst_privattr->lstColor,COLOR_BLACK);
				drvLcd_WriteBMPColor(Suspension_points,2,16,kRow,kColumn);
			}
		}*/
		
		disp_info.string = string;
		if(plst_privattr->focusItem == i)
			disp_info.color = plst_privattr->lstFocusColor;
		else
			disp_info.color = plst_privattr->lstColor;
		disp_info.font_size = plst_privattr->font_size;
		disp_info.font_type = plst_privattr->font_type;
		disp_info.flag      = plst_privattr->position;
		disp_info.keyStr = NULL;
		Grap_WriteString(x1+plst_privattr->x_start, y1+plst_privattr->y_start, x1+plst_privattr->x_start+8*plst_privattr->curLen, y2, &disp_info);
	}

	return SUCCESS;
}

// �����²㺯����Ĭ��Ϊ�ա��ú���������
S32 Grap_ListerEnterItem(tGRAPLIST *pLister, U16 i)
{
	return SUCCESS;
}

// �õ�list���ִ���Ĭ��Ϊ�ա��ú���������
// ��ʵ���У�List�����������������еģ�һ���Ǹ����������Լ���õ��ִ��ġ���ôi��������Ҫ��������
S32 Grap_ListerGetString(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{

	return SUCCESS;
}

// List �����ؼ��Ļ��ƺ�����
S32 Grap_ListerDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLIST *pLister;
	U16 i;
	tGrapViewAttr *pview_attr;
	tGrapListerPriv *plst_privattr;

	pLister = (tGRAPLIST *)pView;
	pview_attr = &(pLister->view_attr);
	plst_privattr = &(pLister->lst_privattr);
	
	Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1+pview_attr->viewWidth), 
		(U16)(pview_attr->viewY1+pview_attr->viewHeight));
	
	if(plst_privattr->totalItem > plst_privattr->lstNum)
	{//��Ҫ��ʾ������
		if(plst_privattr->topItem > plst_privattr->totalItem - plst_privattr->lstNum)
		{
			plst_privattr->topItem = plst_privattr->totalItem - plst_privattr->lstNum;
		}
	}
	
	if(plst_privattr->totalItem <= plst_privattr->lstNum && plst_privattr->topItem > 0)
	{
		plst_privattr->topItem = 0;
	}
	if(plst_privattr->focusItem >= plst_privattr->totalItem && plst_privattr->totalItem > 0)
	{
		plst_privattr->focusItem = plst_privattr->totalItem-1;
	}
	
	for (i = 0; i < plst_privattr->lstNum; i++)
	{
		if(plst_privattr->topItem + i >= plst_privattr->totalItem)
			break;
		pLister->updateList(pLister, (U16)(plst_privattr->topItem + i));
	}
	
	//����Lister��ʾ��ǰ"�ڼ���/�ܹ�������" 
	if(plst_privattr->isShowCount==1 && plst_privattr->totalItem > 0)
	{//��ʾ����
		U8 buf[20];
		tStrDispInfo disp_info;
		
		
		memset(buf,0,20);
		sprintf(buf,"%d/%d",(plst_privattr->focusItem+1),plst_privattr->totalItem);
		
		disp_info.string = buf;
		disp_info.color = COLOR_YELLOW;//plst_privattr->lstFocusColor;
		disp_info.font_size = plst_privattr->font_size;
		disp_info.font_type = plst_privattr->font_type;
		disp_info.flag = 2;
		disp_info.keyStr = NULL;
		Grap_WriteString(pview_attr->viewX1, pview_attr->viewY1+pview_attr->viewHeight-20, 
					pview_attr->viewX1+pview_attr->viewWidth,pview_attr->viewY1+pview_attr->viewHeight, &disp_info);
	}
	
	//����ǵ�ǰ����,�������滭�߿�
	//Grap_DrawFocusRect(pview_attr, COLOR_RED);
	
	return SUCCESS;

}

S32 Grap_ListerDistroy(void *pView)
{
	free(pView);
	
	return SUCCESS;
}

S32 Grap_ListerHandle(void *pView, GRAP_EVENT *pEvent)
{
	U32 message, wParam, lParam ;
	tGRAPLIST *pLister;
	tGrapViewAttr *pview_attr;
	tGrapListerPriv *plst_privattr;
	S32 ret = SUCCESS;

	pLister = (tGRAPLIST *)pView;
	pview_attr = &(pLister->view_attr);
	plst_privattr = &(pLister->lst_privattr);
	
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;

	switch(message)
	{
	case VIEW_INIT:
		if(plst_privattr->focusItem >= plst_privattr->totalItem)
			plst_privattr->focusItem = plst_privattr->topItem;
		break;
	case VIEW_ENABLE:
		pLister->view_attr.enable = 1;
		pLister->draw(pLister, 0, 0);
		break;
	case VIEW_DISABLE:
		pLister->view_attr.enable = 0;
		break;
	case VIEW_DRAW:
		pLister->draw(pLister, 0, 0);
		pLister->lst_privattr.TimerLft = pLister->lst_privattr.TimerTot;
		plst_privattr->firstCharPos = 0 ;
		break;
	case VIEW_TIMER:
		if (plst_privattr->enable == 1 && plst_privattr->totalItem > 0 &&
		    plst_privattr->focusItem >= plst_privattr->topItem &&
		    plst_privattr->focusItem < plst_privattr->topItem+plst_privattr->lstNum)
		{
			if (plst_privattr->TimerLft == 0)
			{
			//hyUsbPrintf("list timer lastkey = %d \r\n", pLister->lastKey);
				plst_privattr->TimerLft = plst_privattr->TimerTot;
				//��Ҫ�ж��Ƿ��ڵȴ�����
				if(pLister->lastKey != 0)
				{
					pLister->keyInput(pLister,0xFF);//0xff��ʾ��timer�����
				}
				else
				{
					pLister->listtimer(pLister,plst_privattr->focusItem);
				}
			}
			else
			{
				S32 timeleft;
				
				timeleft =  plst_privattr->TimerLft-lParam;
				
				if (timeleft < 0)  plst_privattr->TimerLft = 0;
				else plst_privattr->TimerLft = timeleft;				
			}
		}
		break;
	case VIEW_PRESS:
		if(plst_privattr->numkeyenable == 1)
		{
			if(1 == pLister->keyInput(pLister,lParam))
			{
				//����������
				return PARSED;
			}
		}
		switch(lParam)
		{
		case KEYEVENT_UP:
			if (plst_privattr->focusItem > 0)
			{
				plst_privattr->focusItem--;
				if(plst_privattr->focusItem < plst_privattr->topItem)
					plst_privattr->topItem = plst_privattr->focusItem;
			}
			else
			{
				plst_privattr->focusItem = plst_privattr->totalItem-1;
				
				plst_privattr->topItem = plst_privattr->totalItem-plst_privattr->lstNum;
			}
			pLister->draw(pLister, 0, 0);
			
			pLister->lst_privattr.TimerLft = pLister->lst_privattr.TimerTot;
			plst_privattr->firstCharPos = 0 ;
			pLister->changeItem(pLister,plst_privattr->focusItem);
			ret = PARSED;
			break;
		case KEYEVENT_DOWN:
			if (plst_privattr->focusItem+1 == plst_privattr->totalItem)
			{
				plst_privattr->focusItem = 0;
				plst_privattr->topItem = 0;
			}
			else
			{
				plst_privattr->focusItem++;
				if(plst_privattr->focusItem+1-plst_privattr->lstNum > plst_privattr->topItem)
				{
					plst_privattr->topItem++;
				}
			}
			pLister->draw(pLister, 0, 0);
			
			pLister->lst_privattr.TimerLft = pLister->lst_privattr.TimerTot;
			plst_privattr->firstCharPos = 0 ;
			pLister->changeItem(pLister,plst_privattr->focusItem);
			ret = PARSED;
			break;
		}
		
		if((pLister->view_attr.curFocus == 1 || pEvent->reserve == 1) && cKEYB_EVENT_RELEASE == wParam)//����ʱ��ӦOK��
		{
			switch(lParam)
			{
			case KEYEVENT_OK:
				ret = pLister->enter(pLister, plst_privattr->focusItem);
				switch( ret )
				{
				case RETURN_REDRAW:
					if (plst_privattr->focusItem < plst_privattr->topItem)	// if delete last item(in the top of last screen),must change to pre-page.
					{
						 plst_privattr->topItem = plst_privattr->focusItem;
					}
					else
					{
						ReDraw(pLister, 0, 0);
					}
					ret = PARSED;
					break;
				case RETURN_CANCLE:
					break;
				case RETURN_QUIT:
					*((tGRAPDESKBOX *)pLister->pParent)->pQuitCnt = 1;	//should not recall desk message
					ret = PARSED;
					break;
				case RETURN_FOCUSMSG:
					return ret;
					break;	
				}
				break;
			case KEYEVENT_LEFT:
				if(plst_privattr->totalItem<=plst_privattr->lstNum)
					break;
				if (plst_privattr->topItem)
				{
					if (plst_privattr->topItem > plst_privattr->lstNum)
						plst_privattr->topItem -= plst_privattr->lstNum;
					else
						plst_privattr->topItem = 0;
				}
				else
				{
					plst_privattr->topItem = plst_privattr->totalItem - plst_privattr->lstNum;
				}
				plst_privattr->focusItem = plst_privattr->topItem;
				pLister->draw(pLister, 0, 0);
				
				pLister->lst_privattr.TimerLft = pLister->lst_privattr.TimerTot;
				plst_privattr->firstCharPos = 0 ;
				pLister->changeItem(pLister,plst_privattr->focusItem);
				ret = PARSED;
				break;
			case KEYEVENT_RIGHT:
				if(plst_privattr->totalItem<=plst_privattr->lstNum)
					break;
				if (plst_privattr->topItem + plst_privattr->lstNum < plst_privattr->totalItem)
				{
					plst_privattr->topItem += plst_privattr->lstNum;
				}
				else
				{
					plst_privattr->topItem = 0;
				}
				plst_privattr->focusItem = plst_privattr->topItem;
				pLister->draw(pLister, 0, 0);
				
				pLister->lst_privattr.TimerLft = pLister->lst_privattr.TimerTot;
				plst_privattr->firstCharPos = 0 ;
				pLister->changeItem(pLister,plst_privattr->focusItem);
				ret = PARSED;
				break;
			default:
				break;
			}
		}
		break;
	case VIEW_QUIT:
	case VIEW_ESC:
		//will quit at out side
		break;
	case VIEW_REV_SYSMSG:
		pLister->recvSysMsg(pView);
		break;
	case VIEW_REV_PULLMSG:
	    ret = pLister->recvPullMsg(pLister,pEvent);
	    break;	
	
	}

	return ret;
}


tGRAPLIST *Grap_CreateLister(tGrapViewAttr *pview_attr, tGrapListerPriv *plst_privattr)
{
	tGRAPLIST *pList;

	pList = (tGRAPLIST *)malloc(sizeof(tGRAPLIST));
	if (!pList)
	{
		return NULL;
	}
	memset(pList,0,sizeof(tGRAPLIST));
	pList->view_attr = *pview_attr;
	pList->lst_privattr = *plst_privattr;
	
	pList->draw = Grap_ListerDraw;
	pList->handle = Grap_ListerHandle;
	pList->updateList = Grap_ListerUpdateItem;
	pList->getListString = Grap_ListerGetString;
	pList->enter = Grap_ListerEnterItem;
	pList->destroy = Grap_ListerDistroy;
	pList->listtimer = Grap_ListerTimer;
	pList->drawIcon = Grap_ListDrawIcon;
	pList->changeItem = Grap_ChangeItem;
	pList->keyInput   = Grap_ListKeyInput;
	pList->recvSysMsg = Grap_ListerRecvSysMsg;
	pList->recvPullMsg = Grap_ListerRecvPullMsg;
	
	pList->ConfirmAndEnter = 1; //Ĭ�ϵ����������
	return pList;
}


tGRAPLIST *Grap_InsertLister(void *pView, tGrapViewAttr *pview_attr, tGrapListerPriv *plst_privattr)
{
	tGRAPDESKBOX *pParentView;
	tGRAPLIST *pList;

	pParentView = (tGRAPDESKBOX *)pView;
	if (!pParentView) return NULL;
	pList = (tGRAPLIST *)malloc(sizeof(tGRAPLIST));
	if (!pList)
	{
		return NULL;
	}
	memset(pList,0,sizeof(tGRAPLIST));
	pList->pPrev   = pParentView->pTail;
	pParentView->pTail->pNext = (tGRAPVIEW *)pList;
	pParentView->pTail = (tGRAPVIEW *)pList;
	pList->pParent = pParentView;
	
	//pList->pNext = pParentView->pNext;
	//pParentView->pNext = (tGRAPVIEW *)pList;
	//pList->pParent = pParentView;
	
	pList->view_attr = *pview_attr;
	pList->lst_privattr = *plst_privattr;
	
	pList->draw = Grap_ListerDraw;
	pList->handle = Grap_ListerHandle;
	pList->updateList = Grap_ListerUpdateItem;
	pList->getListString = Grap_ListerGetString;
	pList->enter = Grap_ListerEnterItem;
	pList->destroy = Grap_ListerDistroy;
	pList->listtimer = Grap_ListerTimer;
	pList->drawIcon = Grap_ListDrawIcon;
	pList->changeItem = Grap_ChangeItem;
	pList->keyInput   = Grap_ListKeyInput;
	pList->recvSysMsg = Grap_ListerRecvSysMsg;
	pList->recvPullMsg = Grap_ListerRecvPullMsg;
	
	pList->ConfirmAndEnter = 1; //Ĭ�ϵ����������
	return pList;
} 