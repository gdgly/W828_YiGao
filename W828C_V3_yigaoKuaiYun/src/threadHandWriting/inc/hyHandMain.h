#ifndef __HYHAND_MAIN_H__
#define __HYHAND_MAIN_H__

#include "ISHWR_Engine.h"


//��¼�켣��buf (��Ŵ�����)
#define HY_REC_BUF_SIZE  (1024)

//���buf�ĳ��� (���ձ�ѡ��)
#define HY_OUT_BUF_SIZE   (11)//(100)//

//ʶ��ı�־״̬
#define HY_REG_STASTUS_IDEL  (0)
#define HY_REG_STASTUS_START (1)

//��ǰ�켣��״̬��־
#define HY_TRACE_IDEL    (0)
#define HY_TRACE_START   (1)
#define HY_TRACE_PARSE   (2)

//������д�ϱ�ʱ��
#define HY_TRACE_TIME_PAUSE (8)
#define HY_TRACE_TIME_STOP  (60)//80
//#define HY_TRACE_TIME_PAUSE (5)
//#define HY_TRACE_TIME_STOP  (40)//80

typedef struct _tHY_RECG
{
     void *EventHyRcg;						//����wait��־
     
     unsigned int lastTick;					//�ϴδ�����tick�������жϵ�ǰ�������״̬
     
     unsigned char enable;					//ʶ���Ƿ�����־
     
     unsigned char traceStatus;				//��ǰ�켣��״̬

     unsigned int index;					//��Ч��ĸ���
     ISHWR_POINT_T  recBuf[HY_REC_BUF_SIZE];//�洢��
     
     unsigned char outFlag;					//�Ƿ�������ı�־
     short  outBuf[HY_OUT_BUF_SIZE];		//����ʶ��ѡ��
     char   outBakBuf[HY_OUT_BUF_SIZE*2];	//���ڱ����ַ���
      
}tHY_RECG;

/*---------------------------------------------------
 * ���� : hyHWRecog_RecogTrace
 * ���� : ��¼��д��,��ʱû�е�ʱҲ��Ҫ��ʱ�����
 * ���� : none
 * ���� : none 
---------------------------------------------------*/
void hyHWRecog_RecogTrace(int x, int y);

/*---------------------------------------------------
 * ���� : hyHWRecog_GetLetter
 * ���� : �õ�ʶ���ַ�
 * ���� : pbuf:�����buf
 * ���� : �õ����ַ��� 
---------------------------------------------------*/
int hyHWRecog_GetLetter(char *pbuf);

/*---------------------------------------------------
 * ���� : hyHWRecog_RecognizeMain
 * ���� : ʧ��/ʹ����дʶ��
 * ���� : none
 * ���� : none 
---------------------------------------------------*/
void hyHWRecog_EnableFlag(int type);

/*---------------------------------------------------
 * ���� : hyHWRecog_RecognizeMain
 * ���� : ��дʶ����ѭ������
 * ���� : none
 * ���� : none 
---------------------------------------------------*/
void hyHWRecog_RecognizeMain(void);

#endif