#ifndef ABSTRACT_FILE_SYSTEM_H
#define ABSTRACT_FILE_SYSTEM_H

/*************************************************************************
 * INCLUDES
 *************************************************************************/

/*************************************************************************
 * DEFINES
 *************************************************************************/

#define AFS_READ    0
#define AFS_WRITE   1
#define AFS_RDWR    2

#define AFS_SEEK_OFFSET_FROM_START      0
#define AFS_SEEK_OFFSET_FROM_CURRENT    1
#define AFS_SEEK_OFFSET_FROM_END        2

#define AFS_NAME_SIZE   8
#define AFS_EXT_SIZE    3

#define AFS_DIRECTORY_ENTRY_NEVER_USED  0x00
#define AFS_DIRECTORY_ENTRY_ERASED      0xE5
#define AFS_DIRECTORY_ENTRY_DIRECTORY   0x2E

#define AFS_ATTRIBUTE_LONG_FILE_ENTRY   0x0F
#define AFS_ATTRIBUTE_DIRECTORY         0x10
#define AFS_ATTRIBUTE_SYSTEM            0x02
#define AFS_ATTRIBUTE_HIDDEN            0x04
#define AFS_ATTRIBUTE_VOLUME            0x08

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
#define	NAME_SIZE	8			/*	8.3�Ķ��ļ��� */
#define	EXT_SIZE	3

#define NAND_BLOOK_POOL_MANAGER_SIZE		(256)//(128)

typedef struct
{
	char			name[NAME_SIZE];	/* filename */
	char 			ext[EXT_SIZE];		/* extension */
	unsigned char 	attribute;			/* offset 11 -- file attribute */
	
	unsigned char	reserved;			/* offset 12 -- ֻ�ж��ļ�ʱ 0x08��ʾ�ļ���Сд��0x10��ʾ��չ��Сд */
	unsigned char	creattime10th;		/* create time, 10ths of a second (0-199 are valid) */
	unsigned short	creattime;			/* offset 14 -- creation time  */
	unsigned short	creatdate;			/* offset 16 -- creation date	*/
	unsigned short	lastaccessdate;		/* offset 18 -- last access date  */
	unsigned short	startcluster_h;		/* high word of first cluster,  (FAT32) */
	unsigned short	time;				/* time created or last updated */
	unsigned short	date;				/* date created or last updated */
	unsigned short	startcluster;		/* starting cluster */
	unsigned int 	file_size;			/* file size */
} DIRECTORY,AFS_DirectoryEntry_st;


typedef enum 
{
    AFS_SINGLE_DELETE_FDM,
    AFS_RECURSIVE_DIRECTORIES_FDM    

} AFS_FileDeleteMode_en;

typedef void (* ProtectUnprotectFn) (void);

typedef struct
{
    ProtectUnprotectFn ProtectFunction;
    ProtectUnprotectFn UnprotectFunction;
} AFS_ProtectUnprotectFunctions_st;
    
extern Int8 *pNandBlockPoolManager;
/*************************************************************************
 * FUNCTION DECLARATIONS
 *************************************************************************/

/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_IsLock
* ����:	�ж��ļ�ϵͳ�Ƿ�����(�Ƿ�����ʹ��)
* ����:	none
* ����:	0 --- �ļ�ϵͳδlock
 		1 --- �ļ�ϵͳ�ѱ�lock
*----------------------------------------------------------------------------*/
int AbstractFileSystem_IsLock(void);

/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_Init
* ����:	AbstractFileSystem��ʼ����׼��buffer
*			spReadBuffer----AFS�����棬����1k�����ͷ�
*			openfileMode----AFS��дģʽ��¼�����ڹر�nandд��������90bytes�����ͷ�
*			ע��spReadBuffer��openfileMode�ռ�������ǲ��ͷŵģ�
*				���Ըú��������ڳ������������ڼ�ֻ�ܵ���һ��
* ����:	none
* ����:	HY_OK------��ʼ���ɹ�
*		HY_ERROR---��ʼ��ʧ��
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Init( void );

