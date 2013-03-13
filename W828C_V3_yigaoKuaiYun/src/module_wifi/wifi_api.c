/*
*��Ӧ�ò��ṩ�ӿ�
*/

#include "hyTypes.h"
#include "hyErrors.h"
#include "wifi_user.h"
#include "wmi.h"
#include "wifi_api.h"
#include "hyhwCkc.h"
#include "hyhwSDIO.h"
#include "hymwSDIO.h"
#include "hyhwIntCtrl.h"
#include "hyhwGpio.h"
#include "glbVariable.h"
//#include "EventEncumberInit.h"

U8 gu8Wifi_ConnectState;//wifi����״̬ 0:δ����  1:������
struct llist_head tApList;
//U8 gApSortedInfo[MAX_AP_COUNT];

/**************************************************************
*��������hy_stop_scan_ap
*���ܣ�ֹͣɨ��
*������none
*���أ�none
***************************************************************/
void hy_stop_scan_ap(void)
{
	return ;
}

/**************************************************************
*��������hy_get_ap_rssi
*���ܣ���ȡָ��AP���ź��źŵȼ�
*������index:������AP����
*���أ��źŵȼ�
***************************************************************/
S32 hy_get_ap_rssi(U8 index)
{
	S32 currssi = 0;

	return currssi;
}

/**************************************************************
*��������hy_get_connectap_rssi
*���ܣ����µ�ǰwifi�����ӵ�AP���źŵȼ�
*������none
*���أ��źŵȼ�
***************************************************************/
S32 hy_get_connected_ap_rssi(void)
{
	S32 currssi = 0;
	
	currssi = csr_getconnect_rssi();
	
	return currssi;	
}

/**************************************************************
*��������hy_get_scan_ap_count
*���ܣ���ȡɨ�赽��AP����
*������none
*���أ�ɨ�赽��AP��
***************************************************************/
U16 hy_get_scan_ap_count(void)
{
	U16 apcount = 0;
	
	tWMI_PARSE_BECON *pPos;
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
        apcount++;  
    }
	
	return apcount;
}



/**************************************************************
*��������hy_reconnect_ap
*���ܣ����������ϴ����ӵ�AP
*������none
*���أ�HY_OK:���ӳɹ� HY_ERROR:����ʧ��
***************************************************************/
U32 hy_reconnect_ap(void)
{
	
	return HY_OK;
}

/**************************************************************
*��������hy_connect_anoter_ap
*���ܣ��Ͽ���ǰAP,������һ��ָ����AP
*������apMac
*���أ�HY_OK:���ӳɹ� HY_ERROR:����ʧ��
***************************************************************/
U32 hy_connect_anoter_ap(U8 *apMac)
{
	
	return HY_OK;
}

/**************************************************************
*��������hy_get_ap_info_sort
*���ܣ���ȡָ��AP��Ϣ
*������index:������AP���
*���أ�AP��Ϣ
***************************************************************/
tWMI_BECON_BUFF *hy_get_ap_info_sort(U8 index)
{
	tWMI_BECON_BUFF *pApinfo = NULL;
	int count = 0;
	
	tWMI_PARSE_BECON *pPos;
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
        if (count == index)
        {
             pApinfo = &pPos->beconBuff;
             break;
        }
        
        count++;  
    }
		
	return pApinfo;
}

tWMI_BECON_BUFF *hy_get_ap_info_noEncryp()
{
	tWMI_BECON_BUFF *pApinfo = NULL;
	
	tWMI_PARSE_BECON *pPos;
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
        if(pPos->beconBuff.wmiEncryptionType == 0)//δ����
        {
             pApinfo = &pPos->beconBuff;
             break;
        }
    }
		
	return pApinfo;
}

tWMI_BECON_BUFF *hy_get_ap_info_bymac(U8* pMac)
{
	tWMI_BECON_BUFF *pApinfo = NULL;
	
	tWMI_PARSE_BECON *pPos;
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
        if(memcmp(pMac, pPos->beconBuff.apBssInfo.bssid, 6) == 0)
        {
             pApinfo = &pPos->beconBuff;
             break;
        }
    }
		
	return pApinfo;
}


