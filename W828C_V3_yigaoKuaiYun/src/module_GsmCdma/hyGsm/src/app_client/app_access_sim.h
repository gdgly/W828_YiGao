#ifndef _APP_API_H
#define _APP_API_H


#include "std.h"
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>


#include "usock.h"
#include "gbk2unicode.h"



#define ERR_OPERATION_NOT_ALLOWED	-3
#define ERR_SIM_NOT_INSERTED		-10
#define ERR_SIM_PIN_REQUIRED		-11
#define ERR_SIM_PUK_REQUIRED		-12
#define ERR_SIM_FAILURE				-13
#define ERR_SIM_WRONG				-15
#define ERR_INCORRECT_PASSWORD		-16
#define ERR_SIM_PUK2_REQUIRED		-18



typedef union _accessSIM_u
{
	//phb_SIM_get_storage_t
	struct 
	{
		int used;
		int total;
	}storage;

	//phb_SIM_read_node_t,phb_SIM_write_node_t
	struct 
	{
		int 		index;
		char 		*number; //[GSMD_PB_NUMB_MAXLEN+1];
		u_int8_t 	type;	 //��ο�enum lgsm_pb_addr_type
		char 		*name;	 //[GSMD_PB_TEXT_MAXLEN+1];
	}rwNode;
	
	//phb_SIM_read_allNode_t
	struct
	{
		int 	total;
		struct 	gsmd_phonebook *allNode;
	}readAllNode;
	
	//phb_SIM_del_node_t,phb_SIM_del_allNode_t
	struct
	{
		int delNum;
		int *delNodeIndex;
	}delNode;
}accessSIM_u;



typedef union _accessPhone_u
{
	u_int8_t simSerial[16];
	
	struct
	{
		int funMode;
		int result;
	}cfun;

	int simDetect;

}accessPhone_u;


typedef union _accessNet_u
{
	int netreg_stat;
	
	//��������
	struct
	{
		int 		index;	 //index:0,1
		char 		*number; //[GSMD_PB_NUMB_MAXLEN+1];
		u_int8_t 	type;	 //��ο�enum lgsm_pb_addr_type
		char 		*name;	 //[GSMD_PB_TEXT_MAXLEN+1];
	}rwNode;
	
	struct
	{
		int 	total;
		struct 	gsmd_phonebook *allNode;	//MALLOC(2*sizeof(struct gsmd_phonebook))
	}ownNumber;

	char operName[30+1];		//��ǰ������Ӫ��,���й��ƶ�Ϊ"CHINA MOBILE"/"CMCC"/"46000"

}accessNet_u;



enum call_fwd_reason {
	CALL_FWD_REASON_UNCOND				= 0,
	CALL_FWD_REASON_BUSY				= 1,
	CALL_FWD_REASON_NO_REPLY			= 2,
	CALL_FWD_REASON_NOT_REACHABLE		= 3,
	CALL_FWD_REASON_ALL_FORWARD			= 4, //���ڹر����к���ת��
	//CALL_FWD_REASON_ALL_COND_FORWARD	= 5, 
};


typedef union _accessVoice_u
{
	struct
	{
		enum call_fwd_reason reason;
		
		u_int8_t status;	//0-not active	1-active
		u_int8_t classx;
		char number[GSMD_ADDR_MAXLEN+1];
		u_int8_t time;
	}call_fwd;

}accessVoice_u;


enum pin_type {				/* waiting for ... */
	PIN_READY		= 0,	/* not pending for any password */
	PIN_SIM_PIN		= 1,	/* SIM PIN */
	PIN_SIM_PUK		= 2,	/* SIM PUK */
	PIN_PH_SIM_PIN	= 3,	/* phone-to-SIM passowrd */
	PIN_PH_FSIM_PIN	= 4,	/* phone-to-very-first SIM password */
	PIN_PH_FSIM_PUK	= 5,	/* phone-to-very-first SIM PUK password */
	PIN_SIM_PIN2	= 6,	/* SIM PIN2 */
	PIN_SIM_PUK2	= 7,	/* SIM PUK2 */
	PIN_PH_NET_PIN	= 8,	/* netwokr personalisation password */
	PIN_PH_NET_PUK	= 9,	/* network personalisation PUK */
	PIN_PH_NETSUB_PIN	= 10, 	/* network subset personalisation PIN */
	PIN_PH_NETSUB_PUK	= 11,	/* network subset personalisation PUK */
	PIN_PH_SP_PIN	= 12,	/* service provider personalisation PIN */
	PIN_PH_SP_PUK	= 13,	/* service provider personalisation PUK */
	PIN_PH_CORP_PIN	= 14,	/* corporate personalisation PIN */
	PIN_PH_CORP_PUK	= 15,	/* corporate personalisation PUK */
	__NUM_PIN
};


