#ifndef _HYCO_API_H
#define _HYCO_API_H

#include "hyTypes.h"







/*++++++++++++++++++++++++++++++++++++++++ Ӳ����� API fun ++++++++++++++++++++++++++++++++++++++++++++*/
/*==============================================������Դ��ؿ���============================================*/
/*---------------------------------------
* ����:Power_SetBattAutoOffTimer
* ����:�Զ��ػ�ʱ��Ŀ��ơ� ��� timeStr ��ΪNULL,���Ը�ʱ����Ϊ�ؼ�ʱ��, ������delaySecond�ӳ����ػ�ʱ��
* ����:timeStr:ʱ���ַ���, ��ʽ:yyyymmddhhmmss (20121202121000)
       delaySecond:�ӵ�ǰʱ�俪ʼ�����ӳ�����
* ����:0:���óɹ�    -1:ʧ��
*----------------------------------------*/
int Power_SetBattAutoOffTimer(char *timeStr, unsigned int delaySecond);


/*---------------------------------------
* ����:Power_DisablePOff
* ����:��Դ����ֹʹ��
* ����:none
* ����:none
*----------------------------------------*/
void Power_DisablePOff();


/*---------------------------------------
* ����:Power_EnablePOff
* ����:��Դ������ʹ��
* ����:none
* ����:none
*----------------------------------------*/
void Power_EnablePOff();

/*---------------------------------------
* ����:Power_GetVoltageLevel
* ����:������ȡ��
* ����:none
* ����:0~4:0��4��   5:������
*----------------------------------------*/
U32 Power_GetVoltageLevel();

/*---------------------------------------
* ����:Power_
* ����:���ͼ��Ŀ���
* ����:none
* ����:none
*----------------------------------------*/
//void Power_();

/*===========================================�����̣ãĿ���(����Ҫ��Ҫ�ṩ������ϸ��API)=========================================*/
/*---------------------------------------
* ����:LCD_OpenBacklight
* ����:����ʾ������
* ����:none
* ����:none
*----------------------------------------*/
void LCD_OpenBacklight();

/*---------------------------------------
* ����:LCD_CloseBacklight
* ����:�ر���ʾ������
* ����:none
* ����:none
*----------------------------------------*/
void LCD_CloseBacklight();

/*---------------------------------------
* ����:LCD_SetBacklightGrade
* ����:������ʾ���������ȵȼ�
* ����:grade:���ȵȼ�  0~4��
* ����:none
*----------------------------------------*/
void LCD_SetBacklightGrade(unsigned char grade);

//to do.......

/*===========================================3��״̬���Ŀ���=========================================*/
/*---------------------------------------
* ����:JT_SetIMEState
* ����:�������뷨״̬
* ����:state:0:�ر�  1:123  2:abc  3:ABC  4:��д  5:ƴ��
* ����:none
*----------------------------------------*/
void JT_SetIMEState(unsigned char state);

/*===========================================4�������Ŀ���(����Ҫ��Ҫ�ṩ������ϸ��API)=========================================*/
/*---------------------------------------
* ����:
* ����:������ֵ��ȡ�úͿ���
* ����:none
* ����:none
*----------------------------------------*/
//void ();

/*---------------------------------------
* ����:
* ����:�����������
* ����:none
* ����:none
*----------------------------------------*/
//void ();

/*---------------------------------------
* ����:
* ����:�����������
* ����:none
* ����:none
*----------------------------------------*/
//void ();

/*---------------------------------------
* ����:
* ����:�漰����������Ͱ�����Ӧ��ص�API
* ����:none
* ����:none
*----------------------------------------*/
//void ();

/*===========================================5��LED ����=========================================*/
/*---------------------------------------
* ����:LED_Control
* ����:��ʾ����ɫ
* ����:color: 0:�ر�  1:��  2:��  4:��  3:��  5:Ʒ��  6:��  7:��
       times:��˸����
* ����:none
*----------------------------------------*/
void LED_Control(int color, int times);

/*===========================================6����������=========================================*/
/*---------------------------------------
* ����:Buzzer_On
* ����:����������
* ����:beepType: �������� 0:��  6:�εεε� 
       ledType:LED���� 0:�ر�  1:��  2:��  4:��  3:��  5:Ʒ��  6:��  7:��
* ����:none
*----------------------------------------*/
void Buzzer_On(int beepType, int ledType);

/*---------------------------------------
* ����:Buzzer_Off
* ����:ֹͣ������
* ����:none
* ����:none
*----------------------------------------*/
void Buzzer_Off();
		
