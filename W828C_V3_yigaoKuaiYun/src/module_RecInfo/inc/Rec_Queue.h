#ifndef _REC_QUEUE_H_
#define _REC_QUEUE_H_

/*===== include files =======*/
#include "hyTypes.h"
#include "Rec_Info.h"

/*---------------------------------------------------*/
/*��ż�¼��Ϣ�Ķ���,�ϴ�ʱʹ��*/

//����ÿ��������Ϣ
typedef struct _queNode_t
{
	U32		fileId;		//�ļ�id
	U32		recIdx;		//��¼���� ��0��ʼ
	U16		delFlag;	//ɾ����־ 2:��ʾ��ɾ��
	U16		opType;		//ҵ���������
	
	//���ݲ���
	tDATA_INFO	tData;
	
}tQUE_NODE;

//���нṹ
typedef struct _data_queue_t
{
	U32		rd;	//��λ��
	U32		wr;	//дλ��
	U32		delCnt;	//������ɾ���ĸ�ʽ
	U32		size;	//�������еĴ�С
	
	tQUE_NODE	*pQueBuf;
		
}tDATA_QUEUE;


#define MAX_SUBCODE_NUM		8000	//����ֻ���ݶ������ֵ,ʵ�ʿɼ����ӵ������ʼ��ʱ��������buf��С�й�

//����һƱ���ʱʹ�õĶ���(256 Kbytes)    ��ʱ������8000Ʊ�����ز�������(Ĭ���ӵ������ᳬ��8000Ʊ)
typedef struct _multi_code_t
{
	tDATA_INFO	tData;
//	U8	ifOver;		//�Ƿ������� 0:���λ�δ������  1:�Ѽ�����
//	U8	fileIdx;	//�ϴμ��ص��ļ�����
//	U16	recIdx;		//�ϴμ��صļ�¼����
	U8	ifOver;		//�Ƿ������� 0:���λ�δ������  1:�Ѽ�����
	U8	sendType;	//���ͷ�ʽ  0:��̨����   1:ǰ̨����
	U16	total;		//�ܵ��ӵ���  (���δ���������ʾ�Ѿ����ص�����)
	U16 maxNum;		//���ɼ����ӵ���
	
	U8	mainCode[BARCODE_LEN];	//������
	U8	subCode[MAX_SUBCODE_NUM][BARCODE_LEN];	//�ӵ���
	
}tMULTI_CODE;


extern tDATA_QUEUE	gtData_Que;	//���ݶ���

extern tMULTI_CODE	*gptMultiCode;
#endif //_REC_QUEUE_H_