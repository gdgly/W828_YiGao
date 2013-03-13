
/*--------------------------------------------------------------------------
Standard include files:
--------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "glbStatusDef.h"
#include "glbCmdDef.h"
#include "string.h"

/*--------------------------------------------------------------------------
*INCLUDES
--------------------------------------------------------------------------*/
#include "CrcVerify.h"
#include "glbVariable_base.h"
#include "glbVariable.h"


/*--------------------------------------------------------------------------
*DEFINES
--------------------------------------------------------------------------*/
#define CRC_OK		0
#define CRC_ERR		1


extern const U8 gu8Lcd_BackLight[];
/*--------------------------------------------------------------------------
*TYPEDEFS
--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
*STRUCTURE
--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
* Variable ,constant or function
--------------------------------------------------------------------------*/
//#pragma arm section zidata="App_Parameter", rwdata="App_Parameter"
// system parameter

glb_HycParameter_t   	gtHyc;
glb_HycParameter_Bak_t	gtHycBak;
//#pragma arm section


void glbAllVariable_SaveParameter(void);
void glbBakVariable_SaveToPersistentArea(void);


void glbVariableBak_LoadDefaultValue()
{
	memset((char *)&gtHycBak, 0, sizeof(glb_HycParameter_Bak_t));
	strcpy(gtHycBak.companyId, "000000");
	strcpy(gtHycBak.key, "0000000000");
}

/*-----------------------------------------------------------------------------
* ����:	glbVariable_LoadDefaultValue
* ����:	loadϵͳ����Ĭ��ֵ
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void glbVariable_LoadDefaultValue(void)
{
	int i;
	U8 *Serial;
	U32 crc ;
	
	gtHyc.ScanSound		= 1;//ɨ���������أ�0����  1����
	gtHyc.KeySound		= 0;//����������    0����  1����
	gtHyc.RemindType	= 1;//��ʾ��ʽ      0����  1: ��
	gtHyc.BackLight 	= gu8Lcd_BackLight[1];//
	
	gtHyc.ParamCfg = 0x0;
	
	gtHyc.ScreenProtect	= cSCREENPROTECT_DEFAULT;//����
	
	gtHyc.scanMode	   = 0;//��ס�����ɿ��ر�(�3-4��)
	gtHyc.sendType     = 0;//�Զ�����
	gtHyc.autoSendTime = 2;
	gtHyc.autoSendNum  = 50;
	gtHyc.printTimes = 1;
    
    gtHyc.gpsSwitch = 0;
    gtHyc.gpsGetInterval = 5;
	gtHyc.gpsSendInterval= 30;
	gtHyc.workDay = 0;//Ĭ��Ϊ0��
	
	//gtHyc.NetSelected = 1;//gsm
	gtHyc.NetSelected = 0;//wifi
	gtHyc.UrlSelected = 0;//��������
	gtHyc.GprsAccess = 0 ; /* cmnet */
	
	memset(gtHyc.MachineCode,0,18);		//�ն˱��
	autDrv_ds28cn01_readSN(gtHyc.MachineCode);
	
	strcpy(gtHyc.simCard, "00000000000");
	
	gtHyc.ipMode  = 0;
	gtHyc.dnsMode = 0;
	memset(gtHyc.ipAddr, 0, 16);
	memset(gtHyc.mask, 0, 16);
	memset(gtHyc.gateWay, 0, 16);
	memset(gtHyc.dns1, 0, 16);
	memset(gtHyc.dns2, 0, 16);
	
	//ap��Ϣ
	memset(gtHyc.apInfo, 0, sizeof(tAPINFO)*WIFI_SAVEPW_MAX);
	
	//memset(gtHyc.bt_addr1,0,16);
	memset(gtHyc.bt_addr2,0,16);
	strcpy(gtHyc.bt_pwd,"0000");
	gtHyc.bt_devenable = 3;
	
	gtHyc.touchParam.TouchOffsetX = TOUCH_DEFAULT_X_OFF;
	gtHyc.touchParam.TouchOffsetY = TOUCH_DEFAULT_Y_OFF;
	gtHyc.touchParam.TouchCoffX   = TOUCH_DEFAULT_X_COEF;
	gtHyc.touchParam.TouchCoffY   = TOUCH_DEFAULT_Y_COEF;
	
    ts_parameter_set(&gtHyc.touchParam);
    
    memset(&gtHyc.lastLogin, 0, sizeof(tLAST_LOGIN));
    
    /////////////////////////////////////////////////