/*===========================================7���𶯿���=========================================*/
/*---------------------------------------
* ����:VIB_On
* ����:����
* ����:millisecond:��ʱ�� ��λ ms
* ����:none
*----------------------------------------*/
void VIB_On(unsigned int millisecond);

/*---------------------------------------
* ����:VIB_Off
* ����:ֹͣ
* ����:none
* ����:none
*----------------------------------------*/
void VIB_Off();

/*===========================================8��ɨ�����=========================================*/
/*---------------------------------------
* ����:Scan_InitEx
* ����:ɨ���豸�ĳ�ʼ��
* ����:none
* ����:none
*----------------------------------------*/
void Scan_InitEx();

/*---------------------------------------
* ����:Scan_DeinitEx
* ����:ɨ���豸�Ĺر�
* ����:none
* ����:none
*----------------------------------------*/
void Scan_DeinitEx();

/*---------------------------------------
* ����:Scan_EnableOn
* ����:��ɨ�� (�򿪼���)
* ����:none
* ����:none
*----------------------------------------*/
void Scan_EnableOn();

/*---------------------------------------
* ����:Scan_EnableOff
* ����:��ɨ�� (�رռ���)
* ����:none
* ����:none
*----------------------------------------*/
void Scan_EnableOff();

/*---------------------------------------
* ����:Scan_SetScannerParam
* ����:ɨ����ز��������ã�����������ɨ�跽ʽ���������͡�checkdigt�ȵȡ�
* ����:none
* ����:none
*----------------------------------------*/
void Scan_SetScannerParam();

/*---------------------------------------
* ����:Scan_GetScannerParam
* ����:ɨ����ز�����ȡ�á�
* ����:none
* ����:none
*----------------------------------------*/
void Scan_GetScannerParam();

/*===========================================9��BluetoothͨѶ����=========================================*/
/*---------------------------------------
* ����:BLT_PowerOn
* ����:�����豸�������� 
* ����:none
* ����:none
*----------------------------------------*/
void BLT_PowerOn();	

/*---------------------------------------
* ����:BLT_PowerOff
* ����:�����豸�Ĺرա�  
* ����:none
* ����:none
*----------------------------------------*/
void BLT_PowerOff();	

//Masterģʽ����ͨ�Ź����漰���ĺ������Լ���ز��������ú�ȡ��
//Slaveģʽ����ͨ�Ź����漰���ĺ������Լ���ز��������ú�ȡ��
			
/*===========================================10��Ӳ�������Ϣ��ȡ��=========================================*/
//���Ʋ���ϵͳ�汾����Ʒ���кŵȵ�
			
/*===========================================11��WIFI��GPRS��ͨѶ���=========================================*/
//wifi���ӵĻ�ȡ��ѡ��	
//wifi��GPRSͨѶģʽ�Ļ���ת��



/*++++++++++++++++++++++++++++++++++++++++ ������ API fun ++++++++++++++++++++++++++++++++++++++++++++*/
/*===========================================����TCPͨ��WinsockAPI=========================================*/


	/*����TCPͨ��WinsockAPI		
	No. 	�������� 	����
	1	socket	The socket function creates a socket that is bound to a specific transport service provider.
	2	connect	The connect function establishes a connection to a specified socket.
	3	send	The send function sends data on a connected socket.
	4	recv	The recv function receives data from a connected or bound socket.
	5	shutdown	The shutdown function disables sends or receives on a socket.
	6	closesocket	The closesocket function closes an existing socket.
	7	WSAStartup	The WSAStartup function initiates use of the Winsock DLL by a process.
	8	WSACleanup	The WSACleanup function terminates use of the Winsock 2 DLL (Ws2_32.dll).
	9	WSAGetLastError	The WSAGetLastError function returns the error status for the last Windows Sockets operation that failed.
	10	select	The select function determines the status of one or more sockets, waiting if necessary, to perform synchronous I/O.
	11	htons	The htons function converts a u_short from host to TCP/IP network byte order (which is big-endian).
	*/		
/*===========================================�������뷨���API=========================================*/

/*	�������뷨���API		
	No. 	�������� 	����
	1	ImmSetOpenStatus	Opens or closes the IME.
	2	ImmSetConversionStatus	Sets the current conversion status.
	3	ImmSetCompositionFont	Sets the logical font to use to display characters in the composition window.
	4	ImmReleaseContext	Releases the input context and unlocks the memory associated in the input context. An application must call this function for each call to the ImmGetContext function.
*/

/*===========================================3��������ʾ���API=========================================*/
			
