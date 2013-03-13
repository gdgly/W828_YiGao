#include "grap_edit2.h"
#include "grap_event.h"
#include "grap_deskbox.h"

//��ͷ��ɫ   0x3186
const U8 UpArrow[16]=//w=16 h=8
{
	0x01,0x80,0x03,0xC0,0x07,0xE0,0x0F,0xF0,0x1F,0xF8,0x3F,0xFC,0x7F,0xFE,0xFF,0xFF
};

const U8 DownArrow[16]=//w=16 h=8
{
	0xFF,0xFF,0x7F,0xFE,0x3F,0xFC,0x1F,0xF8,0x0F,0xF0,0x07,0xE0,0x03,0xC0,0x01,0x80
};

/*const U16 Edit2ScrollBarColor[16]=
{
	0xA75F,0x86FF,0x86FF,0x769F,0x661F,0x559F,0x453F,0x34BF,0x1C5E,0x39C,0x39C,0x35A,0x318,0xB7A,
	0x3BB,0x45F 
};*/

//����ȡģ   ��ɫOx632c
const U16 Edit2ScrollBarColor[]=
{
	//0xad55,0xad55,0xa534,0x9cf3,0x9cd3,0x9492,0x8c51,0x8410,0x7bcf,0x73ae,0x6b6d,0x632c,0x5aeb,0x5acb,
	//0x528a,0x4a49,0x4208,0x39e7,0x31a6
	
	/*0x640,0x600,0x5c0,0x580,0x560,0x520,0x4e0,0x4c0,0x480,
    0x440,0x420,0x3e0,0x3a0,0x380,0x340,0x300,0x2e0,0x2a0,
    0x260,*/
    
    //���ɸ軪���
    0x867e,0x865e,0x7e1d,0x75fc,0x75dc,0x6dbb,0x657a,0x655a,0x5d3a,
	0x5cf9,0x54d8,0x4cb8,0x4c97,0x4457,0x4436,0x3c16,0x33d5,0x33b5,
	0x2b94 
};

static void moveCursor(void *pView, U16 x, U16 y);

U16 keyValue[10][5] = 
{
	{'@',	':',	'/',	0,	KEYEVENT_1},
	{'a',	'b',	'c',	0,	KEYEVENT_2},
	{'d',	'e',	'f',	0,	KEYEVENT_3},
	{'g',	'h',	'i',	0,	KEYEVENT_4},
	{'j',	'k',	'l',	0,	KEYEVENT_5},
	{'m',	'n',	'o',	0,	KEYEVENT_6},
	{'p',	'q',	'r',	's',KEYEVENT_7},
	{'t',	'u',	'v',	0,	KEYEVENT_8},
	{'w',	'x',	'y',	'z',KEYEVENT_9},
	{',',	'.',	'?',	'!',KEYEVENT_0}
};

U8  Keycount = 0;
U32 Keylasttime = 0;
U16 Keylastkey = 0xff;

/*
 *���½ڵ���Ϣ
 */
static S32 LineInfo_UpdateNode(tCHARPOS **pHead, U16 LineNo, U32 firstCharPos)
{
	tCHARPOS *pSub;
	
	pSub = *pHead;
	while(pSub != NULL)
	{
		if(pSub->LineNo == LineNo)
		{
			pSub->firstCharPos = firstCharPos;
			break;
		}

		pSub = pSub->pNext;
	}
	
	return 0;
}

/*
 *����һ����¼����Ϣ�Ľڵ�
 */
static S32 LineInfo_InsertNode(tCHARPOS **pHead, tCHARPOS **pTail, U16 LineNo, U32 firstCharPos)
{
	tCHARPOS *pNewNode = NULL, *pSub = NULL;
	
	if(*pTail != NULL && (*pTail)->LineNo >= LineNo)
	{
		LineInfo_UpdateNode(pHead, LineNo, firstCharPos);
	}
	else
	{
		pNewNode = (tCHARPOS *)malloc(sizeof(tCHARPOS));
		if(pNewNode == NULL)
			return -1;//ʧ��
		pNewNode->LineNo = LineNo;
		pNewNode->firstCharPos = firstCharPos;
		pNewNode->pNext = NULL;
		*pTail = pNewNode;
		
		if(*pHead == NULL)
		{
			*pHead = pNewNode;
		}
		else
		{
			pSub = *pHead;
			while(pSub->pNext != NULL)
			{
				pSub = pSub->pNext;
			}
			pSub->pNext = pNewNode;
		}
	}
	
	return 0;
}
/*
 *ɾ��һ����¼����Ϣ�Ľڵ�
 */
static S32 LineInfo_DeleteNode(tCHARPOS **pHead, tCHARPOS **pTail)
{
	tCHARPOS *pTemp;
	
	if(*pHead == NULL)	return 0;
	if(*pHead == *pTail)
	{
		pTemp = *pHead;
		*pHead = NULL;
		*pTail = NULL;
		free((void *)pTemp);
	}
	else
	{
		pTemp = *pHead;
		while(pTemp->pNext != *pTail)
		{
			pTemp = pTemp->pNext;
		}
		pTemp->pNext = NULL;
		free((void *)(*pTail));
		*pTail = pTemp;
	}
	
	return 0;
}

/*
 *����һ����¼����Ϣ�Ľڵ�
 */
static S32 LineInfo_FindNode(tCHARPOS *pHead, U16 LineNo, U32 *firstCharPos)
{
	tCHARPOS *pSub;
	
	*firstCharPos = 0;
	pSub = pHead;
	while(pSub != NULL)
	{
		if(pSub->LineNo == LineNo)
		{
			*firstCharPos = pSub->firstCharPos;
			break;
		}
		pSub = pSub->pNext;
	}
	
	return 0;
}

/*
 *�ͷŸ�����
 */
static void LineInfo_Destroy(tCHARPOS *pHead)
{
	tCHARPOS *pSub, *pTemp;
	
	pSub = pHead;
	while(pSub != NULL)
	{
		pTemp = pSub->pNext;
		free((void *)pSub);
		pSub = pTemp;
	}
	
	return;
}

/*
 *flag:0:ֱ������ָ��λ�� 1:���� -1:����
 */
int moveBar(tGRAPEDIT2 *pEdit, U16 y, int flag)
{
	tEdit2Scroll *pEdit2Scr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	float temp;
	U16 barH, tempH, linenum;
	
	pedit_ctrl = &(pEdit->edit_ctrl);
	pEdit2Scr  = &(pEdit->edit2Scr);
	
	temp = (float)pEdit2Scr->barScrollLen / pedit_ctrl->totLine;//һ��Ӧռ�ĸ߶�
	barH = pEdit2Scr->end_y_pos - pEdit2Scr->start_y_pos;//�����
	
	if(flag == 0)
	{
		if(y > pEdit2Scr->end_y_pos)//�·�
		{
			pEdit2Scr->end_y_pos = y + barH/2;
			if(pEdit2Scr->end_y_pos >= pEdit2Scr->bar_end_y)
			{
				pEdit2Scr->end_y_pos = pEdit2Scr->bar_end_y;
				pEdit2Scr->start_y_pos = pEdit2Scr->end_y_pos - barH;
				pedit_ctrl->topLine_idx = pedit_ctrl->totLine - pedit_ctrl->line_screen;
			}
			else
			{
				pEdit2Scr->start_y_pos = pEdit2Scr->end_y_pos - barH;
				tempH = pEdit2Scr->start_y_pos - pEdit2Scr->bar_start_y;
				linenum = tempH/temp;
				if(linenum+pedit_ctrl->line_screen >= pedit_ctrl->totLine)
	   			{
	   				pedit_ctrl->topLine_idx = pedit_ctrl->totLine - pedit_ctrl->line_screen;
	   			}
	   			else
	   			{
	   				pedit_ctrl->topLine_idx = linenum;
	   			}
			}	
		}
		else //�Ϸ�
		{
			if(y >= barH/2)
			{
				pEdit2Scr->start_y_pos = y - barH/2;
				if(pEdit2Scr->start_y_pos < pEdit2Scr->bar_start_y)
					pEdit2Scr->start_y_pos = pEdit2Scr->bar_start_y;
			}
			else
			{
				pEdit2Scr->start_y_pos = pEdit2Scr->bar_start_y;
			}
			pEdit2Scr->end_y_pos = pEdit2Scr->start_y_pos + barH;
			
			tempH = pEdit2Scr->start_y_pos - pEdit2Scr->bar_start_y;
			linenum = tempH/temp;
			pedit_ctrl->topLine_idx = linenum;
		}
		
		return -1;
	}
	else if(flag == 1)//��
	{
		if(pEdit2Scr->end_y_pos >= pEdit2Scr->bar_end_y)	return -1;

		pEdit2Scr->end_y_pos += y;
		if(pEdit2Scr->end_y_pos >= pEdit2Scr->bar_end_y)
		{
			pEdit2Scr->end_y_pos = pEdit2Scr->bar_end_y;
			pEdit2Scr->start_y_pos = pEdit2Scr->end_y_pos - barH;
			pedit_ctrl->topLine_idx = pedit_ctrl->totLine - pedit_ctrl->line_screen;
		}
		else
		{
			pEdit2Scr->start_y_pos = pEdit2Scr->end_y_pos - barH;
			tempH = pEdit2Scr->start_y_pos - pEdit2Scr->bar_start_y;
			linenum = tempH/temp;
			if(linenum+pedit_ctrl->line_screen >= pedit_ctrl->totLine)
   			{
   				pedit_ctrl->topLine_idx = pedit_ctrl->totLine - pedit_ctrl->line_screen;
   			}
   			else
   			{
   				pedit_ctrl->topLine_idx = linenum;
   			}
		}
	}
	else//��
	{
		if(pEdit2Scr->start_y_pos <= pEdit2Scr->bar_start_y)	return -1;
		if(pEdit2Scr->start_y_pos > y)
		{
			pEdit2Scr->start_y_pos -= y;
			if(pEdit2Scr->start_y_pos < pEdit2Scr->bar_start_y)
				pEdit2Scr->start_y_pos = pEdit2Scr->bar_start_y;
		}
		else
		{
			pEdit2Scr->start_y_pos = pEdit2Scr->bar_start_y;
		}
		pEdit2Scr->end_y_pos = pEdit2Scr->start_y_pos + barH;
			
		tempH = pEdit2Scr->start_y_pos - pEdit2Scr->bar_start_y;
		linenum = tempH/temp;
		pedit_ctrl->topLine_idx = linenum;
	}

	return 0;
}

