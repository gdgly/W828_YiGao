
/*--------------------------------------------------------------------------
Standard include files:
--------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"

#include "glbStatusDef.h"
#include "glbCmdDef.h"
#include "string.h"

/*--------------------------------------------------------------------------
*INCLUDES
--------------------------------------------------------------------------*/
#include "CrcVerify.h"
#include "glbVariable_base.h"
#include "glbVariable.h"
#include "LcdAPIFunc.h"

/*--------------------------------------------------------------------------
*DEFINES
--------------------------------------------------------------------------*/
#define CRC_OK		0
#define CRC_ERR		1


/*--------------------------------------------------------------------------
*TYPEDEFS
--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
*STRUCTURE
--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
* Variable ,constant or function
--------------------------------------------------------------------------*/

//��ʾ��
U16  gLcd_Total_Column = cLCD_TOTAL_COLUMN;
U16  gLcd_Total_Row = cLCD_TOTAL_ROW;

// Ϊ����ĳЩ��Ʒ����ȷ����ʾ�����ڸ���ģʽ�£���ʾ�Ľ��޲�һ��
U16 gu16Lcd_ColumnLimit = cLCD_TOTAL_COLUMN;//��1��cLCD_TOTAL_COLUMN

U16 gFrontColor = COLOR_WHITE;
U16 gBackColor = COLOR_BLACK;

//U8 gScreen_Display_Mode = 0;//0:���ַ���  1:���ַ���

U8 gu8EnableRefreshSrc = 1;

BOOL gRefreshNeedQue = 1;

BOOL bUSB_Status = FALSE; //usb����״̬
int  gs32VolumeFd = -1;   //C�̵�handle

U16 gu16ScreenPosX = 0,gu16ScreenPosY = 0;

//resource.bin load offset
S32  gs32BmpOffset = cSYSTEM_BMP_OFFSET;

RtcTime_st gtRtc_solar;

//#pragma arm section
/*-----------------------------------------------------------------------------
* ����:	Hyc_crc32
* ����:	����32bit CRCУ����
* ����:	array----��Ҫ����У���buffer
*		size----buffer��С
* ����:	32λCRCУ����
*----------------------------------------------------------------------------*/
U32 Hyc_crc32(void *array,U32 size)
{
	unsigned long crc;
	CRC32_Generate(&crc,(unsigned char *)array,size);
	return crc;
} 


/*-----------------------------------------------------------------------------
* ����:	glbVariable_CalculateCRC
* ����:	����CRCУ����
* ����:	pBuf----��Ҫ����У���buffer
*		size----buffer��С
* ����:	16λCRCУ����
*----------------------------------------------------------------------------*/
U16 glbVariable_CalculateCRC(U8 *pBuf,U32 size)
{
	U16 crc;
	
	for( crc = 0xffff; size-- ; )
	{
		crc = (U16)((crc >> 8) | (crc << 8));
		crc = (U16)(crc^(*pBuf++));
		crc = (U16)(crc^(U8)(crc & 0xff) >> 4);
		crc = (U16)(crc^(crc << 12));
		crc = (U16)(crc^((crc & 0xff) << 5));
	}
	return crc;
}

/*-----------------------------------------------------------------------------
* ����:	glbVariable_CheckCRC
* ����:	CRCУ��
* ����:	kAddress----��У��buffer
*		size--------buffer��С
* ����:	16λCRCУ����
*----------------------------------------------------------------------------*/
U8 glbVariable_CheckCRC(U8 * kAddress,U32 size)
{
	U16 *paddress,crc;
	U32 tempSize;
	
	tempSize = size -2;
	paddress  = ( U16 *)(kAddress  + tempSize );
	
	crc = glbVariable_CalculateCRC((U8 *)kAddress,tempSize);
	
	if ( (*paddress) == crc)
	{
		return CRC_OK;
	}
	else
	{
		return  CRC_ERR;
	}
}

/*-----------------------------------------------------------------------------
* ����:	glb_SetPersistentParameter
* ����:	�����ϵͳ������Ŀ¼��Ϣ����������Ϣ��Favorite��Ϣ��¼���ļ������Ϣ��
*		��Ƶ��Ƶ�ļ���Map��Sdram Parameter Area��
* ����: source		--	ָ������߸����Ļ��棬����Ҫ���������
*		offset		--	���persistent area��ƫ���������ֽڼ�
*		len			--	���ȣ����ֽڼ�
* ����:	������
*----------------------------------------------------------------------------*/
int glb_SetPersistentParameter( char* source, int offset, int len )
{
	memcpy((char*)((char *)hsaPersistentParameters_GetBuffer()+offset),source,len);	
	return cSSA_OK;
}

