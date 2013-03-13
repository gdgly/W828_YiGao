/*----------------------------------

-----------------------------------*/

#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Op_Common.h"
#include "Op_Struct.h"
#include "Op_BarRuler.h"
#include "Rec_Info.h"


#ifdef VER_CONFIG_HYCO	

U8	gau8Sub_MainCode[32];
U8  gu8SubSequency;//�ӵ��Ƿ���������   0:��   1:��
U8  gu8SubFinal;   //�ӵ�¼���Ƿ����   0:��   1:��
int TotalSubCnt, ScanSubCnt;//�ӵ�����  ��ɨ����


void SubCode_Init()
{
	gu8SubSequency = 0;
	gu8SubFinal    = 0;
	TotalSubCnt    = 0;
	ScanSubCnt     = 0;
	memset(gau8Sub_MainCode,0,sizeof(gau8Sub_MainCode));
}

void SubCode_SetMainCode(U8 *pMainCode)
{
	int len = strlen(pMainCode);
	
	if(len >= 32) len = 31;
	memcpy(gau8Sub_MainCode, pMainCode, len);
}

U8 *SubCode_GetMainCode()
{
	return gau8Sub_MainCode;
}

void SubCode_SetSubTotal(int subTotal)
{
	TotalSubCnt = subTotal;
}

int SubCode_GetSubTotal()
{
	return TotalSubCnt;
}

int SubCode_SetSubScan(int scanTotal)
{
	ScanSubCnt = scanTotal;
}

int SubCode_GetSubScan()
{
	return ScanSubCnt;
}

void SubCode_SetFinalState(U8 final)
{
	gu8SubFinal = final;
}

U8 SubCode_GetFinalState()
{
	return gu8SubFinal;
}

//��ȡ��������  -1ʧ��
int SubCode_GetBase(U8 *pCode, int *addCnt)
{
	int base=-1;
	int i, len, cnt, num=1;
	
	cnt = *addCnt;
	if(cnt <= 0) return -1;
	
	do
	{
		num++;
		cnt /= 10;
		
	}while(cnt);
	
	*addCnt = num;
	
	len = strlen(pCode);
	len = (len >= num)? (len-num):0;
	
	//�ж��Ƿ�����
	if(-1 == OpCom_IfDigit(pCode+len))
	{
		return -1;
	}
	
	base = atoi(pCode+len);
	
	return base;
}

int SubCode_GeneratorSubCode(U8 *pDst, U8 *pPart, int val, int cnt)
{
	int len;
	U8  c;
	
	len = strlen(pPart)+cnt;
	
	strcpy(pDst, pPart);
hyUsbPrintf("============ len = %d   code = %s \r\n", len, pDst);	
	
	pDst[len--] = 0;
	do
	{
		c = (val%10)+'0';
		
		pDst[len--] = c;
		val /= 10;
		
		hyUsbPrintf("cccc = %c val = %d    dst = %s \r\n", c, val, pDst);
		cnt--;
	
	}while(cnt>0);
	
	
	return 0;
}


/*==========================================�ռ�����==========================================*/
const tGrapViewAttr SubCode_ViewAttr[] =
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
	
	{BUTTON_ID_4,	   5,  35 ,90, 20,   5,35,95,55,   0,0,0,0,1,0,0,0},//�ӵ�����
	{BUTTON_ID_11,	   128,  35 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//�ӵ���
	{BUTTON_ID_12,	   5,  60 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//�ӵ���
	
	{EDIT_ID_1,	   177, 35,50,22,   0,0,0,0, 0,0,0xCE59,0,1,0,0,0},//
	{EDIT_ID_2,	   55, 60,172,22,   55,60,227,82, 0,0,0xffff,0,1,1,1,0},//�ӵ��� 21
};

const tGrapButtonPriv SubCode_BtnPriv[]=
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
	
	{"�ӵ�����",COLOR_YELLOW,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_LEFT,0, TY_UNITY_BG1_EN,NULL},//
	{"�ӵ���",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"�ӵ���",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
};

const tGrapEditPriv SubCode_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,5,  1, 5,		0,0,2,0,0,0,  50,50,0,  0,1,1,0},//�ӵ���
	{NULL,0x0,0,0,2,4,21,  1, BCHID_LEN-1,		0,0,2,0,0,1,  50,50,0,  0,1,1,1},//�ӵ���
};


