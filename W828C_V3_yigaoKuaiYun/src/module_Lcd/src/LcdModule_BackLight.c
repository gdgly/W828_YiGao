/*----------------------�ļ�����--------------------------------
* ��������: 2008/05/28 
* ����    : ���ļ�Ϊ����������һЩ����
*--------------------------------------------------------------*/

/*
*include files
*/
#include "hyTypes.h"
#include "LcdModule_BackLight.h"
#include "glbVariable_base.h"
#include "appControlProcess.h"
#include "grap_api.h"
#include "edit.h"




/*
*define variable
*/
//������ʱ
S32  gu32ScreenPro = -1;

//lcd PWM������Ƽ���
const U8 gu8Lcd_BackLight[5]=
{
    LCD_PWM_GRADE_1,
    LCD_PWM_GRADE_2,
    LCD_PWM_GRADE_3,
    LCD_PWM_GRADE_4,
    LCD_PWM_GRADE_5
};

/*---------------------------------------------------------------------------
 ������BackLight_Get_LevValue
 ���ܣ�
 ��������
 ���أ�
* -----------------------------------------------------------------------------*/
int BackLight_Get_LevValue(U32 lev)
{
    if (lev > 4)
    {
        lev = 3;
    }
    
    return  gu8Lcd_BackLight[lev];
}

/*---------------------------------------------------------------------------
 ������BackLight_UpdataLightTime
 ���ܣ����±���ʱ��
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/ 
void BackLight_UpdataLightTime(U32 t)
{
     gu32ScreenPro = t;
}

/*---------------------------------------------------------------------------
 ������BackLight_CalculateTime
 ���ܣ��Ա����ʱ,��ʱΪ0ʱ��رձ���
 ��������
 ���أ�none
* -----------------------------------------------------------------------------*/ 
void  BackLight_CalculateTime(void)
{
    if (gu32ScreenPro != 0)
    {
        if (--gu32ScreenPro == 0)
		{
			LcdModulePowerOnOff(0);
			hyhwLcd_resetOn();
			gu8EnableRefreshSrc = 0; //������ˢ��			
		}    
    }
}

/*---------------------------------------------------------------------------
 ������BackLight_Light
 ���ܣ��Ƿ�������״̬
 ��������
 ���أ�1:�б��� 0:�ޱ���
* -----------------------------------------------------------------------------*/ 
U32 BackLight_HaveLight(void)
{
    return gu8EnableRefreshSrc;        
}

/*---------------------------------------------------------------------------
 ������BackLight_ShutLight
 ���ܣ��رձ��⣬�õ�ĳЩ����
 ��������
 ���أ���
* -----------------------------------------------------------------------------*/ 
S32 BackLight_ShutLight(void)
{
	int rv = HY_OK;

	//�Ѿ��ر�����Ч���������ڹر�״̬
	if (0 == gu8EnableRefreshSrc) return rv;

	gu32ScreenPro = 0;

	LcdModulePowerOnOff(0);
	hyhwLcd_resetOn();
	gu8EnableRefreshSrc = 0; //������ˢ��

	return rv; 
}


/*---------------------------------------------------------------------------
 ������BackLight_ResumeChg_BKValue
 ���ܣ��ָ�����ֵ
 ��������
 ���أ�
* -----------------------------------------------------------------------------*/
void BackLight_ResumeChg_BKValue(U8 value)
{
	gtHyc.BackLight = value;
}


/*---------------------------------------------------------------------------
 ������BackLight_EventTurnOnLcd
 ���ܣ����ڴ�������״̬�£������¼���Ҫ�������� ��:���绰�ˣ��������µ�
 ��������
 ���أ�1������ 0��ά��
* -----------------------------------------------------------------------------*/ 
U32  BackLight_EventTurnOnLcd(void *pView)
{
	U32 rc = 0;
	tGRAPDESKBOX *pDesk; 
	
	if (0 == gu8EnableRefreshSrc)//080922
	{
		hyhwLcd_resetOff();
		drvLcd_InitLcdModuleInstructs();

		pDesk = ((tGRAPVIEW *)pView)->pParent;

		ConSume_ChangeCoefTemp(360,1);

		if (1 == edit_get_input_status(pDesk))
		{
			drvLcd_IsUseQueue(0);
			alpha_disp_all_to_screen((tCTR_PANEL *)((tGRAPDESKBOX *)Grap_Get_Current_Desk())->pdata); 
		}
		else
		{
			if(gScreen_Display_Mode == 1)
			{//�������ǰ�Ǻ���,������תһ��
				gScreen_Display_Mode = 0;
				drvLcd_220176DisplayMode();
			}

			//090331
			ReDraw(pDesk,0,0);
			drvLcd_IsUseQueue(1); 
			drvLcd_ClearQueue();
			drvLcd_BMPFromDisplayMemory2Screen();
		}

		syssleep(5);
		LcdModulePowerOnOff(1);

		gu8EnableRefreshSrc = 1;
		rc = 1;
		SysSleep_SetWakeMode(0);
	}

	gu32ScreenPro = gtHyc.ScreenProtect;

	return rc;
}


/*---------------------------------------------------------------------------
 ������BackLight_KeyLight
 ���ܣ��а���ʱ��������ʱ��
 ��������
 ���أ���
* -----------------------------------------------------------------------------*/ 
void BackLight_KeyLight(void)
{
    if (1 == gu8EnableRefreshSrc)    
    {  
		if(gtHyc.ScreenProtect != 0)
		gu32ScreenPro = gtHyc.ScreenProtect;		
    }
}

/***************************************
* ����: BackLight_CloseScreenProtect
* ����: �ر�����������֮ǰ����ֵ
* ����: none
* ����: ֮ǰ����ֵ
****************************************/
U16 BackLight_CloseScreenProtect()
{
	U16 temp;
	
	temp = gtHyc.ScreenProtect;
	
	gtHyc.ScreenProtect = cSCREENPROTECT_CLOSE;
	
	return temp;
}

/***************************************
* ����: BackLight_CloseScreenProtect
* ����: �ر�����������֮ǰ����ֵ
* ����: ����ֵ
* ����: none
****************************************/
void BackLight_SetScreenProtect(U16 value)
{
	gtHyc.ScreenProtect = value;
	gu32ScreenPro = value;	
	return ;
}
/*---------------------------------------------------------------------------
 ������BackLight_AvoidThis
 ���ܣ��ڵ�ǰģʽ�����δ����
 ��������
 ���أ�1:���� 0:������
* -----------------------------------------------------------------------------*/ 
U32 BackLight_AvoidThis(void)
{
    U32 rc = 0;
    U32 len,i;
    tAPPLICATION *pApp;	 //eAPPLICATION_NO
    U32 app_pid[]={/*USB_HANDLER_AN,*/ POWER_ON_AN, POWER_DOWN_AN};
    
    len = sizeof(app_pid)/4;
        
    for (i=0; i<len; i++)
    {
        pApp = AppCtrl_GetAppInfor(app_pid[i]);
        
        if (0 != pApp->isFocus)
        {
            rc = 1;
            break;
        }    
    }
    
    return rc;
}
