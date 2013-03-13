/*******************************************************************************
*  (C) Copyright 2010 hyco, All rights reserved              
*
*  This source code and any compilation or derivative thereof is the sole      
*  property of hyco and is provided pursuant 
*  to a Software License Agreement.  This code is the proprietary information of      
*  hyco and is confidential in nature.  Its use and dissemination by    
*  any party other than hyco is strictly limited by the confidential information 
*  provisions of the Agreement referenced above.      
*
*******************************************************************************/

/*------------------------------------------------------------------------------
Standard include files:
------------------------------------------------------------------------------*/
#include "hyOsCpuCfg.h"
#include "hyTypes.h"
#include "hyErrors.h"
#include "stdarg.h"
/*------------------------------------------------------------------------------
  Project include files:
------------------------------------------------------------------------------*/
#include "vhchip.h"
#include "hyUsbDebug.h"

/*---------------------------------------------------------------------------
   Types and defines:
   --------------------------------------------------------------------------*/
#ifdef USB_DEBUG
#define USB_DEBUG_PRINT
#endif

#define FRMWRI_BUFSIZE		256			// ��ʽ����Ļ���
#define SEND_BUFFER_MAX_SIZE	(10*1024)//(5*1024*1024)//send buf���5M

int usbDebugEn = 0;
int usbDebugLockInit = 0;
char* pMsgBuf = NULL;

int usbSendBufSize = 0;
char *pUsbSendBuf = NULL;

// static function prototype
static void putOneCharInBuff (char c, char** ptr);
static int formatted_write_ (const char *format,
                  void put_one_char(char, char**),
                  char * str_pointer,
                  va_list ap);

static int hyUsbConnect_Detect(void);


/*-----------------------------------------------------------------------------
* ����:	putOneCharInBuff
* ����:	��һ���ַ�д���ַ����Ļ�����
* ����:	c		-- �ַ�
*		ptr		-- ���ַ�������
* ����:	none
*----------------------------------------------------------------------------*/
static void putOneCharInBuff (char c, char** ptr)
{
   *(*(char **)ptr)++ = c;
}

