/*----------------------�ļ�����--------------------------------
* ��������: 2008/04/12 
* ����    : ϵͳ��Ϣ���壬�������̡���ꡢ�ⲿ�ж�
*			Ӧ�ó����������˳������𣬻ָ���
*           1��ȷ�е�˵����ǰ��focusӦ�ý���ֻ��һ����Ҳ����˵������
*              ͬʱ��������Ӧ�ý��̶�����focus,
*           2������ʱ��Ҫ��ϸߵ�Ӧ�ã�����ʹ�øüܹ���ʽ������������:camera
*           3����Ӧ�ý����У��ؼ���ȡ��Ϣ��ʱ����ʹ��get��ʽ����ȡ��Ϣ[camera��̫�ʺ����ַ�ʽ]   
*--------------------------------------------------------------*/

#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"
#include "hyTypes.h"
#include "std.h"
#include "msgQueue.h"
#include "ospriority.h"
#include "hyhwGpio.h"
#include "sys.h"
#include "hyhwRtc.h"
#include "hyhwUart.h"
#include "HycDeviceConfig.h"
#include "hyUsbDebug.h"
#include "PhoneDebug.h"
#include "beeperCtrl.h"
#include "AbstractFileSystem.h"
#include "appControlProcess.h"
#include "glbStatusDef.h"
#include "AppCtrlConstData.h"
#include "Battery.h"
#include "glbVariable_base.h"
#include "glbVariable.h"
#include "grap_api.h"
#include "grap_view.h"
#include "Common_Dlg.h"
#include "AppControlConsume.h"

#include "DisplayMemory.h"
#include "Common_Widget.h"
#include "LcdModule_BackLight.h"

/*
* extern function or variable
*/
extern void SoftWare_Init(void);

U8  gu8USBtest;//0:δ����debug����   1:�Ѿ�����,����δ����    2:�Ѿ��ɹ�����debug
static U32 gu32UsbDebugTime = 0;//kong 2012-10-22


static U32 VbsInvalidTime;
//������̨Ҫ��ϵͳ�˳�ʱ��¼����Ϣ��
static tSYS_REQ_QUIT_INF sgtSysReqInf;

/* ȫ�ֱ���������̨���̵Ľ�����Ϣ����Ϣ���� */
tMSG_QUEUE *ptMsg_controlDesk;

//static tAPPLICATION	gtActiveApp[TOTAL_APP_AN];
extern tAPPLICATION	gtActiveApp[TOTAL_APP_AN];

void AppCtrl_CreatPro(U16 id , void *pText);


U8 App_GetUsbDebugStat()
{
	return gu8USBtest;
}


void App_SetVbsInvalidTime(void)
{
	VbsInvalidTime = rawtime(NULL);
}
/*---------------------------------------------------------------------------
 ������AppCtrl_IntialVariable
 ���ܣ���ʼ�����ƽṹ��
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/
static void AppCtrl_IntialVariable(void)
{
    int i;
    
    for (i=0; i<TOTAL_APP_AN; i++)
	{
		gtActiveApp[i].pid = APPCTRL_INVALID_PID;
		gtActiveApp[i].isFocus = 0;	
		gtActiveApp[i].ptmsg = NULL;	
	}  
	
	pvEventWait	= NULL;
	memset(&gtkernelEvent,0,sizeof(tKERNEL_EVENT));
	
	memset((char *)&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));	 
	
	ptMsg_controlDesk =  msg_queue_init(10, 40);  
}

/*---------------------------------------------------------------------------
 ������AppCtrl_SysReqQuitAction
 ���ܣ��������̺���Ϣ���в�������Ӧ��focus��Ϣ
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/
static void AppCtrl_SysReqQuitAction(void)
{ 	
	U16 startpid;
	U16 mypid;
	U8 flag;
	U8  msgputbuf[40]={0};
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;

	startpid = sgtSysReqInf.StartPid;
	mypid = sgtSysReqInf.MyPid;
	flag =  sgtSysReqInf.Flag;

	if (APPCTRL_INVALID_PID == gtActiveApp[startpid].pid)
	{
		AppCtrl_CreatPro(startpid,0);
	}

	ptPutMsg->body.id =  SYS_FOCUS_EVENT;			    	
	ptPutMsg->body.msg.Pid.MyPid = mypid; //���Ǹ����̼��������Ӧ�ý��̣����߶Է�
	ptPutMsg->body.msg.Pid.StartPid = startpid;//��Ҫ����Ľ���
	ptPutMsg->body.msg.Pid.Flag = flag; //��������ʱ�ĸ�����Ϣ
	ptPutMsg->length = sizeof(eMSG_ID)+sizeof(tPIDINFOR);
	ptPutMsg->priority = 10;
	msg_queue_put(gtActiveApp[startpid].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);

}

/*---------------------------------------------------------------------------
 ������AppCtrl_SysAckStopMusic
 ���ܣ�����̨����Ӧ�õ�Ҫ���Ѿ�ֹͣ�˺�̨���ֲ��ţ���������Ϣ�ط�ȷ��
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/
static void AppCtrl_SysAckEnterUSB(void)
{
	U32 i;
	U8  msgputbuf[40]={0};
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;
    
    USBHandler_SetScsiFlag(0);
    
    memset(&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));
    
    ptPutMsg->body.id = SYS_REQ_QUIT_EVENT;
	ptPutMsg->length  = sizeof(eMSG_ID);
	ptPutMsg->priority = 10;
	for (i=0; i<TOTAL_APP_AN; i++)
	{ 	
		tAPPLICATION *pAppActive;
		int pid;
		
		pAppActive = &gtActiveApp[i];
		pid = pAppActive->pid;
		if(APPCTRL_INVALID_PID != pid  && NULL != pAppActive->ptmsg && PSPROC(pid)->state != P_INACT)
		{
			msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
			sgtSysReqInf.ReqPidNum++;
		}
	}
		
    if(sgtSysReqInf.ReqPidNum == 0)
    {
    	//��ǰû���������е����ý��̣�û�з����κ���Ϣ
    	memset(&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));
    	return;
    }
    
    //����USB����
    sgtSysReqInf.StartPid = USB_HANDLER_AN;
    sgtSysReqInf.MyPid = TOTAL_APP_AN;
    sgtSysReqInf.AppCtrl_Action = AppCtrl_SysReqQuitAction;
    
    return;
}


/*---------------------------------------------------------------------------
 ������AppCtrl_CreatPro
 ���ܣ��������̺���Ϣ���в������Ϣ��
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/
void AppCtrl_CreatPro(U16 id , void *pText)
{
	U32	startValue;
	tPROCESS_CONFIG  *ptProcConfig;
	ptProcConfig	= &(gtproc_Config[id]);
	
	if (APPCTRL_INVALID_PID != gtActiveApp[id].pid) return;
	
	ConSume_UpdataAppCoef(id, 0);
	
	//ֻ���������Ϣ�������Ľ��̴�������������---gao
	if (id == PPP_AN		||
		id == WIFI_DRIVER_AN||
		id == BT_DRIVER_AN	||
		id == UPLOAD_APP_AN ||
		id == GPS_DRIVER_AN)
	{
		return;
	}
	
	gtActiveApp[id].pid = createp(  ptProcConfig->entry,
					                ptProcConfig->pname,
					                ptProcConfig->stackSize,
					                ptProcConfig->priority,
					                NULL,NULL,NULL);

	if (ptProcConfig->msgQueDeep != 0)
	{//����Ϣ���вŴ�����Ϣ����
		gtActiveApp[id].ptmsg	= msg_queue_init(ptProcConfig->msgQueDeep, ptProcConfig->msg_maxSize);
	}

	startValue = pstart( gtActiveApp[id].pid, (void *)ptProcConfig->entry);
}

/*---------------------------------------------------------------------------
 ������AppCtrl_CreatPro_Virtual
 ���ܣ����ⴴ�����̲������Ϣ��
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/
void AppCtrl_CreatPro_Virtual(U16 id , int pid)
{
	U32	startValue;
	tPROCESS_CONFIG  *ptProcConfig;
	ptProcConfig	= &(gtproc_Config[id]);
	
	if (APPCTRL_INVALID_PID != gtActiveApp[id].pid) return;
	
	//ֻ���������Ϣ�������Ľ��̴�������������---gao
	if (id == PPP_AN		||
		id == WIFI_DRIVER_AN||
		id == BT_DRIVER_AN	||
		id == UPLOAD_APP_AN ||
		id == GPS_DRIVER_AN)
	{
		gtActiveApp[id].pid = pid;
		ConSume_UpdataAppCoef(id, 0);
	}
}

/*---------------------------------------------------------------------------
 ������USB_VbusDetect
 ���ܣ�USB Vbus�ļ��		 
 ������none
 ���أ�none
* -----------------------------------------------------------------------------*/
static int USB_Need_USB_Status(void)
{
    int rc = 2;
    tAPPLICATION *pAppPowerOn,*pAppPowerDwon;
    
    //����ʱֱ�ӽ���USB
    pAppPowerOn = AppCtrl_GetAppInfor(POWER_ON_AN);
    
    pAppPowerDwon = AppCtrl_GetAppInfor(POWER_DOWN_AN);
    
    if (1 == pAppPowerOn->isFocus)
    {
    	int pid;
    	pid = gtActiveApp[POWER_ON_AN].pid;
    	if (PSPROC(pid)->state == P_INACT)
    	{
    		//���������Ѿ�exit����CtrlDesk��δ���������˳���Ϣ
    		//���Բ������䷢��USB��Ϣ
    		rc = 3;
    	}
    	else
    	{
    		rc = 1;
    		AppCtrl_SysAckEnterUSB();
    	}
        return rc;
    }
    else if (1 == pAppPowerDwon->isFocus)
    {
    	int pid;
    	pid = gtActiveApp[POWER_DOWN_AN].pid;
    	if (PSPROC(pid)->state == P_INACT)
    	{
    		rc = 3;
    	}
    	else
    	{
    		rc = 1;
    		AppCtrl_SysAckEnterUSB();
    	}
        return rc;
    }    
    
    if (NULL != sgtSysReqInf.AppCtrl_Action)
    {
        rc = 0;        
    }
    
    return rc;        
}

