#ifndef BARCODE_API_H
#define BARCODE_API_H


typedef void* HANDLE;


#define BARCODE_SUCCESS 0
#define BARCODE_ERROR -1
#define BARCODE_ERR_DEC_MORE_DATA -2
#define BARCODE_ERR_DEC_CHECK_CODE -3

#define CODE_CTRL_FNC2 206 //code128 FNC2
#define CODE_CTRL_FNC3 207 //code128 FNC3
#define CODE_CTRL_FNC4 208 //code128 FNC4


/*************************************************************/


typedef enum barcode_pic_type
{
	BARCODE_PIC_TYPE_RGB888 = 0,
	BARCODE_PIC_TYPE_Y,
	BARCODE_PIC_TYPE_1BIT, //8bits every byte, lowest bit first
	BARCODE_PIC_TYPE_STRIPE
}BARCODE_PIC_TYPE;


typedef enum barcode_code_type
{
	BARCODE_CODE_TYPE_EAN8 = 0,			//0
	BARCODE_CODE_TYPE_EAN13,			//1
	BARCODE_CODE_TYPE_EAN128,			//2
	BARCODE_CODE_TYPE_UPCA,				//3
	BARCODE_CODE_TYPE_UPCE,				//4
	BARCODE_CODE_TYPE_CODE11,			//5
	BARCODE_CODE_TYPE_CODE39,			//6
	BARCODE_CODE_TYPE_CODE93,			//7
	BARCODE_CODE_TYPE_CODE128,			//8
	BARCODE_CODE_TYPE_GS1_DATABAR,		//9
	BARCODE_CODE_TYPE_GS1_LIMITED,		//10
	BARCODE_CODE_TYPE_GS1_EXPANDED,		//11
	BARCODE_CODE_TYPE_ITF25,			//12
	BARCODE_CODE_TYPE_INDUS25,			//13
	BARCODE_CODE_TYPE_IATA25,			//14
	BARCODE_CODE_TYPE_MATRIX25,			//15
	BARCODE_CODE_TYPE_CHINESE25,		//16
	BARCODE_CODE_TYPE_CODABAR,			//17
	BARCODE_CODE_TYPE_MSI,				//18
	BARCODE_CODE_TYPE_MAX
}BARCODE_CODE_TYPE;


typedef enum barcode_scan_type
{
	BARCODE_SCAN_TYPE_EAN = 0,			//0
	BARCODE_SCAN_TYPE_UPC,				//1
	BARCODE_SCAN_TYPE_CODE39,			//2
	BARCODE_SCAN_TYPE_CODE128,			//3
	BARCODE_SCAN_TYPE_CODE93,			//4
	BARCODE_SCAN_TYPE_GS1,				//5	
	BARCODE_SCAN_TYPE_CODE25,			//6
	BARCODE_SCAN_TYPE_CODE11,			//7
	BARCODE_SCAN_TYPE_CODABAR,			//8
	BARCODE_SCAN_TYPE_MSI,				//9
	BARCODE_SCAN_TYPE_MAX
}BARCODE_SCAN_TYPE;


typedef enum barcode_dec_mode
{
	BARCODE_DEC_MODE_SINGLE_SINGLE = 0,	//normalize with single stripe, decode with single stripe
	BARCODE_DEC_MODE_SINGLE_DISTANCE,	//normalize with single stripe, decode with min code distance
	BARCODE_DEC_MODE_COMBINED_SINGLE,	//normalize with adjacent stripe, decode with single stripe
	BARCODE_DEC_MODE_COMBINED_COMBINED,	//normalize with adjacent stripe, decode with adjacent stripe
	BARCODE_DEC_MODE_BINARY_SINGLE,		//normalize only narrow and wide stripe, decode with single stripe
	BARCODE_DEC_MODE_MAX
}BARCODE_DEC_MODE;


typedef enum barcode_dec_reliable
{
	BARCODE_DEC_RELIABLE_LOW = 0,
	BARCODE_DEC_RELIABLE_MIDDLE,
	BARCODE_DEC_RELIABLE_HIGH,
	BARCODE_DEC_RELIABLE_MAX
}BARCODE_DEC_RELIABLE;


typedef enum barcode_opt_type
{
	BARCODE_OPT_TYPE_SCAN = 0,
	BARCODE_OPT_TYPE_DEC
}BARCODE_OPT_TYPE;


