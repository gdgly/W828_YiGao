#include "dictionary_i.h"
#include "InputRange_api.h"


int InuptKey_Array[MAX_INPUT_CODE_EXPAND_LENGTH+1];
struct ImageFileDescriptor gu_ChineseImages[5];
DICTIONARY_HANDLE gu_CineseHandle;
static unsigned int gu_TotNumber;
static unsigned int gu_RightNum;


#pragma comment (lib, "win32vs2008.lib")
//#pragma comment (lib, "libtouchpal.lib")

// define the flag to only enable features for basic editon of prediction engine
#define ENGINE_EDITION_BASIC

// the batch size the UI asks dictionary to return
// usually UI just needs limited word candidates being returned for each page
// the MAX page size is defined as 9 in this edition and could be adjusted at compile time.
#define PAGE_SIZE 12 

#define MAX_DISPLAY_HZ_LEN		14


extern int CooTekReadStart;
extern int CooTekReadStart_User;


//==========================================
//==========================================
// expand callback function implementations
// these methods will are called by engine in SearchDictionary method.
// they are used to translate input code to corresponding characters.
// they are the core magic to implement predictive input, mistyping correction 
//     and accent letter mapping!!!

// Expand callback function for QWERTY layout without mistyping correction
// it receives input code and directly put them as result bcos there is no mistypings
// For example, if you call SearchDictionary with "ack" as input code with this expand callback function,
//     the engine will call this function 3 times with "a", "c" and "k" as code parameter.
//     Given there is no mistyping corrections, each keycode directly maps to one letter, so, just put
//     the code directly as result parameter


int GetInputCodeExpandResultTestQWERTY(int code, WCHAR result[], void* context)
{
    CT_MEMSET(result, 0, sizeof(result));
    result[0] = code;
    return 1;//return the length of result, ex: in t+ layout, we should return 2
}

// Expand callback function for PhonePad layout
// it receives input code and maps it to correct letters on the PhonePad button.
// For example, if you call SearchDictionary with "ad" as input code with this expand callback function,
//     the engine will call this function 2 times with "a" and "d" as code parameters.
//     On PhonePad layout, each keycode should be translated to 3 or 4 letters.
int GetInputCodeExpandResultTestPhonePad(int code, WCHAR result[], void* context)
{
    int i =3;
    CT_MEMSET(result, 0, sizeof(result));
    
     switch(code)
     {
     case 2:
         result[0] = 'a';
         result[1] = 'b';
         result[2] = 'c';
         break;
     case 3:
         result[0] = 'd';
         result[1] = 'e';
         result[2] = 'f';
         break;
     case 4:
         result[0] = 'g';
         result[1] = 'h';
         result[2] = 'i';
         break;
     case 5:
         result[0] = 'j';
         result[1] = 'k';
         result[2] = 'l';
         break;
     case 6:
         result[0] = 'm';
         result[1] = 'n';
         result[2] = 'o';
         break;
     case 7:
         result[0] = 'p';
         result[1] = 'q';
         result[2] = 'r';
         result[3] = 's';
         i = 4;
         break;
     case 8:
         result[0] = 't';
         result[1] = 'u';
         result[2] = 'v';
         break;
     case 9:
         result[0] = 'w';
         result[1] = 'x';
         result[2] = 'y';
         result[3] = 'z';
         i = 4;
         break;
     default:
         result[0] = code;
         i=1;
         break;
     }
     return i;
}

