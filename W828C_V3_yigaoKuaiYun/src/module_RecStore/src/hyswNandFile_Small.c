#include "hyTypes.h"
#include "hyswNandFile.h"
#include "reserveManager.h"

/*-----------------------------------------------------------------------------------------------------------------------*/

static  U32	gPagetoSaveBitmap;		/* ��һ�λش�bitmap��page index*/
static  U32	gPagetoSaveFileEntry;/* ��һ�λش�file Entry��page index*/

static  U8  gNeedSaveBitmap = 0; /*�Ƿ���Ҫ����bitmap*/
/*	ȫ�ֱ��� */

/* nand ��bitmap */
static	U32		gnandBitmap[NAND_BITMAP_WORDS];

/*  �ļ���Ϣ��*/ 
tFILE_ENTRY	gtFileEntry[MAX_NAND_FILE];     

/*-----------------------------------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------
*������nandFile_startBlock
*���ܣ�
*������
*���أ������洢��¼��nand��ʼ��block index
*------------------------------------------------------------*/
int nandFile_startBlock()
{
	int blockNum, size;
	
	size = HSA_GetReserveManagerSize();
	
	blockNum = RESEVE_START_BLOCK_R_FILE+((size*NAND_SIZE_UNIT)>>gtnandInfo.blockSizeBit);
	
	return blockNum;
}

/*-----------------------------------------------------------
*������nandFile_readFileAbstract
*���ܣ��õ��ļ��е�ժҪ��Ϣ. ���Դ�ժҪ��ȫ��page������ȡ
*������fileIdx	-- �ļ�������
*	   pagebuf	-- ������ʱbuf�����ժҪ��Ϣ
*	   absStart -- ժҪ��Ϣ��ʼ��ȡλ��,��Ϊÿ��ժҪblock��ֻ��һ������page,���Ը�ֵ��ʵ��blk[]��ժҪblockλ��
*	   absNum   -- ϣ��������ȡ��ժҪ����,����Ҫ�����Ӽ���ժҪblock�ж�ȡ
*���أ��ɹ�����ȡ��page��   ʧ�ܣ�-1
*------------------------------------------------------------*/
int nandFile_readFileAbstract(int fileIdx, char *pagebuf, int absStart, int absNum)
{
	int	page_start, page, abstractBlk;
	int i,j;
	tFILE_ENTRY		*ptFile;
	
	if(fileIdx < 0 || fileIdx >= MAX_NAND_FILE)
	{
		return HY_ERROR;
	}
		
	ptFile	= &gtFileEntry[fileIdx];
	
	for(i = 0,j = 0; i < absNum; i++)
	{
		abstractBlk = ptFile->blk[absStart+i];//���ժҪ��blk
		
		page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
		page		= page_start + ptFile->pageOffset[absStart+i];
		//hyUsbPrintf("read abs blk = %x  page = %d \r\n",abstractBlk,page);
		if(HY_OK != hyhwNandFlash_DataRead_Virt(pagebuf, page , 0, NAND_PAGE_SIZE))
		{
			//hyUsbPrintf("read abs err \r\n");
			return HY_ERROR;
		}
		pagebuf += NAND_PAGE_SIZE;
		j++;
	}

	return j;
}

/*************************************************************************
* ����:	nandFile_getNewestSequecePage
* ˵��: ����ѭ��ʹ�õ�page�����µ�page��
* ����: page_start	--	��ʼ����page ��
*		page_end	--  ��������page ��,�����һ����Чpage
*		pagebuf		--  �����pagebuf����ʱʹ�ã���Ҫ��֤4�ֽڶ��룬�������µ�page��Ϣ
*						ע���С��Ҫ2��page
*		pSavePage	--  ȡ����Ϣ��page ��
* ����: �ҵ����򷵻�HY_OK, ���򷵻�HY_ERROR
*************************************************************************/
U32 nandFile_getNewestSequecePage(U32 page_start, U32 page_end, char *pagebuf, int *pSavePage) 
{
	int j,page, crc;
	U32 rc,*pword;	
	tSEQUECE_PAGE	*ptseq_page, *ptseq_nextpage;
	
	/*	�ȼٶ���ʼ������ */
	rc = HY_OK;
	
	/* ʹ�������buf*/
	ptseq_page		= (tSEQUECE_PAGE *)pagebuf;
	ptseq_nextpage	= (tSEQUECE_PAGE *)(pagebuf + NAND_PAGE_SIZE);

	
	/*	������Щnand��Ҫecc��������ȡ����page_start����ȡ����page_start�Ĵ��ۺ�
		�������ֽڵĴ���������Ϊ����Ҫ�ȴ�24usʱ��,
		���ң�����MLC����ʹ��������Ǽ����ֽڣ����ڴ���У�����⣬�ײ�driver�����һ��page_start */
	page	= page_start;
	
	rc = hyhwNandFlash_DataRead_Virt((S8*)ptseq_page, page_start, 0, NAND_PAGE_SIZE);
	if(HY_ERROR == rc)
	{
		//hyUsbPrintf("seq read err \r\n");
	}
	crc = glbVariable_CalculateCRC((U8*)ptseq_page,NAND_PAGE_DATASIZE);
	
	while(page <= page_end)
	{	
		/*	����ؼ��ִ��ڣ���ʾnandflash bitmap ���� */
		if(	ptseq_page->tag1 == NAND_SEQUECE_PAGE_TAG &&
			ptseq_page->tag2 == NAND_SEQUECE_PAGE_TAG &&
			ptseq_page->crc16== crc )
		{

			/* ӳ����У�����������һ��page�������page�������µ�ӳ���*/
			if( page == page_end )
			{
				/* ���һ��page������bitmap�����*/
				break;
			}
			else
			{
				/* �������һ��page�����Ƿ���һ��page��û��д���ģ����жϵ�ǰ�Ƿ������µ�bitmap*/
				hyhwNandFlash_DataRead_Virt((S8*)ptseq_nextpage,(page+1), 0, NAND_PAGE_SIZE);
				crc = glbVariable_CalculateCRC((U8*)ptseq_nextpage,NAND_PAGE_DATASIZE);
				
				/* �ж� ���������һ��������Ч���ͱȽ�������һ���ǿհ�ҳ������ʾ�ҵ��� */
				if(	ptseq_nextpage->tag1 == NAND_SEQUECE_PAGE_TAG &&
					ptseq_nextpage->tag2 == NAND_SEQUECE_PAGE_TAG &&
					ptseq_nextpage->crc16== crc             	  &&
					(( ptseq_nextpage->sequece < ptseq_page->sequece) || ( ptseq_nextpage->sequece == 0xFFFF && ptseq_page->sequece == 0))
				  )
				{
					break;
				}
				else
				{
					/* ��һ��ҳ�ǿհ�ҳ����� */
					pword = (U32 *) ptseq_nextpage;
					for(j = 0;j<(NAND_PAGE_SIZE>>2); j++)
					{
						if(*pword++ != 0xFFFFFFFF)
						{
							break;
						}
					}
					
					if(j == (NAND_PAGE_SIZE>>2))  /* �ҵ�������page ���� block ����ʹ���˼���page �����Ϊ�������� */
					{
						U32  blk ;
						blk	 = (page >> PAGE_PER_BLK_BIT);						
						
						if(((blk+1)<<PAGE_PER_BLK_BIT) > page_end) break;  /* ������block */
						
						/* ����һ��block �ĵ�һ��page */
						hyhwNandFlash_DataRead_Virt((S8*)ptseq_nextpage,((blk+1)<<PAGE_PER_BLK_BIT), 0, NAND_PAGE_SIZE);
						crc = glbVariable_CalculateCRC((U8*)ptseq_nextpage,NAND_PAGE_DATASIZE);						
						
						if(	ptseq_nextpage->tag1 == NAND_SEQUECE_PAGE_TAG &&
							ptseq_nextpage->tag2 == NAND_SEQUECE_PAGE_TAG &&
							ptseq_nextpage->crc16== crc                   &&
							ptseq_nextpage->sequece == ptseq_page->sequece )
						{
							page = (blk+1)<<PAGE_PER_BLK_BIT ;
							page -= 1 ; /* ��Ϊ������ page++ �� */
						}
						else
						{
							/* crc ����ȷ����Ϊ�ǿ�page ���ҵ���,�п��ܴ���crc���Ե����ǿ�page�����*/
							break;
						}		
					}		
				}
			}
			
		}
		else
		{
			/* ���������Ч������һֱ����ȥ������������飬��ֹ©���õ�page��*/   /*-----���µ�page����һ����Ӧ��Ϊ0xff��----12-24--Ҫ��Ӵ��� ����------------*/
			
			/* �������һ��page�����Ƿ���һ��page��û��д���ģ����жϵ�ǰ�Ƿ������µ�bitmap*/
			rc = hyhwNandFlash_DataRead_Virt((S8*)ptseq_nextpage,(page+1), 0, NAND_PAGE_SIZE);
			if(HY_ERROR == rc)
			{
				//PhoneTrace(0,"read fail !");
			}
			crc = glbVariable_CalculateCRC((U8*)ptseq_nextpage,NAND_PAGE_DATASIZE);
		}
		
		memcpy((S8*)ptseq_page,(S8*)ptseq_nextpage, NAND_PAGE_SIZE);
		page ++;	
	}
	
	/* �洢������Ϣ��page �Żش���������*/
	*pSavePage	= page;
	
	if(page > page_end)
	{
		/* û���ҵ���Ч���ͷ��ʼ��*/
		*pSavePage = page_start;
		rc = HY_ERROR;
	}
	
	return rc;
}


