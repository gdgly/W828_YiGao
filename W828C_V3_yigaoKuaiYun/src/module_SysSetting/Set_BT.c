#include "SysSetting.h"
#include "bt_server_dev.h"
#include "btApi.h"


U8	gu8BtConnIdx;
U8    *bt_protocolSet[] = {/*"�ر�", */(U8 *)"��ϴ��",(U8 *)"���С��",(U8 *)"ҫ����"};
extern S32 ComWidget_LabTimerISR(void *pView) ;
extern int bt_connect_enable(int *btindex) ;
extern int bt_connect_checkpwd(U8 *pwd) ;
extern S32 ComWidget_LabTimerTimeOutISR(void *pView);

/*const U8 gu8BmpSelect[] = 
{	
	0xFF,0xFF,0xF0,0x80,0x00,0x10,0x80,0x00,0x10,0x80,0x00,0x30,0x80,0x00,0x70,0x80,
	0x00,0xF0,0x80,0x01,0xD0,0x80,0x03,0x90,0xC0,0x07,0x10,0xE0,0x0E,0x10,0xF0,0x1C,
	0x10,0xB8,0x38,0x10,0x9C,0x70,0x10,0x8E,0xE0,0x10,0x87,0xC0,0x10,0x83,0x80,0x10,
	0x81,0x00,0x10,0x80,0x00,0x10,0x80,0x00,0x10,0xFF,0xFF,0xF0,
};
const U8 gu8BmpUnselect[] = 
{
	0xFF,0xFF,0xF0,0x80,0x00,0x10,0x80,0x00,0x10,0x80,0x00,0x10,0x80,0x00,0x10,0x80,
	0x00,0x10,0x80,0x00,0x10,0x80,0x00,0x10,0x80,0x00,0x10,0x80,0x00,0x10,0x80,0x00,
	0x10,0x80,0x00,0x10,0x80,0x00,0x10,0x80,0x00,0x10,0x80,0x00,0x10,0x80,0x00,0x10,
	0x80,0x00,0x10,0x80,0x00,0x10,0x80,0x00,0x10,0xFF,0xFF,0xF0,
};*/