int Grap_Edit2Enter(void *pView)
{
	
	return SUCCESS;
}

S32 Grap_Edit2GetFocus(void *pView)
{
	return SUCCESS;
}

S32 Grap_Edit2RecvPullMsg(void *pView, GRAP_EVENT *pEvent)
{
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	tEdit2Scroll *pEdit2Scr;
	tGRAPVIEW *pTempView;
	S32 ret = FAIL;
	U16 x,y,lastX, lastY;
	
	y = HIWORD(pEvent->lParam) ;
	x = LOWORD(pEvent->lParam) ;
	lastY = HIWORD(pEvent->wParam) ;
	lastX = LOWORD(pEvent->wParam) ;//�ϴε�
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	pEdit2Scr  = &(pEdit->edit2Scr);
	
	if(pEvent->extParam == VIEW_PULL_START)//��ʼ��
	{
		if(x >= pview_attr->touchX1 && x < (pview_attr->touchX2+pEdit->edit2Scr.bar_end_x) &&
		   y >= pview_attr->touchY1 && y < pview_attr->touchY2 )//�ڸÿռ���
		{ 	
			ret = VIEW_PULL_START;
		}
		else
		{
			return FAIL;
		}
	}
	
	if(VIEW_PULL_START == pEvent->extParam)
	{
		if(pEdit->view_attr.FocusEnable)
		{
			if(!pEdit->view_attr.curFocus)
			{
				Grap_ChangeFocusEnable(pEdit);
				pEdit->get_focus(pEdit);
			}
		}
		
		//������
		if(x >= pview_attr->touchX1 && x < pview_attr->touchX2 &&
	       y >= pview_attr->touchY1 && y < pview_attr->touchY2 )//д����
	    {
	    	if(pedit_privattr->cursor_en)
	    	{
		    	moveCursor(pEdit,x,y);
		    	pedit_ctrl->TimerLft = 1;
				pedit_ctrl->showState = 0;
				pEdit->draw(pEdit, 0, 0);
		    }
	    }
	    else if(x >= pEdit->edit2Scr.bar_start_x && x < pEdit->edit2Scr.bar_end_x
			 && y >= pEdit->edit2Scr.arrow1_areaY1 && y < pEdit->edit2Scr.arrow2_areaY2
			 && pEdit->edit2Scr.need == 1)//������
		{
			if(y <= pEdit->edit2Scr.arrow1_areaY2)//�ϼ�ͷ
			{
				if(pedit_ctrl->cursor_line > 0)
					pedit_ctrl->cursor_line-=1;
			}
			else if(y >= pEdit->edit2Scr.arrow2_areaY1)//�¼�ͷ
			{
				if(pedit_ctrl->cursor_line + 1 < pedit_ctrl->totLine)
					pedit_ctrl->cursor_line+=1;
			}
			else//��������
			{
				if(y >= pEdit->edit2Scr.start_y_pos && y<= pEdit->edit2Scr.end_y_pos)
		   		{
		   			return VIEW_PULL_START ;
		   		}
		   		else
		   		{
		   			moveBar(pEdit,y,0);
		   		}
		   		
		   		pedit_ctrl->cursor_line = pedit_ctrl->topLine_idx;
		   		LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &(pedit_ctrl->cursor_pos));
	   		
				pedit_ctrl->TimerLft = 1;
				pedit_ctrl->showState = 0;
				pEdit->draw(pEdit, 0, 0);
					
				return VIEW_PULL_START ;
			}
			
			if(pedit_ctrl->cursor_line < pedit_ctrl->topLine_idx)//�������
	   		{
	   			pedit_ctrl->topLine_idx = pedit_ctrl->cursor_line;
	   		}
	   		else if(pedit_ctrl->cursor_line - pedit_ctrl->topLine_idx >= pedit_ctrl->line_screen)
	   		{
	   			pedit_ctrl->topLine_idx = pedit_ctrl->cursor_line-pedit_ctrl->line_screen+1;
	   		}
	   		
	   		LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &(pedit_ctrl->cursor_pos));
	   		if(pedit_privattr->cursor_en)
			{
				pedit_ctrl->TimerLft = 1;
				pedit_ctrl->showState = 0;
				pEdit->draw(pEdit, 0, 0);
			}
		}
	}
	else if (VIEW_PULL_ING == pEvent->extParam)
	{//��ק��
		if(pedit_ctrl->totLine <= pedit_ctrl->line_screen)
		{
			return FAIL;
		}
		if(x >= pEdit->edit2Scr.bar_start_x && x < pEdit->edit2Scr.bar_end_x
		&& y >= pEdit->edit2Scr.arrow1_areaY1 && y < pEdit->edit2Scr.arrow2_areaY2
		&& pEdit->edit2Scr.need == 1)
		{

			if(lastX >= pEdit->edit2Scr.bar_start_x && lastX < pEdit->edit2Scr.bar_end_x
			&& lastY >= pEdit->edit2Scr.arrow1_areaY1 && lastY < pEdit->edit2Scr.arrow2_areaY2)
			{
				if(lastY <= pEdit->edit2Scr.arrow1_areaY2 && y <= pEdit->edit2Scr.arrow1_areaY2)//�ϼ�ͷ
				{
					if(pedit_ctrl->cursor_line > 0)
						pedit_ctrl->cursor_line-=1;
				}
				else if(lastY >= pEdit->edit2Scr.arrow2_areaY1 && y >= pEdit->edit2Scr.arrow2_areaY1)//�¼�ͷ
				{
					if(pedit_ctrl->cursor_line + 1 < pedit_ctrl->totLine)
						pedit_ctrl->cursor_line+=1;
				}
				else//��������
				{
					int temp,re=-1;
					
					temp = y - lastY;
					
					if(temp > 0)//��
					{
						re = moveBar(pEdit,ABS(temp),1);
					}
					else//��
					{
						re = moveBar(pEdit,ABS(temp),-1);
					}
			   		
			   		if(re == -1)
			   		{
			   			return FAIL;
			   		}
			   		
			   		pedit_ctrl->cursor_line = pedit_ctrl->topLine_idx;
			   		LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &(pedit_ctrl->cursor_pos));
		   		
					pedit_ctrl->TimerLft = 1;
					pedit_ctrl->showState = 0;
					pEdit->draw(pEdit, 0, 0);
						
					return VIEW_PULL_ING ;
			   	}
			   	
			   	if(pedit_ctrl->cursor_line < pedit_ctrl->topLine_idx)//�������
		   		{
		   			pedit_ctrl->topLine_idx = pedit_ctrl->cursor_line;
		   		}
		   		else if(pedit_ctrl->cursor_line - pedit_ctrl->topLine_idx >= pedit_ctrl->line_screen)
		   		{
		   			pedit_ctrl->topLine_idx = pedit_ctrl->cursor_line-pedit_ctrl->line_screen+1;
		   		}
		   		
		   		LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &(pedit_ctrl->cursor_pos));
		   		
				pedit_ctrl->TimerLft = 1;
				pedit_ctrl->showState = 0;
				pEdit->draw(pEdit, 0, 0);
				
				return VIEW_PULL_ING ;
			}
		} 
		else if(x >= pview_attr->touchX1 && x < (pview_attr->touchX2+pEdit->edit2Scr.bar_end_x) &&
		        y >= pview_attr->touchY1 && y < pview_attr->touchY2 )//�ڸÿռ���
		{
			S16 moveP;
			
			moveP = y - lastY;
	
			if(ABS(moveP) >= pedit_ctrl->line_height)
			{
				if(moveP > 0)//������
				{
					if(pedit_ctrl->topLine_idx > 0)
					{
						pedit_ctrl->topLine_idx--;
						if(pedit_privattr->cursor_en)
						{
							pedit_ctrl->cursor_line--;
							LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line,&(pedit_ctrl->cursor_pos));
							pedit_ctrl->TimerLft = 1;
							pedit_ctrl->showState = 0;
						}
						pEdit->draw(pEdit, 0, 0);
					}
				}
				else//������
				{
					if(pedit_ctrl->topLine_idx+pedit_ctrl->line_screen < pedit_ctrl->totLine)
					{
						pedit_ctrl->topLine_idx++;
						if(pedit_privattr->cursor_en)
						{
							pedit_ctrl->cursor_line++;
							LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line,&(pedit_ctrl->cursor_pos));
							pedit_ctrl->TimerLft = 1;
							pedit_ctrl->showState = 0;
						}
						pEdit->draw(pEdit, 0, 0);
					}
				}
				ret = VIEW_PULL_ING;
			}
		}
	}
	else if (VIEW_PULL_END == pEvent->extParam)
	{//up			
		ret = VIEW_PULL_END;
	}
	else if (VIEW_PULL_TIMEOUT == pEvent->extParam)
	{//��ʱ
	    ret = VIEW_PULL_TIMEOUT;
	}
	
	return ret; 
}


