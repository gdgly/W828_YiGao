/*************************************************************************
 * INCLUDES
 *************************************************************************/

 /*----------------------------------------------------------------------------
   Standard include files:
   --------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "vhChip.h"

//#include "hyhwPmu.h"
//#include "hyhwClockSet.h"
//#include "hyhwDma.h"
#include "hyhwNandflash.h"
#include "hyhwIntCtrl.h"

//#define HYHW_MEMORY_SD_MMC
#ifdef HYHW_MEMORY_SD_MMC
	#include "hyhwSdmmc.h"
	#include "hyswSdmmc.h"
#endif

#include "std.h"
#include "sys.h"
#include "ospriority.h"

#include "ioctl.h"

#include "HSA_API.h"
#include "AbstractFileSystem.h"
#include "BoardSupportPackage.h"

#include "hyhwGpio.h"


#define USB_MAPPING_BLOCK0_TO_SDRAM	//USB ʱBLOCK0��ӳ�䵽Sdram
#define USB_FILE_SYS

#define USB_USE_MAX_BUF

/*************************************************************************
 * DEFINES
 *************************************************************************/

#define USB_VOLUME              ("C")

#define USB_RUN_HIGH_SPEED      192, 96, 48, 48
#define USB_RUN_LOW_SPEED        48, 48, 24, 24

#define SUSPEND_COUNT           (40)//40*50 = 2�룬����2���⵽suspendʱ�����˳�USB
#define ADAPTER_COUNT           (200)//200*50 = 10�룬10�뻹δ����USB�����˳�USB

#define USB_LOWSPEED_IDLE_CNT   (12000)	//12000*50 = 10���Ӻ�������
#define SD_DETECT_COUNT         (10)//10*50 = 0.5�룬������⵽SD�����룬����Ϊ�Ѳ���SD��

#define FLUSHING_PERIOD         (10)//10*50 = 0.5�룬USB��������0.5��󣬻�д�ļ�ϵͳ��Ϣ

#define USB_NAND_BLOOK_POOL_MANAGER_SIZE		(2*1024*1024)

/*************************************************************************
 * TYPEDEFS
 *************************************************************************/
 
/*************************************************************************
 * GLOBALS
 *************************************************************************/
UInt8  gScsiApplnScsiMaxLunFlag;

#pragma arm section zidata="usb_zidata", rwdata="usb_zidata"
/*************************************************************************
 * STATICS
 *************************************************************************/
static BOOL   sbUSB_AdapterFlag;
static BOOL   sbUSBspeedDetectFlag;
static UInt8  su8USB_AdapterCnt;
static Int8	  susbTransferStatus;
static UInt16 su16USB_IdleCnt;

#ifdef HYHW_MEMORY_SD_MMC
	static Bool   sbSdInit;
	static UInt8  su8SdDetectCnt;
	static UInt8  su8SdSdLastStatus;
#endif

#pragma arm section //zidata="usb_zidata", rwdata="usb_zidata"

static void USBHandler_InitialiseHardware(void);
static void USBHandler_DeInitialiseHardware(void);
static void USBHandler_DetectSDMMC(void);

void USBHandler_USB_DeInit(int volumeHandle);

void (* usb_UpdataSpeedDirect)(int clk);

/*************************************************************************
 * EXTERNALS
 *************************************************************************/
extern Int8 gtmUsbTransferInProgressRx;
extern Int8 gtmUsbTransferInProgressTx;
extern BOOL gbSuspendDetected;

extern void fnUSB_DevInit(void);
extern int  hyudsDcd_Connected(void);

extern void mappingTable_ReadTabletoBuffer(int value);
//extern void nandLogicDrv_SetBlockPool(void* buffer, int size);
extern void nandLogicDrv_InvalidBlockPool(void);

extern void *hsaUsbTransport_GetBuffer(void);
extern int  hsaUsbTransport_GetBufferSize(void);

void USBHandler_ClearAdapterCnt(void);

extern hyudsDcd_UsbDispatchReadStatus(void);

void usb_UpdataSpeed_Config(void(*pHandle)(int clk))
{
	usb_UpdataSpeedDirect = pHandle;
}

