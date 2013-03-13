#ifndef _OP_STRUCT_H
#define _OP_STRUCT_H


#include "hyTypes.h"


//����һЩ��Ϣ�ĳ���
#define COMPANY_NAME_LEN		64	//��������
#define COMPANY_ID_LEN			12	//������
#define	COMPANY_RIGHT_LEN		4	//��˾Ȩ��

#define EMPLOYEE_ID_LEN			16	//Ա�����
#define EMPLOYEE_NAME_LEN		40	//Ա������
#define EMPLOYEE_PW_LEN			16	//Ա������
#define EMPLOYEE_ROLE_LEN		16	//Ա����ɫ

#define PROBLEM_ID_LEN			8	//�������ͱ���
#define PROBLEM_CODE_LEN		8	//�������ʹ���
#define PROBLEM_NAME_LEN		32	//������������
#define PROBLEM_TYPE_LEN		4	//��������

#define LEAVE_CODE_LEN		8	//�������ʹ���
#define LEAVE_NAME_LEN		32	//������������

#define CLASS_ID_LEN			8	//���䷽ʽ����
#define CLASS_NAME_LEN			16	//���䷽ʽ����

#define FLIGHT_ID_LEN			16	//������Ϣ����
#define FLIGHT_NAME_LEN			64	//������Ϣ����

#define CAR_ID_LEN				16	//������Ϣ����
#define CAR_NAME_LEN			64	//������Ϣ����

#define CAR_INDEX_LEN			16	//���
#define	BARCODE_LEN				24	//����
#define WEIGHT_LEN				8	//����
#define FEE_LEN					8	//�˷�

#define	REASON_ID_LEN			4	//��������
#define REASON_DISC_LEN			32	//��������
#define CUSTOMER_ID_LEN			20	//�ͻ����
#define CUSTOMER_NAME_LEN		10	//�ͻ�����

#define PHONE_NUM_LEN			16	//�绰���볤��
#define REMARK_LEN				80	//��ע

#define BCHID_LEN				30	//���γ���
#define GOODS_NAME_LEN			20	//��������

#define COMMON_LESSTHAN8		8	//С�ڵ���8�Ķ�����ʹ��
#define COMMON_LESSTHAN4		4	//С�ڵ���4�Ķ�����ʹ��


/*++++++++++++++++++++++++++++++++++ҵ��ṹ+++++++++++++++++++++++++++++++++++++++*/
//װ  ��  ��  ж  
//��  ��  ��  ��  
//ɨ  ��  ��  ǩ  


//ɨ��ҵ���ϴ����ݽṹ   416
typedef struct _SaoMiao_
{
	U8      bchId[BCHID_LEN]  ;    //���α��  Ĭ��Ϊʱ��+��ǹ���
	
	U8		othercode[BARCODE_LEN];	//ת����/����������
	U8		packetcode[BARCODE_LEN];	//�����
	U8      obj_ids[COMMON_LESSTHAN4]   ;  //�������� 	 20:��� 21:�ļ�
	U8		cheliang[BARCODE_LEN];	//������    �磺��G56842  ����Ϊ��010G56842��   9��10λ	
	U8		chexian[BARCODE_LEN];	//������    10λ���������ֱ���
	U8      wei_tpyes[COMMON_LESSTHAN4] ;  //��������   1:�� 2:�� 3:С
	U8      obj_wei[COMMON_LESSTHAN8]   ;  //��Ʒ����
	U8      obj_len[COMMON_LESSTHAN8]   ;  //��Ʒ��
	U8      obj_wid[COMMON_LESSTHAN8]   ;  //��Ʒ��
	U8      obj_hei[COMMON_LESSTHAN8]   ;  //��Ʒ��
	U8      vol_tpyes[COMMON_LESSTHAN4] ;  //�������   1:�� 2:�� 3:С
	U8      obj_vol[COMMON_LESSTHAN8]   ;  //��Ʒ���
	U8      tran_types[COMMON_LESSTHAN4];  //��������
	U8      cmt_ids[COMMON_LESSTHAN4]   ;  //��ע����
	U8      cmt_inf[REMARK_LEN] ;  //��ע��Ϣ
	U8      origstat[COMPANY_ID_LEN];    //ʼ��վ��
	U8      prevstat[COMPANY_ID_LEN]  ;  //�ϼ�վ��
	U8      nextstat[COMPANY_ID_LEN]  ;  //�¼�վ��
	U8      nextprov[COMMON_LESSTHAN8];   //�¼�ʡ��
	U8      deststat[COMPANY_ID_LEN];    //Ŀ��վ��
	U8      dest_zone[COMPANY_ID_LEN];  //Ŀ������
	U8      customer[EMPLOYEE_NAME_LEN] ;  //�ͻ�����
	U8      deliverer[EMPLOYEE_ID_LEN];  //������Ա
	U8		daishou_fee[FEE_LEN];//���տ�
	U8		daofu_fee[FEE_LEN];//������
	U8		worktype;	//��������  91 ж���ֹ�  92 װ���ֹ�
	U8		signType;			//ǩ�����
	U8      picflag;  			//�Ƿ�����

}tSAOMIAO;


