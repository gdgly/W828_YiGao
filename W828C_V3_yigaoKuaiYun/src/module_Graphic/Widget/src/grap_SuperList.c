#include "grap_SuperList.h"
#include "grap_event.h"
#include "grap_deskbox.h"


const U16 ScrollBarColor[19]=
{
	//0xD69A,0xD69A,0xD69A,0xCE59,0xC618,0xB5B6,0xA534,0x9CD3,
	//0x9492,0x8C51,0x8410,0x73AE,0x6B6D,0x632C,0x5ACB,0x528A,
	//0x4A49,0x4208,0x31A6
	0x640,0x600,0x5c0,0x580,0x560,0x520,0x4e0,0x4c0,0x480,
    0x440,0x420,0x3e0,0x3a0,0x380,0x340,0x300,0x2e0,0x2a0,
    0x260,
};


void ItemInfo_Destroy(tItemInfo *pInfo)
{
	tItemInfo *pTemp = NULL;
	
	pTemp = pInfo;
	while(pTemp != NULL)
	{
		pInfo = pInfo->pNext;
		pTemp->pNext = NULL;
		free((void*)pTemp);
		pTemp = pInfo;
	}
	return;
}

S32 ItemInfo_Create(tItemInfo **pInfoH,tItemInfo **pInfoT, U16 totalItem)
{
	tItemInfo *pTemp = NULL;
	
	while(totalItem > 0)
	{
		totalItem -= 1;
		pTemp = (tItemInfo *)malloc(sizeof(tItemInfo));
		if(NULL == pTemp)	return -1;
		pTemp->curItemNo = totalItem;
		pTemp->isSelect  = 0;
		pTemp->pNext = NULL;
		if(*pInfoH == NULL)
		{
			*pInfoH = pTemp;
			*pInfoT = pTemp;
		}
		else
		{
			pTemp->pNext = *pInfoH;
			*pInfoH = pTemp;
		}
	}
	
	return 0;
}

S32 ItemInfo_AddNode(tItemInfo **pInfoH,tItemInfo **pInfoT)
{
	tItemInfo *pTemp = NULL;
	U16 itemNo;
	
	if(*pInfoH == NULL || *pInfoT == NULL)//�޽��
	{
		pTemp = (tItemInfo *)malloc(sizeof(tItemInfo));
		if(NULL == pTemp)	return -1;
		pTemp->curItemNo = 0;
		pTemp->isSelect  = 0;
		pTemp->pNext = NULL;
		
		(*pInfoH) = pTemp;
		(*pInfoT) = pTemp;
		
		pTemp = NULL;
		return 0;
	}
	
	itemNo = (*pInfoT)->curItemNo + 1;
	if((*pInfoT)->pNext == NULL)
	{
		pTemp = (tItemInfo *)malloc(sizeof(tItemInfo));
		if(NULL == pTemp)	return -1;
		pTemp->curItemNo = itemNo;
		pTemp->isSelect  = 0;
		pTemp->pNext = NULL;
		
		(*pInfoT)->pNext = pTemp;
		(*pInfoT) = pTemp;
		
		pTemp = NULL;
	}
	else
	{
		(*pInfoT) = (*pInfoT)->pNext;
		(*pInfoT)->curItemNo = itemNo;
		(*pInfoT)->isSelect  = 0;
	}
	
	return 0;
}

S32 ItemInfo_GetCurItemInfo(tItemInfo *pInfoH, U16 curItem)
{
	tItemInfo *pTemp = NULL;
	U16 isSelect = 0;
	
	pTemp = pInfoH;
	while(pTemp != NULL)
	{
		if(pTemp->curItemNo == curItem)
		{
			isSelect = pTemp->isSelect;
			break;
		}
		else
		{
			pTemp = pTemp->pNext;
		}
	}
	
	return isSelect;
}

S32 ItemInfo_ChangeItemInfo(tItemInfo *pInfoH, U16 curItem)
{
	tItemInfo *pTemp = NULL;
	U16 isSelect = 0;
	
	pTemp = pInfoH;
	while(pTemp != NULL)
	{
		if(pTemp->curItemNo == curItem)
		{
			pTemp->isSelect = !pTemp->isSelect;
			isSelect = pTemp->isSelect;
			break;
		}
		else
		{
			pTemp = pTemp->pNext;
		}
	}
	
	return isSelect;
}

S32 ItemInfo_ChangeAllItemInfo(tItemInfo *pInfoH, U16 isSelect)
{
	tItemInfo *pTemp = NULL;
	
	pTemp = pInfoH;
	while(pTemp != NULL)
	{
		pTemp->isSelect = isSelect;
		pTemp = pTemp->pNext;
	}
	
	return 0;
}

S32 ItemInfo_DelNode(tItemInfo **pInfoH,tItemInfo **pInfoT)
{
	tItemInfo *pTemp = NULL, *pTemp1 = NULL;
	
	pTemp = (*pInfoH);
	
	if(*pInfoH == *pInfoT && pTemp->isSelect == 1)
	{
		*pInfoH = NULL;
		*pInfoT = NULL;
		free((void*)pTemp);
		pTemp = NULL;
		
		return 0;
	}
	pTemp1 = pTemp;
	while(pTemp != NULL)
	{
		if(pTemp->isSelect == 1)
		{
			if(pTemp == *pInfoH)
			{
				if(*pInfoH == *pInfoT)
				{
					*pInfoH = NULL;
					*pInfoT = NULL;
					free((void*)pTemp);
					pTemp = NULL;
				}
				else
				{
					*pInfoH = pTemp->pNext;
					pTemp->pNext = NULL;
					free((void*)pTemp);
					pTemp = *pInfoH;
				}
			}
			else if(pTemp == *pInfoT)
			{
				*pInfoT = pTemp1;
				(*pInfoT)->pNext = NULL;
				free((void*)pTemp);
				pTemp = NULL;
			}
			else
			{
				pTemp1->pNext = pTemp->pNext;
				pTemp->pNext = NULL;
				free((void*)pTemp);
				pTemp = pTemp1->pNext;
			}
		}
		else
		{
			pTemp1 = pTemp;
			pTemp = pTemp->pNext;
		}
	}
	
	return 0;
}

S32 ItemInfo_Update(tItemInfo *pInfoH)
{
	tItemInfo *pTemp = NULL;
	U16 totalItem = 0;
	
	pTemp = pInfoH;
	while(pTemp != NULL)
	{
		pTemp->curItemNo = totalItem++;
		pTemp = pTemp->pNext;
	}
	
	return totalItem;
}

void updateScrbar(void *pView)
{
	tSUPPERLIST		*pSupList;
	tSupListPriv	*pSupListPriv;
	tListScr		*pListScr;
	
	pSupList = (tSUPPERLIST *)pView;
	pSupListPriv = &(pSupList->SupList_priv);
	pListScr = &(pSupList->ListScr);
	
	pListScr->barStart = pListScr->scrStartY + (U16)(pSupListPriv->topItemNo*pListScr->ratio);
	pListScr->barEnd = pListScr->barStart + pListScr->barH;
	
	if(pSupListPriv->topItemNo == pSupListPriv->totalItem - pSupListPriv->itemScreenNum)
	{
		pListScr->barEnd = pListScr->scrEndY;
		pListScr->barStart = pListScr->barEnd-pListScr->barH;
	}
	
	return ;
}

void updateInfo(void *pView)
{
	tSUPPERLIST		*pSupList;
	tSupListPriv	*pSupListPriv;
	tListScr		*pListScr;
	tRES_PARA       tRes;
	
	pSupList = (tSUPPERLIST *)pView;
	pSupListPriv = &(pSupList->SupList_priv);
	pListScr = &(pSupList->ListScr);
	
	if (res_load_bmp_info(pSupListPriv->scrBarHeadId, &tRes) < 0) return;
	
	if(pSupListPriv->focusItemNo >= pSupListPriv->totalItem)//���㳬������
	{
		if(pSupListPriv->totalItem > 0)
			pSupListPriv->focusItemNo = pSupListPriv->totalItem-1;
		else
			pSupListPriv->focusItemNo = 0;
	}
	
	if(pSupListPriv->totalItem > pSupListPriv->itemScreenNum)//һ����ʾ����,��Ҫ������
	{
		pSupListPriv->ListInfo |= NEED_SCRBAR;
		if(pSupListPriv->topItemNo + pSupListPriv->itemScreenNum > pSupListPriv->totalItem)//topԽ��
			pSupListPriv->topItemNo = pSupListPriv->totalItem - pSupListPriv->itemScreenNum;
		
		pListScr->barH = (pListScr->scrLen*pSupListPriv->itemScreenNum)/pSupListPriv->totalItem;
		if(pListScr->barH < 10)	pListScr->barH = 10;//��СΪ10�����ظ�
		
		pListScr->ratio = (float)(pListScr->scrLen-pListScr->barH)/(pSupListPriv->totalItem - pSupListPriv->itemScreenNum);
		
		pListScr->barStart = pListScr->scrStartY + (U16)(pSupListPriv->topItemNo*pListScr->ratio);
		pListScr->barEnd = pListScr->barStart + pListScr->barH;
		
		if(pSupListPriv->topItemNo == pSupListPriv->totalItem - pSupListPriv->itemScreenNum)
		{
			pListScr->barEnd = pListScr->scrEndY;
			pListScr->barStart = pListScr->barEnd-pListScr->barH;
		}
		
		pSupList->view_attr.viewWidth = pSupListPriv->titleW - tRes.width;
	}
	else //����Ҫ������
	{
		pSupListPriv->ListInfo &= ~NEED_SCRBAR;
		pSupListPriv->topItemNo = 0;
		pSupList->view_attr.viewWidth = pSupListPriv->titleW;
	}
	
	return ;
}
/*
 *distance �ƶ�����  ��direction == 0ʱֱ���ƶ����ô�
 *direction 0:���ƶ�,1:����, -1:����
 *���� -1:δ��ק 0:��ק
 */
