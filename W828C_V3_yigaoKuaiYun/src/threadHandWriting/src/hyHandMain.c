#include "hyTypes.h"
#include "hyHandMain.h"


#define HYHAND_OFFSET	   (32602415)
#define HYHAND_SIZE		   (2144956)

#define IS_VALID_POS(X,Y)  ((X) != -1 || (Y) != -1)


static ISHWR_ENGINE_HANDLE	ghEngine;
static tHY_RECG				*ptHyRecg = NULL;
static U8					gsu8RecogNum;



/**************************
* ����:hyHWRecog_Init
* ����:��ʼ��ʶ���
* ����:none
* ����:0:�ɹ� -1:ʧ��
****************************/
int hyHWRecog_Init()
{
	int		par;
	char    *pModelData;
	
	pModelData = (char *)hsaSdram_HandWriteBuf();
	
	HSA_AccessReservedArea(pModelData, HYHAND_OFFSET, HYHAND_SIZE);
	
	ghEngine = ISHWR_Initialize( pModelData, HYHAND_SIZE, NULL, ISHWR_ADAPT_SIZE );
	
	if(ghEngine == NULL)
	{
		return -1;
	}
	
    par = ISHWR_CHARSET_DEFAULT;
    ISHWR_SetParam( ghEngine, ISHWR_Param_CharSet, &par, sizeof(par) );
	
	gsu8RecogNum = 0;
	
	return 0;
}


/*---------------------------------------------------
 * ���� : hyHWRecog_StartFlag
 * ���� : �ж��Ƿ��Ѿ�������дʶ����߿���ʧ�ܵȵ�
 * ���� : none
 * ���� : 1:�Ѿ�����  0 ��û�п��� 
---------------------------------------------------*/
static int hyHWRecog_StartFlag(void)
{
     if (ptHyRecg == NULL || 0 == ptHyRecg->enable) return 0;
     
     return 1;
}

/*---------------------------------------------------
 * ���� : hyHWRecog_WakeParse
 * ���� : ���ѽ������ֽ���
 * ���� : none
 * ���� : none 
---------------------------------------------------*/
static void hyHWRecog_WakeParse(void)
{
      ptHyRecg->traceStatus = HY_TRACE_PARSE;
      ewakeup(&ptHyRecg->EventHyRcg);
}

/*---------------------------------------------------
 * ���� : hyHWRecog_RecogTrace
 * ���� : ��¼��д��,��ʱû�е�ʱҲ��Ҫ��ʱ�����
 * ���� : none
 * ���� : none 
---------------------------------------------------*/
void hyHWRecog_RecogTrace(int x, int y)
{
	U32 curTick;
	int id;
	int traceStatus;
	
	if(0 == hyHWRecog_StartFlag())
	{
		return; 
	}
	
	traceStatus = ptHyRecg->traceStatus;
	
	//�ڽ����켣ʱ 
	if(HY_TRACE_PARSE == traceStatus || (HY_TRACE_IDEL == traceStatus && !IS_VALID_POS(x,y)))
	{
		return ;
	}
	
	curTick  = rawtime(NULL);

	id = ptHyRecg->index;

	if (id + 5 > HY_REC_BUF_SIZE) 
	{
		//id = 2;
		//��Ҫ���ѽ��̽��н�������
		hyHWRecog_WakeParse();
		
		return ;
	}
	
	//û�д�����ʱ������״̬    
	if (!IS_VALID_POS(x,y))
	{
		int diffTick;

		if(id == 0)
		{
			return;
		}

		diffTick = ABS(curTick - ptHyRecg->lastTick);

		//Ҫô����(-1,-1) Ҫô������
		if(diffTick >= HY_TRACE_TIME_STOP)
		{
			if (ptHyRecg->recBuf[id - 1].x != -1)
			{
				ptHyRecg->recBuf[id].x   = -1;
				ptHyRecg->recBuf[id++].y = -1;
			}

			//��Ҫ���ѽ��̽��н�������
			hyHWRecog_WakeParse();
		} 
		else if (diffTick >= HY_TRACE_TIME_PAUSE)
		{
			if (-1 != ptHyRecg->recBuf[id - 1].x)
			{
				ptHyRecg->recBuf[id].x   = -1;
				ptHyRecg->recBuf[id++].y = -1;
				ptHyRecg->index = id;
			} 
		}

		return;
	}

	//�д�����ʱֻ�ܼ�¼�����
	if(traceStatus == HY_TRACE_IDEL)
	{
		ptHyRecg->traceStatus = HY_TRACE_START;
	}

	ptHyRecg->recBuf[id].x   = x;
	ptHyRecg->recBuf[id++].y = y;
	ptHyRecg->index = id;

	ptHyRecg->lastTick = curTick;
	
	return ;
}

