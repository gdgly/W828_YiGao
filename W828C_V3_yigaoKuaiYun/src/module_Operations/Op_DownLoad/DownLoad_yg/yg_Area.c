/**********************************************************************
������������   ��ʵʱռ��buf,ֻ�������ػ��ѯʱ��ʹ��,����û�й̶�����buf
**********************************************************************/
#include "hyTypes.h"
#include "grap_api.h"
#include "base.h"
#include "http.h"
#include "glbVariable.h"
#include "Common_Dlg.h"
#include "Common_Widget.h"
#include "Common_AppKernal.h"
#include "AbstractFileSystem.h"
#include "Net_Evt.h"
#include "Op_Common.h"
#include "Op_DownLoad.h"
#include "Op_Struct.h"
#include "Op_RequestInfo.h"


typedef struct ProvinceSearchList
{
	U8 *Province;//ʡ������
	U16 Count;//��ѯ���ļ�¼����
}ProvinceSearchList;
U8 ProvinceFlag[34];
ProvinceSearchList Province[34] = 
{
	{(U8 *)"������",0},
	{(U8 *)"�����",0},
	{(U8 *)"�ӱ�ʡ",0},
	{(U8 *)"ɽ��ʡ",0},
	{(U8 *)"���ɹ�",0},
	{(U8 *)"����ʡ",0},
	{(U8 *)"����ʡ",0},
	{(U8 *)"������ʡ",0},
	{(U8 *)"�Ϻ���",0},
	{(U8 *)"����ʡ",0},
	{(U8 *)"�㽭ʡ",0},
	{(U8 *)"����ʡ",0},
	{(U8 *)"����ʡ",0},
	{(U8 *)"����ʡ",0},
	{(U8 *)"ɽ��ʡ",0},
	{(U8 *)"����ʡ",0},
	{(U8 *)"����ʡ",0},
	{(U8 *)"����ʡ",0},
	{(U8 *)"�㶫ʡ",0},
	{(U8 *)"����",0},
	{(U8 *)"����ʡ",0},
	{(U8 *)"������",0},
	{(U8 *)"�Ĵ�ʡ",0},
	{(U8 *)"����ʡ",0},
	{(U8 *)"����ʡ",0},
	{(U8 *)"����",0},
	{(U8 *)"����ʡ",0},
	{(U8 *)"����ʡ",0},
	{(U8 *)"�ຣʡ",0},
	{(U8 *)"����",0},
	{(U8 *)"�½�",0},
	{(U8 *)"̨��ʡ",0},
	{(U8 *)"���",0},
	{(U8 *)"����",0}	
};
#define KYEWORDLEN 34
extern U8  gu8Url[];

U8  gu8AreaVer[AREA_VER_LEN], gu8AreaVerTemp[AREA_VER_LEN];
U8 Keyword[KYEWORDLEN];

/*---------------------------CSV�ļ��淶-------------------------
����
CSV�ļ���Ҳ�ж��ŷָ�ֵ�ļ���Ӣ������COMMA SEPARATED VALUE�������ʽ�������£�
* ÿ����¼ռһ��
* �Զ���Ϊ�ָ���
* ����ǰ��Ŀո�ᱻ����
* �ֶ��а����ж��ţ����ֶα�����˫����������
* �ֶ��а����л��з������ֶα�����˫����������
* �ֶ�ǰ������пո񣬸��ֶα�����˫����������
* �ֶ��е�˫����������˫���ű�ʾ
* �ֶ��������˫���ţ����ֶα�����˫����������
* ��һ����¼���������ֶ���
----------------------------------------------------------------*/
U8		*gu8Allspace,*pgu8Strs,*pgu8Strcur,*gu8StrsShow;
tCSV	*ptAllinfo;
U16		*pgu16Searchs,*province_str,search_total=0;


/*---------------------------------------------------------------------
*������DownLoad_Area_Init
*���ܣ���ʼ������������Ϣ
*������pBuf:�ⲿ�ṩbuf
*���أ�
----------------------------------------------------------------------*/
void DownLoad_Area_Init(U8 *pBuf)
{
	pBuf = (U8 *)(((U32)pBuf + 4) & 0xfffffffc); //4�ֽڶ���
	
	gu8Allspace = pBuf;
	memset(gu8Allspace,0,SITE_MEM_MAX);
	ptAllinfo   = (tCSV *)gu8Allspace ;
	pgu8Strs    = gu8Allspace + sizeof(tCSV)*SITE_MAX_RECORD;
	pgu8Strcur  = pgu8Strs ;
	pgu16Searchs= (U16 *)(pBuf+SITE_MEM_MAX);
	memset((char *)pgu16Searchs,0,0x20000);
	pgu16Searchs = (U16 *)(((U32)pgu16Searchs + 4) & 0xfffffffc); //4�ֽڶ���
	province_str = (U8 *)hsaSdram_GetChaXunBuf();
	memset(province_str,0,hsaSdram_GetChaXunBufSize());
}


void DownLoad_Area_WriteFile(U8 *ver)
{
	int fp,size;
	U16 crc;
	fp = AbstractFileSystem_Open(AREA_FILE_PATH,AFS_READ);
	if(fp >= 0)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(AREA_FILE_PATH,AFS_SINGLE_DELETE_FDM);
	}
	fp = AbstractFileSystem_Create(AREA_FILE_PATH);
	if(fp >= 0)
	{
		size = pgu8Strcur - gu8Allspace ;
		size = size > SITE_MEM_MAX ? SITE_MEM_MAX : size ;
		
		crc = glbVariable_CalculateCRC((U8 *)gu8Allspace,size);
		
		AbstractFileSystem_Write(fp,ver,AREA_VER_LEN);
		AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
		AbstractFileSystem_Write(fp,gu8Allspace,size);
		AbstractFileSystem_Close(fp);
		
		memset(gu8AreaVer,0,AREA_VER_LEN);
		strncpy(gu8AreaVer,ver,AREA_VER_LEN);
	}
}


int DownLoad_Area_ReadFile(void)
{
	U16 crc ;
	int fp , size;
	
	fp = AbstractFileSystem_Open(AREA_FILE_PATH,AFS_READ);
	if(fp < 0) return -1 ;
	
	size = AbstractFileSystem_FileSize(fp);
	if(size <= AREA_VER_LEN+4)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(AREA_FILE_PATH,AFS_SINGLE_DELETE_FDM);
		memset(gu8AreaVer,0,AREA_VER_LEN);
		return -1;
	}
	size -= (AREA_VER_LEN+4);
	
	AbstractFileSystem_Read(fp,gu8AreaVer,AREA_VER_LEN);
	AbstractFileSystem_Read(fp,(char *)&search_total,sizeof(U16));
	AbstractFileSystem_Read(fp,(char *)&crc,sizeof(U16));
	AbstractFileSystem_Read(fp,gu8Allspace,size);
	AbstractFileSystem_Close(fp);
	if(crc != glbVariable_CalculateCRC((U8 *)gu8Allspace,size))
	{
		memset(gu8AreaVer,0,AREA_VER_LEN);
		return -2 ;
	}
	
	return 0;
}


int DownLoad_Area_InitVer(void)
{
	U16 crc ;
	int fp , size;
	
	memset(gu8AreaVer,0,AREA_VER_LEN);
	
	fp = AbstractFileSystem_Open(AREA_FILE_PATH,AFS_READ);
	if(fp < 0) return -2 ;
	
	size = AbstractFileSystem_FileSize(fp);
	size -= (AREA_VER_LEN+2);
	
	//��ʡʱ��Ͳ������ݽ���У����
	AbstractFileSystem_Read(fp,gu8AreaVer,AREA_VER_LEN);
	//AbstractFileSystem_Read(fp,(char *)&crc,sizeof(U16));
	//AbstractFileSystem_Read(fp,pdatabuf,size);
	AbstractFileSystem_Close(fp);
	
	//if(crc != glbVariable_CalculateCRC((U8 *)pdatabuf,size))
	//{
	//	memset(gu8AreaVer,0,AREA_VER_LEN) ;
	//	return -2;
	//}
	
	return 0 ;
}



