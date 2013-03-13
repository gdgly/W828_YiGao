/*
ǩ��

�˵���
ǩ����
����
�쳣ԭ��   ��ע
����
*/
#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Op_Common.h"
#include "Op_Struct.h"
#include "Op_BarRuler.h"
#include "Rec_Info.h"

#ifdef VER_CONFIG_HYCO	//�ƴ��汾

/*====================================================================================*/
const tGrapViewAttr QianShou_ViewAttr[] =
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
	{BUTTON_ID_4,	   148,116, 73,34,   148,116,221,150,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	
	{BUTTON_ID_10,	   5,  32 ,48, 20,   0,0,0,0,   0,0,0,0,  1,0,0,0},//�˵���
	{BUTTON_ID_11,	   5,  50 ,48, 20,   0,0,0,0,   0,0,0,0,  1,0,0,0},//ǩ����
	{BUTTON_ID_12,	   5,  100 ,32, 20,   0,0,0,0,   0,0,0,0,  1,0,0,0},//����
	{BUTTON_ID_13,	   5,  125 ,32, 20,   0,0,0,0,   0,0,0,0,  0,0,0,0},//ԭ��
	{BUTTON_ID_14,	   5,  152 ,32, 20,   0,0,0,0,   0,0,0,0,  0,0,0,0},//��ע
	
	{BUTTON_ID_20,	   145,55,80,60,  145,55,225,115, YD_Z_P_EN,YD_Z_P_EN,0,     0,1,0,0,0},//��Ƭ���
	
	{EDIT_ID_1,	    55, 32,172,22,    55, 30,227, 52, 0,0,0xffff,0,  1,1,1,0},//�˵���
	{EDIT_ID_2,	    5,72,135,22,      5,72,140,94, 0,0,0xffff,0,  1,0,1,0},//ǩ����
	
	{COMBOBOX_ID_1,	   40, 100,100,22,   40,100,140,122,  0,0,0xffff,0,1,0,1,0},//ǩ������  ����ǩ��   �쳣ǩ��
	{COMBOBOX_ID_2,	   40, 125,100,22,   40,125,140,147, 0,0,0xffff,0,0,0,1,0},//�쳣ǩ��ԭ��
	
	{EDIT_ID_3,	    40,152,172,22,      55,152,227,174, 0,0,0xffff,0,  0,0,1,0},//��ע
};

