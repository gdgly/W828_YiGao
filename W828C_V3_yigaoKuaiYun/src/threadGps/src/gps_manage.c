/*-----------------------------------------------------------
*
*gps������Ҫ�ǹ����������
*1.���������Զ���λ,��ϵ���͹���
*2.�ֶ���λ,���ǵ���1����ͻ
*3.�յ�ȥӲ�����Կ��Թ鲢Ϊ2ͳһΪ����������Ϣ
*
*------------------------------------------------------------*/


#include "glbVariable.h"
#include "ospriority.h"
#include "sys.h"
#include "Msgqueue.h"
#include "GPS_Parse.h"

#define GPS_VALID_MAXNUM		60
#define GPS_RUN_TIMES           4  /* gps�رյĵ���ʱ���� ��Ҫ��Ϊ�˵͹��Ķ���� */
#define GPS_CLOSE_TIMES         4  /* gps�رյĵ���ʱ���� ��Ҫ��Ϊ�˵͹��Ķ���� */
#define GPS_LAT_CMP            (3) /* γ�ȱȽϵ�λ�� */
#define GPS_LONG_CMP           (4) /* ���ȱȽϵ�λ�� */
static tMSG_QUEUE *ptGpsMsg ;/* ��Ϣ���� */
static uDATA *ptGpsmSend ;  /* ��¼��ȡ��gps��Ϣ     */
static U8  su8GpsmGetState; /* gps�Ƿ�λ�ɹ��ı�־ 1 �ɹ� 0 δ��λ�� */
extern U8  su8GpsWorkMode;  /* gps����ģʽ: bit[0]=1 �Զ���λ�ϴ� bit[1]=1 �ֶ���λ  */
static int sGpsRunTick   ;  /* ��¼gps����ģʽ1���е�Tick�� */
static U32 su8GpsRunTimes[GPS_RUN_TIMES] = 
			{30000,18000,6000,3500};/* 5min,3min,1min,20s,10s */
static U32 su8GpsCloseTimes[GPS_CLOSE_TIMES] = 
			{30000,60000,90000,120000};/* 5min,10min,15min,20min */

static U8  su8RunIndex,su8CloseIndex,getGpsCnt;
static int saveGet,saveSend,gDelayTick;
static tGPGGA stGpgga ;
static U8 gu8Lat[8],gu8Long[8];

static U32 gu32GetGpsInfo = 0 ;

void gpsm_sendMsg(U8 onoff);
/*-------------------------------
* ����:gpsm_int()
* ����:
* ����:none
* ����:
*-------------------------------*/
void gpsm_int(void)
{
	memset(&stGpgga,0,sizeof(tGPGGA));
	ptGpsmSend = (uDATA *)hsaSdram_GetGpsBuf();
	memset((U8 *)ptGpsmSend,0,hsaSdram_GetGpsBufSize());
	saveGet  = rawtime(NULL);
	saveSend = rawtime(NULL);
	su8RunIndex = 0;
	su8CloseIndex = 0xFF ;
	getGpsCnt    = 0;
	su8GpsWorkMode=0;
	su8GpsmGetState=0;
	gDelayTick = 0 ;
	
	memset(gu8Lat ,0,8);
	memset(gu8Long,0,8);
}
/*-------------------------------
* ����:gpsm_setState()
* ����:
* ����:none
* ����:
*-------------------------------*/
void gpsm_setState(tGPGGA *pt)
{
	wlock((unsigned)&gu32GetGpsInfo);
	memcpy(&stGpgga,pt,sizeof(tGPGGA));
	YDhtest_gps(pt);
	su8GpsmGetState = 1 ;
	unlock((unsigned)&gu32GetGpsInfo);
}