/*-------------------------------------------------------------------------
* ����: DownLoad_Area_GetOneline
* ˵��: ȡһ���ַ�����,0d 0a ��Ϊ��һ��
* ����: 
* ����: 
* -------------------------------------------------------------------------*/
#if 0
int DownLoad_Area_GetOneline(U8 *sin,U32 maxlen,U8 *sout,U32 curlen)
{
	U32 i,j,k ;
	U32 l ;
	U8  *pi,*po ;

	i  = j = k = l = 0 ;
	pi = sin ;
	po = sout; 
	
	while (1)
	{
		if (0 == *pi)
		{
			if (curlen+i < maxlen)
			{
				i++;
				pi++;
				continue ;
			}
			else
			{
				return -1 ;
			}
		}
		
		if (0x0d == *pi && 0x0a == *(pi+1))
		{
			break;
		}
		*po++ = *pi++ ;
		i++ ;
		
		if(i >= 0x20000)
		{
			return -2 ;
		}

	}
	i += 2 ; // �س�����
	
	return  i ;
}
#else
int DownLoad_Area_GetOneline(U8 *sin,U32 maxlen,U8 *sout,U32 curlen)
{
	U32 i,j,k ;
	U32 l ;
	U8  *pi,*po ;

	i  = j = k = l = 0 ;
	pi = sin ;
	po = sout; 
	
	while (1)
	{
		
		if (0 == *pi)
		{
			if (curlen+i < maxlen)
			{
				i++;
				pi++;
				continue ;
			}
			else
			{
				return -1 ;
			}
		}
		if (curlen==0&&0x0d == *pi && 0x0a == *(pi+1))
		{
			break;
		}
		else if (0xa1 == *pi && 0xa3 == *(pi+1)&&0x0d == *(pi+2)&&0x0a == *(pi+3)&&(0x00 == *(pi+4)||(0x30 <= *(pi+4)&&0x39 >= *(pi+4))))//���\r\n��Ϊһ�еĽ���
		{
			break;
		}
		*po++ = *pi++ ;
		i++ ;
		
		if(i >= 0x20000)
		{
			return -2 ;
		}

	}
	i += 2 ; // ���
	i += 2 ; // �س�����
	
	return  i ;
}
#endif

/*-------------------------------------------------------------------------
* ����: DownLoad_Area_ParseOneline
* ˵��: ����һ���ַ���
* ����: 
* ����: 
* -------------------------------------------------------------------------*/
#if 0
int DownLoad_Area_ParseOneline(U8 *sin,U32 inlen,tCSV *pt)
{
	U8  i ;
	U8  *pi ,*ptem;
	U8  scnt ;               //��¼����˫���ŵĸ��� ��������:Ԫ�ؿ�ʼ����� ��ż����:�ַ�������
	U8  index ,elef;

	ptem = pgu8Strcur ;
	pi = sin ;
	scnt = 0 ;
	index= 0 ;
	elef = 0 ;
	pt->offset[index] = ptem - pgu8Strs ;
	
	while (1)
	{
		if (0 == *pi) break;
		
		// ele start 
		if ('"' == *pi)
		{	
			pi++   ;
			scnt++ ;
			continue ;
		}
		else
		{
			if (scnt&0x01 != 0)//������ "
			{
				for (i = 0 ; i < (scnt-1)/2 ; i++)
				{
					*ptem++ = '"' ;
				}

				if (',' == *pi)// һ��Ԫ�ؽ���
				{
					elef = 0 ;
					pi++ ;
					index++ ;
					ptem++  ; // û���ַ�����һ���ռ��� 0
					pt->offset[index] = ptem - pgu8Strs ;
				}
				else
				{
					elef = 1 ; //����һ��Ԫ��
					*ptem++ = *pi++ ;
				}
			}
			else               //ż������0��
			{
				if (0 != scnt)//Ԫ�ز��ֺ��� "
				{
					for (i = 0 ; i < scnt/2 ; i++)
					{
						*ptem++ = '"' ;
					}
					*ptem++ = *pi++ ;
				}
				else
				{
					if (',' == *pi && 0 == elef)// һ��Ԫ�ؽ���
					{
						pi++ ;
						index++ ;
						ptem++  ; // û���ַ�����һ���ռ��� 0
						pt->offset[index] = ptem - pgu8Strs ;
					}
					else
					{
						*ptem++ = *pi++ ;
					}
				}
			}
			
			scnt = 0 ;
		}
	}
	ptem++;//����һ��0�ַ���Ϊ������
	pgu8Strcur = ptem ;

	return 0;
}
#else
int DownLoad_Area_ParseOneline(U8 *sin,U32 inlen,tCSV *pt)
{
	U8  i ;
	U8  *pi ,*ptem;
	U8  scnt ;               //��¼����˫���ŵĸ��� ��������:Ԫ�ؿ�ʼ����� ��ż����:�ַ�������
	U8  index ,elef;
	ptem = pgu8Strcur ;
	pi = sin ;
	scnt = 0 ;
	index= 0 ;
	elef = 0 ;
	pt->offset[index] = ptem - pgu8Strs ;
	
	while (1)
	{
		if (0 == *pi) break;
		
		// ele start 
		if ('"' == *pi)
		{	
			pi++   ;
			scnt++ ;
			continue ;
		}
		else
		{
			if (scnt&0x01 != 0)//������ "
			{
				for (i = 0 ; i < (scnt-1)/2 ; i++)
				{
					*ptem++ = '"' ;
				}

				if ('	' == *pi)// һ��Ԫ�ؽ���
				{
					elef = 0 ;
					*pi = 0;
					pi++ ;
					index++ ;
					ptem++  ; // ÿ���ַ�����һ���ռ��� 0
					pt->offset[index] = ptem - pgu8Strs ;
					hyUsbPrintf("����\r\n");
				}
				else
				{
					elef = 1 ; //����һ��Ԫ��
					*ptem++ = *pi++ ;
				}
			}
			else               //ż������0��
			{
				if (0 != scnt)//Ԫ�ز��ֺ��� "
				{
					for (i = 0 ; i < scnt/2 ; i++)
					{
						*ptem++ = '"' ;
					}
					*ptem++ = *pi++ ;
				}
				else
				{
					if ('	' == *pi && 0 == elef)// һ��Ԫ�ؽ���
					{
						*pi = 0;
						pi++ ;
						index++ ;
						ptem++  ; // ÿ���ַ�����һ���ռ��� 0
						pt->offset[index] = ptem - pgu8Strs ;
							
					}
					else
					{
						*ptem++ = *pi++ ;
					}
				}
			}
			
			scnt = 0 ;
		}
	}
	ptem++;//����һ��0�ַ���Ϊ������
	pgu8Strcur = ptem ;

	return 0;
}
#endif

/*-------------------------------------------------------------------------
* ����: DownLoad_Area_DataProcess
* ˵��: �����ȡ���ļ�����
* ����:    ע�⣺pStrsIn buf ������ʹ��֮ǰһ��Ҫ����շ��� DownLoad_Area_GetOneline �������ܲ������
* ����: 0 :success ;
* -------------------------------------------------------------------------*/
void DownLoad_Area_DataProcess(U8 *pStrsIn,U32 totallen)
{
	U32  curIndex             ;
	U8   *outbuf,*inbuf       ;
	int  datalen,ret          ;
	U16  i                    ;
	
	curIndex = 0 ;
	inbuf  = pStrsIn ;
	outbuf = (U8 *)hsaSdram_DecodeBuf() ;
	memset(outbuf,0,hsaSdram_DecodeSize()) ;
hyUsbPrintf("str addr = 0x%x   len = %d  out addr = 0x%x \r\n", (U32)pStrsIn, totallen, (U32)outbuf);
	//�ļ��ĵ�һ���Ǳ���,��ȡ��ȥ��
	datalen = DownLoad_Area_GetOneline(inbuf,totallen,outbuf,curIndex);
	if (datalen < 0)return;  //�ļ�������
	inbuf += datalen ;
	curIndex += datalen ;
	
	for(i = 0 ; i < SITE_MAX_RECORD; i++)
	{
		memset(outbuf,0,hsaSdram_DecodeSize()) ;
		datalen = DownLoad_Area_GetOneline(inbuf,totallen,outbuf,curIndex);
		hyUsbPrintf("line %d:\r\n",i);
		if (datalen < 0)
		{
			search_total = i;
			break;  //�ļ�������
		}
		inbuf += datalen ;
		curIndex += datalen ;
		
		memset(&ptAllinfo[i], 0, sizeof(tCSV));
		ret = DownLoad_Area_ParseOneline(outbuf,0x10000,&ptAllinfo[i]);
		//hyUsbPrintf("i = %d  ret = %d  addr = 0x%x \r\n", i, ret, (U32)&ptAllinfo[i]);
	}
	
	hyUsbPrintf("ret = %d \r\n", ret);
}


