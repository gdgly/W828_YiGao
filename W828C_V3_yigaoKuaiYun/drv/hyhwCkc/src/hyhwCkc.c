
/*----------------------------------------------------------------------------
   Standard include files:
--------------------------------------------------------------------------*/
#include "hyTypes.h"
#include "hyhwChip.h"
#include "hyErrors.h"
/*---------------------------------------------------------------------------
   Local Types and defines:
-----------------------------------------------------------------------------*/
#include "hyhwIIC.h"
#include "hyhwCkc.h"
#include "hyhwGpio.h"
#include "hyhwPmu.h"

#define	tca_wait()				{ }//volatile S32 i; for (i=0; i<0x10000; i++); }
tSYSCLKREGISTER_ST  SysClkRegister;

/* add begin
   added by yanglijing, 20090910, suggested by gaowenzhong
   for system clk changing stable 
ע:   200M ���ϣ���ЩλҪΪ1, �ڸ�λ״̬����Щλ�Ѿ�Ϊ1
*/   
#define ECFG0_REGISTER     TCC7901_ECFG0_REGISTER
    #define SDW_SUBCORE_DTCM_WAIT_BIT  BIT14
    #define IW_MAINCORE_ITCM_WAIT_BIT  BIT13
    #define DW_MAINCORE_DTCM_WAIT_BIT  BIT12
