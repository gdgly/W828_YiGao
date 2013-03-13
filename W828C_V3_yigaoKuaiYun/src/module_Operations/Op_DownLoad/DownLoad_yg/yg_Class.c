/**********************************************************************
						���䷽ʽ����
**********************************************************************/


#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "AbstractFileSystem.h"
#include "Op_Common.h"
#include "Op_DownLoad.h"
#include "Op_Struct.h"
#include "Op_RequestInfo.h"



U32			gu32ClassTotal;
U32			gu32ClassBufSize;
tCLASS	*gptClass;
/*****************************************************
* ������Op_Class_Init
* ���ܣ����ļ��ж�ȡ���䷽ʽ����
* ������none
* ���أ�0 �ɹ�  -1 ʧ��
******************************************************/
 U8	gaClassVer[32];

int Op_Class_Init()
{
	int fp,size;
	U16 crc;
	
	memset(gaClassVer,0,32);
	gu32ClassTotal = 0;
	gptClass = (tCLASS *)hsaSdram_WeiVolBuf();
	fp = AbstractFileSystem_Open(CLASS_FILE_PATH,AFS_READ);	
	if(fp >= 0)
	{hyUsbPrintf("open ok\r\n");
		size = AbstractFileSystem_FileSize(fp);
		
		if(size <= 34 || size >= hsaSdram_WeiVolSize())
		{
			AbstractFileSystem_Close(fp);
			return -1;
		}
	
		AbstractFileSystem_Seek(fp,0,AFS_SEEK_OFFSET_FROM_START);
		AbstractFileSystem_Read(fp,(char *)gaClassVer,32);
		size -= 34;
		AbstractFileSystem_Read(fp,(char *)gptClass,size);
		AbstractFileSystem_Read(fp,(char *)&crc,sizeof(U16));
		AbstractFileSystem_Close(fp);
		
		gu32ClassTotal = size/sizeof(tCLASS);
		hyUsbPrintf("gu32ClassTotal = %d\r\n",gu32ClassTotal);
		if(crc != glbVariable_CalculateCRC((U8 *)gptClass,size))
		{//�ļ�����
			gu32ClassTotal = 0;
			memset(gaClassVer,0,32);
			strcpy(gaClassVer,LOAD_VER_NULL);
		}
	}
	else
	{hyUsbPrintf("open fail\r\n");
		strcpy(gaClassVer,LOAD_VER_NULL);
	}
	
	return 0;
}

//��ȡ���䷽ʽ����
int Op_DownLoad_GetClassCnt()
{
	return gu32ClassTotal;
}

//��ȡ���䷽ʽ����
U8 *Op_DownLoad_GetClassName(U8 index)
{
	return gptClass[index].name;
}


//��ȡ���䷽ʽ����
U8 *Op_DownLoad_GetClassCode(U8 index)
{
	return gptClass[index].id;
}
//ͨ��id��ȡname
U8 *Op_DownLoad_ByidGetClassName(U8 *id)
{
	int i;
	if(id==NULL||strlen(id) ==0)
		return NULL;
	for(i=0;i<gu32ClassTotal;i++)
	{
		if(strcmp(gptClass[i].id,id)==0)
			return gptClass[i].name;
	}
	return NULL;
}

/**************************************************************************************************
									���䷽ʽ�����б�
**************************************************************************************************/
const tGrapViewAttr Op_Class_ViewAttr[] =
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

const tGrapListerPriv Op_Class_ListPriv[]=
{
	{24,8,0xffff,0x00,	0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,10,4,26,0,50,50,1,0,0,1,1,0},
};

const tGrapButtonPriv Op_Class_BtnPriv[]=
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

const tGrapEditPriv Op_Class_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,15,  1, CLASS_NAME_LEN-1,  0,0,0,0,0,1,  50,50,0,  0,1,1,1},//���/����
};
int DownLoadWholeFile(void *p);
S32 Op_Class_ButtonEnter(void *pView, U16 state)
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
		if(DLG_CONFIRM_EN != Com_SpcDlgDeskbox("ȷ���������䷽ʽ�б�?",0,pDesk,3,NULL,NULL,800))
		{
			break;
		}
		
		if(-1 == Net_ChangeEvt(pDesk, NET_DOWNDATA_EVT))
		{
			break;
		}
		lastValue = BackLight_CloseScreenProtect();//�ر�����
		
		pSend = (U8 *)hsaSdram_UpgradeBuf();
		*pSend = 4;
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
				if (gu32ClassTotal <= 0)
					pList->lst_privattr.totalItem = 1;
				else
					pList->lst_privattr.totalItem = gu32ClassTotal;
				
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
	case BUTTON_ID_2://ESC
		pDesk->view_attr.reserve = 0xffffffff;
		return RETURN_QUIT;
		break;
	case BUTTON_ID_3://ȷ��
		pList = (tGRAPLIST *)Grap_GetViewByID(pDesk,LISTER_ID_1);
		if (gu32ClassTotal <= 0)
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
		return RETURN_QUIT;
		break;
	default:
		break;
	}
	
	return RETURN_REDRAW;
}


