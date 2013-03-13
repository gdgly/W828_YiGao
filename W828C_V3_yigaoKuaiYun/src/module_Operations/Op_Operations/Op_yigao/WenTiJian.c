#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Op_Common.h"
#include "Op_Struct.h"
#include "Op_BarRuler.h"
#include "Rec_Info.h"


#ifdef VER_CONFIG_HYCO	//�ƴ��汾

extern U8 gu8BtErrorFlag;
extern U8 BtTmkdStr[50];

static const U8 *const Reason_Str[]=
{
	"��ַ����",	"�浥����",	"���޴���",	"����",	"�ܸ�������",	"�ͻ�����",	"�����",
	"Υ��Ʒ",	"δ��/�ٴ����ͷ�",	"�쳣����",	"Ӧ��δ����",	"����",	"����"
};

/*==========================================���������==========================================*/
const tGrapViewAttr YGWenTi_ViewAttr[] =
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
	
	{BUTTON_ID_10,	   5,  35 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//ԭ��
	{BUTTON_ID_11,	   5,  60 ,72, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//�˵���
	
	{COMBOBOX_ID_1,	   78, 35,160,22,   55,35,238,57, 0,0,0xffff,0,1,1,1,0},//ԭ��
	{EDIT_ID_1,	   	   78, 60,160,22,   55,60,238,82, 0,0,0xffff,0,1,0,1,0},//�˵���
};

const tGrapButtonPriv YGWenTi_BtnPriv[]=
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
	
	{"��������",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"�� �� ��",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
};

const tGrapEditPriv YGWenTi_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,21,  1, BCHID_LEN-1,		0,0,2,0,0,1,  50,50,0,  0,1,1,1},//�˵���
};

const tComboboxPriv YGWenTi_ComAttr[]=
{
	{0,6,0,0,0,0,    1,140,16,160,18,0,50,50,   FONTSIZE8X16,0,  0,0,0xFFE0,0x0,0xf800,0xffff,0,  0,2,18,  0,   0,0,0,0},//���䷽ʽ
};


S32 YGWenTi_ComboboxGetStr(void *pView,U16 i, U8 *pStr)
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

S32 YGWenTi_ComboxEnter(void *pView, U16 i)
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

int YGWenTi_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tDATA_INFO 		Recd;
	int ret;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
		
		
		if(gpEdit_Code==NULL)
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
				
				gpEdit_Code->view_attr.curFocus = 1;
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
*����:Operation_YGWenTi_Desk
*����:�ռ�
*����:
*����:
*-----------------------------------------------*/
int Operation_YGWenTi_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	tGRAPCOMBOBOX	*pComBox;
	int				i,idx=0;
	
	Op_CommonList_Clear();
	
	gtOpInfoGlb.tempIndex = 0;
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&YGWenTi_ViewAttr[idx++], "�����");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_ALL;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	for (i=0; i<5; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &YGWenTi_ViewAttr[idx++], &YGWenTi_BtnPriv[i]);
		if(NULL == pBtn ) return -1;
		
		if(i < 3)
		{
			pBtn->pressEnter = YGWenTi_BtnPress;
		}
	}
	
	pComBox = Grap_InsertCombobox(pDesk, &YGWenTi_ViewAttr[idx++], &YGWenTi_ComAttr[0]);
	if(NULL == pComBox ) return -1;
	pComBox->comboPriv.total_num = sizeof(Reason_Str)/sizeof(U8 *);//Op_DownLoad_GetProblemCnt();//
	pComBox->comboxgetstr = YGWenTi_ComboboxGetStr;
	pComBox->comboxenter  = YGWenTi_ComboxEnter;
	
	for(i = 0; i < 1; i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &YGWenTi_ViewAttr[idx++], &YGWenTi_EditPriv[i]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(NULL == pEdit ) return -1;
		if(i == 0)
		{
			gpEdit_Code = pEdit;
		}
	}
	
	Op_CommonInsertListShow(pDesk, 10, 90, 220, 190);
	
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
const U8* const opType_YGWenTi[]=//�����
{
	"״̬:",
	
	"��������:",
	"ɨ������:",
	"Ա�����:",
	//"Ա������:",
	"ɨ��ʱ��:",
	
	"�쳣ԭ��:",
	"����:",
};


