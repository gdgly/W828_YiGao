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
const tGrapViewAttr PaiJian_ViewAttr[] =
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
	{BUTTON_ID_4,	   80,85, 73,34,    80,85,153,119,    YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	
	{BUTTON_ID_10,	   5,  35 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//�ɼ�Ա
	{BUTTON_ID_11,	   5,  60 ,48, 22,   0,0,0,0,   0,0,0,0,1,0,0,0},//�˵���
	
	{BUTTON_ID_20,	   155,85,80,60,  145,85,235,145, YD_Z_P_EN,YD_Z_P_EN,0,     0,1,0,0,0},//��Ƭ���
	
	{EDIT_ID_1,	   55, 35,83,22,    55,35,138,57, 0,0,0xffff,0,1,0,1,0},//�ɼ�Ա
	{EDIT_ID_2,	   55, 60,172,22,   55,60,227,82, 0,0,0xffff,0,1,1,1,0},//�˵���
	//{EDIT_ID_3,	    5,120,125,22,    5,120,130,142,0,0,0xffff,0,1,0,1,0},//ǩ����
	
	{EDIT_ID_20,   140,35 ,100,22,   0,0,0,0,     0,0,0xCE59,0,1,0,0,0},
};

const tGrapButtonPriv PaiJian_BtnPriv[]=
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
	
	{"�ɼ�Ա",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	{"�˵���",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//
	
	{NULL, 0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//��Ƭ���
};

const tGrapEditPriv PaiJian_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,10,  1, EMPLOYEE_ID_LEN-1,  0,0,2,0,0,1,  50,50,0,  0,1,1,1},//�ɼ�Ա
	{NULL,0x0,0,0,2,4,14,  1, BARCODE_LEN-1,		0,0,2,0,0,1,  50,50,0,  0,1,1,1},//�˵���
	
	{gtOpInfoGlb.employee,0x0,0,0,2,4,12,  1, EMPLOYEE_NAME_LEN-1,		0,0,0,0,0,0,  50,50,0,  1,0,0,0},//
};


int PaiJian_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tDATA_INFO 		Recd;
	//tGRAPEDIT		*pEdit_Signer;
	int ret;
	U8	*pCode;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
		//pEdit_Signer = (tGRAPEDIT *)Grap_GetViewByID(pDesk, EDIT_ID_3);
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
			
			//if(pEdit_Signer->view_attr.curFocus == 1 || gpEdit_Code->view_attr.curFocus == 1)
			if(gpEdit_Code->view_attr.curFocus == 1)
			{
				pCode = gpEdit_Code->edit_privattr.digt;
				
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
						//�����ʱû��ǩ������Ϣ,�򽹵��л�
						//if(strlen(pEdit_Signer->edit_privattr.digt) == 0)
						/*if(gpEdit_Code->view_attr.curFocus == 1)
						{
							//Com_SpcDlgDeskbox("����дǩ������Ϣ!",0,pDesk,1,NULL,NULL,100);
							gpEdit_Code->view_attr.curFocus = 0;
							pEdit_Signer->view_attr.curFocus= 1;
							Grap_ChangeInputMode(pEdit_Signer);
							break;
						}*/
						
						//����Ƿ��Ѿ�����
						if(gu32PhotoSize == 0)
						{
							if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�Ƿ���Ҫ����?",0,pDesk,3,NULL,NULL,500))
							{
								OpComm_Photo(pDesk, 0, NULL);
								break;
							}
							gu32PhotoSize = 0;
							gpPhotoBtn->btn_privattr.pBufPic = NULL;
						}

						//����¼�Ĺ�������
						OpCom_FillCommInfo(&Recd, pCode, NULL, RECORD_UNSEND);

						//ҵ�񲿷�
						if(gu32PhotoSize != 0)
						{
							Recd.uData.saomiao.picflag = 1;
						}
						//strcpy(Recd.uData.qianshou.customer, pEdit_Signer->edit_privattr.digt);
						strcpy(Recd.uData.saomiao.deliverer, gpEdit_Employee->edit_privattr.digt);
						
						//д��¼
						ret = Rec_WriteNewRecode(&Recd, NULL);
						if (ret == 0)
						{
							Op_CommonListShow_Add(pCode);
							
							//����ͼƬ
							if (gu32PhotoSize > 0)
							{
								// ͼƬ д��nand
								ret = PicStore_Write(gpEdit_Code->edit_privattr.digt,gpDcWorkBuf,gu32PhotoSize);
								if(ret < 0)
								{
									Com_SpcDlgDeskbox("ͼƬ����ʧ��!",6,pDesk,2,NULL,NULL,150);
									return RETURN_REDRAW;
								}
							}
							gu32PhotoSize = 0;
						}
						else
						{
							Com_SpcDlgDeskbox("��¼����ʧ��!",0,pDesk,1,NULL,NULL,100);
						}
					}
					memset(gpEdit_Code->edit_privattr.digt, 0, gpEdit_Code->edit_privattr.containLen);
				}
				else if(strlen(pCode) > 0)
				{
					Com_SpcDlgDeskbox("�˵���ʽ����!",0,pDesk,1,NULL,NULL,100);
				}
				if(gpEdit_Code->view_attr.curFocus != 1)
				{
					Grap_ChangeFocusEnable(gpEdit_Code);
				}
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
				Com_SpcDlgDeskbox("�˵��Ų���ȷ!",0,pDesk,1,NULL,NULL,150);
				return RETURN_REDRAW;
			}

			/*if(strcmp(gau8PhotoCode, gpEdit_Code->edit_privattr.digt) == 0)
			{
				if(DLG_CONFIRM_EN != Com_SpcDlgDeskbox("���浥������,�Ƿ���������?",6, pDesk, 3, NULL, NULL, 3000))
				{
					return RETURN_REDRAW;
				}
			}*/
			
			OpComm_Photo(pDesk, 0, NULL);
			break;
		default:
			break;
		}
	}
	
	return RETURN_REDRAW;
}

