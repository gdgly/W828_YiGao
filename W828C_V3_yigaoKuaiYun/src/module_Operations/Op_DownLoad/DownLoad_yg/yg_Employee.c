#include "hyTypes.h"
#include "grap_api.h"
#include "base.h"
#include "http.h"
#include "glbVariable.h"
#include "Common_Dlg.h"
#include "Common_Widget.h"
#include "Common_AppKernal.h"
#include "AbstractFileSystem.h"
#include "Net_Evt.h"
#include "Op_Common.h"
#include "Op_DownLoad.h"
#include "Op_Struct.h"
#include "Op_RequestInfo.h"



char	ga8EmployeeVer[EMPLOYEE_VER_LEN];
U32		gu32EmployeeBufSize;
U32 	gu32EmployeeTotal;//���ص�Ա���ܸ���

tEMPLOYEE	*gptEmployee;
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*****************************************************
* ������DownLoad_ParseEmployee
* ���ܣ����������Ϸ��ص�Ա����Ϣ���ݲ�����ļ�
* ������pData ���緵�ص����� utf-8����
* ���أ�>0 �ɹ�  -1 ʧ��
******************************************************/
int DownLoad_ParseEmployee(U8 *pData, int buflen)
{
	int ret = 0,len,total,off;
	int subLen,size;
	int fp;
	U16 crc;
	U8  *pStart,*pTmp;
	
	off   = 0;
	total = 0;
	memset(&gptEmployee[total],0,sizeof(tEMPLOYEE));

	pStart = strstr(pData,"<userID>");

	while(pStart != NULL)
	{
		//�û����
		pStart += strlen("<userID>");
		pTmp = strstr(pStart,"</userID>");
		len = pTmp-pStart;
		if(len >= EMPLOYEE_ID_LEN) len = EMPLOYEE_ID_LEN-1;
		if(len > 0)
			memcpy(gptEmployee[total].userid, pStart, len);
		
		//�û�����
		pStart = strstr(pTmp,"<Password>");
		pStart += strlen("<Password>");
		pTmp = strstr(pStart,"</Password>");
		len = pTmp-pStart;
		if(len >= EMPLOYEE_PW_LEN) len = EMPLOYEE_PW_LEN-1;
		if(len > 0)
			memcpy(gptEmployee[total].userpw, pStart, len);
		
		//�û���
		pStart = strstr(pTmp,"<userName>");
		pStart += strlen("<userName>");
		pTmp = strstr(pStart,"</userName>");
		len = pTmp-pStart;
		if(len >= EMPLOYEE_NAME_LEN) len = EMPLOYEE_NAME_LEN-1;
		if(len > 0)
			memcpy(gptEmployee[total].username, pStart, len);
		
		//��������
		pStart = strstr(pTmp,"<belongSite>");
		pStart += strlen("<belongSite>");
		pTmp = strstr(pStart,"</belongSite>");
		len = pTmp-pStart;
		if(len >= COMPANY_ID_LEN) len = COMPANY_ID_LEN-1;
		if(len > 0)
			memcpy(gptEmployee[total].company, pStart, len);
					
		pStart = strstr(pTmp,"<userID>");
		
		//strcpy(gptEmployee[total].company, glbVar_GetCompanyId());
	hyUsbPrintf("%s  %s  %s  %s \r\n", gptEmployee[total].company,  gptEmployee[total].userid, gptEmployee[total].username, gptEmployee[total].userpw);
		total++;
		if(total >= (gu32EmployeeBufSize/sizeof(tEMPLOYEE))) break;
		
		memset(&gptEmployee[total],0,sizeof(tEMPLOYEE));
	}
hyUsbPrintf("employee total = %d \r\n", total);	
	
	return total;
}

