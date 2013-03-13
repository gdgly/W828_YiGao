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
* �ļ�:	hyfilebios.h 
* ����:	ZhouJie 
* ����:	2005 12 05 15:52
* ˵��:	�йش��̵ĵײ��һЩ�ṹ�Ķ��壬�ο���Ƥ�飬����
*		1��Bios Parameter Block����Ϣ�Ľṹ
*		2��Ŀ¼����Ϣ�Ľṹ���ṹ�еĳ�Ա��λ����
* ��أ�������ļ�
* ƽ̨:	ARM 32λ��С�˸�ʽ
*****************************************************************************/

#ifndef _HYFILE_BIOS_H_
#define _HYFILE_BIOS_H_


/*	16 �ֽڵ�dos��������Ϣ�Ľṹ, ��MBR�ṹ��һ�������ݵĽṹ��MBR�ṹ���� 
	http://en.wikipedia.org/wiki/Partition_(computing)*/
typedef struct _PARTITION 
{
	unsigned char		active;			/* offset 0		����״̬�����0x80���ǻ����0x0  */
	unsigned char		start_head;		/* offset 1		�÷�������ʼ��ͷ  */
	unsigned char		start_cs_0;		/* offset 2		�÷�������ʼ���������  */
	unsigned char		start_cs_1;		
	unsigned char		type;			/* offset 4		�÷������͡�
														0x4	-- DOS FAT16<32M
														0x5	-- EXTEND
														0x6	-- DOS FAT16>32M
														0x7	-- NTFS(OS/2)
														0xB	-- FAT32
														83---LINUX>64M  */
	unsigned char		end_head;		/* offset 5		�÷�������ֹ��ͷ	*/
	unsigned char		end_cs_0;		/* offset 6		�÷�������ֹ���������  */
	unsigned char		end_cs_1;		
	unsigned char		start_0;		/* offset 8		�÷�����ʼ��������  */
	unsigned char		start_1;		
	unsigned char		start_2;		
	unsigned char		start_3;		
	unsigned char		size_0;			/* offset 12	�����Ĵ�С�������������� */
	unsigned char		size_1;			
	unsigned char		size_2;			
	unsigned char		size_3;			
} PARTITION, *pPARTITION;

/*	�����ṹ��Ԥ���� */
#define	PT_ACTIVE			0x80
#define	PT_NOACTIVE			0x00

#define PT_TYPE_FAT16S		0x04		/* DOS FAT16<32M */
#define PT_TYPE_EXTEND		0x05		
#define PT_TYPE_FAT16L		0x06		/* DOS FAT16>32M */
#define PT_TYPE_NTFS		0x07		
#define PT_TYPE_FAT32		0x0B
		



/*	Master Boot Record structure 
	���ڿ��ƶ��Ĵ洢���ʣ������У�Ҳ����û��.����У����߼�0������*/
#define	MBR_BOOTCODE_SIZE		0x1BE
#define	MAX_DOS_PARTITION		4
typedef struct _MBR 
{
	unsigned char	bootcode[MBR_BOOTCODE_SIZE];	/* boot sector 446 �ֽ� */
	PARTITION		partition[MAX_DOS_PARTITION];	/* 4����������Ϣ��ÿ��16�ֽ� */
	unsigned char	sig_55;							/* 0xAA55��ǩ�� */
	unsigned char	sig_aa;					
} MBR, *pMBR;



/*	BPB- BIOS Parameter Block �Ľṹ���μ���Ƥ��  */
#define BPB16_SIZE		62				/* number of bytes in a BPB for FAT16/12 */
#define BPB32_SIZE		90				/* number of bytes in a BPB for FAT32*/

/*	ע������ṹ���ڱ�����������⣬ ��Щʹ���˼���byte��ʾһ��int��short����
	Ϊ�˽���buffer�ʹӽ����ж�ȡ���㣬������ offset��0��ʼ����Ϣ
	����,�ͺ�BPB�Ķ�������Ϣ��Ӧ�ˣ�Ҳ��buffer���ͽṹ���׶�Ӧ */
