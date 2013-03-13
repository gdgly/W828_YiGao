
#ifndef _TYPES_H_
#define _TYPES_H_


typedef  unsigned long long		u_int64_t;
//typedef  __int64				int64_t;
typedef  unsigned int			u_int32_t;
typedef  int					int32_t;
typedef	 unsigned short			u_int16_t;
typedef	 short					int16_t;
typedef  unsigned char			u_int8_t;
typedef  signed   char			int8_t;


//isblank���ո�ˮƽ�Ʊ��
//isspace���ո�ˮƽ�Ʊ������ֱ�Ʊ������ҳ���س������У����հ��ַ�
#define isblank(c) 	((c) == ' ' || (c) == '\t')

//return 'true' if c is an ASCII character (0-127)
#define isascii(c) 	((c) <= 127)

//return 'true' if c is alphanumeric
#define isalnum(c) 	(((c)<='z' && (c)>='a') ||	\
          			((c)<='Z' && (c)>='A')  ||	\
          			((c)<='9' && (c)>='0'))



#define	MALLOC	malloc
#define FREE 	free




#endif