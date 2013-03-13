#ifndef __ALPHA_DISPLAY_H__
#define __ALPHA_DISPLAY_H__

#include "edit.h"

#define ALPHA_MEMORY_OFFSET   ((char *)hsaSdram_AlphaInputBuf())
#define ALPHA_BLOCK_SIZE      (153600)
#define ALPHA_BLOCK_NUM       (3)

#define ALPHA_MEMORY_VALUE_OFFSET   ((char *)hsaSdram_AlphaDisplayBuf())
#define ALPHA_BLOCK_VALUE_SIZE      (76800)
#define ALPHA_BLOCK_VALUE_NUM       4

//ALPHA ��ʾ�������С
#define ALPHA_SHOW_ROW 0
#define ALPHA_SHOW_COL 0
#define ALPHA_SHOW_HIG 320
#define ALPHA_SHOW_WID 240


/*-------------------------------------------------------
 *����: alpha_disp_all_to_screen  
 *����: alphaģʽ�½�������
 *����: none
 *����: none
--------------------------------------------------------*/
void alpha_disp_all_to_screen(tCTR_PANEL *pCtrPan);

#endif