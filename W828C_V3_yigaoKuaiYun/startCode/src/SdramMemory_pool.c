/*******************************************************************************
*  (C) Copyright 2010 hyco, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of hyco and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  hyco and is confidential in nature.  Its use and dissemination by    
*  any party other than hyco is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/

/**
 * @file memory_pool.c
 * @brief Reusable memory pool.
 *
 * 
 * Assumptions:
 * - None
 *
 * Functional limitations:                     
 * - None
 */
/**
 * \addtogroup tmAppDemolication
 */
/*@{*/

/*------------------------------------------------------------------------------
Standard include files:
------------------------------------------------------------------------------*/

#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "memory_pool.h"
#include "SdramMemory_pool.h"
/*------------------------------------------------------------------------------
Local Types and defines:
------------------------------------------------------------------------------*/

/*	Shark Sdram Memory ����

shark �ⲿ�� 2M SDRAM �ķ���ʹ�ñ� ����Ӳ����ͬ��������
=================================================================================	
	ֻ��2M SDRAM ��ʱֻ������220*176����ʾ��С
	0x20000000�ĵ�ַ��cachable��bufferable��
	0x28000000�ĵ�ַ��noncachable��nonbufferable��
	+----------------------+------+----------------------+
	|0x20000000~0x200FFFFF |  1M  | ����ǰ1M������       |  **
	+----------------------------------------------------+
	|0x20100000~0x201007FF |  2K  | nandflash block map  |  **
	+----------------------------------------------------+
	|0x20100800~0x20101FFF |  6K  | ��Ҫ���������buffer |  **
	+----------------------------------------------------+
	|0x20102000~0x20114BFF |  76K | �Դ�                 |  **
	+----------------------------------------------------+
	|0x20114C00~0x201277FF |  76K | ����ͼ               |  **
	+----------------------------------------------------+
	|0x20127800~0x2013A3FF |  76K | ��ʾ������           |  **
	+----------------------------------------------------+
	|0x2013A400~0x20185BFF | 302K | ��Ƶ����buffer       |  **
	+----------------------------------------------------+
	
	ע:
		1.0x20185C00֮����������ֲ���ʱ��������(��:������,ͼƬ��);
		2.��Ƶ(MP4/AVI/SWF)����ʱ,�ɴ�0x20114C00֮��ʼʹ��.

shark �ⲿ�� 8M SDRAM �ķ���ʹ�ñ� ����Ӳ����ͬ��������
=================================================================================	
	0x20000000�ĵ�ַ��cachable��bufferable��
	0x28000000�ĵ�ַ��noncachable��nonbufferable��
	+----------------------+------+----------------------+
	|0x20000000~0x200FFFFF |  1M  | ����ǰ1M������       |  **
	+----------------------------------------------------+
	|0x20100000~0x201007FF |  2K  | nandflash block map  |  **
	+----------------------------------------------------+
	|0x20100800~0x20101FFF |  6K  | ��Ҫ���������buffer |  **
	+----------------------------------------------------+
	|0x20100800~0x2010BFFF | 40K  | Ӧ�ò�����ļ�buffer |  **
	+----------------------------------------------------+
	|0x2010C000~0x2010C3FF |  1K  | SD����ʼ��buffer     |  **
	+----------------------------------------------------+
	|0x2010C400~0x2017CBFF |150K*3| �Դ�+����ͼ+��ʾ������,��Ӧ�ò㻮��  |  **
	+----------------------------------------------------+
	|0x2017CC00~0x201C83FF | 302K | 10K+40K+252K=302K    |  **
	+----------------------------------------------------+
	|��code������Ҫռ��2+6+40+1+150*3+302k=801k        |
	+----------------------------------------------------+
	2007-11-1 05:42���� �ߣ�����˵��

shark �ⲿ�� 16M SDRAM �ķ���ʹ�ñ� ����Ӳ����ͬ��������
=================================================================================	
	0x20000000�ĵ�ַ��cachable��bufferable��
	0x28000000�ĵ�ַ��noncachable��nonbufferable��
	+----------------------+------+----------------------+
	|0x20000000~0x201FFFFF |  1M  | ����ǰ1M������       |  **
	+----------------------------------------------------+
	|0x20200000~0x202007FF |  2K  | nandflash block map  |  **
	+----------------------------------------------------+
	|0x20200800~0x20201FFF |  6K  | ��Ҫ���������buffer |  **
	+----------------------------------------------------+
	|0x20200800~0x2020BFFF | 40K  | Ӧ�ò�����ļ�buffer |  **
	+----------------------------------------------------+
	|0x2020C000~0x2020C3FF |  1K  | SD����ʼ��buffer     |  **
	+----------------------------------------------------+
	|0x2020C400~0x2027CBFF |150K*3| �Դ�+����ͼ+��ʾ������,��Ӧ�ò㻮��  |  **
	+----------------------------------------------------+
	|0x2027CC00~0x202C83FF | 302K | 10K+40K+252K=302K    |  **
	+----------------------------------------------------+
	|��code������Ҫռ��2+6+40+1+150*3+302k=801k        |
	+----------------------------------------------------+
	2007-11-1 05:42���� �ߣ�����˵��
*/


