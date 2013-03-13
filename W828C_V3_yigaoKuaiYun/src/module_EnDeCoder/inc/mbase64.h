/*                 base64.h           */
#ifndef _MBASE64_H_
#define _MBASE64_H_

#include "hyTypes.h"

/*
	data:��������ִ�
	data_len:�ִ�����
	encode:�������ִ�
	encodelen:in:encode buf�ĳ���  out:ʵ�ʱ����ĳ���   ʧ��ʱ���ر������賤��
	
	����:0:�ɹ�  -1:ʧ��
*/
int mbase64_encode(const char *data, int data_len, char *encode, int *encodelen);

/*
	data:��������ִ�
	data_len:�ִ�����
	decode:�������ִ�
	decodelen:in:decode buf�ĳ���  out:ʵ�ʽ����ĳ���   ʧ��ʱ���ؽ������賤��
	
	����:0:�ɹ�  -1:ʧ��
*/
int mbase64_decode(const char *data, int data_len, char *decode, int *decodelen);


#endif /* _MBASE64_H_ */