/*	3��������ʾ���API		
	No. 	�������� 	����
	1	SelectObject	The SelectObject function selects an object into the specified device context (DC). The new object replaces the previous object of the same type. 
	2	GetStockObject	The GetStockObject function retrieves a handle to one of the stock pens, brushes, fonts, or palettes.
	3	SetRectEmpty	The SetRectEmpty function creates an empty rectangle in which all coordinates are set to zero.
	4	SetBkColor	The SetBkColor function sets the current background color to the specified color value, or to the nearest physical color if the device cannot represent the specified color value. 
	5	ExtTextOut	The ExtTextOut function draws text using the currently selected font, background color, and text color. You can optionally provide dimensions to be used for clipping, opaquing, or both. 
	6	GetCurrentObject	The GetCurrentObject function retrieves a handle to an object of the specified type that has been selected into the specified device context (DC). 
	7	SetTextColor	The SetTextColor function sets the text color for the specified device context to the specified color.
	8	DrawText	The DrawText function draws formatted text in the specified rectangle. It formats the text according to the specified method (expanding tabs, justifying characters, breaking lines, and so forth). 
	9	BitBlt	The BitBlt function performs a bit-block transfer of the color data corresponding to a rectangle of pixels from the specified source device context into a destination device context.
	10	DeleteObject	The DeleteObject function deletes a logical pen, brush, font, bitmap, region, or palette, freeing all system resources associated with the object. After the object is deleted, the specified handle is no longer valid. 
	11	GetDC	The GetDC function retrieves a handle to a display device context (DC) for the client area of a specified window or for the entire screen. You can use the returned handle in subsequent GDI functions to draw in the DC. 
	12	CreateCompatibleDC	The CreateCompatibleDC function creates a memory device context (DC) compatible with the specified device. 
	13	CreateCompatibleBitmap	The CreateCompatibleBitmap function creates a bitmap compatible with the device that is associated with the specified device context. 
	14	ReleaseDC	The ReleaseDC function releases a device context (DC), freeing it for use by other applications. The effect of the ReleaseDC function depends on the type of DC. It frees only common and window DCs. It has no effect on class or private DCs. 
	15	GetDeviceCaps	The GetDeviceCaps function retrieves device-specific information for the specified device. 
	16	CreateSolidBrush	The CreateSolidBrush function creates a logical brush that has the specified solid color. 
	17	FillRect	The FillRect function fills a rectangle by using the specified brush. This function includes the left and top borders, but excludes the right and bottom borders of the rectangle. 
	18	DrawIcon	The DrawIcon function draws an icon or cursor into the specified device context.
	19	SetBkMode	The SetBkMode function sets the background mix mode of the specified device context. The background mix mode is used with text, hatched brushes, and pen styles that are not solid lines. 
	20	GetClientRect	The GetClientRect function retrieves the coordinates of a window's client area. 
	21	InvalidateRect	The InvalidateRect function adds a rectangle to the specified window's update region. The update region represents the portion of the window's client area that must be redrawn. 
	22	BeginPaint	The BeginPaint function prepares the specified window for painting and fills a PAINTSTRUCT structure with information about the painting. 
	23	EndPaint	The EndPaint function marks the end of painting in the specified window. This function is required for each call to the BeginPaint function, but only after painting is complete. 
*/

/*===========================================4���ַ�������API=========================================*/
			
/*	4���ַ�������API		
	No. 	�������� 	����
	1	strlen	Get the length of a string.
	2	strcmp	Compare strings.
	3	atoi	Convert strings to integer.
	4	itoa	Converts an integer to a string.
	5	isdigit	Each routine returns nonzero if c is a particular representation of a decimal-digit character.
	6	islower	This routine returns nonzero if c is a particular representation of a lowercase character.
	7	isupper	This routine returns nonzero if c is a particular representation of an uppercase letter.
	8	MultiByteToWideChar	"This function maps a character string to a wide-character (Unicode) string. 
The character string mapped by this function is not necessarily from a multibyte character set. "
	9	WideCharToMultiByte	This function maps a wide-character string to a new character string. The new character string is not necessarily from a multibyte character set. 
	10	strcpy	Copy a string.
	11	strcat	Append a string.
	12	strstr	Find a substring.
	13	strncpy	Copy characters of one string to another.
	14	StringCchPrintf	"This function is a replacement for sprintf. It accepts a format string and a list of arguments and returns a formatted string. 
The size, in characters, of the destination buffer is provided to the function to ensure that StringCchPrintf does not write past the end of this buffer."
*/

/*===========================================5���ļ��������API=========================================*/
			
	/*5���ļ��������API		
	No. 	�������� 	����
	1	GetFileAttributes	Retrieves a set of FAT file system attributes for a specified file or directory.
	2	CreateDirectory	Creates a new directory. If the underlying file system supports security on files and directories, the function applies a specified security descriptor to the new directory.
	3	fopen	Open a file.
	4	fclose	Closes a stream or closes all open streams.
	5	fread	Reads data from a stream.
	6	fwrite	Writes data to a stream.
	*/
	
