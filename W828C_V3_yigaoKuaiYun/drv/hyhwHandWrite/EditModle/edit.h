#ifndef __EDIT_H__
#define __EDIT_H__

/*--------------------------------------------------------------------------
*INCLUDES
--------------------------------------------------------------------------*/
#include "glbVariable.h"
#include "grap_api.h"
#include "grap_view.h"
#include "base.h"

#define INPUT_BIG_ABS_START     (7)
#define INPUT_BIG_ABS_LEN		(30)
#define INPUT_SMAL_ABS_START    (INPUT_BIG_ABS_START+INPUT_BIG_ABS_LEN)
#define INPUT_SMAL_ABS_LEN		(30)
#define INPUT_DIGIT_START  		(INPUT_SMAL_ABS_START+INPUT_SMAL_ABS_LEN)
#define INPUT_DIGIT_LEN			(16)
#define INPUT_SIGN_START  		(INPUT_DIGIT_START+INPUT_DIGIT_LEN)
#define INPUT_SIGN_LEN			(36)
#define INPUT_WWW_START  		(INPUT_SIGN_START+INPUT_SIGN_LEN)
#define INPUT_WWW_LEN			(15)

#define INPUT_HANDW_START  		(INPUT_WWW_START+INPUT_WWW_LEN)
#define INPUT_HANDW_LEN			(12)//(18) //12

#define INPUT_IME_START  		(INPUT_HANDW_START+INPUT_HANDW_LEN)
#define INPUT_IME_LEN			(37) 

#define INPUT_SEL_WORD_START    (INPUT_IME_START+INPUT_IME_LEN)  //183
#define INPUT_SEL_WORD_LEN		(32)


#define INPUT_BTN_TOTAL         (INPUT_SEL_WORD_START+INPUT_SEL_WORD_LEN)  //215

#define INPUT_TOTAL_WIDGET      (INPUT_BTN_TOTAL+1)

#define INPUT_TOTAL_CTRL        (9)

//ɾ���ַ���ֵ
#define INPUT_VALUE_DEL	  (10)

//������ư��ģʽ
#define INPUT_CTR_BIGABC  1
#define INPUT_CTR_SMAABC  2
#define INPUT_CTR_DIGIT   3 
#define INPUT_CTR_SIGN    4
#define INPUT_CTR_IME     5 /* ƴ�� */
#define INPUT_CTR_CHI     6 /* ��д */
#define INPUT_CTR_WWW     7
#define INPUT_CTR_PREVFOCUS 8
#define INPUT_CTR_NEXTFOCUS 9

//������ʾ���ַ��Ŀռ����
#define INPUT_SHOW_BUF_TOTAL  (40)

//����ʹ�ܵ����ؼ���
#define INPUT_REGISTER_MAX_WIDGET  (20)

typedef S32 (*Button_Return_Word)(void *pView, U16 word);
typedef S32 (*edit_change_focus)(void *pView, int type);

#define  MAX_SHOW_HZ			6
#define  MAX_IDENTIFY_HZ		5
#define ARROW_F_TAIL			20
#define ARROW_T_FRONT			220



//���ڹ����ڲ�ƴ�����뷨����
typedef struct _tIME_SHOW
{
    char *pIme;
    char inBuf[9];//�����BUF
    U8   pageIndex;
    U8   type; //Ŀǰbuf��״̬ 0��ƴ�� 1��������
}tIME_SHOW;

typedef struct _tINPUT_BMP_CLR
{
    U16 press;  //press color
    U16 normal; //normal color
    U16 writeClr; //д��ʱ�����ɫ  
    U8  inputMode; //
    U8  status;  //���뷨��״̬ �򿪻��ǹر�   
}tINPUT_BMP_CTR;