const tGrapViewAttr gtBTsetVAtrr[]=
{
	{COMM_DESKBOX_ID,   0,0,240,320,   		0,  0,  0,0,  		TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//����
#ifdef W818C
	{BUTTON_ID_1,	  	5,282,73,34,   		5,282,78,316,   	YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//ȷ��
	{BUTTON_ID_2,		83,282,73, 34, 		83,282,156,316,   	YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_3,		161,282,73,34, 		161,282,234,316,   	YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
#else
	{BUTTON_ID_3,	  	5,282,73,34,   		5,282,78,316,   	YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_2,		83,282,73, 34, 		83,282,156,316,   	YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_1,		161,282,73,34, 		161,282,234,316,   	YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//ȷ��
#endif	
	{BUTTON_ID_4,		0, 38, 48, 20,  	0,0,0,0,  			0,0,  0,0,1,0,0,0}, //MAC��ַ
	{BUTTON_ID_5,	  	0, 64, 48, 20,  	0,0,0,0,  			0,0,  0,0,1,0,0,0}, //����
	{BUTTON_ID_6,	  	0, 90, 80, 20,  	0,0,0,0,  			0,0,  0,0,1,0,0,0}, //ѡ����ӳ�
	{BUTTON_ID_7,	  	0,114, 96, 16,  	0,0,0,0,  			0,0,  0,0,1,0,0,0}, //�����豸

	{EDIT_ID_1,    		70, 38,160, 20,  	70, 38,230,58,  	0,0,0xffff,0,1,1,1,0}, //�Է�MAC
	{EDIT_ID_2,    		70, 64,160, 20,  	70, 64,230,84,  	0,0,0xffff,0,1,0,1,0}, //�Է�����
	{COMBOBOX_ID_1,   	84, 90,145, 20,   	84,90,229,110 , 	0,0,0,0,1,0,1,0}, //ѡ����ӳ�
	{LISTER_ID_1,     	5,134,220, 144,   	5,134,225,278,      0,0,0,0,1,0,1,0}, //�����豸�б�
};

const tGrapButtonPriv gtBTsetBPri[]=
{
#ifdef W818C
	{(U8 *)"ȷ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,1,  NULL},
	{(U8 *)"����F1", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_F1,1,  NULL},
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,1, NULL},
#else
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC,1, NULL},
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_F1,1,  NULL},
	{(U8 *)"ȷ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,1,  NULL},
#endif	
	{(U8 *)"�Է�MAC:", 0xffff,0xffff,FONTSIZE8X16,0,2,0,1,  NULL},
	{(U8 *)"�Է�����:", 0xffff,0xffff,FONTSIZE8X16,0,2,0,1,  NULL},
	{(U8 *)"ѡ����ӳ�", 0xffff,0xffff,FONTSIZE8X16,0,2,0,1,  NULL},
	{(U8 *)"�����豸�б�", 0xffff,0xffff,FONTSIZE8X16,0,2,0,1,  NULL},	
};


const tGrapListerPriv gtBTsetLPri[]=
{   
	{24, 6, 0xffff, COLOR_YELLOW,	0, 2, 0, TY_UNITY_BG1_EN, 0, 4, 4, 26, 0, 30, 30, 1, 0, 0, 0,0},
};


const tGrapEditPriv gtBTsetEPri[] = 
{
	{NULL,0x0,0,0,2,2,19,  1,20,0,0,2,0,0,1,  50,50,0,  0,1,2,1},
	{NULL,0x0,0,0,2,2,19,  1,20,0,0,2,0,0,1,  50,50,0,  0,1,1,1},
};

const tComboboxPriv gtBTsetCPri[]=
{
	{4, 6, 0, 0, 0, 0, 1, 145, 12, 145, 14, 0, 50, 50,  FONTSIZE8X16, 0, 0, 0, 0xFFE0, 0x0, 0xf800, 0xffff, 0, 0, 0, 20,  0, 0, YD_HD_S_EN,YD_HD_X_EN,YD_HD_Z_EN},
};


static S32 SetBT_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON  *pBtn;
	tGRAPDESKBOX *pDesk;
	tGRAPLIST	 *pList;
	tGRAPEDIT 	 *pEdit;
	tGRAPCOMBOBOX	*pCombo;
	U32 tickCnt;
	int kDlgState ,err;
	int foucIndex ,ret;
	int retpwd ;
	U8 btname[42]; //�������40
	tBdAddress btaddr;
	
	if (state != STATE_NORMAL) return SUCCESS ;
	pBtn = (tGRAPBUTTON *)pView;
	pDesk = (tGRAPDESKBOX *)pBtn->pParent;
	pList = (tGRAPLIST *)Grap_GetViewByID(pDesk, LISTER_ID_1);
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1: //ȷ��		
		if(pList->lst_privattr.totalItem > 0)
		{
			if (hyco_getConnectState() == TRUE)
			{
				if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�Ƿ�Ͽ�����?",6, pDesk, 3, NULL, NULL, 3000))
				{
					bt_disconnect(pDesk);
				}
			}
			else if(hyco_getBtInitState() == TRUE)
			{
				foucIndex = pList->lst_privattr.focusItem; 
				cartoon_app_entry(bt_connect_enable,&foucIndex);
				AppCtrl_CreatPro(KERNAL_APP_AN, 0);
				kDlgState = Com_CtnDlgDeskbox(pDesk,2,"�������������豸...",NULL,NULL,ComWidget_LabTimerISR,50,82000);
				AppCtrl_DeletePro(KERNAL_APP_AN);
				if(DLG_CONFIRM_EN == kDlgState) 
				{
					err = kernal_app_get_stat();
					if(err != TRUE)
					{
						Com_SpcDlgDeskbox("����ʧ��!",6,pDesk,1,NULL,NULL,100);
					}
					else
					{
						if (hyco_getConnectState() == FALSE)
						{
							//��Ҫ��������
							pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_2);

							cartoon_app_entry(bt_connect_checkpwd,pEdit->edit_privattr.digt);
							AppCtrl_CreatPro(KERNAL_APP_AN, 0);
							kDlgState = Com_CtnDlgDeskbox(pDesk,2,"�������...",NULL,NULL,ComWidget_LabTimerISR,50,82000);
							AppCtrl_DeletePro(KERNAL_APP_AN);
							if(DLG_CONFIRM_EN == kDlgState) 
							{
								retpwd = kernal_app_get_stat();

								if (TRUE == retpwd)
								{
									//��Գɹ�  ��������豸��ַ������
									ret = hyco_getSearchList(foucIndex, &btaddr, &btname[0]);
									if(TRUE == ret)
									{
										BT_AddrToStr(&btaddr, btname, 0);
										if(strcmp(gtHyc.bt_addr2,btname) != 0 || strcmp(gtHyc.bt_pwd,pEdit->edit_privattr.digt) != 0)
										{
											strcpy(gtHyc.bt_addr2,btname);									
											strcpy(gtHyc.bt_pwd,pEdit->edit_privattr.digt);
											glbVariable_SaveParameter();
										}
									}

									Com_SpcDlgDeskbox("��Գɹ�!",0,pDesk,1,NULL,NULL,100);
								}
								else
								{
									Com_SpcDlgDeskbox("���ʧ��,��ȷ�������Ƿ���ȷ!",6,pDesk,1,NULL,NULL,200);
								}
							}
						}
						else
						{
							//����OK
							ret = hyco_getSearchList(foucIndex, &btaddr, &btname[0]);
							if(TRUE == ret)
							{
								BT_AddrToStr(&btaddr, btname, 0);
								if(strcmp(gtHyc.bt_addr2,btname) != 0)
								{
									strcpy(gtHyc.bt_addr2,btname);									
									glbVariable_SaveParameter();
								}
							}

							Com_SpcDlgDeskbox("��Գɹ�!",0,pDesk,1,NULL,NULL,100);
							return RETURN_REDRAW;
						}
					}
				}
			}
			else
			{
				Com_SpcDlgDeskbox("�����豸��û��׼����!",6,pDesk,1,NULL,NULL,100);
			}
		}
		else
		{
			pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_1);
			if(strlen(pEdit->edit_privattr.digt) > 0)//������MAC
			{
				strcpy(gtHyc.bt_addr2, pEdit->edit_privattr.digt);
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_2);
				strcpy(gtHyc.bt_pwd,pEdit->edit_privattr.digt);
				if(1 == bt_dev_connect(pDesk))
				{
					glbVariable_SaveParameter();
				}
			}
			else
			{
				Com_SpcDlgDeskbox("�������豸!",6,pDesk,1,NULL,NULL,100);
			}
		}
		break;
	case BUTTON_ID_2: //����
		if(hyco_getBtInitState() != TRUE)
		{
			Com_SpcDlgDeskbox("�����豸��û��׼����!",0,pDesk,1,NULL,NULL,100);
		}
		else
		{
			if (hyco_getConnectState() == TRUE)
			{
				bt_disconnect(pDesk);
			}
			
			//��ʾ�����б�һ��һ����ʾ��20�볬ʱ���м��ֹͣ
			hyco_startInquiry();
			Com_CtnDlgDeskbox(pDesk,2,"�������������豸...",NULL,NULL,ComWidget_LabTimerTimeOutISR,50,1200);
			hyco_cancelSearch();
			pList->lst_privattr.totalItem = hyco_getSearchListNum();
		}
		break;
	case BUTTON_ID_3: //����
		return RETURN_QUIT;
		break;
	default:
		break;
	}

	return RETURN_REDRAW;
}

