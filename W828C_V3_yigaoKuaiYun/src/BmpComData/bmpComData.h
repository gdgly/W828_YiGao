#ifndef __BMP_COM_DATA_H__
#define __BMP_COM_DATA_H__

extern const U8 gBmpSelect[] ;
extern const U8 gBmpUnselect[];
extern const U8 gBmpCircleData[];

typedef struct _tBMP_ATTR
{
   unsigned char *pBmp;
   unsigned short hig;
   unsigned short wid;
   //unsigned short selColor;
   //unsigned short unSelColor; 
}tBMP_ATTR;

typedef enum _eBMP_DATA_ID
{
     BMP_ID_CIRCLE,
     BMP_ID_UNREAD,
     BMP_ID_SENDED_FAIL,     
     BMP_ID_SENDED,
     BMP_ID_WASTE, 
     BMP_ID_READ,
     
     BMP_ID_DOWNARROW, //���뷨���õ������¼�ͷ
     BMP_ID_MAX    
}eBMP_DATA_ID;

//����Բ��ѡ��ͼ��
#define COLOR_CIRBMP_UNSELECT  (0x9cd3)
#define COLOR_CIRBMP_SELECT    (0xFFE0)

//δѡ��ʱ��ɫΪ0x320 (��ɫ)����ʱ���ֵ���ɫΪ��ɫ
//ѡ��ʱ����������Ϊ��ɫ����ʱ������ɫΪ��ɫ
//ѡ��ʱ�޻�������Ϊ0x339 (��ɫ)����ʱ���ֵ���ɫ��ͬ
#define  COLOR_BMP_UNSELECT   (0x320)
#define  COLOR_BMP_SELECT     (0x339)
#define  COLOR_BMP_BARSELECT  (0xFFFF)	   

/*---------------------------------------------------------------------------
* ����: bmpcom_get_attr_data
* ˵��: ��ȡ����bmpͼƬ�����ԣ����ǹ�����ͼƬ��Ҫ�������е�������
* ����: 
* ����:	
*-----------------------------------------------------------------------------*/
void bmpcom_get_attr_data(tBMP_ATTR **ppBmpAttr, eBMP_DATA_ID  id);

#endif