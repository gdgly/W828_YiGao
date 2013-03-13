#include "string.h"
#include "epass_function.h"


#define PI					3.14159265358979323846
#define HALFPI				1.57079632679489661923//(PI/2)

#define RADIAN2ANGLE		57.29577951308//180/PI


double sin_val[] = 
{
	0,	
	0.01745240643728351,	0.03489949670250097,	0.05233595624294383,
	0.0697564737441253,		0.08715574274765816,	0.10452846326765346,
	0.12186934340514747,	0.13917310096006544,	0.15643446504023087,
	0.17364817766693033,	0.1908089953765448,		0.20791169081775931,
	0.22495105434386497,	0.24192189559966773,	0.25881904510252074,
	0.27563735581699916,	0.2923717047227367,		0.3090169943749474,
	0.3255681544571567,		0.3420201433256687,		0.35836794954530027,
	0.374606593415912,		0.3907311284892737,		0.40673664307580015,
	0.42261826174069944,	0.4383711467890774,		0.45399049973954675,
	0.4694715627858908,		0.48480962024633706,	0.49999999999999994,
	0.5150380749100542,		0.5299192642332049,		0.544639035015027,
	0.5591929034707468,		0.573576436351046,		0.5877852522924731,
	0.6018150231520483,		0.6156614753256583,		0.6293203910498375,
	0.6427876096865392,		0.6560590289905073,		0.6691306063588582,
	0.6819983600624985,		0.6946583704589972,		0.7071067811865475,
	0.7193398003386511,		0.7313537016191705,		0.7431448254773941,
	0.7547095802227719,		0.766044443118978,		0.7771459614569708,
	0.7880107536067219,		0.7986355100472928,		0.8090169943749474,
	0.8191520442889918,		0.8290375725550417,		0.8386705679454239,
	0.848048096156426,		0.8571673007021122,		0.8660254037844386,
	0.8746197071393957,		0.8829475928589269,		0.8910065241883678,
	0.898794046299167,		0.9063077870366499,		0.9135454576426009,
	0.9205048534524404,		0.9271838545667873,		0.9335804264972017,
	0.9396926207859083,		0.9455185755993167,		0.9510565162951535,
	0.9563047559630354,		0.9612616959383189,		0.9659258262890683,
	0.9702957262759965,		0.9743700647852352,		0.9781476007338057,
	0.981627183447664,		0.984807753012208,		0.9876883405951378,
	0.9902680687415704,		0.992546151641322,		0.9945218953682733,
	0.9961946980917455,		0.9975640502598242,		0.9986295347545738,
	0.9993908270190958,		0.9998476951563913,		1
};
double cos_val[] = 
{
	1,
	0.9998476951563913,		0.9993908270190958,		0.9986295347545738 ,
	0.9975640502598242,		0.9961946980917455,		0.9945218953682733 ,
	0.992546151641322,		0.9902680687415704,		0.9876883405951378 ,
	0.984807753012208,		0.981627183447664,		0.9781476007338057 ,
	0.9743700647852352,		0.9702957262759965,		0.9659258262890683 ,
	0.9612616959383189,		0.9563047559630355,		0.9510565162951535 ,
	0.9455185755993168,		0.9396926207859084,		0.9335804264972017 ,
	0.9271838545667874,		0.9205048534524404,		0.9135454576426009 ,
	0.9063077870366499,		0.898794046299167,		0.8910065241883679 ,
	0.882947592858927,		0.8746197071393957,		0.8660254037844387 ,
	0.8571673007021123,		0.848048096156426,		0.838670567945424 ,
	0.8290375725550417,		0.8191520442889918,		0.8090169943749474 ,
	0.7986355100472928,		0.7880107536067219,		0.7771459614569709 ,
	0.766044443118978,		0.754709580222772,		0.7431448254773942 ,
	0.7313537016191705,		0.7193398003386512,		0.7071067811865476 ,
	0.6946583704589974,		0.6819983600624985,		0.6691306063588582 ,
	0.6560590289905074,		0.6427876096865394,		0.6293203910498375 ,
	0.6156614753256583,		0.6018150231520484,		0.5877852522924731 ,
	0.5735764363510462,		0.5591929034707468,		0.5446390350150272 ,
	0.5299192642332049,		0.5150380749100544,		0.5000000000000001 ,
	0.4848096202463371,		0.46947156278589086,	0.4539904997395468 ,
	0.43837114678907746,	0.42261826174069944,	0.4067366430758004 ,
	0.3907311284892737,		0.3746065934159122,		0.35836794954530015 ,
	0.3420201433256688,		0.32556815445715675,	0.30901699437494745 ,
	0.29237170472273677,	0.27563735581699916,	0.25881904510252074 ,
	0.24192189559966767,	0.22495105434386514,	0.20791169081775923 ,
	0.19080899537654491,	0.17364817766693041,	0.15643446504023092 ,
	0.13917310096006546,	0.12186934340514749,	0.10452846326765346 ,
	0.08715574274765836,	0.06975647374412523,	0.052335956242943966 ,
	0.03489949670250108,	0.0174524064372836,		0
};