S32 SetBT_ListerEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX *pDesk;
	tGRAPEDIT 	 *pEdit;
	tGRAPCOMBOBOX	*pCombo;
	U32 tickCnt;
	int kDlgState ,err;
	int ret;
	int retpwd ;
	U8 btname[42]; //�������40
	tBdAddress btaddr;
	
	pDesk = (tGRAPDESKBOX *)pLister->pParent;
	
	if (hyco_getConnectState() == TRUE)
	{
		if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�Ƿ�Ͽ�����?",6, pDesk, 3, NULL, NULL, 3000))
		{
			bt_disconnect(pDesk);
			gu8BtConnIdx = 0xff;
		}
	}
	else if(hyco_getBtInitState() == TRUE)
	{
		cartoon_app_entry(bt_connect_enable,&i);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		kDlgState = Com_CtnDlgDeskbox(pDesk,2,"�������������豸...",NULL,NULL,ComWidget_LabTimerISR,50,82000);
		AppCtrl_DeletePro(KERNAL_APP_AN);
		if(DLG_CONFIRM_EN == kDlgState) 
		{
			err = kernal_app_get_stat();
			if(err != TRUE)
			{
				Com_SpcDlgDeskbox("����ʧ��!",6,pDesk,1,NULL,NULL,100);
			}
			else
			{
				if (hyco_getConnectState() == FALSE)
				{
					//��Ҫ��������
					pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_2);

					cartoon_app_entry(bt_connect_checkpwd,pEdit->edit_privattr.digt);
					AppCtrl_CreatPro(KERNAL_APP_AN, 0);
					kDlgState = Com_CtnDlgDeskbox(pDesk,2,"�������...",NULL,NULL,ComWidget_LabTimerISR,50,82000);
					AppCtrl_DeletePro(KERNAL_APP_AN);
					if(DLG_CONFIRM_EN == kDlgState) 
					{
						retpwd = kernal_app_get_stat();

						if (TRUE == retpwd)
						{
							//��Գɹ�  ��������豸��ַ������
							ret = hyco_getSearchList(i, &btaddr, &btname[0]);
							if(TRUE == ret)
							{
								BT_AddrToStr(&btaddr, btname, 0);
								if(strcmp(gtHyc.bt_addr2,btname) != 0 || strcmp(gtHyc.bt_pwd,pEdit->edit_privattr.digt) != 0)
								{
									strcpy(gtHyc.bt_addr2,btname);									
									strcpy(gtHyc.bt_pwd,pEdit->edit_privattr.digt);
									glbVariable_SaveParameter();
								}
							}
							
							gu8BtConnIdx = i;
							Com_SpcDlgDeskbox("��Գɹ�!",0,pDesk,1,NULL,NULL,100);
						}
						else
						{
							Com_SpcDlgDeskbox("���ʧ��!",6,pDesk,1,NULL,NULL,100);
						}
					}
				}
				else
				{
					//����OK
					ret = hyco_getSearchList(i, &btaddr, &btname[0]);
					if(TRUE == ret)
					{
						BT_AddrToStr(&btaddr, btname, 0);
						if(strcmp(gtHyc.bt_addr2,btname) != 0)
						{
							strcpy(gtHyc.bt_addr2,btname);									
							glbVariable_SaveParameter();
						}
					}
					
					gu8BtConnIdx = i;
					Com_SpcDlgDeskbox("��Գɹ�!",0,pDesk,1,NULL,NULL,100);
					return RETURN_REDRAW;
				}
			}
		}
	}
	else
	{
		Com_SpcDlgDeskbox("�����豸��û��׼����!",6,pDesk,1,NULL,NULL,100);
	}
	
	return RETURN_REDRAW;
}