void USBHandler_StartDetectUSBStatus(void)
{
#if 1
	//hyhwInt_ConfigUsbSuspend_ISR(hyudsDcd_UsbDispatchReadStatus);
	/* ��ʼ����ʱ�� */
	hyhwTimer_countDisable(1);
	hyhwTimer_intDisable(1);
	hyhwInt_ConfigTimer1_ISR(hyudsDcd_UsbDispatchReadStatus);
	hyhwTimer_SetFrequency(1, 500);
	hyhwTimer_intEnable(1);
	hyhwTimer_countEnable(1);
#endif
}
void USBHandler_StopDetectUSBStatus(void)
{
#if 1
	//hyhwInt_ConfigUsbSuspend_ISR(NULL);
	hyhwTimer_countDisable(1);
	hyhwTimer_intDisable(1);
	hyhwInt_ConfigTimer1_ISR(NULL);
#endif
}

/*-----------------------------------------------------------------------------
* ����:	USBHandler_USBInit
* ����:	����USB״̬ʱ����USB���г�ʼ��(��Ӳ��)
* ����:	none
* ����:	��ʼ���ɹ�-----�����ļ�ϵͳhandle("C"�̷���handle)
*       ��ʼ��ʧ��-----HY_ERROR
*----------------------------------------------------------------------------*/
int USBHandler_USBInit(void)
{
    int volumeHandle = -1;
    
    USB_InitMemory_buf();
    sbUSB_AdapterFlag   = True;
    su16USB_IdleCnt     = 0;
    sbUSBspeedDetectFlag  = True;
    su8USB_AdapterCnt   = 0;
    gScsiApplnScsiMaxLunFlag = 0;
    
    /* Initialise Hardware */
    USBHandler_InitialiseHardware();

	/* �ر�nand ����㣬��д */
	hyswNandFlash_Close();
	
	/* ����FAT2 */
	volumeHandle = open("C", AFS_WRITE);
	ioctl(volumeHandle, UPDATE_FAT2);
	close(volumeHandle);
	volumeHandle = -1;

	/* �ر�nand ����㣬��д */
	hyswNandFlash_Close();
	
	/* ��mapping table ���뵽ram�У� �ӿ����ֻ���ϴ�ʱ���ٶ� */
	mappingTable_ReadTabletoBuffer(1);
	#ifdef USB_MAPPING_BLOCK0_TO_SDRAM
	{
		char * usbTransBuf;int usbTransBufSize;
		//usbTransBuf = (char *)hsaUsbTransport_GetBuffer();
		//usbTransBufSize = hsaUsbTransport_GetBufferSize();
		usbTransBuf = (char *)hsaSdramReusableMemoryPool();
		//usbTransBufSize = hsaSdramReusableMemoryPool_Size();
		usbTransBufSize = USB_NAND_BLOOK_POOL_MANAGER_SIZE;
		#ifdef USB_USE_MAX_BUF
		nandLogicDrv_SetBlockPool(pNandBlockPoolManager, NAND_BLOOK_POOL_MANAGER_SIZE,
									(char *)usbTransBuf,
									usbTransBufSize);
		#else
		nandLogicDrv_SetBlockPool(pNandBlockPoolManager, NAND_BLOOK_POOL_MANAGER_SIZE,
									(char *)hsaUsbTransport_GetBuffer(),
									hsaUsbTransport_GetBufferSize());
		#endif
	}
	#endif

#ifdef USB_FILE_SYS
    /* Open volume */
    volumeHandle = AbstractFileSystem_Open(USB_VOLUME, AFS_RDWR);
    if (volumeHandle < 0)
    {//�ļ�ϵͳ����ֱ�ӷ��ش���
        return ((int)HY_ERROR);
    }
#else
	hyswNandFlash_Open();
	volumeHandle = 0;
#endif    
    
#ifdef HYHW_MEMORY_SD_MMC
 	hyhwDma_Init();
 	
	/* Scsi drivers initialisation - SD card*/
    //hyhwPmu_SwitchPeripheral(hyhwPmu_PeripheralClk_Dma, EnableOn);
    //hyhwPmu_SwitchPeripheral(hyhwPmu_PeripheralClk_Ssi, EnableOn);
	
	su8SdDetectCnt = 0;
	su8SdSdLastStatus = 0;
	sbSdInit= False;
	/* ����������л����Ƿ��п� */
  	if( hyswSdMmc_Init() == HY_OK)
  	{
	  	sbSdInit = True;
	  	su8SdSdLastStatus = 1;
  	}
#endif

    /* Initialise USB  */
    tmllpeScsiAppln_Initialise(volumeHandle);

    /* initialise Usb Stack */
  	fnUSB_DevInit();
  	/*	����Ƿ�ΪUSB�߲���
  		�ж�����ΪUSB���ٶȼ�⣬��ʮ��׼ȷ
  		��USBHandler_Application()�л�Ҫ��gScsiApplnScsiMaxLunFlag�ж�
  		��gScsiApplnScsiMaxLunFlagΪ1ʱ������������USB״̬
  	*/
  	if (hyudsDcd_GetSpeedDetectFlag() == FALSE)
  	{
  		//�������������
  		USBHandler_USB_DeInit(volumeHandle);
  		volumeHandle = -1;
  	}
	hyudsDcd_StopSpeedDetect();
	su8USB_AdapterCnt = 0;
	hyudsDcd_ConfigCallback(USBHandler_ClearAdapterCnt);
	susbTransferStatus = 0;

    return (volumeHandle);
}


