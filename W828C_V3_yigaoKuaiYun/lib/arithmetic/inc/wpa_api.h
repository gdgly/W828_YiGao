#ifndef __WPA_API_H__
#define __WPA_API_H__

#include "wmi.h"


typedef struct wpa_param_st
{
	unsigned char *ApMac;	//the AP's mac address
	unsigned char *ApSsid;	//the AP's ssid, that is, AP name.
	unsigned char *ApPwd;	//AP's password
	short ApChannel;		//AP transfer channel
	eWMI_ENCRYPTION_TYPE cpt_type;	//support: WMI_WPA_PSK_TKIP / WMI_WPA_PSK_AES / WMI_WPA_PSK_TKIP_AES /
	                                //         WMI_WPA2_PSK_TKIP / WMI_WPA2_PSK_AES / WMI_WPA2_PSK_TKIP_AES.
}tWPA_PARAM;


typedef enum
{
	WPA_SUCCESS = 0,
	WPA_INIT_FAIL,
	WPA_CONNECT_FAIL,
	WPA_WAIT_TIMEOUT,
	WPA_MEMORY_OUT,
	WPA_PROCESS_ERR,
	WPA_UNKNOWN_ERR,
}eWpaRet;


/*----------------------------------------------------------------------
* ����: Wpa_IsRunning
* ����: �ж�Wpa/wpa2�ǲ�����������״̬
* ����: none
* ����: 1: running; 0: no run
* ˵����
*-----------------------------------------------------------------------*/
int Wpa_IsRunning();


/*----------------------------------------------------------------------
* ����: Wpa_Setup
* ����: ����Wpa/Wpa2��֤�����л���WPA/WPA2����ģʽ��
* ����: tWPA_PARAM *pParam:
*			ApMac��Ҫ���ӵ�AP��Mac��ַ
*			ApSsid��Ҫ���ӵ�AP��ssidֵ
*			ApPwd��Ҫ���ӵ�AP��password
*			ApChannel��Ҫ���ӵ�AP�������ӵ�ͨ����
*			cpt_type�����ּ������ͣ�Ŀǰ֧��6�����͡�����WPA-PSK/WPA2-PSK
*					  ����������
* ����: 
*		WPA_SUCCESS: success; other: fail
* ˵�����ú�����ȷ���غ��Ժ��������շ������ݾ���TKIP(AES,+AES)���ܵ����ݡ�
*		����Щ������������ֱ�Ӽ���/���ܵģ�Ӧ�ò�������ġ�
*		ֻҪ֪���ú�����ȷ���غ󣬼�������WPA/WPA2���ܼ��ɡ�
*-----------------------------------------------------------------------*/
eWpaRet Wpa_Setup(tWPA_PARAM *pParam);


/*----------------------------------------------------------------------
* ����: Wpa_end
* ����: ����Wpa/wpa2����/���ܣ��Ͽ����ӡ�
* ����: none
* ����: 0���ɹ���other: ʧ��
* ˵�����ú������غ󣬽��Ͽ����ӡ�
*-----------------------------------------------------------------------*/
int Wpa_end();



//#define NO_MEMORY_CONTROL	//MACRO: ��ʱ������WPA�����Դ���malloc������


#endif	//__WPA_API_H__



