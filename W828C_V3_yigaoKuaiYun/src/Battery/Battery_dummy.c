/*----------------------�ļ�����--------------------------------
*��������: 08/03/14
*����    : ��ص���
*--------------------------------------------------------------*/
#include "hyTypes.h"
#include "Battery.h"

//Battery Detect Timer
#define BATTERY_DETECT_TIMER_INTR  10//200 //5s ����һ��
#define BATTERY_LEV_DIFFERENCE 	   8

/*
*DEFINE variable
*/
U32 gu32Battery_DetectCnt = 0;
volatile U32 gu32BatteryLev = 0xFFFFFFFF;//������

static U8 gu8ReadAdcCnt = 0; //adc ��ȡ�Ĵ���
//static U16 gu16ReadAdc[3] = {POWER_LEVER_3V3};
//static 
volatile U16 gu16ReadAdc[BATTERY_DETECT_CNT] = {POWER_LEVER_3V3};


extern UInt8 gScsiApplnScsiMaxLunFlag;


//#define BATTERY_WIRTE_LOG

#define BATTERY_FILE_NAME  "C:/battery.txt"
#define BATTERY_POOL_OFFSET  0x25600000
#define BATTERY_POOL_SIZE  0x500000
#define BATTERY_LOG_FLAG  "\n ----end-------\n"

#ifdef BATTERY_WIRTE_LOG


char *debug_batter_pool  = (char *)BATTERY_POOL_OFFSET;
int debug_batter_size = 0;
void Battery_CreatLog(void)
{
     debug_batter_pool = (char *)BATTERY_POOL_OFFSET;
     debug_batter_size = 0;
}

void Battery_WriteLog(void)
{
    int i;
    char tempBuf[200];
    int len;
    
    sprintf(tempBuf,"%x,%x,%x,%x,%x\n",gu16ReadAdc[0],gu16ReadAdc[1],gu16ReadAdc[2],gu16ReadAdc[3],gu16ReadAdc[4]);
    len = strlen(tempBuf); 
    sprintf(tempBuf+len,"%x,%x,%x,%x,%x\n",gu16ReadAdc[5],gu16ReadAdc[6],gu16ReadAdc[7],gu16ReadAdc[8],gu16ReadAdc[9]);
    len = strlen(tempBuf);     
    
    if (debug_batter_size+len > BATTERY_POOL_SIZE)
    {
         debug_batter_size = 0;
    }
    
    memcpy(debug_batter_pool+debug_batter_size,tempBuf,len);
    debug_batter_size += len; 
}

#endif

void Battery_CloseLog(void)
{
#ifdef BATTERY_WIRTE_LOG    
    
    int len;
    int fp;
    
    fp = AbstractFileSystem_Open(BATTERY_FILE_NAME,2);
    if (fp >=0) 
    {
        int size;
        size = AbstractFileSystem_FileSize(fp);
        AbstractFileSystem_Seek(fp,size,0);
    }
    else
    {
        fp = AbstractFileSystem_Create(BATTERY_FILE_NAME); 
    }    
    
    if (debug_batter_size+100 > BATTERY_POOL_SIZE)
    {
         debug_batter_size = 0;
    }
    
    len = strlen(BATTERY_LOG_FLAG); 
    memcpy(debug_batter_pool+debug_batter_size,BATTERY_LOG_FLAG,len);
    debug_batter_size += len;
    
    AbstractFileSystem_Write(fp,debug_batter_pool,debug_batter_size);    
    AbstractFileSystem_Close(fp);
    
#endif    
}

void Battery_WriteInfo()
{}

int hy_adc_read(int chl)
{
    int adc;
    
    adc = hyhwAdc_Read(chl);
    
    if (0x3ff == adc)
    {
        adc = -1;
    }
    
    return adc;
}