// Expand callback function for QWERTY layout with mistyping correction
// For example, if you call SearchDictionary with "rgus" as input code with this expand callback function,
//     the engine will call this function 4 times with "r", "g", "u" and "s" as code parameters.
//     On QWERTY with mistyping correction, "r" could be "r", "e" or "t"; "g" -> "fgh"; u -> "yui"; "s"->"asd",
int GetInputCodeExpandResultTestQWERTYMistypingCorrection(int code, WCHAR result[], void* context)
{
    int i;
    char ErrorCorrection[26][4] = {{'s', 0}, {'v', 'n'}, {'x', 'v'}, {'s', 'f'},
        {'w', 'r'}, {'d', 'g'}, {'f', 'h'}, {'g', 'j'}, {'u', 'o'},
        {'h', 'k'}, {'j', 'l'}, {'k', 0 }, {'n', 0}, {'b', 'm'}, {'i', 'p'}, {'o', 0},
        {'w', 0}, {'e', 't'}, {'a', 'd'}, {'r', 'y'}, {'y', 'i'}, {'c', 'b'}, {'q', 'e'},
        {'z', 'c'}, {'t', 'u'}, {'x', 0}
    };

    CT_MEMSET(result, 0, sizeof(result));

    if (code >= 'A' && code <= 'Z')
    {
        code = code + ('a' - 'A');
    }

    result[0] = code;

    if (code < 'a' || code > 'z')
    {
        return 1;
    }

    for (i = 0; i < 2; i++)
    {
        if (ErrorCorrection[code-'a'][i] != 0)
        {
            result[i+1] = ErrorCorrection[code-'a'][i];
        }
        else
        {
            break;
        }
    }

    return ++i;
}

// end of expand callback functions
//==========================================
//==========================================


// initialize English dictionary image files
void InitEnglishImageFileDescriptors(struct ImageFileDescriptor images[])
{
    CT_MEMSET(images, 0, sizeof(struct ImageFileDescriptor) * 5);

    // please input the path of the english.img file
    images[0].fHandle = CT_FOPEN("C:/ϵͳ/InputRange/eng_data.img", "r+b", O_RDWR);
    images[1].fHandle = CT_FOPEN("C:/ϵͳ/InputRange/english_usr.img", "r+b", O_RDWR); 
    images[2].fHandle = CT_FOPEN("C:/ϵͳ/InputRange/eng_p.img", "r+b", O_RDWR);
    images[3].fHandle = CT_FOPEN("C:/ϵͳ/InputRange/eng_pi.img", "r+b", O_RDWR);
    images[4].fHandle = CT_FOPEN("C:/ϵͳ/InputRange/eng_pl.img", "r+b", O_RDWR);
}

void InitChineseImageFileDescriptors(struct ImageFileDescriptor images[])
{
    CT_MEMSET(images, 0, sizeof(struct ImageFileDescriptor) * 5);

    // please input the path of the chinese.img file
    //images[0].fHandle = CT_FOPEN("C:/ϵͳ/InputRange/chinese_pinyin.img", "r+b", O_RDWR);
   // images[1].fHandle = CT_FOPEN("C:/ϵͳ/InputRange/chinese_usr.img", "r+b", O_RDWR); 
   //images[0].fHandle = 1;
    //images[1].fHandle = -1;
    
    CooTekReadStart = gu_CootekStart;
    CooTekReadStart_User = COOTEK_START_USER;
    
    images[0].fHandle = 0;
    images[1].fHandle = 1;
}


// deinitialize dictionary images
void DeinitImageFileDescriptors(struct ImageFileDescriptor images[], int n)
{
    int i;
    for(i = 0; i < n; i++)
    {
         if(images[i].fHandle >= 0)
         {
   	        CT_FCLOSE( images[i].fHandle );
         }
   }
}


/*************************************
* ����: InputRange_ClearKeyArray
* ����: ���������¼
* ����: none
* ����: none
*************************************/
void InputRange_ClearKeyArray(void)
{
	memset(InuptKey_Array, 0, (MAX_INPUT_CODE_EXPAND_LENGTH+1)*sizeof(int));
	InuptKey_Array[0] = 0;
	gu_TotNumber = 0;
	gu_RightNum  = 0;
}


