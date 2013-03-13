/*----------------------�ļ�����--------------------------------
*��������: 08/02/06 
*����    : �ܵ�UI���Ƚ��̣����Դ������߻��ѽ���
*--------------------------------------------------------------*/
/*
*include local files
*/
#include "grap_api.h"
#include "Common_Widget.h"
#include "drvKeyb.h"
#include "app_access_sim.h"
#include "glbVariable.h"

//extern U32 gu32TestTotal,gu32TestSend;

static U8 gu8ShellPermit = 0;


//������Ҫ�ķ���
int Shell_StartService()
{
	ConSume_UpdataSpeedDirect(SHELL_AN, 120);
	
	t9_init();			//�����ֿ�
	
	Net_BackEvt_CreateThread(); /* ���������¼��������0313 */
	syssleep(50);
	
	WifiConnect_Start();//wifi��̨�Զ����ӽ���
	
	GsmReset_Start();	//gsm��̨��������
	
	ppp_p_Start();		//ppp����
	
	bt_create_process();//bt
	
	//Gps_Init();
	
	Remind_Fun(0,0);
	ConSume_UpdataSpeedDirect(SHELL_AN, 0);
}


//ֹͣ����ʱ�����ķ���
int Shell_StopService()
{
	glbAllVariable_SaveParameter();
	
	t9_deinit();//�����ֿ�
	
	Net_BackEvt_ExitThread();//��̨�����¼��������
	
	WifiConnect_p_Exit();	//wifi��̨�Զ����ӽ���
	
	WifiDriver_Exit();	//wifi����
	
	GsmReset_p_Exit();	//gsm��̨��������
	
	ppp_p_Exit();		//ppp����
	
	bt_close();		//bt
	
	netdev_remove_gsm_dev();//tcp
	netdev_close_tcpip();
	
	gsmcdma_stop_service();//gsm
	
	//Gps_DeInit();
}

int Shell_main_handle(tGRAPDESKBOX *pDesk)
{
	int 		len;
	U16 		id;
	uMSG 		*pMsg;
	tGRAPVIEW	*pWidget;
	tGRAPEDIT	*pEdit;	
	U8 			code[100];
	U8          i;
	
	if(pDesk->scanEnable)
	{
		len = 0;
		memset(code, 0, 100);
		barBuf_readBarCode(&code[0], &len);
		
		if(len > 0)
		{
			//��ɨ��ͷ���͵͹�������
			//hwScanner_sendLowCmd(0);
			//BackLight_UpdataLightTime(10);
			
			drvKey_ClearScanTime();
			
			pWidget = (tGRAPVIEW *)Grap_GetCurFocus(pDesk,NULL);
			if(pWidget)
			{
				pEdit = (tGRAPEDIT *)pWidget;
				id = pEdit->view_attr.id;
				if(pEdit->edit_privattr.scanEnable && id >= pDesk->editMinId && id <= pDesk->editMaxId)
				{
					/*��ȡ����ɹ�*/
					Remind_Fun(0, 2);
					
					memset(pEdit->edit_privattr.digt,0,pEdit->edit_privattr.containLen);
					if(len > pEdit->edit_privattr.containLen)
					{
						len = pEdit->edit_privattr.containLen;
					}
					memcpy(pEdit->edit_privattr.digt,code,len);
					pEdit->edit_privattr.Pos = len;
					pEdit->editKeyEnd(pEdit, 0);
					if(pEdit->edit_privattr.sendOK)//�Ƿ���OK��
					{
						pDesk->ptGetMsgBody->id = KEY_INPUT_EVENT;
						pMsg = &(pDesk->ptGetMsgBody->msg);
						pMsg->Key.keycode  = Keyb_Code_OK;
						pMsg->Key.keyevent = 1;
					}
					else
					{
						ReDraw(pDesk, 0, 0);
						pDesk->ptGetMsgBody->id = 0;
					}
				}
			}
		}
	}
		
	return HY_ERROR ;
}


/*---------------------------------------------------------
*����: Shell_ThreadMethod_Init
*����: ��ʼ��һЩ������overlay������
*����: 
*����: none
*---------------------------------------------------------*/
static void Shell_ThreadMethod_Init(void)
{

}

/*---------------------------------------------------------
*����: Shell_Permit_Enter
*����: �Ƿ��������
*����: 
*����: 1����  ������
*---------------------------------------------------------*/
int Shell_Permit_Enter(void)
{
     return gu8ShellPermit;
}

