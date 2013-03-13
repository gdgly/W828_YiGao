#include "Op_RequestInfo.h"
#include "glbVariable_base.h"
#include "glbVariable.h"

#ifdef VER_CONFIG_HYCO

extern char	ga8CompanyVer[];


eNET_REQTYPE	geReqType;

//////////////////////////////////////////////////////////////////
//����ӿڵİ汾,
const U8 *Req_ver[] = 
{
	ga8CompanyVer,//0 ��ѯ��˾
	//"1.0",//��ѯ��˾  ǿ��ȫ������ʱ��ʹ�� ����ʹ���ļ��м�¼�İ汾
	
	"1.0", //1 ��ѯ����
	"1.0", //2 �����ն�ʱ��
	"2.0", //3 ��ѯȨ��
	"5.0", //4 ¼��
	"3.0", //5 ��ȡ¼����Ϣ
	"2.0", //6 ��ѯ����
	"2.0", //7 ��ѯ����
	"1.0", //8 �ϴ�λ����Ϣ
	"2.0", //9 ����λ����Ϣ
	
	"1.0", //10 ��ȡ��������
	
	//new -------------------------------
	"1.0", //11 ����
	"1.0", //12 ɨ��
	"1.0", //13 ����
	"1.0", //14 ����
	"1.0", //15 ж��
	"1.0", //16 װ��
	"1.0", //17 �⳵
	"1.0", //18 �⳵
	"1.0", //19 ֪ͨ
	"1.0", //20 ���������ȡ
	//-----------------------------------
			 
	"1.0",	//21 ��ѯ����   NOUSE
	"1.0",	//22 ��ѯ����� NOUSE	
	
	"1.0",	//23 �쳣������
	"1.0",	//24 ��ȡsim����
};
//�������  ע��һһ��Ӧ
const U8 * const Req_name[]=
{
	(const U8 *)"getcompanylist",	//0  ��ѯ��˾
	(const U8 *)"getusers",		    //1  ��ѯ����
	(const U8 *)"getlastcfg",		//2  �����ն���Ϣ
	(const U8 *)"getright",		    //3  ��ѯȨ��
	(const U8 *)"postdandata",      //4  ¼��
	(const U8 *)"getdocdtl",		//5  ¼�����ݻ�ȡ
	(const U8 *)"getpacksts",		//6  ��ѯ����
	(const U8 *)"getgunupdate",		//7  ��ѯ����
	(const U8 *)"postaddress",		//8  �ϴ�λ����Ϣ
	(const U8 *)"postgpsdata",		//9  ����λ����Ϣ	
	
	(const U8 *)"getsiteinfo",	    //10 ��ȡ��������
	// new -------------------------------------
	
	(const U8 *)"op01liu",		    //11 ����
	(const U8 *)"op02sao",		    //12 ɨ��
	(const U8 *)"op03pai",          //13 ����
	(const U8 *)"op04chz",		    //14 ����
	(const U8 *)"op05xcd",		    //15 ж��
	(const U8 *)"op06zcd",		    //16 װ��
	(const U8 *)"op07jcd",		    //17 �⳵
	(const U8 *)"op08fcd",		    //18 �⳵
	(const U8 *)"op09msg",		    //19 ֪ͨ
	(const U8 *)"getweivol",		//20 ���������ȡ
	//------------------------------------------
		
	(const U8 *)"getgroup",		    //21 ��ѯ���� NOUSE
	(const U8 *)"getproblem",		//22 �����	  NOUSE
	
	
	(const U8 *)"getabsigninfo",		//23 ��ȡ�쳣����
	(const U8 *)"getsimid",			//24 ��ȡsim����
};

//���ش������
const U8 * const Net_ErrorCode[]=
{
	(const U8 *)"��������",
	
	(const U8 *)"�˺Ż��������", //1���������
	(const U8 *)"����ȱʧ",       //2������ȱʧ
	(const U8 *)"�������",       //3���������
	(const U8 *)"ϵͳά��",       //4��ϵͳ����
	(const U8 *)"����У��ʧ��(�ֶ�ȱʧ)",  //5������У��ʧ�ܣ��ļ���ȫ���ֶ�ȱʧ��
	(const U8 *)"���ݿ��ж�(�޷���֤Ȩ��)",//6�����ݿ��жϣ��޷���֤Ȩ�ޣ�
	(const U8 *)"��¼���ʧ��"
};


///////////////////////////////////////////////////////////
void NetReq_SetFrontReqType(eNET_REQTYPE type)
{
	geReqType = type;
}

eNET_REQTYPE NetReq_GetFrontReqType()
{
	return geReqType;
}


