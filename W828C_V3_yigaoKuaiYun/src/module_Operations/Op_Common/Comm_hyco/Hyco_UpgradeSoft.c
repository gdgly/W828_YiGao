#include "hyTypes.h"
#include "grap_api.h"
#include "AbstractFileSystem.h"
#include "HycDeviceConfig.h"
#include "glbVariable.h"
#include "std.h"
#include "sys.h"
#include "ospriority.h"
#include "socket_api.h"
#include "http.h"
#include "Common_AppKernal.h"
#include "Common_Dlg.h"
#include "Net_Evt.h"
#include "Op_Struct.h"
#include "Op_RequestInfo.h"


#ifdef VER_CONFIG_HYCO



#define  UPGRADE_URL_LEN		256
#define  FILE_NAME_LEN			62


#define  ZLIB_BASE_FILE_NAME		"C:/image.zip"
#define  ZLIB_RESOURCE_NAME         "C:/resource.bin"

#define  SOFT_TEMP_BASE				"C:/base_temp.zip"
#define  SOFT_TEMP_SOURCE           "C:/resource_temp.zip"

#define  THREAD_INFO_FILE			"C:/thread.cfg"


#define  UPGRADE_THREAD_CNT		1//5	//�����ļ��Ľ�����

const U8 *const upgrade_name[]=
{
	(const U8 *)"upgrade1",(const U8 *)"upgrade2",(const U8 *)"upgrade3",(const U8 *)"upgrade4",
	(const U8 *)"upgrade5",(const U8 *)"upgrade6",(const U8 *)"upgrade7",(const U8 *)"upgrade8",
};


//�����������̵߳���ز���
typedef struct dnld_cell_t
{      
    int  start_pos;			//Ҫ�����ļ��Ŀ�ʼλ��
    int  total;				//������Ҫ���صĴ�С
    int  finished;			//���߳������صĴ�С
    int	 end;
    char *url;
    char *pBuf;
    
}tDLCELL;

tDLCELL gtUpgrade[UPGRADE_THREAD_CNT];

U8  gu8ThreadId,gu8ThreadEnd;
U8  gu8ThreadFail;


typedef struct softfile_info_t
{
	char name[FILE_NAME_LEN];
	U16  crc;
	U32  size;
	
}tSF_INFO;


tSF_INFO   gtSfInfo[4];

U8  gu8UpgradeState;//�Ƿ�������������  0��δ����   1��������
U8  gu8Path[50];
U8  gu8Url[UPGRADE_URL_LEN];
U8  gu8FileNum;	//��Ҫ�������ļ�����

U8	gu8UpgradgeFlag=0;	//������־  0:û��  1:��ǿ��  2:ǿ��

U8  gau8NewestSoftVer[20];//�������ϵ�����汾

extern const U8 *YDreq_ver[]; 
////////////////////////////////////////////////////////////////////////////////////////////////

/*----------------------------һ�¼�������ʱ���ع��õ�------------------*/
U8 Comm_GetUpFlag()
{
	return gu8UpgradgeFlag;
}

void Comm_SetUpFlag(U8 flag)
{
	gu8UpgradgeFlag = flag;
	
	return ;
}

U8 *Comm_GetNewestVer()
{
	return gau8NewestSoftVer;
}

void Comm_ClearSoftVer()
{
	memset(gau8NewestSoftVer, 0, sizeof(gau8NewestSoftVer));
}

void Comm_SetUpgradeState(U8 state)
{
	gu8UpgradeState = state;
}

U8 Comm_GetUpgradeState()
{
	return gu8UpgradeState;
}
/*-----------------------------------����end---------------------------------------*/

