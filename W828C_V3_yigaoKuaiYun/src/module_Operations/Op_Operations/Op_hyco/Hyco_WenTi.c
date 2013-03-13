#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Op_Common.h"
#include "Op_Struct.h"
#include "Op_BarRuler.h"
#include "Rec_Info.h"


#ifdef VER_CONFIG_HYCO	//�ƴ��汾

static const U8 *const Reason_Str[]=
{
	"��ַ����",	"�浥����",	"���޴���",	"����",	"�ܸ�������",	"�ͻ�����",	"�����",
	"Υ��Ʒ",	"δ��/�ٴ����ͷ�",	"�쳣����",	"Ӧ��δ����",	"����",	"����"
};

/*==========================================���������==========================================*/
const tGrapViewAttr WenTi_ViewAttr[] =
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
	
	{BUTTON_ID_10,	   5,  35 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//ԭ��
	{BUTTON_ID_11,	   5,  60 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//�˵���
	{BUTTON_ID_12,	   5,  85 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//��ע
	
	{COMBOBOX_ID_1,	   55, 35,158,22,   55,35,220,57, 0,0,0xffff,0,1,1,1,0},//ԭ��
	{EDIT_ID_1,	   	   55, 60,172,22,   55,60,227,82, 0,0,0xffff,0,1,0,1,0},//�˵���
	{EDIT_ID_2,	   	   55, 85,172,22,   55,85,227,107, 0,0,0xffff,0,1,0,1,0},
};

const tGrapButtonPriv WenTi_BtnPriv[]=
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
	
	{"ԭ  ��",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"�˵���",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"��  ע",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
};

const tGrapEditPriv WenTi_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,21,  1, BARCODE_LEN-1,		0,0,2,0,0,1,  50,50,0,  0,1,1,1},//�˵���
	{NULL,0x0,0,0,2,4,21,  1, 40,				0,0,0,0,0,1,  50,50,0,  0,0,5,0},//��ע
};

const tComboboxPriv WenTi_ComAttr[]=
{
	{0,6,0,0,0,0,    1,138,16,158,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//���䷽ʽ
};


S32 WenTi_ComboboxGetStr(void *pView,U16 i, U8 *pStr)
{
	tGRAPCOMBOBOX	*pCombox;
	
	pCombox = (tGRAPCOMBOBOX *)pView;
	
	if(pCombox->comboPriv.total_num == 0)
	{
		strcpy(pStr, "�����ԭ���б�");
	}
	else if(i < pCombox->comboPriv.total_num)
	{
		strcpy(pStr, Reason_Str[i]);
		//strcpy(pStr, Op_DownLoad_GetProblemName(i));
	}
	
	return SUCCESS ;
}

S32 WenTi_ComboxEnter(void *pView, U16 i)
{
	tGRAPCOMBOBOX	*pCombox;
	
	pCombox = (tGRAPCOMBOBOX *)pView;
	
	if(i < pCombox->comboPriv.total_num)
	{
		if(pCombox->list_opened == 1)
		{
			gtOpInfoGlb.tempIndex = i;
			Grap_ChangeFocus(pCombox,1);
		}
	}
	
	return SUCCESS;
}

int WenTi_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT		*pEdit;
	tDATA_INFO 		Recd;
	int ret;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
		pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_2);
		
		if(pEdit == NULL || gpEdit_Code==NULL)
		{
			Com_SpcDlgDeskbox("�ؼ���Ϣ�쳣!",0,pDesk,1,NULL,NULL,100);
			return RETURN_REDRAW;
		}
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://ȷ��
			if(strlen(gpEdit_Code->edit_privattr.digt) == 0)
			{
				break;
			}
			/*if(Op_DownLoad_GetProblemCnt() == 0)
			{
				Com_SpcDlgDeskbox("���ȸ���ԭ����Ϣ!",0,pDesk,1,NULL,NULL,150);
				return RETURN_REDRAW;
			}*/
			
			//�жϵ���
			ret = OpCom_CheckBarcode(gpEdit_Code->edit_privattr.digt,TYPE_BARCODE);
			if(ret == 0 || ret == 1)
			{
				//��鵥���Ƿ����
				ret = Op_ComCheckExist(pDesk, gpEdit_Code->edit_privattr.digt, OpCom_GetCurOpType());
				if(ret == 0)//�Ѵ���,δɾ��
				{
					
				}
				else if(ret == 1)//�Ѵ���,��ɾ��
				{
					
				}
				else //������
				{
					//����¼�Ĺ�������
					OpCom_FillCommInfo(&Recd, gpEdit_Code->edit_privattr.digt, NULL, RECORD_UNSEND);

					//ҵ�񲿷�
					//strcpy(Recd.uData.saomiao.cmt_ids, Op_DownLoad_GetProblemCode(gtOpInfoGlb.tempIndex));
					strcpy(Recd.uData.saomiao.cmt_ids, Reason_Str[gtOpInfoGlb.tempIndex]);
					strcpy(Recd.uData.saomiao.cmt_inf,pEdit->edit_privattr.digt);
					
					//д��¼
					ret = Rec_WriteNewRecode(&Recd, NULL);
					if (ret == 0)
					{
						Op_CommonListShow_Add(gpEdit_Code->edit_privattr.digt);
					}
					else
					{
						Com_SpcDlgDeskbox("��¼����ʧ��!",0,pDesk,1,NULL,NULL,100);
					}
				}
				memset(gpEdit_Code->edit_privattr.digt, 0, gpEdit_Code->edit_privattr.containLen);
				memset(pEdit->edit_privattr.digt, 0, pEdit->edit_privattr.containLen);
				
				gpEdit_Code->view_attr.curFocus = 1;
				pEdit->view_attr.curFocus = 0;
				Grap_ChangeInputMode(gpEdit_Code);
			}
			else
			{
				Com_SpcDlgDeskbox("�˵���ʽ����!",0,pDesk,1,NULL,NULL,100);
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
		default:
			break;
		}
	}
	
	return RETURN_REDRAW;
}