/*-----------------------------------------------------------------------------
* ����:	formatted_write_
* ����:	���hyco��printf����ʹ�ã���Ҫ����Ŀɱ�������տ��Ƹ�ʽд�뵽str_pointer
*		�ַ���������
* ����:	format			-- ��ʽ�����ַ�����
*		put_one_char	-- ����ָ�룬ָ��ʵ�����һ���ַ��ĺ���
*		str_pointer		-- ������ɱ������������ַ���
*		ap				-- �ɱ�����б�
* ����:	���뵽���������ַ����е��ַ�����  
*----------------------------------------------------------------------------*/
static int formatted_write_ (const char *format,
                  void put_one_char(char, char**),
                  char * str_pointer,
                  va_list ap)
{
    char format_flag;
    int  precision;
    int  n;
    int  field_width, nr_of_chars;
    char plus_space_flag, left_adjust, l_L_modifier;
    char h_modifier, alternate_flag;
    long ulong;
    unsigned long div_factor;
    char *buf_pointer;
    char *ptr, *hex;
    char zeropad;
    char *buf;

    buf = (char *)malloc(FRMWRI_BUFSIZE);
    nr_of_chars = 0;
    for (;;)                         /* Until full format string read */
    {
       while ((format_flag = *format++) != '%')  /* Until '%' or '\0' */
       {
          if (!format_flag)
          {
            put_one_char ('\0', &str_pointer);
            free(buf);
            return (nr_of_chars);
          }
          put_one_char (format_flag, &str_pointer);
          nr_of_chars++;
       }
       if (*format == '%') /* %% prints as % */
       {
         format++;
         put_one_char('%',&str_pointer);
         nr_of_chars++;
         continue;
       }

       plus_space_flag = left_adjust = alternate_flag = zeropad = 0;
       ptr = buf_pointer = &buf[0];
       hex = "0123456789ABCDEF";
       
       /* ����Ƿ���ǰ���� '-', '+', ' ','#' or '0' ���  */
       for (;;)
       {
          switch (*format)
          {
             case ' ':
               if (plus_space_flag)
                goto NEXT_FLAG;
             case '+':
               plus_space_flag = *format;
               goto NEXT_FLAG;
             case '-':
               left_adjust++;
               goto NEXT_FLAG;
             case '#':
               alternate_flag++;
               goto NEXT_FLAG;
             case '0':
               zeropad++;
               goto NEXT_FLAG;
          }
          break;
       NEXT_FLAG:
         format++;
      }

      /*����Ƿ��в�����ĸ�ʽ����'*'�� ����%*s  */
      if (*format == '*')
      {
         field_width = va_arg(ap, int);
         if (field_width < 0)
         {
            field_width = -field_width;
            left_adjust++;
         }
         format++;
      }
      else
      {
         field_width = 0;
         while (*format >= '0' && *format <= '9')
           field_width = field_width * 10 + (*format++ - '0');
      }

      /*���ȿ���'.'�� ��'*' */
      if (*format=='.')
      {
        if (*++format == '*')
        {
           precision = va_arg(ap, int);
           format++;
        }
        else
        {
          precision = 0;
          while (*format >= '0' && *format <= '9')
            precision = precision * 10 + (*format++ - '0');
        }
      }
      else
        precision = -1;

      /* �����
      	left_adjust	��Ϊ��ʱ����ʾ��һ�����ţ�
      	zeropad		Ϊ1ʱ��ʾ��һ��ǰ���㣻Ϊ0ʱ��ʾû��ǰ���㣻
      	field_width	Ϊ���ݵ�С����ǰ�����ݿ�ȣ�
      	precision	Ϊ���ݵ�С���������ݿ�ȣ����û��С�����֣�precisionΪ-1;
      	plus_space_flag Ϊ0ʱ��ʾ���ӱ�ǣ�Ϊ�ӺŻ�ո�ʱ��ʾ�����ӺŻ�ո�*/

      l_L_modifier = h_modifier = 0;
      /*�Ƿ��п�ѡ��'l','L' r 'h' ���η�? */
      switch (*format)
      {
        case 'l':
        case 'L':
          l_L_modifier++;
          format++;
          break;
        case 'h':
          h_modifier++;
          format++;
          break;
      }

      /*�������switch����˳��󣬽����ʹ�buf_pointer��ʼ����"ptr"-1�������ַ��� */
      switch (format_flag = *format++)
      {
        case 'n':
          if (sizeof(short) == sizeof(long))
          {
            *va_arg(ap,int *) = nr_of_chars;
          }
          else if (sizeof(short) != sizeof(int))
          {
            if (sizeof(int) != sizeof(long))
            {
              if (h_modifier)
                *va_arg(ap,short *) = nr_of_chars;
              else if (l_L_modifier)
                *va_arg(ap,long *) = nr_of_chars;
              else
               *va_arg(ap,int *) = nr_of_chars;
            }
            else
            {
              if (h_modifier)
                *va_arg(ap,short *) = nr_of_chars;
              else
                *va_arg(ap,int *) = nr_of_chars;
            }
         }
         else
         {
            if (l_L_modifier)
              *va_arg(ap,long *) = nr_of_chars;
            else
              *va_arg(ap,int *) = nr_of_chars;
         }
         continue;

       case 'c':
          buf[0] = va_arg(ap, int);
          ptr++;
          break;

       case 's':
         if ( !(buf_pointer = va_arg(ap,char *)) )
            buf_pointer = "(null pointer)";
         if (precision < 0)
           precision = 10000;
         for (n=0; *buf_pointer++ && n < precision; n++)
         ;
         ptr = --buf_pointer;
         buf_pointer -= n;
         break;

       case 'x':
         hex = "0123456789abcdef";
       case 'o':
       case 'u':
       case 'p':
       case 'X':
         if (format_flag == 'p')
           ulong = (long)va_arg(ap,char *);
         else if (sizeof(short) == sizeof(long))
           ulong = va_arg(ap,unsigned int);
         else if (sizeof(short) == sizeof(int))
           ulong = l_L_modifier ? va_arg(ap,long) : va_arg(ap,unsigned int);
         else
          ulong = h_modifier ? (unsigned short) va_arg(ap, int) : va_arg(ap, int);

         div_factor = (format_flag == 'o') ? 8 : (format_flag == 'u') ? 10 : 16;
         goto INTEGRAL_CONVERSION;

       case 'd':
       case 'i':
         if (sizeof(short) == sizeof(long))
         {
            if ( (long)(ulong = va_arg(ap,long)) < 0)
            {
              plus_space_flag = '-';
              ulong = -ulong;
            }
         }
         else if (sizeof(short) == sizeof(int))
         {
           if ( (long)(ulong = l_L_modifier ? va_arg(ap,long) : (long) va_arg(ap,int)) < 0)
           {
             plus_space_flag = '-';
             ulong = -ulong;
           }
         }
         else
         {
           if ( (long)(ulong = (long) (h_modifier ?
             (short) va_arg(ap, int) : va_arg(ap,int))) < 0)
           {
             plus_space_flag = '-';
             ulong = -ulong;
           }
         }
         div_factor = 10;

        /*ת��Ϊ����*/
      INTEGRAL_CONVERSION:
         ptr = buf_pointer = &buf[FRMWRI_BUFSIZE - 1];
         do
            *--buf_pointer = hex[ulong % div_factor];
         while (ulong /= div_factor);
         if (precision < 0)  /* "precision" takes precedence */
           if (zeropad)
             precision = field_width - (plus_space_flag != 0);
         while (precision > ptr - buf_pointer)
           *--buf_pointer = '0';
         if (alternate_flag && *buf_pointer != '0')
         {
           if (format_flag == 'x' || format_flag == 'X')
           {
             *--buf_pointer = format_flag;
             *--buf_pointer = '0';
           }
           else if (format_flag == 'o')
           {
             *--buf_pointer = '0';
           }
         }
         break;
        break;

       case '\0':            /* Really bad place to find NULL in */
         format--;

       default:
        /*û�ж����ת��*/
         ptr = buf_pointer = "???";
         ptr += 3;
         break;

      }

      /*���²��ַ��͸�ʽ���õ��ַ�����put_one_char */
      if ( (precision = ptr - buf_pointer) > field_width)
      {
        n = 0;
      }
      else
      {
        n = field_width - precision - (plus_space_flag != 0);
      }

      /*����ǰ��������ַ� */
      if (!left_adjust)
         while (--n >= 0)
         {
           put_one_char(' ', &str_pointer);
           nr_of_chars++;
         }

      /*���ͱ��  */
       if (plus_space_flag)
      {
        put_one_char(plus_space_flag, &str_pointer);
        nr_of_chars++;
      }

      /* �����ַ������� */
      while (--precision >= 0)
      {
        put_one_char(*buf_pointer++, &str_pointer);
        nr_of_chars++;
      }

      /* ���� trailing �Ŀո� */
      if (left_adjust)
        while (--n >= 0)
        {
          put_one_char(' ', &str_pointer);
          nr_of_chars++;
        }
    }
    free(buf);
    return 0;
}

