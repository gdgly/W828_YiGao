

#include "std.h"
#include "types.h"


unsigned long int strtoul(const char *nptr, char **endptr, int base)
{
	unsigned long int number;

	/* Sanity check the arguments */
	if (base==1 || base>36 || base<0)base=0;

	/* advance beyond any leading whitespace */
	while (isspace(*nptr))nptr++;

	/* check for optional '+' or '-' */
	if (*nptr=='-')
		nptr++;
	else if (*nptr=='+')
		nptr++;


	/* If base==0 and the string begins with "0x" then we're supposed
	 to assume that it's hexadecimal (base 16). */
	if (base==0 && *nptr=='0')
	{
		if (toupper(*(nptr+1))=='X')
		{
			base=16;
			nptr+=2;
		}
		/* If base==0 and the string begins with "0" but not "0x",
		then we're supposed to assume that it's octal (base 8). */
		else
		{
			base=8;
			nptr++;
		}
	}

	/* If base is still 0 (it was 0 to begin with and the string didn't begin
	 with "0"), then we are supposed to assume that it's base 10 */
	if (base==0)base=10;

	number=0;
	while (isascii(*nptr) && isalnum(*nptr))
	{
		int ch = *nptr;
		
		if (islower(ch)) ch = toupper(ch);
		ch -= (ch<='9' ? '0' : 'A'-10);
		if (ch>base)break;

		number= (number*base)+ch;
		nptr++;
	}

	/* Some code is simply _impossible_ to write with -Wcast-qual .. :-\ */
	if (endptr!=NULL)*endptr=(char *)nptr;

	/* All done */
	return number;
}




/*-----------------------------------------------------------------------------
* ����:	strtol()	�˺���Ϊstdlib.h�������Ŀ⺯��,����std.h��û�У��Լ�ʵ��
* ����:	����ֵ�ַ���ת���ɳ����θ�ʽ��
* ����:	nptr		��ת���ĸ����ַ���
		endptr		ת�����β���ַ���
		base		ת��������
* ����:	������ת�����
*----------------------------------------------------------------------------*/
long int strtol(const char *nptr, char **endptr, int base)
{
	const char * ptr;
	unsigned short negative;
	long int number;

	ptr=nptr;

	while (isspace(*ptr))ptr++;

	negative=0;
	if (*ptr=='-')negative=1;

	number=(long int)strtoul(nptr, endptr, base);

	return (negative ? -number:number);
}
