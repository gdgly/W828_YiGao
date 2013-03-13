/*
 * ==================================================
 *   Copyright 2009, CooTek
 *   All rights reserved. http://www.cootek.com
 * ==================================================
 */

#ifndef _DICTIONARY_I_H_
#define _DICTIONARY_I_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "predefined.h"
#include "hal.h"

    typedef enum _DictionaryEngineType
    {
        Standard,
        ChinesePinYin,
        ChineseBiHua,
        ChineseZhuyin,
        Japanese,
    } DictionaryEngineType;

    // DictionaryHandleInternal is an internal-used structure
    // the only reason to read this structure is for CandidateItem.source_dic_handle:
    //      if ( DictionaryHandleInternal.internalDictionaryHandle == CandidateItem.source_dic_handle)
    //      { means this word comes from this dictionary instance. }
    struct DictionaryHandleInternal
    {
        DictionaryEngineType engineType;
        void* internalDictionaryHandle;
    };

    struct ImageFileDescriptor
    {
        IMAGE_HANDLE fHandle;
    };

    struct ImageArrayDescriptor
    {
        char *image;
        int imageSize;
    };

	/*
	ʹ���ļ������ʼ���ֵ䣬����ֵΪ�ֵ�����languageTag�����ڶ����Ի���ʱ���ߵ����ߺ�ѡ���Ǵ��ĸ��ֵ������������
	*/
    DICTIONARY_HANDLE InitializeDictionary(DictionaryEngineType engineType, struct ImageFileDescriptor fImages[], int languageTag);


	/*
	ʹ���ڴ�ָ���ʼ���ֵ䡣ʹ�õ��ļ�ͬ�ϡ�������Ҫע�������ҪΪһЩ�ļ����������ڴ棺
	1. usr�ļ���������Ҫ�����û���ӵ��û���
	2. ���pi��pl�ļ���Ϊ0����������ҪΪpi��pl�����16k(16*1024B)���ڴ�
	*/
    DICTIONARY_HANDLE InitializeDictionaryByArray(DictionaryEngineType engineType, struct ImageArrayDescriptor arrayImages[], int languageTag);


	/*
	�ͷ�ʹ���ļ������ʼ�����ֵ䣬��������ʹ�á�
	*/
    void  DeinitializeDictionary(DICTIONARY_HANDLE handle);


	/*
	
	*/
    void  DeinitializeDictionaryByArray(DICTIONARY_HANDLE handle);


	/*
	����input�Ĳ��������ֵ䣬���������result�result����������䣬�ͷ��ڴ�
	*/
    void  SearchDictionary(DICTIONARY_HANDLE handle, struct InputContext* input, struct SearchResult* result);

    void  RetrieveNextWordCandidates(DICTIONARY_HANDLE handle, struct InputContext *input, struct SearchResult* result);

    void  AdjustWordPriority(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[], char manualAdjustFlag, int targetPriority);

    int RetrieveWordPriority(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[]);

    BOOL  IsWordExists(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[]);

    void  AddWordToDictionary(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[], int targetPriority);

    void  RemoveWordFromDictionary(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[]);

    BOOL  IsUserDefinedWord(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[]);

    BOOL  HasLinkedWords(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[]);

    void  GetLinkedWords(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[], struct SearchResult* result);

    void  GetLinkedWordsByIndex(DICTIONARY_HANDLE handle, WCHAR index[], WCHAR word[], int directionIndex, struct SearchResult* result);

    // mixed language input interfaces
    void AttachSecondaryDictionary(DICTIONARY_HANDLE handle, DICTIONARY_HANDLE secondaryDictionaryHandle);

    void DetachSecondaryDictionary(DICTIONARY_HANDLE handle, DICTIONARY_HANDLE secondaryDictionaryHandle);

    void SearchDictionaryForward(DICTIONARY_HANDLE handle, struct InputContext* input, struct SearchResult* result);

    // translate words to code
    void QueryDictionaryCode(DICTIONARY_HANDLE handle, WCHAR word[], WCHAR mapping[], struct SearchResult* result);

    BOOL IsValidDictionaryCode(DICTIONARY_HANDLE handle, WCHAR word[], WCHAR code[], WCHAR mapping[]);

    void QueryAbbreviations(DICTIONARY_HANDLE handle, WCHAR word[], WCHAR mapping[], struct SearchResult* result);

    BOOL IsAbbreviationMatchingCode(WCHAR word[], WCHAR code[], WCHAR mapping[]);

    void AfterWordSelected(DICTIONARY_HANDLE handle, struct InputContext *input);

    int GetDictionaryVersion(DICTIONARY_HANDLE handle);

    void SetDictionaryVersion(DICTIONARY_HANDLE handle, int version);
#ifdef __cplusplus
}
#endif

#endif