typedef struct _tASSN_CTR
{
    U8   flag;//�Ƿ��������
    U32  times;//��������Ĵ���
    U16  lastWord; //�ϴ�������ַ�
    U8   assnBuf[INPUT_SHOW_BUF_TOTAL*2];//��������buf
    U8   validTotal;//��Ч�ַ��ĸ���
    U8   willGetNum;  //��Ҫ�õ��ĺ��ָ���
}tASSN_CTR;

typedef struct _tSEL_WORD_CTR
{
    U8  type;  //0 û��������ѡ��  1:ƴ�� 2:��д
    U8  enable; //�Ƿ���������ѡ�� 0 �� 1
}tSEL_WORD_CTR;

typedef struct _tCTR_PANEL
{
    U8 inputType;  //���뷨����  
    U8 alphaDisp;//�Ƿ���ʾ 1����ʾ 0:����ʾ
    tINPUT_BMP_CTR tSysCtr; //�ܵ�������
    tGRAP_RECT	tRect;
    U8  *pBtnEnable[250];//btnʹ�ܿ���	  //INPUT_BTN_TOTAL
    U8  *pHandLabEnable; //��д��ʹ�ܿ���
    
    tGRAPEDIT  *pEditInput;  //�������뷨ʹ��
    U8  *pCptBtn;  //�������뷨ʹ��  ������ɰ���
    
    U8  BtnCharShow[INPUT_SHOW_BUF_TOTAL][3];//�ַ���ʾ��buf�ռ�
    U8  HandWordShowFlag;//��дʶ��ʱ���Ƿ���ʾ����
    tIME_SHOW tIme;
    Button_Return_Word edit_return_word;
    tASSN_CTR tAssnCtr; //�������뷨����
    tSEL_WORD_CTR tSelWord;//������ѡ�ֿ���
    edit_change_focus edit_change_widget_focus;
    tGRAPVIEW *pWidget[INPUT_REGISTER_MAX_WIDGET];
    U8        curRegCnt;
    
    //�������õ�btn����
    tGRAPBUTTON *pShareBtn[2];
    
    //����
    U8 Is_WriteHand;//0:����ĺ���   1����дʶ��ĺ���
    tGRAPBUTTON *gu_ShowHZ_sx[MAX_SHOW_HZ];
	tGRAPBUTTON *gu_ShowHZ_arrow[2];
	tGRAPBUTTON *gu_ShowHZ_identify[MAX_IDENTIFY_HZ];
	tGRAPBUTTON *gu_DeleteBtn;
	
}tCTR_PANEL;

extern const U16 gSelBtnShowId[];	  //INPUT_SEL_WORD_LEN
extern const U16 gImeBtnShowId[];
extern const U16 gHandBtnShowId[];
extern tINPUT_BMP_CTR gtBtnBmpCtr;


/*---------------------------------------------------------
*����: edit_get_input_status
*����: get edit status 
*����: none
*����: 1 : open 0:close
*---------------------------------------------------------*/
int  edit_get_input_status(tGRAPDESKBOX *pDesk);

/*---------------------------------------------------------
*����: edit_auto_change_input_mode
*����: aoto change mode: close or open
*����: none
*����: none
*---------------------------------------------------------*/
void edit_auto_change_input_mode(tGRAPDESKBOX *pDesk, U16 color);

/*---------------------------------------------------------
*����: edit_get_special_string
*����: �������ǳ��ַ�����ʱ�򷵻����ֵ�����򷵻�none
*����: none
*����: none
*---------------------------------------------------------*/
char *edit_get_special_string(U16 word, void *pView);

/*---------------------------------------------------------
*����: edit_modify_input_mode
*����: �ı����뷽ʽ
*����: none
*����: none
*---------------------------------------------------------*/
void edit_modify_input_mode(tGRAPDESKBOX *pDesk, int mode, U16 color);

/*---------------------------------------------------------
*����: edit_creat_panel   
*����: �������뷨			 
*����: none
*����: ����
*---------------------------------------------------------*/
void edit_creat_panel(tGRAPDESKBOX *pDesk, int mode, Button_Return_Word returnWord, edit_change_focus changeFocus);

#endif