/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_Format
* ����:	��ʽ��ָ���Ĵ���
* ����:	volume---�̷�(�磺"C","D","E","F")
* ����:	HY_OK------��ʽ���ɹ�
*		HY_ERROR---��ʽ��ʧ��
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Format( char *volume );
/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_ValidateBootSector
* ����:	�����̵�BPB��Ϣ�Ƿ���ȷ
* ����:	volume		-- �̷�
* ����:	HY_OK				-- no error
*		HY_ERROR			-- �̷������ڻ��ܶ���������������ٽ��и�ʽ��
*		HY_ERR_BAD_FORMAT	-- BPB��������ȷ��ֻ������������ſ��Խ�һ����ʽ��
*----------------------------------------------------------------------------*/
int AbstractFileSystem_ValidateBootSector( char * volume );

/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_Create
* ����:	�����ļ��������ɹ����ļ��Ѿ��Զ�д��ʽ��
*		���������ַ�����ʽΪ�ļ��У��򴴽��ļ��У��ҷ��ش���
* ����:	fname-----�ļ���
* ����:	�ɹ����ļ�handle
*		ʧ�ܣ�HY_ERROR
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Create( char *fname );

/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_CreateBySize
* ����:	�����ļ�
* ����:	fname-----�ļ���
*		mode------������ʽ��
*					6----����ָ����size������������cluster
*		needsize--ϣ��������ļ���С
* ����:	�ļ�handle
*----------------------------------------------------------------------------*/
int AbstractFileSystem_CreateBySize( char *fname, int mode, unsigned int needsize );

/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_Truncate
* ����:	�������size�ض��ļ�
* ����:	fhandle---�ļ�hanle
*		size------��ȡ���ļ��Ĵ�С
* ����:	�ļ�handle
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Truncate( int fhandle, unsigned int size );

/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_Open
* ����:	��ָ���ķ�ʽ���ļ�
* ����:	fname---�ļ���
*		mode----�򿪷�ʽ(�磺AFS_READ��AFS_WRITE��AFS_RDWR)
* ����:	�ļ�handle(>=0������ȷ��<0���򿪴���)
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Open( char *fname, int mode );


/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_Close
* ����:	�ر��Ѿ��򿪵��ļ�
* ����:	fd---�ļ�handle
* ����:	0----�ر���ȷ
*		-1---�رմ���
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Close( int fd );

/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_Read
* ����:	��ȡ�ļ�����
* ����:	fd-------�ļ�handle
*		buffer---��ȡ�����ڴ���
*		size-----��ȡ��С�����ֽڼ�
* ����:	ʵ�ʶ�ȡ��size
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Read( int fd, char *buffer, int size );

/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_Write
* ����:	д�ļ�
* ����:	fd-------�ļ�handle
*		buffer---�����ڴ������д���ļ�
*		size-----д���С�����ֽڼ�
* ����:	ʵ��д���size
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Write( int fd, char * buffer, int size );

/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_Seek
* ����:	�ļ���дλ�ö�λ
* ����:	fd-------�ļ�handle
*		offset---ƫ��
*		sense----��λ��ʽ
*			AFS_SEEK_OFFSET_FROM_START-------���ļ���ʼ��
*			AFS_SEEK_OFFSET_FROM_CURRENT-----�ӵ�ǰλ�ü�
*			AFS_SEEK_OFFSET_FROM_END---------���ļ�ĩβ��
* ����:	ʵ�ʶ�λ��λ��(����ļ���ʼ)
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Seek( int fd, int offset, int sense );

/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_DeleteFile
* ����:	ɾ���ļ�
* ����:	fname----�ļ���
*		mode-----ɾ����ʽ
*			AFS_SINGLE_DELETE_FDM----ɾ�������ļ�
* ����:	0-�����ɹ�
*		-1 ����ʧ��
*----------------------------------------------------------------------------*/
int AbstractFileSystem_DeleteFile( char *fname, AFS_FileDeleteMode_en mode );

