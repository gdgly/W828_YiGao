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
#include "std.h"
#include "sys.h"
#include "ospriority.h"
#include "appControlProcess.h"

/*
* extern function or variable
*/


/* ȫ�ֱ�����ϵͳ���¼� */
tKERNEL_EVENT gtkernelEvent;

/* ȫ�ֱ���������ϵͳ�¼������Ҫ���ѵĽ����б����ھ�һ������̨���� */
void *pvEventWait	= NULL;
tAPPLICATION	gtActiveApp[TOTAL_APP_AN];


/*---------------------------------------------------------------------------
 ������AppCtrl_DeletePro
 ���ܣ�ɱ�����̺���Ϣ���в������Ϣ��
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/
void AppCtrl_DeletePro(U16 id)
{ 
	U16 pid;
	
	if (id >= TOTAL_APP_AN)
	{
	    return;
	}
	
	if (APPCTRL_INVALID_PID == gtActiveApp[id].pid) return;	
	
	pid = gtActiveApp[id].pid;
	gtActiveApp[id].pid = APPCTRL_INVALID_PID;
	gtActiveApp[id].isFocus	= 0;	

	//ֻ���������Ϣ�������Ľ������ٲ���������---gao
	if (id == PPP_AN		||
		id == WIFI_DRIVER_AN||
		id == BT_DRIVER_AN	||
		id == UPLOAD_APP_AN ||
		id == GPS_DRIVER_AN)
	{
		return;
	}

	if (NULL != gtActiveApp[id].ptmsg)
	{
		msg_queue_destroy(gtActiveApp[id].ptmsg);
		gtActiveApp[id].ptmsg = NULL;						
	}
	 
	//�ɿ���̨����ȥɱ������Ӧ�ý���
	kill(pid,1);
	
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	deletep(pid);
	
}
/*---------------------------------------------------------------------------
 ������AppCtrl_IsValidPro
 ���ܣ������Ƿ񴴽�
 ��������
 ���أ�1---�Ѵ���
 	   0---û�д���
* -----------------------------------------------------------------------------*/
int AppCtrl_IsValidPro(U16 id)
{
	if (APPCTRL_INVALID_PID != gtActiveApp[id].pid)
	{
		return 1;
	}
	return 0;
}