/*************************************
* ����: InputRange_Init
* ����: ��ʼ���ֵ�
* ����: none
* ����: none
*************************************/
int InputRange_Init(void)
{
	S32 ResOffset;
	
	ResOffset = GetResouceBmpOffset();
	
	//hyUsbPrintf1("offset=%x\r\n", ResOffset);
	
	if (ResOffset < 0x2480000)
	{
		//hyUsbPrintf1("old img\r\n");
		gu_CootekStart = OLD_COOTEK_START;
		gu_CootekEnd   = OLD_COOTEK_END;
		CooTekReadStart= OLD_COOTEK_START;
	}
	else
	{
		//hyUsbPrintf1("new img\r\n");
		gu_CootekStart = COOTEK_START;
		gu_CootekEnd   = COOTEK_END;
		CooTekReadStart= COOTEK_START;
	}

	CooTekReadStart_User= COOTEK_START_USER;

	COOTEK_InitToSdram_buf();
	
	InputRange_ClearKeyArray();
	
	InitChineseImageFileDescriptors(gu_ChineseImages);
    gu_CineseHandle = InitializeDictionary(ChinesePinYin, gu_ChineseImages, ENG_DIC_ID);
    if(gu_CineseHandle == ERROR_DICTIONARY_HANDLE)
	{
        DeinitImageFileDescriptors(gu_ChineseImages, 2);
        return -1;
	}

	return 0;
}


/*************************************
* ����: InputRange_CloseFd
* ����: �ر��ֵ�
* ����: none
* ����: none
*************************************/
void InputRange_CloseFd(void)
{
	DeinitializeDictionary(gu_CineseHandle);
	DeinitImageFileDescriptors(gu_ChineseImages, 2);
}


/*************************************
* ����: InputRange_DeleteOneKey
* ����: ɾ��һ����ֵ
* ����: returnValue�����ؽ��
* ����: none
*************************************/
void InputRange_DeleteOneKey(tReturnWord *returnValue)
{
	int i, j=0, k=0, idx, m, lenTemp, len, temp;
	struct InputContext context;
	struct SearchResult result;
	struct CandidateItem candidateItems[PAGE_SIZE];
	WCHAR display_evidence_temp[MAX_WORD_ARRAY_SIZE * 3];
	U8 filter_list[MAX_FILTER_LIST_SIZE][PINYIN_INDEX_PINYIN_LETTER_SIZE];
	U8 filter_temp[PINYIN_INDEX_PINYIN_LETTER_SIZE];
	char word_temp[MAX_WORD_ARRAY_SIZE];
	
	memset(filter_temp, 0, PINYIN_INDEX_PINYIN_LETTER_SIZE);
	memset(filter_list, 0, MAX_FILTER_LIST_SIZE*PINYIN_INDEX_PINYIN_LETTER_SIZE);
	memset(display_evidence_temp, 0, sizeof(display_evidence_temp));
	
	result.candidate_items = candidateItems;
	
	memset(&context, 0, sizeof(struct InputContext));
	context.milestone = 0;
	
	context.request_size = PAGE_SIZE;
	context.history_size = 0;
	context.GetInputCodeExpandResult = GetInputCodeExpandResultTestPhonePad;

	if (InuptKey_Array[0] > 0)
	{
		InuptKey_Array[InuptKey_Array[0]] = 0;
		InuptKey_Array[0]--;
		
		//hyUsbPrintf1("inputkeyarrar[0]=%d\r\n", InuptKey_Array[0]);
		
		memset(returnValue, 0, sizeof(tReturnWord));
				
		if (InuptKey_Array[0] > 0)
		{
			context.input_codes_size = InuptKey_Array[0];
			
			for (i=0; i<InuptKey_Array[0]; i++)
			{
				context.input_codes[i] = InuptKey_Array[i+1];
			}
			
			for (i=0; i<GET_MAXNUM; i++)
			{
				SearchDictionary(gu_CineseHandle, &context, &result);
				if (result.candidate_items_size > 0 && result.candidate_items_size <= PAGE_SIZE)
				{
					for (idx=0; idx<result.candidate_items_size; idx++)
					{
						//Hyc_Unicode2Gbk((char *)result.candidate_items[idx].word, returnValue->HanZi[k++], 0);
						
						memset(word_temp, 0, MAX_WORD_ARRAY_SIZE);
						Hyc_Unicode2Gbk((char *)result.candidate_items[idx].word, word_temp, 0);
						if (strlen(word_temp) <= MAX_DISPLAY_HZ_LEN)
						{
							strcpy(returnValue->HanZi[k++], word_temp);
						}
					}
					if (result.candidate_items_size < PAGE_SIZE)
					{
						break;
					}
					context.history_size = result.milestone;
				}
				else
				{
					break;
				}
			}
			returnValue->HZ_Num = k;
			
			//hyUsbPrintf("DELETE:candidate_items_size=%d\r\n", result.candidate_items_size);
			
			if (result.candidate_items_size > 0 && result.candidate_items_size <= PAGE_SIZE)
			{
				if (PAGE_SIZE == result.candidate_items_size)
					gu_TotNumber = result.milestone;
				else
					gu_TotNumber = 0;
				
				//hyUsbPrintf1("filter_list_size=%d\r\n", result.filter_list_size);
				
				if (result.filter_list_size > 0)
				{
					for (i=0; i<result.filter_list_size; i++)
					{
						Hyc_Unicode2Gbk((char *)result.filter_list[i], filter_list[i], 0);
					}
					for (i=0; i<result.filter_list_size-1; i++)
					{
						temp = i;
						lenTemp = strlen(filter_list[i]);
						for (m=i+1; m<result.filter_list_size; m++)
						{
							len = strlen(filter_list[m]);
							if (len > lenTemp)
							{
								temp = m;
								lenTemp = len;
							}
						}
						if (temp != i)
						{
							strcpy(filter_temp, filter_list[i]);
							strcpy(filter_list[i], filter_list[temp]);
							strcpy(filter_list[temp], filter_temp);
						}
					}
					for (i=0; i<result.filter_list_size; i++)
					{
						strcpy(returnValue->PinYing[i], filter_list[i]);
					}
					returnValue->PY_Num = result.filter_list_size;
				}
				
				if (result.display_evidence_size > 0)
				{
					for (i=0; i<result.display_evidence_size; i++)
					{
						if (result.display_evidence[i] > 0)
						{
							display_evidence_temp[j++] = result.display_evidence[i];
						}
					}
					Hyc_Unicode2Gbk(display_evidence_temp, returnValue->ShowInput, 0);
					returnValue->Show_Num = result.display_evidence_size;
				}
			}
		}
	}
	else
	{
		memset(returnValue, 0, sizeof(tReturnWord));
	}
}


