/*------------------------------------------------
�����ҵ��Ա�ɼ�
-------------------------------------------------*/
#include "hyTypes.h"
#include "Common_Widget.h"
#include "Common_Dlg.h"
#include "Op_Common.h"
#include "Op_Struct.h"
#include "Op_BarRuler.h"
#include "Rec_Info.h"

#ifdef VER_CONFIG_HYCO	//�ƴ��汾

/*==========================================�ɼ�����==========================================*/
const tGrapViewAttr PaiJianWT_ViewAttr[] =
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
	{BUTTON_ID_4,	   165,61, 70,20,    165,61,238,81,    YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//��������
	
	{BUTTON_ID_10,	   5,  35 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//�ɼ�Ա
	{BUTTON_ID_11,	   5,  60 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//��  ��
	{BUTTON_ID_12,	 140,  60 ,16, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//Kg
	{BUTTON_ID_13,	   5,  85 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//�˵���
	
	{EDIT_ID_1,	   55, 35,83,22,    55,35,138,57, 0,0,0xffff,0,1,0,1,0},//�ɼ�Ա
	{EDIT_ID_2,	   55, 60,83,22,    55,60,138,82, 0,0,0xCE59,0,1,0,1,0},//����
	{EDIT_ID_3,	   55, 85,172,22,   55,85,227,107, 0,0,0xffff,0,1,1,1,0},//�˵��� 21
	
	{EDIT_ID_20,   140,35 ,100,22,   0,0,0,0,     0,0,0xCE59,0,1,0,0,0},
};

const tGrapButtonPriv PaiJianWT_BtnPriv[]=
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
	
	{"�ռ�Ա",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"��  ��",  0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"Kg",  	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"�˵���",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
};

const tGrapEditPriv PaiJianWT_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,10,  1, EMPLOYEE_ID_LEN-1,  0,0,2,0,0,1,  50,50,0,  0,1,1,1},//����/ҵ��Ա
	{gtOpInfoGlb.weight,  0x0,0,0,2,4,9,  1, WEIGHT_LEN-1,	0,0,1,0,0,1,  0,4,0,  1,0,1, 0},//����
	{NULL,0x0,0,0,2,4,21,  1, BARCODE_LEN-1,		0,0,2,0,0,1,  50,50,0,  0,1,1,1},//�˵���
	
	{gtOpInfoGlb.employee,0x0,0,0,2,4,12,  1, EMPLOYEE_NAME_LEN-1,		0,0,0,0,0,0,  50,50,0,  1,0,0,0},//
};


int PaiJianWT_BtnPress(void *pView, U16 state)
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
		
		pEditWT = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_10);
		if(gpEdit_Employee==NULL || gpEdit_Code==NULL)
		{
			Com_SpcDlgDeskbox("�ؼ���Ϣ�쳣!",0,pDesk,1,NULL,NULL,100);
			return RETURN_REDRAW;
		}
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://ȷ��
			if(gpEdit_Employee->view_attr.curFocus == 1)
			{
				gpEdit_Employee->editenter(gpEdit_Employee);
				break;
			}
			
			if(gpEdit_Code->view_attr.curFocus == 1 || strlen(gtOpInfoGlb.weight))
			{
				pCode = gpEdit_Code->edit_privattr.digt;
				if(strlen(pCode) == 0)
				{
					Grap_ChangeFocusEnable(gpEdit_Code);
					break;
				}
				
				//�жϵ���
				ret = OpCom_CheckBarcode(pCode,TYPE_BARCODE);
				if(ret == 0 || ret == 1)
				{
					if(strlen(gtOpInfoGlb.employee) == 0)
					{
						Com_SpcDlgDeskbox("����������ȷ��Ա��!",0,pDesk,1,NULL,NULL,100);
						memset(gpEdit_Code->edit_privattr.digt, 0, gpEdit_Code->edit_privattr.containLen);
						break;
					}
					
					//�ж�����
					if((gtHyc.ParamCfg&0x02) == 0)//�Ƿ���������Ϊ��  ������
					{
						if(strlen(pEditWT->edit_privattr.digt) == 0 || strcmp(pEditWT->edit_privattr.digt, "0.00") == 0)
						{
							Com_SpcDlgDeskbox("����������!",0,pDesk,1,NULL,NULL,100);
							break;
						}
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
						strcpy(Recd.uData.saomiao.deliverer, gpEdit_Employee->edit_privattr.digt);
						strcpy(Recd.uData.saomiao.obj_wei, pEditWT->edit_privattr.digt);
						
						//д��¼
						ret = Rec_WriteNewRecode(&Recd, NULL);
						if (ret == 0)
						{
							Op_CommonListShow_Add(pCode);
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

U32 PaiJianWT_EditKeyEnd(void *pView, U8 type)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit,*pEditName;
	int ret = 0;
	
	pEdit = (tGRAPEDIT *)pView;
	
	if(pEdit->view_attr.id == EDIT_ID_1)//ҵ��Ա
	{
		if(strlen(pEdit->edit_privattr.digt) > 0)
		{
			ret = Employee_GetNameById(pEdit->edit_privattr.digt, gtOpInfoGlb.employee, sizeof(gtOpInfoGlb.employee));
		}
		
		if(ret == 0)
		{
			memset(gtOpInfoGlb.employee, 0, sizeof(gtOpInfoGlb.employee));
		}
		
		pDesk= (tGRAPDESKBOX *)pEdit->pParent;
		pEditName = (tGRAPEDIT *)Grap_GetViewByID(pDesk,EDIT_ID_20);//ID��һ��ȷ�Ϻ�
		
		if(pEditName) pEditName->draw(pEditName, 0, 0);
	}
	
	return SUCCESS ;
}

S32 PaiJianWT_EditEnter(void *pView)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPEDIT 		*pEdit;
	U8 *pCode;
	int ret;
	
	pEdit = (tGRAPEDIT *)pView;
	pDesk= (tGRAPDESKBOX *)pEdit->pParent;
	
	//�༭��û�������κ�����
	pCode = pEdit->edit_privattr.digt;
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	ret = Op_DownLoad_Employee_Desk(pDesk, 1);
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	//����û���ж�ret, ��ΪEmployee_GetIdNameByIdx�������ж�
	Employee_GetIdNameByIdx(ret, pEdit->edit_privattr.digt, pEdit->edit_privattr.containLen, gtOpInfoGlb.employee, sizeof(gtOpInfoGlb.employee));
	pEdit->edit_privattr.Pos = strlen(pEdit->edit_privattr.digt);
	
	if(ret != -1)
	{
		Grap_ChangeFocus(pEdit,1);
	}
	
	return RETURN_REDRAW;
}

/*-----------------------------------------------
*����:Operation_PaiJianWT_Desk
*����:�ռ�
*����:
*����:
*-----------------------------------------------*/
int Operation_PaiJianWT_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	int				i,idx=0;
		
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&PaiJianWT_ViewAttr[idx++], "�ɼ�����");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_2;
	
	for (i=0; i<8; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &PaiJianWT_ViewAttr[idx++], &PaiJianWT_BtnPriv[i]);
		if(NULL == pBtn ) return -1;
		
		if(i < 4)
		{
			pBtn->pressEnter = PaiJianWT_BtnPress;
			if(i == 3)
			{
				pBtn->draw = OpComm_BtnBTDraw;
			}
		}
	}
	
	for (i=0; i<4; i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &PaiJianWT_ViewAttr[idx++], &PaiJianWT_EditPriv[i]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(NULL == pEdit ) return -1;
		if(i == 0)//�ɼ�Ա
		{
			pEdit->editenter  = PaiJianWT_EditEnter;
			pEdit->editKeyEnd = PaiJianWT_EditKeyEnd;
			gpEdit_Employee = pEdit;
			//Ĭ���ǵ�¼��
			strcpy(pEdit->edit_privattr.digt, Login_GetUserId());
		}
		else if(i == 1)//����
		{
			pEdit->editTimer = BT_WeightEditTimer;
		}
		else if(i == 2)//�˵���
		{
			gpEdit_Code = pEdit;
		}
		else if(i == 3)
		{
			strcpy(pEdit->edit_privattr.digt, Login_GetName());
		}
	}
	
	Op_CommonInsertListShow(pDesk, 10, 115, 220, 160);
	
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
const U8* const opType_PaiJianWT[]=//�ռ�
{
	"״̬:",
	
	"��������:",
	"ɨ������:",
	"Ա�����:",
	"ɨ��ʱ��:",
	
	"�ɼ�Ա:",
	"����",
	"����:",
};


/*---------------------------------------------------------------
*����: PaiJianWT_SearchDisplay
*����: ��ʾ��¼��ѯ�������ַ���
*����: pData:��ѯ���Ľ��
       total:��ѯ��������
       pDispStr:(out)��ʾ��buf
       dispBufLen:pDispStr����󳤶�
*����: 
----------------------------------------------------------------*/
int PaiJianWT_SearchDisplay(void *p)
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
%s%s\r\n%s%s\r\n%s%s\r\n-----------------------\r\n",
							opType_PaiJianWT[0], str[stat],
							opType_PaiJianWT[1], pComm->opType,
							opType_PaiJianWT[2], pComm->scanStat,
							opType_PaiJianWT[3], pComm->userId,
							opType_PaiJianWT[4], pComm->scanTime,
							
							opType_PaiJianWT[5], saomiao->deliverer,
							opType_PaiJianWT[6], saomiao->obj_wei,
							opType_PaiJianWT[7], pComm->code
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
*����: PaiJianWT_FillData
*����: �����������
*����: pSend:���ڷ��͵�buf
       pNode:׼��������͵�����
       cnt:׼������ĸ���
       pUrl:�����url
*����: 0:�ɹ�   -1:����ͨѶ�쳣
----------------------------------------------------------------*/
int PaiJianWT_FillData(void *p)
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

		sprintf(&pSend[dataLen], "<data>%02d%-14s%-14s%-1s%-1s%-16s%-15s%-8s%-1s%-7s%-8s%-1d%-10s%-10s</data>",
					pComm->opType,saomiao->deliverer,pComm->scanTime," "," ",pComm->code,pComm->userId,da,
					" ",saomiao->obj_wei," ",saomiao->picflag," "," ");
		
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

hyUsbPrintf("PaiJianWT send = ");
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
	hyUsbPrintf("PaiJianWT recv = %s \r\n",pSend);	
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
hyUsbPrintf("PaiJianWT ret = %d \r\n",ret);	
	
	return ret;
}

#endif //�ƴ��汾