/*-----------------------------------------------------------------------------
* ����:	Battery_IntialVariable()
* ����:	������ʱ���ʼ������
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void Battery_IntialVariable(void)
{
    int i;
#ifdef BATTERY_WIRTE_LOG     
    Battery_CreatLog();
#endif    
    
    //for (i=0; i<10; i++)
    {
    	hy_adc_read(ADCCHANNEL_BATTERY);
    }
    
    //ÿ�ζ����е�ѹ�ɼ�
    gu32Battery_DetectCnt = 0xFFFFFFFF;
    gu32BatteryLev = POWER_LEVER_3V3;
    gu8ReadAdcCnt = 0;
    
    memset(gu16ReadAdc,0,sizeof(gu16ReadAdc)); 
    
}

void Battery_NormalDetect(void)
{
	//BATTERY_DETECT_TIMER_INTR�ν���һ�ε�ѹ�ɼ�
	gu32Battery_DetectCnt = 0;
}


/*-----------------------------------------------------------------------------
* ����:	Battery_DetectIntial()
* ����:	��ʼ������ѹ��������ʱ��ǿ�Ƽ��3�α�֤�ܹ��ѵ���������
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void Battery_DetectIntial(void)
{
	int tempADCLev=0;
#if 0
    
    hyhwAdc_start();
    tempADCLev = hy_adc_read(ADCCHANNEL_BATTERY);
    
    if (tempADCLev == -1) return ;
    
    gu16ReadAdc[gu8ReadAdcCnt++] = tempADCLev; 
    
//	hyhwAdc_Init();
//	hyhwAdc_start(ADCCHANNEL_BATTERY);
//	gu16ReadAdc[gu8ReadAdcCnt++] = hyhwAdc_Read(ADCCHANNEL_BATTERY);

	
	if (gu8ReadAdcCnt >= BATTERY_DETECT_CNT /*|| 0xFFFFFFFF != gu32BatteryLev*/)
	{
		int i,j;
		U32 ktempAdcValue;U16 tempAdc;
#ifdef BATTERY_WIRTE_LOG		
		Battery_WriteLog();
#endif		
		
		//����
		for (i=0; i<(BATTERY_DETECT_CNT-1); i++)
		{
			for (j=i+1; j<BATTERY_DETECT_CNT; j++)
			{
				if (gu16ReadAdc[i] > gu16ReadAdc[j])
				{
					ktempAdcValue  = gu16ReadAdc[i];
					gu16ReadAdc[i] = gu16ReadAdc[j];
					gu16ReadAdc[j] = ktempAdcValue;
				}
			}
		}
		//ȥ�������С��ȡƽ��
		ktempAdcValue = 0;
		for (i=1; i<(BATTERY_DETECT_CNT-1); i++)
		{
			ktempAdcValue += gu16ReadAdc[i];
		}
		ktempAdcValue = ktempAdcValue/(BATTERY_DETECT_CNT-2);
		
		if (ABS(gu32BatteryLev - ktempAdcValue) > BATTERY_LEV_DIFFERENCE)
		{//����ADCֵ
			gu32BatteryLev = ktempAdcValue;
		}
		gu8ReadAdcCnt = 0;
       	    
	}
#else
    gu32BatteryLev = POWER_LEVER_3V3; 
#endif	
}

/*-----------------------------------------------------------------------------
* ����:	Battery_Detect()
* ����:	����ص�ѹ
* ����:	none
* ����:	������ʵ�ʵ�ѹֵ
*----------------------------------------------------------------------------*/
U32  Battery_Detect(void)
{
    if (gu32Battery_DetectCnt == 0xFFFFFFFF)
    {
        Battery_DetectIntial();
        return gu32BatteryLev;
	}
    
    if (gu32Battery_DetectCnt == 0)
    {
        Battery_DetectIntial();
	}
	
	gu32Battery_DetectCnt++;
	if (gu32Battery_DetectCnt >= BATTERY_DETECT_TIMER_INTR)  gu32Battery_DetectCnt = 0;
		
	return gu32BatteryLev;
}


/*-----------------------------------------------------------------------------
* ����:	Battery_Level()
* ����:	����صȼ�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
U32 Battery_Level(void)
{
	U16 battery;
	U32 lev = 4;
	
	battery = gu32BatteryLev;
	
	if (battery >= POWER_LEVER_FULL) lev = 5;
	else if (battery >= POWER_LEVER_3V3) lev = 4;
	else if (battery >= POWER_LEVER_2V3) lev = 3;
	else if (battery >= POWER_LEVER_1V3) lev = 2;
	else if (battery >= POWER_LEVEL_0V3) lev = 1; 
	else  lev = 0; 	
	
	return lev;
}

/*-----------------------------------------------------------------------------
* ����:	Battery_If_StartPowerOff()
* ����:	������ʱ�����Ƿ�С��3.1V���С�ڸ�ֵ����ֱ�ӹص��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void Battery_If_StartPowerOff(void)
{
}

U32 Battery_CurrentLevel(void)
{
    return 5;
}

U32 Battery_UpdateLevel(void)
{

}

/*-----------------------------------------------------------------------------
* ����:	Battery_PermitRun()
* ����:	�����жϵ�ǰ����Ƿ�ϵͲ�����󹦺��豸����
* ����:	none
* ����:	0��������  1������
*----------------------------------------------------------------------------*/
U32 Battery_PermitRun(void)
{
   	 
   	 return 0;
}

/*-----------------------------------------------------------------------------
* ����:	Battery_EnforcePowerDown()
* ����:	ֻ���ڵ͵�����ʱ�������ʹ�øú�������ǿ�ƹػ�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void Battery_EnforcePowerDown(void)
{

}

/*-----------------------------------------------------------------------------
* ����:	Battery_DetectEnforcePowerDown()
* ����:	�������Ҫǿ�ƹػ�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
int Battery_DetectEnforcePowerDown(void)
{
    int rv = 0;
    
   
    return rv;
}
void bat_show_initial(void)
{
}
void  bat_show_low_battery_deskbox(void	*pFatherDesk)
{
}
/*-------------------------�޸ļ�¼--------------------------------
*
*-----------------------------------------------------------------*/