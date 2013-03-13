/*----------------------�ļ�����--------------------------------
*��������: 08/03/12
*����    : ��ʾ��������
*--------------------------------------------------------------*/

/*
*include files
*/
#include "std.h"
#include "grap_api.h"
#include "base.h"
#include "LcdAPIFunc.h"
#include "glbVariable_base.h"
#include "glbVariable.h"
#include "LcdModule_BackLight.h"
#include "Common_BmpData.h"
#include "Battery.h"
#include "hyhwCkc.h"
#include "LedCtl.h"

/*
*define variable
*/
const U8 * const gu8EngWeek[]=
{
    (U8 * const )"��",(U8 * const )"һ", (U8 * const )"��", (U8 * const )"��", (U8 * const )"��",
    (U8 * const )"��", (U8 * const )"��"
};

//USB������Ϣ
const U8  * const pUSB_ChargeString[]=
{
	//���ڳ��...
	(U8 *const) "���ڳ��",

     //����ѳ���!
    (U8 *const) "����ѳ���!",

    //����ѳ���,��ε������!	
    (U8 *const) "����ѳ���,��ε������!"
};


//����USB��������һ����ʾʱ��Ҫ��ʾ��ͼƬ
//static U8 gu8UsbInitFlag = 0;

const U32 gu32USBCtn[]={CHARGE_CHARGE1_EN,CHARGE_CHARGE2_EN,CHARGE_CHARGE3_EN,CHARGE_CHARGE4_EN,
                  CHARGE_CHARGE5_EN,CHARGE_CHARGE6_EN,CHARGE_CHARGE7_EN,CHARGE_CHARGE8_EN};

                 
/*
*���ؼ��Ĺ�������
*/
const tGrapViewAttr gtUSBViewAttr[]=
{
	{COMM_DESKBOX_ID,	0,	0,	cLCD_TOTAL_COLUMN,	cLCD_TOTAL_ROW,	0,	0,	cLCD_TOTAL_COLUMN,	cLCD_TOTAL_ROW,	
	 TY_ON_20_EN,	TY_ON_20_EN, 0,0, 1,	0,	0,	0},

	{CARTOON_ID_1,	 48,125,159,100,  0,  0,  0,  0,	
	 TY_ON_20_EN,	TY_ON_20_EN, 0,0, 1,	0,	0,	0},
	 
	//{PICTURE_ID_1,	102,235, 48, 30,  0,  0,  0,  0,	
	{PICTURE_ID_1,	95,247, 64, 34,  0,  0,  0,  0,	
	 TY_ON_20_EN,	TY_ON_20_EN, 0,0, 1,	0,	0,	0},

};


/*
*���嶯����˽������
*/
const tGrapCartoonPriv gtUSBCtn[]=
{
	{0,1,20,0,8,0,(U32 *)&gu32USBCtn,TY_ON_20_EN},
};

//��ʼ����ʱ��
static U32 gu32ChargeStartTime = 0;
static U32 gu32ChargeFsm = 0; //����Ҫ������ı�־��ֻ����ʾָ����ʱ��

static U8 gu8ReserveFlag = 0;
static U8 gu8UsbInFlag = 0 ;  /* USB�Ƿ�����־,����UI����л�ʹ�� */

#define USB_NONEED_CHARGE_TIME    (8000)	 //80S
#define USB_NEED_CHARGE_TIME      (60000) //10 min


//����һЩ״̬
#define USB_INI_CHARGE_FLAG       0
#define USB_NONEED_CHARGE_FLAG    1
#define USB_NEED_CHARGE_FLAG      2
#define USB_DEAD_CHARGE_FLAG      3

static void USB_CartoonTimerDraw(int type);


/*---------------------------------------------------------
*����: usb_inflag_set
*����: ����USB�����־
*����: none
*����: none
*---------------------------------------------------------*/
void usb_inflag_set(U8 flag)
{
	gu8UsbInFlag = flag ;
}

/*---------------------------------------------------------
*����: usb_inflag_get
*����: ��ȡUSB�����־
*����: none
*����: none
*---------------------------------------------------------*/
U8 usb_inflag_get(void)
{
	return gu8UsbInFlag;
}