/*-------------------------------
* ����:gpsm_CommEvent
* ����:
* ����:none
* ����:
*-------------------------------*/
void gpsm_CommEvent(void)
{
	int getTime,sendTime ;
	int curTime,i;
	
	if(gDelayTick > 0 && su8CloseIndex < GPS_CLOSE_TIMES)
	{	
		/* ��ʱ��λ��ʱ�������źŲ���һֱ��λ�������޷�����͹��� */
		if(ABS(rawtime(NULL)-gDelayTick) >= su8GpsCloseTimes[su8CloseIndex])
		{
			gDelayTick = 0 ;
		}
		
		return ;
	}
	
	
	if(gtHyc.GPSSwitch != 0) /* ����ʹ��gps ��ͳһ��gps�Ŀ������� */
	{
		if(1 == net_onlyRead_regStatus()) /* sim�����òſ����Զ���λ�ϴ� */
		{
			getTime  = atoi(gtHyc.GPSGetTime );
			sendTime = atoi(gtHyc.GPSSendTime);
			getTime = (getTime >=1 && getTime <= 60) ? getTime : 3 ; /* �жϻ�ȡʱ�����ȷ�� */
			sendTime= (sendTime>=3 && sendTime<= 60) ? sendTime: 3 ; /* �жϷ���ʱ�����ȷ�� */	
			
			if(ABS(rawtime(NULL) - saveGet) >= getTime*6000)	/* ��ȡgps��Ϣ */
			{
				saveGet = rawtime(NULL) ; /* ��ʱ���� */
				su8GpsWorkMode |= 0x01 ;
				gpsm_sendMsg(1);          /* ����gps */
			}
			
			if(ABS(rawtime(NULL) - saveSend) >= sendTime*6000) /* ��gps��Ϣд�뷢�Ͷ��� */
			{
				saveSend = rawtime(NULL) ; /* ��ʱ���� */
				if(getGpsCnt > 0)
				{
					for(i = 0 ; i < getGpsCnt ; i++)
					{
						YDnet_add2Que(0,0,POSTGPS,&ptGpsmSend[i]);
					}
					memset((U8 *)ptGpsmSend,0,hsaSdram_GetGpsBufSize());
					getGpsCnt = 0 ; /* ��� */
				}
			}
		}
	}
	
	/* �Զ��ϴ�gpsģʽ�Ĺرղ��� */
	if(su8GpsWorkMode & 0x01)
	{		
		curTime = ABS(rawtime(NULL)-sGpsRunTick) ;
		if(curTime < su8GpsRunTimes[su8RunIndex]) /* ��ָ����ʱ���ڶ�λ�� */
		{
			if(1 == su8GpsmGetState)
			{
				if((su8GpsWorkMode & 0x02) == 0) /* �޹���ģʽ2�͹ر� */
				{
					gpsm_sendMsg(0);
				}
				if(getGpsCnt < (hsaSdram_GetGpsBufSize()/sizeof(uDATA))) /* gps��λ�ĸ�����������,�Է���� */
				{
					wlock((unsigned)&gu32GetGpsInfo);
					
					if(0 != memcmp(gu8Lat,stGpgga.lat,GPS_LAT_CMP) || 0 != memcmp(gu8Long,stGpgga.Long,GPS_LONG_CMP)) /* 100m�ڲ���¼���� */
					{
						memcpy(gu8Lat,stGpgga.lat,GPS_LAT_CMP);
						memcpy(gu8Long,stGpgga.Long,GPS_LONG_CMP);
						gu8Lat[GPS_LAT_CMP]   = 0 ;
						gu8Long[GPS_LONG_CMP] = 0 ;
						
						strcpy(ptGpsmSend[getGpsCnt].gps.Long,stGpgga.Long);
						strcpy(ptGpsmSend[getGpsCnt].gps.Lat ,stGpgga.lat);
						strcpy(ptGpsmSend[getGpsCnt].gps.high,stGpgga.high);
						strcpy(ptGpsmSend[getGpsCnt].gps.groupId,gtLogoInfo.pGroup[gtLogoInfo.groupSelect].groupId);
						strcpy(ptGpsmSend[getGpsCnt].gps.date,YD_GetTimeStr());
						getGpsCnt++;
					}
					
					unlock((unsigned)&gu32GetGpsInfo);
				}
				su8GpsmGetState = 0 ;
				su8GpsWorkMode &= 0xFE ;
				
				for(i = GPS_RUN_TIMES ; i > 0 ; i--)
				{
					if(curTime < su8GpsRunTimes[i])
					{
						su8RunIndex = i ;
						break;
					}
				}
				
				su8CloseIndex = 0xFF ;
			}
		}
		else /* ����ָ����ʱ��͹ر� */
		{
			if((su8GpsWorkMode & 0x02) == 0) /* �޹���ģʽ2�͹ر� */
			{
				gpsm_sendMsg(0);				
				su8GpsWorkMode &= 0xFE ;
				gDelayTick = rawtime(NULL) ;/* ��ʱ����ʼtick */				
				su8RunIndex = (0 >= su8RunIndex) ? 0 : (--su8RunIndex) ; /* ������λʱ�� */
				if(su8CloseIndex >= GPS_CLOSE_TIMES)su8CloseIndex = 0 ;
				else
				{
					su8CloseIndex ++ ;
				}
			}
			
			sGpsRunTick = rawtime(NULL);   /* ������ʼtick */
		}
	}	
}
/*-------------------------------
* ����:gpsm_process
* ����:
* ����:none
* ����:
*-------------------------------*/
void gpsm_process(void)
{
	int		 ret ;
	U8       msgbuf[40] ;
	
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgbuf;
	
	while(1)
	{
		ret = msg_queue_get(ptGpsMsg,(char*)(&ptPutMsg->body),ptPutMsg->length);
		if (ret < MSG_QUEUE_OK)continue;
		
		//PhoneTrace2(0,"get gps msg");
		
		if(GPS_OPEN == ptPutMsg->body.id)/* ����gps */
		{
			if(0 != Gps_isRunning())
			{
				sGpsRunTick = rawtime(NULL);   /* ������ʱ���¼��ʼtick */
				//PhoneTrace2(0,"get open gps msg");
				Gps_Init();
				//PhoneTrace2(0,"get open gps OK");
			}
		}
		else if(GPS_CLOSE == ptPutMsg->body.id)/* �ر�gps */
		{
			if(0 == Gps_isRunning())  
			{
				//PhoneTrace2(0,"get close gps msg");
				Gps_DeInit();
				//PhoneTrace2(0,"get close gps OK");
			}
		}
	}
}
/*-------------------------------------------------------------------------
* ����: gpsm_sendMsg
* ˵��: ������Ϣgps ���ؽ���
* ����: ���ر�ʶ 0 :close 1 :open
* ����: ��
* -------------------------------------------------------------------------*/
void gpsm_sendMsg(U8 onoff)
{
	U8       msgbuf[40] ;
	U32      msgId      ;
	tMSG_PUT_PARAM *ptPutMsg = (tMSG_PUT_PARAM *)msgbuf;
	
	msgId = (0 == onoff) ? GPS_CLOSE : GPS_OPEN;
	
	ptPutMsg->length   = sizeof(eMSG_ID);
	ptPutMsg->body.id  = msgId;
	ptPutMsg->priority = 10;
	//PhoneTrace2(0,"send %d gps",onoff);
	msg_queue_tryput(ptGpsMsg,(char*)(&ptPutMsg->body),ptPutMsg->length,ptPutMsg->priority);
}


