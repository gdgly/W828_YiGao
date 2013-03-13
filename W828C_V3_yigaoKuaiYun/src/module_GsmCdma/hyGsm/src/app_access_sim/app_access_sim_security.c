
#include "std.h"
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>


#include "libgsmd.h"
#include "misc.h"
#include "app_access_sim.h"



/*-----------------------------------------------------------------------------
* ����:	security_access_SIM
* ����:	
* ����:	
* ����:	0:SUCCESS
*----------------------------------------------------------------------------*/
int security_access_SIM(accessSecurity_u *u, u_int8_t subtype)
{
	int		brk,fd,len,result,rc = -1;
	char	buf[128+1];
	struct 	lgsm_handle *lh;
	struct 	gsmd_msg_hdr *gmh;
	struct gsmd_pin_remain_times *gprt;


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
	case GSMD_SECURITY_PIN_GET_STATUS:
		lgsm_security_pin_status(lh);
		break;
	case GSMD_SECURITY_PIN_INPUT:
		{
		struct gsmd_pin gp;
		
		gp.type = (enum gsmd_pin_type)u->pin.type;
		strncpy(gp.pin, u->pin.pin, sizeof(gp.pin));
		strncpy(gp.newpin, u->pin.newpin, sizeof(gp.newpin));
		lgsm_security_pin_input(lh, &gp);
		}
		break;
	case GSMD_SECURITY_PIN_INPUT_REMAIN_TIMES:
		lgsm_security_pin_remain_times(lh);
		break;
	case GSMD_SECURITY_LOCK_EN:
	case GSMD_SECURITY_LOCK_DIS:
		{
		struct gsmd_facility gfac;

		gfac.type = (enum gsmd_facility_type)u->lock.type;
		strncpy(gfac.pwd, u->lock.pwd, sizeof(gfac.pwd));
		if(GSMD_SECURITY_LOCK_EN == subtype)
			lgsm_security_lock_en(lh, &gfac);
		else
			lgsm_security_lock_dis(lh, &gfac);
		}
		break;
	case GSMD_SECURITY_LOCK_GET_STATUS:
		{
		struct gsmd_facility gfac;

		gfac.type = (enum gsmd_facility_type)u->lock.type;
		lgsm_security_lock_status(lh, &gfac);
		}
		break;
	case GSMD_SECURITY_CHANGE_PWD:
		{
		struct gsmd_facility gfac;

		gfac.type = (enum gsmd_facility_type)u->lock.type;
		strncpy(gfac.pwd, u->lock.pwd, sizeof(gfac.pwd));
		strncpy(gfac.newpwd, u->lock.newpwd, sizeof(gfac.newpwd));
		lgsm_security_change_pwd(lh, &gfac);
		}
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
				
			if (GSMD_MSG_SECURITY != gmh->msg_type)
				break;
			
			switch (gmh->msg_subtype)
			{
			case GSMD_SECURITY_PIN_GET_STATUS:
				result = *(int *) gmh->data;
				if(result >= 0)
				{
					u->pin.type = (enum pin_type)result;
					rc = 0;
				}
				else
				{
					//+CME ERROR: 10	SIM not inserted
					rc = result;
				}
				
				brk = 1;
				break;
			case GSMD_SECURITY_PIN_INPUT:
				rc = *(int *) gmh->data;

				//+CME ERROR: 12	SIM PUK required
				//+CME ERROR: 16	Incorrect password
				//+CME ERROR: 18	SIM PUK2 required
				brk = 1;
				break;
			case GSMD_SECURITY_PIN_INPUT_REMAIN_TIMES:
				gprt = (struct gsmd_pin_remain_times *)gmh->data;
				if(gprt->pin >= 0)
				{
					u->pin_times.pin = gprt->pin;
					u->pin_times.pin2 = gprt->pin2;
					u->pin_times.puk = gprt->puk;
					u->pin_times.puk2 = gprt->puk2;
					
					rc = 0;
				}
				brk = 1;
				break;
				
			case GSMD_SECURITY_LOCK_EN:
			case GSMD_SECURITY_LOCK_DIS:
				rc = *(int *) gmh->data;

				//+CME ERROR: 12	SIM PUK required
				//+CME ERROR: 16	Incorrect password
				brk = 1;
				break;
			case GSMD_SECURITY_LOCK_GET_STATUS:
				rc = *(int *) gmh->data;

				//+CME ERROR: 11	SIM PIN required
				//+CME ERROR: 12	SIM PUK required
				brk = 1;
				break;
			case GSMD_SECURITY_CHANGE_PWD:
				rc = *(int *) gmh->data;

				//+CME ERROR: 3		operation not allowed
				//+CME ERROR: 12	SIM PUK required
				//+CME ERROR: 16	Incorrect password
				//+CME ERROR: 18	SIM PUK2 required
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



/*---------------------------------------------------------------------------------
* ����:	security_check_pin_status
* ����:	��ѯSIM���Ƿ���Ҫ���룬��������ֵΪ0ʱv->pin.type��������Ҫ���������ͣ����
		����ΪPIN_READY�������������
		1)������Ҫ��������
		2)SIM��PUK���ѱ�������ͨ����ѯPUK�����������Ƿ�Ϊ0��ȷ��SIM���ѱ�����
* ����:	accessSecurity_u *v
* ����:	0:						SUCCESS
		ERR_SIM_NOT_INSERTED:	û�м�⵽SIM��   
		others:					δ֪����
*---------------------------------------------------------------------------------*/
int security_check_pin_status(accessSecurity_u *v)
{
	return security_access_SIM(v,GSMD_SECURITY_PIN_GET_STATUS);
}



/*---------------------------------------------------------------------------------
* ����:	security_pin_input
* ����:	����SIM�����룬��������ֵΪ0ʱ������ȷ
* ����:	accessSecurity_u *v
* ����:	0:						SUCCESS
		ERR_SIM_PUK_REQUIRED:	��Ҫ����SIM��PUK��(����PUK��ʱ����Ҫͬʱ�����µ�PIN��)
		ERR_SIM_FAILURE:		SIM����ʧ�ܣ���ʱ��Ҫ�ٲ�ѯPUK�����������Ƿ�
								Ϊ0�����Ϊ0˵��SIM���ѱ�����
		ERR_INCORRECT_PASSWORD:	���벻��ȷ
		ERR_SIM_PUK2_REQUIRED:	��Ҫ����SIM��PUK2��(����PUK��ʱ����Ҫͬʱ�����µ�PIN2��)
		others:					δ֪����
*---------------------------------------------------------------------------------*/
int security_pin_input(accessSecurity_u *v)
{
	return security_access_SIM(v,GSMD_SECURITY_PIN_INPUT);
}



/*---------------------------------------------------------------------------------
* ����:	security_pin_remain_times
* ����:	��ѯSIM��������������
* ����:	accessSecurity_u *v
* ����:	0:						��ѯ�ɹ�
		others:					δ֪����
*---------------------------------------------------------------------------------*/
int security_pin_remain_times(accessSecurity_u *v)
{
		return security_access_SIM(v,GSMD_SECURITY_PIN_INPUT_REMAIN_TIMES);
}


/*---------------------------------------------------------------------------------
* ����:	security_lock_en
* ����:	�����豸��������(��PIN��������)
* ����:	accessSecurity_u *v
* ����:	0:						���������ɹ�
		ERR_SIM_PUK_REQUIRED:	��Ҫ����SIM��PUK��(����PUK��ʱ����Ҫͬʱ�����µ�PIN��)
		ERR_INCORRECT_PASSWORD:	���벻��ȷ
		others:					δ֪����
*---------------------------------------------------------------------------------*/
int security_lock_en(accessSecurity_u *v)
{
	return security_access_SIM(v,GSMD_SECURITY_LOCK_EN);
}


/*---------------------------------------------------------------------------------
* ����:	security_lock_dis
* ����:	����豸��������(��PIN��������)
* ����:	accessSecurity_u *v
* ����:	0:						����������ɹ�
		ERR_SIM_PUK_REQUIRED:	��Ҫ����SIM��PUK��(����PUK��ʱ����Ҫͬʱ�����µ�PIN��)
		ERR_INCORRECT_PASSWORD:	���벻��ȷ
		others:					δ֪����
*---------------------------------------------------------------------------------*/
int security_lock_dis(accessSecurity_u *v)
{
	return security_access_SIM(v,GSMD_SECURITY_LOCK_DIS);
}


/*---------------------------------------------------------------------------------
* ����:	security_lock_status
* ����:	��ѯ�豸����״̬(��PIN��������)
* ����:	accessSecurity_u *v
* ����:	0:						�豸����δ����
		1:						�豸����������
		ERR_SIM_PIN_REQUIRED:	��Ҫ����SIM��PIN��(����ʱ)
		ERR_SIM_PUK_REQUIRED:	��Ҫ����SIM��PUK��(����PUK��ʱ����Ҫͬʱ�����µ�PIN��)
		ERR_SIM_FAILURE:		SIM����ʧ�ܣ���ʱ��Ҫ�ٲ�ѯPUK�����������Ƿ�
								Ϊ0�����Ϊ0˵��SIM���ѱ�����
		others:					δ֪����
*---------------------------------------------------------------------------------*/
int security_lock_status(accessSecurity_u *v)
{
	return security_access_SIM(v,GSMD_SECURITY_LOCK_GET_STATUS);
}


/*---------------------------------------------------------------------------------
* ����:	security_change_pwd
* ����:	�޸�����(��PIN���)
* ����:	accessSecurity_u *v
* ����:	0:							�޸�����ɹ�
		ERR_OPERATION_NOT_ALLOWED:	����������(�����豸����δ����)
		ERR_SIM_PUK_REQUIRED:		��Ҫ����SIM��PUK��(����PUK��ʱ����Ҫͬʱ�����µ�PIN��)
		ERR_INCORRECT_PASSWORD:		���벻��ȷ
		ERR_SIM_PUK2_REQUIRED:		��Ҫ����SIM��PUK2��(����PUK��ʱ����Ҫͬʱ�����µ�PIN2��)
		others:						δ֪����
*---------------------------------------------------------------------------------*/
int security_change_pwd(accessSecurity_u *v)
{
	return security_access_SIM(v,GSMD_SECURITY_CHANGE_PWD);
}