/*---------------------------------------------------------
*����: usb_charge_intial
*����: ��ʼ������    
*����: none
*����: none
*---------------------------------------------------------*/
void usb_charge_intial(void)
{
    int lev;
    
    gu32ChargeStartTime = rawtime(NULL);
    lev = Battery_Level();	
	if (lev == 5)
	{//ֻ�趯����ʾ1���Ӿ���ʾ������������Ҫ�����������������ʾ30���ӵĶ�������ʾ������
	    gu32ChargeFsm = USB_NONEED_CHARGE_FLAG;
	}
	else
	{
	    gu32ChargeFsm = USB_INI_CHARGE_FLAG;
	} 
}

/*---------------------------------------------------------
*����: usb_need_charge
*����: �ж��Ƿ���Ҫ���     
*����: none
*����: 1:��Ҫ 0������Ҫ
*---------------------------------------------------------*/
int usb_need_charge(void)
{
    int rc = 1;
    
    if (USB_DEAD_CHARGE_FLAG == gu32ChargeFsm)
    {
        rc = 0;
    }
    
    return rc;    
}

/*---------------------------------------------------------
*����: usb_updata_charge_fsm
*����: ���³��״̬��     
*����: none
*����: ��ǰ״̬��״̬
*---------------------------------------------------------*/
U32 usb_updata_charge_fsm(void)
{
    int lev;
    /*	ʹ�õ�ǰ����ֵ����ֹ���ڲ�ֵ��ΧBATTERY_LEV_DIFFERENCE
    	�����²��ܸ��³�����
    */
    lev = Battery_CurrentLevel();
    
    if (5 == lev && 1 == Battery_DcInCheck()) /* ��ѹ����оƬ��Ϊ���� */
    {
        switch(gu32ChargeFsm)
        {
            case  USB_INI_CHARGE_FLAG://��ʼ״̬
                gu32ChargeFsm = USB_NEED_CHARGE_FLAG;
                gu32ChargeStartTime = rawtime(NULL); 
                //������ʾ����ֵΪ��ǰ���ֵ(����������)
                Battery_UpdateLevel();
                break;
            case  USB_NONEED_CHARGE_FLAG://�����͵�����
                if (ABS(rawtime(NULL)-gu32ChargeStartTime) > USB_NONEED_CHARGE_TIME)
                {
                    gu32ChargeFsm = USB_DEAD_CHARGE_FLAG;    
                }
                break;
            case USB_NEED_CHARGE_FLAG://�����ѳ��� ��Ҫ����
                if (ABS(rawtime(NULL)-gu32ChargeStartTime) > USB_NEED_CHARGE_TIME)/* ���ݵ�ѹ��Ϊ������ */
                {
                    gu32ChargeFsm = USB_DEAD_CHARGE_FLAG;
                }
                break;    
            default:
                break;         
        }
    }
    else
    {//��������ʱ�ı�״̬��
        gu32ChargeFsm = USB_INI_CHARGE_FLAG;
    }

    return gu32ChargeFsm;    
}

/*---------------------------------------------------------
*����: usb_check_resource_quict
*����: ����Ƿ��Ѿ������USB״̬�ˣ���Ҫ����û����Դ�������    
*����: none
*����: 1 ���� ���� 0
*---------------------------------------------------------*/
int usb_check_resource_quit(void)
{
     int rv = 1;
     
     if (GetResouceBmpOffset() == -1)
     {
         if (gu8ReserveFlag != 0)
         {
             rv = 1;
         }
         else
         {
             rv = 0;
         }    
     }
     
     return rv;
}

static void hyAuthen_UsbErrorHandle(void)
{
     exit(1);
}

