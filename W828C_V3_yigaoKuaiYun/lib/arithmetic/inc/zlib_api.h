#ifndef __ZLIB_FLASH_API_H__
#define __ZLIB_FLASH_API_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hyTypes.h"
#include "AbstractFileSystem.h"

//#ifndef NO_DUMMY_DECL
//struct internal_state      {int dummy;}; /* for buggy compilers */
//#endif

#define CHUNK 10*1024

typedef unsigned char uchar; 

#define PLATFORM_S2100

#ifndef PLATFORM_S2100

#define STREAM_READ "rb"
#define STREAM_WRITE "wb"
#define STREAM_RDWR
#define STREAM_SEEK_BEGIN SEEK_SET
#define STREAM_SEEK_CURENT SEEK_CUR
#define STREAM_SEEK_END SEEK_END

#define OPENFILE(FileName,Mode) fopen(FileName,Mode)
#define CLOSEFILE(pFile) fclose(pFile)
#define READFILE(pBuf,Size,Len,pFile) fread(pBuf,Size,Len,pFile)
#define WRITEFILE(pBuf,Size,Len,pFile) fwrite(pBuf,Size,Len,pFile)
#define SEEKFILE(pFile,Offset,Mode) fseek(pFile,Offset,Mode)
#define FILEPOS(pFile) ftell(pFile)

#else

//#ifndef STREAM_READ
#if 1

#define STREAM_READ AFS_READ	//HSA_READ_FOM
#define STREAM_WRITE AFS_WRITE	//HSA_WRITE_FOM
#define STREAM_RDWR AFS_RDWR	//HSA_RDWR_FOM
#define STREAM_SEEK_BEGIN AFS_SEEK_OFFSET_FROM_START
#define STREAM_SEEK_CURENT AFS_SEEK_OFFSET_FROM_CURRENT
#define STREAM_SEEK_END AFS_SEEK_OFFSET_FROM_END

#define CREATEFILE(FileName) AbstractFileSystem_Create((char *)(FileName))
#define OPENFILE(FileName,Mode) AbstractFileSystem_Open((char *)(FileName),Mode)
#define CLOSEFILE(pFile) AbstractFileSystem_Close(pFile)
#define READFILE(pBuf,Size,Len,pFile) AbstractFileSystem_Read(pFile,(char *)(pBuf),(Size)*(Len))
#define WRITEFILE(pBuf,Size,Len,pFile) AbstractFileSystem_Write(pFile,(char *)(pBuf),(Size)*(Len))
#define SEEKFILE(pFile,Offset,Mode) AbstractFileSystem_Seek(pFile,Offset,Mode)
#define FILEPOS(pFile) AbstractFileSystem_FileSize(pFile)
//#define CREATEFILE(FileName)  AbstractFileSystem_Create(FileName)

#endif

#endif

//����Ϊ�����ýӿڣ�

/***********************************************************************
 *
 * ��������		Zlib_Uncompress_Data
 *
 * �������ܣ�	ʹ��deflate�㷨��ѹһ�����ݿ�
 * 
 * @Param: 		'srcbuf'��ָ��һ��Ҫ��ѹ�����ݿ�
 * @Param: 		'srcLen'����ʾ'srcbuf'�ĳ���
 * @Param: 		'destbuf'��ָ��һ���Ž�ѹ�����ݵ��ڴ��
 * @Param: 		'destLen'������'destbuf'�ĳ���
 * @Param: 		'bhead'����ʾҪ��ѹ��������ݿ��ǲ��Ǳ��ļ��ĵ�һ������
 *
 * ����ֵ��'0'����ʾ�ɹ���С��0��ʾ�д���
 ***********************************************************************/
int Zlib_Uncompress_Data1(const uchar *srcbuf, int srcLen, uchar **destbuf, int *destLen, int bhead);


