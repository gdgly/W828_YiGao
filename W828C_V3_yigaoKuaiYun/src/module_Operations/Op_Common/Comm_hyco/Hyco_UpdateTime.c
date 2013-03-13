#include "hyTypes.h"
#include "glbVariable.h"
#include "TcpIp_p.h"
#include "http.h"
#include "ppp_gprs.h"
#include "socket_api.h"
#include "time.h"
#include "semaphore.h"
#include "hyhwRtc.h"
#include "std.h"
#include "sys.h"
#include "ospriority.h"
#include "Op_Config.h"


#ifdef VER_CONFIG_HYCO


#define   NTP_PORT            123             //NTP Э��˿ں�
#define   NTP_PACKET_LEN      48              //NTP �������ݰ�Ϊ48�ֽ�
#define   NTP_SERVER_IP1      "210.72.145.44" // NTP �����������ַ ������ʱ����Ȩ���ģ�
#define   NTP_SERVER_IP2      "202.120.2.101" // NTP �����������ַ(�Ϻ���ͨ��ѧ��������NTP��������ַ)  


//�Ƿ��Ѿ�ͬ��ʱ�� 0x00:δͬ��    0x01:����ͬ��    0x02:�ɹ�      0x82:�ɹ�������ʾ    0x03:ʧ��     0x83:ʧ��������ʾ
U8 gu8UpDateTime=0;
char gas8FailReason[50]={0};	//ͬ��ʱ��ʧ��ԭ��

static U8 gServSelcFlag;//��������ַѡ���־ 

#define UPDATETIME_P_NAME		"updatetime_p"

SEMAPHORE *UpdateTime_p_Exit_sem = NULL;

typedef struct ntp { 
    char flag;                        
    char stratum; 
    char poll; 
    char precision; 
    char rootDelay[4]; 
    char rootDispersion[4]; 
    char refId[4]; 
    char refTime[8]; 
    char originateTime[8]; 
    char recvTime[8]; 
    char transmitTime[8]; 
    
}tNTP;


void SetUpDateState(U8 type)
{
	gu8UpDateTime = type;
}

U8 GetUpDateState()
{
	return gu8UpDateTime;
}

/*---------------------------------------------
* ����:UpDateTime_SetFailReason
* ����:����ͬ��ʱ��ʧ��ԭ��ʧ�ܵ�ԭ��
* ����:pReason:ԭ��
* ����:none
----------------------------------------------*/
void UpDateTime_SetFailReason(U8 *pReason)
{
	int len;
	
	memset(gas8FailReason, 0, sizeof(gas8FailReason));
	
	len = strlen(pReason);
	if(len >= 50)
	{
		len = 49;
	}
	memcpy(gas8FailReason, pReason, len);
}

/*---------------------------------------------
* ����:UpDateTime_GetFailReason
* ����:��ȡͬ��ʱ��ʧ�ܵ�ԭ��
* ����:nome
* ����:ԭ��
----------------------------------------------*/
U8	*UpDateTime_GetFailReason()
{
	return gas8FailReason;
}

/*-----------------------------------------
*����: Comm_TimeStr2Rtc
*����: ���ַ�����ʽʱ��ת���ɽṹ����ʽ 2010-01-11 15:22:12
*����: 
*����: 
*------------------------------------------*/
void Comm_TimeStr2Rtc(U8 *pTime, RtcTime_st *pRtc)
{
	U32 i, res, len, datalen;
	U8  *pStr;

	memset(pRtc,0,sizeof(RtcTime_st));
	
	datalen = strlen(pTime);

	//��
	pStr = strstr(pTime,"-");
	if(pStr == NULL)
	{
		len = strlen(pTime);
	}
	else
	{
		len  = pStr - pTime;
	}
	for (i = 0, res = 0; i < len; i++)
	{
		res = res * 10 + pTime[i] - '0';
	}
	pRtc->year = (U16)res;
	
	len += 1;
	pTime += len;
	if(len >= datalen)//������
	{
		return ;
	}
	
	//��
	pStr = strstr(pTime,"-");
	if(pStr == NULL)
	{
		len = strlen(pTime);
	}
	else
	{
		len  = pStr - pTime;
	}
	for (i = 0, res = 0; i < len; i++)
	{
		res = res * 10 + pTime[i] - '0';
	}
	pRtc->month = res;
	
	len += 1;
	pTime += len;
	if(len >= datalen)//������
	{
		return ;
	}
	
	//��
	pStr = strstr(pTime," ");
	if(pStr == NULL)
	{
		len = strlen(pTime);
	}
	else
	{
		len  = pStr - pTime;
	}
	for (i = 0, res = 0; i < len; i++)
	{
		res = res * 10 + pTime[i] - '0';
	}
	pRtc->day = res;
	
	len += 1;
	pTime += len;
	if(len >= datalen)//������
	{
		return ;
	}
	
	//ʱ
	pStr = strstr(pTime,":");
	if(pStr == NULL)
	{
		len = strlen(pTime);
	}
	else
	{
		len  = pStr - pTime;
	}
	for (i = 0, res = 0; i < len; i++)
	{
		res = res * 10 + pTime[i] - '0';
	}
	pRtc->hour = res;
	
	len += 1;
	pTime += len;
	if(len >= datalen)//������
	{
		return ;
	}
	
	//��
	pStr = strstr(pTime,":");
	if(pStr == NULL)
	{
		len = strlen(pTime);
	}
	else
	{
		len  = pStr - pTime;
	}
	for (i = 0, res = 0; i < len; i++)
	{
		res = res * 10 + pTime[i] - '0';
	}
	pRtc->minute = res;
	
	len += 1;
	pTime += len;
	if(len >= datalen)//������
	{
		return ;
	}
	
	//��
	pStr = strstr(pTime,":");
	if(pStr == NULL)
	{
		len = strlen(pTime);
	}
	else
	{
		len  = pStr - pTime;
	}
	for (i = 0, res = 0; i < len; i++)
	{
		res = res * 10 + pTime[i] - '0';
	}
	pRtc->second = res;

	return ;
}