enum lock_type {
	LOCK_SC	= 0,	/* SIM (lock SIM card) */
	LOCK_AO	= 1,	/* ��ֹ���г��ֺ��� */
	LOCK_OI	= 2,	/* ��ֹ���й��ʳ��ֺ��� */
	LOCK_OX	= 3,	/* ��ֹ���й��ʳ��ֺ��У����������� */
	LOCK_AI	= 4,	/* ��ֹ������ֺ��� */
	LOCK_IR	= 5,	/* ��������������ʱ����ֹ������ֺ��� */
	LOCK_AB	= 6,	/* All Barring services (refer GSM02.30[19]) */
	LOCK_AG	= 7,	/* All out Going barring services (refer GSM02.30[19]) */
	LOCK_AC	= 8,	/* All in Coming barring services (refer GSM02.30[19]) */
	LOCK_PN	= 9,	/* ������Ի�����ο�GSM 02.22 [33]�� */
	LOCK_PU	= 10, 	/* �����Ӽ����Ի�����ο�GSM 02.22 [33]�� */
	LOCK_PP	= 11,	/* ����Ӧ�̸��Ի�����ο�GSM 02.22 [33]�� */
	LOCK_PC	= 12,	/* ��˾���Ի�����ο�GSM 02.22 [33]�� */
	LOCK_P2	= 13,	/* SIM PIN2 */
	__NUM_LOCK
};



typedef union _accessSecurity_u
{
	struct
	{
		enum pin_type type;
		char pin[GSMD_PIN_MAXLEN+1];
		char newpin[GSMD_PIN_MAXLEN+1];
	}pin;

	struct
	{
		int pin;
		int pin2;
		int puk;
		int puk2;
	}pin_times;

	struct
	{
		enum lock_type type;	//type=FAC_SC����PIN���������������ѯ״̬
		char pwd[GSMD_PIN_MAXLEN+1];
		char newpwd[GSMD_PIN_MAXLEN+1];
	}lock;

}accessSecurity_u;



extern u_int32_t appLockAccessSIM;


/*-----------------------------------------------------------------------------
* ����:	net_query_reg
* ����:	ʵʱ��ѯGSM/CDMAע��״̬(ע��״̬��accessNet_u�ṹ�е�netreg_stat���أ�
*		netreg_stat  ---  -1:δע��  0:����ע��   1:��ע��)
* ����:	accessNet_u *v
* ����:	0:SUCCESS(��ѯ�ɹ�)	-1:FAIL(��ѯʧ��)
*----------------------------------------------------------------------------*/
int net_query_reg(accessNet_u *v);


/*-----------------------------------------------------------------------------------
* ����:	net_query_reg_nrt
* ����:	��ʵʱ��ѯGSM/CDMAע��״̬(����Ҫ�󲻵ȴ��Ҷ�������ֵ��Ҫ��ʵʱֵ�ĵط�����)
*		
* ����:	
* ����:	-1:δע��  0:����ע��   1:��ע��
*-----------------------------------------------------------------------------------*/
int net_query_reg_nrt(void);


/*-----------------------------------------------------------------------------------
* ����:	net_query_sigq_set
* ����:	��ʵʱ��ѯGSM/CDMA�ź�ǿ��(����Ҫ�󲻵ȴ��Ҷ�������ֵ��Ҫ��ʵʱֵ�ĵط�����)
*		
* ����:	
* ����:	�ź�ǿ�� 0-31,99
*-----------------------------------------------------------------------------------*/
u_int32_t net_query_sigq_nrt(void);


