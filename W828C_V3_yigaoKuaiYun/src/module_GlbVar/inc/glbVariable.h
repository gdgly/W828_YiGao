#ifndef _GLOBAL_VARIABLE_H_
#define _GLOBAL_VARIABLE_H_


#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------
Standard include files:
--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
*INCLUDES
--------------------------------------------------------------------------*/
#include "touchScreenMain.h"

#define WIFI_SAVEPW_MAX			10//����wifi��Ϣ

/*--------------------------------------------------------------------------
*TYPEDEFS
--------------------------------------------------------------------------*/

//AP��Ϣ
typedef struct _AP_info_t
{
	U8	apName[32];		//AP����
	U8	mac[6];			//MAC��ַ
	U16 pwType;			//��������
	U8	pw[64];			//����
	
	U32	lastTime;		//���һ��ʹ��ʱ�� ��2012-01-01-00:00:00 Ϊ����
	
}tAPINFO;

//���ߵ�¼ʱʹ��,�����ϴε�¼��Ϣ
typedef struct _last_login_t
{
	U8	userId[20];
	U8	userPw[20];
	U8	userName[32];
	U8	userRight;
	U8	userType;
	U8	groupTotal;
	U8	groupDefault;
	U8	groupSelect;
	U8	groupId[12];
	U8	groupName[52];
	
}tLAST_LOGIN;

/*--------------------------------------------------------------------------
*STRUCTURE
--------------------------------------------------------------------------*/
typedef struct _glb_HycParameter_t
{
	U32 CRC;	//У���
	
	U8 	ScanSound;			//ɨ���������أ�0����  1����
	U8  KeySound;			//����������    0����  1����
	U8	RemindType;			//��ʾ��ʽ      0����  1: ��  
	U8  BackLight;			//����ȼ�
	
	U32	ParamCfg;			//ÿ��bit��ʶ1��(bit0:�Ƿ������������   bit1:�����Ƿ�����Ϊ��  bit2:ǩ���Ƿ��������)
	
	U16 ScreenProtect;		//����ʱ��
	U8	scanMode;			//ɨ��ģʽѡ�� 0:��ס�����ɿ��ر�(�3-4��)  1:��һ�γ�������3-4��
	U8	sendType;			//���ͷ�ʽ 0:�Զ�(����/ʱ�䷧ֵ������һ����ʼ����)   1:�ֶ�(����/ʱ�䷧ֵ��Ч,���ֶ���F1�ϴ�) 
	U8  autoSendNum;		//�Զ�����������ֵ      20-200
	U8	autoSendTime;		//�Զ�����ʱ���� 		1--30����
	U8	printTimes;			//��ӡ����  1---10
	U8	gpsSwitch;			//gps���� 0:��  1:��
	U8	gpsGetInterval;		//gps�ɼ����ʱ��   1---60����
	U8	gpsSendInterval;	//gsp���ͼ��ʱ��   5---60����
	U8	workDay;			//�����տ�ʼʱ��� //Ĭ��Ϊ0��
	
	U8	NetSelected;		//����ѡ��  0:wifi   1:gsm   0xff:�����л���
	U8 	GprsAccess;			//GPRS����㣺4���ѡ 0��cmnet   1��cmwap   2��uninet   3��uniwap
	U8 	MachineCode[18];	//�ն˱��  ʹ��16λӲ�����
	U8	simCard[12];	//sim��	   	
	U8  UrlSelected;        //Url��ַѡ��  0:�������� 1:��ͨ����
	
	U8	ipMode;			   //0:�Զ���ȡIP   1:�ֶ���д
	U8	dnsMode;		   //0:�Զ���ȡdns  1:�ֶ���д
	U8	ipAddr[16];		    //ip
	U8	mask[16];			//��������
	U8	gateWay[16];		//����
	U8	dns1[16];			//��ѡdns
	U8	dns2[16];			//����dns
	
	tAPINFO	apInfo[WIFI_SAVEPW_MAX];		//����5��ap��Ϣ
	
	U8  bt_addr1[16];      //���ص�ַ
	U8  bt_addr2[16];      //��ѡ�豸��ַ
	U8  bt_pwd[18];        //��ѡ�豸����
	U8  bt_devenable;      //������ʹ��,Ŀǰֻ�г�,������ʹ�� 0:��ʹ�� 1/2/3:���ֳƵı������
	
	tTOUCH_PARAMETER touchParam;
	  
	////////////////////////////
	tLAST_LOGIN	lastLogin;	//�ϴε�¼��Ϣ
	
	U8	key[36];		//ϵͳ�����key
	
	U8	companyId[20];	//��˾����
	U8	companyName[80];//��˾����
	
	U8 	defaultId;			//Ĭ�Ϸ�����ID��1��2��3  ����IP��DNS��һһ��Ӧ�ģ���ֻ���¼Ĭ��ʹ�õķ�������ż��ɡ����Ϊ1�����ʾĬ��ʹ��DNS1�������������ʧ�ܣ���ֱ��ʹ��IP1
	U8  serverIP[3][18];	//������IP  (���ã���������ʧ��ʱʹ��)
	U8 	serverDNS[3][32];	//����(λ��������30λ����)
	U8 	serverPort1[6];		//�������˿�(�����շ�ר��)
	U8 	serverPort2[6];		//�������˿�(�����������ר��)
	U8 	inquiryIP[18];		//���״̬���߲�ѯ������IP
	U8 	inquiryPort[6];		//���״̬���߲�ѯ�˿�
	
	U8	url[256];			//��������ַ
	
} glb_HycParameter_t, *pglb_HycParameter_t;


//������Ϣ
typedef struct _glb_HycParameter_Bak_t
{
	U32 CRC;	//У���
	
	U8	companyId[20];	//��˾����
	
	U8	userID[20];		//Ա�����
	U8	userPw[40];		//Ա������ (������MD5)
	
	U8	key[36];		//�����key
	U8	groupId;		//������
	
	U8	userRight;		//Ȩ��
	U8	userType;		//Ա������
	
	U8	pad;
	
}glb_HycParameter_Bak_t;

/*--------------------------------------------------------------------------
* external Variable ,constant or function
--------------------------------------------------------------------------*/
//system parameter that need to save
extern glb_HycParameter_t    	gtHyc;
extern glb_HycParameter_Bak_t	gtHycBak;

//System parameter that needn't to save

/*-----------------------------------------------------------------------------
* ����:	glbVariable_LoadDefaultValue
* ����:	loadϵͳ����Ĭ��ֵ
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void glbVariable_LoadDefaultValue();

/*-----------------------------------------------------------------------------
* ����:	glbVariable_CheckValue
* ����:	���������ϵͳ����ֵ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void glbVariable_CheckValue(void);

/*-----------------------------------------------------------------------------
* ����:	glbVariable_LoadFromPersistentArea
* ����:	��PersistentArea��load�����ϵͳ����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void glbVariable_LoadFromPersistentArea(void);

/*-----------------------------------------------------------------------------
* ����:	glbVariable_SaveToPersistentArea
* ����:	�����ϵͳ������PersistentArea��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void glbVariable_SaveToPersistentArea(void);


#ifdef __cplusplus
}
#endif

#endif //_GLOBAL_VARIABLE_H_
