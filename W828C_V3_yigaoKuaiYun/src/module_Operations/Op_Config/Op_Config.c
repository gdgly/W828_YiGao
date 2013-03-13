#include "Op_Config.h"
#include "Op_Types.h"


extern int Rec_Statistics(void *p);
extern int Rec_DelErrScanDesk(void *p);

/*------------------------------------------------------------------------------*/
//�ļ�¼��
extern int Operation_LuDan_Desk(void *p);

/*�ռ�����*/
extern int Operation_ShouJian_Desk(void *p);
extern int Operation_ShouJianWT_Desk(void *p);
extern int Operation_ShouJianWtFee_Desk(void *p);

/*��������*/
extern int Operation_FaJian_Desk(void *p);
extern int Operation_FaJianWT_Desk(void *p);
//extern int Operation_FaJianWtFee_Desk(void *p);
extern int Operation_FaJianQR_Desk(void *p);
extern int Operation_FaJianZZ_Desk(void *p);

/*��������*/
extern int Operation_DaoJian_Desk(void *p);
extern int Operation_DaoJianWT_Desk(void *p);
extern int Operation_DaoJianWtFee_Desk(void *p);
extern int Operation_DaoJianQR_Desk(void *p);

/*�ɼ�����*/
extern int Operation_PaiJian_Desk(void *p);
extern int Operation_PaiJianWT_Desk(void *p);
extern int Operation_PaiJianWtFee_Desk(void *p);

//ǩ��
extern int Operation_QianShou_Desk(void *p);

//װ��
extern int Operation_ZhuangDai_Desk(void *p);

//���
extern int Operation_ChaiDai_Desk(void *p);


extern int Operation_WenTi_Desk(void *p);
extern int Operation_LiuCang_Desk(void *p);
extern int Operation_DaiShou_Desk(void *p);
/*------------------------------------------------------------------------------*/
//�ļ�¼��
extern int LuDan_FillData(void *p);

/*�ռ�����*/
extern int ShouJian_FillData(void *p);
extern int ShouJianWT_FillData(void *p);
extern int ShouJianWtFee_FillData(void *p);


/*��������*/
extern int FaJian_FillData(void *p);
extern int FaJianWT_FillData(void *p);
extern int FaJianQR_FillData(void *p);
extern int FaJianZZ_FillData(void *p);

/*��������*/
extern int DaoJian_FillData(void *p);
extern int DaoJianWT_FillData(void *p);
extern int DaoJianWtFee_FillData(void *p);
extern int DaoJianQR_FillData(void *p);

/*�ɼ�����*/
extern int PaiJian_FillData(void *p);
extern int PaiJianWT_FillData(void *p);
extern int PaiJianWtFee_FillData(void *p);

//ǩ��
extern int QianShou_FillData(void *p);

//װ��
extern int ZhuangDai_FillData(void *p);

//���
extern int ChaiDai_FillData(void *p);

extern int WenTi_FillData(void *p);
extern int LiuCang_FillData(void *p);
extern int DaiShou_FillData(void *p);
/*------------------------------------------------------------------------------*/
//�ļ�¼��
extern int LuDan_SearchDisplay(void *p);

/*�ռ�����*/
extern int ShouJian_SearchDisplay(void *p);
extern int ShouJianWT_SearchDisplay(void *p);
extern int ShouJianWtFee_SearchDisplay(void *p);

/*��������*/
extern int FaJian_SearchDisplay(void *p);
extern int FaJianWT_SearchDisplay(void *p);
//extern int FaJianWtFee_SearchDisplay(void *p);
extern int FaJianQR_SearchDisplay(void *p);
extern int FaJianZZ_SearchDisplay(void *p);

/*��������*/
extern int DaoJian_SearchDisplay(void *p);
extern int DaoJianWT_SearchDisplay(void *p);
extern int DaoJianWtFee_SearchDisplay(void *p);
extern int DaoJianQR_SearchDisplay(void *p);

/*�ɼ�����*/
extern int PaiJian_SearchDisplay(void *p);
extern int PaiJianWT_SearchDisplay(void *p);
extern int PaiJianWtFee_SearchDisplay(void *p);