/*---------------------------------------------------------
*����: Shell_ThreadMethod
*����: ϵͳ��ʼ������øý��̣��൱����ǰ��swichmode���Ǹý���
*      ���Դ�������߻�������UI����
*����: 
*����: none
*---------------------------------------------------------*/
int Shell_ThreadMethod(void)
{
	tGRAPDESKBOX SwiDesk;
	U8  gbSwiFocus;
	U8  gu8SwiQuitAll;
	U8  MsgBody[SHELL_MSG_MAX_SIZE];
	U8  PutMsgBody[(SHELL_MSG_MAX_SIZE+MSG_PUT_HEAD_SIZE)];
	U8 	rv;
    tMSG_PUT_PARAM tPutParam;
    tMSG_BODY      tGetBody;
    tAPPLICATION *pPro;
    U32 putadd;
    	
	Shell_ThreadMethod_Init();	
	
	memset(PutMsgBody,0,sizeof(PutMsgBody));
	memset(&SwiDesk,0,sizeof(tGRAPDESKBOX));
	
	gu8SwiQuitAll = 0;
	gbSwiFocus = FALSE;
	
	SwiDesk.pUIFocus = &gbSwiFocus;
	SwiDesk.pQuitCnt= &gu8SwiQuitAll;
	
	
	pPro = AppCtrl_GetAppInfor(SHELL_AN);
	
	SwiDesk.pPutMsg = &putadd;
	SwiDesk.pGetMsg = pPro->ptmsg;
	*SwiDesk.pPutMsg = NULL;
	SwiDesk.ptGetMsgBody = (tMSG_BODY *)MsgBody;
	SwiDesk.ptPutMsgParam = (tMSG_PUT_PARAM *)PutMsgBody;
        
    gu8ShellPermit = 0;	
	
    if(gtHyc.NetSelected == 1)//ѡ���gsm����
	{
		//if((rv = Common_CheckPin(&SwiDesk)) == 0)
	    {
			
	    }
		SIM_set_pin_stat(1);
	}
	
    gu8ShellPermit = 1;	
	SwiDesk.threadCallback = Shell_main_handle;
	
	Shell_StartService();
	if (*SwiDesk.pQuitCnt == 0)
	{		
    	pgtComFirstChd = ComWidget_CreatAllObj();
    	*(SwiDesk.pUIFocus) = TRUE;	       
		
		if(Rec_CheckRecMaxSize() > 512)//��������
		{
			Com_SpcDlgDeskbox("��¼��С������Χ.",0,&SwiDesk,2,NULL,NULL,0xFFFFFFFF);
		}
		
		//gu32TestTotal = 0;
		//gu32TestSend  = 0;
		
		//TestFun_Auto(&SwiDesk);
        Login_Desk(&SwiDesk);        
        
        ComWidget_DeleteAllObj(pgtComFirstChd);
        pgtComFirstChd = NULL;
    }

    if (NULL == *SwiDesk.pPutMsg)
    {
    	*SwiDesk.pPutMsg = ptMsg_controlDesk;
    	SwiDesk.ptPutMsgParam->body.id = USE_QUIT_EVENT;
	    SwiDesk.ptPutMsgParam->priority = 10;
	    SwiDesk.ptPutMsgParam->length = sizeof(eMSG_ID);
    }
    
    //�����Ƿ����������
    msg_queue_put(*SwiDesk.pPutMsg,(char *)&SwiDesk.ptPutMsgParam->body,SwiDesk.ptPutMsgParam->length,SwiDesk.ptPutMsgParam->priority);
	
	return 1;
}

void shell_savescreen()
{
	tGRAPDESKBOX *ptCurDesk;
	char path[50];
	U16  *pData;
	
	
	ptCurDesk = Grap_Get_Current_Desk();
	
	pData = (U16 *)Get_Display_DisplayMemory_Offset();
	
	sprintf(path, "C:/%s.bmp", ptCurDesk->pDisName);
	
	BMP_SaveFile(pData, 240, 320, path);
}

void shell_log(char *name)
{
#if 0
	static U32 logtime=0;
	int  fp;
	U8   buf[100],date[22];
	char path[100];
	
	if(name == NULL && ABS(rawtime(NULL)-logtime) < 18000)
	{
		return ;
	}
	
	if(name != NULL)
	{
		sprintf(path, "C:/%s.txt", name);
	}
	else
	{
		strcpy(path, "C:/log.txt");
	}
	
	OpCom_GetTimeStr(date);
	sprintf(buf, "%s  bat=%d  T=%d;S=%d\r\n", date, Battery_Detect(),gu32TestTotal,gu32TestSend);
	
	fp = AbstractFileSystem_Open(path, 2);
	if(fp >= 0)
	{
		AbstractFileSystem_Seek(fp, 0, 2);
		AbstractFileSystem_Write(fp, buf, strlen(buf));
		AbstractFileSystem_Close(fp);
	}
	else
	{
		fp = AbstractFileSystem_Create(path);
		if(fp >= 0)
		{
			AbstractFileSystem_Write(fp, buf, strlen(buf));
			AbstractFileSystem_Close(fp);
		}
	}
	
	logtime = rawtime(NULL);
#endif
}