/*---------------------------------------------------------------------------
 ������USB_VbusDetect
 ���ܣ�USB Vbus�ļ��
 ������none
 ���أ�none
* -----------------------------------------------------------------------------*/
static void USB_VbusDetect(void)
{
	//#define ADAPTER_DETECT_COUNT	(100)//100*10ms = 1s����ϵͳtick��
	#define ADAPTER_DETECT_COUNT	(400)//400*10ms = 4s����ϵͳtick��
	/*
	USB����߼���
		1.���Vbus�Ƿ�����
		2.�����3��(��ʱ����ݵ�ǰ���̶೤ʱ������һ�ζ��仯��������2ticks����һ��)
		3.ȷ��Vbus���ߺ󣬽���USB��Ӳ����ʼ��
		4.��1���ڼ���Ƿ�ΪUSB�߲��룬����ǣ�������������USB���̣�������ǣ���Ϊ����������
	*/
	static U8 vbus_scan_cnt = 0;
	static U8 usbstate = 0;
	static int usb_sysTicks = 0;   	
	//static U32 VbsInvalidTime;
											 
	U32 i;
	U8  msgputbuf[10]={0};
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;
	int vbus_status;
	
#ifdef USB_DEBUG
	if (hyUsbTrans_isRunning() == HY_OK && USBHandler_USBConnect() == 0)
	{
	     //���½��г�ʼ��
	     hyUsbDebugDeviceInit();
	}
	else if (hyUsbTrans_isRunning() == HY_OK)
	{
		hyUsbTrans_DetectStart();
		return;
	}
	else if(gu8USBtest == 2 && hyUsbTrans_isRunning() == HY_ERROR)//kong 2012-10-22
	{
		gu8USBtest = 0;
	}
#endif
	
    #if 1
	if (AppCtrl_IsValidPro(USB_HANDLER_AN) == 1)
	{
		if(0 == usb_inflag_get() && 1 == USBHandler_USBConnect())
		{
			usb_inflag_set(1);
		}
		
		return;
	}
	#endif
    if (sgtSysReqInf.AppCtrl_Action == AppCtrl_SysAckEnterUSB)
    {//USB��Ӧ�Ƿ�ʱ
        if (ABS(rawtime(NULL)-VbsInvalidTime) > 550)
        {
            memset(&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF)); 
        }
   	}
	vbus_status = USBHandler_USBConnect();
	if (USBHandler_GetScsiFlag() == 0)
    {
        if(vbus_status == 1)
        {           
            if (0 == usbstate)
            {
	            vbus_scan_cnt++;
	            if (vbus_scan_cnt >= 1)
	            {//Vbus�˲���ȷ��
	            	usbstate = 1;
	            	vbus_scan_cnt = 0;
	            	
	     	        hyudsDcd_HwInit();
	     	        usb_sysTicks = rawtime(NULL);
	            }
            }
            
            if (1 == usbstate)
            {
            	int sysTicks;int rc;
            	
            	sysTicks = rawtime(NULL);            	
            	if (TRUE == hyudsDcd_GetSpeedDetectFlag())
            	{//ȷ��USB�߲��룬����������USB����
					
            		usbstate = 2;
            		
        	    	USBHandler_SetScsiFlag(2);
            		rc = USB_Need_USB_Status();
	        	    if (3 == rc)
	        	    {
	        	    	//rc = 3�����ܷ�����Ϣ����USB����
	        	    	//�ָ�״̬����һ��ѭ���ټ��
	        	    	USBHandler_SetScsiFlag(0);
	        	    	usbstate = 1;
	        	        return;
	        	    }
	        	    else if (2 != rc)
	        	    {
	        	    	//rc = 1���ػ������У�ֱ�ӽ���USB״̬
	        	    	//rc = 0���ܽ���USB�����(�ڵ绰�����ϣ��޹����ؼ������ϵ�)��ֱ�ӽ�����״̬
						hyudsDcd_StopSpeedDetect();
	        	        return;
	        	    }
					hyudsDcd_StopSpeedDetect();
	        	    
#ifdef USB_DEBUG
	if(gu8USBtest == 1)
	{
		if (hyUsbTrans_softInit() == HY_OK)
		{
			//hyUsbMessage("hyUsbTrans_softInit OK!!\r\n");
			
			syssleep(100);
			hyUsbTrans_Init();
			
			usbstate = 2;
			gu8USBtest = 2;////kong 2012-10-22 �ɹ�����USB debug
			return ;
		}
	}
#endif
					
	        	    if(NULL != sgtSysReqInf.AppCtrl_Action) return ;
	        	    //�����Ի����û���ѡ���Ƿ����U��״̬
	            	ptPutMsg->body.id = USE_USB_IS_ACT;
	            	ptPutMsg->length  = sizeof(eMSG_ID);
	            	ptPutMsg->priority = 10;
	            	
	            	memset(&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));
	            	sgtSysReqInf.ReqPidNum = 0;
	            	for (i=0; i<TOTAL_APP_AN; i++)
	            	{
	            		if (1 == gtActiveApp[i].isFocus && NULL != gtActiveApp[i].ptmsg)
	            		{
	            		    msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
	            		    sgtSysReqInf.ReqPidNum++;
	            		}
	            	}
	            	
			        //����USB����
			        sgtSysReqInf.StartPid = USB_HANDLER_AN;
			        sgtSysReqInf.MyPid = TOTAL_APP_AN;
					sgtSysReqInf.AppCtrl_Action = AppCtrl_SysAckEnterUSB;
					
					VbsInvalidTime = sysTicks;
            		return;
            	}
            	else if (ABS(sysTicks - usb_sysTicks) >= ADAPTER_DETECT_COUNT)
            	{//ȷ���������߲��룬������״̬
					
					hyudsDcd_HwDeinit();
					hyudsDcd_StopSpeedDetect();
            		USBHandler_SetScsiFlag(2);
            		usbstate = 2;
            		
            		return;
            	}
            }
        }
        else
        {
            vbus_scan_cnt = 0;//�������
            usbstate = 0;
        }
    }
    
    if (vbus_status == 0)
    {
		if (AppCtrl_IsValidPro(USB_HANDLER_AN) == 0)
		{
    		if (2 == usbstate)
    		{
        		hyudsDcd_HwDeinit();
        		hyudsDcd_StopSpeedDetect();
    		}

	    	if (USBHandler_GetScsiFlag() == 2)
	    	    USBHandler_SetScsiFlag(0); 
    	}
    	
    	vbus_scan_cnt = 0;//�������
    	usbstate = 0;
    }
}