/*-----------------------------------------
* ����: NetReq_FillRequestHead
* ����: ���ݲ�ͬ�ӿ�,�������ͷ
* ����: kReqType:��������   pHead:����ͷbuf  pId:Ա�����
* ����: 0
*-----------------------------------------*/
int NetReq_FillRequestHead(eNET_REQTYPE kReqType, U8 *pHead, U8 *pCode, U8 *pId)
{
	int ret = 0;
	
	if(kReqType < REQ_ALL)
	{
	hyUsbPrintf("FillRequestHead  id = %s \r\n", pId);
		if(kReqType > REQ_UPGRADE && kReqType < REQ_ALL)//ʹ���ϴε�¼��Ϣ��������(��Ҫ���˳���¼����ϴ�)
		{
			sprintf(pHead,"<h><ver>%s</ver><company>%s</company><user>%s</user><pass>%s</pass><dev1>%s</dev1><dev2>%s</dev2></h>",
			Req_ver[kReqType],glbVar_GetCompanyId(),gtHycBak.userID,gtHycBak.userPw,glbVar_GetMachineId(),glbVar_GetSimCode());
		}
		else if(strncmp(pId,"000000",6) == 0)//����Ա��¼ʹ��Ĭ����Կ
		{//����Ա
			sprintf(pHead,"<h><ver>%s</ver><company>%s</company><user></user><pass>%s</pass><dev1>%s</dev1><dev2>%s</dev2></h>",
			Req_ver[kReqType],glbVar_GetCompanyId(),glbVar_GetSysKey(),glbVar_GetMachineId(),glbVar_GetSimCode());
		}
		else if(REQ_RIGHT == kReqType)// ��½ʱ����
		{
			sprintf(pHead,"<h><ver>%s</ver><company>%s</company><user>%s</user><pass>%s</pass><dev1>%s</dev1><dev2>%s</dev2></h>",
					Req_ver[kReqType],pCode,pId,Login_GetPw_MD5(),glbVar_GetMachineId(),glbVar_GetSimCode());
		}
		else
		{
			sprintf(pHead,"<h><ver>%s</ver><company>%s</company><user>%s</user><pass>%s</pass><dev1>%s</dev1><dev2>%s</dev2></h>",
					Req_ver[kReqType],glbVar_GetCompanyId(),pId,Login_GetPw_MD5(),glbVar_GetMachineId(),glbVar_GetSimCode());
		}
		
		hyUsbPrintf("pHead  =  %s  \r\n", pHead);
	}
	else
	{
		ret = -1;
	}
	
	return ret;
}


int NetReq_FillUpdateTimeHead(U8 *pHead)
{
	sprintf(pHead,"<req op=\"%s\"><h><ver>%s</ver><company>%s</company><user></user>\
<pass>%s</pass><dev1>%s</dev1><dev2>%s</dev2></h></req>",
			Req_name[REQ_PDATIME],Req_ver[REQ_PDATIME],glbVar_GetCompanyId(),/*gtHyc.impower*/"JKrqwU3sSTfMh4bu",glbVar_GetMachineId(),glbVar_GetSimCode());
	
	return 0;
}


int NetReq_FillHead_ZTO(U8 *pHead, U8 *pType)
{
	int len;
	
	sprintf(pHead, "<?xml version=\"1.0\" encoding=\"utf-8\"?><root><h><ver>1.0</ver><optype>%s</optype><manufacturer>HYCO</manufacturer>\
<u_company>%s</u_company><u_code>%s</u_code><sys_sn>%s</sys_sn><key>%s</key></h>", 
			pType, glbVar_GetCompanyId(),Login_GetUserId(),glbVar_GetMachineId(),glbVar_GetSysKey());
	
	len = strlen(pHead);
	
	return len;
}


int NetReq_FillHead_yida(U8 *pHead, U8 *ver, U8 *pMethod)
{
	int len;
	
	sprintf(pHead, "<?xml version=\"1.0\" encoding=\"GBK\"?><senddata><accessMethod>%s</accessMethod><versionTime>%s</versionTime>\
<baseValue><dev>%s</dev><userName>%s</userName><password>%s</password><belongSite>%s</belongSite></baseValue>",
				pMethod, ver, glbVar_GetMachineId(),Login_GetUserId(),Login_GetUserPw(),glbVar_GetCompanyId());
	
	len = strlen(pHead);
	
	return len;
}

int NetReq_FillHead_DHL(U8 *pHead, U16 type, U8 *pName)
{
	char *str[]={"","SJ", "FJ", "DJ", "PJ", "ZD", "CD", "WT", "LC", "QS"};
	int len, name[100];
	RtcTime_st	kRtc;
hyUsbPrintf("type = %d   str = %s \r\n", type, str[type]);	
	if(pName == NULL)
	{
		hyhwRtc_GetTime(&kRtc);
		sprintf(name, "%s%04d%02d%02d%02d%02d%02d-%s-%s.TXT", str[type],kRtc.year,kRtc.month,kRtc.day,kRtc.hour,kRtc.minute,kRtc.second,
												glbVar_GetCompanyId(), glbVar_GetMachineId());
	}
	else
	{
		sprintf(name, "%s.jpg", pName);
	}
	
	sprintf(pHead, "<?xml version=\"1.0\" encoding=\"GBK\"?><senddata><userID>%s</userID><password>%s</password><belongSite>%s</belongSite><dev>%s</dev>\
<fileName>%s</fileName>",
				Login_GetUserId(),Login_GetUserPw(),glbVar_GetCompanyId(),glbVar_GetMachineId(),name);
	
	len = strlen(pHead);
	
	return len;
}

#endif //VER_CONFIG_HYCO