/*-------------------------------------------------------------------------
* ����: DownLoad_Area_GetFileData
* ˵��: ��ȡsite.csv�ļ�����
* ����: 
* ����: 0 :success
* -------------------------------------------------------------------------*/
int DownLoad_Area_GetFileData(void)
{
	int  ret ,fp ;
	int  datalen ;
	U8     *pTem ;
	
	ret = 1 ;
	/* ����ռ��ݴ洦���ļ����� */
	pTem = (U8 *)hsaSdramReusableMemoryPool();
	memset(pTem,0,SITE_FILE_MAX);
	
	/* ��ȡ�ļ����� */
	fp = AbstractFileSystem_Open(AREA_FILE_PATH,AFS_READ);
	if(fp >= 0)
	{
		datalen = AbstractFileSystem_Read(fp,(char *)pTem,SITE_FILE_MAX);
		if(datalen > 0)
		{
			//PhoneTrace4(0,"read : = %d",datalen);
			/* �����ݽ��д��� */
 			DownLoad_Area_DataProcess(pTem,datalen);
		}
		else
		{
			ret = 3 ;
		}
		AbstractFileSystem_Close(fp);
	}
	else
	{
		ret = 2 ;
	}
	
	return ret ;
}

/*-------------------------------------------------------------------------
* ����: DownLoad_Area_Search
* ˵��: 
* ����: 
* ����: 0 :success ;
* -------------------------------------------------------------------------*/
#if 0
int DownLoad_Area_Search(U8 *strs, U8 searchf, tCSV *ptin, U16 *pout)
{
	tCSV *ptNext;
	int i,n;
	U8  index, *curStr;

	ptNext = ptin;
	index = searchf;
	n = 0;
	for (i = 0 ; i < search_total/*SITE_MAX_RECORD*/; i++)
	{
		//hyUsbPrintf("addr = 0x%x   off = %d \r\n", (U32)pgu8Strs, ptNext[i].offset[index]);
		curStr = (U8 *)((U32)pgu8Strs+ptNext[i].offset[index]);
		//hyUsbPrintf("i = %d  str = %s \r\n", i, curStr);
		//if(curStr == pgu8Strs && i > 0)break; //�������
		if (0 != strstr(curStr,strs,strlen(strs)))
		{
			pout[n] = i;
			n++;
		}	
	}

	return  n;
}
#else
int DownLoad_Area_Search(U8 *strs,U8 searchf, tCSV *ptin,U16 *pout)
{
	tCSV *ptNext ;
	int i=0 ,n=0,j=0;
	U8  index ,*curStr;
	
	ptNext = ptin ;
	
	index = searchf;
	for(i=0;i<34;i++)
	{
		Province[i].Count=0;
	}
	hyUsbPrintf("search_total = %d\r\n",search_total);
	for (i = 0 ; i < search_total ; i++)
	{
		curStr = (U8 *)((U32)pgu8Strs+ptNext[i].offset[index]);
		//if(curStr == pgu8Strs && i > 0)break; //�������
		if (0 != strstr(curStr,strs,strlen(strs)))
		{
			pout[n] = i ;
			n++;
			for(j=0;j<34;j++)
			{
				if (0 != strstr((U8 *)((U32)pgu8Strs+ptNext[i].offset[4]),Province[j].Province,strlen(Province[j].Province)))
				{
					Province[j].Count++;
					//hyUsbPrintf("%s/%s:%d/%d,n=%d\r\n",(U8 *)((U32)pgu8Strs+ptNext[i].offset[4]),Province[j].Province,i,pout[n-1],n-1);
					break;
				}
			}
		}	
	}

	return  n ;

}
#endif


//size:0:��ʾ��   >0:��ʾд
int DownLoad_TempFile(U8 *pBuf, int size, U8 *pVer)
{
	int fp, len=0;
	int filesize;
	U8  ver[AREA_VER_LEN];
	
	if(size == 0)
	{
		fp = AbstractFileSystem_Open(AREA_TEMPFILE_PATH, AFS_READ);
		if(fp >= 0)
		{
			filesize = AbstractFileSystem_FileSize(fp);
			if(filesize <= AREA_VER_LEN)
			{
				AbstractFileSystem_Close(fp);
				AbstractFileSystem_DeleteFile(AREA_TEMPFILE_PATH, AFS_SINGLE_DELETE_FDM);
			}
			else
			{
				AbstractFileSystem_Seek(fp, 0, AFS_SEEK_OFFSET_FROM_START);
				AbstractFileSystem_Read(fp, ver, AREA_VER_LEN);
				if(strcmp(pVer, ver) == 0)//�汾��ͬ
				{
					len = filesize-AREA_VER_LEN;
					AbstractFileSystem_Read(fp, pBuf, len);
				}
				AbstractFileSystem_Close(fp);
				AbstractFileSystem_DeleteFile(AREA_TEMPFILE_PATH, AFS_SINGLE_DELETE_FDM);
			}
		}
	}
	else
	{
		fp = AbstractFileSystem_Open(AREA_TEMPFILE_PATH, AFS_READ);
		if(fp >= 0)
		{
			AbstractFileSystem_Close(fp);
			AbstractFileSystem_DeleteFile(AREA_TEMPFILE_PATH, AFS_SINGLE_DELETE_FDM);
		}

		fp = AbstractFileSystem_Create(AREA_TEMPFILE_PATH);
		if(fp >= 0)
		{
			AbstractFileSystem_Write(fp, pVer, AREA_VER_LEN);
			AbstractFileSystem_Write(fp, pBuf, size);
			AbstractFileSystem_Close(fp);
		}
		
		len = size+AREA_VER_LEN;
	}
		
	return len;
}


