#ifndef OP_DOWNLOAD_H
#define OP_DOWNLOAD_H


#include "hyTypes.h"
#include "Op_Common.h"
#include "Op_Config.h"


//����
#define LOAD_COMPANY_MAX   (hsaSdram_CompanySize()/sizeof(tCOMPANY))
#define COMPANY_FILE_PATH	"C:/data/Company.bin" //�����ļ�·��
#define LOAD_COMPANY_BAK    "C:/data/bak/Company.bak"
#define COMPANY_VER_LEN		38

//��Ա
#define LOAD_EMPLOYEE_MAX   (hsaSdram_EmployeSize()/sizeof(tEMPLOYEE))
#define EMPLOYEE_FILE_PATH	"C:/data/Employee.bin" //�����ļ�·��
#define LOAD_EMPLOYEE_BAK   "C:/data/bak/Employee.bak"
#define EMPLOYEE_VER_LEN	38

//����ԭ��
#define LOAD_PROBLEM_MAX   (hsaSdram_ProblemSize()/sizeof(tPROBLEM))
#define PROBLEM_FILE_PATH	"C:/data/Problem.bin" //�����ļ�·��
#define LOAD_PROBLEM_BAK    "C:/data/bak/Problem.bak"

//����ԭ��
#define LOAD_LEAVE_MAX   (hsaSdram_LeaveSize()/sizeof(tLEAVE))
#define LEAVE_FILE_PATH	"C:/data/Leave.bin" //�����ļ�·��
#define LOAD_LEAVE_BAK    "C:/data/bak/Leave.bak"

//���䷽ʽ
#define LOAD_CLASS_MAX   (hsaSdram_WeiVolSize()/sizeof(tCLASS))
#define CLASS_FILE_PATH	  "C:/data/Class.bin" //�����ļ�·��
#define LOAD_CLASS_BAK    "C:/data/bak/Class.bak"


//Ŀ�ĵ�
#define LOAD_DESTADDR_MAX   (hsaSdram_RegionSize()/sizeof(tDESTADDR))
#define DESTADDR_FILE_PATH	"C:/data/DestAddr.bin" //�����ļ�·��
#define LOAD_DESTADDR_BAK    "C:/data/bak/DestAddr.bak"

//������Ϣ
#define LOAD_FLIGHT_MAX   (hsaSdram_FlightSize()/sizeof(tFLIGHT))
#define FLIGHT_FILE_PATH   "C:/data/Flight.bin" //�����ļ�·��
#define LOAD_FLIGHT_BAK    "C:/data/bak/Flight.bak"

//������Ϣ
#define LOAD_CAR_MAX   (hsaSdram_CarSize()/sizeof(tCAR))
#define CAR_FILE_PATH	"C:/data/Car.bin" //�����ļ�·��
#define LOAD_CAR_BAK    "C:/data/bak/Car.bak"


//�쳣ԭ��
#define ABNORMITY_FILE_PATH	"C:/data/Abnormity.bin" //�����ļ�·��

//��������
#define AREA_TEMPFILE_PATH	"C:/data/site_temp.zip"
#define AREA_CSVFILE_PATH	"C:/data/site.csv"
#define AREA_FILE_PATH		"C:/data/site2.bin" //�����ļ�·��
#define AREA_VER_LEN		32

#define LOAD_VER_NULL "2007-01-01 00:00:00"//
//�����������
#define WEIVOL_FILE_PATH	"C:/data/WeiVol.bin" //�����ļ�·��

typedef int (*DownLoadFun)(void *pView, U8 flag);


typedef struct _download_cfg_t
{
	U8			*pName;	//��ʾ����
	DownLoadFun	fun;	//ִ�к���
	
}tDOWNLOAD_CFG;


typedef struct _employee_t
{
	U8	company[COMPANY_ID_LEN];		//������˾
	U8	userid[EMPLOYEE_ID_LEN];		//�û����
	U8	username[EMPLOYEE_NAME_LEN];	//�û���
	U8	userpw[EMPLOYEE_PW_LEN];		//�û�����
	U8	userrole[EMPLOYEE_ROLE_LEN];	//�û���ɫ
	//U8	lasttime[20];	//������ʱ��

}tEMPLOYEE;



typedef struct _company_t
{
	U8	companyid[COMPANY_ID_LEN];	//��˾����
	U8	company[COMPANY_NAME_LEN];	//��˾����
	U8	right[COMPANY_RIGHT_LEN];	//

}tCOMPANY;

//�����ԭ���
typedef struct _problem_t
{
	U8	code[PROBLEM_CODE_LEN];	//����
	U8	name[PROBLEM_NAME_LEN];	//��������
	
}tPROBLEM;

