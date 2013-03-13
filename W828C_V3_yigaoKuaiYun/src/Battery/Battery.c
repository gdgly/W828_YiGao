/*----------------------�ļ�����--------------------------------
*��������: 08/03/14
*����    : ��ص���
*--------------------------------------------------------------*/
#include "hyTypes.h"
#include "Battery.h"
#include "hyhwGpio.h"
#include "hardWare_Resource.h"

/* �����˿� ��4��״̬:  ���ʹ��,�޵��       �ÿ�Ϊ��
							��粻ʹ��,           �ÿ�Ϊ��
							���ʹ��,�е���ҳ��� �ÿ�Ϊ��
							���ʹ��,�е��û���� �ÿ�Ϊ��
*/


#define BATTERY_DETECT_TIMER_INTR   50//10//200 //5s ����һ��
#define BATTERY_LEV_DIFFERENCE 	   8

/*
*DEFINE variable
*/
#define BATTERY_ENFORCE_VALUE   (0x12)
static U8 gu8EnforcePDn = 0;
U32 gu32Battery_DetectCnt = 0;
volatile U32 gu32BatteryLev = 0xFFFFFFFF;//����(������ʾ)
volatile U32 gu32BatteryCurLev = 0xFFFFFFFF;//��ǰ����

static U8 gu8ReadAdcCnt = 0; //adc ��ȡ�Ĵ���
//static U16 gu16ReadAdc[3] = {POWER_LEVER_3V3};
//static 
volatile U16 gu16ReadAdc[BATTERY_DETECT_CNT] = {POWER_LEVER_3V3};



//���Դ򿪴˶���
//#define BATTERY_WIRTE_LOG
//#define BATTERY_WIRTE_LOG_FILE

//#define BATTERY_WIRTE_LOG_UART

#ifdef BATTERY_WIRTE_LOG
#include "appControlProcess.h"
#include "AppCtrlConstData.h"

#ifdef BATTERY_WIRTE_LOG_UART
#include "hyhwCkc.h"
#include "hyhwGpio.h"
#include "hyhwUart.h"
#endif

#define BATTERY_FILE_NAME		"C:/battery.txt"
#define BATTERY_LOG_FLAG		"\r\n ----end------- \r\n"

int fd_battery = -1;
char *pBatterybuf = NULL;
U16 gu16ReadAdcTest0[BATTERY_DETECT_CNT];
U16 gu16ReadAdcTest[BATTERY_DETECT_CNT];
volatile U8  gu16WriteFull = 0;
#define TEMP_BUF_SIZE (300)	 //512

void Battery_UartSend(char *data, int len)
{
#ifdef BATTERY_WIRTE_LOG_UART
	int i;
	
	for (i=0; i<len; i++)
	{
		hyhwUart_WriteFifo(*data++, UART3);
	}
#endif
}

void Battery_CreatLog(void)
{
#ifdef BATTERY_WIRTE_LOG
#ifdef BATTERY_WIRTE_LOG_FILE
	int fd;
	fd = AbstractFileSystem_Open(BATTERY_FILE_NAME, 2);//AFS_RDWR
	if (fd >= 0)
	{
		AbstractFileSystem_Seek(fd, 0, 2);
	}
	else
	{
		fd = AbstractFileSystem_Create(BATTERY_FILE_NAME);
	}
	if (fd >= 0)
	{
		AbstractFileSystem_Write(fd, "\r\n---bat test---\r\n", strlen("\r\n---bat test---\r\n"));
		fd_battery = fd;
	}
#endif
#ifdef BATTERY_WIRTE_LOG_UART
	Battery_UartSend(infoStr, strlen(infoStr));
#endif
#endif
}

void Battery_WriteBuf(U16 adcValue)
{}

void Battery_WriteLog(U16 adcValue)
{
#ifdef BATTERY_WIRTE_LOG
	float v;char tempStr[50];
	int i, len;
	
	v = (float)(((float)adcValue*3*2)/1024);
	sprintf(tempStr, "ADC=%04X, V=%1.4f, T=%08X\r\n", adcValue, v, rawtime(NULL));
	len = strlen(tempStr);
	for (i=12; i<18; i++)
	{
		if (tempStr[i]<'0' || tempStr[i]>'9')
		{
			tempStr[i] = '.';
			break;
		}
	}
	#ifdef BATTERY_WIRTE_LOG_FILE
	if (fd_battery >= 0)
	{
		AbstractFileSystem_Write(fd_battery, tempStr, len);
	}
	#endif
	#ifdef BATTERY_WIRTE_LOG_UART
	#endif
#endif
}