/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_Rename
* ����:	�ļ�������
* ����:	fname----�������ļ���
*		newName--���ļ���
* ����:	0-�����ɹ�
*		-1 ����ʧ��
*----------------------------------------------------------------------------*/
int AbstractFileSystem_Rename( char *fname, char *newName );

/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_MakeDir
* ����:	����Ŀ¼
* ����:	dname----Ŀ¼��(�磺"C:/����"��"C:/����/")
* ����:	0-�����ɹ�
*		-1 ����ʧ��
*----------------------------------------------------------------------------*/
int AbstractFileSystem_MakeDir( char *dname );

/*-----------------------------------------------------------------------------
* ����:	AbstractFileSystem_DeleteDir
* ����:	ɾ��Ŀ¼
* ����:	dname----Ŀ¼��(�磺"C:/����"��"C:/����/")
* ����:	0-�����ɹ�
*		-1 ����ʧ��
*----------------------------------------------------------------------------*/
int AbstractFileSystem_DeleteDir( char * dname );


/*---------------------------------------------------------
* ����: AbstractFileSystem_VolumeSpace
* ����: ���ָ���̷��Ŀռ�״��(��������ʣ������)
* ����: volume---�̷�ָ��(�磺"C","D","E","F")
*		������pTotalSpace��pFreeSpace����
*		ע�⣺�������Ϊ64λ���(�����Ժ����������4G���)
* ����:	HY_OK	-- no error
*		HY_ERROR-- �̷������ڻ��������
*---------------------------------------------------------*/
int AbstractFileSystem_VolumeSpace( char *volume, U64 *pTotalSpace, U64 *pFreeSpace);

/*---------------------------------------------------------
* ����: AbstractFileSystem_FileSize
* ����: ����Ѵ��ļ��Ĵ�С
* ����: fd------�ļ�handle
* ����:	fileSize----�ļ���С
*---------------------------------------------------------*/
int AbstractFileSystem_FileSize(int fd);

/*---------------------------------------------------------
* ����: AbstractFileSystem_VolumePresent
* ����: ���ָ�����̷��Ƿ����
* ����: volume---�̷�ָ��(�磺"C","D","E","F")
* ����: HY_OK------�̷�����
*		HY_ERROR---�̷�������
*---------------------------------------------------------*/
int AbstractFileSystem_VolumePresent(char* volume);

/*---------------------------------------------------------
* ����: AbstractFileSystem_VolumeFormatted
* ����: ���ָ�����̷��Ĵ��̸�ʽ
* ����: volume---�̷�ָ��(�磺"C","D","E","F")
* ����:	HY_OK				-- no error
*		HY_ERROR			-- �̷������ڻ��ܶ���������������ٽ��и�ʽ��
*		HY_ERR_BAD_FORMAT	-- BPB��������ȷ��ֻ������������ſ��Խ�һ����ʽ��
*---------------------------------------------------------*/
int AbstractFileSystem_VolumeFormatted(char* volume);

/*---------------------------------------------------------
* ����: AbstractFileSystem_VolumeMaintenance
* ����: ά��ָ�����̷�
* ����: volume---�̷�ָ��(�磺"C","D","E","F")
* ����:	HY_OK	-- no error
*		HY_ERROR-- �̷������ڻ��ܶ�
*---------------------------------------------------------*/
int AbstractFileSystem_VolumeMaintenance(char* volume);

/*---------------------------------------------------------
* ����: AbstractFileSystem_NameFilter
* ����: ����ļ������﷨�Ƿ��д��Ƿ��в�������ַ���
* 		����ļ������Ϸ�����path������������ļ���
* ����: path -- ָ��·���ַ��������ַ�������Ҫ����ڴ����ɸ�
* ����: HY_OK------ �ļ���ok
* 		HY_ERROR--- �ļ����д�����path�Ѿ���������
*---------------------------------------------------------*/
int AbstractFileSystem_NameFilter(char *path);

#endif /* ABSTRACT_FILE_SYSTEM_H */
 