// add end

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_minSubCpu()
* ����:	set sub process clock divider Ϊ12 M ֱ���ṩ����������Ƶ
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_minSubCpu(void)
{
	CKC_CLKCTRL_REGISTER	&= ~0xf000;
	CKC_CLKCTRL_REGISTER	|= (1 << 12);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_closeSubCpu()
* ����:	close processor
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_closeSubCpu(void )
{
	CKC_BCLKCTR0_REGISTER	&= ~(IO_CKC_BUS_VideoC|IO_CKC_BUS_VideoH|IO_CKC_BUS_VideoP);
	CKC_BCLKCTR1_REGISTER	&= ~BIT5;	 					// Video Encoder
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_closeSubCpu()
* ����:	open sub processor
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCmp_openSubCpu(void)
{
	CKC_BCLKCTR0_REGISTER	|= (BIT19|BIT23|BIT25);
	CKC_BCLKCTR1_REGISTER	|= BIT5;
	
	CKC_SWRESET0_REGISTER	|= (BIT19|BIT23|BIT25);
	CKC_SWRESET1_REGISTER	|= BIT5;	
	
	CKC_SWRESET0_REGISTER	&= ~(BIT19|BIT23|BIT25);	
	CKC_SWRESET1_REGISTER	&= ~BIT5;
}


/* 
   TCC7901
   32768Hz      0.9V, ������ܴ�EXTERCLK ��Ƶ��Ӧ��ѡ�� CLK_32k
   <12M         1.1V
   12M~266M     1.2V
   266~332      1.3V
   332~399      1.4V
   399~498      1.55V
*/
/*------------------------------------------------------------------------------
* ����:	hyhwSysClk_getVoltage
* ����:	����cpu��busƵ�ʻ�ȡcpu��mem�ĵ�ѹĿ��ֵ
* ����:	cpuClk----cpuƵ����MHzΪ��λ----in
*		busClk----busƵ����MHzΪ��λ----in
*		pCpuVoltage---cpu��ѹ----out
*		pMemVoltage---mem��ѹ----out
* ����:	HY_OK		-- �ɹ�
*		HY_ERRORR	-- ��������
-----------------------------------------------------------------------------*/
int hyhwSysClk_getVoltage(U32 cpuClk, U32 busClk, 
						eACT8600_VOLTAGE *pCpuVoltage,
						eACT8600_VOLTAGE *pMemVoltage)
{
	if (pCpuVoltage==NULL || pMemVoltage==NULL) return HY_ERROR;
	
	if(cpuClk >= 399000)
	{
		*pCpuVoltage = ACT8600_1V6;
	}
	else if(cpuClk >= 332000)
	{
		*pCpuVoltage = ACT8600_1V4;
	}
	else if(cpuClk >= 266000)
	{
		*pCpuVoltage = ACT8600_1V3;
	}
	else
	{
		*pCpuVoltage = ACT8600_1V2;
	}

	if(busClk >= 133000)
	{
		// Sdram �����ٶȸ���133MHzʱ��LDO3�����ѹ��Ҫ������1.95V
		*pMemVoltage = ACT8600_1V95;
	}
	else
	{
		*pMemVoltage = ACT8600_1V8;
	}
	
	return HY_OK;
}

/*------------------------------------------------------------------------------
* ����:	hyhwSysClk_Set
* ����: ����ϵͳʱ��
*			PLL��ʱ����ԴΪӲ������
*				PLL1Ϊϵͳ�ṩʱ��
*			ICLK,HCLK,PCLK,MCLK��ʱ��ԴΪPLL
*			������CLK�������Ƕ����ģ�����Ϊ���ⲿ�����Ķ�д������
*			�ʱ���������¹��򣺣�������ܻ���ɶ�����ϵ��ʧ�ܣ�
*				1.ICLK������HCLK��������
*				2.HCLK������PCLK��������
*				3.HCLK������MCLK��һ��������
*			   
* ����: 
*		sysclkϵͳʱ�ӣ�ö��
    	cfgExtal        �ⲿʱ�� EXTAL_CLK 
    	����hyhwSysClk_Set(ICLK266M_AHB133M,EXTAL_CLK)
    	
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
-----------------------------------------------------------------------------*/
U32 hyhwSysClk_Set(eSYS_Clk_EN sysclk, U32 cfgExtal)
{
	U8 interrupt;
	U32 regBuff,pllOut;
	U32 hdiv;
	U32 mCpuClk, fSysClk, busClk;
	U32 cpuVoltageFlag, memVoltageFlag;		//0---�������ѹ��1---�ȵ�����ѹ��2---������
	eACT8600_VOLTAGE cpuVoltage=ACT8600_1V4, memVoltage=ACT8600_1V8;
	eACT8600_VOLTAGE cpuVoltage_last=ACT8600_1V4, memVoltage_last=ACT8600_1V8;
	U32 rc = HY_OK;
	
	pllOut = PLLFREQ((sysclk>>24),((sysclk>>16)&0xff),((sysclk>>8)&0xff));
	
	mCpuClk = pllOut/1000;
	fSysClk = pllOut/(sysclk&0xff)/1000;
	
	hdiv   = ((sysclk&0xff)-1);
	busClk = (pllOut/(hdiv+1));
	
	/*	����Ƶ�ʵ����͵�ѹ���ù�ϵ��Ŀ���Ǿ������ٵ�ѹ��������(IICͨѶ�ٶȽϵ�)
		��Ƶ���ȵ�����ѹ��Ŀ��ֵ���ٵ���Ƶ��
		��Ƶ���ȵ���Ƶ�ʣ��ٵ�����ѹ��Ŀ��ֵ
		ע��
		�ڽ���͹���ʱ��Ƶ�ʵ����ڵ͹��ĺ�������
		��SysClkRegister.mCpuClk��ֵΪ0(32kHz),��ʾ<1MHz
		�������˳��͹��ĵ���Ƶ��ʱ�������ȵ�����ѹ
	*/
	
	//����core��ѹĿ��ֵ
	hyhwSysClk_getVoltage(mCpuClk, busClk, &cpuVoltage, &memVoltage);
	hyhwSysClk_getVoltage(SysClkRegister.mCpuClk, SysClkRegister.busClk,
						&cpuVoltage_last, &memVoltage_last);
	//�ж��ȵ�����ѹ���Ǻ������ѹ
#if 1
	//CPU
	cpuVoltageFlag = 2;	//Ĭ��Ϊ��������ѹ
	if (cpuVoltage > cpuVoltage_last)
	{
		//CPU �ȵ�����ѹ
		cpuVoltageFlag = 1;
		act8600_output_setVoltage(ACT8600_OUT1, cpuVoltage);
	}
	else if (cpuVoltage < cpuVoltage_last)
	{
		//CPU �������ѹ
		cpuVoltageFlag = 0;
	}
	
	//MEM
	memVoltageFlag = 2;	//Ĭ��Ϊ��������ѹ
	if (memVoltage > memVoltage_last)
	{
		//MEM �ȵ�����ѹ
		memVoltageFlag = 1;
		act8600_output_setVoltage(ACT8600_OUT3, memVoltage);
	}
	else if (memVoltage < memVoltage_last)
	{
		//MEM �������ѹ
		memVoltageFlag = 0;
	}
	
#else
	if (mCpuClk > SysClkRegister.mCpuClk)
	{
		//CPU Ŀ��Ƶ�ʱȵ�ǰƵ�ʸߣ��ȵ�����ѹ
		cpuVoltageFlag = 1;
		act8600_output_setVoltage(ACT8600_OUT1, cpuVoltage);
	}
	
	memVoltageFlag = 0;	//Ĭ��Ϊ�������ѹ
	if ((busClk>=133000 && SysClkRegister.busClk<133000) ||
		(busClk<133000 && SysClkRegister.busClk>=133000))
	{
		//��Ҫ���� MEM ��ѹ
		if (busClk > SysClkRegister.busClk)
		{
			//MEM Ŀ��Ƶ�ʱȵ�ǰƵ�ʸߣ��ȵ�����ѹ
			memVoltageFlag = 1;
			act8600_output_setVoltage(ACT8600_OUT3, memVoltage);
		}
	}
	else
	{
		//����Ҫ���� MEM ��ѹ
		memVoltageFlag = 2;
	}
#endif

	SysClkRegister.mCpuClk = mCpuClk;		//��MHzΪ��λ
	SysClkRegister.fSysClk = fSysClk;		//��MHzΪ��λ
	SysClkRegister.busClk = busClk;			//��MHzΪ��λ
	
/* ������Ƶ����ֹ�ж� */
	interrupt = InterruptDisable();
	hyhwNand_setIoSpeed(fSysClk);
	hyhwLcd_setBusTime(fSysClk);

	if(pllOut > EXTAL_CLK)	// multiply the clk	
	{
		if(pllOut >= 200000)
		{
			ECFG0_REGISTER |= ( SDW_SUBCORE_DTCM_WAIT_BIT
							   |IW_MAINCORE_ITCM_WAIT_BIT
		   					   |DW_MAINCORE_DTCM_WAIT_BIT
		   					  );
		}
		
		/* ����Ƶ��֮ǰ�Ȱ�Ƶ�����õ�12M CLK*/
		regBuff  = CKC_CLKCTRL_REGISTER;
		regBuff &= ~(MCPUCLK_BIT_DIV_MASK   				/* clear clk divider / bus divider */ 
					|BUSCLK_BIT_DIV_MASK
					|SYSCLK_BIT_SOU_MASK
					);
		regBuff |= ((CLK_12M << 0)							/* set cpu clk and bus clk divider,selection*/
				   |(15 <<MCPUCLK_BIT_DIV)					/*MCUP CLK == FCLK; BUS CLK  == FCLK/2*/
				   |(1  <<BUSCLK_BIT_DIV)
				   );
		CKC_CLKCTRL_REGISTER  = regBuff;

		rc =hyhwCpm_setPll0(sysclk,cfgExtal);

		if(rc == HY_OK)
		{
			regBuff  = CKC_CLKCTRL_REGISTER;
			regBuff &= ~(MCPUCLK_BIT_DIV_MASK   			/* clear clk divider / bus divider */ 
						|BUSCLK_BIT_DIV_MASK
						|SYSCLK_BIT_SOU_MASK
						);
			regBuff |= ((PLL0_CLK << 0)						/* set cpu clk and bus clk divider,selection*/
					   |(15 <<MCPUCLK_BIT_DIV)
					   |((hdiv)<< BUSCLK_BIT_DIV)
					   );		   
			CKC_CLKCTRL_REGISTER  = regBuff;
		}
	}
	else if(pllOut == EXTAL_CLK) //neither divide nor multiply
	{
		regBuff  = CKC_CLKCTRL_REGISTER;
		regBuff &= ~(MCPUCLK_BIT_DIV_MASK   				/* clear clk divider / bus divider */ 
					|BUSCLK_BIT_DIV_MASK
					|SYSCLK_BIT_SOU_MASK
					);
		regBuff |= ((CLK_12M << 0)							/* set cpu clk and bus clk divider,selection*/
				   |(15 <<MCPUCLK_BIT_DIV)
				   |((hdiv)<<BUSCLK_BIT_DIV)
				   );
		CKC_CLKCTRL_REGISTER  = regBuff;
	}
	else	//divide clk
	{
		regBuff  = CKC_CLKDIVC_REGISTER;
		regBuff &= ~(XIN_BIT_DIV_MASK );  					/* clear clk divider / bus divider */ 
		
		
		regBuff |= (XIN_BIT_DIV_ENABLE						/* set cpu clk and bus clk divider,selection*/
				   |(((EXTAL_CLK / pllOut)-1)<<XIN_BIT_DIV)
				   );
		CKC_CLKDIVC_REGISTER  = regBuff;	
		
		regBuff  = CKC_CLKCTRL_REGISTER;
		regBuff &= ~(MCPUCLK_BIT_DIV_MASK   				/* clear clk divider / bus divider */ 
					|BUSCLK_BIT_DIV_MASK
					|SYSCLK_BIT_SOU_MASK
					);
		regBuff |= ((DIV_12M_CLK << 0)						/* set cpu clk and bus clk divider,selection*/
				   |(15 <<MCPUCLK_BIT_DIV)
				   |((hdiv)<<BUSCLK_BIT_DIV)
				   );
		CKC_CLKCTRL_REGISTER  |= regBuff;				   
   		CKC_PLL0CFG_REGISTER |= PLL0_BIT_DISABLE;     		/* disable PLL */  
	}
/* ��ɵ������ָ��ж� */
	InterruptRestore(interrupt);
	
	if (cpuVoltageFlag == 0)
	{
		//CPU Ŀ��Ƶ�ʱȵ�ǰƵ�ʵͣ��������ѹ
		act8600_output_setVoltage(ACT8600_OUT1, cpuVoltage);
	}
	if (memVoltageFlag == 0)
	{
		//MEM Ŀ��Ƶ�ʱȵ�ǰƵ�ʵͣ��������ѹ
		act8600_output_setVoltage(ACT8600_OUT3, memVoltage);
	}
	
	return rc;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwSysClk_GetCurValue
* ����:	�õ���ǰϵͳ��Ƶ��
* ����:	none
* ����: ϵͳƵ��
*----------------------------------------------------------------------------*/
U32 hyhwSysClk_GetCurValue(void)
{
     return SysClkRegister.mCpuClk;
}

/*-----------------------------------------------------------------------------
* ����:	void hyhwSystemPower_stepInit(void)
* ����:	��ϵͳ��Դ�ȼ����Ƴ�ʼ��(Ĭ�ϵ�ѹ��1.2V)
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwSystemPower_stepInit(void)
{
/*δ�ҵ���ӦIO��
	hyhwGpio_setAsGpio(PORT_B,BIT9);
	hyhwGpio_setAsGpio(PORT_C,BIT3);
	hyhwGpio_setOut(PORT_B,BIT9);
	hyhwGpio_setOut(PORT_C,BIT3);
*/
}

/*----------------------------------------------------------------------------
* ����:	hyhwSubSysClk_Set()
* ����:	set sub processor's clock
* ����:	sCpuClk------sub process's clock value 
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwSubSysClk_Set(U32 sCpuClk)
{	//����scpu clk��mcpu
	U32 scupDiv;
	SysClkRegister.sCpuClk 	 = sCpuClk;
	scupDiv = ((SysClkRegister.sCpuClk - SysClkRegister.busClk)<<4)/(SysClkRegister.fSysClk - SysClkRegister.busClk)-1;
	
	CKC_CLKCTRL_REGISTER	&= ~0xf000;
	CKC_CLKCTRL_REGISTER	|= (scupDiv << 12);

}

/*----------------------------------------------------------------------------
* ����:	tca_ckc_getsystemsource()
* ����:	��ȡʱ��Դ
* ����:	none
* ����:	����ʱ��ѡ���������ɾݴ���ֵ��ѯ��Ӧʱ��Դ
*----------------------------------------------------------------------------*/
S32 tca_ckc_getsystemsource(void)
{
	return (CKC_CLKCTRL_REGISTER & 0x7);
}

/*----------------------------------------------------------------------------
* ����:	tca_ckc_setsystemsource()
* ����:	����ʱ��Դ
* ����:	ClockSource--ʱ��Դ��ţ��ݴ���ſɲ��Ӧʱ��Դ
* ����:	none
*----------------------------------------------------------------------------*/
void tca_ckc_setsystemsource( U32 ClockSource )
{
	CKC_CLKCTRL_REGISTER	&= ~0x7;
	CKC_CLKCTRL_REGISTER	|= ClockSource;
}

/*----------------------------------------------------------------------------
* ����:	tca_ckc_setpll()
* ����:	����PLL��ͨ�ú���
* ����:	P--PLL ��Pֵ
*		M--PLL ��Mֵ
*		S--PLL ��Sֵ
*		uCH--PLL��� 0 ����1
* ����:	none
*----------------------------------------------------------------------------*/
void tca_ckc_setpll(U8 P, U8 M, U8 S, U32 uCH)
{
    volatile U32 *pPLLCFG;
	if(uCH)
	{
		pPLLCFG		 = (&CKC_PLL1CFG_REGISTER);	
	}
	else
	{
		pPLLCFG 	 = (&CKC_PLL0CFG_REGISTER);	
	}

	//Change PLL Clock
	//Disable PLL 
	*pPLLCFG			|= PLL1CFG_PD_DIS;
    //Set P
    *pPLLCFG			&= ~0x3f;
    *pPLLCFG			|= P;
	//Set M
	*pPLLCFG			&= ~0xff00;
    *pPLLCFG			|= (M << 8);
	//Set S
	*pPLLCFG			&= ~0x70000;
    *pPLLCFG			|= (S << 16);
    //Enable PLL
    *pPLLCFG			&= ~PLL1CFG_PD_DIS;
	
	tca_wait();	
}

/*---------------------------------------------------------------------------
* ����:	hyhwCpm_setPll0
* ����:	set PLL output frequency 
* ����:	pllclk	-- PLL 0 ���Ƶ�� 
*		cfgExtal-- �ⲿʱ��
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- �������� 
*----------------------------------------------------------------------------*/
U32 hyhwCpm_setPll0(U32 pllclk, U32 cfgExtal)
{
	tca_ckc_setpll((pllclk>>24),((pllclk>>16)&0xff),((pllclk>>8)&0xff),0);
	return HY_OK;
	
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setPll1
* ����:	set PLL output frequency 
* ����:	pllclk	-- PLL 1 ���Ƶ�� 
*		cfgExtal-- �ⲿʱ��
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- �������� 
*----------------------------------------------------------------------------*/
U32 hyhwCpm_setPll1(ePLL_CLK_EN pllclk, U32 cfgExtal)
{
	tca_ckc_setpll(((pllclk>>16)&0xff),((pllclk>>8)&0xff),((pllclk)&0xff),1);
	return HY_OK;
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getPllOut0
* ����:	get PLL output frequency 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getPllOut0(void)
{
	U32 m, p, s, pllout0;
		
	pllout0 = CKC_PLL0CFG_REGISTER;
	m 		= (pllout0&PLL0_BIT_MDIV_MASK)>>PLL0_BIT_MDIV;
	p	 	= (pllout0&PLL0_BIT_PDIV_MASK)>>PLL0_BIT_PDIV;
	s 		= (pllout0&PLL0_BIT_SDIV_MASK)>>PLL0_BIT_SDIV;
	
	pllout0 = ((EXTAL_CLK * m) / (p * (2<<(s))))<<1;

	return pllout0;
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getPllOut1
* ����:	get PLL output frequency 
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getPllOut1(void)
{
	U32 m, p, s, pllout1;
		
	pllout1 = CKC_PLL1CFG_REGISTER;
	m 		= (pllout1&PLL0_BIT_MDIV_MASK)>>PLL0_BIT_MDIV;
	p	 	= (pllout1&PLL0_BIT_PDIV_MASK)>>PLL0_BIT_PDIV;
	s 		= (pllout1&PLL0_BIT_SDIV_MASK)>>PLL0_BIT_SDIV;
	
	pllout1 = ((EXTAL_CLK * m) / (p * (2<<(s))))<<1;

	return pllout1;
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setUSB11HClk
* ����:	����USB 1.1 Host ʱ�� 20100427
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- �������� 
*----------------------------------------------------------------------------*/
void hyhwCpm_setUSB11HClk(void)
{	
	U32 regBuff,divValue,pllOut;						//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue

	pllOut 	 = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / USB11H_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_USBH);

	regBuff  = CKC_PCK_USB11H_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_USB11H_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_USBH);
	
	return ;
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setSdmmcClk
* ����:	����SD\MMC ʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setSdmmcClk(void)
{	
	U32 regBuff, divValue;								//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	
	divValue = ((EXTAL_CLK / SDMMC_CLK)) - 1;			//��Ƶ������
	hyhwCpm_setSoftReset(CKC_SDMMC);
	
	regBuff  = CKC_PCK_SDMMC_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_SDMMC_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_SDMMC);

}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setMstickClk
* ����:	����Memory stick ʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setMstickClk(void)
{	
	U32 regBuff, divValue;								//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue

	divValue = ((EXTAL_CLK / MSTICK_CLK)) - 1;			//��Ƶ������
	hyhwCpm_setSoftReset(CKC_MSTICK);
	
	regBuff  = CKC_PCK_MSTICK_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
				
	CKC_PCK_MSTICK_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_MSTICK);

}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setI2cClk
* ����:	����I2Cʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setI2cClk(void)
{
	U32 regBuff,divValue;								//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	
	divValue = ((EXTAL_CLK / I2C_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_I2C);

	regBuff  = CKC_PCK_I2C_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_I2C_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_I2C);

}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_enableTimer
* ����:	ʹ�ܶ�ʱ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_enableTimer(void)
{
	hyhwCpm_clearSoftReset(CKC_TIMER);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_disableTimer
* ����:	�رն�ʱ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_disableTimer(void)
{
	hyhwCpm_setSoftReset(CKC_TIMER);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setTimer32Clk
* ����:	���ö�ʱ��Ƶ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setTimer32Clk(void)
{
	U32 regBuff,divValue;								//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	
	divValue = ((EXTAL_CLK / TCZ_CLK)) - 1;
//	hyhwCpm_setSoftReset(CKC_TIMER);	�������Ƿ��ʺ����ڵ�ϵͳ

	regBuff  = CKC_PCK_TCZ_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_I2C_REGISTER  = regBuff;
//	hyhwCpm_clearSoftReset(CKC_TIMER);

}
/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setTcxClk
* ����:	����Tcxʱ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setTcxClk(void)
{
	U32 regBuff,divValue;		//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	divValue = ((EXTAL_CLK / TCX_CLK)) - 1;
//	hyhwCpm_setSoftReset(CKC_TIMER);	�������Ƿ��ʺ����ڵ�ϵͳ

	regBuff  = CKC_PCK_TCX_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_TCX_REGISTER  = regBuff;
//	hyhwCpm_clearSoftReset(CKC_TIMER);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_SetTdClk32K
* ����:	����Tcxʱ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_SetTdClk32K(void)
{
     // ����TCX���32.768ʱ��, ����TDģ��
     /* ��ʼ��ģ��ʹ��32.768����, ���þ���������, ��˸Ĵ�cpuֱ�����*/
     // added by yanglijing, 20090909
	U32 regBuff,divValue;		//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue

	hyhwGpio_setTcxClkOut0Function();
	regBuff  = CKC_PCK_TCX_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_32k << 24			
				|0					
				);
	
	CKC_PCK_TCX_REGISTER  = regBuff;
//	hyhwCpm_clearSoftReset(CKC_TIMER);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setTctClk
* ����:	����Tctʱ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setTctClk(void)
{
	U32 regBuff,divValue;								//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	
	divValue = ((EXTAL_CLK / TCT_CLK)) - 1;
//	hyhwCpm_setSoftReset(CKC_TIMER);	�������Ƿ��ʺ����ڵ�ϵͳ

	regBuff  = CKC_PCK_TCT_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_TCT_REGISTER  = regBuff;
//	hyhwCpm_clearSoftReset(CKC_TIMER);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setLcdClk
* ����:	����LCDʱ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setLcdClk(void)
{
	U32 regBuff,divValue,pllOut;						//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	pllOut = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / LCD_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_LCD);

	regBuff  = CKC_PCK_LCD_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_LCD_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_LCD);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setCifClk
* ����:	����CIF PXclockʱ�� //camera pixel clock
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setCifClk(U32 cifClk)
{
	U32 regBuff,divValue,pllOut;						//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue

	pllOut 	 = hyhwCpm_getPllOut1();
	divValue = ((pllOut / cifClk)) - 1;
	hyhwCpm_setSoftReset(CKC_CAM);

	regBuff  = CKC_PCK_CAM_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_CAM_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_CAM);
	
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setScalerClk
* ����:	����ScalerClk ʱ�� //camera pixel clock
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setScalerClk(U32 scalerClk)
{
	U32 regBuff,divValue,pllOut;						//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue

	pllOut 	 = hyhwCpm_getPllOut1();
	divValue = ((pllOut / scalerClk)) - 1;
	//hyhwCpm_setSoftReset(CKC_MTM);

	regBuff  = CKC_PCK_SCALER_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_SCALER_REGISTER  = regBuff;
	//hyhwCpm_clearSoftReset(CKC_MTM);
	
}

U32 hyhwCpm_setUartRegister(U32 UartId, U32 divValue)
{
	volatile U32 *pUart_CkcPckRegister;
	U32 regBuff;
	
	if (UartId > 3) return HY_ERR_BAD_PARAMETER;
	
	pUart_CkcPckRegister = (volatile U32 *)(TCC7901_CKC_BASEADDR + 0x98 + UartId*4);
	regBuff  = *pUart_CkcPckRegister;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24
				|divValue
				);
	
	*pUart_CkcPckRegister  = regBuff;
	
	return HY_OK;
}
/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setUartClk
* ����:	set uart clk 
* ����:	uartclk -- uart�豸��Ҫ��ʱ�� ��λ��MHZ 
*				UnitId  -- ���豸�ţ�spi Ϊ(0 \ 1 \ 2 \ 3)
* NOTE: ���� UART CLK ���ϵͳʱ�Ӳ��������� ��ôuart clk ����ʧ�ܡ�����ı�ϵͳʱ�� 
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
U32 hyhwCpm_setUartClk(U32 UartId, ePERI_CLK uartClk)
{
	U32 regBuff,pllclk,divValue;
	UInt32 rc = HY_OK;
	
	//if((UartId > 3)||(uartClk > UART_CLK_MAX))
	if (UartId > 3)
	{
		rc = HY_ERR_BAD_PARAMETER;
	}
	else
	{
		pllclk = hyhwCpm_getPllOut1();
		if((pllclk % (uartClk)) == 0)
		{
			divValue = ((pllclk / uartClk)) - 1;
			
			switch (UartId)
			{
				case 0:
					hyhwCpm_setSoftReset(CKC_UART0);
					hyhwCpm_setUartRegister(UartId, divValue);
					hyhwCpm_clearSoftReset(CKC_UART0);
					break;
				case 1:
					hyhwCpm_setSoftReset(CKC_UART1);
					hyhwCpm_setUartRegister(UartId, divValue);
					hyhwCpm_clearSoftReset(CKC_UART1);
					break;
				case 2:
					hyhwCpm_setSoftReset(CKC_UART2);
					hyhwCpm_setUartRegister(UartId, divValue);
					hyhwCpm_clearSoftReset(CKC_UART2);
					break;
				case 3:
					hyhwCpm_setSoftReset(CKC_UART3);
					hyhwCpm_setUartRegister(UartId, divValue);
					hyhwCpm_clearSoftReset(CKC_UART3);
					break;
				default:
					break;
			}
		}
	}
	
	return rc;
}
/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setUart0Clk
* ����:	����UART0ʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setUart0Clk(void)
{
	U32 regBuff,divValue,pllOut;						//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue

	pllOut 	 = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / UART0_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_UART0);

	regBuff  = CKC_PCK_UART0_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_UART0_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_UART0);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setUart1Clk
* ����:	����UART1ʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setUart1Clk(void)
{
	U32 regBuff,divValue,pllOut;						//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	pllOut = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / UART1_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_UART1);

	regBuff  = CKC_PCK_UART1_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_UART1_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_UART1);
	
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setUart2Clk
* ����:	����UART2ʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setUart2Clk(void)
{
	U32 regBuff,divValue,pllOut;						//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	pllOut 	 = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / UART2_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_UART2);

	regBuff  = CKC_PCK_UART2_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_UART2_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_UART2);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setUart3Clk
* ����:	����UART3ʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setUart3Clk(void)
{
	U32 regBuff,divValue,pllOut;						//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	
	pllOut = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / UART3_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_UART3);

	regBuff  = CKC_PCK_UART3_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_UART3_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_UART3);
}
/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setDaiClk
* ����:	����DAI Controllerʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setMmcClk(ePERI_CLK clk)
{
	U32 regBuff,divValue,pllOut;						//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	
	pllOut = hyhwCpm_getPllOut1();
	divValue = ((pllOut / clk)) - 1;
	hyhwCpm_setSoftReset(CKC_SDMMC);

	regBuff  = CKC_PCK_SDMMC_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_SDMMC_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_SDMMC);
}
/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setDaiClk
* ����:	����DAI Controllerʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setDaiClk(U32 daiClk)
{
	U32 regBuff,divValue,pllOut;						//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	
	pllOut = hyhwCpm_getPllOut1()*1000;
	divValue = ((pllOut / daiClk)) - 1;
	hyhwCpm_setSoftReset(CKC_DAI);

	regBuff  = CKC_PCK_DAI_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (DAI_DIV_SELECT
				|MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_DAI_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_DAI);
}
/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setDaiDcoClk
* ����:	����DAI clk dco ģʽ��Ƶ�ʻ�׼ȷһЩ
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setDaiDcoClk(U32 daiClk)
{
	U32 regBuff,divValue,pllOut;						//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	U32 pllclk;
	U64 clk;
	
	
	
	pllOut = hyhwCpm_getPllOut1()*1000;
	clk = daiClk;
	clk = (clk *65536)/pllOut;
	
	divValue = 65536 - clk;
	hyhwCpm_setSoftReset(CKC_DAI);

	regBuff  = CKC_PCK_DAI_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_DAI_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_DAI);
}
/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setGpsb0Clk
* ����:	����GPSB0ʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setGpsb0Clk(void)
{
	U32 regBuff,divValue,pllOut;						//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	pllOut = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / (GPSB0_CLK<<1))) - 1;
	hyhwCpm_setSoftReset(CKC_GPSB0);

	regBuff  = CKC_PCK_GPSB0_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_GPSB0_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_GPSB0);
	
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setGpsb1Clk
* ����:	����GPSB1ʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setGpsb1Clk(void)
{
	U32 regBuff,divValue,pllOut;						//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	pllOut = hyhwCpm_getPllOut1();
	divValue = ((pllOut / (GPSB1_CLK<<1))) - 1;
	hyhwCpm_setSoftReset(CKC_GPSB1);
	
	
	regBuff  = CKC_PCK_GPSB1_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_GPSB1_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_GPSB1);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setGpsb2Clk
* ����:	����GPSB2ʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setGpsb2Clk(void)
{

	U32 regBuff,divValue,pllOut;						//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	pllOut = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / (GPSB2_CLK<<1))) - 1;
	hyhwCpm_setSoftReset(CKC_GPSB2);

	regBuff  = CKC_PCK_GPSB2_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_GPSB2_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_GPSB2);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setGpsb3Clk
* ����:	����GPSB3ʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setGpsb3Clk(void)
{
	
	U32 regBuff,divValue,pllOut;						//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	pllOut = hyhwCpm_getPllOut1();
	
	divValue = ((pllOut / (GPSB3_CLK<<1))) - 1;
	hyhwCpm_setSoftReset(CKC_GPSB3);

	regBuff  = CKC_PCK_GPSB3_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|PLL1_CLK << 24			
				|divValue					
				);
	
	CKC_PCK_GPSB3_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_GPSB3);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setAdcClk