S32 moveScrBar(tSUPPERLIST *pSupList,U16 distance, S16 direction)
{
	tGrapViewAttr	*pView_attr;
	tSupListPriv	*pSupListPriv;
	tListScr		*pListScr;
	U16 tempItem, tempDistance, Num;
	
	pView_attr = &(pSupList->view_attr);
	pSupListPriv = &(pSupList->SupList_priv);
	pListScr = &(pSupList->ListScr);
	
	if(direction == 0)
	{
		if(distance <= pListScr->scrStartY || distance >= pListScr->scrEndY)	return -1;
		if(distance >= pListScr->barStart && distance <= pListScr->barEnd)	return -1;

		if(distance > pListScr->barEnd)//�����·�
		{
			tempDistance = distance - pListScr->barEnd;
			Num = (U16)(tempDistance/pListScr->ratio);
			if( (U16)(Num*pListScr->ratio) < tempDistance)	Num++;
			
			pSupListPriv->topItemNo += Num;
			
			pListScr->barEnd += (U16)(Num*pListScr->ratio);
			if(pListScr->barEnd > pListScr->scrEndY)
			{
				pListScr->barEnd = pListScr->scrEndY;
				pSupListPriv->topItemNo = pSupListPriv->totalItem - pSupListPriv->itemScreenNum;
			}
			pListScr->barStart = pListScr->barEnd - pListScr->barH;
		}
		else //�����Ϸ�
		{
			pListScr->barStart = distance;
			pListScr->barEnd = pListScr->barStart+pListScr->barH;
			
			distance = pListScr->barStart - pListScr->scrStartY;//��Թ������ľ���
		
			tempItem = (U16)(distance/pListScr->ratio);
			
			if(tempItem == pSupListPriv->topItemNo)	return -1;
			
			pSupListPriv->topItemNo = tempItem;
		}
		
		if(pSupListPriv->topItemNo+pSupListPriv->itemScreenNum >= pSupListPriv->totalItem)
		{
			pSupListPriv->topItemNo = pSupListPriv->totalItem - pSupListPriv->itemScreenNum;
			
			pListScr->barEnd = pListScr->scrEndY;
			pListScr->barStart = pListScr->barEnd-pListScr->barH;
		}
		else
		{
			pListScr->barStart = pListScr->scrStartY + (U16)(pSupListPriv->topItemNo*pListScr->ratio);
			pListScr->barEnd = pListScr->barStart+pListScr->barH;
		}

		pSupList->draw(pSupList,0,0);
	}
	else if(direction == 1)//��
	{
		if(pListScr->barEnd >= pListScr->scrEndY)	return -1;
		
		Num = (U16)(distance/pListScr->ratio);
		if( (U16)(Num*pListScr->ratio) < distance)	Num++;
		
		pSupListPriv->topItemNo += Num;
		
		pListScr->barEnd += (U16)(Num*pListScr->ratio);
		if(pListScr->barEnd > pListScr->scrEndY)
		{
			pListScr->barEnd = pListScr->scrEndY;
			pSupListPriv->topItemNo = pSupListPriv->totalItem - pSupListPriv->itemScreenNum;
		}
		pListScr->barStart = pListScr->barEnd - pListScr->barH;
			
		pSupList->draw(pSupList,0,0);
	}
	else if(direction == -1)//��
	{
		if(pListScr->barStart <= pListScr->scrStartY)	return -1;
		
		Num = (U16)(distance/pListScr->ratio);
		if( (U16)(Num*pListScr->ratio) < distance)	Num++;
		
		if(pSupListPriv->topItemNo > Num)
		{
			pSupListPriv->topItemNo -= Num;
			pListScr->barStart -= (U16)(Num*pListScr->ratio);
		}
		else
		{
			pSupListPriv->topItemNo = 0;
			pListScr->barStart = pListScr->scrStartY;
		}

		pListScr->barEnd = pListScr->barStart + pListScr->barH;
		pSupList->draw(pSupList,0,0);
	}
	
	return 0;
}

/********************************************************************************/
//����1 ��ʾ����Ĵ�������key   0 ��ʾδ����
//��ʾ��1��ʼ   ��0���ʾ10�Ŀ�ݼ�        keycode:0xff��ʾ��timer�����
S32 Grap_SupListKeyInput(tSUPPERLIST *pSupLister, U16 keycode)
{
	int ret=0;
	U16 key;
	tSupListPriv *plst_privattr;

	plst_privattr = &(pSupLister->SupList_priv);
//hyUsbPrintf("keycode ==== %d  total = %d \r\n",keycode,plst_privattr->totalItem);
	if(keycode == 0xFF)//��timer����   �ڶ���������ʱδ��
	{
	//hyUsbPrintf("keycode = 0x%02x,  lastkey = 0x%02x,  total = %d \r\n", keycode, pLister->lastKey, plst_privattr->totalItem);
		//��ʱû�еȵ���������,��ֱ��ִ��֮ǰ�����Ķ���
		key = pSupLister->lastKey&0x7F;
		pSupLister->timerEn = (pSupLister->lastKey>>7);
		pSupLister->lastKey = 0;
		if(key <= plst_privattr->totalItem)
		{
			plst_privattr->focusItemNo = key-1;
			if(plst_privattr->focusItemNo < plst_privattr->topItemNo)	// if delete last item(in the top of last screen),must change to pre-page.
			{
				 plst_privattr->topItemNo = plst_privattr->focusItemNo;
			}
			else if(plst_privattr->focusItemNo >= plst_privattr->topItemNo+plst_privattr->itemScreenNum)
			{
				plst_privattr->topItemNo = plst_privattr->focusItemNo+1-plst_privattr->itemScreenNum;
			}
			pSupLister->draw(pSupLister,0,0);
			DisplayData2Screen();
			syssleep(15);
			ret = pSupLister->SupListEnter(pSupLister, plst_privattr->focusItemNo);
			if(ret == RETURN_REDRAW)
			{
				ReDraw(pSupLister, 0, 0);
			}
		}
		
		return 1;
	}
	
	//ֻ����0--9���ּ�,�����ֻ֧��2λ(0--99)
	if(keycode < KEYEVENT_0 || keycode > KEYEVENT_9)
	{
	//hyUsbPrintf("keycode too big = %d  lastkey = %d  timer = %d \r\n",keycode,pLister->lastKey,plst_privattr->enable);
		if(pSupLister->lastKey > 0)
		{
			pSupLister->timerEn = (pSupLister->lastKey>>7);
			pSupLister->lastKey = 0;
		}
		return 0;
	}
	
	keycode -= KEYEVENT_0;
	
	if(pSupLister->lastKey == 0)//��һ����
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
			plst_privattr->focusItemNo = key-1;
			if(plst_privattr->focusItemNo < plst_privattr->topItemNo)	// if delete last item(in the top of last screen),must change to pre-page.
			{
				 plst_privattr->topItemNo = plst_privattr->focusItemNo;
			}
			else if(plst_privattr->focusItemNo >= plst_privattr->topItemNo+plst_privattr->itemScreenNum)
			{
				plst_privattr->topItemNo = plst_privattr->focusItemNo+1-plst_privattr->itemScreenNum;
			}
			pSupLister->draw(pSupLister,0,0);
			DisplayData2Screen();
			syssleep(15);
			ret = pSupLister->SupListEnter(pSupLister, plst_privattr->focusItemNo);
			if(ret == RETURN_REDRAW)
			{
				ReDraw(pSupLister, 0, 0);
			}
			return 1;
		}
		else if(key*10 >= plst_privattr->totalItem)
		{
		//hyUsbPrintf("key*10  = %d \r\n", key*10);
			//��ͳ�2λ���󳬹�����,��ֻ�ܰ�һλ����
			plst_privattr->focusItemNo = key-1;
			if(plst_privattr->focusItemNo < plst_privattr->topItemNo)	// if delete last item(in the top of last screen),must change to pre-page.
			{
				 plst_privattr->topItemNo = plst_privattr->focusItemNo;
			}
			else if(plst_privattr->focusItemNo >= plst_privattr->topItemNo+plst_privattr->itemScreenNum)
			{
				plst_privattr->topItemNo = plst_privattr->focusItemNo+1-plst_privattr->itemScreenNum;
			}
			pSupLister->draw(pSupLister,0,0);
			DisplayData2Screen();
			syssleep(15);
			ret = pSupLister->SupListEnter(pSupLister, plst_privattr->focusItemNo);
			if(ret == RETURN_REDRAW)
			{
				ReDraw(pSupLister, 0, 0);
			}
			return 1;
		}
		else
		{
		//hyUsbPrintf(" init timer  = %d \r\n",plst_privattr->enable);
			//��Ҫ�ȴ��ڶ�������
			pSupLister->lastKey = key;
			if(pSupLister->timerEn == 1)
			{
				pSupLister->lastKey |= 0x80;
			}
			
			//����timer
			pSupLister->timerEn = 1;
			plst_privattr->TimerLft = 100;//�ȴ�1S
		//hyUsbPrintf("    timer  = %d \r\n",plst_privattr->enable);
		}
	}
	else //if(pLister->lastKey != 0)//֮ǰ�а���,�����Ǻ�����
	{
		key = (pSupLister->lastKey&0x7F);
		pSupLister->timerEn = (pSupLister->lastKey>>7);
		pSupLister->lastKey = 0;
		
		key = key*10+keycode;
	//hyUsbPrintf("keycode = %d  key = %d \r\n",keycode, key);
		if(key <= plst_privattr->totalItem)
		{
			plst_privattr->focusItemNo = key-1;
			if(plst_privattr->focusItemNo < plst_privattr->topItemNo)	// if delete last item(in the top of last screen),must change to pre-page.
			{
				 plst_privattr->topItemNo = plst_privattr->focusItemNo;
			}
			else if(plst_privattr->focusItemNo >= plst_privattr->topItemNo+plst_privattr->itemScreenNum)
			{
				plst_privattr->topItemNo = plst_privattr->focusItemNo+1-plst_privattr->itemScreenNum;
			}
			pSupLister->draw(pSupLister,0,0);
			DisplayData2Screen();
			syssleep(15);
			ret = pSupLister->SupListEnter(pSupLister, plst_privattr->focusItemNo);
			if(ret == RETURN_REDRAW)
			{
				ReDraw(pSupLister, 0, 0);
			}
			return 1;
		}
		return 0;
	}
	
	return 0;
}