/*�����ɾ�����ػ�*/
void Grap_Edit2Rest_AfterInputOrDel(void *pView, Character_Type_e kLycStr_Type)
{
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	tEdit2Scroll *pEdit2Scr;
	U8 *pCurPageStr = NULL;
	U32 curPos, i;
	U32 charContent;//һ�����ݵ��ַ�
	U32 lineContent;//һҳ���ݵ�����
	U32 kLineNum, kCharNum, totalCharLen;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	
	LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &curPos);
	pCurPageStr = pedit_privattr->digt + curPos;//������������ַ���ʼ���ַ���
	
	pedit_ctrl->totLine = pedit_ctrl->cursor_line;//֮��ÿд��һ���ټ�1,������line_pos
	
	charContent = pedit_ctrl->char_line;
	lineContent = pedit_ctrl->line_screen;
	
	/*�����֮���ַ�������*/
	totalCharLen = Get_StringLength(kLycStr_Type,pCurPageStr);//�ַ�����(UNICODE��ǵ�Ҳ��2��)

	i = 0;
	if(kLycStr_Type == CHARACTER_UNICODE)
	{
		if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return ;
		pedit_ctrl->totLine++;//������1
		while(totalCharLen > 0)
		{
			for(kCharNum=0; kCharNum<charContent; )
			{
				if(totalCharLen <= 0)//�ַ�д��
				{
					if(pedit_ctrl->pLineInfoT->LineNo >= pedit_ctrl->totLine)
						LineInfo_DeleteNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT));
					return ;
				}
				if(pCurPageStr[i] == 0x0D && pCurPageStr[i+1] == 0x0A)
				{
					i+=2;
					curPos += 2;
					totalCharLen -= 2;
					
					if(totalCharLen > 0)
					{
						break;
					}
					else
					{
						if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return ;
						pedit_ctrl->totLine++;//������1
						if(pedit_ctrl->pLineInfoT->LineNo >= pedit_ctrl->totLine)
							LineInfo_DeleteNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT));
						return;
					}
				}
				else if(pCurPageStr[i] > 0x80 && kCharNum+2 <= charContent)//ȫ��
				{
					i+=2;
					curPos += 2;
					kCharNum += 2;
					totalCharLen -= 2;
				}
				else if(pCurPageStr[i] > 0x80 && kCharNum+2 > charContent)//ȫ��
				{
					break;
				}
				else//���
				{
					i+=2;
					curPos += 2;
					kCharNum += 1;
					totalCharLen -= 2;
				}
			}
			if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return ;
			pedit_ctrl->totLine++;//������1
		}
	}
	else//������
	{
		if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return ;
		pedit_ctrl->totLine++;//������1
		while(totalCharLen > 0)
		{
			for(kCharNum=0; kCharNum<charContent; )
			{
				if(totalCharLen <= 0)//�ַ�д��
				{
					if(pedit_ctrl->pLineInfoT->LineNo >= pedit_ctrl->totLine)
						LineInfo_DeleteNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT));
					return ;
				}
				if(pCurPageStr[i] == 0x0D && pCurPageStr[i+1] == 0x0A)
				{
					i+=2;
					curPos += 2;
					totalCharLen -= 2;
					
					if(totalCharLen > 0)
					{
						break;
					}
					else
					{
						if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return ;
						pedit_ctrl->totLine++;//������1
						if(pedit_ctrl->pLineInfoT->LineNo >= pedit_ctrl->totLine)
							LineInfo_DeleteNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT));
						return;
					}
				}
				else if(pCurPageStr[i] > 0x80 && kCharNum+2 <= charContent)//ȫ��
				{
					i+=2;
					curPos += 2;
					kCharNum += 2;
					totalCharLen -= 2;
				}
				else if(pCurPageStr[i] > 0x80 && kCharNum+2 > charContent)//ȫ��
				{
					break;
				}
				else//���
				{
					i+=1;
					curPos += 1;
					kCharNum += 1;
					totalCharLen -= 1;
				}
			}
			if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return ;
			pedit_ctrl->totLine++;//������1
		}
	}
	if(pedit_ctrl->pLineInfoT->LineNo >= pedit_ctrl->totLine)
		LineInfo_DeleteNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT));
	
	return ;
}

static U16 Grap_Edit2_MoveCursor(U8 *pStr, U16 len, Character_Type_e kLycStr_Type)
{
	U16 i, j = 0;
	
	for(i=0; i<len; )
	{
		if(pStr[i] > 0x80 || kLycStr_Type == CHARACTER_UNICODE)
		{
			i+=2; j = 2;
		}
		else
		{
			i+=1; j = 1;
		}
	}
	
	return j;
}


/*ɾ�����ǰ���ַ�*/
S32 Grap_Edit2_DeleteChar(void *pView)
{
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	U32 curPos, strLen, tempPos;
	U16 len, movenum = 0;
	U8 *temp = NULL, *pStr = NULL;
	Character_Type_e kLycStr_Type;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	
	kLycStr_Type = pedit_privattr->font_type;//�ַ�����
	
	temp = (U8 *)malloc(pedit_privattr->containLen);
	if (NULL == temp) return -1;
	memset(temp, 0, pedit_privattr->containLen);
	
	strcpy(temp, pedit_privattr->digt + pedit_ctrl->cursor_pos);//��������ַ�
	
	LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &curPos);
	pStr = pedit_privattr->digt + curPos;//������������ַ���ʼ���ַ���
	
	LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line+1, &tempPos);
	/*Ҫ�ֹ����һ��β��������һ��ͷ��*/
	if(pedit_ctrl->cursor_pos == tempPos)//�������β��
	{
		len = pedit_ctrl->cursor_pos - curPos;//����뱾�п�ͷ�ľ���
	}
	else if(pedit_ctrl->cursor_pos == curPos)//�������ͷ��
	{//��ʱ�����Ҫ���д���(��������ڵ�2��),��ʱ��Ҫ�ж��Ƿ���Ҫ��ҳ
		pedit_ctrl->cursor_line -= 1;//�������
		if(pedit_ctrl->topLine_idx > pedit_ctrl->cursor_line)//��Ҫ���Ϸ�
		{
			pedit_ctrl->topLine_idx = pedit_ctrl->cursor_line;
		}
		LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &curPos);
		pStr = pedit_privattr->digt + curPos;//������������ַ���ʼ���ַ���
		len = pedit_ctrl->cursor_pos - curPos;//����뱾�еľ���
	}
	else//���������
	{
		len = pedit_ctrl->cursor_pos - curPos ;//����뱾�еľ���
	}
	strLen = Get_StringLength(kLycStr_Type,pStr);//�ַ�����(UNICODE��ǵ�Ҳ��2��)
	
	if(pedit_privattr->digt[pedit_ctrl->cursor_pos-2]==0x0D
		&& pedit_privattr->digt[pedit_ctrl->cursor_pos-1]==0x0A)
	{
		movenum = 2;
	}
	else
	{
		movenum = Grap_Edit2_MoveCursor(pStr,len,kLycStr_Type);
	}
	pedit_ctrl->cursor_pos -= movenum;//���ǰ��
	strcpy(pedit_privattr->digt + pedit_ctrl->cursor_pos,temp);//��֮ǰ������ַ�copy��ȥ
	
	free((void *)temp);
	if(pedit_ctrl->topLine_idx > 0 && pedit_ctrl->topLine_idx+pedit_ctrl->line_screen > pedit_ctrl->cursor_line+1)
	{
		pedit_ctrl->topLine_idx-=1;
	}
	Grap_Edit2Rest_AfterInputOrDel(pEdit, kLycStr_Type);
	return 0;
}