* ����:	����Adcʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setAdcClk(void)
{
	U32 regBuff,divValue;								//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue

	divValue = ((EXTAL_CLK / ADC_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_ADC);

	regBuff  = CKC_PCK_ADC_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_ADC_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_ADC);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setSpdifClk
* ����:	���� S/PDIF Tx Controllerʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setSpdifClk(void)
{
	U32 regBuff,divValue;								//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	
	divValue = ((EXTAL_CLK / SPDIF_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_SPDIF);

	regBuff  = CKC_PCK_SPDIF_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_SPDIF_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_SPDIF);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setEhi0Clk
* ����:	���� EHI 0 controllerʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setEhi0Clk(void)
{
	U32 regBuff,divValue;								//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	divValue = ((EXTAL_CLK / EHI0_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_EHI0);

	regBuff  = CKC_PCK_EHI0_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_EHI0_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_EHI0);
	
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setEhi1Clk
* ����:	���� EHI 1 controllerʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setEhi1Clk(void)
{
	U32 regBuff,divValue;								//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	divValue = ((EXTAL_CLK / EHI1_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_EHI1);

	regBuff  = CKC_PCK_EHI1_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M << 24			
				|divValue					
				);
	
	CKC_PCK_EHI1_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_EHI1);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setSdramRefreshClk
* ����:	���� SDRAM ˢ��ʱ��
* ����:	none
* ����:	HY_OK					-- �ɹ�
*		HY_ERR_BAD_PARAMETER	-- ��������
*----------------------------------------------------------------------------*/
void hyhwCpm_setSdramRefreshClk(void)
{
	U32 regBuff,divValue;								//�Ĵ���ֵ�洢��ʱ����regBuff, ��Ƶֵ����divValue
	
	divValue = ((EXTAL_CLK / RFREQ_CLK)) - 1;
	hyhwCpm_setSoftReset(CKC_EMC);

	regBuff  = CKC_PCK_EHI0_REGISTER;
	regBuff &= ~(MODEL_BIT_ENABLE						//ʱ��ʹ��λ����
				|MODEL_BIT_SOURCE_MASK					//ʱ��Դѡ��λ
				|MODEL_BIT_DIV_MASK						//��Ƶ��Ƶ��λ
				);
				
	regBuff |= (MODEL_BIT_ENABLE		
				|CLK_12M		//|( DIV_PLL0_DIV0_CLK<< 24	)	// 			
				|divValue					
				);
	
	CKC_PCK_RFREQ_REGISTER  = regBuff;
	hyhwCpm_clearSoftReset(CKC_EMC);
	
}


/*----------------------------------------------------------------------------
* ����:	hyhwCpm_busClkEnable
* ����:	����Χ�豸ʱ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_busClkEnable(eMODEL_ID UnitId)
{
	if(UnitId > 31)
	{
		UnitId = UnitId - 32;
		CKC_BCLKCTR1_REGISTER |= (1 << UnitId);
	}
	else
	{
		CKC_BCLKCTR0_REGISTER |= (1 << UnitId);
	} 	
}
/*----------------------------------------------------------------------------
* ����:	hyhwCpm_busClkDisable
* ����:	�ر���Χ�豸ʱ��
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_busClkDisable(eMODEL_ID UnitId)
{
	if(UnitId > 31)
	{
		UnitId = UnitId - 32;
		CKC_BCLKCTR1_REGISTER &= ~(1 << UnitId);
	}
	else
	{
		CKC_BCLKCTR0_REGISTER &= ~(1 << UnitId);
	} 
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_setSoftReset
* ����:	�����λ��Χ�豸
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_setSoftReset(eMODEL_ID UnitId)
{
	if(UnitId > 31)
	{
		UnitId = UnitId - 32;
		CKC_SWRESET1_REGISTER |= (1 << UnitId);
	}
	else
	{
		CKC_SWRESET0_REGISTER |= (1 << UnitId);
	} 
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_clearSoftReset
* ����:	�����Χ�豸��λ
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwCpm_clearSoftReset(eMODEL_ID UnitId)
{
	if(UnitId > 31)
	{
		UnitId = UnitId - 32;
		CKC_SWRESET1_REGISTER &= ~(1 << UnitId);
	}
	else
	{
		CKC_SWRESET0_REGISTER &= ~(1 << UnitId);
	} 
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getSysClkSourceo
* ����:	get sysClk Sourceo  
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/

U32 hyhwCpm_getSysClkSourceo(void)
{

	return(CKC_CLKCTRL_REGISTER & SYSCLK_BIT_SOU_MASK);
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getClk
* ����:	get clock 
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/

U32 hyhwCpm_getClk(U32 div,U32 clkSour)
{


	if(clkSour == PLL0_CLK)							  		/*  ѡ�� PLL0 */
	{
		if((CKC_PLL0CFG_REGISTER & PLL0_BIT_DISABLE)==0) 	/* �ж� PLL0 �Ƿ�ENABLE */
		{
			return hyhwCpm_getPllOut0() / div;
		}
		else 
		{
			return 0;	          						 	/*  note : ѡ��PLL�������û��enable ��PLLOUT �Ƕ��٣��� */
		}
	}
	else if(clkSour == PLL1_CLK)							/*  ѡ�� PLL1  */
	{
		if((CKC_PLL1CFG_REGISTER & PLL1_BIT_DISABLE)==0)
		{
			return hyhwCpm_getPllOut1() / div;
		}
		else 
		{
			return 0;										/*  note : ѡ��PLL�������û��enable ��PLLOUT �Ƕ��٣��� */
		}
	}
	else if(clkSour == CLK_12M)								/*  ѡ�� 12M  */
	{
		return EXTAL_CLK / div;
	}
	else if(clkSour == CLK_32k)								/*  ѡ�� 32.768K  */
	{
		return 32768 / div;
	}
	else
	{
		return HY_ERR_BAD_PARAMETER;
	}
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getBusclk
* ����:	get bus  clock
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getBusClk(void)
{
	U32 ratio,clkSour;
	
	ratio    = hyhwCpm_getBusClkDiv() + 1;
	clkSour = hyhwCpm_getSysClkSourceo();
	return (hyhwCpm_getClk(ratio,clkSour));

}


/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getCpuClk
* ����:	get CPU core clock 
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getCpuClk(void)
{
	U32 ratio,busClk,pllClk,cpuClk;
	
	pllClk = hyhwCpm_getPllOut0();
	busClk = hyhwCpm_getBusClk();
	ratio  = hyhwCpm_getCpuClkDiv() + 1;
	cpuClk = busClk + ((pllClk - busClk) * ratio) / 16;
	return cpuClk;
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getTimerClk
* ����:	get timer clock 
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getTimerClk(void)
{
	U32 div,clkSour;
	
	clkSour = (CKC_PCK_TCZ_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	div   = (CKC_PCK_TCZ_REGISTER & MODEL_BIT_DIV_MASK) + 1;
	
	return (hyhwCpm_getClk(div,clkSour));
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getTimer32Clk
* ����:	get timer clock 
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getTimer32Clk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_TCZ_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_TCZ_REGISTER & MODEL_BIT_DIV_MASK) ) + 1;
	return (hyhwCpm_getClk(ratio,clkSour));
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getI2cClk
* ����:	get i2c clock 
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getI2cClk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_I2C_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_I2C_REGISTER & MODEL_BIT_DIV_MASK) ) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));

}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getLcdClk
* ����:	get lcd clock 
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getLcdClk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_LCD_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_LCD_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));

}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getAdcClk
* ����:	get adc clock 
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getAdcClk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_ADC_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_ADC_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));

}
/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getSpi0Clk
* ����:	get SPI0 clock 
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getSpiClk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_SDMMC_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_SDMMC_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getUart0Clk
* ����:	get UART0 clock 
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getUart0Clk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_UART0_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_UART0_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getUart1Clk
* ����:	get UART1 clock 
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getUart1Clk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_UART1_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_UART1_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}
/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getUart2Clk
* ����:	get UART2 clock 
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getUart2Clk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_UART2_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_UART2_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}
/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getUart2Clk
* ����:	get UART3 clock 
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getUart3Clk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_UART3_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_UART3_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}
/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getDaiClk
* ����:	get DAI clock 
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getDaiClk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_DAI_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_DAI_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getUsbClk
* ����:	get USB clock 
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getUsbClk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (CKC_PCK_USB11H_REGISTER & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((CKC_PCK_USB11H_REGISTER & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}

/*----------------------------------------------------------------------------
* ����:	hyhwCpm_getMmcClk
* ����:	get MMC clock 
* ����:	none 
* ����:	none
*----------------------------------------------------------------------------*/
U32 hyhwCpm_getMmcClk(void)
{
	U32 ratio,clkSour;
	
	clkSour = (SDMMC_CLK & MODEL_BIT_SOURCE_MASK) >> 24;
	ratio   = ((SDMMC_CLK & MODEL_BIT_DIV_MASK)) + 1;	
	return (hyhwCpm_getClk(ratio,clkSour));
}


U32 hyhwCpm_Get_LcdClk()
{
	return LCD_CLK;
}


/* end of file */

