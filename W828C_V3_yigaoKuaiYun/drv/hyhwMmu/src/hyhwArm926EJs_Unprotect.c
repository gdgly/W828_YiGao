/*---------------------------------------------------------------------------------
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
*  ˵�������ļ�Ϊ arm926ej �˵� cp15 Э��������ز����Ĵ���
*  		��ϸ��Ϣ����ο�ARM��˾���ĵ�ADS_DeveloperGuide_D.pdf ��DDI0198C_926_trm.pdf
*		ע�⣬����ARM��ͬ���ںˣ��ں����üĴ�������������ͬ����Ҫ������صļ����ĵ�

*  Ӧ�ð���1�����ڳ�����ƣ�overlay��ʱ����icache��dcache�򿪵�����£���Ҫ�����²�����
*		��Ҫ��Ϊ�˱���overlayʱ����ԭ�г�����cache�У���������ʱʵ��������cache��
*		�ĳ����û��������copy�ĳ���������̰�����icache��dcache��ˢ�£�����˳��Ϊ
*		step1. ��дdata Cache  (���ú���hyhwArm926e_writeBackDcache)
*		step2. ��write buffer������ǿ�ƻ�д������ռ� (���ú��� hyhwArm926ejs_drainWriteBuffer)
*		step3. ʹInstruct Cache ��Data Cache��������Ч(���ú��� hyhwArm926ejs_invalidIcacheDcache)
* ---------------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwArm926EJs.h"




/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_readFaultStatus
* ����:	ͨ����ȡЭ��������Fault state register c5, �õ������״̬
* ����:	none
* ����:	�����״̬
*----------------------------------------------------------------------------*/
U32 hyhwArm926ejs_readFaultStatus(void)
{
	U32 val;
	__asm
	{
		MRC p15, 0, val, c5, c0, 0
	}
	return val;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_readFaultAddress
* ����:	ͨ����ȡЭ��������Fault Address register c6, �õ�����ĵ�ַ
* ����:	none
* ����:	����ĵ�ַ
*----------------------------------------------------------------------------*/
U32 hyhwArm926ejs_readFaultAddress(void)
{
	U32 val;
	__asm
	{
		MRC p15, 0, val, c6, c0, 0
	}
  	return val;
}


/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_drainWriteBuffer
* ����:	��write buffer������ǿ�ƻ�д������ռ� 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_drainWriteBuffer(void)
{
	U32 src=0;
	__asm 
	{
		MCR p15, 0, src, c7, c10, 4
	}
}



/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_updateControl
* ����:	����cp15�Ŀ��ƼĴ��� c1��ֵ
* ����:	val		-- Ҫд�뵽���ƼĴ��� c1��ֵ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_updateControl(U32 val)
{
	__asm
	{
		/* дЭ������cp15�Ŀ��ƼĴ��� c1*/
		MCR p15, 0, val, c1, c0, 0
		NOP
		NOP
	}
	
}



/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_enableDcache
* ����:	ʹ�� data cache 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_enableDcache(void)
{
	U32 val;
	
	__asm
	{
		/*  ��ȡЭ������cp15�Ŀ��ƼĴ��� c1 */
		MRC p15, 0, val, c1, c0, 0

		/* ���BIT2 ( C bit DCache enable/disable ) 0 = DCache disabled 1 = DCache enabled
		   Ȼ���д c1*/
		ORR val, val, ARM926EJS_CP15C1_BIT_DCACHE_ENABLE
		MCR p15, 0, val, c1, c0, 0
		NOP
		NOP
	}
	
}


/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_disableDcache
* ����:	��ֹ data cache 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_disableDcache(void)
{
	UInt32 val;
	
	__asm
	{
		/*  ��ȡЭ������cp15�Ŀ��ƼĴ��� c1 */
		MRC p15, 0, val, c1, c0, 0

		/* ���BIT2 ( C bit DCache enable/disable ) 0 = DCache disabled 1 = DCache enabled
		   Ȼ���д c1*/
		BIC val, val, ARM926EJS_CP15C1_BIT_DCACHE_ENABLE
		MCR p15, 0, val, c1, c0, 0
		NOP
		NOP
	}
	
}


/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_enableIcache
* ����:	ʹ�� Instruct cache 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_enableIcache(void)
{
	U32 val;
	
	__asm
	{
		/*  ��ȡЭ������cp15�Ŀ��ƼĴ��� c1 */
		MRC p15, 0, val, c1, c0, 0

		/* ��λBIT12 ( I bit ICache enable/disable ) 0 = ICache disabled 1 = ICache enabled
		   Ȼ���д c1*/
		ORR val, val, ARM926EJS_CP15C1_BIT_ICACHE_ENABLE  
		MCR p15, 0, val, c1, c0, 0
		NOP
		NOP
	}
	
}

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_disableIcache
* ����:	��ֹ Instruct cache 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_disableIcache(void)
{
	U32 val;
	
	__asm
	{
		/* ��ȡЭ������cp15�Ŀ��ƼĴ��� c1 */
		MRC p15, 0, val, c1, c0, 0

		/* ���BIT12 ( I bit ICache enable/disable ) 0 = ICache disabled 1 = ICache enabled
		   Ȼ���д c1*/
		BIC val, val, ARM926EJS_CP15C1_BIT_ICACHE_ENABLE  
		MCR p15, 0, val, c1, c0, 0
		NOP
		NOP
	}
	
}

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_invalidateDcache
* ����:	ʹ Data cache ��Ч 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_invalidateDcache(void)
{
	unsigned int src = 0;
	__asm
	{
		MCR p15, 0, src, c7, c6, 0
		NOP
		NOP
	}
}



/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_invalidateIcache
* ����:	ʹ Instruct cache ��ָ����Ч
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_invalidateIcache(void)
{
	U32 src = 0;
	
	__asm
	{
		MCR p15, 0, src, c7, c5, 0
		NOP
		NOP
	}
}

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_invalidIcacheDcache
* ����:	ʹ Instruct cache �� Data cache ��Ч
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_invalidIcacheDcache(void)
{
	U32 src = 0;
	
	__asm
	{
		MCR p15, 0, src, c7, c7, 0
		NOP
		NOP
	}
}

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926e_writeBackDcache
* ����:	��дData Cache����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926e_writeBackDcache(void)
{
	__asm
	{
		testClean:	
			MRC p15, 0, r15, c7, c10, 3
			BNE testClean
	}
}

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_mmuEnable
* ����:	ʹ��MMU
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_mmuEnable (void)
{
	int val;

	__asm
	{
		/* ��ȡЭ�������Ŀ��ƼĴ��� c1 */ 
		MRC p15, 0, val, c1, c0, 0

		/* ��λ BIT0: M�� ʹ��MMU��Ȼ���дc1 */
		ORR	val, val, ARM926EJS_CP15C1_BIT_MMU_ENABLE 
    	MCR p15, 0, val, c1, c0, 0
		NOP 
		NOP 
	    NOP
	}
}

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_mmuEnable
* ����:	��ֹMMU
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_mmuDisable (void)
{
	UInt32 val;
	
	__asm
	{
		/* ��ȡЭ�������Ŀ��ƼĴ��� c1 */
		MRC p15, 0, val, c1, c0, 0

		/* ���λBIT0: M�� ��ֹMMU��Ȼ���дc1 */
		BIC val, val, ARM926EJS_CP15C1_BIT_MMU_ENABLE
		MCR p15, 0, val, c1, c0, 0
		NOP
		NOP
	}
}

/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_mmuConfig
* ����:	����arm926ej��mmu��ͨ��Э������cp15���
* ����:	sectionTable	-- ָ��mmu ��section table �ĵ�ַ
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_mmuConfig(int *sectionTable)
{
	int val;
  
	/*  ͨ��Э������cp15����MMU */
	__asm
	{
		/* ��ȡЭ�������Ŀ��ƼĴ��� c1 */
		MRC p15, 0, val, c1, c0, 0
		
		/* �����ƼĴ��� c1 �� R λ(BIT9) �� Sλ (BIT8) ��λ���ٻ�д�� c1  
			R λ -- ROM protection 
			S λ -- System protection
			�� ĳ��domain�� access permitssion Ϊ��ʱ����access������Ӱ��  */  
		ORR val, val, (ARM926EJS_CP15C1_BIT_ROM_PROTECT|ARM926EJS_CP15C1_BIT_SYS_PROTECT)
    	MCR p15, 0, val, c1, c0, 0
		NOP
		NOP
		NOP
                
		/* д��Translation table base register c2����װpage table  */
		MCR p15, 0, sectionTable, c2, c0, 0
        
        /*	ʹTLB��Ч��Ŀ����ȷ��ʹ����д���TLB��*/
		MCR	p15, 0, r0, c8, c7, 0		// Invalidate TLB
		
		/* Domain Setting 
		   16��domain��ÿ��domain����ռ��2BIT������ÿ��domain��2��BIT ���� 01b��
		   ��ʾpermission check������ÿ��region��access permitssion������ 
		   ����������� 0x55555555 */
		mov	r0, 0x55555555
		MCR	p15, 0, r0, c3, c0, 0	
	}
	
}



/*-----------------------------------------------------------------------------
* ����:	hyhwArm926ejs_dtcmConfig
* ����:	TCM (Tightly Coupled Memory) �Ŀ���
* ����:	baseAddr	-- data TCM memory �Ļ���ַ
*		sizebit		-- ��ʾTCM��С�Ķ��壬ע��ʹbitλ�ı�ʾ
*		enable		-- �Ƿ�enable 0-diable�� 1- enable
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwArm926ejs_dtcmConfig(U32 baseAddr, U32 sizebit, U32 enable)
{
	U32	val;
	
	val = (baseAddr|sizebit|enable);
	__asm
	{
		MCR	p15, 0, val, c9, c1, 0
	}	

}


/* -----------------------------------------------------------------------------------------
��¼1: ARM926EJ-s core coprocessor registers


0. Cache type register 0  ����
   +--------------------------------------------------------------------------+
   | 31 30 29 | 28   25 | 24 | 23        12 | 11         					0 |
   | 0  0  0  | Ctype   | S  |    Dsize     |     Isize    					  |
   +--------------------------------------------------------------------------+ 
                                            +---------------------------------+
                                            | 11 10 | 9   6 | 5   3 | 2 | 1 0 |
                                            | 0  0  | Size  | Assoc | M | Len |
                                            +---------------------------------+   
1. Control register 1
   +-------------------------------------------------------------------------------------+
   | 31        19 | 18 17 16  | 15 | 14 | 13 | 12 | 11 10 | 9 | 8 | 7 | 6  3 | 2 | 1 | 0 |
   |    SBZ       | O  Z   O  | L4 | RR | V  | Z  |  SBZ  | R | S | B |  SBO | C | A | M |
   +-------------------------------------------------------------------------------------+  

2. Translation table base register 2
   +-----------------------------------------+
   | 31             14 | 13                0 |
   |      TTBR         |     UNP/SBZ         |
   +-----------------------------------------+  

3. Domain access control register 3

	There are 16 domains, each has 2 bit in this register, indicate the access permission
	2 BIT   meaning
	00  No access	Any access generates a domain fault
	01  Client		Accesses are checked against the access permission bits in the section or page descriptor
	10  Reserved	Reserved. Currently behaves like the no access mode
	11  Manager		Accesses are not checked against the access permission bits so a permission
					fault cannot be generated
   +-----------------------------------------------+
   | 31 30 | 29 28 | 27  26 | ............. | 1  0 |
   |  D15  |  D14  |  D13   | ............. |  D0  |  
   +-----------------------------------------------+  
 
5. Fault state register 5
   The FSR is updated for alignment faults, external abort that occur 
   while the MMU is disabled 
   +------------------------------------------------+
   | 31               9 | 8 | 7         4 | 3     0 |
   |     UNP/SBZ        | 0 |   Domain    |  Status |
   +------------------------------------------------+    

6. Fault address register 6
   The FAR is only updated for data aborts, not for prefetch abort. The FAR is updated 
   for alignment faults, external abort that occur while the MMU is disabled.
   
7. Cache operation register 7
   Reading from CP15 r7 is UNP, with the exception of two test and clean operation

8. Tlb operation register 8
   Write only register.    

9. Cache lockdown and TCM operation register 9                 
   +--------------------------------------------------------+
   | 31              16 | 15           4 | 3     		 0  |
   |     UNP/SBZ        |       SBO      |  L bits 			|
   +--------------------------------------------------------+   
   +--------------------------------------------------------+
   | 31              12 | 11           6 | 5     2 | 1 | 0  |
   |    Base addr(phy)  |     SBZ/UNP    |  Size   | 0 | en |
   +--------------------------------------------------------+

10. TLB lockdown register 10
   +-------------------------------------------------+
   | 31        29 | 28     26 | 25             1 | 0 |
   |     SBZ      |  Victim   |    UNP/SBZ       | P |
   +-------------------------------------------------+   

11. 12. Accessing these registers causes unpredicated behavior

13. Process ID register 13
   +-------------------------------------------------+
   | 31            25 | 24                        0  |
   |     FCSE PID     |            SBZ               |
   +-------------------------------------------------+    
   +-------------------------------------------------+
   | 31                                           0  |
   |            context identifier                   |
   +-------------------------------------------------+   

14. Accessing these register is reserved

* ------------------------------------------------------------------------------------------*/ 


/* ------------------------------------------------------------------------------------------- 
��¼2: ARM926EJ-s �� mmu table ��˵�� 
	ARM ��MMU ��Ԫ�ĵ�ַӳ���Ϊ���ࣺsection-mapped access �� page-mapped access. ���ڶ���
	section-mapped access��ӳ����ڴ��Сֻ��һ��Ϊ 1M��������page-mapped accesses ��3�� ��С��
	�ֱ�Ϊlarge pages  small pages tiny pages.

	mmu First-level descriptor, 4096 word, ÿһ��word ��Ӧ 1M �ĵ�ַ�ռ䣬
	��1MΪ��λ�������ַΪ�������������Ӧ�ı�����ݼ�Ϊӳ��������ַ�ͱ�ǵ���ϣ�Ҳ����˵��
	����ռ��0x0��ʼ����1MΪ��λ��ÿ�������ض�Ӧÿ������ռ��ӳ�������ռ䡣4096�������4G��
	ARM��ַ�ռ䡣ע��ARM Core �����ĵ�ַ����ָ�����ַ��
	
	���ǵ�һ����ӳ���
	����BIT[1:0]�������������section descriptor, ���ַת������������page descriptor��
	��Ҫ���еڶ�����ת��, �����������
	
	31				20 19           12 11 10 9 8 7 6 5 4 3 2 1 0
   +------------------+---------------+-----+-+-------+---------+
   |                                                        |0|0| ����
   +------------------------------------------------------------+
   |coarse page table base address          | |domain |1|   |0|1| coarse page table
   +------------------+---------------+-------------------------+
   |section Base addr |               | AP  | |domain |1|C|B|1|0| section 
   +------------------------------------------------------------+
   |fine page table base address      |       |domain |!|   |1|1| fine page table
   +------------------------------------------------------------+
	ע��Bit[1:0]	��ʾ�˵�һ��descriptor ������
					11	-- �������word�� fine page table descriptor����Ҫ�������ݵڶ����ı�ת��
					10 	-- �������word�� section descriptor,1M�Ĵ�С �����������ַת�����˽���
						   ǰ���λ�����������section ���������fine page ��coarse page������ͬ
					01	-- �������word�� coarse page table descriptor,��Ҫ�������ݵڶ����ı�ת��
					00	-- invalid
 
	
	���BIT[1:0] = 10b , ���ʾ��һ��Section descriptor,���������ַת�����˽�����һ�������ʹ��
	����section descriptor�϶࣬ͨ���Ͱ������һ���ı���� section descriptor ��
	ÿ�����������Ϊ  refer to ARM926EJ-s core manual: DDI0198C_926_trm.pdf������fine page��
	coarse page����Ϣ��Ҳ�ɲμ�����ĵ�
	Bit[31:20]  ӳ��������ַ�ĸ�12λ, form the corresponding bits of the physical address for a section
	Bit[19:12]  Ӧ��Ϊ0
	Bit[11:10]  access permission for this section, the interpreting depending on R S bits of control 
				register c1
				BIT[11:10]	R	S	Previleged permissions		User permissions
				00			0	0	No Access					No Access
				00			0   1   Read-only					No Access		
				00			1   0   Read-only					Read-only		
				00			1   1   Unpredictable				Unpredictable
				01			x	x	Read/Write					No Access
				10 			x   x	Read/Write					Read-only
				11			x	x	Read/Write					Read/Write
	Bit[9]  	Ӧ��Ϊ0
	Bit[8:5]	domain number 0~15 ��cp15��domain access register ������ÿ��domain����accessȨ��
				����16��domain�Ĵ�ȡȨ�޲�ͬ����������ζ�Ź���ĸ����ռ���16�ֿ��ܵĴ�ȡȨ��ѡ��
	Bit[4]		ARM core �涨������Ϊ1�� Ϊ������ǰ����
	Bit[3:2]	BIT3= Cachable; BIT2 = bufferable. 
					11	-- write-back cachable, 
					10	-- write-through cachable, 
					01	-- noncached buffered, 
					00	-- noncached nonbuffered
	Bit[1:0]	ӦΪ10 	-- �������word�� section descriptor,1M�Ĵ�С �����������ַת�����˽���
* ----------------------------------------------------------------------------------------------------*/
   
