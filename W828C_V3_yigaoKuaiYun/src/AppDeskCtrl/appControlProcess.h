#ifndef _APPCONTROL_PROCESS_H_
#define _APPCONTROL_PROCESS_H_

/*----------------------�ļ�����--------------------------------
* ��������: 2008/04/12 
* ����    : ϵͳ��Ϣ���壬�������̡���ꡢ�ⲿ�ж�
*			Ӧ�ó����������˳������𣬻ָ���
*--------------------------------------------------------------*/

/*include standard files*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"
#include "keyCodeEvent.h"
#include "Msgqueue.h"
#include "touchScreenMain.h"

#define SYS_DATA_SHOWLEN           (20)

//����λΪ�¼�����	
#define	KERNEL_EVENT_TYPE_MASK		0xF0000000		

/* ��һ������ */
#define	KERNEL_EVENT_KEY_INPUT		0x10000000
#define	KERNEL_EVENT_TIMER			0x20000000
#define	KERNEL_EVENT_PULL			0x40000000
#define	KERNEL_EVENT_APP			0x80000000


/* �ڶ�������֮�ⲿ�ж� */
#define	KERNEL_EVENT_EXTINT_MASK	0x0000000F		
#define	KERNEL_EVENT_EXTINT_USB		0x00000000		

/* Ӧ�ó����¼�
 BIT31   |29                                           0
	+----+-------------------------------------------------------------+
	|type| ������ʱ����Ӧ�ó�������кţ����������Ӧ�ó���pid[15:0]�� |         		
	+----+-------------------------------------------------------------+*/
#define KERNEL_EVENT_APP_MASK		0xC0000000
#define KERNEL_EVENT_APP_PID_MASK	0x0000FFFF

#define KERNEL_EVENT_APP_START		0x00000000
#define KERNEL_EVENT_APP_QUIT		0x40000000
#define KERNEL_EVENT_APP_SUSPEND	0x80000000
#define KERNEL_EVENT_APP_RESUME		0xC0000000



//ʹ�õ�����Ϣ���У�ÿ�����̶���һ����������Ϣ����
//�������̵���Ϣ���в���
#define MSG_PUT_HEAD_SIZE               (8)

#define POWER_ON_MSG_QUE_DEEP           (4)
#define POWER_ON_MSG_MAX_SIZE           (40)

#define POWER_DOWN_MSG_QUE_DEEP         (4)
#define POWER_DOWN_MSG_MAX_SIZE         (40)

#define USB_MSG_QUE_DEEP                (10)
#define USB_MSG_MAX_SIZE                (40)

#define SHELL_MSG_QUE_DEEP              (10)
#define SHELL_MSG_MAX_SIZE              (40)

#define KERNAL_APP_MSG_QUE_DEEP         (0)
#define KERNAL_APP_MSG_MAX_SIZE         (0)

#define SMS_APP_MSG_QUE_DEEP	        (10)
#define SMS_APP_MSG_MAX_SIZE            (50)

#define GSMCDMA_SERVER_MSG_QUE_DEEP	    (0)
#define GSMCDMA_SERVER_MSG_MAX_SIZE     (50)

#define GSMCDMA_CLIENT_MSG_QUE_DEEP	    (0)
#define GSMCDMA_CLIENT_MSG_MAX_SIZE     (50)


#define PPP_MSG_QUE_DEEP	            (0)
#define PPP_MSG_MAX_SIZE                (0)

#define BT_MSG_QUE_DEEP                (0)
#define BT_MSG_MAX_SIZE                (0)

//�������̵�����
#define USB_HANDLER_NAME		 "USBHandler"
#define POWER_ON_NAME			 "PowerOn"
#define POWER_DOWN_NAME			 "PowerDown"
#define SHELL_NAME				 "Shell"
#define TCPIP_NAME				 "TcpIp"
#define CTRL_APP_NAME		     "Ctrl"
#define KERNAL_APP_NAME      	 "KernalApp"
#define SMS_APP_NAME             "SMSApp"
#define GSMCDMA_SERVER_NAME      "GsmCdma"
#define GSMCDMA_CLIENT_NAME      "GsmCdmaUser"
#define PPP_NAME                 "PPP"
#define UPLOAD_NAME				 "UpLoad"
#define BEEPER_PRO_NAME          "beeper"
#define TOUCH_NAME               "touch"
#define HANDWRITING_NAME         "ISHW_RCOG"
#define VIBRATOR_CTRL_NAME		 "vibrator"
#define GPS_P_NAME		         "Gps_p"
#define GPS_MANAGE_NAME		     "Gps Manage"
#define BT_NAME                  "bt service"
#define WIFI_DRIVER_NAME		 "WiFiDriver"

