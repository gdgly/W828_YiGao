/*
*include standard files
*/
#include "ospriority.h"
#include "appControlProcess.h"

int touchScreen_ThreadMethod(void)
{
     hyhwAk4183_Initial();
     
     ts_main();
}

/*-----------------------------------------------------------------------------
* ����:	touchscreen_create_thread(void);
* ����:	����������������
* ����:	none
* ����: none
*----------------------------------------------------------------------------*/
void touchscreen_create_thread(void)
{
    int pid;
    pid = createp( touchScreen_ThreadMethod, TOUCH_NAME, TOUCH_SIZE, TOUCH_PRIOPITY, NULL,NULL,NULL); // �������ȼ��� 6 --> 12
	pstart( pid, (void *)touchScreen_ThreadMethod);
}
