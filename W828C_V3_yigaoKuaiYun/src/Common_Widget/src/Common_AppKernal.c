/*
 * file name : common_AppKernal.c
 * ����
*/

/*
 * include files
 */
 
#include "std.h"
#include "appControlProcess.h"
#include "grap_api.h"

//����ָ�����ڽ��̵�ʵ��ִ��
static void (*kernal_app_process)(void); 
static int (*cartoon_app_process)(void *p);
static void *pCApp;

static volatile U8 gu8KernalEventCpt = 0xff; //�ں˽����¼��Ƿ���ɱ�־

/*--------------------------------------------------------
 * ���� ��kernal_app_complete
 * ���� : �ж��ں˽����Ƿ����
 * ���� : ��������� 0xff��δ��� 
 *-------------------------------------------------------*/
int kernal_app_get_stat(void)
{
    return  gu8KernalEventCpt;
}

/*--------------------------------------------------------
 * ���� ��kernal_app_config_entry
 * ���� : ע��������к���
 * ���� : ����ָ��
 * ���� : none 
 *-------------------------------------------------------*/
void kernal_app_config_entry(void (*process)(void))
{
    kernal_app_process =  process;
    gu8KernalEventCpt = 0xff;   
    
    cartoon_app_process = NULL;
    pCApp = NULL;
}

/*--------------------------------------------------------
 * ���� ��cartoon_app_entry
 * ���� : ע��������к���
 * ���� : ����ָ��
 * ���� : none 
 *-------------------------------------------------------*/
void cartoon_app_entry(int (*process)(void),void *p)
{
    cartoon_app_process =  process;
    pCApp = p;
    gu8KernalEventCpt = 0xff;
    
    kernal_app_process = NULL;
}

/*--------------------------------------------------------
 * ���� ��Kernal_App_ThreadMethod
 * ���� : Ӧ�ý��̵��ں˽���
 * ���� : 1 
 *-------------------------------------------------------*/
int Kernal_App_ThreadMethod(void)
{
	U8 MsgBody[KERNAL_APP_MSG_MAX_SIZE+1];
	U8 PutMsgBody[(50+MSG_PUT_HEAD_SIZE)];

	tGRAPDESKBOX LocalDesk;
	U8   focus;	
	U8   QuitAll = 0;
    tMSG_PUT_PARAM tPutParam;
    tMSG_BODY      tGetBody;
    tAPPLICATION   *pPro;
    U32 putadd;
    U32 startid;
    S32 ret;
    U32 len;
	
	gu8KernalEventCpt = 0xff;
	
	memset(&LocalDesk,0,sizeof(tGRAPDESKBOX));
	memset(PutMsgBody,0,sizeof(PutMsgBody));
	
	QuitAll = 0;
	focus = TRUE;
	
	LocalDesk.pUIFocus = &focus;
	LocalDesk.pQuitCnt= &QuitAll;
	
	pPro = AppCtrl_GetAppInfor(KERNAL_APP_AN);
	
	LocalDesk.pPutMsg = &putadd;
	LocalDesk.pGetMsg = pPro->ptmsg;
	*LocalDesk.pPutMsg = NULL;
	LocalDesk.ptGetMsgBody = (tMSG_BODY *)MsgBody;
	LocalDesk.ptPutMsgParam = (tMSG_PUT_PARAM *)PutMsgBody;
	
    if (NULL != kernal_app_process)
    {
        kernal_app_process();
        kernal_app_process = NULL;
        ret = 1;
    }
    else if(NULL != cartoon_app_process)
    {
    	ret = cartoon_app_process(pCApp);
    	cartoon_app_process = NULL;
    	pCApp = NULL;
    }
    gu8KernalEventCpt = ret; 
 	
	exit(1);
	
#if 0    
	*LocalDesk.pPutMsg = ptMsg_controlDesk;
	LocalDesk.ptPutMsgParam->body.id = USE_QUIT_EVENT;
	LocalDesk.ptPutMsgParam->body.msg.Pid.MyPid = KERNAL_APP_AN; 	
	LocalDesk.ptPutMsgParam->priority = 10;
    LocalDesk.ptPutMsgParam->length = sizeof(eMSG_ID)+sizeof(tPIDINFOR);
     
    //�����Ƿ����������
    msg_queue_put(*LocalDesk.pPutMsg,(char *)&LocalDesk.ptPutMsgParam->body,LocalDesk.ptPutMsgParam->length,LocalDesk.ptPutMsgParam->priority);
 #endif   
    return 1;
}