/*************************************
* ����: InputRange_GetWord
* ����: �����ֵ����ö�Ӧ�ĺ���
* ����: key����ֵ   returnValue�����ؽ��
* ����: none
*************************************/
void InputRange_GetWord(int key, tReturnWord *returnValue)
{
	int i, j=0, k=0, idx, m, lenTemp, len, temp;
	struct InputContext context;
	struct SearchResult result;
	struct CandidateItem candidateItems[PAGE_SIZE];
	WCHAR display_evidence_temp[MAX_WORD_ARRAY_SIZE * 3];
	U8 filter_list[MAX_FILTER_LIST_SIZE][PINYIN_INDEX_PINYIN_LETTER_SIZE];
	U8 filter_temp[PINYIN_INDEX_PINYIN_LETTER_SIZE];
	char word_temp[MAX_WORD_ARRAY_SIZE];
	
	memset(filter_temp, 0, PINYIN_INDEX_PINYIN_LETTER_SIZE);
	memset(display_evidence_temp, 0, sizeof(display_evidence_temp));
	memset(filter_list, 0, MAX_FILTER_LIST_SIZE*PINYIN_INDEX_PINYIN_LETTER_SIZE);
	
	result.candidate_items = candidateItems;
	
	memset(&context, 0, sizeof(struct InputContext));
	context.milestone = 0;
	
	context.request_size = PAGE_SIZE;
	context.history_size = 0;
	context.GetInputCodeExpandResult = GetInputCodeExpandResultTestPhonePad;
	
	if (InuptKey_Array[0] < MAX_INPUT_CODE_EXPAND_LENGTH)
	{
		memset(returnValue, 0, sizeof(tReturnWord));
	
		InuptKey_Array[InuptKey_Array[0]+1] = key;
		InuptKey_Array[0]++;
				
		context.input_codes_size = InuptKey_Array[0];
		
		for (i=0; i<InuptKey_Array[0]; i++)
		{
			context.input_codes[i] = InuptKey_Array[i+1];
		}
		
		for (i=0; i<GET_MAXNUM; i++)
		{
			SearchDictionary(gu_CineseHandle, &context, &result);
			if (result.candidate_items_size > 0 && result.candidate_items_size <= PAGE_SIZE)
			{
				for (idx=0; idx<result.candidate_items_size; idx++)
				{
					//Hyc_Unicode2Gbk((char *)result.candidate_items[idx].word, returnValue->HanZi[k++], 0);
				
					memset(word_temp, 0, MAX_WORD_ARRAY_SIZE);
					Hyc_Unicode2Gbk((char *)result.candidate_items[idx].word, word_temp, 0);
					if (strlen(word_temp) <= MAX_DISPLAY_HZ_LEN)
					{
						strcpy(returnValue->HanZi[k++], word_temp);
					}
				}
				if (result.candidate_items_size < PAGE_SIZE)
				{
					break;
				}
				context.milestone = result.milestone;
			}
			else
			{
				break;
			}
		}
		returnValue->HZ_Num = k;
		
		//hyUsbPrintf("GET:candidate_items_size=%d\r\n", result.candidate_items_size);
		
		if (result.candidate_items_size > 0 && result.candidate_items_size <= PAGE_SIZE)
		{
			if (PAGE_SIZE == result.candidate_items_size)
				gu_TotNumber = result.milestone;
			else
				gu_TotNumber = 0;
			
			if (result.filter_list_size > 0)
			{
				for (i=0; i<result.filter_list_size; i++)
				{
					Hyc_Unicode2Gbk((char *)result.filter_list[i], filter_list[i], 0 );
				}
				for (i=0; i<result.filter_list_size-1; i++)
				{
					temp = i;
					lenTemp = strlen(filter_list[i]);
					for (m=i+1; m<result.filter_list_size; m++)
					{
						len = strlen(filter_list[m]);
						if (len > lenTemp)
						{
							temp = m;
							lenTemp = len;
						}
					}
					if (temp != i)
					{
						strcpy(filter_temp, filter_list[i]);
						strcpy(filter_list[i], filter_list[temp]);
						strcpy(filter_list[temp], filter_temp);
					}
				}
				for (i=0; i<result.filter_list_size; i++)
				{
					strcpy(returnValue->PinYing[i], filter_list[i]);
				}
				returnValue->PY_Num = result.filter_list_size;
			}
			
			if (result.display_evidence_size > 0)
			{
				for (i=0; i<result.display_evidence_size; i++)
				{
					if (result.display_evidence[i] > 0)
					{
						display_evidence_temp[j++] = result.display_evidence[i];
					}
				}
				Hyc_Unicode2Gbk(display_evidence_temp, returnValue->ShowInput, 0);
				returnValue->Show_Num = result.display_evidence_size;
			}
		}
		else
		{
			if (gu_TotNumber > 0)
				gu_TotNumber = 0;
		}
	}
}