/*************************************************************************
* ����:	nandFile_saveSequecePage
* ˵��: �洢ѭ��ʹ�õ�page�ĵ�nand�ϣ� blk �������ģ��ʺ��������Ѿ�����������
*		blk�� bitmap �� fileentry ��page�Ĵ洢�����ʺϰ���ѭ��page�洢���ļ���
*		abstract blk��ѭ��page
* ����: page_start	--	�洢�ռ����ʼpage ��
*       page_end	--  �洢�ռ������page ��
*		pagebuf		--  �����pagebuf����Ҫ��֤4�ֽڶ���, һ��page ��С
*		pSavePage	--  ����/�������save ��page �ţ�������δ��page ��
*						��֮����page ���������µ���Ϣ
* ����: �ɹ��洢��û�з����¿飬�򷵻�HY_OK == 0
*		���򷵻�HY_ERROR   = -1
*************************************************************************/
U32 nandFile_saveSequecePage(U32 page_start, U32 page_end, char *pagebuf, int *pSavePage) 
{
	U8		isBitmap = 0;//���δ洢���Ƿ�bitmap
	U32		rc,*pBitmap,*pBadBlock;
	int		page;
	int		blk, pageOffset;
	int		i,sblk,eblk;
	int		maxPage,totalPage;
	tSEQUECE_PAGE	*ptseq_page;
	
	if(page_start == (BLOCK_START_BITMAP<<PAGE_PER_BLK_BIT))
	{
		isBitmap = 1;
	}
	
	/*	�ȼٶ����� */
	rc = HY_OK;
	ptseq_page	= (tSEQUECE_PAGE *)pagebuf;
	
	pBitmap   = (U32 *)(ptseq_page->data);
	
	/* ��������Ҫ���λ�� */
	page	= *pSavePage;
	
	/* ���Ҫд��λ�ü�1*/
	page ++;
	maxPage 	= page_end - page_start +1;
	
	if(page > page_end)
	{
		/* ����save page no �Ǽ򵥼�1�ģ�������Ҫ�ж� */
		page -= maxPage;
		
		/* ��page_start ��������ֺ���Ҫ��1 �ˣ� ����� 0xFFFF����1�������Ϊ0�� ������Ҫ�Ľ��*/
		if( page == page_start)
		{
			ptseq_page->sequece +=1;
		}
	}
	
	if(page == page_start)
	{//�Ѻ���blkȫerase
		sblk = (page_start >> PAGE_PER_BLK_BIT)+1;
		eblk = (page_end >> PAGE_PER_BLK_BIT);
		for(i = sblk; i <= eblk; i++)
		{
			hyhwNandFlash_Erase_Virt(i);
		}
	}
	
	/* ��page ���� ʹ����page ������ѭ�� */
	totalPage = 0;
	while( totalPage <= maxPage)
	{
		blk			= (page >> PAGE_PER_BLK_BIT);
		pageOffset	= page - (blk<<PAGE_PER_BLK_BIT);	
		
		/* ע�����������˳���ܱ䣬Ҳ���ܼ�*/
		if((pageOffset == 0 && 
		    hyhwNandFlash_Erase_Virt(blk) == HY_OK &&
		    hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK) 
		    )
		{
			break;
		}
		else if(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK)
		{
			break;
		}
		else	 	
		{		
			/*������*/
			//SET_BLOCK_BAD(blk);
			SET_BLOCK_USED(blk);
			if(isBitmap == 1)
			{
				//�˴���Ҫͬ�����±�����Ҫд�����Ϣ
				pBitmap[blk>>5] &= ~(1<<(blk &0x1F));
			}
			else
			{
				gNeedSaveBitmap = 1;//�´���Ҫ��bitmap
			}
			
			/* ����ȥ�˺ܶ�page */
			page		+= (1<<PAGE_PER_BLK_BIT) - pageOffset;
			totalPage	+= (1<<PAGE_PER_BLK_BIT) - pageOffset;
			
			if(page > page_end)
			{
				/* ����save page no �Ǽ򵥼�1�ģ�������Ҫ�ж� */
				page -= maxPage;//(page_end - page_start + 1);
				
				/* ��page_start ��������ֺ���Ҫ��1 �ˣ� ����� 0xFFFF����1�������Ϊ0�� ������Ҫ�Ľ��*/
				if( page == page_start)
				{
					ptseq_page->sequece +=1;
				}	
			}
		}
	}
	
	if( totalPage > maxPage)
	{
		rc = HY_ERROR;
	}
	else
	{
		*pSavePage = page ;	
	}
	
	return rc;
}

/*************************************************************************
* ����: nandFile_saveFileEntry
* ˵��: ����fileentry
* ����: 
* ����: ��������ش�ɹ����򷵻�HY_OK, ���򷵻�HY_ERROR
 *************************************************************************/
U32 nandFile_saveFileEntry(char *pagebuf)
{
	int		page;
	U32     ret, sblk,eblk;
	tFILE_ENTRY     *pFile;
	tSEQUECE_PAGE	*ptseq_page;
	
	ptseq_page	= (tSEQUECE_PAGE *)pagebuf;
	
	hyhwNandFlash_DataRead_Virt((S8*)ptseq_page, gPagetoSaveFileEntry, 0, NAND_PAGE_SIZE);
	
	pFile = &gtFileEntry[0];
	
	/* ������Ϣ*/
	memcpy( ptseq_page->data, (char *)pFile, MAX_NAND_FILE *sizeof(tFILE_ENTRY));
	ptseq_page->tag1 = NAND_SEQUECE_PAGE_TAG ;
	ptseq_page->tag2 = NAND_SEQUECE_PAGE_TAG ;
	ptseq_page->crc16 = glbVariable_CalculateCRC((U8*)ptseq_page,NAND_PAGE_DATASIZE);
	
	sblk = BLOCK_START_FILE;
	eblk = BLOCK_END_FILE+1;
	
	/* ע������ָ��Ҫ�õ�ַ����������תһ�� */
	page= gPagetoSaveFileEntry;
	ret = nandFile_saveSequecePage( (sblk<<PAGE_PER_BLK_BIT),
								    (eblk<<PAGE_PER_BLK_BIT)-1,
							 	    (char *)(ptseq_page), 
							 	    &page);
	if(HY_ERROR == ret)
	{
		//hyUsbPrintf("nandFile_saveFileEntry  fail  \r\n");
	}
	
	gPagetoSaveFileEntry = page;
	
	return ret;			 	 
}