//����ջ�Ĵ�С
#define USB_HANDLER_STACK_SIZE		(2048)
#define POWER_ON_STACK_SIZE			(1024)
#define POWER_DOWN_STACK_SIZE		(1024)
#define SHELL_STACK_SIZE			(6144)
#define TCPIP_SIZE					(2048)
#define CTRL_APP_SIZE			    (2048)
#define KERNAL_APP_SIZE      		(6144)
#define SMS_APP_SIZE      		    (2048)
#define GSMCDMA_SERVER_SIZE         (5120)
#define GSMCDMA_CLIENT_SIZE         (4096)
#define PPP_SIZE                    (4096)
#define UPLOAD_SIZE					(4096)
#define BEEPER_SIZE                 (256)
#define TOUCH_SIZE					(512)
#define HANDWRITING_SIZE            (512)
#define VIBRATOR_SIZE               (128)
#define GPS_P_SIZE                  (4096)
#define GPS_MANAGE_SIZE             (1024)
#define BT_SIZE                     (1024)
#define WIFI_DRIVER_SIZE			(2048)

//��������������ȼ� Ӧ�� 7 ���� 8-9 USB 7 AppCtrl 15
//���̵����ȼ�

#define USB_HANDLER_PRIOPITY		PRIORITY_5_OSPP
#define POWER_ON_PRIOPITY			PRIORITY_6_OSPP
#define POWER_DOWN_PRIOPITY			PRIORITY_6_OSPP

#define CTRL_APP_PRIOPITY		    PRIORITY_10_OSPP /* 5-->10 */
#define KERNAL_APP_PRIOPITY	        PRIORITY_6_OSPP
#define SHELL_PRIOPITY				PRIORITY_10_OSPP /* 7-->10 */
#define TCPIP_PRIOPITY				PRIORITY_9_OSPP
#define GSMCDMA_SERVER_PRIOPITY	    PRIORITY_10_OSPP
#define GSMCDMA_CLIENT_PRIOPITY	    PRIORITY_8_OSPP
#define PPP_PRIOPITY	            PRIORITY_11_OSPP
#define UPLOAD_PRIOPITY				PRIORITY_7_OSPP  /* 6-->7 */
#define GSMRESET_PRIOPITY			PRIORITY_6_OSPP
#define WIFICONNECT_PRIOPITY		PRIORITY_6_OSPP
#define BEEPER_PRIOPITY				PRIORITY_12_OSPP /* 8-->12*/
#define TOUCH_PRIOPITY              PRIORITY_10_OSPP /* 6-->10*/
#define HANDWRITING_PRIOPITY        PRIORITY_4_OSPP
#define VIBRATOR_PRIOPITY           PRIORITY_11_OSPP
#define GPS_P_PRIOPITY              PRIORITY_4_OSPP
#define GPS_MANAGE_PRIOPITY         PRIORITY_5_OSPP
#define BT_PRIOPITY                 PRIORITY_11_OSPP
#define WIFI_DRIVER_PRIOPITY		PRIORITY_11_OSPP

#define BARCODE_PRIOPITY			PRIORITY_12_OSPP

/*��Ч�Ľ���pid��*/
#define APPCTRL_INVALID_PID    (0xFFFF)

/* ϵͳ��������ͬʱ���Ӧ�ó����� */
#define	MAX_ACTIVE_APP				TOTAL_APP_AN

typedef	enum 
{
	USB_HANDLER_AN	= 0,		
	POWER_ON_AN,		
	POWER_DOWN_AN,		
	SHELL_AN,		
	TCPIP_AN,	
	KERNAL_APP_AN,   //�ں˳�����̣�����ʹ�ú���ָ�������ں�����ֵ��һ�����ڶ����������ʾ��	      
    GSMCDMA_SERVER_AN,
    GSMCDMA_CLIENT_AN,
    PPP_AN,     
    WIFI_DRIVER_AN,
    BT_DRIVER_AN,
    UPLOAD_APP_AN,//��̨�ϴ�
    GPS_DRIVER_AN,
    
	TOTAL_APP_AN      /* ���һ�� */	   
}eAPPLICATION_NO;