/*************************************
* ����: InputRange_GetNextWord
* ����: ����ǰһ���ֻ�ʣ����������ֻ��
* ����: firstWord��ǰһ����   returnValue�����ؽ��
* ����: 0:�ɹ�   -1:ʧ��
*************************************/
int  InputRange_GetNextWord(U8 *firstWord, tReturnWord *returnValue)
{
	int i,j, k=0, ret = 0;
	struct InputContext context;
    struct SearchResult result;
    struct CandidateItem candidateItems[PAGE_SIZE];
    char word_temp[MAX_WORD_ARRAY_SIZE];
	
	//hyUsbPrintf("firstWord = %s  %x\r\n",firstWord,returnValue);
    result.candidate_items = candidateItems;
    
    memset(&context, 0, sizeof(struct InputContext));
	context.milestone = 0;

    memset(&context, 0, sizeof(struct InputContext));
    context.milestone = 0;
    context.request_size = PAGE_SIZE;
    context.input_codes_size = 0;
    context.history_size = 1;
    context.GetInputCodeExpandResult = GetInputCodeExpandResultTestQWERTY;
    
    memset(returnValue->ShowInput, 0, sizeof(returnValue->ShowInput));
    
    if (strlen(firstWord)>0 && returnValue!=NULL)
    {
    	Hyc_Gbk2Unicode((char *)firstWord, context.history[0], 1);
    	
    	for (j=0; j<GET_MAXNUM; j++)
    	{
    		RetrieveNextWordCandidates(gu_CineseHandle, &context, &result);
    		if (result.candidate_items_size > 0 && result.candidate_items_size <= PAGE_SIZE)
    		{
    		//hyUsbPrintf("candidate_items_size=%d  milestone=%d\r\n", result.candidate_items_size, result.milestone);
    			for (i=0; i<result.candidate_items_size; i++)
				{
					//Hyc_Unicode2Gbk((char *)result.candidate_items[i].word, returnValue->HanZi[k++], 0);
					
					memset(word_temp, 0, MAX_WORD_ARRAY_SIZE);
					Hyc_Unicode2Gbk((char *)result.candidate_items[i].word, word_temp, 0);
					if (strlen(word_temp) <= MAX_DISPLAY_HZ_LEN)
					{
						strcpy(returnValue->HanZi[k++], word_temp);
					}
				}
				if (result.candidate_items_size < PAGE_SIZE)
				{
					break;
				}
				context.milestone = result.milestone;
    		}
    		else 
    		{
    			if (i == 0)
    				ret = -1;
    			break;
    		}
    	}
    	returnValue->HZ_Num = k;
    	if (returnValue->HZ_Num == 0)
    		ret = -1;
    }
    else
    {
    	ret = -1;
    }
    
    //hyUsbPrintf1("����->HZ_Num=%d\r\n", returnValue->HZ_Num);
    
    return ret;
}


