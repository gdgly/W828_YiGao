 /*----------------------�ļ�����--------------------------------
* ��������: 2008/05/29 
* ����    : ����ϵͳ�Ĺ��Ŀ��Ʋ���
*--------------------------------------------------------------*/

/*
*include files
*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "hyhwCkc.h"

#include "AppControlProcess.h"
#include "AppCtrlConstData.h"
#include "AppControlConsume.h"

//#define NOT_CHANGE_SPEED

/*
*define const data
*/
static const  tAPP_CTR_CSM gtApp_Ctr_Csm[]=
{  
	{USB_HANDLER_AN,	192},
	{POWER_ON_AN,		120},
	{POWER_DOWN_AN,		120},
	{SHELL_AN,			48},
	{TCPIP_AN,			0},
	{KERNAL_APP_AN,		0},
	{GSMCDMA_SERVER_AN,	120},
	{GSMCDMA_CLIENT_AN,	120},
	{PPP_AN,			0},
	{WIFI_DRIVER_AN,	120},
	{BT_DRIVER_AN,		0},
	{UPLOAD_APP_AN,		0},//����̨ʱ��ͬ��ʹ��
	{GPS_DRIVER_AN,		96},

	{TOTAL_APP_AN,		0},
}; 

static const U32 geSysClc[]=
{       
    //sys clcock
    24, ICLK24M_AHB12M, 
//	36, ICLK36M_AHB18M,
    48, ICLK48M_AHB24M,         
//	50, ICLK50M_AHB25M,
//	52, ICLK52M_AHB26M,
//	54, ICLK54M_AHB27M,
//	56, ICLK56M_AHB28M,
//	58, ICLK58M_AHB29M,
//	60, ICLK60M_AHB30M,
    72, ICLK72M_AHB36M,
//	84, ICLK84M_AHB42M,
    96, ICLK96M_AHB48M,    
//	108,ICLK108M_AHB54M,
    120,ICLK120M_AHB60M,
//	132,ICLK132M_AHB66M,
//	144,ICLK144M_AHB72M,
//	156,ICLK156M_AHB78M,
//	168,ICLK168M_AHB84M,
//	180,ICLK180M_AHB90M,
    192,ICLK192M_AHB96M,    
//	240,ICLK240M_AHB120M,
//	250,ICLK250M_AHB125M,
//	260,ICLK260M_AHB130M,

//	360,ICLK360M_AHB120M,
    //399,ICLK399M_AHB133M,     
    
    0xffff   
};

/*
*define variable
*/
//��ǰ����ϵ��
static U32 gu32TotalCoef = 0; //�ܵĹ���ϵ��

static U16 gu8AppCoef[TOTAL_APP_AN]={0};

static U8 gu8TempChangeCoefFlag = 0;
static U32 gu32ChangeStartTime = 0;

/*-----------------------------------------------------------------------------
* ����:	wLock_Consume
* ����:	������Դ���벢lock
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void wLock_Consume(void)
{
#ifdef CONSUME_USE_WLOCK
	U32 *pConsume = (U32*)&gu32TotalCoef;
	(void)wlock((unsigned) pConsume);
#endif
}
/*-----------------------------------------------------------------------------
* ����:	unLock_NandFlash
* ����:	������Դ���벢lock
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void unLock_Consume(void)
{
#ifdef CONSUME_USE_WLOCK
	U32 *pConsume = (U32*)&gu32TotalCoef;
	(void)unlock((unsigned) pConsume);
#endif
}

/*---------------------------------------------------------------------------
 ������ConSume_CalculateICLK
 ���ܣ����������ϵͳƵ��ֵ����Ӧ������Ƶ�ʱ��У�û�и�Ƶ�ʵ�����ȡ��
 ������coef:��Ҫ�ı��Ƶ��
 ���أ�none
* -----------------------------------------------------------------------------*/
static eSYS_Clk_EN ConSume_CalculateICLK(U32 iclk)
{
    U32 i=0;
    
    while(1)
    {
        if (0xFFFF == geSysClc[i]) 
        {
            i -= 2;
            break;
        }
        
        if (iclk < geSysClc[0])
        {//���С��24M��24M����             
            break;
        }          
        else if (iclk <= geSysClc[i]) 
        {
            break;    
        }       
        
        i += 2;
    }
    
    return geSysClc[i+1];
}