int DownLoad_DownEmployee(void *p)
{
	struct protocol_http_request tRequest;
	tCARTOONPARA 	*pUp;
	int				ret = 1,getCnt;
	int				bufLen,recvLen;
	U8  			*pSend,*pGbk;
	U8				*pSub,name[30];
	
	if(Net_ConnectNetWork(NULL) != 1)
	{
		OpCom_SetLastErrCode(ERR_CONNECTFAIL);//��������ʧ��
		return 3;
	}

	pUp    = (tCARTOONPARA *)p;
	pSend  = pUp->pdata;
	bufLen = pUp->datalen;
	
	recvLen = NetReq_FillHead_yida(pSend, ga8EmployeeVer, "getUserBySite");
	strcat(&pSend[recvLen], "</senddata>");
	
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	tRequest.method         = HTTP_POST_METHOD;
    tRequest.user_agent     = "hyco";
    tRequest.content_length = strlen(pSend);
    tRequest.content_data   = pSend;
    tRequest.referer        = NULL;
    tRequest.content_type   = "text/xml; charset=GBK";
    tRequest.range          = NULL;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;
hyUsbPrintf("employee send url = %s   %s \r\n", gtHyc.url, pSend);	
	recvLen = http_send_request(gtHyc.url, bufLen, &tRequest);
hyUsbPrintf("employee recv = %d \r\n", recvLen);
	if(recvLen > 0)
	{
		pGbk = (U8 *)hsaSdram_DecodeBuf();
		recvLen = Net_DecodeData(pSend,recvLen,pGbk);
		if(recvLen > 0)
		{
			memcpy(pSend,pGbk,recvLen);
			pSend[recvLen] = 0;
			pSend[recvLen+1] = 0;
			
			pSub = strstr(pSend,"<result>0");
			if(pSub == NULL)
			{//ʧ��
				OpCom_SetLastErrCode(ERR_OTHER+(pSub[8]-'0'));//��������
				
				return 20+(pSub[8]-'0');
			}

			//��������
			getCnt = DownLoad_ParseEmployee(pSend, bufLen);
			if(-1 == getCnt)
			{
				OpCom_SetLastErrCode(ERR_DATAERROR);
				return 11;
			}
			
			if(getCnt > 0)
			{
				DownLoad_Employee_WriteFile(gptEmployee, getCnt, ga8EmployeeVer);
			}
			
			Employee_GetNameById(Login_GetUserId(), name, 30);
			Login_SetUserName(name);
			
			OpCom_SetLastErrCode(ERR_SUCCESS);
			return 1;
		}
		else
		{
			OpCom_SetLastErrCode(ERR_SENDFAIL);
			return 2;
		}
	}
	else
	{
		OpCom_SetLastErrCode(ERR_SENDFAIL);
		ret = 2;
	}
	
	return ret;
}