#define COMM_RECVBUF_LEN		16384
#define COMM_RANGE_STEP			10240//40960
int UpgradeThread()
{
	struct protocol_http_request request;
	U8     range[20],*url;
	U8     threadId = gu8ThreadId++;
	U8	   tError=0,maxErr=80;
	U8	   *pSave,*recvBuf;
	int    fd = -1,recvLen,err=0;
	int    start_pos,finished,total,curlen;
	U8	   i,startppp=0,gsmreset=0;
    struct timeval to;
    fd_set fs;
    
	start_pos = gtUpgrade[threadId].start_pos;
	finished  = gtUpgrade[threadId].finished;
	total     = gtUpgrade[threadId].total;
	url       = gtUpgrade[threadId].url;
	pSave     = gtUpgrade[threadId].pBuf;
	
	Com_SetPercentRecv(Com_GetPercentRecv() + finished);
	
	recvBuf = pSave+finished;
	recvLen = COMM_RECVBUF_LEN;
	
	sprintf(range,"%d-%d",start_pos+finished,start_pos+total-1);
	memset(&request, 0, sizeof(struct protocol_http_request));
    request.method 	= HTTP_GET_METHOD;
	request.user_agent 		= "hyco";
    request.content_type 	= NULL;
    request.content_length 	= 0;
    request.content_data 	= NULL;
    request.referer 		= NULL;
    request.range 			= range;
    request.connection		= 1;
    request.gzip			= 0;
    
	to.tv_sec = 5;
    to.tv_usec = 0;
//hyUsbPrintf("thread (%d) start(%d)  total(%d)  finished(%d) \r\n",threadId, start_pos,total,finished);

	while(!tError)
	{
		//�ж�gsm�Ƿ�����
		if(GsmReset_IfReset() == 1)//gsm������
		{
			syssleep(100);
			gsmreset = 1;
			continue;
		}
		
		startppp = 0;
		if(gsmreset == 1)//֮ǰ������gsm ��Ҫ���²���
		{//��Ҫppp���²��� (��Ŀǰ�����id��С��һ������)
			
			for(i = 0; i < threadId; i++)
			{
				if(gtUpgrade[i].end == 0)//��id��С�Ľ���û�н���, �����̲�Ҫ����ppp
				{
					break;
				}
			}
			if(i >= threadId)
			{
				startppp = 1;//��Ҫ�����̲���
			}
			else
			{
				//�ж�ppp״̬
				if(gprs_get_status() != HY_OK)
				{
					//ppp��û������
					syssleep(50);
					continue;
				}
				//hyUsbPrintf("ppp state = %d   id = %d \r\n",gprs_get_status(), threadId);
			}
		}
	//hyUsbPrintf("start ppp   id = %d  startppp = %d \r\n", threadId, startppp);
		//if(startppp == 1 && YDnet_StartGprs(NULL) != 1)
		if(startppp == 1 && Net_ConnectNetWork(NULL) != 1)
		{
			syssleep(200);
			err++;
			if(err > maxErr)
			{
			//hyUsbPrintf("thread StartGprs fail id == %d\r\n",threadId);
				tError = 1;
			}
			continue;
		}
		gsmreset = 0;
		startppp = 0;
		
		if(fd == -1)
		{
			fd = protocol_open(url, NULL, &request);
			if(fd == -1)
        	{
        		err++;
				if(err > maxErr)
				{
				//hyUsbPrintf("thread socket fail id == %d\r\n",threadId);
					tError = 1;
				}
				continue;
			}
			FD_ZERO(&fs);
	   		FD_SET(fd, &fs);
		}
		
		recvLen = COMM_RECVBUF_LEN;
        curlen  = 0;
        while ((recvLen=protocol_recv_t(fd,fs,to,recvBuf, recvLen)) > 0)
        {
        	curlen   += recvLen;
            finished += recvLen;
            Com_SetPercentRecv(Com_GetPercentRecv() + recvLen);
            recvLen  = COMM_RECVBUF_LEN;
           	recvBuf  = pSave+finished;
            if(finished >= total && !tError)
                break;
        }
//hyUsbPrintf("thread id(%d)  finished(%d)  total(%d) \r\n",threadId,finished,total);
        protocol_http_close(fd);
        fd = -1;
        if (finished == total)      //��ȷ����
        {
            break;
        }
        else if (finished < total)  //�ϵ�����
        {
        	err++;
        	//hyUsbPrintf("err cnt = %d    id = %d \r\n",err, threadId);
        	if(err >= maxErr)
        	{
        		tError = 1;
        		break;
        	}
        	sprintf(range,"%d-%d",start_pos+finished,start_pos+total-1);
        }
        else//�������
        {
        	//hyUsbPrintf("recv error ! id = %d \r\n",threadId);
        	break;
        }
	}
	gtUpgrade[threadId].finished = finished;
	gtUpgrade[threadId].end = 1;
	if(finished < total)
	{
		gu8ThreadFail++;
	}
	
	gu8ThreadEnd++;
	
	//hyUsbPrintf("----------- thread end   id = %d \r\n", threadId);
	
	exit(1);
	return 0;
}

