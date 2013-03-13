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
* �ļ�:	dosfile.h 
* ����:	ZhouJie 
* ����:	2005 12 05 15:52
* ˵��:	�й��ļ�ϵͳ��һЩ�ṹ�Ķ��壬����
*		1���ļ�ϵͳ�ṹ����
*		2��Ŀ¼������Ľṹ����
*		3����ǰ����Ŀ¼������Ľṹ����
*		4���򿪵��ļ��ṹ����
* ��أ�1��driver.h	  �ļ�������FDTAB�Ľṹ����
* ƽ̨:	ARM 32λ��С�˸�ʽ
*****************************************************************************/

#ifndef _DOSFILE_H_
#define _DOSFILE_H_


/* Windows 98��95�����û�ʹ�ó��ļ���������·�����ڵ��ļ����������ɴ�255��ASCII�ַ���
	����windows ��Ƥ��涨��*/
#define	MAX_PATH_LENGTH				255		
typedef char PATH[MAX_PATH_LENGTH + 1];

/*	unicode���ַ���������Ϊasciiת���ģ��ͳ���2*/
#define MAX_LONGNAME_UNICODE_LENGTH	((MAX_PATH_LENGTH<<1)+4) 

/*	�ļ�ϵͳ�Ľṹ������ÿ��ʹ�õ��ļ�ϵͳ����Ӧһ�������Ľṹ����
	��Ȼ����������ֻ��һ���ļ�ϵͳ */
#define		FAT_POOL_SIZE	0x400		/* fat Pool ���ֽ��� */
typedef struct
{
	FDTAB			fsfdtab;			/* info for file sub-system device  */
										/*	��Ҫ��ʾ�ļ�ϵͳ�У�ʹ��ʲô���ļ� */
										/*	����ʹ��ʲô�Ĵ��̣��豸������ */
	short volatile	fsopens;			/* number of concurrent opens */
	short volatile	fsflags;			/* ������offlag�е�	F_BUSY��ǣ���ʾ�ļ�ϵͳæ */
	
	char			*fswait;			/* process waiting for FILE_SYS */
	
	/*	bpb �Ĳ�����Ϣ ������BPB��Ϣ����Щ�������ò����ģ����ԾͲ���Ҫ���������Ա��ʡ�ڴ� 	*/
	unsigned short	bpb_bytsPerSec;		/* bytes per sector */
	unsigned short	bpb_rsvdSecCnt;		/* reserved sectors, starting at 0 */
	unsigned int	bpb_FATSz;			/* ��FATSz16��FATSz32 ������number of sectors per FAT */
	char			bpb_secPerClus;		/* sectors per allocation unit */
	char			bpb_numFATs;		/* number of FATs */
	unsigned short	bpb_rootEntCnt;		/* ��������FAT16/12��number of root-directory entries */
	
	/* �洢���ʵ�ĳЩ�ؼ���Ϣ����bpb��Ϣ�е����� */
	unsigned int	fsfat_start;		/* ��һ��FAT�����ʼ��ַ�����ֽ�Ϊ��λ */
	unsigned int	fsrootDir;			/* ����FAT16/12��Ϊ��Ŀ¼������ʼ��ַ�����ֽ�Ϊ��λ */
										/* ����FAT32��Ϊ��Ŀ¼��ʼ�Ĵغ�	*/
	unsigned int	fsfile_start;		/* �ļ������ݣ�������ʼ��ַ�����ֽ�Ϊ��λ�ĵ�ַ */
	unsigned int	fscluster_size;		/* һ���ص��ֽ��� */
	unsigned int	number_of_clusters;	/* ��������cluster��Ŀ�����������洢���ݵĴ��� */
	unsigned int volatile free_cluster;	/* lst. free cluster */
	
	int				fatType;			/* FAT type�� 0-FAT32��1-FAT16��2-FAT12 */
	


	/* ����ΪFAT�����صĻ���*/									
	char    	fatPool[FAT_POOL_SIZE];	/*	�����й�д����ʱ��pfatcache_wr ָ��fatPool��
											��ʱ��fatPool�����ݴ洢������pfatcache_wr�涨�� 
											��û�н��й�д����ʱ��fatPool����Ǵ����ϵ�fat��
											�ľֲ������� */
	
	unsigned int	*pfatcache_wr;		/*	ע��FAT���cache�д����һ���ļ���FAT���һ��һ�Ե����ݣ�
											����ĳ���ļ�ռ�ô�3��5��6
											����cache�д�ľ���(3��5)��(5��6)��(6��0x0fffffff)  */
											
	unsigned		fatsnapstart;		/*	��û�н��й�д��������fat�������ŵ���fat���ݵľֲ���
											���ȫ�ֱ�����ʾfat���ֲ��Ŀ�ʼ */
 
	unsigned		fat_change_start;
	unsigned		fat_change_end;		/* 	���ļ�ϵͳ������fat��ʱ���ڹر��ļ�ϵͳʱ���轫��һ��fat�������浽�ڶ���fat��
											������ȫ�ֵı�����ʾ��Ҫ��ľֲ�fat�Ŀ�ʼ�ͽ���*/ 
	
	/* 20061016 ���Ҫ֧��˫FAT��Ĳ���������Ҫ�������µ��򣬵������ڻ�û��֧�� */
	//int  fat_2_is_present;			/*�ڶ���FAT��ı�־*/
 	//unsigned long  fsfat2_start;		/* start of second FAT adrress*/
} FILE_SYS;