/*---------------------------------------------------------
*����: USB_InitialUSB
*����: �ж�USB״̬��copy USB����      
*����: none
*����: none
*---------------------------------------------------------*/
int USB_InitialUSB(void *pView)
{
	static U32 sysTickA = 0;
	U32 sysTickB;
	int ret;	
	tGRAPDESKBOX *pDesk;

	pDesk = (tGRAPDESKBOX *)pView;
	
	sysTickB = rawtime(NULL);
	
	if (USBHandler_GetScsiFlag() != 2)
	{//USB״̬��δ�жϳ��Ƿ�ΪUSB	     
	    if (bUSB_Status == FALSE)
	    {
	        ConSume_UpdataSpeedDirect_USB(240);
	        
	        Shell_StopService();
	        
	        {
            	char *pRandomBuf;
            	pRandomBuf = malloc(128);
            	hyAuthen_reg(NULL, hyAuthen_UsbErrorHandle);
            	hyAuthen_verify(1, pRandomBuf, NULL);
            	free(pRandomBuf);
        	}
        	
        	usb_UpdataSpeed_Config(ConSume_UpdataSpeedDirect_USB);
	        gs32VolumeFd = USBHandler_USBInit();
	        if (gs32VolumeFd >= 0)
	        {
		        if (USBHandler_USBConnect() == 0)
		        {
				    USBHandler_USB_DeInit(gs32VolumeFd);
				    gs32VolumeFd = -1;
		        }
		        else
		        {
	            	bUSB_Status = TRUE;
	            }
	        }
	        else
	        {
	        	USBHandler_SetScsiFlag(2);
	        }
	    }
	    
	    if (bUSB_Status == TRUE)
	    {
	       /* USB Stack is working in polled mode */
	        hyudsDcd_UsbDispatch();
	        if (USBHandler_GetScsiFlag() == 1)
	        {
	        	//hyhwTimer_countEnable(4);
	        	bUSB_Status = TRUE;
	        }
	        
	        if (ABS(sysTickB - sysTickA) >= 5)
	        {   	        
		        ret = USBHandler_Application(&gs32VolumeFd);
		        sysTickA = sysTickB;    
	            if (ret == 0)
	            {//�˳�USB����PC���˳����ڰγ�USB�ߣ��۲������������
	                
	                USBHandler_USB_DeInit(gs32VolumeFd);
	                gs32VolumeFd = -1;
	                bUSB_Status = FALSE;
	                USBHandler_SetScsiFlag(2);
	            }
	        }  
	    }
	}	
	
	
	if (FALSE == bUSB_Status && USBHandler_USBConnect() == 0 && 1 == usb_check_resource_quit())
    {    
	    USBHandler_StopDetectUSBStatus();
	    USBHandler_SetScsiFlag(0);
	    gu8UsbInFlag = 0;        /* ���־ */
	    if(0 == Battery_DcInCheck())
	    {
	    	*pDesk->pQuitCnt = 0xff;
	    }
	    return 0;
    }
    return 1;
}


/*---------------------------------------------------------
*����: Usb_PicHandle
*����: USB handle ����      
*����: none
*����: none
*---------------------------------------------------------*/
static S32 Usb_CartonHandle(void *pView, GRAP_EVENT *pEvent )
{
	U32 message,lParam;
	tGrapViewAttr  *pview_attr;
	tGRAPCARTOON *pCartoon;
	tGRAPDESKBOX *pDesk;
	tGrapCartoonPriv *pcartoon_privattr;
	GRAP_EVENT Event;
	
	pCartoon = (tGRAPCARTOON *)pView;
	pview_attr = &(pCartoon->view_attr);
	pcartoon_privattr = &(pCartoon->cartoon_privattr);
	
	pDesk = (tGRAPDESKBOX *)pCartoon->pParent;
	
	message = pEvent->message;	
	lParam = pEvent->lParam;

	if(1 == gu8UsbInFlag)
	{
		if (USB_InitialUSB(pDesk) == 0)
		{
			return SUCCESS;
		}
	}
	else if(0 == Battery_DcInCheck()) /* ��USB���� �� ���γ� */
	{
		*pDesk->pQuitCnt = 0xff;
		return SUCCESS;
	}
	switch(message)
	{
	case VIEW_DRAW:
		pCartoon->draw(pCartoon, 0, 0);
		break;
	case VIEW_TIMER:
		
		if (pcartoon_privattr->lftTimeIntv == 0)
		{
			pcartoon_privattr->lftTimeIntv = pcartoon_privattr->timeIntv;	//reset timer
			pcartoon_privattr->curFrame++;
			pCartoon->draw(pCartoon, 0, 0);
		}
		else
		{
			S32 timeleft;
			
			timeleft = pcartoon_privattr->lftTimeIntv - lParam;
			
			if (timeleft < 0) pcartoon_privattr->lftTimeIntv = 0;
			else  pcartoon_privattr->lftTimeIntv = timeleft;
		}	
		break;
	case VIEW_PRESS:
		/*if (KEYEVENT_ESC == pEvent->lParam && USBHandler_GetScsiFlag() == 2)
		{//�Ͽ�USB״̬�£����п���
			*pDesk->pQuitCnt = 0xff;
		}*/
		break;
	default:
		break;
	}
	
	return SUCCESS;
}