//ע��: �ڷ�����Ϣʱ��Ӧ���ϸ���Ҫ������Ϣ����Ȼϵͳ���ܻ᲻��Ӧ��Ϣ
typedef enum {
	SYS_FOCUS_EVENT = 1,      //���̵õ�������Ϣ�����̿�ʼ����  ����̨->Ӧ��
	SYS_REQ_QUIT_EVENT,		  //���ͽ����˳�����������ɿ���̨�����͸����еĽ��̣������еĽ���ǿ���˳����ɿ���̨�ջ���Դ	
    USE_ACK_QUIT_EVENT,       //����ȷ���˳����� Ӧ��->����̨��һ������µ�Ӧ�ý����յ� SYS_QUIT_EVENT �����Ӧ�ý��̹ر���Ӧ
                              //��Դ���ɿ���̨������Դ  
	USE_REQ_START_TASK_EVENT, //�������̣� Ӧ�ý���->����̨���̣��ɿ���̨��������������Ӧ�ý��̣��Լ�������Դ�ÿ���̨����
	USE_LOST_FOCUS_EVENT,     //��ʧ�Լ��Ľ���,Ӧ�ý���->����̨���Լ�����������̨���ջ���Դ���ض������������Ӧ�ý���
	SYS_DEPRIVE_FOCUS_EVENT,  //���ό��  ����̨->Ӧ�ã�Ӧ���յ��������������Լ� [��:�绰���ˣ�Ҫ�󽹵���ͣ���绰�������ٻָ�����]
	USE_DEPRIVED_FOCUS_EVENT, //�����Ѿ������� Ӧ��->����̨��Ӧ������[���ÿ���̨������]
	USE_QUIT_EVENT,           //������ɱ  Ӧ��->����̨������̨�յ�����������Դ
	
	SYS_PHONE_EVENT,          //����̨-��Ӧ��[�绰����]��������������ʹ�ø���Ϣ

    USE_KILL_OTHER_EVENT,     //Ӧ��->���ƣ�Ӧ�ò�Ҫ��ɱ������Ӧ�ý���
    
	USE_BATTERY_LOW_EVENT,    //Ӧ��->����̨��������̨�յ�����Ϣ�󣬿���̨ɱ�����н��̣����ػ�
	
	USE_PHONE_NUMB_EVENT,     //�绰������Ϣ
	
	USE_USB_IS_ACT,           //����̨->Ӧ�� �Ƿ����USB״̬
	USE_USB_ENTER_ACT,        //Ӧ��->����̨ �û�ȷ�Ͻ���USB״̬
	USE_USB_IGNORE,           //Ӧ��->����̨ ����USB������USB״̬
	
	USE_REQ_POWERDOWN,        //�û�����Ҫ��ǿ�ƹػ������ƽ��������ǿ�ƹػ�

	SYS_REQ_USE_KILL_SELF,    //����̨Ҫ������Լ������˳�
	USE_ACK_KILL_SELF,        //Ӧ�ý��������˳�

	SYS_REQ_TIPSHOW ,         //��ʾϵͳ����ʾ��Ϣ
	
	KEY_INPUT_EVENT,  //��е������Ϣ�������Ϣֻ��foucus�����ܹ�����
	TOUCH_KEY_EVENT,  //����������Ϣ�������Ϣֻ��foucus�����ܹ����� 	
	SYS_TIMER_EVENT,  //ϵͳtimer��Ϣ�������Ϣֻ��foucus�����ܹ����ܣ���ʱ�����������������
	KEY_PULL_EVENT,   //������ק��Ϣ
	TOUCH_KEY_PULL_EVENT,   //������ק��㴥��Ϣ(��ʱ������ȫ��Ϊ��ͼģʽ)
	
	KEY_LOCK_SCR,    //��е����������Ϣ
	KEY_UNLOCK_SCR,  //��е����������Ϣ 20100427
	
	KEY_WAKEUP_SYS,	 //ͨ����������ϵͳ
	
	POP_UNLOCKDIALOG_EVENT,	//����״̬������ʾ�����Ի��� 20100427
	
	MSG_MAX_EVENT = 0x7FFFFFFF,
	
	INVALID_EVENT = -1
}eMSG_ID;


#define KERNEL_EVENT_NUM 20
typedef	struct kernelEvent_st
{
	U32 event[KERNEL_EVENT_NUM];
	U16 rd;
	U16 wr;
}tKERNEL_EVENT;


