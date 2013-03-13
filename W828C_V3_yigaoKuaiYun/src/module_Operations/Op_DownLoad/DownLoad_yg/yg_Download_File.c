#include "socket_api.h"
#include "http.h"
#include "Op_DownLoad.h"
#include "AbstractFileSystem.h"

const U8 *const op_key[]=
{
	(const U8 *)"UpdataStation",  
	(const U8 *)"UpdataEmployee",
	(const U8 *)"UpdataProblem",
	(const U8 *)"UpdataKeepType",
	(const U8 *)"UpdataClass",
	(const U8 *)"UpdataDestAddr",
	(const U8 *)"UpdataFlight",
	(const U8 *)"UpdataCar",
	(const U8 *)"UpdataSite",
	(const U8 *)"UpadatSoft",
	(const U8 *)"getKinfo"
};
//
/*---------------------------------------------------------------------------*/
void file_load_delsave(U8 *path)
{
	int fp ;
	
	fp = AbstractFileSystem_Open(path,AFS_READ);
	if(fp >= 0)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(path,AFS_SINGLE_DELETE_FDM);
	}	
}
void file_load_savefail(U8 *ver ,U8 *buf,U32 savelen,U32 totallen,U8 *path)
{
	int fp ;
	
	if(ver == NULL || buf == NULL || savelen <= 0 || totallen <= 0 || NULL == path)
	return  ;
	
	fp = AbstractFileSystem_Open(path,AFS_READ);
	if(fp >= 0)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(path,AFS_SINGLE_DELETE_FDM);
	}
	fp = AbstractFileSystem_Create(path);
	if(fp >= 0)
	{
		U16 crc ;
		
		crc = glbVariable_CalculateCRC((U8 *)buf,savelen);
		
		AbstractFileSystem_Write(fp,ver,32);
		AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
		AbstractFileSystem_Write(fp,(char *)&totallen,sizeof(U32));
		AbstractFileSystem_Write(fp,buf,savelen);
		AbstractFileSystem_Close(fp);
	}
}
extern int gs32PercentTotal,gs32PercentRecv ;
#define GET_RECVBUF_LEN		16384