/*---------------------------------------------------------------------------
 ������AppCtrl_GetAppInfor
 ���ܣ����ؽ�����Ϣ�����ṹ tAPPLICATION
 ������appindex ; Ӧ�ý��̵�id��
 ���أ�������Ϣָ��
* -----------------------------------------------------------------------------*/
tAPPLICATION *AppCtrl_GetAppInfor(eAPPLICATION_NO appindex)
{
	 tAPPLICATION *pApp = NULL;
	 
	 if (TOTAL_APP_AN > appindex)
	 {
	 	pApp =  &gtActiveApp[appindex];
	 }
	 
	 return pApp;
}


/*---------------------------------------------------------------------------
 ������AppCtrl_GetCurAppPid
 ���ܣ��õ���ǰ���н��̵���pid��
 ������none
 ���أ�������pid��
* -----------------------------------------------------------------------------*/
U32 AppCtrl_GetCurAppPid(void)
{
	 tAPPLICATION *pApp = NULL;
	 U32 i;
	 
	 for (i=0; i<TOTAL_APP_AN; i++)
	 {
	    pApp =  &gtActiveApp[i];
	    if (1 == pApp->isFocus) break;  
	 } 
	 						
	 return i;
} 

/*---------------------------------------------------------------------------
 ������AppCtrl_GetPidFromSystem
 ���ܣ��õ���ǰӦ�ý��̵���pid��
 ������none
 ���أ�������pid��
* -----------------------------------------------------------------------------*/
S32 AppCtrl_GetPidFromSystem(void)
{
	S32 pid,i;
    S32 curPid = -1;
    
    pid = getpid();
	 
	 for (i=0; i<TOTAL_APP_AN; i++)
	 {
	    if (pid == gtActiveApp[i].pid)
	    {
	        curPid = i;
	        break;
	    }
	 } 
	 
	 return curPid;
}


/*---------------------------------------------------------------------------
 ������AppCtrl_PowerOffProc
 ���ܣ���е�����ػ�����  �ǹػ���������Ӧ�ý����˳����������ػ����̣�USB״̬�¹ػ����������ٿ��� ����ǿ��������ٷ��������
 ��������
 ���أ�1���ɹ� 0��ʧ�ܴ�ʱ��USB״̬Ӧ�ý��п�������
* -----------------------------------------------------------------------------*/ 
U32 AppCtrl_PowerOffProc(void)
{
	U32 i;
	U32 sendMsgStatus;
	U8  flag = 0;//�ǿ������ǹػ�״̬ 1������ 0 ���ػ�
	U8  msgputbuf[20]={0};
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;
	
	if (gtActiveApp[POWER_DOWN_AN].isFocus == 1) return flag;
	
	if (gtActiveApp[USB_HANDLER_AN].isFocus != 1 && gtActiveApp[POWER_ON_AN].isFocus != 1)
	{//����USB
		flag = 1;
	}
	
	if (NULL != sgtSysReqInf.AppCtrl_Action) return flag;//�Ѿ��а����ػ�
	
	if (1 == flag)
	{//����״̬������йػ�����
	
		ptPutMsg->body.id =  SYS_REQ_QUIT_EVENT;
		ptPutMsg->priority = 10;
		ptPutMsg->length = sizeof(eMSG_ID);

		for(i = 0; i<TOTAL_APP_AN; i++)
		{
			if(APPCTRL_INVALID_PID != gtActiveApp[i].pid && NULL != gtActiveApp[i].ptmsg)
			{
				/*�����Ϣһ���÷���ȥ����Ȼ�ػ���BUG*/
				sendMsgStatus	= msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
				//gtActiveApp[i].isFocus = 0;
				sgtSysReqInf.ReqPidNum++;
			}
		}
		
		//�����ػ����̹ػ�
		sgtSysReqInf.StartPid = POWER_DOWN_AN;
		sgtSysReqInf.MyPid = TOTAL_APP_AN;
		sgtSysReqInf.AppCtrl_Action = AppCtrl_SysReqQuitAction;
		
	}
	
	return flag;
}