/*************************************************************************
* ����: nandFile_saveBitmap
* ˵��: ��gnandBitmap��ӳ������ڴ��buffer�� д�뵽nandFlash�� 
*       ����close nandflashʱ����һ��                 							
* ����: none 
* ����: ��������ش�ɹ����򷵻�HY_OK, ���򷵻�HY_ERROR
 *************************************************************************/
U32 nandFile_saveBitmap( char *pagebuf )
{
	int page;
	U32 ret, sblk,eblk;
	tSEQUECE_PAGE	*ptseq_page;
	
	if(gNeedSaveBitmap == 0)
	{
		return HY_OK;
	}
	
	ptseq_page	= (tSEQUECE_PAGE *)pagebuf;

	/* �ش� fileentry���ش� fileEntry 	*/
	/* ���һ��������Է����ܴ�nandfile entry��ʱ�򣬸ı�bitmap*/
	
	hyhwNandFlash_DataRead_Virt((S8*)ptseq_page, gPagetoSaveBitmap, 0, NAND_PAGE_SIZE);
	
	memcpy( ptseq_page->data, (char *)gnandBitmap, NAND_BITMAP_WORDS *sizeof(U32));
	
	ptseq_page->tag1 = NAND_SEQUECE_PAGE_TAG ;
	ptseq_page->tag2 = NAND_SEQUECE_PAGE_TAG ;
	ptseq_page->crc16 = glbVariable_CalculateCRC((U8*)ptseq_page,NAND_PAGE_DATASIZE);
	
	sblk = BLOCK_START_BITMAP;
	eblk = BLOCK_END_BITMAP+1;
	
	/* ע������ָ��Ҫ�õ�ַ����������תһ�� */
	page	= gPagetoSaveBitmap;
	ret  = nandFile_saveSequecePage( (sblk<<PAGE_PER_BLK_BIT),
								     (eblk<<PAGE_PER_BLK_BIT)-1,
						 		     (char *)(ptseq_page), 
						 		     &page);
	if(HY_ERROR == ret)
	{
		//hyUsbPrintf("nandFile_saveBitmap fail  \r\n");
	}				 	
						 	
	gPagetoSaveBitmap = page ;
	
	gNeedSaveBitmap = 0;
	
	return ret;						 	 
}


/*************************************************************************
* ����:	nandFile_format
* ˵��: ��������block��bitmap�� ����Ҫ���ǣ���bitmap����file entry ����blk
*		Ҫ�����ɾ�
* ����: nandReservedSize	--  nand�ϱ�Ӧ�ó���reserve�Ŀռ䣬��λΪM�ֽ�
* ����: ���������ʼ���ɹ����򷵻�HY_OK, ���򷵻�HY_ERROR
*************************************************************************/
U32 nandFile_format(U32 nandReservedSize, char *pagebuf)
{
	S32	page, i, usedBlknum;
	U32 *pword, sblk,eblk;
	tSEQUECE_PAGE	*ptpageBitmap, *ptpageEntry;
	

	gPagetoSaveBitmap    = (BLOCK_START_BITMAP<<PAGE_PER_BLK_BIT);
	gPagetoSaveFileEntry = (BLOCK_START_FILE<<PAGE_PER_BLK_BIT);
	
	/* ��Ϊ����format,��һ��page Ҳû�� ,���԰ѵ�ǰ��page ��Ϊ��һ�� */
	gPagetoSaveBitmap    -= 1;
	gPagetoSaveFileEntry -= 1;
	
	for(i = 0 ; i < BLOCK_NUM_BITMAP; i++)
	{
		hyhwNandFlash_Erase_Virt(BLOCK_START_BITMAP+i);
	}
	
	for(i = 0 ; i < BLOCK_NUM_FILEENTRY; i++)
	{
		hyhwNandFlash_Erase_Virt(BLOCK_START_FILE+i);
	}

	/*	�����������ʼ��ʱ��telechip����ʹ�õ�block����Ϊbitmap �Լ�reserve�ռ��blockΪ"��"�飬
		��Ϊ���ǲ���������Ϊ�洢���ݵ�block�� */
	usedBlknum		= BLOCK_END_FILE;
	
	memset((char *)gnandBitmap,0xff,NAND_BITMAP_WORDS * sizeof(U32));
	
	pword = (U32*)gnandBitmap;
	for(i= 0; i<=usedBlknum; i++)
	{
		/* ��Ǹ�block�ѱ�ʹ�� */
		pword[i>>5] &= ~(1<<(i &0x1F));
	}
	
	//��������в��ֱ���block
	pword = (U32*)gnandBitmap;
	for(i= BLOCK_DATA_END+1; i < (NAND_FLASH_SIZE<<3); i++)
	{
		/* ��Ǹ�block�ѱ�ʹ�� */
		pword[i>>5] &= ~(1<<(i &0x1F));
	}
	
	/*-------------------------------------------entry-----------------------*/
	memset((U8 *)gtFileEntry,0xff, MAX_NAND_FILE *sizeof(tFILE_ENTRY));
	
	ptpageEntry		= (tSEQUECE_PAGE *)pagebuf;
	
	memset((char *)ptpageEntry,0xFF,NAND_PAGE_DATASIZE);	
	ptpageEntry -> sequece	= 0;
	ptpageEntry -> tag1		= NAND_SEQUECE_PAGE_TAG;
	ptpageEntry -> tag2		= NAND_SEQUECE_PAGE_TAG;
	ptpageEntry ->crc16 = glbVariable_CalculateCRC((U8*)ptpageEntry,NAND_PAGE_DATASIZE);
	
	sblk = BLOCK_START_FILE;
	eblk = BLOCK_END_FILE+1;
	page = gPagetoSaveFileEntry;
	nandFile_saveSequecePage( (sblk<<PAGE_PER_BLK_BIT),
							  (eblk<<PAGE_PER_BLK_BIT)-1,
							  (char *)(ptpageEntry), 
							  &page);
	gPagetoSaveFileEntry = page ;	
	/*------------------------------------------bitmap------------------------*/
	/* ʹ�������buf*/
	ptpageBitmap		= (tSEQUECE_PAGE *)pagebuf;
	
	/* �ȼٶ����е�block���Ǻõ�*/
	memset((char *)ptpageBitmap,0xFF,NAND_PAGE_DATASIZE);
	ptpageBitmap -> sequece	= 0;
	ptpageBitmap -> tag1		= NAND_SEQUECE_PAGE_TAG;
	ptpageBitmap -> tag2		= NAND_SEQUECE_PAGE_TAG;

	
	memcpy(ptpageBitmap->data,(char *)gnandBitmap, NAND_BITMAP_WORDS * sizeof(U32));

	ptpageBitmap ->crc16 = glbVariable_CalculateCRC((U8*)ptpageBitmap,NAND_PAGE_DATASIZE);	
	
	/* ����bitmap*/
	page 	= gPagetoSaveBitmap;
	
	sblk = BLOCK_START_BITMAP;
	eblk = BLOCK_END_BITMAP+1;
	nandFile_saveSequecePage( (sblk<<PAGE_PER_BLK_BIT),
							  (eblk<<PAGE_PER_BLK_BIT)-1,
							  (char *)(ptpageBitmap), 
							  &page);
						 	 
	gPagetoSaveBitmap = page;
	
	/*--------------------------------------------------------------------*/
						 	 
	return HY_OK;
}

