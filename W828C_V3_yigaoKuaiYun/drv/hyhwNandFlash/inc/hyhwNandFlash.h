


#ifndef HY_HW_NANDFLASH_H_
#define HY_HW_NANDFLASH_H_


/*---------------------------------------------------------------------------
   Project include files:
   --------------------------------------------------------------------------*/

#define NAND_SIZE_UNIT_BIT		(20)//M Bytes��λ��bit��

/* NandFlash type 
*/
typedef enum
{
    NAND_16K_512B = 0,
    NAND_128K_2K,
    NAND_32K_512B,
    NAND_512K_2K,
    NAND_256K_2K_MLC,	/* ��ʵ��128K_2K, ������żҳ�ڲ�ͬ��block�ϣ�����block size���ڴ���һ�� */
    NAND_256K_2K,
    NAND_512K_2K_MLC,	/* ��ʵ��256K_2K, ������żҳ�ڲ�ͬ��block�ϣ�����block size���ڴ���һ�� */
    NAND_512K_2K_JOINT,	/* ��ʵ��256K_2K, ���ڵ�2��256k block���Ϊһ��512k��block */
    NAND_512K_4K,
    NAND_1M_4K_JOINT,
    NAND_UNKNOWN
} eNAND_TYPE;

/* NandFlash class 
*/
typedef enum
{
    SLC ,
    MLC
} eNAND_CLASS;

typedef enum
{
    DISABLE ,
    ENABLE
} eNAND_SELECT;

typedef enum
{
    CS0 ,
    CS1 ,
    CS2 ,
	CS3 ,
	CS4 ,
    CS5 ,
    CS6 ,
	CS7 
} eNAND_CS;

typedef enum
{
    NandFlashProcess_success,
    NandFlashProcess_fall
} eNAND_PROCESS;



/* NandFlash bank ���� */
typedef struct
{
    int          Gpio_nCE;		/* Gpio mask bit pattern , corresponding bit equals 1*/
    int          Gpio_nWP;		/* Gpio mask bit pattern , corresponding bit equals 1*/
    unsigned int SMCBank_DATA;
    unsigned int SMCBank_CLE;
    unsigned int SMCBank_ALE;    
    int          bankNo;

} NandFlashBank_st;

/* nandflash�Ĵ洢���� */
typedef struct
{
    NandFlashBank_st bank0;
    NandFlashBank_st bank1;

    unsigned int	reservedSize;

} InternalStorageMedia_st;





typedef volatile struct 
{
	U16	setUp;
	U16	pulseWidth;
	U16	hold;
	U32	regValue;
} tNAND_IO_CYCLE;

typedef volatile struct 
{
	U8	makerCode;
	U8	deviceCode;
	U8	extendedIDCode;
	U8	extendedIDCode_e1;
	U8	extendedIDCode_e2;
	U32	nandFlashSize;
	eNAND_TYPE nandFlashType;
} tNAND_ID;

/* nandflash ��nandPageSize �� nandPagesPerBlocksBits 
   �������ڼٶ�����nand�� type��һ���ģ����Բ���Ҫʹ������
   nandPagesPerBlockBits ��ʹ��2��ָ������ʾ��ÿblock ��page �� 
	type				pagesPerBlock	nandPagesPerBlockBits 
	NAND_16K_512B			32				5				
	NAND_32K_512B			64				6
	NAND_128K_2KB			64				6
	NAND_256K_2K			128				7		�����ͺ��������
	NAND_512K_2KB			256				8	*/     

typedef volatile struct 
{
	
	
	/* ������һЩ�������ڳ�ʼ��ʱȷ����Ϊ�˼��㷽�㣬*/
	U32		blockSize;
	U16		pageSize;
	
	U16		blockSizeBit;
	U16		pageSizeBit;	/*	pageSizeBit ������Ϊ�˱����������ָ����ʾ��pageSize	
										pageSize ���ڽ���2�֣� 512B�� 2KB����ӦpageSizeBit Ϊ 9 �� 11*/
	U16		pagesPerBlockBit;
	
	U32		size;			/* ������bank������һ��ʱ���ܵ���������MΪ��λ */
	eNAND_TYPE	type;
	
	U16		readWaitTime;	/* �󲿷�nandflash �������֮�����ʱΪ24ms����һ����50ms
										������Ҫ������ݽ��е��� */
	/*	����ײ��ٱ���һ��д�г������Щblock��Ӧ��page����ֹ��usb�ź��и���ʱ��������߼����mapping table
		�Ӷ�����Ա�����block��д�룬����ʹ��unsigned int���͵���������ʾpage index 
		�߼�Ϊ  protectPageStart <= page < protectPageEnd 
		ע�����page������������page  */
	unsigned int		protectPageStart;	
	unsigned int		protectPageEnd;	

	/*	ע����ڶ���NAND_256K_2K_MLC�������߼�page������2֮�󣬲�������page
		���������͵�flash����������page */
	unsigned int		reservePageStart;	
	unsigned int		reservePageEnd;	

} tNAND_INFO;