/*---------------------------------------------------------------------------
 ������AppCtrl_PowerOffShow
 ���ܣ�
 ��������
 ���أ�1���ɹ� 0��ʧ��
* -----------------------------------------------------------------------------*/ 
U8 AppCtrl_PowerOffShow(void)
{
	U32 i;
	U32 sendMsgStatus;
	U8  msgputbuf[20]={0};
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;
	
	
	if (1 == gtActiveApp[USB_HANDLER_AN].isFocus
	  || 1 == gtActiveApp[POWER_ON_AN].isFocus
	  || 1 == gtActiveApp[POWER_DOWN_AN].isFocus)
	{//����USB
		return 0;
	}
	
	if (NULL != sgtSysReqInf.AppCtrl_Action) return 0;//�Ѿ��а����ػ�����USB�в��붯����
	
	
	ptPutMsg->body.id = USE_REQ_POWERDOWN;
	ptPutMsg->length  = sizeof(eMSG_ID);
	
	ptPutMsg->priority = 10;
	
	for (i=0; i<TOTAL_APP_AN; i++)
	{
		if (gtActiveApp[i].isFocus)
		{
			msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
			break ;
		}
	}
	
	sgtSysReqInf.AppCtrl_Action = AppCtrl_SysReqQuitAction ;
	
	return 1 ;
}


/*---------------------------------------------------------------------------
 ������AppCtrl_WakeupPutGWKey
 ���ܣ��ӵ͹��Ļ���ʱ��GW������
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/ 
void AppCtrl_WakeupPutGWKey(int type)
{
	drvKeyb_SetEvent(Keyb_Code_Power, cKEYB_EVENT_RELEASE);
}

/*---------------------------------------------------
 * ���� : AppCtrl_Record_Trace
 * ���� : ��¼�����
 * ���� : pbuf:�����buf
 * ���� : �õ����ַ��� 
---------------------------------------------------*/
static void AppCtrl_Record_Trace(int type , int x, int y)
{
    if (type == VIEW_PULL_END)
    {
        hyHWRecog_RecogTrace(-1,-1);
    }
    else
    {
        //hzrecog_rec_trace(x,y); 
        hyHWRecog_RecogTrace(x,y); 
    }
}

/*------------------------------------------
* ����:AppCtrl_UsbDebugTimer
* ����:usbdebug��ʱ�ر�
* ����:
* ����:
*------------------------------------------*/
void AppCtrl_UsbDebugTimer()//kong 2012-10-22
{
	//�Ƿ�ʱδ����debug
	if(gu8USBtest == 1 && ABS(rawtime(NULL) - gu32UsbDebugTime) >= 1000)
	{
		gu8USBtest = 0;
		hyUsbTrans_DeInit();
	}
}

//*135791  ����debug   *135790  �ر�   ��ʱδ���ӹر�
/*---------------------------------------------------------
* ����:AppCtrl_UsbDebug
* ����:�򿪻�ر�usbdebug����;  �������� *135791 ��usb debug     ��������*135790 �ر�usb debug����   (����֮�䲻�ܳ���2S)
* ����:keycode:��ǰ����ֵ
* ����:
*--------------------------------------------------------*/
void AppCtrl_UsbDebug(Keyb_Code_e keycode)//kong 2012-10-22
{
	static Keyb_Code_e	lastKey = 0;
	
	if(lastKey != 0 && Keyb_Code_Xing != keycode && ABS(rawtime(NULL) - gu32UsbDebugTime) > 200)
	{
		lastKey = 0;
		return ;
	}
	
	switch(lastKey)
	{
	case 0:
		if(keycode == Keyb_Code_Xing)
		{
			lastKey = keycode;
			gu32UsbDebugTime = rawtime(NULL);
		}
		break;
	case Keyb_Code_Xing:
		if(keycode == Keyb_Code_1)
		{
			lastKey = keycode;
			gu32UsbDebugTime = rawtime(NULL);
		}
		else
		{
			lastKey = 0;
			gu32UsbDebugTime = 0;
		}
		break;
	case Keyb_Code_1:
		if(keycode == Keyb_Code_3)
		{
			lastKey = keycode;
			gu32UsbDebugTime = rawtime(NULL);
		}
		else
		{
			lastKey = 0;
			gu32UsbDebugTime = 0;
		}
		break;
	case Keyb_Code_3:
		if(keycode == Keyb_Code_5)
		{
			lastKey = keycode;
			gu32UsbDebugTime = rawtime(NULL);
		}
		else
		{
			lastKey = 0;
			gu32UsbDebugTime = 0;
		}
		break;
	case Keyb_Code_5:
		if(keycode == Keyb_Code_7)
		{
			lastKey = keycode;
			gu32UsbDebugTime = rawtime(NULL);
		}
		else
		{
			lastKey = 0;
			gu32UsbDebugTime = 0;
		}
		break;
	case Keyb_Code_7:
		if(keycode == Keyb_Code_9)
		{
			lastKey = keycode;
			gu32UsbDebugTime = rawtime(NULL);
		}
		else
		{
			lastKey = 0;
			gu32UsbDebugTime = 0;
		}
		break;
	case Keyb_Code_9:
		if(keycode == Keyb_Code_0)//�ر�
		{
			if(gu8USBtest != 0)
			{
				gu8USBtest = 0;
				hyUsbTrans_DeInit();
			}
		}
		else if(keycode == Keyb_Code_1)//����
		{
			if(gu8USBtest == 0)
			{
				gu8USBtest = 1;
				USBHandler_SetScsiFlag(0);
				hyudsDcd_connectInit();
				
				gu32UsbDebugTime = rawtime(NULL);
			}
		}
		lastKey = 0;
		break;
	default:
		lastKey = 0;
		break;
	}
}

