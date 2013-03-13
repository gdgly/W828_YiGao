#include "hyTypes.h"
#include "hyErrors.h"
#include "resManage.h"
//#include "base.h"
#include "glbVariable_base.h"

/*-----------------------------------------------------------------
* ����: res_read_head_info
* ����: ��ȡ�ļ�ͷ��Ϣ
* ����: id:ͼƬ���  pRes���ⲿָ�� ����ɹ�������������
* ����: <0 ʧ��  ��= 0  �ɹ�
-------------------------------------------------------------------*/
static int res_read_head_info(U32 id, tRES_PARA *pOutRes)
{
    int rv = -1;
    U32 offset;
    char *pBuf;
    U32 total;
    U32 index;
    
    if (id > 10000) 
    {
        return rv;
    }  
    
    offset = GetResouceBmpOffset() + (id-1) * sizeof(tRES_PARA);
    
    HSA_AccessReservedArea((char *)pOutRes, offset, sizeof(tRES_PARA));
    
    //��ȡ����Ϣ����crcУ��
    if (pOutRes->high > 400 || pOutRes->width > 400)
    {
        return -1;
    }
      
    return 0;
}

/*-----------------------------------------------------------------
* ����: res_read_body_info
* ����: ��ȡ�ļ�ͷ��Ϣ
* ����: id:ͼƬ���  pRes���ⲿָ�� ����ɹ�������������
* ����: <0 ʧ��  ��= 0  �ɹ�
-------------------------------------------------------------------*/
static int res_read_body_info(char *pOutbuf, tRES_PARA *pInRes)
{
    int rv = -1;
    U32 offset;

    offset = GetResouceBmpOffset() + pInRes->offset;
    
    HSA_AccessReservedArea((char *)pOutbuf, offset, pInRes->size);
    
    return 0;
}

/*-----------------------------------------------------------------
* ����: res_load_total_head_info
* ����: ��ȡ��Դͷ�ϵ�������Ϣ��buf��
* ����: id:ͼƬ���  pRes���ⲿָ�� ����ɹ�������������
* ����: <0 ʧ��  ��= 0  �ɹ�
-------------------------------------------------------------------*/
void res_load_total_head_info(void)
{
    /*char *pBuf;
    int len;
    
    pBuf = hsaSdram_GetResHeadBuf();
    len  = hsaSdram_GetReaHeadBufLen();
    
    HSA_AccessReservedArea((char *)pBuf, GetResouceBmpOffset(), len); */
}

/*-----------------------------------------------------------------
* ����: res_uncompress_data
* ����: ��ѹ���ݰ�
* ����: 
* ����: <0 ʧ��  ��= 0  �ɹ�
-------------------------------------------------------------------*/
static int res_uncompress_body_data(char *pOutBuf,int *pOutLen, char *pInBuf, int inLen)
{
    int rv = -1; 
    char *pTembuf;
    
    pTembuf = (unsigned char *)hsaSdram_GetPicDecodeBuf();
    
    rv = zlibuncompress_data(pInBuf, inLen, pTembuf, pOutBuf, pOutLen);
        
    return rv; 
}


/*-----------------------------------------------------------------
* ����: res_uncompress_data
* ����: ��ѹ���ݰ�������������䵽��Ӧ��buf��
* ����: 
* ����: <0 ʧ��  ��= 0  �ɹ�
-------------------------------------------------------------------*/
int res_load_bmp_to_memory(U32 id, int posX, int posY)
{
    int rv;
    tRES_PARA tRes;
    char *pTempBuf;
    char *pPressBuf;
    int outSize;
    
    //return -1;
    
    if(id > 10000)
    {
        return -1;
    }
        
    pTempBuf    = (char *)Get_Display_DisplayBuffer_Offset();
    pPressBuf = (char *)Get_Uncompress_Buffer_Offset();
    
    rv = res_read_head_info(id,&tRes);
    
    if (rv < 0) 
    {
        return rv;
    } 
    
    rv = res_read_body_info(pPressBuf, &tRes);
    
    if (rv < 0)
    {
        return rv;
    }
    
    //����������������
    switch(tRes.type)
    {
    case  RES_TYPE_JPG:
        break;
    case  RES_TYPE_ZLIB:
        outSize = 0;
        rv = res_uncompress_body_data(pTempBuf,&outSize,pPressBuf,tRes.size);
        break; 
    default:
        break;
    }
    
    
    if (rv < 0)
    {
        return rv;
    }
    
    drvLcd_write_to_bgdisp_memory(pTempBuf,&tRes,posX,posY); 
    
    return 0;  
    
}