/* һ��Ӧ�ý��̵Ľṹ������ �ͽ�����Ϣ�Ķ���*/
typedef struct application_st
{
	//eAPPLICATION_NO	app_no;
	U16				pid;
	U16				isFocus;
	tMSG_QUEUE		*ptmsg;
}tAPPLICATION;


typedef struct _tPIDINFOR
{
	U16 MyPid;         /*���������Ϣʱ���Լ���pid��*/
	U16 StartPid;	   /*��Ҫ������PID��*/
	U32 Flag;          /*���ͽ��̺ŵ�ʱ����ʱ��Ҫ������Ϣ*/
}tPIDINFOR;

typedef struct _tKEY_EVENT
{
	U32 keycode;
	U8 keyevent;
}tKEY_EVENT;

typedef struct _tTOUCH_EVENT
{
	U16 row;
	U16 col;
}tTOUCH_EVENT;


typedef struct _tPHONE_EVENT
{
	U8 PhoneEvent;
	U8 Type;      /*0:GSM  1 : wifi */
	U8 Buf[80];  /*��Ϣ�ַ���*/
}tPHONE_EVENT;

//��ק��
typedef struct _tPOINT_PULL
{
    U16 x;
    U16 y;
}tPOINT_PULL;

typedef struct _tYD_NET
{
    U16 lpa;
    U16 wpa;
}tYDNETmsg;


typedef union _UMSG
{
	tPOINT_KEY_PULL   PointKeyPull;    /*��ק��㴥��, Ŀǰ������ҳ�����20080811*/
	tPOINT_PULL   Point;    /*��ק��*/
	tKEY_EVENT    Key;	    /*��е������Ϣ*/
	tTOUCH_EVENT  Touchkey; /*������������*/
	U32 Timer;              /*ϵͳtick,��һ��Ϊtimer��Ϣ*/
	U32 Flag;	            /*ĳЩEvent��Ϣ������Ҫ�����־��ʶ����Ϣ*/
	tPIDINFOR Pid;          /*��Ҫ������pid��*/
	U32 MsgQueAdd;          /*��Ϣ���е�ָ��λ��*/
	
	tPHONE_EVENT PhoneMsg;  /*�绰ר����Ϣ�ṹ���������ʹ��*/
	tYDNETmsg    net;        /* ����ͨѶ��Ϣ */
	
	char         buf[200];
}uMSG;

/*
*һ�������ʹ�����Ĭ�ϵĽṹ�壬���Է��ͺͽ�����Ϣ��,��
*��Ҫ������Ϣ���ݵ�ʱ��������ӳ�Աmsg���������鲻Ҫ��̫��
*��union ��Ա
*/
typedef struct _tMSG_BODY
{
	eMSG_ID	id;       /*ϵͳ�¼�ID��ʹ��ʱ��μ� msg_Msgqueue.h�ļ�*/
	uMSG	msg;      /*��Ϣ������*/
}tMSG_BODY;

typedef struct _tMSG_PUT_PARAM
{
	U32			length;
	U32			priority;
	tMSG_BODY	body;
}tMSG_PUT_PARAM;	
 
 
/* */
extern tKERNEL_EVENT gtkernelEvent;
extern tMSG_QUEUE *ptMsg_controlDesk;
extern void *pvEventWait;

/*---------------------------------------------------------------------------
 ������AppCtrl_CreatPro
 ���ܣ��������̺���Ϣ���в������Ϣ��
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/
void AppCtrl_CreatPro(U16 id,void *pText);

/*---------------------------------------------------------------------------
 ������AppCtrl_DeletePro
 ���ܣ�ɱ�����̺���Ϣ���в������Ϣ��
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/
void AppCtrl_DeletePro(U16 id);

/*---------------------------------------------------------------------------
 ������AppCtrl_GetAppInfor
 ���ܣ����ؽ�����Ϣ�����ṹ tAPPLICATION
 ������appindex ; Ӧ�ý��̵�id��
 ���أ�������Ϣָ��
* -----------------------------------------------------------------------------*/
tAPPLICATION *AppCtrl_GetAppInfor(eAPPLICATION_NO appindex);

/*---------------------------------------------------------------------------
 ������AppCtrl_GetCurAppPid
 ���ܣ��õ���ǰ���н��̵���pid��
 ������none
 ���أ�������pid��
* -----------------------------------------------------------------------------*/
U32 AppCtrl_GetCurAppPid(void);





#endif