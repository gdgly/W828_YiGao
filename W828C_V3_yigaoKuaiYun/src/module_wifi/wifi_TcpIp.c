#include "hyTypes.h"
#include "hyhwUart.h"
#include "hyhwIntCtrl.h"
#include "net_api.h"
#include "socket_api.h"
#include "appControlProcess.h"
#include "ospriority.h"
#include "std.h"
#include "sys.h"
#include "glbVariable.h"
/*===================================================================================*/

/*-------------------------------------------------------------
*����: netdev_stop_tcpip
*����: ��ͣ�豸������豸��صĲ���ֹͣ
*����: none
*����: none
*------------------------------------------------------------*/ 
void  netdev_stop_tcpip(void)
{
    tNET_STATE  tstate;
    int time_out = 0;
    
    //Э��ջ���ǻ״̬����ֹͣЭ��ջ
    if (NET_STATE_ACTIVE != Net_GetState()) return;
    
    if (NET_SUCCESS != Net_DeActiveDev(NET_IF_WIFI)) return;
    
    Net_RemoveDev(NET_IF_WIFI);
    
    if (NET_SUCCESS != Net_DeActive(NET_IF_WIFI)) return;
}

/*-------------------------------------------------------------
*����: netdev_get_tcpip_status
*����: �鿴Э��ջ״̬
*����: none
*����: FAIL OR SUCCESS
*------------------------------------------------------------*/ 
int netdev_get_tcpip_status(void)
{
    int rc = 0;
    tNET_STATE  tstate;
    
    if (1 == hy_get_ap_link_status())
    {
        Net_GetDevState(NET_IF_WIFI, &tstate);
        
        if (tstate.AddrState == IF_ADDR_STATE_OK) rc = 1;
    }
    
    return rc;
}

int netdev_remove_wifi_dev(void)
{
    int 		rv = 0;
    tNET_STATE  tstate;    
    
    Net_GetDevState(NET_IF_WIFI,&tstate);
    hyUsbPrintf("tstate.DevState = %d \r\n",tstate.DevState);
    if (tstate.DevState == IF_DEV_STATE_ACTIVE)
    {
        Net_DeActiveDev(NET_IF_WIFI);
        Net_RemoveDev(NET_IF_WIFI);
        
        rv = 1;
    }
    
    return rv;
}


int netdev_add_ip()
{
    int 		rv = 0; 
    tNET_DEV    tNet_Dev;//�������ò���
	
    if (Net_GetState() != NET_STATE_ACTIVE)
    {
        if (NET_SUCCESS != Net_Active())
        {
            return -1;    
        }       
    }
    
    netdev_remove_wifi_dev();
    
    memset(&tNet_Dev,0,sizeof(tNET_DEV));
    Net_GetDev(NET_IF_WIFI,&tNet_Dev);
    
    memcpy(tNet_Dev.EtAddr,csr_getmac(),6);
    tNet_Dev.pHostName = "hyco";
    
    hyUsbPrintf("set mac = %02x-%02x-%02x-%02x-%02x-%02x \r\n",
    	tNet_Dev.EtAddr[0],tNet_Dev.EtAddr[1],tNet_Dev.EtAddr[2],
    	tNet_Dev.EtAddr[3],tNet_Dev.EtAddr[4],tNet_Dev.EtAddr[5]);
#if 1
    if(gtHyc.ipMode == 0)//�Զ���ȡ
    {
		tNet_Dev.DHCP = 1;
		if(gtHyc.dnsMode != 0)//�ֶ�����dns
		{
			tNet_Dev.DNS0 = htonl(inet_addr((char*)gtHyc.dns1));
			tNet_Dev.DNS1 = htonl(inet_addr((char*)gtHyc.dns2));
		}
    }
    else
    {
		tNet_Dev.DHCP = 0;
		tNet_Dev.IpAddr = htonl(inet_addr((char*)gtHyc.ipAddr)); 
		tNet_Dev.Mask = htonl(inet_addr((char*)gtHyc.mask));
		tNet_Dev.Gateway = htonl(inet_addr((char*)gtHyc.gateWay));
		tNet_Dev.DNS0 = htonl(inet_addr((char*)gtHyc.dns1));
		tNet_Dev.DNS1 = htonl(inet_addr((char*)gtHyc.dns2));
    }
#else
	tNet_Dev.DHCP = 0;
	tNet_Dev.IpAddr = htonl(inet_addr((char*)"192.168.1.102")); 
	tNet_Dev.Mask = htonl(inet_addr((char*)"255.255.255.0"));
	tNet_Dev.Gateway = htonl(inet_addr((char*)"192.168.1.1"));
	tNet_Dev.DNS0 = htonl(inet_addr((char*)"211.98.4.1"));
	tNet_Dev.DNS1 = htonl(inet_addr((char*)"211.98.4.1"));
#endif

	rv = Net_AddDev(NET_IF_WIFI, &tNet_Dev);
	hyUsbPrintf("Net_AddDev rv = %d \r\n",rv);
	if (NET_SUCCESS == Net_ActiveDev(NET_IF_WIFI))
    {
    	Net_SetDefaultRoute(NET_IF_WIFI);
        rv = 1;    
    }
    else
    {
        Net_RemoveDev(NET_IF_WIFI);
        rv = 0;
    }
    hyUsbPrintf("Net_ActiveDev rv = %d \r\n",rv);
    
    return rv; 
}

