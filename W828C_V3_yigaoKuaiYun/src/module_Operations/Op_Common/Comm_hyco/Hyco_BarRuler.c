#include "Op_BarRuler.h"


#ifdef VER_CONFIG_HYCO

/*--------------------------------------
*������OpCom_CheckBarcode
*���ܣ�����ָ������������Ϸ���
*������pCode:����������
       codeType:��������
*���أ�1:�ɹ�    -1:ʧ��     0:û�й���
*--------------------------------------*/
int OpCom_CheckBarcode(U8 *pCode, eBARCODE_TYPE codeType)
{
	int ret = 0, len;
	
	len = strlen(pCode);//���볤��
	if(len == 0)
	{
		ret = -1;
	}
	switch(codeType)
	{
	case TYPE_EMPLOYEID:
		/*if(len < 12)//
		{
			ret = 1;
		}
		else
		{
			ret = -1;
		}*/
		break;
	case TYPE_COMPANYID:
		/*if(len < 12)//
		{
			ret = 1;
		}
		else
		{
			ret = -1;
		}*/
		break;
	case TYPE_BARCODE://V��2 ��ͷ��12λ��������
		//if(len == 12 && (pCode[0] == 'V' || pCode[0] == 'v' || pCode[0] == '2'))//
		/*if(len == 9 || len == 10 || len == 12 || hy_memcmp(pCode, "VIP", 3) == 0)
		{
			ret = 1;
		}
		else
		{
			ret = -1;
		}*/
		break;
	case TYPE_PACKET:
		/*if(len < 10 || len > 20)
		{
			ret = -1;
		}
		else
		{
			ret = 1;
		}*/
		break;
	default:
		break;
	}

	return ret;
}

#endif //VER_CONFIG_YUNDA