S32 Grap_SupListSysMsg(void *pView)
{
	return SUCCESS;
}

S32 Grap_SupListGetStr(void *pView, U16 itemNo, U8 *str, U16 maxLen)
{
	return SUCCESS;
}

S32 Grap_SupListEnter(void *pView, U16 itemNo)
{
	return SUCCESS;
}

S32 Grap_SupListDelOpen(void *pView, U16 itemNo)//ɾ����ǰ�򿪵���
{
	tSUPPERLIST		*pSupList;
	tSupListPriv	*pSupListPriv;
	tItemInfo		*pInfo, *pTemp;
	
	pSupList = (tSUPPERLIST *)pView;
	pSupListPriv = &(pSupList->SupList_priv);
	
	if(itemNo >= pSupListPriv->totalItem || pSupListPriv->totalItem == 0)
	{
		return -1;
	}
	
	if(pSupList->pItemInfoH == pSupList->pItemInfoT && pSupList->pItemInfoH->curItemNo == itemNo)//ֻ��һ��
	{
		pInfo = pSupList->pItemInfoH;
		pSupList->pItemInfoH = NULL;
		pSupList->pItemInfoT = NULL;
		free((void*)pInfo);
		pInfo = NULL;
	}
	else
	{
		pInfo = pSupList->pItemInfoH;
		pTemp = pSupList->pItemInfoH;
		while(pInfo != NULL)
		{
			if(pInfo->curItemNo == itemNo)
			{
				if(pInfo == pSupList->pItemInfoH)//ͷ
				{
					pSupList->pItemInfoH = pInfo->pNext;
					pInfo->pNext = NULL;
				}
				else if(pInfo == pSupList->pItemInfoT)//β
				{
					pTemp->pNext = NULL;
					pSupList->pItemInfoT = pTemp;
				}
				else
				{
					pTemp->pNext = pInfo->pNext;
					pInfo->pNext = NULL;
				}
				free((void*)pInfo);
				pInfo = NULL;
				pTemp = NULL;
				
				break;
			}
			else
			{
				pTemp = pInfo;
				pInfo = pInfo->pNext;
			}
		}
	}
	
	return 0;
}

S32 Grap_SupListDelete(void *pView)
{
	tSUPPERLIST		*pSupList;
	
	pSupList = (tSUPPERLIST *)pView;
	
	ItemInfo_DelNode( &(pSupList->pItemInfoH), &(pSupList->pItemInfoT) );	
	
	pSupList->SupListUpDate(pSupList);
	
	return SUCCESS;
}

S32 Grap_SupListDrawSrc(void *pView)
{
	tSUPPERLIST		*pSupList;
	tGrapViewAttr	*pView_attr;
	tSupListPriv	*pSupListPriv;
	tListScr		*pListScr;
	U16 		x1,x2,y1,y2;
	U16 row,column,width,height,i,j;
	tRES_PARA   tRes;
	U32 *pClr;
	
	pSupList = (tSUPPERLIST *)pView;
	pView_attr = &(pSupList->view_attr);
	pSupListPriv = &(pSupList->SupList_priv);
	pListScr = &(pSupList->ListScr);
	
	if( (pSupListPriv->ListInfo & NEED_SCRBAR) == 0)
		return SUCCESS;
	
	if (res_load_bmp_info(pSupListPriv->scrBarId, &tRes) < 0) return FAIL;
	
	x1 = pListScr->viewX1;
	x2 = pListScr->viewX2;
	
	y1 = pListScr->viewY1;
	y2 = pListScr->scrStartY;
	Grap_ShowPicture(pSupListPriv->scrBarHeadId, x1, y1);//��һ����

	y1 = pListScr->scrStartY;
	y2 = pListScr->scrEndY;
	while(y1+tRes.high < y2)
	{
		Grap_ShowPicture(pSupListPriv->scrBarId,x1, y1);//�м䲿��
		y1 = y1+tRes.high;
	}
	y1 = y2 - tRes.high;
	Grap_ShowPicture(pSupListPriv->scrBarId, x1, y1);//�м䲿��
	
	y1 = pListScr->scrEndY;
	y2 = pListScr->viewY2;
	Grap_ShowPicture(pSupListPriv->scrBarTailId, x1, y1);//�ڶ�����
	
	//��������
	row = pListScr->barStart;
	column = pListScr->viewX1+1;
	width = tRes.width-2;
	height = pListScr->barH;
	for(i=0; i < width; i++)
	{
		drvLcd_SetColor(ScrollBarColor[i], COLOR_BLACK);
		Memory_SetRowColumn(row, column++, height, 1);
		
		for(j=0; j < height; j++)
		{
			if(j == 0 || j == height-1)	drvLcd_WritePoint(0);
			else drvLcd_WritePoint(gFrontColor);
		}
	}
	
	
	return SUCCESS;
}

S32 Grap_SupListDrawIcon(void *pView, U16 itemNo)
{
	tSUPPERLIST		*pSupList;
	tGrapViewAttr	*pView_attr;
	tSupListPriv	*pSupListPriv;
	tListScr		*pListScr;
	U16	isSelect, x1,x2,y1,y2;
	
	pSupList = (tSUPPERLIST *)pView;
	pView_attr = &(pSupList->view_attr);
	pSupListPriv = &(pSupList->SupList_priv);
	pListScr = &(pSupList->ListScr);
	
	x1 = pView_attr->viewX1 + pSupListPriv->iconX;
	x2 = x1 + pSupListPriv->iconW;
	y1 = (itemNo - pSupListPriv->topItemNo) * pSupListPriv->itemH + pListScr->viewY1 + pSupListPriv->iconY;
	y2 = y1 + pSupListPriv->iconH;
	
	isSelect = ItemInfo_GetCurItemInfo(pSupList->pItemInfoH,itemNo);
	
	if(isSelect == 1)//ѡ��
	{
		if(pSupListPriv->icon_SelectId)
			Grap_ShowPicture(pSupListPriv->icon_SelectId, x1, y1);
	}
	else
	{
		if(pSupListPriv->icon_NoSelectId)
			Grap_ShowPicture(pSupListPriv->icon_NoSelectId, x1, y1);
	}
		
	return SUCCESS;
}

