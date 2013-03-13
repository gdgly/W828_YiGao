/******************************************************************************
*  �ļ�:		nandSeqStore.c (Hyctron Nandflash Abstract Layer 's fbd driver)
*  ������		ZhouJie 
*  �������� :	2010 12 17 15:52
*  ˵��     :	�������Ӧ�ã�ֱ��ʹ��nand���д洢�ķ���

*   ����
*    1����ȡ����������δ�ϴ����ݡ�
*    2�����ɼ�¼��
*    3���ϴ��ɹ���Ըü�¼����ǣ���ʶ�ü�¼���ϴ���
*    4��ɾ����¼��(���ϴ��������ϴ��ļ�¼����ɾ��)��ɾ�����Ǹü�¼��
*    5��ҵ��ͳ��: �������ڡ�ɨ������ ��ȡָ�����ڵ����и�ɨ������ ������ �� ���ϴ���  
*       (ע�����ڿ���Ϊָ����,ָ���£� ͬһ���ļ���������ɨ������)
*    6���鿴��¼: �鿴ĳһ��ɨ������ ��ָ��ĳһ�������ɨ���״̬ (ע: ״̬ Ϊ ���ϴ���δ�ϴ�) ��
*		��֧�ָ���״̬����ɾ��δ�ϴ���״̬��¼ ,�Ա��̨�����ϴ�
*    7��ɾ��ָ��ɨ�����͵ĵ����¼ �� ɾ�� ������������ʷ��¼

*	���

*	1�� bitmap ռһ��page ѭ��д�룬�����ֽڱ�ʾ��Ч��������
*   2�� file entry ռһ��page ѭ��д��, �����ֽڱ�ʾ��Ч��������
*   3�� ÿ��file �ĵ�һ��block ��¼�ļ��еļ�¼��״̬����Ϣ��pageѭ��д��
*        һ���ļ�24 block  һ��block 128 page�����¼��� 24*128 = 3076�� һ����¼ռ2bit
*		�����Щ��¼������һ��page�У���ÿ����¼���Է�10���ֽ�

*	����
*	������ƣ�Ϊ�˼������nand ��page ��Ϊ4096 �ֽڵĴ�С

*  ��������Լ��:	page ָ �����page�����������֣�512 ��2048
*					sector�� ָ logic sector������512��size
*  Ԥ����˵����	NAND_SUPPORT_HW_MLC			�Ƿ�֧��MLC��flash�����֧�֣���Ŀǰ��֧��1G��flash

*  �޸�(������ 2011-06-15)
*  ֧��slc (512M   page size 2048  һ��block 64��page)
*
*  �޸�(������ 2012-07-21)
   mlc��֧��ȥ��,û��ͬ������
   ���֧��30���ļ�,ÿ���ļ����洢4608����¼,ÿ����¼���512�ֽ�,ÿ�����ռ�ö���ļ��洢,��ÿ���ļ�����ͬʱ�洢����ļ�¼,
   ���ĳ���ļ��ļ�¼ȫ��ɾ��,����ļ�Ҳ���ͷ�,���ٴ�ʹ��. 
   
   ���ĳ��block�ļ�¼ȫ����ɾ��,���block�����ͷ�,�����¼�����ļ��ļ�¼����; ????????????Ŀǰ��ûʵ��,�������ʱ��Ҫ�������ժҪ��block
   
   ���Ӽ�¼״̬(3bit��ʾ bit2,bit1,bit0,����ʾ7��״̬),
   0:δ��(����״̬�ļ�¼����ʱ���ص��ϴ����з���)
   1:�Ѵ�(��״̬��¼��ʾ�Ѿ��ɹ��ϴ�,��ռ�ô洢�ռ�,ҵ��ͳ��ʱ����ͳ�Ƴ���)
   2:��ɾ(��״̬�ļ�¼��ʾ��ɾ������ռ���Ŵ洢�ռ�,�ļ��еĴ洢����Ҳ������,ҵ��ͳ��ʱ��ͳ����)
   3:����(����״̬��ʾ�ü�¼��Ҫ��ӵ����Ͷ�����,��Ҫ�ȴ��źŲż���)
   4:�ɴ�(��״̬��"����"״̬�ȵ������źź�ı�ɵ�,���Լ��ص��ϴ����з���;��"δ��"״̬���������ڸ�״̬����ʱ��Ҫ�ҵ����й����ļ�¼һ����)
   
 *****************************************************************************/


#ifndef HY_HW_NANDFILE_H_
#define HY_HW_NANDFILE_H_

