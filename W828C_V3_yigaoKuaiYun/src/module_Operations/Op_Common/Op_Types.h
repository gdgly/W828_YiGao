#ifndef OP_TYPES_H
#define OP_TYPES_H


#include "Op_Config.h"

/*
�����ܴ�
�ļ�¼��¼��
�ռ�ɨ��
����ɨ��
����ȷ��
��ת����
����ɨ��
����ȷ��
�ɼ�ɨ��
ǩ��
װ��
���
����
����
*/
//ҵ��������� ����ҵ��ֵ���ݾ���ͻ������޸�
typedef enum _op_type_e
{
	HYCO_OP_SJ=1,	//�ռ�
	HYCO_OP_FJ=2,	//����
	HYCO_OP_DJ=3,	//����
	HYCO_OP_PJ=4,	//�ɼ�
	
	HYCO_OP_ZD=5,	//װ��
	HYCO_OP_CD=6,	//���
	
	HYCO_OP_WENTI=7,	//�����
	HYCO_OP_LIUCANG=8,	//���ּ�
	
	HYCO_OP_QS=9,		//ǩ��
	
	HYCO_OP_DSHK=10,	//���ո���
	
	HYCO_OP_FJQR=11,	//����ȷ��
	HYCO_OP_ZZFJ=12,	//��ת����
	HYCO_OP_DJQR=13,	//����ȷ��
	
	HYCO_OP_LD=14,		//ǩ��¼��
	
	//HYCO_OP_15=15,
	//HYCO_OP_16=16,
	//HYCO_OP_17=17,
	//HYCO_OP_18=18,
	//HYCO_OP_19=19,
	//HYCO_OP_20=20,
	
	HYCO_OP_SJWT=21,	//�����ռ�
	HYCO_OP_FJWT=22,	//���ط���
	HYCO_OP_DJWT=23,	//���ص���
	HYCO_OP_PJWT=24,	//�����ɼ�

	HYCO_OP_TEST=254,	//����ҵ��
	HYCO_TYPE_MAX = 255,//��ͳ�Ʊ�����
	
}eOPTYPE;


#endif //OP_TYPES_H