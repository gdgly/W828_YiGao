#include "hyTypes.h"
#include "hyErrors.h"
#include "appControlProcess.h"
#include "ppp_gprs.h"
#include "glbVariable.h"
#include "appControlProcess.h"
#include "glbVariable.h"
#include "socket_api.h"
#include "net_api.h"


/*-----------------------------------------
* ������gprs_start_connect
* ���ܣ���̨����GPRS����,��Ҫ��̨��������
* ������none
* ���أ�>=0  
*-------------------------------------------*/
int gprs_start_connect(void)
{
	int rv = HY_ERROR;
	int count = 0;
    int value;
    U8  state;
    
    //gsm û��ע��
    //if (1 != net_query_reg_nrt())
    if (1 != net_onlyRead_regStatus())
    {
         return rv;
    }
    
    //����ppp����
    ppp_WakeUp();
    syssleep(50);
    
    while(1)
    {
    	state = ppp_get_dialstate();
    	if(state == 0)//����ʧ��
    	{
    	//hyUsbPrintf("ppp dial fail \r\n");
    		syssleep(50);
    		return rv;
    	}
    	else if(state == 1)//���ųɹ�
    	{
    	//hyUsbPrintf("ppp dial success \r\n");
    		syssleep(50);
    		break;
    	}
    	else if(GsmReset_IfReset() == 1)
    	{
    		return rv;
    	}
    	else
    	{
    		syssleep(50);
    		ppp_WakeUp();
    	}
    }
    
    //hyUsbPrintf("ppp_get_status \r\n");
    //��Ҫ�ȴ��ɹ������tcp����
    while(1)
    {
    	 value = ppp_get_status();
    	     
	     if (1 == value) 
	     {
	         syssleep(100);
	         rv = HY_OK;
			 gprs_set_status(HY_OK);
	         break;
	     }    
	     else if (-1 == value) 
	     {
	        ppp_close(); 
            ppp_clear_brg_flag();
	        break;
	     }	     
		else if(GsmReset_IfReset() == 1)
		{
			break;
		}   
		else
		{
			syssleep(1);
		}
    }
    
    return rv;
}