/*-------------------------------
* ����:gpsm_thread
* ����:ֻ����gps�Ŀ��͹�
* ����:none
* ����:
*-------------------------------*/
int gpsm_thread(void)
{
	int pid, startValue;
	
	gpsm_int();
	Gps_DeInit();
	pid = createp((int (*)())gpsm_process,
							GPS_MANAGE_NAME,
							GPS_MANAGE_SIZE,
							GPS_MANAGE_PRIOPITY,
							NULL,NULL,NULL);
							
	ptGpsMsg = msg_queue_init(10,40);
	
	startValue = pstart( pid, (void *)gpsm_process);
	
	return HY_OK;
}


/*-------------------------------------------------------------------------
* ����: gpsm_mode2
* ˵��: �ֶ���λ�Ŀ���
* ����: ���ر�ʶ 0 :close 1 :open
* ����: -1 :gps Ϊ����
* -------------------------------------------------------------------------*/
int gpsm_mode2(U8 onoff)
{	
	if(gtHyc.GPSSwitch == 0)  return -1;
	if(0 == onoff)su8GpsWorkMode &= 0xFD;
	else  su8GpsWorkMode |= 0x02 ;
	
	//PhoneTrace2(0,"open gps");
	if(su8GpsWorkMode & 0x01) return 0 ; /* ������ģʽ1 ��gps ���� */
	
	gpsm_sendMsg(onoff);
}

/*-------------------------------------------------------------------------
* ����: gpsm_workState
* ˵��: ����gps�������
* ����: none
* ����: 0 :gps Ϊ�ر� 
* -------------------------------------------------------------------------*/
U8 gpsm_workState(void)
{
	return su8GpsWorkMode ;
}