/*-----------------------------------------------------------------------------
* ����:	USBHandler_USB_DeInit
* ����:	�˳�USB״̬ʱ����USB���з���ʼ��(��Ӳ��)
* ����:	volumeHandle------�ļ�ϵͳhandle("C"�̷���handle)
* ����:	none
*----------------------------------------------------------------------------*/
void USBHandler_USB_DeInit(int volumeHandle)
{
#ifdef USB_FILE_SYS
    /* Close volume */
    AbstractFileSystem_Close( volumeHandle );
#else
	hyswNandFlash_Close();
#endif
    
	/* �ر�nand ����㣬��д */
	hyswNandFlash_Close();
	#ifdef USB_MAPPING_BLOCK0_TO_SDRAM
	nandLogicDrv_InvalidBlockPool();
	mappingTable_ClearTableInBuffer();
	
	mappingTable_ReadTabletoBuffer(1);
	nandLogicDrv_SetBlockPool(pNandBlockPoolManager, NAND_BLOOK_POOL_MANAGER_SIZE,
								(char *)hsaUsbTransport_GetBuffer(),
								hsaUsbTransport_GetBufferSize());
	#endif

    /* Restore hard ware initial settings */
    USBHandler_DeInitialiseHardware();
}

#pragma O0
#pragma arm section rodata="usb_function", code="usb_function"
/*-----------------------------------------------------------------------------
* ����:	USBHandler_Application
* ����:	usb ״̬�µĵ��õĹ��ܺ���
* ����:	volumeHandle------�ļ�ϵͳhandle("C"�̷���handle)
* ����:	none
*----------------------------------------------------------------------------*/
int USBHandler_Application(int *pvolumeHandle)
{
    HSA_USBCommunicationStatus_en usbStatus;
    Bool connect_status = True;
    
    //ɨ��SD�����Ȱβ�
    USBHandler_DetectSDMMC();
    
    /* �����Եص��ã��������True��
       ��ʾ�Ѿ������ؿ�����16�Σ����Ե���flash close �Ի�д */
    if ( gScsiApplnScsiMaxLunFlag == 1 )
    {
        UInt32 USB_DeviceStatus;
        
    #if 1
		//���PC���Ƿ�Ͽ�USB����(��Ͽ�)
       if (gbSuspendDetected == TRUE)
        {
			//�˳�USB
			connect_status = False;
        }
        else
    #endif
        {
            /* Process transfer transitions */
            if ( gtmUsbTransferInProgressRx )
            {
                usbStatus = HSA_USB_RECEIVING_UCS;
            }
            else if ( gtmUsbTransferInProgressTx )
            {
                usbStatus = HSA_USB_TRANSMITTING_UCS;
            }
            else
            {
                usbStatus = HSA_USB_IDLE_UCS;
            }
            if (susbTransferStatus == 1)
            {
            	if (usbStatus == HSA_USB_IDLE_UCS)
            	{
            		//�������״̬���������USB�豸״̬
            		USBHandler_StartDetectUSBStatus();
            	}
            }
            else
            {
            	if (usbStatus != HSA_USB_IDLE_UCS)
            	{
            		//�������ݴ���״̬��ֹͣ���USB�豸״̬
            		USBHandler_StopDetectUSBStatus();
            	}
            }
            
            /* Reset Transfer in progress flag */
            gtmUsbTransferInProgressRx = False;
            gtmUsbTransferInProgressTx = False;
            
            /* Reset counter if we are transmitting */
            if( usbStatus != HSA_USB_IDLE_UCS )
            {
                susbTransferStatus = 1;
                if (su16USB_IdleCnt >= USB_LOWSPEED_IDLE_CNT)
                {//����Ѿ��������״̬����ô������ٶ�
    				//hyhwSysClk_SetFree(USB_RUN_HIGH_SPEED,PLL1);
    			  	//hyhwClk_SetSsiClk(SSICLKMAX);
    			  	if (usb_UpdataSpeedDirect != NULL)
    			  	{
    			  		usb_UpdataSpeedDirect(192);
    			  	}
                }
                su16USB_IdleCnt = 0;
            }
    		else //HSA_USB_IDLE_UCS
    		{
                susbTransferStatus = 0;
                if (su16USB_IdleCnt < USB_LOWSPEED_IDLE_CNT)
                {
    	            su16USB_IdleCnt++;
    	            switch(su16USB_IdleCnt)
    	            {
    	                case FLUSHING_PERIOD:
                            /* Flush cached contents */
                        #ifdef USB_FILE_SYS
                            AbstractFileSystem_Close(*pvolumeHandle);
                            hyswNandFlash_Close();
                          /* ��usb mapping table ����ָ�� */
                            mappingTable_ReadTabletoBuffer(0);
							#ifdef USB_MAPPING_BLOCK0_TO_SDRAM
							{
								char * usbTransBuf;int usbTransBufSize;
								//usbTransBuf = (char *)hsaUsbTransport_GetBuffer();
								//usbTransBufSize = hsaUsbTransport_GetBufferSize();
								usbTransBuf = (char *)hsaSdramReusableMemoryPool();
								//usbTransBufSize = hsaSdramReusableMemoryPool_Size();
								usbTransBufSize = USB_NAND_BLOOK_POOL_MANAGER_SIZE;
								#ifdef USB_USE_MAX_BUF
								nandLogicDrv_SetBlockPool(pNandBlockPoolManager, NAND_BLOOK_POOL_MANAGER_SIZE,
															(char *)usbTransBuf,
															usbTransBufSize);
								#else
								nandLogicDrv_SetBlockPool(pNandBlockPoolManager, NAND_BLOOK_POOL_MANAGER_SIZE,
															(char *)hsaUsbTransport_GetBuffer(),
															hsaUsbTransport_GetBufferSize());
								#endif
							}
							#endif
                            *pvolumeHandle = AbstractFileSystem_Open(USB_VOLUME, AFS_RDWR);
                        #else
                            hyswNandFlash_Close();
                            hyswNandFlash_Open();
                        #endif
#if 0
                            /* ��usb mapping table ����ָ�� */
                            mappingTable_ReadTabletoBuffer(0);
							#ifdef USB_MAPPING_BLOCK0_TO_SDRAM
							{
								char * usbTransBuf;int usbTransBufSize;
								usbTransBuf = (char *)hsaUsbTransport_GetBuffer();
								//usbTransBufSize = hsaUsbTransport_GetBufferSize();
								usbTransBufSize = USB_NAND_BLOOK_POOL_MANAGER_SIZE;
								#ifdef USB_USE_MAX_BUF
								nandLogicDrv_SetBlockPool(pNandBlockPoolManager, NAND_BLOOK_POOL_MANAGER_SIZE,
															(char *)usbTransBuf,
															usbTransBufSize);
								#else
								nandLogicDrv_SetBlockPool(pNandBlockPoolManager, NAND_BLOOK_POOL_MANAGER_SIZE,
															(char *)hsaUsbTransport_GetBuffer(),
															hsaUsbTransport_GetBufferSize());
								#endif
							}
							#endif
#endif
    	                    break;
    	                case USB_LOWSPEED_IDLE_CNT:
        					//idle��ʱ���󣬽����ٶ�
        					//hyhwSysClk_SetFree(USB_RUN_LOW_SPEED,PLL1);
        				 	//hyhwClk_SetSsiClk(SSICLKMAX);        				 
		    			  	if (usb_UpdataSpeedDirect != NULL)
		    			  	{
		    			  		usb_UpdataSpeedDirect(0);
		    			  	}
    	                    break;
    	                default:
    	                    break;
    	            }
    	        }
            }//if( usbStatus != HSA_USB_IDLE_UCS )
        }//if ((USB_DeviceStatus & 0x80) != 0)
    }
    else //if ( gScsiApplnScsiMaxLunFlag == 0)
    {
        /* ��������������������gScsiApplnScsiMaxLunFlag���ᱻ��True
           �ȴ�һ��ʱ���������ǵȲ���True������Ϊ�����������Ƴ�USB״̬ 
        */
        if (sbUSB_AdapterFlag == True)
        {
            su8USB_AdapterCnt++;
            if (su8USB_AdapterCnt >= ADAPTER_COUNT)
            {//�˳�USB
                gScsiApplnScsiMaxLunFlag = 2;
                connect_status = False;
            }
        }
    }//if ( gScsiApplnScsiMaxLunFlag == 1 )
    
    /* Check if USB still connected ��Automatic USB connection */
    if ( !(hyudsDcd_Connected() & 1))
    {
        connect_status = False; 
    	#if 0
    	{
    	    //��ʱ����֤��д�ļ�ϵͳ��Ϣ
        	int i;
        	i=0x007FFFFF;
        	while(i--);
        	//syssleep(100);
    	}
    	#endif
    }
    
    return (int)(connect_status);
}

