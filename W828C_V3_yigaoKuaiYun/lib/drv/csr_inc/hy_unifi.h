#ifndef _HY_UNIFI_H_
#define _HY_UNIFI_H_

#include "hyTypes.h"

/*  */
typedef enum 
{
	CMD_STA_IDLE=0,			//״̬����������Ҫ����
	CMD_STA_INITED,			//״̬����ʼ�����
	
	CMD_SCAN=2,				//���SCAN
	CMD_STA_SCANNING,		//״̬�����ڴ���SCAN����
	CMD_STA_SCANNED,		//״̬��SCAN��������
	CMD_STA_SCAN_ERR,		//״̬��SCAN��������
	
	CMD_SET_WEP_KEY=6,		//���SET WEP KEY
	CMD_STA_SET_WEP_KEY_OK,	//״̬��SET WEP KEY
	CMD_STA_SET_WEP_KEY_ERR,	//״̬��SET WEP KEY
	
	CMD_DEL_WEP_KEY=9,		//���SET WEP KEY
	CMD_STA_DEL_WEP_KEY_OK,	//״̬��SET WEP KEY
	CMD_STA_DEL_WEP_KEY_ERR,	//״̬��SET WEP KEY
	
	CMD_SET_WPA_KEY=12,		//���SET WPA KEY
	CMD_STA_SET_WPA_KEY_OK,	//״̬��SET WPA KEY
	CMD_STA_SET_WPA_KEY_ERR,	//״̬��SET WPA KEY
	
	CMD_DEL_WPA_KEY=15,		//���SET WPA KEY
	CMD_STA_DEL_WPA_KEY_OK,	//״̬��SET WPA KEY
	CMD_STA_DEL_WPA_KEY_ERR,	//״̬��SET WPA KEY
	
	CMD_CONNECT=18,			//���CONNECT
//	CMD_STA_JOINING,		//״̬�����ڴ���JOIN����
	CMD_STA_JOINED,			//״̬��JOIN�����ɹ�
	CMD_AUTHENTICATE,		//AUTHENTICATE����
//	CMD_STA_AUTHENTICATING,	//״̬�����ڴ���AUTHENTICATE����
	CMD_STA_AUTHENTICATED,	//״̬��AUTHENTICATE�����ɹ�
	CMD_ASSOCIATE,			//ASSOCIATE����
//	CMD_STA_ASSOCIATING,	//״̬�����ڴ���ASSOCIATE����
	CMD_STA_CONNECTED,		//״̬��ASSOCIATE�����ɹ���������
	CMD_STA_CONNECT_ERR,	//״̬�������������
	
	CMD_DISCONNECT=25,			//���DISCONNECT
//	CMD_STA_DISCONNECTING,	//״̬���Ͽ�����ִ����
	CMD_STA_DISCONNECTED,	//״̬���ѶϿ�����
	CMD_STA_DISCONNECT_ERR,	//״̬���Ͽ��������
	
	CMD_SET_POWER_MODE=28,		//���SET POWER MODE
	CMD_STA_SET_POWER_MODE_OK,	//״̬��SET POWER MODE OK
	CMD_STA_SET_POWER_MODE_ERR,	//״̬��SET POWER MODE ERR
}eWifi_cmd_state;

typedef enum 
{
	STATE_DISCONNECTED=0,		//�Ͽ�״̬
	STATE_CONNECTED,			//����״̬��δ����
	STATE_ASSOCIATED			//����״̬����״̬�²���ʹ������(�϶���������״̬)
} eWifi_connectState;


extern eWifi_cmd_state hy_wifiCmdAndState;
extern eWifi_connectState hy_wifiConnectState;

#endif //#ifndef _HY_UNIFI_H_