#endif

void Battery_WriteInfo(char *infoStr)
{
#ifdef BATTERY_WIRTE_LOG
	if (fd_battery >= 0)
	{
	#ifdef BATTERY_WIRTE_LOG_FILE
		AbstractFileSystem_Write(fd_battery, infoStr, strlen(infoStr));
	#endif
	#ifdef BATTERY_WIRTE_LOG_UART
		Battery_UartSend(infoStr, strlen(infoStr));
	#endif
	}
#endif
}

void Battery_CloseLog(void)
{
#ifdef BATTERY_WIRTE_LOG
	char *tempBuf = (char *)BATTERY_LOG_FLAG;
	int len = strlen(BATTERY_LOG_FLAG);
	if (fd_battery >= 0)
	{
#ifdef BATTERY_WIRTE_LOG_FILE
		AbstractFileSystem_Write(fd_battery, tempBuf, len);
		AbstractFileSystem_Close(fd_battery);
#endif
#ifdef BATTERY_WIRTE_LOG_UART
		Battery_UartSend(tempBuf, len);
#endif
		if (pBatterybuf != NULL)
		{
			free(pBatterybuf);
		}
		fd_battery = -1;
		pBatterybuf = NULL;
	}
#endif
}

/*------------------------------------------------------------
* ����:	Battery_Init
* ����:	��Դ�л����ֵĳ�ʼ�� ����gpio�ڲ��ɱ�����
* ����:	��
* ����:	��
--------------------------------------------------------------*/
void Battery_Init(void)
{
	hyhwGpio_setAsGpio(DCIN_DET_PORT, DCIN_DET_GPIO);		//DC������
	hyhwGpio_setIn(DCIN_DET_PORT, DCIN_DET_GPIO);
	
	//hyhwGpio_setAsGpio(CHARGE_STATE, CHARGE_STATE_BIT);		//�����
	//hyhwGpio_setIn(CHARGE_STATE, CHARGE_STATE_BIT);
	
	//hyhwGpio_setAsGpio(ENCHG_BAT, ENCHG_BAT_BIT);		//ʹ�ܳ��
	//hyhwGpio_setOut(ENCHG_BAT, ENCHG_BAT_BIT);
	//hyhwGpio_setLow(ENCHG_BAT, ENCHG_BAT_BIT);
}

/*-----------------------------------------------------------------------------
* ����:	Battery_DcInCheck
* ����:	�Ƿ��г��������
* ����:	none
* ����:	0 :no 1��yes
*----------------------------------------------------------------------------*/
U32 Battery_DcInCheck(void)
{
	return (hyhwGpio_Read(DCIN_DET_PORT, DCIN_DET_GPIO));
}

int hy_adc_read(int chl)
{
    int adc;
    
    adc = hyhwAdc_Read(chl);
    
    //PhoneTrace4(0,"%x",adc);
    
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
    
    hyhwGpio_setAdcFunction();
    //for (i=0; i<10; i++)
    {
    	hy_adc_read(ADCCHANNEL_BATTERY);
    }
    
    //ÿ�ζ����е�ѹ�ɼ�
    gu32Battery_DetectCnt = 0xFFFFFFFF;
    gu32BatteryLev = POWER_LEVER_3V3;
    gu8ReadAdcCnt = 0;
    
    memset(gu16ReadAdc,0,sizeof(gu16ReadAdc));
    
    hyhwInt_ConfigADC(hyhwAdc_start); 
    
    gu8EnforcePDn = 0;
    
}

void Battery_NormalDetect(void)
{
	//BATTERY_DETECT_TIMER_INTR�ν���һ�ε�ѹ�ɼ�
	gu32Battery_DetectCnt = 0;
	//gu32Battery_DetectCnt = 0xFFFFFFFF;
    gu32BatteryLev = POWER_LEVER_3V3;
}