/*-----------------------------------------------------------------------------
* ����:	USBHandler_DetectSDMMC
* ����:	usb ״̬��ɨ��SD�����Ȱβ�
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
static void USBHandler_DetectSDMMC(void)
{
#ifdef HYHW_MEMORY_SD_MMC
    int SdDetected, flag;
	
    //ɨ��SD�����Ȱβ�
    flag = 0;
    SdDetected = hyhwCardDetect();
    if (SdDetected == su8SdSdLastStatus)
    {
        su8SdDetectCnt++;
        if (su8SdDetectCnt >= SD_DETECT_COUNT)
        {//�˲�
            if (SdDetected)
            {
                flag = 1;
            }
            
            su8SdDetectCnt = SD_DETECT_COUNT;
        }
    }
    else
    {
        su8SdDetectCnt = 0;
        su8SdSdLastStatus = SdDetected;
    }

    if (flag)
    {
        if(sbSdInit == False)
        {
            if( hyswSdMmc_Init() == 0)
            {//��ʼ���ɹ�
                tmllpeScsiAppln_SDInitialise(1);
                sbSdInit = True;
            }
        }
    }
    else
    {
        sbSdInit = False;
    }
#endif
}
#pragma arm section	/* rodata="usb_function", code="usb_function" */


extern void USB_InitExeRam_Instruction(void);
extern void USB_InitExeRam_Data(void);
/*-----------------------------------------------------------------------------
* ����:	USBHandler_InitialiseHardware
* ����:	��ʼ��USB����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
static void USBHandler_InitialiseHardware( void )
{
    /* USB״̬�¹ر�Codec�����͹��� */