//ǩ��
extern int QianShou_SearchDisplay(void *p);

//װ��
extern int ZhuangDai_SearchDisplay(void *p);

//���
extern int ChaiDai_SearchDisplay(void *p);

extern int WenTi_SearchDisplay(void *p);
extern int LiuCang_SearchDisplay(void *p);
extern int DaiShou_SearchDisplay(void *p);


//-------------------------------------�׸�ҵ��--------------------------------------
//¼������
extern int Operation_YGLuDan_Desk(void *pDeskFather);
extern int YGLuDan_FillData(void *p);
//�ռ�
extern int Operation_YGShouJian_Desk(void *pDeskFather);
extern int YGShouJian_FillData(void *p);
//�˼��ռ�
extern int Operation_YGTuiJian_ShouJian_Desk(void *pDeskFather);
extern int YGTuiJian_ShouJian_FillData(void *p);
//����
extern int Operation_YGFaJian_Desk(void *pDeskFather);
extern int YGFaJian_FillData(void *p);
//�˼�����
extern int Operation_YGTuiJian_FaJian_Desk(void *pDeskFather);
extern int YGTuiJian_FaJian_FillData(void *p);
//�ص�����
extern int Operation_YGHuiDan_FaJian_Desk(void *pDeskFather);
extern int YGHuiDan_FaJian_FillData(void *p);
//����
extern int Operation_YGDaoJian_Desk(void *pDeskFather);
extern int YGDaoJian_FillData(void *p);
//��������
extern int Operation_YGDaoJianWT_Desk(void *pDeskFather);
extern int YGDaoJianWT_FillData(void *p);
//�˼�����
extern int Operation_YGTuiJian_DaoJian_Desk(void *pDeskFather);
extern int YGTuiJian_DaoJian_FillData(void *p);
//�ص�����
extern int Operation_YGHuiDan_DaoJian_Desk(void *pDeskFather);
extern int YGHuiDan_DaoJian_FillData(void *p);
//�ɼ�
extern int Operation_YGPaiJian_Desk(void *pDeskFather);
extern int YGPaiJian_FillData(void *p);
//ǩ��
extern int Operation_YGQianShou_Desk(void *pDeskFather);
extern int YGQianShou_FillData(void *p);
//�����
extern int Operation_YGWenTi_Desk(void *pDeskFather);
extern int YGWenTi_FillData(void *p);
//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Ȩ��    -----Ŀǰ��ʱûʹ��
ҵ������  ���嶨����Op_Types.h��
ҵ������  ���8���ֽ�(��4������)

ҵ���������
ҵ�����緢�ͺ���
ҵ���ѯ
ҵ���ѯ�����ʾ
ҵ��ɾ��
һƱ�������
*/