tWMI_BECON_BUFF *hy_get_ap_info_byName(U8 *pName)
{
	tWMI_BECON_BUFF *pApinfo = NULL;
	
	tWMI_PARSE_BECON *pPos;
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
        if(strcmp(pName, pPos->beconBuff.ap.apName) == 0)
        {
             pApinfo = &pPos->beconBuff;
             break;
        }
    }
		
	return pApinfo;
}

/**************************************************************
*��������hy_get_ap_module_status
*���ܣ���ȡwifiģ���Ƿ����
*������none
*���أ�0==������1==����
***************************************************************/
U8 hy_get_ap_module_status(void)
{
	U8 status;
	
	status = 0;//������Ϊ������ȷ��

	return status;
}

/**************************************************************
*��������hy_get_ap_link_status
*���ܣ���ȡ����״̬
*������none
*���أ�0:δ����  1:������
***************************************************************/
U8 hy_get_ap_link_status()
{
	return gu8Wifi_ConnectState;
}

void hy_set_ap_link_status(U8 state)
{
	gu8Wifi_ConnectState = state;
}

/**************************************************************
*��������hy_get_ap_ready_status
*���ܣ���ȡwifiģ��׼��״̬
*������none
*���أ���MV_READY_STATUS
***************************************************************/
MV_READY_STATUS hy_get_ap_ready_status()
{
	MV_READY_STATUS status;
	
	status = adapter.mvReadyStatus;
	
	return status;
}

/**************************************************************
*��������hy_get_ap_scan_status
*���ܣ���ȡwifiģ��ɨ��AP״̬
*������none
*���أ���
***************************************************************/
U8 hy_get_ap_scan_status()
{
	U8 status;
	
	return status;
}

/**************************************************************
*��������hy_get_ap_scan_status
*���ܣ���ȡ�Ƿ�ȡ���źŵ�״̬
*������none
*���أ�0 = δȡ����1=��ȡ��
***************************************************************/
U8 hy_get_ap_rssi_status()
{
	U8 status;
	
	//drv_MutexProtect();

	//drv_MutexUnprotect();
	
	return status;
}

/**************************************************************
*��������hy_sort_ap_rssi
*���ܣ���ap�б��������,�������:�����ϵ�λ�ڵ�һ�� ���Ϊ�ź�ǿ������
*������none
*���أ�none
***************************************************************/
void hy_sort_ap_rssi(void)
{
	tWMI_BECON_BUFF beconBuff;
	tWMI_PARSE_BECON *pPos1,*pPos2;
	
	drv_MutexProtect();
	
	for (pPos1=(tWMI_PARSE_BECON *)tApList.next; pPos1->list.next != &tApList; pPos1 = pPos1->list.next)
	{
	     for (pPos2=pPos1->list.next; (struct llist_head *)pPos2 != &tApList; pPos2 = pPos2->list.next)
	     {
	         
	          if (pPos2->beconBuff.apStatus&0x1)
	          {
	              beconBuff = pPos2->beconBuff;
	              pPos2->beconBuff = pPos1->beconBuff;
	              pPos1->beconBuff = beconBuff;	                
	          }
	          else if (pPos2->beconBuff.apBssInfo.apRssi > pPos1->beconBuff.apBssInfo.apRssi && !(pPos1->beconBuff.apStatus&0x1))
	          {
	              beconBuff = pPos2->beconBuff;
	              pPos2->beconBuff = pPos1->beconBuff;
	              pPos1->beconBuff = beconBuff;	  
	          }
	     }
	}
	
	drv_MutexUnprotect();
	
	return ;
}


/**************************************************************
*��������hy_get_ap_rssi_range
*���ܣ����ݴ����rssi���з�6���ȼ�
*������rssi:�ź�ǿ��ֵ
*���أ��źŵȼ�
***************************************************************/ 
U32 hy_get_ap_rssi_range(S32 rssi)
{
	U32 index = 0;

    //if(rssi < -90) index = 0;
	//else 
	if(rssi < -82) index = 1;
	else if(rssi < -76) index = 2;
	else if(rssi < -68) index = 3;
	else if(rssi < -60) index = 4;
	else index = 5;
	
	//G40��AP�ź�ͼ��ֻ��4��
	if (index>4) index = 4;

	return index;
}