int SubCode_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT 		*pEdit;
	tDATA_INFO 		Recd;
	int i, ret;
	U8	*pCode, code[32];
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
		pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_2);
		
		if(pEdit==NULL)
		{
			Com_SpcDlgDeskbox("�ؼ���Ϣ�쳣!",0,pDesk,1,NULL,NULL,100);
			return RETURN_REDRAW;
		}
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://ȷ��
			if(pEdit->view_attr.curFocus == 1)
			{
				pCode = pEdit->edit_privattr.digt;
				if(strlen(pCode) == 0)
				{
					break;
				}
				
				//�ж��Ƿ��Ѿ�ɨ�����
				if(ScanSubCnt == TotalSubCnt)
				{
					if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�ӵ���ɨ�����,�Ƿ񷵻�",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
					{
						return RETURN_QUIT;
					}
					else
					{
						break;
					}
				}
				
				//�жϵ���
				ret = OpCom_CheckBarcode(pCode,TYPE_BARCODE);
				if(ret == 0 || ret == 1)
				{
					//��鵥���Ƿ����
					if(0 != Op_ComCheckExist(pDesk, pCode, OpCom_GetCurOpType()))
					{
						
						//����¼�Ĺ�������
						OpCom_FillCommInfo(&Recd, gau8Sub_MainCode, pCode, RECORD_UNSEND);
						
						strcpy(Recd.uData.ludan.bchId, gau8Sub_MainCode);
						strcpy(Recd.uData.ludan.num, "1");
						strcpy(Recd.uData.ludan.sub_code, pEdit->edit_privattr.digt);
						hyUsbPrintf("barcode = %s sub_code = %s\r\n",Recd.uData.ludan.bchId,Recd.uData.ludan.sub_code);
						
						//д��¼
						ret = Rec_WriteNewRecode(&Recd, NULL);
						if (ret == 0)
						{
							ScanSubCnt++;
							Op_CommonListShow_Add(pCode);
							//�ж��Ƿ��Ѿ�ɨ�����
							if(ScanSubCnt == TotalSubCnt)
							{
								if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�ӵ���ɨ�����,�Ƿ񷵻�",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
								{
									return RETURN_QUIT;
								}
								else
								{
									break;
								}
							}
						}
						else
						{
							Com_SpcDlgDeskbox("��¼����ʧ��!",0,pDesk,1,NULL,NULL,100);
						}
						
					}
					memset(pEdit->edit_privattr.digt, 0, pEdit->edit_privattr.containLen);
				}
				else
				{
					Com_SpcDlgDeskbox("�˵���ʽ����!",0,pDesk,1,NULL,NULL,100);
				}
			}
			break;
		case BUTTON_ID_2://ɾ��
			if(gu8SubFinal == 0)
			{
				Op_ComDelCode(pDesk, OpCom_GetCurOpType());
			}
			else
			{
				Com_SpcDlgDeskbox("�õ������,���ɲ���!",6,pDesk,1,NULL,NULL,100);
			}
			break;
		case BUTTON_ID_3://����
			if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�Ƿ��˳��ò���?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
			{
				return RETURN_QUIT;
			}
			break;
		case BUTTON_ID_4://�ӵ�����
			gu8SubSequency = !gu8SubSequency;
			break;
		default:
			break;
		}
	}
	
	return RETURN_REDRAW;
}