/*-----------------------------------------------------------------------------
* ����:	Battery_AmendValue
* ����:	����ϵͳ����״̬������ʹ��������������ADC
* ����:	adcLev----ʵ�ʲɼ�����ADC
* ����:	�������ADC
*----------------------------------------------------------------------------*/
int Battery_AmendValue(int adcLev)
{
#if 1
	int status;
	int coeff, offsetAdcLev;
	
	offsetAdcLev = 0;
	if (Battery_DcInCheck() == 1)//ԭ���������USB���
	{
		offsetAdcLev -= 16;
	}
	/*if(YD_getPrintEnable() == 1)
	{
		offsetAdcLev += 45;
	}*/
	/*
	if (GetUSBStatus() == 1)
	{//USB host
		offsetAdcLev += 20;
	}*/  	
	adcLev += offsetAdcLev;
#endif
	return adcLev;
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
#if 1

    tempADCLev = hy_adc_read(ADCCHANNEL_BATTERY);
    
	#ifdef BATTERY_WIRTE_LOG
    	gu16ReadAdcTest0[gu8ReadAdcCnt] = tempADCLev;
	#endif
    
    tempADCLev = Battery_AmendValue(tempADCLev);
    
    if (tempADCLev == -1) return ;
    
    gu16ReadAdc[gu8ReadAdcCnt++] = tempADCLev; 
    
	if (gu8ReadAdcCnt >= BATTERY_DETECT_CNT /*|| 0xFFFFFFFF != gu32BatteryLev*/)
	{
		int i,j;
		U32 ktempAdcValue;U16 tempAdc;
		
	#ifdef BATTERY_WIRTE_LOG
		memcpy(gu16ReadAdcTest, gu16ReadAdc, BATTERY_DETECT_CNT*2);
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
		
		if (ABS((int)gu32BatteryLev - (int)ktempAdcValue) > BATTERY_LEV_DIFFERENCE)
		{//����ADCֵ
			gu32BatteryLev = ktempAdcValue;
		}
		gu32BatteryCurLev = ktempAdcValue;
		#if 0
		{
			float v;char tempStr[20];
			v = (float)(((float)ktempAdcValue*3.3*2)/1024);
			sprintf(tempStr, "%1.4f", v);
			for (i=0; i<strlen(tempStr); i++)
			{
				if (tempStr[i]<'0' || tempStr[i]>'9')
				{
					tempStr[i] = '.';
					break;
				}
			}
			//PhoneTrace(1,"bat ADC = %04X, V=%s", gu32BatteryCurLev, tempStr);
			hyUsbPrintf("bat ADC = %04X, V=%s\r\n", gu32BatteryCurLev, tempStr);
		}
		#endif
		gu8ReadAdcCnt = 0;
       	
		#ifdef BATTERY_WIRTE_LOG
			Battery_WriteLog(ktempAdcValue);
			if (gu32BatteryCurLev >= POWER_LEVER_FULL)
			{
				Battery_WriteInfo("***********bat full!!!**************\r\n");
			}
		#endif		
	}
#else
    gu32BatteryLev = POWER_LEVER_3V3; 
	//if(PhoneTrace_base)
	//	PhoneTrace1(1,"SdramSize= %d M",hyhwSdramGetCapability());
#endif

	//PhoneTrace1(1,"bat LevADC = %04X",gu32BatteryLev);
    //gu32BatteryLev = POWER_LEVEL_OFFV3+2;
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
    
    //if (ABS(rawtime(NULL) - gu32Battery_DetectCnt) >= 100)
    if (ABS(rawtime(NULL) - gu32Battery_DetectCnt) >= 20)
    {
		gu32Battery_DetectCnt = rawtime(NULL);
        Battery_DetectIntial();
	}
	
	
#ifdef BATTERY_WIRTE_LOG    
    //Battery_WriteLog(hyhwAdc_Read(ADC_CH2));
#endif
	
	//gu32Battery_DetectCnt++;
	//if (gu32Battery_DetectCnt >= BATTERY_DETECT_TIMER_INTR)  gu32Battery_DetectCnt = 0;
	
	/*if(ABS(rawtime(NULL)-tt) > 3000)//30���ӡһ�δ�ѹֵ
	{
		bat = gu32BatteryLev*66;
		bat = bat/10240;
		sprintf(buf,"bat=0x%x__%f",gu32BatteryLev,bat);
		if(PhoneTrace_base)
			PhoneTrace_base(0,"%s",buf);
		tt = rawtime(NULL);
	}*/

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
	else if (battery >= POWER_LEVER_LOW) lev = 1;  //POWER_LEVEL_0V3
	else  lev = 0; 	
	
	return lev;
}
/*-----------------------------------------------------------------------------
* ����:	Battery_CurrentLevel()
* ����:	����ص�ǰ�ȼ�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
U32 Battery_CurrentLevel(void)
{
	U16 battery;
	U32 lev;
	
	battery = (U16)gu32BatteryCurLev;
	
	if (battery >= POWER_LEVER_FULL) lev = 5;
	else if (battery >= POWER_LEVER_3V3) lev = 4;
	else if (battery >= POWER_LEVER_2V3) lev = 3;
	else if (battery >= POWER_LEVER_1V3) lev = 2;
	else if (battery >= POWER_LEVER_LOW) lev = 1;  //POWER_LEVEL_0V3
	else  lev = 0; 	
	
	return lev;
}

/*-----------------------------------------------------------------------------
* ����:	Battery_UpdateLevel()
* ����:	������ʾ����Ϊ��ǰ����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
U32 Battery_UpdateLevel(void)
{
	gu32BatteryLev = gu32BatteryCurLev;
}

/*-----------------------------------------------------------------------------
* ����:	Battery_PermitRun()
* ����:	�����жϵ�ǰ����Ƿ�ϵͲ�����󹦺��豸����
* ����:	none
* ����:	0��������  1������
*----------------------------------------------------------------------------*/
U32 Battery_PermitRun(void)
{
   	 if (0 == Battery_DcInCheck() && gu32BatteryLev < POWER_PERMIT_RUN_LEV)
   	 {
   	     return 0;
   	 }
   	 
   	 return 1;
}

