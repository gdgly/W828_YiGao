#include "hyTypes.h"
#include "hyswNandFile.h"
#include "reserveManager.h"

/*===============================================================*/

static U32 gu32PicSaveBitmap;	//�´δ��bitmap��page index
static U32 gu32PicSaveFile[2];	//�´δ���ļ���page index


static U8   gu8PicNeedSaveBitmap = 0;
static U32	gu32PicBitmap[PIC_BITMAP_WORDS];

char	*gpPicBuf = NULL;

tPICFILE	gtPicFile[2];

/*---------------------------------------------------------------------------------*/
U32 nandPic_saveSequecePage(U32 page_start, U32 page_end, char *pagebuf, int *pSavePage) 
{
	U8		isBitmap = 0;//���δ洢���Ƿ�bitmap
	U32		rc,*pBitmap,*pBadBlock;
	int		page;
	int		blk, pageOffset;
	int		i,sblk,eblk;
	int		maxPage,totalPage;
	tSEQUECE_PAGE	*ptseq_page;
	
	
	if(page_start == (PIC_BLOCK_START_BITMAP<<PAGE_PER_BLK_BIT))
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
//hyUsbPrintf("page = %d  start = %d  end = %d \r\n",page, page_start, page_end);	
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
		//hyUsbPrintf("save seq  start == 0000,  erase \r\n");
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
			//hyUsbPrintf("11 save ok crc = %d tag1 = %x tag2 = %x  page = %d\r\n",ptseq_page->crc16,ptseq_page->tag1,ptseq_page->tag2,page);
			break;
		}
		else if(hyhwNandFlash_PageWrite_Virt((S8 *)pagebuf,page,0,NAND_PAGE_SIZE) == HY_OK)
		{
			//hyUsbPrintf("22 save ok crc = %d tag1 = %x tag2 = %x  page = %d\r\n",ptseq_page->crc16,ptseq_page->tag1,ptseq_page->tag2,page);
			break;
		}
		else	 	
		{		
			//PIC_SET_BLOCK_USED(blk);
			if(isBitmap == 1)
			{
				//�˴���Ҫͬ�����±�����Ҫд�����Ϣ
				pBitmap[blk>>5] &= ~(1<<(blk &0x1F));
				//pBadBlock[blk>>5] &= ~(1<<(blk &0x1F));
			}
			else
			{
				gu8PicNeedSaveBitmap = 1;//�´���Ҫ��bitmap
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
* ����:	nandPic_getNewestSequecePage
* ˵��: ����ѭ��ʹ�õ�page�����µ�page��
* ����: page_start	--	��ʼ����page ��
*		page_end	--  ��������page ��,�����һ����Чpage
*		pagebuf		--  �����pagebuf����ʱʹ�ã���Ҫ��֤4�ֽڶ��룬�������µ�page��Ϣ
*						ע���С��Ҫ2��page
*		pSavePage	--  ȡ����Ϣ��page ��
* ����: �ҵ����򷵻�HY_OK, ���򷵻�HY_ERROR
*************************************************************************/
U32 nandPic_getNewestSequecePage(U32 page_start, U32 page_end, char *pagebuf, int *pSavePage) 
{
	int j;
	U32 rc;	
	int page, crc;
	tSEQUECE_PAGE	*ptseq_page, *ptseq_nextpage;
	U32 *pword;
	
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
	//hyUsbPrintf("page = %d tag1  =%d  tag2 = %d  crc = %d   %d \r\n",page,ptseq_nextpage->tag1,ptseq_nextpage->tag2,crc,ptseq_nextpage->crc16);
	while(page <= page_end)
	{	
		
		/*	����ؼ��ִ��ڣ���ʾnandflash bitmap ���� */
		if(	ptseq_page->tag1 == NAND_SEQUECE_PAGE_TAG &&
			ptseq_page->tag2 == NAND_SEQUECE_PAGE_TAG &&
			ptseq_page->crc16== crc )
		{
			
			//hyUsbPrintf("crc ok  page = %d \r\n",page);
			/* ӳ����У�����������һ��page�������page�������µ�ӳ���*/
			if( page == page_end )
			{
			//hyUsbPrintf("page == page_end \r\n");
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
					ptseq_nextpage->crc16== crc             &&
					( ( ptseq_nextpage->sequece < ptseq_page->sequece) || 
					  ( ptseq_nextpage->sequece == 0xFFFF && ptseq_page->sequece == 0)
				    ) )
				{
					//hyUsbPrintf("nextpage->sequece = %d  ptseq_page->sequece =%d \r\n",ptseq_nextpage->sequece,ptseq_page->sequece);
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
							ptseq_nextpage->crc16== crc             &&
							ptseq_nextpage->sequece == ptseq_page->sequece )
						{
							page = (blk+1)<<PAGE_PER_BLK_BIT ;
							page -= 1 ; /* ��Ϊ������ page++ �� */
							
							//hyUsbPrintf("nest block page is ok \r\n");
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
			//hyUsbPrintf("crc err   = %x  %x  %d  %d \r\n",ptseq_page->tag1,ptseq_page->tag2,crc,ptseq_page->crc16);
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

/*----------------------------------
* ����: nandPic_format
* ����: ��ʽ��bitmap���ļ�
* ����: pagebuf
* ����: 0:�ɹ�    -1:ʧ��
*-----------------------------------*/
U32 nandPic_format(char *pagebuf)
{
	S32	page, i, usedBlknum;
	U32 *pword, sblk,eblk;
	tSEQUECE_PAGE	*ptpageBitmap, *ptpageEntry;
	

	gu32PicSaveBitmap  = (PIC_BLOCK_START_BITMAP<<PAGE_PER_BLK_BIT);
	gu32PicSaveFile[0] = (PIC_FILE1_START<<PAGE_PER_BLK_BIT);
	gu32PicSaveFile[1] = (PIC_FILE2_START<<PAGE_PER_BLK_BIT);
	
	/* ��Ϊ����format,��һ��page Ҳû�� ,���԰ѵ�ǰ��page ��Ϊ��һ�� */
	gu32PicSaveBitmap  -= 1;
	gu32PicSaveFile[0] -= 1;
	gu32PicSaveFile[1] -= 1;
	
	for(i = 0 ; i < PIC_BLOCK_NUM_BITMAP; i++)
	{
		hyhwNandFlash_Erase_Virt(PIC_BLOCK_START_BITMAP+i);
	}
	
	for(i = 0 ; i < PIC_BLOCK_FILEINFO; i++)
	{
		hyhwNandFlash_Erase_Virt(PIC_FILE1_START+i);
	}
	
	for(i = 0 ; i < PIC_BLOCK_FILEINFO; i++)
	{
		hyhwNandFlash_Erase_Virt(PIC_FILE2_START+i);
	}
	
	/*	�����������ʼ��ʱ��telechip����ʹ�õ�block����Ϊbitmap �Լ�reserve�ռ��blockΪ"��"�飬
		��Ϊ���ǲ���������Ϊ�洢���ݵ�block�� */
	usedBlknum		= PIC_FILE2_END;
	
	memset((char *)gu32PicBitmap,0xff,PIC_BITMAP_WORDS * sizeof(U32));
	
	pword = (U32*)gu32PicBitmap;
	for(i= 0; i <= usedBlknum; i++)
	{
		/* ��Ǹ�block�ѱ�ʹ�� */
		pword[i>>5] &= ~(1<<(i &0x1F));
	}
	
	//��������в��ֱ���block
	pword = (U32*)gu32PicBitmap;
	for(i= BLOCK_PIC_END+1; i < (NAND_FLASH_SIZE<<3); i++)
	{
		/* ��Ǹ�block�ѱ�ʹ�� */
		pword[i>>5] &= ~(1<<(i &0x1F));
	}
	
	/*-------------------------------------------entry-----------------------*/
	memset((char *)&gtPicFile[0],0xff, 2*sizeof(tPICFILE));
	//��1��
	ptpageEntry		= (tSEQUECE_PAGE *)pagebuf;
	
	memset((char *)ptpageEntry,0xFF,NAND_PAGE_DATASIZE);	
	ptpageEntry -> sequece	= 0;
	ptpageEntry -> tag1		= NAND_SEQUECE_PAGE_TAG;
	ptpageEntry -> tag2		= NAND_SEQUECE_PAGE_TAG;
	ptpageEntry ->crc16 = glbVariable_CalculateCRC((U8*)ptpageEntry,NAND_PAGE_DATASIZE);
	
	sblk = PIC_FILE1_START;
	eblk = (PIC_FILE1_END+1);
	page	= gu32PicSaveFile[0];
	nandPic_saveSequecePage( (sblk<<PAGE_PER_BLK_BIT),
							  (eblk<<PAGE_PER_BLK_BIT)-1,
							  (char *)(ptpageEntry), 
							  &page);
	gu32PicSaveFile[0] = page;
	
	//��2��
	ptpageEntry		= (tSEQUECE_PAGE *)pagebuf;
	
	memset((char *)ptpageEntry,0xFF,NAND_PAGE_DATASIZE);
	ptpageEntry -> sequece	= 0;
	ptpageEntry -> tag1		= NAND_SEQUECE_PAGE_TAG;
	ptpageEntry -> tag2		= NAND_SEQUECE_PAGE_TAG;
	ptpageEntry ->crc16 = glbVariable_CalculateCRC((U8*)ptpageEntry,NAND_PAGE_DATASIZE);
	
	sblk = PIC_FILE2_START;
	eblk = (PIC_FILE2_END+1);
	
	page	= gu32PicSaveFile[1];
	nandPic_saveSequecePage( (sblk<<PAGE_PER_BLK_BIT),
							  (eblk<<PAGE_PER_BLK_BIT)-1,
							  (char *)(ptpageEntry), 
							  &page);
	gu32PicSaveFile[1] = page ;
	
	/*------------------------------------------bitmap------------------------*/
	/* ʹ�������buf*/
	ptpageBitmap		= (tSEQUECE_PAGE *)pagebuf;
	
	/* �ȼٶ����е�block���Ǻõ�*/
	memset((char *)ptpageBitmap,0xFF,NAND_PAGE_DATASIZE);
	ptpageBitmap -> sequece	= 0;
	ptpageBitmap -> tag1		= NAND_SEQUECE_PAGE_TAG;
	ptpageBitmap -> tag2		= NAND_SEQUECE_PAGE_TAG;

	
	memcpy(ptpageBitmap->data,(char *)gu32PicBitmap, PIC_BITMAP_WORDS * sizeof(U32));

	ptpageBitmap ->crc16 = glbVariable_CalculateCRC((U8*)ptpageBitmap,NAND_PAGE_DATASIZE);	
	
	/* ����bitmap*/
	page 	= gu32PicSaveBitmap;
	
	sblk = PIC_BLOCK_START_BITMAP;
	eblk = (PIC_BLOCK_END_BITMAP+1);
	nandPic_saveSequecePage( (sblk<<PAGE_PER_BLK_BIT),
							  (eblk<<PAGE_PER_BLK_BIT)-1,
							  (char *)(ptpageBitmap), 
							  &page);
						 	 
	gu32PicSaveBitmap = page;
	
	/*--------------------------------------------------------------------*/
						 	 
	return 0;
}

/*----------------------------------
* ����: nandPic_saveBitmap
* ����: ����bitmap
* ����: pagebuf
* ����: 0:�ɹ�    -1:ʧ��
*-----------------------------------*/
U32 nandPic_saveBitmap(char *pagebuf)
{
	int page;
	U32 ret, sblk,eblk;
	tSEQUECE_PAGE	*ptseq_page;
	//hyUsbPrintf("save bitmap  = %d \r\n",gu8PicNeedSaveBitmap);
	if(gu8PicNeedSaveBitmap == 0)
	{
		return 0;
	}
	
	ptseq_page	= (tSEQUECE_PAGE *)pagebuf;

	/* �ش� fileentry���ش� fileEntry 	*/
	/* ���һ��������Է����ܴ�nandfile entry��ʱ�򣬸ı�bitmap*/
	
	hyhwNandFlash_DataRead_Virt((S8*)ptseq_page, gu32PicSaveBitmap, 0, NAND_PAGE_SIZE);
	
	memcpy( ptseq_page->data, (char *)gu32PicBitmap, NAND_BITMAP_WORDS *sizeof(U32));
	
	ptseq_page->tag1 = NAND_SEQUECE_PAGE_TAG ;
	ptseq_page->tag2 = NAND_SEQUECE_PAGE_TAG ;
	ptseq_page->crc16 = glbVariable_CalculateCRC((U8*)ptseq_page,NAND_PAGE_DATASIZE);
	
	sblk = PIC_BLOCK_START_BITMAP;
	eblk = (PIC_BLOCK_END_BITMAP+1);
	
	/* ע������ָ��Ҫ�õ�ַ����������תһ�� */
	page	= gu32PicSaveBitmap;
//hyUsbPrintf("save bitmap ss = %d  ee = %d  page == %d \r\n",sblk,eblk,page);
	ret  = nandPic_saveSequecePage( (sblk<<PAGE_PER_BLK_BIT),
								     (eblk<<PAGE_PER_BLK_BIT)-1,
						 		     (char *)(ptseq_page), 
						 		     &page);
	if(-1 == ret)
	{
		//hyUsbPrintf("nandFile_saveBitmap fail  \r\n");
	}				 	
						 	
	gu32PicSaveBitmap = page ;
	
	gu8PicNeedSaveBitmap = 0;
//hyUsbPrintf("save bitmap  page == %d \r\n",page);	
	return ret;						 	 
}


/*----------------------------------
* ����: nandPic_saveFile1
* ����: �����һ���ļ�
* ����: pagebuf
* ����: 0:�ɹ�    -1:ʧ��
*-----------------------------------*/
U32 nandPic_saveFile1(char *pagebuf)
{
	int		page;
	U32     ret, sblk,eblk;
	tPICFILE     *pFile;
	tSEQUECE_PAGE	*ptseq_page;
	
	ptseq_page	= (tSEQUECE_PAGE *)pagebuf;
	
	hyhwNandFlash_DataRead_Virt((S8*)ptseq_page, gu32PicSaveFile[0], 0, NAND_PAGE_SIZE);
	
	pFile = &gtPicFile[0];
	
	/* ������Ϣ*/
	memcpy( ptseq_page->data, (char *)pFile, sizeof(tPICFILE));
	ptseq_page->tag1 = NAND_SEQUECE_PAGE_TAG ;
	ptseq_page->tag2 = NAND_SEQUECE_PAGE_TAG ;
	ptseq_page->crc16 = glbVariable_CalculateCRC((U8*)ptseq_page,NAND_PAGE_DATASIZE);
	
	sblk = PIC_FILE1_START;
	eblk = (PIC_FILE1_END+1);
	
	/* ע������ָ��Ҫ�õ�ַ����������תһ�� */
	page	= gu32PicSaveFile[0];
	ret = nandPic_saveSequecePage( (sblk<<PAGE_PER_BLK_BIT),
								    (eblk<<PAGE_PER_BLK_BIT)-1,
							 	    (char *)(ptseq_page), 
							 	    &page);
	if(-1 == ret)
	{
		//hyUsbPrintf("nandFile_saveFileEntry  fail  \r\n");
	}
	
	gu32PicSaveFile[0] = page;	
	
	return ret;			 	 
}

/*----------------------------------
* ����: nandPic_saveFile2
* ����: �����һ���ļ�
* ����: pagebuf
* ����: 0:�ɹ�    -1:ʧ��
*-----------------------------------*/
U32 nandPic_saveFile2(char *pagebuf)
{
	int		page;
	U32     ret, sblk,eblk;
	tPICFILE     *pFile;
	tSEQUECE_PAGE	*ptseq_page;
	
	ptseq_page	= (tSEQUECE_PAGE *)pagebuf;
	
	hyhwNandFlash_DataRead_Virt((S8*)ptseq_page, gu32PicSaveFile[1], 0, NAND_PAGE_SIZE);
	
	pFile = &gtPicFile[1];
	
	/* ������Ϣ*/
	memcpy( ptseq_page->data, (char *)pFile, sizeof(tPICFILE));
	ptseq_page->tag1 = NAND_SEQUECE_PAGE_TAG ;
	ptseq_page->tag2 = NAND_SEQUECE_PAGE_TAG ;
	ptseq_page->crc16 = glbVariable_CalculateCRC((U8*)ptseq_page,NAND_PAGE_DATASIZE);
	
	sblk = PIC_FILE2_START;
	eblk = (PIC_FILE2_END+1);
	
	/* ע������ָ��Ҫ�õ�ַ����������תһ�� */
	page	= gu32PicSaveFile[1];
	ret = nandPic_saveSequecePage( (sblk<<PAGE_PER_BLK_BIT),
								    (eblk<<PAGE_PER_BLK_BIT)-1,
							 	    (char *)(ptseq_page), 
							 	    &page);
	if(-1 == ret)
	{
		//hyUsbPrintf("nandFile_saveFileEntry  fail  \r\n");
	}
	
	gu32PicSaveFile[1] = page;	
	
	return ret;			 	 
}

/*----------------------------------
* ����: nandPic_checkBitmap
* ����: ���bitmap����
* ����: 
* ����: 0:����   -1:�쳣
*-----------------------------------*/
int nandPic_checkBitmap()
{
	int 		i,j,idx,sblk,eblk;
	int			blk,used,ret=0;
	tPICFILE 	*pFile;
	
	sblk = BLOCK_PIC_START;
	eblk = BLOCK_PIC_END;
	
	for(i = sblk; i <= eblk; i++)
	{
	//hyUsbPrintf("cur block id = %d \r\n", i);
		if(!PIC_IS_BLOCK_FREE(i))//��ռ��
		{
			used = 0;
			
			//����Ƿ�ȷʵ���ļ�ռ��
			for(idx = 0; idx < 2; idx++)
			{
				pFile = &gtPicFile[idx];
				if(pFile->totalPic == 0)
				{
					continue;
				}
				for(j = 0; j < PIC_MAX_BLOCKS_PER_FILE; j++)
				{
					blk = (pFile->blk[j]&0x7FFF);
					hyUsbPrintf("file id = %d  block = %d \r\n", idx, blk);
					if(blk == i)//�����ļ�ռ��
					{
						used = 1;//���
						break;
					}
				}
			}
			
			hyUsbPrintf("block used = %d \r\n",used);
			if(used == 0)//û�б��ļ�ռ��
			{
				PIC_SET_BLOCK_FREE(i);
				ret = -1;
			}
			
			hyUsbPrintf("block stat = %d   %d \r\n", i, PIC_IS_BLOCK_FREE(i));
		}
	}
	
	return ret;
}

/*----------------------------------
* ����: nandPic_int
* ����: ��ʼ��
* ����: pBuf:�������ṩ�ڲ�ʹ�õ��ڴ�
* ����: 0:�ɹ�    -1:ʧ��
*-----------------------------------*/
int nandPic_int(void *pBuf)
{
	int 	ret = 0;
	char 	*pagebuf;
	int		page;
	S32		page_start,page_end;
	tSEQUECE_PAGE	*ptseq_page, *ptseq_nextPage;
	
	gpPicBuf = (char *)pBuf;
	pagebuf  = gpPicBuf;
	
	gu8PicNeedSaveBitmap = 1;
	
	ptseq_page		= (tSEQUECE_PAGE *)pagebuf;
	ptseq_nextPage	= (tSEQUECE_PAGE *)(pagebuf + NAND_PAGE_SIZE);
	
	//bitmap
	//hyUsbPrintf("bitmap blk = %d \r\n",PIC_BLOCK_START_BITMAP,PIC_BLOCK_END_BITMAP);
	page_start	= (PIC_BLOCK_START_BITMAP<< PAGE_PER_BLK_BIT);
	page_end	= ((PIC_BLOCK_END_BITMAP +1) << PAGE_PER_BLK_BIT)-1;
	//hyUsbPrintf("init start = %d end = %d \r\n",page_start,page_end);
	if(nandPic_getNewestSequecePage(page_start, page_end, pagebuf, &page) == -1)
	{
	//hyUsbPrintf("init get bitmap seq fail \r\n");
		ret = nandPic_format(pagebuf);
	}
	else
	{
		gu32PicSaveBitmap = page;
		memcpy((char *)gu32PicBitmap, pagebuf, sizeof(U32)*NAND_BITMAP_WORDS);
		
		//���ҵ�һ���ļ�
		page_start	= (PIC_FILE1_START<< PAGE_PER_BLK_BIT);
		page_end	= ((PIC_FILE1_END +1) << PAGE_PER_BLK_BIT)-1;
		if(nandPic_getNewestSequecePage(page_start, page_end, pagebuf, &page) == -1)
		{
		//hyUsbPrintf("init get file1 seq fail \r\n");
			/* û���ҵ���Ч��file entry��û���ļ������ */
			gu32PicSaveFile[0] = page_start - 1;
			memset((char *)&gtPicFile[0],0xff, sizeof(tPICFILE));
			nandPic_saveFile1(pagebuf);
			//��saveFileEntryʱ���ܻ����bitmap
			nandPic_saveBitmap(pagebuf);
		}
		else
		{
			gu32PicSaveFile[0] = page;
			/* �ҵ�file��copy */
			memcpy((char *)&gtPicFile[0], pagebuf, sizeof(tPICFILE));
		}
		
		//�ҵڶ����ļ�
		page_start	= (PIC_FILE2_START<< PAGE_PER_BLK_BIT);
		page_end	= ((PIC_FILE2_END +1) << PAGE_PER_BLK_BIT)-1;
		if(nandPic_getNewestSequecePage(page_start, page_end, pagebuf, &page) == -1)
		{
		//hyUsbPrintf("init get file2 seq fail \r\n");
			/* û���ҵ���Ч��file entry��û���ļ������ */
			gu32PicSaveFile[1] = page_start - 1;
			memset((char *)(&gtPicFile[1]),0xff, sizeof(tPICFILE));
			nandPic_saveFile1(pagebuf);
			//��saveFileEntryʱ���ܻ����bitmap
			nandPic_saveBitmap(pagebuf);
		}
		else
		{
			gu32PicSaveFile[1] = page;
			/* �ҵ�fileEntry��copy */
			memcpy((char *)(&gtPicFile[1]), pagebuf, sizeof(tPICFILE));
		}
	}
	
	gu8PicNeedSaveBitmap = 0;
	
	//���bitmap
	if(-1 == nandPic_checkBitmap())
	{
		gu8PicNeedSaveBitmap = 1;
		nandPic_saveBitmap(pagebuf);
	}
	
	return ret;
}

/*----------------------------------
* ����: nandPic_findFreeBlock
* ����: ��bitmap�в��ҳ���һ���յ�block��ʹ��. ���ز��ҵ���block.
* ����: none
* ����: �ɹ���blk num  ʧ��  -1
*-----------------------------------*/
int nandPic_findFreeBlock()
{
	int i,findBlock=-1;
	int sblk,eblk;
	
	sblk = BLOCK_PIC_START;
	eblk = BLOCK_PIC_END;
	//hyUsbPrintf("find blk  sss= %d  eee = %d \r\n",sblk,eblk);
	
	for(i = sblk; i <= eblk; i++)
	{
		if(PIC_IS_BLOCK_FREE(i))
		{
			//hyUsbPrintf("find block == %d \r\n",i);
			if(hyhwNandFlash_Erase_Virt(i) == 0)
			{
				findBlock =i;
				PIC_SET_BLOCK_USED(i);
				gu8PicNeedSaveBitmap = 1;
				break;
			}
			else
			{
				//hyUsbPrintf("find block erase fail = %d \r\n",i);
			}
		}
		else
		{
			//hyUsbPrintf("find block is used = %d \r\n",i);
		}
	}
	
	return findBlock;
}

/*-----------------------------------------------------------
*����:	nandPic_ReadAbs
*����:	��ȡָ���ļ�������ժҪ��Ϣ
*������	
*����:	�ɹ���0   ʧ�ܣ�-1
*------------------------------------------------------------*/
int nandPic_ReadAbs(tPICFILE *pFile, char *pBuf)
{
	int ret = 0;
	int i,blk,page;
	
	for(i = 0; i < PIC_ABS_BLOCKS_PER_FILE; i++)
	{
		blk = pFile->blk[i];
		if(blk == 0xFFFF)
		{
			break;
		}
		
		blk &= 0x7FFF;
		page = (blk<<PAGE_PER_BLK_BIT) + pFile->pageOffset[i];
		if(-1 == hyhwNandFlash_DataRead_Virt(pBuf, page, 0, NAND_PAGE_SIZE))
		{
			ret = -1;
			break;
		}
		pBuf += NAND_PAGE_SIZE;
	}
	
	return ret;
}


/*-----------------------------------------------------------
*����:	nandPic_WriteAbs
*����:	дժҪ��Ϣ
*������	
*����:	�ɹ���0   ʧ�ܣ�-1
*------------------------------------------------------------*/
int nandPic_WriteAbs(int fileIdx, char *pBuf, U16 idx)
{
	int i,ret = 0;
	int blk,needNewBlk;
	int page,page_start,page_end;
	tPICFILE *pFile;
	tPICABS	 *pAbs;
	
	pFile = &gtPicFile[fileIdx];
	
	blk = pFile->blk[idx];
//hyUsbPrintf("write abs blk = %x \r\n",blk);
	blk &= 0x7FFF;
	
	page_start	= (blk<<PAGE_PER_BLK_BIT);
	page_end	= ((blk+1)<<PAGE_PER_BLK_BIT)-1;
	page		= page_start + pFile->pageOffset[idx];
	
	needNewBlk = 0;
	do
	{
		if(needNewBlk)
		{
			blk = nandPic_findFreeBlock();
			if(blk == -1)
			{
				return -1;
			}
			
			pFile->blk[idx] = blk;
			pFile->pageOffset[idx] = 0;
			
			page_start	= (blk<<PAGE_PER_BLK_BIT);
			page_end	= ((blk+1)<<PAGE_PER_BLK_BIT)-1;
			page		= page_start + pFile->pageOffset[idx];
			
			needNewBlk = 0;
		}
		else
		{
			page++;
			if(page > page_end)
			{
				/* �����ˣ����´�ǰ��д�� */
				page = page_start;
				if(hyhwNandFlash_Erase_Virt(blk) == HY_ERROR)
				{
					//PIC_SET_BLOCK_USED(blk);
					//gu8PicNeedSaveBitmap = 1;
					needNewBlk = 1;
					continue;
				}
			}
		}
		
		if(hyhwNandFlash_PageWrite_Virt(pBuf,page,0,NAND_PAGE_SIZE) == 0)
		{
			needNewBlk = 0;
		}
		else
		{
			//PIC_SET_BLOCK_USED(blk);
			needNewBlk = 1;
		}
		
	}while(needNewBlk);
	
	pFile->pageOffset[idx] = page - page_start;
	
	pAbs = (tPICABS *)pBuf;
	for(i = PIC_ABS_NUM_PER_BLOCK-1; i >= 0; i--)
	{
		if(pAbs[i].status != 0)
		{
			break;
		}
	}
	
	//ȫ����Ч����
	if(i <= 0)
	{
		pFile->blk[idx] = blk|0x8000;
		//hyUsbPrintf("write abs blk all invalid= %x \r\n",pFile->blk[idx]);
	}
	
	if(fileIdx == 0)
	{
		nandPic_saveFile1(pBuf);
	}
	else
	{
		nandPic_saveFile2(pBuf);
	}
	nandPic_saveBitmap(pBuf);
	
	
	return ret;
}


/*-----------------------------------------------------------
*����:	nandPic_WritePic
*����:	дͼƬ����
*������	
*����:	�ɹ���0   ʧ�ܣ�-1
*------------------------------------------------------------*/
int nandPic_WritePic(int fileIdx, char *pBuf, U8 *pCode, U8 *pData, U16 size)
{
	int 		i,ret = 0;
	int			blk,blkIdx,page,needNewBlk;
	int			pageOffset,absIdx;
	int			page_start, page_end;
	tPICFILE 	*pFile;
	tPICABS		*pAbs;
	U8  		*pOldBuf = NULL;
	
	pFile = &gtPicFile[fileIdx];
	
	pageOffset = ((pFile->totalPic&0x7FFF)%PIC_PER_BLOCK)*32;
	blkIdx = (pFile->totalPic&0x7FFF)/PIC_PER_BLOCK;
	blkIdx+= PIC_ABS_BLOCKS_PER_FILE;
	blk    = pFile->blk[blkIdx];
	
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
			blk = nandPic_findFreeBlock();
			if(blk == -1)
			{
			//hyUsbPrintf("find block fail \r\n");
				return -1;
			}
			
			pFile->blk[blkIdx] = blk;
			
			page = (blk<<PAGE_PER_BLK_BIT) + pageOffset;
			
			needNewBlk = 0;
		}
		
		if(pOldBuf && hyhwNandFlash_PageWrite_Virt(pOldBuf, page, 0,PIC_PER_SIZE) != HY_OK)
		{
			needNewBlk = 1;
			//PIC_SET_BLOCK_USED(blk);
			continue;
		}
		else if(pOldBuf)
		{
			page += 32;
		}
		
		needNewBlk = 1;
		
		//д�����ļ�
		if(hyhwNandFlash_PageWrite_Virt(pData, page, 0,PIC_PER_SIZE) == HY_OK)
		{
		//hyUsbPrintf("write pic blk = %d  page = %d \r\n",blk,page);
			pFile->totalPic++;
			pFile->validPic++;
			needNewBlk = 0;
		}
		else if(page > 0 && pOldBuf == NULL)
		{
			page = (blk <<PAGE_PER_BLK_BIT);
			
			pOldBuf = pBuf;
			hyhwNandFlash_DataRead_Virt(pOldBuf, page, 0, PIC_PER_SIZE);
		}
		else
		{
			//PIC_SET_BLOCK_USED(blk);
		}
		
	}while(needNewBlk);
	
	//дժҪ
	blkIdx		= ((pFile->totalPic&0x7FFF)-1)/PIC_ABS_NUM_PER_BLOCK;
	pageOffset	= ((pFile->totalPic&0x7FFF)-1)%PIC_ABS_NUM_PER_BLOCK;
	
	blk			= pFile->blk[blkIdx];
	
	needNewBlk = 0;
	if(blk == 0xFFFF)
	{
		needNewBlk = 1;
	}
	else
	{
		blk &= 0x7FFF;
		
		page_start	= (blk<<PAGE_PER_BLK_BIT);
		page_end	= ((blk+1)<<PAGE_PER_BLK_BIT)-1;
		page		= page_start + pFile->pageOffset[blkIdx];
	}
	
	pAbs = (tPICABS *)pBuf;
	memset(pBuf,0xFF,NAND_PAGE_SIZE);
	do
	{
		if(needNewBlk)
		{
			blk = nandPic_findFreeBlock();
			if(blk == -1)
			{
				//hyUsbPrintf("abs find block fail \r\n");
				return -1;
			}
			
			pFile->blk[blkIdx] = blk;
			pFile->pageOffset[blkIdx] = 0;
			
			page_start	= (blk<<PAGE_PER_BLK_BIT);
			page_end	= ((blk+1)<<PAGE_PER_BLK_BIT)-1;
			page		= page_start + pFile->pageOffset[blkIdx];
			
			needNewBlk = 0;
			
			memcpy(pAbs[pageOffset].barcode,pCode,strlen(pCode));
			pAbs[pageOffset].status = 1;
			pAbs[pageOffset].size = size;
			
			
		}
		else
		{
			hyhwNandFlash_DataRead_Virt((S8*)pAbs, page, 0, NAND_PAGE_SIZE);
			memcpy(pAbs[pageOffset].barcode,pCode,strlen(pCode));
			pAbs[pageOffset].status = 1;
			pAbs[pageOffset].size = size;
			
			page++;
			if(page > page_end)
			{
				/* �����ˣ����´�ǰ��д�� */
				page = page_start;
				if(hyhwNandFlash_Erase_Virt(blk) == HY_ERROR)
				{
					//PIC_SET_BLOCK_USED(blk);
					//gu8PicNeedSaveBitmap = 1;
					needNewBlk = 1;
					continue;
				}
			}
		}
		
		//PhoneTrace2(0,"status w %d",pAbs[pageOffset].status);
		
		if(hyhwNandFlash_PageWrite_Virt((S8 *)pAbs,page,0,NAND_PAGE_SIZE) == HY_OK)
		{
			//hyUsbPrintf("write abs blk = %d  page = %d \r\n",blk,page);
			needNewBlk = 0;
		}
		else
		{
			//PIC_SET_BLOCK_USED(blk);
			//gu8PicNeedSaveBitmap = 1;
			needNewBlk = 1;
		}
		
	}while(needNewBlk);
	
	pFile->pageOffset[blkIdx] = page - page_start;
	
	if(fileIdx == 0)
	{
		nandPic_saveFile1(pBuf);
	}
	else
	{
		nandPic_saveFile2(pBuf);
	}
	nandPic_saveBitmap(pBuf);
	
	
	return ret;
}







/*---------------------------------------- API -----------------------------------------*/
/*-----------------------------------------------------------
*����:	PicStore_Format
*����:	��ʽ��
*������	pBuf:�������ṩ�ڴ�
*����:	�ɹ���0   ʧ�ܣ�-1
*------------------------------------------------------------*/
int PicStore_Format()
{
	int ret;
	
	wlock((unsigned)&appLockNandCtr);
	ret = nandPic_format(gpPicBuf);
	//hyUsbPrintf("format ret = %d  %d  %d\r\n",ret,gtPicFile[0].totalPic,gtPicFile[1].totalPic);
	if(gtPicFile[0].totalPic == 0xFFFF || (gtPicFile[0].totalPic&0x7FFF) == MAX_PICS_PER_FILE)
	{
		memset((char *)&gtPicFile[0],0,sizeof(tPICFILE));
		memset((char *)&(gtPicFile[0].blk[0]),0xFF,sizeof(U16)*PIC_MAX_BLOCKS_PER_FILE);
	}
	if(gtPicFile[1].totalPic == 0xFFFF || (gtPicFile[1].totalPic&0x7FFF) == MAX_PICS_PER_FILE)
	{
		memset((char *)&gtPicFile[1],0,sizeof(tPICFILE));
		memset((char *)&(gtPicFile[1].blk[0]),0xFF,sizeof(U16)*PIC_MAX_BLOCKS_PER_FILE);
	}
	
	
	unlock((unsigned)&appLockNandCtr);
	
	return ret;
}

/*-----------------------------------------------------------
*����:	PicStore_Init
*����:	ģ���ʼ��.
*������	pBuf:�������ṩ�ڴ�
        bufsize:pBuf�Ĵ�С
*����:	�ɹ���0   ʧ�ܣ�-1
*------------------------------------------------------------*/
int PicStore_Init(U8 *pBuf, U32 bufsize)
{
	int ret;
	
	if(bufsize < 64*1024)
	{
		return -1;
	}
	
	wlock((unsigned)&appLockNandCtr);
	ret = nandPic_int(pBuf);
	//hyUsbPrintf("init ret = %d  %d  %d\r\n",ret,gtPicFile[0].totalPic,gtPicFile[1].totalPic);
	if(gtPicFile[0].totalPic == 0xFFFF || (gtPicFile[0].totalPic&0x7FFF) == MAX_PICS_PER_FILE)
	{
		memset((char *)&gtPicFile[0],0,sizeof(tPICFILE));
		memset((char *)&(gtPicFile[0].blk[0]),0xFF,sizeof(U16)*PIC_MAX_BLOCKS_PER_FILE);
	}
	if(gtPicFile[1].totalPic == 0xFFFF || (gtPicFile[1].totalPic&0x7FFF) == MAX_PICS_PER_FILE)
	{
		memset((char *)&gtPicFile[1],0,sizeof(tPICFILE));
		memset((char *)&(gtPicFile[1].blk[0]),0xFF,sizeof(U16)*PIC_MAX_BLOCKS_PER_FILE);
	}
	
	
	unlock((unsigned)&appLockNandCtr);
	
	return ret;
}

/*-----------------------------------------------------------
*����:	PicStore_DelFile
*����:	ɾ��ָ���ļ�
*������	idx:�ļ�����  0--1
*����:	�ɹ���0   ʧ�ܣ�-1
*------------------------------------------------------------*/
int PicStore_DelFile(int idx)
{
	int 		i,ret = 0;
	int			blk;
	tPICFILE 	*pFile;
	
	wlock((unsigned)&appLockNandCtr);
	
	pFile = &gtPicFile[idx];
//hyUsbPrintf("del fileidx = %d  total = %d \r\n",idx, pFile->totalPic);	
	if(pFile->totalPic == 0)
	{
		return 0;
	}
	
	for(i = 0; i < PIC_MAX_BLOCKS_PER_FILE; i++)
	{
		blk = pFile->blk[i];
		if(blk !=0xFFFF)
		{
			blk &= 0x7FFF;
			PIC_SET_BLOCK_FREE(blk);
			gu8PicNeedSaveBitmap = 1;
		}
	}
	
	memset((char *)pFile,0,sizeof(tPICFILE));
	memset((char *)&(pFile->blk[0]),0xFF,sizeof(U16)*PIC_MAX_BLOCKS_PER_FILE);
	
	if(idx == 0)
	{
		nandPic_saveFile1(gpPicBuf);
	}
	else
	{
		nandPic_saveFile2(gpPicBuf);
	}
	nandPic_saveBitmap(gpPicBuf);
	
	unlock((unsigned)&appLockNandCtr);
	
	return ret;
}

/*-----------------------------------------------------------
*����:	PicStore_Write
*����:	д��ͼƬ��Ϣ
*������.
*����:	�ɹ���0   ʧ�ܣ�-1
*------------------------------------------------------------*/
int PicStore_Write(U8 *pCode, U8 *pData, U16 size)
{
	int 		ret = -1;
	int 		i,j,isEnd = 0;
	U16			absIdx;
	tPICFILE	*pFile;
	tPICABS		*pAbs;
	
	wlock((unsigned)&appLockNandCtr);
hyUsbPrintf("write code = %s \r\n",pCode);	
	pAbs = (tPICABS *)gpPicBuf;
	
	//д��������ǰ�Ȳ����Ƿ�����ͬ����
	for(i = 0; i < 2 && isEnd == 0; i++)
	{
		pFile = &gtPicFile[i];
		if(pFile->totalPic == 0)
		{
			continue;
		}
		if(-1 == nandPic_ReadAbs(pFile, (char *)pAbs))
		{
			unlock((unsigned)&appLockNandCtr);
			return -1;
		}
		
		for(j = 0; j < (pFile->totalPic&0x7FFF); j++)
		{
			if(j % PIC_ABS_NUM_PER_BLOCK == 0)
			{
				absIdx = j/PIC_ABS_NUM_PER_BLOCK;
				if(pFile->blk[absIdx]&0x8000)
				{
					j+=PIC_ABS_NUM_PER_BLOCK;
					j--;
					continue;
				}
			}
			//������Ч  �ҵ�����ͬ
			//hyUsbPrintf("write count = %d status = %d  code = %s \r\n",j,pAbs[j].status,pAbs[j].barcode);
			if(pAbs[j].status == 1 && memcmp(pAbs[j].barcode, pCode, strlen(pCode)) == 0)
			{
				pAbs[j].status = 0;//��Ϊ��Ч
				pFile->validPic--;
				absIdx = j/PIC_ABS_NUM_PER_BLOCK;
				
				hyUsbPrintf("write delete old pic = %d   %d \r\n",j,absIdx);
				if(-1 == nandPic_WriteAbs(i, (char *)&pAbs[absIdx*PIC_ABS_NUM_PER_BLOCK], absIdx))
				{
					unlock((unsigned)&appLockNandCtr);
					return -1;
				}
				//��Ϊÿ��д��ǰ���ж�,�������ֻ�����һ����ͬ����
				isEnd = 1;
				ret   = 0;
				break;
			}
		}
	}
	
	if(gtPicFile[0].totalPic >= MAX_PICS_PER_FILE)//��һ���ļ����� ��ڶ����ļ�д
	{
		ret = nandPic_WritePic(1, gpPicBuf, pCode, pData, size);
		hyUsbPrintf("000 write total000 = %d  total111 = %d  ret = %d \r\n",gtPicFile[0].totalPic,gtPicFile[1].totalPic,ret);
		if(ret == -1 && gtPicFile[1].totalPic >= MAX_PICS_PER_FILE)//�ڶ����ļ�����ɾ����һ���ļ�
		{
			gtPicFile[1].totalPic |= 0x8000;//���λ��1  ��ʾ������д��
			hyUsbPrintf("del file000 . file000 total = %d  file111 total = %d \r\n",gtPicFile[0].totalPic,gtPicFile[1].totalPic);
			PicStore_DelFile(0);
			
			ret = nandPic_WritePic(0, gpPicBuf, pCode, pData, size);
		}
	}
	else
	{
		ret = nandPic_WritePic(0, gpPicBuf, pCode, pData, size);
		hyUsbPrintf("111 write total000 = %d  total111 = %d  ret = %d \r\n",gtPicFile[0].totalPic,gtPicFile[1].totalPic,ret);
		if(ret == -1 && gtPicFile[0].totalPic >= MAX_PICS_PER_FILE)//��һ���ļ�д��ʧ�ܻ�д����ɾ���ڶ����ļ�
		{
			gtPicFile[0].totalPic |= 0x8000;//���λ��1  ��ʾ������д��
			
			hyUsbPrintf("del file111 . file000 total = %d  file111 total = %d \r\n",gtPicFile[0].totalPic,gtPicFile[1].totalPic);
			PicStore_DelFile(1);
			
			ret = nandPic_WritePic(1, gpPicBuf, pCode, pData, size);
		}
	}
	
	
	unlock((unsigned)&appLockNandCtr);
	
	return ret;
}


/*-----------------------------------------------------------
*����:	PicStore_Read
*����:	����ͼƬ����
*������.
*����:	�ɹ���0   ʧ�ܣ�-1     �������ṩbuf���Ȳ���: -2
*------------------------------------------------------------*/
int PicStore_Read(U8 *pCode, U8 *pData, U16 bufsize)
{
	int 		ret = -1;
	int			absIdx,i,j,isEnd=0;
	int			blk,page;
	tPICFILE	*pFile;
	tPICABS		*pAbs;
	
	wlock((unsigned)&appLockNandCtr);
hyUsbPrintf("read code = %s \r\n",pCode);
	pAbs = (tPICABS *)gpPicBuf;
	for(i = 0; i < 2 && isEnd == 0; i++)
	{
		pFile = &gtPicFile[i];
		if(pFile->totalPic == 0 || pFile->validPic == 0)
		{
			continue;
		}
		if(-1 == nandPic_ReadAbs(pFile, (char *)pAbs))
		{
			unlock((unsigned)&appLockNandCtr);
			hyUsbPrintf("ReadAbs  fail \r\n");
			return -1;
		}
		
		hyUsbPrintf("read total = %d \r\n",pFile->totalPic);
		for(j = 0; j < (pFile->totalPic&0x7FFF); j++)
		{
			if(j % PIC_ABS_NUM_PER_BLOCK == 0)
			{
				absIdx = j/PIC_ABS_NUM_PER_BLOCK;
				if(pFile->blk[absIdx]&0x8000)
				{
					j+=PIC_ABS_NUM_PER_BLOCK;
					j--;
					hyUsbPrintf("read block all invalid = %d    %x\r\n",j,pFile->blk[absIdx]);
					continue;
				}
			}
			
			//������Ч  �ҵ�����ͬ
			hyUsbPrintf("j = %d read status = %d \r\n",j);
			if(pAbs[j].status == 1 && memcmp(pAbs[j].barcode, pCode,strlen(pCode)) == 0)
			{
				blk = j/PIC_PER_BLOCK;
				blk+= PIC_ABS_BLOCKS_PER_FILE;
				blk = pFile->blk[blk];
				
				page = (blk<<PAGE_PER_BLK_BIT);
				
				if(j%2)
				{
					page += 32;
				}
				
				bufsize = (pAbs[j].size > bufsize)? bufsize:pAbs[j].size;
				if(bufsize < pAbs[j].size)
				{
					unlock((unsigned)&appLockNandCtr);
					return -2;
				}
				hyhwNandFlash_DataRead_Virt((S8*)pData, page, 0, pAbs[j].size);
				hyUsbPrintf("read get size = %d\r\n",pAbs[j].size);
				isEnd = 1;
				ret   = pAbs[j].size;
				
				PhoneTrace2(0,"rel %d , %d",pAbs[j].size , ret);
				break;
			}
		}
	}
	
	unlock((unsigned)&appLockNandCtr);
	
	return ret;
}

/*-----------------------------------------------------------
*����:	PicStore_GetPic
*����:	��ȡͼƬ����, �����Ѿ��ϴ���Ҳ���Եõ�  ������PicStore_Read��ͬ�ĵط�
*������.
*����:	�ɹ���> 0   ʧ�ܣ�-1     �������ṩbuf���Ȳ���: -2
*------------------------------------------------------------*/
int PicStore_GetPic(U8 *pCode, U8 *pData, U16 bufsize)
{
	int 		ret = -1;
	int			absIdx,i,j,isEnd=0;
	int			blk,page;
	tPICFILE	*pFile;
	tPICABS		*pAbs;
	
	wlock((unsigned)&appLockNandCtr);
hyUsbPrintf("read code = %s \r\n",pCode);
	pAbs = (tPICABS *)gpPicBuf;
	for(i = 0; i < 2 && isEnd == 0; i++)
	{
		pFile = &gtPicFile[i];
		if(pFile->totalPic == 0)
		{
			continue;
		}
		if(-1 == nandPic_ReadAbs(pFile, (char *)pAbs))
		{
			unlock((unsigned)&appLockNandCtr);
			hyUsbPrintf("ReadAbs  fail \r\n");
			return -1;
		}
		
		hyUsbPrintf("read total = %d \r\n",pFile->totalPic);
		for(j = 0; j < (pFile->totalPic&0x7FFF); j++)
		{
			if(j % PIC_ABS_NUM_PER_BLOCK == 0)
			{
				absIdx = j/PIC_ABS_NUM_PER_BLOCK;
				if(pFile->blk[absIdx]&0x8000)
				{
					j+=PIC_ABS_NUM_PER_BLOCK;
					j--;
					hyUsbPrintf("read block all invalid = %d    %x\r\n",j,pFile->blk[absIdx]);
					continue;
				}
			}
			
			//������Ч  �ҵ�����ͬ
			hyUsbPrintf("j = %d read status = %d \r\n",j);
			if(memcmp(pAbs[j].barcode, pCode,strlen(pCode)) == 0)
			{
				blk = j/PIC_PER_BLOCK;
				blk+= PIC_ABS_BLOCKS_PER_FILE;
				blk = pFile->blk[blk];
				
				page = (blk<<PAGE_PER_BLK_BIT);
				
				if(j%2)
				{
					page += 32;
				}
				
				bufsize = (pAbs[j].size > bufsize)? bufsize:pAbs[j].size;
				if(bufsize < pAbs[j].size)
				{
					unlock((unsigned)&appLockNandCtr);
					return -2;
				}
				hyhwNandFlash_DataRead_Virt((S8*)pData, page, 0, pAbs[j].size);
				hyUsbPrintf("read get size = %d\r\n",pAbs[j].size);
				isEnd = 1;
				ret   = pAbs[j].size;
				
				break;
			}
		}
	}
	
	unlock((unsigned)&appLockNandCtr);
	
	return ret;
}

/*-----------------------------------------------------------
*����:	PicStore_IfExist
*����:	���ͼƬ�Ƿ����
*������.
*����:	���ڣ�0   �����ڣ�-1
*------------------------------------------------------------*/
int PicStore_IfExist(U8 *pCode)
{
	int 		ret = -1;
	int			absIdx,i,j,isEnd=0;
	int			blk,page;
	tPICFILE	*pFile;
	tPICABS		*pAbs;
	
	wlock((unsigned)&appLockNandCtr);

	pAbs = (tPICABS *)gpPicBuf;
	for(i = 0; i < 2 && isEnd == 0; i++)
	{
		pFile = &gtPicFile[i];
		if(pFile->totalPic == 0 || pFile->validPic == 0)
		{
			continue;
		}
		if(-1 == nandPic_ReadAbs(pFile, (char *)pAbs))
		{
			unlock((unsigned)&appLockNandCtr);
			return -1;
		}
		

		for(j = 0; j < (pFile->totalPic&0x7FFF); j++)
		{
			if(j % PIC_ABS_NUM_PER_BLOCK == 0)
			{
				absIdx = j/PIC_ABS_NUM_PER_BLOCK;
				if(pFile->blk[absIdx]&0x8000)
				{
					j+=PIC_ABS_NUM_PER_BLOCK;
					j--;

					continue;
				}
			}
			
			//������Ч  �ҵ�����ͬ
			if(pAbs[j].status == 1 && memcmp(pAbs[j].barcode, pCode,strlen(pCode)) == 0)
			{
				isEnd = 1;
				ret   = 0;
				break;
			}
		}
	}
	
	unlock((unsigned)&appLockNandCtr);
	
	return ret;
}

/*-----------------------------------------------------------
*����:	PicStore_Delete
*����:	ɾ��ͼƬ����
*������.
*����:	�ɹ���0   ʧ�ܣ�-1
*------------------------------------------------------------*/
int PicStore_Delete(U8 *pCode)
{
	int 		ret = -1;
	int 		i,j,isEnd = 0;
	U16			absIdx;
	tPICFILE	*pFile;
	tPICABS		*pAbs;
	
	wlock((unsigned)&appLockNandCtr);
	
	pAbs = (tPICABS *)gpPicBuf;
//hyUsbPrintf("delete code = %s \r\n",pCode);
	
	for(i = 0; i < 2 && isEnd == 0; i++)
	{
		pFile = &gtPicFile[i];
		if(pFile->totalPic == 0 || pFile->validPic == 0)
		{
			continue;
		}
		if(-1 == nandPic_ReadAbs(pFile, (char *)pAbs))
		{
			unlock((unsigned)&appLockNandCtr);
			return -1;
		}
				
//		hyUsbPrintf("delete total = %d \r\n",pFile->totalPic);
		for(j = 0; j < (pFile->totalPic&0x7FFF); j++)
		{
			if(j % PIC_ABS_NUM_PER_BLOCK == 0)
			{
				absIdx = j/PIC_ABS_NUM_PER_BLOCK;
				if(pFile->blk[absIdx]&0x8000)
				{
					j+=PIC_ABS_NUM_PER_BLOCK;
					j--;
					continue;
				}
			}
			
			//������Ч  �ҵ�����ͬ
//			hyUsbPrintf("delete status = %d  code = %s \r\n",pAbs[j].status,pAbs[j].barcode);
			if(pAbs[j].status == 1 && memcmp(pAbs[j].barcode, pCode, strlen(pCode)) == 0)
			{
				pAbs[j].status = 0;//��Ϊ��Ч
				pFile->validPic--;
				
				absIdx = j/PIC_ABS_NUM_PER_BLOCK;
				
				if(-1 == nandPic_WriteAbs(i, (char *)&pAbs[absIdx*PIC_ABS_NUM_PER_BLOCK], absIdx))
				{
					unlock((unsigned)&appLockNandCtr);
					return -1;
				}

				isEnd = 1;
				ret   = 0;
				break;
			}
		}
	}
	
	unlock((unsigned)&appLockNandCtr);
	
	return ret;
}

//////////////test
/*
void test_pic(Keyb_Code_e eKeyCode)
{
	int ret=0;
	U32 i,total;
	static U32 count = 0;
	U8 buf[16];
	

	switch(eKeyCode)
	{
	case Keyb_Code_1://д
		total = 128;
		for(i = 0; i < total; i++)
		{
			sprintf(buf,"%013d",count);
			PicStore_Write(buf, buf, strlen(buf));
			count++;
		}
		break;
	case Keyb_Code_2://��
		//for(i = 0; ret == 0; i++)
		{
			strcpy(buf,"9999999999999");
			ret = PicStore_Read(buf, buf, strlen(buf));
		}
		break;
	case Keyb_Code_3://��
		for(i = 0; gtPicFile[0].validPic>0 || gtPicFile[1].validPic>0; i++)
		{
			sprintf(buf,"%013d",i);
			ret = PicStore_Delete(buf);
		}
		break;
	case Keyb_Code_Del://ɾ
		PicStore_DelFile(0);
		PicStore_DelFile(1);
		break;
	case Keyb_Code_Jing:
		PicStore_Init(hsaSdram_GetUpgradeBuf());
		break;
	case Keyb_Code_W:
		//hyUsbPrintf("format \r\n");
		PicStore_Format();
		break;
	default:
		break;
	}
	
	
	
	
}*/