S32 Grap_SupListPullMsg(void *pView,GRAP_EVENT *pEvent)
{
	S32 ret = FAIL;
	tSUPPERLIST		*pSupList;
	tGrapViewAttr	*pView_attr;
	tSupListPriv	*pSupListPriv;
	tListScr		*pListScr;
	U16 curX,curY,lastX,lastY;
	U16 x1,x2,y1,y2;
	static S16 selectItem = -1, lastFocusItem = -1;
	
	pSupList = (tSUPPERLIST *)pView;
	pView_attr = &(pSupList->view_attr);
	pSupListPriv = &(pSupList->SupList_priv);
	pListScr = &(pSupList->ListScr);
	
	curY = HIWORD(pEvent->lParam);
	curX = LOWORD(pEvent->lParam);
	
	lastY = HIWORD(pEvent->wParam);
	lastX = LOWORD(pEvent->wParam);
	
	if (VIEW_PULL_START == pEvent->extParam)
	{
		if(GRAP_INWIDGET_RANGE(curX,curY,pSupList))
		{
			ret = VIEW_PULL_START;
		}
		else
		{
			return ret;
		}
	}
	if (VIEW_PULL_START == pEvent->extParam)
	{
		Grap_ChangeFocusEnable(pSupList);
		if( (curY <= pListScr->viewY1) && (pSupListPriv->ListInfo & NEED_TITLE))//�ڱ�����
		{
			x1 = pView_attr->viewX1;
			x2 = x1+pSupListPriv->iconX + pSupListPriv->iconW+40;
			y1 = pView_attr->viewY1;
			y2 = y1+pSupListPriv->titleH;
			if(curX >= x1 && curX <= x2 && curY >= y1 && curY <= y2)
			{
				pSupList->pullFlag |= FIRSTPOINT_INTITLEICON;
			}
		}
		else if(curX >= pListScr->viewX1 && curX <= pListScr->viewX2 &&
				curY >= pListScr->viewY1 && curY <= pListScr->viewY2 && (pSupListPriv->ListInfo & NEED_SCRBAR))//����������
		{
			pSupList->pullFlag |= FIRSTPOINT_INSCRBAR;
			if(curY <= pListScr->arrow1_Y2)//�ϼ�ͷ
			{
				pSupList->SupListLineUp(pSupList,0);
			}
			else if(curY >= pListScr->arrow2_Y1)//�¼�ͷ
			{
				pSupList->SupListLineDown(pSupList,0);
			}
			else //������
			{
				moveScrBar(pSupList,curY,0);
			}
		}
		else //list����
		{
			int i;
			x1 = pView_attr->viewX1;
			x2 = x1+pSupListPriv->iconX + pSupListPriv->iconW;
			
			for(i = 0; i < pSupListPriv->itemScreenNum;i++)
			{
				y1 = i*pSupListPriv->itemH + pListScr->viewY1;
				y2 = y1 + pSupListPriv->itemH;
				if(curY >= y1 && curY < y2)
				{
					selectItem = i+pSupListPriv->topItemNo;//�������Ŀ
					break;
				}
			}
			if(selectItem < pSupListPriv->totalItem && selectItem != -1)
			{
				if(curX >= x1 && curX <= x2)//ͼ����
				{
					pSupList->pullFlag |= FIRSTPOINT_INLISTICON;
				}
				else //list��
				{
					pSupList->pullFlag |= FIRSTPOINT_INLIST;
					
					if(selectItem != pSupListPriv->focusItemNo)//���ǵ�ǰ����
					{
						if( (pSupListPriv->ListInfo & CONFIRMANDENTER)==0 )
						{
							lastFocusItem = pSupListPriv->focusItemNo;
						}
						selectItem = selectItem^pSupListPriv->focusItemNo;
						pSupListPriv->focusItemNo = pSupListPriv->focusItemNo^selectItem;
						selectItem = selectItem^pSupListPriv->focusItemNo;
						
						pSupList->SupListDrawOne(pSupList, selectItem);//ˢ��֮ǰ����
						pSupList->SupListDrawOne(pSupList, pSupListPriv->focusItemNo);//ˢ�µ�ǰ����
						pSupList->SupListDrawTitle(pSupList);
					}
				}
			}
			else
			{
				selectItem = -1;
			}		
		}
		
	}
	else if(VIEW_PULL_ING == pEvent->extParam)
	{
		/*if(pSupList->pullFlag & FIRSTPOINT_INLIST)//��list��
		{
			y1 = (pSupListPriv->focusItemNo-pSupListPriv->topItemNo)*pSupListPriv->itemH + pListScr->viewY1;
			y2 = y1 + pSupListPriv->itemH;
			if(curY > y2)//��������
			{
				pSupList->SupListLineDown(pSupList,1);
				pSupList->pullFlag |= HAS_PULLED;
			}
			else if(curY < y1)//��������
			{
				pSupList->SupListLineUp(pSupList,1);
				pSupList->pullFlag |= HAS_PULLED;
			}
		}
		else */if(GRAP_INWIDGET_RANGE(curX,curY,pSupList))
		{
			if(pSupList->pullFlag & FIRSTPOINT_INTITLEICON)//��ʼ���ڱ����е�Сͼ����
			{
				return FAIL;
			}
			else if(pSupList->pullFlag & FIRSTPOINT_INSCRBAR)//��ʼ���ڹ�������
			{
				if(curX >= pListScr->viewX1 && curX <= pListScr->viewX2 &&
				   curY >= pListScr->viewY1 && curY <= pListScr->viewY2)//���λ��ڹ�������
				{
					if(curY <= pListScr->arrow1_Y2 && lastY <= pListScr->arrow1_Y2)//�ϼ�ͷ
					{
						pSupList->SupListLineUp(pSupList,0);
					}
					else if(curY >= pListScr->arrow2_Y1 && lastY >= pListScr->arrow2_Y1)//�¼�ͷ
					{
						pSupList->SupListLineDown(pSupList,0);
					}
					else if(curY >= pListScr->scrStartY && curY <= pListScr->scrEndY && 
							lastY >= pListScr->scrStartY && lastY <= pListScr->scrEndY) //������
					{
						S16 temp,re;
						
						temp = curY - lastY;

						if(temp > 0)
							re = moveScrBar(pSupList,ABS(temp),1);//����
						else
							re = moveScrBar(pSupList,ABS(temp),-1);//����
						
						if(re == 0)
						{
							pSupList->pullFlag |= HAS_PULLED;
							return VIEW_PULL_ING;
						}
						else	return FAIL;
					}
				}
			}
			else if(pSupList->pullFlag & FIRSTPOINT_INLISTICON)//list��ͼ���� ������ק
			{
				return FAIL;
			}
			else if(pSupList->pullFlag & FIRSTPOINT_INLIST)//list�� ����ק
			{
				y1 = (pSupListPriv->focusItemNo-pSupListPriv->topItemNo)*pSupListPriv->itemH + pListScr->viewY1;
				y2 = y1 + pSupListPriv->itemH;
				if(curY > y2)//top�����ƶ�
				{
					if(pSupListPriv->topItemNo > 0)
						pSupList->SupListLineUp(pSupList,0);
					//else //ֻ�����ƶ�����
					//	pSupList->SupListLineDown(pSupList,1);
					pSupList->pullFlag |= HAS_PULLED;
				}
				else if(curY < y1)//top�����ƶ�
				{
					if(pSupListPriv->topItemNo + pSupListPriv->itemScreenNum < pSupListPriv->totalItem)
						pSupList->SupListLineDown(pSupList,0);
					//else //ֻ�����ƶ�����
					//	pSupList->SupListLineUp(pSupList,1);
					pSupList->pullFlag |= HAS_PULLED;
				}
			}
		}
		else
		{
			return FAIL;
		}
	}
	else if(VIEW_PULL_END == pEvent->extParam)
	{
		ret = VIEW_PULL_END;
		if(GRAP_INWIDGET_RANGE(curX,curY,pSupList))
		{
			if(pSupList->pullFlag & FIRSTPOINT_INTITLEICON)//��ʼ���ڱ����е�Сͼ����
			{
				if( (pSupList->pullFlag & HAS_PULLED) == 0)//δ������ק
				{
					x1 = pView_attr->viewX1;
					x2 = x1+pSupListPriv->iconX + pSupListPriv->iconW+40;
					y1 = pView_attr->viewY1;
					y2 = y1+pSupListPriv->titleH;
					if(curX >= x1 && curX <= x2 && curY >= y1 && curY <= y2)
					{
						if(pSupListPriv->ListInfo & SELECT_ALL)//֮ǰ��ȫѡ״̬
						{
							pSupListPriv->ListInfo &= ~SELECT_ALL;
							ItemInfo_ChangeAllItemInfo(pSupList->pItemInfoH, 0);//���ȫ��ûѡ
						}
						else //֮ǰ��û��ȫѡ
						{
							pSupListPriv->ListInfo |= SELECT_ALL;
							ItemInfo_ChangeAllItemInfo(pSupList->pItemInfoH, 1);//�ó�ȫѡ
						}
						pSupList->draw(pSupList,0,0);
					}
				}
			}
			else if(pSupList->pullFlag & FIRSTPOINT_INSCRBAR)//��ʼ���ڹ�������
			{
				//̧��ʱ��������
			}
			else if(pSupList->pullFlag & FIRSTPOINT_INLISTICON)//list����Сͼ����
			{
				if( (pSupList->pullFlag & HAS_PULLED) == 0)//δ������ק
				{
					int i;
					S16 curItem=-1;
					
					x1 = pView_attr->viewX1;
					x2 = x1+pSupListPriv->iconX + pSupListPriv->iconW;
			
					for(i = 0; i < pSupListPriv->itemScreenNum;i++)
					{
						y1 = i*pSupListPriv->itemH + pListScr->viewY1;
						y2 = y1 + pSupListPriv->itemH;
						if(curX >= x1 && curX <= x2 && curY >= y1 && curY < y2)
						{
							curItem = i+pSupListPriv->topItemNo;//�������Ŀ
							break;
						}
					}

					if(selectItem == curItem && curItem != -1)//���ڿ�ʼ�����ͼ����
					{
						U16 isSelect;
						isSelect = ItemInfo_ChangeItemInfo(pSupList->pItemInfoH, (U16)selectItem);
						pSupList->SupListDrawOne(pSupList,(U16)selectItem);
						if(isSelect == 0 && (pSupListPriv->ListInfo & SELECT_ALL) && (pSupListPriv->ListInfo & NEED_TITLE) )
						{
							pSupListPriv->ListInfo &= ~SELECT_ALL;
							pSupList->SupListDrawTitle(pSupList);
						}
					}
				}
			}
			else if(pSupList->pullFlag & FIRSTPOINT_INLIST)//list��
			{
				if( (pSupList->pullFlag & HAS_PULLED) == 0)//δ������ק
				{
					int i;
					x1 = pView_attr->viewX1+pSupListPriv->iconX + pSupListPriv->iconW;
					x2 = pView_attr->viewX1 + pView_attr->viewWidth;
					
					for(i = 0; i < pSupListPriv->itemScreenNum;i++)
					{
						y1 = i*pSupListPriv->itemH + pListScr->viewY1;
						y2 = y1 + pSupListPriv->itemH;
						if(curX >= x1 && curX <= x2 && curY >= y1 && curY <= y2)
						{
							i += pSupListPriv->topItemNo;
							
							if(pSupListPriv->ListInfo & CONFIRMANDENTER)//��һ�ε���ͽ���
							{
								if(pSupListPriv->focusItemNo != i)//���ǵ�ǰ����
								{
									pSupListPriv->focusItemNo = i;
									pSupList->draw(pSupList, 0, 0);
									DisplayData2Screen();
									syssleep(25);
								}
								ret = pSupList->SupListEnter(pSupList,pSupListPriv->focusItemNo);
								pSupList->SupListUpDate(pSupList);
								if (ret == RETURN_REDRAW)
								{
									ReDraw(pSupList, 0, 0);
								}
								else if (RETURN_QUIT == ret)
								{
									*((tGRAPDESKBOX *)pSupList->pParent)->pQuitCnt = 1;							
								}
								ret = VIEW_PULL_END;
							}
							else
							{
								if(pSupListPriv->focusItemNo != lastFocusItem && lastFocusItem != -1)//���ǵ�ǰ����
								{
									pSupList->draw(pSupList, 0, 0);
								}
								else
								{
									ret = pSupList->SupListEnter(pSupList,pSupListPriv->focusItemNo);
									pSupList->SupListUpDate(pSupList);
									if (ret == RETURN_REDRAW)
									{
										ReDraw(pSupList, 0, 0);
									}
									else if (RETURN_QUIT == ret)
									{
										*((tGRAPDESKBOX *)pSupList->pParent)->pQuitCnt = 1;							
									}
									ret = VIEW_PULL_END;
								}
							}
							break;
						}
					}
				}
			}
		}
		lastFocusItem = -1;
		selectItem = -1;
		pSupList->pullFlag = 0;
	}
	else if(VIEW_PULL_TIMEOUT == pEvent->extParam)
	{
		lastFocusItem = -1;
		selectItem = -1;
		pSupList->pullFlag = 0;
		ret = VIEW_PULL_TIMEOUT;
	}
	
	
	return ret;
}