/*************************************************************/


typedef struct barcode_init_param
{
	void		*(*pMalloc)(U32 uSize);
	void		(*pFree)(void *pBuf);
}BARCODE_INIT_PARAM;


typedef struct barcode_buf
{
	U32			uLen;
	void		*pData;
}BARCODE_BUF;

typedef struct barcode_image
{
	U16			uWidth;
	U16			uHeight;

	U16			uStartX;
	U16			uStartY;
	U16			uEndX;
	U16			uEndY;

	void		*pData;
}BARCODE_IMAGE;

typedef struct barcode_pic
{
	BARCODE_PIC_TYPE eType;

	union
	{
		BARCODE_BUF tBBuf;//BARCODE_PIC_TYPE_BINARY_1BIT, BARCODE_PIC_TYPE_STRIPE
		BARCODE_IMAGE tBImage;//BARCODE_PIC_TYPE_RGB888, BARCODE_PIC_TYPE_Y
	}tData;	
}BARCODE_PIC;


typedef struct barcode_code
{
	BARCODE_CODE_TYPE eType;

	U8			*pBuf;
	U32			uLen;
	U32			uStripeNum;
}BARCODE_CODE;


#define BARCODE_OPT_SCAN_FLAG_BIDIRECT (1<<0)//�Ƿ�����ݳ���˫�����
#define BARCODE_OPT_SCAN_FLAG_FILTER (1<<1)//�Ƿ���ɨ��ǰ��������
#define BARCODE_OPT_SCAN_FLAG_REVERSE (1<<2)//�Ƿ���з�ɫ���

typedef struct barcode_opt_scan
{
	U32			uFlag;

	U16			uScanSeqNum;
	BARCODE_SCAN_TYPE eScanSeq[BARCODE_SCAN_TYPE_MAX];
}BARCODE_OPT_SCAN;


//decode flag
//bit0 - bit7, control
#define BARCODE_OPT_DEC_FLAG_ENABLE (1<<0)//�Ƿ����ý�����
#define BARCODE_OPT_DEC_FLAG_CHECK_ECC (1<<1)//�������У��, �Ƿ����У��
#define BARCODE_OPT_DEC_FLAG_FULL_ASCII (1<<2)//�������ת��, �Ƿ�ת��Full ASCII
#define BARCODE_OPT_DEC_FLAG_SUPPLEMENTALS (1<<3)//������Խ���supplementals, �Ƿ����supplementals

//bit8 - bit15, output
#define BARCODE_OPT_DEC_FLAG_OUTPUT_ECC (1<<8)//�����У����, �Ƿ����У����
#define BARCODE_OPT_DEC_FLAG_OUTPUT_PREAMBLE (1<<9)//�����ǰ����, �Ƿ����ǰ����

typedef struct barcode_opt_dec
{
	BARCODE_CODE_TYPE eType;//������ǰ��������������
	BARCODE_DEC_MODE eAnchorMode;//��λ�����뷽ʽ
	BARCODE_DEC_MODE eDataMode;//���ݽ��뷽ʽ
	BARCODE_DEC_RELIABLE eReliable;//����ɿ���
	
	U8			uCheckCount;//uCheckRegion�ν�����, �����uCheckCount�γɹ��ͷ��ؽ������
	U8			uCheckRegion;

	U8			uMinLen;//��С���볤��
	U8			uMaxLen;//������볤��

	U32			uFlag;
}BARCODE_OPT_DEC;


typedef struct barcode_opt
{
	BARCODE_OPT_TYPE eType;

	union
	{
		BARCODE_OPT_SCAN tOptScan;
		BARCODE_OPT_DEC tOptDec;
	}tParam;
}BARCODE_OPT;


/*************************************************************/


HANDLE Barcode_OpenDec(BARCODE_INIT_PARAM *pBParam);
HANDLE Barcode_OpenEnc(BARCODE_INIT_PARAM *pBParam);
void Barcode_Close(HANDLE hBc);
int Barcode_Decode(HANDLE hBc,BARCODE_PIC *pBPic,BARCODE_CODE *pBCode);
int Barcode_Encode(HANDLE hBc,BARCODE_CODE *pBCode,BARCODE_PIC *pBPic);
int Barcode_SetOpt(HANDLE hBc,BARCODE_OPT *pBOpt);
const U8 *Barcode_GetVerInfo();



#endif //BARCODE_API_H