int DownLoad_DelAndSave(U8 *pBuf, int size)
{
	int fp, len;
	U16 crc;
	
	//��csv�ļ�����һ��,��ʵû��Ҫ  ֻ��Ϊ�˺����鿴����
	fp = AbstractFileSystem_Open(AREA_CSVFILE_PATH, AFS_READ);
	if(fp >= 0)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(AREA_CSVFILE_PATH, AFS_SINGLE_DELETE_FDM);
	}

	fp = AbstractFileSystem_Create(AREA_CSVFILE_PATH);
	if(fp >= 0)
	{
		AbstractFileSystem_Write(fp, pBuf, size);
		AbstractFileSystem_Close(fp);
	}
	
	//�����ļ�,�����������ļ�
	DownLoad_Area_Init((U8 *)hsaSdram_UpgradeBuf());
	DownLoad_Area_DataProcess(pBuf, size);
	fp = AbstractFileSystem_Open(AREA_FILE_PATH, AFS_READ);
	if(fp >= 0)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(AREA_FILE_PATH, AFS_SINGLE_DELETE_FDM);
	}

	fp = AbstractFileSystem_Create(AREA_FILE_PATH);
	if(fp >= 0)
	{
		len = pgu8Strcur - gu8Allspace ;
		hyUsbPrintf("len = %d   0x%x  0x%x\r\n", len, (U32)pgu8Strcur, (U32)gu8Allspace);
		len = len > SITE_MEM_MAX ? SITE_MEM_MAX : len ;
		
		crc = glbVariable_CalculateCRC((U8 *)gu8Allspace,len);
		
		AbstractFileSystem_Write(fp,gu8AreaVerTemp,AREA_VER_LEN);
		AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
		AbstractFileSystem_Write(fp,gu8Allspace,len);
		AbstractFileSystem_Close(fp);
		
		memset(gu8AreaVer,0,AREA_VER_LEN);
		strcpy(gu8AreaVer,gu8AreaVerTemp);
	}
	
	return 0;
}
int Gznd_DownLoad_DelAndSave(U8 *pBuf, int size,U8 *ver)
{
	int fp, len;
	U16 crc;
	
	//��csv�ļ�����һ��,��ʵû��Ҫ  ֻ��Ϊ�˺����鿴����
	fp = AbstractFileSystem_Open(AREA_CSVFILE_PATH, AFS_READ);
	if(fp >= 0)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(AREA_CSVFILE_PATH, AFS_SINGLE_DELETE_FDM);
	}

	fp = AbstractFileSystem_Create(AREA_CSVFILE_PATH);
	if(fp >= 0)
	{
		AbstractFileSystem_Write(fp, pBuf, size);
		AbstractFileSystem_Close(fp);
	}
	
	//�����ļ�,�����������ļ�
	DownLoad_Area_Init((U8 *)hsaSdram_UpgradeBuf());
	DownLoad_Area_DataProcess(pBuf, size);
	fp = AbstractFileSystem_Open(AREA_FILE_PATH, AFS_READ);
	if(fp >= 0)
	{
		AbstractFileSystem_Close(fp);
		AbstractFileSystem_DeleteFile(AREA_FILE_PATH, AFS_SINGLE_DELETE_FDM);
	}

	fp = AbstractFileSystem_Create(AREA_FILE_PATH);
	if(fp >= 0)
	{
		len = pgu8Strcur - gu8Allspace ;
		hyUsbPrintf("len = %d   0x%x  0x%x\r\n", len, (U32)pgu8Strcur, (U32)gu8Allspace);
		len = len > SITE_MEM_MAX ? SITE_MEM_MAX : len ;
		
		crc = glbVariable_CalculateCRC((U8 *)gu8Allspace,len);
		
		AbstractFileSystem_Write(fp,ver,AREA_VER_LEN);
		AbstractFileSystem_Write(fp,(char *)&search_total,sizeof(U16));
		AbstractFileSystem_Write(fp,(char *)&crc,sizeof(U16));
		AbstractFileSystem_Write(fp,gu8Allspace,len);
		AbstractFileSystem_Close(fp);
		
		memset(gu8AreaVer,0,AREA_VER_LEN);
		strcpy(gu8AreaVer,ver);
	}
	
	return 0;
}
//1:�ɹ�   3:��������ʧ��   11:�������°汾  12:�������������ݸ�ʽ����    >20:���������صĴ���
int DownLoad_DownArea(void *p)
{
	struct protocol_http_request tRequest;
	tCARTOONPARA	*pUp;
	int		filesize, bufLen, recvLen, start;
	U8		*pRecv, *pUnComp;
	
	pUp = (tCARTOONPARA *)p;
	pRecv = pUp->pdata;
	bufLen= pUp->datalen;
	
	filesize = http_getsize(gu8Url);
hyUsbPrintf("filesize = %d  bufLen = %d \r\n", filesize,bufLen);
	if(filesize <= 0)
	{
		OpCom_SetLastErrCode(ERR_CONNECTFAIL);//��������ʧ��
		return 3;
	}
	else if(filesize >= 0x300000)
	{
		OpCom_SetLastErrCode(ERR_DATAERROR);//���ݴ���
		return 12;
	}
	
	//��ʼ����
	//�ȿ����Ƿ����ϴ�����ʧ�ܱ������ʱ�ļ�
	start = DownLoad_TempFile(pRecv, 0, gu8AreaVerTemp);
	
	Com_SetPercentTotal(filesize);
	Com_SetPercentRecv(start);
	recvLen = http_downfile(gu8Url, start, filesize, pRecv, bufLen);
hyUsbPrintf("recvLen ===== %d  filesize = %d \r\n",recvLen,filesize);
	if(recvLen <= 0)//û�����ص�����
	{
		OpCom_SetLastErrCode(ERR_UPGRADEFAIL);
		return 3;
	}
	else if(recvLen < filesize)//û��������
	{
		//���������ص��ļ�
		DownLoad_TempFile(pRecv, recvLen, gu8AreaVerTemp);
		
		OpCom_SetLastErrCode(ERR_UPGRADEFAIL);
		return 3;
	}
	
	//�������, ��ѹ
	pUnComp = (U8 *)hsaSdramReusableMemoryPool();
	bufLen  = 0x400000;
	if(0 <= Gzip_Uncompress(pRecv,recvLen,pUnComp,&bufLen,1))
	{
		if(0 != DownLoad_DelAndSave(pUnComp, bufLen))
		{
			OpCom_SetLastErrCode(ERR_DATAERROR);//���ݴ���
			return 12;
		}
	}
	else
	{
		OpCom_SetLastErrCode(ERR_DATAERROR);//���ݴ���
		return 12;
	}
	
	OpCom_SetLastErrCode(ERR_SUCCESS);
	return 1;
}


//1:�ɹ�   3:��������ʧ��   11:�������°汾  12:�������������ݸ�ʽ����    >20:���������صĴ���
int DownLoad_GetAreaInfo(void *p)
{
	struct protocol_http_request tRequest;
	tCARTOONPARA	*pUp;
	int		ret = 1, bufLen, recvLen, dataLen;
	U8		*pSend, *pS, *pE, *pGbk;
		
	if(Net_ConnectNetWork() != 1)
	{
	hyUsbPrintf("sendRequest ����ʧ��\r\n");
		OpCom_SetLastErrCode(ERR_CONNECTFAIL);//��������ʧ��
		return 3;
	}
	
	pUp = (tCARTOONPARA *)p;
	pSend = pUp->pdata;
	bufLen= pUp->datalen;
	
	//��֯��������
	strcpy(pSend, "<req op=\"getsiteinfo\"><h><ver>1.0</ver><company>529951</company><user>1007</user><pass>202cb962ac59075b964b07152d234b70</pass><dev1>180000666</dev1><dev2>00000000000</dev2></h><data><o><area_ver>2012-11-01</area_ver><site_ver>2012-11-01</site_ver></o></data></req>");
	
	memset(&tRequest, 0, sizeof(struct protocol_http_request));
	tRequest.method         = HTTP_POST_METHOD;
    tRequest.user_agent     = "hyco";
    tRequest.content_length = strlen(pSend);
    tRequest.content_data   = pSend;
    tRequest.referer        = NULL;
    tRequest.content_type   = "text/xml; charset=utf-8";
    tRequest.range          = NULL;
    tRequest.connection		= 0;
	tRequest.gzip			= 0;
	
	recvLen = http_send_request("http://opws.yundasys.com:9900/ws/ws.jsp", bufLen, &tRequest);
/*<dta st="ok" res="0" op="getsiteinfo">
<h><ver>1.0</ver><time>2012-11-01 19:38:28</time></h>
<data>
<o><d>2011-12-13</d><d>http://pbzz.yundasys.com/area_20111213.zip</d></o>
<o><d>2011-12-13</d><d>http://pbzz.yundasys.com/site_20111213.zip</d></o>
</data>
</dta>*/	
	if(recvLen > 0)
	{
		pGbk = (U8 *)hsaSdram_DecodeBuf();
		recvLen = Net_DecodeData(pSend,recvLen,pGbk);
		if(recvLen > 0)
		{
			memcpy(pSend,pGbk,recvLen);
			pSend[recvLen] = 0;
			pSend[recvLen+1] = 0;

			pS = strstr(pSend,"st=\"ok\"");
			if(pS == NULL)
			{//ʧ��
				OpCom_SetLastErrCode(ERR_DATAERROR);//���ݴ���
				return 12;
			}
			
			pS = strstr(pSend,"<o><d>");
			if(pS == NULL)
			{
				OpCom_SetLastErrCode(ERR_DATAERROR);//���ݴ���
				return 12;
			}
			pSend = pS+strlen("<o><d>");
			pS = strstr(pSend,"<o><d>");
			if(pS == NULL)
			{
				OpCom_SetLastErrCode(ERR_DATAERROR);//���ݴ���
				return 12;
			}
			pS += strlen("<o><d>");
			pE = strstr(pS,"</d><d>");
			dataLen = pE - pS;
			if(dataLen >= AREA_VER_LEN)
			{
				OpCom_SetLastErrCode(ERR_DATAERROR);//���ݴ���
				return 12;
			}
			memcpy(gu8AreaVerTemp, pS, dataLen);
			gu8AreaVerTemp[dataLen] = 0;
			
			pS = pE+strlen("</d><d>");
			pE = strstr(pS,"</d></o>");
			dataLen = pE - pS;
			if(dataLen >= 256)
			{
				OpCom_SetLastErrCode(ERR_DATAERROR);//���ݴ���
				return 12;
			}
			memcpy(gu8Url, pS, dataLen);
			gu8Url[dataLen] = 0;
			
			if(strcmp(gu8AreaVerTemp, gu8AreaVer) <= 0)
			{
				OpCom_SetLastErrCode(ERR_NEWEST);//���ݴ���
				return 11;
			}
			
			OpCom_SetLastErrCode(ERR_SUCCESS);
			return 1;
		}
		else
		{
			OpCom_SetLastErrCode(ERR_SENDFAIL);
			return 2;
		}
	}
	else
	{
		OpCom_SetLastErrCode(ERR_SENDFAIL);
		return 2;
	}
	
	return ret;
}