/*��ȡĬ�ϵ��ַ���----������,����ֻ�ǲ�����*/
S32 Grap_Edit2GetDefaultStr(void *pView)
{
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	Character_Type_e kLycStr_Type;
	S32 curPos=0,totalCharLen;
	U8  *pCurPageStr;
	U32 charContent;//һ�����ݵ��ַ�
	U32 lineContent;//һҳ���ݵ�����
	U32 kLineNum, kCharNum;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	
	if(pedit_ctrl->pLineInfoH != NULL)
	{
		LineInfo_Destroy(pedit_ctrl->pLineInfoH);
		pedit_ctrl->pLineInfoH = NULL;
		pedit_ctrl->pLineInfoT = pedit_ctrl->pLineInfoH;
	}
	
	kLycStr_Type = pedit_privattr->font_type;//�ַ�����
	
	totalCharLen = Get_StringLength(kLycStr_Type,pedit_privattr->digt);//�ַ�����(UNICODE��ǵ�Ҳ��2��)

	charContent = pedit_ctrl->char_line;
	lineContent = pedit_ctrl->line_screen;
	pedit_ctrl->topLine_idx = 0;
	pedit_ctrl->totLine = 0;
	pedit_ctrl->cursor_pos = 0;
	pedit_ctrl->cursor_line = 0;
	pCurPageStr = pedit_privattr->digt;
	/*��Ϊ�ǲ���,����ֻ��������*/
	/*2008-0809���ӻس�����*/
	if(kLycStr_Type == CHARACTER_UNICODE)
	{
		//......
	}
	else//������
	{
		if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return -1;
		pedit_ctrl->totLine++;
		while(totalCharLen > 0)
		{
			for(kCharNum=0; kCharNum<charContent; )
			{
				if(totalCharLen <= 0)//�ַ�д��
				{
					return 0;
				}
				if(pCurPageStr[curPos] == 0x0D && pCurPageStr[curPos+1] == 0x0A)//�س�����
				{
					curPos += 2;
					totalCharLen -= 2;
					
					if(totalCharLen > 0)
					{
						break;
					}
					else
					{
						if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return -1;
						pedit_ctrl->totLine++;//������1
						return;
					}
				}
				else if(pCurPageStr[curPos] == 0x0d || pCurPageStr[curPos] == 0x0a)
				{
					curPos += 1;
					totalCharLen -= 1;
					
					if(totalCharLen > 0)
					{
						break;
					}
					else
					{
						if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return -1;
						pedit_ctrl->totLine++;//������1
						return;
					}
				}
				else if(pCurPageStr[curPos] > 0x80 && kCharNum+2 <= charContent)//ȫ��
				{
					curPos += 2;
					kCharNum += 2;
					totalCharLen -= 2;
				}
				else if(pCurPageStr[curPos] > 0x80 && kCharNum+2 > charContent)//ȫ��
				{
					break;
				}
				else//���
				{
					curPos += 1;
					kCharNum += 1;
					totalCharLen -= 1;
				}
			}
			if(-1==LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),pedit_ctrl->totLine,curPos))	return -1;
			pedit_ctrl->totLine++;//������1
		}
	}
	return 0;
}

S32 Grap_Edit2RecvSysMsg(void *pView)
{

	return SUCCESS;
}
/*****************************************************/
void Grap_Edit2CursorShow(void *pView)
{
	int i ;
	U16 date;
	S32 ret;
	U16 color,x1,y1;
	U32 pos;   //x�����pos
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	U16 font_H, font_W;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);

	LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &pos);
	pos = pedit_ctrl->cursor_pos - pos;
	
	font_H = pedit_ctrl->line_height-4;
	font_W = font_H/2;
	
	y1 = pview_attr->viewY1 + ( (pedit_ctrl->cursor_line-pedit_ctrl->topLine_idx) * pedit_ctrl->line_height) + 1;
	x1 = pview_attr->viewX1 + (pos* font_W)+pedit_privattr->xoffset;
	y1 += pedit_privattr->yoffset;
	if (pedit_privattr->cursor_en)
	{
		if(pedit_ctrl->showState)	
		{
			color = COLOR_BLACK;		
		}
		else//Ӧ���ñ���ɫ
		{
			color = COLOR_WHITE;		
		}
		Grap_BrushScreen(x1, y1, (U16)(x1+1), (U16)(y1+font_H-1), color);
	}
}

//��ָ����
S32 Grap_Edit2DrawOneLine(void *pView, U32 line_idx, U16 x_start, U16 y_start)
{
	tGRAPEDIT2 *pEdit;
	tGRAPEDIT2CTRL *pedit_ctrl;
	tGrapEdit2Priv *pedit_privattr;
	tGrapViewAttr  *pview_attr;
	U32 totalCharLen, pos, tempPos;      //Ҫ��ʾ�����ַ����ַ����е�λ��
	tStrDispInfo disp_info;
	Character_Type_e kLycStr_Type;
	U32 curDrawLen;
	U8  *pTempStr;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	
	LineInfo_FindNode(pedit_ctrl->pLineInfoH, line_idx, &pos);
	
	if(line_idx == pedit_ctrl->totLine-1)//���һ��
	{
		kLycStr_Type = pedit_privattr->font_type;//�ַ�����
		totalCharLen = Get_StringLength(kLycStr_Type,(pedit_privattr->digt+pos));//�ַ�����(UNICODE��ǵ�Ҳ��2��)
		curDrawLen = totalCharLen;
	}
	else
	{
		LineInfo_FindNode(pedit_ctrl->pLineInfoH, line_idx+1, &tempPos);
		curDrawLen = tempPos - pos;
	}
	pTempStr = (U8 *)malloc(curDrawLen+2);
	if(NULL == pTempStr) return -1;
	
	memcpy(pTempStr,(U8*)(&pedit_privattr->digt[pos]),curDrawLen);
	pTempStr[curDrawLen] = 0;
	pTempStr[curDrawLen+1] = 0;
	disp_info.string = pTempStr;
	disp_info.color = pedit_privattr->color;
	disp_info.font_size = pedit_privattr->font_size;
	disp_info.font_type = pedit_privattr->font_type;
	disp_info.flag = pedit_privattr->position;
	disp_info.keyStr = NULL;
	disp_info.keyStrColor=COLOR_YELLOW;
	Grap_WriteString(x_start+pedit_privattr->xoffset, y_start+pedit_privattr->yoffset, 
				(U16)(x_start+pview_attr->viewWidth),
				(U16)(y_start + pview_attr->viewHeight), &disp_info);
	free((void *)pTempStr);
	return SUCCESS;
}

