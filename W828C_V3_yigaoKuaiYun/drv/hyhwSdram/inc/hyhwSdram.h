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

#ifndef HY_HW_SDRAM_H_
#define HY_HW_SDRAM_H_

// ʹ�õ�SDRAM���ͺţ��Ժ���Ҫ��������ϵͳ�������ļ���, ���ڷ���target��
//#define 	SD_4M32BIT
#define 	SD_8M32BIT
//#define 	SD_16M32BIT




/*-----------------------------------------------------------------------------
* ����:	hyhwSdram_Init
* ����:	��ʼ��EMI�п���SDRAM�ļĴ���, ��Ҫ��SDRAM�Ķ�дwait Cycle��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwSdram_init(void);
/*-----------------------------------------------------------------------------
* ����:	hyhwSdram_PowerDown
* ����:	SDRAM AUTO POWERDOWN Enable or disable
* ����:	EnableOff or EnableOn
* ����:	none
*----------------------------------------------------------------------------*/
//void hyhwSdram_powerDown(eENABLE enablePowerDown);
/*-----------------------------------------------------------------------------
* ����:	hySdram_Standby
* ����:	SDRAM����keepingģʽ����ʱ���ܶ�дSDRAM������SDRAM�е����ݱ��֣����������Ҫ����
*		��nandFlash��дʱ��������CE, �����SDRAM�����������ı�����SDRAM�е����ݿ��ܱ��ƻ�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwSdram_standby(void);
/*-----------------------------------------------------------------------------
* ����:	hySdram_Wakeup
* ����:	SDRAM�뿪keepingģʽ����hySdram_Standby���ʹ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwSdram_wakeup(void);
/*-----------------------------------------------------------------------------
* ����:	hySdram_powerDown
* ����:	SDRAM AUTO POWERDOWN Enable or disable
* ����:	EnableOff or EnableOn
* ����:	none
*----------------------------------------------------------------------------*/
//void hyhwSdram_powerDown(eENABLE enablePowerDown);
/*-----------------------------------------------------------------------------
* ����:	hySdram_autoPowerSave
* ����:	SDRAM AUTO POWER save
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hySdram_autoPowerSave( void );

/*-----------------------------------------------------------------------------
* ����:	hyhwScanSdramCapability
* ����:	ɨ��Sdram�Ĵ�С��ɨ��ÿ��2M bytes�����һ��short��
*		���ڼٶ�Sdramֻ���õ�2��4��8��16��32M bytes
*		ע�����ｫSdram����ʼ��ַ�̶�Ϊ0x20000000
*			�˺���������MMU disable��ʹ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwScanSdramCapability(void);
	

#endif //HY_HW_SDRAM_H_