/*---------------------------------------------------------------------------
   Project include files:
--------------------------------------------------------------------------*/
#include "hyTypes.h"


//#define NANDFLASH_DEBUG


/*********************************************
* slc 512M һ��block 64��page  һ��page 2k
  ��С��¼�洢��
  ���¼�洢��
  
  
  С��¼�洢�� 30���ļ�     ÿ���ļ���� 4608��  ÿ���ļ�Ҫռ��23block(ժҪռ��5��  ��¼ռ��18��)    ��Ҫռ��23*30=690 blocks
  
*********************************************/
#define PAGE_SIZE_2K			//nand page�Ĵ�С

#define NAND_FLASH_SIZE			512		//M  nand�ܴ�С
#define NAND_ENABLE_USESIZE		320		//M  �����ṩ�洢��¼ʹ�õ�nand��С
#define NAND_PAGE_SIZE			2048	//nand page�Ĵ�С

/*==============================================С��¼===========================================================*/

#define MAX_NAND_FILE			30		//���֧�ֵ��ļ���

#define REC_USE_MAXSIZE			100 	//M  �ɹ��˴�ʹ�õ�nand��С
#define BLOCK_MAX_NUM			(REC_USE_MAXSIZE<<3) /*����ʹ�õ��ܹ�block�� (REC_USE_MAXSIZE*1024)/128 */

#define SAVE_REC_MAX_BLOCKS		18		/*������ż�¼��block   ���ɴ��4608����¼  ��Ҫ5��block���ժҪ
								 		 ��ÿ���ļ����ռ��23��block    Ԥ�����30���ļ�   ���� 690block
								 		 bitmap  14��block  fileentry 26��block   ��730��block  ��91.25M���̿ռ�*/
								 
#define RECORD_PER_SIZE			512		/*ÿ����¼ռ���ֽ���*/
#define RECORDS_PER_PAGE		4		/*ÿ��page��ż�¼������*/
#define PAGE_PER_BLOCK			64		/*ÿ��block��page����*/
#define RECORDS_PER_BLOCK		(RECORDS_PER_PAGE*PAGE_PER_BLOCK)		/* һ��block�д�ż�¼������,����page ��*4 256 */

#define PAGE_PER_BLK_BIT		6		/*  1<<6  = 64 */
#define BLK_SIZE_BIT			17		/*  1<<17 = 128k*/

#define NAND_PAGE_DATASIZE		(NAND_PAGE_SIZE - 8) /*������һ��block �����8���ֽ������洢seq crc 0xaa55 0xaa55��
                                                      *����ʵ������������Ҫ����*/

#define	NAND_BITMAP_WORDS		128	/*�ɹ���block��= NAND_BITMAP_WORDS*32*/

#define NAND_BAD_BLOCKS			NAND_BITMAP_WORDS	//��¼����

#define	BLOCK_NUM_BITMAP		14	/* Ԥ����nand bitmap��һ������block����Ŀ*/
#define	BLOCK_START_BITMAP		nandFile_startBlock()	/* Ԥ����nand bitmap��һ������block����ʼblock index*/
#define	BLOCK_END_BITMAP		(BLOCK_START_BITMAP+BLOCK_NUM_BITMAP-1)	/* Ԥ����nand bitmap��һ������block�����block index*/

#define	BLOCK_NUM_FILEENTRY		26	/* Ԥ����file entry��һ������block����Ŀ*/
#define BLOCK_START_FILE		(BLOCK_END_BITMAP+1)//file��block��ʼ
#define BLOCK_END_FILE			(BLOCK_START_FILE+BLOCK_NUM_FILEENTRY-1)//file��block����


#define BLOCK_DATA_NUM			(BLOCK_MAX_NUM-BLOCK_NUM_BITMAP-BLOCK_NUM_FILEENTRY)	/*���ݿ���block����*/
#define BLOCK_DATA_START		(BLOCK_END_FILE+1)										/*���ݿ�ʼ��block*/
#define BLOCK_DATA_END			(BLOCK_DATA_START+BLOCK_DATA_NUM-1)						/*���ݽ�����block*/


/*	һ���ļ�ռ�õ�����block ��
	������Ҫ������ļ���Ϣ�洢��һ��page��,һ��page���ô�СΪ2048-8 = 2040;
	2040/MAX_NAND_FILE = ÿ���ļ�����ʹ�õĴ�С filesize
	ÿ���ļ�ǰ��12�ֽڹ̶�,�����洢pageOffset[]��blk[]�Ŀռ�Ϊ  (filesize-12)/2  ������U16��,����Ҫ /2*/
