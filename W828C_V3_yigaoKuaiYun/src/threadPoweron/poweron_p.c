/*----------------------�ļ�����--------------------------------
*��������: 08/03/07
*����    : ��������
*--------------------------------------------------------------*/

/*
*include standard files
*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

/*
*include local files
*/
#include "hyhwCkc.h"

#include "AbstractFileSystem.h"
#include "glbVariable_base.h"
#include "LcdAPIFunc.h"
#include "DisplayMemory.h"

#include "grap_deskbox.h"
#include "poweron_p.h"

#include "poweron_showdesk.h"
#include "LcdModule_BackLight.h"


void SoftWare_Init(void);
static void CheckDiskVolume(void);

extern void * hsaDisplayMemoryPool_GetBuffer(void);
extern int hsaDisplayMemoryPool_GetBufferSize(void );

/*---------------------------------------------------------
*����: PowerOn_ThreadMethod_Init
*����: ��ʼ��һЩ������overlay������
*����: 
*����: none
*---------------------------------------------------------*/
static void PowerOn_ThreadMethod_Init(void)
{
	Gsm_InitToSdram_buf();
	gsm_SetPinCode(getPINCode());
}
/*void ttgsm()
{
	AppCtrl_CreatPro(GSMCDMA_CLIENT_AN,0);
    AppCtrl_CreatPro(GSMCDMA_SERVER_AN,0);
}*/
/*---------------------------------------------------------
*����: PowerOn_ThreadMethod
*����: ϵͳ��ʼ������øý��̣�
*����: 
*����: none
*---------------------------------------------------------*/
int PowerOn_ThreadMethod(void)
{
	char MsgBody[POWER_ON_MSG_MAX_SIZE];
	char PutMsgBody[(POWER_ON_MSG_MAX_SIZE+MSG_PUT_HEAD_SIZE)];
	tGRAPDESKBOX LocalDesk;
	U8   focus;	
	U8   QuitAll = 0;
    tAPPLICATION   *pPro;
    U32 putadd;
    U32 len;
    S32 ret;
    
	memset(MsgBody, 0, POWER_ON_MSG_MAX_SIZE);
    LcdModulePowerOnOff(0);
    
  	PowerOn_ThreadMethod_Init();

    memset(&LocalDesk,0,sizeof(tGRAPDESKBOX));
	memset(PutMsgBody,0,sizeof(PutMsgBody));

	
	QuitAll = 0;
	focus = TRUE;
	
	LocalDesk.pUIFocus = &focus;
	LocalDesk.pQuitCnt= &QuitAll;
	
	LocalDesk.pPutMsg = &putadd;
	
	pPro = AppCtrl_GetAppInfor(POWER_ON_AN);
	
	LocalDesk.pGetMsg = pPro->ptmsg;
	*LocalDesk.pPutMsg = NULL;
	LocalDesk.ptGetMsgBody = (tMSG_BODY *)MsgBody;
	LocalDesk.ptPutMsgParam = (tMSG_PUT_PARAM *)PutMsgBody;

	//hyUsbDebug_init();
    //����ʱ������
    Battery_If_StartPowerOff();

    //hyhwSysClk_Set(ICLK192M_AHB96M, EXTAL_CLK);
//    hyhwSysClk_Set(ICLK96M_AHB48M, EXTAL_CLK);
    //hyhwSysClk_Set(ICLK120M_AHB60M, EXTAL_CLK);
    if(gtHyc.NetSelected == 1)//ѡ���gsm����
    {
	    AppCtrl_CreatPro(GSMCDMA_CLIENT_AN,0);
	    AppCtrl_CreatPro(GSMCDMA_SERVER_AN,0);
	    
	    SIM_set_pin_stat(1);
 		//����GSM ����
		GsmReset_WakeUp_Ctrl(1);
    }
    else
    {
    	//�����wifi����������һ��GSM���ٹر�
    	gsmcdma_powerOff_start();
    	WifiDriver_Start();
    	GsmReset_WakeUp_Ctrl(0);
    }
    
    AppCtrl_CreatPro(TCPIP_AN,0);


shell_log("poweron");

    PowerOn_ShowDeskbox(&LocalDesk);
    
    drvBadScanKey_check(0); // ������ɨ����ļ��
	ret = 0;
	while(ret >= 0)
	{
		syssleep(1);
	    ret = msg_queue_tryget(LocalDesk.pGetMsg,LocalDesk.ptGetMsgBody,&len);
		#if 1
		if (ret >= MSG_QUEUE_OK)
		{
			int id;
			id = LocalDesk.ptGetMsgBody->id;
			if (SYS_REQ_QUIT_EVENT == id)
			{
			    *LocalDesk.pPutMsg = ptMsg_controlDesk;
				
			    LocalDesk.ptPutMsgParam->body.id = USE_ACK_QUIT_EVENT;
			    LocalDesk.ptPutMsgParam->body.msg.MsgQueAdd = (U32)LocalDesk.pGetMsg;
				LocalDesk.ptPutMsgParam->priority = 10;
				LocalDesk.ptPutMsgParam->length =  sizeof(U32)+sizeof(eMSG_ID);
			}
		}
		#endif
	}
	
	if (NULL == *LocalDesk.pPutMsg)
    {
    	*LocalDesk.pPutMsg = ptMsg_controlDesk;
    	LocalDesk.ptPutMsgParam->body.id = USE_QUIT_EVENT;
	    LocalDesk.ptPutMsgParam->priority = 10;
	    LocalDesk.ptPutMsgParam->length = sizeof(eMSG_ID);
    }    
    //�����Ƿ����������
    msg_queue_put(*LocalDesk.pPutMsg,(char *)&LocalDesk.ptPutMsgParam->body,LocalDesk.ptPutMsgParam->length,LocalDesk.ptPutMsgParam->priority);
	
	return 1;
}

