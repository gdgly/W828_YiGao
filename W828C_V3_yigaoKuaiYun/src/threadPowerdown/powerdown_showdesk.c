/*----------------------�ļ�����--------------------------------
*��������: 08/03/07
*����    : ��ʾ�ػ���������
*--------------------------------------------------------------*/

/*
*include files
*/
#include "hyTypes.h"
#include "hyhwTcc7901.h"
#include "hyhwCkc.h"


#include "ioctl.h"
#include "glbVariable_base.h"
#include "grap_api.h"
#include "base.h"
#include "Common_Dlg.h"
#include "AbstractFileSystem.h"



const U32 gu32PowerDownCtn[]={TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,
                     TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN,TY_ON_T_EN};
/*
*���ؼ��Ĺ�������
*/
const tGrapViewAttr gtPowerDownViewAttr[]=
{
	{COMM_DESKBOX_ID,	0,	0,	cLCD_TOTAL_COLUMN,	cLCD_TOTAL_ROW,	0,	0,	cLCD_TOTAL_COLUMN,	cLCD_TOTAL_ROW,	
	 TY_ON_20_EN,	TY_ON_20_EN,0,	0,	1,	0,	0,	0},
	{CARTOON_ID_1,	24, 93,196, 78,0,0,0,0,
	 TY_ON_T_EN,	TY_ON_T_EN,	0,	0,	1,	0,	0,	0},

};

/*
*���嶯����˽������
*/
const tGrapCartoonPriv gtPowerDownCtn[]=
{
	{0,1,8,0,20,0,(U32 *)&gu32PowerDownCtn,TY_ON_20_EN},
};

/*---------------------------------------------------------
*����: PowerOn_CartoonHandle
*����: �ػ�����handle ����      
*����: none
*����: none
*---------------------------------------------------------*/
S32 PowerDown_CartoonHandle(void *pView, GRAP_EVENT *pEvent )
{
	U32 message, wParam, lParam ;
	tGRAPCARTOON *pCartoon;
	tGrapCartoonPriv *pcartoon_privattr;
	tGRAPDESKBOX *pDesk; 
	//int ret ;
	U32 frame;
	U32 count;
	
	pCartoon = (tGRAPCARTOON *)pView;
	pcartoon_privattr = &(pCartoon->cartoon_privattr);

	message = pEvent->message ;
	switch(message)
	{
	case VIEW_TIMER:
		if (pcartoon_privattr->lftTimeIntv == 0)
		{
			pcartoon_privattr->lftTimeIntv = pcartoon_privattr->timeIntv;	//reset timer
			frame = pcartoon_privattr->curFrame;
						
			if(frame >= pcartoon_privattr->totFrame)
			{//��������,����shell���̣����������������ʾ
				int handle;
				
				/* �ر�nand ����㣬��д */
				hyswNandFlash_Close();
				
				/* ����FAT2 */
				handle = open("C", AFS_WRITE);
				ioctl(handle, UPDATE_FAT2);
				close(handle);
				
				/* �ر�nand ����㣬��д */
				hyswNandFlash_Close();
				
				mappingTable_ReadTabletoBuffer(1);
				nandLogicDrv_SetBlockPool(pNandBlockPoolManager, NAND_BLOOK_POOL_MANAGER_SIZE,
											(char *)hsaUsbTransport_GetBuffer(),
											hsaUsbTransport_GetBufferSize());
				  					
				Led_ctrl(0);
				
				#if 1
				if (1 == Battery_DcInCheck()) /* �г�����ͽ�������� */
				{
				    tMSG_PUT_PARAM *ptPutMsgPara;
				    extern void drvKeyb_GetEvent_Timer0ISR(void);
				    
				    drvKeyb_ClearKey();
				    hyhwInt_ConfigKeyScanTimer0_ISR(drvKeyb_GetEvent_Timer0ISR);
				    
				    pDesk = (tGRAPDESKBOX *)pCartoon->pParent;    
							
					ptPutMsgPara =  pDesk->ptPutMsgParam;		
					
					*pDesk->pPutMsg = ptMsg_controlDesk;
						
				    ptPutMsgPara->body.id = USE_REQ_START_TASK_EVENT;
				    ptPutMsgPara->body.msg.Pid.MyPid = POWER_DOWN_AN; 	
					ptPutMsgPara->body.msg.Pid.StartPid = USB_HANDLER_AN;				
					ptPutMsgPara->priority = 10;
					ptPutMsgPara->length =  sizeof(tPIDINFOR)+sizeof(eMSG_ID);	
					*pDesk->pQuitCnt = 0xff;
				
				}
				else
				#endif
				{
					//LcdModulePowerOnOff(0);
				
				//��ʱ�����Ǽٹػ�,�Ա�֤ϵͳ�ȶ�	
				#if 1
					InterruptDisable();  
					
					while(1)
					{
					    if (0 != USBHandler_USBConnect())
					    {
					    #ifdef TCC7901
					        hyhwWatchdog_initMs(1); 
					    #endif
					    }
					    VibratorCtrl_RunTime(100);
					    LcdModulePowerOnOff(0);
					    hyhwLcd_powerCtrl(EnableOff);
					    Led_ctrl(0);
					    hyhwBatteryPowerOnOff(0);
					    for (count=0; count < 5000; count++);
					}
				#else
					TurnOffHandle();
				#endif
				}
				
				return RETURN_QUIT;
			}
			else if (frame == 2)
			{
				BackLight_EventTurnOnLcd(pCartoon);						    
			    
			    hyhwInt_ConfigKeyScanTimer0_ISR(NULL);
        	    syssleep(100);
			}
			else if(frame == 1)
			{
				LcdModulePowerOnOff(1);
				Shell_StopService();
			}

			pCartoon->draw(pCartoon, 0, 0);
			pcartoon_privattr->curFrame++;
			
			Led_ctrl(pcartoon_privattr->curFrame%8);
		}
		else
		{
			pcartoon_privattr->lftTimeIntv--;
		}
		break;
	case VIEW_REV_SYSMSG:
		pCartoon->recvSysMsg(pView);
		break;	
	default:
		break;	
	}
	
	return SUCCESS;
}