typedef volatile struct 
{

/*�������򡢱����������Դ����*/	

	U32		protectPageStart;
	U32		protectPageEnd;
	U32		protectblockStart;
	U32		protectblockEnd;	
	
	U32		programAreapageStart;
	U32		programAreapageEnd;
	U32		programAreablockStart;
	U32		programAreablockEnd;
	
	Bool	needReadBack;
} tNAND_PROTECT_RESERV;

typedef volatile struct
{
	U32		bankNo;
	U32 	smcBankDATA;
	U32 	smcBankALE;
	U32 	smcBankCLE;
	S32		gpioNWP;
	U32		Gpio_nWP;
} tNAND_BANK;



typedef volatile struct
{
	tNAND_BANK 	bank0;
	tNAND_BANK 	bank1;
	tNAND_BANK 	bank2;
	tNAND_BANK 	bank3;
	U32			reservedSize;
} tINTERNAL_STORAGE_MEDIA;


/*---------------------------------------------------------------------------
   Global variables:
   -------------------------------------------------------------------------*/
extern  tNAND_PROTECT_RESERV			protectAndReserv;
extern	tNAND_INFO						gtnandInfo;
extern  const InternalStorageMedia_st	internalStorageMedia;

/*-----------------------------------------------------------------------------
* ����:	hyhwNand_init
* ����:	��ʼ��NandFlash�����nandflash����Ϣ
*		ϵͳ�У�����nandLogicDrv_Init�ڳ�ʼ���е���һ�Σ����ԣ�����overlay�� sram
*		�У��Խ�ʡsram��ʹ��
* ����:	none
* ����:	none�� nandflash����Ϣ��ȫ�ֱ���gtnandInfo��
*----------------------------------------------------------------------------*/
eNAND_PROCESS hyhwNandFlash_Init( void );