//    hyhwCodec_Configuration(hwCodec_Off);

    /* ��ʼ�� usb ����ʱ��RAM�ռ䣬��scatter������䣬overlay��������������ʼ����
       RO �����RW���������һ������һ�𣬶��Ǵ�һ��load��copy��image�������ԣ����scatter��
       ������RO��RW������copyʱ���RW����һ�𿽱���
       overlay manager ����overlay.s �� */ 
//    USB_InitExeRam_Instruction();
//    USB_InitExeRam_Data();

    /* dma �������ظ������ˣ���ʼ��Ӧ���� 
       hyhwDma_Init �л�򿪵�Դ */
    hyhwDma_Init();

    // ����USBʱ��NandFlash����д
    hyhwNandFlash_EnableWrite();

    /* ֪ͨnandflashӲ����������usb״̬����usb״̬�£� nandflashӲ��������ʹ��DMA */
//    hyhwNandflash_InformUsbStatus(1);

    /* ��ϵͳʱ�������� USB ������״̬�£�usb2.0�ٶȲ���̫��  */
//    hyhwSysClk_SetFree(USB_RUN_HIGH_SPEED,PLL1);
//    hyhwClk_SetSsiClk(SSICLKMAX);
  	if (usb_UpdataSpeedDirect != NULL)
  	{
  		usb_UpdataSpeedDirect(192);
  	}

    /* Enable USB clocks */