/*----------------------------------
* ����: nandFile_checkRecBitmap
* ����: ���bitmap����
* ����: 
* ����: 0:����   -1:�쳣
*-----------------------------------*/
int nandFile_checkRecBitmap()
{
	int 		i,j,idx,sblk,eblk;
	int			blk,used,ret=0;
	tFILE_ENTRY	*pFile;

	sblk = BLOCK_DATA_START;
	eblk = BLOCK_DATA_END;
	
	for(i = sblk; i <= eblk; i++)
	{
	//hyUsbPrintf("cur block id = %d \r\n", i);
		if(!IS_BLOCK_FREE(i))//��ռ��
		{
			used = 0;
			
			//����Ƿ�ȷʵ���ļ�ռ��
			for(idx = 0; idx < MAX_NAND_FILE; idx++)
			{
				pFile = &gtFileEntry[idx];
				if(pFile->totalRecords == 0)
				{
					continue;
				}
				for(j = 0; j < MAX_BLOCKS_PER_FILE; j++)
				{
					blk = pFile->blk[j];
					if(blk == i)//�����ļ�ռ��
					{
						used = 1;//���
						break;
					}
				}
			}
			#ifdef NANDFLASH_DEBUG
				hyUsbPrintf("block used = %d \r\n",used);
			#endif
			if(used == 0)//û�б��ļ�ռ��
			{
				SET_BLOCK_FREE(i);
				ret = -1;
			}
			#ifdef NANDFLASH_DEBUG
				hyUsbPrintf("block stat = %d   %d \r\n", i, IS_BLOCK_FREE(i));
			#endif
		}
	}
	
	return ret;
}

/*************************************************************************
* ����:	nandFile_Init
* ˵��: ���һ�����block��bitmap ��bit0��ʾ��Ӧ������block ռ�ã�1��ʾû��ռ��
*		���nandflash bitmap��TAG ���ڣ����ʾ��bitmap�����򣬾ʹ���
*       ����
* ����: pagebuf				--	buffer����С��nandflash��page size������2��page ��С
*		nandReservedSize	--  nand�ϱ�Ӧ�ó���reserve�Ŀռ䣬��λΪM�ֽ�
* ����: ���������ʼ���ɹ����򷵻�HY_OK, ���򷵻�HY_ERROR
*************************************************************************/
U32 nandFile_Init( U32 reserve_ziku)
{
	char *pagebuf ;
	U32 rc;	
	int	page;
	S32	page_start,page_end;
	tSEQUECE_PAGE	*ptseq_page, *ptseq_nextPage;
	
	/*	�ȼٶ���ʼ������ */
	rc = HY_OK;
	
	gNeedSaveBitmap = 1;
	
	pagebuf = (U8 *)gpu8RecBuf;
	
	ptseq_page		= (tSEQUECE_PAGE *)pagebuf;
	ptseq_nextPage	= (tSEQUECE_PAGE *)(pagebuf + NAND_PAGE_SIZE);

	/* ��һ���֣���NandFlash�� �洢nand bitmap��block ����ȡbitmap  ��
	�����֤��ԭ���Ѿ���nand bitmap�ˣ��Ͳ������½���,�������µ�nand bitmap,*/
	
	/* blk ����page index */
	page_start	= (BLOCK_START_BITMAP<< PAGE_PER_BLK_BIT);
	page_end	= ((BLOCK_END_BITMAP +1) << PAGE_PER_BLK_BIT)-1;
	
	if(nandFile_getNewestSequecePage(page_start, page_end, pagebuf, &page) == HY_ERROR)
	{
		/* ӳ�����Ч����Ҫд��nand bitmap�����Ծ�enable write */
		hyhwNandFlash_EnableWrite();
		rc = nandFile_format(reserve_ziku, pagebuf);
		hyhwNandFlash_DisableWrite();
	}
	else
	{
		gPagetoSaveBitmap = page;
		
		/* �ҵ�bitmap��copy */
		memcpy((char *)gnandBitmap, pagebuf, sizeof(U32)*NAND_BITMAP_WORDS);
		
		/* bitmap ���ˣ���file entry ��������*/
		/* �ڶ����֣���NandFlash�� �洢file entry �����ļ���Ϣ,*/
		
		//��fileentry
		page_start	= (BLOCK_START_FILE<< PAGE_PER_BLK_BIT);
		page_end	= ((BLOCK_END_FILE +1) << PAGE_PER_BLK_BIT)-1;
		if(nandFile_getNewestSequecePage(page_start, page_end, pagebuf, &page) == HY_ERROR)
		{
			/* û���ҵ���Ч��file entry��û���ļ������ */
			gPagetoSaveFileEntry = page_start - 1;
			memset((U8 *)gtFileEntry,0xff, MAX_NAND_FILE *sizeof(tFILE_ENTRY));
			nandFile_saveFileEntry(pagebuf);
			//��saveFileEntryʱ���ܻ����bitmap
			nandFile_saveBitmap(pagebuf);
		}
		else
		{
			//PhoneTrace(0,"entry init ok!");
			gPagetoSaveFileEntry = page;
			/* �ҵ�fileEntry��copy */
			memcpy((char *)gtFileEntry, pagebuf, MAX_NAND_FILE *sizeof(tFILE_ENTRY));
		}
	}
	gNeedSaveBitmap = 0;
	
	//���bitmap
	if(-1 == nandFile_checkRecBitmap())
	{
		gNeedSaveBitmap = 1;
		nandFile_saveBitmap(pagebuf);
	}
	
	return rc;
}


/*************************************************************************
* ����: nandFile_find1FreeBlock
* ˵��:	��bitmap�в��ҳ���һ���յ�block��ʹ��. ���ز��ҵ���block.
* ����:	none
* ����:	�ɹ���blk num  ʧ��  -1
*************************************************************************/
int nandFile_find1FreeBlock( void )
{
	int i,findBlock=HY_ERROR;
	
	for(i = BLOCK_DATA_START; i <= BLOCK_DATA_END; i++)
	{
		if(IS_BLOCK_FREE(i))
		{
			#ifdef NANDFLASH_DEBUG
				hyUsbPrintf("find1FreeBlock i = %d \r\n", i);
			#endif
			if(hyhwNandFlash_Erase_Virt(i) == HY_OK)
			{
				#ifdef NANDFLASH_DEBUG
					hyUsbPrintf("find1FreeBlock erase ok \r\n");
				#endif
				findBlock =i;
				SET_BLOCK_USED(i);
				gNeedSaveBitmap = 1;
				break;
			}
		}
	}
	
	return findBlock;
}

/*************************************************************************
* ����: nandFile_create
* ˵��:	����һ���ļ�, ����ʼ����ǰ�ļ�����Ϣ(bitmap����,FileEntry����,block[0]�ݲ���)	  
* ����:	name	-- Ҫ�������ļ���
        cnt:�Ѿ����ڵ�ͬ���ļ���
* ����:	fileIdx  ʧ��:-1
*************************************************************************/
int nandFile_create( char *name, int cnt)
{
	int i, minidx=-1,abstractBlk, rc = HY_OK;
	char *pagebuf;
	int  minname,filename;
	tFILE_ENTRY	*ptFileEntry;
	tSEQUECE_PAGE	*ptseq_page;
	
	pagebuf = (U8 *)gpu8RecBuf;
	ptseq_page = (tSEQUECE_PAGE	*)pagebuf;
	
	filename = atoi(name);
	minname = 0x7FFFFFFF;//����������������һ���ļ���
	ptFileEntry	= NULL;
	/* �ҵ�һ���յ�fileEntry */
	for(i = 0; i < MAX_NAND_FILE; i++)
	{
		if( gtFileEntry[i].name == -1)
		{
			/* �ҵ�һ���յ���*/
			ptFileEntry = &gtFileEntry[i];
			rc = i ;
			break;
		}
		else
		{
			#ifdef NANDFLASH_DEBUG
				hyUsbPrintf("minidx = %d minname = %d  file idx = %d  file name = %d \r\n",minidx,minname,i,gtFileEntry[i].name);
			#endif
			if(minname > gtFileEntry[i].name)
			{
				minname = gtFileEntry[i].name;
				minidx = i;
			}
		}
	}
	#ifdef NANDFLASH_DEBUG
		hyUsbPrintf("minname = %d    %d \r\n",minname,minidx);
	#endif
	if(i >= MAX_NAND_FILE && minidx != -1)//û�п����ļ�,ɾ�������һ��
	{
		#ifdef NANDFLASH_DEBUG
			hyUsbPrintf("ɾ�������һ���ļ� \r\n");
		#endif
		nandFile_deleteByIndex(minidx);
		ptFileEntry = &gtFileEntry[minidx];
		rc = minidx;
		i = minidx;
	}
	
	if(ptFileEntry)
	{
		memset((char *)ptFileEntry,0xFF,sizeof(tFILE_ENTRY));
		
		ptFileEntry->name = filename; 
		if(cnt < 0) cnt = 0;
		ptFileEntry->dayIdx = cnt;
		ptFileEntry->totalRecords	= 0;
		ptFileEntry->reportedRecords= 0;
		ptFileEntry->deletedRecords	= 0;
		memset((char *)ptFileEntry->pageOffset,0,ABS_BLOCKS_PER_FILE*sizeof(U16));
		
		/* �ش� fileEntry���ش� fileEntry 	*/
		nandFile_saveFileEntry(pagebuf);
		/* ���һ���������Ϊ���ܴ�nandfile entry��ʱ�򣬸ı�bitmap*/
		nandFile_saveBitmap(pagebuf);
	}
	else
	{
		/* û���ҵ��յ��ļ�����ش��� */
		rc = -1;
	}

	return rc;
}	