/*-----------------------------------------------------------------------------
* ����:	hyUsbPrintf
* ����:	hyco��printf����
* ����:	format	-- ��ʽ�����ַ���
*		...		-- �ɱ�����б�
* ����:	print���ַ�����
*----------------------------------------------------------------------------*/
int hyUsbPrintf(const char *format, ...)
{
	va_list list;
	int     n = 0;
	
	if(App_GetUsbDebugStat() == 0)
	{
		return 0;
	}
	
#ifdef USB_DEBUG_PRINT
	if (usbDebugLockInit == 1)
		wlock(&usbDebugEn);
	
	if (hyUsbConnect_Detect() == HY_OK)
	{
		int len;
		va_start (list, format);
		
		n = formatted_write_(format, putOneCharInBuff, pMsgBuf, list);
		
		len = strlen(pMsgBuf);
		if (usbDebugEn == 1 || usbDebugEn == 2)
		{
			//PCδ׼���ý��գ�����д��buf��
			if ((usbSendBufSize+len) < SEND_BUFFER_MAX_SIZE)
			{
				memcpy(&pUsbSendBuf[usbSendBufSize], pMsgBuf, len);
				usbSendBufSize += len;
			}
		}
		else
		{
			hyhwUsb_TransmitData(pMsgBuf, len);
		}
		va_end (list);
	}
	if (usbDebugLockInit == 1)
		unlock(&usbDebugEn);
#endif
	return (n);
}

