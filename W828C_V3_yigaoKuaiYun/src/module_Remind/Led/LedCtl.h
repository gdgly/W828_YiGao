#ifndef __LEDCTL_H__
#define __LEDCTL_H__


#define LED_CLOSE		(0x00)
#define LED_RED			(0x01)
#define LED_GREEN		(0x02)
#define LED_BLUE		(0x04)

#define LED_YELLOW		(0x03)//(LED_RED|LED_GREEN)		//��
#define LED_MAGENTA		(0x05)//(LED_RED|LED_BLUE)		//Ʒ��
#define LED_CYAN		(0x06)//(LED_GREEN|LED_BLUE)	//��ɫ
#define LED_WHITE		(0x07)//(LED_RED|LED_GREEN|LED_BLUE)
/*-------------------------------------------------------------------------
* ����: Led_init
* ˵��: ����LED����IO�ĳ�ʼ��
* ����: ��
* ����: ��
-------------------------------------------------------------------------*/
void Led_init(void);

/*-------------------------------------------------------------------------
* ����: Led_ctrl
* ˵��: LED����
* ����: color��0----7	0�رգ�7��ɫ
* ����: ��
-------------------------------------------------------------------------*/
void Led_ctrl(int color);

/*-------------------------------------------------------------------------
* ����: Led_keyPad_ctrl
* ˵��: ����LED����
* ����: enable: 0----�رգ������
* ����: ��
-------------------------------------------------------------------------*/
void Led_keyPad_ctrl(int enable);

#endif