/**************************************************************
*��������hy_ap_wait_link_success
*���ܣ��ȴ�wifi������
*������none
*���أ��ɹ�: SUCCESS ʧ��:FAIL
***************************************************************/ 
U32 hy_ap_wait_link_success(void)
{
    U32 timecnt = 0;
    U32 rc = 0;
    
    while(0 == hy_get_ap_link_status())
    {
        
        
        syssleep(1);
        if (timecnt++ > 700)
        {
            break;
        }        
    }
    
    if ( 1 == hy_get_ap_link_status())
    {
//        mvWmi_cmdRssi();
        rc = 1;
    }
    
    return rc;
}

/**************************************************************
*��������hy_adhoc_wait_discon
*���ܣ��ȴ�ap�ضϿ���Ϣ
*������none
*���أ�HY_OK:�ɹ� HY_ERROR:ʧ��
***************************************************************/
static int hy_adhoc_wait_discon(void)
{
    U32 timecnt = 0;
    int rc = HY_ERROR;
    
    while(1 == hy_get_ap_link_status())
    {
        syssleep(1);
        if (timecnt++ > 700)
        {
            break;
        }        
    }
    
    if ( 1 != hy_get_ap_link_status())
    {
        rc = HY_OK;
    }
    
    return rc; 
}

/**************************************************************
*��������hy_ap_discon_adhoc
*���ܣ�������������ap��Ϣ
*������eHY_APCMD�е�һЩ����
*���أ�HY_OK:�ɹ� HY_ERROR:ʧ��
***************************************************************/
int hy_ap_discon_adhoc(void)
{
	U32 ret = HY_OK;
    tWMI_PARSE_BECON *pPos,*pPos1;	         
    int flag = 0;
    U8 status;

	if( adapter.mvWlanStatus ==  WLAN_STATUS_CONNECT)
	{
		
         llist_for_each_entry_safe(pPos,pPos1,tWMI_PARSE_BECON,&tApList,list)
         {
             status = pPos->beconBuff.apStatus;
             
             if ((status&0x1) && (status&0x4))
             {
                 llist_del(&pPos->list);
                 unifi_free(NULL, pPos);
                 break;
             }
             else if ((status&0x1))
             {
                 pPos->beconBuff.apStatus &= (~1); 
                 break;
             }
         }	
   	
//    	 mvWmi_cmdAdHocStop();

		// do not know if event will come back!			
	}
	//adapter.mvWlanStatus =  WLAN_STATUS_NOT_CONNECT;

    return ret;
}

/*-------------------------------------------------------------
*��������hy_ap_get_spc_becon
*���ܣ�����mac��ַ�õ�becon��ַ
*������pMac��mac��ַ
*���أ����ɹ�����NULL
*------------------------------------------------------------*/
tWMI_BECON_BUFF *hy_ap_get_spc_becon(U8 *pMac)
{
    tWMI_PARSE_BECON *pPos = NULL;
    tWMI_BECON_BUFF  *pBuf = NULL;
      
    llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
         if(memcmp( pPos->beconBuff.apBssInfo.bssid,pMac,6) == 0 )
    	 {
    		 pBuf = &pPos->beconBuff;
    		 break;
    	 }
    }  
      
    return pBuf;
}

/*-------------------------------------------------------------
*��������hy_ap_del_list_all
*���ܣ�ɾ��ap list ������
*������
*���أ�
*------------------------------------------------------------*/
void hy_ap_del_list_all(void)
{
    tWMI_PARSE_BECON *pPos,*pPos1;
    //int i=0;
    
    llist_for_each_entry_safe(pPos,pPos1,tWMI_PARSE_BECON,&tApList,list)
    {
        llist_del(&pPos->list);
        //hyUsbPrintf("ies buf = %x \r\n", (U32)pPos->beconBuff.ies.buf);
        unifi_free(NULL, pPos->beconBuff.ies.buf);
        unifi_free(NULL, pPos);
    } 
}

