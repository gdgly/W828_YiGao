#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "hyErrors.h"
#include "semaphore.h"

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/


typedef struct msg_qentry_t tMSG_QENTRY;

/* ��Ϣ����ͷ�Ľṹ�� */
typedef struct msg_queue_t
{
	SEMAPHORE	*getsem;			/* �����ź�������¼��Ϣ�����е�get����Ϣ�� */
	SEMAPHORE	*putsem;			/* �����ź�������¼��Ϣ�����е�put����Ϣ�� */
    tMSG_QENTRY	*q;					/* queue entries in use */
    tMSG_QENTRY	*freelist;			/* queue entries not in use */
    void		*queuespace;		/* placeholder for the dynamically allocated area */
    unsigned int queuespacesize;
    unsigned int itemmaxsize;       /* ��Ϣ������ÿ����Ϣ���ݵ���󳤶� */

} tMSG_QUEUE;

/* һ����Ϣ�Ľṹ�� */
typedef struct msg_qentry_t
{
	struct msg_qentry_t	*next;
	unsigned int	priority;			/* ����Ϣ�����ȼ� */
	unsigned int	buflen;				/* ��Ϣ���ݵĳ��� */
	unsigned int	busy;				/* ����Ϣ���Ƿ�æ�����ڷֶ����������ݣ���֤�ٽ����ڴ��������������� */
										/* ��ʵ�ϣ���busy�����������֤put/get��Э���ģ�������put/putЭ����get/getЭ���� */
	char			*buf;				/* ��Ϣ���ݵ�buff */
	
} tMSG_QENTRY;

#define MSG_QUEUE_OK                HY_OK
/* ��Ϣ��������ʧ�ܵķ���ֵ���� */
#define MSG_QUEUE_ERR_POINT_NULL	(-1)		/* ����ָ��Ϊ�� */
#define MSG_QUEUE_ERR_TIMEOUT		(-2)		/* ��ʱ���� */
#define MSG_QUEUE_ERR_FULL			(-3)		/* ������ */
#define MSG_QUEUE_ERR_EMPTY			(-4)		/* ���п� */
#define MSG_QUEUE_ERR_SEM			(-5)		/* �����ź����������� */
#define MSG_QUEUE_ERR_PARAM			(-6)		/* �������� */
#define MSG_QUEUE_ERR_FAIL			(-7)		/* ����ʧ�� */

/*----------------------------------------------------------------------
* ����: msg_queue_init
* ˵��: ������Ϣ���ж���
* ����: maxmsgs: �����Ϣ���е���Ϣ����
*		maxmsgsize��ÿ����Ϣ���ݵ����ռ�ÿռ�(����byte)
* ����: ��������ɹ�������ָ�����Ϣ���е�ָ�룻���򷵻ؿ�ָ��
*-----------------------------------------------------------------------*/
tMSG_QUEUE *msg_queue_init( int maxmsgs, int maxmsgsize );


/*----------------------------------------------------------------------
* ����: msg_queue_destroy
* ˵��: ������Ϣ����
* ����: pqueue�� ָ����Ϣ���е�ָ��
* ����: 0:SUCCESS or -1:FAIL
*		����н�������ʹ�ø���Ϣ����(�ڵȴ��б���)�����������٣�����ʧ�ܡ�
* ע�⣺����жϵ�����ֵΪSUCCESSʱ��˵�����ٳɹ�������pqueueΪNULL��
*		��ֹ�´λ��������̵������á�
*-----------------------------------------------------------------------*/
int msg_queue_destroy( tMSG_QUEUE *pqueue );


/*----------------------------------------------------------------------
* ����: msg_queue_put
* ˵��: ��Ϣ�����з���һ����Ϣ(�ȴ���ʽ)
* ����: pqueue: ��Ϣ���ж���ָ��
*		buf: Ҫ�ŵ���Ϣ������ָ�룬����ΪNULL
*		len: Ҫ�ŵ���Ϣ�����ݵĳ���
*		prio:����Ϣ�����ȼ�
* ����: 0: success; < 0: fail
* ˵���������Ϣ���Ѿ�������ȴ�ֱ���ɷ������ϢΪֹ
*-----------------------------------------------------------------------*/
int msg_queue_put( tMSG_QUEUE *pqueue, char *buf, int len, unsigned int prio);