/*-----------------------------------------------------------
*������nandFile_open
*���ܣ�ͨ���ļ����ҵ��ļ�������file idx   
       ע��:���ܴ���ͬ���ļ�, ���һ��ļ�¼̫��,һ���ļ���Ų���,��Ҫʹ��2���ļ�,��2���ļ�����ͬ.
*������	name -- �ļ���
*		pagebuf	-- ��ʱbuf,һ��page ��С
*���أ��ɹ��������ҵ����ļ�����   ʧ�ܣ�<=0
*------------------------------------------------------------*/
int nandFile_open(char *name, int *pFileIdx)
{
	int i, fileCnt;
	int filename;
	
	filename = atoi(name);
	/* �ȼٶ�û���ҵ�	*/
	fileCnt = 0;
	
	if(name[4] == 0)//���²�
	{
		/* �ҵ��ļ� */
		for(i = 0; i<MAX_NAND_FILE; i++)
		{
			if((gtFileEntry[i].name/100) == filename) 
			{
				/* �ҵ��ļ�*/
				pFileIdx[fileCnt++] = i;
			}
		}
	}
	else
	{
		/* �ҵ��ļ� */
		for(i = 0; i<MAX_NAND_FILE; i++)
		{
			if(gtFileEntry[i].name == filename) 
			{
				/* �ҵ��ļ�*/
				pFileIdx[fileCnt++] = i;
			}
		}
	}
	
	return fileCnt;
}