//standard
void* epass_memcpy(void * _Dst, const void * _Src, size_t _Size){
	return memcpy( _Dst, _Src, _Size );
}

void* epass_memset(void * _Dst, int _Val, size_t _Size){
	return memset( _Dst, _Val, _Size );
}

char* epass_strcpy(char * _Dst, const char * _Src){
	return strcpy( _Dst, _Src );
}

char* epass_strcat(char * _Dst, const char * _Src){
	return strcat( _Dst, _Src );
}

char* epass_strncat(char * _Dst, const char * _Src, size_t _Size){
	return strncat( _Dst, _Src, _Size );
}

char* epass_strstr(char * _Str, const char * _SubStr){
	return strstr( _Str, _SubStr );
}

char* epass_strchr(char * _Str, int _Ch){
	return strchr( _Str, _Ch );
}

int epass_strcmp(const char * _Str1, const char * _Str2){
	return strcmp( _Str1, _Str2 );
}

int epass_islower(int c){
	return islower(c);
}

int epass_isupper(int c){
	return isupper(c);
}

int epass_tolower(int c){
	return tolower(c);
}

int epass_toupper(int c){
	return toupper(c);
}

int epass_atoi(const char *_Str){
	return atoi( _Str );
}

size_t epass_strlen(const char * _Str){
	return strlen( _Str );
}

void * epass_malloc(size_t size){
	return malloc(size);
}

void epass_free(void* ptr){
	free( ptr );
}

int epass_dprintf(const char * _Format, ...){
	//hyUsbPrintf(_Format);
	return 0;
}

int epass_fprintf(void * _File, const char * _Format, ...){
	return 0;
}

int epass_sprintf(char * _Dst, const char * _Format, ...){
	return 0;
}

void epass_qsort(void * _Base, size_t _NumOfElements, size_t _SizeOfElements, int (* _PtFuncCompare)(const void *, const void *)){
	return ;
}

//math
double epass_atan(double _X)
{
	double val;
	double TX,X3,X5,X7;
	
	if(_X >= -1.0 && _X <= 1.0)
	{
		X3 = _X*_X*_X;
		X5 = X3*_X*_X;
		X7 = X5*_X*_X;
		
		val = _X-X3/3+X5/5-X7/7;
	}
	else
	{//ATAN(X)+ATAN(1/X)=PI/2，先计算出atan(1/x)后，再计算atan(x) 
		TX = 1/_X;
		X3 = TX*TX*TX;
		X5 = X3*TX*TX;
		X7 = X5*TX*TX;
		
		val = TX-X3/3+X5/5-X7/7;
		
		val = HALFPI - val;
	}
hyUsbPrintf("epass_atan \r\n");
	return val;
}

//sinX = X- X3/3! + X5/5! - X7/7!....
double epass_sin(double _X)
{
	double val;
	
	int    angle,sing = 1;
	
	angle = (int)(_X*RADIAN2ANGLE);
	
	angle = angle%360;
	
	if(angle < 0)
	{
		sing *= -1;
		angle *= -1;
	}
	
	if(angle >= 180)
	{
		sing *= -1;
		angle -= 180;
	}
	
	if(angle > 90)
	{
		angle = 180 - angle;
	}
	
	val = sing*sin_val[angle];
hyUsbPrintf("epass_sin \r\n");
	return val;
}

//cosX = 1- X2/2! + X3/3!...
double epass_cos(double _X)
{
	double val;
	int    angle,sing = 1;
	
	angle = (int)(_X*RADIAN2ANGLE);
	
	angle = angle%360;
	
	if(angle < 0)
	{
		angle *= -1;
	}

	if(angle >= 180)
	{
		sing *= -1;
		angle -= 180;
	}
	
	if(angle > 90)
	{
		sing *= -1;
		angle = 180 - angle;
	}
	
	val = sing*cos_val[angle];
hyUsbPrintf("epass_cos \r\n");	
	return val;
}