S32 Grap_SupListInt(void *pView)
{
	tSUPPERLIST		*pSupList;
	tGrapViewAttr	*pView_attr;
	tSupListPriv	*pSupListPriv;
	tListScr		*pListScr;
	float temp;
	tRES_PARA       tRes;
	
	pSupList = (tSUPPERLIST *)pView;
	pView_attr = &(pSupList->view_attr);
	pSupListPriv = &(pSupList->SupList_priv);
	pListScr = &(pSupList->ListScr);
	
	if((pSupListPriv->ListInfo & NEED_TITLE) == 0)//����Ҫ������
	{
		pSupListPriv->titleH = 0;
	}
	if(pSupListPriv->titleW == 0)
	{
		pSupListPriv->titleW = cLCD_TOTAL_COLUMN;
	}
	//pSupListPriv->titleW = pView_attr->viewWidth;
	
	if(pSupList->pItemInfoH != NULL)//�Ȱ�ԭ���Ľڵ��ͷŵ�
	{
		pSupList->view_attr.viewWidth = pSupListPriv->titleW;
		ItemInfo_Destroy(pSupList->pItemInfoH);
		pSupList->pItemInfoH = NULL;
		pSupList->pItemInfoT = NULL;
	}
	
	if(-1 == ItemInfo_Create( &(pSupList->pItemInfoH), &(pSupList->pItemInfoT), pSupListPriv->totalItem))	return -1;
	
	
	if (res_load_bmp_info(pSupListPriv->scrBarHeadId, &tRes) < 0) return FAIL;
	
	//pListScr->viewX1 = pView_attr->viewX1+pView_attr->viewWidth - tRes.width;
	pListScr->viewX1 = pView_attr->viewX1+pSupListPriv->titleW - tRes.width;
	pListScr->viewY1 = pView_attr->viewY1+pSupListPriv->titleH;
	pListScr->viewX2 = pView_attr->viewX1+pView_attr->viewWidth;
	//pListScr->viewY2 = pView_attr->viewY1+pView_attr->viewHeight;
	pListScr->viewY2 = pView_attr->viewY1+(pSupListPriv->itemH)*(pSupListPriv->itemScreenNum);
	
	pListScr->scrStartY = pListScr->viewY1+tRes.high;
	pListScr->scrEndY 	= pListScr->viewY2 - tRes.high;
	pListScr->scrLen 	= pListScr->scrEndY - pListScr->scrStartY;
	
	pListScr->arrow1_Y1 	= pListScr->viewY1;
	pListScr->arrow1_Y2 	= pListScr->viewY1+tRes.high-3;
	pListScr->arrow2_Y2 	= pListScr->viewY2;
	pListScr->arrow2_Y1 	= pListScr->arrow2_Y2-tRes.high+3;
	
	updateInfo(pSupList);

	return SUCCESS;
}

/*****************************************
* ���ݵ�ǰ�ڵ�������,�����¼���������
*****************************************/
S32 Grap_SupListUpDate(void *pView)
{
	tSUPPERLIST		*pSupList;
	tGrapViewAttr	*pView_attr;
	tSupListPriv	*pSupListPriv;
	tListScr		*pListScr;
	float temp;
	
	pSupList = (tSUPPERLIST *)pView;
	pView_attr = &(pSupList->view_attr);
	pSupListPriv = &(pSupList->SupList_priv);
	pListScr = &(pSupList->ListScr);
	
	pSupListPriv->totalItem = ItemInfo_Update( pSupList->pItemInfoH );
	
	updateInfo(pSupList);
	
	return SUCCESS;
}

/*****************************************
* ���ݵ�ǰ����������,�����¸��������������ڵ���
*****************************************/
S32 Grap_SupListUpDateByTotal(void *pView)
{
	tSUPPERLIST		*pSupList;
	tSupListPriv	*pSupListPriv;
	U16 total;
	
	pSupList = (tSUPPERLIST *)pView;
	pSupListPriv = &(pSupList->SupList_priv);
	
	total = pSupListPriv->totalItem;
	
	if(pSupList->pItemInfoH != NULL)//�Ȱ�ԭ���Ľڵ��ͷŵ�
	{
		ItemInfo_Destroy(pSupList->pItemInfoH);
		pSupList->pItemInfoH = NULL;
		pSupList->pItemInfoT = NULL;
	}
	
	//�����������´����ڵ�
	ItemInfo_Create( &(pSupList->pItemInfoH), &(pSupList->pItemInfoT), total );
	
	updateInfo(pSupList);
	
	
	return SUCCESS;
}


S32 Grap_SupListTimer(void *pView, U16 itemNo)
{
	tSUPPERLIST		*pSupList;
	tGrapViewAttr	*pView_attr;
	tSupListPriv	*pSupListPriv;
	tListScr		*pListScr;
	U8	tmpStr[100];
	U8  string[30], *pStr;
	U16 templen,tempwid;
	int t;
	U16 len=0,lenstr ,widstr,x1, y1, x2, y2;
	tStrDispInfo disp_info;
	Character_Type_e kStr_Type;
	
	pSupList = (tSUPPERLIST *)pView;
	pView_attr = &(pSupList->view_attr);
	pSupListPriv = &(pSupList->SupList_priv);
	pListScr = &(pSupList->ListScr);
	
	if(itemNo >= pSupListPriv->totalItem)	return SUCCESS;
	memset(tmpStr,0,100);
	memset(string,0,30);
	
	pSupList->SupListGetStr(pSupList,itemNo,tmpStr,100);
	
	kStr_Type = pSupListPriv->font_type;
	lenstr = Get_StringLength(kStr_Type,tmpStr);
	widstr = Get_StringWidth(kStr_Type,tmpStr);//�ַ����(UNICODE��ǵİ�1��)
	
	if(widstr <= pSupListPriv->screenStrLen)//����Ҫ�ƶ���ʾ
		return SUCCESS;
	
	x1 = pView_attr->viewX1;
	x2 = pView_attr->viewX1 + pView_attr->viewWidth;
	y1 = (itemNo - pSupListPriv->topItemNo) * pSupListPriv->itemH + pListScr->viewY1;
	y2 = y1 + pSupListPriv->itemH;
	
	if(pSupListPriv->ListInfo & NEED_SCRBAR)//��Ҫ������ʱ���̵�
	{
		if(pSupListPriv->shortCoverPicId)
			Grap_ShowPicture(pSupListPriv->shortCoverPicId, x1, y1);
		else if(pSupListPriv->backPicId)
			Grap_ClearScreen(x1, y1, x2, y2);
	}
	else
	{
		if(pSupListPriv->longCoverPicId)
			Grap_ShowPicture(pSupListPriv->longCoverPicId, x1, y1);
		else if(pSupListPriv->backPicId)
			Grap_ClearScreen(x1, y1, x2, y2);
	}
	
	//��ǰ���Сͼ��
	pSupList->SupListDrawIcon(pSupList,itemNo);
	
	if(kStr_Type == CHARACTER_LOCALCODE)
	{
		if(tmpStr[pSupListPriv->firstCharPos]>=0x80)	pSupListPriv->firstCharPos+=2;
		else	pSupListPriv->firstCharPos++;
	}
	else
	{
		pSupListPriv->firstCharPos+=2;
	}
	if(pSupListPriv->firstCharPos >= lenstr) pSupListPriv->firstCharPos = 0;
	
	
	pStr = &tmpStr[pSupListPriv->firstCharPos];
	templen = Get_StringLength(kStr_Type,pStr);
	tempwid = Get_StringWidth(kStr_Type,pStr);
	if(tempwid <= pSupListPriv->screenStrLen && tempwid+6 >= pSupListPriv->screenStrLen)
	{
		memcpy(string, pStr, templen);
		string[templen]=0;
		string[templen+1]=0;
	}
	else if(tempwid+6 < pSupListPriv->screenStrLen)
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
		
		tempwid = pSupListPriv->screenStrLen - tempwid - 6;
		pStr = tmpStr;
		t = Grap_GetDispStr(kStr_Type,tempwid,pStr);
		memcpy(&string[templen], pStr, t);
		string[templen+t]  =0;
		string[templen+t+1]=0;
	}
	else if(tempwid > pSupListPriv->screenStrLen)
	{
		tempwid = pSupListPriv->screenStrLen;
		t = Grap_GetDispStr(kStr_Type,tempwid,pStr);
		memcpy(string, pStr, t);
		string[t]=0;
		string[t+1]=0;
	}

	disp_info.string = string;
	disp_info.color = pSupListPriv->fontFocusColor;
	disp_info.font_size = pSupListPriv->font_size;
	disp_info.font_type = pSupListPriv->font_type;
	disp_info.flag = 0;
	disp_info.keyStr = NULL;
	Grap_WriteString(pSupListPriv->strX, y1+pSupListPriv->strY, pSupListPriv->strX+8*pSupListPriv->screenStrLen, y2, &disp_info);
	
	return SUCCESS;
}

