#ifndef _EVENTENCUMBER_H_
#define _EVENTENCUMBER_H_

/*
ʹ���¼��ȴ�����ʱע�⣺
1	�����ȴ���
2	Event_Wait��Event_Wakeup�ǳɶ�ʹ�õ�
3	������Event_Wait��Ч��Event_Wakeup����Ч������Event_Wait��Ч
4 	������¼��Ƕ�ֵ��
5	ʹ��Event_Waitǰ�����ȹ��жϣ���ָ��ж�
*/

typedef int hyc_event_t;

/*
������Event_Create
���ܣ������ȴ��¼�
����������Event, count Ϊ0,��ʱ����
���أ�0 ==ok,-1==���� 
*/
int Event_Create(hyc_event_t **Event,int count);

/*
������Event_Wait
���ܣ��������¼�ʱ��������״̬
������Event
���أ���
*/
void Event_Wait(hyc_event_t *Event);

/*
������Event_WaitTimeout
���ܣ����̴�ʱ�����������,���TimeoutΪ0�����ܵ�ͬ��Event_Wait
������Event
���أ���
*/
void Event_WaitTimeout(hyc_event_t *Event,int Timeout);
/*
������Event_Wakeup
���ܣ����̻��ѽ��������Ľ���
������Event
���أ���
*/
void Event_Wakeup(hyc_event_t *Event);

/*
������Event_Destroy
���ܣ�����Event
������Event
���أ���
*/
void Event_Destroy(hyc_event_t **Event);

#endif


