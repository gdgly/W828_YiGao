#include "ppp_processor.h"
#include "ppp_api.h"

#include "app_shell.h"
#include "app_client_includes.h"
#include "appControlProcess.h"
#include "grap_api.h"
#include "Common_Dlg.h"
#include "glbVariable.h"
#include "std.h"
#include "sys.h"
#include "ospriority.h"

 
SEMAPHORE	*ppp_p_Exit_sem = NULL;
int			*Eventppp = NULL;

U8			gu8ppp_dialState=-1;//ppp����״̬  0������ʧ��   1���ɹ�   -1:׼������

void ppp_clear_brg_flag(void);


const U8 *GPRS_accessPoint[4][4] = 
{
	/*����*/             /*�����*/ /*�����ַ*/  /*����˿�*/
	"�ƶ�GPRS���ӻ�����", "cmnet",   "10.0.0.172",   "80",
	"�ƶ�����GPRS",       "cmwap",   "10.0.0.172",   "80",
	"��ͨGPRS���ӻ�����", "uninet",  "10.0.0.172",   "80",
	"��ͨ�ֻ�����",       "uniwap",  "10.0.0.172",   "80",
} ;

U8 ppp_get_dialstate()
{
	return gu8ppp_dialState;
}


void *ppp_dial_intial(void)
{
   	
   	//char *pBuf;
    int len;
    int rv;
    int i;

    #define PPP_REG_APN_TIMER  (300)
    #define PPP_DIAL_TIMER     (500)
    
    if(GsmReset_IfReset() == 1)
	{//ģ��������,������GPRS
		return NULL;
	}
	
    syssleep(2);
    /*pBuf = (char *)malloc(3*1024);
    if (NULL == pBuf)
   	{
   	    return NULL;
   	}*/
   	
	if(lgsmh_gprs != NULL)
	{
		if(GsmReset_IfReset() == 1)
    	{
    		free(lgsmh_gprs);
    	}
    	else
    	{
    		lgsm_exit(lgsmh_gprs);
    	}
    	lgsmh_gprs = NULL;
		//hyUsbPrintf("ppp alive ===\r\n");
		return NULL;
	}
   	lgsmh_gprs = NULL;
	lgsmh_gprs = lgsm_init(USER_GPRS);
	
	if (!lgsmh_gprs)
	{
		//free(pBuf);
		return NULL;
	}
	
	if (gtHyc.GprsAccess >= 4)
	{
	    goto  ext;
	}
	
	//regiest apn
	lgsm_gprs_set_APN(lgsmh_gprs,GPRS_accessPoint[gtHyc.GprsAccess][1]);
	
    for (i=0; i<PPP_REG_APN_TIMER; i++)
    {
    	rv = gprs_get_data(NULL,&len);
    	   
    	if (PPP_RT_CGT == rv) break;
    	else if (PPP_RT_DIS == rv || i == PPP_REG_APN_TIMER-2) 
    	{
    	    goto ext;
     	}
     	else
     	{
     	    syssleep(1);
     	}     	
	}
	
	//dial at*99#
	gprs_connect_intial();	
	
    
    for (i=0; i<PPP_DIAL_TIMER; i++)
    {
        rv = gprs_get_data(NULL,&len);

        if (PPP_RT_CON == rv) break;
        else if (PPP_RT_DIS == rv || i == PPP_DIAL_TIMER-2) 
        {
           goto ext;
        }
        else
        {
            syssleep(1);
        } 
    }
    
    return  lgsmh_gprs;
ext:
    //free(pBuf);
    lgsm_exit(lgsmh_gprs);
    lgsmh_gprs = NULL;
    return NULL;
}

void ppp_close_flag(void);
void PPP_RecvIpClose(void);


void ppp_WakeUp()
{
	if(Eventppp != NULL)
	{
	//hyUsbPrintf("wakeup ppp ====\r\n");
		gu8ppp_dialState = -1;
		gprs_set_status(HY_ERROR);
		ewakeup(&Eventppp);
	}
}

void ppp_p_Exit(void)
{
	int pid;
	
	pid = ntopid(PPP_NAME);
	if (pid<0 || pid>30) return;
	
	if (ppp_p_Exit_sem != NULL)
		semaphore_post(ppp_p_Exit_sem);
	
	//if(0 == gprs_get_status())//ppp������״̬
	{
		ppp_close();
		ppp_clear_brg_flag();
	}
	
	ewakeup(&Eventppp);
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	    //PhoneTrace(0,"close ppp");
	}
	
	deletep(pid);
}

