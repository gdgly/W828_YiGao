#include "gps.h"
#include "string.h"

#include "hyhwUart.h"
#include "hyhwCkc.h"
#include "hyhwGpio.h"
#include "hyhwIntCtrl.h"
#include "hyhwRtc.h"
#include "hardWare_Resource.h"

#include "std.h"
#include "sys.h"
#include "ospriority.h"
#include "semaphore.h"
#include "GN_GPS_api.h"
#include "glbVariable.h"
#include "appControlProcess.h"

////////////////////////////////////////////////////////

SEMAPHORE	*Gps_p_Exit_sem = NULL;
int			*GpsWait = NULL;

U32 testGpsRecvFlag = 0;


extern U32 gu32GpsRunTime;

int Gps_ProcStart(void);
void Gps_p_Exit(void);
///////////////////////////////////////////////////



#define GPS_DATA_BUF_SIZE		(2048)//(1024)//
typedef struct gsp_data_buf
{
	int readPos;
	int writePos;
	int validNum;
	char buffer[GPS_DATA_BUF_SIZE];
} tGPS_DATA_BUF;
tGPS_DATA_BUF tGps_recvData;

void Gps_recvBufferInit(void)
{
	memset(&tGps_recvData, 0x00, sizeof(tGPS_DATA_BUF));
}


/*
�Ӵ��ڶ�ȡ����


*/
int Gps_ReadData(char *pBuf)
{
	int readLen = 0;
	int rp, wp, validNum, tempLen;
	char *pRecvBuf;
	U8 IntValue;
	
	IntValue = InterruptDisable();	//��ֹ�ж�
	rp = tGps_recvData.readPos;
	wp = tGps_recvData.writePos;
	validNum = tGps_recvData.validNum;
	pRecvBuf = &tGps_recvData.buffer[rp];
	InterruptRestore(IntValue);
	
	if(validNum == 0)
	{
		return 0;
	}
	
	if(wp > rp)
	{
		readLen = wp - rp;
		memcpy(pBuf,pRecvBuf,readLen);
		rp = wp;
	}
	else
	{
		tempLen = GPS_DATA_BUF_SIZE-rp;
		memcpy(pBuf,pRecvBuf,tempLen);
		
		pRecvBuf = &tGps_recvData.buffer[0];
		memcpy((char *)&pBuf[tempLen],pRecvBuf,wp);
		
		readLen = tempLen+wp;
		rp = wp;
	}
	
	IntValue = InterruptDisable();
	tGps_recvData.readPos = rp;
	tGps_recvData.validNum -= readLen;
	InterruptRestore(IntValue);
	
	return readLen;
}

//�ú���ֻ����ѭ��д�룬����ʣ��ռ��Ƿ��㹻
//����д��buf���ֽ���
int Gps_recvData(char *pBuf, int recvLen)
{
	U8 IntValue;
	int rp, wp, validNum, tempLen;
	char *pRecvBuf;
	
	IntValue = InterruptDisable();
	rp = tGps_recvData.readPos;
	wp = tGps_recvData.writePos;
	validNum = tGps_recvData.validNum;
	pRecvBuf = &tGps_recvData.buffer[wp];
	InterruptRestore(IntValue);
	
	if (recvLen <= (GPS_DATA_BUF_SIZE-validNum))
	{
		//ʣ��buff������
		tempLen = GPS_DATA_BUF_SIZE-wp;
		if (tempLen >= recvLen)
		{
			memcpy(pRecvBuf, pBuf, recvLen);
		}
		else
		{
			memcpy(pRecvBuf, pBuf, tempLen);
			pRecvBuf = &tGps_recvData.buffer[0];
			pBuf += tempLen;
			memcpy(pRecvBuf, pBuf, recvLen-tempLen);
		}
		wp += recvLen;
		if (wp >= GPS_DATA_BUF_SIZE)
		{
			wp = wp-GPS_DATA_BUF_SIZE;
		}
		IntValue = InterruptDisable();
		tGps_recvData.writePos = wp;
		tGps_recvData.validNum += recvLen;
		InterruptRestore(IntValue);
		//if(recvLen > 20)
		//	PhoneTrace2(0,"uart= %d",recvLen);
		//else
		//	PhoneTrace2(1,"uart= %d",recvLen);
		return recvLen;
	}
	else
	{
		//PhoneTrace2(0,"uart lost = %d",recvLen);
		testGpsRecvFlag++;
	}
	return 0;
}
U32 uartDataLen = 0;