/*-------------------------------------------------------------
*������hy_ap_get_curnet_type
*���ܣ��õ���ǰ���������
*������none
*���أ�
*------------------------------------------------------------*/
int hy_ap_get_curnet_type(void)
{
	tWMI_PARSE_BECON *pPos = NULL;
    tWMI_BECON_BUFF  *pBuf = NULL;
	int type = AP_NET_TYPE_UNKNOW;
	int status;
	
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
         status = pPos->beconBuff.apStatus; 
         if(status&0x1)
         {
    		 if (status&0x2)
    		 {
    		     type = AP_NET_TYPE_ADHOC;
    		 }
    		 else
    		 {
    		     type = AP_NET_TYPE_WIFI;
    		 }
    		 break;
    	 }
    }
    
    return type;
}

/*-------------------------------------------------------------
*������hy_ap_get_connect_infor
*���ܣ��õ������ϵ�ap����Ϣ
*������none
*���أ�
*------------------------------------------------------------*/
tWMI_BECON_BUFF *hy_ap_get_connect_infor(void)
{
    tWMI_PARSE_BECON *pPos = NULL;
    tWMI_BECON_BUFF  *pBuf = NULL;
	
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
         if(pPos->beconBuff.apStatus&0x1)
         {
    		 pBuf = &pPos->beconBuff;
    		 break;
    	 }
    }
    
    if(pBuf == NULL)
    {
    	//���û�д�ap�б����ҵ�ap,�������û�о���ɨ���ֱ�����ӵ��ϴ�AP
    	pBuf = WifiConnect_GetCurAp();
    }
    
    return pBuf;
}

/*-------------------------------------------------------------
*������hy_ap_updata_ap_list
*���ܣ�����ap list ��Ϣ
*������none
*���أ�none
*------------------------------------------------------------*/
void hy_ap_updata_ap_list(int type)
{
    tWMI_PARSE_BECON *pPos = NULL, *pPos1 = NULL;
	int status;
	
	if (gu8Wifi_ConnectState == 1)  return;
	
	llist_for_each_entry_safe(pPos,pPos1,tWMI_PARSE_BECON,&tApList,list)
    {
         status = pPos->beconBuff.apStatus; 
         if(status&0x1)
         {

    		 if (AP_LIST_REFRESH == type)
    		 {
    		     pPos->beconBuff.apStatus &= (~1);
    		 }
    		 else if (AP_LIST_PART_DEL == type)
    		 {
    		     llist_del(pPos); 
    		     unifi_free(NULL, pPos);
    		 }   
    		 break;
    	 }
    }
    
    return ;
}

/*-------------------------------------------------------------
*������hy_ap_rescan_tidy_list
*���ܣ�ɨ��ʱ��������һ��ap list,��û������ap��Ϣ��ɾ����
*������none
*���أ�none
*------------------------------------------------------------*/
void hy_ap_rescan_tidy_list(void)
{
    tWMI_PARSE_BECON *pPos = NULL, *pPos1 = NULL;

    llist_for_each_entry_safe(pPos,pPos1,tWMI_PARSE_BECON,&tApList,list)
    {
         if(!(pPos->beconBuff.apStatus&0x1))
         {
    		 llist_del(pPos);
    		 unifi_free(NULL, pPos);
    	 }
    }
}/*-------------------------------------------------------------
*������hy_ap_get_cur_encryptionType()
*���ܣ��õ������ϵ�ap�ļ�������
*������none
*���أ�
*------------------------------------------------------------*/
eWMI_ENCRYPTION_TYPE hy_ap_get_cur_encryptionType(void)
{
    tWMI_PARSE_BECON *pPos = NULL;
    tWMI_BECON_BUFF  *pBuf = NULL;
	
	llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
    {
         if(pPos->beconBuff.apStatus&0x1)
         {
    		 pBuf = &pPos->beconBuff;
    		 break;
    	 }
    }
    
    return pBuf->wmiEncryptionType;
}

int hy_ap_setinfo_byidx(U8 idx)
{
	int ret = -1;
	tWMI_BECON_BUFF *pap;
	
	pap = hy_get_ap_info_sort(idx);
	if(pap != NULL)
	{
		mlme_update_scan_info(&(pap->ies), &(pap->msi), pap->wmiEncryptionType);
		ret = 0;
	}
	
	return ret;
}

int hy_ap_setinfo(tWMI_BECON_BUFF *pap)
{
	int ret = 0;

	mlme_update_scan_info(&(pap->ies), &(pap->msi), pap->wmiEncryptionType);
	
	return ret;
}

