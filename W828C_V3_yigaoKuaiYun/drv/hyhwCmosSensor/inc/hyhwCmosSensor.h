
#ifndef _HYHW_CMOSSENSOR_H_
#define _HYHW_CMOSSENSOR_H_

/*************************************************************************
 * INCLUDES
 *************************************************************************/
#include "hyTypes.h"

/*------------------------------------------------------------------------------
Local Types and defines:
------------------------------------------------------------------------------*/
#define SLAVE_ADDR_OV9650			0x60

#define CMOSSENSOR_DATA_ADDRESS		(0x01000000)

//#ifdef AiMei_H0103
#if 1
//#define HREF						GPIO_13	// TXD
#define VSYNC						GPIO_12	// SSI_DI
#define SCCB_CLK					GPIO_10	// SSI_DO
#define SCCB_DATA					GPIO_3	// SSI_CLK
#define WAIT						GPIO_21
#define EGPIO_LE    				GPIO_11 // JTAG-RTCK
#define EGPIO_OUTPUTEN				GPIO_6 	// MD_BOOT   //������������ʹ����JTAG pin��ֱ��ʹ��

//extended GPIO
#define eDATAENABLE					GPIO_5	// BIT7
#define eCMOSSENSOR_RESET 			NULL	// BIT4
#define eCMOSSENSOR_POWERDOWN		GPIO_4	// BIT5

#else

//#define HREF						GPIO_13	// TXD
#define VSYNC						GPIO_1	// SSI_DI
#define SCCB_CLK					GPIO_2	// SSI_DO
#define SCCB_DATA					GPIO_3	// SSI_CLK
#define WAIT						GPIO_21
#define EGPIO_LE    				GPIO_6  // MD_BOOT
#define EGPIO_OUTPUTEN				GPIO_5 	// JTAG-RTCK    //������������ʹ����JTAG pin��ֱ��ʹ��

//extended GPIO
#define eDATAENABLE					BIT7
#define eCMOSSENSOR_RESET 			BIT4
#define eCMOSSENSOR_POWERDOWN		BIT5

#endif

#define COM1	0x04
#define COM6	0x0F
#define COM7	0x12
#define COM10	0x15
#define COM15	0x40
#define CLKRC	0x11

#define CMOSSENSOR_DVK
#ifdef CMOSSENSOR_DVK
	#define IIC_CLK					GPIO_2
	#define IIC_DAT					GPIO_3
#else
	#define IIC_CLK					GPIO_8
	#define IIC_DAT					GPIO_9
#endif



/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
//extern __align(4) char ComsSensorReadBuf_DC[0x1800];//6K

//extern __align(4) char ComsSensorReadBuf_DV[0x9800];//38K
extern __align(4) char ComsSensorReadBuffer[0x4B00];//38K


typedef enum
{
	hwCmosSensor_Scene_Auto_en,			//�Զ�
	hwCmosSensor_Scene_Character_en,	//����
	hwCmosSensor_Scene_Scenery_en,		//�羰
	hwCmosSensor_Scene_Night_en			//ҹ��

} hyhwCmosSensor_mode_en;

/*************************************************************************
 * Function Prototypes:
 *************************************************************************/
/*-----------------------------------------------------------------------------
* ����:	hyhwCmosSensor_Init
* ����:	��ʼ��Cmos sensor�� 
* ����:	none          
* ����:	0:success
*----------------------------------------------------------------------------*/
int hyhwCmosSensor_Init();

/*-----------------------------------------------------------------------------
* ����:	hyhwCmosSensor_ReadRegister
* ����:	read the register value 
* ����:	register address and value         
* ����:	none (always success)
*----------------------------------------------------------------------------*/
//U32 hyhwCmosSensor_ReadRegister(U8 Addr,U8* Value);
U32 hyhwCmosSensor_ReadRegister_MIC(U16 Addr,U16* Value);

/*-----------------------------------------------------------------------------
* ����:	hyhwCmosSensor_WriteRegister
* ����:	write the value to the specified register 
* ����:	register address and value         
* ����:	none (always success)
*----------------------------------------------------------------------------*/
//U32 hyhwCmosSensor_WriteRegister(U8 Addr,U8 Value);
U32 hyhwCmosSensor_WriteRegister_MIC(U16 Addr,U16 Value);

/*-----------------------------------------------------------------------------
* ����:	hyhwCmosSensor_SetWindowArea
* ����:	����cmos sensor��window
* ����:	         
* ����:	none (always success)
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_SetWindowArea(U16 RowStart,U16 RowEnd,U16 ColStart,U16 ColEnd);

/*-----------------------------------------------------------------------------
* ����:	hyhwCmosSensor_GetOneFrameQVGA
* ����:	read frame and display 
* ����:	none        
* ����:	none (always success)
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_GetOneFrameQVGA(U8 *pFrameBuf);

/*-----------------------------------------------------------------------------
* ����:	hyhwCmosSensor_ReadRegister
* ����:	read the register value 
* ����:	register address and value         
* ����:	none (always success)
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_GetOneFrameSXGA(U8 *pFrameBuf,U16 Mode); //1280X1024

/*-----------------------------------------------------------------------------
* ����:	hyhwCmosSensor_Reset()
* ����:	cmos sensor reset control; EnableOn :reset 
* ����:	
* ����:	
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_Reset(Enable_en EnalbeReset );

/*-----------------------------------------------------------------------------
* ����:	hyhwCmosSensor_PowerDown()
* ����:	power down control ,EnableOn :power down. //Ϊ�˽�Լ���ģ���ʹ��sensorӦ����powerdown
* ����:	
* ����:	
*----------------------------------------------------------------------------*/
void hyhwCmosSensor_PowerDown(Enable_en EnalbePowerDown );

U32 hyhwCmosSensor_WriteRegister(U16 Addr,U16 Value);
U32 hyhwCmosSensor_ReadRegister(U16 Addr,U16* Value);

/*-----------------------------------------------------------------------------
* ����:	hyhwCmosSensor_WriteRegister
* ����:	write the value to the specified register 
* ����:	register address and value         
* ����:	none (always success)
*----------------------------------------------------------------------------*/
U32 hyhwCmosSensor_WriteRegister_OV(U16 Addr,U16 Value);
#endif /* _HYHW_CMOSSENSOR_H_ */

 /**@}*/
