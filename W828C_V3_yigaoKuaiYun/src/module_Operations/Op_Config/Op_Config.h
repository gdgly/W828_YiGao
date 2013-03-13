#ifndef _OP_CONFIG_H
#define _OP_CONFIG_H

#include "hyTypes.h"

typedef int (*OpConfigFun)(void *p);


//ҵ����ѡ�񿪹�

//����ÿ����˾���ݽӿڸ�ʽ����һ��,����������
#define	VER_CONFIG_HYCO		//�ƴ�
//#define	VER_CONFIG_YUNDA	//�ϴ�
//#define	VER_CONFIG_ZTO		//��ͨ
//#define	VER_CONFIG_YIDA		//���
//#define	VER_CONFIG_GZND		//�����ܴ�





//ʵ��ҵ����gaOp_Config�е�λ��,��Щ�ͻ���Ҫ2��ͳ��,��Щ�ͻ�ֻ��һ��
#define OP_CONFIG_OFFSET	1


//ҵ����Ϣ���� ����ѡ����ʾ/��ѯ/ɾ����ɨ/����/��ѯ��ʾ
typedef struct _op_config_t
{
	U16		right;			//Ȩ��(���ܲ�ͬ��Ȩ�޿�����ҵ��һ��)
	U16		opType;			//ҵ������
	U8		*pName;			//ҵ������
	
	OpConfigFun		opFun;		//ҵ�����
	OpConfigFun		sendFun;	//���緢��
	OpConfigFun		searchFun;	//��ѯ
	OpConfigFun		dispFun;	//��ʾ��ѯ���
	OpConfigFun		delFun;		//ɾ����ɨ
	OpConfigFun		sendFunMore;//һƱ�������
	
}tOP_CONFIG;


//���緢��ʱʹ�õĽṹ��
typedef struct _op_send_info
{
	U8		*pSend;		//����ʱʹ�õ�buf
	int		bufLen;		//����buf�ĳ���
	U8		*pNode;		//һƱһ����tQUE_NODE����,һƱ�����tMULTI_CODE����,ʹ��ʱ��ת��. ��Ҫ���͵�����
	int		cnt;		//��Ҫ�������ݵĸ���   һƱ���ʱ�ò���
	U8		*pUrl;		//��ַ
	
}tOP_SENDINFO;

//��¼��ѯ�����ʾʱʹ�õĽṹ��
typedef struct _op_disp_info
{
	U8		*pData;	//��ѯ�������� tDATA_INFO ����,ʹ��ʱת��
	int		total;	//��ѯ��������
	U8		*pDispBuf;	//�ṩ��ʾ��buf
	int		dispBufLen;	//��ʾbuf�ĳ���
	
}tOP_DISPINFO;


typedef union _op_cfg_u
{
	void	*p;
	tOP_SENDINFO	tSend;
	tOP_DISPINFO	tDisp;
	
}uOP_CFG;


typedef struct _op_callback_t
{
	U8	right;	//�û�Ȩ��
	U8	funType;//��������  0:�����ӿ�    1:���ͽӿ�   2:��ѯ�ӿ�   3:��ѯ��ʾ�ӿ�    4:ɾ���ӿ�	5:һƱ�������
	U16	opType; //ҵ������
	
	uOP_CFG	uOpcfg;
	
}tOP_CALLBACK;


//////////////////////////////////extern//////////////////////////
extern const tOP_CONFIG gaOp_Config[];


#endif //_OP_CONFIG_H