/*===========================================6��ע���������API=========================================*/
	
	/*		
	6��ע���������API		
	No. 	�������� 	����
	1	RegOpenKeyEx	This function opens the specified key.
	2	RegQueryValueEx	This function retrieves the type and data for a specified value name associated with an open registry key.
	3	RegCloseKey	This function releases the handle of the specified key.
	4	RegCreateKeyEx	This function creates the specified key. If the key already exists in the registry, the function opens it.
	5	RegSetValueEx	This function stores data in the value field of an open registry key. It can also set additional value and type information for the specified key.
	6	fwrite	Writes data to a stream.
	*/
	
/*===========================================7�������ַȡ�����API	=========================================*/
/*			
	7�������ַȡ�����API		
	No. 	�������� 	����
	1	gethostname	This function returns the standard host name for the local machine.
	2	gethostbyname	This function retrieves host information corresponding to a host name from a host database.
	3	GetAdaptersInfo	This function retrieves adapter information for the local computer.
*/

/*===========================================8��ʱ��������API	=========================================*/

			
	/*8��ʱ��������API		
	No. 	�������� 	����
	1	GetLocalTime	This function retrieves the current local date and time.
	2	GetTickCount	Retrieves the number of milliseconds that have elapsed since the system was started, up to 49.7 days.
	3	rand	Generates a pseudorandom number.
	4	srand	Sets a random starting point.
	5	SetTimer	The SetTimer function creates a timer with the specified time-out value.
	6	KillTimer	The KillTimer function destroys the specified timer. 
	7	Sleep	This function suspends the execution of the current thread for a specified interval. 
	*/

/*===========================================9����Ϣ�������API	=========================================*/
	
	/*9����Ϣ�������API		
	No. 	�������� 	����
	1	PeekMessage	This function checks a thread message queue for a message and places the message (if any) in the specified structure.
	2	PostMessage	The PostMessage function places (posts) a message in the message queue associated with the thread that created the specified window and returns without waiting for the thread to process the message.
	3	SendMessage	This function sends the specified message to a window or windows. SendMessage calls the window procedure for the specified window and does not return until the window procedure has processed the message. 
	*/
	
/*===========================================10��Windows�������API	=========================================*/
			
	/*10��Windows�������API		
	No. 	�������� 	����
	1	RegisterClass	This function registers a window class for subsequent use in calls to the CreateWindow or CreateWindowEx function.
	2	CreateWindowEx	The CreateWindowEx function creates an overlapped, pop-up, or child window with an extended window style; otherwise, this function is identical to the CreateWindow function. 
	3	IsWindow	The IsWindow function determines whether the specified window handle identifies an existing window. 
	4	SetWindowPos	This function changes the size, position, and z-order of a child, pop-up, or top-level window. Child, pop-up, and top-level windows are ordered according to their appearance on the screen. The topmost window receives the highest rank and is the first window in the z-order.
	5	ShowWindow	This function sets the specified window's show state.
	6	UpdateWindow	The UpdateWindow function updates the client area of the specified window by sending a WM_PAINT message to the window if the window's update region is not empty. The function sends a WM_PAINT message directly to the window procedure of the specified window, bypassing the application queue. If the update region is empty, no message is sent. 
	7	FindWindow	This function retrieves the handle to the top-level window whose class name and window name match the specified strings. This function does not search child windows. 
	8	DefWindowProc	This function calls the default window procedure to provide default processing for any window messages that an application does not process. This function ensures that every message is processed. DefWindowProc is called with the same parameters received by the window procedure.
	9	SetFocus	This function sets the keyboard focus to the specified window. All subsequent keyboard input is directed to this window. The window, if any, that previously had the keyboard focus loses it.
	10	GetWindowRect	This function retrieves the dimensions of the bounding rectangle of the specified window. The dimensions are given in screen coordinates that are relative to the upper-left corner of the screen. 
	11	MoveWindow	This function changes the position and dimensions of the specified window. For a top-level window, the position and dimensions are relative to the upper-left corner of the screen. For a child window, they are relative to the upper-left corner of the parent window's client area.
	12	IsWindowVisible	This function retrieves the visibility state of the specified window.
	*/		
			
/*===========================================11�������������API	=========================================*/
			
/*	11�������������API		
	No. 	�������� 	����
	1	malloc	Allocates memory blocks.
	2	free	Deallocates or frees a memory block.
	3	memset	Sets buffers to a specified character.
*/





#endif //_HYCO_API_H