//U32 pppcloset;
int ppp_ThreadMethod(void)
{
    int rv,len;
    int count = 0;
    //char *pBuf;
    int ret;
    unsigned ilev;
 
 	gprs_set_status(HY_ERROR);
 	
    ppp_p_Exit_sem = semaphore_init(0);
    gu8ppp_dialState = -1;
    while(1)
    {
    	ilev = setil(_ioff);
		ewait(&Eventppp);
		(void) setil(ilev);
		
		PPP_InitMemory_buf();
	    ppp_close_flag();
	    
		ret = semaphore_trywait(ppp_p_Exit_sem);
		if(ret == 0)
		{
			gu8ppp_dialState = 0;
			break;
		}
		
		if(NULL == ppp_dial_intial())
		{
		hyUsbPrintf("ppp_dial_intial fail ------ \r\n");
			gu8ppp_dialState = 0;
			continue;
		}
		hyUsbPrintf("ppp_dial_intial OK ------ \r\n");
		gu8ppp_dialState = 1;
		count = 0;
		
	    ppp_intial();
	hyUsbPrintf("ppp open \r\n");
		ret = ppp_open();
		gprs_set_status(HY_ERROR);
	hyUsbPrintf("ppp open  ret === %d \r\n",ret);
		if(0 == ret)
		{
			while(1)
	    	{
	    		if(GsmReset_IfReset() == 1)
	    		{
	    			break;
	    		}
	    	    rv = gprs_get_data(NULL, &len);
	    	    if (++count > 400)
	    	    {
	    	        break;
	    	    }
	    	    else if (rv == PPP_RT_DIS)
	    	    {
	    	        break;
	    	    }
	    	    else
	    	    {
	    	        syssleep(1);
	    	    }			  	   
	        }
		}
	//pppcloset = ABS(rawtime(NULL)-pppcloset);
		if(lgsmh_gprs)
	    {
	    	if(GsmReset_IfReset() == 1)
	    	{
	    		free(lgsmh_gprs);
	    	}
	    	else
	    	{
	    		lgsm_exit(lgsmh_gprs);
	    	}
	    	
	    	lgsmh_gprs = NULL;
		}
	//pppquit = 3;	
		ppp_delete();
		
		hyUsbPrintf("ppp clear!!!\r\n");
	    if(GsmReset_IfReset() == 0)
	    {
	    	modem_sleep_enable();
	    	hyUsbPrintf("netgprs close!!!\r\n");
	    }
	    
	    ret = semaphore_trywait(ppp_p_Exit_sem);
		if(ret == 0)
		{
			break;
		}	
    }
    
	semaphore_destroy(ppp_p_Exit_sem, OS_DEL_NO_PEND);
	ppp_p_Exit_sem = NULL;
	Eventppp = NULL;
	gu8ppp_dialState = 0;
	//hyUsbPrintf("ppp proc Exit!!!\r\n");
	return 1;
}

int ppp_p_Start(void)
{
	int pid, startValue, pidStatus = HY_ERROR;
	
	pid = ntopid(PPP_NAME);
	if (pid>0 && pid<30)
	{
		//���̴��ڣ��ж�״̬
		if (PSPROC(pid)->state == P_INACT)
		{
			//deletep(pid);//ɾ�����ٴ���
			pidStatus = HY_OK;//���̴����Ҵ���exit״̬����ֱ������
		}
		else
		{
			ewakeup(&Eventppp);
			//hyUsbPrintf("ppp  wakeup\r\n");
			return HY_ERROR;//���̻������У�ֱ���˳�����ǰ������Ч
		}
	}

	if (pidStatus != HY_OK)
	{
		//���̲����ڣ���Ҫ����
		pid = createp((int (*)())ppp_ThreadMethod,
								PPP_NAME,
								PPP_SIZE,
								PPP_PRIOPITY,
								NULL,NULL,NULL);
	}
	startValue = pstart( pid, (void *)ppp_ThreadMethod );
	return HY_OK;
}

   
//---------------------------�ṩ�������ⲿ������----------------------------------------------
static volatile U8 gu8PPPClose = 0; //0�ر� 1��Ҫ�´��ٴ�����ppp
/*-----------------------------------------
* ������ppp_close_ext
* ���ܣ��ⲿ���ýӿ�,�ر�Э��ջ���Ҽ�¼״̬
* ������none
* ���أ�none
*-------------------------------------------*/
void ppp_close_flag(void)
{
     gu8PPPClose = 1;
}

/*-----------------------------------------
* ������ppp_start_brg
* ���ܣ���������Э��ջ����Ҫ��¼״̬
* ������none
* ���أ�-1 gprs����ʧ��  0 �ɹ�
*-------------------------------------------*/
static int ppp_start_brg(void)
{
     int rv = -1;
     int i;
     
     if (1 != gu8PPPClose) 
     {
         rv = 1;
         return;
     }         
     
     gu8PPPClose = 0;
     
     for (i=0; i<3; i++)
     {
     
         syssleep(200);
         
         if ( NULL != ppp_dial_intial())
         {
              AppCtrl_CreatPro(PPP_AN , 0); 
              return;
         }        
     }
}

/*-----------------------------------------
* ������ppp_clear_brg_flag
* ���ܣ������¼
* ������none
* ���أ�none
*-------------------------------------------*/
void ppp_clear_brg_flag(void)
{
     gu8PPPClose = 0;
}


