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



char	gaDestAddrVer[32];
U32		gu32DestAddrBufSize;
U32 	gu32DestAddrTotal;//���ص��ܸ���

tDESTADDR	*gptDestAddr;
int DownLoadWholeFile(void *p);
/*--------------------------------------------------
* ����:DownLoad_DestAddr_Action
* ����:����Ŀ�ĵ��б�Ľ���ִ�к���
* ����:
* ����: 0:�ɹ�   ����ʧ��,����ʧ��ԭ����Ե���OpCom_GetLastErrCode�õ�
----------------------------------------------------*/
int DownLoad_DestAddr_Action(tGRAPDESKBOX *pDesk)
{
	int				ret = -1;
	U8				*pSend;
	eDLG_STAT		kDlgState;
	tCARTOONPARA	tUp;
	int				err,len,lastValue;
	
	lastValue = BackLight_CloseScreenProtect();//�ر�����
		
	pSend = (U8 *)hsaSdram_UpgradeBuf();
	*pSend = 5;
	tUp.pdata   = pSend;
	tUp.datalen = hsaSdram_UpgradeSize();
	Com_SetPercentTotal(0);
	Com_SetPercentRecv(0);				
	cartoon_app_entry(DownLoadWholeFile,&tUp);
	AppCtrl_CreatPro(KERNAL_APP_AN, 0);
	kDlgState = Com_CtnDlgDeskbox(pDesk,2,"���ڸ�������,���Ժ�...",NULL,NULL,Com_PercentLabTimerISR,50,DLG_ALLWAYS_SHOW);//
	AppCtrl_DeletePro(KERNAL_APP_AN);
	
	Net_DelEvt(NET_FOREGROUND_EVT);
	BackLight_SetScreenProtect(lastValue);//������
	
	if(DLG_CONFIRM_EN == kDlgState)
	{
		err = kernal_app_get_stat();
		if(err == 1)//�ɹ�
		{
			ret = 0;
		}
	}
	else
	{
		OpCom_SetLastErrCode(ERR_CONNECTTIMEOUT);
	}
	
	return ret;
}

/*****************************************************
* ������Op_DestAddr_Init
* ���ܣ����ļ��ж�ȡ��˾����
* ������none
* ���أ�0 �ɹ�  -1 ʧ��
******************************************************/


int Op_DestAddr_Init()
{
	int ret = 0;
	int fp,size;
	U16 crc;
	
	memset(gaDestAddrVer,0,32);
	gu32DestAddrTotal = 0;
	gptDestAddr = (tDESTADDR *)hsaSdram_RegionBuf();
	fp = AbstractFileSystem_Open(DESTADDR_FILE_PATH,AFS_READ);	
	if(fp >= 0)
	{hyUsbPrintf("open ok\r\n");
		size = AbstractFileSystem_FileSize(fp);
		
		if(size <= 34 || size >= hsaSdram_RegionSize())
		{
			AbstractFileSystem_Close(fp);
			return -1;
		}
	
		AbstractFileSystem_Seek(fp,0,AFS_SEEK_OFFSET_FROM_START);
		AbstractFileSystem_Read(fp,(char *)gaDestAddrVer,32);
		size -= 34;
		AbstractFileSystem_Read(fp,(char *)gptDestAddr,size);
		AbstractFileSystem_Read(fp,(char *)&crc,sizeof(U16));
		AbstractFileSystem_Close(fp);
		
		gu32DestAddrTotal = size/sizeof(tDESTADDR);
		hyUsbPrintf("gu32DestAddrTotal = %d\r\n",gu32DestAddrTotal);
		if(crc != glbVariable_CalculateCRC((U8 *)gptDestAddr,size))
		{//�ļ�����
			gu32DestAddrTotal = 0;
			memset(gaDestAddrVer,0,32);
			strcpy(gaDestAddrVer,LOAD_VER_NULL);
		}
	}
	else
	{hyUsbPrintf("open fail\r\n");
		strcpy(gaDestAddrVer,LOAD_VER_NULL);
	}
	
	return ret;
}