/*
���������ļ��Ľ���
*/
int Comm_CreateThread(int filesize, int threadCnt, char *url, char *pBuf)
{
	int i,first=0,start=0,total,total_t;
	int pid,ret = -1;
	int fp;
	char path[40];
	
	total = filesize/threadCnt;
	first = filesize%threadCnt;
		
	gu8ThreadFail= 0;
	gu8ThreadEnd = 0;
	gu8ThreadId  = 0;
	
	memset((char *)&gtUpgrade[0], 0, sizeof(tDLCELL)*UPGRADE_THREAD_CNT);
	
	fp = AbstractFileSystem_Open(THREAD_INFO_FILE,AFS_READ);
	if(fp >= 0)
	{
		AbstractFileSystem_Read(fp,(char *)&gtUpgrade[0],sizeof(tDLCELL)*UPGRADE_THREAD_CNT);
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(THREAD_INFO_FILE,AFS_SINGLE_DELETE_FDM);
		//hyUsbPrintf("read thread cfg ok !!!!!!\r\n");
	}
	
	for(i = 0; i < threadCnt; i++)
	{
		if(i == 0)
		{
			total_t = total+first;
		}
		else
		{
			total_t = total;
		}
		
		sprintf(path, "C:/thread_%d.zip", i);
		if(gtUpgrade[i].start_pos == start && gtUpgrade[i].total == total_t && gtUpgrade[i].finished == total_t)
		{
			fp = AbstractFileSystem_Open(path,AFS_READ);
			if(fp >= 0)
			{
				AbstractFileSystem_Read(fp,pBuf+start,total_t);
				AbstractFileSystem_Close(fp);
				AbstractFileSystem_DeleteFile(path,AFS_SINGLE_DELETE_FDM);
				
				gu8ThreadId++;
				gu8ThreadEnd++;
				
				start += gtUpgrade[i].total;
				Com_SetPercentRecv(Com_GetPercentRecv() + gtUpgrade[i].total);
				//hyUsbPrintf("thread (%d) have finished \r\n",i);
				continue;
			}
			else
			{
				gtUpgrade[i].finished = 0;
			}
		}
		
		gtUpgrade[i].start_pos = start;
		gtUpgrade[i].total	   = total_t;
		gtUpgrade[i].end	   = 0;
		gtUpgrade[i].url       = url;
		gtUpgrade[i].pBuf      = pBuf+start;

		start += gtUpgrade[i].total;
		
		if(gtUpgrade[i].finished > 0)
		{
		//hyUsbPrintf("read thread(%d) file size = %d \r\n",i,gtUpgrade[i].finished);
			fp = AbstractFileSystem_Open(path,AFS_READ);
			if(fp >= 0)
			{
				AbstractFileSystem_Read(fp,(char *)gtUpgrade[i].pBuf,gtUpgrade[i].finished);
				AbstractFileSystem_Close(fp);
				AbstractFileSystem_DeleteFile(path,AFS_SINGLE_DELETE_FDM);
			}
			else
			{
				gtUpgrade[i].finished = 0;
			}
		}
		else
		{
			AbstractFileSystem_DeleteFile(path,AFS_SINGLE_DELETE_FDM);
		}
				
		pid = ntopid(upgrade_name[i]);
		if(pid > 0)
		{
			if(PSPROC(pid)->state == P_INACT)
			{
				ret = 0;
			}
		}
		if(ret != 0)
		{
			//���̲����ڣ���Ҫ����
			pid = createp((int (*)())UpgradeThread,
								upgrade_name[i],
								2048,
								PRIORITY_7_OSPP,
								NULL,NULL,NULL);
		}
		pstart( pid, (void *)UpgradeThread );
		syssleep(100);
	}
	
	return 0;
}