const tOP_CONFIG gaOp_Config[]=//���Ʋ�Ҫ����4������ ������ͳ��ҳ����ʾ��ȫ
{
	//Ȩ��	����			����		��������							���ͺ���				��ѯ				��ѯ�����ʾ				ɾ��					һƱ�������
	{0,		0,				"ҵ��ͳ��",	NULL,								NULL,					Rec_Statistics,		NULL,						NULL,					NULL},
	//{0,		HYCO_TYPE_MAX,	"����ͳ��",	NULL,							NULL,					Rec_Statistics,		NULL,						NULL,					NULL},
	//����Ϊͳ��
	
	{0,		HYCO_OP_LD,		"¼��",		Operation_YGLuDan_Desk,				YGLuDan_FillData,		Rec_Statistics,		LuDan_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	
	{0,		HYCO_OP_SJ,		"�ռ�",		Operation_YGShouJian_Desk,			YGShouJian_FillData,	Rec_Statistics,		ShouJian_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	//{0,		HYCO_OP_SJWT,	"�ռ�",		Operation_ShouJianWT_Desk,		ShouJianWT_FillData,	Rec_Statistics,		ShouJianWT_SearchDisplay,	Rec_DelErrScanDesk,		NULL},
	{0,		HYCO_OP_SJWT,	"�˼��ռ�",	Operation_YGTuiJian_ShouJian_Desk,	YGTuiJian_ShouJian_FillData,Rec_Statistics,		ShouJianWtFee_SearchDisplay,Rec_DelErrScanDesk,		NULL},
		
	{0,		HYCO_OP_FJ,		"����",		Operation_YGFaJian_Desk,			YGFaJian_FillData,		Rec_Statistics,		FaJian_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	{0,		HYCO_OP_FJQR,	"�˼�����",	Operation_YGTuiJian_FaJian_Desk,	YGTuiJian_FaJian_FillData,		Rec_Statistics,		FaJianQR_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	{0,		HYCO_OP_ZZFJ,	"�ص�����",	Operation_YGHuiDan_FaJian_Desk,		YGHuiDan_FaJian_FillData,		Rec_Statistics,		FaJianZZ_SearchDisplay,		Rec_DelErrScanDesk,		NULL},

	//{0,		HYCO_OP_FJWT,	"��������",	Operation_FaJianWT_Desk,		FaJianWT_FillData,		Rec_Statistics,		FaJianWT_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	
	{0,		HYCO_OP_DJ,		"����",		Operation_YGDaoJian_Desk,			YGDaoJian_FillData,		Rec_Statistics,		DaoJian_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	{0,		HYCO_OP_DJQR,	"��������",	Operation_YGDaoJianWT_Desk,			YGDaoJianWT_FillData,		Rec_Statistics,		DaoJianQR_SearchDisplay,	Rec_DelErrScanDesk,		NULL},
	//{0,		HYCO_OP_DJWT,	"��������",	Operation_DaoJianWT_Desk,		DaoJianWT_FillData,		Rec_Statistics,		DaoJianWT_SearchDisplay,	Rec_DelErrScanDesk,		NULL},
	{0,		HYCO_OP_DJWT,	"�˼�����",	Operation_YGTuiJian_DaoJian_Desk,	YGTuiJian_DaoJian_FillData,	Rec_Statistics,		DaoJianWtFee_SearchDisplay,	Rec_DelErrScanDesk,		NULL},
	{0,		HYCO_OP_PJWT,	"�ص�����",	Operation_YGHuiDan_DaoJian_Desk,	YGHuiDan_DaoJian_FillData,		Rec_Statistics,		PaiJianWT_SearchDisplay,	Rec_DelErrScanDesk,		NULL},
	
	{0,		HYCO_OP_PJ,		"�ɼ�",		Operation_YGPaiJian_Desk,			YGPaiJian_FillData,		Rec_Statistics,		PaiJian_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	
	{0,		HYCO_OP_QS,		"ǩ��",		Operation_YGQianShou_Desk,			YGQianShou_FillData,		Rec_Statistics,		QianShou_SearchDisplay,		Rec_DelErrScanDesk,		NULL},

	//{0,		HYCO_OP_ZD,		"װ��",		Operation_ZhuangDai_Desk,		ZhuangDai_FillData,		Rec_Statistics,		ZhuangDai_SearchDisplay,	Rec_DelErrScanDesk,		NULL},
	//{0,		HYCO_OP_CD,		"���",		Operation_ChaiDai_Desk,			ChaiDai_FillData,		Rec_Statistics,		ChaiDai_SearchDisplay,		Rec_DelErrScanDesk,		NULL},

	{0,		HYCO_OP_WENTI,	"�����",	Operation_YGWenTi_Desk,				YGWenTi_FillData,			Rec_Statistics,		WenTi_SearchDisplay,		Rec_DelErrScanDesk,		NULL},
	//{0,		HYCO_OP_LIUCANG,"���ּ�",	Operation_LiuCang_Desk,			LiuCang_FillData,		Rec_Statistics,		LiuCang_SearchDisplay,		Rec_DelErrScanDesk,		NULL},

	
	//{0,		HYCO_OP_DSHK,	"���ո���",	Operation_DaiShou_Desk,			DaiShou_FillData,		Rec_Statistics,		DaiShou_SearchDisplay,		Rec_DelErrScanDesk,		NULL},

};

#define OP_CFG_TOTAL	(sizeof(gaOp_Config)/sizeof(tOP_CONFIG))

//////////////////////////////////////////////////////////////////////////
/*-------------------------------------------------------
* ����:Op_Config_GetCntByRight
* ����:��ѯָ��Ȩ�޵�ҵ������
* ����:
* ����:<=0ʧ��
--------------------------------------------------------*/
int Op_Config_GetCntByRight(U16 right)
{
	int	total=0,i;
	
	for(i = 0; i < OP_CFG_TOTAL; i++)
	{
		if(gaOp_Config[i].right == right)
		{
			total++;
		}
	}
	
	return total;
}

/*-------------------------------------------------------
* ����:Op_Config_GetOpTypeByRight
* ����:��ѯָ��Ȩ��ָ��index��ҵ������
* ����:index:��0��ʼ
* ����:-1:����
--------------------------------------------------------*/
int Op_Config_GetOpTypeByRight(U16 right, U16 index)
{
	int	cnt=0,i;
	int opType = -1;
	
	for(i = 0; i < OP_CFG_TOTAL; i++)
	{
		if(gaOp_Config[i].right == right)
		{
			if(cnt == index)
			{
				opType = gaOp_Config[i].opType;
				break;
			}
			cnt++;
		}
	}
	
	return opType;
}

/*-------------------------------------------------------
* ����:Op_Config_GetIndexByType
* ����:����ָ��ҵ������,�ҵ���gaOp_Config�е�����
* ����:opType:ҵ������
* ����:-1:����
--------------------------------------------------------*/
int Op_Config_GetIndexByType(U16 opType)
{
	int i,index = -1;
	
	for(i = 0; i < OP_CFG_TOTAL; i++)
	{
		if(gaOp_Config[i].opType == opType)
		{
			index = i;
			break;
		}
	}
	
	return index;
}

/*-------------------------------------------------------
* ����:Op_Config_CallBack
* ����:����ҵ������,ѡ��ִ�����úõĹ��ܺ���
* ����:
* ����:0:�ɹ�    ����:ʧ��
--------------------------------------------------------*/
int Op_Config_CallBack(tOP_CALLBACK *pOpCb)
{
	int		i,ret = 0;
	U8		right,funType;
	U16		opType;
	
	right   = pOpCb->right;
	funType = pOpCb->funType;
	opType  = pOpCb->opType;
	
	//�Ҷ�Ӧ�ӿ�
	for(i = 0; i < OP_CFG_TOTAL; i++)
	{
		if(gaOp_Config[i].right == right && gaOp_Config[i].opType == opType)//Ȩ�޺�ҵ��������ͬ
		{
			//�Ҷ�Ӧ�ӿ�
			switch(funType)
			{
			case 0://�����ӿ�
				if(gaOp_Config[i].opFun != NULL)
				{
					ret = gaOp_Config[i].opFun(pOpCb->uOpcfg.p);
				}
				break;
			case 1://���ͽӿ�
				if(gaOp_Config[i].sendFun != NULL)
				{
				hyUsbPrintf("���ͽӿ�  i = %d  type = %d \r\n",i,opType);
					ret = gaOp_Config[i].sendFun((void *)&(pOpCb->uOpcfg.tSend));
				}
				break;
			case 2://��ѯ�ӿ�
				if(gaOp_Config[i].searchFun != NULL)
				{
					ret = gaOp_Config[i].searchFun(pOpCb->uOpcfg.p);
				}
				break;
			case 3://��ѯ��ʾ�ӿ�
				if(gaOp_Config[i].dispFun != NULL)
				{
				hyUsbPrintf("i = %d  type = %d \r\n",i,opType);
					ret = gaOp_Config[i].dispFun((void *)&(pOpCb->uOpcfg.tDisp));
				}
				break;
			case 4://ɾ���ӿ�
				if(gaOp_Config[i].delFun != NULL)
				{
					ret = gaOp_Config[i].delFun(pOpCb->uOpcfg.p);
				}
				break;
			case 5://һƱ�������
				if(gaOp_Config[i].sendFunMore != NULL)
				{
					ret = gaOp_Config[i].sendFunMore((void *)&(pOpCb->uOpcfg.tSend));
				}
				break;
			default:
				break;
			}
			
			break;
		}
	}
	
	return ret;
}