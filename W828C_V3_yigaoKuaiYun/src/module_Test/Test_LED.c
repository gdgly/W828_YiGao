#include "Test_MainList.h"



/*=============================================LED测试界面===================================================*/


/*-------------------------------------------------------------
* 函数:TestFun_LED
* 功能:LED测试界面
* 参数:
* 返回:
--------------------------------------------------------------*/
int TestFun_LED(void *pDeskFather)
{
	
	Led_ctrl(1);
	syssleep(50);
	
	Led_ctrl(2);
	syssleep(50);
	
	Led_ctrl(4);
	syssleep(50);
	
	Led_ctrl(0);
		
	return 0;
}
