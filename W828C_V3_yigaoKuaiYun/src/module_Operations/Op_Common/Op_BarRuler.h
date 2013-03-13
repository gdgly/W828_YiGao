#ifndef _OP_BARRULER_H
#define _OP_BARRULER_H


#include "hyTypes.h"
#include "Op_Config.h"




typedef enum _barcode_type_e
{
	TYPE_EMPLOYEID=1,	//Ա��ID
	TYPE_COMPANYID,		//������
	TYPE_BARCODE,		//�˵���
	TYPE_CARID,			//������
	TYPE_CARLINE,		//������
	TYPE_PACKET,		//�����
	TYPE_BAKCODE,		//�ص���
	TYPE_TAG,			//��ǩ
	
	TYPE_NULL=255
	
}eBARCODE_TYPE;




#endif //_OP_BARRULER_H