#define NON_CACHEABLE_MASK		(~0x04000000)
/*------------------------------------------------------------------------------
Global variables:
------------------------------------------------------------------------------*/

#pragma arm section zidata = "sdram_memory_pool"
	char gSdramSysMemoryPool[SDRAM_SYS_MEMORY_POOL_SIZE];
#pragma arm section

#pragma arm section zidata = "sdram_video_sfc_memory_pool"
	char gSdramVideoSfcMemoryPool[SDRAM_VIDEO_SFC_MEMORY_POOL_SIZE];
#pragma arm section

//#pragma arm section zidata = "file_manager_memory_pool"
//	char gSdramFileMangerMemoryPool[SDRAM_FILE_MANAGER_MEM_POOL_SIZE];
//#pragma arm section

//#pragma arm section zidata = "sdram_recinfo_memory_pool"
//	char gSdramRecInfoMemoryPool[SDRAM_REC_INFO_POOL_SIZE];
//#pragma arm section

#pragma arm section zidata = "sdram_pic_decode_memory_pool"
	char gSdramPicDecodeMemoryPool[SDRAM_PIC_DECODE_MEMORY_POOL_SIZE];
#pragma arm section

//#pragma arm section zidata = "sdram_res_head_memory_pool"
//	char gSdramResHeadMemoryPool[SDRAM_RES_HEAD_MEMORY_POOL_SIZE];
//#pragma arm section

//#pragma arm section zidata = "config_file_memory_pool"
//	char gSdramCnfgFileMemoryPool[SDRAM_CONFIG_FILE_MEMORY_POOL_SIZE];
//#pragma arm section

//#pragma arm section zidata = "ie_memory_pool"
//	char gSdramIeMemoryPool[SDRAM_IE_MEMORY_POOL_SIZE];
//#pragma arm section

/*------------------------------------------------------------------------------
Exported functions:
------------------------------------------------------------------------------*/
//С��¼�洢ʹ��
#define SMALL_REC_BUF_OFFSET	0
#define SMALL_REC_BUF_SIZE		(140*1024)//Ԥ��128k�������������ݰ�����

//���¼�洢ʹ��
#define LARGE_REC_BUF_OFFSET	(SMALL_REC_BUF_OFFSET+SMALL_REC_BUF_SIZE)
#define LARGE_REC_BUF_SIZE		(64*1024)

//һƱ���ʹ��
#define MULTI_CODE_BUF_OFFSET	(LARGE_REC_BUF_OFFSET+LARGE_REC_BUF_SIZE)
#define MULTI_CODE_BUF_SIZE		(256*1024)

//С��¼��ŵĶ���
#define REC_QUEUE_BUF_OFFSET	(MULTI_CODE_BUF_OFFSET+MULTI_CODE_BUF_SIZE)
#define REC_QUEUE_BUF_SIZE		(512*1024)

