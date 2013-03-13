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
#include "hyTypes.h"
#include "hyErrors.h"
#include "hyhwIIC.h"
#include "DS28CN01.h"
#include "hyhwCkc.h"

/*----------------------------------------------
|  e2prom ��ַ 0 ��ʼ    2011.11.16            | 
|  �������к�1  len = 16                       | 
|  �������к�2  len = 16 					   | 
|  ��������ʱ   len = 8						   | 
|  ��������     len = 8						   | 
|  ��������     len = 8  Askey- ����		   | 
|	'A','S','K','E','Y','S','Z','-',  		   |
------------------------------------------------*/

#define  DS28CN01_PRODUCT_INFO_LEN  (56) 

extern void hyhw_pmu_restore(void);

/*-----------------------------------------------------------------------------
* ����:	autDrv_ds28cn01_longDelay()
* ����:	�ϳ�ʱ�����ʱ�� // test on 399-133  
* ����:	delay	
* ����:	none
*------------------------------------------------------------------------------*/
extern tSYSCLKREGISTER_ST  SysClkRegister;
static void autDrv_ds28cn01_longDelay(U32 us)
{
   
	U32 clk;
	clk = (SysClkRegister.mCpuClk)*us;
	
	clk /= 3;
	while( clk --);
   
}

/*-----------------------------------------------------------------------------
* ����:	 autDrv_ds28cn01_e2promWrite
* ����:	 E2PROMд������
* ����:	  00H--7FH, E2PROM��Ϊ4page��ÿpage��32BYTE������128BYTE ,
*         Ŀǰֻ��д3��page,���ĸ�page ����ʹ��

*    	 offset -- д���ַ,������8�ı���
		 pbuffer-- data buffer  ע��: д�ĳ��Ȳ���8�ı�����ǿ��д�㣬ע��buf����8�ı���
		 len    -- data length   
* ����:	 HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_e2promWrite(U8 offset, U8* pbuffer, int len)
{	
	U32 addr ,rc = HY_ERROR;
	U8  i ,page,cnt,outputbuf[20],random[128];
	U8  *pIndex ;
	
	if(offset%8 != 0) return HY_ERROR ; /* ��ַ������8�ı��� */
	
	addr = offset + len ;
	page = addr/32 ;
	page = addr%32 == 0 ? page : page+1 ;
	
	if(page >= 3) return HY_ERROR ; /* ��4page ������ʹ�� */
	
	cnt = len/8 ;
	cnt = len%8 == 0 ? cnt : cnt+1 ;
	
	
	for(i = 0 ; i < cnt ; i++)
	{
		/* ���ڼ���mac Ҫʹ����֤������������֤�������ж�eeprom page�����Ա������ǰ����ã��������iic write ֮ǰ*/	
		addr   = offset + (i<<3) ;
		pIndex = pbuffer + (i<<3) ;
		
		memset(outputbuf ,0 ,20);
		outputbuf[0] = 0x4 ;
		outputbuf[1] = addr;
		memcpy(&outputbuf[2],pIndex,8);
		
		rc = hyAuthen_verify(1, random, outputbuf);
		if(HY_ERROR == rc)break ;
		
		/* ֪ͨds28cn01�� ����д�� ������оƬ����sha �㷨��Ȼ������ҲҪ����󣬲�������д�ɹ�*/
		rc = autDrv_ds28cn01_e2promWriteStart(addr,(char *)pIndex);
		if(HY_ERROR == rc)break ;
		/*  delay 3ms for DS28CN01 completing SHA-1 algorithm */
		autDrv_ds28cn01_longDelay(5000);
		
		/* ���������mac ��Ҫд��оƬ�У� д�ɹ��󣬱���д��eeprom ���ܳɹ� */
		rc = autDrv_ds28cn01_writeMAC(outputbuf,20);
		if(HY_ERROR == rc)break ;
		autDrv_ds28cn01_longDelay(10000);
	}
	
//	hyAuthen_reg(NULL, NULL);
//	hyAuthen_verify(1, random, NULL);
	
 	return rc;
}

/*-----------------------------------------------------------------------------
* ����:	 autDrv_ds28cn01_e2promRead1
* ����:	 E2PROM��������
* ����:	 00H--7FH, E2PROM��Ϊ4page��ÿpage��32BYTE������128BYTE�� �Ƿ���Բ���page���� ��Ҫȷ��
*    	 addr -- ��ȡ��ַ
		 pbuffer-- data buffer 
		 len    -- ��ȡ����
* ����:	 HY_OK /HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_e2promRead1(U8 addr, U8* pbuffer, int len)
{
	U32 i, j,raddr,pageI, offset,dert,cnt,rc;
	U8  readbuf[128],rw;
	
	pageI = addr/32 ;/* �ڼ���page��ʼ */
	offset= addr%32 ;
	dert  = len - (32 - offset);
	cnt   = 1 + dert/32 ;
	cnt   = dert%32 == 0 ? cnt : cnt+1 ; /* Ҫ����page˵ */
	rw    = 0 ;
	
	memset(readbuf,0,128);
	
	for(i = pageI ; (i < cnt && i < 4) ; i++)
	{
		raddr = i * 32 ;
		
		//rc = hyhwI2c_readData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, raddr, &readbuf[rw], 32);
		rc = HY_OK;
		for (j=0; j<32; j++)
	    {
	    	rc = hyhwI2c_readData( DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR, raddr++, &readbuf[rw+j], 1);
	    	if (rc != HY_OK) break;
	    }
		if(HY_ERROR == rc)return HY_ERROR ;
		
		rw += 32 ;		
	}
	
	if(len >= rw)
	{
		memcpy(pbuffer,&readbuf[offset],rw) ;
	}
	else
	{
		memcpy(pbuffer,&readbuf[offset],len) ;
	}
	
 	return  HY_OK ;
}