//0:�ɹ�   -1:ʧ��
int Comm_SaveFinalFile(tSF_INFO *pInfo, U8 *pBuf)
{
	int ret=-1;
	int fp;
	U16 crc;
	U8  *pTemp, *pFinal;

//hyUsbPrintf("save final filename(%s) \r\n",pInfo->name);
	if(memcmp("base_", pInfo->name, 5) == 0)
	{
		pTemp = SOFT_TEMP_BASE;
		pFinal= ZLIB_BASE_FILE_NAME;
	}
	else if(memcmp("resource_", pInfo->name, 9) == 0)
	{
		pTemp = SOFT_TEMP_SOURCE;
		pFinal= ZLIB_RESOURCE_NAME;
	}
	else
	{
		return -1;
	}
//hyUsbPrintf("save final file (%s) \r\n",pFinal);
	fp = AbstractFileSystem_Open(pFinal,AFS_READ);
	if(fp >= 0)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(pFinal,AFS_SINGLE_DELETE_FDM);
	}
	
	crc = glbVariable_CalculateCRC(pBuf,pInfo->size);
	
//hyUsbPrintf("save final file crc(%d)   infocrc(%d) \r\n",crc,pInfo->crc);
	if(crc == pInfo->crc)
	{
		fp = AbstractFileSystem_Create(pFinal);
		if(fp >= 0)
		{
			AbstractFileSystem_Write(fp, pBuf,pInfo->size);
			AbstractFileSystem_Close(fp);
			AbstractFileSystem_DeleteFile(pTemp,AFS_SINGLE_DELETE_FDM);
			ret = 0;
		}
	}
	else
	{
		AbstractFileSystem_DeleteFile(pTemp,AFS_SINGLE_DELETE_FDM);
	}
	
	return ret;
}

//1:�ɹ�,>1:ʧ��
int Comm_UpgradeSoft(void *p)
{
	int i,j,pid,ret = 7;
	int fp,curdown,fail = 0;
	U16 crc;
	U8  *pGetFile;
	U8  temp[64];
//U32 ticks;	
	
	ConSume_UpdataSpeedDirect(SHELL_AN, 120);
	
	if(Net_ConnectNetWork(NULL) == 1)
	{
		hyIE_init();
//ticks = rawtime(NULL);
//hyUsbPrintf("gu8FileNum=%d\r\n", gu8FileNum);

		for(i = 0; i < gu8FileNum; i++)
		{
			pGetFile = (U8 *)hsaSdram_UpgradeBuf();
			
			memset(gu8Url,0,UPGRADE_URL_LEN);
			if (strlen(gtHyc.serverDNS[gtHyc.defaultId-1]) > 0)
				sprintf(gu8Url,"http://%s:%s%s/%s",gtHyc.serverDNS[gtHyc.defaultId-1],gtHyc.serverPort2,gu8Path,gtSfInfo[i].name);
			else
				sprintf(gu8Url,"http://%s:%s%s/%s",gtHyc.serverIP[gtHyc.defaultId-1],gtHyc.serverPort2,gu8Path,gtSfInfo[i].name);
			
			Comm_CreateThread(gtSfInfo[i].size, UPGRADE_THREAD_CNT, gu8Url, pGetFile);
			while(gu8ThreadEnd < UPGRADE_THREAD_CNT)
			{
				syssleep(100);
			}
			
			for(j = 0; j < UPGRADE_THREAD_CNT; j++)
			{
				pid = ntopid(upgrade_name[j]);
				if(pid >= 0)
				{
					while(PSPROC(pid)->state != P_INACT)
					{
					    syssleep(1);
					}
					
					deletep(pid);
				}
				if(gu8ThreadFail > 0)//���߳�����ʧ��,������ʱ�߳����ص��ļ�
				{
					//hyUsbPrintf("thread fail  save thread (%d) file \r\n", j);
					sprintf(temp, "C:/thread_%d.zip",j);
					fp = AbstractFileSystem_Open(temp,AFS_READ);
					if(fp >= 0)
					{
						AbstractFileSystem_Close(fp);
						AbstractFileSystem_DeleteFile(temp,AFS_SINGLE_DELETE_FDM);
					}
					fp = AbstractFileSystem_Create(temp);
					if(fp >= 0)
					{
						AbstractFileSystem_Write(fp, gtUpgrade[j].pBuf,gtUpgrade[j].finished);
						AbstractFileSystem_Close(fp);
					}
				}
			}
			
			if(gu8ThreadFail > 0)//���߳�����ʧ��,�����߳���Ϣ�ļ�
			{
				fp = AbstractFileSystem_Open(THREAD_INFO_FILE,AFS_READ);
				if(fp >= 0)
				{
					AbstractFileSystem_Close(fp);
					AbstractFileSystem_DeleteFile(THREAD_INFO_FILE,AFS_SINGLE_DELETE_FDM);
				}
				fp = AbstractFileSystem_Create(THREAD_INFO_FILE);
				if(fp >= 0)
				{
					AbstractFileSystem_Write(fp,(char *)&gtUpgrade[0],sizeof(tDLCELL)*UPGRADE_THREAD_CNT);
					AbstractFileSystem_Close(fp);
				}
				
				ret = 6;
				
				break;
			}
		
			if(ret != 6)
			{
				ret = 5;
				pGetFile = (U8 *)hsaSdram_UpgradeBuf();
				if(-1 == Comm_SaveFinalFile(&gtSfInfo[i], pGetFile))
				{
				hyUsbPrintf("save file fail \r\n");
					ret = 7;
					break;
				}
			}
		}
		hyUsbPrintf("soft hyIE_destroy start    \r\n");
		hyIE_destroy();
	}
	
	ConSume_UpdataSpeedDirect(SHELL_AN, 120);
		
	return ret;
}

