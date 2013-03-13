#ifndef _IUPUTRANGE_API_H_
#define _IUPUTRANGE_API_H_

#include "predefined.h"
#include "hyTypes.h"


#define OLD_COOTEK_START	0x02130e07
#define OLD_COOTEK_END		(OLD_COOTEK_START+0x0019c1cb)
#define COOTEK_START		0x02130c07
#define COOTEK_END			(COOTEK_START+0x004aa83a)

#define COOTEK_START_USER	0
#define COOTEK_END_USER		30


#define GET_MAXNUM  10

typedef struct _return_word
{
	U8 HanZi[MAX_CANDIDATE_ITEM_SIZE*GET_MAXNUM][MAX_WORD_ARRAY_SIZE];
	U8 PinYing[MAX_FILTER_LIST_SIZE][PINYIN_INDEX_PINYIN_LETTER_SIZE];
	U8 ShowInput[MAX_WORD_ARRAY_SIZE];
	
	U32 HZ_Num;
	U32 PY_Num;
	U32 Show_Num;

}tReturnWord;



extern U32	gu_CootekStart;
extern U32  gu_CootekEnd;




/*************************************
* ����: InputRange_ClearKeyArray
* ����: ���������¼
* ����: none
* ����: none
*************************************/
void InputRange_ClearKeyArray(void);


/*************************************
* ����: InputRange_Init
* ����: ��ʼ���ֵ�
* ����: none
* ����: none
*************************************/
int InputRange_Init(void);


/*************************************
* ����: InputRange_CloseFd
* ����: �ر��ֵ�
* ����: none
* ����: none
*************************************/
void InputRange_CloseFd(void);


/*************************************
* ����: InputRange_DeleteOneKey
* ����: ɾ��һ����ֵ
* ����: returnValue�����ؽ��
* ����: none
*************************************/
void InputRange_DeleteOneKey(tReturnWord *returnValue);


/*************************************
* ����: InputRange_GetWord
* ����: �����ֵ����ö�Ӧ�ĺ���
* ����: key����ֵ   returnValue�����ؽ��
* ����: none
*************************************/
void InputRange_GetWord(int key, tReturnWord *returnValue);


/*************************************
* ����: InputRange_GetNextWord
* ����: ����ǰһ���ֻ�ʣ����������ֻ��
* ����: firstWord��ǰһ����   returnValue�����ؽ��
* ����: 0:�ɹ�  -1��ʧ��
*************************************/
int InputRange_GetNextWord(U8 *firstWord, tReturnWord *returnValue);


/*************************************
* ����: InputRange_GetExactlyWord
* ����: ����ƴ����ȡ��ȷ�ĺ���
* ����: inputPY��ѡ�е�ƴ��   returnValue�����ؽ��
* ����: none
*************************************/
void InputRange_GetExactlyWord(U8 *inputPY, tReturnWord *returnValue);


/*************************************
* ����: InputRange_GetPriority
* ����: ��ô�������ȼ�
* ����: input_Word��Ŀ�����
* ����: ���ȼ�ֵ
*************************************/
int InputRange_GetPriority(U8 *input_Word);


/*************************************
* ����: InputRange_ChangePriority
* ����: ����input_Word�����ȼ�
* ����: input_Word��Ҫ�����Ĵ�   
		manualAdjustFlag��0  �Զ��������ȼ�
                          1  �����ȼ�����ΪtargetPriority
        targetPriority��Ҫ���������ȼ�
* ����: none
*************************************/
void InputRange_ChangePriority(U8 *input_Word, char manualAdjustFlag, int targetPriority);


/*************************************
* ����: InputRange_IsWordIn
* ����: �ж���������Ƿ�����ֵ���
* ����: input_Word��Ŀ�����
* ����: TRUE������   FALSE ������
*************************************/
BOOL InputRange_IsWordIn(U8 *input_Word);


/*************************************
* ����: InputRange_IsWordInUser
* ����: �ж���������Ƿ�����û��ֵ���
* ����: input_Word��Ŀ�����
* ����: TRUE������   FALSE ������
*************************************/
BOOL InputRange_IsWordInUser(U8 *input_Word);


/*************************************
* ����: InputRange_AddToUser
* ����: ��input_Word��ӵ��ֵ���
* ����: input_Word��Ҫ��ӵĴ�
* ����: none
*************************************/
void InputRange_AddToUser(U8 *input_Word);


/*************************************
* ����: InputRange_RemoveFromUser
* ����: ��input_Word���û��ֵ���ɾ��
* ����: input_Word��Ҫɾ���Ĵ�
* ����: none
*************************************/
void InputRange_RemoveFromUser(U8 *input_Word);


/*************************************
* ����: InputRange_NextOrBefore
* ����: ��ǰ������
* ����: direction:����   1�����   0����ǰ
		returnValue�����ؽ��
* ����: none
*************************************/
void InputRange_NextOrBefore(int direction, tReturnWord *returnValue);

#endif