/*---------------------------------------------------------------------------
 ������ConSume_ChangeMode
 ���ܣ��ı䵱ǰ��ϵͳ����Ƶ��
 ������none
 ���أ�none
* -----------------------------------------------------------------------------*/
//static 
void ConSume_ChangeMode(void)
{
	U8 interrupt;
	eSYS_Clk_EN clk;
	U32 curClk;
	
#ifdef NOT_CHANGE_SPEED
	return ;//kong 
#endif
	
    clk = ConSume_CalculateICLK(gu32TotalCoef);
    
    curClk = hyhwSysClk_GetCurValue();
    if (curClk == (clk>>8))
	{
		return;
	}

	//hyUsbPrintf("ConSume_ChangeMode = %08X\r\n", clk);
	//interrupt = InterruptDisable();
	hyhwSysClk_Set(clk,EXTAL_CLK);
	//InterruptRestore(interrupt);
}

/*---------------------------------------------------------------------------
 ������ConSume_CheckCurrentMode
 ���ܣ��ж��Ƿ���Ҫ�ı䵱ǰ��,���ֵ����120ʱ����ȡ���ģ�С��120ʱ�����㷨
 ������none
 ���أ�0������Ҫ�ı���� 1:��Ҫ�ı����
* -----------------------------------------------------------------------------*/
static U32 ConSume_CheckCurrentMode(void)
{
     #define CONSUME_BASE_MAX_COEF  (120)
     U32 ret = 0;
     U32 pro_coef,app_coef;
     tAPPLICATION *pApp;
     U32 i;
     U32 attribute = 0;
     int curCoef,maxCoef;
     int totalCoef;   
     U32 curClk;  
	static U32 testLastTick=0;
	U32 now, flag=0;
	
	now = rawtime(NULL);
	if ((now-testLastTick) >= 100)
	{
		flag = 1;
		testLastTick = now;
	}
     
     pro_coef = 0;
     app_coef = 0;     
     maxCoef = 0;
     
     for (i=0; i<TOTAL_APP_AN; i++)
     {
        pApp = AppCtrl_GetAppInfor(i);
        
        if (APPCTRL_INVALID_PID == pApp->pid) continue; 
        
        curCoef = 0; 
        
        attribute = gtproc_Config[i].attribute;
        if ( (attribute == APP_ATTR_SYSTEM_AA) ||
        	((attribute&APP_ATTR_SYSTEM_AA) == 0 && pApp->isFocus == 1))
        {
        	//��ϵͳ���̣���UI�ý��̴��ڽ���״̬
            pro_coef += gtApp_Ctr_Csm[i].coef;
            curCoef = gtApp_Ctr_Csm[i].coef;
        }
        app_coef += gu8AppCoef[i];
		//if (flag)
		//	hyUsbPrintf("idx=%d, app_coef=%d\r\n", i, gu8AppCoef[i]);
        
        curCoef += gu8AppCoef[i];
        
        if (maxCoef < curCoef)
        {
            maxCoef = curCoef;
        }
     }
     
     totalCoef = pro_coef+app_coef;
     
     if (totalCoef != 0)
     {
         if (maxCoef >= CONSUME_BASE_MAX_COEF)
         {
             curCoef = maxCoef;
         }
         else
         {
             curCoef = maxCoef+(totalCoef-maxCoef)*(totalCoef-maxCoef)/totalCoef;
         }    
     }
     else 
     {
         curCoef = 0;
     }
     
     //�����ǰ����ʱ�ı�Ƶ�ʲ��ҵ�ǰƵ�ʱȼ����Ƶ��
     curClk = hyhwSysClk_GetCurValue();
     
	if (((curClk>=curCoef) || ((curClk==360) && (curCoef>=360))) &&
		(1 == gu8TempChangeCoefFlag))
	{
	//	hyUsbPrintf("CheckCurrMode curCoef = %08X\r\n", curCoef);
          return ret;
     }
          
     if (curCoef != gu32TotalCoef)
     {//��Ҫ������
	//	hyUsbPrintf("gu32TotalCoef=%d, curCoef = %08X\r\n", gu32TotalCoef, curCoef);
          gu32TotalCoef = curCoef;
          ret = 1;    
     }   
     
     return ret;      
}

/*---------------------------------------------------------------------------
 ������ConSume_CoefInitial
 ���ܣ���ʼ������
 ������none
 ���أ�none
* -----------------------------------------------------------------------------*/
void ConSume_CoefInitial(void)
{
    gu8TempChangeCoefFlag = 0;
}