//	strcpy(gtHyc.key, "JKrqwU3sSTfMh4bu");//ϵͳĬ��key
	//strcpy(gtHyc.key, "E8221538C0B04336");//zto
	strcpy(gtHyc.key, "0000000000");
	
	strcpy(gtHyc.companyId, "000000");
	strcpy(gtHyc.companyName, "��������");
	
	//�鿴�������Ƿ���ȷ,�����ȷ��ʹ�ñ��������
	Mutex_Protect();
	
	memset((char*)&gtHycBak,0,sizeof(glb_HycParameter_Bak_t));
	HSA_GetPersistentParameter(1,(char*)&gtHycBak, 0, sizeof(glb_HycParameter_Bak_t));
	crc = Hyc_crc32((char *)(&gtHycBak)+4,sizeof(glb_HycParameter_Bak_t)-4);
	if(crc != gtHycBak.CRC)
	{
		glbVariableBak_LoadDefaultValue();
		strcpy(gtHycBak.companyId, gtHyc.companyId);
		strcpy(gtHycBak.key, gtHyc.key);
		glbBakVariable_SaveToPersistentArea();
	}
	else
	{
		strcpy(gtHyc.companyId, gtHycBak.companyId);
		strcpy(gtHyc.key, gtHycBak.key);
		glbBakVariable_SaveToPersistentArea();
	}
	
	Mutex_Unprotect();
	
	gtHyc.defaultId = 1;//��1��ʼ
	//������IP  (���ã���������ʧ��ʱʹ��)
	memset(gtHyc.serverIP[0],0,18);
	memset(gtHyc.serverIP[1],0,18);
	memset(gtHyc.serverIP[2],0,18);
	
	//����(λ��������30λ����)
	strcpy(gtHyc.serverDNS[0],"opws.yundasys.com");
	strcpy(gtHyc.serverDNS[1],"opws.yundasys.com");
	strcpy(gtHyc.serverDNS[2],"opws.yundasys.com");
	
	strcpy(gtHyc.serverPort1,"9900");	//�������˿�(�����շ�ר��)
	strcpy(gtHyc.serverPort2,"9900");	//�������˿�(�����������ר��)
	
	memset(gtHyc.inquiryIP,0,18);		//���״̬���߲�ѯ������IP
	
	strcpy(gtHyc.inquiryPort,"9900");	//���״̬���߲�ѯ�˿�
	
	//memset(gtHyc.url, 0, sizeof(gtHyc.url));
	//strcpy(gtHyc.url, "http://60.191.136.231/PDA.ashx");
	//strcpy(gtHyc.url, "http://120.90.2.126:8888/yiDaSystem/scanData_getAccessMethod.do");
	//strcpy(gtHyc.url, "http://120.90.2.126:8099/PDAService/PDAService.asmx");
	strcpy(gtHyc.url,"http://120.90.2.126:8099/pdaMsSql_WebService/PDAService.asmx");
	//strcpy(gtHyc.url, "http://211.154.154.106:22223/daJinSystem/pda_addFile.do");

	return;
}

//����0:��ȷ   -1:����
int glbVariable_CheckIp(unsigned char *pIp)
{
	int ret = -1;
	int len,i,cnt,lasttype,typecnt;
	unsigned char  val;
	
	len = strlen((const char *)pIp);
	if(len == 0)
	{
		ret = 0;
	}
	else if(len >= 7 && len <= 15 && pIp[0] >= '1' && pIp[0] <= '9')
	{
		//����ʽ(192.168.8.47)
		typecnt = 0;
		lasttype= -1;
		cnt = 0;
		for(i = 0; i < len; i++)
		{
			val = pIp[i];
			if(val >= '0' && val <= '9')//����
			{
				if(lasttype != 0)//�ϴβ�������
				{
					typecnt++;
					lasttype = 0;
					cnt = 1;
				}
				else
				{
					cnt++;
					if(cnt > 3)//�������ָ������ܳ���3��
					{
						break;
					}
				}
			}
			else if(val == '.')//.
			{
				if(lasttype != 1)//�ϴβ���.
				{
					typecnt++;
					lasttype = 1;
					cnt = 1;
				}
				else
				{
					cnt++;
					if(cnt > 1)//����.�ĸ������ܳ���1
					{
						break;
					}
				}
			}
			else 
			{
				break;
			}

			if(typecnt > 7)
			{
				break;
			}
		}
		
		if(i >= len)
		{
			ret = 0;
		}
	}
	
	return ret;
}