/*--------------------------------------------------
* ����:DownLoad_Area_Action
* ����:������������Ľ���ִ�к���
* ����: flag:0:��ȡ��Ϣ    1:����
* ����: 0:�ɹ�   ����ʧ��,����ʧ��ԭ����Ե���OpCom_GetLastErrCode�õ�
----------------------------------------------------*/
int DownLoad_Area_Action(tGRAPDESKBOX *pDesk, U8 flag)
{
	int				ret = -1;
	U8				*pSend;
	eDLG_STAT		kDlgState;
	tCARTOONPARA	tUp;
	int				err,len,lastValue;
	
	lastValue = BackLight_CloseScreenProtect();//�ر�����
		
	pSend = (U8 *)hsaSdram_UpgradeBuf();
	tUp.pdata   = pSend;
	tUp.datalen = hsaSdram_UpgradeSize();
	
	if(flag == 0)
	{
		//��ȡ�汾��Ϣ
		cartoon_app_entry(DownLoad_GetAreaInfo,&tUp);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		kDlgState = Com_CtnDlgDeskbox(pDesk,2,"���ڻ�ȡ��Ϣ,���Ժ�...",NULL,NULL,ComWidget_LabTimerISR,50,DLG_ALLWAYS_SHOW);
		AppCtrl_DeletePro(KERNAL_APP_AN);
		
		if(DLG_CONFIRM_EN == kDlgState)
		{
			err = kernal_app_get_stat();
			if(err == 1)
			{
				ret = 0;
			}
		}
		else
		{
			OpCom_SetLastErrCode(ERR_CONNECTTIMEOUT);
		}
	}
	else if(flag == 1)
	{
		Com_SetPercentTotal(0);
		Com_SetPercentRecv(0);
		cartoon_app_entry(DownLoad_DownArea,&tUp);
		AppCtrl_CreatPro(KERNAL_APP_AN, 0);
		kDlgState = Com_CtnDlgDeskbox(pDesk,2,"���ڸ�������,���Ժ�...",NULL,NULL,Com_PercentLabTimerISR,50,DLG_ALLWAYS_SHOW);
		AppCtrl_DeletePro(KERNAL_APP_AN);
					
		if(DLG_CONFIRM_EN == kDlgState)
		{
			err = kernal_app_get_stat();
			if(err == 1)//�ɹ�
			{
				ret = 0;
			}
		}
		else
		{
			OpCom_SetLastErrCode(ERR_CONNECTTIMEOUT);
		}
	}
	
	Net_DelEvt(NET_FOREGROUND_EVT);
	BackLight_SetScreenProtect(lastValue);//������
	
	return ret;
}

/*---------------------------------------------------------
*����: Op_DownLoad_Area_Desk
*����: 
*����: 
*����: ��
*---------------------------------------------------------*/
int Op_DownLoad_Area_Desk(void *pDesk, U8 ifSearch)
{
	U64 		totalspace,freespace = 0;
	int			ret;
	
	if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�Ƿ������������?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
	{
		AbstractFileSystem_VolumeSpace( "C", &totalspace, &freespace);
		freespace >>= 20;
		if(freespace < 6)//M
		{
			Com_SpcDlgDeskbox("���̿ռ䲻��,��6M.",6,pDesk,2,NULL,NULL,DLG_ALLWAYS_SHOW);
			return -1;
		}
		
		//���������¼�
		if(-1 == Net_ChangeEvt(pDesk, NET_DOWNDATA_EVT))
		{
			return -1;
		}
		
		ret = DownLoad_Area_Action(pDesk, 0);
		if(0 == ret || OpCom_GetLastErrCode() == ERR_NEWEST)
		{
			if(OpCom_GetLastErrCode() == ERR_NEWEST)//�������°汾
			{
				if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�������°汾,�Ƿ����?",0,pDesk,3,NULL,NULL,DLG_ALLWAYS_SHOW))
				{
					ret = 0;
				}
				else
				{
					ret = 11;
				}
			}
			if(ret == 0)
			{
				ret = DownLoad_Area_Action(pDesk, 1);
				if(ret == 0)
				{
					Com_SpcDlgDeskbox("�������!",0,pDesk,1,NULL,NULL,100);
				}
				else
				{
					OpCom_ErrorRemind(pDesk);
				}
			}
		}
		else
		{
			OpCom_ErrorRemind(pDesk);
		}
	}
	
	return 0;
}

/*++++++++++++++++++++++++++++++++++++++++++++��ʾ��ϸ��Ϣ++++++++++++++++++++++++++++++++++++++++++++*/
U16 gu16KeyColor;
U8  *gpu8KeyStr;
const tGrapViewAttr AreaInfoViewAttr[]=
{
	{COMM_DESKBOX_ID,   0,  0,240,320,     0,  0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0,0,  1,  0,  0,  0},//����ͼƬ1 
	
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//����	
	
	{EDIT_ID_1,   5, 40,232, 240,      5,  40,  237,  280,   0,	0,	0,0,1,0,0,0},//ÿ��29���ַ�
};

const tGrapButtonPriv  AreaInfoBtnAttr[]=
{
	{(U8 *)"����",0xffff,0xffff,0,0,2,KEYEVENT_ESC,TY_UNITY_BG1_EN, NULL},//����
};

const tGrapEdit2Priv  AreaInfoEdit2Attr[]=
{
 	{NULL,	0xffff,0,0,0,TY_UNITY_BG1_EN,0,0,10240,1,1,1,0},//��ʾ
};

int AreaInfo_BtnPress(void *pView, U16 state)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON 	*pBtn;
	
	if(state == STATE_NORMAL)
	{
		pBtn  = (tGRAPBUTTON *)pView;
		pDesk = (tGRAPDESKBOX *)pBtn->pParent;
		
		switch(pBtn->view_attr.id)
		{
		case BUTTON_ID_1://�˳�
			return RETURN_QUIT;
			break;
		default:
			break;
		}
	}
	
	return RETURN_CANCLE;
}

S32 AreaInfo_FindNode(tCHARPOS *pHead, U16 LineNo, U32 *firstCharPos)
{
	tCHARPOS *pSub;
	
	*firstCharPos = 0;
	pSub = pHead;
	while(pSub != NULL)
	{
		if(pSub->LineNo == LineNo)
		{
			*firstCharPos = pSub->firstCharPos;
			break;
		}
		pSub = pSub->pNext;
	}
	
	return 0;
}

//��ָ����
S32 AreaInfo_Edit2DrawOneLine(void *pView, U32 line_idx, U16 x_start, U16 y_start)
{
	tGRAPEDIT2 *pEdit;
	tGRAPEDIT2CTRL *pedit_ctrl;
	tGrapEdit2Priv *pedit_privattr;
	tGrapViewAttr  *pview_attr;
	U32 totalCharLen, pos, tempPos;      //Ҫ��ʾ�����ַ����ַ����е�λ��
	tStrDispInfo disp_info;
	Character_Type_e kLycStr_Type;
	U32 curDrawLen;
	U8  *pTempStr;
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	
	AreaInfo_FindNode(pedit_ctrl->pLineInfoH, line_idx, &pos);
	
	if(line_idx == pedit_ctrl->totLine-1)//���һ��
	{
		kLycStr_Type = pedit_privattr->font_type;//�ַ�����
		totalCharLen = Get_StringLength(kLycStr_Type,(pedit_privattr->digt+pos));//�ַ�����(UNICODE��ǵ�Ҳ��2��)
		curDrawLen = totalCharLen;
	}
	else
	{
		AreaInfo_FindNode(pedit_ctrl->pLineInfoH, line_idx+1, &tempPos);
		curDrawLen = tempPos - pos;
	}
	pTempStr = (U8 *)malloc(curDrawLen+2);
	if(NULL == pTempStr) return -1;
	
	memcpy(pTempStr,(U8*)(&pedit_privattr->digt[pos]),curDrawLen);
	pTempStr[curDrawLen] = 0;
	pTempStr[curDrawLen+1] = 0;
	disp_info.string = pTempStr;
	disp_info.color = pedit_privattr->color;
	disp_info.font_size = pedit_privattr->font_size;
	disp_info.font_type = pedit_privattr->font_type;
	disp_info.flag = pedit_privattr->position;
	disp_info.keyStr = gpu8KeyStr;
	disp_info.keyStrColor = gu16KeyColor;

	Grap_WriteString(x_start+pedit_privattr->xoffset, y_start+pedit_privattr->yoffset, 
				(U16)(x_start+pview_attr->viewWidth),
				(U16)(y_start + pview_attr->viewHeight), &disp_info);
	free((void *)pTempStr);
	return SUCCESS;
}


