/*

Copyright 2007  Julian aka masterfreek64 and Joseph Pearson aka chessmaster42 and Joel Teichroeb aka bobjr777

This file is part of OblivionOnline.

    OblivionOnline is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    OblivionOnline is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"

#include "ServerAdminGUI.h"
#include "OOPackets.h"

//-------------------------
//----Begin Dialog Code----
//-------------------------

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

HWND hAdminDlg;
HANDLE hServerThread;
SOCKET ServerSocket;
bool Started = false;

char commandString[256];
char adminMsg[512];



//Prototypes for GUI functions
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	ServerConsoleDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

//Prototypes for net
DWORD WINAPI net_main(LPVOID Params);

//Main api entry
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	//Init global strings
	LoadString(hInstance, IDS_APPTITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SERVERCONSOLE, szWindowClass, MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERVERCONSOLE));
	
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GUISERVER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDR_GUISERVER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_GUISERVERSMALL));

	return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, 640, 480, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case ID_FILE_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_SERVER_START:
			if(!Started)
			{
				Started = true;
				hServerThread = CreateThread(NULL, NULL, net_main, NULL, NULL, NULL);
			}
			break;
		case ID_SERVER_STOP:
			if(Started)
			{
				TerminateThread(hServerThread, 0);
				CloseHandle(hServerThread);
				closesocket(ServerSocket);
				ServerSocket = INVALID_SOCKET;
				WSACleanup();
				Started = false;
			}
			break;
		case ID_SERVER_RESTART:
			if(Started)
			{
				TerminateThread(hServerThread, 0);
				CloseHandle(hServerThread);
				closesocket(ServerSocket);
				ServerSocket = INVALID_SOCKET;
				WSACleanup();
				Started = true;
				hServerThread = CreateThread(NULL, NULL, net_main, NULL, NULL, NULL);
			}
			break;
		case ID_SERVER_TOGGLECONSOLE:
			DialogBox(hInst,MAKEINTRESOURCE(IDD_SERVERCONSOLE),hWnd,ServerConsoleDlg);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for Server Console
INT_PTR CALLBACK ServerConsoleDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)"Server console loaded");
		
		hAdminDlg = hDlg;

		return true;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_KICK:
			{
				int Num = GetDlgItemInt(hAdminDlg, IDC_PLAYERNUMBER, NULL, false);
				if (Num < 0 || Num > 255)
				{
					MessageBoxA(NULL, "Player number out of range", "Error", NULL);
					return true;
				}
				UInt8 PlayerNum = Num;
				OOPkgAdminInfo pkgBuf;
				pkgBuf.etypeID = OOPAdminInfo;
				pkgBuf.Flags = 0;
				pkgBuf.ControlCommand = KICKCONTROL;
				char *SendBuf;
				void *DataDest;
				SendBuf = (char *)malloc(sizeof(OOPkgAdminInfo)+1);
				memcpy(SendBuf,&pkgBuf,sizeof(OOPkgAdminInfo));
				DataDest=(SendBuf+sizeof(OOPkgAdminInfo));
				memcpy(DataDest,&PlayerNum,1);
				send(ServerSocket,SendBuf,sizeof(OOPkgAdminInfo)+1,0);
				free(SendBuf);
				return true;
			}
		case IDC_CLOSE:
			EndDialog(hDlg, LOWORD(wParam));
			return true;
		case IDC_SEND:
			{
				//Read in data from the dialog
				int Num = GetDlgItemInt(hAdminDlg, IDC_PLAYERNUMBER, NULL, false);
				if (Num < 0 || Num > 255)
				{
					MessageBoxA(NULL, "Player number out of range", "Error", NULL);
					return true;
				}
				UInt8 PlayerNum = Num;
				char ServerChat[256];
				int Length = GetDlgItemTextA(hAdminDlg, IDC_COMMAND_ENTER, ServerChat+1, 255);
				if (Length < 1)
				{
					MessageBoxA(NULL, "No command entered.", "Error", NULL);
					return true;
				}

				//Add 1 byte for PlayerNum
				Length++;
				ServerChat[0] = PlayerNum;

				//Prepage packet and send it
				OOPkgAdminInfo pkgBuf;
				pkgBuf.etypeID = OOPAdminInfo;
				pkgBuf.Flags = 0;
				pkgBuf.ControlCommand = CHATCONTROL;
				char *SendBuf;
				void *DataDest;
				SendBuf = (char *)malloc(sizeof(OOPkgAdminInfo)+Length);
				memcpy(SendBuf,&pkgBuf,sizeof(OOPkgAdminInfo));
				DataDest=(SendBuf+sizeof(OOPkgAdminInfo));
				memcpy(DataDest,ServerChat,Length);
				send(ServerSocket,SendBuf,sizeof(OOPkgAdminInfo)+Length,0);
				free(SendBuf);
				return true;
			}
		};
		break;
	}
	return false;
}

//-------------------------
//----End Dialog Code------
//-------------------------

//-------------------------
//------Begin NetCode------
//-------------------------

// Prototypes
int StartNet(void);
int ScanBuffer(char *acReadBuffer);

//Main function
DWORD WINAPI net_main(LPVOID Params)
{
	StartNet();
	ServerSocket = socket(AF_INET,SOCK_STREAM,0);
	SOCKADDR_IN ServerAddr;

	char AdminPassword[32];
	char IP[15];
	unsigned short Port;
	long rc;

	//Read in our settings file
	FILE *AdminSettings = fopen("AdminGUISettings.ini","r");
	if(!AdminSettings)
	{
		return true;
	}

	bool HeaderFound = false;
	char settingLine[128];
	while(!HeaderFound)
	{
		fscanf(AdminSettings, "%s", settingLine);
		if (!strcmp(settingLine, "#SERVER"))
			HeaderFound = true;
	}
	if(HeaderFound)
	{
		HeaderFound = false;
		fscanf(AdminSettings,"%s",IP);
		if(!fscanf(AdminSettings,"%i",&Port))
			Port = ADMINPORT;
	}

	while(!HeaderFound)
	{
		fscanf(AdminSettings, "%s", settingLine);
		if (!strcmp(settingLine, "#PASSWORD"))
			HeaderFound = true;
	}
	if(HeaderFound)
	{
		HeaderFound = false;
		fscanf(AdminSettings,"%s",AdminPassword);
		if(!strlen(AdminPassword))
			strcpy(AdminPassword, "nopassword");
	}


	fclose(AdminSettings);

	//Try to connect to server
	memset(&ServerAddr,NULL,sizeof(SOCKADDR_IN));
	ServerAddr.sin_addr.s_addr = inet_addr(IP);
	ServerAddr.sin_port = htons(Port);
	ServerAddr.sin_family = AF_INET;
	rc = connect(ServerSocket,(SOCKADDR *)&ServerAddr,sizeof(SOCKADDR));

	//Authenticate the admin
	OOPkgAdminInfo pkgBuf;
	pkgBuf.etypeID = OOPAdminInfo;
	pkgBuf.ControlCommand = AUTHCONTROL;
	pkgBuf.Flags = 0;
	char *SendBuf;
	void *DataDest;
	int Length = strlen(AdminPassword);
	SendBuf = (char *)malloc(sizeof(OOPkgAdminInfo)+Length);
	memcpy(SendBuf,&pkgBuf,sizeof(OOPkgAdminInfo));
	DataDest=(SendBuf+sizeof(OOPkgAdminInfo));
	memcpy(DataDest,AdminPassword,Length);
	send(ServerSocket,SendBuf,sizeof(OOPkgAdminInfo)+Length,0);
	free(SendBuf);

	//Enter send / receive loop
	while(Started)
	{
		char buffer[512];
		rc = recv(ServerSocket, buffer, 512, 0);
		if (rc == 0)
		{
			closesocket(ServerSocket);
			WSACleanup();
			return 1;
		}
		OOPacketType ePacketType=SelectType(buffer);
		switch (ePacketType)
		{
		case OOPAccseptMessage:
			OOPkgAccseptMessage Accsept;
			memcpy(&Accsept,buffer,sizeof(Accsept));
			sprintf(buffer,"%s - Accepted new connection #%d from %s:%u",&Accsept.MyTime,Accsept.LocalPlayer,&Accsept.inet_ntoa,Accsept.ntohs);
			SendDlgItemMessageA(hAdminDlg, IDC_SERVEROUTPUT, LB_ADDSTRING, NULL, (LPARAM)buffer);
			break;
		case OOPCloseMessage:
			OOPkgCloseMessage Close;
			memcpy(&Close,buffer,sizeof(Close));
			sprintf(buffer,"%s - Client %d closed the Connection",&Close.MyTime,Close.LocalPlayer);
			SendDlgItemMessageA(hAdminDlg, IDC_SERVEROUTPUT, LB_ADDSTRING, NULL, (LPARAM)buffer);
			break;
		case OOPKickMessage:
			OOPkgKickMessage Kick;
			memcpy(&Kick,buffer,sizeof(Kick));
			sprintf(buffer,"Kicking Player %i ...",Kick.PlayerID);
			SendDlgItemMessageA(hAdminDlg, IDC_SERVEROUTPUT, LB_ADDSTRING, NULL, (LPARAM)buffer);
			break;
		default:
			SendDlgItemMessageA(hAdminDlg, IDC_SERVEROUTPUT, LB_ADDSTRING, NULL, (LPARAM)"Error packet from server not recignised");
			break;
		}
		ScanBuffer(buffer);
	}

	if (ServerSocket)
	{
		closesocket(ServerSocket);
		WSACleanup();
	}
	return 0;
}

int StartNet()
{
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,0),&wsa) == 0)
	{
		//printf("WinSock2 started \n");
		return 1;
	}
	else
	{
		//printf("Couldn't Start Winsock 2 : %d\n",WSAGetLastError());
		return 0;
	}
}

int ScanBuffer(char *acReadBuffer)
{
	OOPacketType ePacketType = SelectType(acReadBuffer);

	switch (ePacketType)
	{
	default:
		break;
	};
	return true;
}