#define	MAX_NUMS_PER_FILE		((NAND_PAGE_DATASIZE/MAX_NAND_FILE - 12) / 2)	//pageOffset[]��blk[] ����ռ�õĿռ�   ��30���ļ�,��ֵΪ28

/*����MAX_NUMS_PER_FILE��������Ҫ���ժҪ��block����,������ҪժҪblock��Ϊx  (ժҪ�в��ٴ�crc����Ϣ)
  ����һԪ����ʽ
  (MAX_NUMS_PER_FILE-x-x)*64*4 = 2048/2*x ; 
�� MAX_NUMS_PER_FILE = (1024/256 + 2)*x ;
   x = MAX_NUMS_PER_FILE / (1024/256 + 2)
 
 ���� x = MAX_NUMS_PER_FILE/6 (����ȡ��) */
#define ABS_BLOCKS_PER_FILE		((MAX_NUMS_PER_FILE+5)/6)//ժҪռ��block    ��30���ļ�,��ֵΪ5

#define MAX_BLOCKS_PER_FILE		(MAX_NUMS_PER_FILE-ABS_BLOCKS_PER_FILE)//ÿ���ļ��ɹ���block�� ����ժҪ�ͼ�¼  ��30���ļ�,��ֵΪ23
#define DATA_BLOCKS_PER_FILE	(MAX_BLOCKS_PER_FILE-ABS_BLOCKS_PER_FILE)//ÿ���ļ���ż�¼��block��  ��30���ļ�,��ֵΪ18

/*ǰ���Ѿ�����ÿ���ļ��������洢��¼�����block��   ����˴����������ֵ��֮ǰ�����,�����ʹ��֮ǰ�����*/
#if (DATA_BLOCKS_PER_FILE > SAVE_REC_MAX_BLOCKS)
	#define MAX_RECORDS_PER_FILE	(SAVE_REC_MAX_BLOCKS * RECORDS_PER_BLOCK)	//ÿ���ļ����Դ�����ݵ������Ŀ
#else
	#define MAX_RECORDS_PER_FILE	(DATA_BLOCKS_PER_FILE * RECORDS_PER_BLOCK)	//ÿ���ļ����Դ�����ݵ������Ŀ
#endif


/* ժҪ�м�¼����ϢΪ*/
typedef struct RECORD_STATUS
{
	unsigned short	type:8;        	//ɨ������
	unsigned short	userType:5; 	//Ա������(Ȩ��)
	unsigned short  recState:3;		/*��¼״̬
									  0:δ��(����״̬�ļ�¼����ʱ���ص��ϴ����з���)
			   						  1:�Ѵ�(��״̬��¼��ʾ�Ѿ��ɹ��ϴ�,��ռ�ô洢�ռ�,ҵ��ͳ��ʱ����ͳ�Ƴ���)
			   						  2:��ɾ(��״̬�ļ�¼��ʾ��ɾ������ռ���Ŵ洢�ռ�,�ļ��еĴ洢����Ҳ������,ҵ��ͳ��ʱ��ͳ����)
			   						  3:����(����״̬��ʾ�ü�¼��Ҫ��ӵ����Ͷ�����,��Ҫ�ȴ��źŲż���)
			   						  4:�ɴ�(��״̬��"����"״̬�ȵ������źź�ı�ɵ�,���Լ��ص��ϴ����з���;��"δ��"״̬���������ڸ�״̬����ʱ��Ҫ�ҵ����й����ļ�¼һ����)
			   						*/
	
}tRECORD_STATUS;

#define ABSTRACT_SIZE			sizeof(tRECORD_STATUS)	/* 2 byte. ÿ����¼��ժҪ��ռ���ֽ���, �������8λҵ������   5λԱ������   3λ��¼״̬*/
#define ABSTRACT_NUM_PER_PAGE	(NAND_PAGE_SIZE/ABSTRACT_SIZE)	/*һ��page�洢ժҪ�ĸ���*/

#define	NAND_SEQUECE_PAGE_TAG	0xAA55	/*ÿ��block���8���ֽ�Ϊ���sequence�� CRC16 ,0xAA55, 0xAA55*/


#define	FILE_NAME_SIZE			6		/* �ļ����� 6���ֽ�*/

