#ifndef _APP_SHELL_H
#define _APP_SHELL_H

#include "lgsm_internals.h"


struct sms_num_list
{
	u_int32_t num;
	u_int32_t used;
	u_int32_t total;
	struct gsmd_sms_list *sms;
};


#define SIMCARD_IMSI_LEN	16
struct phb_sim_info
{
	int ready;		//0-ready  -2-sim not inserted
	
	int ownNumTotal;
	
	int used;
	int total;
	
	char simSerial[SIMCARD_IMSI_LEN];
	
	struct gsmd_phonebook *allNode;	//ǰ2��λ�ô洢���������¼������Ϊ�绰����¼
};


//#define STEP_SET_NEW_SMS_IND 	111
//#define STEP_SET_SMS_FMT 	112
#define STEP_GET_PHB_STORAGE 	113
#define STEP_LIST_PHB 	114
//#define STEP_GET_SMS_STORAGE 115
enum shell_init_step
{
	STEP_OPEN_MUX_MODE = 0,
	STEP_OPEN_DLC0_MODE,
	STEP_OPEN_DLC1_PN,
	STEP_OPEN_DLC1_MODE,
	STEP_OPEN_DLC2_PN,
	STEP_OPEN_DLC2_MODE,

	STEP_GET_FUN_MODE,			//���+CFUNΪ4����ִ����һ��������ִ��STEP_DETECT_SIMCARD
	STEP_SET_FUN_MODE,			//����+CFUNΪ1
	STEP_DETECT_SIMCARD,		//����Ƿ���SIM������SIM��ִ����һ��������step��ֵΪINIT_STEP_NUM
	STEP_NET_REGISTER,			//ִ������ע��
	
	STEP_SET_NEW_SMS_IND,		//����CNMI����
	STEP_SET_SMS_FMT,			//����SMS��ʽ
	STEP_SET_COLP,	
	STEP_SET_CLIP,	
	STEP_GET_SIMCARD_IMSI,		//��ȡIMSI��
//INIT_STEP_NUM,
	//STEP_GET_PHB_STORAGE,	//15
	//STEP_LIST_PHB,
	STEP_GET_OWN_NUMBER,   
//INIT_STEP_NUM,	
	STEP_GET_SMS_STORAGE,//18
	STEP_LIST_SMS,
	STEP_DEL_SMS,	
INIT_STEP_NUM,
	STEP_NET_RE_REGISTER,
//INIT_STEP_NUM,
};


enum reset_step
{
	RESET_GET_FUN_MODE		= 0,	//���+CFUNΪ4����ִ����һ��������ִ��RESET_DETECT_SIMCARD
	RESET_SET_FUN_MODE,				//����+CFUNΪ1
	//RESET_DETECT_SIMCARD,			//����Ƿ���SIM������SIM��ִ����һ��������step��ֵΪRESET_STEP_NUM

	RESET_CHECK_PIN,
	RESET_INPUT_PIN,

	RESET_SET_NEW_SMS_IND,			//����CNMI����
	RESET_SET_SMS_FMT,				//����SMS��ʽ
	RESET_SET_COLP,
	RESET_SET_CLIP,

	RESET_NET_REGISTER,				//ִ������ע��
	RESET_STEP_NUM
};



extern int shell_main(struct lgsm_handle *lgsmh);
extern int pending_responses;



#endif