//���ͼ�¼ʱʹ�õĲ���
#define REC_SEND_BUF_OFFSET		(REC_QUEUE_BUF_OFFSET+REC_QUEUE_BUF_SIZE)
#define REC_SEND_BUF_SIZE		(100*1024)

//�洢�����¼   (�ô�С�����˿��Դ����������  Ŀǰ�����50��)
#define REC_GROUP_BUF_OFFSET	(REC_SEND_BUF_OFFSET+REC_SEND_BUF_SIZE)
#define REC_GROUP_BUF_SIZE		(26*1024)

//����Ա�buf
#define BARCOMPARE_BUF_OFFSET	(REC_GROUP_BUF_OFFSET+REC_GROUP_BUF_SIZE)
#define BARCOMPARE_SIZE			(64*1024)

//���ݱ����
#define DECODE_BUF_OFFSET		(BARCOMPARE_BUF_OFFSET+BARCOMPARE_SIZE)
#define DECODE_BUF_SIZE			(200*1024)

//�û�����(��¼ʱ���ص�¼��Ա������Ϣ)
#define USERGROUP_BUF_OFFSET	(DECODE_BUF_OFFSET+DECODE_BUF_SIZE)
#define USERGROUP_BUF_SIZE		(2048)

/*--------���Ϲ�ռ�� 1197056 bytes   1.1416015625 M------------*/
//GPSʹ��
#define GPS_BUF_OFFSET			(USERGROUP_BUF_OFFSET+USERGROUP_BUF_SIZE)
#define GPS_BUF_SIZE			(32*1024)

//��дʶ��
#define HAND_WRITE_BUF_OFFSET	(GPS_BUF_OFFSET+GPS_BUF_SIZE)
#define HAND_WRITE_BUF_SIZE		(0x210000)

//ƴ�����뷨
#define ALPHA_INPUT_OFFSET      (HAND_WRITE_BUF_OFFSET+HAND_WRITE_BUF_SIZE)
#define ALPHA_INPUT_SIZE        (0x70800)

//ƴ�����뷨��ʾ
#define ALPHA_DISPLAY_OFFSET	(ALPHA_INPUT_OFFSET+ALPHA_INPUT_SIZE)
#define ALPHA_DISPLAY_SIZE		(0x4B000)

//�������뷨  0x4aa89e
#define INPUT_BUF_OFFSET		(ALPHA_DISPLAY_OFFSET+ALPHA_DISPLAY_SIZE)
#define INPUT_BUF_SIZE			(0x4aa900)

//��ǰ̨ʹ�õ���ʱbuf 1
#define FRONT_BUF1_OFFSET		(INPUT_BUF_OFFSET+INPUT_BUF_SIZE)
#define FRONT_BUF1_SIZE			(4096)

//��ǰ̨ʹ�õ���ʱbuf 2
#define FRONT_BUF2_OFFSET		(FRONT_BUF1_OFFSET+FRONT_BUF1_SIZE)
#define FRONT_BUF2_SIZE			(10240)

//��ǰ̨ʹ�õ���ʱbuf 3
#define FRONT_BUF3_OFFSET		(FRONT_BUF2_OFFSET+FRONT_BUF2_SIZE)
#define FRONT_BUF3_SIZE			(20480)

//��˾�б�
#define COMPANY_BUF_OFFSET		(FRONT_BUF3_OFFSET+FRONT_BUF3_SIZE)
#define COMPANY_BUF_SIZE		(0x200000)

//Ա���б�
#define EMPLOYE_BUF_OFFSET		(COMPANY_BUF_OFFSET+COMPANY_BUF_SIZE)
#define EMPLOYE_BUF_SIZE		(200*1024)

//(��������)Ŀ�ĵ��б�
#define REGION_BUF_OFFSET		(EMPLOYE_BUF_OFFSET+EMPLOYE_BUF_SIZE)
#define REGION_BUF_SIZE			(0x100000)

//�쳣ԭ���
#define ABNORMAL_BUF_OFFSET		(REGION_BUF_OFFSET+REGION_BUF_SIZE)
#define ABNORMAL_BUF_SIZE		(10*1024)