//����5 �ɹ�
int Comm_GetSoftInfo()
{
	struct protocol_http_request tRequest;
	int   ret=0;
	int   i,buflen,recvLen;
	U8    *pGetFile,*pTmp,*pEnd;
	U8    ver[10],buf[20];
	U8    *pVer,*dns;
	
	ConSume_UpdataAppCoef(SHELL_AN, 120);
	
	Com_SetPercentTotal(0);
	Com_SetPercentRecv(0);
	
	strcpy(gau8NewestSoftVer, "�汾��ȡʧ��");
	
	if(Net_ConnectNetWork(NULL) == 1)
	{
		pGetFile = (U8 *)hsaSdram_UpgradeBuf();
		buflen   = 20480;
	hyUsbPrintf("right = %d \r\n",Login_GetRight());
		if(Login_GetRight() != 0)//���ǹ���Ա
		{
			NetReq_FillRequestHead(REQ_UPGRADE,Login_GetReqHead(),glbVar_GetCompanyId(),Login_GetUserId());
		}
		else
		{
			sprintf(Login_GetReqHead(),"<h><ver>%s</ver><company>%s</company><user></user><pass>%s</pass><dev1>%s</dev1><dev2>%s</dev2></h>",
				Req_ver[REQ_UPGRADE],glbVar_GetCompanyId(),gtHyc.key,glbVar_GetMachineId(),glbVar_GetSimCode());
		}
		//sprintf(pGetFile,"<req op=\"%s\">%s<data><o><d>W900B</d></o></data></req>",Req_name[REQ_UPGRADE],Login_GetReqHead());
		sprintf(pGetFile,"<req op=\"%s\">%s<data><o><d>W828Z</d></o></data></req>",Req_name[REQ_UPGRADE],Login_GetReqHead());
//hyUsbPrintf("soft send = %s \r\n", pGetFile);		
		
		memset(&tRequest, 0, sizeof(struct protocol_http_request));
		tRequest.method         = HTTP_POST_METHOD;
	    tRequest.user_agent     = "hyco";
	    tRequest.content_length = strlen(pGetFile);
	    tRequest.content_data   = pGetFile;
	    tRequest.referer        = NULL;
	    tRequest.content_type   = "text/xml; charset=utf-8";
	    tRequest.range          = NULL;
	    tRequest.connection		= 0;
		tRequest.gzip			= 0;
		
		recvLen = http_send_request(gtHyc.url, buflen, &tRequest);
		//hyUsbPrintf("soft recv = %s \r\n", pGetFile);
		if(recvLen > 0)
		{
			pTmp = strstr(pGetFile, "ok");
			if(pTmp == NULL)//û�г����ļ�
			{
				ret = 10;
				goto getSoftInfo_quit;
			}
			ret = 5;
			
			//app ����    
			//<o><d>t=W900;v=2.1.003;n=2;f=app_86210_2.1.003_33943.zip,base_268990_2.1.003_10309.zip;u=/ws/hccupd</d></o>

			pTmp = strstr(pGetFile,"<o><d>t=");
			if(pTmp != NULL)
			{
				pTmp+=8;
				memset(ver,0,10);
				memcpy(ver,pTmp,6);
				if(0 != memcmp(ver,appVersionInfo,5))
				{
					ret = 10;
					goto getSoftInfo_quit;
				}
			}
			else
			{
				ret = 7;
				goto getSoftInfo_quit;
			}
			
			/* ��ȡ�汾�� */
			pTmp = strstr(pGetFile,"v=");
			if(pTmp != NULL)
			{
				pTmp+=2;
				memset(ver,0,10);
				memcpy(ver,pTmp,7);
			}
			else
			{
				ret = 7;
				goto getSoftInfo_quit;
			}

			/* ��ȡ�ļ��� */
			pEnd = pTmp;
			pTmp = strstr(pEnd,"n=");
			if(pTmp != NULL)
			{
				pTmp += 2;
				gu8FileNum = *pTmp-0x30;//��Ҫ�������ļ�����
			}
			else
			{
				ret = 7;
				goto getSoftInfo_quit;
			}

			pEnd = pTmp;
			pTmp = strstr(pEnd,"f=");
			if(pTmp != NULL)
			{
				pTmp += 2;
			}
			else
			{
				ret = 7;
				goto getSoftInfo_quit;
			}

			//�ļ�
			memset((char *)&gtSfInfo[0], 0, sizeof(tSF_INFO)*4);
			for(i = 0; i < gu8FileNum; i++)
			{
				pEnd = strstr(pTmp,"zip");
				if(pEnd == NULL)
				{
					ret = 7;
					goto getSoftInfo_quit;
				}
				pEnd += 3;
				if(pEnd - pTmp >= FILE_NAME_LEN)
				{
					ret = 7;
					goto getSoftInfo_quit;
				}
				//name
				memcpy(gtSfInfo[i].name, pTmp, pEnd - pTmp);
				
				//hyUsbPrintf("file(%d) name = %s \r\n",i,gtSfInfo[i].name);
				
				pEnd = strstr(pTmp,"_");
				if(pEnd == NULL)
				{
					ret = 7;
					goto getSoftInfo_quit;
				}
				pTmp = pEnd+1;
				
				//size
				pEnd = strstr(pTmp,"_");
				if(pEnd == NULL)
				{
					ret = 7;
					goto getSoftInfo_quit;
				}
				if(pEnd - pTmp >= 20)
				{
					ret = 7;
					goto getSoftInfo_quit;
				}
				memset(buf,0,20);
				memcpy(buf,pTmp,pEnd-pTmp);
				gtSfInfo[i].size = atoi(buf);
				
				Com_SetPercentTotal(Com_GetPercentTotal() + gtSfInfo[i].size);
				
				//hyUsbPrintf("file(%d) size = %d total = %d \r\n",i,gtSfInfo[i].size,gs32Total);
				
				//crc
				pTmp = pEnd+1;
				pEnd = strstr(pTmp,"_");
				if(pEnd == NULL)
				{
					ret = 7;
					goto getSoftInfo_quit;
				}
				pTmp = pEnd+1;
				pEnd = strstr(pTmp,".zip");
				if(pEnd == NULL)
				{
					ret = 7;
					goto getSoftInfo_quit;
				}
				if(pEnd - pTmp >= 20)
				{
					ret = 7;
					goto getSoftInfo_quit;
				}
				memset(buf,0,20);
				memcpy(buf,pTmp,pEnd-pTmp);
				gtSfInfo[i].crc = atoi(buf);
				
				//hyUsbPrintf("file(%d) crc = %d \r\n",i,gtSfInfo[i].crc);
				
				pTmp = pEnd+5;
			}

			//3  URL
			buf[0] = 0;
			pEnd = strstr(pTmp,"u=");
			if(pEnd != NULL)
			{
				pTmp = pEnd+2;
				pEnd = strstr(pTmp,";s=");
				if(pEnd == NULL)
				{
				pEnd = strstr(pTmp,"</d></o>");
				if(pEnd == NULL || pEnd-pTmp >= 50)
				{
						ret = 7;
						goto getSoftInfo_quit;
				}
				}
				else
				{
					//�Ƿ�ǿ������
					buf[0] = *(pEnd+3)-'0';
				}
				memset(gu8Path,0,50);
				memcpy(gu8Path,pTmp,pEnd-pTmp);
			}
			else
			{
				ret = 7;
				goto getSoftInfo_quit;
			}

			pTmp = appVersionInfo+10 ;
			strcpy(gau8NewestSoftVer, ver);//����������ϵİ汾
			if(memcmp(ver,pTmp,strlen(ver)) <= 0)
			{
				ret = 9;
			}
			
			if (ret == 9)
			{
				gu8UpgradgeFlag = 0;
			}
			else
			{
				if((buf[0] == 1 || buf[0] == 2) && (Net_GetEvt()&NET_CHECKSOFT_EVT))
				{
					gu8UpgradgeFlag = buf[0];
				}
			}
			
			//hyUsbPrintf("gu8UpgradgeFlag  = %d \r\n",gu8UpgradgeFlag);
		}
		else
		{
			ret = 7;//����ʧ��
		}
		
	}
	else
	{
		ret = 6;//��������ʧ��
	}
getSoftInfo_quit:
	ConSume_UpdataAppCoef(SHELL_AN, 0);
	
	return ret;
}