// Edit draw function
S32 Grap_Edit2Draw(void *pView, U32 xy, U32 wh)
{
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	tEdit2Scroll   *pEdit2Scr;
	U32 i = 0;
	U32 line;  //һ����ʾ������
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	pEdit2Scr  = &(pEdit->edit2Scr);
	
	//clear edit screen
	if (pview_attr->clNormal)
	{
		Grap_ShowPicture(pview_attr->clNormal, pview_attr->viewX1, pview_attr->viewY1);
	}
	else if(pedit_privattr->coverPicId)
	{
		Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
	}
	else
	{
		Grap_BrushScreen(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight), pview_attr->color);
	}
	
	if(1)
	{
		if(pedit_ctrl->line_screen < pedit_ctrl->totLine)//��Ҫ��������
		{
			float temp;
			U16 barH, barW;
			U16 row,column,i,j;
			
			pEdit2Scr->need = 1;
			
			/*����������ͼ*/
			Grap_BrushScreen(pEdit2Scr->bar_start_x, pEdit2Scr->bar_start_y, 
						pEdit2Scr->bar_end_x,pEdit2Scr->bar_end_y, pEdit2Scr->barBGColor);
			/*�ϼ�ͷ��ͼ*/
			Grap_BrushScreen(pEdit2Scr->arrow1_areaX1, pEdit2Scr->arrow1_areaY1, 
						pEdit2Scr->arrow1_areaX2,pEdit2Scr->arrow1_areaY2, pEdit2Scr->barBGColor);
			/*�¼�ͷ��ͼ*/
			Grap_BrushScreen(pEdit2Scr->arrow2_areaX1, pEdit2Scr->arrow2_areaY1, 
						pEdit2Scr->arrow2_areaX2,pEdit2Scr->arrow2_areaY2, pEdit2Scr->barBGColor);
			/*�ϼ�ͷ*/
			Grap_WriteBMPColor(pEdit2Scr->arrow1_areaX1,pEdit2Scr->arrow1_areaY1+1,16,8, (U8*)UpArrow, ARROWCOLOR);
			/*�¼�ͷ*/
			Grap_WriteBMPColor(pEdit2Scr->arrow2_areaX1,pEdit2Scr->arrow2_areaY1+1,16,8, (U8*)DownArrow, ARROWCOLOR);

			
			//temp = (float)pview_attr->viewHeight / pedit_ctrl->totLine;//һ��Ӧռ�ĸ߶�
			temp = (float)pEdit2Scr->barScrollLen / pedit_ctrl->totLine;//һ��Ӧռ�ĸ߶�
			//barH = pview_attr->viewHeight - temp*(pedit_ctrl->totLine - pedit_ctrl->line_screen);
			barH = pEdit2Scr->barScrollLen - temp*(pedit_ctrl->totLine - pedit_ctrl->line_screen);
			if(barH <= 2)	barH = 2;
			barW = pEdit2Scr->bar_end_x - pEdit2Scr->bar_start_x;
			pEdit2Scr->start_y_pos = pEdit2Scr->bar_start_y + (U16)(pedit_ctrl->topLine_idx*temp);
			pEdit2Scr->end_y_pos = pEdit2Scr->start_y_pos + barH;
			if(pEdit2Scr->end_y_pos > pEdit2Scr->bar_end_y)
			{
				pEdit2Scr->end_y_pos = pEdit2Scr->bar_end_y;
				barH = pEdit2Scr->end_y_pos - pEdit2Scr->start_y_pos;
			}
			row  = pEdit2Scr->start_y_pos;
			column = pEdit2Scr->bar_start_x;
			for(i=0; i < barW; i++)
			{
				Memory_SetRowColumn(row, column++, barH, 1);
				
				for(j=0; j < barH; j++)
				{
					if(j == 0 || j == barH-1)	drvLcd_WritePoint(0x632c);
					else drvLcd_WritePoint(Edit2ScrollBarColor[i]);
				}
			}
		}
		else
		{
			pEdit2Scr->need = 0;
		}
	}
	//draw lines  from top line
	for ( i = pedit_ctrl->topLine_idx; i < pedit_ctrl->topLine_idx + pedit_ctrl->line_screen; i++ )
	{
		if ( i < pedit_ctrl->totLine )
		{
			pEdit->edit2DrawOneLine(pView, i, pview_attr->viewX1, 
					pview_attr->viewY1 + (i-pedit_ctrl->topLine_idx)*pedit_ctrl->line_height);
		}
	}
	
	//����ǵ�ǰ����,�������滭�߿�
	//Grap_DrawFocusRect(pview_attr, COLOR_RED);
	if(pEdit2Scr->need == 1)
	{
		if(pview_attr->FocusEnable && pview_attr->curFocus)
		{
			drvLcd_SetColor(COLOR_RED,0x0);
			drvLcd_Rectangle(pview_attr->viewY1, pview_attr->viewX1, pview_attr->viewHeight, pview_attr->viewWidth+18, 1);
		}
	}
	else
	{
		if(pview_attr->FocusEnable && pview_attr->curFocus)
		{
			drvLcd_SetColor(COLOR_RED,0x0);
			drvLcd_Rectangle(pview_attr->viewY1, pview_attr->viewX1, pview_attr->viewHeight, pview_attr->viewWidth, 1);
		}
	}
	
	return SUCCESS;
}


static void moveCursor(void *pView, U16 x, U16 y)
{
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	Character_Type_e kLycStr_Type;
	int pos ; //
	U32 new_line; //��ǰ���괦�ڵ�����
	U32 len, templen; //temp variable
	U8 *pStr;
			
			
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);

	kLycStr_Type = pedit_privattr->font_type;//�ַ�����
	
	
	//��y��Ӧ����
	new_line = (y - pview_attr->viewY1) / pedit_ctrl->line_height;//��ǰҳ�ڼ���
	if(new_line >= pedit_ctrl->line_screen && pedit_ctrl->topLine_idx+pedit_ctrl->line_screen < pedit_ctrl->totLine)
	{
		new_line-=1;
		pedit_ctrl->topLine_idx+=1;
	}
	new_line += pedit_ctrl->topLine_idx;//�ܵĵڼ���
	if(new_line >= pedit_ctrl->totLine)
	{
		if(pedit_ctrl->totLine > 0)
		{
			pedit_ctrl->cursor_line = pedit_ctrl->totLine-1;
			LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &pos);
			pStr = pedit_privattr->digt + pos;//���һ���ַ�
			templen = (U16)Get_StringLength(kLycStr_Type,pStr);
			if(templen>=2 && pStr[templen-2]==0x0D && pStr[templen-1]==0x0A)
				templen-=2;
			pedit_ctrl->cursor_pos = templen + pos;
		}
		else
		{
			pedit_ctrl->cursor_line = 0;
			pedit_ctrl->cursor_pos = 0;
		}

		return ;
	}
	pedit_ctrl->cursor_line = new_line;
	LineInfo_FindNode(pedit_ctrl->pLineInfoH,new_line, &pos);
	pStr = pedit_privattr->digt + pos;
	len = (x - pview_attr->viewX1)/((pedit_ctrl->line_height-4)/2);
	
	if(new_line == pedit_ctrl->totLine-1)//���һ��
	{
		templen = (U16)Get_StringLength(kLycStr_Type,pStr);
	}
	else//�м�ĳ��
	{
		LineInfo_FindNode(pedit_ctrl->pLineInfoH,new_line+1, &templen);
		templen -= pos;//�����ַ���
	}
	
	if(len > templen)
	{
		if(templen>=2 && pStr[templen-2]==0x0D && pStr[templen-1]==0x0A)
			templen-=2;
		pedit_ctrl->cursor_pos = templen + pos;
	}
	else
	{
		int j=0;
		for(templen=0; templen<len; )
		{
			if(pStr[templen]==0x0D && pStr[templen+1]==0x0A)
			{
				len = templen;
				j=0;
				break;
			}
			else if(pStr[templen]>0x80 && templen+2<=len)
			{
				templen+=2;
				j=0;
			}
			else if(pStr[templen]>0x80 && templen+2>len)
			{
				j = 1;
				break;
			}
			else
			{
				templen+=1;
			}
		}
		len -= j;

		pedit_ctrl->cursor_pos = len + pos;
	}
	
	return;
}