//������Ϣ��
#define CAR_BUF_OFFSET			(ABNORMAL_BUF_OFFSET+ABNORMAL_BUF_SIZE)
#define CAR_BUF_SIZE			(100*1024)

//������Ϣ��
#define FLIGHT_BUF_OFFSET		(CAR_BUF_OFFSET+CAR_BUF_SIZE)
#define FLIGHT_BUF_SIZE			(100*1024)

//������б�
#define PROBLEM_BUF_OFFSET		(FLIGHT_BUF_OFFSET+FLIGHT_BUF_SIZE)
#define PROBLEM_BUF_SIZE		(10*1024)

//���ּ��б�
#define LEAVE_BUF_OFFSET		(PROBLEM_BUF_OFFSET+PROBLEM_BUF_SIZE)
#define LEAVE_BUF_SIZE			(10*1024)

#define CHAXUN_BUFF_OFFSET     (LEAVE_BUF_OFFSET+LEAVE_BUF_SIZE)
#define CHAXUN_BUFF_SIZE        (40960)//40K

//�����������
#define VOLWEI_BUF_OFFSET		(CHAXUN_BUFF_OFFSET+CHAXUN_BUFF_SIZE)
#define VOLWEI_BUF_SIZE			(2*1024)

//���䷽ʽ(���)
#define TRANSPORT_BUF_OFFSET	(VOLWEI_BUF_OFFSET+VOLWEI_BUF_SIZE)
#define TRANSPORT_BUF_SIZE		(10*1024)


//�������ʹ��    (������������ǰִ̨��,���Ը�buf����������Ϊǰ̨����ʱbufʹ��)
#define UPGRADE_BUF_OFFSET		(TRANSPORT_BUF_OFFSET+TRANSPORT_BUF_SIZE)
#define UPGRADE_BUF_SIZE		(0x400000)


//��̨ʹ�õ���ʱbuf
/*------�ӵ��ô���ռ��  bytes   M   */


//���Ϲ�ռ��  +   =  M

//ע��::::�ô�ʣ�����С��3M,��Ҫ�ر�ע����øú����ĵط�,�Ƿ��г����Ŀ���,�м��м�!!
#define REUSABLE_BUF_OFFSET		(UPGRADE_BUF_OFFSET+UPGRADE_BUF_SIZE)
#define REUSABLE_BUF_SIZE		(SDRAM_VIDEO_SFC_MEMORY_POOL_SIZE-REUSABLE_BUF_OFFSET)


/* ע���άʶ���buff�ռ��Ǵ�SFC�з����
   ע��������ʼ��Ҫ����Ҫ��buff���� ��С��Ҫ������SFC�ķ�Χ 20110504 */
//#define QR_DECODE_BUF_OFFSET  (0x25400000)
//#define QR_DECODE_BUF_SIZE    (0x700000)
/*----------------------------------------------------------------------------*/
static char *hsaSdramMemory()
{
	return ((char *) ((int)&gSdramVideoSfcMemoryPool[0] ) );
}

/*----------------------------------------------------------------------------*/
//С��¼�洢ʹ��
void *hsaSdram_SmallRecBuf()
{
	return hsaSdramMemory()+SMALL_REC_BUF_OFFSET;
}

U32 hsaSdram_SmallRecSize()
{
	return SMALL_REC_BUF_SIZE;
}

//���¼�洢ʹ��
void *hsaSdram_LargeRecBuf()
{
	return hsaSdramMemory()+LARGE_REC_BUF_OFFSET;
}

U32 hsaSdram_LargeRecSize()
{
	return LARGE_REC_BUF_SIZE;
}

//һƱ���ʹ��
void *hsaSdram_MultiRecBuf()
{
	return hsaSdramMemory()+MULTI_CODE_BUF_OFFSET;
}

U32 hsaSdram_MultiRecSize()
{
	return MULTI_CODE_BUF_SIZE;
}

//С��¼��ŵĶ���
void *hsaSdram_RecQueBuf()
{
	return hsaSdramMemory()+REC_QUEUE_BUF_OFFSET;
}