const tGrapButtonPriv QianShou_BtnPriv[]=
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
	{"����",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, 			TY_UNITY_BG1_EN,NULL},//����
	
	{"�˵���",		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//�˵���
	{"ǩ����",		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//ǩ����
	{"����",		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//����
	{"ԭ��",		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//ԭ��
	{"��ע",		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//��ע
	
	{NULL, 0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//��Ƭ���
};

const tGrapEditPriv QianShou_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,21,  1, BARCODE_LEN-1,  		0,0,2,0,0,1,  50,50,0,  0,1,1,1},//�˵���
	{NULL,0x0,0,0,2,4,14,  1, EMPLOYEE_NAME_LEN-1,	0,0,0,0,0,1,  50,50,0,  0,0,4,0},//ǩ����
	
	{NULL,0x0,0,0,2,4,21,  1, REMARK_LEN-1,	0,0,0,0,0,1,  50,50,0,  0,0,5,0},//��ע
};

const tComboboxPriv QianShou_ComAttr[]=
{
	{2,2,0,0,0,0,    1,80,10,100,12,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//ǩ������
	
	{2,6,0,0,0,0,    1,80,10,100,12,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//�쳣ԭ��
};

int Op_QianShou_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT		*pEdit_Signer, *pEditReason;
	tGRAPCOMBOBOX	*pCombox;
	tDATA_INFO 		Recd;
	U8 *pBuf, *pTemp;
	int ret;
	U32 photoLen=0;
	
	if(state != STATE_NORMAL)
	{
		return SUCCESS;
	}
	
	pBtn  = (tGRAPBUTTON *)pView;
	pDesk = (tGRAPDESKBOX *)pBtn->pParent;
	
	pEdit_Signer = (tGRAPEDIT *)Grap_GetViewByID(pDesk, EDIT_ID_2);
	pEditReason  = (tGRAPEDIT *)Grap_GetViewByID(pDesk, EDIT_ID_3);
	if (gpEdit_Code==NULL || pEdit_Signer==NULL)
	{
		Com_SpcDlgDeskbox("���ҿؼ�ʧ��!",0,pDesk,1,NULL,NULL,150);
		return RETURN_REDRAW;
	}
	
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://ȷ��
		if(gpEdit_Code->view_attr.curFocus == 1)
		{
			if(strlen(gpEdit_Code->edit_privattr.digt) == 0)
			{
				break;
			}
			
			if(OpCom_CheckBarcode(gpEdit_Code->edit_privattr.digt,TYPE_BARCODE) == -1)
			{
				Com_SpcDlgDeskbox("�˵���ʽ����!",0,pDesk,1,NULL,NULL,100);
				break;
			}
			
			//��鵥���Ƿ����
			if(0 == Op_ComCheckExist(pDesk, gpEdit_Code->edit_privattr.digt,OpCom_GetCurOpType()))
			{
				break;
			}
			
			gpEdit_Code->view_attr.curFocus = 0;
			pEdit_Signer->view_attr.curFocus = 1;
			Grap_ChangeInputMode(pEdit_Signer);
		}
		else if(pEdit_Signer->view_attr.curFocus == 1 || (pEditReason->view_attr.enable && pEditReason->view_attr.curFocus))//����
		{
			if(strlen(gpEdit_Code->edit_privattr.digt) == 0)
			{
				Com_SpcDlgDeskbox("����ɨ���˵���!",0,pDesk,1,NULL,NULL,100);
				break;
			}
			
			if(strlen(pEdit_Signer->edit_privattr.digt) == 0)
			{
				Com_SpcDlgDeskbox("����дǩ����!",0,pDesk,1,NULL,NULL,100);
				break;
			}
			
			if(OpCom_CheckBarcode(gpEdit_Code->edit_privattr.digt,TYPE_BARCODE) == -1)
			{
				Com_SpcDlgDeskbox("�˵���ʽ����!",0,pDesk,1,NULL,NULL,100);
				break;
			}
			
			//��鵥���Ƿ����
			ret = Op_ComCheckExist(pDesk, gpEdit_Code->edit_privattr.digt,OpCom_GetCurOpType());
			if(1 == ret)
			{
				//ɾ����Ӧ��ͼƬ
				PicStore_Delete(gpEdit_Code->edit_privattr.digt);
				memset(gpEdit_Code->edit_privattr.digt, 0, gpEdit_Code->edit_privattr.containLen);
				break;
			}
			else if(ret == 0)
			{
				break;
			}
			
			//����Ƿ��Ѿ�����
			if(gu32PhotoSize == 0)
			{
				if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�Ƿ���Ҫ����?",0,pDesk,3,NULL,NULL,500))
				{
					OpComm_Photo(pDesk, 0, gpEdit_Code->edit_privattr.digt);
					break;
				}
				gu32PhotoSize = 0;
				gpPhotoBtn->btn_privattr.pBufPic = NULL;
			}
			
			//�洢
			memset(&Recd, 0, sizeof(tDATA_INFO));
			//��������
			//����¼�Ĺ�������
			OpCom_FillCommInfo(&Recd, gpEdit_Code->edit_privattr.digt, NULL, RECORD_UNSEND);
			
			if(gu32PhotoSize > 0)
			{
				Recd.uData.saomiao.picflag = 1;
				Recd.tComm.nopacket = 1;//���ܴ��
			}
			Recd.uData.saomiao.signType = gtOpInfoGlb.tempIndex;
			if(gtOpInfoGlb.tempIndex == 1)//�쳣ǩ��
			{
				pCombox = (tGRAPCOMBOBOX *)Grap_GetViewByID(pDesk, COMBOBOX_ID_2);
			//	DownLoad_GetReasonCodeByIndex(pCombox->comboPriv.cur_index);
				sprintf(Recd.uData.saomiao.cmt_ids, "%d", pCombox->comboPriv.cur_index);
				strcpy(Recd.uData.saomiao.cmt_inf, pEditReason->edit_privattr.digt);
			}

			strcpy(Recd.uData.saomiao.customer,pEdit_Signer->edit_privattr.digt);
			
			//д��¼
			ret = Rec_WriteNewRecode(&Recd, NULL);
			if (ret == 0)
			{
				Op_CommonListShow_Add(gpEdit_Code->edit_privattr.digt);
				if(gu32PhotoSize > 0)//�Ĺ���Ƭ
				{
					if(-1 == PicStore_Write(gpEdit_Code->edit_privattr.digt, gpDcWorkBuf, gu32PhotoSize))
					{
						Com_SpcDlgDeskbox("ͼƬ����ʧ��!",0,pDesk,1,NULL,NULL,150);
					}
				}
				//memset(gau8PhotoCode, 0, BARCODE_LEN+2);
				gu32PhotoSize = 0;
				gpPhotoBtn->btn_privattr.pBufPic = NULL;
				memset(gpEdit_Code->edit_privattr.digt, 0, gpEdit_Code->edit_privattr.containLen);
				memset(pEdit_Signer->edit_privattr.digt, 0, pEdit_Signer->edit_privattr.containLen);
				memset(pEditReason->edit_privattr.digt, 0, pEditReason->edit_privattr.containLen);
			}
			else
			{
				Com_SpcDlgDeskbox("��¼����ʧ��!",0,pDesk,1,NULL,NULL,150);
			}
			
			gpEdit_Code->view_attr.curFocus = 1;
			pEdit_Signer->view_attr.curFocus = 0;
			pEditReason->view_attr.curFocus = 0;
			Grap_ChangeInputMode(gpEdit_Code);
		}
		else
		{
			//��������
			Grap_ChangeFocus(gpEdit_Code, 1);//�����л�����
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
	case BUTTON_ID_4://����
		if(OpCom_CheckBarcode(gpEdit_Code->edit_privattr.digt,TYPE_BARCODE) == -1)
		{
			Com_SpcDlgDeskbox("�˵���ʽ����!",0,pDesk,1,NULL,NULL,100);
			return RETURN_REDRAW;
		}

		/*if(strcmp(gau8PhotoCode, gpEdit_Code->edit_privattr.digt) == 0)
		{
			if(DLG_CONFIRM_EN != Com_SpcDlgDeskbox("���浥������,�Ƿ���������?",6, pDesk, 3, NULL, NULL, 3000))
			{
				return RETURN_REDRAW;
			}
		}*/
		OpComm_Photo(pDesk, 0, gpEdit_Code->edit_privattr.digt);
		break;
	case BUTTON_ID_20://Ԥ��
		OpComm_Photo(pDesk, 1, gau8PhotoCode);
		break;
	default:
		break;
	}
	
	return RETURN_REDRAW;
}
	
S32 Op_QianShou_BtnDestroy(void *pView)
{
	tGRAPBUTTON *pButton;
	
	pButton = (tGRAPBUTTON *)pView;
	free((void *)pButton);
	
	return SUCCESS;
}

S32 QianShou_ComboboxGetStr(void *pView,U16 i, U8 *pStr)
{
	tGRAPCOMBOBOX	*pCombox;
	
	pCombox = (tGRAPCOMBOBOX *)pView;
	
	if(pCombox->view_attr.id == COMBOBOX_ID_1)
	{
		if(i == 0)
		{
			strcpy(pStr, "����ǩ��");
		}
		else
		{
			strcpy(pStr, "�쳣ǩ��");
		}
	}
	else if(pCombox->view_attr.id == COMBOBOX_ID_2)
	{
		strcpy(pStr, "����");
	}
	
	return SUCCESS;
}

S32 QianShou_ComboxEnter(void *pView, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPCOMBOBOX	*pCombox;
	tGRAPCOMBOBOX	*pCombox2;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	
	pCombox = (tGRAPCOMBOBOX *)pView;
	pDesk   = (tGRAPDESKBOX *)pCombox->pParent;
	if(pCombox->view_attr.id == COMBOBOX_ID_1)
	{
		if(pCombox->list_opened == 1)
		{
			gtOpInfoGlb.tempIndex = i;
			if(i == 0)//����ǩ��
			{
				pBtn = (tGRAPBUTTON *)Grap_GetViewByID(pDesk, BUTTON_ID_13);
				pBtn->view_attr.enable = 0;
				
				pBtn = (tGRAPBUTTON *)Grap_GetViewByID(pDesk, BUTTON_ID_14);
				pBtn->view_attr.enable = 0;
				
				pCombox2 = (tGRAPCOMBOBOX *)Grap_GetViewByID(pDesk, COMBOBOX_ID_2);
				pCombox2->view_attr.enable = 0;
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk, EDIT_ID_3);
				pEdit->view_attr.enable = 0;
			}
			else//�쳣ǩ��
			{
				pBtn = (tGRAPBUTTON *)Grap_GetViewByID(pDesk, BUTTON_ID_13);
				pBtn->view_attr.enable = 1;
				pBtn->view_attr.curFocus = 0;
				
				pBtn = (tGRAPBUTTON *)Grap_GetViewByID(pDesk, BUTTON_ID_14);
				pBtn->view_attr.enable = 1;
				pBtn->view_attr.curFocus = 0;
				
				pCombox2 = (tGRAPCOMBOBOX *)Grap_GetViewByID(pDesk, COMBOBOX_ID_2);
				pCombox2->view_attr.enable = 1;
				pCombox2->view_attr.curFocus = 0;
				
				pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk, EDIT_ID_3);
				pEdit->view_attr.enable = 1;
				pEdit->view_attr.curFocus = 0;
			}
			Grap_ChangeFocus(pCombox, 1);//�����л�����
		}
	}
	else if(pCombox->view_attr.id == COMBOBOX_ID_2)
	{
		if(pCombox->list_opened == 1)
		{
			Grap_ChangeFocus(pCombox, 1);//�����л�����
		}
	}
	
	return SUCCESS;
}

S32 QianShou_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX	*pDesk;
	tEXIST_CHECK	*pExist;
	
	pExist = Rec_GetExistCodeByIdx(i);
	if(pExist != NULL)
	{
	hyUsbPrintf("other str = %s \r\n", pExist->other);
		pDesk = (tGRAPDESKBOX *)pLister->pParent;
		OpComm_Photo(pDesk, 2, pExist->code);
	}
	else
	{
		Com_SpcDlgDeskbox("��ȡ��Ϣʧ��!",0,pDesk,1,NULL,NULL,150);
	}
	
	return RETURN_REDRAW;
}


/*-----------------------------------------------
*����:Operation_QianShou_Desk
*����:�ɼ�ǩ��
*����:
*����:
*-----------------------------------------------*/
int Operation_QianShou_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	tGRAPCOMBOBOX	*pComBox;
	tGRAPLIST		*pList;
	int				i,idx=0;
	
	OpComm_PhotoBufInit();
	
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&QianShou_ViewAttr[idx++], "���ǩ��");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_ALL;
	pDesk->inputMode = 1;
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_3;
	
	for (i=0; i<10; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &QianShou_ViewAttr[idx++], &QianShou_BtnPriv[i]);
		if(NULL == pBtn ) return -1;
		if(i < 4)
		{
			pBtn->pressEnter = Op_QianShou_BtnPress;
		}
		else if(pBtn->view_attr.id == BUTTON_ID_20)//��ƬԤ��
		{
			gpPhotoBtn = pBtn;
			pBtn->pressEnter = Op_QianShou_BtnPress;
			pBtn->destroy = Op_QianShou_BtnDestroy;
		} 
	}
	
	//�˵���   ǩ����
	for (i=0; i<2; i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &QianShou_ViewAttr[idx++], &QianShou_EditPriv[i]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(NULL == pEdit ) return -1;
		if(i == 0)//�˵���
		{
			gpEdit_Code = pEdit;
		}
	}
	
	for(i = 0; i < 2; i++)
	{
		pComBox = Grap_InsertCombobox(pDesk, &QianShou_ViewAttr[idx++], &QianShou_ComAttr[0]);
		if(NULL == pComBox ) return -1;
		if(i == 0)
		{
			pComBox->comboPriv.total_num = 2;
		}
		else
		{
			pComBox->comboPriv.total_num = 1;
		}
		pComBox->comboxgetstr = QianShou_ComboboxGetStr;
		pComBox->comboxenter  = QianShou_ComboxEnter;
	}
	
	//��ע
	pEdit = Grap_InsertEdit(pDesk, &QianShou_ViewAttr[idx++], &QianShou_EditPriv[2]);
	pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
	if(NULL == pEdit ) return -1;
	
	pList = Op_CommonInsertListShow(pDesk, 10, 180, 220, 100);
	pList->enter = QianShou_ListEnter;
	
	t9_insert(pDesk,NULL);
	edit_creat_panel(pDesk, 6, NULL,NULL);
	edit_auto_change_input_mode(pDesk,0);
	edit_change_input_mode(pDesk,0,(pDesk->inputMode==4));
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}