/*-----------------------------------------------------------------------------
* ����:	glbVariable_CheckValue
* ����:	���������ϵͳ����ֵ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void glbVariable_CheckValue(void)
{
	int i;		
	
	U32 ksrceenpro[]={cSCREENPROTECT_DEFAULT,cSCREENPROTECT_1MIN,
	                 cSCREENPROTECT_5MIN, cSCREENPROTECT_10MIN,
	                 cSCREENPROTECT_CLOSE};
	
	if(gtHyc.ScanSound > 1)	gtHyc.ScanSound = 1;//ɨ���������أ�0����  1����
	
	if(gtHyc.KeySound > 1)	gtHyc.KeySound = 0;//����������    0����  1����
	if(gtHyc.RemindType > 1)	gtHyc.RemindType = 1;//��ʾ��ʽ      0����  1: ��
	
	for(i = 0; i < 5; i++)
	{
		if(gtHyc.BackLight == gu8Lcd_BackLight[i])
		{
			break;
		}
	}
	if(i >= 5) gtHyc.BackLight 	= gu8Lcd_BackLight[2];//
	//hyhwLcd_lightSet(gtHyc.BackLight);
	
	if(gtHyc.ParamCfg > 0x07)
	{
		gtHyc.ParamCfg = 0x0;
	}
	
	for(i = 0; i < 5; i++)
	{
		if(gtHyc.ScreenProtect == ksrceenpro[i])
		{
			break;
		}
	}
	if(i >= 5) gtHyc.ScreenProtect	= cSCREENPROTECT_DEFAULT;//����
	BackLight_UpdataLightTime(gtHyc.ScreenProtect);
	
	if(gtHyc.scanMode > 1) gtHyc.scanMode = 0;
	if(gtHyc.sendType > 1) gtHyc.sendType = 0;
	if(gtHyc.autoSendNum < 20 || gtHyc.autoSendNum > 200) gtHyc.autoSendNum = 50;
	if(gtHyc.autoSendTime < 1 || gtHyc.autoSendTime > 30) gtHyc.autoSendTime = 1;
	
	if(gtHyc.printTimes < 1 || gtHyc.printTimes > 10) gtHyc.printTimes = 1;
    
    if(gtHyc.gpsSwitch > 1) gtHyc.gpsSwitch = 0;
    
    if(gtHyc.gpsGetInterval < 1 || gtHyc.gpsGetInterval > 60) gtHyc.gpsGetInterval = 5;
	if(gtHyc.gpsSendInterval < 5 || gtHyc.gpsSendInterval > 60) gtHyc.gpsSendInterval= 30;
	
	if(gtHyc.workDay > 23) gtHyc.workDay = 0;
	
	if(gtHyc.NetSelected > 1) gtHyc.NetSelected = 0;//wifi
	if(gtHyc.UrlSelected > 1) gtHyc.UrlSelected = 0;//��������
	if(gtHyc.GprsAccess > 3) gtHyc.GprsAccess = 0 ; /* cmnet */
	
	memset(gtHyc.MachineCode,0,18);		//�ն˱��
	autDrv_ds28cn01_readSN(gtHyc.MachineCode);
	
	if(strlen(gtHyc.simCard) != 11) strcpy(gtHyc.simCard, "00000000000");
	
	//wifi ip ��Ϣ
	if(gtHyc.ipMode > 1)
	{
		gtHyc.ipMode  = 0;
		gtHyc.dnsMode = 0;
		memset(gtHyc.ipAddr, 0, 16);
		memset(gtHyc.mask, 0, 16);
		memset(gtHyc.gateWay, 0, 16);
		memset(gtHyc.dns1, 0, 16);
		memset(gtHyc.dns2, 0, 16);
	}
	else
	{
		//���IP��Ϣ����ȷ��
		if(glbVariable_CheckIp(gtHyc.ipAddr) == -1 ||
		   glbVariable_CheckIp(gtHyc.mask) == -1   ||
		   glbVariable_CheckIp(gtHyc.gateWay) == -1||
		   glbVariable_CheckIp(gtHyc.dns1) == -1   ||
		   glbVariable_CheckIp(gtHyc.dns2) == -1)
		{
			gtHyc.ipMode  = 0;
			gtHyc.dnsMode = 0;
			memset(gtHyc.ipAddr, 0, 16);
			memset(gtHyc.mask, 0, 16);
			memset(gtHyc.gateWay, 0, 16);
			memset(gtHyc.dns1, 0, 16);
			memset(gtHyc.dns2, 0, 16);
		}
	}
    
    if(gtHyc.bt_devenable > 3) gtHyc.bt_devenable = 3;
	for(i=0; i<sizeof(gtHyc.bt_pwd); i++)
	{
		if (gtHyc.bt_pwd[i] == 0)
		{
			break;
		}
	}
	if(i == sizeof(gtHyc.bt_pwd))
	{
		strcpy(gtHyc.bt_pwd,"0000");
	}
    
    i = ts_is_valid_offset_coef(&gtHyc.touchParam);
	//if (i != HY_OK)
	{
		gtHyc.touchParam.TouchOffsetX = TOUCH_DEFAULT_X_OFF;
		gtHyc.touchParam.TouchOffsetY = TOUCH_DEFAULT_Y_OFF;
		gtHyc.touchParam.TouchCoffX   = TOUCH_DEFAULT_X_COEF;
		gtHyc.touchParam.TouchCoffY   = TOUCH_DEFAULT_Y_COEF;
	}
	
    ts_parameter_set(&gtHyc.touchParam);
    
    if(gtHyc.defaultId < 1 || gtHyc.defaultId > 3) gtHyc.defaultId = 2;
    
	return;
}