U32 GpsUart_DataLen()
{
	return uartDataLen;
}

//////////////////////////////////////////////////////
void GpsUart_IntIsr(void)
{
	U8		i;
	U8		data[20];
	U8		recvLen;
	
	//��������
	memset(data,0,20);
	recvLen = 0;
	for (i=0; i<16; i++)
	{
		if (hyhwUart_CheckRecDataReady(GPS_UART)==HY_OK)
		{
			hyhwUart_ReadFifo((U8 *)&data[recvLen++], GPS_UART);
		}
    	else
    	{
    		break;
    	}
	}
	if (recvLen > 0)
	{
		uartDataLen = recvLen;
		//PhoneTrace2(0,"uart data = %d",recvLen);
		Gps_recvData(&data[0], recvLen);
		ewakeup(&GpsWait);
	}
	else
	{
		//PhoneTrace2(0,"uart no data");
	}
}


void hyhwGps_resetOn(void)
{
	hyhwGpio_setAsGpio(GPS_RESET_PORT, GPS_RESET_GPIO);
	hyhwGpio_setOut(GPS_RESET_PORT, GPS_RESET_GPIO);
	hyhwGpio_setLow(GPS_RESET_PORT, GPS_RESET_GPIO);
}
void hyhwGps_resetOff(void)
{
	hyhwGpio_setAsGpio(GPS_RESET_PORT, GPS_RESET_GPIO);
	hyhwGpio_setOut(GPS_RESET_PORT, GPS_RESET_GPIO);
	hyhwGpio_setHigh(GPS_RESET_PORT, GPS_RESET_GPIO);
}
void hyhwGps_resetDisable(void)
{
	hyhwGpio_setAsGpio(GPS_RESET_PORT, GPS_RESET_GPIO);
	hyhwGpio_setIn(GPS_RESET_PORT, GPS_RESET_GPIO);
	hyhwGpio_pullDisable(GPS_RESET_PORT, GPS_RESET_GPIO);
}

void Gps_Init()
{
	GpsLib_InitToSdram_buf();
	uartDataLen = 0;
	
	Gps_recvBufferInit();
	hyhwGps_resetOn();
	hyhwGps_powerCtrl(EnableOn);
	syssleep(1);
	hyhwGps_resetOff();

	hyhwUart_Init(Uart_Baudrate_115200, Uart_DataBits_8, Uart_StopBits_1, 
				  Uart_Parity_Disable, Uart_Fifo_Mode, GPS_UART,
				  GPS_UART_PORT, FLOW_UNSUPPORT, DMA_UNSUPPORT);
 	
 	hyhwUart_rxIntDisable(GPS_UART);
 	hyhwInt_ConfigUart2_ISR(GpsUart_IntIsr); // �����жϷ�����
	hyhwUart_rxIntEnable(GPS_UART); // ʹ�ܽ����ж�
    hyhwInt_setIRQ(INT_BIT_UART);
    hyhwInt_enable(INT_BIT_UART);

    Gps_ProcStart();
    gu32GpsRunTime = rawtime(NULL);

	return ;
}

void Gps_HwDeInit()
{
	hyhwUart_rxIntDisable(GPS_UART);
	hyhwCpm_busClkDisable(CKC_UART2);
 	hyhwInt_ConfigUart2_ISR(NULL);
	
	//GPS���IO����Ϊ����״̬
 	hyhwGpio_setAsGpio(GPS_UART2_TXD_PORT, GPS_UART2_TXD_GPIO);//UART2 TX
 	hyhwGpio_setIn(GPS_UART2_TXD_PORT, GPS_UART2_TXD_GPIO);
 	hyhwGpio_pullDisable(GPS_UART2_TXD_PORT, GPS_UART2_TXD_GPIO);
 	hyhwGpio_setAsGpio(GPS_UART2_RXD_PORT, GPS_UART2_RXD_GPIO);//UART2 RX
 	hyhwGpio_setIn(GPS_UART2_RXD_PORT, GPS_UART2_RXD_GPIO);
 	hyhwGpio_pullDisable(GPS_UART2_RXD_PORT, GPS_UART2_RXD_GPIO);
 	
	hyhwGps_resetDisable();
}