static void Update_Resource(U16 row, U16 column) 
{
	int reserveHandle, resourceHandle;
	S32 bmpOffset;

	bmpOffset = HSA_GetReserveAreaBlockMap((char *)hsaSdramReusableMemoryPool());
	//bmpOffset = -1;//for test
	
	reserveHandle = AbstractFileSystem_Open("C:/reserve.bin", AFS_READ);
	AbstractFileSystem_Close(reserveHandle);
	resourceHandle = AbstractFileSystem_Open("C:/resource.bin", AFS_READ);
	AbstractFileSystem_Close(resourceHandle);
	if(reserveHandle >= 0 || resourceHandle >= 0)
	{
		//���̵�
		Led_ctrl(2);
		
		drvLcd_SetColor(COLOR_WHITE,COLOR_BLACK);
		drvLcd_ClearDisplayMemory(0, 0, gLcd_Total_Row, gLcd_Total_Column);
		drvLcd_SetColor(COLOR_BLACK,COLOR_BLACK);
		gu16Lcd_ColumnLimit=gLcd_Total_Column;
		drvLcd_DisplayString(row,column,0,(U8*)"Updating UI...", NULL, 0);
		DisplayData2Screen();

		enable_IF();
		hyhwLcd_lightSet(LCD_PWM_GRADE_3);//LcdModule_LightBack();
		if (reserveHandle >= 0)
		{
			bmpOffset = HSA_UpdateReservedArea("C:/reserve.bin");
		}
		if (resourceHandle >= 0 && bmpOffset > 0)
		{
			HSA_UpdateReservedAreaFile("C:/resource.bin", bmpOffset);
		}
		
		Led_ctrl(0);
	}
	SetResouceBmpOffset(bmpOffset);
	
	res_load_total_head_info();
	
//	if (GetResouceBmpOffset() == -1)
//	{
//		//û���ҵ��ֿ⼰��Դ����block�����ʽ������
//		//AbstractFileSystem_Format("C");
//	}
}

void CheckDiskVolume(void)
{
	//Check whether the volume C exists.
	U8 testVol = 0;
	
	//if (testVol == 1)	
	//AbstractFileSystem_Format("C");//��һ�¼�ⲻ���ҵ�����ʱ�����ô���䣬������
	if ( AbstractFileSystem_VolumePresent("C") == HY_OK )
	{
		int rc;
		rc = AbstractFileSystem_VolumeFormatted("C");
	
		if( rc == HY_ERROR)	
		{
			AbstractFileSystem_Format("C");
		}
		
		// Volume maintenance
		AbstractFileSystem_VolumeMaintenance( "C" );
	}
	if ( AbstractFileSystem_VolumePresent("D") == HY_OK )
	{
		//int rc;
		/*
		rc = AbstractFileSystem_VolumeFormatted("D");
	
		if( rc == HY_ERROR)	
		{
			AbstractFileSystem_Format("D");
		}
		*/
		// Volume maintenance
		AbstractFileSystem_VolumeMaintenance( "D" );
	}
}


static void CreateDocument(void)
{
	int dirhandle;
	U64 totalspace,freespace = 0; 
	
	if (GetResouceBmpOffset() == -1) return;//û���ҵ��ֿ⼰��Դ����block���򲻴���Ĭ���ļ���


	AbstractFileSystem_VolumeSpace( "C", &totalspace, &freespace);
	
	if (freespace < 0x100000) return;

    AbstractFileSystem_MakeDir((char*)"C:/ϵͳ");
    AbstractFileSystem_MakeDir((char*)"C:/data");//������ص�����
}

#define SYS_DATA_STRING  "2011/07/01"
#define SYS_TIME_STRING  "12:00:00"

void SoftWare_Init(void)
{
	AbstractFileSystem_Init();
	CheckDiskVolume();
	
    Hyc_Set_SysTime(SYS_DATA_STRING,SYS_TIME_STRING);
	Update_Resource(180, 59);
	CreateDocument(); 	
	glb_GetPersistentParameterFromNand();
	glbVariable_LoadFromPersistentArea();
	glbVariable_CheckValue();
	
    memset((char *)hsaDisplayMemoryPool_GetBuffer(), 0, 
                   hsaDisplayMemoryPool_GetBufferSize()); 
	
}

/*-------------------------�޸ļ�¼--------------------------------
*
*-----------------------------------------------------------------*/