/*-----------------------------------------------------------------------------
* ����:	glbVariable_LoadFromPersistentArea
* ����:	��PersistentArea��load�����ϵͳ����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void glbVariable_LoadFromPersistentArea(void)
{
	U32 crc;
	U8	flag = 1;
	
	Mutex_Protect();
	// we have only 1 structure to save in Persistent area of Flash, the index is 0
	glb_GetPersistentParameter((char *)(&gtHyc), cPARAMETER_OFFSET,sizeof(glb_HycParameter_t));
	
	crc = Hyc_crc32((char *)(&gtHyc)+4,sizeof(glb_HycParameter_t)-4);

	if(crc != gtHyc.CRC)
	{
		glbVariable_LoadDefaultValue();
		flag = 0;
	}
	
	//���ݲ���
	memset((char*)&gtHycBak,0,sizeof(glb_HycParameter_Bak_t));
	HSA_GetPersistentParameter(1,(char*)&gtHycBak, 0, sizeof(glb_HycParameter_Bak_t));
	crc = Hyc_crc32((char *)(&gtHycBak)+4,sizeof(glb_HycParameter_Bak_t)-4);
	if(crc != gtHycBak.CRC)
	{
		glbVariableBak_LoadDefaultValue();
		if(flag == 1)//��ȫ�ֵ��������
		{
			strcpy(gtHycBak.companyId, gtHyc.companyId);
			strcpy(gtHycBak.key, gtHyc.key);
		}
		glbAllVariable_SaveParameter();
	}
	else if(flag == 0)
	{
		strcpy(gtHyc.companyId, gtHycBak.companyId);
		strcpy(gtHyc.key, gtHycBak.key);
		glbAllVariable_SaveParameter();
	}
	
	/*glb_GetPersistentParameter((char *)(&gtHycBak), cPARAMETER_BAK_OFFSET,sizeof(glb_HycParameter_Bak_t));
	
	crc = Hyc_crc32((char *)(&gtHycBak)+4,sizeof(glb_HycParameter_Bak_t)-4);
	
	if(crc != gtHycBak.CRC)
	{
		glbVariableBak_LoadDefaultValue();
		if(flag == 1)//��ȫ�ֵ��������
		{
			strcpy(gtHycBak.companyId, gtHyc.companyId);
			strcpy(gtHycBak.key, gtHyc.key);
		}
	}
	else if(flag == 0)
	{
		strcpy(gtHyc.companyId, gtHycBak.companyId);
		strcpy(gtHyc.key, gtHycBak.key);
	}*/
	Mutex_Unprotect();
}