/*-----------------------------------------------------------------------------
* ����:	hyUsbMessage
* ����:	hyco��Message����, ����Ҫ��ʽ���ƣ�����������ַ���
* ����:	msg	-- ��Ҫ������ַ���
* ����:	none  
*----------------------------------------------------------------------------*/
void hyUsbMessage(const char *msg)
{
	if(App_GetUsbDebugStat() == 0)
	{
		return ;
	}
	
#ifdef USB_DEBUG_PRINT
	if (usbDebugLockInit == 1)
		wlock(&usbDebugEn);
	if (hyUsbConnect_Detect() == HY_OK)
	{
		int len = strlen(msg);
		if (usbDebugEn == 1 || usbDebugEn == 2)
		{
			//PCδ׼���ý��գ�����д��buf��
			if ((usbSendBufSize+len) < SEND_BUFFER_MAX_SIZE)
			{
				memcpy(&pUsbSendBuf[usbSendBufSize], msg, len);
				usbSendBufSize += len;
			}
		}
		else
		{
			hyhwUsb_TransmitData(msg, len);
		}
	}
	if (usbDebugLockInit == 1)
		unlock(&usbDebugEn);
#endif
}

/*-----------------------------------------------------------------------------
* ����:	hyUsbMessageByLen
* ����:	hyco��Message����, ����Ҫ��ʽ���ƣ�����������ַ���
* ����:	msg	-- ��Ҫ������ַ���
* ����:	none  
*----------------------------------------------------------------------------*/
void hyUsbMessageByLen(const char *msg, int len)
{
	if(App_GetUsbDebugStat() == 0)
	{
		return ;
	}
	
#ifdef USB_DEBUG_PRINT
	if (usbDebugLockInit == 1)
		wlock(&usbDebugEn);
	if (hyUsbConnect_Detect() == HY_OK)
	{
		//int len = strlen(msg);
		if (usbDebugEn == 1 || usbDebugEn == 2)
		{
			//PCδ׼���ý��գ�����д��buf��
			if ((usbSendBufSize+len) < SEND_BUFFER_MAX_SIZE)
			{
				memcpy(&pUsbSendBuf[usbSendBufSize], msg, len);
				usbSendBufSize += len;
			}
		}
		else
		{
			hyhwUsb_TransmitData(msg, len);
		}
	}
	if (usbDebugLockInit == 1)
		unlock(&usbDebugEn);
#endif
}

/*-----------------------------------------------------------------------------
* ����:	hyUsbData
* ����:	hyco��data���ͺ���, 
* ����:	pdata	-- ����ָ��
		len		-- ���ݳ���
* ����:	none  
*----------------------------------------------------------------------------*/
void hyUsbData(char *pdata, int len)
{
#ifdef USB_DEBUG_PRINT
	int sendCnt, remainCnt, i;
	
	if(App_GetUsbDebugStat() == 0)
	{
		return ;
	}
	
	sendCnt = len/512;
	remainCnt = len%512;
	if (usbDebugLockInit == 1)
		wlock(&usbDebugEn);
	if (hyUsbConnect_Detect() == HY_OK)
	{
		if (usbDebugEn == 1 || usbDebugEn == 2)
		{
			//PCδ׼���ý��գ�����д��buf��
			if ((usbSendBufSize+len) < SEND_BUFFER_MAX_SIZE)
			{
				memcpy(&pUsbSendBuf[usbSendBufSize], pdata, len);
				usbSendBufSize += len;
			}
		}
		else
		{
			for (i=0; i<sendCnt; i++)
			{
				hyhwUsb_TransmitData(&pdata[i*512], 512);
			}
			if (remainCnt > 0)
			{
				hyhwUsb_TransmitData(&pdata[sendCnt*512], remainCnt);
			}
		}
	}
	if (usbDebugLockInit == 1)
		unlock(&usbDebugEn);
#endif
}