typedef struct	_BPB16
{
	/*	FAT12/16 ��FAT32 ��offset 0x0 ~ offset 0x36 ��BPB�Ķ�����һ����
		BPB��Ϣ������3���ֽڵ���ת��ͣ����ֽڵ� OEM ��Ϣ */
	char			jmpBoot[3];			/* offset 0	 ��ת��*/
	char			OEMName[8];			/* OEM name & version */
	
	/* ����Ϊ��Ƥ���еģ�����Ϣ�Ŀ�ʼ��*/
	unsigned char	bytsPerSec_0;		/* offset 11	ÿ�����ֽ�����ȡֵֻ��������ֵ: 512,1024,2048,4096 */
	unsigned char	bytsPerSec_1;			
	char			secPerClus;			/* ÿ�ص�������, �Ϸ�ֵΪ1, 2, 4, 8, 16, 32, 64,128. */
										/* ��bytsPerSec��ϣ�ÿ���ֽ�����Ӧ����32k	*/
	unsigned char	rsvdSecCnt_0;		/* �ӵ�һ��������ʼ�ı���������������������㡣*/
	unsigned char	rsvdSecCnt_1;		/* ����FAT16/12��, ����ֻ��Ϊ1������FAT32��, ����ֵΪ32��*/
										
	unsigned char	numFATs;			/* offset 16	number of FATs */
	unsigned char	rootEntCnt_0;		/* ����FAT16/12, ��ʾ�ڸ�Ŀ¼����32�ֽڵ�Ŀ¼��ĸ��� */
	unsigned char	rootEntCnt_1;		/* ����ֵΪ0x0200 ������FAT32��, ������Ϊ0*/
	
	unsigned char	totSec16_0;			/* �߼��̵���������������totSec16 ��totSec32�� */
	unsigned char	totSec16_1;			/* �����������С��0x10000, ��16λ���ܴ���,�ʹ���totSec16�� 
										   ͬʱtotSec32Ϊ0*/
	unsigned char	media;				/* offset 21	media descriptor byte */
	
	unsigned char	FATSz16_0;			/* FAT12/FAT16 ��һ��FAT ռ�õ�������	FAT32 ��ֵ����Ϊ0 */
	unsigned char	FATSz16_1;			
	unsigned char	secPerTrk_0;		/* Ϊ�ж�0x13ʹ�õ�ÿ���ŵ��������� */
	unsigned char	secPerTrk_1;		
	
	unsigned char	numHeads_0;			/* Ϊ�ж�0x13ʹ�õĴ�ͷ�� */
	unsigned char	numHeads_1;		
		
	unsigned char	hiddSec_0;			/* �ڰ�����FAT��ķ���֮ǰ��������������Ŀ�� */  
	unsigned char	hiddSec_1;			/* ��������ж�0x13�ɼ��Ľ����йء� */
	unsigned char	hiddSec_2;			/* ����û�з����Ľ��ʣ������Ӧ������Ϊ0�� */
	unsigned char	hiddSec_3;			/* ����з����Ľ��ʣ���ʾBPB����֮ǰ�������� */

	unsigned char	totSec32_0;			/* offset 32	number of huge sectors (>32MGB) */
	unsigned char	totSec32_1;			/* 4 bytes */
	unsigned char	totSec32_2;			
	unsigned char	totSec32_3;		
	
	/*	���� offset 0x36��ʼ��Ϊ��չBPB��Ϣ, ΪFAT12/16 ���еģ���FAT32��ͬ */	
	char			drvNum;				/* drive number */
	char			reserved1;			/* reserved��FAT12/16 Ӧ����Ϊ0 */
	char			bootSig;			/* ��չ����ǩ��(0x29). ���Ǹ�ǩ���ֽ�ָ���������������д��������������*/
	unsigned char	volID_0;			/* volume ID number  �����к�*/
	unsigned char	volID_1;			
	unsigned char	volID_2;			
	unsigned char	volID_3;			
	char			volLab[11];			/* volume label */
	char			filSysType[8];		/* FAT16/12������Ϊ�ַ��� "FAT12   ", "FAT16   ", �� "FAT     "֮һ�� */
										/* FAT32������Ϊ�ַ��� "FAT32   " */
} BPB16, *pBPB16;



typedef struct	_BPB32
{
	char			jmpBoot[3];
	char			OEMName[8];		
	
	unsigned char	bytsPerSec_0;		
	unsigned char	bytsPerSec_1;			
	char			secPerClus;			
										
	unsigned char	rsvdSecCnt_0;		
	unsigned char	rsvdSecCnt_1;		
										
	unsigned char	numFATs;			
	unsigned char	rootEntCnt_0;		
	unsigned char	rootEntCnt_1;		
	
	unsigned char	totSec16_0;			
	unsigned char	totSec16_1;			 
										  
	unsigned char	media;				
	
	unsigned char	FATSz16_0;			
	unsigned char	FATSz16_1;			
	unsigned char	secPerTrk_0;		
	unsigned char	secPerTrk_1;		
	
	unsigned char	numHeads_0;			
	unsigned char	numHeads_1;			
	unsigned char	hiddSec_0;			
	unsigned char	hiddSec_1;			
	unsigned char	hiddSec_2;			
	unsigned char	hiddSec_3;			

	unsigned char	totSec32_0;			
	unsigned char	totSec32_1;			
	unsigned char	totSec32_2;			
	unsigned char	totSec32_3;		
	
	/*	���� offset 0x36��ʼ��Ϊ��չBPB��Ϣ
		ΪFAT32 ���еģ���FAT12/16��ͬ */	
	unsigned char	FATSz32_0;			/* big FAT size in sectors  */
	unsigned char	FATSz32_1;			
	unsigned char	FATSz32_2;			
	unsigned char	FATSz32_3;			
	unsigned char	extFlags_0;			/* extended flags */
	unsigned char	extflags_1;			
	unsigned char	FSVer_0;			/* filesystem version (0x00) low byte	*/
	unsigned char	FSVer_1;			/* filesystem version (0x00) high byte	*/
	unsigned char	rootClus_0;			/*	��Ŀ¼�ĵ�һ���صĴغţ�ͨ��Ϊ2����Ҫ��һ����2 */
	unsigned char	rootClus_1;				
	unsigned char	rootClus_2;				
	unsigned char	rootClus_3;				
	unsigned char	FSInfo_0;			/* ��FAT32 ��ı������� FSINFO �ṹ�������ţ�ͨ��Ϊ 1��*/
	unsigned char	FSInfo_1;			
	unsigned char	bkboot_0;			/* ������㣬����boot record��һ�������ھ������������š�ͨ��Ϊ6��*/
	unsigned char	bkboot_1;			
	unsigned char	reserved[12];		/* reserved, should be 0 */

	/* ���´�����Ͷ�FAT12 �� FAT16���ʵĶ�����ͬ��
		Ψһ��ͬ�Ƕ��� FAT32���ʣ�������������ƫ������ͬ��	
		ƫ����Ϊ0x64 */	
	char			drvNum;				
	char			reserved1;			
	char			bootSig;			
	unsigned char	volID_0;			
	unsigned char	volID_1;			
	unsigned char	volID_2;			
	unsigned char	volID_3;			
	char			volLab[11];			
	char			filSysType[8];		
} BPB32, *pBPB32;




