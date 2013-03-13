#include "Test_MainList.h"


tGRAPBUTTON *gpTestPhotoBtn=NULL;
char *gpTestDcWorkBuf, *gpTestPreviewBuf;//����ʱ��
tGrapButtonBuffPic  gtTestRgbbuf;
/*=============================================���ղ��Խ���===================================================*/
const tGrapViewAttr TestDC_ViewAttr[] =
{
	{COMM_DESKBOX_ID,    0,  0,240,320,  0,0,0,0,  TY_UNITY_BG1_EN,TY_UNITY_BG1_EN,0,0,1,0,0,0},//����
#ifdef W818C
	{BUTTON_ID_1,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_2,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
#else	
	{BUTTON_ID_2,	   15,282,73,34,   20,282,88,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_1,	   152,282,73,34,  147,282,225,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
#endif	
	{BUTTON_ID_3,	   90,  100 ,80, 60,   0,0,0,0,   0,0,0,0,1,0,0,0},//ͼƬԤ��
};

const tGrapButtonPriv TestDC_BtnPriv[]=
{
#ifdef W818C
	{"��������",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//����
	{"����",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//����
#else
	{"����",	0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_ESC, TY_UNITY_BG1_EN,NULL},//����
	{"��������",0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,KEYEVENT_OK,  TY_UNITY_BG1_EN,NULL},//����
#endif
	{NULL,		0xffff,0x0,FONTSIZE8X16,0,SHOWSTRING_LIAN_MID,0, TY_UNITY_BG1_EN,NULL},//ͼƬԤ��
};


int TestDC_BtnEnter(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	U32 i,size;
	U8	path[50];
	int fp;
	
	if(state != STATE_NORMAL)
	{
		return SUCCESS;
	}
	
	pBtn  = (tGRAPBUTTON *)pView;
	pDesk =  (tGRAPDESKBOX *)pBtn->pParent;
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://����
		gpTestDcWorkBuf = (char *)hsaSdram_UpgradeBuf();
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		gpTestDcWorkBuf = DC_Desk(pDesk,gpTestDcWorkBuf,&size);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
		
		if (size > 0)
		{
			//����ͼƬ
			sprintf(path, "C:/pic/%d.jpg", rawtime(NULL));
			fp = AbstractFileSystem_Open(path, AFS_READ);
			if(fp >= 0)
			{
				AbstractFileSystem_Close(fp);
				AbstractFileSystem_DeleteFile(path, AFS_SINGLE_DELETE_FDM);
			}
			fp = AbstractFileSystem_Create(path);
			if(fp >= 0)
			{
				AbstractFileSystem_Write(fp, gpTestDcWorkBuf, size);
				AbstractFileSystem_Close(fp);
			}
			
			//Test_WriteLog(gpTestDcWorkBuf, size);			
			memset(&gtTestRgbbuf, 0, sizeof(tGrapButtonBuffPic));
			if(0 == Photo_Decode(gpTestDcWorkBuf,size,60,80, gpTestDcWorkBuf+size,gpTestPreviewBuf,TRUE))
			{				
				gtTestRgbbuf.rgb = gpTestPreviewBuf;
		    	gtTestRgbbuf.off_x1 = 0;
		    	gtTestRgbbuf.off_y1 = 0;
		    	gtTestRgbbuf.width = 80;
		    	gtTestRgbbuf.height = 60;
		    	gpTestPhotoBtn->btn_privattr.pBufPic = (tGrapButtonBuffPic *)&gtTestRgbbuf;
		    }
		    else
		    {
		    	Com_SpcDlgDeskbox("Ԥ��ͼ����ʧ��!",6,pDesk,2,NULL,NULL,150);
		    }
		}
		else
		{
			Com_SpcDlgDeskbox("����ʧ��!",6,pDesk,2,NULL,NULL,150);
		}
		break;
	case BUTTON_ID_2://����
		return RETURN_QUIT;
		break;
	default:
		break;
	}
	
	return RETURN_REDRAW;
}

S32 TestDC_BtnDestroy(void *pView)
{
	tGRAPBUTTON *pButton;
	
	pButton = (tGRAPBUTTON *)pView;
	free((void *)pButton);
	
	return SUCCESS;
}

/*-------------------------------------------------------------
* ����:TestFun_DC
* ����:���ղ���
* ����:
* ����:
--------------------------------------------------------------*/
int TestFun_DC(void *pDeskFather)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	int i, idx = 0;
	
	gpTestDcWorkBuf = (char *)hsaSdram_UpgradeBuf();
	gpTestPreviewBuf = (char *)hsaSdram_UpgradeBuf()+0x300000;
	
	pDesk = Grap_CreateDeskbox(&TestDC_ViewAttr[idx++], NULL);
	if(!pDesk)	return -1;
	Grap_Inherit_Public((tGRAPDESKBOX *)pDeskFather,pDesk);
	
	for(i = 0; i < 3; i++)
	{
		pBtn = Grap_InsertButton(pDesk, &TestDC_ViewAttr[idx++], &TestDC_BtnPriv[i]);
		if (NULL == pBtn) return -1;
		if(i < 2)
		{
			pBtn->pressEnter = TestDC_BtnEnter;
		}
		else if(i == 2)
		{
			gpTestPhotoBtn = pBtn;
			pBtn->destroy = TestDC_BtnDestroy;
		}
	}
	
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
		
	return 0;
}