typedef struct
{
	char data[NAND_PAGE_DATASIZE];	/* ����  (2048 - 8)�ֽڣ�һ��page 2k�� ������8���ֽ��Ǳ�ʾ����*/
	
	unsigned short	sequece;	/* ��Ϊѭ��д�룬�ػ��󣬲�֪��д������µ�page��ʹ��sequece ��ʾһ��
	                               ���磬��һ�ֶ���1����2�ֿ�ʼ����2���������һ��page �����2������һ��
	                               page �����1�����ʾpage Ϊ2��page Ϊ���£���block ʱ����*/
	
	unsigned short	crc16;		/* ÿ��block���6���ֽ�Ϊ��� CRC ,0xAA55, 0xAA55*/
	
	unsigned short	tag1;
	unsigned short	tag2;
	
}tSEQUECE_PAGE;	



/* �ļ���Ϣ��͹����block ��Ŀ�й� */
typedef struct
{
	//ʹ��int   ʣ��2�ֽ����ļ�����
	int				name;
	unsigned short	dayIdx;					/*���ļ��ǵ���ڼ����ļ�*/
	/*char			name[FILE_NAME_SIZE];*/	/* �����ļ�����6�ֽڣ���2�ֽڣ���2�ֽڣ���2�ֽ� 
										   		���ļ�֧��ʹ�ö��Ŀ¼�������Ŀ¼���Ϊ
										   		name[0] == 0xF������Ϊ�ڼ�������Ŀ¼���������ݶ���block index*/
	unsigned short	totalRecords;			/* �ļ����ݼ�¼���ܼ�¼��  */
	unsigned short	reportedRecords;		/* �����̨����ļ�¼�� */
	unsigned short	deletedRecords;			/* ��ɾ���ļ�¼�� */
	
	unsigned short  pageOffset[ABS_BLOCKS_PER_FILE];/* ÿ��ժҪblock��������Ϣ��λ��*/
	unsigned short	blk[MAX_BLOCKS_PER_FILE];		/* ���ժҪ������*/
	
}tFILE_ENTRY;



/* ͨ��nand ��block��bitmap������֪�Ƿ��block���� , ��Ӧ��bit Ϊ1��ʾû��ռ��*/
#define  IS_BLOCK_FREE(blk)		(gnandBitmap[blk>>5] & (1<<(blk &0x1F)))
#define	 SET_BLOCK_USED(blk)	(gnandBitmap[blk>>5] &= ~(1<<(blk &0x1F)))
#define	 SET_BLOCK_FREE(blk)	(gnandBitmap[blk>>5] |= (1<<(blk &0x1F)))

//�����޸ĺ��״̬
typedef int (* MODIFY_STATUE_FUNC)(tRECORD_STATUS *pStatus);


extern	tFILE_ENTRY	gtFileEntry[MAX_NAND_FILE];

extern U8	*gpu8RecBuf;


/*==============================================���¼===========================================================*/
/*
����2���ļ�  
ÿ���ļ����洢1550��  
2���ļ����洢 1550*2 = 3100
��ռ�� 1600+50 = 1650
*/
#define MAX_PIC_FILE				2		//���֧�ֵ��ļ���

#define	PIC_USE_MAXSIZE				(NAND_ENABLE_USESIZE-REC_USE_MAXSIZE)	//220
#define PIC_BLOCK_NUM				(PIC_USE_MAXSIZE<<3)	//1760 blocks

#define PIC_PER_SIZE				(1024*64)	//ÿ��ͼƬ�Ĵ�С
#define PIC_PER_BLOCK				2			//ÿ��block�д��ͼƬ�ĸ���

#define	PIC_BITMAP_WORDS			128	/*�ɹ���block��= PIC_BITMAP_WORDS*32*/

/*bitmap  �� fileentry ��ʹ��50 blocks*/
#define	PIC_BLOCK_NUM_BITMAP		14	/* Ԥ����nand bitmap��һ������block����Ŀ*/
#define	PIC_BLOCK_START_BITMAP		(BLOCK_START_BITMAP+BLOCK_MAX_NUM)	/* Ԥ����nand bitmap��һ������block����ʼblock index*/
#define	PIC_BLOCK_END_BITMAP		(PIC_BLOCK_START_BITMAP+PIC_BLOCK_NUM_BITMAP-1)	/* Ԥ����nand bitmap��һ������block�����block index*/

//����page����,��2���ļ��洢   36 
#define PIC_BLOCK_FILEINFO			18	//�洢�ļ���Ϣ��block

#define PIC_FILE1_START				(PIC_BLOCK_END_BITMAP+1)
#define PIC_FILE1_END				(PIC_FILE1_START+PIC_BLOCK_FILEINFO-1)