/*---------------------------------------------------
 * ���� : hyHWRecog_GetLetter
 * ���� : �õ�ʶ���ַ�  
 * ���� : pbuf:�����buf
 * ���� : �õ����ַ���  <0 û���ַ�  0����Ҫˢ���ַ� 
---------------------------------------------------*/
int hyHWRecog_GetLetter(char *pbuf)
{
	int len = 0;

	if (0 == hyHWRecog_StartFlag() || 0 == ptHyRecg->outFlag) return -1; 

	if (2 == ptHyRecg->outFlag)
	{
		ptHyRecg->outFlag = 0;
		return 0;
	}  

	memcpy(pbuf,ptHyRecg->outBakBuf,gsu8RecogNum<<1);
//hyUsbPrintf("Recog str = %s   num = %d \r\n", pbuf, gsu8RecogNum);
	len = gsu8RecogNum;
	
	ptHyRecg->outFlag = 0;
	gsu8RecogNum = 0;
	
	return len;
}

/*---------------------------------------------------
 * ���� : hyHWRecog_EnableFlag
 * ���� : ʧ��/ʹ����дʶ��
 * ���� : none
 * ���� : none 
---------------------------------------------------*/
void hyHWRecog_EnableFlag(int type)
{
    ptHyRecg->enable = type;
    
    //�ⲿ�رյ�ʱ�����buf����
    if (type == 0)
    {
    	 ptHyRecg->outFlag = 0;
    	 ptHyRecg->traceStatus = HY_TRACE_IDEL;
    	 ptHyRecg->index = 0; 
    } 
}

/*---------------------------------------------------
 * ���� : hyHWRecog_RecognizeMain
 * ���� : ��дʶ����ѭ������
 * ���� : none
 * ���� : none 
---------------------------------------------------*/
void hyHWRecog_RecognizeMain(void)
{
	int id,i;
	char buf[3];
	U16 uniArray[2];

	ptHyRecg = (tHY_RECG *)malloc(sizeof(tHY_RECG));
	if (NULL == ptHyRecg)
	{
		while(1);
	}

	memset(ptHyRecg,0,sizeof(tHY_RECG));

	while(1)
	{
		ewait(&ptHyRecg->EventHyRcg);
		if(ptHyRecg->index > 5)
		{
			gsu8RecogNum = ISHWR_Recognize( ghEngine, ptHyRecg->recBuf, ptHyRecg->index, ptHyRecg->outBuf, 5 );
			if (gsu8RecogNum > 0)
			{
				for (i=0; i<gsu8RecogNum; i++)
				{
					uniArray[0] = ptHyRecg->outBuf[i];
					uniArray[1] = 0;
					
					Hyc_Unicode2Gbk( (char *)&uniArray, ptHyRecg->outBakBuf+(i<<1), 0);
				}

				ptHyRecg->outFlag = 1;
			}
		}
		else
		{
			ptHyRecg->outFlag = 2; 
		}

		ptHyRecg->index = 0;
		ptHyRecg->traceStatus = HY_TRACE_IDEL;
	}

	free(ptHyRecg);

	return ;
}