U32 PaiJian_EditKeyEnd(void *pView, U8 type)
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

S32 PaiJian_EditEnter(void *pView)
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

S32 Operation_PaiJian_BtnDestroy(void *pView)
{
	tGRAPBUTTON *pButton;
	
	pButton = (tGRAPBUTTON *)pView;
	free((void *)pButton);
	
	return SUCCESS;
}

/*-----------------------------------------------
*����:Operation_PaiJian_Desk
*����:�ռ�
*����:
*����:
*-----------------------------------------------*/
int Operation_PaiJian_Desk(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT		*pEdit;
	int				i,idx=0;
		
	OpComm_PhotoBufInit();
	
	pDesk = Grap_CreateDeskbox((tGrapViewAttr*)&PaiJian_ViewAttr[idx++], "�ɼ�ɨ��");
	if(pDesk == NULL) return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_3;
	
	for (i=0; i<7; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &PaiJian_ViewAttr[idx++], &PaiJian_BtnPriv[i]);
		if(NULL == pBtn ) return -1;
		
		if(i < 4)
		{
			pBtn->pressEnter = PaiJian_BtnPress;
		}
		else if(i == 6)//��ƬԤ��
		{
			gpPhotoBtn = pBtn;
			pBtn->destroy = Operation_PaiJian_BtnDestroy;
		}
	}
	
	for (i=0; i<3; i++)
	{
		pEdit = Grap_InsertEdit(pDesk, &PaiJian_ViewAttr[idx++], &PaiJian_EditPriv[i]);
		pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
		if(NULL == pEdit ) return -1;
		if(i == 0)//����/�ռ�Ա
		{
			pEdit->editenter  = PaiJian_EditEnter;
			pEdit->editKeyEnd = PaiJian_EditKeyEnd;
			gpEdit_Employee = pEdit;
			//Ĭ���ǵ�¼��
			strcpy(pEdit->edit_privattr.digt, Login_GetUserId());
		}
		else if(i == 1)//�˵���
		{
			gpEdit_Code = pEdit;
		}
		else if(i == 2)
		{
			strcpy(pEdit->edit_privattr.digt, Login_GetName());
		}
	}
	
	Op_CommonInsertListShow(pDesk, 10, 155, 220, 120);
	
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
const U8* const opType_PaiJian[]=//�ռ�
{
	"״̬:",
	
	"��������:",
	"ɨ������:",
	"Ա�����:",
	"ɨ��ʱ��:",
	
	"�ɼ�Ա:",
	//"ǩ����:",
	"�Ƿ�����:",
	"����:",
};