/*---------------------------------------------------------------------------
 ������ConSume_ChangeCoefTemp
 ���ܣ���ʱ�ı�һ��Ƶ�ʣ���Ҫ�������ڽ����л���ˢ������ʱ������ʱ�ĸо�,�����ǰƵ�ʱ�Ҫ�����ĸ��򲻸ı�Ƶ��
 ������coef:��Ҫ�ı��ϵ��
 ���أ�none
* -----------------------------------------------------------------------------*/
void ConSume_ChangeCoefTemp(U32 coef, int type)
{
    U8 interrupt;
    eSYS_Clk_EN clk;
    U32 curClk;
	
#ifdef NOT_CHANGE_SPEED
	return ;//kong
#endif

	wLock_Consume();
	
	if (coef < gu32TotalCoef)
	{
		unLock_Consume();
		return;
	}

	if (1==gu8TempChangeCoefFlag && type==0)
	{
		gu32ChangeStartTime = rawtime(NULL);
		unLock_Consume();
        return; 
    }
    
    clk = ConSume_CalculateICLK(coef);
    
    curClk = hyhwSysClk_GetCurValue();
    if (curClk != (clk>>8))
	{
		//hyUsbPrintf("ConSume_ChangeCoefTemp = %08X\r\n", clk);
		//interrupt = InterruptDisable();
		hyhwSysClk_Set(clk,EXTAL_CLK);
		//InterruptRestore(interrupt);
    }
    
    gu8TempChangeCoefFlag = 1;
	gu32ChangeStartTime = rawtime(NULL);
	unLock_Consume();
}

/*---------------------------------------------------------------------------
 ������ConSume_ChangeCoefTempElapseTime
 ���ܣ���ʱ�ı�һ��Ƶ�ʵ�ʱ���Ƿ�ʱ
 ������none
 ���أ�none
* -----------------------------------------------------------------------------*/
void ConSume_ChangeCoefTempElapseTime(void)
{
#ifdef NOT_CHANGE_SPEED
	return ;//kong
#endif
	
	if (1 != gu8TempChangeCoefFlag)
	{
		return;
	}

	wLock_Consume();
	if (ABS(rawtime(NULL)-gu32ChangeStartTime) > APP_CSM_ELAPSE_TIME)
	{
		ConSume_ChangeMode();
		gu8TempChangeCoefFlag = 0;
	}
	unLock_Consume();
}

/*---------------------------------------------------------------------------
 ������ConSume_UpdataAppCoef
 ���ܣ�����Ӧ�ý��̵�ϵ��,���ϵ���Ƕ�̬�ı��
 ������AppId:���̱��ö��  coef:��Ҫ�ı��ϵ��
 ���أ�none
* -----------------------------------------------------------------------------*/
void ConSume_UpdataAppCoef(eAPPLICATION_NO AppId, U32 coef)
{    
    gu8AppCoef[AppId] = coef;    
}

/*---------------------------------------------------------------------------
 ������ConSume_UpdataSpeedDirect
 ���ܣ�ֱ�Ӹ���Ƶ�ʣ�����Ҫ����̨ȥ�޸�Ƶ��
 ������AppId:���̱��ö��  coef:��Ҫ�ı��ϵ��
 ���أ�none
* -----------------------------------------------------------------------------*/
void ConSume_UpdataSpeedDirect(eAPPLICATION_NO AppId, U32 coef)
{
#ifdef NOT_CHANGE_SPEED
	return ;//kong
#endif

	gu8AppCoef[AppId] = coef;
	//hyUsbPrintf("Updata Direct AppId = %d\r\n", AppId);

	wLock_Consume();
	gu8TempChangeCoefFlag = 0;
	if (1 == ConSume_CheckCurrentMode())
	{
		ConSume_ChangeMode();
	}
	unLock_Consume();
}

/*---------------------------------------------------------------------------
 ������ConSume_Get_PidSpeed
 ���ܣ��õ���ǰpid��Ƶ��
 ������AppId:���̱��ö��  coef:��Ҫ�ı��ϵ��
 ���أ�none
* -----------------------------------------------------------------------------*/
U16 ConSume_Get_PidSpeed(eAPPLICATION_NO AppId)
{
	 return gu8AppCoef[AppId];
}

/*---------------------------------------------------------------------------
 ������ConSume_DetectCurCoef
 ���ܣ���̨����Ƶ��
 ������none
 ���أ�none
* -----------------------------------------------------------------------------*/
void ConSume_DetectCurCoef(void)
{
#ifdef NOT_CHANGE_SPEED
	return ;//kong
#endif
	
	wLock_Consume();
    //������ʱ�ı�Ƶ��ʱ����ʱ������Ƶ��
	if (1 == gu8TempChangeCoefFlag)
	{
		unLock_Consume();
		return;
	}

	if (1 == ConSume_CheckCurrentMode())
	{
		//hyUsbPrintf("ConSume_DetectCurCoef!!\r\n");
		ConSume_ChangeMode();
		gu8TempChangeCoefFlag = 0;
	}
	unLock_Consume();
}

//������
U32 ConSume_CurTotalCoef(void)
{
	return gu32TotalCoef;
}

void ConSume_UpdataSpeedDirect_USB(int coef)
{
	ConSume_UpdataSpeedDirect(USB_HANDLER_AN, coef);
}
