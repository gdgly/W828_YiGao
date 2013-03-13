#include "Test_MainList.h"

U8 gu8ScanCntBuf[20];

U32 gu32TestScanTotal, gu32SuccessScan;
U32 gu32ScanUseTime;
/*=============================================ɨ����Խ���===================================================*/
/*�ؼ�*/
const tGrapViewAttr TestScanViewAtrr[]=
{
	{COMM_DESKBOX_ID,   0,  0, 240,320, 0,0,0,0,   TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/	
#else
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/	
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},/*����*/
#endif	
	{BUTTON_ID_3,	10,70,220, 16,	0,0,0,0 ,	    0,0,0,0,1,0,0,0},
	{BUTTON_ID_4,	10,100,220, 16,	0,0,0,0 ,	    0,0,0,0,1,0,0,0},
	
	{EDIT_ID_1,		29,140,186,24,  29,100,215,164,   0,0,0xffff,0,1,1,1,0},/*�����ı���*/
					
};

/*btn ����*/
const tGrapButtonPriv TestScanBtnAttr[]=
{
#ifdef W818C
	{(U8 *)"ȷ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},
#else
	{(U8 *)"����", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},	
	{(U8 *)"ȷ��", 0xffff,0x0,FONTSIZE8X16,0,2,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},
#endif
	{gu8ScanCntBuf, 0xffff,0x0,FONTSIZE8X16,0,2,0, TY_UNITY_BG1_EN,NULL},
	{gau8TestFunBuf, 0xffff,0x0,FONTSIZE8X16,0,2,0, TY_UNITY_BG1_EN,NULL},
};
/*edt ����*/
const tGrapEditPriv	  TestScanEdtAttr []=
{
	{NULL,COLOR_STR_BARSELECT,0,0,2,6,23,  1,30,0,0,1,0,0,1,   50,50,0,  0,1,1,1},
};


/*��ť�¼�*/
static S32 TestScan_BtnEnter(void *pView,U16 state)
{
	tGRAPBUTTON  *pBtn ;
	tGRAPDESKBOX *pDesk;
	tGRAPEDIT    *pEdit;
	U32  id;
	S32  rv = SUCCESS;

	pBtn  = (tGRAPBUTTON*)pView;
	pDesk = (tGRAPDESKBOX*)pBtn->pParent;
	
	id=pBtn->view_attr.id;
	
	if (state==STATE_NORMAL)
	{
		switch (id)
		{
		case BUTTON_ID_1:
			gu32SuccessScan++;
			pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk, EDIT_ID_1);
			sprintf(gu8ScanCntBuf, "�ɹ�%d��",gu32SuccessScan);
			strcpy(gau8TestFunBuf, pEdit->edit_privattr.digt);
			pEdit->edit_privattr.digt[0] = 0;
			break;
		case BUTTON_ID_2:
			return RETURN_QUIT;
			break;
		}
		
		return RETURN_REDRAW;
	}
	
	return rv;
}



/*---------------------------------------------
*����:TestFun_Scan
*����:ɨ�����
*����:
*����:
*-----------------------------------------------*/
int TestFun_Scan(void *pDeskFather)
{
	tGRAPDESKBOX 	*pDesk;
	tGRAPBUTTON 	*pBtn;
	tGRAPEDIT 		*pEdt;
	U8 				i, idx=0;
	U16 			lastValue;
	
	strcpy(gau8TestFunBuf, "(��ɨ��0��)");
	gu32TestScanTotal = 0;
	gu32SuccessScan = 0;
	
	pDesk=Grap_CreateDeskbox(&TestScanViewAtrr[idx++], NULL);
	if (NULL==pDesk)
	{
		return -1;
	}
	Grap_Inherit_Public(pDeskFather,pDesk);/*��Ϣ�̳�*/
	pDesk->scanEnable = 1;
	pDesk->inputTypes = INPUT_TYPE_123|INPUT_TYPE_abc|INPUT_TYPE_ABC;
	pDesk->inputMode = 1;//123
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	
	/*���밴ť*/
	for (i=0; i<4; i++)
	{
		pBtn=Grap_InsertButton(pDesk, &TestScanViewAtrr[idx++], &TestScanBtnAttr[i]);
		if (NULL == pBtn)
		{
			return -1;
		}
		pBtn->pressEnter = TestScan_BtnEnter;
	}
	/*����༭��*/
	pEdt= Grap_InsertEdit(pDesk, &TestScanViewAtrr[idx++], &TestScanEdtAttr[0]);
	pEdt->DrawInputCnt = OpCom_EditDrawInputCnt;
	if (NULL == pEdt)
	{
		return -1;
	}
	
	lastValue = BackLight_CloseScreenProtect();//��¼������Ϣ �˳�ʱ��ԭ
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);	
	Grap_DeskboxDestroy(pDesk);
	BackLight_SetScreenProtect(lastValue);
	
	return 0;
}


