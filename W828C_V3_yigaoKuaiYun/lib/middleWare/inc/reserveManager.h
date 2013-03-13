#ifndef RESERVE_MANAGER_H
#define RESERVE_MANAGER_H

/*************************************************************************
 * INCLUDES
 *************************************************************************/

 /*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "vhChip.h"
#include "hyhwNandFlash.h"

#include "string.h"
#include "AbstractFileSystem.h"
#include "BoardSupportPackage.h"

/*************************************************************************
 * DEFINES
 *************************************************************************/

/********************************************************************************
��һƬ����nand �ķ���ͼ 
    +----------------------------------------------------------+
    | block0 | block1--     |block10��ʼ | ������ |���8M       |
    |        | block9       |            |       |             |
    +----------------------------------------------------------+
    |telechip| nand��U����  | reserve �� | U������| ��������    |
    | ռ��   | mapping table|            |       |�ײ���������  |
    +----------------------------------------------------------+
*********************************************************************************/

#define NAND_SIZE_UNIT						(1024*1024)	//NAND SIZE��λΪ M

#define RESERVED_AREA_USED					(0xF0F0F0F0)

#define RESEVE_START_BLOCK_R_ONLY			(1)		//reserve����ʼblock��(ֻ��reserve��)
#define RESEVE_START_BLOCK_R_FILE			(10)	//reserve����ʼblock��(��reserve����Ҳ���ļ�ϵͳ��)
#define RESEVE_EARE_START_BLOCK				HSA_GetReserveEare_StartBlockNum()

//#define IMAGE_AREA_SIZE						(20)	//��MΪ��λ
//#define PROGRAM_AREA_SIZE					(IMAGE_AREA_SIZE*2)	//��MΪ��λ ���ڴ��ϵͳ�������򣬷�Ϊ2�������ֱ����
#define IMAGE_AREA_SIZE						(10)	//��MΪ��λ
#define PROGRAM_AREA_SIZE					(IMAGE_AREA_SIZE*4)	//��MΪ��λ ���ڴ��ϵͳ�������򣬷�Ϊ2�������ֱ����

#define IDENTIFIER_STRING_MAX_LEN			(64)	//��ʶ������󳤶�

#define RESERVED_AREA_BLK_MAP_SIZE  		(8*1024)	//SDRAM_BLOCK_MAP_SIZE


/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
typedef enum
{
	reserveOnly = 0,		/*	nandֻ��reserve������û���ļ�ϵͳ��
								û��nand sw��
							*/
	reserveAndFileSys		/*	nand��reserve����Ҳ���ļ�ϵͳ��
								��reserve���Ĺ�����nand sw��֮��
							*/
	
} eNAND_USE;

typedef struct image_data_info_st
{
	int  imageLen;
	int  imageCrc;
} tIMAGE_DATA_INFO;

typedef struct app_image_data_info_st
{
	char versionStr[16];
	U32  *pRunAddr;		//appImage ���е�ַ
	int  imageMaxLen;
	int  imageLen;
	int  imageCrc;
} tAPP_IMAGE_DATA_INFO;

/*************************************************************************
 * GLOBALS
 *************************************************************************/



/*************************************************************************
* ����: HSA_SetPersistentParameter
* ˵��: ����һЩ��Ҫ�ػ�ʱ����Ĳ����������������һ�θ����е�����
* ����: destination	--	ָ������߸����Ļ��棬����������󣬻�õ���Ϣ�ڴ˻�����
*		offset		--	���persistent area��ƫ���������ֽڼƣ�����Ϊ0
*		len			--	���ȣ����ֽڼƣ����ĳ���
* ����: ������
 *************************************************************************/
int HSA_SetPersistentParameter(int type, char* source, int offset, int len);

/*************************************************************************
* ����: HSA_GetPersistentParameter
* ˵��: ��ȡһЩ��Ҫ�ػ�ʱ����Ĳ��� 
* ����: destination	--	ָ������߸����Ļ��棬����������󣬻�õ���Ϣ�ڴ˻�����
*		offset		--	���persistent area��ƫ���������ֽڼ�
*		len			--	���ȣ����ֽڼ�
* ����: ������
 *************************************************************************/
int HSA_GetPersistentParameter(int type, char *destination, int offset, int len);


int HSA_UpdateReservedArea( char* reservedAreaFile );
int HSA_UpdateReservedAreaFile( char* reservedAreaFile, unsigned offset);


/*************************************************************************
* ����: HSA_GetReserveAreaBlockMap
* ˵��: ��ô洢��Դ�ļ���reserve����blockMap Table
* ����: memory	--	�����߹�����ڴ棬����2K������blockMap����ʱbuffer
* ����: ��1����Դ�ļ���size����ȡblock��С��size��,
*		���û�� blockMap��������0	
*		���ң�ȫ�ֱ���gkBlockMapָ����ڴ��д��blockmap ��
 *************************************************************************/
int HSA_GetReserveAreaBlockMap( char * memory );

/*************************************************************************
* ����: HSA_AccessReservedArea
* ˵��: ���reserve���е���Ϣ
* ����: data	--	�����߹�����ڴ棬���ڷ��ػ�õ�����
*		offset	--	reserve��Ϣ��offset
*		size	--	Ҫ��õ����ݵ��ֽ���
* ����: ������
 *************************************************************************/
int HSA_AccessReservedArea( char * data, int offset, int size );



#endif //#ifndef RESERVE_MANAGER_H