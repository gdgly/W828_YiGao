
#ifndef	_HYHW_ARM926EJS_H_
#define	_HYHW_ARM926EJS_H_




/*	cp15�Ŀ��ƼĴ��� c1 ��λ���� */
#define ARM926EJS_CP15C1_BIT_RESERVE		0x00050078		/*BIT[18:16],BIT[6:3] reserve ��bit ��ȡֵ�����*/
#define ARM926EJS_CP15C1_BIT_ROUNDROBIN		BIT14			/* Replacement strategy for ICache and DCache:
																0 = Random replacement
																1 = Round-robin replacement. round-robin replacement */
#define ARM926EJS_CP15C1_BIT_ICACHE_ENABLE	BIT12
#define ARM926EJS_CP15C1_BIT_ROM_PROTECT	BIT9
#define ARM926EJS_CP15C1_BIT_SYS_PROTECT	BIT8
#define ARM926EJS_CP15C1_BIT_DCACHE_ENABLE	BIT2
#define ARM926EJS_CP15C1_BIT_MMU_ENABLE		BIT0


/*	data Tightly coupled memory size �Ķ���*/
#define ARM926EJS_DTCM_SIZE_0K				0x00000000
#define ARM926EJS_DTCM_SIZE_4K				0x0000000C
#define ARM926EJS_DTCM_SIZE_8K				0x00000010
#define ARM926EJS_DTCM_SIZE_16K				0x00000014
#define ARM926EJS_DTCM_SIZE_32K				0x00000018
#define ARM926EJS_DTCM_SIZE_64K				0x0000001C
#define ARM926EJS_DTCM_SIZE_128K			0x00000020
#define ARM926EJS_DTCM_SIZE_256K			0x00000024
#define ARM926EJS_DTCM_SIZE_512K			0x00000028
#define ARM926EJS_DTCM_SIZE_1M				0x0000002C

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_readFaultStatus
* ����:	ͨ����ȡЭ��������Fault state register c5, �õ������״̬
* ����:	none
* ����:	�����״̬
*----------------------------------------------------------------------------*/
U32 hyhwArm926ejs_readFaultStatus(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_readFaultAddress
* ����:	ͨ����ȡЭ��������Fault Address register c6, �õ�����ĵ�ַ
* ����:	none
* ����:	����ĵ�ַ
*----------------------------------------------------------------------------*/
U32 hyhwArm926ejs_readFaultAddress(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_drainWriteBuffer
* ����:	��write buffer������ǿ�ƻ�д������ռ� 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_drainWriteBuffer(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_updateControl
* ����:	����cp15�Ŀ��ƼĴ��� c1��ֵ
* ����:	val		-- Ҫд�뵽���ƼĴ��� c1��ֵ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_updateControl(U32 val);

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_enableDcache
* ����:	ʹ�� data cache 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_enableDcache(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_disableDcache
* ����:	��ֹ data cache 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_disableDcache(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_enableIcache
* ����:	ʹ�� Instruct cache 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_enableIcache(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_disableIcache
* ����:	��ֹ Instruct cache 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_disableIcache(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_invalidateDcache
* ����:	ʹ Data cache ��Ч 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_invalidateDcache(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_invalidateIcache
* ����:	ʹ Instruct cache ��ָ����Ч
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_invalidateIcache(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_invalidIcacheDcache
* ����:	ʹ Instruct cache �� Data cache ��Ч
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_invalidIcacheDcache(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926e_writeBackDcache
* ����:	��дData Cache����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926e_writeBackDcache(void);

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_mmuEnable
* ����:	ʹ��MMU
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_mmuEnable (void);

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_mmuEnable
* ����:	��ֹMMU
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_mmuDisable (void);

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_mmuConfig
* ����:	����arm926ej��mmu��ͨ��Э������cp15���
* ����:	sectionTable	-- ָ��mmu ��section table �ĵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_mmuConfig(int *sectionTable);


/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_dtcmConfig
* ����:	TCM (Tightly Coupled Memory) �Ŀ���
* ����:	baseAddr	-- data TCM memory �Ļ���ַ
*		sizebit		-- ��ʾTCM��С�Ķ��壬ע��ʹbitλ�ı�ʾ
*		enable		-- �Ƿ�enable 0-diable�� 1- enable
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_dtcmConfig(U32 baseAddr, U32 sizebit, U32 enable);




#endif  /* _HYHW_MMU_H_  */