/*---------------------------------------------------------------
*����: YGWenTi_SearchDisplay
*����: ��ʾ��¼��ѯ�������ַ���
*����: pData:��ѯ���Ľ��
       total:��ѯ��������
       pDispStr:(out)��ʾ��buf
       dispBufLen:pDispStr����󳤶�
*����: 
----------------------------------------------------------------*/
int YGWenTi_SearchDisplay(void *p)
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
							opType_YGWenTi[0], str[stat],
							opType_YGWenTi[1], pComm->opType,
							opType_YGWenTi[2], pComm->scanStat,
							opType_YGWenTi[3], pComm->userId,
							opType_YGWenTi[4], pComm->scanTime,
							
							opType_YGWenTi[5], saomiao->cmt_ids,
							opType_YGWenTi[6], pComm->code
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
*����: YGWenTi_FillData
*����: �����������
*����: pSend:���ڷ��͵�buf
       pNode:׼��������͵�����
       cnt:׼������ĸ���
       pUrl:�����url
*����: 0:�ɹ�   -1:����ͨѶ�쳣
----------------------------------------------------------------*/
int YGWenTi_FillData(void *p)
{
	struct protocol_http_request tRequest;
	int			dataLen,ret=-1;
	int 		i,bufLen, cnt,len;
	tOP_SENDINFO *pOpCb;
	tQUE_NODE 	*pNode;
	tDATA_COMM	*pComm;
	tSAOMIAO	*saomiao;
	U8 *pSend, *pUrl, *pGbk, *pTemp,*pTmp,url[256];
	U8	buf[20],da[10];
	
	pOpCb = (tOP_SENDINFO *)p;
	pSend = pOpCb->pSend;
	bufLen= pOpCb->bufLen;
	pNode = (tQUE_NODE *)( pOpCb->pNode);
	cnt   = pOpCb->cnt;
	pUrl  = pOpCb->pUrl;

	pComm = &(pNode[0].tData.tComm);
	saomiao = &(pNode[0].tData.uData.saomiao);
	sprintf(pSend, "StrJson=[Request][data][no Code='14'][a]%s[/a][b]%s[/b][c]%s[/c][d]%s[/d][e]%s[/e][f]%s[/f][/no][/data][/Request]&Checking_Key=002",
				pComm->userId,pComm->scanStat,gtHyc.MachineCode,pComm->code,saomiao->cmt_ids,pComm->scanTime);
	
	dataLen = strlen(pSend);
		
hyUsbPrintf("dataLen = %d \r\n",dataLen);	
	if(dataLen <= 0)
	{
		return -1;
	}
#if 1	
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
    tRequest.content_type   = "application/x-www-form-urlencoded";
    tRequest.range          = NULL;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;

hyUsbPrintf("ZhuangDai send = ");
hyUsbMessageByLen(pSend, strlen(pSend));
hyUsbPrintf("\r\n");

	sprintf(url,"%s/RequestInvoke",gtHyc.url);
hyUsbPrintf("url = %s\r\n",url);
	cnt = http_send_request(url, bufLen, &tRequest);
	if(cnt > 0)
	{
		pGbk = (U8 *)hsaSdram_DecodeBuf();
		
		cnt = Net_DecodeData(pSend,cnt,pGbk);
		if(cnt > 0)
		{
			cnt = html_decode(pGbk, cnt, pSend);
					
			hyUsbPrintf("recv (front)\r\n");
			hyUsbMessageByLen(pSend, cnt);
			hyUsbPrintf("\r\n");
			
			pTmp = strstr(pSend,"<result>");
			pSend = pTmp + 8;
			if(memcmp(pSend, "true", 4) == 0)
			{
				
			}
			else if(memcmp(pSend,"false",5) == 0)
			{
				memset(BtTmkdStr,0,50);
				pTmp = strstr(pSend,"<reason>");
				pSend = pTmp + 8;
				pTmp = strstr(pSend,"</reason>");
				if(pTmp != NULL)
				{
					gu8BtErrorFlag = 1;
					len = (int)(pTmp-pSend);
					if(len >= 50) len = 50;
					memcpy(BtTmkdStr,pSend,len);
				}
			}
			ret = 0;
		}
	}
hyUsbPrintf("ZhuangDai ret = %d \r\n",ret);	
	
	return ret;
}

#endif //�ƴ��汾