int DownLoadWholeFile(void *p);
/*--------------------------------------------------
* ����:DownLoad_Employee_Action
* ����:����Ա���б�Ľ���ִ�к���
* ����:
* ����: 0:�ɹ�   ����ʧ��,����ʧ��ԭ����Ե���OpCom_GetLastErrCode�õ�
----------------------------------------------------*/
int DownLoad_Employee_Action(tGRAPDESKBOX *pDesk)
{
	int				ret = -1;
	U8				*pSend;
	eDLG_STAT		kDlgState;
	tCARTOONPARA	tUp;
	int				err,len,lastValue;
	
	lastValue = BackLight_CloseScreenProtect();//�ر�����
		
	pSend = (U8 *)hsaSdram_UpgradeBuf();
	*pSend = 1;
	tUp.pdata   = pSend;
	tUp.datalen = hsaSdram_UpgradeSize();
	Com_SetPercentTotal(0);
	Com_SetPercentRecv(0);				
	cartoon_app_entry(DownLoadWholeFile,&tUp);
	AppCtrl_CreatPro(KERNAL_APP_AN, 0);
	kDlgState = Com_CtnDlgDeskbox(pDesk,2,"���ڸ�������,���Ժ�...",NULL,NULL,Com_PercentLabTimerISR,50,DLG_ALLWAYS_SHOW);
	AppCtrl_DeletePro(KERNAL_APP_AN);
	
	Net_DelEvt(NET_FOREGROUND_EVT);
	BackLight_SetScreenProtect(lastValue);//������
	
	if(DLG_CONFIRM_EN == kDlgState)
	{
		err = kernal_app_get_stat();
		if(err == 1)//�ɹ�
		{hyUsbPrintf("---------------------------------exit seccsee\r\n");
			ret = 0;
		}
	}
	else
	{
		OpCom_SetLastErrCode(ERR_CONNECTTIMEOUT);
	}
	
	return ret;
}
#if 0
/*========================================UI=====================================*/
/*----------------------------------------------------------
* ����:Op_Employee_Init
* ����:�û���Ϣ��ʼ��
* ����:
* ����:
------------------------------------------------------------*/
int Op_Employee_Init()
{
	int fp,size;
	U16 crc;
	
	gu32EmployeeTotal = 0;
	strcpy(ga8EmployeeVer,"0");
	gptEmployee = (tEMPLOYEE *)hsaSdram_EmployeBuf();
	gu32EmployeeBufSize = hsaSdram_EmployeSize();
	
	fp = AbstractFileSystem_Open(EMPLOYEE_FILE_PATH, AFS_READ);
	if (fp >= 0)
	{
		size = AbstractFileSystem_FileSize(fp);
		
		if(size <= 34 || size >= gu32EmployeeBufSize)
		{
			AbstractFileSystem_Close(fp);
			AbstractFileSystem_DeleteFile(EMPLOYEE_FILE_PATH,AFS_SINGLE_DELETE_FDM);
		}
		else
		{
			AbstractFileSystem_Seek(fp,0,AFS_SEEK_OFFSET_FROM_START);
			AbstractFileSystem_Read(fp,(char *)ga8EmployeeVer,EMPLOYEE_VER_LEN);
			AbstractFileSystem_Read(fp,(char *)&crc,sizeof(U16));
			size -= (EMPLOYEE_VER_LEN+2);
			AbstractFileSystem_Read(fp,(char *)gptEmployee,size);
			AbstractFileSystem_Close(fp);
			
			if(crc != glbVariable_CalculateCRC((U8 *)gptEmployee,size))
			{//�ļ�����
				AbstractFileSystem_DeleteFile(EMPLOYEE_FILE_PATH,AFS_SINGLE_DELETE_FDM);
			}
			else
			{
				gu32EmployeeTotal = size/sizeof(tEMPLOYEE);
			}
		}
	}
	
	return 0;
}
#else
U8	gaUserVer[32];
int Op_Employee_Init()
{
	int ret = 0;
	int fp,size;
	U16 crc;
	
	memset(ga8EmployeeVer,0,32);
	gu32EmployeeTotal = 0;
	gptEmployee = (tEMPLOYEE *)hsaSdram_EmployeBuf();
	fp = AbstractFileSystem_Open(EMPLOYEE_FILE_PATH,AFS_READ);	
	if(fp >= 0)
	{hyUsbPrintf("open ok\r\n");
		size = AbstractFileSystem_FileSize(fp);
		hyUsbPrintf("size = %d\r\n",size);
		if(size <= 34 || size >= hsaSdram_EmployeSize())
		{
			AbstractFileSystem_Close(fp);
			return -1;
		}
		
		AbstractFileSystem_Seek(fp,0,AFS_SEEK_OFFSET_FROM_START);
		AbstractFileSystem_Read(fp,(char *)ga8EmployeeVer,32);
		size -= 34;
		AbstractFileSystem_Read(fp,(char *)gptEmployee,size);
		AbstractFileSystem_Read(fp,(char *)&crc,sizeof(U16));
		AbstractFileSystem_Close(fp);
		
		gu32EmployeeTotal = size/sizeof(tEMPLOYEE);
		hyUsbPrintf("gu32EmployeeTotal = %d\r\n",gu32EmployeeTotal);
		if(crc != glbVariable_CalculateCRC((U8 *)gptEmployee,size))
		{//�ļ�����
			gu32EmployeeTotal = 0;
			memset(ga8EmployeeVer,0,32);
			strcpy(ga8EmployeeVer,LOAD_VER_NULL);
		}
		
	}
	else
	{hyUsbPrintf("open fail\r\n");
		strcpy(ga8EmployeeVer,LOAD_VER_NULL);
	}
	
	return ret;
}
#endif
int DownLoad_Employee_WriteFile(tEMPLOYEE *pEmployee, int cnt, U8 *ver)
{
	int fp;
	int size;
	U16 crc;
	
	if(cnt <= 0)
	{
		return -1;
	}
	
	gu32EmployeeTotal = 0;
	fp = AbstractFileSystem_Open(EMPLOYEE_FILE_PATH, AFS_READ);
	if(fp >= 0)//�ļ�˵�Ѿ�������ɾ��
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(EMPLOYEE_FILE_PATH,AFS_SINGLE_DELETE_FDM);
	}
	fp = AbstractFileSystem_Create(EMPLOYEE_FILE_PATH);
	if(fp >= 0)
	{
		//��д�汾��
		AbstractFileSystem_Write(fp, ga8EmployeeVer, EMPLOYEE_VER_LEN);
		
		//дcrc
		size = cnt*sizeof(tEMPLOYEE);
		crc = glbVariable_CalculateCRC((U8 *)pEmployee,size);
		AbstractFileSystem_Write(fp, (char *)&crc, sizeof(U16));
		
		//д����
		AbstractFileSystem_Write(fp, (char *)pEmployee, size);
		AbstractFileSystem_Close(fp);
		
		gu32EmployeeTotal = cnt;
	}
	
	return 0;
}