S32 AreaInfo_Edit2Draw(void *pView, U32 xy, U32 wh)
{
	tGRAPEDIT2 *pEdit;
	tGrapViewAttr  *pview_attr;
	tGrapEdit2Priv *pedit_privattr;
	tGRAPEDIT2CTRL *pedit_ctrl;
	U32 i = 0;
	U32 line;  //һ����ʾ������
	
	pEdit = (tGRAPEDIT2 *)pView;
	pview_attr = &(pEdit->view_attr);
	pedit_privattr = &(pEdit->edit_privattr);
	pedit_ctrl = &(pEdit->edit_ctrl);
	
	//clear edit screen
	if (pview_attr->clNormal)
	{
		Grap_ShowPicture(pview_attr->clNormal, pview_attr->viewX1, pview_attr->viewY1);
	}
	else if(pedit_privattr->coverPicId)
	{
		Grap_ClearScreen(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
			(U16)(pview_attr->viewY1 + pview_attr->viewHeight));
	}
	else
	{
		Grap_BrushScreen(pview_attr->viewX1, pview_attr->viewY1, (U16)(pview_attr->viewX1 + pview_attr->viewWidth),
				(U16)(pview_attr->viewY1 + pview_attr->viewHeight), pview_attr->color);
	}

	//draw lines  from top line
	for ( i = pedit_ctrl->topLine_idx; i < pedit_ctrl->topLine_idx + pedit_ctrl->line_screen; i++ )
	{
		if ( i < pedit_ctrl->totLine )
		{
			AreaInfo_Edit2DrawOneLine(pView, i, pview_attr->viewX1, 
					pview_attr->viewY1 + (i-pedit_ctrl->topLine_idx)*pedit_ctrl->line_height);
		}
	}
	
	return SUCCESS;
}

//��ʾ��ѯ����ϸ��Ϣ
int AreaInfo_ShowDesk(void *pDeskFather, tCSV *dataStr, U8 *SearchStr)
{
	tGRAPDESKBOX	*pDesk;
	tGRAPBUTTON		*pBtn;
	tGRAPEDIT2		*pEdit2;
	int	i,idx=0,len;
	U8				*pStrs;
	U8              *strs[13] = {"����˾���롿","����˾���ơ�","����˾���","���ϼ���λ���ơ�",
								"��ʡ�ݡ�","�������ˡ�","���ֻ���","���绰��","�����桿",
								"---------����������--------","--------������������-------","��ʱЧ��","����ע��"}; 
								
								
	
	gpu8KeyStr  = SearchStr;
	gu16KeyColor= COLOR_YELLOW;
	gu8StrsShow = (U8 *)hsaSdram_FrontBuf3() ;	
	memset(gu8StrsShow,0,hsaSdram_Front3Size());
							
	pDesk = Grap_CreateDeskbox(&AreaInfoViewAttr[idx++], NULL);	
	if(pDesk == NULL) return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	
	pBtn = Grap_InsertButton(pDesk,&AreaInfoViewAttr[idx++],&AreaInfoBtnAttr[0]);
	if(pBtn == NULL) return ;
	pBtn->pressEnter = AreaInfo_BtnPress;
	
	pEdit2 = Grap_InsertEdit2(pDesk, &AreaInfoViewAttr[idx++], &AreaInfoEdit2Attr[0], 0);
	if(pEdit2 == NULL) return ;
	pEdit2->draw = AreaInfo_Edit2Draw;
	pEdit2->edit_privattr.digt = gu8StrsShow;
	for(i = 0 ; i < sizeof(strs)/sizeof(U8*) ; i++)
	{
		pStrs = pgu8Strs + dataStr->offset[i];
		strcat(pEdit2->edit_privattr.digt,strs[i]);
		len = strlen(strs[i]);
		if(0 != *pStrs)
		{
			strcat(pEdit2->edit_privattr.digt,pStrs);
			len += strlen(pStrs);
		}
		if(len % 29)
		{
			strcat(pEdit2->edit_privattr.digt,"\r\n");
		}
	}
	pEdit2->get_default_str(pEdit2);
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);	
	Grap_DeskboxRun(pDesk);	
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*++++++++++++++++++++++++++++++++++++++++++++�Ѳ�ѯ����վ�㰴ʡ�ݷ���++++++++++++++++++++++++++++++++++++++++++++++++*/

const tGrapViewAttr Province_ViewAttr[] =
{
	{COMM_DESKBOX_ID,	  0, 0,240,320,    0, 0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0, 0, 1,0,0, 0},
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//����
	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
	
};

const tGrapListerPriv Province_ListPriv[]=
{
	{24,9,0xffff,0x00,  0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,30,4,20,0,50,50,1,0,0,1,1,1},
};

const tGrapButtonPriv Province_BtnPriv[]=
{
	{(U8 *)"����",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
};

S32 Province_ButtonEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn ;
	GRAP_EVENT	 	event;
	
	pBtn  = (tGRAPBUTTON *)pView;
	
	if (state != STATE_NORMAL) return SUCCESS ;
	
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://ESC
		return RETURN_QUIT;
		break;
	default:
		break;
	}
	
	return RETURN_REDRAW;
}


S32 Province_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	tGRAPDESKBOX 	*pDesk;
	U16 len1,len2,j=0 ;
	U8  *pcomCode ,*pcomName,*ptem;
	
	pcomCode = pgu8Strs + ptAllinfo[province_str[i]].offset[0] ;
	pcomName = pgu8Strs + ptAllinfo[province_str[i]].offset[1] ;
	ptem     = str ;
	
	len1 = strlen(pcomCode);
	len2 = strlen(pcomName);
	
	if(len1+len2 < maxLen-2)
	{
		memcpy(ptem,pcomCode,len1);
		ptem[len1] = '|';
		ptem += len1+1 ;
		memcpy(ptem,pcomName,len2);
		ptem[len2] = 0;
		ptem[len2+1] = 0;
	}
	else
	{
		if(len1 < maxLen-2)
		{
			memcpy(ptem,pcomCode,len1);
			ptem[len1] = '|';
			ptem += len1+1 ;
			memcpy(ptem,pcomName,maxLen-len1-1);
			ptem[maxLen-1] = 0;
			ptem[maxLen-2] = 0;
		}
		else
		{
			memcpy(ptem,pcomCode,maxLen);
			str[maxLen-1] = 0; 
			str[maxLen-2] = 0; 
		}
	}	
		
	return SUCCESS;
}

S32 Province_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX 	*pDesk ;
	pDesk = (tGRAPDESKBOX *)pLister->pParent ;
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	AreaInfo_ShowDesk(pDesk,&ptAllinfo[province_str[i]],Keyword);
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	return RETURN_REDRAW ;
}

