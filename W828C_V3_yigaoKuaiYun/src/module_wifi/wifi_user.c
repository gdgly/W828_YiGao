#include "hyTypes.h"
#include "hyErrors.h"
#include "wifi_user.h"
#include "wmi.h"
#include "hyhwCkc.h"
#include "hyhwSDIO.h"
#include "hymwSDIO.h"
#include "hyhwGpio.h"
#include "hardWare_Resource.h"

extern U8 gu8Wifi_ConnectState;//wifi����״̬ 0:δ����  1:������

ADAPTER adapter;
U32 sdioDataTemp[512];

#pragma arm section rwdata="wifi_userBuf", zidata="wifi_userBuf"
U32 gSendDataBuff[ WIFIDRIVER_SENDBUFF_LENGTH ];
tWMI_DATA_SEND gDataBuffSend;
#pragma arm section /*#pragma arm section rwdata="wifi_userBuf", zidata="wifi_userBuf"*/


void drv_MutexProtect()
{
	Mutex_Protect();
}
void drv_MutexUnprotect()
{
	Mutex_Unprotect();
}


/***************************************************************************
* ����:        	mvWlan_initVariable
* ����:        	init wifi variable
* ����:        	
* ����:        	none
****************************************************************************/
void mvWlan_initVariable( void )
{
	U32 i;
    //send data buff; total num is 16
	gDataBuffSend.dataBuffSend = 0;
	gDataBuffSend.dataBuffRecv = 0; 
	gDataBuffSend.dataBuffNum  = 0;
	for( i = 0; i < DATA_BUFF_NUM; i ++ )
	{
		gDataBuffSend.dataBuff[i].pDataSend = gSendDataBuff;
		gDataBuffSend.dataBuff[i].dataSendLength = 0;
	}
	
	memset((U8 *)&adapter,0,sizeof( ADAPTER ));
	adapter.txCtrl = 0x5a0b;//0x4503;
	gu8Wifi_ConnectState = 0;//δ����
	
//	mvWIFI_setPsFlag(0);
}
void WiFi_Power_Enable(void)
{
	TCC7901_CKC_BCLKCTR0_REGISTER |= (BIT17); 	//sdio bus clk enable
	hyhwSDIO_SoftwareReset();					//reset sdio bus clk
	
#if 1
	//WL_VREG_EN  ����Ϊdisable����resetʱ��Ϊenable
	hyhwGpio_setAsGpio(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);
	hyhwGpio_setOut(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);
	hyhwGpio_setLow(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);
	syssleep(1);
	
	//wifiPowerEn
	hyhwWifiBt_powerCtrl(1);//�򿪵�Դ
	
	//RST
	hyhwGpio_setAsGpio(WIFI_RESET_PORT, WIFI_RESET_GPIO);
	hyhwGpio_setOut(WIFI_RESET_PORT, WIFI_RESET_GPIO);
	hyhwGpio_setHigh(WIFI_RESET_PORT, WIFI_RESET_GPIO);
	
	syssleep(1);//����һ��׼���뼶����ʱ
#endif
}