/*	FILE_SYS�ṹ��fatType��ȡֵ */
#define	FATTYPE_FAT32		0	
#define	FATTYPE_FAT16		1	
#define	FATTYPE_FAT12		2	
#define	FATTYPE_NUM			3	


/*	Ŀ¼����� */
typedef struct
{
	unsigned long long disk_addr;		/* Ŀ¼��ĵ�ַ���ǴӾ���0��ַ��ʼ����ĵ�ַ,���Ϊ0����ʾ��Ŀ¼ */
											
	FILE_SYS		*pfs;				/* ��ʹ�õ��ļ�ϵͳ */
	DIRECTORY		dir;				/* Ŀ¼��Ŀ�������32���ֽڵ�copy */
	unsigned 		eof_cluster;		/* end of file cluster */
} FSDIR;

/*	��ǰ����Ŀ¼����� Current Working Directory Structure��
	����ÿ��������һ������ṹ���� 
	ע������ṹ��ǰ3��������FSDIR�ṹ��һ����*/
typedef struct
{
	unsigned		disk_addr;			/* Ŀ¼��ĵ�ַ���ǴӾ���0��ַ��ʼ����ĵ�ַ */
	FILE_SYS		*pfs;				/* ��ʹ�õ��ļ�ϵͳ */
	DIRECTORY		dir;				/* Ŀ¼��Ŀ�������32���ֽڵ�copy */
	PATH			path;				/* pathname of directory */
	void			(*pexit)();			/* onexit linkage */
} CWD;

/*  �򿪵��ļ��ṹ�壬Open File Structure, 
	����ÿ���򿪵��ļ�,��һ��������� */
typedef struct
{
	FSDIR			ofsdir;				/* �ļ���Ŀ¼����Ϣ */
	PATH			ofpath;				/* complete pathname */
	short			ofopens;			/* ���˼��� */
	short			offlags;			/* ��ǣ����庬�������Ķ��壬��Ҫ���Ŀ¼�����ݻ��ļ������Ƿ�
										   ������ */
	unsigned 		ofcloffset;			/* ��ǰ���ڶ�д�� cluster offset���Ӹ��ļ��ĵ�һ��cluster���� 
											��ʾ�õ�����cluster����ʾ�����ļ��е���Զ�дλ��*/
	unsigned 		ofclustno;			/* ��ǰ���ڶ�д�ľ��� cluster no. ��ʾ��ǰ���ڴ����ϵĶ�дλ�� */
} OFILE;


/*  OFILE �ṹ���� offlags bits �Ķ��� */
#define F_MOD		1				/* directory was modified */
#define F_BUSY		2				/* some process has exclusive use */
#define F_ERROR		4				/* I/O error occurred */
#define F_FMOD		8				/* file was modified */


#if 0  // ����Ϊ������Ĳο� 20061016
//�ڶ����ļ���ʱ��,����FAT32/16/12; ????
//PC�Ĳ����ļ��Ķ���
#ifndef _FILE_DEFINED
struct _iobuf {
        char *_ptr;
        int   _cnt;
        char *_base;
        int   _flag;
        int   _file;
        int   _charbuf;
        int   _bufsiz;
        char *_tmpfname;
        };
typedef struct _iobuf FILE;
#endif
#endif

/* windows ��Ƥ��涨������� Cluster ֵ*/
#define EOF_CLUSTER		0x0FFFFFFF
#define FREE_CLUSTER	0x00000000
#define BAD_CLUSTER		0x0FFFFFF7
#define IS_EOF(c)		((unsigned)(c) >= 0x0FFFFFF8)
#define IS_FREE(c)		((unsigned)(c) == FREE_CLUSTER)
#define IS_BAD(c)		((unsigned)(c) >= 0xFFF0 && !IS_EOF(c))


/*	function prototypes */
int		_chkfs(char **ppath);
//int		 _closfile(FILE_SYS *pfs);
short	_dosdate(void);
short	_dostime(void);
OFILE	*hyfile_findOpenfileHandler(FILE_SYS *pfs);
//void	_lockfs(FILE_SYS *pfs);
//void	_ulockfs(FILE_SYS *pfs);
//void	_unopen(FILE_SYS *pfs);


/* �ⲿ����  */
extern CWD *_cwd;		/* process current working directory */



#endif /*_DOSFILH */