/*-----------------------------------------------------------------------------
* ����:	net_get_subscriber_num
* ����:	��ѯ��������
*		
* ����:	accessNet_u *v
* ����:	0:SUCCESS(��ѯ�ɹ�)	-1:FAIL(��ѯʧ��)
*----------------------------------------------------------------------------*/
int net_get_subscriber_num(accessNet_u *v);



/*-----------------------------------------------------------------------------
* ����:	net_set_subscriber_num
* ����:	���ñ�������
*		
* ����:	accessNet_u *v
* ����:	0:SUCCESS(���óɹ�)	-1:FAIL(����ʧ��)
*----------------------------------------------------------------------------*/
int net_set_subscriber_num(accessNet_u *v);


/*-----------------------------------------------------------------------------
* ����:	net_get_operator
* ����:	��ѯ��ǰ������Ӫ��
*		
* ����:	accessNet_u *v
* ����:	0:SUCCESS(��ѯ�ɹ�)	-1:FAIL(��ѯʧ��)
*----------------------------------------------------------------------------*/
int net_get_operator(accessNet_u *v);


/*-----------------------------------------------------------------------------
* ����:	net_register_operator
* ����:	��������Ӫ��ע��
*		
* ����:	accessNet_u *v
* ����:	0:						SUCCESS(ע��ɹ�)
		ERR_SIM_NOT_INSERTED:	û�м�⵽SIM��
		ERR_SIM_WRONG:			SIM�����󣬸�SIM����ע��
		others:					δ֪����
*----------------------------------------------------------------------------*/
int net_register_operator(accessNet_u *v);


/*-----------------------------------------------------------------------------
* ����:	net_deregister_operator
* ����:	��������ע��ע��
*		
* ����:	accessNet_u *v
* ����:	0:						SUCCESS(ע���ɹ�)
		ERR_SIM_NOT_INSERTED:	û�м�⵽SIM��
		others:					δ֪����
*----------------------------------------------------------------------------*/
int net_deregister_operator(void);





/*-----------------------------------------------------------------------------
* ����:	phb_SIM_check_ready_t
* ����:	���SIM���绰���Ƿ���׼����
* ����:	
* ����:	0:Ready   -1:Check Fail   -2:SIM not inserted
*----------------------------------------------------------------------------*/
int phb_SIM_check_ready_t(void);


/*-----------------------------------------------------------------------------
* ����:	phb_SIM_get_storage_t
* ����:	��ȡSIM���绰�����ÿռ䡢�ܴ洢�ռ�
* ����:	accessSIM_u *v
* ����:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phb_SIM_get_storage_t(accessSIM_u *v);


/*-----------------------------------------------------------------------------
* ����:	phb_SIM_read_node_t
* ����:	��ȡSIM���绰����ָ��index�ļ�¼
* ����:	accessSIM_u *v
* ����:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phb_SIM_read_node_t(accessSIM_u *v);


/*-----------------------------------------------------------------------------
* ����:	phb_SIM_read_allNode_t
* ����:	��ȡSIM���绰�������м�¼
* ����:	accessSIM_u *v
* ����:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phb_SIM_read_allNode_t(accessSIM_u *v);


/*-----------------------------------------------------------------------------
* ����:	phb_SIM_write_node_t
* ����:	д��SIM���绰����ָ��index�ļ�¼
* ����:	accessSIM_u *v
* ����:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phb_SIM_write_node_t(accessSIM_u *v);

/*-----------------------------------------------------------------------------
* ����:	phb_SIM_write_node_t
* ����:	д��SIM���绰����ָ��index�ļ�¼
* ����:	accessSIM_u *v
* ����:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phb_SIM_del_node_t(accessSIM_u *v);





/*-----------------------------------------------------------------------------
* ����:	phone_detect_simcard
* ����:	����Ƿ���SIM����AT+ESIMS �C Query SIM Status
* ����:	
* ����:	1:�п�	0:�޿�	-1:���ʧ��
*----------------------------------------------------------------------------*/
int phone_detect_simcard(void);