/*-----------------------------------------------------------
*������nandFile_write
*���ܣ�д���¼
*������fileIdx	-- �ļ���
*	   data  	-- ���棬��Ҫд������ݣ������ݲ���
*      len     -- ���ݳ�
*      recState  --- д��¼ʱ��¼�ĳ�ʼ״̬
*      reIdx   -- ����index
*���أ��ɹ���0   ʧ�ܣ�-1
*------------------------------------------------------------*/
int nandFile_write(int fileIdx, char *data,int len,U8 recState,U32 *reIdx)
{
	U32 pageOffset, idxOfPage, page_start, page_end;
	int needNewBlk, recordIdx, page;
	int i, blk, blknum, abstractBlk;
	U8  type, usertype;
	tFILE_ENTRY		*ptFile;
	U8  *pOldBuf = NULL, *pagebuf;
	int  oldNum = 0,oldPage;
	
	if(fileIdx < 0 || fileIdx > MAX_NAND_FILE || len > NAND_PAGE_DATASIZE)
	{
		return HY_ERROR;
	}
	
	ptFile		= &gtFileEntry[fileIdx];
	
	if(ptFile->totalRecords >= MAX_RECORDS_PER_FILE)//�ﵽ���block��
	{
		#ifdef NANDFLASH_DEBUG
			hyUsbPrintf("totalRecords (%d) > MAX_RECORDS_PER_FILE (%d) \r\n", ptFile->totalRecords, MAX_RECORDS_PER_FILE);
		#endif
		//���½��ļ�  �ݲ�����
		return  HY_ERROR;
	}
	
	/* �ҵ� blk Idx �� pageOffset*/
	blknum		= ptFile->totalRecords/RECORDS_PER_BLOCK;
	pageOffset	= ptFile->totalRecords%RECORDS_PER_BLOCK;
	
	idxOfPage   = pageOffset%RECORDS_PER_PAGE;
	pageOffset  = pageOffset/RECORDS_PER_PAGE;
	
	/* ����ǰ��ABS_BLOCKS_PER_FILE ��	blk  ������;������blknum +ABS_BLOCKS_PER_FILE, ��������õ�����blk idx*/
	blknum		+= ABS_BLOCKS_PER_FILE;
	blk	= ptFile->blk[blknum];
	
	
	/* �ж��Ƿ���Ҫ�µ�blk */
	needNewBlk = 0;
	if(blk == 0xFFFF)
	{
		needNewBlk = 1;
	}
	else
	{
		page = (blk<<PAGE_PER_BLK_BIT) + pageOffset;
	}

	
	do
	{
		if(needNewBlk)
		{
			/* ����*/
			blk = nandFile_find1FreeBlock();
			if(blk == -1)
			{
				#ifdef NANDFLASH_DEBUG
					hyUsbPrintf("--- write record find block fail \r\n");
				#endif
				/* ���䲻�����ˣ����ش��� */
				return HY_ERROR;
			}
			
			ptFile->blk[blknum] = blk;
			
			/* ��block ͷ��ʼд*/
			page = (blk <<PAGE_PER_BLK_BIT); 
			
			/* ��ʾ�Ѿ��������*/
			needNewBlk = 0;
		}
		
		/*	kong 2011-05-16 
			��һ��ѭ���У�û�гɹ�д�룬֮ǰ��blk����  ��Ҫ�Ѽ�¼copy����blk�� 
			���� һ:�ᵼ�¼�¼��ʧ  ��:�ᵼ��nand�м�¼������totalRecords ��һ��  ����readʱ�쳣 */
		if(pOldBuf && hyhwNandFlash_PageWrite_Virt(pOldBuf, page, 0,oldNum*NAND_PAGE_SIZE+RECORD_PER_SIZE*idxOfPage) != HY_OK)
		{
			
			/* ǰ��ľ���Ϣcopy���µ�block ��û�гɹ������������¿� */
			needNewBlk = 1;
			SET_BLOCK_USED(blk);	/* ֱ�ӱ��Ϊ���� */
			gNeedSaveBitmap = 1;//�´���Ҫ��bitmap
			continue;
		}
		else if(pOldBuf)//kong 2011-05-24 �������block�е����ݳɹ�д�뵽�������block��,��д������ǰ��Ҫ����page
		{
			page += oldNum;
		}
		
		/*kong 2011-05-24 �������ȰѸñ����Ϊ1,����д�ɹ����������Ϊ0 */
		needNewBlk = 1;
		
		if(hyhwNandFlash_PageWrite_Virt(data, page, RECORD_PER_SIZE*idxOfPage,RECORD_PER_SIZE) == HY_OK)
		{
			//hyUsbPrintf("\r\nwrite totalRecords = %d date = %c  blk = %d  page = %d  idx = %d\r\n",
			//	ptFile->totalRecords,ptFile->name[5],blk,page,idxOfPage);
			
			/* д�ɹ������ */
			ptFile->totalRecords++;
			needNewBlk = 0;
		}
		else if(pOldBuf == NULL)
		{
			/* ����֮ǰ��û�з����¿飬û��д�ɹ�������봦������ԭ�еļ�¼��Ҫcopy����block��*/
			oldPage	= (blk <<PAGE_PER_BLK_BIT);
			oldNum	= pageOffset;
			
			if(oldNum > 0 || idxOfPage > 0)
			{
				/* �����õ���Ϣ������ */
				pOldBuf = (U8 *)(gpu8RecBuf+12288); /*128k */
				hyhwNandFlash_DataRead_Virt((S8*)pOldBuf, oldPage, 0, oldNum*NAND_PAGE_SIZE+RECORD_PER_SIZE*idxOfPage);
			}

			SET_BLOCK_USED(blk);	/* ֱ�ӱ��Ϊ���� */
			gNeedSaveBitmap = 1;//�´���Ҫ��bitmap
			//hyUsbPrintf("PageWrite fail   blk = %d   page = %d\r\n",blk,page);
		}
		else//kong 2011-05-24 �Ѿɼ�¼ȫ��copy���������block�гɹ���������copy��ǰһ���¼�¼ʱʧ���ˣ���Ҫ�ٴΰ��������block���Ϊ����
		{
			SET_BLOCK_USED(blk);	/* ֱ�ӱ��Ϊ���� */
			gNeedSaveBitmap = 1;//�´���Ҫ��bitmap
		}
		
	}while(needNewBlk);
		
	/* ���濪ʼ�ļ�¼��ժҪ��Ϣ */
	pagebuf = (U8 *)gpu8RecBuf;
	
	/* �õ���ǰ��¼��type �� usertype */
	nandFile_CallBack_GetStatueValue(data, len, &type, &usertype);
	
	/* ����record ��״̬����ժҪblk ���� */
	recordIdx	= ptFile->totalRecords -1;
	
	//�ҵ��ü�¼��Ҫд���block��page
	blk 		= recordIdx / ABSTRACT_NUM_PER_PAGE;//��ǰ��Ҫ���µļ�¼���ڵ�blk
	pageOffset  = recordIdx % ABSTRACT_NUM_PER_PAGE;//�ü�¼�ڵ�ǰpage�е�ƫ��
		
	abstractBlk = ptFile->blk[blk];
	
	needNewBlk = 0;
	if(abstractBlk == 0xFFFF)
	{
		needNewBlk = 1;
	}
	else
	{
		page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
		page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
		page		= page_start + ptFile->pageOffset[blk];
	}
	#ifdef NANDFLASH_DEBUG
		hyUsbPrintf("write   type = %d  usertype  = %d  recState = %d  \r\n",type, usertype, recState);	
	#endif
	memset(pagebuf,0xFF,NAND_PAGE_SIZE);
	do
	{
		if(needNewBlk)
		{
			/* ����*/
			abstractBlk = nandFile_find1FreeBlock();
			if(abstractBlk == -1)
			{
				#ifdef NANDFLASH_DEBUG
					hyUsbPrintf("--- write abs find block fail \r\n");
				#endif
				/* ���䲻�����ˣ����ش��� */
				return HY_ERROR;
			}
			
			ptFile->blk[blk] = abstractBlk;
			ptFile->pageOffset[blk] = 0;
			
			/* ��block ͷ��ʼд*/
			page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
			page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
			page		= page_start;
						
			/* ��ʾ�Ѿ��������*/
			needNewBlk = 0;
			
			((tRECORD_STATUS *)pagebuf)[pageOffset].type     = type ;
			((tRECORD_STATUS *)pagebuf)[pageOffset].userType = usertype;
			((tRECORD_STATUS *)pagebuf)[pageOffset].recState = recState;
			//hyUsbPrintf("write rec type = %d, usertype = %d, state = %d \r\n",
			//	((tRECORD_STATUS *)pagebuf)[pageOffset].type,
			//	((tRECORD_STATUS *)pagebuf)[pageOffset].userType,
			//	((tRECORD_STATUS *)pagebuf)[pageOffset].recState);
		}
		else
		{
			hyhwNandFlash_DataRead_Virt((S8*)pagebuf, page, 0, NAND_PAGE_SIZE);
			((tRECORD_STATUS *)pagebuf)[pageOffset].type     = type ;
			((tRECORD_STATUS *)pagebuf)[pageOffset].userType = usertype;
			((tRECORD_STATUS *)pagebuf)[pageOffset].recState = recState;
			//hyUsbPrintf("write rec type = %d, usertype = %d, state = %d \r\n",
			//	((tRECORD_STATUS *)pagebuf)[pageOffset].type,
			//	((tRECORD_STATUS *)pagebuf)[pageOffset].userType,
			//	((tRECORD_STATUS *)pagebuf)[pageOffset].recState);
				
			page++;
			if(page > page_end)
			{
				/* �����ˣ����´�ǰ��д�� */
				page = page_start;
				if(hyhwNandFlash_Erase_Virt(abstractBlk) == HY_ERROR)
				{
					SET_BLOCK_USED(abstractBlk);
					gNeedSaveBitmap = 1;
					needNewBlk = 1;
					continue;
				}
			}
		}
		
		if(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK)
		{
			needNewBlk = 0;
		}
		else
		{
			/*дʧ�ܺ������֮ǰ����page�д��ժҪȫcopy����,��Ϊ���µ�һ��page�������Ѿ�������,��������blockʱֻ�����������д�뼴��*/
			SET_BLOCK_USED(abstractBlk);
			gNeedSaveBitmap = 1;
			needNewBlk = 1;
		}
		
	}while(needNewBlk);
	
	ptFile->pageOffset[blk] = page - page_start;

	nandFile_saveFileEntry(pagebuf);
	nandFile_saveBitmap(pagebuf);
	
	*reIdx = ptFile->totalRecords -1 ;
	
	return HY_OK;
}

/*-----------------------------------------------------------
*������nandFile_read
*���ܣ���ȡָ����Ϣ������
*������fileIdx
*	   :���  pBuf:��Ŷ�ȡ������ data:ָ��������
*���أ��ɹ���0   ʧ�ܣ�-1
*------------------------------------------------------------*/
int nandFile_read(int fileIdx, int recordIdx, char *buff, int len)
{
	U32 blk, page, pageOffset,idxOfPage;
	U32 rc;
	U8 *pagebuf;
	tFILE_ENTRY		*ptFile;
	
	pagebuf = (U8 *)gpu8RecBuf;
	
	if (fileIdx < 0 || fileIdx >= MAX_NAND_FILE)
	{
		#ifdef NANDFLASH_DEBUG
			hyUsbPrintf("fileidx err !!! = %d \r\n",fileIdx);
		#endif
		return HY_ERROR;
	}
	
	ptFile		= &gtFileEntry[fileIdx];
	
	if(recordIdx >= ptFile->totalRecords || ptFile->totalRecords == 0xFFFF)
	{
		#ifdef NANDFLASH_DEBUG
			hyUsbPrintf("recordIdx or totalRecords  err !!! = %d  %d\r\n",recordIdx, ptFile->totalRecords);
		#endif
		/* record idx ���� �� �ļ��ѱ�ɾ��*/
		return  HY_ERROR;
	}
		
	/* �ҵ� blk Idx �� pageOffset*/
	blk 		= recordIdx/RECORDS_PER_BLOCK;
	pageOffset	= recordIdx%RECORDS_PER_BLOCK;
	
	idxOfPage   = pageOffset%RECORDS_PER_PAGE;
	pageOffset  = pageOffset/RECORDS_PER_PAGE;
	

	/* ����ǰ��ABS_BLOCKS_PER_FILE��blk ������;������blknum +ABS_BLOCKS_PER_FILE, ��������õ�����blk idx*/
	blk	+= ABS_BLOCKS_PER_FILE;
	blk	= ptFile->blk[blk];
	
	page= (blk<<PAGE_PER_BLK_BIT) + pageOffset; 
	
	rc = hyhwNandFlash_DataRead_Virt((S8*)pagebuf, page, idxOfPage*RECORD_PER_SIZE, len);
	//hyUsbPrintf("hyhwNandFlash_DataRead_Virt ��ȡ���(0:�ɹ�)  %d\r\n",rc);
	if(HY_OK == rc)
	{
		//hyUsbPrintf("read data = %d\r\n",*(U32*)pagebuf);
		//kong 2011-05-16   �Զ������������ж�
		if(*(U32*)pagebuf != 0xFFFFFFFF)
		{
			memcpy(buff, pagebuf, len);
		}
		else
		{
			rc = HY_ERROR;
		}	
	}
	
	return rc;
}