U32 hsaSdram_RecQueSize()
{
	return REC_QUEUE_BUF_SIZE;
}

//���ͼ�¼ʱʹ�õĲ���
void *hsaSdram_RecSendBuf()
{
	return hsaSdramMemory()+REC_SEND_BUF_OFFSET;
}

U32 hsaSdram_RecSendSize()
{
	return REC_SEND_BUF_SIZE;
}

//�洢�����¼   (�ô�С�����˿��Դ����������  Ŀǰ�����50��)
void *hsaSdram_RecGroupBuf()
{
	return hsaSdramMemory()+REC_GROUP_BUF_OFFSET;
}

U32 hsaSdram_RecGroupSize()
{
	return REC_GROUP_BUF_SIZE;
}

//����Ա�
void *hsaSdram_BarCompareBuf()
{
	return hsaSdramMemory()+BARCOMPARE_BUF_OFFSET;
}

U32 hsaSdram_BarCompareSize()
{
	return BARCOMPARE_SIZE;
}

//���ݱ����
void *hsaSdram_DecodeBuf()
{
	return hsaSdramMemory()+DECODE_BUF_OFFSET;
}

U32 hsaSdram_DecodeSize()
{
	return DECODE_BUF_SIZE;
}

//�û�����(��¼ʱ���ص�¼��Ա������Ϣ)
void *hsaSdram_UserGroupBuf()
{
	return hsaSdramMemory()+USERGROUP_BUF_OFFSET;
}

U32 hsaSdram_UserGroupSize()
{
	return USERGROUP_BUF_SIZE;
}


//GPSʹ��
void *hsaSdram_GpsBuf()
{
	return hsaSdramMemory()+GPS_BUF_OFFSET;
}

U32 hsaSdram_GpsSize()
{
	return GPS_BUF_SIZE;
}

//��дʶ��
void *hsaSdram_HandWriteBuf()
{
	return hsaSdramMemory()+HAND_WRITE_BUF_OFFSET;
}

U32 hsaSdram_HandWriteSize()
{
	return HAND_WRITE_BUF_SIZE;
}

//ƴ�����뷨
void *hsaSdram_AlphaInputBuf()
{
	return hsaSdramMemory()+ALPHA_INPUT_OFFSET;
}

U32 hsaSdram_AlphaInputSize()
{
	return ALPHA_INPUT_SIZE;
}

//ƴ�����뷨��ʾ
void *hsaSdram_AlphaDisplayBuf()
{
	return hsaSdramMemory()+ALPHA_DISPLAY_OFFSET;
}

U32 hsaSdram_AlphaDisplaySize()
{
	return ALPHA_DISPLAY_SIZE;
}

//�������뷨
void *hsaSdram_InputBuf()
{
	return hsaSdramMemory()+INPUT_BUF_OFFSET;
}

U32 hsaSdram_InputSize()
{
	return INPUT_BUF_SIZE;
}

//��ǰ̨ʹ�õ���ʱbuf 1
void *hsaSdram_FrontBuf1()
{
	return hsaSdramMemory()+FRONT_BUF1_OFFSET;
}

U32 hsaSdram_Front1Size()
{
	return FRONT_BUF1_SIZE;
}

//��ǰ̨ʹ�õ���ʱbuf 2
void *hsaSdram_FrontBuf2()
{
	return hsaSdramMemory()+FRONT_BUF2_OFFSET;
}

U32 hsaSdram_Front2Size()
{
	return FRONT_BUF2_SIZE;
}

//��ǰ̨ʹ�õ���ʱbuf 3
void *hsaSdram_FrontBuf3()
{
	return hsaSdramMemory()+FRONT_BUF3_OFFSET;
}

U32 hsaSdram_Front3Size()
{
	return FRONT_BUF3_SIZE;
}

//��˾�б�
void *hsaSdram_CompanyBuf()
{
	return hsaSdramMemory()+COMPANY_BUF_OFFSET;
}