S32 SetBT_ListerDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPLIST *pLister;
	U16 i;
	tGrapViewAttr *pview_attr;
	tGrapListerPriv *plst_privattr;

	pLister = (tGRAPLIST *)pView;
	pview_attr = &(pLister->view_attr);
	plst_privattr = &(pLister->lst_privattr);
	
	Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1+pview_attr->viewWidth), 
		(U16)(pview_attr->viewY1+pview_attr->viewHeight));

	if(plst_privattr->totalItem > plst_privattr->lstNum)
	{//��Ҫ��ʾ������
		if(plst_privattr->topItem > plst_privattr->totalItem - plst_privattr->lstNum)
		{
			plst_privattr->topItem = plst_privattr->totalItem - plst_privattr->lstNum;
		}
	}
	
	if(plst_privattr->totalItem <= plst_privattr->lstNum && plst_privattr->topItem > 0)
	{
		plst_privattr->topItem = 0;
	}
	if(plst_privattr->focusItem >= plst_privattr->totalItem && plst_privattr->totalItem > 0)
	{
		plst_privattr->focusItem = plst_privattr->totalItem-1;
	}
	
	for (i = 0; i < plst_privattr->lstNum; i++)
	{
		if(plst_privattr->topItem + i >= plst_privattr->totalItem)
			break;
		pLister->updateList(pLister, (U16)(plst_privattr->topItem + i));
	}
	
	drvLcd_SetColor(COLOR_WHITE,COLOR_BLACK);
	drvLcd_Rectangle(pview_attr->viewY1-2, pview_attr->viewX1-2, pview_attr->viewHeight+4, pview_attr->viewWidth+4,1);
	
	return SUCCESS;
}