void WiFi_Power_Disable()
{
	TCC7901_CKC_BCLKCTR0_REGISTER &= ~(BIT17); 	//sdio bus clk disable
	
	hyhwWifiBt_powerCtrl(0);//�رյ�Դ
	
	//wifi reset
	hyhwGpio_setAsGpio(WIFI_RESET_PORT, WIFI_RESET_GPIO);
	hyhwGpio_setOut(WIFI_RESET_PORT, WIFI_RESET_GPIO);
	hyhwGpio_setLow(WIFI_RESET_PORT, WIFI_RESET_GPIO);
	
	//wifi vreg disable
	hyhwGpio_setAsGpio(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);
	hyhwGpio_setOut(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);
	hyhwGpio_setLow(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);

	//wifi reset pull down
	hyhwGpio_setAsGpio(WIFI_RESET_PORT, WIFI_RESET_GPIO);
	hyhwGpio_setIn(WIFI_RESET_PORT, WIFI_RESET_GPIO);
	hyhwGpio_setPullDown(WIFI_RESET_PORT, WIFI_RESET_GPIO);
	//wifi vreg pull down
	hyhwGpio_setAsGpio(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);
	hyhwGpio_setIn(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);
	hyhwGpio_setPullDown(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);

	hyhwGpio_setAsGpio(WIFI_SD_DAT3_PORT,WIFI_SD_DAT3_GPIO);
	hyhwGpio_setIn(WIFI_SD_DAT3_PORT,WIFI_SD_DAT3_GPIO);
	
	hyhwGpio_setAsGpio(WIFI_SD_DAT2_PORT,WIFI_SD_DAT2_GPIO);
	hyhwGpio_setIn(WIFI_SD_DAT2_PORT,WIFI_SD_DAT2_GPIO);
	
	hyhwGpio_setAsGpio(WIFI_SD_DAT1_PORT,WIFI_SD_DAT1_GPIO);
	hyhwGpio_setIn(WIFI_SD_DAT1_PORT,WIFI_SD_DAT1_GPIO);
	
	hyhwGpio_setAsGpio(WIFI_SD_DAT0_PORT,WIFI_SD_DAT0_GPIO);
	hyhwGpio_setIn(WIFI_SD_DAT0_PORT,WIFI_SD_DAT0_GPIO);
	
	hyhwGpio_setAsGpio(WIFI_SD_CMD_PORT,WIFI_SD_CMD_GPIO);
	hyhwGpio_setIn(WIFI_SD_CMD_PORT,WIFI_SD_CMD_GPIO);
	
	hyhwGpio_setAsGpio(WIFI_SD_CLK_PORT,WIFI_SD_CLK_GPIO);
	hyhwGpio_setIn(WIFI_SD_CLK_PORT,WIFI_SD_CLK_GPIO);
	
	#if 0	//828��û�����IO
	hyhwGpio_setAsGpio(WIFI_HOST_WAKEUP_PORT,WIFI_HOST_WAKEUP_GPIO);
	hyhwGpio_setIn(WIFI_HOST_WAKEUP_PORT,WIFI_HOST_WAKEUP_GPIO);
	#endif
}

void mvWlan_wifiDeInit( void )
{
#if 0
	Sip_StopService();
	netdev_stop_tcpip();
	if (hy_get_ap_link_status() == WLAN_STATUS_CONNECT)
	{
		netdev_disconnect();
	}
	WifiTimer3_DeInit();//����ʹ��timer2���ѽ���
	wifi_stop_drv();
	mvWlan_initVariable();
	AppCtrl_DeletePro(WIFI_DRIVER_AN);
	Wifi_Power_OnOff(0);
	
	hyhwGpio_setAsGpio(PWR_WIFI_BT_ON_PORT, PWR_WIFI_BT_ON_GPIO); //wifiPowerEn Power Down
	hyhwGpio_setIn(PWR_WIFI_BT_ON_PORT, PWR_WIFI_BT_ON_GPIO);
	hyhwGpio_setAsGpio(WIFI_RESET_PORT, WIFI_RESET_GPIO); //RST
	hyhwGpio_setIn(WIFI_RESET_PORT, WIFI_RESET_GPIO);
//	hyhwGpio_setAsGpio(PORT_B,BIT8); //WIFI_PD ���GPIO���Ժ������
//	hyhwGpio_setIn(PORT_B,BIT8);		
/*	û�з��ֵ����������ʱ������
	hyhwGpio_setAsGpio(PORT_C,BIT4); //D0
	hyhwGpio_setIn(PORT_C,BIT4);
	hyhwGpio_setAsGpio(PORT_C,BIT5); //D1
	hyhwGpio_setIn(PORT_C,BIT5);
	hyhwGpio_setAsGpio(PORT_C,BIT6); //D2
	hyhwGpio_setIn(PORT_C,BIT6);
	hyhwGpio_setAsGpio(PORT_C,BIT7); //D3
	hyhwGpio_setIn(PORT_C,BIT7);
	
	hyhwGpio_setAsGpio(PORT_C,BIT10); //CLK
	hyhwGpio_setIn(PORT_C,BIT10);
	hyhwGpio_setAsGpio(PORT_C,BIT11); //CMD
	hyhwGpio_setIn(PORT_C,BIT11);
*/
	
	TCC7901_CKC_PCLKCFG13_REGISTER = 0; 		//ֹͣSDIOʱ�� ��ЩҪȷ�ϵ�
	TCC7901_CKC_BCLKCTR0_REGISTER &= ~(BIT17); 	//sdio bus clk disable
#endif
}

