#ifndef _COMMON_WIDGET_H_
#define _COMMON_WIDGET_H_

/*
*include standard files
*/
#include "hyTypes.h"

/*
*include local files
*/
#include "grap_api.h"
#include "LcdAPIFunc.h"
#include "base.h"
#include "hyhwRtc.h"


#define COM_WIDGET_NUM   		7


#define COM_LOGO_ID			(EDIT_ID_BASE-1)//logo
#define COM_GPRSEVT_ID		(EDIT_ID_BASE-2)//gprs �¼�
#define COM_DATE_ID			(EDIT_ID_BASE-3)//����ʱ��
#define COM_GSMSIG_ID		(EDIT_ID_BASE-4)//gsm�ź�
#define COM_WIFISIG_ID		(EDIT_ID_BASE-5)//wifi�ź�
#define COM_BAT_ID			(EDIT_ID_BASE-6)//����
#define COM_INPUT_ID		(EDIT_ID_BASE-7)//���뷨



#define LOGO_X		0
#define LOGO_Y		3
#define LOGO_W		68
#define LOGO_H		23

#define GPRS_X		70
#define GPRS_Y		0
#define GPRS_W		74
#define GPRS_H		16

#define DATE_X		145
#define DATE_Y		2
#define DATE_W		40
#define DATE_H		9

//��ʾ�������������������Ϳ������ܸ���,�����������ÿؼ�,�ɶ�Ӧedit��draw���
#define EDIT_X		145
#define EDIT_Y		12
#define EDIT_W		40
#define EDIT_H		16

#define	GSM_X		186
#define GSM_Y		7
#define GSM_W		31
#define GSM_H		13

#define	WIFI_X		186
#define WIFI_Y		7
#define WIFI_W		31
#define WIFI_H		13

#define BAT_X		219
#define BAT_Y		0
#define BAT_W		21
#define BAT_H		11

#define INPUT_X		219
#define INPUT_Y		14
#define INPUT_W		20
#define INPUT_H		10


#define	GSMWIFI_X1		186
#define GSMWIFI_Y1		0
#define GSMWIFI_X2		240
#define GSMWIFI_Y2		26

extern tGRAPLABEL *pgtComFirstChd;




/*---------------------------------------------------------
*����: ComWidget_CreatAllObj
*����: ���������ĵ�ض�������      
*����: none
*����: ��һ�������ؼ���ָ��
*---------------------------------------------------------*/
tGRAPLABEL *ComWidget_CreatAllObj(void);


/*---------------------------------------------------------
*����: ComWidget_DeleteAllObj
*����: �ͷŹ�����������,ֻ���ڹػ�ʱ������ʹ������������ͷ����й�������      
*����: none
*����: ��һ�������ؼ���ָ��
*---------------------------------------------------------*/
void ComWidget_DeleteAllObj(void *pView);

/*---------------------------------------------------------
*����: ComWidget_Link2Desk
*����: �ѹ����ռ����ӵ�desk��      
*����: pDesk��Ҫ���ӵ�deskָ�� pFirstCom:��һ���ؼ���ָ��
*����: 
*---------------------------------------------------------*/
void ComWidget_Link2Desk(void *pDesk,void *pFirstCom);

/*---------------------------------------------------------
*����: ComWidget_Link2Desk
*����: �ѹ����ռ����ӵ�desk��      
*����: pDesk��Ҫ���ӵ�deskָ�� pFirstCom:��һ���ؼ���ָ�� TotalChild:���봰��ؼ�����
*����: 
*---------------------------------------------------------*/
void ComWidget_LeftFromDesk(void *pDesk,void *pFirstCom,U32 TotalChild);

/*---------------------------------------------------------
*����: ComWidget_CheckObjAttar
*����: ����źź͵绰�ȹ���ͼ�꣬������     
*����: pDesk ����ָ��
*����: 
*---------------------------------------------------------*/
void ComWidget_CheckObjAttar(void *pView);







#endif