/*---------------------------------------------------------
*����: ProvinceList_Desk
*����: ��������鰴ʡ����
*����: 
*����: ��
*---------------------------------------------------------*/
int ProvinceList_Desk(void *pDeskFather,U8 type)
{
	tGRAPDESKBOX    *pDesk;
	tGRAPBUTTON     *pBttn;
	tGRAPLIST       *pList;
	U8              index, *pBuf;
	
	index = 0;
	pDesk = Grap_CreateDeskbox(&Province_ViewAttr[index++], Province[ProvinceFlag[type]].Province);
	if (pDesk == NULL)  return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	pBttn = Grap_InsertButton(pDesk, &Province_ViewAttr[index++], &Province_BtnPriv[0]);
	if (pBttn == NULL)  return -1;
	pBttn->pressEnter = Province_ButtonEnter;
	
	pList = Grap_InsertLister(pDesk, &Province_ViewAttr[index++], &Province_ListPriv[0]);
	if (pList == NULL) return -1;
	pList->lst_privattr.totalItem = Province[ProvinceFlag[type]].Count;
	pList->getListString = Province_ListGetStr;
	pList->enter         = Province_ListEnter;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
/*++++++++++++++++++++++++++++++++++++++++++++���ݲ�ͬ��Ϣ��ѯ++++++++++++++++++++++++++++++++++++++++++++*/
const tGrapViewAttr AreaSearch_ViewAttr[] =
{
	{COMM_DESKBOX_ID,	  0, 0,240,320,    0, 0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0, 0, 1,0,0, 0},
#ifdef W818C
	{BUTTON_ID_1,	   5,282,73,34,     5,282,78,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//ȷ��
	{BUTTON_ID_2,	   161,282,73,34,  161,282,234,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
#else	
	{BUTTON_ID_2,	   5,282,73,34,     5,282,78,316,     YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//����
	{BUTTON_ID_1,	   161,282,73,34,  161,282,234,316,   YD_AN1_EN,YD_AN2_EN,0,0,1,0,0,0},//ȷ��
#endif	
	{BUTTON_ID_10,	   5, 35,72,16,    0,0,0,0,   0,0,0,0,1,0,0,0},//
	{EDIT_ID_1,	   	   10, 60,215,22,    10,60,225,82, 0,0,0xffff,0,1,1,1,0},
	
	{LISTER_ID_1,	   10,90,224,190,	10,90,234,280,	0,0,0,0,1,0,1,0},
};

const tGrapListerPriv AreaSearch_ListPriv[]=
{
	{24,7,0xffff,0x00,	0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,10,4,26,0,50,50,1,0,0,1,1,0},
};

const tGrapButtonPriv AreaSearch_BtnPriv[]=
{
#ifdef W818C
	{"ȷ��",0xffff,0x0000,0,0,2,KEYEVENT_OK ,0,0},
	{"����",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
#else
	{"����",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
	{"ȷ��",0xffff,0x0000,0,0,2,KEYEVENT_OK ,0,0},
#endif	
	{NULL,0xffff,0x0000,0,0,1,0,0,0},
	
};

const tGrapEditPriv AreaSearch_EditPriv[] = 
{
	{NULL,0x0,0,0,2,4,26,  1, KYEWORDLEN-2,  0,0,0,0,0,1,  50,50,0,  0,1,1,1},//
};


S32 AreaSearch_BtnEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn;
	tGRAPDESKBOX 	*pDesk;
	tGRAPLIST       *pList;
	tGRAPEDIT 	 	*pEdit;
	int  cnt,i;
	if (state != STATE_NORMAL) return SUCCESS ;
	
	pBtn  = (tGRAPBUTTON *)pView;
	pDesk = (tGRAPDESKBOX *)pBtn->pParent;
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://ȷ��
		pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk, EDIT_ID_1);
		pList = (tGRAPLIST *)Grap_GetViewByID(pDesk, LISTER_ID_1);
		
		if(strlen(pEdit->edit_privattr.digt) == 0)
		{
			Com_SpcDlgDeskbox("�������ѯ����!",0,pDesk,1,NULL,NULL,100);
			break;
		}
		
		
		cnt = DownLoad_Area_Search(pEdit->edit_privattr.digt,pDesk->view_attr.reserve,ptAllinfo,pgu16Searchs);
		
		if(cnt > 0)
		{
			
			pList->view_attr.reserve = cnt;
			cnt= 0;
			memset(ProvinceFlag,0,34);
			for(i=0;i<34;i++)
			{
				if(Province[i].Count>0)
				{
					ProvinceFlag[cnt++] = i;
				}
			}
			memset(Keyword,0,KYEWORDLEN);
			strcpy(Keyword,pEdit->edit_privattr.digt);
			Grap_ChangeFocus(pEdit,1);
		}
		pList->lst_privattr.totalItem = cnt ;
		break;
	case BUTTON_ID_2://����
		return RETURN_QUIT;
		break;
	default:
		break;
	}
	
	return RETURN_REDRAW;
}
#if 0
S32 AreaSearch_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	tGRAPDESKBOX 	*pDesk;
	U16 len1,len2 ;
	U8  *pcomCode ,*pcomName,*ptem;
	
	pcomCode = pgu8Strs + ptAllinfo[pgu16Searchs[i]].offset[0];
	pcomName = pgu8Strs + ptAllinfo[pgu16Searchs[i]].offset[1];
	ptem     = str ;
	
	len1 = strlen(pcomCode);
	len2 = strlen(pcomName);
	
	if(len1+len2 < maxLen-2)
	{
		memcpy(ptem,pcomCode,len1);
		ptem[len1] = '|';
		ptem += len1+1 ;
		memcpy(ptem,pcomName,len2);
		ptem[len2] = 0;
		ptem[len2+1] = 0;
	}
	else
	{
		if(len1 < maxLen-2)
		{
			memcpy(ptem,pcomCode,len1);
			ptem[len1] = '|';
			ptem += len1+1 ;
			memcpy(ptem,pcomName,maxLen-len1-1);
			ptem[maxLen-1] = 0;
			ptem[maxLen-2] = 0;
		}
		else
		{
			memcpy(ptem,pcomCode,maxLen);
			str[maxLen-1] = 0; 
			str[maxLen-2] = 0; 
		}
	}	
	
	return SUCCESS;
}
#else
S32 AreaSearch_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	sprintf(str,"%s | %d",Province[ProvinceFlag[i]].Province,Province[ProvinceFlag[i]].Count) ;
	return SUCCESS;
}
#endif
#if 0
S32 AreaSearch_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX 	*pDesk ;
	tGRAPEDIT 	 	*pEdit;
	
	if(i < pLister->lst_privattr.totalItem)
	{
		pDesk = (tGRAPDESKBOX *)pLister->pParent ;
		pEdit = (tGRAPEDIT *)Grap_GetViewByID(pDesk, EDIT_ID_1);
		ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
		AreaInfo_ShowDesk(pDesk,&ptAllinfo[pgu16Searchs[i]],pEdit->edit_privattr.digt);
		ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	}
	
	return RETURN_REDRAW ;
	
}
#else
S32 AreaSearch_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX 	*pDesk ;
	U16 j,n=0;
	pDesk = (tGRAPDESKBOX *)pLister->pParent ;
	for(j=0;j<pLister->view_attr.reserve;j++)
	{
		U8 *temp = NULL;
		temp = pgu8Strs + ptAllinfo[pgu16Searchs[j]].offset[4] ;
		if(0 != strstr(temp,Province[ProvinceFlag[i]].Province))
		{
			province_str[n++] = pgu16Searchs[j];
			
		}
	}
	
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	ProvinceList_Desk(pDesk,i);
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	
	return RETURN_REDRAW ;
	
}
#endif
/*---------------------------------------------------------
*����: AreaSearch_Desk
*����: ������Ϣ��ѯ
*����: type: ����
*����: 
*---------------------------------------------------------*/
int AreaSearch_Desk(void *pDeskFather, U8 searchid, U8 type)
{
	tGRAPDESKBOX    *pDesk;
	tGRAPBUTTON     *pBttn;
	tGRAPLIST       *pList;
	tGRAPEDIT 		*pEdit;
	U8              index = 0;
	int 			i;
	U8   *topword[5] = {"�����빫˾����","�����빫˾����","�����빫˾�绰","����������͵�","�����벻���͵�"};
	
	pDesk = Grap_CreateDeskbox(&AreaSearch_ViewAttr[index++], "�����ѯ");
	if (pDesk == NULL)  return ;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->inputTypes = INPUT_TYPE_ALL;
	pDesk->inputMode = 1;//123
	pDesk->scanEnable = 1;
	pDesk->editMinId = EDIT_ID_1;
	pDesk->editMaxId = EDIT_ID_1;
	pDesk->view_attr.reserve = searchid;
	
	for(i = 0; i < 3; i++)
	{
		pBttn = Grap_InsertButton(pDesk,&AreaSearch_ViewAttr[index++],&AreaSearch_BtnPriv[i]);
		if (pBttn == NULL)  return ;
		if(i < 2)
		{
			pBttn->pressEnter = AreaSearch_BtnEnter;
		}
		else
		{
			pBttn->btn_privattr.pName = topword[type];
		}
	}
	
	pEdit = Grap_InsertEdit(pDesk, &AreaSearch_ViewAttr[index++], &AreaSearch_EditPriv[0]);
	pEdit->DrawInputCnt = OpCom_EditDrawInputCnt;
	if(type == 1 || type == 3 || type == 4)
	{
		pDesk->inputMode = 4;
		pEdit->edit_privattr.input = 4;
	}
	
	pList = Grap_InsertLister(pDesk,&AreaSearch_ViewAttr[index++],&AreaSearch_ListPriv[0]);
	if (pList == NULL) return;
	pList->getListString = AreaSearch_ListGetStr;
	pList->enter         = AreaSearch_ListEnter;
	pList->lst_privattr.totalItem = 0;
	
	t9_insert(pDesk,NULL);
	edit_creat_panel(pDesk, 6, NULL,NULL);
	edit_auto_change_input_mode(pDesk,0);
	edit_change_input_mode(pDesk,0,(pDesk->inputMode==4));
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}