//����-1: ʧ�� 0:�������ַ�  1:Ӧ�滻֮ǰ����
S8 KeyInputChar(U32 *inputkey)
{
	U8	inputMode;
	S8  ret = -1;
	U32 i,key;
	
	key = *inputkey;
	inputMode = Grap_GetCurDesk_InputMode();
	//hyUsbPrintf("inputMode  == %d \r\n",inputMode);
	if(inputMode == 1 || inputMode == 4)//����  ��д 
	{
		key = key - KEYEVENT_0 + 0x30;
		*inputkey = key;
		return 0;
	}
	
	for(i = 0; i < 10; i++)
	{
		if(keyValue[i][4] == key)
		{
			break;
		}
	}
	if(i == 10)//����
		return ret;
	
	if(key == Keylastkey && rawtime(NULL)-Keylasttime < 100)//1���ڰ���ͬһ����
	{
		Keycount++;
		if(Keycount >= 4 || keyValue[i][Keycount] == 0)
		{
			Keycount = 0;
		}
		ret = 1;
	}
	else
	{
		Keycount = 0;
		ret = 0;
	}
	Keylastkey  = key;
	Keylasttime = rawtime(NULL);
	
	if(inputMode == 2)//Сд��ĸ
	{
		*inputkey = keyValue[i][Keycount];
	}
	else if(inputMode == 3)//��д��ĸ
	{
		if(i >= 1 && i <= 8)//��ĸ ��Ҫת�ɴ�д
		{
			*inputkey = keyValue[i][Keycount]-0x20;
		}
		else
		{
			*inputkey = keyValue[i][Keycount];
		}
		hyUsbPrintf("key = %c \r\n", *inputkey);
	}
	
	return ret;
}

U32 Grap_Edit2KeyInput(void *pView, U32 *keycode)
{
	
	tGRAPEDIT2		*pEdit;
	tGrapViewAttr  	*pview_attr;
	tGrapEdit2Priv 	*pedit_privattr;
	tGRAPEDIT2CTRL 	*pedit_ctrl;
	U32 i,key;
	S8  ret;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);	
	
	key = *keycode;
	
	switch(key)
	{
	case KEYEVENT_0://0--9
	case KEYEVENT_1:
	case KEYEVENT_2:
	case KEYEVENT_3:
	case KEYEVENT_4:
	case KEYEVENT_5:
	case KEYEVENT_6:
	case KEYEVENT_7:
	case KEYEVENT_8:
	case KEYEVENT_9:
		ret = KeyInputChar(keycode);
		if(ret == -1)//����
		{
			return PARSED;
		}
		else if(ret == 0)//��һ������
		{
			return SUCCESS;
		}
		else
		{
			pedit_privattr->digt[pedit_ctrl->cursor_pos-1] = *keycode;
			pEdit->draw(pEdit,0,0);
			return FAIL;
		}
		break;
	case KEYEVENT_STAR://*  ��������   (��������'.'����)
		*keycode = '.';
		Keylastkey  = key;
		Keylasttime = rawtime(NULL);
		return SUCCESS;
		break;
	case KEYEVENT_UP:
		Keylastkey  = key;
		Keylasttime = rawtime(NULL);
		//Grap_ChangeFocus(pEdit,0);
		/*if(pedit_ctrl->cursor_line > 0)
			pedit_ctrl->cursor_line-=1;
   		
   		LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &(pedit_ctrl->cursor_pos));
   		if(pedit_privattr->cursor_en)
		{
			pedit_ctrl->TimerLft = 1;
			pedit_ctrl->showState = 0;
			//pEdit->draw(pEdit, 0, 0);
		}*/
		
		return FAIL;
		break;
	case KEYEVENT_DOWN:
		Keylastkey  = key;
		Keylasttime = rawtime(NULL);
		
		/*if(pedit_ctrl->cursor_line + 1 < pedit_ctrl->totLine)
			pedit_ctrl->cursor_line+=1;
   		
   		LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &(pedit_ctrl->cursor_pos));
   		if(pedit_privattr->cursor_en)
		{
			pedit_ctrl->TimerLft = 1;
			pedit_ctrl->showState = 0;
			//pEdit->draw(pEdit, 0, 0);
		}*/
		//hyUsbPrintf("edit2 cursor_line = %d \r\n",pedit_ctrl->cursor_line);
		//Grap_ChangeFocus(pEdit,1);
		return FAIL;
		break;
	case KEYEVENT_LEFT:
		Keylastkey  = key;
		Keylasttime = rawtime(NULL);
		return FAIL;
		break;
	case KEYEVENT_RIGHT:
		Keylastkey  = key;
		Keylasttime = rawtime(NULL);
		return FAIL;
		break;
	case KEYEVENT_DELETE:
		if(pedit_ctrl->cursor_pos > 0)//���ǰ���ַ�,���Խ���ɾ��
		{
			Grap_Edit2_DeleteChar(pEdit);
		
			pedit_ctrl->TimerLft = 1;
			pedit_ctrl->showState = 0;	
			pEdit->draw(pEdit, 0, 0);
		}
		Keylastkey  = key;
		Keylasttime = rawtime(NULL);
		break;
	default:
		Keylastkey  = 0xff;
		Keylasttime = rawtime(NULL);
		return FAIL;
		break;
	}
	
	return FAIL;
}

