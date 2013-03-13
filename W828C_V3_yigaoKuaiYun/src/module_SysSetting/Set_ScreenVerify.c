#include "SysSetting.h"


/*-----------------------------------------------------------------
marco def
-----------------------------------------------------------------*/
#define POINT_VERIFY_MIN_OFFSET	 (150)
#define POINT_VERIFY_MAX_OFFSET  (3000)
/*-----------------------------------------------------------------
globe para :
-----------------------------------------------------------------*/
static U8     sAdjFlag = 0; //������ʱ���Ƿ���Ҫ����������
static U8     ADnumber = 0; //��Ļ�����л���
tPOINT RectBuf[5]  ; //ǰ5��Ϊԭʼ�㣬��5��Ϊ�����ĵ�

/*
*������Ĺ�������
*/
const U16 AdjScrPoint[][6]=
{
	{  0,  0, 0,  0,  240,  320},
	{216,  0, 0,  0,  240,  320},
	{  0,296, 0,  0,  240,  320},
	{216,296, 0,  0,  240,  320},
	{107,147, 0,  0,  240,  320}
};
const tPOINT srcPoint[5]=
{
	{12,  12},
	{227, 12},
	{12,  307},
	{227, 307},
	{118, 158}
};
/* У��Сͼ�� 24X24 */
const U8 picBitmap[72] = 
{
	0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,
	0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,
	0x00,0xFF,0xE7,0xFF,0xFF,0xE7,0xFF,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,
	0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,
	0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00
};


/*==============================================================================*/
const tGrapViewAttr    gtLcdCheckVAttr[] = 
{	
	{COMM_DESKBOX_ID, 	0,  0, 240,320,   0,  0,  0,  0, 	0,0,0,0,1,0,0,0},/*����   */
	{BUTTON_ID_1, 		0,184, 240, 50,   0,  0,  0,  0, 	0,0,0,0,1,0,0,0},/*��ʾ   */
	{BUTTON_ID_2, 		0,  0,  27, 25,   0,  0,240,320,  	0,0,0,0,1,0,0,0},
};
const tGrapButtonPriv gtLcdCheckBPro[]=
{
	{NULL,COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC,0,NULL},
	{NULL,COLOR_STR_UNSELECT,COLOR_STR_BARSELECT,0,0,SHOWSTRING_LIAN_MID,0,0,NULL}
};

S32 SetLcdVerify_ButnDraw0(void *pView, U32 xy, U32 wh)
{		
	return SUCCESS;
}

S32 SetLcdVerify_ButnDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tGrapButtonBuffPic *pbtnpic;
	U16 viewX1,viewY1,viewX2,viewY2;

	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
	
	viewX1 = pview_attr->viewX1;
	viewY1 = pview_attr->viewY1;
	viewX2 = viewX1 + pview_attr->viewWidth;
	viewY2 = viewY1 + pview_attr->viewHeight;
	drvLcd_SetColor(COLOR_WHITE,COLOR_BLACK);
	drvLcd_ClearDisplayMemory(0, 0, gLcd_Total_Row, gLcd_Total_Column);
	drvLcd_SetColor(COLOR_BLACK,COLOR_BLACK);
	gu16Lcd_ColumnLimit=gLcd_Total_Column;
	drvLcd_DisplayString(184,24,0,(U8*)"������У������ͼ������", NULL, 0);
	drvLcd_DisplayString(202,24,0,(U8*)"     �����ؼ��˳�", NULL, 0);
	drvLcd_WriteBMPColor(picBitmap,24,24,viewY1,viewX1);
	//Grap_Clear_Screen(viewX1, viewY1, viewX2,viewY2);
		
	return SUCCESS;
}