//���ո���
typedef struct _daishou_t
{
	U8		collect_id[EMPLOYEE_ID_LEN];	//�ռ�Ա
	U8		pay_type[REASON_ID_LEN];		//֧������id
	U8		pay_name[REASON_ID_LEN];		//֧����������
	
	U8		cost_trans[COMMON_LESSTHAN8];	//���ո����˷�
	U8		money[COMMON_LESSTHAN8];		//���ս��
	U8		cost_protect[COMMON_LESSTHAN8];//���۽��
	
	U8		customer_id[CUSTOMER_ID_LEN];	//�ͻ����
	
}tDAISHOU;


//GPS�����ϴ��ṹ    ���ȣ�ά�ȣ��߶ȣ��ɼ�ʱ��   156
typedef struct _tGpsData_
{
	U8      bchId[BCHID_LEN]  ;    //���α��  Ĭ��Ϊʱ��+��ǹ���
	
	U8		Long[14];		//����
	U8		Lat[14];		//γ��
	U8		high[8];		//�߶�
	U8      addr[40];       //��ַ
	
	U8		date[20];		//����ʱ��
	U8		machinecode[18];//�������
	U8		simcard[12];	//SIM����

}tGPSDATA;


//¼��   461
typedef  struct  _ludan_t
{
	U8      bchId[BCHID_LEN];    //���α��  Ĭ��Ϊʱ��+��ǹ���
	
	U8      sub_code[BCHID_LEN];
	U8      deststat[COMPANY_ID_LEN];//Ŀ�ĵ�(վ��)
	U8      obj_wei[WEIGHT_LEN];//����
	U8		num[COMMON_LESSTHAN8];//����
	U8		payType;//֧����ʽ
	U8		daishouCode[BARCODE_LEN];//����ǩ��
	U8		feeDaishou[FEE_LEN];//���ջ���
	U8		feeTotal[FEE_LEN];//����
	U8		customer_id[CUSTOMER_ID_LEN];//�ͻ�
	U8		tranType;//���䷽ʽ
	U8		goodsType;//��Ʒ���
	U8		goodsName[GOODS_NAME_LEN];//��Ʒ����
	U8		moneyType;//����
	U8		ifNotice;//����֪ͨ
	U8		noticeNum[PHONE_NUM_LEN];//֪ͨ����
	U8		recvName[CUSTOMER_NAME_LEN];//�ռ�������
	U8		recvPhone[PHONE_NUM_LEN];//�ռ��˵绰
	U8		recvAddr[REMARK_LEN];//�ռ��˵�ַ
	U8		sendName[CUSTOMER_NAME_LEN];//�ļ�������
	U8		sendPhone[PHONE_NUM_LEN];//�ļ��˵绰
	U8		sendAddr[REMARK_LEN];//�ļ��˵�ַ
	
}tLUDAN;


//����ҵ��
typedef struct _test_op_t
{
	U8 Station[COMPANY_ID_LEN];//��һվ
	
}tTESTOP;




#endif//_OP_STRUCT_H