/*************************************
* ����: InputRange_GetExactlyWord
* ����: ����ƴ����ȡ��ȷ�ĺ���
* ����: inputPY��ѡ�е�ƴ��   returnValue�����ؽ��
* ����: none
*************************************/
void InputRange_GetExactlyWord(U8 *inputPY, tReturnWord *returnValue)
{
	int i, idx, j=0, k=0;
	struct InputContext context;
	struct SearchResult result;
	struct CandidateItem candidateItems[PAGE_SIZE];
	WCHAR display_evidence_temp[MAX_WORD_ARRAY_SIZE * 3];
	
	memset(display_evidence_temp, 0, sizeof(display_evidence_temp));
	
	//hyUsbPrintf("inputPY = %s\r\n",inputPY);
	
	result.candidate_items = candidateItems;
	
	memset(&context, 0, sizeof(struct InputContext));
	context.milestone = 0;
	
	context.request_size = PAGE_SIZE;
	context.history_size = 0;
	context.GetInputCodeExpandResult = GetInputCodeExpandResultTestPhonePad;
	
	if (InuptKey_Array[0] > 0)
	{
		context.input_codes_size = InuptKey_Array[0];
		
		for (i=0; i<InuptKey_Array[0]; i++)
		{
			context.input_codes[i] = InuptKey_Array[i+1];
		}
		
		SearchDictionary(gu_CineseHandle, &context, &result);
		
		/*
		if (result.filter_list_size > 0)
		{
			for (i=0; i<result.filter_list_size; i++)
			{
				Hyc_Unicode2Gbk((char *)result.filter_list[i], returnValue->PinYing[i], 0);
			}
			returnValue->PY_Num = result.filter_list_size;
		}
		*/
		
		Hyc_Gbk2Unicode((char *)inputPY, context.filter, 1);
				
		for (idx=0; idx<GET_MAXNUM; idx++)
		{
			SearchDictionary(gu_CineseHandle, &context, &result);
			if (result.candidate_items_size > 0 && result.candidate_items_size <= PAGE_SIZE)
			{
				for (i=0; i<result.candidate_items_size; i++)
				{
					Hyc_Unicode2Gbk((char *)result.candidate_items[i].word, returnValue->HanZi[k++], 0);
				}
				if (result.candidate_items_size < PAGE_SIZE)
				{
					break;
				}
				context.milestone = result.milestone;
			}
			else
			{
				break;
			}
		}
		returnValue->HZ_Num = k;
		
		if (result.display_evidence_size > 0)
		{
			for (i=0; i<result.display_evidence_size; i++)
			{
				if (result.display_evidence[i] > 0)
				{
					display_evidence_temp[j++] = result.display_evidence[i];
				}
			}
			Hyc_Unicode2Gbk(display_evidence_temp, returnValue->ShowInput, 0);
			returnValue->Show_Num = result.display_evidence_size;
		}
	}
}