/*-----------------------------------------------------------------------------
* ����:	hyUsbTrans_softInit
* ����:	��ʼ��Usb����
* ����:	none
* ����:	HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
int hyUsbTrans_softInit(void)
{
#ifdef USB_DEBUG_PRINT
	usbDebugEn = 0;
	usbDebugLockInit = 0;
	usbSendBufSize = 0;
	pUsbSendBuf = NULL;
	
	if (hyudsDcd_Connected() == TRUE)
	{
		pMsgBuf = (char *)malloc(FRMWRI_BUFSIZE);
		if (pMsgBuf == NULL)
		{
			return HY_ERROR;
		}
		usbDebugEn = 1;
		usbDebugLockInit = 1;
		pUsbSendBuf = (char *)malloc(SEND_BUFFER_MAX_SIZE);
		return HY_OK;
	}
	return HY_ERROR;
#else
	return HY_ERROR;
#endif
}
/*-----------------------------------------------------------------------------
* ����:	hyUsbTrans_Init
* ����:	��ʼ��Usb����
* ����:	none
* ����:	HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
int hyUsbTrans_Init(void)
{
#ifdef USB_DEBUG_PRINT
	if (hyudsDcd_Connected() == TRUE)
	{
		int usbFlag = 0;
		int sysTicks, usb_sysTicks;
		
		//ȷ����usb���뻹�ǳ�����
		USB_InitMemory_buf();
		fnUSB_DevInit();
	  	if (hyudsDcd_GetSpeedDetectFlag() == FALSE)
	  	{
			usbDebugEn = 0;
			usbDebugLockInit = 0;
			usbSendBufSize = 0;
			if (pMsgBuf != NULL)
			{
				free(pMsgBuf);
				pMsgBuf = NULL;
			}
			if (pUsbSendBuf != NULL)
			{
				free(pUsbSendBuf);
				pUsbSendBuf = NULL;
			}
	  		hyudsDcd_HwDeinit();
	  		return HY_ERROR;
		}
		
		usbDebugEn = 2;
		
		return HY_OK;
	}
	else
	{
		usbDebugEn = 0;
		usbDebugLockInit = 0;
		usbSendBufSize = 0;
		if (pMsgBuf != NULL)
		{
			free(pMsgBuf);
			pMsgBuf = NULL;
		}
		if (pUsbSendBuf != NULL)
		{
			free(pUsbSendBuf);
			pUsbSendBuf = NULL;
		}
		return HY_ERROR;
	}
#else
	return HY_ERROR;
#endif
}


int hyUsbTrans_DeInit(void)
{
	int ret = HY_ERROR;
	if (usbDebugEn != 0)
	{
		if (usbDebugEn >= 2)
		{
			//USBӲ����ʼ���ɹ���
			//if (hyudsDcd_Connected() == FALSE)
			{
				//USB�Ѿ��Ͽ�
				usbDebugEn = 0;
				usbDebugLockInit = 0;
				usbSendBufSize = 0;
				if (pMsgBuf != NULL)
				{
					free(pMsgBuf);
					pMsgBuf = NULL;
				}
				if (pUsbSendBuf != NULL)
				{
					free(pUsbSendBuf);
					pUsbSendBuf = NULL;
				}
				hyudsDcd_HwDeinit();
				return ret;
			}
		}
		//if (usbDebugEn == 2)
		{
			ret = HY_OK;
		}
	}
	return ret;
}
/*-----------------------------------------------------------------------------
* ����:	hyUsbTrans_Init
* ����:	����Ƿ������USBͨ����������
* ����:	none
* ����:	none
*----------------------------------------------------------------------------*/
void hyUsbTrans_DetectStart(void)
{
#ifdef USB_DEBUG_PRINT
	if (usbDebugLockInit == 1)
		wlock(&usbDebugEn);
	if (usbDebugEn == 2)
	{
		U32 recvLen;
		char* pRecvBuf = NULL;
		//����
		hyhwUsb_Dispatch();
		recvLen = hyhwUsb_CheckDataReceived();
		if(recvLen != 0)
		{
			pRecvBuf = (char *)malloc(FRMWRI_BUFSIZE);
			if (pRecvBuf == NULL)
			{
				return;
			}
			hyhwUsb_ReceiveData(pRecvBuf, recvLen);
			if (memcmp(pRecvBuf, "USB_START", strlen("USB_START")) == 0)
			{
				//���buf����δ�����ݣ���ȫ������
				if (usbSendBufSize>0)
				{
					int sendCnt, remainCnt, i;
					
					sendCnt = usbSendBufSize/512;
					remainCnt = usbSendBufSize%512;
					
					for (i=0; i<sendCnt; i++)
					{
						hyhwUsb_TransmitData(&pUsbSendBuf[i*512], 512);
					}
					if (remainCnt > 0)
					{
						hyhwUsb_TransmitData(&pUsbSendBuf[sendCnt*512], remainCnt);
					}
				}
				usbDebugEn = 3;
			}
			if (pRecvBuf != NULL)
			{
				free(pRecvBuf);
				pRecvBuf = NULL;
			}
		}
	}
	if (usbDebugLockInit == 1)
		unlock(&usbDebugEn);
#endif	
}
/*-----------------------------------------------------------------------------
* ����:	hyUsbTrans_isRunning
* ����:	�ж��Ƿ����USB����ģʽ״̬
* ����:	none
* ����:	HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
int hyUsbTrans_isRunning(void)
{
#ifdef USB_DEBUG_PRINT
	if (usbDebugEn != 0)
	{
		return HY_OK;
	}
	else
	{
		return HY_ERROR;
	}
#else
	return HY_ERROR;
#endif
}

/*-----------------------------------------------------------------------------
* ����:	hyUsbConnect_Detect
* ����:	���USB�Ƿ�Ͽ�������Ͽ������ͷ��ڴ棬����״̬
* ����:	none
* ����:	HY_OK, HY_ERROR
*----------------------------------------------------------------------------*/
static int hyUsbConnect_Detect(void)
{
	int ret = HY_ERROR;
	if (usbDebugEn != 0)
	{
		if (usbDebugEn >= 2)
		{
			//USBӲ����ʼ���ɹ���
			if (hyudsDcd_Connected() == FALSE)
			{
				//USB�Ѿ��Ͽ�
				usbDebugEn = 0;
				usbDebugLockInit = 0;
				usbSendBufSize = 0;
				if (pMsgBuf != NULL)
				{
					free(pMsgBuf);
					pMsgBuf = NULL;
				}
				if (pUsbSendBuf != NULL)
				{
					free(pUsbSendBuf);
					pUsbSendBuf = NULL;
				}
				hyudsDcd_HwDeinit();
				return ret;
			}
		}
		//if (usbDebugEn == 2)
		{
			ret = HY_OK;
		}
	}
	return ret;
}