/*----------------------------------------------------------------------
* ����: msg_queue_timed_put
* ˵��: ��Ϣ�����з���һ����Ϣ(ʱ�����Ʒ�ʽ)
* ����: pqueue: ��Ϣ���ж���ָ��
*		buf: Ҫ�ŵ���Ϣ������ָ�룬����ΪNULL
*		len: Ҫ�ŵ���Ϣ�����ݵĳ���
*		prio:����Ϣ�����ȼ�
*		ticks:���Ƶȴ���ʱ��
* ����: 0: success; < 0: fail ��-2����ʱ���أ�
* ˵���������Ϣ���Ѿ�������ȴ����ɷ������Ϣ��ʱ����
*-----------------------------------------------------------------------*/
int msg_queue_timed_put( tMSG_QUEUE *pqueue, char *buf, int len, unsigned int prio, unsigned int ticks);


/*----------------------------------------------------------------------
* ����: msg_queue_tryput
* ˵��: ��Ϣ�����з���һ����Ϣ(���ȴ��ķ�ʽ)
* ����: pqueue: ��Ϣ���ж���ָ��
*		buf: Ҫ�ŵ���Ϣ������ָ�룬����ΪNULL
*		len: Ҫ�ŵ���Ϣ�����ݵĳ���
*		prio:����Ϣ�����ȼ�
* ����: 0: success; < 0: fail
* ˵���������Ϣ���Ѿ������򲻵ȴ�ֱ�ӷ���
*-----------------------------------------------------------------------*/
int msg_queue_tryput( tMSG_QUEUE *pqueue, char *buf, int len, unsigned int prio);


/*----------------------------------------------------------------------
* ����: msg_queue_get
* ˵��: ����Ϣ������ȡ��һ����Ϣ(�ȴ��ķ�ʽ)
* ����: pqueue: ��Ϣ���ж���ָ��
*		buf:	��Ϣ����Ҫд��ĵ�ַ�������ַ�����ڵ��øú���ǰ����ã�
*				����Ӧ�ô���/����initʱ��maxmsgsize����
*		len:	���صõ�����Ϣ���ݵĳ���
* ����: 0: success; < 0: fail
* ˵���������Ϣ���Ѿ��գ���ȴ�ֱ������Ϣ����Ϊֹ
*-----------------------------------------------------------------------*/
int msg_queue_get( tMSG_QUEUE *pqueue, char *buf, int *len);


/*----------------------------------------------------------------------
* ����: msg_queue_timed_get
* ˵��: ����Ϣ������ȡ��һ����Ϣ(ʱ�����Ƶķ�ʽ)
* ����: pqueue: ��Ϣ���ж���ָ��
*		buf:	��Ϣ����Ҫд��ĵ�ַ�������ַ�����ڵ��øú���ǰ����ã�
*				����Ӧ�ô���/����initʱ��maxmsgsize����
*		len:	���صõ�����Ϣ���ݵĳ���
* ����: 0: success; < 0: fail
* ˵���������Ϣ���Ѿ��գ���ȴ�������Ϣ������߳�ʱ����
*-----------------------------------------------------------------------*/
int msg_queue_timed_get( tMSG_QUEUE *pqueue, char *buf, int *len, unsigned int ticks);


/*----------------------------------------------------------------------
* ����: msg_queue_tryget
* ˵��: ����Ϣ������ȡ��һ����Ϣ(���ȴ��ķ�ʽ)
* ����: pqueue: ��Ϣ���ж���ָ��
*		buf:	��Ϣ����Ҫд��ĵ�ַ�������ַ�����ڵ��øú���ǰ����ã�
*				����Ӧ�ô���/����initʱ��maxmsgsize����
*		len:	���صõ�����Ϣ���ݵĳ���
* ����: 0: success; < 0: fail
* ˵���������Ϣ���Ѿ������򲻵ȴ�ֱ�ӷ���
*-----------------------------------------------------------------------*/
int msg_queue_tryget( tMSG_QUEUE *pqueue, char *buf, int *len);




#endif /* MESSAGE_QUEUE_H */