/*---------------------------------------------------------------
*����: PaiJian_SearchDisplay
*����: ��ʾ��¼��ѯ�������ַ���
*����: pData:��ѯ���Ľ��
       total:��ѯ��������
       pDispStr:(out)��ʾ��buf
       dispBufLen:pDispStr����󳤶�
*����: 
----------------------------------------------------------------*/
int PaiJian_SearchDisplay(void *p)
{
	U8 *str[]={"δ��", "�Ѵ�", "��ɾ", "δ��", "δ��"};
	U8 *pic[]={"��", "��"};
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
							opType_PaiJian[0], str[stat],
							opType_PaiJian[1], pComm->opType,
							opType_PaiJian[2], pComm->scanStat,
							opType_PaiJian[3], pComm->userId,
							opType_PaiJian[4], pComm->scanTime,
							
							opType_PaiJian[5], saomiao->deliverer,
							//opType_PaiJian[6], qianshou->customer,
							opType_PaiJian[6], pic[saomiao->picflag],
							opType_PaiJian[7], pComm->code
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
*����: PaiJian_FillData
*����: �����������
*����: pSend:���ڷ��͵�buf
       pNode:׼��������͵�����
       cnt:׼������ĸ���
       pUrl:�����url
*����: 0:�ɹ�   -1:����ͨѶ�쳣
----------------------------------------------------------------*/
int PaiJian_FillData(void *p)
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
	
	pOpCb = (tOP_SENDINFO *)p;
	pSend = pOpCb->pSend;
	bufLen= pOpCb->bufLen;
	pNode = (tQUE_NODE *)( pOpCb->pNode);
	cnt   = pOpCb->cnt;
	pUrl  = pOpCb->pUrl;
return 0;
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
					" "," "," ",saomiao->picflag," "," ");
		
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

hyUsbPrintf("paijian send = ");
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
	hyUsbPrintf("paijian recv = %s \r\n",pSend);
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
hyUsbPrintf("paijian ret = %d \r\n",ret);	
	
	//���ͳɹ�����ǩ��ͼƬ
	if(ret == 0)
	{
		for(i = 0; i < pOpCb->cnt; i++)
		{
			pComm = &(pNode[i].tData.tComm);
			saomiao = &(pNode[i].tData.uData.saomiao);
			if(saomiao->picflag == 0)
			{
				continue;
			}
			dataLen = NetReq_FillHead_DHL(pSend, pComm->opType, pComm->code);
			strcpy(&pSend[dataLen], "<data>");
			dataLen += strlen("<data>");
		hyUsbPrintf("send pic = %s \r\n", pSend);	
			//���ͼƬ����
			pTemp = (U8 *)hsaSdram_UpgradeBuf();
			picsize = PicStore_Read(pComm->code,pTemp,0xFC00);
		hyUsbPrintf("picsize = %d \r\n",picsize);	
			if(picsize > 0 && picsize < 0x7000)
			{
				U8  *encode;
				int encodelen;
				
				encode = pTemp+0x20000;
				
				//base64����
				encodelen = 0x20000;
				ret = mbase64_encode(pTemp,picsize,encode,&encodelen);
			hyUsbPrintf("picsize base64 = %d   %d \r\n",ret, encodelen);	
				if(0 != ret)
				{
					continue;
				}
				
				memcpy(&pSend[dataLen], encode, encodelen);
				dataLen += encodelen;
			}
			else
			{
				continue;
			}
			strcpy(&pSend[dataLen], "</data></senddata>");
			dataLen += strlen("</data></senddata>");
			
			memset(&tRequest, 0, sizeof(struct protocol_http_request));
			
			tRequest.method         = HTTP_POST_METHOD;
		    tRequest.user_agent     = "hyco";
		    tRequest.content_length = dataLen;
		    tRequest.content_data   = pSend;
		    tRequest.referer        = NULL;
		    tRequest.content_type   = "text/xml; charset=GBK";
		    tRequest.range          = NULL;
		    tRequest.connection		= 0;
			tRequest.gzip			= 0;
			
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
			hyUsbPrintf("paijian recv = %s \r\n",pSend);
					pTemp = strstr(pSend, "<result>");
					pE = strstr(pSend, "</result>");
					if(pTemp != NULL && pE != NULL)
					{
						pTemp += strlen("<result>");
						dataLen = pE - pTemp;
						if(dataLen > 4) dataLen = 4;
						memcpy(buf,pTemp, dataLen);
						buf[dataLen] = 0;
						hyUsbPrintf("buf = %s \r\n", buf);
						ret = atoi(buf);
					}
					
					if(ret == 0)
					{
						hyUsbPrintf("del pic = %s \r\n", pComm->code);
						PicStore_Delete(pComm->code);
					}
				}
			}
		}
		
		ret = 0;
	}
	
	return ret;
}

#endif //�ƴ��汾