// default handle
S32 Grap_Edit2Handle(void *pView, GRAP_EVENT *pEvent)
{
	U32 message, wParam, lParam ;
	S32 len ,ret = SUCCESS;
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	Character_Type_e kLycStr_Type;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);

	kLycStr_Type = pedit_privattr->font_type;//�ַ�����
	
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	
	if(message == VIEW_PRESS && pview_attr->curFocus && pEdit->edit2KeyInput != NULL)
	{
		U32 inputret;
		
		inputret = pEdit->edit2KeyInput(pEdit,&lParam);
		if(PARSED == inputret)
		{
			pEdit->draw(pEdit,0,0);
			return PARSED;
		}
		else if(inputret == FAIL)
		{
		}
		else if(lParam != 0xff)
		{
			message = VIEW_INPUT;
			ret = PARSED;
		}
	}
	
	switch(message)
	{
	case VIEW_INIT:
		pEdit->get_default_str(pView);
		break;
	case VIEW_DRAW:
		pEdit->draw(pEdit, 0, 0);
		break;
	case VIEW_INPUT: //�ڹ�����ڴ�����һ���ַ�
	case VIEW_DIGIT:
		len = strlen(pedit_privattr->digt);
		if (len < pedit_privattr->containLen)
		{
			U8 *temp = NULL;
			U8 *pStr = NULL;
			U32 pos = 0, num = 0, i;
			U8 hCode = 0, lCode = 0;

			lCode = (U8)(lParam&0xff) ;
			hCode = (U8)((lParam>>8)&0xff) ;
			
			temp = (U8 *)malloc(pedit_privattr->containLen);
			if (!temp) break;
			memset(temp, 0, pedit_privattr->containLen);

			strcpy(temp, pedit_privattr->digt + pedit_ctrl->cursor_pos);//��������ַ�
			
			if(hCode == 0x0D && lCode == 0x0A)//�س�����
			{
				pedit_privattr->digt[pedit_ctrl->cursor_pos++] = hCode;
				pedit_privattr->digt[pedit_ctrl->cursor_pos++] = lCode;
				
				strcpy(pedit_privattr->digt + pedit_ctrl->cursor_pos, temp);
				free((void *)temp);

				pedit_ctrl->cursor_line+=1;
				LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),
											pedit_ctrl->cursor_line,pedit_ctrl->cursor_pos);
				if(pedit_ctrl->cursor_line - pedit_ctrl->topLine_idx >= pedit_ctrl->line_screen)
				{
					pedit_ctrl->topLine_idx+=1;
				}
				Grap_Edit2Rest_AfterInputOrDel(pEdit, kLycStr_Type);
				pedit_ctrl->TimerLft = 1;
				pedit_ctrl->showState = 0;	
				pEdit->draw(pEdit, 0, 0);
				
				break;
			}
			//Ҫ�������������������ڴ��ǲ������һ�����ܲ��ܷ���һ�����֣�������Щ�жϹ���Ƿ��У�
			//������в������һ�У�������ÿ�����ַ���Ҫ���д���
			LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line, &pos);
			pStr = pedit_privattr->digt + pos;  //��ǰ��������ַ�
			pos = pedit_ctrl->cursor_pos - pos; //�����������׵ľ���
			
			pos = pedit_ctrl->char_line - pos;//���й��������ݵ��ַ���
			if(pos == 0)
			{
				pedit_ctrl->cursor_line+=1;
				LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),
											pedit_ctrl->cursor_line,pedit_ctrl->cursor_pos);
				if(pedit_ctrl->cursor_line - pedit_ctrl->topLine_idx >= pedit_ctrl->line_screen)
				{
					pedit_ctrl->topLine_idx+=1;
				}
			}
			else if(pos == 1 && hCode > 0)
			{
				pedit_ctrl->cursor_line+=1;
				LineInfo_InsertNode(&(pedit_ctrl->pLineInfoH),&(pedit_ctrl->pLineInfoT),
											pedit_ctrl->cursor_line,pedit_ctrl->cursor_pos);
				if(pedit_ctrl->cursor_line - pedit_ctrl->topLine_idx >= pedit_ctrl->line_screen)
				{
					pedit_ctrl->topLine_idx+=1;
				}
			}

			if(hCode)
				pedit_privattr->digt[pedit_ctrl->cursor_pos++] = hCode;
			pedit_privattr->digt[pedit_ctrl->cursor_pos++] = lCode;
			strcpy(pedit_privattr->digt + pedit_ctrl->cursor_pos, temp);
			free((void *)temp);

			Grap_Edit2Rest_AfterInputOrDel(pEdit, kLycStr_Type);
			pedit_ctrl->TimerLft = 1;
			pedit_ctrl->showState = 0;	
			pEdit->draw(pEdit, 0, 0);			
		}
		break;
	case VIEW_DELETE:
		if(pedit_ctrl->cursor_pos > 0)//���ǰ���ַ�,���Խ���ɾ��
		{
			Grap_Edit2_DeleteChar(pEdit);
		
			pedit_ctrl->TimerLft = 1;
			pedit_ctrl->showState = 0;	
			pEdit->draw(pEdit, 0, 0);
		}
		break;
	case VIEW_KEY_MSG:
	case VIEW_PRESS:
		if(pEdit->view_attr.curFocus == 1 || pEvent->reserve == 1)//����ʱ��ӦOK��
		{
			switch(lParam)
			{
			case KEYEVENT_OK:
				if(cKEYB_EVENT_RELEASE == wParam)
				{
					ret = pEdit->edit2Enter(pEdit);
					if ( ret== RETURN_REDRAW)
					{
						ReDraw(pEdit, 0, 0);
						return PARSED;
					}
					else if(ret == RETURN_QUIT)
					{
						*((tGRAPDESKBOX *)(pEdit->pParent))->pQuitCnt = 1;
						pEdit->draw(pEdit, 0, 0);
					}
				}
				return ret;
				break;
			case KEYEVENT_DELETE:
				if(pedit_ctrl->cursor_pos > 0)//���ǰ���ַ�,���Խ���ɾ��
				{
					Grap_Edit2_DeleteChar(pEdit);
				
					pedit_ctrl->TimerLft = 1;
					pedit_ctrl->showState = 0;	
					pEdit->draw(pEdit, 0, 0);
				}
				return PARSED;
				break;
			case KEYEVENT_UP:
				if(pedit_ctrl->topLine_idx > 0)
				{
					pedit_ctrl->topLine_idx--;
					if(pedit_privattr->cursor_en)
					{
						pedit_ctrl->cursor_line--;
						LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line,&(pedit_ctrl->cursor_pos));
						pedit_ctrl->TimerLft = 1;
						pedit_ctrl->showState = 0;
					}
					pEdit->draw(pEdit, 0, 0);
				}
				return PARSED;
				break;
			case KEYEVENT_DOWN:
				if(pedit_ctrl->topLine_idx+pedit_ctrl->line_screen < pedit_ctrl->totLine)
				{
					pedit_ctrl->topLine_idx++;
					if(pedit_privattr->cursor_en)
					{
						pedit_ctrl->cursor_line++;
						LineInfo_FindNode(pedit_ctrl->pLineInfoH,pedit_ctrl->cursor_line,&(pedit_ctrl->cursor_pos));
						pedit_ctrl->TimerLft = 1;
						pedit_ctrl->showState = 0;
					}
					pEdit->draw(pEdit, 0, 0);
				}
				return PARSED;
				break;
			default:
				break;
			}
		}
		break;
	case VIEW_DELETEALL:
		if(pedit_ctrl->pLineInfoH != NULL)
		{
			LineInfo_Destroy(pedit_ctrl->pLineInfoH);
			pedit_ctrl->pLineInfoH = NULL;
			pedit_ctrl->pLineInfoT = pedit_ctrl->pLineInfoH;
		}
		memset(pEdit->edit_privattr.digt, 0, pEdit->edit_privattr.containLen+2);
		pedit_ctrl->topLine_idx = 0;
		pedit_ctrl->totLine = 0;
		pedit_ctrl->cursor_pos = 0;
		pedit_ctrl->cursor_line = 0;
		
		pedit_ctrl->TimerLft = pedit_privattr->TimerTot;
		break;
	case VIEW_TIMER:
		if (pview_attr->curFocus == 0) break; 

		if ( pedit_ctrl->TimerLft == 0 )
		{
			pedit_ctrl->TimerLft = pedit_privattr->TimerTot;
			pedit_ctrl->showState = (1 - pedit_ctrl->showState);
			Grap_Edit2CursorShow(pEdit);			
		}
		else
		{
			S32 timeleft;
			timeleft =  pedit_ctrl->TimerLft - lParam;
			
			if (timeleft < 0)  pedit_ctrl->TimerLft = 0;
			else pedit_ctrl->TimerLft = timeleft;
		}
		break;
	case VIEW_CLICK:
		break;
	case VIEW_REV_PULLMSG:
		ret = pEdit->recvPullMsg(pEdit,pEvent);
		break;
	default:
		break;
	}

	return ret;
}

S32 Grap_Edit2Destroy(void *pView)
{
	tGRAPEDIT2 *pEdit;
	
	pEdit = (tGRAPEDIT2*)pView;
	
	if(0 == pEdit->edit_privattr.mallocFlag &&
			pEdit->edit_privattr.digt != NULL )
		free((void*)(pEdit->edit_privattr.digt));
	if(pEdit->edit_ctrl.pLineInfoH != NULL)
	{
		LineInfo_Destroy(pEdit->edit_ctrl.pLineInfoH);
		pEdit->edit_ctrl.pLineInfoH = NULL;
		pEdit->edit_ctrl.pLineInfoT = NULL;
	}
	
	free((void*)pEdit);
	return SUCCESS;
}