/*=============================================��¼��ѯʱ��ʾ����Ϣ===============================================*/
const U8* const opType_Qianshou[]=//
{
	"״̬:",
	
	"��������:",
	"ɨ������:",
	"Ա�����:",
	"ɨ��ʱ��:",
	
	"ǩ����:",
	"ǩ�����:",
	"��ע:",
	"�˵���:"
};


/*---------------------------------------------------------------
*����: QianShou_SearchDisplay
*����: ��ʾ��¼��ѯ�������ַ���
*����: pData:��ѯ���Ľ��
       total:��ѯ��������
       pDispStr:(out)��ʾ��buf
       dispBufLen:pDispStr����󳤶�
*����: 
----------------------------------------------------------------*/
int QianShou_SearchDisplay(void *p)
{
	U8 *str[]={"δ��", "�Ѵ�", "��ɾ", "δ��", "δ��"};
	U8 *pDispStr, reason[50];
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
		saomiao = &pData[i].uData.saomiao;
		
		if(saomiao->signType == 0)//����ǩ��
		{
			strcpy(reason, "����ǩ��");
		}
		else
		{
			//DownLoad_GetReasonByCode(saomiao->cmt_ids, reason, sizeof(reason));
			strcpy(reason, "����");
		}
		
		sprintf(&pDispStr[len], "%s%s\r\n%s%d\r\n%s%s\r\n%s%s\r\n%s%s\r\n\
%s%s\r\n%s%s\r\n%s%s\r\n%s%s\r\n-----------------------\r\n",
							opType_Qianshou[0], str[stat],
							opType_Qianshou[1], pComm->opType,
							opType_Qianshou[2], pComm->scanStat,
							opType_Qianshou[3], pComm->userId,
							opType_Qianshou[4], pComm->scanTime,
							
							opType_Qianshou[5], saomiao->customer,
							opType_Qianshou[6], reason,
							opType_Qianshou[7], saomiao->cmt_inf,
							opType_Qianshou[8], pComm->code
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


/*===============================================================================================*/
/*
����ǩ��

http://www.hyco.cc/11/sendpic.asp    �ϴ�ͼƬ
�鿴 http://www.hyco.cc/pic.asp
*/
/*---------------------------------------------------------------
*����: QianShou_FillData
*����: �����������
*����: pSend:���ڷ��͵�buf
       bufLen:pSend�ĳ���
       pNode:׼��������͵�����
       cnt:׼������ĸ���
       pUrl:�����url
*����: 0:�ɹ�   -1:����ͨѶ�쳣
----------------------------------------------------------------*/
int QianShou_FillData(void *p)
{
	struct protocol_http_request tRequest;
	int			dataLen,ret=-1;
	int 		i,bufLen, cnt,picsize;
	tOP_SENDINFO *pOpCb;
	tQUE_NODE 	*pNode;
	tDATA_COMM	*pComm;
	tSAOMIAO	*saomiao;
	U8 *pSend, *pUrl, *pGbk, *pTemp,*pE;
	U8	buf[20],da[10];
	U8  *encode;
	int encodelen;
	
	pOpCb = (tOP_SENDINFO *)p;
	pSend = pOpCb->pSend;
	bufLen= pOpCb->bufLen;
	pNode = (tQUE_NODE *)( pOpCb->pNode);
	cnt   = pOpCb->cnt;
	pUrl  = pOpCb->pUrl;

	//���������ϴ�	
	//�������ͷ
	pComm = &(pNode[0].tData.tComm);
	pTemp = (U8 *)hsaSdram_DecodeBuf();
	encode = pTemp+0x20000;
	picsize = PicStore_Read(pComm->code,pTemp,0xFC00);
	if(picsize > 0 && picsize < 0x7000)
	{
		//base64����
		encodelen = 0x20000;
		ret = mbase64_encode(pTemp,picsize,encode,&encodelen);
	hyUsbPrintf("picsize base64 = %d   %d \r\n",ret, encodelen);	
		if(0 != ret)
		{
			strcpy(encode, "no image");
		}
	}
	else
	{
		strcpy(encode, "no image");
	}
	
	sprintf(pSend,"<?xml version=\"1.0\" encoding=\"UTF-8\"?><req op=\"sendpic\"><code>%s</code><pic>%s</pic></req>",
					pComm->code,encode);
	
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
	
	cnt = http_send_request("www.hyco.cc/11/sendpic.asp", bufLen, &tRequest);
	if(cnt > 0)
	{
		pTemp = strstr(pSend, "<result>0");
		if(pTemp != NULL)
		{
			ret = 0;
		}
	}

	return ret;
}

#endif //�ƴ��汾