U32 hsaSdram_CompanySize()
{
	return COMPANY_BUF_SIZE;
}

//Ա���б�
void *hsaSdram_EmployeBuf()
{
	return hsaSdramMemory()+EMPLOYE_BUF_OFFSET;
}

U32 hsaSdram_EmployeSize()
{
	return EMPLOYE_BUF_SIZE;
}

//(��������)Ŀ�ĵ��б�
void *hsaSdram_RegionBuf()
{
	return hsaSdramMemory()+REGION_BUF_OFFSET;
}

U32 hsaSdram_RegionSize()
{
	return REGION_BUF_SIZE;
}
//�쳣ԭ���б�
void *hsaSdram_AbnormalBuf()
{
	return hsaSdramMemory()+ABNORMAL_BUF_OFFSET;
}

U32 hsaSdram_AbnormalSize()
{
	return ABNORMAL_BUF_SIZE;
}
//������Ϣ�б�
void *hsaSdram_CarBuf()
{
	return hsaSdramMemory()+CAR_BUF_OFFSET;
}

U32 hsaSdram_CarSize()
{
	return CAR_BUF_SIZE;
}
//������Ϣ�б�
void *hsaSdram_FlightBuf()
{
	return hsaSdramMemory()+FLIGHT_BUF_OFFSET;
}
//�����б�ģ����ѯʹ��buf
void *hsaSdram_GetChaXunBuf(void)
{
	return hsaSdramMemory()+CHAXUN_BUFF_OFFSET;
}
int hsaSdram_GetChaXunBufSize()
{
	return CHAXUN_BUFF_SIZE;
}
U32 hsaSdram_FlightSize()
{
	return FLIGHT_BUF_SIZE;
}
//������б�
void *hsaSdram_ProblemBuf()
{
	return hsaSdramMemory()+PROBLEM_BUF_OFFSET;
}

U32 hsaSdram_ProblemSize()
{
	return PROBLEM_BUF_SIZE;
}

//���ּ��б�
void *hsaSdram_LeaveBuf()
{
	return hsaSdramMemory()+LEAVE_BUF_OFFSET;
}

U32 hsaSdram_LeaveSize()
{
	return LEAVE_BUF_SIZE;
}

//�����������
void *hsaSdram_WeiVolBuf()
{
	return hsaSdramMemory()+VOLWEI_BUF_OFFSET;
}

U32 hsaSdram_WeiVolSize()
{
	return VOLWEI_BUF_SIZE;
}


//���䷽ʽ(���)
void *hsaSdram_TransportBuf()
{
	return hsaSdramMemory()+TRANSPORT_BUF_OFFSET;
}

U32 hsaSdram_TransportSize()
{
	return TRANSPORT_BUF_SIZE;
}

//�������ʹ��    (������������ǰִ̨��,���Ը�buf����������Ϊǰ̨����ʱbufʹ��)
void *hsaSdram_UpgradeBuf()
{
	return hsaSdramMemory()+UPGRADE_BUF_OFFSET;
}

U32 hsaSdram_UpgradeSize()
{
	return UPGRADE_BUF_SIZE;
}


//
void * hsaSdramReusableMemoryPool( void )
{
	return hsaSdramMemory()+REUSABLE_BUF_OFFSET;
}

unsigned int hsaSdramReusableMemoryPool_Size( void )
{
	return (REUSABLE_BUF_SIZE);
}

/*void *hsaSdram_QRDecodeBuf()
{
	return (U8 *)QR_DECODE_BUF_OFFSET;//hsaSdramReusableMemoryPool()+REUSABLE_BUF_SIZE-QR_DECODE_BUF_SIZE;
}

int hsaSdram_QRDecodeBufSize()
{
	return QR_DECODE_BUF_SIZE ;
}*/

void * hsaSdramUnCacheReusableMemoryPool( void )
{
	return (((U32)hsaSdramReusableMemoryPool())&NON_CACHEABLE_MASK);
}

unsigned int hsaSdramUnCacheReusableMemoryPool_Size( void )
{
	return (REUSABLE_BUF_SIZE);
}