/*-----------------------------------------------------------------------------
* ����:	glb_GetPersistentParameter
* ����:	��Sdram Parameter Area�л�ȡϵͳ������Ŀ¼��Ϣ����������Ϣ��Favorite��Ϣ��¼���ļ������Ϣ��
*		��Ƶ��Ƶ�ļ���Map��
* ����: destination	--	ָ������߸����Ļ��棬��ȡ��������ݽ�����˻���
*		offset		--	���persistent area��ƫ���������ֽڼ�
*		len			--	���ȣ����ֽڼ�
* ����:	������
*----------------------------------------------------------------------------*/
int glb_GetPersistentParameter( char* destination, int offset, int len )
{
	memcpy(destination,(char*)((char *)hsaPersistentParameters_GetBuffer()+offset),len);
	
	return cSSA_OK;
}

/*-----------------------------------------------------------------------------
* ����:	glb_GetPersistentParameterFromNand
* ����:	��Nand PersistentArea��һ���Ի�ȡϵͳ������Ŀ¼��Ϣ����������Ϣ��Favorite��Ϣ��¼���ļ������Ϣ��
*		��Ƶ��Ƶ�ļ���Map��Sdram Parameter Area
* ����: none
* ����:	������
*----------------------------------------------------------------------------*/
int glb_GetPersistentParameterFromNand(void)
{
	int rc;
	
	Mutex_Protect();
	memset((char*)(hsaPersistentParameters_GetBuffer()), 0, cPARAMETER_TOTAL_SIZE);
	rc = HSA_GetPersistentParameter(0,(char*)(hsaPersistentParameters_GetBuffer()), 0, cPARAMETER_TOTAL_SIZE);
	Mutex_Unprotect();
	
	return rc;
}

/*-----------------------------------------------------------------------------
* ����:	glb_SetPersistentParameterToNand
* ����:	��Sdram Parameter Area��ϵͳ������Ŀ¼��Ϣ����������Ϣ��Favorite��Ϣ��¼���ļ������Ϣ��
*		��Ƶ��Ƶ�ļ���Map��һ����д�뵽Nand PersistentArea
* ����: none
* ����:	������
*----------------------------------------------------------------------------*/
int glb_SetPersistentParameterToNand(void)
{
	int rc = HY_OK;
	glb_HycParameter_t curPara;
		
	HSA_GetPersistentParameter(0,(char*)&curPara, 0, sizeof(glb_HycParameter_t));
	
	if(memcmp((U8 *)&curPara,(char *)(hsaPersistentParameters_GetBuffer()),sizeof(glb_HycParameter_t)) != 0)
	{
		rc = HSA_SetPersistentParameter(0,(char *)(hsaPersistentParameters_GetBuffer()), 0, cPARAMETER_TOTAL_SIZE);
	}

	return rc;
}

int LcdModule_Get_ShowType(void)
{
    return  gScreen_Display_Mode;
}

/*-----------------------------------------------------------------------------
* ����:	GetResouceBmpOffset
* ����:	�����Դ��nand reserve���е�OffSet
* ����:	none
* ����:	>0----��Դ��nand reserve���е�OffSet
*		-1----nand reserve��������Դ
*----------------------------------------------------------------------------*/
S32 GetResouceBmpOffset(void)
{
	return gs32BmpOffset;
}

/*-----------------------------------------------------------------------------
* ����:	SetResouceBmpOffset
* ����:	������Դ��nand reserve���е�OffSet
* ����:	offset
* ����:	none
*----------------------------------------------------------------------------*/
void SetResouceBmpOffset(S32 offset)
{
	gs32BmpOffset = offset;
}

/*-----------------------------------------------------------------------------
* ����:	lcdRefreshQue_get
* ����:	��ȡ����ˢ��ʹ��/��ֹ״̬
* ����:	none
* ����:	0:��ֹ����ˢ����1:ʹ��
*----------------------------------------------------------------------------*/
BOOL lcdRefreshQue_get(void)
{
	return gRefreshNeedQue;
}

/*-----------------------------------------------------------------------------
* ����:	lcdRefreshQue_set
* ����:	����ʹ��/��ֹ����ˢ��
* ����:	enable-----0:��ֹ����ˢ����1:ʹ��
* ����:	none
*----------------------------------------------------------------------------*/
void lcdRefreshQue_set(BOOL enable)
{
	gRefreshNeedQue = enable;
}

/*-----------------------------------------------------------------------------
* ����:	rtcSolar_get
* ����:	��õ�ǰʱ��(����)
* ����:	pTime
* ����:	ds
*----------------------------------------------------------------------------*/
void rtcSolar_get(RtcTime_st *psolarTime)
{
	if (psolarTime == NULL) return;
	memcpy(psolarTime, &gtRtc_solar, sizeof(RtcTime_st));
}

