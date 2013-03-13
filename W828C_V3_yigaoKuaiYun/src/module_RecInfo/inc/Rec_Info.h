#ifndef _REC_INFO_H_
#define _REC_INFO_H_

/*===== include files =======*/
#include "hyTypes.h"
#include "Op_Struct.h"

/*---------------------------------------------------*/
//#define ADJUST_EXIST_MAX	10		//�ظ�ɨ���ж�  �����


/************************�������ݽṹ**********************/


/////////////////////////////////��¼״̬����///////////////////////////////////////
#define RECORD_UNSEND			0 //δ����
#define RECORD_SEND				1 //�ѷ���
#define RECORD_DELETE			2 //��ɾ��
#define RECORD_WAITSEND			3 //������
#define RECORD_ENABLESEND		4 //�ɷ���


typedef struct _Rec_Info_t
{
	int total;
	int	reported;
	int unreported;
	U8	recType[50];
	U8	cnt;
	U8	max;//recType ����
	
}tREC_INFO;

typedef struct _Rec_Search_t
{
	U16 fileId;
	U16 recIdx;
	U8	recState;
	U8  ifimage;
	U8	barCode[BARCODE_LEN];
	
}tREC_SEARCH;


//��ʼ��ʱ�Լ�¼�洢���������Ϣ
typedef struct _Rec_Config_t
{
	U16		groupMax;	//������ϴ���
	U16		existMax;	//�ظ�ɨ���жϵ������
	U16		delaySend;	//�ӳٷ��͵����ʱ�� (10msΪ��λ)
	
}tREC_CONFIG;

typedef struct _exist_check_t
{
	U8 code[32];
	U8 other[32];
	
}tEXIST_CHECK;

//�ж��ظ�ɨ��
typedef struct _tExist_
{
	int	rd;
	int	wr;
	int	maxcnt;
	int	curcnt;
	
	tEXIST_CHECK	*exist;
	
}tREC_EXIST;


/*��¼�Ĺ�������,�������ҵ��Ľṹ���к͸ù�������һ����Ԫ��,ҵ��ṹ�п���ȥ��,��ʡ�ռ�
	��96�ֽ� */
typedef struct _data_comm_t
{
	U8		opType;		//ҵ���������
	U8		right:5;	//��¼�û�Ȩ��  �����
	U8		state:3;	//��¼��״̬
	U8		groupId;	//�û�����id
	U8		nopacket:4;	//�ü�¼���ܴ������
	U8		ifimage:4;	//�Ƿ�ͼƬ
	U8		userId[EMPLOYEE_ID_LEN];	//��¼�û�����
	U8		scanStat[COMPANY_ID_LEN];	//ɨ��վ��
	U8		code[BARCODE_LEN];		//����, һƱ���ʱ,��Ʊ��
	U8		subCode[BARCODE_LEN];		//�ӵ���, һƱһ��ʱΪ��   һƱ���ʱΪ��ǰ�ӵ���, 
										//һƱ���������ʱҪ����codeһ������Ϣ,������ݸ���Ϣ�жϱ�Ʊ�����
	
	U8		scanTime[16];//ɨ��ʱ�� 20120721102353
	
}tDATA_COMM;

#define DATABUF_MAXLEN		(512-sizeof(tDATA_COMM))		//��¼�ṹ�����ֵ  ���ܴ��ڸ�ֵ

//ҵ������  ÿ����¼���512�ֽ�  ��ȥ��ǰ���һЩ��Ϣ, ���Ǳ�union������С
typedef union _data_buf_u
{
	//�Ѳ�ͬ�������͵����ݽṹ��������,  ::ע���С
	
	tSAOMIAO	saomiao;
	tLUDAN	    ludan;
	tGPSDATA	gps;
	tDAISHOU	daishou;

	tTESTOP		testOp;		//����ҵ��
	
	//���
	char	dataBuf[DATABUF_MAXLEN];
	
}uDATA_BUF;

//��¼�����ݸ�ʽ
typedef struct _data_info_t
{
	//��¼�ṹ�Ĺ�������
	tDATA_COMM	tComm;
	
	//���Ϲ�96byte  ����uDATA_BUF���Ϊ512-96=416
	
	uDATA_BUF	uData;
	
}tDATA_INFO;


//�ⲿ��ʼ���ÿ�ʱ��Ҫ��д�Ľṹ
typedef struct _store_cfg_t
{
	U8		groupMax;		//��¼������͵��������
	U8		delaySecond;	//�ӳٷ���ʱ��(�� Ϊ��λ)
	
	U8		*pSmallRecBuf;	//С��¼�洢��Ҫʹ�õ�buf
	U32		smallBufSize;	//pSmallRec ��С   >=12K
	
	U8		*pLargeRecBuf;	//���¼�洢��Ҫʹ�õ�buf
	U32		largeBufSize;	//pLargeRec ��С   >=64K
	
	U8		*pQueBuf;		//���Ͷ���buf
	U32		queBufSize;		//pQueBuf ��С
	
	U8		*pMultiCodeBuf;	//һƱ���ʱʹ�õ�buf
	U32		multiBufSize;	//pMultiCodeBuf ��С    >=250K
	
	U8		*pExistBuf;		//�ظ�ɨ���жϵ�buf
	U32		existBufSize;	//pExistBuf ��С
	
}tSTORE_CFG;

extern U32		gu32RecUnSendTotal;//δ�ϴ���¼����
extern U32		gu32RecUnSendImage;//δ�ϴ�ͼƬ����
extern U32		gu32RecInNandTotal;//δ�ϴ���¼��nand��δ�����ص�����
extern U32		gu32LastScanTime;//���һ��ɨ��ʱ��
extern U32		gu32MultiInNand;

extern tREC_EXIST	gtRecExist;

#endif //_REC_INFO_H_