/*-----------------------------------------------------------------------------
* ����:	phone_detect_simcard_nrt
* ����:	��ʵʱ����Ƿ���SIM��(����Ҫ�󲻵ȴ��Ҷ�������ֵ��Ҫ��ʵʱֵ�ĵط�����)
* ����:	
* ����:	1:�п�	0:�޿�
*----------------------------------------------------------------------------*/
int phone_detect_simcard_nrt(void);


/*-----------------------------------------------------------------------------
* ����:	phone_get_sim_imsi
* ����:	��ȡSIM����IMSI(International Mobile Subscriber Identity),�������ƶ���
*		��ʶ����
* ����:	accessPhone_u *v
* ����:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phone_get_sim_imsi(accessPhone_u *v);


/*-----------------------------------------------------------------------------
* ����:	phone_get_IMEI
* ����:	��ȡģ���IMEI(international mobile equipment identifier),�������ƶ���
*		��ʶ����
* ����:	accessPhone_u *v
* ����:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phone_get_IMEI(accessPhone_u *v);


/*-----------------------------------------------------------------------------
* ����:	phone_set_fun_mode
* ����:	����GSMģ��Ĺ���ģʽ
* ����:	funMode  ---  1:����ģʽ  0:����ģʽ
* ����:	0:SUCCESS	other:FAIL
*----------------------------------------------------------------------------*/
int phone_set_fun_mode(int funMode);





/*---------------------------------------------------------------------------------
* ����:	security_check_pin_status
* ����:	��ѯSIM���Ƿ���Ҫ���룬��������ֵΪ0ʱv->pin.type��������Ҫ���������ͣ����
		����ΪPIN_READY�������������
		1)������Ҫ��������
		2)SIM��PUK���ѱ�������ͨ����ѯPUK�����������Ƿ�Ϊ0��ȷ��SIM���ѱ�����
* ����:	accessSecurity_u *v
* ����:	0:						SUCCESS
		ERR_SIM_NOT_INSERTED:	û�м�⵽SIM��   
		others:					δ֪����
*---------------------------------------------------------------------------------*/
int security_check_pin_status(accessSecurity_u *v);


/*---------------------------------------------------------------------------------
* ����:	security_pin_input
* ����:	����SIM�����룬��������ֵΪ0ʱ������ȷ
* ����:	accessSecurity_u *v
* ����:	0:						SUCCESS
		ERR_SIM_PUK_REQUIRED:	��Ҫ����SIM��PUK��(����PUK��ʱ����Ҫͬʱ�����µ�PIN��)
		ERR_SIM_FAILURE:		SIM����ʧ�ܣ���ʱ��Ҫ�ٲ�ѯPUK�����������Ƿ�
								Ϊ0�����Ϊ0˵��SIM���ѱ�����
		ERR_INCORRECT_PASSWORD:	���벻��ȷ
		ERR_SIM_PUK2_REQUIRED:	��Ҫ����SIM��PUK2��(����PUK��ʱ����Ҫͬʱ�����µ�PIN2��)
		others:					δ֪����
*---------------------------------------------------------------------------------*/
int security_pin_input(accessSecurity_u *v);


/*---------------------------------------------------------------------------------
* ����:	security_pin_remain_times
* ����:	��ѯSIM��������������
* ����:	accessSecurity_u *v
* ����:	0:						��ѯ�ɹ�
		others:					δ֪����
*---------------------------------------------------------------------------------*/
int security_pin_remain_times(accessSecurity_u *v);


/*---------------------------------------------------------------------------------
* ����:	security_lock_en
* ����:	�����豸��������(��PIN��������)
* ����:	accessSecurity_u *v
* ����:	0:						���������ɹ�
		ERR_SIM_PUK_REQUIRED:	��Ҫ����SIM��PUK��(����PUK��ʱ����Ҫͬʱ�����µ�PIN��)
		ERR_INCORRECT_PASSWORD:	���벻��ȷ
		others:					δ֪����
*---------------------------------------------------------------------------------*/
int security_lock_en(accessSecurity_u *v);