/*
src:����������
srcLen:���������ݳ���
dest:���ս�������ݵ�buf
destLen:��������ݳ���
bhead:�Ƿ���ѹ��ͷ�Ŀ�  һ����1

����:>=0:�ɹ�  
*/
int Gzip_Uncompress(const unsigned char *src, int srcLen, unsigned char *dest, int *destLen, int bhead);

/*
dest:ѹ�����ļ����buf
destLen(in):dest ����    (out):ѹ�������ݵĴ�С
source:��ѹ���ļ�
sourceLen:��ѹ���ļ���С
pName:ԭʼ�ļ���,���ΪNULL,��ʾ����Ҫ����Դ�ļ���

����:>=0:�ɹ�  
ע:�ú���Ŀǰû���ṩ����ԭʼ�ļ�������ʱ�����Ϣ
*/
int Gzip_Compress(unsigned char *dest, unsigned int *destLen, unsigned char *source, int sourceLen, char *pName);


/***********************************************************************
 *
 * ��������		Gz_Uncompress_Data
 *
 * �������ܣ�	ʹ��gzip�㷨��ѹһ������
 * 
 * @Param: 		'srcbuf'��ָ��һ��Ҫ��ѹ�����ݿ�
 * @Param: 		'srcLen'����ʾ'srcbuf'�ĳ���
 * @Param: 		'destbuf'��ָ��һ���Ž�ѹ�����ݵ��ڴ��
 * @Param: 		'destLen'������'destbuf'�ĳ���
 * @Param: 		'bhead'����ʾҪ��ѹ��������ݿ��ǲ��Ǳ��ļ��ĵ�һ������
 *
 * ����ֵ��		'0'����ʾ�ɹ���С��0��ʾ�д���
 **********************************************************************/
int Gz_Uncompress_Data(const uchar *srcbuf, int srcLen, uchar **destbuf, int *destLen, int bhead);



/***********************************************************************
 * 
 * ��������		Gz_Free
 * 
 * �������ܣ�	��ӦZlib_Uncompress_Data1��Gz_Uncompress_Data���ͷŽ�ѹ��
 *              ��ռ�õ���Դ
 *
 * @Param:		'memblock'��ָ��Ҫ�ͷŵ��ڴ��ĵ�ַ
 *
 * ����ֵ��	
 **********************************************************************/
void Gz_Free(void *memblock);


/***********************************************************************
 * 
 * ��������		Zlib_Uncompress_Data
 * 
 * �������ܣ�	��ѹһ��swf���ݿ�
 *
 * @Param:		'pData'��ָ��һ��Ҫ��ѹ��flash���ݿ�
 * @Param:		'pTempBuf'�����ڽ�ѹ������ʱ������ݵ��ڴ��
 * @Param:		'TempLen'����ʱ���ݿ�ĳ���
 * @Param:		'pBuf'��ָ��һ�����ڴ�Ž�ѹ���������ڴ��
 * @Param:		'BufLen'����ѹ�����ݿ�ĳ���
 * @Param:		'pureFlag'����ʾswfѡ���Ƿ��
 *
 * ����ֵ��	    0: SUCCESS; -1: decode error; -2: open_file error.
 **********************************************************************/
int Zlib_Uncompress_Data(U8 *pData,U8 *pTempBuf, U32 TempLen,U8 *pBuf,U32 BufLen, U8 pureFlag);


/***********************************************************************
 * 
 * ��������		Zlib_Uncompress_File
 * 
 * �������ܣ�	��ѹswf�ļ�
 *
 * @Param:		'pSrcFileName'��Դ�ļ���
 * @Param:		'pDestFileName'��Ŀ���ļ���
 * @Param:		'pBuf'����ѹ������ʹ�õ���ʱ�ڴ��
 * @Param:		'BufLen'����ʱ�ڴ�鳤��
 *
 * ����ֵ��	
 **********************************************************************/
S32 Zlib_Uncompress_File(U8 *pSrcFileName,U8 *pDestFileName,U8 *pBuf,U32 BufLen);

#ifdef __cplusplus
}
#endif

#endif //__ZLIB_FLASH_API_H__


