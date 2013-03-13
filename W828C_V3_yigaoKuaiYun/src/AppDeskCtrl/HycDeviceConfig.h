#ifndef __HYC_DEVICE_CONFIG_H__
#define __HYC_DEVICE_CONFIG_H__

//��ͷ�ļ���Ҫ��������Ӳ���豸�����ģ��벻Ҫ�����������

//#define HYCO_VERSION			"W828  1.1"
//#define HYCO_VERSION_APP		HYCO_VERSION##".008"
//#define HYCO_VERSION_INFO		"Version "##HYCO_VERSION_APP##" Build on "##__DATE__##" "##__TIME__

/*
W828  XAL  x.xxx.xxx
W828ΪӲ�����ͣ�ռ��5���ַ����������Կո��룬��W828  , W560  ....
XALΪ�ͻ���Ŀ��д��ռ��5���ַ����������ѿո��룬XAL  ������¡��LB   ��������......
x.xxx.xxx�����֣���ɫ�������λ�������׼��İ汾�ţ��ۺ�ɫ�����������ͻ��İ汾�ţ���0.001���ε���
*/
#define HYCO_VERSION			"W828 HYCO 1.20"
#define HYCO_VERSION_APP		HYCO_VERSION##"1.009"
#define HYCO_VERSION_INFO		"Version "##HYCO_VERSION_APP##" Build on "##__DATE__##" "##__TIME__

extern const char appVersionInfo_all[];
extern const char appVersionInfo[];

//�꿪�������������ɵĳ���
//#define RELEASE_HX
//#define RELEASE_TY
//#define RELEASE_TM
//#define RELEASE_R6//TG2
//#define RELEASE_HXRGB
//#define RELEASE_BYD
#define RELEASE_BYD_8877N
//#define RELEASE_BCT_HX8347
//#define DEBUG_HX
//#define DEBUG_TY
//#define DEBUG_TM
//#define DEBUG_R6//TG2
//#define DEBUG_HXRGB
//#define DEBUG_BYD

//������
#define HYC_DEV_TP_JC	//��˸

//-DUSE_WIFI

//����ͷ����
//#define HYC_DEV_SENSOR_130
//#define HYC_DEV_SENSOR_300
//#define HYC_DEV_SENSOR_NANO_300
//#define HYC_DEV_SENSOR_OV7690 //30W
//#define HYC_DEV_SENSOR_OV9660 //
//#define HYC_DEV_SENSOR_OV3640 //300W

//#define HYC_DEV_SENSOR_OV2655
//#define HYC_DEV_SENSOR_HM2055
#define HYC_DEV_SENSOR_HM2057

#if 0
//Lcd ��������
#if  defined (RELEASE_HX)|| defined(DEBUG_HX)
    #define HYC_DEV_LCD_HX8347
#elif defined(RELEASE_TY) || defined(DEBUG_TY)   
    #define HYC_DEV_LCD_N2748TLY
#elif defined(RELEASE_TM) || defined(DEBUG_TM) 
    #define HYC_DEV_LCD_TIANMA
#elif defined(RELEASE_R6) || defined(DEBUG_R6)     
    #define HYC_DEV_LCD_R61505V
#elif defined(RELEASE_HXRGB) || defined(DEBUG_HXRGB)    
    #define HYC_DEV_LCD_HXRGB
#elif defined(RELEASE_BYD) || defined(DEBUG_BYD)    
    #define HYC_DEV_LCD_BYD
#endif
#else//#if 0
#if defined(RELEASE_TY) || defined(DEBUG_TY)
	#define HYC_DEV_LCD_N2748TLY
#endif
#if defined(RELEASE_BYD) || defined(DEBUG_BYD)
	#define HYC_DEV_LCD_BYD
#endif
#if defined(RELEASE_BYD_8877N) || defined(DEBUG_BYD_8877N)
	#define HYC_DEV_LCD_8877N_BYD
#endif
#if defined(RELEASE_BCT_HX8347) || defined(DEBUG_BCT_HX8347)
	#define HYC_DEV_LCD_HX8347_BCT
#endif
#endif//#if 0

//ѡ�񱳹�����оƬ
#define CP2142MM	//��һ��ʱ���ڸ������������������������ȣ����ڲ���������(G20/G20W/W818C)
//#define ACT6311		//��������pwm��ռ�ձ��������������ȣ����ڴ���������(W900/W900B)

//WATCHDOG ����
//�����汾��Ҫʹ��watchdog
#define HYC_DEV_USE_WATCHDOG


//Ӳ���͹�������
#define HYC_DEV_USE_LOW_PS


#endif