/* Disk Directory�� Ŀ¼�32���ֽ� */
#define	NAME_SIZE			8			/*	8.3�Ķ��ļ��� */
#define	EXT_SIZE			3
#define	SHORT_NAME_SIZE		NAME_SIZE	/*	8.3�Ķ��ļ��� */
#define	SHORT_EXT_SIZE		EXT_SIZE

#define NAME_LOWERCASE		0x08		/*	8.3�Ķ��ļ���ȫ��Сд�����*/
#define EXT_LOWERCASE		0x10		/*	8.3�ĺ�׺��ȫ��Сд�����*/


typedef struct
{
	char			name[NAME_SIZE];	/* filename */
	char 			ext[EXT_SIZE];		/* extension */
	unsigned char 	attribute;			/* offset 11 -- file attribute */
	
	unsigned char	reserved;			/* offset 12  
										   �����ַ���8.3���ȣ����ļ����ͻ���չ��ȫ��Сдʱ��
										   ���������ļ����������ڴ�����ֻ�д�д�Ķ��ļ�����/��Ŀ¼����
										   0x08��ʾ�ļ���ȫ��Сд��0x10��ʾ��չ��ȫ��Сд,
										   ���������������ճ��ļ����ķ�ʽ���� */
	unsigned char	creattime10th;		/* create time, 10ths of a second (0-199 are valid) */
	unsigned short	creattime;			/* offset 14 -- creation time  */
	unsigned short	creatdate;			/* offset 16 -- creation date	*/
	unsigned short	lastaccessdate;		/* offset 18 -- last access date  */
	unsigned short	startcluster_h;		/* high word of first cluster,  (FAT32) */
	unsigned short	time;				/* time created or last updated */
	unsigned short	date;				/* date created or last updated */
	unsigned short	startcluster;		/* starting cluster */
	unsigned int 	file_size;			/* file size */
} DIRECTORY;


/*	Ŀ¼��������� name �ĵ�һ���ֽڵ����������Ԥ���� */
#define FN_NEVER_USED   0x00	/*	��Ŀ¼���û��ʹ�ù��������Ŀ¼�Ľ��� */
#define FN_E5			0x05	/*	���ֵĵ�һ���ַ���0xE5������0xE5�������ɾ�����ļ� */
#define FN_DIRECTORY	0x2E	/*	��ǰ�򸸵���Ŀ¼�ı��������ַ�".",�����һ���ֽ�Ҳ��0x2E, 
									���ַ���"..", ����Ŀ¼��Ĵص������Ǹ�Ŀ¼�Ĵغţ�����غ�
									Ϊ0����Ŀ¼Ϊ��Ŀ¼�� */
#define FN_ERASED		0xE5	/*	��ɾ�����ļ� */

/* Attributes */
#define AT_RDONLY		0x01	/* file is read-only */
#define AT_HIDDEN		0x02	/* file is hidden */
#define AT_SYSTEM		0x04	/* system file */
#define AT_VOLNAME		0x08	/* volume name */
#define AT_DIRECTORY	0x10	/* subdirectory */
#define AT_ARCHIVE		0x20	/* file has been written to and closed */
#define AT_VFATENTRY    0x0F    /* VFAT long filename entry */


/* Time field encoding bit masks */
#define TM_SECS			0x001F	/*	binary number of 2-second increments
									0-29, corresponding to 0-58 seconds */
#define TM_MINUTES  	0x07E0	/* binary number of minutes */
#define TM_HOURS		0xF800	/* binary number of hours */

/* Date field encoding bit masks */
#define DT_DAY			0x001F	/* day of month */
#define DT_MONTH		0x01E0	/* month */
#define DT_YEAR			0xFE00	/* year (relative to 1980) */


#endif   /* _HYFILE_BIOS_H_ */
