
#include "hyTypes.h"
#include "ospriority.h"
#include "std.h"
#include "sys.h"
#include "hyHandProc.h"
#include "appControlProcess.h"



/*-----------------------------------------------------------------------------
* ����:	hyHWRecog_ThreadMethod(void);
* ����:	��дʶ����������
* ����:	none
* ����: none
*----------------------------------------------------------------------------*/
int hyHWRecog_ThreadMethod(void)
{
	HandWriting_InitMemory();
	
	if(-1 == hyHWRecog_Init())
	{
		//hyUsbPrintf("init fail\r\n");
		while(1);
	}
	else
	{
		//hyUsbPrintf("init success\r\n");
	}
	
	hyHWRecog_RecognizeMain();
	
	return 0;
}

/*-----------------------------------------------------------------------------
* ����:	hyHWRecog_CreatThread(void);
* ����:	������дʶ�����
* ����:	none
* ����: none
*----------------------------------------------------------------------------*/
S32  hyHWRecog_CreatThread(void)
{
	
    S32 pid, ret = -1;
    
    pid = ntopid(HANDWRITING_NAME);
    
    if (pid > 0)
    {
        if (PSPROC(pid)->state == P_INACT)
        {
            ret = 0;
        }
        else
        {
            return -1;
        }
    }
    if (ret != 0)
    {
        pid = createp((int (*)())hyHWRecog_ThreadMethod, 
                                HANDWRITING_NAME, 
                                HANDWRITING_SIZE, 
                                HANDWRITING_PRIOPITY, 
                                NULL, NULL, NULL);
    }
    pstart(pid, (void *)hyHWRecog_ThreadMethod);
    
    return ret;
}