/*-----------------------------------------------------------------------------
* ����:	hyhwNand_pageRead
* ����:	��ָ��page��ָ��ƫ�ƶ�ȡָ����С�����ݣ� ���ƫ����Ϊ0�����page��ʼ�ĵط�
*		�����ݣ���ԭ����pageRead�ĺ����Ĺ���һ��
* ����:	data----ָ���������ȡ���ݵ�buffer
*		page----ָ����page��
*		offset--��ָ��page�е�ƫ��
*		size----��Ҫд�����ݵĳ��ȣ�byte��
* ����:	HY_OK	//2006.12.23 �� �޸�
*		HY_ERROR//�������bit������4���򷵻�HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_DataRead(Int8 * pdata, U32 page, U32 offset, U32 size);

/*-----------------------------------------------------------------------------
* ����:	hyhwNand_erase
* ����:	��ָ����block����
* ����:	block----��Ҫ������block��
* ����:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_Erase( U32 block );

/*-----------------------------------------------------------------------------
* ����:	hyhwNand_pageWrite
* ����:	��ָ����page��д��ָ����С������
* ����:	data----ָ���������Ҫд������
*		page----ָ����page��
*		size----��Ҫд�����ݵĿ�ȣ�byte��
* ����:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_PageWrite( Int8* pdata, U32 page, U32 size );

/*-----------------------------------------------------------------------------
* ����:	hyhwNand_pageWrite
* ����:	��ָ����page��д��ָ����С������
* ����:	data----ָ���������Ҫд������
*		page----ָ����page��
*		size----��Ҫд�����ݵĿ�ȣ�byte��
* ����:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_PageWrite_slc( Int8* pdata, U32 page, U32 offset, U32 size );

/*-----------------------------------------------------------------------------
* ����:	hyNandflash_InformUsbStatus
* ����:	�� usb ״̬�£�nandflash hardware �����ᱣ��reserve�������������usb ׼������
*		֪ͨnandflash Ӳ��drv��������usb״̬
* ����:	usbStatus	--	1, ��ʾҪ����usb ״̬�ˣ�0 ��ʾ�˳���usb ״̬
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwNandflash_InformUsbStatus(U32 usbStatus);
/*-----------------------------------------------------------------------------
* ����:	hyNandFlash_EnableWrite
* ����:	��nandflash��WP�øߣ�ʹд�������Խ��У���ȥ��д����״̬
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/

void hyhwNandFlash_EnableWrite(void); ///���ڻ�û��д������������
/*-----------------------------------------------------------------------------
* ����:	hyNandFlash_DisableWrite
* ����:	��nandflash��WP�õͣ�ʹд���������Խ��У�������д����״̬
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwNandFlash_DisableWrite(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwNand_blockWrite
* ����:	��ָ����block��д��ָ����С������
* ����:	data----ָ���������Ҫд������,�����ݱ�����sdram�У�ע����д�Ĺ����н����ݰ��Ƶ�sram��
*		block----ָ����block��
*		pagenum----��Ҫд���page��
* ����:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_BlockWrite( Int8* pdata, U32 block, U32 pagenum);


/*-----------------------------------------------------------------------------
* ����:	hyhwNand_informUsbStatus
* ����:	�� usb ״̬�£�nandflash hardware �����ᱣ��reserve�������������usb ׼������
*		֪ͨnandflash Ӳ��drv��������usb״̬
* ����:	usbStatus	--	1, ��ʾҪ����usb ״̬�ˣ�0 ��ʾ�˳���usb ״̬
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwNandflash_InformUsbStatus(U32 usbStatus);

/*-----------------------------------------------------------------------------
* ����:	hyhwNand_enableWrite
* ����:	��nandflash��WP�øߣ�ʹд�������Խ��У���ȥ��д����״̬
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwNandFlash_EnableWrite(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwNand_disableWrite
* ����:	��nandflash��WP�õͣ�ʹд���������Խ��У�������д����״̬
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwNandFlash_DisableWrite(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwNandFlash_BlockWriteNormal
* ����:	��ָ����block��д��ָ����С������
* ����:	data----ָ���������Ҫд������,�����ݱ�����sdram�У�ע����д�Ĺ����н����ݰ��Ƶ�sram��
*		block----ָ����block��
*		pagenum----��Ҫд���page��
* ����:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_BlockWriteNormal( Int8 * pdata, U32 block, U32 pagenum);
/*-----------------------------------------------------------------------------
* ����:	hyhwNandFlash_BlockWriteTwoPlane
* ����:	��ָ����block��д��ָ����С������
* ����:	data----ָ���������Ҫд������,�����ݱ�����sdram�У�ע����д�Ĺ����н����ݰ��Ƶ�sram��
*		block----ָ����block��
*		pagenum----��Ҫд���page��
* ����:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_BlockWriteTwoPlane( Int8* pdata, U32 block, U32 pagenum);
/*-----------------------------------------------------------------------------
* ����:	hyhwNand_twoPlanePageWrite
* ����:	��ָ����page��д��ָ����С������
* ����:	data----ָ���������Ҫд������
*		page1----ָ����page��
*		page2----ָ����page��
* ����:	HY_OK
*		HY_ERROR
* ע�⣺�ú���Ŀǰֻ��֧��2K��pagesize��page1��page2�ڽ��ٵ�2������block�У�page1
*       ��ǰһ����ż����block��page2�ں�һ��(����)��������block�е�ƫ��������ͬ�ģ�
*       Ҳ����˵��page2��page1 �� blocksize
*       �ú�����֧��˫Ƭ����flash����Ϊ�������͵�flash��֮ǰ���Ѿ�ʵ��������д�Ĺ���
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_TwoPlanePageWrite( Int8 * pdata1,Int8 * pdata2, U32 page1, U32 page2);

/*-----------------------------------------------------------------------------
* ����:	hyhwNand_twoPageModeBlockErase
* ����:	��ָ����block����
* ����:	block----��Ҫ������block��
* ����:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_TwoPageModeBlockErase( U32 block );


#endif /* HY_HW_NANDFLASH_H_ */