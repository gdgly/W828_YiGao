/*----------------------�ļ�����--------------------------------
*��������: 08/03/07
*����    : ��������
*--------------------------------------------------------------*/

/*
*include standard files
*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"
#include "appControlProcess.h"
#include "grap_api.h"

/*---------------------------------------------------------
*����: PowerDown_ThreadMethod_Init
*����: ��ʼ��һЩ������overlay������
*����: 
*����: none
*---------------------------------------------------------*/
static void PowerDown_ThreadMethod_Init(void)
{
}

/*---------------------------------------------------------
*����: PowerDown_ThreadMethod
*����: ϵͳ�ػ�ʱ���øý��̣���ʾ�ػ�����
*����: 
*����: none
*---------------------------------------------------------*/
int PowerDown_ThreadMethod(void)
{
	U8 MsgBody[POWER_DOWN_MSG_MAX_SIZE];
	U8 PutMsgBody[(POWER_DOWN_MSG_MAX_SIZE+MSG_PUT_HEAD_SIZE)];

	tGRAPDESKBOX LocalDesk;
	U8   focus;	
	U8   QuitAll = 0;
    tMSG_PUT_PARAM tPutParam;
    tMSG_BODY      tGetBody;
    tAPPLICATION   *pPro;
    U32 putadd;
	
	PowerDown_ThreadMethod_Init();	
	
	memset(&LocalDesk,0,sizeof(tGRAPDESKBOX));
	memset(PutMsgBody,0,sizeof(PutMsgBody));
	
	QuitAll = 0;
	focus = TRUE;
	
	LocalDesk.pUIFocus = &focus;
	LocalDesk.pQuitCnt= &QuitAll;
	
	
	pPro = AppCtrl_GetAppInfor(POWER_DOWN_AN);
	
	LocalDesk.pPutMsg = &putadd;
	LocalDesk.pGetMsg = pPro->ptmsg;
	*LocalDesk.pPutMsg = NULL;
	LocalDesk.ptGetMsgBody = (tMSG_BODY *)MsgBody;
	LocalDesk.ptPutMsgParam = (tMSG_PUT_PARAM *)PutMsgBody;
	
	PowerDown_ShowDeskbox(&LocalDesk);
	
	if (NULL == *LocalDesk.pPutMsg)
    {
    	*LocalDesk.pPutMsg = ptMsg_controlDesk;
    	LocalDesk.ptPutMsgParam->body.id = USE_QUIT_EVENT;
	    LocalDesk.ptPutMsgParam->priority = 10;
	    LocalDesk.ptPutMsgParam->length = sizeof(eMSG_ID);
    }
    
    //�����Ƿ����������
    msg_queue_put(*LocalDesk.pPutMsg,(char *)&LocalDesk.ptPutMsgParam->body,LocalDesk.ptPutMsgParam->length,LocalDesk.ptPutMsgParam->priority);

	return 1;
	
}