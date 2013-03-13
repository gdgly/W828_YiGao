#ifndef __RES_MANAGE_H__
#define __RES_MANAGE_H__

#define RES_TYPE_ZLIB     (0)
#define RES_TYPE_JPG      (1)
#define RES_TYPE_BITMAP   (2)

//
typedef struct _tRES_PARA
{
    unsigned int     id; //��Դ���
    unsigned int     type; //��Դ��������� , ���ڽ�ѹ��ֱ����ȡ
    unsigned short 	 high;  //��Դ��
    unsigned short   width; //��Դ��
    unsigned int     size;  //��ȡѹ��ͼƬ�Ĵ�С
    unsigned int     offset; //��ȡͼƬ����ʼλ��
    unsigned int     crc; //У������
}tRES_PARA;






#endif