/*************************************
* ����: InputRange_GetPriority
* ����: ��ô�������ȼ�
* ����: input_Word��Ŀ�����
* ����: ���ȼ�ֵ
*************************************/
int InputRange_GetPriority(U8 *input_Word)
{
	WCHAR wBuf[MAX_WORD_ARRAY_SIZE];
	
	if (strlen(input_Word) > 0)
	{
		memset( wBuf, 0, sizeof(wBuf));
		Hyc_Gbk2Unicode((char *)input_Word, wBuf, 1);
		return RetrieveWordPriority(gu_CineseHandle, wBuf, wBuf);
	}
}


/*************************************
* ����: InputRange_ChangePriority
* ����: ����input_Word�����ȼ�
* ����: input_Word��Ҫ�����Ĵ�   
		manualAdjustFlag��0  �Զ��������ȼ�
                          1  �����ȼ�����ΪtargetPriority
        targetPriority��Ҫ���������ȼ�
* ����: none
*************************************/
void InputRange_ChangePriority(U8 *input_Word, char manualAdjustFlag, int targetPriority)
{
#if 0
	WCHAR wBuf[MAX_WORD_ARRAY_SIZE];
	
	if (strlen(input_Word) > 0)
	{
		memset( wBuf, 0, sizeof(wBuf));
		Hyc_Gbk2Unicode((char *)input_Word, wBuf, 1);
		AdjustWordPriority(gu_CineseHandle, wBuf, wBuf, manualAdjustFlag, targetPriority);
	}
#endif
}


/*************************************
* ����: InputRange_IsWordIn
* ����: �ж���������Ƿ�����ֵ���
* ����: input_Word��Ŀ�����
* ����: TRUE������   FALSE ������
*************************************/
BOOL InputRange_IsWordIn(U8 *input_Word)
{
	WCHAR wBuf[MAX_WORD_ARRAY_SIZE];
	
	if (strlen(input_Word) > 0)
	{
		memset( wBuf, 0, sizeof(wBuf));
		Hyc_Gbk2Unicode((char *)input_Word, wBuf, 1);
		return IsWordExists(gu_CineseHandle, wBuf, wBuf);
	}
}



/*************************************
* ����: InputRange_IsWordInUser
* ����: �ж���������Ƿ�����û��ֵ���
* ����: input_Word��Ŀ�����
* ����: TRUE������   FALSE ������
*************************************/
BOOL InputRange_IsWordInUser(U8 *input_Word)
{
	WCHAR wBuf[MAX_WORD_ARRAY_SIZE];
	
	if (strlen(input_Word) > 0)
	{
		memset( wBuf, 0, sizeof(wBuf));
		Hyc_Gbk2Unicode((char *)input_Word, wBuf, 1);
		return IsUserDefinedWord(gu_CineseHandle, wBuf, wBuf);
	}
}


/*************************************
* ����: InputRange_AddToUser
* ����: ��input_Word��ӵ��û��ֵ���
* ����: input_Word��Ҫ��ӵĴ�
* ����: none
*************************************/
void InputRange_AddToUser(U8 *input_Word)
{
	WCHAR wBuf[MAX_WORD_ARRAY_SIZE];

	if (strlen(input_Word) > 0)
	{
		memset(wBuf, 0, sizeof(wBuf));
		Hyc_Gbk2Unicode((char *)input_Word, wBuf, 1);
		AddWordToDictionary(gu_CineseHandle, wBuf, wBuf, 3);
	}
}