//����ͼƬ������
void *hsaSdram_GetPicDecodeBuf(void)
{
     return ((void *) &(gSdramPicDecodeMemoryPool[0]) ); 
}

int hsaSdram_GePicDecodeBufLen(void)
{
    return  SDRAM_PIC_DECODE_MEMORY_POOL_SIZE;
}


//���nand reserve block map(2k)��Sdram�ϵ�λ��
void * hsaNandReserv_GetBlockMapBuffer(void ) 
{
	return ((void *) &(gSdramSysMemoryPool[SDRAM_BLOCK_MAP_OFFSET]) );
}
//���nand reserve block map�Ĵ�С(2k)
unsigned int  hsaNandReserv_GetBlockMapBufferSize(void )
{
	return ((unsigned int) SDRAM_BLOCK_MAP_SIZE );
}

//�����Ҫ����Ĳ���(6k)��Sdram�ϵ�λ��
void * hsaPersistentParameters_GetBuffer(void )
{
	return ((void *) &(gSdramSysMemoryPool[SDRAM_PERSISTENT_PARAMETER_OFFSET]) );
}
//�����Ҫ����Ĳ����ܴ�С(6k)
unsigned int  hsaPersistentParameters_GetBufferSize(void )
{
	return ((unsigned int) SDRAM_PERSISTENT_PARAMETER_SIZE );
}

//�����ʾ������ڴ�
//��ʾ��QVGA��С320*240-----(((320*240)*2)*3)=150k*3
//��ʾ��QCIF��С220*176-----(((220*176)*2)*3)=77440*3
void * hsaDisplayMemoryPool_GetBuffer( void )
{
	return ((void *) (&gSdramSysMemoryPool[SDRAM_DISP_MEMORY_POOL_OFFSET]) );
}
//�����ʾ������ڴ��С
unsigned int  hsaDisplayMemoryPool_GetBufferSize( void )
{
	return ((unsigned int)SDRAM_DISP_MEMORY_POOL_SIZE);
}

/*----------------------------------------------------------------------------*/
#if 1
/*----------------------------------------------------------------------------*/
void * hyswNandFlash_GetReadPool_NoUse(void)
{
    //return ((void * ) ( &gkReadPool[0x400]));
    return ((void * ) ( &gkReadPool[0]));
}
/*----------------------------------------------------------------------------*/
void * hyswNandFlash_GetWritePool_NoUse(void)
{
    return ((void * ) (&gtmMemoryPool[0]));
}
/*----------------------------------------------------------------------------*/
#endif

void * hyswNandFlash_GetReadPool(void)
{
	//��Sdram��Ԥ��4k = 1��Nand����pagesize
	(void)hyswNandFlash_GetReadPool_NoUse();
    return ((void * ) (&gSdramSysMemoryPool[SW_NAND_READ_POOL_OFFSET]));
}
void * hyswNandFlash_GetWritePool(void)
{
	//72k = 4+4+64k(4k nandд���棬4k MLC nandд���棬64k mapping table)
    return ((void * ) (&gSdramSysMemoryPool[SW_NAND_WRITE_POOL_OFFSET]));
}
/*----------------------------------------------------------------------------*/

//USBʱ��BLOCKӳ��ʹ�õ�buffer
void * hsaUsbTransport_GetBuffer( void )
{
	return ((void *) (&gSdramSysMemoryPool[USB_TRANSPORT_OFFSET] ) );
}

unsigned int hsaUsbTransport_GetBufferSize( void )
{
    return (USB_TRANSPORT_SIZE);
}

/*-----------------------------------------------------------
 * ����:hsaConvertUnCacheMemory
 * ����:��cacheble�ĵ�ַת����uncheble��ַ
 * ����:��Ҫת����ָ��
 * ����:�õ�uncheble�ĵ�ַ
 *----------------------------------------------------------*/
void *hsaConvertUnCacheMemory(void *pMem)
{
    return ((void *)((U32)(((U32)pMem) & NON_CACHEABLE_MASK)));    
}

/*----------------------------------------------------------------------------*/
