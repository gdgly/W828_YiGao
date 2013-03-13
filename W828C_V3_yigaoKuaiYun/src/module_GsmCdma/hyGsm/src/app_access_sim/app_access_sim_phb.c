
#include "std.h"
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>

#include "libgsmd.h"
#include "misc.h"

#include "usock.h"
#include "phonebook.h"
#include "app_access_sim.h"


#define   LITTLE_ENDIAN 0 //С�˱��뷽ʽ
#define   BIG_ENDIAN    1 //��˱��뷽ʽ


/*-----------------------------------------------------------------------------
* ����:	phb_access_SIM
* ����:	
* ����:	
* ����:	0:OK	
*----------------------------------------------------------------------------*/
int phb_access_SIM(accessSIM_u *u, u_int8_t subtype)
{
	int		brk,fd,len,readNum=0,rc = -1;
	char 	*payload;
	char	buf[512+1];
	struct 	lgsm_handle *lh;
	struct 	gsmd_msg_hdr *gmh;
	
	struct gsmd_phonebook_mem *gpm;
	struct gsmd_phonebook *gp;
	struct gsmd_phonebooks *gps;


	
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
	case GSMD_PHONEBOOK_GET_STORAGE:
		lgsm_pb_get_storage(lh);
		break;
	case GSMD_PHONEBOOK_READ:
		lgsm_pb_read_entry(lh, u->rwNode.index);
		break;
	case GSMD_PHONEBOOK_READRG:
		{
		struct lgsm_phonebook_readrg pb_readrg;
		
		pb_readrg.index1 = 1;
		pb_readrg.index2 = u->readAllNode.total;
		lgsm_pb_read_entries(lh,&pb_readrg);
		}
		break;
	case GSMD_PHONEBOOK_WRITE:
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

		lgsm_pb_write_entry(lh,	&phb);
		}
		break;
	case GSMD_PHONEBOOK_DELETE:
		if(u->delNode.delNum > 0)
			lgsm_pb_del_entry(lh, u->delNode.delNodeIndex[--(u->delNode.delNum)]);
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
				
			if (GSMD_MSG_PHONEBOOK != gmh->msg_type)
				break;
			
			switch (gmh->msg_subtype)
			{		
			case GSMD_PHONEBOOK_GET_STORAGE:
				gpm = (struct gsmd_phonebook_mem *)((char *)gmh + sizeof(*gmh));
				if(gpm->total > 0)
				{
					//DEBUGP("%s, %d, %d\n", gpm->type, gpm->used, gpm->total);
					u->storage.used = gpm->used;
					u->storage.total = gpm->total;
					rc = 0;
				}
				brk = 1;
				break;

			case GSMD_PHONEBOOK_READ:
				gp = (struct gsmd_phonebook *) ((char *)gmh + sizeof(*gmh));
				if (gp->index >0)
				{
					//u->rwNode.index = gp->index;
					strlcpy(u->rwNode.number, gp->numb, GSMD_PB_NUMB_MAXLEN+1);
					//u->rwNode.type = gp.type;
					//strlcpy(u->rwNode.name, gp->text, GSMD_PB_TEXT_MAXLEN+1);
					Hyc_Unicode2Gbk(gp->text, u->rwNode.name, BIG_ENDIAN);
					gp->text[GSMD_PB_TEXT_MAXLEN] = '\0';
					
					rc = 0;
				}
				brk = 1;
				break;

			case GSMD_PHONEBOOK_READRG:
				gps = (struct gsmd_phonebooks *) ((char *)gmh + sizeof(*gmh));
				if (gps->pb.index > 0)
				{
					//DEBUGP("%d, %s, %d, %s\n",
					//		gps->pb.index, gps->pb.numb,
					//		gps->pb.type, gps->pb.text);

					if(gps->pb.index <= u->readAllNode.total)
					{
						gp = (struct gsmd_phonebook *)(u->readAllNode.allNode+(gps->pb.index-1));

						//memcpy(gp, &(gps->pb), sizeof(struct gsmd_phonebook));
						gp->index = gps->pb.index;
						strlcpy(gp->numb, gps->pb.numb, GSMD_PB_NUMB_MAXLEN+1);
						gp->type = gps->pb.type;
						Hyc_Unicode2Gbk(gps->pb.text, gp->text, BIG_ENDIAN);
						gp->text[GSMD_PB_TEXT_MAXLEN] = '\0';
						
						readNum++;
					}
				}
				else
				{
					//��¼��ȡ���ִ��󣬻��¼Ϊ��
					brk = 1;
				}
				
				if (gps->is_last)
				{
					//���м�¼��ȡ���
					u->readAllNode.total = readNum;
					
					rc = 0;
					brk = 1;
				}
				break;


			case GSMD_PHONEBOOK_WRITE:
				/* TODO: Need to handle error */
				payload = (char *)gmh + sizeof(*gmh);
				if(NULL!= payload && !strncmp(payload, "OK", 2))
				{
					//DEBUGP("%s\n", payload);
					rc = 0;
				}
				brk = 1;
				break;

			case GSMD_PHONEBOOK_DELETE:
				/* TODO: Need to handle error */
				payload = (char *)gmh + sizeof(*gmh);
				if(NULL!= payload && !strncmp(payload, "OK", 2))
				{
					//DEBUGP("%s\n", payload);
					if(u->delNode.delNum > 0)
						lgsm_pb_del_entry(lh, u->delNode.delNodeIndex[--(u->delNode.delNum)]);
					else
					{
						//���м�¼ɾ�����
						rc = 0;
						brk = 1;
					}
				}
				else
				{
					//��¼ɾ������
					brk = 1;
				}
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
* ����:	phb_SIM_check_ready_t
* ����:	���SIM���绰���Ƿ���׼����
* ����:	
* ����:	0:Ready   -1:Check Fail   -2:SIM not inserted
*----------------------------------------------------------------------------*/
int phb_SIM_check_ready_t(void)
{
	int rc;


	rc = phone_detect_simcard();
	if(0 == rc)
	{
		//�޿�
		rc = -2;
	}
	else if(-1 == rc)
	{
		//���ʧ��
		rc = -1;
	}
	else
	{
		//�п�
		accessSIM_u v;
		
		if(0 == phb_SIM_get_storage_t(&v))
		{
			rc = 0;
		}
		else
		{
			//���ʧ��
			rc = -1;
		}
	}
	
	return rc;
}




/*-----------------------------------------------------------------------------
* ����:	phb_SIM_get_storage_t
* ����:	��ȡSIM���绰�����ÿռ䡢�ܴ洢�ռ�
* ����:	accessSIM_u *v
* ����:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phb_SIM_get_storage_t(accessSIM_u *v)
{
	return phb_access_SIM(v, GSMD_PHONEBOOK_GET_STORAGE);
}


/*-----------------------------------------------------------------------------
* ����:	phb_SIM_read_node_t
* ����:	��ȡSIM���绰����ָ��index�ļ�¼
* ����:	accessSIM_u *v
* ����:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phb_SIM_read_node_t(accessSIM_u *v)
{
	return phb_access_SIM(v, GSMD_PHONEBOOK_READ);
}


/*-----------------------------------------------------------------------------
* ����:	phb_SIM_read_allNode_t
* ����:	��ȡSIM���绰�������м�¼
* ����:	accessSIM_u *v
* ����:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phb_SIM_read_allNode_t(accessSIM_u *v)
{
	return phb_access_SIM(v, GSMD_PHONEBOOK_READRG);
}


/*-----------------------------------------------------------------------------
* ����:	phb_SIM_write_node_t
* ����:	д��SIM���绰����ָ��index�ļ�¼
* ����:	accessSIM_u *v
* ����:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phb_SIM_write_node_t(accessSIM_u *v)
{
	return phb_access_SIM(v, GSMD_PHONEBOOK_WRITE);
}


/*-----------------------------------------------------------------------------
* ����:	phb_SIM_write_node_t
* ����:	д��SIM���绰����ָ��index�ļ�¼
* ����:	accessSIM_u *v
* ����:	0:SUCCESS	-1:FAIL
*----------------------------------------------------------------------------*/
int phb_SIM_del_node_t(accessSIM_u *v)
{
	return phb_access_SIM(v, GSMD_PHONEBOOK_DELETE);
}



//int phb_self_auto_update(U8 *name, U8 *number)