/*-----------------------------------------------------------------------
* ����: SetSyn_GetNetworkTime
* ����NTPЭ��ͬ������ʱ��
* @param newTime OUT �����ȡ������ʱ��/
* @param otimeSec IN ��ʱʱ��,��otimeSec����,�������޻�Ӧ
* @param ser_selFlag ������ѡ���־ 0��NTP_SERVER_IP1  1��NTP_SERVER_IP2
* @ret SUCCEED 1 FAILED -1
 -----------------------------------------------------------------------*/
int SetSyn_GetNetworkTime(time_t *newTime, int otimeSec,U8 ser_selFlag)
{
    struct sockaddr_in recvAddr; 
    struct sockaddr_in sendAddr; 
    int sendAddrSize, sock;
    time_t recvTime; 
    struct timeval to;   
    fd_set fs;   
	tNTP pdu; 
	
	sendAddrSize = sizeof(sendAddr);
        
    memset(&pdu, '\0', sizeof(tNTP));
	pdu.flag = 0x1b;
	pdu.transmitTime[0] = 0xc7;
	pdu.transmitTime[1] = 0x50;
	pdu.transmitTime[2] = 0xe7;
	pdu.transmitTime[3] = 0xa0;

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
    if (sock < 0)
    { 
    	return -1;
    }

    recvAddr.sin_family = AF_INET; 
    recvAddr.sin_port = htons(NTP_PORT); 
    if(0 == ser_selFlag)
    {
    	 recvAddr.sin_addr.s_addr = inet_addr(NTP_SERVER_IP1); 
    }
    else if(1 == ser_selFlag)
    {
    	 recvAddr.sin_addr.s_addr = inet_addr(NTP_SERVER_IP2); 
    }
   

    sendto(sock, (char *)&pdu, NTP_PACKET_LEN, 0, (struct sockaddr *) &recvAddr, sizeof(recvAddr)); 

	to.tv_sec = otimeSec;   
    to.tv_usec = 0;   
        
    FD_ZERO(&fs);   
    FD_SET(sock, &fs);   

	if (select(sock, &fs, 0, 0, &to) > 0)
    {
    	recvfrom(sock, (char *)&pdu, NTP_PACKET_LEN, 0, (struct sockaddr *)&sendAddr, &sendAddrSize); 
   		closesocket(sock);
    }
    else 
    {
	    closesocket(sock);
		return -1;
    }
    
    memcpy(&recvTime, pdu.recvTime, 4); 
    recvTime = ntohl(recvTime) - 2208988800;   //��ʼ��1900�� = 2208988800
    *newTime = recvTime;  
    
    return 1;
}

//2:�ɹ�   3:ʧ��   0:����ԭ��ʧ��
int SetSyn_NetTime(void)
{
	time_t		netTime;
	struct tm	*tt;
	RtcTime_st	SNetTime;
	int			ret=0;
//U32 tick = rawtime(NULL);

	gServSelcFlag = 0;
	ret = SetSyn_GetNetworkTime(&netTime, 3,gServSelcFlag);
	if(ret == -1)//���³���:ret=-1   ���³ɹ�:ret=1
	{
		gServSelcFlag = 1;
		ret = SetSyn_GetNetworkTime(&netTime, 3,gServSelcFlag);
	}
//hyUsbPrintf("updatetime ret = %d   = %d \r\n",ret, ABS(rawtime(NULL)-tick));	
	if(ret == 1)
	{
		tt = localtime(&netTime);

		SNetTime.second	= tt->tm_sec;
		SNetTime.minute	= tt->tm_min;
		SNetTime.hour	= tt->tm_hour;
		SNetTime.day    = tt->tm_mday;
		SNetTime.month	= tt->tm_mon+1;
		SNetTime.year	= tt->tm_year+1900;
		
        if( SNetTime.year<1950 || SNetTime.year>2085 || SNetTime.month<1 || SNetTime.month>12 ||
            SNetTime.day<1     || SNetTime.day>31    || SNetTime.hour<0  || SNetTime.hour>24  || 
            SNetTime.minute<0  || SNetTime.minute>59 || SNetTime.second<0|| SNetTime.second>59 ) 
        {//���µ�ʱ�䳬��������֧�ֵ�ʱ�䷶Χ
			ret = -1; 
        }
        else 
        {
			hyhwRtc_SetTime(&SNetTime);
			ret = 2;
		}
	}
	else
	{
		ret = 0;
	}
	
	return ret;
}


#endif //#ifdef VER_CONFIG_HYCO