U32 Op_Employee_Total()
{
	return gu32EmployeeTotal;
}



//����û������뼰������˾
//1:��ȷ   0:����
int Op_DownLoad_CheckIdPw(U8 *pCode, U8 *pId, U8 *pPw)
{
	int i,ret = 0;
	
	for(i = 0; i < gu32EmployeeTotal; i++)
	{
		if(strcmp(pId, gptEmployee[i].userid) == 0 && 
		   strcmp(pPw, gptEmployee[i].userpw) == 0)
		{
			ret = 1;
			break;
		}
	}
	
	return ret;
}

/*--------------------------------------
*������Employee_GetIdNameByIdx
*���ܣ�ͨ��������ȡԱ��ID��name
*������pID:(out)��˾���   pName:(out)����    nameBufLen:pName  buf�ĳ���
       ���pIdΪNULL  �򲻻�ȡid  ͬ��pNameΪNULLҲ����ȡname
*���أ�1:�ɹ�    0:ʧ��
*--------------------------------------*/
int Employee_GetIdNameByIdx(int idx, U8 *pId, int idBufLen, U8 *pName, int nameBufLen)
{
	int i,ret = 0;
	
	if(idx >= 0 && idx < gu32EmployeeTotal)
	{
		if(pId != NULL)
		{
			if(strlen(gptEmployee[idx].userid) >= idBufLen)
			{
				memcpy(pId, gptEmployee[idx].userid, idBufLen-1);
			}
			else
			{
				strcpy(pId, gptEmployee[idx].userid);
			}
		}
		
		if(pName != NULL)
		{
			if(strlen(gptEmployee[idx].username) >= nameBufLen)
			{
				memcpy(pName, gptEmployee[idx].username, nameBufLen-1);
			}
			else
			{
				strcpy(pName, gptEmployee[idx].username);
			}
		}
		ret = 1;
	}
	
	return ret;
}

int Employee_GetNameById(U8 *pId, U8 *pName, int nameBufLen)
{
	int i,ret = 0;
	if(strlen(pId) == 0)
	{
		return 0;
	}
	for(i = 0; i < gu32EmployeeTotal; i++)
	{
	//hyUsbPrintf("i = %d  pId  = %s   userid  = %s ",i, pId, gptEmployee[i].userid);
		if(strcmp(pId, gptEmployee[i].userid) == 0)
		{
			if(strlen(gptEmployee[i].username) >= nameBufLen)
			{
				memcpy(pName, gptEmployee[i].username, nameBufLen-1);
			}
			else
			{
				strcpy(pName, gptEmployee[i].username);
			}
			ret = 1;
			break;
		}
	}
	return ret;
}
U8 *Employee_GetNameById2(U8 *pId)
{
	int i;
	if(strlen(pId) == 0)
	{
		return NULL;
	}
	for(i = 0; i < gu32EmployeeTotal; i++)
	{
		if(strcmp(pId, gptEmployee[i].userid) == 0)
		{
			return gptEmployee[i].username;
		}
	}
	return NULL;
}

//�����û�id��ȡ�������������
int Employee_GetCompanyById(U8 *pId, U8 *pCompany, int BufLen)
{
	int i,ret = 0;
	
	if(strlen(pId) == 0)
	{
		return 0;
	}
	for(i = 0; i < gu32EmployeeTotal; i++)
	{
		if(strcmp(pId, gptEmployee[i].userid) == 0)
		{
			if(strlen(gptEmployee[i].company) >= BufLen)
			{
				memcpy(pCompany, gptEmployee[i].company, BufLen-1);
			}
			else
			{
				strcpy(pCompany, gptEmployee[i].company);
			}
			
			hyUsbPrintf("company = %s \r\n", pCompany);
			ret = 1;
			break;
		}
	}
	
	return ret;
}