/*----------------------------------------------------------
* ����: Common_NetUpgrade
* ����: ��������
* ����: 
* ����: 
------------------------------------------------------------*/
U8 Common_NetUpgrade(U8 lpa, tGRAPDESKBOX *pDesk)
{
	U8  ret = 4;
	U8     		    retDlg;
	tCARTOONPARA	tCon;
	
	if(1 == lpa)
	{
		ret = Comm_GetSoftInfo();
	}
	else if(2 == lpa)
	{
		ret = Comm_UpgradeSoft(NULL);
	}
	else if(3 == lpa)//ǰ̨����
	{
		//�����¼�,�ȴ���̨��ͣ
		if(-1 == Net_ChangeEvt(pDesk, NET_UPDATE_SOFT_EVT))
		{
			return -1;
		}
		NetReq_SetFrontReqType(REQ_UPGRADE);
		
		Comm_SetUpgradeState(1);
		
		cartoon_app_entry(Comm_UpgradeSoft,NULL);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		ret = Com_CtnDlgDeskbox(pDesk,2,"��������,���Ե�!",NULL,NULL,Com_PercentLabTimerISR,4, 0x8FFFFFFF);
		AppCtrl_DeletePro(KERNAL_APP_AN);
		
		Comm_SetUpgradeState(0);
		
		if (ret == DLG_CONFIRM_EN)
		{
			retDlg = kernal_app_get_stat();
		
			if(5 == retDlg)
			{
				if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�������,��ػ����ٴο�������.",0, pDesk, 2, NULL, NULL, 0x8FFFFFFF))				
				{
					AppCtrl_PowerOffProc();
				}
			}
			else if(7 == retDlg)
			{
				Com_SpcDlgDeskbox("�ļ�����,������!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
			}
			else if(6 == retDlg)
			{
				Com_SpcDlgDeskbox("�������,���Ե�1��������!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
			}
			else
			{
				Com_SpcDlgDeskbox("�������,���Ե�1��������!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
			}
		}
		else
		{
			Com_SpcDlgDeskbox("�������,���Ե�1��������!",6, pDesk, 2, NULL, NULL, 0x8FFFFFFF);
		}
	}
	
	return ret ;
}


#endif //#ifdef VER_CONFIG_YUNDA