S32 SetBT_ListStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	U32 len;
	tBdAddress btaddr;
	char name[42],strs[50];
	int ret;

	memset(name, 0x00, 42);
	ret = hyco_getSearchList(i, &btaddr, &name[0]);
	if(i == gu8BtConnIdx)
	{
		sprintf(strs,"%d.(����)%s",i+1,name);
	}
	else
	{
		sprintf(strs,"%d.(δ��)%s",i+1,name);
	}
	if (ret == TRUE)
	{
		len = strlen((char *)strs);
		
		if(len >= maxLen-1)
		{
			memcpy(str,strs,maxLen);
			str[maxLen-1] = 0; 
			str[maxLen-2] = 0; 
		}
		else
		{
			memcpy(str,strs,len);
			str[len] = 0;
			str[len+1] = 0;
		}
		
		return SUCCESS;
	}
	return FAIL;
}

 /*----------------------------------------------------------
 * ������ string Ϊ�յ� ʱ��ſ��øĺ�����ȡ
 *
 *------------------------------------------------------------*/
static S32 SetBT_ComboboxGetStr(void *pView,U16 i, U8 *pStr)
{
	U32 len;
	U8 *pSrcStr;
		
	pSrcStr = (U8 *)(bt_protocolSet[i]);
	
	len = strlen((char *)pSrcStr);
	
	memcpy(pStr,pSrcStr,len);
	pStr[len] = 0;
	pStr[len+1] = 0;
		
	return SUCCESS;
}

S32 SetBT_ComboxEnter(void *pView, U16 i)
{
	tGRAPCOMBOBOX	*pCombox;
	
	gtHyc.bt_devenable = i;
	
	return SUCCESS;
}


/*---------------------------------------------
*����:Set_BT
*����:��������
*����:
*����:
*-----------------------------------------------*/
void Set_BT(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPLIST		*pList;
	tGRAPEDIT 		*pEdit;
	tGRAPCOMBOBOX	*pCombo;
	int i, idx = 0;
	
	gu8BtConnIdx = 0xff;
	gu32SettingTemp = gtHyc.bt_devenable;
	if(gu32SettingTemp >= 4)
	{
		gu32SettingTemp = 0;
	}
	
	pDesk = Grap_CreateDeskbox(&gtBTsetVAtrr[idx++], "�������");
	if(!pDesk)	return ;
	Grap_Inherit_Public((tGRAPDESKBOX *)pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable= 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_2;
	
	for (i=0 ;i<7; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &gtBTsetVAtrr[idx++], &gtBTsetBPri[i]);
		if (NULL == pBtn) return;
		if(i < 3)
		{
			pBtn->pressEnter = SetBT_BtnEnter;
		}
	}
	
	pEdit = Grap_InsertEdit(pDesk, &gtBTsetVAtrr[idx++], &gtBTsetEPri[0]);
	pEdit->DrawInputCnt = Set_EditDrawInputCnt;
	if (NULL == pEdit) return;
	strcpy(pEdit->edit_privattr.digt,gtHyc.bt_addr2);
	
	pEdit = Grap_InsertEdit(pDesk, &gtBTsetVAtrr[idx++], &gtBTsetEPri[1]);
	pEdit->DrawInputCnt = Set_EditDrawInputCnt;
	if (NULL == pEdit) return;
	strcpy(pEdit->edit_privattr.digt, gtHyc.bt_pwd);

	//���ӳ�ѡ���
	pCombo = Grap_InsertCombobox(pDesk,&gtBTsetVAtrr[idx++], &gtBTsetCPri[0]);
	if (NULL == pCombo)return ;
	pCombo->comboPriv.total_num = sizeof(bt_protocolSet)/sizeof(U8*);
	pCombo->comboxgetstr = SetBT_ComboboxGetStr;
	pCombo->comboxenter	= SetBT_ComboxEnter;
	pCombo->comboPriv.cur_index = gtHyc.bt_devenable;
	
	pList = Grap_InsertLister(pDesk,&gtBTsetVAtrr[idx++], &gtBTsetLPri[0]);
	if (NULL== pList) return ;
	pList->enter = SetBT_ListerEnter;
	pList->getListString = SetBT_ListStr;
	pList->lst_privattr.totalItem = 0;//hyco_getSearchListNum();
	pList->draw = SetBT_ListerDraw;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	//�˳��ͶϿ�BT
	if (hyco_getConnectState() == TRUE)
	{
		bt_disconnect(pDeskFather);
	}
	
	if(gu32SettingTemp != gtHyc.bt_devenable)
	{
		glbVariable_SaveParameter();
	}
	
	return ;
}