/**************************************************************************************************
									Ա����Ϣ���������
**************************************************************************************************/
const tGrapViewAttr Op_DownLoadEmployee_ViewAttr[] =
{
	{COMM_DESKBOX_ID,	  0, 0,240,320,    0, 0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0, 0, 1,0,0, 0},
#ifdef W818C
	{BUTTON_ID_1,	   5,282,73,34,     5,282,78,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_2,	   161,282,73,34,  161,282,234,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
#else	
	{BUTTON_ID_2,	   5,282,73,34,     5,282,78,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_1,	   161,282,73,34,  161,282,234,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
#endif	
	{BUTTON_ID_10,	   5,  38,72,16,    0,0,0,0,   0,0,0,0,1,0,0,0},//���/����
	{EDIT_ID_1,	   	   78, 35,150,22,    78,35,230,57, 0,0,0xffff,0,1,1,1,0},
	
	{LISTER_ID_1,	   10,64,224,212,	10,64,234,276,	0,0,0,0,1,0,1,0},
	
	{BUTTON_ID_1,	   83,282,73,34,    83,282,156,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},
};

const tGrapListerPriv Op_DownLoadEmployee_ListPriv[]=
{
	{24,8,0xffff,0x00,	0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,10,4,26,0,50,50,1,0,0,1,1,0},
};

const tGrapButtonPriv Op_DownLoadEmployee_BtnPriv[]=
{
#ifdef W818C
	{"����",0xffff,0x0000,0,0,2,KEYEVENT_OK ,0,0},
	{"����",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
#else
	{"����",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
	{"����",0xffff,0x0000,0,0,2,KEYEVENT_OK ,0,0},
#endif	
	{"���/����",0xffff,0x0000,0,0,1,0,0,0},
	
	{"����",0xffff,0x0000,0,0,2,0 ,0,0},
};

const tGrapEditPriv Op_DownLoadEmployee_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,15,  1, EMPLOYEE_NAME_LEN-1,  0,0,0,0,0,1,  50,50,0,  0,1,1,1},//���/����
};


S32 Op_DownLoad_Employee_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	tGRAPDESKBOX 	*pDesk;
	tGRAPLIST       *pList;
	int				err;
	
	if (state != STATE_NORMAL) return SUCCESS ;
	
	pBtn  = (tGRAPBUTTON *)pView;
	pDesk = (tGRAPDESKBOX *)pBtn->pParent;
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://����
		if(DLG_CONFIRM_EN != Com_SpcDlgDeskbox("ȷ������Ա���б�?",0,pDesk,3,NULL,NULL,800))
		{
			break;
		}
		
		if(-1 == Net_ChangeEvt(pDesk, NET_DOWNDATA_EVT))
		{
			break;
		}
		err = DownLoad_Employee_Action(pDesk);
		
		gpu16SearchInfo[0] = 0xffff;
		if(0 == err)
		{
			pList = (tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
			if (gu32EmployeeTotal <= 0)
				pList->lst_privattr.totalItem = 1;
			else
				pList->lst_privattr.totalItem = gu32EmployeeTotal;
			
			Com_SpcDlgDeskbox("�������!",0,pDesk,1,NULL,NULL,100);
		}
		else
		{
			OpCom_ErrorRemind(pDesk);
		}
		break;
	case BUTTON_ID_2://����
		pDesk->view_attr.reserve = 0xffffffff;
		return RETURN_QUIT;
		break;
	case BUTTON_ID_3://ȷ��
		pList = (tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
		/*
		if (gu32EmployeeTotal <= 0)
		{
			pDesk->view_attr.reserve = 0xffffffff;
		}
		else
		{
			if(gpu16SearchInfo[0] == 0xffff)//û�н�������
			{
				pDesk->view_attr.reserve = pList->lst_privattr.focusItem;
			}
			else
			{
				pDesk->view_attr.reserve = gpu16SearchInfo[pList->lst_privattr.focusItem];
			}
		}
		*/
		pList->enter(pList,pList->lst_privattr.focusItem);
		return RETURN_QUIT;
		break;
	default:
		break;
	}
	
	return RETURN_REDRAW;
}

S32 Op_DownLoad_Employee_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	if (gu32EmployeeTotal <= 0)
	{
		strcpy(str, "�����Ա����Ϣ");
	}
	else
	{
		if(gpu16SearchInfo[0] == 0xffff)//û�н�������
		{
			//sprintf(str, "%s|%s|%s", gptEmployee[i].userid,gptEmployee[i].username,gptEmployee[i].company);
			sprintf(str, "%s|%s|%s", gptEmployee[i].userid,gptEmployee[i].username,gtHyc.companyName);
		}
		else
		{
			//sprintf(str, "%s|%s|%s", gptEmployee[gpu16SearchInfo[i]].userid, gptEmployee[gpu16SearchInfo[i]].username,gptEmployee[gpu16SearchInfo[i]].company);
			sprintf(str, "%s|%s|%s", gptEmployee[gpu16SearchInfo[i]].userid, gptEmployee[gpu16SearchInfo[i]].username,gtHyc.companyName);
		}
	}

	return SUCCESS;
}

U32 Op_DownLoadEmployee_EditKeyEnd(void *pView, U8 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit;
	tGRAPLIST       *pList;
	U8	*pId, *pNa;
	int i,ret = 0,total = 0;
	
	pEdit = (tGRAPEDIT *)pView;
	
	if(pEdit->view_attr.id == EDIT_ID_1 && gu32EmployeeTotal > 0)//
	{
		pDesk= (tGRAPDESKBOX *)pEdit->pParent;
		pList = (tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);//ID��һ��ȷ�Ϻ�
		
		if(strlen(pEdit->edit_privattr.digt) > 0)
		{
		//hyUsbPrintf("digt = %s   %d \r\n", pEdit->edit_privattr.digt,pList->lst_privattr.totalItem);
			if(type == 0 && gpu16SearchInfo[0] != 0xffff)//����
			{
				for(i = 0; i < pList->lst_privattr.totalItem; i++)
				{
				//hyUsbPrintf("id = %s  name = %s \r\n", gptEmployee[gpu16SearchInfo[i]].userid, gptEmployee[gpu16SearchInfo[i]].username);
					pId = hy_strstr(gptEmployee[gpu16SearchInfo[i]].userid, pEdit->edit_privattr.digt);
					if(pId == NULL)//ͨ��IDû��ƥ�䵽
					{
					//hyUsbPrintf("id cmp fail \r\n");
						pNa = hy_strstr(gptEmployee[gpu16SearchInfo[i]].username, pEdit->edit_privattr.digt);
						if(pNa != NULL)//ƥ�䵽����
						{
						//hyUsbPrintf("name cmp ok \r\n");
							ret = 1;
						}
						else
						{
							ret = 0;
						}
					}
					else
					{
						ret = 1;
					}
					
					if(ret == 1)
					{
					//hyUsbPrintf("total = %d   i = %d \r\n", total, gpu16SearchInfo[i]);
						gpu16SearchInfo[total++] = gpu16SearchInfo[i];
					}
				}
			}
			else
			{
				for(i = 0; i < gu32EmployeeTotal; i++)
				{
					pId = hy_strstr(gptEmployee[i].userid, pEdit->edit_privattr.digt);
					if(pId == NULL)//ͨ��IDû��ƥ�䵽
					{
						pNa = hy_strstr(gptEmployee[i].username, pEdit->edit_privattr.digt);
						if(pNa != NULL)//ƥ�䵽����
						{
							ret = 1;
						}
						else
						{
							ret = 0;
						}
					}
					else
					{
						ret = 1;
					}
					
					if(ret == 1)
					{
						gpu16SearchInfo[total++] = i;
					}
				}
			}
			pList->lst_privattr.totalItem = total;
		}
		else
		{
			gpu16SearchInfo[0] = 0xffff;//��־û�н�������
			if (gu32EmployeeTotal <= 0)
				pList->lst_privattr.totalItem = 1;
			else
				pList->lst_privattr.totalItem = gu32EmployeeTotal;
		}
		
		if(pList) pList->draw(pList, 0, 0);
	}
	
	return SUCCESS ;
}
S32 Op_DownLoad_Employee_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX 	*pDesk ;
	
	if(i < pLister->lst_privattr.totalItem)
	{
		pDesk = (tGRAPDESKBOX *)pLister->pParent ;
		if (gu32EmployeeTotal <= 0)
		{
			pDesk->view_attr.reserve = 0xffffffff;
		}
		else
		{
			if(gpu16SearchInfo[0] == 0xffff)//û�н�������
			{
				pDesk->view_attr.reserve = i;
			}
			else
			{
				pDesk->view_attr.reserve = gpu16SearchInfo[i];
			}
		}
	}
	
	return RETURN_QUIT;
	
}

