#ifndef _GLOBAL_VARIABLE_BASE_H_
#define _GLOBAL_VARIABLE_BASE_H_


#ifdef __cplusplus
extern "C"
{
#endif

/*--------------------------------------------------------------------------
Standard include files:
--------------------------------------------------------------------------*/

#include "glbStatusDef.h"
#include "glbCmdDef.h"

/*--------------------------------------------------------------------------
*INCLUDES
--------------------------------------------------------------------------*/
#include "hyhwRtc.h"

/*--------------------------------------------------------------------------
*DEFINES
--------------------------------------------------------------------------*/


#define cSYSTEM_BMP_OFFSET				0x00360000


//��Sdram�Ͽ�һ�����棬����ʱһ���Զ�ȡNand persistent area��ȫ���������Ϣ���ݵ��˻���
//���������У���ȡ��д�뱣�����Ϣ���ݶ��ڴ˻����н���
//�ػ�ʱ��һ����д��Nand persistent area
	
#define cPARAMETER_TOTAL_SIZE		30720

//ȫ�ֽṹ
#define cPARAMETER_OFFSET			0
#define cPARAMETER_SIZE				(sizeof(glb_HycParameter_t))

//���ݵ�ȫ�ֽṹ
#define cPARAMETER_BAK_OFFSET		(cPARAMETER_OFFSET+cPARAMETER_SIZE)
#define cPARAMETER_BAK_SIZE			(sizeof(glb_HycParameter_Bak_t))

/*--------------------------------------------------------------------------
*DEFINES
--------------------------------------------------------------------------*/
#define OFFSET_UNICODE_FONT16X16     	0
#define OFFSET_GBK_FONT16X16         	0x00200000
#define OFFSET_KOREAN_FONT16X16      	0x002BD000
#define OFFSET_JAPAN_FONT16X16       	0x00377860

#define OFFSET_UNICODE_FONT24X24       	0x00431860
#define OFFSET_GBK_FONT24X24           	0x008B1860
#define OFFSET_KOREAN_FONT24X24        	0x00A5AC60
#define OFFSET_JAPAN_FONT24X24         	0x00BFE738

#define OFFSET_UNICODE_FONT32X32       	0x00DA0F38
#define OFFSET_GBK_FONT32X32           	0x015A0F38
#define OFFSET_KOREAN_FONT32X32        	0x01894F38
#define OFFSET_JAPAN_FONT32X32         	0x01B7F0B8

// BIT 7: REVERSE DISPLAY
// BIT 6: ENGLISH/CHINESE_CHARACTER
#define REVERSE_DISPLAY_BIT		0x80	// 0: Normal display; 1: reverse display
#define CHARACTER_TYPEBIT		0x40	// 0: English char; 1: Chinese char

#define FONTSIZE_BIT			0x0F	// 0: 8x16; 1: 6x7; 2: 6x6; 3: 6x5 ; 4: 9x16; 5: 4x4

//��ʾ�ַ�����Դ
#define CHARACTER_SRC_MASK			0x7F00	//�ַ���Դ����
#define CHARACTER_SRC_PROGRAM		0x0000	//�����ͬ������ļ����ַ�
#define CHARACTER_SRC_FILENAME		0x0100	//��PSA��ȡ���ļ���
#define CHARACTER_SRC_MP3ID3		0x0200	//��PSA��ȡ��MP3ID3��Ϣ
#define CHARACTER_SRC_WMAID3		0x0300	//��PSA��ȡ��WMAID3��Ϣ
#define CHARACTER_SRC_LRCLOCAL		0x0400	//��LRC�ļ��еõ��ı������ַ�
#define CHARACTER_SRC_LRCUNICODE	0x0500	//��LRC�ļ��еõ���UNICODE�ַ�

#define CHARACTER_NOT_UTF8			0x8000	//������ǰ�ַ�������UTF8�����ַ���

#define FONTSIZE8X16		0x0
#define FONTSIZE6X7			0x1
#define FONTSIZE6X6			0x2
#define FONTSIZE6X5			0x3
#define FONTSIZE9X16		0x4
#define FONTSIZE4X4			0x5
#define FONTSIZE5X16_DDASH	0x6
#define	FONTSIZE5X8			0x7
#define FONTSIZE7X11        0x8
#define FONTSIZE12X17       0x9
#define FONTSIZE6X9         0xA
#define FONTSIZE9x7			0xB	//���ұߵ����
#define FONTSIZE9x7_LR		0xC	//����ߵ��ұ�
#define FONTSIZE5X7			0xD

#define FONTSIZE24X24		0xE
#define FONTSIZE32X32		0xF


#define cLCD_16BIT_COLOR
//#define cLCD_18BIT_COLOR


#ifdef cLCD_16BIT_COLOR
	// define color pattern  65k color 16 bits
	#define  COLOR_RED			0xF800
	#define  COLOR_GREEN		0x07E0
	#define  COLOR_BLUE			0x001F
	#define  COLOR_WHITE		0xFFFF
	#define  COLOR_BLACK		0x0000
	#define  COLOR_YELLOW		0xFFE0
	#define	 COLOR_CYAN			0x07FF
	#define  COLOR_BACKGND0X0005	0x0005
	#define  COLOR_BACKGND0X0138	0x0138
	#define  COLOR_200225250	0xCB9F
	#define  COLOR_255075000	0xF920
	#define  COLOR_BATRED		0xD100
	#define  COLOR_BATYELLOW	0xEF40
	#define  COLOR_BATGREEN		0x0660
	#define  COLOR_000000085	0x55
	#define  COLOR_085162252	0xAA9F
	#define  COLOR_126168255	0x7abf

	//�������	EVA
	#define  COLOR_ORANGE		0xFA80
	#define  COLOR_CORAL		0xF9EA
	#define  COLOR_BRASS		0xB280
	#define  COLOR_SILVER		0xC318
	#define  COLOR_MAGENTA		0xF81F

	#define PERSPECTIVE_DISPLAY_BIT		0x80//͸��ͼ��־
#endif //#ifdef cLCD_16BIT_COLOR


#ifdef cLCD_18BIT_COLOR
	#define  COLOR_RED   	        0x3f000
	
	#define  COLOR_GREEN   	        0x0fc0
	#define  COLOR_BLUE             0x003f
	#define  COLOR_BLACK            0x00000000
	#define  COLOR_WHITE            0x3ffff

    #define  COLOR_YELLOW		    0x3ffc0
    
#endif

#define c1POINT_OCCUPY_BYTES		2//ÿ����ɫ��ռ���ֽ���

#define cLCD_TOTAL_ROW				320
#define cLCD_TOTAL_COLUMN			240


//����״̬
#define CLS_STATUS_UNLOCK     	0  	//û������ 
#define CLS_STATUS_INTIAL     	1
#define CLS_STATUS_LOCKING    	2
#define CLS_STATUS_LOCKED     	3  	//��е����
#define CLS_STATUS_UNLOCKING	4	//������


//����ʱ��
#define cSCREENPROTECT_DEFAULT	30//10//10�� MEI
#define cSCREENPROTECT_1MIN	    60//60//20//20s
#define cSCREENPROTECT_5MIN	    300//300//40//40s
#define cSCREENPROTECT_10MIN	600//600//60//60s
#define cSCREENPROTECT_CLOSE	0//0


/*--------------------------------------------------------------------------
*TYPEDEFS
--------------------------------------------------------------------------*/
typedef enum //_Character_Type_e
{
	CHARACTER_LOCALCODE=0,
	CHARACTER_UNICODE,
	CHARACTER_UTF8
	
}Character_Type_e;



//USB����״̬ ����: gUsbConnectStatus
typedef enum _USB_ConnectStatus_e
{
	USB_Disconnected = 0,
	USB_Connect_Init,
	USB_Connecting,
	USB_Suspend_Init,
	USB_Charging,
	USB_PowerDown
	//USB_Disconnect_Init

} USB_ConnectStatus_e,* pUSB_ConnectStatus_e;


/*--------------------------------------------------------------------------
* external Variable ,constant or function
--------------------------------------------------------------------------*/
extern U16  gLcd_Total_Column;
extern U16  gLcd_Total_Row;
// Ϊ����ĳЩ��Ʒ����ȷ����ʾ�����ڸ���ģʽ�£���ʾ�Ľ��޲�һ��
extern U16 gu16Lcd_ColumnLimit;

extern U16 gFrontColor;
extern U16 gBackColor;

//resource.bin load offset
extern S32 gs32BmpOffset;
extern BOOL gRefreshNeedQue;

extern U8 gu8EnableRefreshSrc; //�Ƿ�����ˢ��

extern BOOL bUSB_Status;
extern int  gs32VolumeFd;

extern U16 gu16ScreenPosX,gu16ScreenPosY;

extern U8 gScreen_Display_Mode;

extern RtcTime_st gtRtc_solar;
/*-----------------------------------------------------------------------------
* ����:	Hyc_crc32
* ����:	����32bit CRCУ����
* ����:	array----��Ҫ����У���buffer
*		size----buffer��С
* ����:	32λCRCУ����
*----------------------------------------------------------------------------*/
U32 Hyc_crc32(void *array,U32 size);


/*-----------------------------------------------------------------------------
* ����:	glb_SetPersistentParameter
* ����:	�����ϵͳ������Ŀ¼��Ϣ����������Ϣ��Favorite��Ϣ��¼���ļ������Ϣ��
*		��Ƶ��Ƶ�ļ���Map��Sdram Parameter Area��
* ����: source		--	ָ������߸����Ļ��棬����Ҫ���������
*		index		--	���persistent area��ƫ���������ֽڼ�
*		len			--	���ȣ����ֽڼ�
* ����:	������
*----------------------------------------------------------------------------*/
int glb_SetPersistentParameter( char* source, int index, int len );

/*-----------------------------------------------------------------------------
* ����:	glb_GetPersistentParameter
* ����:	��Sdram Parameter Area�л�ȡϵͳ������Ŀ¼��Ϣ����������Ϣ��Favorite��Ϣ��¼���ļ������Ϣ��
*		��Ƶ��Ƶ�ļ���Map��
* ����: destination	--	ָ������߸����Ļ��棬��ȡ��������ݽ�����˻���
*		index		--	���persistent area��ƫ���������ֽڼ�
*		len			--	���ȣ����ֽڼ�
* ����:	������
*----------------------------------------------------------------------------*/
int glb_GetPersistentParameter( char* destination, int index, int len );

/*-----------------------------------------------------------------------------
* ����:	glb_GetPersistentParameterFromNand
* ����:	��Nand PersistentArea��һ���Ի�ȡϵͳ������Ŀ¼��Ϣ����������Ϣ��Favorite��Ϣ��¼���ļ������Ϣ��
*		��Ƶ��Ƶ�ļ���Map��Sdram Parameter Area
* ����: none
* ����:	������
*----------------------------------------------------------------------------*/
int glb_GetPersistentParameterFromNand(void);

/*-----------------------------------------------------------------------------
* ����:	glb_SetPersistentParameterToNand
* ����:	��Sdram Parameter Area��ϵͳ������Ŀ¼��Ϣ����������Ϣ��Favorite��Ϣ��¼���ļ������Ϣ��
*		��Ƶ��Ƶ�ļ���Map��һ����д�뵽Nand PersistentArea
* ����: none
* ����:	������
*----------------------------------------------------------------------------*/
int glb_SetPersistentParameterToNand(void);


/*-----------------------------------------------------------------------------
* ����:	GetResouceBmpOffset
* ����:	�����Դ��nand reserve���е�OffSet
* ����:	none
* ����:	>0----��Դ��nand reserve���е�OffSet
*		-1----nand reserve��������Դ
*----------------------------------------------------------------------------*/
S32 GetResouceBmpOffset(void);

/*-----------------------------------------------------------------------------
* ����:	SetResouceBmpOffset
* ����:	������Դ��nand reserve���е�OffSet
* ����:	offset
* ����:	none
*----------------------------------------------------------------------------*/
void SetResouceBmpOffset(S32 offset);


#ifdef __cplusplus
}
#endif

#endif //_GLOBAL_VARIABLE_BASE_H_
