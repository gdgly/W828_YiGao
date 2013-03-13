/*************************************************************
%
% Filename     : drvKeyb.h
% Project name : part of the lower key driver of GUI part of hyctron, manage the input key scanning 
%
% Copyright 2003 Hyctron Electronic Design House,
% "Hyctron Electronic Design House" Shanghai, China.
% All rights are reserved. Reproduction in whole or in part is prohibited
% without the prior written consent of the copyright owner.
%
% Rev   Date    	Author          Comments
%      (yymmdd)
% -------------------------------------------------------------
% 001   040128  HYCZJ		Primary version 
% -------------------------------------------------------------
%
%  This source file contains the key process part of the GUI   
% 
****************************************************************/

#ifndef _DRVKEYB_H_ //-----------------
#define _DRVKEYB_H_

#include "HSA_API.h"
#include "keyCodeEvent.h"

#ifdef __cplusplus
extern "C"
{
#endif

//change the Hold key properity, button / switch key
#define KEYB_HOLD_PUSH_BUTTON	0
#define KEYB_HOLD_SWITCH		1

// BIT PATTERN of static S8   s8LockedState
#define MAIN_KEYBOARD_LOCK    0x80
#define LINE_KEYBOARD_LOCK    0x40

#define KEY_7901_PCB        0x0fff  //��ֵ��Ҫ�ͼ�ֵ�г�ͻ  

/*-----------------------------------------------------------------------------
* ����:	drvKeyb_Initialize
* ����:	����������ʼ��
* ����:	none
* ����:	��ʼ���ɹ�������cSSA_OK
*----------------------------------------------------------------------------*/
U32 drvKeyb_Initialize( void );

/*------------------------------------------------------------------------
* ����: PowerOnKeyPressed
* ����: ��⿪�����Ƿ���
* ����: none
* ����: 0------����������
*		����------������û�а���
-------------------------------------------------------------------------*/
U32 drvKeybPowerOnKeyPressed(void);

/*-----------------------------------------------------------------------------
* ����:	drvKeyb_IsLocked
* ����:	�������Ƿ�����
* ����:	none
* ����:	TRUE, �����ѱ�����
	FALSE, ���̴�������״̬
*----------------------------------------------------------------------------*/
U32 drvKeyb_IsLocked( void );

/*-----------------------------------------------------------------------------
* ����: drvKeyb_GetEvent
* ����: ���ؼ�⵽�ļ����¼�
* ����: pKeyb_Code_e    pkeKeyCode,   // O: ����
   	pU32            pku32Event    // O: ���������� �ͷŻ���
* ����: cSSA_OK/cSSA_NOT_OK/cSSA_BAD_PARAMETER/cSSA_NOT_INITIALIZED
---------------------------------------------------------------------------*/
U32 drvKeyb_GetEvent( pKeyb_Code_e   pkeKeyCode, U32 * pku32Event );

/*-----------------------------------------------------------------------------
* ����: drvKeyb_GetPos
* ����: �õ������������
* ����: *row���� *column:��
* ����: 1:�ɹ�ȡ�� 0:ʧ��
---------------------------------------------------------------------------*/
U32 drvKeyb_GetPos(S16 *row,S16 *column);

/*-----------------------------------------------------------------------------
* ����: drvKeyb_ClearPos
* ����: �������������εõ������
* ����: 
* ����: 
---------------------------------------------------------------------------*/
void drvKeyb_ClearPos(void);

/*-----------------------------------------------------------------------------
* ����: drvKeyb_GetEvent_Timer0ISR
* ����: irq�ж�ɨ�谴��
* ����: none
* ����: none
---------------------------------------------------------------------------*/
void drvKeyb_GetEvent_Timer0ISR(void);

#ifdef __cplusplus
}
#endif

#endif // DRVKEYB_H_H //---------------

/*============================ History List ================================
 1. Rev 1.00.00.0000, 2003/07/14 10:00             Xuefeng Yang
============================================================================*/