//�����ԭ���
typedef struct _leave_t
{
	U8	code[LEAVE_CODE_LEN];	//����
	U8	name[LEAVE_NAME_LEN];	//��������
	
}tLEAVE;


//���䷽ʽ
typedef struct _tClass_
{
	U8	id[CLASS_ID_LEN];
	U8	name[CLASS_NAME_LEN];
	
}tCLASS;


typedef struct _Destination
{
	U8	id[COMPANY_ID_LEN];	//����
	U8	name[COMPANY_NAME_LEN];	//����
	
}tDESTADDR;

typedef struct _FLIGHT
{
	U8	id[FLIGHT_ID_LEN];	//����
	U8	name[FLIGHT_NAME_LEN];	//����
	
}tFLIGHT;

typedef struct _CAR
{
	U8	id[CAR_ID_LEN];	//����
	U8	name[CAR_NAME_LEN];	//����
	
}tCAR;

typedef struct _abnormal_t
{
	U8	code[PROBLEM_CODE_LEN];	//����
	U8	name[PROBLEM_NAME_LEN];	//����
	
}tABNORMAL;

//������������б�
typedef struct _volwei_t
{
	U8  type ;      //����: 1 ��� 2 ����
	U8	id[8];		//���
	U8	name[100];	//����
	
}tVOLWEI;

//��������
typedef  struct _areainfo_t
{
	U8  *code ;
	U8  *area ;	
	struct _areainfo_t *next ;
	U8  nodenum;
	
}tAREAINFO;


#define AREA_S_DID1			(0x2000)

#define SITE_MEM_MAX		(0x300000) //����������󻺳�3M

#define SITE_FILE_MAX		(0x280000)          //2.5M

#define SITE_MAX_RECORD		(8000) //����¼����,Ŀǰ��8355

typedef struct csvst
{
	U32 offset[15] ; //�ַ��������buf��offset

}tCSV;

/*�ļ��ṹ
CSV ��ѹ��ת�����ļ���ʽ : name: siteshow.bin
 ____________________
| 32 Byte  ver       |
 --------------------
| 2 Byte CRC         |
 -------------------- 
| n Byte Data        |
 -------------------- 

�ϵ���������ʱ������ļ���ʽ :name: sitedown.temp
 ____________________
| 32 Byte  ver       |
 --------------------
| 2 Byte CRC         |
 -------------------- 
| 4 Byte total size  |
 --------------------
| n Byte Data        |
 -------------------- 
 
*/

//����
extern char	ga8CompanyVer[];
extern U32	gu32CompanyBufSize;
extern U32 	gu32CompanyTotal;//���ص��ܸ���
extern tCOMPANY		*gptCompany;

//��Ա
extern char	ga8EmployeeVer[];
extern U32	gu32EmployeeBufSize;
extern U32 	gu32EmployeeTotal;//���ص�Ա���ܸ���
extern tEMPLOYEE	*gptEmployee;

//�����
extern U32	gu32ProblemTotal;
extern U32	gu32ProblemBufSize;
extern tPROBLEM	*gptProblem;

//���ּ�
extern U32	gu32LeaveTotal;
extern U32	gu32LeaveBufSize;
extern tLEAVE	*gptLeave;

//���䷽ʽ
extern U32	gu32ClassBufSize;
extern U32 	gu32ClassTotal;//�����쳣��������
extern tCLASS *gptClass;//����쳣����

//Ŀ�ĵ�
extern U32	gu32DestAddrBufSize;
extern U32 	gu32DestAddrTotal;//����Ŀ�ĵ�����
extern tDESTADDR *gptDestAddr;//���Ŀ�ĵ�

//������Ϣ
extern U32	gu32FlightBufSize;
extern U32 	gu32FlightTotal;//���غ�����Ϣ����
extern tFLIGHT *gptFlight;//��ź�����Ϣ

//������Ϣ
extern U32	gu32CarBufSize;
extern U32 	gu32CarTotal;//���س�����Ϣ����
extern tCAR *gptCar;//��ų�����Ϣ

//�쳣ԭ��
extern U32	gu32AbnormalBufSize;
extern U32 	gu32AbnormalTotal;//�����쳣��������
extern tABNORMAL *gptAbnormal;//����쳣����



//��������
extern U8  gu8AreaVer[];


//�����������
extern U32		gu32WeiVolTotal;
extern U32		gu32WeiVolBufSize;
extern U16		gu16VolIndex;
extern tVOLWEI	*gptWeiVol;




extern U16 *gpu16SearchInfo;
/*=====================================================================*/

#endif	//OP_DOWNLOAD_H