/*-----------------------------------------------------------------
* ����: res_load_bmp_to_buf
* ����: ��ѹ���ݰ�������������䵽��Ӧ��buf��
* ����: 
* ����: <0 ʧ��  ��= 0  �ɹ�
-------------------------------------------------------------------*/
int res_load_bmp_to_buf(U32 id, U32 *pbuf)
{
    int rv;
    tRES_PARA tRes;
    char *pTempBuf;
    char *pPressBuf;
    int outSize;

    
    if (id > 10000)
    {
        return -1;
    }
        
    pPressBuf = (char *)Get_Uncompress_Buffer_Offset();
    
    rv = res_read_head_info(id,&tRes);
    
    if (rv < 0) 
    {
        return rv;
    } 
    
    rv = res_read_body_info(pPressBuf, &tRes);
    
    if (rv < 0)
    {
        return rv;
    }
    
    //����������������
    switch(tRes.type)
    {
    case  RES_TYPE_JPG:
        break;
    case  RES_TYPE_ZLIB:
        outSize = 0;
        rv = res_uncompress_body_data(pbuf,&outSize,pPressBuf,tRes.size);
        break; 
    default:
        break;
    }
    
    if (rv < 0)
    {
        return rv;
    }
    
    return 0;
}

/*-----------------------------------------------------------------
* ����: res_load_bmp_to_bakanddisp_memory
* ����: load ���������Դ���
* ����: 
* ����: <0 ʧ��  ��= 0  �ɹ�
-------------------------------------------------------------------*/
int res_load_bmp_to_bakanddisp_memory(U32 id, int posX, int posY)
{
    int rv;
    tRES_PARA tRes;
    char *pTempBuf;
    char *pPressBuf;
    int outSize;
    
    //return -1;
    
    if (id > 10000)
    {
        return -1;
    }
        
    pTempBuf    = (char *)Get_Display_DisplayBuffer_Offset();
    pPressBuf = (char *)Get_Uncompress_Buffer_Offset();
    
    rv = res_read_head_info(id,&tRes);
    
    if (rv < 0) 
    {
        return rv;
    } 
    
    rv = res_read_body_info(pPressBuf, &tRes);
    
    if (rv < 0)
    {
        return rv;
    }
    
    //����������������
    switch(tRes.type)
    {
    case  RES_TYPE_JPG:
        break;
    case  RES_TYPE_ZLIB:
        outSize = 0;
        rv = res_uncompress_body_data(pTempBuf,&outSize,pPressBuf,tRes.size);
        break; 
    default:
        break;
    }
    
    
    if (rv < 0)
    {
        return rv;
    }
    
    drvLcd_write_to_bganddisp_memory(pTempBuf,&tRes,posX,posY); 
    
    return 0;  
    
}


/*-----------------------------------------------------------------
* ����: res_load_bmp_info
* ����: ��ȡͼƬ����Ϣ
* ����: 
* ����: <0 ʧ��  ��= 0  �ɹ�
-------------------------------------------------------------------*/
int res_load_bmp_info(U32 id, tRES_PARA *ptRes)
{
    int rv;
    
    if (id > 10000)
    {
        return -1;
    }
        
    rv = res_read_head_info(id,ptRes);
   
    return rv;
}

/*-----------------------------------------------------------------
* ����: res_game_read_head_info
* ����: ��ȡ�ļ�ͷ��Ϣ
* ����: id:ͼƬ���  pRes���ⲿָ�� ����ɹ�������������
* ����: <0 ʧ��  ��= 0  �ɹ�
-------------------------------------------------------------------*/
static int res_game_read_head_info(U32 id, tRES_PARA *pOutRes)
{
    return 0;
}

/*-----------------------------------------------------------------
* ����: res_game_read_body_info
* ����: ��ȡ�ļ�ͷ��Ϣ
* ����: id:ͼƬ���  pRes���ⲿָ�� ����ɹ�������������
* ����: <0 ʧ��  ��= 0  �ɹ�
-------------------------------------------------------------------*/
static int res_game_read_body_info(char *pOutbuf, tRES_PARA *pInRes)
{
    return 0;
}