/*-----------------------------------------------------------------------------
* ����:	 autDrv_ds28cn01Init
* ����:	 ��ʼ��ds2460������ȡ���к�
* ����:	 none
* ����:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
int autDrv_ds28cn01_readProductSerialNumberInit(U8 *pbuffer)
{
	//autDrv_ds28cn01_e2promRead(0,pbuffer,16);
	U32 offset, rc = HY_OK;
	
	offset = 0;
	rc |= autDrv_ds28cn01_e2promRead(offset, (U8*)&pbuffer[0], 8);
	offset += 8;
	rc |= autDrv_ds28cn01_e2promRead(offset, (U8*)&pbuffer[8], 8);
	if (rc != HY_OK)
	{
		return HY_ERROR;
	}
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	 autDrv_ds28cn01Init
* ����:	 ��ʼ��ds2460������ȡ���к�
* ����:	 none
* ����:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01Init(void)
{
	U32 rc;
	//U8  ds2460SerialNumber[8];
	U8  productSerialNumber[16];
	
	
	hyAuthen_setChipAddress(DS28CN01_I2C_CHANNEL, DS28CN01_SLAVE_ADDR);
	rc = autDrv_ds28cn01_reset();
	if (rc != HY_OK)
	{//��ʼ������
		
		return rc;
	}
	
	autDrv_ds28cn01_readSerialNumber();
	autDrv_ds28cn01_readSN(&productSerialNumber[0]);
	hyAuthen_init(&ds28cn01SerialNum.serialNum[0], &productSerialNumber[0]);
	hyAuthen_regPostFunction(hyhw_pmu_restore);
	hyAuthen_verify(0 /*don't care*/, NULL, NULL);
	
	#if 0
	{
		char pRandomBuf[128];
		int ret ,i,m;
		Led_init();
		for(i = 0 ; i < 10 ; i++)
		{
			hyAuthen_reg(NULL, NULL);
			ret = hyosv_verify(1, &pRandomBuf[0], NULL);
			
			if(ret == HY_ERROR)
			{
				Led_init();Led_statusCtrl(1);for(m = 0 ; m < 0xFFFFFFF ; m++);
				
			}
			Led_statusCtrl(1);
			for(m = 0 ; m < 0xFFFFF ; m++);
			Led_statusCtrl(0);
		}		
	}
	#endif
		
	return rc;
}

/*-----------------------------------------------------------------------------
* ����:	 autDrv_ds28cn01_readProductInfo
* ����:	 ��ȡ��Ʒ����������Ϣ 
* ����:	 pSn1 -- ���к�1  ����16�ֽ� (����������)
*        pSn2 -- ���к�2  ����16�ֽ� (����������)
*        pTime-- ����ʱ�� ����16�ֽ� (����������)
*        pInfo-- �������� ����8�ֽ�  (����������)
* ����:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_readProductInfo(U8* pSn1,U8 *pSn2 ,U8 *pTime ,U8 *pInfo)
{
	U32 rc;
	U8  info[DS28CN01_PRODUCT_INFO_LEN] ;
	
	memset(info,0,DS28CN01_PRODUCT_INFO_LEN);
	
	rc = autDrv_ds28cn01_e2promRead1(0,info,DS28CN01_PRODUCT_INFO_LEN);
	
	if(HY_OK == rc)
	{	
		memcpy(pSn1,&info[0],9);
		memcpy(pSn2,&info[16],9);
		memcpy(pTime,&info[32],12);
		memcpy(pSn2,&info[48],8);
	}
	
	return rc;
}

/*-----------------------------------------------------------------------------
* ����:	 autDrv_ds28cn01_writeSN
* ����:	 д���Ʒ���к�
* ����:	 pbuffer-- SN Str�����16�ֽڳ���(���ַ���������)
* ����:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_writeSN(U8* pSerialNum)
{
	U32 len, rc  ;
	U8  wrbuf[16];
	
	len = strlen(pSerialNum);
	
	len = len >=16 ? 16 : len ;
	
	memset(wrbuf,0,16);
	memcpy(wrbuf,pSerialNum,len);
	
	rc = autDrv_ds28cn01_e2promWrite(0,wrbuf,16);
	hyhw_pmu_restore();
	
	return rc;
}


/*-----------------------------------------------------------------------------
* ����:	 autDrv_ds28cn01_readSN
* ����:	 ��ȡ��Ʒ���к�
* ����:	 pbuffer-- SN Str������16�ֽڳ���(���ַ���������)
* ����:	 HY_OK / HY_ERROR
*------------------------------------------------------------------------------*/
U32 autDrv_ds28cn01_readSN(U8* pSerialNum)
{
	U32 offset, rc = HY_OK;
	
	rc = autDrv_ds28cn01_e2promRead1(0,pSerialNum,16);
	
	return rc;
}

void testWriteRead(void)
{
	//U8 testStr[36] = "12345678901234567890123456789012";
	//U8 outbuf[36];
	//U32 rcc,rcc1;
	//memset(outbuf,0,36);
	
	//rcc = autDrv_ds28cn01_e2promWrite(0,testStr,32);
	//rcc1 = autDrv_ds28cn01_e2promRead1(0,outbuf,32);
}