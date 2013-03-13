#ifndef _HYC_T9_H_
#define _HYC_T9_H_


#include "hyTypes.h"
#include "grap_api.h"

//#define USE_OURINPUT


#ifndef USE_OURINPUT

#include "dictionary_i.h"
#include "InputRange_api.h"

#endif
/*---------------------------------------------------------------------------------*/

#if 0

typedef signed char			S8,		*pS8,    Int8,   *pInt8;     /*  8 bit   signed integer */
typedef unsigned char		U8,		*pU8,    UInt8,  *pUInt8;    /*  8 bit unsigned integer */
typedef short				S16,	*pS16,   Int16,  *pInt16;    /* 16 bit   signed integer */
typedef int					S32,	*pS32,   Int32,  *pInt32;    /* 32 bit   signed integer */
typedef unsigned short		U16,	*pU16,   UInt16, *pUInt16;   /* 16 bit unsigned integer */
typedef unsigned int		U32,	*pU32,   UInt32, *pUInt32;   /* 32 bit unsigned integer */
typedef void            	Void,   *pVoid;     /* Void (typeless) */
typedef float           	Float,  *pFloat;    /* 32 bit floating point */
typedef double          	Double, *pDouble;   /* 32/64 bit floating point */
typedef char            	Char,   *pChar;     /* character, character array ptr */
typedef char           		*String, **pString; /* Null terminated 8 bit char str, */
typedef char const     		*ConstString;		/* Null term 8 bit char str ptr */
typedef void 		  		(* PFN)(void);      /* PFN is pointer to function */

#endif

#define T9_HZ_PATH			"C:/ϵͳ/t9.bin"


#define KEY2PY_TOTAL			223
#define PY2HZ_TOTAL				406

#define T9_HZOFFSET				34747371
#define T9_HZSIZE				55324
#define T9_DIS_HZMAX			1024
#define T9_HZ_TOTAL				55322
#define T9_SIGN_NUM				42



//׼������λ��  ��Ҫ����������λ�õ���
#define T9_DISPLAY_Y1		260-25
#define T9_DISPLAY_Y2		100-25


#define T9_DISPLAY_X		0
#define T9_DISPLAY_H		60+25
#define T9_DISPLAY_W		240


//#define T9_CODE_X			0//��һ���������ĸ
//#define T9_CODE_Y_OFF		0//������뷨�ؼ���ƫ��
#define T9_DIS_PY_MAX		10	//һ������ʾ��ƴ������ 40��  ���10
#define T9_PY_STEP			10
#define T9_PY_H				24
#define T9_PY_MINW			40

#define T9_DIS_HZ_MAX		6	//һ������ʾ�ĺ��ָ��� 33��  ���8
#define T9_HZ_STEP			8
#define T9_HZ_H				30
#define T9_HZ_MINW			33

//��ͷ 16X16
#define T9_ARROW_W			16
#define T9_ARROW_H			16
#define FRONT_ARROW			20
#define BACK_ARROW			224
#define PY_ARROW_Y			267
#define HZ_ARROW_Y			295


#define T9_ARROWUP_X		200//ƴ����ʾ��,�ұ��ϼ�ͷλ��
#define T9_ARROWDOWN_X		220

#define T9_ARROW_YOFF		4

//Yֻ��ƫ�� 
#define T9_SHOW_YOFF		25 
#define T9_PY_YOFF			28
#define T9_HLINE_YOFF_1		26
#define T9_HLINE_YOFF_2		55
#define T9_HZ_YOFF			59


#define T9_SIGN_MAX			42
#define T9_SIGN_DIS_MAX		16

#define T9_SIGN_H			30
#define T9_SIGN_W			30

#define T9_SELECT_COLOR		COLOR_YELLOW
#define T9_UNSELECT_COLOR	COLOR_RED

///////////////////////////////////////////////////////////////////////////////////

typedef S32 (*t9_Return)(void *pView, U8 *pWord);

///////////////////////////////////////////////////////////////////////////////////
extern const U8 * const t9_key2py[KEY2PY_TOTAL][7];
extern const U32 t9_HzArray[PY2HZ_TOTAL][2];
extern const U8  t9_Key2PyArray[];
extern const int t9_Py2HzArray[];
extern const U8 t9_sign[];
///////////////////////////////////////////////////////////////////////////////////
typedef struct _tT9SIGN
{
	
	U8	SignFlag;	//�Ƿ���ʾ����
	U8	SignTotal;	//��������
	U8	SignStart;	//��ҳ��һ������
	U8	SignSelect;	//��ǰѡ��ķ���
	
}tT9SIGN;


typedef struct _tT9ASSN_CTR
{
	U8		assnflag;	//�Ƿ��������
	U8		assntotal;	//������Ч�����ָ���
	U16		assntimes;	//����Ĵ��� (��0��ʼ)
	U16		assnstart;	//��ҳ��һ������������
	U16		assnselect;	//��ǰѡ�е�������
	U8		assnbuf[80];//��������ֵ�buf
	
}tT9ASSN_CTR;


#ifdef USE_OURINPUT

#define MAX_CODE_NUM		6

typedef struct _tT9KEY
{
	U8		keynum;		//��ǰ�����key����Ŀ �������6��
	U8		keycode[7];	//����ļ������2--9  ���6��(ƴ������󳤶�)
	U8		tPy[7][7];	//ָ��ƴ����� ���6�����
	U8		tHzStr[T9_DIS_HZMAX];	//����
	
	U16		hzstart;	//��ҳ��һ�����ֵ�����
	U16		hOptal;	//��������
	U16		hzselect;	//��ǰѡ�к���
	
	U8		pystart;	//��ҳ��һ��ƴ��������
	U8		pytotal;	//ƴ������
	U8		pyselect;	//��ǰѡ��ƴ��
	
	struct _tT9ASSN_CTR	assn;
	
	void    *pWidget;
	t9_Return t9_return;
	
}tT9KEY;

#else

#define MAX_CODE_NUM		20

typedef struct _tT9KEY
{
	U8		keynum;		//��ǰ�����key����Ŀ
	
	U8		hzstart;	//��ҳ��һ�����ֵ�����
	U8		hOptal;	//��������
	U8		hzselect;	//��ǰѡ�к���
	
	U8		pystart;	//��ҳ��һ��ƴ��������
	U8		pytotal;	//ƴ������
	U8		pyselect;	//��ǰѡ��ƴ��
	
	U8		sistart;	//
	U8		sitotal;	
	U8		siselect;
	
	U8		assnflag;  //����
	U8		signflag;  //����
	
	struct _return_word result;
	
	void    *pWidget;
	t9_Return t9_return;
	
	tGRAPLABEL	*pT9pyLable[T9_DIS_PY_MAX];
	tGRAPLABEL	*pT9hzLable[T9_DIS_HZ_MAX];
	tGRAPLABEL	*pT9ArrowLable[2];
	tGRAPLABEL	*pT9SignLable[T9_SIGN_DIS_MAX];
	
}tT9KEY;

#endif

//////////////////////////////////////////////////////////////////////////////////




#endif