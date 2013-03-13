/*
  �ṩ��һ������nand driver.
  
  ĿǰרΪ�ϴ�����.
  Ŀǰʹ�õ�nand������,2K*128�ĺ�4K*128,2K*256��. Ϊ�����ϲ����,���ｫ2K*128��2K*256�������װΪ4K*128.
  ��������:
  1.���漰page��block�����ĺ������и�д;
  2.��дʱ,�ϲ�2��blockΪ1��,����page������ʹ��.
  
*/


#include "hyTypes.h"
#include "hyErrors.h"
#include "hyhwChip.h"
#include "hyhwEcc.h"
#include "hyhwNandFlash.h"
#include "hyswNandFile.h"

#define VIR_PAGE_SIZE	2048	//2K

#define MIN_WRITE_UNIT		512

U8 gNandWrite[MIN_WRITE_UNIT];

/*-----------------------------------------------------------------------------
* ����:	hyhwNandFlash_DataRead_Virt
* ����:	��ָ��page��ָ��ƫ�ƶ�ȡָ����С�����ݣ� ���ƫ����Ϊ0�����page��ʼ�ĵط�
*		�����ݣ���ԭ����pageRead�ĺ����Ĺ���һ��
* ����:	data----ָ���������ȡ���ݵ�buffer
*		page----ָ����page��
*		offset--��ָ��page�е�ƫ��
*		size----��Ҫд�����ݵĳ��ȣ�byte��
* ����:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
int hyhwNandFlash_DataRead_Virt(U8 *pdata, U32 page, U32 offset, U32 size)
{
	int ret, readLen;
	
	if(offset != 0)
	{
		readLen = VIR_PAGE_SIZE - offset;//��һ��page���Զ�ȡ������
		readLen = (readLen >= size)? size:readLen;
		
		ret = hyhwNandFlash_DataRead(pdata, page, offset, readLen);
		if (ret == HY_ERROR)
		{
			return HY_ERROR;
		}
		page ++;
		pdata += readLen;
		//��ʣ����Ҫ��ȡ������
		size -= readLen;
	}
	
	while(size > 0)
	{
		readLen = (size <= VIR_PAGE_SIZE) ? size : VIR_PAGE_SIZE;
		
		ret = hyhwNandFlash_DataRead(pdata, page, 0, readLen);
		if (ret == HY_ERROR)
		{
			return HY_ERROR;
		}
		page ++;
		pdata += readLen;
		size -= readLen;
	}
	
	return HY_OK;
}

/*-----------------------------------------------------------------------------
* ����:	hyhwNandFlash_Erase_Virt
* ����:	��ָ����block����
* ����:	block----��Ҫ������block��
* ����:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_Erase_Virt( U32 block )
{

	return hyhwNandFlash_Erase(block);
}


/*-----------------------------------------------------------------------------
* ����:	hyhwNandFlash_PageWrite_Virt
* ����:	��ָ����page��д��ָ����С������,������ҳд
* ����:	data----ָ���������Ҫд������
*		page----ָ����page��
*		offset---page�е�ƫ��
* ����:	HY_OK
*		HY_ERROR
*----------------------------------------------------------------------------*/
U32 hyhwNandFlash_PageWrite_Virt( U8 *pdata, U32 page, U32 offset, U32 size )
{
	int ret, writeLen, left, off;
	int i,Num;
	
	//��Сд��Ԫ 512 + 16�ֽ�ECC

	if(offset != 0)
	{
		memset(gNandWrite,0xFF,MIN_WRITE_UNIT);
		
		writeLen = VIR_PAGE_SIZE - offset;//��һ��page����д�������
		writeLen = (writeLen >= size)? size:writeLen;
		
		off = offset % MIN_WRITE_UNIT;
		offset -= off;
				
		if(writeLen+off <= MIN_WRITE_UNIT)
		{
			memcpy(&gNandWrite[off],pdata,writeLen);
			ret = hyhwNandFlash_PageWrite_slc(gNandWrite, page, offset, MIN_WRITE_UNIT);
			if (ret == HY_ERROR)
			{
				return HY_ERROR;
			}
			page ++;
			pdata += writeLen;
			//��ʣ����Ҫд�������
			size -= writeLen;
		}
		else
		{
			memcpy(&gNandWrite[off],pdata,MIN_WRITE_UNIT-off);
			ret = hyhwNandFlash_PageWrite_slc(gNandWrite, page, offset, MIN_WRITE_UNIT);
			if (ret == HY_ERROR)
			{
				return HY_ERROR;
			}
			pdata   += (MIN_WRITE_UNIT-off);
			size    -= (MIN_WRITE_UNIT-off);
			offset  += MIN_WRITE_UNIT;
			writeLen-= (MIN_WRITE_UNIT-off);
			while(writeLen > 0)
			{
				if(writeLen <= MIN_WRITE_UNIT)
				{
					memset(gNandWrite,0xFF,MIN_WRITE_UNIT);
					memcpy(gNandWrite,pdata,writeLen);
					ret = hyhwNandFlash_PageWrite_slc(gNandWrite, page, offset, MIN_WRITE_UNIT);
					if (ret == HY_ERROR)
					{
						return HY_ERROR;
					}
					pdata += writeLen;
					size  -= writeLen;
					offset+= writeLen;
					break;
				}
				else
				{
					ret = hyhwNandFlash_PageWrite_slc(pdata, page, offset, MIN_WRITE_UNIT);
					if (ret == HY_ERROR)
					{
						return HY_ERROR;
					}
					pdata   += MIN_WRITE_UNIT;
					size    -= MIN_WRITE_UNIT;
					offset  += MIN_WRITE_UNIT;
					writeLen-= MIN_WRITE_UNIT;
				}
			}
			page ++;
		}
	}
	
	Num = size / VIR_PAGE_SIZE;
	left= size % VIR_PAGE_SIZE;
	
	for(i = 0; i < Num; i++)
	{
		ret = hyhwNandFlash_PageWrite_slc(pdata, page, 0, VIR_PAGE_SIZE);
		if (ret == HY_ERROR)
		{
			return HY_ERROR;
		}
		pdata += VIR_PAGE_SIZE;
		page ++;
	}
	
	Num = left / MIN_WRITE_UNIT;
	left= left % MIN_WRITE_UNIT;
	
	if(Num > 0)
	{
		ret = hyhwNandFlash_PageWrite_slc(pdata, page, 0, Num*MIN_WRITE_UNIT);
		if (ret == HY_ERROR)
		{
			return HY_ERROR;
		}
		pdata += Num*MIN_WRITE_UNIT;
		offset = Num*MIN_WRITE_UNIT;
	}
	
	if(left > 0)
	{
		memset(gNandWrite,0xFF,MIN_WRITE_UNIT);
		memcpy(gNandWrite,pdata,left);
		ret = hyhwNandFlash_PageWrite_slc(gNandWrite, page, offset, MIN_WRITE_UNIT);
		if (ret == HY_ERROR)
		{
			return HY_ERROR;
		}
	}
	
	return HY_OK;
}