/*************************************************************************
* ����: nandFile_delete
* ˵��:	ɾ���ļ�	  ע��:���ܴ���ͬ���ļ�
* ����:	name	-- Ҫɾ�����ļ���
* ����:	none
*************************************************************************/
int nandFile_delete(char *name)
{
	int i,j,filename;
	int rc = HY_ERROR;
	char	*pagebuf;
	tFILE_ENTRY	*ptFile;
	
	filename = atoi(name);
	ptFile	= NULL;
	
	/* �ҵ��ļ� */
	for(i = 0; i<MAX_NAND_FILE; i++)
	{
		if(gtFileEntry[i].name == filename)
		{
			rc = HY_OK;
			/* �ҵ��ļ�*/
			ptFile = &gtFileEntry[i];
			
			/* �ͷ�blk*/
			for(j = 0; j< MAX_BLOCKS_PER_FILE; j++)
			{
				/* ��ȷ�����˲����ͷ�*/
				if(ptFile->blk[j] !=0xFFFF)
				{
					SET_BLOCK_FREE(ptFile->blk[j]);
					gNeedSaveBitmap = 1;
				}
			}
			
			/* �ͷ�Ŀ¼��*/
			memset((char *)ptFile,0xFF,sizeof(tFILE_ENTRY));
		}
	}
	
	if(rc == HY_OK)
	{
		pagebuf = (char *)gpu8RecBuf;
		
		/* �ش� fileentry���ش� fileEntry 	*/
		/* ���һ����������ڿ��ܴ�nandfile entry��ʱ�򣬸ı�bitmap*/
		nandFile_saveFileEntry(pagebuf);
		nandFile_saveBitmap(pagebuf);
	}
	
	return rc ;
}	

/*************************************************************************
* ����: nandFile_deleteByIndex
* ˵��:	ɾ���ļ�	  
* ����:	index	-- Ҫɾ�����ļ�����ֵ
* ����:	none
*************************************************************************/
int nandFile_deleteByIndex(int index)
{
	int i,j,rc = HY_OK;
	int filename;
	char *pagebuf;
	tFILE_ENTRY	*ptFile;

	if (index < 0 || index >= MAX_NAND_FILE)
	{
		return HY_ERROR;
	}
	
	ptFile = &gtFileEntry[index];
	filename = ptFile->name;//������ͬ���ļ�,��Ҫ�������ļ��в���һ��
	for(i = 0; i < MAX_NAND_FILE; i++)
	{
		if(gtFileEntry[i].name == filename) 
		{
			/* �ҵ��ļ�*/
			ptFile = &gtFileEntry[i];
			/* �ͷ�blk*/
			for(j = 0; j< MAX_BLOCKS_PER_FILE; j++)
			{
				/* ��ȷ�����˲����ͷ�*/
				if(ptFile->blk[j] !=0xFFFF)
				{
					SET_BLOCK_FREE(ptFile->blk[j]);
					gNeedSaveBitmap = 1;
				}
			}
			
			
			/* �ͷ�Ŀ¼��*/
			memset((char *)ptFile,0xFF,sizeof(tFILE_ENTRY));
		}
	}	
	
	pagebuf = (U8 *)gpu8RecBuf;
	/* �ش� fileentry���ش� fileEntry 	*/
	/* ���һ����������ڿ��ܴ�nandfile entry��ʱ�򣬸ı�bitmap*/
	nandFile_saveFileEntry(pagebuf);
	nandFile_saveBitmap(pagebuf);
	
	return rc;
}

/*-----------------------------------------------------------
*������nandFile_WriteBackAbs
*���ܣ���дժҪblock
*������fileIdx		-- �ļ�index��
*	   recordIdx	-- ��¼index��
       pAbsData     -- �µ�ժҪ��Ϣ
*���أ��ɹ���HY_OK   
*	   ʧ�ܣ�HY_ERROR
*------------------------------------------------------------*/
int nandFile_WriteBackAbs(int fileIdx, int recordIdx, U8 *pAbsData)
{
	int i,blk;
	int	page_start, page_end, page, abstractBlk, pageOffset;
	char *pagebuf;
	int  ret;
	tFILE_ENTRY		*ptFile;
	
	if(fileIdx > MAX_NAND_FILE)
	{
		return HY_ERROR;
	}
	
	pagebuf = (U8 *)gpu8RecBuf;
	ptFile	= &gtFileEntry[fileIdx];
	
	//hyUsbPrintf("change state idx = %d\r\n",recordIdx);
	if(recordIdx >= ptFile->totalRecords)
	{
		return HY_ERROR;
	}
	
	//��д�����ժҪ��Ϣ
	memcpy(pagebuf, pAbsData, NAND_PAGE_SIZE);
	
	blk			= recordIdx / ABSTRACT_NUM_PER_PAGE;
	pageOffset	= recordIdx % ABSTRACT_NUM_PER_PAGE;
	
	abstractBlk = ptFile->blk[blk];
	page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
	page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
	page 		= page_start+ptFile->pageOffset[blk];
	
	/* �ش浽��һ��page �� */
	page ++;
	
	if(page > page_end)
	{
		/* �����ˣ����´�ǰ��д�� */
		page = page_start;
	}
	/* ע�����������˳���ܱ䣬Ҳ���ܼ�*/
	if((page == page_start && 
	    hyhwNandFlash_Erase_Virt(abstractBlk) == HY_OK &&
	    hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK) 
	    )
	{
		
	}
	else if(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK)
	{
		
	}
	else
	{
		do
		{
			/*������*/
			SET_BLOCK_USED(abstractBlk);
			gNeedSaveBitmap = 1;
			
			/*���·��� blk */
			abstractBlk = nandFile_find1FreeBlock();
			if(abstractBlk == -1)
			{
				/* ���䲻�����ˣ����ش��� */
				return HY_ERROR;
			}
			
			ptFile->blk[blk] = abstractBlk;
			ptFile->pageOffset[blk] = 0;
			
			/* kong 2011-05-24 �����������������block,��Ҫ���¶�λpage */
			page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
			page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
			page		= page_start;
		
		}while(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE)!=HY_OK);
	}
	
	ptFile->pageOffset[blk] = page - page_start;
	
	if(ptFile->deletedRecords == ptFile->totalRecords)	//���ȫ��ɾ��,��ɾ���ļ�
	{
		nandFile_deleteByIndex(fileIdx);
	}

	/* ����file entry*/
	nandFile_saveFileEntry(pagebuf);
	nandFile_saveBitmap(pagebuf);
	
	return HY_OK;
}