/*-----------------------------------------------
*����:Operation_WenTi_Desk
*����:�ռ�
*����:
*����:
*-----------------------------------------------*/
int Operation_WenTi_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	tGRAPCOMBOBOX	*pComBox;
	int				i,idx=0;
	
	gtOpInfoGlb.tempIndex = 0;
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&WenTi_ViewAttr[idx++], "�����");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_ALL;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_2;
	
	for (i=0; i<6; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &WenTi_ViewAttr[idx++], &WenTi_BtnPriv[i]);
		if(NULL == pBtn ) return -1;
		
		if(i < 3)
		{
			pBtn->pressEnter = WenTi_BtnPress;
		}
	}
	
	pComBox = Grap_InsertCombobox(pDesk, &WenTi_ViewAttr[idx++], &WenTi_ComAttr[0]);
	if(NULL == pComBox ) return -1;
	pComBox->comboPriv.total_num = sizeof(Reason_Str)/sizeof(U8 *);//Op_DownLoad_GetProblemCnt();//
	pComBox->comboxgetstr = WenTi_ComboboxGetStr;
	pComBox->comboxenter  = WenTi_ComboxEnter;
	
	for(i = 0; i < 2; i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &WenTi_ViewAttr[idx++], &WenTi_EditPriv[i]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(NULL == pEdit ) return -1;
		if(i == 0)
		{
			gpEdit_Code = pEdit;
		}
	}
	
	Op_CommonInsertListShow(pDesk, 10, 115, 220, 160);
	
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

/*-----------------------------------------��¼��ѯ��ʾ------------------------------------------------*/
const U8* const opType_WenTi[]=//�����
{
	"״̬:",
	
	"��������:",
	"ɨ������:",
	"Ա�����:",
	//"Ա������:",
	"ɨ��ʱ��:",
	
	"�쳣ԭ��:",
	"��ע:",
	"����:",
};


/*---------------------------------------------------------------
*����: WenTi_SearchDisplay
*����: ��ʾ��¼��ѯ�������ַ���
*����: pData:��ѯ���Ľ��
       total:��ѯ��������
       pDispStr:(out)��ʾ��buf
       dispBufLen:pDispStr����󳤶�
*����: 
----------------------------------------------------------------*/
int WenTi_SearchDisplay(void *p)
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
%s%s\r\n%s%s\r\n-----------------------\r\n",
							opType_WenTi[0], str[stat],
							opType_WenTi[1], pComm->opType,
							opType_WenTi[2], pComm->scanStat,
							opType_WenTi[3], pComm->userId,
							opType_WenTi[4], pComm->scanTime,
							
							opType_WenTi[5], saomiao->cmt_ids,
							opType_WenTi[6], saomiao->cmt_inf,
							opType_WenTi[7], pComm->code
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
*����: WenTi_FillData
*����: �����������
*����: pSend:���ڷ��͵�buf
       pNode:׼��������͵�����
       cnt:׼������ĸ���
       pUrl:�����url
*����: 0:�ɹ�   -1:����ͨѶ�쳣
----------------------------------------------------------------*/
int WenTi_FillData(void *p)
{
	struct protocol_http_request tRequest;
	int			dataLen,ret=-1;
	int 		i,bufLen, cnt;
	tOP_SENDINFO *pOpCb;
	tQUE_NODE 	*pNode;
	tDATA_COMM	*pComm;
	tSAOMIAO	*saomiao;
	U8 *pSend, *pUrl, *pGbk, *pTemp,*pE;
	U8	buf[20],da[10];
	
	pOpCb = (tOP_SENDINFO *)p;
	pSend = pOpCb->pSend;
	bufLen= pOpCb->bufLen;
	pNode = (tQUE_NODE *)( pOpCb->pNode);
	cnt   = pOpCb->cnt;
	pUrl  = pOpCb->pUrl;

return 0;//test
	
	//���������ϴ�	
	//�������ͷ
	pComm = &(pNode[0].tData.tComm);
	dataLen = NetReq_FillHead_DHL(pSend, pComm->opType, NULL);

	for(i = 0; i < cnt; i++)
	{
		pComm = &(pNode[i].tData.tComm);
		memcpy(da, pComm->scanTime, 8);
		da[8] = 0;
		saomiao = &(pNode[i].tData.uData.saomiao);

		sprintf(&pSend[dataLen], "<data>%02d%-14s%-14s%-1s%-1s%-16s%-15s%-8s%-1s%-7s%-8s%-1s%-10s%-10s</data>",
					pComm->opType," ",pComm->scanTime," "," ",pComm->code,pComm->userId,da,
					" ",saomiao->cmt_ids," ",saomiao->cmt_inf," "," ");
		
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

hyUsbPrintf("ZhuangDai send = ");
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
	hyUsbPrintf("ZhuangDai recv = %s \r\n",pSend);	
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
hyUsbPrintf("ZhuangDai ret = %d \r\n",ret);	
	
	return ret;
}

#endif //�ƴ��汾