/*---------------------------------------------------------------------------------
* ����:	security_lock_dis
* ����:	����豸��������(��PIN��������)
* ����:	accessSecurity_u *v
* ����:	0:						����������ɹ�
		ERR_SIM_PUK_REQUIRED:	��Ҫ����SIM��PUK��(����PUK��ʱ����Ҫͬʱ�����µ�PIN��)
		ERR_INCORRECT_PASSWORD:	���벻��ȷ
		others:					δ֪����
*---------------------------------------------------------------------------------*/
int security_lock_dis(accessSecurity_u *v);


/*---------------------------------------------------------------------------------
* ����:	security_lock_status
* ����:	��ѯ�豸����״̬(��PIN��������)
* ����:	accessSecurity_u *v
* ����:	0:						�豸����δ����
		1:						�豸����������
		ERR_SIM_PIN_REQUIRED:	��Ҫ����SIM��PIN��(����ʱ)
		ERR_SIM_PUK_REQUIRED:	��Ҫ����SIM��PUK��(����PUK��ʱ����Ҫͬʱ�����µ�PIN��)
		ERR_SIM_FAILURE:		SIM����ʧ�ܣ���ʱ��Ҫ�ٲ�ѯPUK�����������Ƿ�
								Ϊ0�����Ϊ0˵��SIM���ѱ�����
		others:					δ֪����
*---------------------------------------------------------------------------------*/
int security_lock_status(accessSecurity_u *v);


/*---------------------------------------------------------------------------------
* ����:	security_change_pwd
* ����:	�޸�����(��PIN���)
* ����:	accessSecurity_u *v
* ����:	0:							�޸�����ɹ�
		ERR_OPERATION_NOT_ALLOWED:	����������(�����豸����δ����)
		ERR_SIM_PUK_REQUIRED:		��Ҫ����SIM��PUK��(����PUK��ʱ����Ҫͬʱ�����µ�PIN��)
		ERR_INCORRECT_PASSWORD:		���벻��ȷ
		ERR_SIM_PUK2_REQUIRED:		��Ҫ����SIM��PUK2��(����PUK��ʱ����Ҫͬʱ�����µ�PIN2��)
		others:						δ֪����
*---------------------------------------------------------------------------------*/
int security_change_pwd(accessSecurity_u *v);





/*-----------------------------------------------------------------------------
* ����:	voice_call_fwd_dis
* ����:	����ָ������(������accessVoice_u�ṹ�е�reasonָ��)�ĺ���ת�ƣ���������
*		�û����ú���ܽ���
* ����:	
* ����:	0:SUCCESS   -1:FAIL
*----------------------------------------------------------------------------*/
int voice_call_fwd_dis(accessVoice_u *v);


/*-----------------------------------------------------------------------------
* ����:	voice_call_fwd_en
* ����:	����ָ������(������accessVoice_u�ṹ�е�reasonָ��)�ĺ���ת��
* ����:	
* ����:	0:SUCCESS   -1:FAIL
*----------------------------------------------------------------------------*/
int voice_call_fwd_en(accessVoice_u *v);


/*-----------------------------------------------------------------------------
* ����:	voice_call_fwd_stat
* ����:	��ѯָ������(������accessVoice_u�ṹ�е�reasonָ��)�ĺ���ת������
* ����:	
* ����:	0:SUCCESS   -1:FAIL
*----------------------------------------------------------------------------*/
int voice_call_fwd_stat(accessVoice_u *v);


/*-----------------------------------------------------------------------------
* ����:	voice_call_fwd_reg
* ����:	����ָ������(������accessVoice_u�ṹ�е�reasonָ��)�ĺ���ת�ƣ����ú��
*		�Ѿ�����
* ����:	
* ����:	0:SUCCESS   -1:FAIL
*----------------------------------------------------------------------------*/
int voice_call_fwd_reg(accessVoice_u *v);


/*-----------------------------------------------------------------------------
* ����:	voice_call_fwd_eras
* ����:	ɾ��ָ������(������accessVoice_u�ṹ�е�reasonָ��)�ĺ���ת������
* ����:	
* ����:	0:SUCCESS   -1:FAIL
*----------------------------------------------------------------------------*/
int voice_call_fwd_eras(accessVoice_u *v);


#endif