/*
 *flag 0:�ƶ�top��  1:�ƶ�focus��
 */
S32 Grap_SupListLineUp(void *pView,U32 flag)
{
	tSUPPERLIST		*pSupList;
	tSupListPriv	*pSupListPriv;
	
	pSupList = (tSUPPERLIST *)pView;
	pSupListPriv = &(pSupList->SupList_priv);
	
	if(pSupListPriv->totalItem == 0)	return SUCCESS;
	
	if(flag == 1)
	{
		if(pSupListPriv->focusItemNo > 0)
		{
			pSupListPriv->focusItemNo--;
			if(pSupListPriv->focusItemNo < pSupListPriv->topItemNo)
			{
				pSupListPriv->topItemNo = pSupListPriv->focusItemNo;
				updateScrbar(pSupList);
				pSupList->draw(pSupList, 0, 0);
			}
			else
			{
				pSupList->SupListDrawOne(pSupList, (U16)(pSupListPriv->focusItemNo+1));
				pSupList->SupListDrawOne(pSupList, pSupListPriv->focusItemNo);
				pSupList->SupListDrawTitle(pSupList);
			}
		}
		else//��һ��
		{
			pSupListPriv->focusItemNo = pSupListPriv->totalItem - 1;
			if(pSupListPriv->totalItem < pSupListPriv->itemScreenNum)
				pSupListPriv->topItemNo = 0;
			else
				pSupListPriv->topItemNo = pSupListPriv->totalItem - pSupListPriv->itemScreenNum;
			
			updateScrbar(pSupList);
			pSupList->draw(pSupList, 0, 0);
		}
	}
	else if(flag == 0)
	{
		if(pSupListPriv->topItemNo > 0)
		{
			pSupListPriv->topItemNo --;
			updateScrbar(pSupList);
			pSupList->draw(pSupList, 0, 0);
		}
	}
	
	
	return SUCCESS;
}
/*
 *flag 0:�ƶ�top��  1:�ƶ�focus��
 */
S32 Grap_SupListLineDown(void *pView,U32 flag)
{
	tSUPPERLIST		*pSupList;
	tSupListPriv	*pSupListPriv;
	
	pSupList = (tSUPPERLIST *)pView;
	pSupListPriv = &(pSupList->SupList_priv);
	
	if(pSupListPriv->totalItem == 0)	return SUCCESS;
	
	if(flag == 1)
	{
		if (pSupListPriv->focusItemNo < pSupListPriv->totalItem - 1)
		{
			pSupListPriv->focusItemNo++;
			if(pSupListPriv->focusItemNo >= pSupListPriv->topItemNo + pSupListPriv->itemScreenNum)
			{
				pSupListPriv->topItemNo++;
				updateScrbar(pSupList);
				pSupList->draw(pSupList, 0, 0);
			}
			else
			{
				pSupList->SupListDrawOne(pSupList, (U16)(pSupListPriv->focusItemNo-1));
				pSupList->SupListDrawOne(pSupList, pSupListPriv->focusItemNo);
				pSupList->SupListDrawTitle(pSupList);
			}
		}
		else
		{
			pSupListPriv->focusItemNo = 0;
			pSupListPriv->topItemNo = 0;
			updateScrbar(pSupList);
			pSupList->draw(pSupList, 0, 0);
		}
	}
	else if(flag == 0)
	{
		if(pSupListPriv->topItemNo+pSupListPriv->itemScreenNum < pSupListPriv->totalItem)
		{
			pSupListPriv->topItemNo ++;
			updateScrbar(pSupList);
			pSupList->draw(pSupList, 0, 0);
		}
	}
	
	
	return SUCCESS;
}

S32 Grap_SupListPageUp(void *pView)
{
	tSUPPERLIST		*pSupList;
	tSupListPriv	*pSupListPriv;
	
	pSupList = (tSUPPERLIST *)pView;
	pSupListPriv = &(pSupList->SupList_priv);
	
	if(pSupListPriv->totalItem <= pSupListPriv->itemScreenNum)	return SUCCESS;
	
	if (pSupListPriv->topItemNo)
	{
		if (pSupListPriv->topItemNo > pSupListPriv->itemScreenNum)
			pSupListPriv->topItemNo -= pSupListPriv->itemScreenNum;
		else
			pSupListPriv->topItemNo = 0;
		pSupListPriv->focusItemNo = pSupListPriv->topItemNo;//+pSupListPriv->itemScreenNum-1;
		updateScrbar(pSupList);
		pSupList->draw(pSupList, 0, 0);
	}
	else
	{
		pSupListPriv->topItemNo = pSupListPriv->totalItem - pSupListPriv->itemScreenNum;
		pSupListPriv->focusItemNo = pSupListPriv->topItemNo;//pSupListPriv->totalItem-1;
		updateScrbar(pSupList);
		pSupList->draw(pSupList, 0, 0);
	}
	
	return SUCCESS;
}

S32 Grap_SupListPageDown(void *pView)
{
	tSUPPERLIST		*pSupList;
	tSupListPriv	*pSupListPriv;
	
	pSupList = (tSUPPERLIST *)pView;
	pSupListPriv = &(pSupList->SupList_priv);
	
	if(pSupListPriv->totalItem <= pSupListPriv->itemScreenNum)	return SUCCESS;
	
	if (pSupListPriv->topItemNo + pSupListPriv->itemScreenNum < pSupListPriv->totalItem)
	{
		pSupListPriv->topItemNo += pSupListPriv->itemScreenNum;
		if(pSupListPriv->topItemNo + pSupListPriv->itemScreenNum > pSupListPriv->totalItem)
		{
			pSupListPriv->topItemNo = pSupListPriv->totalItem-pSupListPriv->itemScreenNum;
		}
		pSupListPriv->focusItemNo = pSupListPriv->topItemNo+pSupListPriv->itemScreenNum-1;
		updateScrbar(pSupList);
		pSupList->draw(pSupList, 0, 0);
	}
	else
	{
		pSupListPriv->topItemNo = 0;
		pSupListPriv->focusItemNo = pSupListPriv->topItemNo+pSupListPriv->itemScreenNum-1;
		updateScrbar(pSupList);
		pSupList->draw(pSupList, 0, 0);
	}
	
	return SUCCESS;
}

S32 Grap_SupListDrawOne(void *pView, U16 itemNo)
{
	tSUPPERLIST		*pSupList;
	tGrapViewAttr	*pView_attr;
	tSupListPriv	*pSupListPriv;
	tListScr		*pListScr;
	U16 x1, y1, x2, y2;
	U32 lenstr, widstr;
	U8	n,tmpStr[100];
	
	pSupList = (tSUPPERLIST *)pView;
	pView_attr = &(pSupList->view_attr);
	pSupListPriv = &(pSupList->SupList_priv);
	pListScr = &(pSupList->ListScr);
	
	if(itemNo >= pSupListPriv->totalItem)	return SUCCESS;
	if(itemNo < pSupListPriv->topItemNo || itemNo >= pSupListPriv->topItemNo+pSupListPriv->itemScreenNum)	return SUCCESS;
	
	memset(tmpStr,0,100);
	
	x1 = pView_attr->viewX1;
	x2 = pView_attr->viewX1 + pView_attr->viewWidth;
	y1 = (itemNo - pSupListPriv->topItemNo) * pSupListPriv->itemH + pListScr->viewY1;
	y2 = y1 + pSupListPriv->itemH;
	
	if(pSupListPriv->ListInfo & NEED_SCRBAR)//��Ҫ������ʱ���̵�
	{
		if(pSupListPriv->shortCoverPicId && pSupListPriv->focusItemNo == itemNo)
			Grap_ShowPicture(pSupListPriv->shortCoverPicId, x1, y1);
		else if(pSupListPriv->backPicId)
			Grap_ClearScreen(x1, y1, x2, y2);
	}
	else
	{
		if(pSupListPriv->longCoverPicId && pSupListPriv->focusItemNo == itemNo)
			Grap_ShowPicture(pSupListPriv->longCoverPicId, x1, y1);
		else if(pSupListPriv->backPicId)
			Grap_ClearScreen(x1, y1, x2, y2);
	}
	
	//��ǰ���Сͼ��
	pSupList->SupListDrawIcon(pSupList,itemNo);
	
	pSupList->SupListGetStr(pSupList,itemNo,tmpStr,100);
	
	if(tmpStr[0]!=0 || tmpStr[1] != 0)
	{	
		U8 string[30];
		U16 len=0;
		tStrDispInfo disp_info;
		Character_Type_e kStr_Type; 
		
		kStr_Type = pSupListPriv->font_type;//�ж�����
		lenstr = Get_StringLength(kStr_Type,tmpStr);//�ַ�����(UNICODE��ǵ�Ҳ��2��)
		widstr = Get_StringWidth(kStr_Type,tmpStr);//�ַ����(UNICODE��ǵİ�1��)

		if(widstr <= pSupListPriv->screenStrLen)
		{
			memcpy(string, tmpStr, lenstr);
			string[lenstr]=0;
			string[lenstr+1]=0;
		}
		else
		{
			lenstr = pSupListPriv->screenStrLen;
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
			widstr = pSupListPriv->screenStrLen-2;//���ܳ��������ʾ���
			t = Grap_GetDispStr(kStr_Type,widstr,tmpStr);
			memcpy(string, tmpStr, t);
			string[t]=0;
			string[t+1]=0;
			
			if(pSupListPriv->focusItemNo != itemNo)//��ʡ�Ժ�
			{
				Num = Get_StringWidth(kStr_Type,string);//�ַ����(UNICODE��ǵİ�1��)
				kRow = y1+pSupListPriv->strY+12;
				kColumn = pSupListPriv->strX+8*Num;
				drvLcd_SetColor(pSupListPriv->fontNoFocusColor,COLOR_BLACK);
				drvLcd_WriteBMPColor(Suspension_points,2,16,kRow,kColumn);
			}
		}*/
		
		disp_info.string = string;
		if(pSupListPriv->focusItemNo == itemNo)
			disp_info.color = pSupListPriv->fontFocusColor;
		else
			disp_info.color = pSupListPriv->fontNoFocusColor;
		disp_info.font_size = pSupListPriv->font_size;
		disp_info.font_type = pSupListPriv->font_type;
		disp_info.flag      = pSupListPriv->position;
		disp_info.keyStr = NULL;
		Grap_WriteString(pSupListPriv->strX, y1+pSupListPriv->strY, pSupListPriv->strX+8*pSupListPriv->screenStrLen, y2, &disp_info);
	}
	
	return SUCCESS;
}