S32 SubCode_BtnDraw(void *pView, U32 xy, U32 wh)
{
	tGRAPBUTTON *pButton;
	tGrapViewAttr *pview_attr;
	tGrapButtonPriv  *pbtn_privattr;
	tStrDispInfo disp_info;
	tGrapButtonBuffPic *pbtnpic;
	U16 viewX1,viewY1,viewX2,viewY2;

	pButton = (tGRAPBUTTON *)pView;
	pview_attr = &(pButton->view_attr);
	pbtn_privattr = &(pButton->btn_privattr);
	
	viewX1 = pview_attr->viewX1;
	viewY1 = pview_attr->viewY1;
	viewX2 = viewX1 + pview_attr->viewWidth;
	viewY2 = viewY1 + pview_attr->viewHeight; 
	
	Grap_ClearScreen(viewX1, viewY1, viewX2,viewY2);
	
	if(gu8SubSequency)
	{
		Grap_ShowPicture(SZ_XZ_K_EN, viewX1, viewY1);
	}
	else
	{
		Grap_ShowPicture(SZ_BX_K_EN, viewX1, viewY1);
	}
	
	if (pbtn_privattr->pName)//��ʾbutton�ϵ��ַ�
	{
		disp_info.string = pbtn_privattr->pName;

		if(pButton->state == STATE_NORMAL)
			disp_info.color = pbtn_privattr->cl_normal;
		else
			disp_info.color = pbtn_privattr->cl_focus;
		disp_info.font_size = pbtn_privattr->font_size;
		disp_info.font_type = pbtn_privattr->font_type;
		disp_info.flag = pbtn_privattr->position;
		disp_info.keyStr = NULL;
		Grap_WriteString(viewX1+25, viewY1+2, viewX2,viewY2, &disp_info);
	}
	
	return SUCCESS;
}

/*-----------------------------------------------
*����:Operation_SubCode_Desk
*����:�ӵ�ɨ��
*����:
*����:
*-----------------------------------------------*/
int Operation_SubCode_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	tDATA_INFO 		*pSub;
	int				i,idx=0;
	
	pDesk = Grap_CreateDeskbox(&SubCode_ViewAttr[idx++], "�ӵ�ɨ��");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_2;
	pDesk->editMaxId = EDIT_ID_2;
	
	for (i=0; i<6; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &SubCode_ViewAttr[idx++], &SubCode_BtnPriv[i]);
		if(NULL == pBtn ) return -1;
		
		if(i < 4)
		{
			pBtn->pressEnter = SubCode_BtnPress;
			if(i == 3)
			{
				pBtn->draw = SubCode_BtnDraw;
			}
		}
	}
	
	//�ӵ���   �ӵ���
	for (i=0; i<2; i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &SubCode_ViewAttr[idx++], &SubCode_EditPriv[i]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(NULL == pEdit ) return -1;
		if(i == 0)//�ӵ���
		{
			sprintf(pEdit->edit_privattr.digt, "%d", TotalSubCnt);
		}
	}
	
	Op_CommonInsertListShow(pDesk, 10, 90, 220, 180);
	/*
	//���ӵ����ص���ɨ�������
	//���ظ�����������ӵ�
	ScanSubCnt = RecStore_GetSubCodeByMainCode(Login_GetRight(), OpCom_GetCurOpType(), NULL, 
									gau8Sub_MainCode, (U8 *)hsaSdram_UpgradeBuf(), hsaSdram_UpgradeSize());
	
	pSub = (tDATA_INFO *)hsaSdram_UpgradeBuf();
	for(i = 0; i < ScanSubCnt; i++)
	{
		if(strcmp(pSub->tComm.code,pSub->tComm.subCode) != 0)
		{
			Rec_AddExist(pSub->tComm.subCode);
			Op_CommonListShow_Add(pSub->tComm.subCode);
		}
		pSub++;
	}
	*/
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
    Grap_DeskboxRun(pDesk);
    ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}

#endif//VER_CONFIG_HYCO