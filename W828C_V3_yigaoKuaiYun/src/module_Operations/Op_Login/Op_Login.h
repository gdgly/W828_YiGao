#ifndef _LOGIN_H_
#define _LOGIN_H_

#include "hyTypes.h"
#include "Op_Common.h"
#include "Op_Config.h"




//����Ա�˺�
#define	ADMIN_ID		"00000000"
#define ADMIN_PW		"34290323"

//Ӳ�������˺�
#define HARDWARE_ID		"0000"
#define HARDWARE_PW		"4926"

//ҵ������˺�
#define OPTEST_ID		"000000"
#define OPTEST_PW		"021"


//������Ϣ
typedef struct _tGroupInfo
{
	U8  groupId[12];	//������
	U8  groupName[52];	//������

}tGROUPINFO;


//��¼��Ϣ
typedef struct _logoinfo_t
{
	U8	userID[20];		//Ա�����
	U8	userPw[20];		//Ա������
	U8	userName[32];	//Ա������
	U8	userRight;		//Ա��Ȩ��
	U8	userType;		//Ա������
	U8  groupTotal;		//��������
	U8  groupDef;		//Ĭ�Ϸ���
	U8  groupSelect;	//Ա�����ڷ�������
	
	struct _tGroupInfo	*pGroup;	//Ա�����ڷ�������
	
	U32 landTime;		//�ϴ���֤ʱ��
	
}tLOGOINFO;




















void Contact_Desk(void *pDeskFather);



#endif//_LOGIN_H_