/*---------------------------------------------------------------------------
 ������AppCtrl_ProcIRQEvent
 ���ܣ�����̨��IRQ��Ϣ����������
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/ 
__inline void AppCtrl_ProcIRQEvent(U32 event)
{
	U32		i,keyEvent;
	S32		sendMsgStatus;
	U8		flag = 1;//1����Ϣ���� 0��û����Ϣ 2:������Ϣ
	U8		msgputbuf[40]={0};
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;
	Keyb_Code_e eKeyCode;
	tPOINT_KEY_PULL	pointKeyPull;
	static U32 lasttick = 0;
	static U8  powerncnt = 0;
	int haveLight;
	
	 /*��Ϣ�������еȼ��ģ�������Ϊ�û����������ȼ�Ҫ����һ���timer��Ϣ�����ڷ�����Ϣ��ʱ��
	   ���������ȼ�Ϊ6����timer��Ϣ�����ȼ�Ϊ5
	*/
	if(KERNEL_EVENT_KEY_INPUT == (event&KERNEL_EVENT_KEY_INPUT))
	{
		/* ����ǰ�����Ϣ�����Ͱ�����Ϣ������Ӧ�ý��̵Ľ�����Ϣ���� */				
		
		ConSume_ChangeCoefTemp(192,0);
		haveLight = BackLight_HaveLight();
		
	    if (drvKeyb_GetEvent(&eKeyCode,&keyEvent) == cSSA_OK)
	    {
		#ifdef USB_DEBUG
			
			AppCtrl_UsbDebug(eKeyCode);//kong 2012-10-22

		#endif
			
			if(eKeyCode == Keyb_Code_RightLongPressed)
			{
				Net_AddSendEvt();//�����ϴ����
			}
			
	    	//PhoneTrace1(0,"key = %x", eKeyCode);
	    	if(Keyb_Code_PowerLongPressed == eKeyCode && cKEYB_EVENT_NORMAL == keyEvent)
	    	{
	    		powerncnt++;    		
	    	}
	    	else
    		{
    			powerncnt = 0 ;
    		}
	    	
	    	ptPutMsg->body.id =  KEY_INPUT_EVENT;
	    	ptPutMsg->body.msg.Key.keycode = eKeyCode;
	    	ptPutMsg->body.msg.Key.keyevent = keyEvent;
	    	ptPutMsg->priority = 6;
	    	ptPutMsg->length = sizeof(tKEY_EVENT)+sizeof(eMSG_ID);
	    	
	    	
	    	//��״̬ʱ
	    	if(0 == haveLight)
	    	{
	    		flag = 0;
	    		ptPutMsg->body.id = KEY_WAKEUP_SYS;
	    		
	    		//ͨ������Ϣ���ѵ�����ʾ��
	    		if((Keyb_Code_OK == eKeyCode || Keyb_Code_F1 == eKeyCode || Keyb_Code_Power == eKeyCode || Keyb_Code_ScanL == eKeyCode || Keyb_Code_ScanR == eKeyCode) &&
	    			cKEYB_EVENT_RELEASE == keyEvent)//ֻ��power�����Ի���
				{
	    	    	flag = 2;
	    	    }
	    	}
	    	else
	    	{
	    	 //������  ֻ�ж̰���
	    		if(gtHyc.KeySound && cKEYB_EVENT_RELEASE == keyEvent)
	    		{
	    			if(eKeyCode != Keyb_Code_Power 
	    			&& eKeyCode != Keyb_Code_ScanL 
	    			&& eKeyCode != Keyb_Code_ScanR
	    			&& eKeyCode != Keyb_Code_ScanM
	    			&& eKeyCode != Keyb_Code_PowerLongPressed 
	    			&& eKeyCode != Keyb_Code_ScanLLongPressed
	    			&& eKeyCode != Keyb_Code_ScanRLongPressed
	    			&& eKeyCode != Keyb_Code_ScanMLongPressed )
	    			{
	    				//beeper_Start(KEY_SOUND,1,0);
	    				Remind_Fun(KEY_SOUND, 1);
	    			}
	    		}
	    	}
	    	
	    	BackLight_KeyLight();
	    	
	    	if (flag == 1)
	    	{
	    		if(Keyb_Code_PowerLongPressed == eKeyCode && 4 <= powerncnt)
	    		{
	    			/*if(Net_GetEvt() != 0) // �к�̨����
	    			{
	    				AppCtrl_PowerOffShow();
	    				flag = 0;
	    			}
	    			else */if (1 == AppCtrl_PowerOffProc())	    			
			 		{
			 			flag = 0;
			 		}
	    		}
	    	}
	    	else if (2 == flag)
	    	{
	    	    flag = 1;
	    	} 
	    	
	    	//���±���ʱ��
	    	BackLight_UpdataLightTime(gtHyc.ScreenProtect);
	    }
	    else if (ts_get_touch_pull_position(&pointKeyPull) == HY_OK)
	    {     
	        int key;
	        U8 pullflag = 1;
	    	
		    if(pullflag == 0)
	        {
	        	ptPutMsg->body.id = POP_UNLOCKDIALOG_EVENT;
	        }
	        else
	        {
	        	ptPutMsg->body.id = TOUCH_KEY_PULL_EVENT;
	    	}
	    		
	    	ptPutMsg->body.msg.PointKeyPull.x = pointKeyPull.x;
	    	ptPutMsg->body.msg.PointKeyPull.y = pointKeyPull.y;
	    	ptPutMsg->body.msg.PointKeyPull.endFlag = pointKeyPull.endFlag;
	    	ptPutMsg->priority = 6;
	    	ptPutMsg->length = sizeof(tPOINT_KEY_PULL)+sizeof(eMSG_ID);
	    	
	    	AppCtrl_Record_Trace(pointKeyPull.endFlag,pointKeyPull.x,pointKeyPull.y);
	    	
	    	if (0 == haveLight)
	    	{
	    	    flag = 0;
	    	}
	    	else
	    	{
				BackLight_KeyLight();
	    	}
	    	
	    	//���±���ʱ��
	    	BackLight_UpdataLightTime(gtHyc.ScreenProtect);
	    }
	    else
	    {//��ֹȡ������ֵ��������Ϣ
	    	flag = 0;
	    }
	     
		/* �����н����Ӧ�ý��̷���Ϣ*/
		if (1 == flag)
		{
			for(i = 0; i<TOTAL_APP_AN; i++)
			{
				if(gtActiveApp[i].isFocus)
				{
					/*��ʱû�п�����Ϣ���Ͳ��������*/
					sendMsgStatus	= msg_queue_tryput(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
					break;
				}
			}
		}
	}			
	
	ConSume_ChangeCoefTempElapseTime();
	
	if(KERNEL_EVENT_TIMER == (event&KERNEL_EVENT_TIMER))
	{ 								
		extern U32	gTime;
		
		/*ie��1s��һ��timer�¼�*/
		static U32  ieLastTick = 0;	
		int ieTickFlg = 0;	
		
	#ifdef USB_DEBUG
		AppCtrl_UsbDebugTimer();//kong 2012-10-22
	#endif
		
		ptPutMsg->body.id =  SYS_TIMER_EVENT;
    	ptPutMsg->body.msg.Timer = gTime;    	
    	ptPutMsg->priority = 5;
    	ptPutMsg->length = sizeof(U32)+sizeof(eMSG_ID);
    	
    	if (ABS(gTime-ieLastTick) > 50)
    	{
    	    ieLastTick = gTime;
    	    ieTickFlg = 1;
    	}
    	
		/*ֻ��focus���̲��ܹ�����timer��Ϣ*/
		for(i = 0; i<TOTAL_APP_AN; i++)
		{
			if(gtActiveApp[i].isFocus)
			{
				sendMsgStatus = msg_queue_tryput(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
				break;
			}
		}
		
		AppCtrl_Record_Trace(VIEW_PULL_END,-1,-1);
	#if 1	
		//�����ж�
		if (cSCREENPROTECT_CLOSE != gtHyc.ScreenProtect)
		{
			if (1 != BackLight_AvoidThis())//�ж��Ƿ���Ҫ���α���
			{
    			if(ABS(gTime-lasttick)>= 100)
    			{
    				lasttick = gTime;
    				BackLight_CalculateTime();
    			}
			}
		}
	#endif
	}
}


/*---------------------------------------------------------------------------
 ������AppCtrl_RespondMsg
 ���ܣ�����̨�����յ���Ϣ������Ӧ�Ķ���
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/
static void AppCtrl_RespondMsg(tMSG_BODY *pMsg)
{
	U32 i;
	U16 mypid;
	U16 startpid;
	eMSG_ID event;
	U8  msgputbuf[60]={0};
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;
	tPROCESS_CONFIG  *ptProcConfig;	 

    //�����л�ʱ�ı�Ƶ�ʣ��ӿ��л��ٶ�
    ConSume_ChangeCoefTemp(192,1);
    
	event =  pMsg->id;
	
	ptPutMsg->body.msg.Pid.Flag = pMsg->msg.Pid.Flag;

	switch(event)
	{
		case USE_LOST_FOCUS_EVENT://Ӧ�ý���������ʧ���㣬Ӧ�ý��̴��ڱ���״̬��������ص�UI���� 	
			startpid = pMsg->msg.Pid.StartPid;
			mypid = pMsg->msg.Pid.MyPid;
			
			//��⵱ǰҪ�������Ľ����Ƿ�Ϊ��ռ��UI���̣����������ֹͣ��̨�������������˳� 			   
	        ptProcConfig =  &gtproc_Config[startpid];			
			{ 			
			
				if (APPCTRL_INVALID_PID == gtActiveApp[startpid].pid)
				{
					AppCtrl_CreatPro(startpid,0);				
				}			
				
				//����focus��Ϣ,���������Ľ���
				ptPutMsg->body.id =  SYS_FOCUS_EVENT;			    	
		    	ptPutMsg->body.msg.Pid.MyPid = mypid; //���Ǹ����̼��������Ӧ�ý��̣����߶Է�
		    	ptPutMsg->body.msg.Pid.StartPid = startpid;//��Ҫ����Ľ���
		    	ptPutMsg->body.msg.Pid.Flag = pMsg->msg.Pid.Flag;
		    	ptPutMsg->length = sizeof(eMSG_ID)+sizeof(tPIDINFOR);
		    	ptPutMsg->priority = 10;
				msg_queue_put(gtActiveApp[startpid].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
							
				gtActiveApp[startpid].isFocus	= 1;
				//�ѷ����Լ��Ľ���focus��Ϊ0 
			}
			
			gtActiveApp[mypid].isFocus = 0;
			break;
		 				
		case USE_REQ_START_TASK_EVENT://Ӧ�ý�������Ҫ��������ؽ��̣����Լ�����ɱ	
			mypid = pMsg->msg.Pid.MyPid;
			startpid = pMsg->msg.Pid.StartPid;			
			
			//�����Ϣ����ֹ����USB����Ӧ��Ϣ
			if (mypid == POWER_ON_AN && startpid == SHELL_AN)
			{
			    memset(&sgtSysReqInf,0,sizeof(sgtSysReqInf));
			}
			
			AppCtrl_DeletePro(mypid);	
			
			//��⵱ǰҪ�������Ľ����Ƿ�Ϊ��ռ��UI���̣����������ֹͣ��̨�������������˳� 			   
	        ptProcConfig =  &gtproc_Config[startpid];
			
			{
			
    			if (gtActiveApp[startpid].pid == APPCTRL_INVALID_PID)
    			{					
    				AppCtrl_CreatPro(startpid,0);				
    			}			
    			
    			if (NULL != gtActiveApp[startpid].ptmsg)
    			{
    				S32 test;
    				ptPutMsg->body.id =  SYS_FOCUS_EVENT;			    	
    		    	ptPutMsg->body.msg.Pid.MyPid = mypid;      //���Ǹ����̼��������Ӧ�ý��̣����߶Է�
    		    	ptPutMsg->body.msg.Pid.StartPid = startpid;//��Ҫ����Ľ���
    		    	ptPutMsg->body.msg.Pid.Flag = pMsg->msg.Pid.Flag;
    		    	ptPutMsg->length = sizeof(eMSG_ID)+sizeof(tPIDINFOR);
    		    	ptPutMsg->priority = 10;
    				test = msg_queue_put(gtActiveApp[startpid].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
    			
    			    if (test >= 0) test++;
    			}			
    			gtActiveApp[startpid].isFocus	= 1;
			}
			break;
						
		case USE_ACK_KILL_SELF:				
		case USE_QUIT_EVENT://ϵͳ�����յ�����Ϣ��ϵͳǿ��ɱ�����̣�������Դ					
			AppCtrl_DeletePro(pMsg->msg.Pid.MyPid);			
			break;
			
		case USE_DEPRIVED_FOCUS_EVENT://Ӧ�ý����Ѿ��������ˣ����ڿ���̨�������������		
			for (i=0; i<TOTAL_APP_AN; i++)
			{
			    if (APPCTRL_INVALID_PID != gtActiveApp[i].pid && pMsg->msg.MsgQueAdd == (U32)gtActiveApp[i].ptmsg)
			    {
			        gtActiveApp[i].isFocus	= 0;
			        break; 
			    }
			    
			}
			
			if (0 != sgtSysReqInf.ReqPidNum) sgtSysReqInf.RecPidNum++;
			
			if (sgtSysReqInf.RecPidNum == sgtSysReqInf.ReqPidNum)
			{
		   		if (sgtSysReqInf.AppCtrl_Action != NULL)
		   		{
		   			sgtSysReqInf.AppCtrl_Action();
		   			gtActiveApp[sgtSysReqInf.StartPid].isFocus	= 1;	
				}
				memset((char *)&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));
			}			
			break;
		case USE_ACK_QUIT_EVENT://Ӧ�ý���ȷ���˳�����̨������Դ
			for (i=0; i<TOTAL_APP_AN; i++)
			{				
				if (pMsg->msg.MsgQueAdd == (U32)gtActiveApp[i].ptmsg)
				{//��Ϣȷ���˳���ʱ�����Լ���Ϣ����ָ��
				    AppCtrl_DeletePro(i);
				    
				    if (0 != sgtSysReqInf.ReqPidNum) sgtSysReqInf.RecPidNum++;
				    
					break;					
				}
			}	   
		
			if (sgtSysReqInf.RecPidNum == sgtSysReqInf.ReqPidNum)
			{
		   		if (sgtSysReqInf.AppCtrl_Action != NULL)
		   		{
		   			sgtSysReqInf.AppCtrl_Action();
		   			gtActiveApp[sgtSysReqInf.StartPid].isFocus	= 1;  		   			
				}
				memset((char *)&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));
			}
					
			break;
						
		case USE_KILL_OTHER_EVENT://ȥ�����������˳�Ӧ��
		    startpid = pMsg->msg.Pid.StartPid;
			mypid = pMsg->msg.Pid.MyPid;
			
			ptPutMsg->body.id =  SYS_REQ_QUIT_EVENT;			    	
	    	ptPutMsg->length = sizeof(eMSG_ID);
	    	ptPutMsg->priority = 10;
	    	
	    	if (NULL != gtActiveApp[startpid].ptmsg)
			msg_queue_put(gtActiveApp[startpid].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
		    break;	
		case USE_BATTERY_LOW_EVENT:
		    ptPutMsg->body.id = SYS_REQ_QUIT_EVENT;
        	ptPutMsg->length  = sizeof(eMSG_ID);
        	ptPutMsg->priority = 10;
    		
    		for (i=0; i<TOTAL_APP_AN; i++)
        	{
        		if (APPCTRL_INVALID_PID != gtActiveApp[i].pid  && NULL != gtActiveApp[i].ptmsg)
        		{
        			msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
        			gtActiveApp[i].isFocus = 0;        			
        			sgtSysReqInf.ReqPidNum++;
        		}
        	}

            //�����ػ�����
            sgtSysReqInf.StartPid = POWER_DOWN_AN;
            sgtSysReqInf.MyPid = TOTAL_APP_AN;
            sgtSysReqInf.Flag = 1;
    		sgtSysReqInf.AppCtrl_Action = AppCtrl_SysReqQuitAction;
		    break;             		
		 case USE_USB_ENTER_ACT:		 
		    sgtSysReqInf.RecPidNum++;  			
			if (sgtSysReqInf.RecPidNum == sgtSysReqInf.ReqPidNum)
			{
		   		if (sgtSysReqInf.AppCtrl_Action != NULL)
		   		{
		   			sgtSysReqInf.AppCtrl_Action();		   			
				}
				else
				{
				    memset((char *)&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));
				}    
			}					  
		    break;
		 case USE_USB_IGNORE:
		    memset((char *)&sgtSysReqInf,0,sizeof(tSYS_REQ_QUIT_INF));
		    break;                   		
		
		//����ǿ�ƹػ�����    
		 case USE_REQ_POWERDOWN:
		    ptPutMsg->body.id = SYS_REQ_QUIT_EVENT;
        	ptPutMsg->length  = sizeof(eMSG_ID);
        	ptPutMsg->priority = 10;
    		
    		for (i=0; i<TOTAL_APP_AN; i++)
        	{
        		if (APPCTRL_INVALID_PID != gtActiveApp[i].pid  && NULL != gtActiveApp[i].ptmsg)
        		{
        			msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
        			gtActiveApp[i].isFocus = 0;        			
        			sgtSysReqInf.ReqPidNum++;
        		}
        	}

            //�����ػ�����
            sgtSysReqInf.StartPid = POWER_DOWN_AN;
            sgtSysReqInf.MyPid = TOTAL_APP_AN;
            sgtSysReqInf.Flag = 2;
    		sgtSysReqInf.AppCtrl_Action = AppCtrl_SysReqQuitAction;
		    
		    break;
		case SYS_REQ_TIPSHOW :
			ptPutMsg->body.id = SYS_REQ_TIPSHOW;
        	ptPutMsg->length  = sizeof(eMSG_ID) + SYS_DATA_SHOWLEN;
        	
        	memcpy(ptPutMsg->body.msg.buf,pMsg->msg.buf,ptPutMsg->length);
        	
        	ptPutMsg->priority = 10;
    		
    		for (i=0; i<TOTAL_APP_AN; i++)
        	{
        		if (gtActiveApp[i].isFocus)
        		{
        			msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
        			break ;
        		}
        	}
			break;
	default:	
		break;
	}
	
	return;
}

/*---------------------------------------------------------------------------
 ������AppCtrl_BatteryLevDetect
 ���ܣ��������͵�ʱ�򣬹ػ�����
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/
static void AppCtrl_BatteryLevDetect(void)
{
    static U16 batterLowCnt = 0;
    static U32  deadUiTime = 0; /* UI ������ʱ,����һ��ʱ����ΪUI���������� */
     
    #define BATTERY_POWER_DWON_CNT  200 //10s��������⵽����3.5V��ػ�����
    int enPowerDn;
    U32 batlev;
  	
    batlev = Battery_Detect();
    
    if(1 == Battery_DcInCheck())
    {
    	deadUiTime = 0;
    	return ; /* Dc in �����ػ� */
    }
    
    enPowerDn = Battery_DetectEnforcePowerDown();
    
    if (NULL == sgtSysReqInf.AppCtrl_Action 
       && 0 == AppCtrl_IsValidPro(POWER_DOWN_AN)
       && (batlev <= POWER_DOWN_LEV || 1 == enPowerDn))
    {   
        if (batterLowCnt > BATTERY_POWER_DWON_CNT || enPowerDn == 1)   
        {//�����ػ�����:�������Ҳ��ǹػ����̵�ʱ��
     
            U32 i;
            U8  msgputbuf[40]={0};
        	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgputbuf;
        	
        	if (NULL != sgtSysReqInf.AppCtrl_Action) return;
        	
        	//PhoneTrace(0,"send msg to dc");
        	
        	ptPutMsg->body.id = SYS_REQ_QUIT_EVENT;
        	ptPutMsg->length  = sizeof(eMSG_ID);
        	ptPutMsg->priority = 10;
    		
    		for (i=0; i<TOTAL_APP_AN; i++)
        	{
        		if (APPCTRL_INVALID_PID != gtActiveApp[i].pid  && NULL != gtActiveApp[i].ptmsg)
        		{
        			msg_queue_put(gtActiveApp[i].ptmsg, (char*)(&ptPutMsg->body), ptPutMsg->length , ptPutMsg->priority);
        			//gtActiveApp[i].isFocus = 0;        			
        			sgtSysReqInf.ReqPidNum++;
        		}
        	}

            //�����ػ�����
            sgtSysReqInf.StartPid = POWER_DOWN_AN;
            sgtSysReqInf.MyPid = TOTAL_APP_AN;
            sgtSysReqInf.Flag = 1;
    		sgtSysReqInf.AppCtrl_Action = AppCtrl_SysReqQuitAction;
        	
        	deadUiTime = rawtime(NULL); /* ��¼����ǿ�ƹػ���tick */
        	
        }
        else
        {
             batterLowCnt++;
        }
    }
    else
    {
         batterLowCnt = 0;
    } 
    
    
    
    if(1 == AppCtrl_IsValidPro(POWER_DOWN_AN))
    {
    	if(0 == deadUiTime)
    	{
    		deadUiTime = rawtime(NULL); /* ��¼����ǿ�ƹػ���tick */
    	}
    }
    
    if(deadUiTime != 0)
    {
    	if(ABS(rawtime(NULL) - deadUiTime) >= 6000) /* 1����ǿ�ƹػ� */
    	{
    		int count ;
    		
			InterruptDisable();  
				
			while(1)
			{
			    hyhwBatteryPowerOnOff(0);
			    for (count=0; count < 5000; count++);
			}
    	}
    }
}

