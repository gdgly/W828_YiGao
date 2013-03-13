#ifndef _OP_REQUESTINFO_H
#define _OP_REQUESTINFO_H


#include "hyTypes.h"
#include "Op_Config.h"


//��������ӿ�����  
typedef enum
{
	REQ_COMPANY=0,	//0 ��ѯ��˾
	REQ_EMPLOYEE,	//1 ��ѯԱ��
	REQ_PDATIME,	//2 �����ն�ʱ��
	REQ_RIGHT,		//3 ��ѯȨ�� ��¼
	REQ_LUDAN,		//4 ¼��
	REQ_GETLUNDAN,  //5 ��ȡ¼����Ϣ
	REQ_PACKET,		//6 ��ѯ����
	REQ_UPGRADE,	//7 ��ѯ����
	REQ_LOCATION,	//8 �ϴ�λ����Ϣ
	REQ_GPS,		//9 ����λ����Ϣ
	
	REQ_DOWNSITE,   //10��ȡ��������
	// new -----------------------------
	REQ_LIU,		//11 ����
	REQ_SAO,		//12 ɨ��
	REQ_PHOTO,		//13 ����
	REQ_CHENG,		//14 ����
	REQ_XIE,		//15 ж��
	REQ_ZHUANG,		//16 װ��
	REQ_JIE,		//17 �⳵
	REQ_FENG,		//18 �⳵
	REQ_MSG ,       //19 ֪ͨ
	REQ_WJP,		//20 ��ҳ����
	REQ_ZXGD,		//21 ����
	//-----------------------------------
	REQ_GETWEIVOL,  //22 ���������ȡ	
		 
	REQ_GROUP,		//23 ��ѯ����   NOUSE
	REQ_PROBLEM,	//24 ��ѯ����� NOUSE		
	
	REQ_YICHANGSTR,	//25 ��ȡ�쳣����	
	
	REQ_SIMID,		//26 ��ȡsim����
	
	REQ_ER_COMPANY,	//27 Ƿ�ѹ�˾
	
	REQ_ALL
	
}eNET_REQTYPE;


///////////////////////////extern////////////////////////
extern eNET_REQTYPE	geReqType;

extern const U8 *Req_ver[];
extern const U8 * const Req_name[];
extern const U8 * const Net_ErrorCode[];



#endif//_OP_REQUESTINFO_H