// ���봰����Ϣѭ����
S32 Grap_USBDeskboxRun(tGRAPDESKBOX *pDesk)
{
	S32		ret = -1;
	U16     i = 0 ;
	U32     len;
	GRAP_EVENT	event;
	tGRAPVIEW	*pSubView ;
	tGRAPZORDER		*pSubZorder, *pTempZorder;
	tGRAPCARTOON *pCartoon;
	
	Grap_Set_Current_Desk(pDesk);
	
	pSubView = pDesk->pNext ;
	while(pSubView)
	{
		pTempZorder = (tGRAPZORDER *)malloc(sizeof(tGRAPZORDER)) ;
		if(!pTempZorder)
		{
			return 0;
		}
		pTempZorder->pMember = pSubView ;
		pTempZorder->zOrder = i++;
		pTempZorder->pNext = NULL ;
		
		if(!(pDesk->pZorderList))
		{
			pDesk->pZorderList = pTempZorder ;
			pSubZorder = pDesk->pZorderList ;
		}
		else
		{
			pSubZorder->pNext = pTempZorder ;
			pSubZorder = pSubZorder->pNext ;
		}

		pSubView = pSubView->pNext ;
	}

    
	if (GetResouceBmpOffset() != -1)
	{
        Grap_ShowPicture(TY_ON_20_EN,0,0);
        
        USB_CartoonTimerDraw(1);
       
        pCartoon = Grap_GetViewByID(pDesk,CARTOON_ID_1);
        pCartoon->draw(pCartoon,0,0); 
    	DisplayData2Screen();
	}

	LcdModule_LightBack();
	
	if (GetResouceBmpOffset() != -1) 	
	{    
	    hyhwLcd_lightSet(gu8Lcd_BackLight[0]);
	    gtHyc.ScreenProtect = 6;
	    BackLight_UpdataLightTime(gtHyc.ScreenProtect);
	}
	
	Led_ctrl(LED_CLOSE);
	while (1)
	{		
		event.message = 0;  
			
		ret = msg_queue_tryget(pDesk->pGetMsg,(char *)pDesk->ptGetMsgBody,&len);
			
		if (ret >= MSG_QUEUE_OK)
		{
			Grap_DealEvent(pDesk,&event,pDesk->ptGetMsgBody);
		}
		
		ret = pDesk->handle(pDesk, &event);
		
		if (*pDesk->pQuitCnt > 0)
		{
			(*pDesk->pQuitCnt)--;
			break;
		}
		if(1 == Battery_DcInCheck())
		{
			if(gu32ChargeFsm != USB_DEAD_CHARGE_FLAG) 
			{
				Led_ctrl(LED_RED);
			}
			else //����
			{
				Led_ctrl(LED_CLOSE);
				BackLight_UpdataLightTime(0);
				BackLight_EventTurnOnLcd(pDesk);
			}
		}		
	}
	return ret;
}