/*---------------------------------------------------------------------------
 ������AppCtrl_UpdataFileSysTime
 ���ܣ�����ϵͳ�ļ���ʱ��
 ������none
 ���أ�none
* -----------------------------------------------------------------------------*/
static void AppCtrl_UpdataFileSysTime(void)
{
    U8 TempData[10]={0};
    U8 TempTime[10]={0};
    U32 cur_time;
    static U32 last_time=0;
    
    cur_time = rawtime(NULL);   
    
    if (ABS(cur_time-last_time) > 200)
    {
        hyhwRtc_GetTime(&gtRtc_solar);
        
        sprintf(TempData,"%4d/%02d/%02d",gtRtc_solar.year,gtRtc_solar.month,gtRtc_solar.day);
        sprintf(TempTime,"%02d/%02d/%02d",gtRtc_solar.hour,gtRtc_solar.minute,gtRtc_solar.second);

        Hyc_Set_SysTime(TempData,TempTime);
                
        last_time = cur_time;
    } 
}



//������֤�˳�
static void hyAuthen_errorHandle(void)
{
    exit(1);
}

static void hyAuthen_check(void)
{
    char *pRandomBuf;
	pRandomBuf = malloc(128);
	hyAuthen_reg(NULL, hyAuthen_errorHandle);
	hyAuthen_verify(1, pRandomBuf, NULL);
	free(pRandomBuf);
}