void WiFi_Reset(void)
{
	//WL_VREG_EN  ����Ϊdisable����resetʱ��Ϊenable
	hyhwGpio_setAsGpio(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);
	hyhwGpio_setOut(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);
	hyhwGpio_setHigh(WL_VREG_EN_PORT, WL_VREG_EN_GPIO);
	syssleep(1);
	
	hyhwGpio_setAsGpio(WIFI_RESET_PORT, WIFI_RESET_GPIO);
	hyhwGpio_setOut(WIFI_RESET_PORT, WIFI_RESET_GPIO);
	hyhwGpio_setLow(WIFI_RESET_PORT, WIFI_RESET_GPIO);
	#if 0
	{
		int i;
		i=500000;
		while(i--);
	}
	#else
	syssleep(5);
	#endif
	hyhwGpio_setHigh(WIFI_RESET_PORT, WIFI_RESET_GPIO);
}

int mvWlan_wifiInit(void)
{
	int i;
	
	mvWlan_initVariable();
	
	hyhwGpio_setSD1Function();
	hyhwSDIO_sdPortSelect( port0, port1 );
    hyhwSDIO_Initialize(Divisor_2, TmclkMult2Exp25);
    
	hyhwSDIO_mDisCardIntStatus();
	hyhwSDIO_mDisCardIntSignal();
	i = 10;
	do{
		if((i--) == 0)
		{
			PhoneTrace(1, "wifi sdio init err");
			hyUsbMessage("wifi sdio init err\r\n");
			return HY_ERROR;
		}
        hyhwCpm_setMmcClk(PERI_CLK_16M);
    	WiFi_Power_Enable();
    	WiFi_Reset();
    }while(hymwSDIO_Initalize()); 

	
	#if 1
	{//set mac����e2prom�ж�ȡ
		int ret, i;
		char mac[8];
		char validMacaddr_FF[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
		char validMacaddr_00[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		ret = hyhwI2c_readData(0, 0xA0, 0xFA, mac, 6);
		hyUsbPrintf("e2prom read ret = %d\r\n", ret);
		if (ret==HY_OK && (memcmp(mac, validMacaddr_FF, 6)!=0 && memcmp(mac, validMacaddr_00, 6)!=0))
		{
			hyUsbPrintf("e2prom mac = %02x-%02x-%02x-%02x-%02x-%02x\r\n",
					mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		}
		else
		{//set mac�����ݼ���оƬ����һ��
			#include "DS28CN01.h"
			char *pSerial;
			char tempMac[6] = {0x00, 0x11, 0xf6, 0x00, 0x00, 0x00};
			memcpy(mac, tempMac, 6);
			pSerial = &ds28cn01SerialNum.serialNum[0];
			mac[3] = 0xA0+(pSerial[3]&0x0F);
			mac[4] = pSerial[2];
			mac[5] = pSerial[1];
			
			hyUsbPrintf("make mac = %02x-%02x-%02x-%02x-%02x-%02x\r\n",
						mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		}
		hy_unifi_setMac(&mac[0]);
	}
	#else
	{//set mac�����ݼ���оƬ����һ��
		#include "DS28CN01.h"
		char *pSerial;
		char mac[6] = {0x00, 0x11, 0xf6, 0x00, 0x00, 0x00};
		pSerial = &ds28cn01SerialNum.serialNum[0];
		mac[3] = 0xA0+(pSerial[3]&0x0F);
		mac[4] = pSerial[2];
		mac[5] = pSerial[1];
		
		hy_unifi_setMac(&mac[0]);
	}
#endif

	if (hy_unifi_init() != 0)
	{
		PhoneTrace(1, "unifi init err");
		hyUsbMessage("wifi unifi init err\r\n");
		hy_unifi_deinit();
		return HY_ERROR;
	}
	return HY_OK;
}

void mvWlan_setMacAddress(U8 *pMacAddr)
{
	memcpy(adapter.macAddr, pMacAddr, 6);
	//memcpy(gtHyc.MACbuf, adapter.macAddr, 6);
}

/*-----------------------------------------------------------------------------
* ����:	mvWlan_txData
* ����:	tx data
* ����:	length 	-- 	the data length
		pData	--	the data 
		pDestMac-- 	the dest mac
		etherNetType -- the etherNetType
* ����:	error flag
*------------------------------------------------------------------------------*/
U32 mvWlan_txData( U16 length, U8 * pData, U8 * pDestMac, U16 etherNetType )
{
#if 0
	TX_DATA_STRUCT * pTxData;
	U8 * pos;
	tATH_MAC_HDR *macHdr;
	tATH_LLC_SNAP_HDR *llcHdr;
	U16 typeorlen, BTypeorlen;
	U16 totalLength;
	
	//802.3 MAC + 802.2 LLC SNAP
//	HeadLength = (U16)(sizeof(tATH_MAC_HDR) + sizeof(tATH_LLC_SNAP_HDR));
	totalLength = 50+length;//(28+22)
#ifdef BLOCK256	
	totalLength = (U16)(( totalLength + 255 ) & 0xff00);
#else	
	totalLength = (U16)(( totalLength + 31 ) & 0xffe0);
#endif		

    Mutex_Protect();
	if( mvSbi_getSendBuff( (U32 *)&pos, totalLength))
	{
		Mutex_Unprotect();
		return HY_ERROR;
	} 	
	
	pTxData = (TX_DATA_STRUCT *)pos;
	
	pTxData->totalLength = (U16)(50+length);
	pTxData->dataType = (U16)(MVSD_DAT);
	pTxData->txStatus = (U32)01;
	pTxData->txControl = (U32)0x7200;//adapter.txCtrl;
	pTxData->rxPacketLocation = (U32)(24);
	pTxData->rxPacketLength = (U16)(length + 22);
	pTxData->TxDestAddrHigh = (U16)(pDestMac[0]|(pDestMac[1]<<8));
	pTxData->TxDestAddrLow = (U32)(pDestMac[2]|(pDestMac[3]<<8)|(pDestMac[4]<<16)|(pDestMac[5]<<24));//*(U32 *)pDestMac;
	pTxData->priority = (U8)0;
	pTxData->flags = (U8)0;
	pTxData->pktDelay_2ms = (U8)0;
	pTxData->reserved	  = (U8)0;	

	pos += 28;
	
	// add ..........
	
	// ��� 802.3 MAC ��header��
	// ��ʽ��| Ŀ�ĵ�ַ  | Դ��ַ | ����  |
	macHdr = (tATH_MAC_HDR *)pos;
	memcpy(macHdr->dstMac,pDestMac,6);
	memcpy(macHdr->srcMac,adapter.macAddr,6);
	typeorlen = 8 + length;	//sizeof(tATH_LLC_SNAP_HDR)
	BTypeorlen = ((typeorlen<<8)&0xFF00) | ((typeorlen>>8)&0x00FF);
	memcpy(&macHdr->typeOrLen,&BTypeorlen,2);
	
	// posָ�����
	pos += 14;	//sizeof(tATH_MAC_HDR);


	// ��� 802.2 LLC SNAP ��header��
	// ��ʽ��| DSAP | SSAP | cntl | org code | type |
	llcHdr = (tATH_LLC_SNAP_HDR *)pos;
	llcHdr->dsap      = 0xAA;
	llcHdr->ssap      = 0xAA;
	llcHdr->cntl      = 0x03;
	llcHdr->orgCode[0] = 0x00;
	llcHdr->orgCode[1] = 0x00;
	llcHdr->orgCode[2] = 0x00;
	typeorlen = etherNetType;//Э������
	BTypeorlen = ((typeorlen<<8)&0xFF00) | ((typeorlen>>8)&0x00FF);
	llcHdr->etherType = BTypeorlen;
	
	// bufferָ�����
	pos += 8;//sizeof(tATH_LLC_SNAP_HDR);
	
	//add end.........
	memcpy((U8 *)pos, (U8 *)(pData), length);
	
	Mutex_Unprotect();
	
	wakeup(&pWifiThreadWait);
	
#endif
	return HY_OK;
}

/***************************************************************************
* ����:        	mvWlan_scan
* ����:        	scan ap
* ����:        	
* ����:        	error flag
****************************************************************************/
U32 mvWlan_scan( void )
{
#if 0
	U8 chanel;
	for(chanel = 1; chanel < 14; chanel ++ )
	{
		mvWmi_cmdScan( chanel );
	}
#endif
	return HY_OK;
}
/***************************************************************************
* ����:        	mvWlan_bgScan
* ����:        	scan ap
* ����:        	
* ����:        	error flag
****************************************************************************/
U32 mvWlan_bgScan( void )
{
#if 0
	U8 channel;
	for(channel = 1; channel < 14; channel ++ )
	{
		mvWmi_cmdBgScanCfg( HostCmd_ACT_SET, 1, channel );
		syssleep(30);
		if( adapter.nextDo == MV_NEXT_DO_SEND_SCAN_QUERY_CMD )
		{
			adapter.nextDo = MV_NEXT_DO_IDDLE;
			mvWmi_cmdBgScanQuery();
		}
	}
	
	mvWmi_cmdBgScanCfg( HostCmd_ACT_SET, 0, channel );
#endif
	return HY_OK;
}

/***************************************************************************
* ����:        	mvWlan_connect
* ����:        	scan ap
* ����:        	
* ����:        	error flag
****************************************************************************/
U32 mvWlan_connect( U8 * apMac )
{
#if 0
	tWMI_PARSE_BECON *pPos;
	
	if( adapter.mvWlanStatus ==  WLAN_STATUS_CONNECT)
	{
		llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
        {
            if (pPos->beconBuff.apStatus&0x1)
            {
                break;
            }
        }	
        
    	
		mvWmi_cmdDeAssociate( pPos->beconBuff.apBssInfo.bssid );
		
		syssleep(1);
		// do not know if event will come back!			
	}
	
	
	while( adapter.mvWlanStatus ==  WLAN_STATUS_CONNECT ) //wait scan rsp
	{
		syssleep(1);
	}
	mvWmi_cmdAssociate( apMac, NULL, 0);
	syssleep(10);
	
	return adapter.mvWlanStatus;
#else
	return 0;
#endif
}

/***************************************************************************
* ����:        	mvWlan_disConnect
* ����:        	disconnect ap
* ����:        	
* ����:        	error flag
****************************************************************************/
U32 mvWlan_disConnect( void )
{
#if 0
	U32 ret = SUCCESS;
    tWMI_PARSE_BECON *pPos;
    int flag = 0;

	if( adapter.mvWlanStatus ==  WLAN_STATUS_CONNECT)
	{
		
		llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
        {
            if (pPos->beconBuff.apStatus&0x1)
            {
                flag = 1; 
                break;
            }
        }
        
        if (flag == 0) return FAIL;
        
        
		if (pPos->beconBuff.wmiEncryptionType == WMI_CRYPTO_OPEN)
		{
			mvWmi_cmdDeAssociate( pPos->beconBuff.apBssInfo.bssid );
			
		}
		else //if (wifi_connect_crypto != WMI_CRYPTO_WEP)
		{
			Wpa_end();
			ret = SUCCESS;
			//PhoneTrace(0,"stop wpa");
		}

		pPos->beconBuff.apStatus &= (~1);
		// do not know if event will come back!
					
	}
	else
	{
	    llist_for_each_entry(pPos,tWMI_PARSE_BECON,&tApList,list)
        {
            if (pPos->beconBuff.apStatus&0x1)
            {
                pPos->beconBuff.apStatus &= (~1);
                break;
            }
        } 
	}
	
	wifi_ChkLink_clear_info();
	
	adapter.mvWlanStatus =  WLAN_STATUS_NOT_CONNECT;
	return ret;
#else
	return 0;
#endif
}
/***************************************************************************
* ����:        	mvWlan_rssi
* ����:        	disconnect ap
* ����:        	
* ����:        	error flag
****************************************************************************/
U32 mvWlan_getCurRssi( S32 * pSssi )
{
	//* pSssi = adapter.rssi;
	return HY_OK;
}
/***************************************************************************
* ����:        	mvWlan_wifiWorkTypeChange
* ����:        	change  wifi work type
* ����:        	
* ����:        	error flag
****************************************************************************/
extern U8 wpaWpa2Flag;
void mvWlan_wifiWorkTypeChange( eWifiWorkType workType )
{
#if 0
    //�жϵ�ǰ�Ƿ���wifi���̣����û����ֱ���˳�
    if (!AppCtrl_IsValidPro(WIFI_DRIVER_AN))
    {
        return;
    }	
	
	adapter.lastWorkType = adapter.workType;
	
	if( ( adapter.workType == workType_voip )&&( adapter.lastWorkType == workType_netWork ))
	{
		workType = workType_netWork;
	}
/**/
	//if(adapter.wifiEncrapt)	
	{
	//	return;
	}
	switch( workType )
	{
		
		case workType_normal:
		{

			adapter.workType = workType_normal;
			mvWmi_cmdPsMode(HostCmd_SubCmd_Exit_PS);
	//		if( adapter.lastWorkType != workType_netWork )
			{
	//			mvWmi_cmdInactivityTimeOut(0x32);
			}
			mvWmi_cmdPsMode(HostCmd_SubCmd_Enter_PS);
	
		}
		break;
		case workType_netWork:
		{
			
			adapter.workType = workType_netWork;
			mvWmi_cmdPsMode(HostCmd_SubCmd_Exit_PS);
	//		if( adapter.lastWorkType != workType_normal )
			{
	//			mvWmi_cmdInactivityTimeOut(0x32);
			}
			mvWmi_cmdPsMode(HostCmd_SubCmd_Enter_PS);
/**/		
		}
		break;
		case workType_voip:
		{
				
			mvWmi_cmdPsMode(HostCmd_SubCmd_Exit_PS);
	//		mvWmi_cmdInactivityTimeOut(0x4);
			adapter.workType = workType_voip;
			
/*			if( wpaWpa2Flag == 0 )
			{
				mvWmi_cmdPsMode(HostCmd_SubCmd_Enter_PS);
			}	
*/
		}
		break;
		case workType_adhoc:
		{
			adapter.workType = workType_adhoc;
		}
		default: 
		break;
	}
/**/
#endif
}

///////////////////////////////

/**************************************************************************
* ����:        	mvSbi_getSendBuff_num
* ����:        	��ȡwifi����buff�е����ݰ�����
* ����:        	none
* ����:        	����buff�е����ݰ�����
****************************************************************************/
U32 mvSbi_getSendBuff_num(void)
{
	return gDataBuffSend.dataBuffNum;
}

/**************************************************************************
* ����:        	mvSbi_getSendBuff
* ����:        	get a send buff
* ����:        	ppDataBuff  -- the buff for send;  dataLength -- the buf length
* ����:        	error flag
****************************************************************************/
U32 mvSbi_getSendBuff( U32 * ppDataBuff, U32 dataLength )
{
	U32 * pDataBuff;
	
	if(gDataBuffSend.dataBuffNum > 13 )		//��һ���ռ����� ��16
	{
	//hyUsbPrintf("gDataBuffSend.dataBuffNum = %d \r\n",gDataBuffSend.dataBuffNum);
 		//ewakeup(&EventWiFiDrv);
		return HY_ERROR;
	}
	
	while(1)
	{		
		pDataBuff = gDataBuffSend.dataBuff[gDataBuffSend.dataBuffRecv].pDataSend;
		
		if( pDataBuff < gDataBuffSend.dataBuff[gDataBuffSend.dataBuffSend].pDataSend )
		{
			if( (( dataLength + 3) >> 2 ) > (gDataBuffSend.dataBuff[gDataBuffSend.dataBuffSend].pDataSend - pDataBuff))
			{
				hyUsbPrintf("getSendBuff fail 11 \r\n");
				return HY_ERROR; //�Ų�����
			}
			else
			{
				break;
			}
		}
		else
		{
			if( (pDataBuff == gDataBuffSend.dataBuff[gDataBuffSend.dataBuffSend].pDataSend)&&( gDataBuffSend.dataBuffNum ) )
			{
				hyUsbPrintf("getSendBuff fail 11 \r\n");
				return HY_ERROR;//�Ų����� //1205 bug
			}
			
			if( (( dataLength + 3) >> 2 ) > ((&gSendDataBuff[WIFIDRIVER_SENDBUFF_LENGTH] ) - pDataBuff ))
			{
				gDataBuffSend.dataBuff[gDataBuffSend.dataBuffRecv].pDataSend = gSendDataBuff;//����BUFF��ʼ
			}
			else
			{
				break;
			}
		}
		
	}
	
	* ppDataBuff = (U32)pDataBuff;
	gDataBuffSend.dataBuff[gDataBuffSend.dataBuffRecv].dataSendLength = dataLength; //�������е����ݳ���
	
	gDataBuffSend.dataBuffNum ++;		//�пռ��ˣ��������������
	gDataBuffSend.dataBuffRecv ++;		//�������ָ����һ��
	gDataBuffSend.dataBuffRecv &= 0xf; 	//0~15	�������Ϊ0~15������15������
	gDataBuffSend.dataBuff[gDataBuffSend.dataBuffRecv].pDataSend = pDataBuff + (( dataLength + 3) >> 2 );	//���ݶ���ָ����һ��32BIT
	gDataBuffSend.dataBuff[gDataBuffSend.dataBuffRecv].dataSendLength = 0;	//�������㣬Ϊ���ж���û�б�д��

	return HY_OK;
}


/***************************************************************************
* ����:        	mvWlan_sendData
* ����:        	�������ݣ���WIFI���ͽ����﷢������,����DATA��CMD
* ����:        	
* ����:        	�����־
****************************************************************************/
U32 mvWlan_sendData(void)
{
	U32	*pSendBuffer;
	U32	buffLength;
	U32 sendNum;
	
	if(gDataBuffSend.dataBuffNum > 0)
	{
		pSendBuffer	 = gDataBuffSend.dataBuff[gDataBuffSend.dataBuffSend].pDataSend;
		buffLength	 = gDataBuffSend.dataBuff[gDataBuffSend.dataBuffSend].dataSendLength;
		if( buffLength )
		{
			//����
			WiFiDriver_UpdataSpeedDirect(1);
			//hyUsbPrintf("wifi send  %d  T=%d\r\n",buffLength, rawtime(NULL));
			unifi_tx_uip(pSendBuffer,buffLength);
			WiFiDriver_UpdataSpeedDirect(0);
		}
		gDataBuffSend.dataBuff[gDataBuffSend.dataBuffSend].dataSendLength = 0;
		gDataBuffSend.dataBuffNum  --;// ���ͳɹ���SIZE--��
		gDataBuffSend.dataBuffSend ++;
		gDataBuffSend.dataBuffSend &= 0xf;
	}
	
	return HY_OK;
}