/*++++++++++++++++++++++++++++++++++++++++++++���������ѯ++++++++++++++++++++++++++++++++++++++++++++++++*/
const U8 * const Hyco_SearchStr[]=
{
	(const U8 *)"���ݹ�˾�����ѯ",(const U8 *)"���ݹ�˾���Ʋ�ѯ",
	(const U8 *)"���ݹ�˾�绰��ѯ",(const U8 *)"���ݿ����͵ز�ѯ",
	(const U8 *)"���ݲ����͵ز�ѯ"
};

const tGrapViewAttr Delivery_ViewAttr[] =
{
	{COMM_DESKBOX_ID,	  0, 0,240,320,    0, 0,  0,  0,    TY_UNITY_BG1_EN,TY_UNITY_BG1_EN, 0, 0, 1,0,0, 0},
	{BUTTON_ID_1,  83,282,73,34,  83,282,156,316,   YD_AN1_EN,	YD_AN2_EN,	0,0,1,0,0,0},//����
	
	{LISTER_ID_1,	10,40,224,236,	10,40,234,276,	0,0,0,0,1,1,1,0},
	
};

const tGrapListerPriv Delivery_ListPriv[]=
{
	{24,9,0xffff,0x00,  0,0,0,	TY_UNITY_BG1_EN,YD_SLIDER2_EN,30,4,20,0,50,50,1,0,0,1,1,1},
};

const tGrapButtonPriv Delivery_BtnPriv[]=
{
	{(U8 *)"����",0xffff,0x0000,0,0,2,KEYEVENT_ESC,0,0},
};

S32 Delivery_ButtonEnter(void *pView, U16 state)
{
	tGRAPBUTTON 	*pBtn ;
	GRAP_EVENT	 	event;
	
	pBtn  = (tGRAPBUTTON *)pView;
	
	if (state != STATE_NORMAL) return SUCCESS ;
	
	switch(pBtn->view_attr.id)
	{
	case BUTTON_ID_1://ESC
		return RETURN_QUIT;
		break;
	default:
		break;
	}
	
	return RETURN_REDRAW;
}


S32 Delivery_ListGetStr(tGRAPLIST *pLister, U16 i, U8 *str, U16 maxLen)
{
	if(i < pLister->lst_privattr.totalItem)
	{
		sprintf(str, "%2d:%s", i+1, Hyco_SearchStr[i]);
	}
	else
	{
		str[0] = 0;
	}

	return SUCCESS;
}

S32 Delivery_ListEnter(tGRAPLIST *pLister, U16 i)
{
	tGRAPDESKBOX 	*pDesk;
	 U8 config[5] = { 0,1,7,9,10};
	
	pDesk = (tGRAPDESKBOX *)(pLister->pParent);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	AreaSearch_Desk(pDesk, config[i], i);
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
			
	return RETURN_REDRAW;
}

/*---------------------------------------------------------
*����: Delivery_InquireArea
*����: ���������ѯ������
*����: 
*����: ��
*---------------------------------------------------------*/
int Delivery_InquireArea(void *pDeskFather)
{
	tGRAPDESKBOX    *pDesk;
	tGRAPBUTTON     *pBttn;
	tGRAPLIST       *pList;
	U8              index, *pBuf;
	
	pBuf = (U8 *)hsaSdram_UpgradeBuf();
	DownLoad_Area_Init(pBuf);
	if(DownLoad_Area_ReadFile() < 0)
	{
		Com_SpcDlgDeskbox("������������Ϣ,���ȵ��\"��������\"���и���!",6,pDeskFather,2,NULL,NULL,DLG_ALLWAYS_SHOW);
		return 0;
	}
	
	index = 0;
	pDesk = Grap_CreateDeskbox(&Delivery_ViewAttr[index++], "�����ѯ");
	if (pDesk == NULL)  return -1;
	Grap_Inherit_Public(pDeskFather,pDesk);
	pDesk->pKeyCodeArr = keyComArray;
	
	pBttn = Grap_InsertButton(pDesk, &Delivery_ViewAttr[index++], &Delivery_BtnPriv[0]);
	if (pBttn == NULL)  return -1;
	pBttn->pressEnter = Delivery_ButtonEnter;
	
	pList = Grap_InsertLister(pDesk, &Delivery_ViewAttr[index++], &Delivery_ListPriv[0]);
	if (pList == NULL) return -1;
	pList->lst_privattr.totalItem = sizeof(Hyco_SearchStr)/sizeof(U8 *);
	pList->getListString = Delivery_ListGetStr;
	pList->enter         = Delivery_ListEnter;
	
	ComWidget_Link2Desk(pDesk,pgtComFirstChd);
	Grap_DeskboxRun(pDesk);
	ComWidget_LeftFromDesk(pDesk,pgtComFirstChd,COM_WIDGET_NUM);
	Grap_DeskboxDestroy(pDesk);
	
	return 0;
}

int DownLoadWholeFile(void *p);
int Op_DownLoad_Area(void *ppDesk, U8 ifSearch)
{
	tGRAPDESKBOX 	*pDesk;
	U8				*pSend;
	eDLG_STAT		kDlgState;
	tCARTOONPARA	tUp;
	int				err,lastValue;
	
	pDesk = (tGRAPDESKBOX *)ppDesk;
	
	if(DLG_CONFIRM_EN != Com_SpcDlgDeskbox("ȷ���������������б�?",0,pDesk,3,NULL,NULL,800))
	{
		return 0;
	}
	
	if(-1 == Net_ChangeEvt(pDesk, NET_DOWNDATA_EVT))
	{
		return 0;
	}
	lastValue = BackLight_CloseScreenProtect();//�ر�����
	
	pSend = (U8 *)hsaSdram_UpgradeBuf();
	*pSend = 8;
	tUp.pdata   = pSend;
	tUp.datalen = hsaSdram_UpgradeSize();
	Com_SetPercentTotal(0);
	Com_SetPercentRecv(0);	
	cartoon_app_entry(DownLoadWholeFile,&tUp);
	AppCtrl_CreatPro(KERNAL_APP_AN, 0);
	kDlgState = Com_CtnDlgDeskbox(pDesk,2,"���ڸ�������,���Ժ�...",NULL,NULL,Com_PercentLabTimerISR,50,DLG_ALLWAYS_SHOW);
	AppCtrl_DeletePro(KERNAL_APP_AN);
	
	Net_DelEvt(NET_FOREGROUND_EVT);
	BackLight_SetScreenProtect(lastValue);//������
	
	gpu16SearchInfo[0] = 0xffff;
	if(DLG_CONFIRM_EN == kDlgState)
	{
		err = kernal_app_get_stat();
		if(err == 1)
		{
			Com_SpcDlgDeskbox("�������!",0,pDesk,1,NULL,NULL,100);
		}
		else if(err == 11)
		{
			Com_SpcDlgDeskbox("�������ش���!",0,pDesk,1,NULL,NULL,100);
		}
		else if(err == 2)
		{
			Com_SpcDlgDeskbox("����ͨѶ����!",6, pDesk, 2,NULL,NULL,100);
		}
		else if(err == 3)
		{
			Com_SpcDlgDeskbox("��������ʧ��,���Ժ�����!",6, pDesk, 2,NULL,NULL,100);
		}
		else
		{
			err -= 20;
			if(err < 1 || err > 6) err = 0;
			Com_SpcDlgDeskbox(Net_ErrorCode[err],6,pDesk,2,NULL,NULL,100);
		}
	}
	else
	{
		Com_SpcDlgDeskbox("����ͨѶ����,���Ժ�����!",6, pDesk, 2,NULL,NULL,100);
	}
	return 0;
}
int file_parse_area(flag,pdata,datalen,getver)
{
	int fp;
	fp = AbstractFileSystem_Create(AREA_CSVFILE_PATH);
	if(fp>=0)
	{
		AbstractFileSystem_Write(fp,(char *)pdata,datalen);
		AbstractFileSystem_Close(fp);
	}
	else
	{
		return HY_ERROR;
	}
	return HY_OK;
}