#define PIC_FILE2_START				(PIC_FILE1_END+1)
#define PIC_FILE2_END				(PIC_FILE2_START+PIC_BLOCK_FILEINFO-1)


//1760-50 = 1710 
#define BLOCK_PIC_NUM				(PIC_BLOCK_NUM-PIC_BLOCK_NUM_BITMAP-PIC_BLOCK_FILEINFO*2)	/*���ݿ���block����*/
#define BLOCK_PIC_START				(PIC_FILE2_END+1)											/*���ݿ�ʼ��block*/
#define BLOCK_PIC_END				(BLOCK_PIC_START+BLOCK_PIC_NUM-1)							/*���ݽ�����block*/


#if 0//�������淽�������,ʵ����Ҫ��blockΪ1916, ������ʹ�õ�blockֻ��1710,  ������
	
	//2���ļ���Ϣ�ֿ����  ����ÿ��page��ֻ�����һ��fileentry
	#define	MAX_NUMS_PIC_FILE			((NAND_PAGE_DATASIZE - 4) / 2)	//1018

	/*
	(MAX_NUMS_PIC_FILE-x-x)*PIC_PER_BLOCK = PIC_ABS_NUM_PER_BLOCK*x
	(1018-x-x)*2 = 64*x
	x = 29.9
	*/
	#define PIC_ABS_BLOCKS_PER_FILE		30		//ÿ���ļ��д��ժҪblock�ĸ���
	#define PIC_MAX_BLOCKS_PER_FILE		(MAX_NUMS_PIC_FILE-PIC_ABS_BLOCKS_PER_FILE)	//�洢ժҪ�����ݵ��ܹ�block��  1018-30 = 988

	#define PIC_DATA_BLOCKS_PER_FILE	(PIC_MAX_BLOCKS_PER_FILE-PIC_ABS_BLOCKS_PER_FILE)//�洢ͼƬ���ݵ�block��  988-30 = 958
	#define MAX_PICS_PER_FILE			(PIC_DATA_BLOCKS_PER_FILE*PIC_PER_BLOCK)	//ÿ���ļ��ܹ����Դ��ͼƬ�� 958*2 = 1916
	
#else
	
	#define PIC_ABS_BLOCKS_PER_FILE		25
	#define PIC_MAX_BLOCKS_PER_FILE		800
	
	#define PIC_DATA_BLOCKS_PER_FILE	(PIC_MAX_BLOCKS_PER_FILE-PIC_ABS_BLOCKS_PER_FILE)	//775
	#define MAX_PICS_PER_FILE			(PIC_DATA_BLOCKS_PER_FILE*PIC_PER_BLOCK)	//775*2 = 1550
	
#endif

#define PIC_SAVECODE_LEN		29

//32 byte
typedef struct _picAbs_t
{
	char		barcode[PIC_SAVECODE_LEN];	//��Ӧ��������
	U8			status;			//����״̬ 1:��Ч  0:��Ч
	U16			size;			//���ݴ�С
	
}tPICABS;

#define PIC_ABSTRACT_SIZE			sizeof(tPICABS)			//ÿ��ժҪռ���ֽ���
#define PIC_ABS_NUM_PER_BLOCK		(NAND_PAGE_SIZE/PIC_ABSTRACT_SIZE)	//һ��block�п��Ա����ժҪ��  2048/32 = 64



typedef struct picfile_t
{
	unsigned short	totalPic;			/* �ļ����ݼ�¼���ܼ�¼��  */
	unsigned short	validPic;			/* ��Ч�ļ�¼�� */
	
	unsigned short  pageOffset[PIC_ABS_BLOCKS_PER_FILE];/* ÿ��ժҪblock��������Ϣ��λ��*/
	unsigned short	blk[PIC_MAX_BLOCKS_PER_FILE];		/* ���ժҪ������*/
	
}tPICFILE;


/* ͨ��nand ��block��bitmap������֪�Ƿ��block���� , ��Ӧ��bit Ϊ1��ʾû��ռ��*/
#define  PIC_IS_BLOCK_FREE(blk)		(gu32PicBitmap[blk>>5] & (1<<(blk &0x1F)))
#define	 PIC_SET_BLOCK_USED(blk)	(gu32PicBitmap[blk>>5] &= ~(1<<(blk &0x1F)))
#define	 PIC_SET_BLOCK_FREE(blk)	(gu32PicBitmap[blk>>5] |= (1<<(blk &0x1F)))


extern U32	appLockNandCtr;

#endif	//HY_HW_NANDFILE_H_