/*-----------------------------------------------------------------------------
* ����:	Battery_EnforcePowerDown()
* ����:	ֻ���ڵ͵�����ʱ�������ʹ�øú�������ǿ�ƹػ�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void Battery_EnforcePowerDown(void)
{
     if (0 != Battery_PermitRun()) 
     {
         return;
     }
     
     gu8EnforcePDn = BATTERY_ENFORCE_VALUE;
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
    
    if (0 == Battery_PermitRun() && gu8EnforcePDn == BATTERY_ENFORCE_VALUE)
    {
        gu8EnforcePDn = 0;
        rv = 1;
    }
    
    return rv;
}

/*-----------------------------------------------------------------------------
* ����:	Battery_If_StartPowerOff()
* ����:	������ʱ�����Ƿ�С��3.55V���С�ڸ�ֵ����ֱ�ӹص��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void Battery_If_StartPowerOff(void)
{
    int adc;
    int count;
    
    //syssleep(15); /* ������ʱ�� ADCһ���� �������ֵ�ǲ���ȷ��,�����ӳ����ж�ȥ���� */
    for(count = 0; count <= BATTERY_DETECT_CNT; count++)
    {
    	syssleep(2);
    	Battery_DetectIntial();
    }
    
    adc = Battery_Detect();
    
    if (0 != Battery_DcInCheck()) return;
    
    if (adc < POWER_LEVEL_FORBID)
    {                
        for(count = 0; count <= BATTERY_DETECT_CNT; count++) //��һ��ȷ��
	    {
	    	syssleep(2);
	    	Battery_DetectIntial();
	    }
	    adc = Battery_Detect();
	    if (0 != Battery_DcInCheck()) return;
	    if (adc > POWER_LEVEL_FORBID) return;
	    
        InterruptDisable();  
					
		while(1)
		{
		    hyhwBatteryPowerOnOff(0);
		    for (count=0; count < 5000; count++);
		}
    }
}

/*-----------------------------------------------------------------------------
* ����:	Battery_print
* ����:	�л�����ӡ״̬�����˳���ӡ״̬
* ����:	printFlag
*			0-----�����ӡ״̬
*			1-----�˳���ӡ״̬
* ����:	0----״̬�л��ɹ�
		-1---״̬�л�ʧ��(����������ӡ״̬)
*----------------------------------------------------------------------------*/
int Battery_print(int printFlag)
{
	int state;
		
	//�ڳ��
	//if (1 == Battery_DcInCheck()) return 0;
	
	//������ʱ���������ӡ
	if(gu32BatteryLev < POWER_PERMIT_RUN_LEV)
	{
		return -1;
	}
	
	return 0;
}

U32 Battery_getAdc(void)
{
	return gu32BatteryLev ;
}

/*-------------------------�޸ļ�¼--------------------------------
*
*-----------------------------------------------------------------*/