S32 Grap_SupListDrawTitle(void *pView)//������������ ɾ�� ����, �Ժ��ټ�
{
	tSUPPERLIST		*pSupList;
	tGrapViewAttr	*pView_attr;
	tSupListPriv	*pSupListPriv;
	tListScr		*pListScr;
	tStrDispInfo	disp_info;
	U16 x1,x2,y1,y2;
	U8  str[10];
	
	pSupList = (tSUPPERLIST *)pView;
	pView_attr = &(pSupList->view_attr);
	pSupListPriv = &(pSupList->SupList_priv);
	pListScr = &(pSupList->ListScr);
	
	if( (pSupListPriv->ListInfo & NEED_TITLE) == 0)
	{
		//����Lister��ʾ��ǰ"�ڼ���/�ܹ�������" 
		if(pSupListPriv->totalItem > 0)
		{//��ʾ����
			U8 buf[40],page_c,page_t;
			tStrDispInfo disp_info;
			
			if(pSupListPriv->backPicId)
			{
				Grap_ClearScreen(pView_attr->viewX1, pView_attr->viewY1+pView_attr->viewHeight-20, 
					(U16)(pView_attr->viewX1+pView_attr->viewWidth), (U16)(pView_attr->viewY1+pView_attr->viewHeight));
			}
			
			//page_t = (pSupListPriv->totalItem-1)/pSupListPriv->itemScreenNum;
			//page_t++;
			
			//page_c = (pSupListPriv->topItemNo)/pSupListPriv->itemScreenNum;
			//page_c++;
			
			//��xxxҳ/��xxxҳ(xxx/xxx)
			memset(buf,0,40);
			//sprintf(buf,"��%dҳ/��%dҳ(%d/%d)",page_c, page_t, (pSupListPriv->focusItemNo+1),pSupListPriv->totalItem);
			sprintf(buf,"%d/%d",(pSupListPriv->focusItemNo+1),pSupListPriv->totalItem);
			
			disp_info.string = buf;
			disp_info.color = COLOR_YELLOW;//plst_privattr->lstFocusColor;
			disp_info.font_size = pSupListPriv->font_size;
			disp_info.font_type = pSupListPriv->font_type;
			disp_info.flag = 2;
			disp_info.keyStr = NULL;
			Grap_WriteString(pView_attr->viewX1, pView_attr->viewY1+pView_attr->viewHeight-20, 
						pView_attr->viewX1+pView_attr->viewWidth,pView_attr->viewY1+pView_attr->viewHeight, &disp_info);
		}
		
		return SUCCESS;
	}
	
	x1 = pView_attr->viewX1 + pSupListPriv->iconX;
	x2 = x1 + pSupListPriv->iconW;
	y1 = pView_attr->viewY1 + (pSupListPriv->titleH - pSupListPriv->iconH)/2;
	y2 = pView_attr->viewY1 + pSupListPriv->iconH;
	
	if(pSupListPriv->backPicId)
	{
		Grap_ClearScreen(pView_attr->viewX1, pView_attr->viewY1, (U16)(pView_attr->viewX1+pSupListPriv->titleW), 
			(U16)(pView_attr->viewY1+pSupListPriv->titleH));
	}
	
	if(pSupListPriv->totalItem == 0)
		pSupListPriv->ListInfo &= ~SELECT_ALL;
	if(pSupListPriv->ListInfo & SELECT_ALL)
	{
		Grap_ShowPicture(pSupListPriv->icon_SelectId, x1, y1);
	}
	else
	{
		Grap_ShowPicture(pSupListPriv->icon_NoSelectId, x1, y1);
	}
	
	x1 = x2 +8;
	x2 = x1 + 40;
#if 0	
	//"ȫ��"�ַ�
	disp_info.string = "ȫ��";
	disp_info.color  = pSupListPriv->fontNoFocusColor;
	disp_info.font_size = pSupListPriv->font_size;
	disp_info.font_type = pSupListPriv->font_type;
	disp_info.flag = 0;
	Grap_WriteString(x1, y1, x2,y2, &disp_info);
#endif	
	memset(str,0,10);
	if(pSupListPriv->totalItem > 0)
		sprintf(str,"%d/%d",(pSupListPriv->focusItemNo+1),pSupListPriv->totalItem );
	else
		sprintf(str,"%d/%d",pSupListPriv->focusItemNo,pSupListPriv->totalItem );
	
	x1 = pView_attr->viewX1 + pSupListPriv->titleW - 80;
	x2 = pView_attr->viewX1 + pSupListPriv->titleW;
	
	disp_info.string = str;
	
	if(pView_attr->curFocus == 1)
		disp_info.color  = COLOR_YELLOW;
	else
		disp_info.color  = pSupListPriv->fontNoFocusColor;
	disp_info.font_size = pSupListPriv->font_size;
	disp_info.font_type = pSupListPriv->font_type;
	disp_info.flag   = 2;
	disp_info.keyStr = NULL;
	Grap_WriteString(x1, y1, x2,y2, &disp_info);
	
	return SUCCESS;
}

S32 Grap_SupListDraw(void *pView, U32 xy, U32 wh)
{
	tSUPPERLIST		*pSupList;
	tGrapViewAttr	*pView_attr;
	tSupListPriv	*pSupListPriv;
	tListScr		*pListScr;
	int i;
	
	pSupList = (tSUPPERLIST *)pView;
	pView_attr = &(pSupList->view_attr);
	pSupListPriv = &(pSupList->SupList_priv);
	pListScr = &(pSupList->ListScr);
	
	//���ж��Ƿ������Լ���ͼƬ,û��ָ��ͼƬʱ���ñ���ˢ
	if(pView_attr->clNormal)
	{
		Grap_ShowPicture(pView_attr->clNormal, pView_attr->viewX1, pView_attr->viewY1);
	}
	else if(pView_attr->clFocus)
	{
		Grap_ShowPicture(pView_attr->clFocus, pView_attr->viewX1, pView_attr->viewY1);
	}
	else if(pSupListPriv->backPicId)
	{//�ñ�������
		Grap_ClearScreen(pView_attr->viewX1, pView_attr->viewY1, (U16)(pView_attr->viewX1+pView_attr->viewWidth), 
			(U16)(pView_attr->viewY1+pView_attr->viewHeight));
	}
	
	//updateInfo(pSupList);
	
	//��������
	//if(pSupListPriv->ListInfo & NEED_TITLE)
		pSupList->SupListDrawTitle(pSupList);

	
	//��������
	if(pSupListPriv->ListInfo & NEED_SCRBAR)
		pSupList->SupListDrawSrc(pSupList);
	
	for (i = 0; i < pSupListPriv->itemScreenNum; i++)
	{
		if(pSupListPriv->topItemNo + i >= pSupListPriv->totalItem)
			break;
		pSupList->SupListDrawOne(pSupList, (U16)(pSupListPriv->topItemNo + i));
	}
	
	pSupListPriv->TimerLft = pSupListPriv->TimerTot;
	pSupListPriv->firstCharPos = 0;
	
	//����ǵ�ǰ����,�������滭�߿�
	//Grap_DrawFocusRect(pView_attr, COLOR_RED);
	
	return SUCCESS;
}

