#include "SysSetting.h"


/*---------------------------------------------
*����:Set_Format
*����:��ʽ��
*����:
*����:
*-----------------------------------------------*/
void Set_Format(void *pDesk)
{
	if(DLG_CONFIRM_EN == Com_SpcDlgDeskbox("�豸���ļ��������,�Ƿ����?",0, pDesk, 3, NULL, NULL, DLG_ALLWAYS_SHOW))
	{
		AbstractFileSystem_Format("C");
		
		Com_SpcDlgDeskbox("��ʽ�����!",0,pDesk,1,NULL,NULL,100);
	}
}
