#ifndef __ALPHA_MAKE_H__
#define __ALPHA_MAKE_H__

#include "DisplayMemory.h"

/*-----------------------------------------------------------------------------
* ����:	edit_make_alpha_value
* ����:	����alphaֵ
* ����:	type ����
* ����:	base offset
*----------------------------------------------------------------------------*/
void edit_make_alpha_value(void);

/*-----------------------------------------------------------------------------
* ����:	edit_bmp_mix_buf
* ����:	�������buf���
* ����:	�����ǰ�е�alpha�������buf���
* ����:	none
*----------------------------------------------------------------------------*/
void edit_bmp_mix_buf(AREA_t *pArea,char *upTempMem, char *dispBuf,tCTR_PANEL *pCtrPan);

#endif