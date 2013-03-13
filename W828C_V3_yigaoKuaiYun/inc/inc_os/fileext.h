//
// ��λ���ļ������ӿں���
//
// ���׺�����Ŀ����ʵ����λ����PC���ļ����������ӿڵ�ͳһ������λ���ļ����������ķ�װ��
// �������ļ�ʱ�벻Ҫͬʱ����PC�µ�stdio.h��
//
//
// ע�⣺
// Ŀǰ��֧�ֱ�κ������� fprintf, fscanf��;
// Ŀǰ��֧�ֿ��ֽڲ����������� fwputc/fwputs��
// Ŀǰ���ļ�ʱֻ֧�� "r, w, a [b][+]" ���������12�ַ�ʽ��������ʽ�򿪽��δ֪
// Ŀǰ�ò��װ�ο�PC��֧�ֻ��棨��СΪ_FILE_BUFF_LEN��4K��������ȻҲ����ͨ���رպ�FILE_SUPPORT_BUFFER���رոù���
// [���棺Ҳ���Ƕ�д�ļ�ʱ���ٳ�һ����СΪ_FILE_BUFF_LEN���ڴ�ռ䣬һ���Զ���/д�룬��ȥƵ������ʵ���ļ��Ķ�д]
//

#ifndef FILE_H
#define FILE_H

#ifndef _WIN32	// PC MACRO

#include "AbstractFileSystem.h"


#ifndef EOF
#define EOF     (-1)
#endif
#ifndef NULL	/* Null */
#define NULL  (0)
#endif 
#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

//#define FILE_SUPPORT_BUFFER			/* �Ƿ�֧�ֻ���ĺ꿪�أ��򿪺���֧�ֻ��� */

#define _FILE_BUFF_LEN	4096		/* д�ļ�ʱ��Ļ����С */

/* define the open Method */
enum _openmethod
{
	OPEN_R = 1,				/* "r" */
	OPEN_W = 2,				/* "w" */
	OPEN_A = 4,				/* "a" */
	OPEN_OPT_B = 8,			/* "rb","wb","ab" */
	OPEN_OPT_ADD = 16		/* "r+","w+","a+","rb+"... */
};


#ifndef _FILE_DEFINED
struct _iobuf {
		int	 _file;			/* the id of this opened file in system */
		int  _flag;			/* "r", "w", "a", "rb", "wb", "ab", "r+", "w+", "ab+", "rb+", "wb+", "ab+" */
		long _curPos;		/* ��ǰ��/д����λ�ã��ļ��е���ʵλ�ã� */
		long _filelen;		/* �ļ��ܳ��� */

		/* ���ﲻ��֧�ֻ��� --- �Ͳ㺯���Ѿ�֧�� */
#ifdef FILE_SUPPORT_BUFFER
		char *_ptr;			/* �����е�ǰ��������λ�� */
		char *_base;		/* ����buffer����ʼ��ַ */
		int  _cnt;			/* ��ǰ�����ڻ��ж��ٿռ���� --- ����ʱ���ֵ��ʾ��Ч���ݻ��ж���/д��ʱ���ʾ���ж��ٿռ��д */
		int  _bufsiz;		/* ����buffer�Ĵ�С */
		int  _error;		/* ��������ʱ�ļ�¼ */
		int  _dirty;		/* seek���Ƿ񳬳��û��������1. �´ζ�/дʱ��Ҫ����buff��0������Ҫ */
#endif
        };
typedef struct _iobuf FILE;
#define _FILE_DEFINED
#endif


/* Seek method constants */
#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0


// ֧�֣�
int feof(FILE *);
long ftell(FILE *);

FILE * fopen(const char *, const char *);
size_t fread(void *, size_t, size_t, FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);
int fseek(FILE *, long, int);
int fclose(FILE *);

int fgetc(FILE *);
char * fgets(char *, int, FILE *);
int fputc(int, FILE *);
int fputs(const char *, FILE *);

// ��֧�֣�
//int ferror(FILE *);
//int fprintf( FILE *stream, const char *format [, argument ]...);
//int fscanf( FILE *stream, const char *format [, argument ]... );
//int fflush(FILE *);


#endif //_WIN32

#endif //FILE_H


