#ifndef _APP_CONTROL_CONSTDATA_H_
#define _APP_CONTROL_CONSTDATA_H_


#define	APP_ATTR_SYSTEM_AA				0x01		/* ϵͳ����*/
#define	APP_ATTR_EXCLUSIVE_AA			0x02		/* ��ռ��Ӧ�ý���*/
#define	APP_ATTR_NON_EXCLUSIVE_AA		0x04		/* ��Ҫ���ռ��Ӧ�ý���*/
#define	APP_ATTR_ONLY_FOREGROUND_AA		0x08		/* ֻ��ǰ̨���е�Ӧ�ý���*/
#define	APP_ATTR_ALLOW_BACKGROUND_AA	0x10		/* �����̨���е�Ӧ�ý���*/


typedef struct process_config_st
{
	int (*entry)();		/* �������е����*/
	char *pname;		/* �������� */
	U16  stackSize;		/* ����stack size����Ϊ��λ*/
	U16	 priority;		/* �������ȼ� */
						/* ÿ�����̶���һ��������ר�������յ���Ϣ����*/
	U32	 attribute;		/* ���̵����� */					
						
	U16	 msgQueDeep;	/* ���յ���Ϣ���е����*/
	U16	 msg_maxSize;	/* ���յ���Ϣ���е������Ϣ�ĳ��ȣ���λΪ�ֽ�*/	

}tPROCESS_CONFIG;


//ϵͳҪ��Ӧ�ý����˳�ʱ��һЩ������ݵļ�¼
typedef struct _tSYS_REQ_QUIT_INF
{
	U8              ReqPidNum;      /*Ҫ���˳���Ӧ�ý��̵ĸ���*/
	U8              RecPidNum;      /*�Ѿ��յ��˳�Ӧ�õĸ���*/
	
	eAPPLICATION_NO StartPid;       /*��Ҫ������pid*/
	eAPPLICATION_NO MyPid;          /*Ҫ��������pid*/
	U32             Flag;           /*��������ʱ�ĸ�����Ϣ*/
//	U8              State;          /*�������绰״̬*/
	void (*AppCtrl_Action)(void);   /*������������*/
}tSYS_REQ_QUIT_INF;


/* ���̵ĳ�ʼ���ñ�˳��һ��Ҫ�� ö�� eAPPLICATION_NO��һ�� */
extern int USBHandler_ThreadMethod(void);
extern int PowerOn_ThreadMethod(void);
extern int PowerDown_ThreadMethod(void);
extern int Shell_ThreadMethod(void);
extern int TcpIp_ThreadMethod(void);
extern int Kernal_App_ThreadMethod(void);
extern int GsmCdmaServer_ThreadMethod(void);
extern int GsmCdmaClient_ThreadMethod(void);
extern int ppp_ThreadMethod(void);
extern int WiFiDriver_ThreadMethod(void);
extern int hycoBt_main(void);
extern int Net_ThreadMain(void);
extern int Gps_Process(void);


extern const tPROCESS_CONFIG gtproc_Config[TOTAL_APP_AN];
#endif