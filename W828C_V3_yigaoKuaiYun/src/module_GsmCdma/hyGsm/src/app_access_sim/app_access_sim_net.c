#include "std.h"
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>


#include "libgsmd.h"
#include "misc.h"

#include "usock.h"
//#include "ts0705.h"
#include "app_access_sim.h"
#include "phonebook.h"


extern int gsmcdma_net_query_reg;
extern int gsmcdma_net_query_sigq;
extern int gsmcdma_net_reg_stat;
extern u_int32_t gsmcdma_net_sigq;


/*-----------------------------------------------------------------------------
* ����:	net_access_SIM
* ����:	
* ����:	
* ����:	0:SUCCESS 	-1:FAIL
*----------------------------------------------------------------------------*/
int net_access_SIM(accessNet_u *u, u_int8_t subtype)
{
	int		brk,fd,len,readNum=0,rc = -1;
	char 	*payload;
	char	buf[128+1];
	struct 	lgsm_handle *lh;
	struct 	gsmd_msg_hdr *gmh;
	enum gsmd_netreg_state state;
	struct gsmd_own_number *num;
	struct gsmd_phonebook *gp;


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
	case GSMD_NETWORK_QUERY_REG:
		lgsm_netreg_query(lh);
		break;
		
	case GSMD_NETWORK_GET_NUMBER:
		lgsm_get_subscriber_num(lh);
		break;

	case GSMD_NETWORK_SET_NUMBER:
	
#ifdef GSM_MODULE_ULC2
		{
		int i,j;
		char name[GSMD_PB_TEXT_MAXLEN+1];
		struct lgsm_phonebook phb;
		
		phb.index = u->rwNode.index;
		memcpy(phb.numb, u->rwNode.number, sizeof(phb.numb));
		phb.numb[sizeof(phb.numb)-1] = '\0';
		if ('+' == phb.numb[0])
			phb.type = LGSM_PB_ATYPE_INTL;
		else
			phb.type = LGSM_PB_ATYPE_OTHE;
		//memcpy(phb.text, u->rwNode.name, sizeof(phb.text));
		//phb.text[sizeof(phb.text)-1] = '\0';
		
		//����6���ֽض�
		memcpy(name, u->rwNode.name, sizeof(name));
		for(i=0,j=0;j<6 && name[i]!=0;j++)
		{
			if(name[i] >= 0x80)
				i+=2;
			else
				i++;
		}
		name[i] = 0;
		Hyc_Gbk2Unicode(name, phb.text, LITTLE_ENDIAN);

		lgsm_set_subscriber_num(lh,	&phb);
		}

#elif defined GSM_MODULE_MD231

		{
		int i,j;
		char *name;
		struct lgsm_phonebook phb;

		phb.index = u->rwNode.index - 1;
		memcpy(phb.numb, u->rwNode.number, sizeof(phb.numb));
		phb.numb[sizeof(phb.numb)-1] = '\0';
		
		i=0;
		name = u->rwNode.name;
		while(i<14)
		{
			if(name[i] >= 0x80)
				break;
			
			i++;
		}

		if(14 == i)
		{
			phb.type = 0;	//�������ֵ�GBK����
			
			memcpy(phb.text, u->rwNode.name, sizeof(phb.text));
			phb.text[sizeof(phb.text)-1] = '\0';
		}
		else
		{
			phb.type = 1;	//���к��ֵ�GBK����
			
			//����6���ֽض�
			name = u->rwNode.name;
			for(i=0,j=0;j<6 && name[i]!=0;j++)
			{
				if(name[i] >= 0x80)
					i+=2;
				else
					i++;
			}
			name[i] = 0;
			Hyc_Gbk2Unicode(u->rwNode.name, phb.text, LITTLE_ENDIAN);
		}
		lgsm_set_subscriber_num(lh,	&phb);
		}
#endif

		break;

	case GSMD_NETWORK_OPER_GET:
		lgsm_oper_get(lh);
		memset(u->operName, 0, sizeof(u->operName));
		break;

	case GSMD_NETWORK_REGISTER:
		{
		gsmd_oper_numeric oper;
		
		memcpy(oper, u->operName, sizeof(gsmd_oper_numeric));
		lgsm_netreg_register(lh, oper);
		}
		break;

	case GSMD_NETWORK_DEREGISTER:
		lgsm_netreg_deregister(lh);
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
				
			if (GSMD_MSG_NETWORK != gmh->msg_type)
				break;

			switch (gmh->msg_subtype)
			{
			case GSMD_NETWORK_QUERY_REG:
				state = *(enum gsmd_netreg_state *) gmh->data;
				rc = 0;
				
				switch (state) 
				{
				case GSMD_NETREG_UNREG:
				case GSMD_NETREG_DENIED:
					u->netreg_stat = -1;	//δע��
					break;
				case GSMD_NETREG_REG_HOME:
				case GSMD_NETREG_REG_ROAMING:
					u->netreg_stat = 1;		//��ע��
					break;
				case GSMD_NETREG_UNREG_BUSY:
					u->netreg_stat = 0;		//����ע��
					break;

				default:
					rc = -1;
					break;
				}
				brk = 1;
				break;

			case GSMD_NETWORK_GET_NUMBER:
				num = (struct gsmd_own_number *)gmh->data;
				if(num->is_last >= 0)
				{
					gp = (struct gsmd_phonebook *)(u->ownNumber.allNode+readNum);
					readNum++;
					
					strlcpy(gp->numb, num->addr.number, GSMD_PB_NUMB_MAXLEN+1);
					Hyc_Unicode2Gbk(num->name, gp->text, BIG_ENDIAN);
					gp->text[GSMD_PB_TEXT_MAXLEN] = '\0';

					if(1 == num->is_last)
					{
						u->ownNumber.total = readNum;
					
						rc = 0;
						brk = 1;
					}
				}
				else
				{
					if(-1 == num->is_last)
					{
						//û�м�¼(totalΪ0)
						u->ownNumber.total = 0;
					
						rc = 0;
					}
					brk = 1;
				}
				break;

			case GSMD_NETWORK_SET_NUMBER:
				payload = (char *)gmh + sizeof(*gmh);
				if(NULL!= payload && !strncmp(payload, "OK", 2))
				{
					//DEBUGP("%s\n", payload);
					rc = 0;
				}
				brk = 1;
				break;

			case GSMD_NETWORK_OPER_GET:
				payload = (char *)gmh + sizeof(*gmh);
				if(NULL!= payload && strlen(payload)>0)
				{
					//DEBUGP("%s\n", payload);
					strncpy(u->operName, payload, sizeof(u->operName)-1);
					rc = 0;
				}
				brk = 1;
				break;

			case GSMD_NETWORK_REGISTER:
				rc = *(int *) gmh->data;
				
				//+CME ERROR: 10	SIM not inserted
				//+CME ERROR: 15	SIM wrong
				brk = 1;
				break;

			case GSMD_NETWORK_DEREGISTER:
				rc = *(int *) gmh->data;

				//+CME ERROR: 10	SIM not inserted
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
* ����:	net_query_reg
* ����:	ʵʱ��ѯGSM/CDMAע��״̬(ע��״̬��accessNet_u�ṹ�е�netreg_stat���أ�
*		netreg_stat  ---  -1:δע��  0:����ע��   1:��ע��)
* ����:	accessNet_u *v
* ����:	0:SUCCESS(��ѯ�ɹ�)	-1:FAIL(��ѯʧ��)
*----------------------------------------------------------------------------*/
int net_query_reg(accessNet_u *v)
{
	return net_access_SIM(v, GSMD_NETWORK_QUERY_REG);
}




/*-----------------------------------------------------------------------------------
* ����:	net_query_reg_nrt
* ����:	��ʵʱ��ѯGSM/CDMAע��״̬(����Ҫ�󲻵ȴ��Ҷ�������ֵ��Ҫ��ʵʱֵ�ĵط�����)
*		
* ����:	
* ����:	-1:δע��  0:����ע��   1:��ע��
*-----------------------------------------------------------------------------------*/
int net_query_reg_nrt(void)
{
	gsmcdma_net_query_reg = 1;
	if(get_modem_init_stat() == 1)
	{
		return gsmcdma_net_reg_stat;
	}
	else
	{
		return 0;
	}
}
/*-----------------------------------------------------------------------------------
* ����:	net_onlyRead_regStatus
* ����:	ֻ��ȡ�Ƿ�ע���������״̬��������ʵ�ʼ��
* ����:	
* ����:	-1:δע��  0:����ע��   1:��ע��
*-----------------------------------------------------------------------------------*/
int net_onlyRead_regStatus(void)
{
	if(get_modem_init_stat() == 1)
	{
		return gsmcdma_net_reg_stat;
	}
	else
	{
		return 0;
	}
}


/*-----------------------------------------------------------------------------------
* ����:	net_query_sigq_set
* ����:	��ʵʱ��ѯGSM/CDMA�ź�ǿ��(����Ҫ�󲻵ȴ��Ҷ�������ֵ��Ҫ��ʵʱֵ�ĵط�����)
*		
* ����:	
* ����:	�ź�ǿ�� 0-31,99
*-----------------------------------------------------------------------------------*/
u_int32_t net_query_sigq_nrt(void)
{
	gsmcdma_net_query_sigq = 1;
	return gsmcdma_net_sigq;
}



/*-----------------------------------------------------------------------------
* ����:	net_get_subscriber_num
* ����:	��ѯ��������
*		
* ����:	accessNet_u *v
* ����:	0:SUCCESS(��ѯ�ɹ�)	-1:FAIL(��ѯʧ��)
*----------------------------------------------------------------------------*/
int net_get_subscriber_num(accessNet_u *v)
{
	return net_access_SIM(v, GSMD_NETWORK_GET_NUMBER);
}



/*-----------------------------------------------------------------------------
* ����:	net_set_subscriber_num
* ����:	���ñ�������
*		
* ����:	accessNet_u *v
* ����:	0:SUCCESS(���óɹ�)	-1:FAIL(����ʧ��)
*----------------------------------------------------------------------------*/
int net_set_subscriber_num(accessNet_u *v)
{
	return net_access_SIM(v, GSMD_NETWORK_SET_NUMBER);
}



/*-----------------------------------------------------------------------------
* ����:	net_get_operator
* ����:	��ѯ��ǰ������Ӫ��
*		
* ����:	accessNet_u *v
* ����:	0:SUCCESS(��ѯ�ɹ�)	-1:FAIL(��ѯʧ��)
*----------------------------------------------------------------------------*/
int net_get_operator(accessNet_u *v)
{
	return net_access_SIM(v, GSMD_NETWORK_OPER_GET);
}



/*-----------------------------------------------------------------------------
* ����:	net_register_operator
* ����:	��������Ӫ��ע��
*		
* ����:	accessNet_u *v
* ����:	0:						SUCCESS(ע��ɹ�)
		ERR_SIM_NOT_INSERTED:	û�м�⵽SIM��
		ERR_SIM_WRONG:			SIM�����󣬸�SIM����ע��
		others:					δ֪����
*----------------------------------------------------------------------------*/
int net_register_operator(accessNet_u *v)
{
	return net_access_SIM(v, GSMD_NETWORK_REGISTER);
}



/*-----------------------------------------------------------------------------
* ����:	net_deregister_operator
* ����:	��������ע��ע��
*		
* ����:	accessNet_u *v
* ����:	0:						SUCCESS(ע���ɹ�)
		ERR_SIM_NOT_INSERTED:	û�м�⵽SIM��
		others:					δ֪����
*----------------------------------------------------------------------------*/
int net_deregister_operator(void)
{
	return net_access_SIM(NULL, GSMD_NETWORK_DEREGISTER);
}



