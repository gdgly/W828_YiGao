#ifndef _OP_COMM_H
#define _OP_COMM_H


#include "hyTypes.h"
#include "grap_api.h"
#include "base.h"
#include "http.h"
#include "glbVariable.h"
#include "Common_Dlg.h"
#include "Common_Widget.h"
#include "Common_AppKernal.h"
#include "Net_Evt.h"
#include "bmpComData.h"
#include "Rec_Queue.h"
#include "Op_Config.h"
#include "drvKeyArray.h"
#include "Op_RequestInfo.h"


typedef struct _scantype_str_t
{
	int	type;
	U8  *pStr;
	
}tTYPE_STR;


typedef  struct areaNode
{
	U8  *code ;
	U8  *area ;	
	struct areaNode *next ;
	U8  nodenum;
	
}AREAnode;


//�������
typedef enum _error_code_e
{
	ERR_SUCCESS=0,	//�ɹ�
	ERR_CONNECTFAIL,//��������ʧ��
	ERR_CONNECTTIMEOUT,//�������ӳ�ʱ
	ERR_SENDFAIL,	//����ͨѶʧ��
	
	ERR_NEWEST,	//�Ѿ������°汾
	ERR_DATAERROR,	//���ݴ���
	
	ERR_UPGRADEFAIL,	//����ʧ��
	
	ERR_OTHER = 0x80,	//��������,���������緵�صĴ���, &0x7f�õ���ֵ�����緵�صĴ�����
	
	ERR_NULL
	
}eERRCODE;

//��Ҫ��ǰ̨��������
/*typedef enum _gprs_reqtype_e
{
	REQ_LOGIN=1,	//��¼
	REQ_UPGRADE,	//��������
	REQ_COMPANY,	//����������Ϣ
	REQ_PROBLEM,	//�����
	REQ_EMPLOYEE,	//Ա����Ϣ
	REQ_GETWEIVOL,	//�������
	REQ_REGION,		//��������
	REQ_CARINFO,	//������Ϣ
	
	REQ_SENDMULTI,	//����һƱ���
	
	REQ_NULL
	
}eNET_REQTYPE;*/


//ҵ�����ҳ������õ���һЩ��Ϣ
typedef struct _opinfo_glb_t
{
	U8	curOptype;		//����ҵ������
	U8	tempIndex;		//��������ʱʹ��
	
	U8	stationId1[COMPANY_ID_LEN];//��һվ/ʼ��վ ���
	U8 	station1[COMPANY_NAME_LEN];	//��һվ/ʼ��վ
	U8	stationId2[COMPANY_ID_LEN];//��һվ/Ŀ��վ ���
	U8 	station2[COMPANY_NAME_LEN];	//��һվ/Ŀ��վ
	
	U8	employeeId[EMPLOYEE_ID_LEN];//��ԱID
	U8 	employee[EMPLOYEE_NAME_LEN];	//��Ա����
	
	U8 	packet[BARCODE_LEN];		//�����
	U8 	carCode[BARCODE_LEN];		//������
	U8 	carLine[BARCODE_LEN];		//������
	
	U8 	weight[WEIGHT_LEN];			//����
	U8 	fee1[FEE_LEN];				//����
	U8 	fee2[FEE_LEN];				//����
	
}tOPINFO_GLB;



/////////////////////////////////extern/////////////////////////////////////////////
extern const U8 OpCom_Month[];

extern tGRAPBUTTON *gpPhotoBtn;
extern U8 	 gau8PhotoCode[];
extern char *gpDcWorkBuf, *gpPreviewBuf;//����ʱ��
extern U32	 gu32PhotoSize;
extern tGrapButtonBuffPic  rgb_buf;

extern tGRAPEDIT	*gpEdit_Stat1;	//��һվ/ʼ��վ
extern tGRAPEDIT	*gpEdit_Stat2;	//��һվ/Ŀ��վ
extern tGRAPEDIT	*gpEdit_Employee;//��Ա
extern tGRAPEDIT	*gpEdit_Packet;	//�����
extern tGRAPEDIT	*gpEdit_CarCode;//������
extern tGRAPEDIT	*gpEdit_CarLine;//������
extern tGRAPEDIT	*gpEdit_Code;	//����

extern tOPINFO_GLB	gtOpInfoGlb;

extern eERRCODE	geLastErrCode;

extern const AREAnode province[];




extern S32 ComWidget_LabTimerISR(void *pView);
extern int Net_SendRequest_F(void *p);
extern int Rec_Statistics(void *pDeskFather);

extern int BT_WeightEditTimer(void *pView);

S32 OpCom_EditDrawInputCnt(tGRAPEDIT *pEdit);

S32 OpComm_BtnBTDraw(void *pView, U32 xy, U32 wh);
S32 OpComm_BTEditDraw(void *pView, U32 xy, U32 wh);
int OpComm_ConnBT(tGRAPDESKBOX *pDesk, tGRAPEDIT *pEditWT);

S32 OpCom_LineInfo_FindNode(tCHARPOS *pHead, U16 LineNo, U32 *firstCharPos);
int OpComm_EditHandle_Price(void *pView, GRAP_EVENT *pEvent);

S32 OpComm_EditTimer(void *pView);

#endif//_OP_COMM_H