/*---------------------------------------------
*����:Set_BT_UseNow
*����:��������
*����:
*����:
*-----------------------------------------------*/
int Set_BT_UseNow(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPLIST		*pList;
	tGRAPEDIT 		*pEdit;
	tGRAPCOMBOBOX	*pCombo;
	int i, idx = 0,connflag=0;
	
	gu8BtConnIdx = 0xff;
	gu32SettingTemp = gtHyc.bt_devenable;
	if(gu32SettingTemp >= 4)
	{
		gu32SettingTemp = 0;
	}
	
	pDesk = Grap_CreateDeskbox(&gtBTsetVAtrr[idx++], "�������");
	if(!pDesk)	return ;
	Grap_Inherit_Public((tGRAPDESKBOX *)pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable= 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_2;
	
	for (i=0 ;i<7; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &gtBTsetVAtrr[idx++], &gtBTsetBPri[i]);
		if (NULL == pBtn) return;
		if(i < 3)
		{
			pBtn->pressEnter = SetBT_BtnEnter;
		}
	}
	
	pEdit = Grap_InsertEdit(pDesk, &gtBTsetVAtrr[idx++], &gtBTsetEPri[0]);
	pEdit->DrawInputCnt = Set_EditDrawInputCnt;
	if (NULL == pEdit) return;
	strcpy(pEdit->edit_privattr.digt,gtHyc.bt_addr2);
	
	pEdit = Grap_InsertEdit(pDesk, &gtBTsetVAtrr[idx++], &gtBTsetEPri[1]);
	pEdit->DrawInputCnt = Set_EditDrawInputCnt;
	if (NULL == pEdit) return;
	strcpy(pEdit->edit_privattr.digt, gtHyc.bt_pwd);

	//���ӳ�ѡ���
	pCombo = Grap_InsertCombobox(pDesk,&gtBTsetVAtrr[idx++], &gtBTsetCPri[0]);
	if (NULL == pCombo)return ;
	pCombo->comboPriv.total_num = sizeof(bt_protocolSet)/sizeof(U8*);
	pCombo->comboxgetstr = SetBT_ComboboxGetStr;
	pCombo->comboxenter	= SetBT_ComboxEnter;
	pCombo->comboPriv.cur_index = gtHyc.bt_devenable;
	
	pList = Grap_InsertLister(pDesk,&gtBTsetVAtrr[idx++], &gtBTsetLPri[0]);
	if (NULL== pList) return ;
	pList->enter = SetBT_ListerEnter;
	pList->getListString = SetBT_ListStr;
	pList->lst_privattr.totalItem = 0;//hyco_getSearchListNum();
	pList->draw = SetBT_ListerDraw;

	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	if(hyco_getConnectState() == TRUE)
	{
		connflag = 1;
	}
	
	if(gu32SettingTemp != gtHyc.bt_devenable)
	{
		glbVariable_SaveParameter();
	}
	
	return connflag;
}