#pragma arm section code="appInterface", rodata="appInterface"
/*---------------------------------------------------------------------------
 ������appControlDesk_process
 ���ܣ�����ϵͳ��Ϣ������Ӧ�ù��ܵĽ���
 ��������
 ���أ����Ƿ���1������ϵͳҪ��
* -----------------------------------------------------------------------------*/
int appControlDesk_process(void)
{
	U32				intLevel;
	tKERNEL_EVENT	tKernelEvent;		/* �ⲿevent����timer��������usb*/
	tPROCESS_CONFIG *ptProcConfig;
	U32				startValue;	
	U32				msgLength;
	U32				sendMsgStatus;	
	U8              Msg[40];
	U32             event;

	
	beeper_PwmDeinit();	//������
		
	gu8USBtest = 0;
#ifdef USB_DEBUG
	//USBHandler_SetScsiFlag(0);//kong 2012-10-22
	//hyudsDcd_connectInit();
#endif
 
    memset( (char *)&tKernelEvent, 0, sizeof(tKERNEL_EVENT));
	
	
	hyhwAdc_start();

	//����оƬ��֤
	hyAuthen_check();

	SoftWare_Init();

 	Battery_IntialVariable();
 	
	//ע�ᰴ��
	drvKeyb_Initialize();
	
	USBHandler_SetScsiFlag(0);
	
	hyudsDcd_connectInit();    
    
    AppCtrl_IntialVariable();
	
	hyhwRtc_GetTime(&gtRtc_solar);	
	
	usb_charge_intial();
	
	bat_show_initial();
    
    ConSume_CoefInitial();
    
	enable_IF();
	
	touchscreen_create_thread();
	hyHWRecog_CreatThread();

//	gpsm_thread(); /* gps���ؽ��� */
	
	{	
		int re1,re2;
		
		syssleep(50);
		re1 = USBHandler_USBConnect();
		re2 = GetResouceBmpOffset();
		
		/* ����USB�ͳ��״̬�������������� */
	    if (re1 == 0 && 0 == Battery_DcInCheck() && re2 != -1)
	    {
	    	drvBadScanKey_check(1); // ��ʼ��ɨ����ļ��
	    	AppCtrl_CreatPro(POWER_ON_AN,0);
	    	gtActiveApp[POWER_ON_AN].isFocus = 1;
	    }
	    else
	    {
	    	if(1 == re1)usb_inflag_set(1); /* ��USB�����־ */

	    	AppCtrl_CreatPro(USB_HANDLER_AN, 0); /* USB UI �� ���UI ���õ� */
	    	gtActiveApp[USB_HANDLER_AN].isFocus = 1;
	    }
	}	
	
#ifdef HYC_DEV_USE_WATCHDOG
	hyhwWatchdog_init(5);
#endif

	while(1)
	{ 				    
		
		/* ��ֹ�жϣ���ewait ֮�󣬲���ʹ���жϣ��Ӷ�����ewakeupִ�� */
		intLevel = setil(_ioff); /* do not want interrupt */

		/* �ȴ�ϵͳ��Ϣ���жϺ����ỽ�Ѵ˽��� */
		(void) ewait(&pvEventWait); 

		/* ȡ��ϵͳ���¼� */
		memcpy( (char *)&tKernelEvent, (char *)&gtkernelEvent, sizeof(tKERNEL_EVENT));
		memset((char *)&gtkernelEvent,0, sizeof(tKERNEL_EVENT));
		
		/* �ָ��ж� */
		(void) setil(intLevel);
		
		
#ifdef HYC_DEV_USE_WATCHDOG
	hyhwWatchdog_feed();
#endif

		while(tKernelEvent.rd < tKernelEvent.wr)
		{			
			event = tKernelEvent.event[tKernelEvent.rd];
			tKernelEvent.rd ++;
			//��IRQ�е��¼����д���
		    AppCtrl_ProcIRQEvent(event);
		}
				
		//USB ��Ⲣ����Ӧ�Ĵ���
		USB_VbusDetect();
		
		//����ϵͳ�ļ�ʱ��
		AppCtrl_UpdataFileSysTime();
shell_log(NULL);		
		//��ص�����⣬��Ҫ�ǵ����͵�ʱ����йػ�����
		AppCtrl_BatteryLevDetect();
		
		usb_updata_charge_fsm();//���״̬������
		
		//���Ŀ��Ʋ���
		ConSume_DetectCurCoef();
		
		/* ������ϵͳ�¼�������Ӧ�ó�������msg, ��Ҫ�Ǹ���Ӧ�õ��������˳������𡢻ָ�*/
		while(1)
		{			 			
			sendMsgStatus	= msg_queue_tryget(ptMsg_controlDesk, (char *)Msg, &msgLength);
			if(sendMsgStatus != HY_OK)
			{
				break;  /* �ò�����Ϣ�ˣ��˳� */
			}		
			
			//����̨���յ�����Ϣ������Ӧ�Ĵ���
			AppCtrl_RespondMsg((tMSG_BODY *)Msg);			
		} 	
		
		Led_keyPad_timeCtrl();
		
//#ifndef USB_DEBUG	
#ifdef HYC_DEV_USE_LOW_PS
		/* �ж��Ƿ���Խ�����͹��Ĵ���״̬ */
		if(gu8USBtest == 0)//kong 2012-10-22
		{
			SysSleepHandler();
		}
#endif
//#endif

		syssleep(1); /* �ó�tick��GSM ͨѶʹ�� */
	
	} /* while(1) */

	return 1;
}
#pragma arm section	//code="appInterface", rodata="appInterface"