static S32 PowerDown_CartoonRecvSysMsg(void *pView)
{
	tGRAPDESKBOX *pDesk;
	tMSG_BODY *pGetMsg;
	tGRAPCARTOON *pCtn;
	U16 id;

	pDesk = (tGRAPDESKBOX *)((tGRAPCARTOON *)pView)->pParent;

	pGetMsg = pDesk->ptGetMsgBody;

	if (SYS_FOCUS_EVENT == pGetMsg->id)
	{//��ʾ���������ڹػ��Ի���

		int flag;

		flag = pGetMsg->msg.Pid.Flag;

		if (1 == flag)
		{
			Com_SpcDlgDeskbox("������,���ڹػ�.",0, pDesk, 1,NULL,NULL,DLG_DEFAULT_TIME);
			//Com_DlgDeskbox(pDesk,1,cDIALOG_BATTERY_LOW_POWEROFF,NULL,NULL,DLG_DEFAULT_TIME);
			ReDraw(pDesk,0,0);
		}
		else if (2 == flag)
		{ 
			//Com_DlgDeskbox(pDesk,1,cDIALOG_ALARM_TIME_PDOWN,NULL,NULL,DLG_DEFAULT_TIME); 
		} 
	}  

	return SUCCESS;
}

/*---------------------------------------------------------
*����: PowerOn_ShowDeskbox
*����: �ػ�������ʾ,����ֻ�Ǽ򵥵Ķ�����ʾ��֮��������ػ�������      
*����: none
*����: none
*---------------------------------------------------------*/
void PowerDown_ShowDeskbox(void *pDeskFather)
{

	tGRAPDESKBOX *pDesk;	
	tGRAPCARTOON *pCartoon;
		
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&gtPowerDownViewAttr[0], NULL);
	if (NULL == pDesk ) return;	
	
    Grap_Inherit_Public(pDeskFather,pDesk);
	
	pCartoon = Grap_InsertCartoon(pDesk,(tGrapViewAttr*)&gtPowerDownViewAttr[1],(tGrapCartoonPriv*)&gtPowerDownCtn[0]);
	if(NULL == pCartoon) return;
	pCartoon->handle = PowerDown_CartoonHandle;
	pCartoon->recvSysMsg = PowerDown_CartoonRecvSysMsg;
	
	Grap_DeskboxRun(pDesk);
	Grap_DeskboxDestroy(pDesk);
	
}
/*-------------------------------------------------------------
*����: netdev_close_tcpip
*����: �ر�Э��ջ
*����: none
*����: FAIL OR SUCCESS
*------------------------------------------------------------*/ 
int netdev_close_tcpip(void)
{
    int rc = SUCCESS;
    
    if (0 == AppCtrl_IsValidPro(TCPIP_AN))  return rc;
    
    Net_Close();
    
    AppCtrl_DeletePro(TCPIP_AN);     
    
    return rc;         
}
/*-------------------------�޸ļ�¼--------------------------------
*
*-----------------------------------------------------------------*/