/*-----------------------------------------------------------
*������nandFile_changeRecordStatus
*���ܣ��޸ļ�¼״̬
*      Ϊ�˼��ٲ������������洢abstract page �� blk ����
*	   ���һ���ļ���abstract page �������ABSTRACT_PAGE_NUM����
*	   ��ʹ��ǰABSTRACT_PAGE_NUM��blk ��Ϊabstract page ��blk
*	   ÿ�θ���ʱ��ÿ��blk ������һ��page
*������fileIdx		-- �ļ�index��
*	   recordIdx	-- ��¼index��
*	   modify_func	-- ����ָ�룬ִ���������޸�
*���أ��ɹ���HY_OK   
*	   ʧ�ܣ�HY_ERROR
*------------------------------------------------------------*/
int nandFile_changeRecordStatus(int fileIdx, int recordIdx, MODIFY_STATUE_FUNC modify_func)
{
	int i,blk;
	int	page_start, page_end, page, abstractBlk, pageOffset;
	char *pagebuf;
	int  ret;
	tFILE_ENTRY		*ptFile;
	
	if(fileIdx > MAX_NAND_FILE)
	{
		return HY_ERROR;
	}
	
	pagebuf = (U8 *)gpu8RecBuf;
	ptFile	= &gtFileEntry[fileIdx];
	
	//hyUsbPrintf("change state idx = %d\r\n",recordIdx);
	if(recordIdx >= ptFile->totalRecords)
	{
		return HY_ERROR;
	}
	
	blk			= recordIdx / ABSTRACT_NUM_PER_PAGE;
	pageOffset	= recordIdx % ABSTRACT_NUM_PER_PAGE;
	
	abstractBlk = ptFile->blk[blk];
	page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
	page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
	page 		= page_start+ptFile->pageOffset[blk];
	
	//����֮ǰ�ļ�¼ժҪ,ֻ����ǰ��Ҫ�ı�ļ�¼
	hyhwNandFlash_DataRead_Virt((S8*)pagebuf, page, 0, NAND_PAGE_SIZE);
	
	ret = modify_func(&((tRECORD_STATUS *)pagebuf)[pageOffset]);	//�޸�,
	//�����޸ĺ��״̬
	if(ret == -1)
	{
		return HY_ERROR;
	}
	
	/* �ش浽��һ��page �� */
	page ++;
	
	if(page > page_end)
	{
		/* �����ˣ����´�ǰ��д�� */
		page = page_start;
	}
	/* ע�����������˳���ܱ䣬Ҳ���ܼ�*/
	if((page == page_start && 
	    hyhwNandFlash_Erase_Virt(abstractBlk) == HY_OK &&
	    hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK) 
	    )
	{
		
	}
	else if(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK)
	{
		
	}
	else
	{
		do
		{
			/*������*/
			SET_BLOCK_USED(abstractBlk);
			gNeedSaveBitmap = 1;
			
			/*���·��� blk */
			abstractBlk = nandFile_find1FreeBlock();
			if(abstractBlk == -1)
			{
				/* ���䲻�����ˣ����ش��� */
				return HY_ERROR;
			}
			
			ptFile->blk[blk] = abstractBlk;
			ptFile->pageOffset[blk] = 0;
			
			/* kong 2011-05-24 �����������������block,��Ҫ���¶�λpage */
			page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
			page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
			page		= page_start;
		
		}while(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE)!=HY_OK);
	}
	
	ptFile->pageOffset[blk] = page - page_start;
	if (ret == 1)//�Ѵ�
	{
		ptFile->reportedRecords++;
	}	
	else if (ret == 2)//��ɾ
	{
		ptFile->deletedRecords++; 
	}
	
	if(ptFile->deletedRecords == ptFile->totalRecords)	//���ȫ��ɾ��,��ɾ���ļ�
	{
		nandFile_deleteByIndex(fileIdx);
	}

	/* ����file entry*/
	nandFile_saveFileEntry(pagebuf);
	nandFile_saveBitmap(pagebuf);
	
	return HY_OK;
}

/*-----------------------------------------------------------
*������nandFile_changeRecordStatusMulti
*���ܣ��޸ļ�¼״̬
*      Ϊ�˼��ٲ������������洢abstract page �� blk ����
*	   ���һ���ļ���abstract page �������ABSTRACT_PAGE_NUM����
*	   ��ʹ��ǰABSTRACT_PAGE_NUM��blk ��Ϊabstract page ��blk
*	   ÿ�θ���ʱ��ÿ��blk ������һ��page
*������fileIdx		-- �ļ�index��
*	   recordIdx	-- ��¼index��
*	   modify_func	-- ����ָ�룬ִ���������޸�
*���أ��ɹ���HY_OK   
*	   ʧ�ܣ�HY_ERROR
*------------------------------------------------------------*/
int nandFile_changeRecordStatusMulti(int fileIdx, int *recordIdx, int *num, MODIFY_STATUE_FUNC modify_func)
{
	int i,blk;
	int	page_start, page_end, page, abstractBlk, pageOffset;
	char *pagebuf;
	int  ret,change;
	tFILE_ENTRY		*ptFile;
	
	//hyUsbPrintf("fileidx = %d num = %d \r\n",fileIdx,*num);
	if(fileIdx > MAX_NAND_FILE)
	{
		return HY_ERROR;
	}
	
	pagebuf = (U8 *)gpu8RecBuf;
	ptFile	= &gtFileEntry[fileIdx];
#ifdef NANDFLASH_DEBUG
	hyUsbPrintf("change state totalRecords = %d  reportedRecords = %d \r\n",ptFile->totalRecords, ptFile->reportedRecords);
#endif
	if(recordIdx[0] >= ptFile->totalRecords)
	{
		return HY_ERROR;
	}
	
	blk			= recordIdx[0] / ABSTRACT_NUM_PER_PAGE;
	
	abstractBlk = ptFile->blk[blk];
	page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
	page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
	page 		= page_start+ptFile->pageOffset[blk];
	
	//����֮ǰ�ļ�¼ժҪ,ֻ����ǰ��Ҫ�ı�ļ�¼
	hyhwNandFlash_DataRead_Virt((S8*)pagebuf, page, 0, NAND_PAGE_SIZE);
	
	change = 0;
	for(i = 0; i < *num; i++)
	{
		pageOffset	= recordIdx[i] % ABSTRACT_NUM_PER_PAGE;
		
		ret = modify_func(&((tRECORD_STATUS *)pagebuf)[pageOffset]);	//�޸�,
		if(ret > 0)
		{
			change++;
		}
	}
	*num = change;
	
	if(change <= 0)
	{
		return HY_ERROR;
	}
	
	/* �ش浽��һ��page �� */
	page ++;
	
	if(page > page_end)
	{
		/* �����ˣ����´�ǰ��д�� */
		page = page_start;
	}
	/* ע�����������˳���ܱ䣬Ҳ���ܼ�*/
	if((page == page_start && 
	    hyhwNandFlash_Erase_Virt(abstractBlk) == HY_OK &&
	    hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK) 
	    )
	{
		
	}
	else if(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK)
	{
		
	}
	else
	{
		do
		{
			/*������*/
			SET_BLOCK_USED(abstractBlk);
			gNeedSaveBitmap = 1;
			
			/*���·��� blk */
			abstractBlk = nandFile_find1FreeBlock();
			if(abstractBlk == -1)
			{
				/* ���䲻�����ˣ����ش��� */
				return HY_ERROR;
			}
			
			ptFile->blk[blk] = abstractBlk;
			ptFile->pageOffset[blk] = 0;
			
			/* kong 2011-05-24 �����������������block,��Ҫ���¶�λpage */
			page_start	= (abstractBlk<<PAGE_PER_BLK_BIT);
			page_end	= ((abstractBlk+1)<<PAGE_PER_BLK_BIT)-1;
			page		= page_start;
		
		}while(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE)!=HY_OK);
	}
	
	ptFile->pageOffset[blk] = page - page_start;
	if (ret == 1 || ret == -1)//�Ѵ�
	{
		#ifdef NANDFLASH_DEBUG
			hyUsbPrintf("change = %d  total = %d  cur = %d \r\n", ptFile->reportedRecords, ptFile->totalRecords, change);
		#endif
		if(ptFile->reportedRecords+change > ptFile->totalRecords-ptFile->deletedRecords)
		{
			ptFile->reportedRecords = ptFile->totalRecords-ptFile->deletedRecords;
		}
		else
		{
			ptFile->reportedRecords+=change;
		}	
	}
	else if (ret == 2 || ret == -2)//��ɾ
	{
		if(ptFile->deletedRecords+change > ptFile->totalRecords-ptFile->reportedRecords)
		{
			ptFile->deletedRecords = ptFile->totalRecords-ptFile->reportedRecords;
		}
		else
		{
			ptFile->deletedRecords+=change;
		}
	}
	
	if (ptFile->deletedRecords == ptFile->totalRecords)	//���ȫ��ɾ��,��ɾ���ļ�
	{
		nandFile_deleteByIndex(fileIdx);
	}

	/* ����file entry*/
	nandFile_saveFileEntry(pagebuf);
	nandFile_saveBitmap(pagebuf);
	
	return HY_OK;
}
