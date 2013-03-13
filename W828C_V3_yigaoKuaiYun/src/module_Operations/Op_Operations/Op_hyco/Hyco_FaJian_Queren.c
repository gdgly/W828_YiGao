/*-----------------------------------------------
����--->��תվ    ��תվ---->��תվ    ��תվ---->����
��������
������ʾ
���䷽ʽ
��תվ
��һվ
����
�˵���
------------------------------------------------*/
#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Op_Common.h"
#include "Op_Struct.h"
#include "Op_BarRuler.h"
#include "Rec_Info.h"

#ifdef VER_CONFIG_HYCO	//�ƴ�

const U8 *const gcCarOrderFJQR[]=
{
	"����",	"����",	"����"
};

/*==========================================��������==========================================*/
const tGrapViewAttr FaJianQR_ViewAttr[] =
{
	{COMM_DESKBOX_ID,    0,  0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//����
	
#ifdef W818C
	{BUTTON_ID_1,	     9,282, 73,34,     9,282, 82,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//ȷ��
	{BUTTON_ID_2,	    83,282, 73,34,    83,282,156,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//ɾ��
	{BUTTON_ID_3,	   158,282, 73,34,   158,282,231,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
#else	
	{BUTTON_ID_3,	     9,282, 73,34,     9,282, 82,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_2,	    83,282, 73,34,    83,282,156,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//ɾ��
	{BUTTON_ID_1,	   158,282, 73,34,   158,282,231,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//ȷ��
#endif	
	{BUTTON_ID_4,	   165,111, 70,20,   165,111,238,121,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	
	{BUTTON_ID_10,	   5,  35 ,64, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//���䷽ʽ
	{BUTTON_ID_11,	   5,  60 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//��תվ
	{BUTTON_ID_12,	   5,  85 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//��һվ
	{BUTTON_ID_13,	   5, 110 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//��  ��
	{BUTTON_ID_14,	 140, 110 ,16, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//Kg
	{BUTTON_ID_15,	   5, 135 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//�˵���
	
	{COMBOBOX_ID_1,	   70, 35,158,22,   70,35,228,57,  0,0,0xffff,0,1,1,1,0},//���䷽ʽ
	{EDIT_ID_1,	   	   55, 60,83, 22,   55,60,138,82,  0,0,0xffff,0,1,0,1,0},//��תվ
	{EDIT_ID_2,	   	   55, 85,83, 22,   55,85,138,107,  0,0,0xffff,0,1,0,1,0},//��һվ
	{EDIT_ID_3,   	   55,110,83, 22,   55,110,138,132, 0,0,0xCE59,0,1,0,1,0},//����
	{EDIT_ID_4,	   	   55,135,172,22,   55,135,227,157,0,0,0xffff,0,1,0,1,0},//�˵��� 21
	
	{EDIT_ID_20,       140,60 ,100,22,  140,60,240,82,     0,0,0xCE59,0,1,0,0,0},//��תվ����
	{EDIT_ID_21,       140,85 ,100,22,  140,85,240,107,    0,0,0xCE59,0,1,0,0,0},//��һվ����
};

const tGrapButtonPriv FaJianQR_BtnPriv[]=
{
#ifdef W818C
	{"ȷ��",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//ȷ��
	{"ɾ��",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, 			TY_UNITY_BG1_EN,NULL},//ɾ��
	{"����",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//����
#else
	{"����",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//����
	{"ɾ��",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, 			TY_UNITY_BG1_EN,NULL},//ɾ��
	{"ȷ��",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//ȷ��
#endif	
	{"����",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, 		TY_UNITY_BG1_EN,NULL},//
	
	{"���䷽ʽ",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"��תվ",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"��һվ",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"��  ��",  0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"Kg",  	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"�˵���",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
};

const tGrapEditPriv FaJianQR_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,10,  1, COMPANY_ID_LEN-1,   0,0,2,0,0,1,  50,50,0,  0,1,1,1},//��תվ
	{NULL,0x0,0,0,2,4,10,  1, COMPANY_ID_LEN-1,   0,0,2,0,0,1,  50,50,0,  0,1,1,1},//��һվ
	{gtOpInfoGlb.weight,  0x0,0,0,2,4,9,  1, WEIGHT_LEN-1,	0,0,1,0,0,1,  0,4,0,  1,0,1, 0},//����
	{NULL,0x0,0,0,2,4,21,  1, BARCODE_LEN-1,		0,0,2,0,0,1,  50,50,0,  0,1,1,1},//�˵���
	
	{gtOpInfoGlb.station1,0x0,0,0,2,4,12,  1, COMPANY_NAME_LEN-1,		0,0,0,0,0,1,  50,50,0,  1,0,0,0},//��תվ
	{gtOpInfoGlb.station2,0x0,0,0,2,4,12,  1, COMPANY_NAME_LEN-1,		0,0,0,0,0,1,  50,50,0,  1,0,0,0},//��һվ
};

const tComboboxPriv FaJianQR_ComAttr[]=
{
	{0,6,0,0,0,0,    1,138,16,158,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//���䷽ʽ
};

int FaJianQR_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT 		*pEditWT;
	tDATA_INFO 		Recd;
	int ret;
	U8	*pCode;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
		if(gpEdit_Stat1==NULL || gpEdit_Stat2==NULL || gpEdit_Code==NULL)
		{
			Com_SpcDlgDeskbox("�ؼ���Ϣ�쳣!",0,pDesk,1,NULL,NULL,100);
			return RETURN_REDRAW;
		}
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://ȷ��
			if(gpEdit_Stat1->view_attr.curFocus == 1)//��תվ
			{
				if(strlen(gtOpInfoGlb.station1) == 0)
				{
					gpEdit_Stat1->editenter(gpEdit_Stat1);
				}
				else
				{
					Grap_ChangeFocus(gpEdit_Stat1,1);
				}
				break;
			}
			else if(gpEdit_Stat2->view_attr.curFocus == 1)//��һվ
			{
				if(strlen(gtOpInfoGlb.station2) == 0)
				{
					gpEdit_Stat2->editenter(gpEdit_Stat2);
				}
				else
				{
					Grap_ChangeFocus(gpEdit_Stat2,1);
				}
				break;
			}
			else if(gpEdit_Code->view_attr.curFocus == 1)
			{
				pCode = gpEdit_Code->edit_privattr.digt;
				if(strlen(pCode) == 0)
				{
					break;
				}
				
				//�жϵ���
				ret = OpCom_CheckBarcode(pCode,TYPE_BARCODE);
				if(ret == 0 || ret == 1)
				{
					if(strlen(gtOpInfoGlb.station2) == 0)
					{
						Com_SpcDlgDeskbox("����������һվ!",0,pDesk,1,NULL,NULL,100);
						memset(gpEdit_Code->edit_privattr.digt, 0, gpEdit_Code->edit_privattr.containLen);
						break;
					}
					
					//��鵥���Ƿ����
					ret = Op_ComCheckExist(pDesk, pCode, OpCom_GetCurOpType());
					if(ret == 0)//�Ѵ���,δɾ��
					{
						
					}
					else if(ret == 1)//�Ѵ���,��ɾ��
					{
						
					}
					else //������
					{
						//����¼�Ĺ�������
						OpCom_FillCommInfo(&Recd, pCode, NULL, RECORD_UNSEND);

						//ҵ�񲿷�
						strcpy(Recd.uData.saomiao.prevstat, gpEdit_Stat1->edit_privattr.digt);
						strcpy(Recd.uData.saomiao.nextstat, gpEdit_Stat2->edit_privattr.digt);
						sprintf(Recd.uData.saomiao.tran_types, "%d", gtOpInfoGlb.tempIndex);//���䷽ʽ
						strcpy(Recd.uData.saomiao.obj_wei, gtOpInfoGlb.weight);
						
						//д��¼
						ret = Rec_WriteNewRecode(&Recd, NULL);
						if (ret == 0)
						{
							Op_CommonListShow_Add(pCode);
							//memset(gtOpInfoGlb.weight, 0, sizeof(gtOpInfoGlb.weight));
						}
						else
						{
							Com_SpcDlgDeskbox("��¼����ʧ��!",0,pDesk,1,NULL,NULL,100);
						}
					}
					memset(gpEdit_Code->edit_privattr.digt, 0, gpEdit_Code->edit_privattr.containLen);
				}
				else
				{
					Com_SpcDlgDeskbox("�˵���ʽ����!",0,pDesk,1,NULL,NULL,100);
				}
			}
			else
			{
				//��������
				//Edit_ChangeFocus(pDesk, 1);
				Grap_ChangeFocus(gpEdit_Code,1);
			}
			break;
		case BUTTON_ID_2://ɾ��
			Op_ComDelCode(pDesk, OpCom_GetCurOpType());
			break;
		case BUTTON_ID_3://����
			if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�Ƿ��˳��ò���?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
			{
				return RETURN_QUIT;
			}
			break;
		case BUTTON_ID_4://�������
			pEditWT = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_3);
			OpComm_ConnBT(pDesk, pEditWT);
			if(gpEdit_Code->view_attr.curFocus != 1)
			{
				Grap_ChangeFocusEnable(gpEdit_Code);
			}
			break;
		default:
			break;
		}
	}
	
	return RETURN_REDRAW;
}

U32 FaJianQR_EditKeyEnd(void *pView, U8 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit,*pEditName;
	int ret = 0;
	
	pEdit = (tGRAPEDIT *)pView;
	
	if(pEdit->view_attr.id == EDIT_ID_1)//��תվ
	{
		if(strlen(pEdit->edit_privattr.digt) > 0)
		{
			ret = Company_GetNameById(pEdit->edit_privattr.digt, gtOpInfoGlb.station1, sizeof(gtOpInfoGlb.station1));
		}

		if(ret == 0)
		{
			memset(gtOpInfoGlb.station1, 0, sizeof(gtOpInfoGlb.station1));
		}
		
		pDesk= (tGRAPDESKBOX *)pEdit->pParent;
		pEditName = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_20);//ID��һ��ȷ�Ϻ�
		
		if(pEditName) pEditName->draw(pEditName, 0, 0);
	}
	else if(pEdit->view_attr.id == EDIT_ID_2)//��һվ
	{
		if(strlen(pEdit->edit_privattr.digt) > 0)
		{
			ret = Company_GetNameById(pEdit->edit_privattr.digt, gtOpInfoGlb.station2, sizeof(gtOpInfoGlb.station2));
		}

		if(ret == 0)
		{
			memset(gtOpInfoGlb.station2, 0, sizeof(gtOpInfoGlb.station2));
		}
		
		pDesk= (tGRAPDESKBOX *)pEdit->pParent;
		pEditName = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_21);//ID��һ��ȷ�Ϻ�
		
		if(pEditName) pEditName->draw(pEditName, 0, 0);
	}
	
	return SUCCESS ;
}

S32 FaJianQR_EditEnter(void *pView)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit;
	U8 *pCode;
	int ret;
	
	pEdit = (tGRAPEDIT *)pView;
	pDesk= (tGRAPDESKBOX *)pEdit->pParent;
	
	if(pEdit->view_attr.id == EDIT_ID_1)//��һվ
	{
		if(strlen(gtOpInfoGlb.station1) == 0)
		{
			ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
			ret = Op_DownLoad_Company_Desk(pDesk, 1);
			ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			//����û���ж�ret, ��ΪCompany_GetNameById�������ж�
			Company_GetIdNameByIdx(ret, pEdit->edit_privattr.digt, pEdit->edit_privattr.containLen, gtOpInfoGlb.station1, sizeof(gtOpInfoGlb.station1));
			pEdit->edit_privattr.Pos = strlen(pEdit->edit_privattr.digt);
				
			if(ret != -1)
			{
				Grap_ChangeFocus(pEdit,1);
			}
		}
		else
		{
			return SUCCESS;
		}
	}
	else if(pEdit->view_attr.id == EDIT_ID_20)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		ret = Op_DownLoad_Company_Desk(pDesk, 1);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		//����û���ж�ret, ��ΪCompany_GetNameById�������ж�
		Company_GetIdNameByIdx(ret, gpEdit_Stat1->edit_privattr.digt, gpEdit_Stat1->edit_privattr.containLen, gtOpInfoGlb.station1, sizeof(gtOpInfoGlb.station1));
		gpEdit_Stat1->edit_privattr.Pos = strlen(gpEdit_Stat1->edit_privattr.digt);
		
		if(ret != -1)
		{
			Grap_ChangeFocusEnable(gpEdit_Stat2);
		}
	}
	else if(pEdit->view_attr.id == EDIT_ID_2)//Ŀ��վ
	{
		if(strlen(gtOpInfoGlb.station2) == 0)
		{
			ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
			ret = Op_DownLoad_Company_Desk(pDesk, 1);
			ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			//����û���ж�ret, ��ΪCompany_GetNameById�������ж�
			Company_GetIdNameByIdx(ret, pEdit->edit_privattr.digt, pEdit->edit_privattr.containLen, gtOpInfoGlb.station2, sizeof(gtOpInfoGlb.station2));
			pEdit->edit_privattr.Pos = strlen(pEdit->edit_privattr.digt);
			
			if(ret != -1)
			{
				Grap_ChangeFocus(pEdit,1);
			}
		}
		else
		{
			return SUCCESS;
		}
	}
	else if(pEdit->view_attr.id == EDIT_ID_21)
	{
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		ret = Op_DownLoad_Company_Desk(pDesk, 1);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		//����û���ж�ret, ��ΪCompany_GetNameById�������ж�
		Company_GetIdNameByIdx(ret, gpEdit_Stat2->edit_privattr.digt, gpEdit_Stat2->edit_privattr.containLen, gtOpInfoGlb.station2, sizeof(gtOpInfoGlb.station2));
		gpEdit_Stat2->edit_privattr.Pos = strlen(gpEdit_Stat2->edit_privattr.digt);
		
		if(ret != -1)
		{
			Grap_ChangeFocusEnable(gpEdit_Code);
		}
	}
	
	return RETURN_REDRAW;
}

S32 FaJianQR_ComboboxGetStr(void *pView,U16 i, U8 *pStr)
{
	strcpy(pStr, gcCarOrderFJQR[i]);
	
	return SUCCESS;
}

S32 FaJianQR_ComboxEnter(void *pView, U16 i)
{
	tGRAPCOMBOBOX	*pCombox;
	
	pCombox = (tGRAPCOMBOBOX *)pView;
	
	if(pCombox->list_opened == 1)
	{
		gtOpInfoGlb.tempIndex = i;
		Grap_ChangeFocus(pCombox, 1);//�����л�����
	}
	
	return SUCCESS;
}

/*-----------------------------------------------
*����:Operation_FaJianQR_Desk
*����:�ռ�
*����:
*����:
*-----------------------------------------------*/
int Operation_FaJianQR_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	tGRAPCOMBOBOX	*pComBox;
	int				i,idx=0;
	
	gtOpInfoGlb.tempIndex = 0;
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&FaJianQR_ViewAttr[idx++], "����ɨ��");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_4;
	
	for (i=0; i<10; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &FaJianQR_ViewAttr[idx++], &FaJianQR_BtnPriv[i]);
		if(NULL == pBtn ) return -1;
		
		if(i < 4)
		{
			pBtn->pressEnter = FaJianQR_BtnPress;
			if(i == 3)
			{
				pBtn->draw = OpComm_BtnBTDraw;
			}
		}
	}
	
	pComBox = Grap_InsertCombobox(pDesk, &FaJianQR_ViewAttr[idx++], &FaJianQR_ComAttr[0]);
	if(NULL == pComBox ) return -1;
	pComBox->comboPriv.total_num = sizeof(gcCarOrderFJQR)/sizeof(U8 *);
	pComBox->comboxgetstr = FaJianQR_ComboboxGetStr;
	pComBox->comboxenter  = FaJianQR_ComboxEnter;
	
	//��תվ  ��һվ   ����  �˵�  ��תվ����  ��һվ����
	for (i=0; i<6; i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &FaJianQR_ViewAttr[idx++], &FaJianQR_EditPriv[i]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(NULL == pEdit ) return -1;
		if(i == 0)//��תվ
		{
			pEdit->editenter  = FaJianQR_EditEnter;
			pEdit->editKeyEnd = FaJianQR_EditKeyEnd;
			gpEdit_Stat1 = pEdit;
		}
		else if(i == 1)//��һվ
		{
			pEdit->editenter  = FaJianQR_EditEnter;
			pEdit->editKeyEnd = FaJianQR_EditKeyEnd;
			gpEdit_Stat2 = pEdit;
		}
		else if(i == 2)//����
		{
			pEdit->handle = OpComm_EditHandle_Price;
			pEdit->editTimer = BT_WeightEditTimer;
			strcpy(pEdit->edit_privattr.digt, "0.00");
		}
		else if(i == 3)//�˵���
		{
			gpEdit_Code = pEdit;
		}
		else if(i == 4 || i == 5)
		{
			pEdit->editenter  = FaJianQR_EditEnter;
		}
	}
		
	Op_CommonInsertListShow(pDesk, 10, 160, 220, 120);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	//�˳��ͶϿ�BT
	if (hyco_getConnectState() == TRUE)
	{
		bt_disconnect(pDeskFather);
	}
	
	return 0;
}

/*-----------------------------------------��¼��ѯ��ʾ------------------------------------------------*/
const U8* const opType_FaJianQR[]=//����
{
	"״̬:",
	
	"��������:",
	"ɨ������:",
	"Ա�����:",
	"ɨ��ʱ��:",
	
	"���䷽ʽ:",
	"��תվ:",
	"��һվ:",
	"����:",
};


/*---------------------------------------------------------------
*����: FaJianQR_SearchDisplay
*����: ��ʾ��¼��ѯ�������ַ���
*����: pData:��ѯ���Ľ��
       total:��ѯ��������
       pDispStr:(out)��ʾ��buf
       dispBufLen:pDispStr����󳤶�
*����: 
----------------------------------------------------------------*/
int FaJianQR_SearchDisplay(void *p)
{
	U8 *str[]={"δ��", "�Ѵ�", "��ɾ", "δ��", "δ��"};
	U8 *pDispStr;
	tOP_DISPINFO 	*pOpCb;
	tDATA_INFO		*pData;
	tDATA_COMM 		*pComm;
	tSAOMIAO		*saomiao;
	int total,dispBufLen;
	int i, j, cnt,stat,len=0;
	
	pOpCb = (tOP_DISPINFO *)p;
	pData = (tDATA_INFO *)(pOpCb->pData);
	total = pOpCb->total;
	pDispStr = pOpCb->pDispBuf;
	dispBufLen = pOpCb->dispBufLen;
	
	for(i = 0; i < total; i++)
	{
		pComm = &(pData[i].tComm);
		stat = pComm->state;
		saomiao = &(pData[i].uData.saomiao);
		sprintf(&pDispStr[len], "%s%s\r\n%s%d\r\n%s%s\r\n%s%s\r\n%s%s\r\n\
%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n-----------------------\r\n",
							opType_FaJianQR[0], str[stat],
							opType_FaJianQR[1], pComm->opType,
							opType_FaJianQR[2], pComm->scanStat,
							opType_FaJianQR[3], pComm->userId,
							opType_FaJianQR[4], pComm->scanTime,
							
							opType_FaJianQR[5], gcCarOrderFJQR[atoi(saomiao->tran_types)],
							opType_FaJianQR[6], saomiao->prevstat,
							opType_FaJianQR[7], saomiao->nextstat,
							opType_FaJianQR[8], pComm->code
							);
		
		len += strlen(&pDispStr[len]);
		//��Ҫ�жϳ����Ƿ񳬳�
		if(len + 512 >= dispBufLen)
		{
			strcpy(&pDispStr[len], "��ʾ�ռ䲻��,������¼�޷���ʾ.");
		}
	}
	
	return 0;
}


/*-----------------------------------------��֯��������------------------------------------------------*/
/*---------------------------------------------------------------
*����: FaJianQR_FillData
*����: �����������
*����: pSend:���ڷ��͵�buf
       pNode:׼��������͵�����
       cnt:׼������ĸ���
       pUrl:�����url
*����: 0:�ɹ�   -1:����ͨѶ�쳣
----------------------------------------------------------------*/
int FaJianQR_FillData(void *p)
{
	struct protocol_http_request tRequest;
	int			dataLen,ret=-1;
	int 		i,bufLen, cnt;
	tOP_SENDINFO *pOpCb;
	tQUE_NODE 	*pNode;
	tDATA_COMM	*pComm;
	tSAOMIAO	*pSaomiao;
	U8 *pSend, *pUrl, *pGbk, *pTemp,*pE;
	U8	buf[10];
	
	pOpCb = (tOP_SENDINFO *)p;
	pSend = pOpCb->pSend;
	bufLen= pOpCb->bufLen;
	pNode = (tQUE_NODE *)( pOpCb->pNode);
	cnt   = pOpCb->cnt;
	pUrl  = pOpCb->pUrl;

return 0;//test
	
	//���������ϴ�	
	//�������ͷ
	dataLen = NetReq_FillHead_yida(pSend, "", "addScanData");
	for(i = 0; i < cnt; i++)
	{
		pComm = &(pNode[i].tData.tComm);
		pSaomiao = &(pNode[i].tData.uData.saomiao);
		
		sprintf(&pSend[dataLen], "<detailValue><scanType>%02d</scanType><scanUser>%s</scanUser><scanSite>%s</scanSite><scanTime>%s</scanTime>\
<scanHawb>%s</scanHawb><preNextSite>%s</preNextSite><shiftTimes>%s</shiftTimes><weight></weight><bagNumber></bagNumber>\
<exceptionCode></exceptionCode><exceptionMemo></exceptionMemo><signatureType></signatureType><signAture></signAture>\
<businessMan>%s</businessMan></detailValue>",
					pComm->opType,pComm->userId,pComm->scanStat,pComm->scanTime,pComm->code,pSaomiao->nextstat,pSaomiao->tran_types,pComm->userId);
		
		dataLen = strlen(pSend);
	}
	
	strcpy(&pSend[dataLen], "</senddata>");
	dataLen += strlen("</senddata>");
	
hyUsbPrintf("dataLen = %d \r\n",dataLen);	
	if(dataLen <= 0)
	{
		return -1;
	}
#if 0	
	//����ת��
	//����ǰ��Ҫ��ת��unicode  ��ת��utf-8
	pTemp = (U8 *)hsaSdram_DecodeBuf();
	//1 תunicode
	Hyc_Gbk2Unicode(pSend,pTemp,0);
	//2 תutf-8
	Unicode2UTF8(pTemp, pSend);
#endif
	
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

hyUsbPrintf("FaJianQR send = ");
hyUsbMessageByLen(pSend, strlen(pSend));
hyUsbPrintf("\r\n");
	cnt = http_send_request(pUrl, bufLen, &tRequest);
	if(cnt > 0)
	{
		pGbk = (U8 *)hsaSdram_DecodeBuf();
		
		cnt = Net_DecodeData(pSend,cnt,pGbk);
		if(cnt > 0)
		{
			memcpy(pSend,pGbk,cnt);
			pSend[cnt] = 0;
			pSend[cnt+1] = 0;
	hyUsbPrintf("FaJianQR recv = %s \r\n",pSend);	
			pTemp = strstr(pSend, "<result>");
			pE = strstr(pSend, "</result>");
			if(pTemp != NULL && pE != NULL)
			{
				pTemp += strlen("<result>");
				dataLen = pE - pTemp;
				if(dataLen > 4) dataLen = 4;
				memcpy(buf,pTemp, dataLen);
				buf[dataLen] = 0;
				ret = atoi(buf);
			}
		}
	}
hyUsbPrintf("FaJianQR ret = %d \r\n",ret);	
	
	return ret;
}

#endif //#ifdef VER_CONFIG_HYCO	//�ƴ