//��Ҫ����ʱ�����ˢ��
static void USB_CartoonTimerDraw(int type)
{
	RtcTime_st tRtc_solar;
	tStrDispInfo disp_info;
	int col;
	int index;
	char temp_arrary[50]={0};
	static U32 lastTick = 0;
	U32 curTick;
											  	
	curTick = rawtime(NULL);
	
	if (type == 0)
	{
	     if (ABS(curTick-lastTick) < 3000) return;
	     else
	     {
	         lastTick = curTick;
	     }
	}
	
	#define USB_TIMER_BMP_R 50//59
	#define BMP_DIGIT_INTER 8
	
	//Grap_ClearScreen(0, USB_TIMER_BMP_R, 240,BMP_DIGIT_2840_H+24,0);
	
	drvLcd_Background2DisplayMemory(BMP_DIGIT_2840_H+35,240,USB_TIMER_BMP_R,0); 
	
	rtcSolar_get(&tRtc_solar);
	drvLcd_SetColor(COLOR_BLACK,COLOR_BLACK);
	
	col = 35;
	
	//ʱ���ʽ 13:01
	index = tRtc_solar.hour/10;	
	drvLcd_WriteBMPColor(gu8BmpDigit2840[index],BMP_DIGIT_2840_H,BMP_DIGIT_2840_W,USB_TIMER_BMP_R,col);
	col += BMP_DIGIT_2840_W+BMP_DIGIT_INTER; 
	
	index = tRtc_solar.hour%10;	
	drvLcd_WriteBMPColor(gu8BmpDigit2840[index],BMP_DIGIT_2840_H,BMP_DIGIT_2840_W,USB_TIMER_BMP_R,col);
	col += BMP_DIGIT_2840_W+BMP_DIGIT_INTER; 


	drvLcd_WriteBMPColor(gu8BmpDigit2840[10],BMP_DIGIT_2840_H,BMP_DIGIT_2840_W,USB_TIMER_BMP_R,col); 
	col += BMP_DIGIT_2840_W+BMP_DIGIT_INTER; 
	
	
	index = tRtc_solar.minute/10;	
	drvLcd_WriteBMPColor(gu8BmpDigit2840[index],BMP_DIGIT_2840_H,BMP_DIGIT_2840_W,USB_TIMER_BMP_R,col);
	col += BMP_DIGIT_2840_W+BMP_DIGIT_INTER; 
	
	index = tRtc_solar.minute%10;	
	drvLcd_WriteBMPColor(gu8BmpDigit2840[index],BMP_DIGIT_2840_H,BMP_DIGIT_2840_W,USB_TIMER_BMP_R,col);
	col += BMP_DIGIT_2840_W+BMP_DIGIT_INTER; 

#if 1	

	//X-X-X  ����X
    sprintf(temp_arrary,"%4d-%d-%d  ����%s",tRtc_solar.year,tRtc_solar.month,tRtc_solar.day,gu8EngWeek[tRtc_solar.week]);

    
    disp_info.string = temp_arrary;
    disp_info.color = COLOR_BLACK;
    disp_info.font_size = FONTSIZE24X24;
    disp_info.font_type = 0;
    disp_info.flag = SHOWSTRING_LIAN_MID;
	disp_info.keyStr = NULL;
    Grap_WriteString(0, 99, 240,124,&disp_info);
#endif 

    //lastTick = curTick;     

}

