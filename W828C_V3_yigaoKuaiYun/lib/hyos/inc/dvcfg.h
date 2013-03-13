/*******************************************************************************
*  (C) Copyright 2005 Shanghai Hyctron Electronic Design House, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of Shanghai Hyctron Electronic Design House and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  Hyctron and is confidential in nature.  Its use and dissemination by    
*  any party other than Hyctron is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/
/******************************************************************************
* �ļ�:	device_cfg.h 
* ����:	ZhouJie 
* ����:	2005 12 05 15:52
* ˵��:	os֧�ֵ��豸�Ķ��壬����
*		1��ĳЩ�豸�ĳ�ʼ�����ݵĽṹ�Ķ���
*		2���豸���ñ�Ķ���
* ��أ�dstat.h, �豸״̬
* ƽ̨:	ARM 32λ��С�˸�ʽ
*****************************************************************************/

#ifndef _DEVICE_CFG_H_
#define _DEVICE_CFG_H_


/*	FILE SYSTEM DEVICE INITIALIZATION STRUCTURE ����DVCFG�ĳ�ʼ�����õ� */
typedef struct filesi
{
	short	max_open;		/* max number of files open at once */
	short	max_fs;			/* max number of file systems accessed at once */
} FILESI;


/*	DEVICE CONFIGURATION TABLE �豸���ñ�
	���ڲ���ϵͳ��˵���豸�Ƕ��ֵ�
	�������ļ�ϵͳ��fifo��memory���洢���ʵȵ�*/
typedef struct dvcfg
{
	char	*_name;				/* name���豸������ */
	int		(*_driver)();		/* driver entry point���豸��driver 
								   ����
								   �豸��������"dvnul", ��������Ϊ nuldrv()
								   �豸��������"dosfile", ��������Ϊ dosfile()
								   �豸��������"C", ��������Ϊ hyosVolumeDrv_Entry*/
	short	_dminor;			/* minor no. 
								   �����ͬ�豸����������һ���������ǵڼ����������豸
								   ���磬"C"��"D"��"E"��"F" �豸��������������hyosVolumeDrv_Entry
								   �����ǵ�_dminor ����Ϊ0��1��2��3 */
	union
	{
		void *pnuli;
		
		FILESI *pfilesi;
		
	} _init;					/* pointer to initialization structure 
								   ��Щ�豸��ʼ��ʱ��ҪһЩ��ʼ��������
								   �����ļ�ϵͳ����Ҫ���򿪵��ļ��������֧�ֵ��ļ�ϵͳ��*/
	
} DVCFG;

/* function prototypes	*/
DVCFG *_n2cfg(char *);


/* �ⲿȫ�ֱ�������hyos_gdata.c�ж��� */
extern DVCFG *_pdvtab;	


#endif	/* _DEVICE_CFG_H_ */