U32 DownLoad_DestAddr_GetTotal()
{
	return gu32DestAddrTotal;
}
/*--------------------------------------
*������DestAddr_GetIdNameByIdx
*���ܣ�ͨ��������ȡ��˾ID��name
*������pID:(out)Ŀ�ĵر��   pName:(out)����    nameBufLen:pName  buf�ĳ���
       ���pIdΪNULL  �򲻻�ȡid  ͬ��pNameΪNULLҲ����ȡname
*���أ�1:�ɹ�    0:ʧ��
*--------------------------------------*/
int DestAddr_GetIdNameByIdx(int idx, U8 *pId, int idBufLen, U8 *pName, int nameBufLen)
{
	int i,ret = 0;
	
	if(idx >= 0 && idx < gu32DestAddrTotal)
	{
		if(pId != NULL)
		{
			if(strlen(gptDestAddr[idx].id) >= idBufLen)
			{
				memcpy(pId, gptDestAddr[idx].id, idBufLen-1);
			}
			else
			{
				strcpy(pId, gptDestAddr[idx].id);
			}
		}
		
		if(pName != NULL)
		{
			if(strlen(gptDestAddr[idx].name) >= nameBufLen)
			{
				memcpy(pName, gptDestAddr[idx].name, nameBufLen-1);
			}
			else
			{
				strcpy(pName, gptDestAddr[idx].name);
			}
		}
		ret = 1;
	}
	
	return ret;
}
/**************************************************************************************************
									Ŀ�ĵ��б����������
**************************************************************************************************/
const tGrapViewAttr Op_DownLoadDestAddr_ViewAttr[] =
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

const tGrapListerPriv Op_DownLoadDestAddr_ListPriv[]=
{
	{24,8,0xffff,0x00,	0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,10,4,26,0,50,50,1,0,0,1,1,0},
};

const tGrapButtonPriv Op_DownLoadDestAddr_BtnPriv[]=
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

const tGrapEditPriv Op_DownLoadDestAddr_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,15,  1, COMPANY_NAME_LEN-1,  0,0,0,0,0,1,  50,50,0,  0,1,1,1},//���/����
};

