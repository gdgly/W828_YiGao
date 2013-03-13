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

#ifndef __DS28CN01_H
#define __DS28CN01_H


//#include "std.h"
//#include "string.h"
#include "hyhwIIC.h"
#include "hyTypes.h"
#include "hyErrors.h"


#define DS28CN01_I2C_CHANNEL		0x1

#define E2PROM_Secret1 	0x1234567890123456
#define E2PROM_Secret2	0x2345678901234567
#define E2PROM_Secret3	0x3456789012345678
#define Wrong_Secret	0x3434564532408978

#define DS28CN01_SLAVE_ADDR      0xA0  
#define I2C_CLK_DELAY      		10

/* ����ΪDS28CN01�Ĵ����ĵ�ַ*/
#define DS28CN01_MEM_SERIAL					0xA0 /* оƬ��Bit64 ��Ψһ���к� ��rom address*/
#define DS28CN01_MEM_COMMUNICATE_MODE		0xA8 /* �õ�ͨѶģʽ  1��SM bus  0��IIC*/
#define DS28CN01_MEM_COMMAND				0xA9 /* д������ */
	#define DS28CN01_COMMAND_LOAD_FIRST_SECRET	0xA0	/* д��64bit�Ļ�����Կ*/
	#define ComputeNextSecret  0xB0
	#define ComputeFinalSecret 0xC0
	#define DS28CN01_COMMAND_COMPUTE_PAGE_MAC	0xD0
	#define ComputeAnonymouslyPageMAC 0xE0
#define DS28CN01_MEM_MAC					0xB0				

//define Command register address
#define Bit64RomAddress  0xA0
#define CommandAddress  0xA9
#define MAC_Address 0xB0




//define DS28CN01 Command mode



	//define DS28CN01 Command mode
#define LoadFirstSecret  0xA0
#define ComputeNextSecret  0xB0
#define ComputeFinalSecret 0xC0
#define ComputePageMAC  0xD0
#define ComputeAnonymouslyPageMAC 0xE0

  
typedef enum Key_status
{
 	KeyMatch,
 	KeyUnMatch
}eKEY_STATUS;

typedef enum Secret_type
{
 	SSecret = 0,
 	ESecret1,
 	ESecret2,
 	ESecret3
}eSECRET_TYPE; 

typedef struct tds28cn01_serialNum
{
 	U8 serialNum[8];
 	
}tDS28CN01_SERIAL_NUM;
  

extern tDS28CN01_SERIAL_NUM  ds28cn01SerialNum;  
  


/*-----------------------------------------------------------------------------
* ����:	 autDrv_ds28cn01Init
* ����:	 ��ʼ��ds2460������ȡ���к�
* ����:	 none
* ����:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01Init(void);

/*-----------------------------------------------------------------------------
* ����:	autDrv_ds28cn01_reset
* ����:	reset the ds28cn01
* ����:	none
* ����:	HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_reset( void );

/*-----------------------------------------------------------------------------
* ����:	autDrv_ds28cn01_readSerialNumber
* ����:	��28cn01 Ψһ�����к�
* ����:	none	
* ����:	HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_readSerialNumber( void );

/*-----------------------------------------------------------------------------
* ����:	 autDrv_ds28cn01_writeSN
* ����:	 д���Ʒ���к�
* ����:	 pbuffer-- SN Str�����16�ֽڳ���(���ַ���������)
* ����:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_writeSN(U8* pSerialNum);

/*-----------------------------------------------------------------------------
* ����:	 autDrv_ds28cn01_readSN
* ����:	 ��ȡ��Ʒ���к�
* ����:	 pbuffer-- SN Str������16�ֽڳ���(���ַ���������)
* ����:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_readSN(U8* pSerialNum);

#endif