void hyUsbDebugDeviceInit(void)
{
     hyUsbConnect_Detect();
}

void hyUsbConnect_WaitStart()
{
#ifdef USB_DEBUG_PRINT
	while(1)
	{
		syssleep(10);
		if (usbDebugEn==3 || usbDebugEn==0)
		{
			break;
		}
	}
#endif
}

//��ͬһ������������PCʶ��USB�豸
void hyUsbConnect_WaitStart1(void)
{
#ifdef USB_DEBUG_PRINT
	while(1)
	{
		hyUsbTrans_DetectStart();
		syssleep(1);
		if (usbDebugEn==3 || usbDebugEn==0)
		{
			break;
		}
	}
#endif
}

int hyUsbConnect_WaitStart2(void)
{
#ifdef USB_DEBUG_PRINT
	int cnt = 0;
	while(1)
	{
		hyUsbTrans_DetectStart();
		syssleep(1);
		if (usbDebugEn==3 || usbDebugEn==0)
		{
			return HY_OK;
			//break;
		}
		cnt++;
		if (cnt >= 5000) return -1;
	}
#endif
	return HY_ERROR;
}

void hyUsbDebug_init(void)
{
#ifdef USB_DEBUG_PRINT
	U8 cnt=0;
	hyudsDcd_connectInit();
	while(1)
	{
		if (hyUsbTrans_softInit() == HY_OK)
		{
			int ret;
			hyUsbMessage("hyUsbTrans_softInit OK!!\r\n");
			syssleep(10);
//again:
	    	hyUsbTrans_Init();
	    	ret = hyUsbConnect_WaitStart2();
	    	if (ret == HY_OK) break;
		}
		syssleep(1);
		cnt++;
		if (cnt == 50)
		{
			Led_ctrl(6);
		}
		else if (cnt == 100)
		{
			Led_ctrl(0);
			cnt = 0;
		}
	}
#endif
}