/*---------------------------------------------------------
*����: Op_DownLoad_Employee_Desk
*����: Ա����Ϣ���ؽ���
*����: ifSearch:�Ƿ��ѯ  1:��   ��ѯʱ������·���ť
*����: ��
*---------------------------------------------------------*/
int Op_DownLoad_Employee_Desk(void *pDeskFather, U8 ifSearch)
{
	tGRAPDESKBOX    *pDesk;
	tGRAPBUTTON     *pBttn;
	tGRAPLIST       *pList;
	tGRAPEDIT 		*pEdit;
	U8              index = 0;
	int 			i,sel;
	
	gpu16SearchInfo = (U16 *)hsaSdram_UpgradeBuf();
	gpu16SearchInfo[0] = 0xffff;
	
	pDesk = Grap_CreateDeskbox(&Op_DownLoadEmployee_ViewAttr[index++], "Ա���б�");
	if (pDesk == NULL)  return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_ALL;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	for(i = 0; i < 3; i++)
	{
		pBttn = Grap_InsertButton(pDesk,&Op_DownLoadEmployee_ViewAttr[index++],&Op_DownLoadEmployee_BtnPriv[i]);
		if (pBttn == NULL)  return ;
		if(i < 2)
		{
			pBttn->pressEnter = Op_DownLoad_Employee_BtnEnter;
		}
	#ifdef W818C
		if(i == 0 && ifSearch == 1)
	#else	
		if(i == 1 && ifSearch == 1)
	#endif
		{
			pBttn->view_attr.id = BUTTON_ID_3;
			pBttn->btn_privattr.pName = "ȷ��";
		}
	}
	
	pEdit = Grap_InsertEdit(pDesk, &Op_DownLoadEmployee_ViewAttr[index++], &Op_DownLoadEmployee_EditPriv[0]);
	pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
	pEdit->editKeyEnd = Op_DownLoadEmployee_EditKeyEnd;
	
	pList = Grap_InsertLister(pDesk,&Op_DownLoadEmployee_ViewAttr[index++],&Op_DownLoadEmployee_ListPriv[0]);
	if (pList == NULL) return;
	pList->getListString = Op_DownLoad_Employee_ListGetStr;
	if(ifSearch == 1)
		pList->enter = Op_DownLoad_Employee_ListEnter;
	if (gu32EmployeeTotal <= 0)
		pList->lst_privattr.totalItem = 1;
	else
		pList->lst_privattr.totalItem = gu32EmployeeTotal;
	
	if(ifSearch == 1)
	{
		pBttn = Grap_InsertButton(pDesk,&Op_DownLoadEmployee_ViewAttr[index++],&Op_DownLoadEmployee_BtnPriv[3]);
		if (pBttn == NULL)  return ;
		pBttn->pressEnter = Op_DownLoad_Employee_BtnEnter;
	}
	
	t9_insert(pDesk,NULL);
	edit_creat_panel(pDesk, 6, NULL,NULL);
	edit_auto_change_input_mode(pDesk,0);
	edit_change_input_mode(pDesk,0,(pDesk->inputMode==4));
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	sel = pDesk->view_attr.reserve;
	Grap_DeskboxDestroy(pDesk);
	
	return sel;
}
/*-------------------------------------------------------------------------
* ����: file_parse_site
* ˵��: �����б�Ľ����ͱ���
* ����: flag : 1 : ��0����  0: ��������
* ����: 
* -------------------------------------------------------------------------*/
int file_parse_employee(U8 flag,U8 *pdata,U32 datalen,U8 *ver)
{
#if 1
	tEMPLOYEE *pEmployee ;
	U8   *pTempTap, *pTab, *pEnter, *pStart;
	int  fp ,len  ;
	U32  total,size ;
	U16  crc;
	//hyUsbPrintf("pdata = %s\r\n",pdata);
	//1. ��ʱbuff������ͳ�ʼ��
	pEmployee = (tEMPLOYEE *)(((U32)hsaSdram_UpgradeBuf() + 4) & 0xfffffffc);
	
	memset((U8 *)pEmployee,0,LOAD_EMPLOYEE_MAX * sizeof(tEMPLOYEE));
	
	total = 0 ;
	/*
	if(flag == 0)
	{
		memcpy((U8 *)pCompany,(U8 *)gptCompany,gu32CompanyTotal * sizeof(tCOMPANY));
		total = gu32CompanyTotal ;
	}
	*/
	

	//2. ����
	pStart = pdata;
	//pTempTap = strchr(pStart, '	');
	while(pTempTap)
	{
		pEnter = strchr(pStart, '\r');
		if (pEnter)
		{
			if(*(pEnter-1) == '1')//ɾ����ʶΪ1ʱ����
			{
				pStart = pEnter+2;
				continue;
			}
			pTempTap = strchr(pStart, '	');
		}
		else
		{
			break;
		}
		if(total>=LOAD_EMPLOYEE_MAX) break;//���������Ʋ�Ҫ����
		
		len = pTempTap - pStart;
		if (len > EMPLOYEE_ID_LEN-2)
			len = EMPLOYEE_ID_LEN-2;
		memcpy(pEmployee[total].userid, pStart, len);//�û�����
		hyUsbPrintf("pEmployee[%d].userid = %s\r\n",total,pEmployee[total].userid);
		pStart = pTempTap+1;
		pTempTap = strchr(pStart, '	');
		if(pTempTap)
		{
			len = pTempTap - pStart;
			if (len > EMPLOYEE_ROLE_LEN-2)
				len = EMPLOYEE_ROLE_LEN-2;
			memcpy(pEmployee[total].userrole, pStart, len);//�û���ɫ
		}
		else
		{
			continue;
		}
		hyUsbPrintf("pEmployee[%d].userrole = %s\r\n",total,pEmployee[total].userrole);
		pStart = pTempTap+1;
		pTempTap = strchr(pStart, '	');
		if(pTempTap)
		{
			/*		
			char *temp;
			temp = pStart;
			for(;*temp!='	';temp++)
			{
				if(*temp == ' ')
				{
					pTempTap = temp;
					break;
				}
			}
			*/
			len = pTempTap - pStart;
			if (len > EMPLOYEE_NAME_LEN-2)
				len = EMPLOYEE_NAME_LEN-2;
			memcpy(pEmployee[total].username, pStart, len);//�û�����
		}
		else
		{
			continue;
		}
		hyUsbPrintf("pEmployee[%d].username = %s\r\n",total,pEmployee[total].username);
		pStart = pTempTap+1;
		pTempTap = strchr(pStart, '	');
		if(pTempTap)
		{
			/*
			len = pTempTap - pStart;
			if (len > COMPANY_ID_LEN-2)
				len = COMPANY_ID_LEN-2;
			memcpy(pEmployee[total].company, pStart, len);//������˾
			*/
			memcpy(pEmployee[total].company,gtHyc.companyId,10);
		}
		else
		{
			continue;
		}
		hyUsbPrintf("pEmployee[%d].company = %s\r\n",total,pEmployee[total].company);
		pStart = pTempTap+1;
		pTempTap = strchr(pStart, '	');
		if(pTempTap)
		{
			len = pTempTap - pStart;
			if (len > EMPLOYEE_PW_LEN-2)
				len = EMPLOYEE_PW_LEN-2;
			memcpy(pEmployee[total].userpw, pStart, len);//Ա������
		}
		else
		{
			continue;
		}
		//hyUsbPrintf("pEmployee[%d].userpw = %s\r\n",total,pEmployee[total].userpw);
		total++;
		
		//pEnter = strchr(pTempTap, '\r');
		//if (NULL == pEnter)break;
		
		pStart = pEnter+2;
		//pTempTap = strchr(pStart, '	');
	}
	
	//3. д�ļ�
	if(1)
	{
		if(total <= 0)
		{
			return HY_ERROR;
		}
		
		fp = AbstractFileSystem_Open(EMPLOYEE_FILE_PATH,AFS_READ);
		if(fp >= 0)
		{
			AbstractFileSystem_Close(fp);
			AbstractFileSystem_DeleteFile(EMPLOYEE_FILE_PATH,AFS_SINGLE_DELETE_FDM);
			
			size = total*sizeof(tEMPLOYEE);
			crc = glbVariable_CalculateCRC((U8 *)pEmployee,size);
			fp = AbstractFileSystem_Create(EMPLOYEE_FILE_PATH);
			if(fp >= 0)
			{hyUsbPrintf("size1 = %d\r\n",size);
				AbstractFileSystem_Write(fp,ver,32);
				AbstractFileSystem_Write(fp,(char *)pEmployee,size);
				AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
				AbstractFileSystem_Close(fp);
			}
		}
		else
		{
			size = total*sizeof(tEMPLOYEE);
			crc = glbVariable_CalculateCRC((U8 *)pEmployee,size);
			fp = AbstractFileSystem_Create(EMPLOYEE_FILE_PATH);
			if(fp >= 0)
			{
				hyUsbPrintf("size = %d\r\n",size);
				AbstractFileSystem_Write(fp,ver,32);
				AbstractFileSystem_Write(fp,(char *)pEmployee,size);
				AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
				AbstractFileSystem_Close(fp);
			}
			else
			{
				hyUsbPrintf("�����ļ�ʧ��\r\n");
			}
		}
		memset(gaUserVer,0,32);
		strcpy(gaUserVer,ver);
		gu32EmployeeTotal = total;
		memcpy((char *)gptEmployee,(char *)pEmployee,size);
	}
	
	return HY_OK ;
	
#endif
}