//    hyhwPmu_SwitchPeripheral(hyhwPmu_PeripheralClk_Udc, EnableOn);
}


/*-----------------------------------------------------------------------------
* ����:	USBHandler_DeInitialiseHardware
* ����:	����ʼ��USB����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
static void USBHandler_DeInitialiseHardware( void )
{
    /* Ϊʡ�磬Disable USB clocks */
//    hyhwPmu_SwitchPeripheral(hyhwPmu_PeripheralClk_Udc, EnableOff);

//	hyhwSysClk_SetFree(USB_RUN_LOW_SPEED,PLL1);
//	hyhwClk_SetSsiClk(SSICLKMAX);
	
	hyudsDcd_HwDeinit(); //080827
	
  	if (usb_UpdataSpeedDirect != NULL)
  	{
  		usb_UpdataSpeedDirect(0);
  	}
	
	/* ֪ͨnandflashӲ�������˳�usb״̬����usb״̬�£� nandflashӲ��������ʹ��DMA */
//	hyhwNandflash_InformUsbStatus(0);
	
	/* �˳�USBʱ��NandFlash��ֹд */
	hyhwNandFlash_DisableWrite();
}

/*-----------------------------------------------------------------------------
* ����:	USBHandler_TransferStatus
* ����:	���USB����״̬
* ����:	none
* ����:	USB����״̬  1----���ڴ���  0----����״̬
*----------------------------------------------------------------------------*/
int USBHandler_TransferStatus(void)
{
	return (int)susbTransferStatus;
}

/*-----------------------------------------------------------------------------
* ����:	USBHandler_SetAdapterFlag
* ����:	����USBʱ���Ƿ�����������ж�
* ����:	adapterFlag-----1:����0:��ֹ�����������ж�
* ����:	none
*----------------------------------------------------------------------------*/
void USBHandler_SetAdapterFlag(Bool adapterFlag)
{
	sbUSB_AdapterFlag = (Bool)adapterFlag;
}

/*-----------------------------------------------------------------------------
* ����:	USBHandler_ClearAdapterCnt
* ����:	�����������ʱ�жϼ������ú������յ�PC��reset����ʱ����
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void USBHandler_ClearAdapterCnt(void)
{
	su8USB_AdapterCnt = 0;
}

/*-----------------------------------------------------------------------------
* ����:	USBHandler_USBConnect
* ����:	���USB Vbus�Ƿ���Ч
* ����:	none
* ����:	1-----USB Vbus��Ч����usb�߲���
*       0-----USB Vbus��Ч����usb��δ����
*----------------------------------------------------------------------------*/
int USBHandler_USBConnect(void)
{
    return hyudsDcd_Connected();
//	return 1;
}

/*-----------------------------------------------------------------------------
* ����:	USBHandler_GetScsiFlag
* ����:	�ж��豸����״̬
* ����:	none
* ����:	1-----USB Vbus��Ч����usb�߲���
*       0-----USB Vbus��Ч����usb��δ����
*----------------------------------------------------------------------------*/
int USBHandler_GetScsiFlag(void)
{
    return gScsiApplnScsiMaxLunFlag;
}

/*-----------------------------------------------------------------------------
* ����:	USBHandler_SetScsiFlag
* ����:	�����豸��״̬
* ����:	none
* ����:	1-----USB Vbus��Ч����usb�߲���
*       0-----USB Vbus��Ч����usb��δ����
*----------------------------------------------------------------------------*/
void USBHandler_SetScsiFlag(int value)
{
    gScsiApplnScsiMaxLunFlag = value;
}

/*-----------------------------------------------------------------------------
* ����:	USBSlave_Enable
* ����:	ʹslave enable
* ����:	
* ����:	none
*----------------------------------------------------------------------------*/
void USBSlave_Enable(int value)
{

}

/*-----------------------------------------------------------------------------
* ����:	hyhwUsbHost_PowerCtrl
* ����:	ʹslave enable
* ����:	value 1 ���⹩�� 0 �����⹩��
* ����:	none
*----------------------------------------------------------------------------*/
void hyhwUsbHost_PowerCtrl(int value)
{
}