S32 Op_DownLoad_DestAddr_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	tGRAPDESKBOX 	*pDesk;
	tGRAPLIST       *pList;
	U8				*pSend;
	eDLG_STAT		kDlgState;
	tCARTOONPARA	tUp;
	int				err,len,lastValue;
	
	if (state != STATE_NORMAL) return SUCCESS ;
	
	pBtn  = (tGRAPBUTTON *)pView;
	pDesk = (tGRAPDESKBOX *)pBtn->pParent;
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://����
		if(DLG_CONFIRM_EN != Com_SpcDlgDeskbox("ȷ����Ŀ�ĵ��б�?",0,pDesk,3,NULL,NULL,800))
		{
			break;
		}
		
		if(-1 == Net_ChangeEvt(pDesk, NET_DOWNDATA_EVT))
		{
			break;
		}
		lastValue = BackLight_CloseScreenProtect();//�ر�����
		
		pSend = (U8 *)hsaSdram_UpgradeBuf();
		*pSend = 5;
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
		
		gpu16SearchInfo[0] = 0xffff;
		if(DLG_CONFIRM_EN == kDlgState)
		{
			err = kernal_app_get_stat();
			if(err == 1)
			{
				pList = (tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
				if (gu32DestAddrTotal <= 0)
					pList->lst_privattr.totalItem = 1;
				else
					pList->lst_privattr.totalItem = gu32DestAddrTotal;
				
				Com_SpcDlgDeskbox("�������!",0,pDesk,1,NULL,NULL,100);
			}
			else if(err == 11)
			{
				Com_SpcDlgDeskbox("�������ش���!",0,pDesk,1,NULL,NULL,100);
			}
			else if(err == 2)
			{
				Com_SpcDlgDeskbox("����ͨѶ����!",6, pDesk, 2,NULL,NULL,100);
			}
			else if(err == 3)
			{
				Com_SpcDlgDeskbox("��������ʧ��,���Ժ�����!",6, pDesk, 2,NULL,NULL,100);
			}
			else
			{
				err -= 20;
				if(err < 1 || err > 6) err = 0;
				Com_SpcDlgDeskbox(Net_ErrorCode[err],6,pDesk,2,NULL,NULL,100);
			}
		}
		else
		{
			Com_SpcDlgDeskbox("����ͨѶ����,���Ժ�����!",6, pDesk, 2,NULL,NULL,100);
		}
		break;
	case BUTTON_ID_2://����
		pDesk->view_attr.reserve = 0xffffffff;
		return RETURN_QUIT;
		break;
	case BUTTON_ID_3://ȷ��
		pList = (tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
		/*
		if (gu32DestAddrTotal <= 0)
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

S32 Op_DownLoad_DestAddr_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	if (gu32DestAddrTotal <= 0)
	{
		strcpy(str, "�����Ŀ�ĵ���Ϣ");
	}
	else
	{
		if(gpu16SearchInfo[0] == 0xffff)//û�н�������
		{
			sprintf(str, "%s|%s", gptDestAddr[i].id, gptDestAddr[i].name);
		}
		else
		{
			sprintf(str, "%s|%s", gptDestAddr[gpu16SearchInfo[i]].id, gptDestAddr[gpu16SearchInfo[i]].name);
		}
	}
	
	return SUCCESS;
}

U32 Op_DownLoadDestAddr_EditKeyEnd(void *pView, U8 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit;
	tGRAPLIST       *pList;
	U8	*pId, *pNa;
	int i,ret = 0,total = 0;
	
	pEdit = (tGRAPEDIT *)pView;
	
	if(pEdit->view_attr.id == EDIT_ID_1 && gu32DestAddrTotal > 0)//
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
				//hyUsbPrintf("id = %s  name = %s \r\n", gptCompany[gpu16SearchInfo[i]].companyid, gptCompany[gpu16SearchInfo[i]].company);
					pId = hy_strstr(gptDestAddr[gpu16SearchInfo[i]].id, pEdit->edit_privattr.digt);
					if(pId == NULL)//ͨ��IDû��ƥ�䵽
					{
					//hyUsbPrintf("id cmp fail \r\n");
						pNa = hy_strstr(gptDestAddr[gpu16SearchInfo[i]].name, pEdit->edit_privattr.digt);
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
				for(i = 0; i < gu32DestAddrTotal; i++)
				{
					pId = hy_strstr(gptDestAddr[i].id, pEdit->edit_privattr.digt);
					if(pId == NULL)//ͨ��IDû��ƥ�䵽
					{
						pNa = hy_strstr(gptDestAddr[i].name, pEdit->edit_privattr.digt);
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
			pList->lst_privattr.topItem   = 0;
			pList->lst_privattr.focusItem = 0;
			pList->lst_privattr.totalItem = total;
		}
		else
		{
			pList->lst_privattr.topItem   = 0;
			pList->lst_privattr.focusItem = 0;
			gpu16SearchInfo[0] = 0xffff;//��־û�н�������
			if (gu32DestAddrTotal <= 0)
				pList->lst_privattr.totalItem = 1;
			else
				pList->lst_privattr.totalItem = gu32DestAddrTotal;
		}
		
		if(pList) pList->draw(pList, 0, 0);
	}
	
	return SUCCESS ;
}
S32 Op_DownLoad_DestAddr_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX 	*pDesk ;
	
	if(i < pLister->lst_privattr.totalItem)
	{
		pDesk = (tGRAPDESKBOX *)pLister->pParent ;
		if (gu32DestAddrTotal <= 0)
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
/*--------------------------------------
*������DestAddr_GetNameById
*���ܣ�ͨ��Ŀ�ĵ�id�õ���Ӧ������
*������pID:(in)Ŀ�ĵر��   pName:(out)����    nameBufLen:pName  buf�ĳ���
*���أ�1:�ɹ�    0:ʧ��
*--------------------------------------*/
int DestAddr_GetNameById(U8 *pId, U8 *pName, int nameBufLen)
{
	int i,ret = 0;
	
	if(strlen(pId) == 0)
	{
		return 0;
	}
	for(i = 0; i < gu32DestAddrTotal; i++)
	{
		if(strcmp(pId, gptDestAddr[i].id) == 0)
		{
			if(strlen(gptDestAddr[i].name) >= nameBufLen)
			{
				memcpy(pName, gptDestAddr[i].name, nameBufLen-1);
			}
			else
			{
				strcpy(pName, gptDestAddr[i].name);
			}
			ret = 1;
			break;
		}
	}
	
	return ret;
}
/*---------------------------------------------------------
*����: Op_DownLoad_Company_Desk
*����: Ա����Ϣ���ؽ���
*����: ifSearch:�Ƿ��ѯ  1:��   ��ѯʱ������·���ť
*����: 
*---------------------------------------------------------*/
int Op_DownLoad_DestAddr_Desk(void *pDeskFather, U8 ifSearch)
{
	tGRAPDESKBOX    *pDesk;
	tGRAPBUTTON     *pBttn;
	tGRAPLIST       *pList;
	tGRAPEDIT 		*pEdit;
	U8              index = 0;
	int 			i,sel;
	
	gpu16SearchInfo = (U16 *)hsaSdram_UpgradeBuf();
	gpu16SearchInfo[0] = 0xffff;
	
	pDesk = Grap_CreateDeskbox(&Op_DownLoadDestAddr_ViewAttr[index++], "Ŀ�ĵ�");
	if (pDesk == NULL)  return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_ALL;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	for(i = 0; i < 3; i++)
	{
		pBttn = Grap_InsertButton(pDesk,&Op_DownLoadDestAddr_ViewAttr[index++],&Op_DownLoadDestAddr_BtnPriv[i]);
		if (pBttn == NULL)  return ;
		if(i < 2)
		{
			pBttn->pressEnter = Op_DownLoad_DestAddr_BtnEnter;
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
	
	pEdit = Grap_InsertEdit(pDesk, &Op_DownLoadDestAddr_ViewAttr[index++], &Op_DownLoadDestAddr_EditPriv[0]);
	pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
	pEdit->editKeyEnd = Op_DownLoadDestAddr_EditKeyEnd;
	
	pList = Grap_InsertLister(pDesk,&Op_DownLoadDestAddr_ViewAttr[index++],&Op_DownLoadDestAddr_ListPriv[0]);
	if (pList == NULL) return;
	pList->getListString = Op_DownLoad_DestAddr_ListGetStr;
	if(ifSearch == 1)
		pList->enter = Op_DownLoad_DestAddr_ListEnter;
	if (gu32DestAddrTotal <= 0)
		pList->lst_privattr.totalItem = 1;
	else
		pList->lst_privattr.totalItem = gu32DestAddrTotal;
	
	if(ifSearch == 1)
	{
		pBttn = Grap_InsertButton(pDesk,&Op_DownLoadDestAddr_ViewAttr[index++],&Op_DownLoadDestAddr_BtnPriv[3]);
		if (pBttn == NULL)  return ;
		pBttn->pressEnter = Op_DownLoad_DestAddr_BtnEnter;
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
* ����: file_parse_destaddr
* ˵��: Ŀ�ĵ��б�Ľ����ͱ���
* ����: flag : 1 : ��0����  0: ��������
* ����: 
* -------------------------------------------------------------------------*/
int file_parse_destaddr(U8 flag,U8 *pdata,U32 datalen,U8 *ver)
{
#if 1

	tDESTADDR *pDestAddr ;
	U8  *pTempTap, *pTab1, *pEnter, *pStart;
	int  fp ,len1   ;
	U32  total,size ;
	U16  crc;
	
	//1. ��ʱbuff������ͳ�ʼ��
	pDestAddr = (tDESTADDR *)(((U32)hsaSdram_UpgradeBuf() + 4) & 0xfffffffc);
	
	memset((U8 *)pDestAddr,0,LOAD_DESTADDR_MAX * sizeof(tDESTADDR));
	
	total = 0 ;
	
	//ע��,Ŀǰ�ǳ���Ŀ�ĵ������Ķ�֧����������,Ŀ�ĵ���ȫ������
	//if(flag == 0)
	//{
	//	memcpy((U8 *)pDestAddr,(U8 *)gptDestAddr,gu32DestAddrTotal * sizeof(tDESTADDR));
	//	total = gu32DestAddrTotal ;
	//}
	
	pStart = pdata;
	pTempTap = strchr(pStart, '	');
	while(pTempTap)
	{
		if(total>=LOAD_DESTADDR_MAX) break;//���������Ʋ�Ҫ����
		
		pTab1 = pTempTap;
		pTempTap++;
		pEnter = strchr(pTempTap, '\r');
		if (pEnter)
		{
			len1 = pTab1 - pStart;
			if (len1 > COMPANY_ID_LEN-2)
				len1 = COMPANY_ID_LEN-2;
			memcpy(pDestAddr[total].id, pStart, len1);
			len1 = pEnter - pTempTap;
			if (len1 > COMPANY_NAME_LEN)
				len1 = COMPANY_NAME_LEN;
			memcpy(pDestAddr[total].name, pTempTap, len1);
			total++;
		}
		else
		{
			break;
		}
		//hyUsbPrintf("pDestAddr[%d].id = %s,pDestAddr[%d].name = %s\r\n",total-1,pDestAddr[total-1].id,total-1,pDestAddr[total-1].name);
		pStart = pEnter+2;
		pTempTap = strchr(pStart, '	');
	}
	
	//3. д�ļ�
	if(1)
	{
		if(total <= 0)
		{
			return HY_ERROR;
		}
		
		fp = AbstractFileSystem_Open(DESTADDR_FILE_PATH,AFS_READ);
		if(fp >= 0)
		{
			AbstractFileSystem_Close(fp);
			AbstractFileSystem_DeleteFile(DESTADDR_FILE_PATH,AFS_SINGLE_DELETE_FDM);
			
			size = total*sizeof(tDESTADDR);
			crc = glbVariable_CalculateCRC((U8 *)pDestAddr,size);
			fp = AbstractFileSystem_Create(DESTADDR_FILE_PATH);
			if(fp >= 0)
			{
				AbstractFileSystem_Write(fp,ver,32);
				AbstractFileSystem_Write(fp,(char *)pDestAddr,size);
				AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
				AbstractFileSystem_Close(fp);
			}
		}
		else
		{
			size = total*sizeof(tDESTADDR);
			crc = glbVariable_CalculateCRC((U8 *)pDestAddr,size);
			fp = AbstractFileSystem_Create(DESTADDR_FILE_PATH);
			if(fp >= 0)
			{
				AbstractFileSystem_Write(fp,ver,32);
				AbstractFileSystem_Write(fp,(char *)pDestAddr,size);
				AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
				AbstractFileSystem_Close(fp);
			}
		}
		
		memset(gaDestAddrVer,0,32);
		strcpy(gaDestAddrVer,ver);
		gu32DestAddrTotal = total;
		memcpy((char *)gptDestAddr,(char *)pDestAddr,size);
	}
	
	return HY_OK ;
	
	
#endif
}