/*---------------------------------------------------------
*����: SetLcdVerify_ButnEnter
*����: ��ĻУ���㷨 ԭ�����ǵ��������ɼ�����ADCֵ, ADCֵ��X����,Y��������
*                         ���һ����λADCֵ����Ӧ�����صĸ�����Ϊϵ��
*����: wu
*����: ��
*---------------------------------------------------------*/
S32 SetLcdVerify_ButnEnter(void *pView, U16 state)
{
	tGRAPBUTTON    *pBtn ;
	int flag = 0;
	U32 dertx , derty ;
	U16 pointX, pointY;
	
	int rv = HY_OK;
	int px,py;

    pBtn=(tGRAPBUTTON*)pView;
    
    if (state == STATE_NORMAL && BUTTON_ID_1 == pBtn->view_attr.id)
	{
		return RETURN_QUIT ;
	}
    
    ts_get_cur_adc(&px,&py);
    
	RectBuf[ADnumber].x = px;
	RectBuf[ADnumber].y = py;
	switch(ADnumber)
	{
	case 1:
		dertx = ABS((S16)(RectBuf[1].x-RectBuf[0].x));
		derty = ABS((S16)(RectBuf[0].y-RectBuf[1].y));
		//�ڶ������һ��Ƚ� y ֵ			
		if (dertx <= POINT_VERIFY_MIN_OFFSET && derty > POINT_VERIFY_MAX_OFFSET)
		{
			flag = 1;
		}
		break;
	case 2:
		dertx = ABS((S16)(RectBuf[2].x-RectBuf[0].x));
		derty = ABS((S16)(RectBuf[2].y-RectBuf[0].y));
		//���������һ��Ƚ� x ֵ
		if (derty <= POINT_VERIFY_MIN_OFFSET && dertx > POINT_VERIFY_MAX_OFFSET)
		{
			flag = 1;
		}
		break;
	case 3:
		dertx = ABS((S16)(RectBuf[3].x-RectBuf[2].x)) ;
		derty = ABS((S16)(RectBuf[3].y-RectBuf[1].y)) ;
		//���ĵ���ڶ���Ƚ� x ֵ���������Ƚ� y ֵ
		if (dertx <= POINT_VERIFY_MIN_OFFSET && derty <= POINT_VERIFY_MIN_OFFSET)
		{
			dertx = ABS((S16)(RectBuf[3].x-RectBuf[1].x)) ;
			derty = ABS((S16)(RectBuf[3].y-RectBuf[2].y)) ;
			if(dertx > POINT_VERIFY_MAX_OFFSET && derty > POINT_VERIFY_MAX_OFFSET)
			{
				flag = 1;
			}
		}
		break;
	case 4:
		//������ x ֵ���һ��͵��ĵ�� x ƽ��ֵ�Ƚ�
		//������ y ֵ���һ��͵��ĵ�� y ƽ��ֵ�Ƚ�
		pointX = (RectBuf[0].x+RectBuf[3].x)/2;
		pointY = (RectBuf[0].y+RectBuf[3].y)/2;
					
		if ((ABS((S16)(RectBuf[4].x-pointX)) <= POINT_VERIFY_MIN_OFFSET) &&
			(ABS((S16)(RectBuf[4].y-pointY)) <= POINT_VERIFY_MIN_OFFSET))
		{							    
			
			//������ x ֵ��ڶ���͵������ x ƽ��ֵ�Ƚ�
			//������ y ֵ��ڶ���͵������ y ƽ��ֵ�Ƚ�
			pointX = (RectBuf[1].x+RectBuf[2].x)/2;
			pointY = (RectBuf[1].y+RectBuf[2].y)/2;
			
			if ((ABS((S16)(RectBuf[4].x-pointX)) <= POINT_VERIFY_MIN_OFFSET) &&
				(ABS((S16)(RectBuf[4].y-pointY)) <= POINT_VERIFY_MIN_OFFSET))
			{
				
				//����ƫ��
				S16 offset_x, offset_y;
				float coff_x, coff_y;
				int rc, i;
				int width, height;
				
				rc = ts_calculate_offset_coef(&RectBuf[0],&srcPoint[0], &offset_x, &offset_y, &coff_x, &coff_y);
				if (rc == HY_OK)
				{
					gtHyc.touchParam.TouchOffsetX = offset_x;
					gtHyc.touchParam.TouchOffsetY = offset_y;
					gtHyc.touchParam.TouchCoffX = coff_x;
					gtHyc.touchParam.TouchCoffY = coff_y;
					ts_parameter_set(&gtHyc.touchParam);
					
					ADnumber = 0;
					glbVariable_SaveParameter();
					
					Com_SpcDlgDeskbox("��ĻУ�����!",0,pBtn->pParent,1,NULL,NULL,100);
                    
                    return RETURN_QUIT;   //������һ��
                }
            }
		}
		break;
	default:
		flag = 1;
		break;
	}
	
	if (flag == 1)
		ADnumber++;
	else
		ADnumber = 0;
	pBtn->view_attr.viewX1	= AdjScrPoint[ADnumber][0];
	pBtn->view_attr.viewY1	= AdjScrPoint[ADnumber][1];
	pBtn->view_attr.touchX1	= AdjScrPoint[ADnumber][2];
	pBtn->view_attr.touchY1	= AdjScrPoint[ADnumber][3];
	pBtn->view_attr.touchX2	= AdjScrPoint[ADnumber][4];
	pBtn->view_attr.touchY2	= AdjScrPoint[ADnumber][5];
  
  return RETURN_REDRAW;
}