tGRAPEDIT2 *Grap_InsertEdit2(void *pView, tGrapViewAttr  *pview_attr, tGrapEdit2Priv *pedit_privattr, U16 barBgColor)
{
	tGRAPDESKBOX *pParentView;
	tGRAPEDIT2 *pEdit;
	U32 lineNum, font_H, font_W;
	
	pParentView = (tGRAPDESKBOX *)pView;
	if (!pParentView) return NULL;
	pEdit = (tGRAPEDIT2 *)malloc(sizeof(tGRAPEDIT2));
	if (!pEdit)
	{
		return NULL;
	}
	memset(pEdit, 0, sizeof(tGRAPEDIT2));
	pEdit->pPrev   = pParentView->pTail;
	pParentView->pTail->pNext = (tGRAPVIEW *)pEdit;
	pParentView->pTail = (tGRAPVIEW *)pEdit;
	pEdit->pParent = pParentView;
	
	//pEdit->pNext = pParentView->pNext;
	//pParentView->pNext = (tGRAPVIEW *)pEdit;
	//pEdit->pParent = pParentView;
	
	pEdit->view_attr = *pview_attr;
	pEdit->edit_privattr = *pedit_privattr;
	
	pEdit->view_attr.reserve |= 0xFF;
	
	if(pEdit->edit_privattr.containLen == 0)//���û�и�ֵ,ȡĬ2k
		pEdit->edit_privattr.containLen = 2048;
	else if(pEdit->edit_privattr.containLen > EDIT2_DATA_SIZE)
		pEdit->edit_privattr.containLen = EDIT2_DATA_SIZE;
	if(0 == pEdit->edit_privattr.mallocFlag)
	{
		pEdit->edit_privattr.digt = (U8 *)malloc(pEdit->edit_privattr.containLen+2);
		memset(pEdit->edit_privattr.digt, 0, pEdit->edit_privattr.containLen+2);
	}
	if(pEdit->edit_privattr.font_size == FONTSIZE24X24)
	{
		font_H = 24;
		font_W = 12;//���ֽ��ַ���
	}
	else if(pEdit->edit_privattr.font_size == FONTSIZE32X32)
	{
		font_H = 32;
		font_W = 16;
	}
	else
	{
		font_H = 16;
		font_W = 8;
	}
	
	pEdit->edit_ctrl.line_height = font_H+4;
	pEdit->edit_ctrl.line_screen = pEdit->view_attr.viewHeight/pEdit->edit_ctrl.line_height;  //һ�������ɵ�����
	pEdit->edit_ctrl.char_line = pEdit->view_attr.viewWidth / font_W;  //һ�������ɵ��ַ���
	
	pEdit->edit_ctrl.topLine_idx = 0;
	pEdit->edit_ctrl.totLine = 0;
	pEdit->edit_ctrl.cursor_pos = 0;
	pEdit->edit_ctrl.cursor_line = 0;
	
	pEdit->edit_ctrl.TimerLft = pedit_privattr->TimerTot;

	pEdit->edit_ctrl.pLineInfoH = NULL;
	pEdit->edit_ctrl.pLineInfoT = pEdit->edit_ctrl.pLineInfoH;
	
	pEdit->edit2Scr.bar_start_x	= pEdit->view_attr.viewX1 + pEdit->view_attr.viewWidth;//�������������ڱ༭����
	pEdit->edit2Scr.bar_end_x	= pEdit->edit2Scr.bar_start_x + SCROLLBARWIDTH/*font_W*2*/;
	
	pEdit->edit2Scr.arrow1_areaX1= pEdit->edit2Scr.bar_start_x;
	pEdit->edit2Scr.arrow1_areaY1= pEdit->view_attr.viewY1;
	pEdit->edit2Scr.arrow1_areaX2= pEdit->edit2Scr.bar_end_x;
	pEdit->edit2Scr.arrow1_areaY2= pEdit->edit2Scr.arrow1_areaY1+SCROLLBARHEIGHT;

	pEdit->edit2Scr.arrow2_areaX2= pEdit->edit2Scr.bar_end_x;
	pEdit->edit2Scr.arrow2_areaY2= pEdit->view_attr.viewY1 + pEdit->view_attr.viewHeight;
	pEdit->edit2Scr.arrow2_areaX1= pEdit->edit2Scr.bar_start_x;
	pEdit->edit2Scr.arrow2_areaY1= pEdit->edit2Scr.arrow2_areaY2-SCROLLBARHEIGHT;
	
	pEdit->edit2Scr.barScrollLen= pEdit->view_attr.viewHeight-2*SCROLLBARHEIGHT-2;
	pEdit->edit2Scr.bar_start_y	= pEdit->edit2Scr.arrow1_areaY2+1;
	pEdit->edit2Scr.bar_end_y	= pEdit->edit2Scr.bar_start_y+pEdit->edit2Scr.barScrollLen;
	
	pEdit->edit2Scr.start_y_pos	= pEdit->edit2Scr.bar_start_y;
	pEdit->edit2Scr.end_y_pos	= pEdit->edit2Scr.bar_start_y;
	
	pEdit->edit2Scr.need	= 0;//Ĭ�ϲ���Ҫ��������
	pEdit->edit2Scr.barBGColor	= 0xB596;//barBgColor;//

	pEdit->draw = Grap_Edit2Draw;
	pEdit->handle = Grap_Edit2Handle;
	pEdit->destroy = Grap_Edit2Destroy;
	pEdit->recvSysMsg = Grap_Edit2RecvSysMsg;
	pEdit->get_default_str = Grap_Edit2GetDefaultStr;
	pEdit->recvPullMsg = Grap_Edit2RecvPullMsg;
	pEdit->get_focus    = Grap_Edit2GetFocus;
	pEdit->edit2KeyInput= Grap_Edit2KeyInput;
	
	pEdit->edit2Enter = Grap_Edit2Enter;
	pEdit->edit2DrawOneLine = Grap_Edit2DrawOneLine;
	
	return pEdit;
}

tGRAPEDIT2 *Grap_CreateEdit2(tGrapViewAttr  *pview_attr, tGrapEdit2Priv *pedit_privattr, U16 barBgColor)
{
	tGRAPEDIT2 *pEdit;
	U32 lineNum, font_H, font_W;
	
	pEdit = (tGRAPEDIT2 *)malloc(sizeof(tGRAPEDIT2));
	if (!pEdit)
	{
		return NULL;
	}
	memset(pEdit, 0, sizeof(tGRAPEDIT2));
	
	pEdit->view_attr = *pview_attr;
	pEdit->edit_privattr = *pedit_privattr;
	
	pEdit->view_attr.reserve |= 0xFF;
	
	if(pEdit->edit_privattr.containLen == 0)//���û�и�ֵ,ȡĬ2k
		pEdit->edit_privattr.containLen = 2048;
	else if(pEdit->edit_privattr.containLen > EDIT2_DATA_SIZE)
		pEdit->edit_privattr.containLen = EDIT2_DATA_SIZE;

	if(0 == pEdit->edit_privattr.mallocFlag)
	{
		pEdit->edit_privattr.digt = (U8 *)malloc(pEdit->edit_privattr.containLen+2);
		memset(pEdit->edit_privattr.digt, 0, pEdit->edit_privattr.containLen+2);
	}

	if(pEdit->edit_privattr.font_size == FONTSIZE24X24)
	{
		font_H = 24;
		font_W = 12;//���ֽ��ַ���
	}
	else if(pEdit->edit_privattr.font_size == FONTSIZE32X32)
	{
		font_H = 32;
		font_W = 16;
	}
	else
	{
		font_H = 16;
		font_W = 8;
	}
	
	pEdit->edit_ctrl.line_height = font_H+4;
	pEdit->edit_ctrl.line_screen = pEdit->view_attr.viewHeight/pEdit->edit_ctrl.line_height;  //һ�������ɵ�����
	pEdit->edit_ctrl.char_line = pEdit->view_attr.viewWidth / font_W;  //һ�������ɵ��ַ���
	
	pEdit->edit_ctrl.topLine_idx = 0;
	pEdit->edit_ctrl.totLine = 0;
	pEdit->edit_ctrl.cursor_pos = 0;
	pEdit->edit_ctrl.cursor_line = 0;
	
	pEdit->edit_ctrl.TimerLft = pedit_privattr->TimerTot;

	pEdit->edit_ctrl.pLineInfoH = NULL;
	pEdit->edit_ctrl.pLineInfoT = pEdit->edit_ctrl.pLineInfoH;
	
	pEdit->edit2Scr.bar_start_x	= pEdit->view_attr.viewX1 + pEdit->view_attr.viewWidth;//�������������ڱ༭����
	pEdit->edit2Scr.bar_end_x	= pEdit->edit2Scr.bar_start_x + SCROLLBARWIDTH/*font_W*2*/;
	
	pEdit->edit2Scr.arrow1_areaX1= pEdit->edit2Scr.bar_start_x;
	pEdit->edit2Scr.arrow1_areaY1= pEdit->view_attr.viewY1;
	pEdit->edit2Scr.arrow1_areaX2= pEdit->edit2Scr.bar_end_x;
	pEdit->edit2Scr.arrow1_areaY2= pEdit->edit2Scr.arrow1_areaY1+SCROLLBARHEIGHT;

	pEdit->edit2Scr.arrow2_areaX2= pEdit->edit2Scr.bar_end_x;
	pEdit->edit2Scr.arrow2_areaY2= pEdit->view_attr.viewY1 + pEdit->view_attr.viewHeight;
	pEdit->edit2Scr.arrow2_areaX1= pEdit->edit2Scr.bar_start_x;
	pEdit->edit2Scr.arrow2_areaY1= pEdit->edit2Scr.arrow2_areaY2-SCROLLBARHEIGHT;
	
	pEdit->edit2Scr.barScrollLen= pEdit->view_attr.viewHeight-2*SCROLLBARHEIGHT-2;
	pEdit->edit2Scr.bar_start_y	= pEdit->edit2Scr.arrow1_areaY2+1;
	pEdit->edit2Scr.bar_end_y	= pEdit->edit2Scr.bar_start_y+pEdit->edit2Scr.barScrollLen;
	
	pEdit->edit2Scr.start_y_pos	= pEdit->edit2Scr.bar_start_y;
	pEdit->edit2Scr.end_y_pos	= pEdit->edit2Scr.bar_start_y;
	
	pEdit->edit2Scr.need	= 0;//Ĭ�ϲ���Ҫ��������
	pEdit->edit2Scr.barBGColor	= barBgColor;//0xB596

	pEdit->draw = Grap_Edit2Draw;
	pEdit->handle = Grap_Edit2Handle;
	pEdit->destroy = Grap_Edit2Destroy;
	pEdit->recvSysMsg = Grap_Edit2RecvSysMsg;
	pEdit->get_default_str = Grap_Edit2GetDefaultStr;
	pEdit->recvPullMsg = Grap_Edit2RecvPullMsg;
	pEdit->get_focus    = Grap_Edit2GetFocus;
	pEdit->edit2KeyInput= Grap_Edit2KeyInput;
	
	pEdit->edit2Enter = Grap_Edit2Enter;
	pEdit->edit2DrawOneLine = Grap_Edit2DrawOneLine;
	
	return pEdit;
}