/*-----------------------------------------------------------------------------
* ����:	glbVariable_SaveToPersistentArea
* ����:	�����ϵͳ������PersistentArea��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void glbVariable_SaveToPersistentArea(void)
{
	U32 crc;
	
	Mutex_Protect();
	ts_parameter_get(&gtHyc.touchParam);
	crc = Hyc_crc32((char *)(&gtHyc)+4,sizeof(glb_HycParameter_t)-4);	
	gtHyc.CRC = crc;
	
	glb_SetPersistentParameter((char *)(&gtHyc), cPARAMETER_OFFSET, sizeof(glb_HycParameter_t));
	
	Mutex_Unprotect();
}


/*-----------------------------------------------------------------------------
* ����:	glbBakVariable_SaveToPersistentArea
* ����:	�����ϵͳ������PersistentArea��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void glbBakVariable_SaveToPersistentArea(void)
{
	glb_HycParameter_Bak_t curPara;
	U32 crc;
	
	Mutex_Protect();
	
	//���ݲ���
	crc = Hyc_crc32((char *)(&gtHycBak)+4,sizeof(glb_HycParameter_Bak_t)-4);	
	gtHycBak.CRC = crc;
	
	HSA_GetPersistentParameter(1,(char*)&curPara, 0, sizeof(glb_HycParameter_Bak_t));
	
	if(memcmp((U8 *)&curPara,(U8 *)&gtHycBak,sizeof(glb_HycParameter_Bak_t)) != 0)
	{
		HSA_SetPersistentParameter(1,(char *)&gtHycBak, 0, sizeof(glb_HycParameter_Bak_t));
	}
	
	//glb_SetPersistentParameter((char *)(&gtHycBak), cPARAMETER_BAK_OFFSET, sizeof(glb_HycParameter_Bak_t));
	Mutex_Unprotect();
}


/*-----------------------------------------------------------------------------
* ����:	glbVariable_SaveParameter
* ����:	��ñ������״̬
* ����:	none
* ����:	����ϵͳ����
*----------------------------------------------------------------------------*/
void glbVariable_SaveParameter(void)
{
	glbVariable_SaveToPersistentArea();
	glb_SetPersistentParameterToNand();
}

/*-----------------------------------------------------------------------------
* ����:	glbAllVariable_SaveParameter
* ����:	��ñ������״̬
* ����:	none
* ����:	����ϵͳ����
*----------------------------------------------------------------------------*/
void glbAllVariable_SaveParameter(void)
{
	glbVariable_SaveToPersistentArea();
	glb_SetPersistentParameterToNand();
	
	glbBakVariable_SaveToPersistentArea();
}

U8 *glbVar_GetCompanyId()
{
	return gtHycBak.companyId;
	//return gtHyc.companyId;
}

/********************************************************
*������glbVar_MachineCode
*���ܣ���ȡ�û������(����оƬ)
*��������
*���أ��������
********************************************************/
U8 *glbVar_GetMachineId(void)
{
	return gtHyc.MachineCode;
}

U8 *glbVar_GetSysKey()
{
	return gtHyc.key;
}

U8 *glbVar_GetSimCode()
{
	return gtHyc.simCard;
}

U8 glbVar_GetSendType()
{
	return gtHyc.sendType;
}

U8 gblVar_GetSendCnt()
{
	return gtHyc.autoSendNum;
}

U32 gblVar_GetSendDelay()//����ת��tick
{
	U32 ticks;
	
	ticks = gtHyc.autoSendTime*6000;
	return ticks;
}


U8 gblVar_GetScanType()
{
	return gtHyc.scanMode;
}

U8 *glbVar_GetUrl()
{
	return gtHyc.url;
}

void glbVar_SetUrl(U8 *url)
{
	strcpy(gtHyc.url,url);
}