S32 Grap_SupListHandle(void *pView, GRAP_EVENT *pEvent)
{
	S32 ret = SUCCESS;
	U32 message, wParam, lParam ;
	tSUPPERLIST		*pSupList;
	tSupListPriv	*pSupListPriv;
	tItemInfo		*pItemInfo;
	
	pSupList = (tSUPPERLIST *)pView;
	pSupListPriv = &(pSupList->SupList_priv);
	
	message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	
	switch(message)
	{
	case VIEW_INIT:
		pSupList->SupListInt(pSupList);
		break;
	case VIEW_UPDATE:
		pSupList->SupListUpDate(pSupList);
		pSupList->draw(pSupList,0,0);
		break;
	case VIEW_PRESS:
		{
			if(1 == pSupList->SupkeyInput(pSupList,lParam))
			{
				//����������
				return PARSED;
			}
		}
		if((pSupList->view_attr.curFocus == 1 || pEvent->reserve == 1)/* && cKEYB_EVENT_RELEASE == wParam*/)//����ʱ��ӦOK��
		{
			switch(lParam)
			{
			case KEYEVENT_OK:
				ret = pSupList->SupListEnter(pSupList, pSupListPriv->focusItemNo);
				pSupList->SupListUpDate(pSupList);
				switch( ret )
				{
				case RETURN_REDRAW:
					ReDraw(pSupList, 0, 0);
					ret = PARSED;
					break;
				case RETURN_CANCLE:
					break;
				case RETURN_QUIT:
					*((tGRAPDESKBOX *)pSupList->pParent)->pQuitCnt = 1;	//should not recall desk message
					ret = PARSED;
					break;
				case RETURN_FOCUSMSG:
					return ret;
					break;	
				}
				break;
			case KEYEVENT_UP:
				pSupList->SupListLineUp(pSupList,1);
				ret = PARSED;
				break;
			case KEYEVENT_DOWN:
				pSupList->SupListLineDown(pSupList,1);
				ret = PARSED;
				break;
			case KEYEVENT_LEFT:
				pSupList->SupListPageUp(pSupList);
				ret = PARSED;
				break;
			case KEYEVENT_RIGHT:
				pSupList->SupListPageDown(pSupList);
				ret = PARSED;
				break;
			default:
				break;
			}
		}
		break;
	case VIEW_TIMER:
		if( ((pSupListPriv->ListInfo & NEED_TIMER) || pSupList->timerEn)  && pSupListPriv->totalItem > 0 && 
			 pSupListPriv->focusItemNo >= pSupListPriv->topItemNo && 
			 pSupListPriv->focusItemNo < pSupListPriv->topItemNo+pSupListPriv->itemScreenNum)
		{
			if (pSupListPriv->TimerLft == 0)
			{
				pSupListPriv->TimerLft = pSupListPriv->TimerTot;
				//��Ҫ�ж��Ƿ��ڵȴ�����
				if(pSupList->lastKey != 0)
				{
					pSupList->SupkeyInput(pSupList,0xFF);//0xff��ʾ��timer�����
				}
				else
				{
					pSupList->SupListTimer(pSupList, pSupListPriv->focusItemNo);
				}
				
				//����ǵ�ǰ����,�������滭�߿�
				//Grap_DrawFocusRect(&(pSupList->view_attr), COLOR_RED);
			}
			else
			{
				S32 timeleft;
				
				timeleft =  pSupListPriv->TimerLft - pEvent->lParam;
				
				if (timeleft < 0)  pSupListPriv->TimerLft = 0;
				else pSupListPriv->TimerLft = timeleft;				
			}
		}
		break;
	case VIEW_DRAW:
		pSupList->draw(pSupList,0,0);
		break;
	case VIEW_DELETE:
		pSupList->SupListDelete(pSupList);
		pSupList->draw(pSupList,0,0);
		break;
	case VIEW_REV_SYSMSG:
		pSupList->recvSysMsg(pSupList);
		break;
	case VIEW_REV_PULLMSG:
		ret = pSupList->recvPullMsg(pSupList,pEvent);
		break;
	default:
		break;
	}
	
	return ret;
}

S32 Grap_SupListDestroy(void *pView)
{
	tSUPPERLIST	*pSupList;
	
	pSupList = (tSUPPERLIST	*)pView;
	if(pSupList->pItemInfoH != NULL)
	{
		ItemInfo_Destroy(pSupList->pItemInfoH);
		pSupList->pItemInfoH = NULL;
		pSupList->pItemInfoT = NULL;
	}
	
	free((void*)pSupList);
	return SUCCESS;
}


tSUPPERLIST *Grap_CreateSupList( tGrapViewAttr *pview_attr, tSupListPriv *plst_privattr)
{
	tGRAPVIEW	*pParent;
	tSUPPERLIST	*pSupList;
	

	pSupList = (tSUPPERLIST *)malloc(sizeof(tSUPPERLIST));
	if (NULL == pSupList)	return NULL;
	memset((char *)pSupList,0,sizeof(tSUPPERLIST));
	
	
	pSupList->view_attr	= *pview_attr;
	pSupList->SupList_priv = *plst_privattr;
	
	
	pSupList->handle			= Grap_SupListHandle;
	pSupList->draw				= Grap_SupListDraw;
	pSupList->destroy			= Grap_SupListDestroy;
	pSupList->recvSysMsg		= Grap_SupListSysMsg;
	pSupList->recvPullMsg		= Grap_SupListPullMsg;
	
	pSupList->SupListInt		= Grap_SupListInt;
	pSupList->SupListUpDate		= Grap_SupListUpDate;
	pSupList->SupListTimer		= Grap_SupListTimer;
	pSupList->SupListLineUp		= Grap_SupListLineUp;
	pSupList->SupListLineDown	= Grap_SupListLineDown;
	pSupList->SupListPageUp		= Grap_SupListPageUp;
	pSupList->SupListPageDown	= Grap_SupListPageDown;
	pSupList->SupListGetStr		= Grap_SupListGetStr;
	pSupList->SupListDrawIcon	= Grap_SupListDrawIcon;
	pSupList->SupListDrawOne	= Grap_SupListDrawOne;
	pSupList->SupListDrawSrc	= Grap_SupListDrawSrc;
	pSupList->SupListDrawTitle	= Grap_SupListDrawTitle;
	pSupList->SupListEnter		= Grap_SupListEnter;
	pSupList->SupListDelete		= Grap_SupListDelete;
	pSupList->SupListDelOpen	= Grap_SupListDelOpen;
	pSupList->SupListUpDateByTotal	= Grap_SupListUpDateByTotal;
	pSupList->SupkeyInput		= Grap_SupListKeyInput;
	//pSupList->popupmenu 	= Grap_SupListPopupMenu;
	//pSupList->Copy			= Grap_SupListPopCopy;
	//pSupList->Paste			= Grap_SupListPopPaste;
	//pSupList->Delete		= Grap_SupListPopDelete;
	
	pSupList->pItemInfoH	= NULL;
	pSupList->pItemInfoT	= NULL;
	
	pSupList->pullFlag = 0;
	pSupList->timerEn = 0;
	pSupList->lastKey = 0;
	
	return pSupList;
}
tSUPPERLIST *Grap_InsertSupList(void *pView, tGrapViewAttr *pview_attr, tSupListPriv *plst_privattr)
{
	tGRAPDESKBOX	*pParent;
	tSUPPERLIST	*pSupList;
	
	pParent = (tGRAPDESKBOX *)pView;
	if (NULL == pParent) return NULL;
	pSupList = (tSUPPERLIST *)malloc(sizeof(tSUPPERLIST));
	if (NULL == pSupList)	return NULL;
	
	memset((char *)pSupList,0,sizeof(tSUPPERLIST));
	pSupList->pPrev   = pParent->pTail;
	pParent->pTail->pNext = (tGRAPVIEW *)pSupList;
	pParent->pTail = (tGRAPVIEW *)pSupList;
	pSupList->pParent = pParent;
	
	//pSupList->pNext = pParent->pNext;
	//pParent->pNext  = (tGRAPVIEW *)pSupList;
	//pSupList->pParent = pParent;
	
	pSupList->view_attr	= *pview_attr;
	pSupList->SupList_priv = *plst_privattr;
	
	pSupList->handle			= Grap_SupListHandle;
	pSupList->draw				= Grap_SupListDraw;
	pSupList->destroy			= Grap_SupListDestroy;
	pSupList->recvSysMsg		= Grap_SupListSysMsg;
	pSupList->recvPullMsg		= Grap_SupListPullMsg;
	
	pSupList->SupListInt		= Grap_SupListInt;
	pSupList->SupListUpDate		= Grap_SupListUpDate;
	pSupList->SupListTimer		= Grap_SupListTimer;
	pSupList->SupListLineUp		= Grap_SupListLineUp;
	pSupList->SupListLineDown	= Grap_SupListLineDown;
	pSupList->SupListPageUp		= Grap_SupListPageUp;
	pSupList->SupListPageDown	= Grap_SupListPageDown;
	pSupList->SupListGetStr		= Grap_SupListGetStr;
	pSupList->SupListDrawIcon	= Grap_SupListDrawIcon;
	pSupList->SupListDrawOne	= Grap_SupListDrawOne;
	pSupList->SupListDrawSrc	= Grap_SupListDrawSrc;
	pSupList->SupListDrawTitle	= Grap_SupListDrawTitle;
	pSupList->SupListEnter		= Grap_SupListEnter;
	pSupList->SupListDelete		= Grap_SupListDelete;
	pSupList->SupListDelOpen	= Grap_SupListDelOpen;
	pSupList->SupListUpDateByTotal	= Grap_SupListUpDateByTotal;
	pSupList->SupkeyInput		= Grap_SupListKeyInput;
	//pSupList->popupmenu 	= Grap_SupListPopupMenu;
	//pSupList->Copy			= Grap_SupListPopCopy;
	//pSupList->Paste			= Grap_SupListPopPaste;
	//pSupList->Delete		= Grap_SupListPopDelete;
	
	pSupList->pItemInfoH	= NULL;
	pSupList->pItemInfoT	= NULL;
	
	pSupList->pullFlag = 0;
	pSupList->timerEn = 0;
	pSupList->lastKey = 0;
	
	return pSupList;
}