S32 SetLcdVerify_ButtonRecvPullMsg(void *pView,GRAP_EVENT *pEvent)
{
    U32 extParam, message, wParam, lParam ;
    tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tGrapButtonBuffPic *pbtnpic;
	tGRAPVIEW *pTempView;
	S32 ret = FAIL;
	int x, y ;

	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
    
    message = pEvent->message ;
	wParam = pEvent->wParam ;
	lParam = pEvent->lParam ;
	extParam = pEvent->extParam;

	y = HIWORD(lParam) ;
	x = LOWORD(lParam) ;
	
	if (VIEW_PULL_START == extParam)
	{//����ʼ��
	     ret = VIEW_PULL_START;
	}
	
	if (VIEW_PULL_START == extParam)
	{//down
	}
	else if (VIEW_PULL_ING == extParam)
	{//��ק��
	    if(GRAP_INWIDGET_RANGE(x,y,pButton))
	     {//����Ч��Χ��
	        	
		     ret = VIEW_PULL_ING; 
	     }
	     else
	     {
	        pButton->state = STATE_NORMAL;
		    pButton->draw(pButton,0,0);
	     }
	}
	else if (VIEW_PULL_END == extParam)
	{//up
	    
		ret = pButton->pressEnter(pView, pButton->state) ;
		if ( ret== RETURN_REDRAW)
		{
			ReDraw(pButton, 0, 0);
			ret = VIEW_PULL_END;
		}
		else if(ret == RETURN_QUIT)
		{
			*((tGRAPDESKBOX *)(pButton->pParent))->pQuitCnt = 1;
		}
		else if (ret == RETURN_FOCUSMSG)
		{
			return ret;
		}			
		return PARSED;
	}
	else if (VIEW_PULL_TIMEOUT == extParam)
	{//��ʱ
	    pButton->state = STATE_NORMAL;
		pButton->draw(pButton,0,0); 
		
		ret = VIEW_PULL_TIMEOUT;  
	}
	
	return ret;  
 
}

/*---------------------------------------------
*����:Set_ScreenVerify
*����:��ĻУ��
*����:
*����:
*-----------------------------------------------*/
void Set_ScreenVerify(void *pDeskFather)
{
	tGRAPDESKBOX   *pDesk;
	tGRAPBUTTON    *pBtn;
    tGRAPLABEL     *pLabel;
    U8              i;
    
	drvLcd_SetColor(COLOR_WHITE,COLOR_BLACK);
	drvLcd_ClearDisplayMemory(0, 0, gLcd_Total_Row, gLcd_Total_Column);
	drvLcd_SetColor(COLOR_BLACK,COLOR_BLACK);
	gu16Lcd_ColumnLimit=gLcd_Total_Column;
	drvLcd_DisplayString(184,24,0,(U8*)"������У������ͼ������", NULL, 0);
	drvLcd_DisplayString(202,24,0,(U8*)"     �����ؼ��˳�", NULL, 0);
	DisplayData2Screen();
    
    pDesk  =  Grap_CreateDeskboxNoCom(&gtLcdCheckVAttr[0], 0); 
	if ( NULL == pDesk ) return;
	
	Grap_Inherit_Public(pDeskFather,pDesk);

	pBtn = Grap_InsertButton(pDesk, &gtLcdCheckVAttr[1],&gtLcdCheckBPro[0]);
	if(NULL == pBtn) return;
	pBtn->pressEnter = SetLcdVerify_ButnEnter;
	pBtn->draw = SetLcdVerify_ButnDraw0 ;
	
	pBtn = Grap_InsertButton(pDesk, &gtLcdCheckVAttr[2],&gtLcdCheckBPro[1]);
	if(NULL == pBtn) return;
	pBtn->pressEnter = SetLcdVerify_ButnEnter;
	pBtn->draw = SetLcdVerify_ButnDraw ;
	pBtn->recvPullMsg = SetLcdVerify_ButtonRecvPullMsg;

    Grap_DeskboxRun(pDesk);
	Grap_DeskboxDestroy(pDesk);
	
	return ;
}