/*-----------------------------------------------------------------
* ����: res_uncompress_data
* ����: ��ѹ���ݰ�������������䵽��Ӧ��buf��
* ����: 
* ����: <0 ʧ��  ��= 0  �ɹ�
-------------------------------------------------------------------*/
int res_game_load_bmp_to_memory(U32 id, int posX, int posY)
{
    int rv;
    tRES_PARA tRes;
    char *pTempBuf;
    char *pPressBuf;
    int outSize;
    
    id--; 
    
    pTempBuf    = (char *)Get_Display_DisplayBuffer_Offset();
    pPressBuf = (char *)Get_Uncompress_Buffer_Offset();
    
    rv = res_game_read_head_info(id,&tRes);
    
    if (rv < 0) 
    {
        return rv;
    } 
    
    rv = res_game_read_body_info(pPressBuf, &tRes);
    
    if (rv < 0)
    {
        return rv;
    }
    
    //����������������
    switch(tRes.type)
    {
    case  RES_TYPE_JPG:
        break;
    case  RES_TYPE_ZLIB:
        outSize = 0;
        rv = res_uncompress_body_data(pTempBuf,&outSize,pPressBuf,tRes.size);
        break; 
    default:
        break;
    }
    
    
    if (rv < 0)
    {
        return rv;
    }
    
    drvLcd_write_to_bgdisp_memory(pTempBuf,&tRes,posX,posY); 
    
    return 0;
}
/*-----------------------------------------------------------------
* ����: res_load_part_to_memory
* ����: ��ѹ���ݰ������Ѳ���������䵽��Ӧ��buf��
* ����: X Y ����ʵλ�������Ļ��λ�� x,y �����ͼƬ��λ��
* ����: <0 ʧ��  ��= 0  �ɹ�
-------------------------------------------------------------------*/
int res_load_part_to_memory(U32 id, int x,int y,int X, int Y,int posH,int posW)
{
    int rv;
    tRES_PARA tRes;
    char *pTempBuf,*pMemBuf;
    char *pPressBuf;
    char *pUnPressBuf,*pbuf;
    int outSize;
    
    if (id > 10000)
    {
        return -1;
    }
        
    pTempBuf    = (char *)Get_Display_DisplayBuffer_Offset();    
    pUnPressBuf = (char *)Get_Uncompress_Buffer_Offset();
    pPressBuf = pTempBuf;
    rv = res_read_head_info(id,&tRes);    
    if (rv < 0) return rv;    
    rv = res_read_body_info(pPressBuf, &tRes);    
    if (rv < 0)return rv;
	
    //����������������
    switch(tRes.type)
    {
    case  RES_TYPE_JPG:
        break;
    case  RES_TYPE_ZLIB:
        outSize = 0;
        rv = res_uncompress_body_data(pUnPressBuf,&outSize,pPressBuf,tRes.size);
        break; 
    default:
        break;
    }    
    if (rv < 0)
    {
        return rv;
    }
    {
    	int i,h,w,size;
    	h = tRes.high ; //ͼƬ��С
    	w = tRes.width  ;
    	if(posH <= tRes.high && posW <= tRes.width)//��������������ʾȫ��ͼƬ
		{
			tRes.high = posH; //��ʾ�Ĳ��ִ�С
			tRes.width = posW;
		}
		size = tRes.width * c1POINT_OCCUPY_BYTES;
		pMemBuf = pTempBuf;
		for(i=0;i<tRes.high;i++)
		{
			pbuf = (char *)(pUnPressBuf + ((y + i) * w + x ) * c1POINT_OCCUPY_BYTES);
			memcpy(pMemBuf,pbuf,size);
			pMemBuf += size ;
		}
    }
    drvLcd_write_to_bgdisp_memory(pTempBuf,&tRes,X,Y); 
    
    return 0;  
    
}
#if 1
#include "AbstractFileSystem.h"

/*-----------------------------------------------------------------------------
* ����:	BMP_SaveFile
* ����:	
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
int BMP_SaveFile(U16 *pData,U32 Width,U32 Height,U8 *FileName)
{
	U8 buf[32*3+3];//32 points and 3 pads
	U16 *phead;
	U32 i,j,k,num,pad;
	int handle;
	U8 r,g,b;
	
	handle=AbstractFileSystem_Open((char *)FileName, AFS_RDWR);
	if(handle<0)
	{
		handle=AbstractFileSystem_Create((char *)FileName);
	}
	if(handle<0) return -1;
	
	phead=(U16 *)buf;
	memset(phead,0,54);
	
	//write sign
	phead[0]=0x4d42;
	
	//write file length
	i=Width*Height*4+54;
	phead[1]=i&0xffff;
	phead[2]=i>>16;
	
	//write data offset
	phead[5]=54;
	phead[6]=0;
	
	//write struct length
	phead[7]=40;
	phead[8]=0;
	
	//write bmp width
	phead[9]=Width&0xffff;
	phead[10]=Width>>16;
	
	//write bmp height
	phead[11]=Height&0xffff;
	phead[12]=Height>>16;

	//write format
	phead[13]=1;
	phead[14]=24;
	
	//write size of image	
	i=Width*Height*3;
	phead[17]=i&0xffff;
	phead[18]=i>>16;

	//write head
	num=AbstractFileSystem_Write(handle, (char *)phead, 54);
	if(num!=54) 
	{
		AbstractFileSystem_Close(handle);
		return -1;
	}
	
	//caculate line pad number
	j=Width*3;
	pad=((j+3)&~3)-j;
	
	//write image data
	k=0;
	for(j=Height*Width;j>0;j-=Width)
	{
		for(i=j-Width;i<j;i++)
		{
			//color transform
			r=pData[i]>>11;
			g=(pData[i]&0x07e0)>>5;
			b=pData[i]&0x1f;
			buf[k++]=b<<3;
			buf[k++]=g<<2;
			buf[k++]=r<<3;
			
			//write data to file
			if(k>=96)
			{
				num=AbstractFileSystem_Write(handle,(char *)buf,k);
				if(num!=k)
				{
					AbstractFileSystem_Close(handle);
					return -1;
				}
				k=0;
			}		
		}
		
		//write pad
		for(i=0;i<pad;i++) buf[k++]=0;
	}
	
	//write remained data
	if(k>0)
	{
		num=AbstractFileSystem_Write(handle, (char *)buf, k);
		if(num!=k)
		{
			AbstractFileSystem_Close(handle);
			return -1;
		}
		k=0;
	}
	
	AbstractFileSystem_Close(handle);
	
	return 0;
}
#endif