/*************************************
* ����: InputRange_RemoveFromUser
* ����: ��input_Word���û��ֵ���ɾ��
* ����: input_Word��Ҫɾ���Ĵ�
* ����: none
*************************************/
void InputRange_RemoveFromUser(U8 *input_Word)
{
	WCHAR wBuf[MAX_WORD_ARRAY_SIZE];

	if (strlen(input_Word) > 0)
	{
		memset(wBuf, 0, sizeof(wBuf));
		Hyc_Gbk2Unicode((char *)input_Word, wBuf, 1);
		RemoveWordFromDictionary(gu_CineseHandle, wBuf, wBuf);
	}
}


/*************************************
* ����: InputRange_NextOrBefore
* ����: ��ǰ������
* ����: direction:����   1�����   0����ǰ
		returnValue�����ؽ��
* ����: none
*************************************/
void InputRange_NextOrBefore(int direction, tReturnWord *returnValue)
{
	int i;
	struct InputContext context;
	struct SearchResult result;
	struct CandidateItem candidateItems[PAGE_SIZE];
	
	result.candidate_items = candidateItems;
	
	memset(&context, 0, sizeof(struct InputContext));
	
	//hyUsbPrintf("RightNum=%d\r\n", gu_RightNum);
	
	if (direction == 1)
	{
		context.milestone = gu_TotNumber;
		gu_RightNum++;
	}
	else if (direction == 0)
	{
		if (gu_RightNum > 0)
		{
			//gu_TotNumber -= (PAGE_SIZE + gu_RightNum * PAGE_SIZE);
			//gu_TotNumber = (PAGE_SIZE * 2) * (gu_RightNum - 1);
			gu_TotNumber = PAGE_SIZE * (gu_RightNum - 1);
			context.milestone = gu_TotNumber;
			gu_RightNum--;
		}
		else
		{
			context.milestone = 0;
		}
	}
	
	//hyUsbPrintf("milestone=%d\r\n", context.milestone);
	
	context.request_size = PAGE_SIZE;
	context.history_size = 0;
	context.GetInputCodeExpandResult = GetInputCodeExpandResultTestPhonePad;
	
	if (InuptKey_Array[0] > 0)
	{
		context.input_codes_size = InuptKey_Array[0];
		
		for (i=0; i<InuptKey_Array[0]; i++)
		{
			context.input_codes[i] = InuptKey_Array[i+1];
		}
		
		SearchDictionary(gu_CineseHandle, &context, &result);
		
		//hyUsbPrintf("LEFT-RIGHT:candidate_items_size=%d\r\n", result.candidate_items_size);
		
		if (result.candidate_items_size > 0 && result.candidate_items_size <= PAGE_SIZE)
		{
			memset(returnValue, 0, sizeof(tReturnWord));
			
			if (PAGE_SIZE == result.candidate_items_size)
			{
				gu_TotNumber = result.milestone;
			}
		
			if (result.candidate_items_size > 0)
			{
				for (i=0; i<result.candidate_items_size; i++)
				{
					Hyc_Unicode2Gbk((char *)result.candidate_items[i].word, returnValue->HanZi[i], 0);
				}
				returnValue->HZ_Num = result.candidate_items_size;
			}
			
			if (result.filter_list_size > 0)
			{
				for (i=0; i<result.filter_list_size; i++)
				{
					Hyc_Unicode2Gbk((char *)result.filter_list[i], returnValue->PinYing[i], 0);
				}
				returnValue->PY_Num = result.filter_list_size;
			}
			
			if (result.display_evidence_size > 0)
			{
				Hyc_Unicode2Gbk(result.display_evidence, returnValue->ShowInput, 0);
				returnValue->Show_Num = result.display_evidence_size;
			}
			
			//hyUsbPrintf("return gu_Num=%d\r\n", gu_TotNumber);
		}
	}
}