void Gps_DeInit()
{
	Gps_p_Exit();
	
	Gps_HwDeInit();
	hyhwGps_powerCtrl(EnableOff);
	
	return ;
}


extern RtcTime_st gtRtc_solar;
/*-------------------------------
* ����:Gps_Process
* ����:
* ����:none
* ����:1
*-------------------------------*/
int Gps_Process(void)
{
	unsigned ilev;
	int ret;
	U32 ticks;
	
	//PhoneTrace2(0,"%04d-%02d-%02d %02d:%02d:%02d",gtRtc_solar.year,gtRtc_solar.month,gtRtc_solar.day,
				//gtRtc_solar.hour,gtRtc_solar.minute,gtRtc_solar.second);
	
	
	//GpsLib_InitToSdram_buf();
	
	ConSume_UpdataAppCoef(GPS_DRIVER_AN, 96);
	GN_GPS_Init();
	//PhoneTrace2(0,"gps int ok");
	ticks = rawtime(NULL);
	while(1)
	{
		
		ret = semaphore_trywait(Gps_p_Exit_sem);
		if(ret == 0)
		{
			break;
		}
		
		ilev = setil(_ioff);
		ewaittm(&GpsWait, 1);
		(void) setil(ilev);
		//syssleep(1);
		
		//ret = semaphore_trywait(Gps_p_Exit_sem);
		//if(ret == 0)
		//{
		//	break;
		//}
		
		//TO DO ....
		//PhoneTrace2(0,"gps run");
		GN_GPS_Thread();
		if(ABS(rawtime(NULL) - ticks) >= 500)
		{
			//PhoneTrace2(0,"gps run %d",uartDataLen);
			hyUsbPrintf(0,"gps running (recv %d)\r\n",uartDataLen);
			ticks = rawtime(NULL);
		}
	}
	//PhoneTrace2(0,"gps exit ok");
	GN_GPS_Exit();
	
	semaphore_destroy(Gps_p_Exit_sem, OS_DEL_NO_PEND);
	Gps_p_Exit_sem = NULL;
	GpsWait = NULL;
	
	ConSume_UpdataAppCoef(GPS_DRIVER_AN, 0);
	return 1;
}


/*-------------------------------
* ����:Gps_ProcStart
* ����:
* ����:none
* ����:
*-------------------------------*/
int Gps_ProcStart(void)
{
	int pid, startValue, pidStatus = HY_ERROR;
	
	pid = ntopid(GPS_P_NAME);
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
			return HY_ERROR;//���̻������У�ֱ���˳�����ǰ������Ч
		}
	}
	Gps_p_Exit_sem = semaphore_init(0);
	
	if (pidStatus != HY_OK)
	{
		//���̲����ڣ���Ҫ����
		pid = createp((int (*)())Gps_Process,
								GPS_P_NAME,
								GPS_P_SIZE,
								GPS_P_PRIOPITY,
								NULL,NULL,NULL);
	}
	startValue = pstart( pid, (void *)Gps_Process);
	AppCtrl_CreatPro_Virtual(GPS_DRIVER_AN, pid);
	
	return HY_OK;
}

/*-------------------------------
* ����:Gps_p_Exit
* ����:
* ����:none
* ����:
*-------------------------------*/
void Gps_p_Exit(void)
{
	int pid;
	
	pid = ntopid(GPS_P_NAME);
	if (pid<0 || pid>30) return;
	
	if (Gps_p_Exit_sem != NULL)
		semaphore_post(Gps_p_Exit_sem);
	
	ewakeup(&GpsWait);
	
	while(PSPROC(pid)->state != P_INACT)
	{
	    syssleep(1);
	}
	
	deletep(pid);
	AppCtrl_DeletePro(GPS_DRIVER_AN);
}
/*-------------------------------
* ����:Gps_isRunning
* ����:�ж�GPS�����Ƿ����ڽ���
* ����:none
* ����:
*-------------------------------*/
U8 Gps_isRunning(void)
{
	int pid;
	
	pid = ntopid(GPS_P_NAME);
	if (pid<0 || pid>30) return 1 ;
	else return 0 ;
}