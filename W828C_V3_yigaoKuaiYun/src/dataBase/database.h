#ifndef _MINIDATABASE_H
#define _MINIDATABASE_H

#ifdef __cplusplus
extern "C"
{
#endif
//////////////////////////////////////////////////////////

#include "hyTypes.h"

#ifdef PC_PLATFORM
	#define HYDB_READ_MODE					"rb"
	#define HYDB_WRITE_MODE					"wb"
	#define HYDB_RDWR_MODE					"rb+"

	#define HYDB_SEEK_BEG					SEEK_SET
	#define HYDB_SEEK_CUR					SEEK_CUR
	#define HYDB_SEEK_END					SEEK_END

	#define HYDB_ZERO						'\0'
	
	#define HYDB_FILE						FILE*
	
	#define HYDB_CREATE(pPath)				fopen(pPath,"wb+")
	#define HYDB_OPEN(fname,mode)			fopen(fname,mode)
	#define HYDB_CLOSE(fp)					fclose(fp)
	#define HYDB_READ(fp,buf,size,count)	fread(buf,size,count,fp)
	#define HYDB_WRITE(fp,buf,size,count)	fwrite(buf,size,count,fp)
	#define HYDB_SEEK(fp,offset,mode)		fseek(fp,offset,mode)
	#define HYDB_REWIND(fp)					rewind(fp)
	#define HYDB_DELETE(fname,mode)			remove(fname)
	#define HYDB_SIZE(fp)					fseek(fp,0,HYDB_SEEK_END);ftell(fp)
#else
#include "AbstractFileSystem.h"

	#define HYDB_READ_MODE					0
	#define HYDB_WRITE_MODE					1
	#define HYDB_RDWR_MODE					2

	#define HYDB_SEEK_BEG					0
	#define HYDB_SEEK_CUR					1
	#define HYDB_SEEK_END					2

	#define HYDB_ZERO						0

	#define HYDB_FILE						int

	#define HYDB_CREATE(pPath)				AbstractFileSystem_Create(pPath)
	#define HYDB_OPEN(fname,mode)			AbstractFileSystem_Open(fname,mode)
	#define HYDB_CLOSE(fp)					AbstractFileSystem_Close(fp)
	#define HYDB_READ(fp,buf,size,count)	AbstractFileSystem_Read(fp,buf,size*count)
	#define HYDB_WRITE(fp,buf,size,count)	AbstractFileSystem_Write(fp,buf,size*count)
	#define HYDB_SEEK(fp,offset,mode)		AbstractFileSystem_Seek(fp,offset,mode)
	#define HYDB_REWIND(fp)					AbstractFileSystem_Seek(fp,0,HYDB_SEEK_BEG)
	#define HYDB_DELETE(fname,mode)			AbstractFileSystem_DeleteFile(fname,mode)
	#define HYDB_SIZE(fp)					AbstractFileSystem_FileSize(fp)
#endif

#define HYDB_RECORD_MAX						10000	//Ŀǰ�ݶ�����¼��

#define MAKEDWORD(l, h)      ((U32)(((U16)(l)) | ((U32)((U16)(h))) << 16))

#define HYDB_KEY_MAXLEN			52	//key��󳤶�

#define HYDB_GET_KEYONLY		1	//ֻ��ȡkey��Ϣ
#define HYDB_GET_KEYVALUE		2	//��ȡkey/value��Ϣ
#define HYDB_GET_DATAONLY		3	//ֻ��ȡvalue��Ϣ


#define	HYDB_GET_CUR			4	//��ȡ��ǰkey��¼
#define HYDB_GET_ALL			5	//��ȡȫ����¼ ��ʱ�����ṩkey��Ϣ
#define	HYDB_GET_INDEX			6	//���������Ż�ȡ��¼(����������)

#define HYDB_DEL_CUR			7	//ɾ����ǰ��¼
#define HYDB_DEL_ALL			8	//ɾ�����м�¼

#define HYDB_NOTOVERWRITE		10	//�����¼ʱ��������ͬkey�ļ�¼(���ؼ�¼�Ѵ���)
#define HYDB_OVERWRITE			11	//����ԭ����¼

#define HYDB_GET_NEW			12	//��ȡ������¼
#define HYDB_GET_ALERT			13	//��ȡ���޸ĵļ�¼
#define HYDB_GET_NEWANDALERT	14	//��ȡ���������޸ĵļ�¼

#define HYDB_SUCCESS			0	//�����ɹ�
#define HYDB_FILEERROR			-1	//�������ݿ��ļ�ʧ��
#define HYDB_DBEXIST			-2	//���ݿ��ļ��Ѵ���
#define HYDB_KEYEXIST			-3	//��¼�Ѵ���
#define HYDB_MAXOVERFLOW		-4	//ϣ�����������¼��������Χ
#define HYDB_CREATEFAILED		-5	//�����ļ�ʧ��
#define HYDB_NODBFILE			-6	//���ݿⲻ����
#define HYDB_MEMORYSMALL		-7	//�ⲿ�ṩbuf̫С
#define HYDB_KEYNOTEXIST		-8	//��¼������
#define HYDB_RECORDCOVERED		-9	//��¼�Ѹ���
#define HYDB_NORECORD			-10	//û�м�¼
#define HYDB_RECORDOVERFLOW		-11	//��¼����������Χ
#define HYDB_NOTFOUND			-12	//û�ҵ���ͬ��¼
#define HYDB_MALLOCERROR		-13	//�ڴ�����ʧ��
	
////////////////////////////////////////////////////////////////////////////////////
/************************************
* ���ݿ�ͷ��Ϣ ��20�ֽ�
************************************/
typedef struct _tdbhead
{
	U32  version;			//�汾��Ϣ ��16bit���汾�� ��16bit���汾��
	U32  record_num;		//RO,��ǰ��¼��Ŀ, ��ֵΪ0
	U32  record_max;		//ϣ��֧�ֵ�����¼��   ���ܳ��� HYDB_RECORD_MAX
	U32  record_size;		//ÿ����¼�Ĵ�С(�ֽ���)
	U32  header_size;		//ͷ��Ϣ��С(�ֽ���)

}tDBHEAD;

/**********************************
* ��¼��(key/value)�Դ洢,value���ֽ������,�������ⲿִ��
* value�Ĵ�С�ڴ������ݿ�ʱ���ⲿ�ṩ
* ��С:HYDB_KEY_MAXLEN+24
**********************************/
typedef struct _tkeydbt
{
	char	data[HYDB_KEY_MAXLEN];	//key��ֵ,�г�������

	U32		Lchildoffset;			//RO,��ڵ��offset 0��ʾ����ڵ�
	U32		Rchildoffset;			//RO,�ҽڵ��offset 0��ʾ���ҽڵ�
	U32		Parentoffset;			//RO,���ڵ��offset 0��ʾ�޸��ڵ�(���ڵ�)
	U32		Recordoffset;			//RO,���key��Ӧ��value��offset
	U32		Selfoffset;				//RO,��ǰkey��offset
	U32		Keyflag;				//RO,��¼��key��Ϣ
									//   0:��Чkey,���������д 1:֮ǰ��¼ 2:�޸� 4:����

}tKEYDBT;

///////////////////////////////////////// fun //////////////////////////////////////////
/********************************
* ����:hyDbDeInit
* ����:��ձ���
* ����:none
* ����:none
*********************************/
void hyDbDeInit();

/********************************************
* ����:hyDbInit
* ����:��ʼ�����ݿ�,������ݿ��ļ�������,�򴴽����ݿ�,��ͷ��Ϣд�����ݿⲢ��ʼ��ȫ�ֱ���
       ������ݿ��ļ�����,ֱ�Ӵ����ݿ��ж�ȡ���ݳ�ʼ��ȫ�ֱ���(���Ӳ�����ȡͷ��Ϣ),����ͷ��Ϣ����
* ����:pPath:���ݿ���·��  
       pHead:����ͷ��Ϣ 
	   pCache:����ṩbuf
	   Cachesize:buf��С ע��:���뱣֤cache��Сһ�ο��Զ�ȡ����key��Ϣ
	   pValue:���value����  �ⲿ�ṩ
* ����:0:�ɹ� <0:ʧ��(����ʧ��ԭ��)
*********************************************/
int hyDbInit(char *pPath, tDBHEAD *pHead, char *pCache, U32 Cachesize, char *pValue);

/********************************************
* ����:hyDbOpen
* ����:�����ݿ� ��д��ʽ�� �ļ����ڵĻ���ʼ������
* ����:pPath:���ݿ���·��
       pHead:��������ͷ��Ϣ
	   pCache:����ṩbuf
	   Cachesize:buf��С
	   pValue:���value��Ϣ �ⲿ�ṩ
* ����:0:�ɹ� <0:ʧ��(����ʧ��ԭ��)
*********************************************/
int hyDbOpen(char *pPath, tDBHEAD *pHead, char *pCache, U32 Cachesize, char *pValue);

/********************************************
* ����:hyDbPut
* ����:���Ӽ�¼
* ����:pKey:��¼��key
	   pValue:��¼��ʵ������
	   flag:�Ƿ񸲸���ͬ���� HYDB_NOTOVERWRITE / HYDB_OVERWRITE
* ����:0:�ɹ� <0:ʧ��(����ʧ��ԭ��)
*********************************************/
int hyDbPut(tKEYDBT *pKey, char *pValue, int flag);

/********************************************
* ����:hyDbDel
* ����:ɾ����¼
* ����:pKey:Ҫɾ���ļ�¼
       flag:ɾ����� HYDB_DEL_CUR / HYDB_DEL_ALL
* ����:0:�ɹ� <0:ʧ��(����ʧ��ԭ��)
*********************************************/
int hyDbDel(tKEYDBT *pKey, int flag);

/********************************************
* ����:hyDbGet
* ����:��ȡ��¼
* ����:pKey:Ҫ��ȡ��¼��key��Ϣ ����ǻ�ȡȫ����Ϣ,�ò�����Ч
       pGetbuf:��Ż�ȡ�������� ���ⲿ�ṩ ���ݸ�ʽ���ⲿ����
	   bufsie:buf��С
	   flag:��ȡ��ʶ HYDB_GET_CUR / HYDB_GET_ALL / HYDB_GET_INDEX
	   data:��ȡ���ݵ����� HYDB_GET_KEYONLY / HYDB_GET_KEYVALUE / HYDB_GET_DATAONLY
	   index:������ flagΪHYDB_GET_INDEXʱ��Ч (0��ʼ)
* ����:��ȡ���ļ�¼����:�ɹ� <0:ʧ��(����ʧ��ԭ��)
*********************************************/
int hyDbGet(tKEYDBT *pKey, char *pGetbuf, U32 bufsie, int flag, int data, U32 index);

/********************************************
* ����:hyDbAlert
* ����:�޸ļ�¼ ���key��Ϣ�б仯 (pOldKey��pNewKey ��data��һ��),��Ҫ��������
* ����:pOldKey:֮ǰ�ؼ�����Ϣ
       pNewKey:��Ҫ�޸ĳɵĹؼ�����Ϣ
       pValue:��value
* ����:0:�ɹ� <0:ʧ��(����ʧ��ԭ��)
*********************************************/
int hyDbAlert(tKEYDBT *pOldKey, tKEYDBT *pNewKey, char *pValue);

/********************************************
* ����:hyDbGetModifyNum
* ����:��ȡ�������޸Ļ������ļ�¼��Ϣ����
* ����:flag:����ȡ������ HYDB_GET_NEW / HYDB_GET_ALERT / HYDB_GET_NEWANDALERT
* ����:����:�ɹ� <0:ʧ��(����ʧ��ԭ��)
*********************************************/
int hyDbGetModifyNum(int flag);

/********************************************
* ����:hyDbGetModify
* ����:��ȡ�������޸Ļ������ļ�¼��Ϣ
* ����:pBuf:�洢��Ϣ �ⲿ�ṩ
       bufsize:pBuf��С
       flag:����ȡ������ HYDB_GET_NEW / HYDB_GET_ALERT / HYDB_GET_NEWANDALERT
       num:��ȡ����
* ����:ʵ�ʻ�ȡ����:�ɹ� <0:ʧ��(����ʧ��ԭ��)
*********************************************/
int hyDbGetModify(char *pBuf, U32 bufsize, int flag, U32 num);

/********************************************
* ����:hyDbDestroy
* ����:�������ݿ�
* ����:pPath:���ݿ���·��
* ����:0:�ɹ� <0:ʧ��(����ʧ��ԭ��)
*********************************************/
int hyDbDestroy();

/********************************************
* ����:hyDbGetRecordNum
* ����:��ȡ��ǰ��¼��Ŀ(����ǰ����ȵ���hyDbInit)
* ����:none
* ����:��¼��Ŀ
*********************************************/
U32 hyDbGetRecordNum();

#ifdef __cplusplus
}
#endif

#endif //_MINIDATABASE_H