#include "std.h"
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>


#include "libgsmd.h"
#include "misc.h"

#include "usock.h"
//#include "ts0705.h"
#include "app_access_sim.h"
#include "app_shell.h"


u_int32_t appLockAccessSIM = 0;

extern int gsmcdma_ph_detect_simcard;
extern int gsmcdma_ph_simcard;


/*-----------------------------------------------------------------------------
* ����:	phone_access_SIM
* ����:	
* ����:	
* ����:	0:OK
*----------------------------------------------------------------------------*/
int phone_access_SIM(accessPhone_u *u, u_int8_t subtype)
{
	int		brk,fd,len,rc = -1;
	char 	*payload;
	char	buf[128+1];
	struct 	lgsm_handle *lh;
	struct 	gsmd_msg_hdr *gmh;

	
	//����Ĵ���ǿ����룬����Ҫlock
	(void)wlock((unsigned)(&appLockAccessSIM));
	lh = lgsm_init(USER_OTHER);
	if (!lh) 
	{
		(void)unlock((unsigned)(&appLockAccessSIM));
		return -1;	//Can't connect to server thread
	}
	fd = lgsm_fd(lh);
	brk = 0;

	switch(subtype)
	{
	case GSMD_PHONE_GET_IMSI:
		lgsm_get_imsi(lh);
		break;

	case GSMD_PHONE_GET_SERIAL:
		lgsm_get_serial(lh);
		break;

	case GSMD_PHONE_POWER:
		lgsm_phone_power(lh, u->cfun.funMode);
		break;

	case GSMD_PHONE_DETECT_SIMCARD:
		lgsm_detect_simcard(lh);
		break;
	}

	while (1)
	{
		//receive data on the gsmd socket
		len = u_recv(fd, buf, sizeof(buf), 0);
		if(0<len)
		{
			/* we've received something on the gsmd socket, pass it
			 * on to the library */
			if (len < sizeof(*gmh))
				break;

			gmh = (struct gsmd_msg_hdr *) buf;
			if (len - sizeof(*gmh) < gmh->len)
				break;
				
			if (GSMD_MSG_PHONE != gmh->msg_type)
				break;
			
			switch (gmh->msg_subtype)
			{
			case GSMD_PHONE_GET_IMSI:	//IMSI
			case GSMD_PHONE_GET_SERIAL:	//IMEI
				payload = (char *)((char *)gmh + sizeof(*gmh));
				if(isdigit(payload[0]))
				{
					strlcpy(u->simSerial, payload, sizeof(u->simSerial));
					rc = 0;
				}
				brk = 1;
				break;

			case GSMD_PHONE_POWER:
				rc = *(int *) gmh->data;

				brk = 1;
				break;

			case GSMD_PHONE_DETECT_SIMCARD:
				payload = (char *)((char *)gmh + sizeof(*gmh));
				if('0'==payload[0] || '1'==payload[0])
				{
					u->simDetect = (int)(payload[0] - '0');
					rc = 0;
				}
				brk = 1;
				break;

			default:
				brk = 1;
				break;
			}

			if(1 == brk)
				break;
		}
		else
		{
			sleep(1);
		}
	}
	lgsm_exit(lh);
	
	(void)unlock((unsigned)(&appLockAccessSIM));
	
    return rc;
}



/*-----------------------------------------------------------------------------
* ����:	phone_detect_simcard
* ����:	����Ƿ���SIM����AT+ESIMS �C Query SIM Status
* ����:	
* ����:	1:�п�	0:�޿�	-1:���ʧ��
*----------------------------------------------------------------------------*/
int phone_detect_simcard(void)
{
	int rc=-1;
	accessPhone_u v;

	if(0 == phone_access_SIM(&v, GSMD_PHONE_DETECT_SIMCARD))
	{
		rc = v.simDetect;
	}

	/*
	rc = pin_check_simcard();
	if(0 == rc)
		rc = 1;
	else if(-2 == rc)
		rc = 0;
	*/
	
	return rc;
}



/*-----------------------------------------------------------------------------
* ����:	phone_detect_simcard_nrt
* ����:	��ʵʱ����Ƿ���SIM��(����Ҫ�󲻵ȴ��Ҷ�������ֵ��Ҫ��ʵʱֵ�ĵط�����)
* ����:	
* ����:	1:�п�	0:�޿�
*----------------------------------------------------------------------------*/
int phone_detect_simcard_nrt(void)
{
	gsmcdma_ph_detect_simcard = 1;
	return gsmcdma_ph_simcard;
}

/*-----------------------------------------------------------------------------
* ����:	phone_onlyRead_simcardStatus
* ����:	ֻ��ȡsim���Ƿ���ڵ�״̬��������ʵ��sim�����
* ����:	
* ����:	1:�п�	0:�޿�
*----------------------------------------------------------------------------*/
int phone_onlyRead_simcardStatus(void)
{
	return gsmcdma_ph_simcard;
}


/*-----------------------------------------------------------------------------
* ����:	phone_get_sim_imsi
* ����:	��ȡSIM����IMSI(International Mobile Subscriber Identity),�������ƶ���
*		��ʶ����
* ����:	accessPhone_u *v
* ����:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phone_get_sim_imsi(accessPhone_u *v)
{
	return phone_access_SIM(v, GSMD_PHONE_GET_IMSI);
}



/*-----------------------------------------------------------------------------
* ����:	phone_get_IMEI
* ����:	��ȡģ���IMEI(international mobile equipment identifier),�������ƶ���
*		��ʶ����
* ����:	accessPhone_u *v
* ����:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phone_get_IMEI(accessPhone_u *v)
{
	return phone_access_SIM(v, GSMD_PHONE_GET_SERIAL);
}



/*-----------------------------------------------------------------------------
* ����:	phone_set_fun_mode
* ����:	����GSMģ��Ĺ���ģʽ
* ����:	funMode  ---  1:����ģʽ  0:����ģʽ
* ����:	0:SUCCESS	other:FAIL
*----------------------------------------------------------------------------*/
int phone_set_fun_mode(int funMode)
{
	int rc;

#ifdef GSM_MODULE_ULC2

	if(1 == funMode)
	{
		accessNet_u	v;

		memset(v.operName, 0, sizeof(v.operName));
		rc = net_register_operator(&v);

		//ERR_SIM_WRONG				SIM�����󣬸�SIM����ע��
		//ERR_SIM_NOT_INSERTED		û�м�⵽SIM��
		if(ERR_SIM_WRONG==rc || ERR_SIM_NOT_INSERTED==rc)
			rc = 0;
	}
	else
	{
		rc = net_deregister_operator();

		//ERR_SIM_NOT_INSERTED		û�м�⵽SIM��
		if(ERR_SIM_NOT_INSERTED == rc)
		{
			accessPhone_u v;
			
			//silent reset (reset MS without resetting the SIM).
			v.cfun.funMode = 15;
			rc = phone_access_SIM(&v, GSMD_PHONE_POWER);
		}
	}

#elif defined GSM_MODULE_MD231

	accessPhone_u v;

	if(1 == funMode)
		v.cfun.funMode = 1;
	else
		v.cfun.funMode = 4;

	rc = phone_access_SIM(&v, GSMD_PHONE_POWER);
#endif

	return rc;
}



/*-----------------------------------------------------------------------------
* ����:	phone_get_fun_mode
* ����:	���GSMģ�鵱ǰ�Ĺ���ģʽ
* ����:	
* ����:	1:����ģʽ  0:����ģʽ
*----------------------------------------------------------------------------*/
/*
int phone_get_fun_mode(void)
{
	return gsmcdma_fun_mode;
}
*/