S32 Op_Class_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	if (gu32ClassTotal <= 0)
	{
		strcpy(str, "��������䷽ʽ�б�");
	}
	else
	{
		if(gpu16SearchInfo[0] == 0xffff)//û�н�������
		{
			sprintf(str,"%s|%s",gptClass[i].id,gptClass[i].name);
		}
		else
		{
			sprintf(str, "%s|%s", gptClass[gpu16SearchInfo[i]].id, gptClass[gpu16SearchInfo[i]].name);
		}
	}
	
	return SUCCESS;
}

U32 Op_Class_EditKeyEnd(void *pView, U8 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit;
	tGRAPLIST       *pList;
	U8	*pId, *pNa;
	int i,ret = 0,total = 0;
	
	pEdit = (tGRAPEDIT *)pView;
	
	if(pEdit->view_attr.id == EDIT_ID_1 && gu32ClassTotal > 0)//
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
				//hyUsbPrintf("id = %s  name = %s \r\n", gptClass[gpu16SearchInfo[i]].code, gptClass[gpu16SearchInfo[i]].name);
					pId = hy_strstr(gptClass[gpu16SearchInfo[i]].id, pEdit->edit_privattr.digt);
					if(pId == NULL)//ͨ��IDû��ƥ�䵽
					{
					//hyUsbPrintf("id cmp fail \r\n");
						pNa = hy_strstr(gptClass[gpu16SearchInfo[i]].name, pEdit->edit_privattr.digt);
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
				for(i = 0; i < gu32ClassTotal; i++)
				{
					pId = hy_strstr(gptClass[i].id, pEdit->edit_privattr.digt);
					if(pId == NULL)//ͨ��IDû��ƥ�䵽
					{
						pNa = hy_strstr(gptClass[i].name, pEdit->edit_privattr.digt);
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
			if (gu32ClassTotal <= 0)
				pList->lst_privattr.totalItem = 1;
			else
				pList->lst_privattr.totalItem = gu32ClassTotal;
		}
		
		if(pList) pList->draw(pList, 0, 0);
	}
	
	return SUCCESS ;
}

/*---------------------------------------------------------
*����: Op_DownLoad_Class_Desk
*����: ���䷽ʽ���ؽ���
*����: ifSearch:�Ƿ��ѯ  1:��   ��ѯʱ������·���ť
*����: ��
*---------------------------------------------------------*/
int Op_DownLoad_Class_Desk(void *pDeskFather, U8 ifSearch)
{
	tGRAPDESKBOX    *pDesk ;
	tGRAPBUTTON     *pBttn ;
	tGRAPLIST       *pList ;
	tGRAPEDIT 		*pEdit;
	U8              index=0;
	int 			i,sel;
	
	gpu16SearchInfo = (U16 *)hsaSdram_UpgradeBuf();
	gpu16SearchInfo[0] = 0xffff;
	
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&Op_Class_ViewAttr[index++], "���䷽ʽ");
	if (pDesk == NULL)  return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_ALL;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	for (i=0; i<3; i++)
	{
		pBttn = Grap_InsertButton(pDesk,(tGrapViewAttr*)&Op_Class_ViewAttr[index++],(tGrapButtonPriv*)&Op_Class_BtnPriv[i]);
		if (pBttn == NULL)  return ;
		if(i < 2)
		{
			pBttn->pressEnter = Op_Class_ButtonEnter;
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
	
	pEdit = Grap_InsertEdit(pDesk, &Op_Class_ViewAttr[index++], &Op_Class_EditPriv[0]);
	pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
	pEdit->editKeyEnd = Op_Class_EditKeyEnd;
	
	pList = Grap_InsertLister(pDesk,(tGrapViewAttr*)(&Op_Class_ViewAttr[index++]),(tGrapListerPriv*)(&Op_Class_ListPriv[0]));
	if (pList == NULL) return;
	pList->getListString = Op_Class_ListGetStr;
	if (gu32ClassTotal <= 0)
		pList->lst_privattr.totalItem = 1;
	else
		pList->lst_privattr.totalItem = gu32ClassTotal;
	
	if(ifSearch == 1)
	{
		pBttn = Grap_InsertButton(pDesk,&Op_Class_ViewAttr[index++],&Op_Class_BtnPriv[3]);
		if (pBttn == NULL)  return ;
		pBttn->pressEnter = Op_Class_ButtonEnter;
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
* ����: file_parse_class
* ˵��: ���䷽ʽ�Ľ����ͱ���
* ����: flag : 1 : ��0����  0: ��������
* ����: 
* -------------------------------------------------------------------------*/
int file_parse_class(U8 flag,U8 *pdata,U32 datalen,U8 *ver)
{
#if 1

	tCLASS *pClass ;
	U8  *pTempTap, *pTab1, *pEnter, *pStart;
	int  fp ,len1,i,j;
	U32  total,size ;
	U16  crc;
	
	//1. ��ʱbuff������ͳ�ʼ��
	pClass = (tCLASS *)(((U32)hsaSdram_UpgradeBuf() + 4) & 0xfffffffc);
	
	memset((U8 *)pClass,0,LOAD_CLASS_MAX * sizeof(tCLASS));
	
	total = 0 ;
	/*
	if(flag == 0)
	{
		memcpy((U8 *)pClass,(U8 *)gptClass,gu32ClassTotal * sizeof(tCLSS));
		total = gu32ClassTotal ;
	}
	*/
	pStart = pdata;
	pTempTap = strchr(pStart, '	');
	while(pTempTap)
	{
		if(total>=LOAD_CLASS_MAX) break;//���������Ʋ�Ҫ����
		
		pTab1 = pTempTap;
		pTempTap++;
		pEnter = strchr(pTempTap, '	');
		if (pEnter)
		{
			len1 = pTab1 - pStart;
			if (len1 > CLASS_ID_LEN-2)
				len1 = CLASS_ID_LEN-2;
			memcpy(pClass[total].id, pStart, len1);
			len1 = pEnter - pTempTap;
			if (len1 > CLASS_NAME_LEN-2)
				len1 = CLASS_NAME_LEN-2;
			memcpy(pClass[total].name, pTempTap, len1);
			total++;
		}
		else
		{
			break;
		}
		//hyUsbPrintf("pClass[%d].id = %s,pClass[%d].name = %s\r\n",total-1,pClass[total-1].id,total-1,pClass[total-1].name);
		#if 1
		for(i=0;i<total-1;i++)
		{
			for(j=i;j<total-1;j++)
			{
				if(memcmp(pClass+i,pClass+j+1,sizeof(tCLASS)) == 0)//���ڸ����ܴ�����䷽ʽ�кܶ��ظ��ģ���������Թ���
				{
					if(j+2<total)
					memcpy(pClass+j+1,pClass+j+2,sizeof(tCLASS)*(total-j-2));
					total--;
				}
			}
		}
	#endif
		pEnter = strchr(pTempTap, '\r');
		if (NULL == pEnter)break;
		
		pStart = pEnter+2;
		pTempTap = strchr(pStart, '	');
	}
	
	//3. д�ļ�
	{
		if(total <= 0)
		{
			return HY_ERROR;
		}
		
		fp = AbstractFileSystem_Open(CLASS_FILE_PATH,AFS_READ);
		if(fp >= 0)
		{
			AbstractFileSystem_Close(fp);
			AbstractFileSystem_DeleteFile(CLASS_FILE_PATH,AFS_SINGLE_DELETE_FDM);
			
			size = total*sizeof(tCLASS);
			crc = glbVariable_CalculateCRC((U8 *)pClass,size);
			fp = AbstractFileSystem_Create(CLASS_FILE_PATH);
			if(fp >= 0)
			{
				AbstractFileSystem_Write(fp,ver,32);
				AbstractFileSystem_Write(fp,(char *)pClass,size);
				AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
				AbstractFileSystem_Close(fp);
			}
		}
		else
		{
			size = total*sizeof(tCLASS);
			crc = glbVariable_CalculateCRC((U8 *)pClass,size);
			fp = AbstractFileSystem_Create(CLASS_FILE_PATH);
			if(fp >= 0)
			{
				AbstractFileSystem_Write(fp,ver,32);
				AbstractFileSystem_Write(fp,(char *)pClass,size);
				AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
				AbstractFileSystem_Close(fp);
			}
		}
		hyUsbPrintf("total = %d\r\n",total);
		memset(gaClassVer,0,32);
		strcpy(gaClassVer,ver);
		gu32ClassTotal = total;
		memcpy((char *)gptClass,(char *)pClass,size);
	}
	
	return HY_OK ;
	
	
#endif
}