static S32 USB_CartoonDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPCARTOON *pCartoon;
	tGrapViewAttr *pview_attr;
	tGrapCartoonPriv *pcartoon_privattr;
	U32 lev,index,tipId;
	tStrDispInfo disp_info;
	int strRow,strEndRow;
	int picH,picW,picY,picX;
	tRES_PARA tRes;
	tGRAPDESKBOX *pDesk ;
	
	/*
	 *����Դʱ�����ʾ
	 */
	if (GetResouceBmpOffset() == -1)
	{
		char *pStr;
		int row, column, width;

		if (USBHandler_GetScsiFlag() == 1)
		{
			pStr = (char *)"USB status...";
			gu8ReserveFlag = 1;	
		}
		else
		{
			if (gu8ReserveFlag == 0)
			{
			    pStr = (char *)"copy reserve & resource!";
			    
			    USBHandler_SetScsiFlag(0);
			}
			else
			{
			    pStr = (char *)"Charge status...";
			}
		}
		row = 150;
		width = strlen(pStr)*8;
		column = (gLcd_Total_Column-width)/2;
		drvLcd_SetColor(COLOR_RED, COLOR_BLACK);
		drvLcd_Background2DisplayMemory(16, 240, row, 0);
		drvLcd_DisplayString(row, column, 0, (U8*)pStr, NULL, 0);
		DisplayData2Screen();
		
		return SUCCESS;
	}

	
	pCartoon = (tGRAPCARTOON *)pView;
	pDesk    = (tGRAPCARTOON *)pCartoon->pParent ;
	pview_attr = &(pCartoon->view_attr);
	pcartoon_privattr = &(pCartoon->cartoon_privattr);
	
	//draw back picture
	if (pcartoon_privattr->curFrame >= pcartoon_privattr->totFrame )	// loop
	{
		pcartoon_privattr->curFrame = 0;
	}
	
	/*
	 * ʱ����ʾ
	 */
	if(KEY_WAKEUP_SYS == pDesk->ptGetMsgBody->id)
	{
		USB_CartoonTimerDraw(1);
	}
	else
	{
		USB_CartoonTimerDraw(0);	  
	}

	
	/*
	 * ���ͼ��USBͼ�л�
	 */
	tipId = 0xffffffff;
	if(1 == Battery_DcInCheck())
	{
	    //������
	    lev = Battery_Level();	
		if (lev > 4) lev = 4;

	    if (0 == usb_need_charge())
	    {
	        index = CHARGE_CHARGE9_EN;
	        tipId = 1;
	    }
	    else
	    {
	        if (0 == pcartoon_privattr->curFrame)
	        {
	             switch(lev)
	             {
                 case 0:
                     pcartoon_privattr->curFrame = 0;
                     break;
                 case 1:
                     pcartoon_privattr->curFrame = 2;
                     break;
                 case 2:
                     pcartoon_privattr->curFrame = 3;
                     break;
                 case 3:
                     pcartoon_privattr->curFrame = 5;
                     break;
                 case 4:
                     pcartoon_privattr->curFrame = 6;
                     break;
	             }
	        }
	        
	        index = pcartoon_privattr->picArray[pcartoon_privattr->curFrame];
	        
	        tipId = 0;
	    }    
		
		Grap_ShowPicture(index,pview_attr->viewX1, pview_attr->viewY1); 	
	}
	
    picH = 64; 
    picW = 34;
    picY = 227;
    picX = 95;
    
    drvLcd_Background2DisplayMemory(picH,240,picY,0);
    
    if (USBHandler_GetScsiFlag() == 1)
    {//��ʾUSBͼ��
        Grap_ShowPicture(CHARGE_USB_BACKGROUND_EN,picX, picY); 
        strRow = 268;
        strEndRow = strRow+20; 
        
        disp_info.string = (U8 *)"USB ������";
		disp_info.color = COLOR_BLACK;
		disp_info.font_size = 0;
		disp_info.font_type = 0;
		disp_info.flag = SHOWSTRING_LIAN_MID;
		disp_info.keyStr = NULL;
		Grap_WriteString(0, strRow, 240,strEndRow, &disp_info);
    }
    else
    {//ˢ��USBͼ��
    	strRow = 227;
        strEndRow = strRow+20;
    }  
    
        
	/*
	 * �ַ�״̬��ʾ�л�
	 */
	//if(1 == Battery_DcInCheck())
	if(tipId != 0xffffffff)
	{
	//PhoneTrace(0,"DcIn == 1");
	    //��ʾ�ַ�	
		disp_info.string = (U8 *)pUSB_ChargeString[tipId];
		disp_info.color = COLOR_BLACK;
		disp_info.font_size = 0;
		disp_info.font_type = 0;
		disp_info.flag = SHOWSTRING_LIAN_MID;
		disp_info.keyStr = NULL;
		
		if(USBHandler_GetScsiFlag() == 1)
		{
			strRow = 288;
        	strEndRow = strRow+20; 
			Grap_WriteString(0, strRow, 240,strEndRow, &disp_info);
		}
		else
		{
			drvLcd_Background2DisplayMemory(20,240,288,0);
			Grap_WriteString(0, strRow, 240,strEndRow, &disp_info);
		}
	}
	else
	{
	//PhoneTrace(1,"DcIn == 0");
		drvLcd_Background2DisplayMemory(pview_attr->viewHeight,240,pview_attr->viewY1,0);		
		drvLcd_Background2DisplayMemory(20,240,288,0);
	}
   
	return SUCCESS;
}


/*---------------------------------------------------------
*����: USB_ShowDeskbox
*����: USB��ʾ      
*����: none
*����: none
*---------------------------------------------------------*/
void USB_ShowDeskbox(void *pDeskFather)
{
	tGRAPDESKBOX *pDesk;	
	tGRAPCARTOON *pCartoon;
	
	gu8ReserveFlag = 0;
	
	hyhwSysClk_Set(ICLK96M_AHB48M, EXTAL_CLK);
	
	pDesk = Grap_CreateDeskboxNoCom((tGrapViewAttr*)&gtUSBViewAttr[0], NULL);
	if (NULL == pDesk ) return;
	Grap_Inherit_Public(pDeskFather,pDesk);

	pCartoon = Grap_InsertCartoon(pDesk,(tGrapViewAttr*)&gtUSBViewAttr[1],(tGrapCartoonPriv*)&gtUSBCtn[0]);
	if(NULL == pCartoon) return;
	pCartoon->draw = USB_CartoonDraw;
	pCartoon->handle = Usb_CartonHandle;
	
	
	drvLcd_IsUseQueue(0);
	Grap_USBDeskboxRun(pDesk);
	Grap_DeskboxDestroy(pDesk);
}

/*-------------------------�޸ļ�¼--------------------------------
*
*-----------------------------------------------------------------*/