int http_get(U8 *url,U8 *buf, U32 len ,U32 savelen,U32 *downlen)
{
	struct protocol_http_request http_request;
	struct protocol_http_headers *requestHead;
	int sendLen,leftLen,revLen;
    int rv,ret;
    int fd;
    U8  *temp;
	int st;
    struct timeval to;
    fd_set fs;
	
	
	//wlock(&gu32LockIE);
	if(NULL == url || NULL == buf)
    {
    	//unlock(&gu32LockIE);
    	return -1;
    }
	
	if(-1 == hyIE_init())
	{
		//unlock(&gu32LockIE);
		return -1;
	}
    hyIE_clearDns();
		
	memset(&http_request, 0, sizeof(struct protocol_http_request));
	http_request.method = HTTP_GET_METHOD;
	
	if ((fd = protocol_open(url, NULL, &http_request)) < 0)
	{
		hyUsbPrintf("get errorr \r\n");
		ret = -1;
		goto HTTP_GET_END;
	}
	
	requestHead = protocol_get_headers(fd,NULL) ;
	if (0 == requestHead)
	{
		hyUsbPrintf("get errorr 11 \r\n");
		protocol_http_close(fd);
		ret = -1;
		goto HTTP_GET_END;
	}
	
	if(requestHead->content_length > len) //buff����
	{
		hyUsbPrintf("get errorr 22 \r\n");
		ret = -2;
		goto HTTP_GET_END;
	}
	
	//hyUsbPrintf("get len = %d \r\n",requestHead->content_length);
	
	leftLen = requestHead->content_length;
	gs32PercentTotal = leftLen ;
	revLen  = 0;
	temp    = buf;
	
	to.tv_sec = 5;
    to.tv_usec = 0;

    FD_ZERO(&fs);
    FD_SET(fd, &fs);

 {
 	int    start_pos,finished,total;
 	U8     range[20] ;
 	U8	   *pSave,*recvBuf;
	int    recvLen,err=0,openerr=0;
	U8	   tError=0,maxErr=20;
 	
 	start_pos = 0 ;
 	finished  = savelen ;
 	total     = leftLen ;
 	pSave     = buf     ;
 	recvBuf = pSave+finished;
 	
 	sprintf(range,"%d-%d",start_pos+finished,start_pos+total-1);
 	http_request.range  = range;
 	while(!tError)
	{
		if(fd == -1)
		{
			fd = protocol_open(url, NULL, &http_request);
			if(fd == -1)
        	{
        		openerr++;
				if(openerr > 5)
				{
				//hyUsbPrintf("thread socket fail id == %d\r\n",threadId);
					ret = -6 ;
					tError = 1;
				}
				continue;
			}
			openerr = 0;
			FD_ZERO(&fs);
	   		FD_SET(fd, &fs);
		}
		
		recvLen = GET_RECVBUF_LEN;
        while ((recvLen=protocol_recv_t(fd,fs,to,recvBuf, recvLen)) > 0)
        {
            finished += recvLen;
            gs32PercentRecv = finished;
            recvLen  = GET_RECVBUF_LEN;
           	recvBuf  = pSave+finished;
            if(finished >= total && !tError)
                break;
        }
        protocol_http_close(fd);
        fd = -1;
        if (finished == total)      //��ȷ����
        {
        	ret = finished ;
            break;
        }
        else if (finished < total)  //�ϵ�����
        {
        	err++;
        	if(err >= maxErr)
        	{
        		ret = -4 ;
        		tError = 1;
        		break;
        	}
        	sprintf(range,"%d-%d",start_pos+finished,start_pos+total-1);
        }
        else//�������
        {
        	ret = -5 ;
        	break;
        }
	}
	
	*downlen = finished ;
}

HTTP_GET_END:
	hyUsbPrintf("revc out %d \r\n",ret);
	hyIE_clearDns();
	hyIE_destroy();
	//unlock(&gu32LockIE);
	
	return ret;
}
/*-------------------------------------------------------------------------
* ����: file_load_filecheck
* ˵��: ����Ƿ����δ������ɵ��ļ� �д�����
* ����: 
* ����: �Ѿ����ص��ļ���С
* -------------------------------------------------------------------------*/
U32 file_load_filecheck(U8 *buf,U32 totallen,U8 *path,U8 *retVer)
{

	U16 crc ;
	int fp , size,totalsize;
	U8  ver[32] , *curVer;
	
	
	// �Ƿ���δ��������ļ�
	fp = AbstractFileSystem_Open(path,AFS_READ);
	if(fp < 0) return 0 ;
	
	// ����δ������ɵ��ļ�,�ļ��Ƿ��ƻ���
	size = AbstractFileSystem_FileSize(fp);
	if(size >= 0x200000)
	{
		AbstractFileSystem_DeleteFile(path,AFS_SINGLE_DELETE_FDM);
		return 0 ;
	}
	
	size -=  38 ;
	
	AbstractFileSystem_Read(fp,ver,32);
	AbstractFileSystem_Read(fp,(char *)&crc,sizeof(U16));
	AbstractFileSystem_Read(fp,(char *)&totalsize,sizeof(int));
	AbstractFileSystem_Read(fp,buf,size);
	AbstractFileSystem_Close(fp);
	
	if(totallen != totalsize || totallen < size)
	{
		AbstractFileSystem_DeleteFile(path,AFS_SINGLE_DELETE_FDM);
		return 0 ;
	}
	if(0 != memcmp(retVer,ver,32))
	{
		AbstractFileSystem_DeleteFile(path,AFS_SINGLE_DELETE_FDM);
		return 0 ;
	}
	if(crc != glbVariable_CalculateCRC((U8 *)buf,size))
	{
		AbstractFileSystem_DeleteFile(path,AFS_SINGLE_DELETE_FDM);
		return 0 ;
	}
	
	return size ;
}
/*-------------------------------------------------------------------------
* ����: file_parse_file
* ˵��: �ļ�����
* ����: flag : 1 : ��0����  0: ��������
* ����: 
* -------------------------------------------------------------------------*/
int file_parse_files(U8 flag,U8 *pdata,U32 datalen,U8 type,U8 *getver)
{
	int ret = HY_ERROR;
	hyUsbPrintf("��ʼ����type = %d\r\n",type);
	switch(type)
	{
		case 0:
			ret = file_parse_site(flag,pdata,datalen,getver);//����
			break;
		case 1:
			ret = file_parse_employee(flag,pdata,datalen,getver);//Ա��
			break;
		case 2:
			ret = file_parse_problem(flag,pdata,datalen,getver);//����
			break;
		case 3:
			ret = file_parse_leave(flag,pdata,datalen,getver);//����
			break;
		case 4:
			ret = file_parse_class(flag,pdata,datalen,getver);//���䷽ʽ
			break;
		case 5:
			ret = file_parse_destaddr(flag,pdata,datalen,getver);//Ŀ�ĵ�
			break;
		case 6:
			ret = file_parse_flight(flag,pdata,datalen,getver);//������Ϣ
			break;
		case 7:
			ret = file_parse_car(flag,pdata,datalen,getver);//������Ϣ
			break;
		case 8:
			//ret = file_parse_area(flag,pdata,datalen,getver);//������������
			ret = Gznd_DownLoad_DelAndSave(pdata,datalen,getver);//������������
			break;
		default:
			break;
	}
	return ret;
}
/*-------------------------------------------------------------------------
* ����: file_loadfile
* ˵��: ��������ض���
* ����: flag :0 :�������� 1����0����
* ����: 
* -------------------------------------------------------------------------*/
U8 file_load_file(U8 flag,U8 *url , U8 *ver ,U8 type)
{
	struct protocol_http_request tRequest;
	U8 *pBuf ,*pOut , *pfilepath ;
	int inlen ,outlen ,filesize,ret,downlen,savelen;
	U8 range[30];
	
	pBuf = (U8 *)hsaSdram_UpgradeBuf();
	
	if(1 == Net_ConnectNetWork(NULL))
	{
		//1 .��ȡ���ص��ļ���С
		memset(&tRequest, 0, sizeof(struct protocol_http_request));
	
		tRequest.method         = HTTP_GET_METHOD;
	    tRequest.user_agent     = NULL;
	    tRequest.content_length = 0;
	    tRequest.content_data   = NULL;
	    tRequest.referer        = NULL;
	    tRequest.content_type   = NULL;
	    tRequest.range          = NULL;
	    tRequest.connection		= 0;
		tRequest.gzip			= 0;
		hyUsbPrintf("url = %s\r\n",url);
		filesize = http_send_request(url, 0, &tRequest);
		hyUsbPrintf("Ҫ���ص����ݵĴ�С:%d\r\n",filesize);
		//filesize = http_get_content_length(url);
		if(filesize <= 0)return 2;
		if(filesize >= hsaSdram_UpgradeSize()/2)return 4;
		
		//2 .�жϺ�������
		if(0 == type) pfilepath = LOAD_COMPANY_BAK;
		else if(1 == type) pfilepath = LOAD_EMPLOYEE_BAK;
		else if(2 == type) pfilepath = LOAD_PROBLEM_BAK;
		else if(3 == type) pfilepath = LOAD_LEAVE_BAK;
		else if(4 == type) pfilepath = LOAD_CLASS_BAK;
		else if(5 == type) pfilepath = LOAD_DESTADDR_BAK;
		else if(6 == type) pfilepath = LOAD_FLIGHT_BAK;
		else if(7 == type) pfilepath = LOAD_CAR_BAK;
		
		//3 .���������Ƿ��Ƕϵ�����(�� ԭ���Ƿ����δ������ɵ��ļ�)
		savelen = file_load_filecheck(pBuf,filesize,pfilepath,ver);
		
		//4 .���ض���
		downlen = 0;
		
		inlen  = http_get(url,pBuf,hsaSdram_UpgradeSize()/2,savelen,(U32 *)&downlen);
		if(inlen <= 0)
		{	//ʧ�ܺ�ı���,���ڶϵ�����
			if(downlen > 0 && downlen <= filesize)
			{
				file_load_savefail(ver,pBuf,downlen,filesize,pfilepath);
			}
			return 2;
		}
		
		//5 .���سɹ�����ļ�����gzip��ѹ
		pOut = (U8 *)hsaSdramReusableMemoryPool()+0x100000;
		memset(pOut,0,hsaSdram_UpgradeSize()/2); //һ��Ҫ��
		hyUsbPrintf("inlen = %d\r\n",inlen);
		
		ret = Gzip_Uncompress(pBuf,inlen,pOut,&outlen,1);
		hyUsbPrintf("��ѹ����ֵ��ret = %d\r\n",ret);
		if(ret < 0)
		{
			//��ѹʧ�ܺ�ɾ���ļ�(���ص����ݳ�����)
			file_load_delsave(pfilepath);
			return 2 ;
		}
		
		//6 .�����ļ�������������������ݱ������ļ���
		//hyUsbPrintf("pOut = %s\r\n",pOut);
		file_parse_files(flag,pOut,outlen,type,ver);
		
		//7 .ɾ�����ڵĶϵ������ļ�
		file_load_delsave(pfilepath);
	}
	else
	{
		return 3 ;
	}
	
	return 0 ;
}
/*--------------------------------------------------
* ����:DownLoad_Company_Action
* ����:���ع�˾�б�Ľ���ִ�к���
* ����:
* ����: 0:�ɹ�   ����ʧ��,����ʧ��ԭ����Ե���OpCom_GetLastErrCode�õ�
----------------------------------------------------*/
int DownLoadWholeFile(void *p)
{
	struct protocol_http_request tRequest;
	tCARTOONPARA *pUp;
	U8		*pSend,flag;
	U8		*pS, *pE, ver[32],temUrl[100],url[100],allUrl[256];
	int		bufLen, recvLen, total, len;
	int		ret = 1, err,type;
	int		start,end,step,getCnt=0,tempCnt;
	
	pUp = (tCARTOONPARA *)p;
	
	pSend = pUp->pdata;
	bufLen= pUp->datalen;
	
	if(Net_ConnectNetWork() != 1)
	{
	hyUsbPrintf("sendRequest ����ʧ��\r\n");
		OpCom_SetLastErrCode(ERR_CONNECTFAIL);//��������ʧ��
		return 3;
	}
		
	pUp    = (tCARTOONPARA *)p;
	pSend  = pUp->pdata;
	bufLen = pUp->datalen;
	type = *pSend;

	
    sprintf(pSend,"<req op=\"%s\"><h><user>%s</user><stat>%s</stat><dev>%s</dev><flag>pda</flag></h><data><o><ver>%s</ver><pda>W818C</pda></o></data></req>", 
	        op_key[type],Login_GetUserId(),glbVar_GetCompanyId(),glbVar_GetMachineId(),LOAD_VER_NULL);
	
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	tRequest.method         = HTTP_POST_METHOD;
    tRequest.user_agent     = "hyco";
    tRequest.content_length = strlen(pSend);
    tRequest.content_data   = pSend;
    tRequest.referer        = NULL;
    tRequest.content_type   = "text/xml; charset=utf-8";
    tRequest.range          = NULL;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;
	hyUsbPrintf("login send = %s \r\n", pSend);

	recvLen = http_send_request(gtHyc.url, bufLen, &tRequest);
	
	hyUsbPrintf("recvLen = %d,login recv = %s \r\n",recvLen, pSend);
	
	if(recvLen > 0)
	{
		// 1 .�������ص�����
		pS = strstr(pSend,"true");
		if (NULL == pS)
		{
			pS = strstr(pSend,"false") ;
			if (NULL == pS) return 2 ;

			pE = strstr(pS,"<err>");
			if (NULL == pE) return 2 ;
			pS = strstr(pE,"</err>");
			if (NULL == pS) return 2 ;
			err = *(pE+5) ;
		}
		else
		{
			//1.11 .��ȡver
			pS = strstr(pSend,"<ver>") ;
			if (NULL == pS) return 2 ;
			pS += 5 ;
			pE = strstr(pS,"</ver>");
			if (NULL == pE) return 2 ;

			len = pE - pS ;
			if (len >= 30) return 2 ;
			
			memset(ver,0,32);
			memcpy(ver,pS,len);
			
			//1.12 .�Ƚϰ汾
			if(0 == memcmp(ver,LOAD_VER_NULL,strlen(LOAD_VER_NULL)))
			{
				flag = 1 ;
			}
			else
			{
				flag = 0 ;
			}

			//1.13 .��ȡurl
			pS = strstr(pE,"<url>") ;
			if (NULL == pS) return 2 ;
			pS += 5 ;
			pE = strstr(pS,"</url>");
			if (NULL == pE) return 2 ;

			len = pE - pS ;
			if (len >= 98) return 2 ;
			
			memset(temUrl,0,100);
			memcpy(temUrl,pS,len);
			
			memset(allUrl,0,256);
			//http://116.255.224.131:8089/K8Interface/webService.do
			pS = strstr(gtHyc.url+strlen("http://"),"/");
			if(pS != NULL)
			{
				memset(url,0,100);
				memcpy(url,gtHyc.url,pS-gtHyc.url);
				sprintf(allUrl,"%s%s",url,temUrl);
			}
			hyUsbPrintf("Url:%s\r\n",allUrl);
			//1.14 .��������
			if(0 != file_load_file(flag,allUrl,ver,type))
			{
				return 2; 
			}
		}
	}
	else
	{
		return 2 ;
	}
	
	return ret;
}