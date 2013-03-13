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
*  �ļ�:	hyfileapi.h
*  ����:	ZhouJie 
*  ����:	2007 3 30 15:23
*  ˵��:	�ļ�ϵͳ���룬�Ͳ���ϵͳ�޹صĲ���
*****************************************************************************/

#ifndef _HY_FILE_API_H_
#define _HY_FILE_API_H_


/* Ӧ�ò���Ҫ֪���Ĵ��̷�������Ϣ*/
typedef struct _DISK_PARTITION_INFO 
{
	unsigned int		start_sector;	/* offset 0		�÷�����ʼ��������  */
	unsigned int		size;			/* offset 4		�����Ĵ�С�������������� */
} DISK_PARTITION_INFO, *pDISK_PARTITION_INFO;


/*-------------------------------------------------------------------------------------------
* ����:	hyfile_format					
* ����:	�����ļ�ϵͳ����format�洢����, pfd�����������ݱ�Ҫ�Ĳ����������Ƕ�Ӧĳ���ļ�
*		�ú������ڸ�ʽ������ʱ��dosfile����
*		��ʱ����bufferΪfatPool, fatPool���ڻ�û���ã����Կ��������������ʹ��
* ����:	volmue		--	 Ϊ"C","D","E","F", ֱ�Ӳ�������
*		secPerClus	--	 ��������cluster�Ĵ�С��sector����Ϊ512�ֽ�
*		psectorBuf	--	 ������Ϊsector��buffer��һ���ڴ棬�ɵ����߸�����4�ֽڶ���	 		
* ����:	Error code
*--------------------------------------------------------------------------------------------*/
int  hyfile_format( char * volume	, int secPerClus, char *psectorBuf);

    


#endif /* ABSTRACT_FILE_SYSTEM_H */
 
