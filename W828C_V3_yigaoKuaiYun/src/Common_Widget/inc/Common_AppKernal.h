#ifndef _COMMON_APPKERNAL_H_
#define _COMMON_APPKERNAL_H_

typedef struct _tCARTOONPARA
{
	int		datalen;
	char	*pdata;

}tCARTOONPARA;

/*--------------------------------------------------------
 * ���� ��kernal_app_complete
 * ���� : �ж��ں˽����Ƿ����
 * ���� : ��������� 0xff��δ��� 
 *-------------------------------------------------------*/
int kernal_app_get_stat(void);

/*--------------------------------------------------------
 * ���� ��kernal_app_config_entry
 * ���� : ע��������к���
 * ���� : ����ָ��
 * ���� : none 
 *-------------------------------------------------------*/
void kernal_app_config_entry(void (*process)(void));

/*--------------------------------------------------------
 * ���� ��Kernal_App_ThreadMethod
 * ���� : Ӧ�ý��̵��ں˽���
 * ���� : 1 
 *-------------------------------------------------------*/
int Kernal_App_ThreadMethod(void);

#endif