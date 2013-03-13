/*******************************************************************************
*  (C) Copyright 2010 hyco, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of hyco and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  hyco and is confidential in nature.  Its use and dissemination by    
*  any party other than hyco is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/


/*------------------------------------------------------------------------------
Standard include files:
------------------------------------------------------------------------------*/
#include "stdarg.h"

/*------------------------------------------------------------------------------
   Types and defines:
------------------------------------------------------------------------------*/
#define SSCANF_EOF -1
#define SSCANF_TAB 9
#define SSCANF_NEW_LINE 10
#define SSCANF_END 0
#define NULL 0


/*-----------------------------------------------------------------------------
* ����:	int_sscanf
* ����:	�������ַ��������а���ʽ����һ��ʮ���ƻ�˽��Ƶ���������,�����������ݴ�
*		����һ�����ͻ�����ͱ�������
*
* ����:	pBuffer			һ��ָ���ַ�ָ���ָ��,���ַ�ָ��ָ��ǰ�����ַ�������
*		pIntArg			���ͻ���������ݴ�ŵ�ַ
*		m				��������	10:ʮ����	8:�˽���
*		FieldWidth		�����ʽָ�����
*		DisFlag			��ʽ�Ƿ���'*',���Ƿ�����. 0:���� 1:�洢
*
* ����:	SSCANF_EOF		�������
*		0				����,δ�洢����
*		1				�ɹ�����һ��ʮ���ƻ�˽�����������				 
*----------------------------------------------------------------------------*/
static int int_sscanf(char **pBuffer, int *pIntArg, int m, int FieldWidth, int ShortFlag, int DisFlag)
{
	char *p = *pBuffer;
	
	if (FieldWidth) 
	{
		int j;
		int int_a = 0;
		
		for ( j=0; j<FieldWidth; j++ )
			if ((**pBuffer >= '0')&&(**pBuffer <= '9')) 
			{
				int_a = int_a * m + (**pBuffer - '0'); 
				(*pBuffer)++; 
			} 
			else 
				break;
		if (*pBuffer == p) 
			return SSCANF_EOF;
		if (DisFlag) 
		{
			if (ShortFlag == 1) 
			{ 
				short sh_a;
				sh_a = int_a; 
				*(short *)pIntArg = sh_a; 
			}
			else if (ShortFlag == 2)
			{
				char ch_a;
				ch_a = int_a;
				*(char *)pIntArg = ch_a;
			}
			else 
				*pIntArg = int_a;
			return 1;
		}
	} 
	else 
	{
		int int_a = 0;
		
		while ( (**pBuffer >= '0')&&(**pBuffer <= '9') )
		{
			int_a = int_a * m + (**pBuffer - '0'); 
			(*pBuffer)++; 
		} 

		if (*pBuffer == p) 
			return SSCANF_EOF;
		if (DisFlag) 
		{
			if (ShortFlag == 1) 
			{ 
				short sh_a;
				sh_a = int_a; 
				*(short *)pIntArg = sh_a; 
			}
			else if (ShortFlag == 2)
			{
				char ch_a;
				ch_a = int_a;
				*(char *)pIntArg = ch_a;
			}
			else 
				*pIntArg = int_a;
			return 1;
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------------
* ����:	int64_sscanf
* ����:	�������ַ��������а���ʽ����һ��ʮ���ƻ�˽��Ƶ���������,�����������ݴ洢��һ��
*		64λlong long�ͱ�������
*
* ����:	pBuffer			һ��ָ���ַ�ָ���ָ��,���ַ�ָ��ָ��ǰ�����ַ�������
*		pI64Arg			64λlong long�����ݴ�ŵ�ַ
*		m				��������	10:ʮ����	8:�˽���
*		FieldWidth		�����ʽָ�����
*		DisFlag			��ʽ�Ƿ���'*',���Ƿ�����. 0:���� 1:�洢
*
* ����:	SSCANF_EOF		�������
*		0				����,δ�洢����
*		1				�ɹ�����һ��ʮ���ƻ�˽�����������				 
*----------------------------------------------------------------------------*/
static int int64_sscanf(char **pBuffer, __int64 *pI64Arg, int m, int FieldWidth, int DisFlag)
{
	char *p = *pBuffer;
	
	if (FieldWidth) 
	{
		int j;
		__int64 int_a = 0;
		
		for ( j=0; j<FieldWidth; j++ )
			if ((**pBuffer >= '0')&&(**pBuffer <= '9')) 
			{
				int_a = int_a * m + (**pBuffer - '0'); 
				(*pBuffer)++; 
			} 
			else 
				break;
		if (*pBuffer == p) 
			return SSCANF_EOF;
		if (DisFlag) 
		{
			*pI64Arg = int_a;
			return 1;
		}
	} 
	else 
	{
		__int64 int_a = 0;
		
		while ( (**pBuffer >= '0')&&(**pBuffer <= '9') )
		{
			int_a = int_a * m + (**pBuffer - '0'); 
			(*pBuffer)++; 
		} 

		if (*pBuffer == p) 
			return SSCANF_EOF;
		if (DisFlag) 
		{
			*pI64Arg = int_a;
			return 1;
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------------
* ����:	xint_sscanf
* ����:	�������ַ��������а���ʽ����һ��ʮ��������������,�����������ݴ洢��һ��
*		���ͻ�����ͱ�������
*
* ����:	pBuffer			һ��ָ���ַ�ָ���ָ��,���ַ�ָ��ָ��ǰ�����ַ�������
*		pIntArg			�������ݴ�ŵ�ַ
*		FieldWidth		�����ʽָ�����
*		DisFlag			��ʽ�Ƿ���'*',���Ƿ�����. 0:���� 1:�洢
*
* ����:	SSCANF_EOF		�������
*		0				����,δ�洢����
*		1				�ɹ�����һ��ʮ��������������				 
*----------------------------------------------------------------------------*/
static int xint_sscanf(char **pBuffer, int *pIntArg, int FieldWidth, int ShortFlag, int DisFlag)
{
	char *p = *pBuffer;
	int j, n, int_a = 0;

	if (FieldWidth) 
	{
		j = FieldWidth;
		n = 1;
	} 
	else 
	{ 
		j = 1; 
		n = 0; 
	}
	
	while (j)
	{
	
		if ((**pBuffer >= '0')&&(**pBuffer <= '9')) 
		{
			int_a = int_a * 16 + (**pBuffer - '0'); 
			(*pBuffer)++; 
		} 
		else if ((**pBuffer >= 'a')&&(**pBuffer <= 'f'))
		{
			int_a = int_a * 16 + (**pBuffer - 'a' + 10); 
			(*pBuffer)++;
		}
		else if ((**pBuffer >= 'A')&&(**pBuffer <= 'F')) 
		{
			int_a = int_a * 16 + (**pBuffer - 'A' + 10); 
			(*pBuffer)++;
		} 
		else break;
		
		j -= n;
	}
	
	if (*pBuffer == p) 
		return SSCANF_EOF;
	if (DisFlag) 
	{
		if (ShortFlag == 1) 
		{ 
			short sh_a;
			sh_a = int_a; 
			*(short *)pIntArg = sh_a; 
		}
		else if (ShortFlag == 2)
		{
			char ch_a;
			ch_a = int_a;
			*(char *)pIntArg = ch_a;
		}
		else 
			*pIntArg = int_a;
		return 1;
	}
	return 0;
}

/*-----------------------------------------------------------------------------
* ����:	xint64_sscanf
* ����:	�������ַ��������а���ʽ����һ��ʮ��������������,�����������ݴ洢��һ��
*		64λlong long�ͱ�������
*
* ����:	pBuffer			һ��ָ���ַ�ָ���ָ��,���ַ�ָ��ָ��ǰ�����ַ�������
*		pI64Arg			64λlong long�����ݴ�ŵ�ַ
*		FieldWidth		�����ʽָ�����
*		DisFlag			��ʽ�Ƿ���'*',���Ƿ�����. 0:���� 1:�洢
*
* ����:	SSCANF_EOF		�������
*		0				����,δ�洢����
*		1				�ɹ�����һ��ʮ��������������				 
*----------------------------------------------------------------------------*/
static int xint64_sscanf(char **pBuffer, __int64 *pI64Arg, int FieldWidth, int DisFlag)
{
	char *p = *pBuffer;
	int j,n;
	__int64 int_a = 0;
	
	if (FieldWidth) 
	{
		j = FieldWidth;
		n = 1;
	} 
	else 
	{ 
		j = 1; 
		n = 0; 
	}
	
	while (j)
	{
		if ((**pBuffer >= '0')&&(**pBuffer <= '9')) 
		{
			int_a = int_a * 16 + (**pBuffer - '0'); 
			(*pBuffer)++; 
		} 
		else if ((**pBuffer >= 'a')&&(**pBuffer <= 'f'))
		{
			int_a = int_a * 16 + (**pBuffer - 'a' + 10); 
			(*pBuffer)++;
		} 
		else if ((**pBuffer >= 'A')&&(**pBuffer <= 'F')) 
		{
			int_a = int_a * 16 + (**pBuffer - 'A' + 10); 
			(*pBuffer)++;
		} 
		else break;
		
		j -= n;
	}
	
	if (*pBuffer == p) 
		return SSCANF_EOF;
	if (DisFlag) 
	{
		*pI64Arg = int_a;
		return 1;
	}
	return 0;
}

/*-----------------------------------------------------------------------------
* ����:	float_sscanf
* ����:	�������ַ��������а���ʽ����һ������������
*
* ����:	pBuffer			һ��ָ���ַ�ָ���ָ��,���ַ�ָ��ָ��ǰ�����ַ�������
*		pFloatArg		���������ݴ�ŵ�ַ
*		FieldWidth		�����ʽָ�����
*		DisFlag			��ʽ�Ƿ���'*',���Ƿ�����. 0:���� 1:�洢
*
* ����:	SSCANF_EOF		�������
*		0				����,δ�洢����
*		1				�ɹ�����һ������������				 
*----------------------------------------------------------------------------*/
static int float_sscanf(char **pBuffer, float *pFloatArg, int FieldWidth, int DisFlag)
{
	char *pp = *pBuffer;
	
	if (FieldWidth) 
	{
	
		int j, e_flag;
		float p = 0, fa = 0;
		
		for ( j=0; j<FieldWidth; j++ )
			if (((**pBuffer >= '0')&&(**pBuffer <= '9'))||(**pBuffer == '.')) 
			{
				if ((**pBuffer == '.')&&(p == 0)) 
				{
					p = 1;
				} 
				else if (**pBuffer == '.')
				{
					break;
				} 
				else 
				{
					fa = fa * 10 + (**pBuffer - '0');
					p *= 10;
				}
				(*pBuffer)++;
			}
			else break; 
			
		if ((**pBuffer == 'e')||(**pBuffer == 'E'))
		{
			char neg_flag;
			
			(*pBuffer)++;
			if (!fa)
				return SSCANF_EOF;
			
			if ((**pBuffer == '+')||(**pBuffer == '-'))
				neg_flag = *(*pBuffer)++;
				
			while ( (**pBuffer >= '0')&&(**pBuffer <= '9') )
			{
				e_flag = e_flag * 10 + (**pBuffer - '0'); 
				(*pBuffer)++; 
			} 
			
			if (neg_flag == '-')
				e_flag = -e_flag;
			if (!p)
				p = 1;
			if (e_flag > 0)
			{
				while(e_flag)
				{
					p /= 10;
					e_flag--;
				} 
			}
			else
			{
				while(e_flag)
				{
					p *= 10;
					e_flag++;
				} 
			}
		}
		
		if (*pBuffer == pp) 
			return SSCANF_EOF;
		if (p) 
			fa = fa / p;
		if (DisFlag) 
		{ 
			*pFloatArg = fa; 
			return 1; 
		}
		
	} 
	else 
	{
		int e_flag = 0;
		float p = 0, fa = 0;
		
		while ( **pBuffer != ' ' )
			if (((**pBuffer >= '0')&&(**pBuffer <= '9'))||(**pBuffer == '.')) 
			{
				if ((**pBuffer == '.')&&(p == 0)) 
				{
					p = 1;
				} 
				else if (**pBuffer == '.')
				{
					break;
				} 
				else 
				{
					fa = fa * 10 + (**pBuffer - '0');
					p *= 10;
				}
				(*pBuffer)++;
			} 
			else break;

		if ((**pBuffer == 'e')||(**pBuffer == 'E'))
		{
			char neg_flag;
			
			(*pBuffer)++;
			if (!fa)
				return SSCANF_EOF;
			
			if ((**pBuffer == '+')||(**pBuffer == '-'))
				neg_flag = *(*pBuffer)++;
				
			while ( (**pBuffer >= '0')&&(**pBuffer <= '9') )
			{
				e_flag = e_flag * 10 + (**pBuffer - '0'); 
				(*pBuffer)++; 
			} 
			
			if (neg_flag == '-')
				e_flag = -e_flag;
			if (!p)
				p = 1;
			if (e_flag > 0)
			{
				while(e_flag)
				{
					p /= 10;
					e_flag--;
				} 
			}
			else
			{
				while(e_flag)
				{
					p *= 10;
					e_flag++;
				} 
			}
		}
			 
		if (*pBuffer == pp) 
			return SSCANF_EOF;
		if (p) 
			fa = fa / p;
		if (DisFlag) 
		{ 
			*pFloatArg = fa;
			return 1; 
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------------
* ����:	LFloat_sscanf
* ����:	�������ַ��������а���ʽ����һ��double������
*
* ����:	pBuffer			һ��ָ���ַ�ָ���ָ��,���ַ�ָ��ָ��ǰ�����ַ�������
*		pFloatArg		double�����ݴ�ŵ�ַ
*		FieldWidth		�����ʽָ�����
*		DisFlag			��ʽ�Ƿ���'*',���Ƿ�����. 0:���� 1:�洢
*
* ����:	SSCANF_EOF		�������
*		0				����,δ�洢����
*		1				�ɹ�����һ������������				 
*----------------------------------------------------------------------------*/
static int LFloat_sscanf(char **pBuffer, double *pFloatArg, int FieldWidth, int DisFlag)
{
	char *pp = *pBuffer;
	
	if (FieldWidth) 
	{
	
		int j, e_flag;
		float p = 0;
		double fa = 0;
		
		for ( j=0; j<FieldWidth; j++ )
			if (((**pBuffer >= '0')&&(**pBuffer <= '9'))||(**pBuffer == '.')) 
			{
				if ((**pBuffer == '.')&&(p == 0)) 
				{
					p = 1;
				} 
				else if (**pBuffer == '.')
				{
					break;
				} 
				else 
				{
					fa = fa * 10 + (**pBuffer - '0');
					p *= 10;
				}
				(*pBuffer)++;
			}
			else 
				break; 
			
		if ((**pBuffer == 'e')||(**pBuffer == 'E'))
		{
			char neg_flag;
			
			(*pBuffer)++;
			if (!fa)
				return SSCANF_EOF;
			
			if ((**pBuffer == '+')||(**pBuffer == '-'))
				neg_flag = *(*pBuffer)++;
				
			while ( (**pBuffer >= '0')&&(**pBuffer <= '9') )
			{
				e_flag = e_flag * 10 + (**pBuffer - '0'); 
				(*pBuffer)++; 
			} 
			
			if (neg_flag == '-')
				e_flag = -e_flag;
			if (!p)
				p = 1;
			if (e_flag > 0)
			{
				while(e_flag)
				{
					p /= 10;
					e_flag--;
				} 
			}
			else
			{
				while(e_flag)
				{
					p *= 10;
					e_flag++;
				} 
			}
		}
		
		if (*pBuffer == pp) 
			return SSCANF_EOF;
		if (p) 
			fa = fa / p;
		if (DisFlag) 
		{ 
			*pFloatArg = fa; 
			return 1; 
		}
		
	} 
	else 
	{
		int e_flag;
		float p = 0;
		double fa = 0;
		
		while ( **pBuffer != ' ' )
			if (((**pBuffer >= '0')&&(**pBuffer <= '9'))||(**pBuffer == '.')) 
			{
				if ((**pBuffer == '.')&&(p == 0)) 
				{
					p = 1;
				} 
				else if (**pBuffer == '.')
				{
					break;
				} 
				else 
				{
					fa = fa * 10 + (**pBuffer - '0');
					p *= 10;
				}
				(*pBuffer)++;
			} 
			else 
				break;

		if ((**pBuffer == 'e')||(**pBuffer == 'E'))
		{
			char neg_flag;
			
			(*pBuffer)++;
			if (!fa)
				return SSCANF_EOF;
			
			if ((**pBuffer == '+')||(**pBuffer == '-'))
				neg_flag = *(*pBuffer)++;
				
			while ( (**pBuffer >= '0')&&(**pBuffer <= '9') )
			{
				e_flag = e_flag * 10 + (**pBuffer - '0'); 
				(*pBuffer)++; 
			} 
			
			if (neg_flag == '-')
				e_flag = -e_flag;
			if (!p)
				p = 1;
			if (e_flag > 0)
			{
				while(e_flag)
				{
					p /= 10;
					e_flag--;
				} 
			}
			else
			{
				while(e_flag)
				{
					p *= 10;
					e_flag++;
				} 
			}
		}	
		
		if (*pBuffer == pp) 
			return SSCANF_EOF;
		if (p) 
			fa = fa / p;
		if (DisFlag) 
		{ 
			*pFloatArg = fa;
			return 1; 
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------------
* ����:	string_sscanf
* ����:	�������ַ��������а���ʽ����һ���ַ���
*
* ����:	pBuffer			һ��ָ���ַ�ָ���ָ��,���ַ�ָ��ָ��ǰ�����ַ�������
*		pChArg			�ַ������ݴ�ŵ�ַ
*		FieldWidth		�����ʽָ�����
*		DisFlag			��ʽ�Ƿ���'*',���Ƿ�����. 0:���� 1:�洢
*
* ����:	SSCANF_EOF		�������
*		0				����,δ�洢����
*		1				�ɹ�����һ���ַ���				 
*----------------------------------------------------------------------------*/
static int string_sscanf(char **pBuffer, char *pChArg, int FieldWidth, int DisFlag)
{
	char *p = *pBuffer;
	
	if (FieldWidth)
	{
		int j;
		
		for ( j=0; j<FieldWidth; j++ )
			if ((**pBuffer != ' ')&&(**pBuffer != SSCANF_TAB)&&(**pBuffer != SSCANF_NEW_LINE)&&(**pBuffer != SSCANF_END)) 
			{
				if (DisFlag) 
				{
					*pChArg = **pBuffer; 
					pChArg++;
				}
				(*pBuffer)++;
			} 
			else break;
			
		if (*pBuffer == p)
			return SSCANF_EOF;
		if (DisFlag) 
		{
			*pChArg = 0;
			return 1;
		}
	} 
	else 
	{
		
		while ((**pBuffer != ' ')&&(**pBuffer != SSCANF_TAB)&&(**pBuffer != SSCANF_NEW_LINE)&&(**pBuffer != SSCANF_END)) 
		{
			if (DisFlag) 
			{
				*pChArg = **pBuffer;
				pChArg++;
			}
			(*pBuffer)++;
		}
		
		if (*pBuffer == p) 
			return SSCANF_EOF;
		if (DisFlag) 
		{
			*pChArg = SSCANF_END;
			return 1;
		}
	}
	return 0;
}

/*-----------------------------------------------------------------------------
* ����:	char_sscanf
* ����:	�������ַ��������а���ʽ����һ���ַ����п���ַ�
*
* ����:	pBuffer			һ��ָ���ַ�ָ���ָ��,���ַ�ָ��ָ��ǰ�����ַ�������
*		pChArg			�ַ������ݴ�ŵ�ַ
*		FieldWidth		�����ʽָ�����
*		DisFlag			��ʽ�Ƿ���'*',���Ƿ�����. 0:���� 1:�洢
*
* ����:	SSCANF_EOF		�������
*		0				����,δ�洢����
*		1				�ɹ�����һ���ַ�				 
*----------------------------------------------------------------------------*/
static int char_sscanf(char **pBuffer, char *pChArg, int FieldWidth, int DisFlag)
{
	char *p = *pBuffer;
	
	if (FieldWidth) 
	{
		int j;
		
		for ( j=0; j<FieldWidth; j++ ) 
			if (**pBuffer != SSCANF_END) 
			{
				if (DisFlag) 
				{
					*pChArg = **pBuffer;
					pChArg++;
				}
				(*pBuffer)++;
			} 
			else break;
			
		if (*pBuffer == p) 
			return SSCANF_EOF;
		if (DisFlag) 
			return 1;
	}
	else 
	{
		if (**pBuffer != SSCANF_END)
		{
			if (DisFlag) 
			{
				*pChArg = **pBuffer; 
				(*pBuffer)++;
				pChArg++;
				return 1;
			}
			else 
				(*pBuffer)++;
		} 
		else 
			return SSCANF_EOF;
	}
	
	return 0;
}


/*-----------------------------------------------------------------------------
* ����:	other_sscanf
* ����:	����ʽΪ"%[..]"ʱ,ʹ�øú�������һ���ַ���,���洢����Ӧ�Ĳ�����
*
* ����:	pFormat			һ��ָ���ַ�ָ���ָ��,���ַ�ָ��ָ��ǰ��ʽ�ַ���
*		pBuffer			һ��ָ���ַ�ָ���ָ��,���ַ�ָ��ָ��ǰ�����ַ�������
*		pChArg			�ַ������ݴ�ŵ�ַ
*		FieldWidth		�����ʽָ�����
*		DisFlag			��ʽ�Ƿ���'*',���Ƿ�����. 0:���� 1:�洢
*
* ����:	SSCANF_EOF		�������
*		0				����,δ�洢����
*		1				�ɹ�����һ���ַ���				 
*----------------------------------------------------------------------------*/
static int other_sscanf(char **pFormat, char **pBuffer, char *pChArg, int FieldWidth, int DisFlag)
{
	char *p = *pBuffer, *pp = *pFormat;
	char a[256];
	int j, n, jj = 1;
	
	memset( a, 0, 256);

/* find if the format is store "until" or "till" the character is encountered.
 * if "until", a[i] = 1(i=0,1,2,...,255), and the index of the character refered 
 * in the format will be 0. */
	if (**pFormat == '^') 
	{
		(*pFormat)++;
		jj = 0;
		memset( a, 1, 256);
	}

/* read the character in the format while the format end. */
	while (**pFormat != ']') 
	{
		if (**pFormat != '-') 
		{
			a[**pFormat] = jj;
			(*pFormat)++;
		} 
		else 
		{
			int i;
			char min, max, c;
			min = *(*pFormat - 1);
			max = *(*pFormat + 1);
			*pFormat += 2;
			if (max == ']') 
				return SSCANF_EOF;
			if (min > max) 
			{
				c = min;
				min = max;
				max = min;
			}
			for ( i=min; i<=max; i++ ) 
				a[i] = jj;
		}	// if (**pFormat != '-')
	}	// while (**pFormat != ']')

/* if the format refered nothing, it means all are included. */
	if (pp == *pFormat) 
		memset( a, 1, 256 );
	
	(*pFormat)++;

/* This is set for FieldWidth. */
	if (FieldWidth)
	{
		j = FieldWidth;
		n = 1;
	} 
	else 
	{ 
		j = 1; 
		n = 0;
	}

	while ((a[**pBuffer])&&(**pBuffer != SSCANF_END)&&(j))
	{
		if (DisFlag) 
		{
			*pChArg = **pBuffer;
			pChArg++;
		}
		(*pBuffer)++;
		j -= n;
	}
	
	if (p == *pBuffer) 
		return SSCANF_EOF;
	if (DisFlag) 
	{
		*pChArg = SSCANF_END;
		return 1;
	}
	return 0;
}



/*-----------------------------------------------------------------------------
* ����:	Format_Sscanf
* ����:	���hyco��sscanf����ʹ��,�������ַ��������е����ݰ���ʽд���ɱ������
* ����:	buffer   �����ַ���
*		format	 ��ʽ�����ַ���
*		list	 �ɱ�����б�
* ����:	���뵽�ɱ�����е����ݸ���  
*----------------------------------------------------------------------------*/
static int Format_Sscanf(char *buffer, char *format, va_list ap)
{
	int j, i = 0;
	char *pFormat = format, *pBuffer = buffer;

	while (*pFormat != 0)
	{
		char type, *pChArg;
		int *pIntArg;
		__int64 *pI64Arg;
		float *pFloatArg;
		double *pDoubleArg;
		
		j = 0;

		/* read the information in the format */
		if ((*pFormat == '%')&&(*(++pFormat) != '%')) 
		{
			int FieldWidth = 0, DisFlag = 1, WidthFlag = 0;
			
			if (*pFormat == '*') 
			{ 
				DisFlag = 0; // if (DisFlag == 0) discard the input field
				pFormat++; 
			}	
			
			while ((*pFormat >= '0')&&(*pFormat <= '9'))	// read the FieldWidth if it's specified
			{
				FieldWidth = FieldWidth * 10 + (*pFormat - '0');
				WidthFlag = 1;
				pFormat++;
			}
			if ((WidthFlag)&&(!FieldWidth))
				return i;

		/* if the format is like this "%[..]", use other_sscanf */
			if (*pFormat == '[') 
			{
				pFormat++;
				if (DisFlag) 
					pChArg = va_arg(ap, char*); 
				else 
					pChArg = NULL;
				j = other_sscanf(&pFormat, &pBuffer, pChArg, FieldWidth, DisFlag);
			} 
			else 
		/* if the format is not like "%[..]", continue to read the rest information. */
			{
				int NegFlag = 0, ShortFlag = 0, LongFlag = 0, LLFlag = 0, jFlag = 0, zFlag = 0, tFlag = 0;

				switch (*pFormat)
				{
				case 'h':
					if (*(pFormat+1) == 'h')
					{
						ShortFlag = 2;	// C99--hh: if (ShortFlag == 2) the type is char
						pFormat += 2; 
					}
					else
					{
						ShortFlag = 1;	// if (ShortFlag == 1) the type is short int
						pFormat++;
					}	
					break;

				case 'l':
					if (*(pFormat+1) == 'l')
					{
						LLFlag = 1;		// C99--ll: if (LLFlag == 1) the type is long long int
						pFormat += 2; 
					}
					else
					{
						LongFlag = 1;	// if (LongFlag == 1) the type is long int
						pFormat++;
					}
					break;

				case 'I':
					if ((*(pFormat+1) == '6')&&(*(pFormat+2) == '4'))
					{
						LLFlag = 1;	// if (LLFlag == 1) the type is long long
						pFormat += 3; 
					}
					break;

				case 'L':
					LongFlag = 1;
					pFormat++;
					break;

				case 'j':		// C99 intmax_t �����޷��˽�Ͷ��������,��ʱֻ����ʽ����
					jFlag = 1;
					pFormat++;
					break;

				case 'z':		// C99 size_t �����޷��˽�Ͷ��������,��ʱֻ����ʽ����
					zFlag = 1;
					pFormat++;
					break;

				case 't':		// C99 ptrdiff_t �����޷��˽�Ͷ��������,��ʱֻ����ʽ����
					tFlag = 1;
					pFormat++;
					break;
				}
				
				type = *pFormat++;
				
				if (type == 'p')
				{
					NegFlag = 0, ShortFlag = 0, LongFlag = 0, LLFlag = 0, jFlag = 0, zFlag = 0, tFlag = 0;
				}
				
				if ((type != 'c')&&(type != 'n'))
					while ((*pBuffer == ' ')||(*pBuffer == SSCANF_TAB)||(*pBuffer == SSCANF_NEW_LINE)) 
						pBuffer++;
				
				if ((type != 's')&&(type != 'c')) 
					if ((*pBuffer == '-')||(*pBuffer == '+')) 
					{ 
						if (*pBuffer == '-') NegFlag = 1; 
						pBuffer++; 
						if (FieldWidth) 
						{
							if (FieldWidth - 1) 
							{
								FieldWidth--;
							} 
							else 
								return i;
						}
					} 

		/* converting the value according to the type */
				switch (type) 
				{
				case 'd':
				case 'i':
				case 'u':
					if (LLFlag) 
					{
						if (DisFlag) 
							pI64Arg = va_arg(ap, __int64*); 
						else 
							pI64Arg = NULL;
						j = int64_sscanf(&pBuffer, pI64Arg, 10, FieldWidth, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pI64Arg = -(*pI64Arg);
					} 
					else 
					{
						if (DisFlag) 
							pIntArg = va_arg(ap, int*); 
						else 
							pIntArg = NULL;
						j = int_sscanf(&pBuffer, pIntArg, 10, FieldWidth, ShortFlag, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pIntArg = -(*pIntArg);
					}
					break;
					
				case 'x':
				case 'X':
				case 'p':
					if (LLFlag) 
					{
						if (DisFlag) 
							pI64Arg = va_arg(ap, __int64*); 
						else 
							pI64Arg = NULL;
						j = xint64_sscanf(&pBuffer, pI64Arg, FieldWidth, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pI64Arg = -(*pI64Arg);
					} 
					else 
					{
						if (DisFlag) 
							pIntArg = va_arg(ap, int*); 
						else 
							pIntArg = NULL;
						j = xint_sscanf(&pBuffer, pIntArg, FieldWidth, ShortFlag, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pIntArg = -(*pIntArg);
					}
					break;
					
				case 'o':
					if (LLFlag) 
					{
						if (DisFlag) 
							pI64Arg = va_arg(ap, __int64*); 
						else 
							pI64Arg = NULL;
						j = int64_sscanf(&pBuffer, pI64Arg, 8, FieldWidth, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pI64Arg = -(*pI64Arg);
					} 
					else 
					{
						if (DisFlag) 
							pIntArg = va_arg(ap, int*); 
						else 
							pIntArg = NULL;
						j = int_sscanf(&pBuffer, pIntArg, 8, FieldWidth, ShortFlag, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pIntArg = -(*pIntArg);
					}
					break;
					
				case 'f':
				case 'F':
				case 'a':	// C99
				case 'A':	// C99 
				case 'e':
				case 'E':
				case 'g':
				case 'G':
					if (LongFlag)
					{
						if (DisFlag) 
							pDoubleArg = va_arg(ap, double*); 
						else 
							pFloatArg = NULL;
						j = LFloat_sscanf(&pBuffer, pDoubleArg, FieldWidth, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pDoubleArg = -(*pDoubleArg);	
					}
					else
					{
						if (DisFlag) 
							pFloatArg = va_arg(ap, float*); 
						else 
							pFloatArg = NULL;
						j = float_sscanf(&pBuffer, pFloatArg, FieldWidth, DisFlag);
						if ((NegFlag)&&(j != SSCANF_EOF)&&(DisFlag)) 
							*pFloatArg = -(*pFloatArg);	
					}
					break;

				case 's':
					if (DisFlag) 
						pChArg = va_arg(ap, char*); 
					else 
						pChArg = NULL;
					j = string_sscanf(&pBuffer, pChArg, FieldWidth, DisFlag);
					break;
					
				case 'c':
					if (DisFlag) 
						pChArg = va_arg(ap, char*); 
					else
						pChArg = NULL;
					j = char_sscanf(&pBuffer, pChArg, FieldWidth, DisFlag);					
					break;
					
				case 'n':
					if (DisFlag) 
					{
						pIntArg = va_arg(ap, int*);
						if (*pBuffer != SSCANF_END) 
							*pIntArg = pBuffer - buffer; 
						else 
							j = SSCANF_EOF;
					}
					else 
						pIntArg = NULL;
					break;
				
				default:
					j = SSCANF_EOF;
				}

			}		// if (*pFormat == '[') 
			
			if (j == SSCANF_EOF) 
			{
				if (pBuffer == buffer) 
					return SSCANF_EOF; 
				else 
					return i;
			} 
			else 
				i += j;
			
		} 
		else
	/* if the format is not a conversion specification(%...), find if it's conflicted with the buffer. */
		{
			if ((*pFormat != ' ')&&(*pFormat != SSCANF_TAB)&&(*pFormat != SSCANF_NEW_LINE))
			{
				if (*pFormat != *pBuffer) 
					return i; 
				else 
				{ 
					pFormat++; 
					pBuffer++; 
				}
			}
			else 
			{
				pFormat++;
				while ((*pBuffer == ' ')||(*pBuffer == SSCANF_TAB)||(*pBuffer == SSCANF_NEW_LINE)) 
					pBuffer++;
			}
		}			// *pFormat == '%'
		
	}	//while(*pFormat != 0)		
	
	if (pBuffer == buffer) 
		return SSCANF_EOF; 
	else 
		return i;
}


/*-----------------------------------------------------------------------------
* ����:	sscanf
* ����:	hyco��sscanf����
* ����:	buffer   �����ַ�������
		format	 ��ʽ�����ַ���
*		...		 �ɱ�����б�
* ����:	��������ݸ���  
*----------------------------------------------------------------------------*/
int sscanf(char *buffer, char *format,...)
{
	va_list list;
	int n;
	
	va_start(list,format);
	
	n = Format_Sscanf( buffer, format, list );
	
	va_end(list);
	
	return (n);
}
