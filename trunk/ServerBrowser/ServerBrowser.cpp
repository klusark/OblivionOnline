/*

Copyright(c) 2007-2008   Julian Bangert aka masterfreek64 and Joseph Pearson aka chessmaster42 

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

// ServerBrowser.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#define CURL_STATICLIB


#include "ServerBrowser.h"
#include "libcurl/curl.h"
#include <queue>
#define NETWORKERROR(x) MessageBoxA(NULL,x,"Networking Error",0); WSACleanup();
#define MAX_LOADSTRING 100

// from curl example
#include "libcurl/types.h"
#include "libcurl/easy.h"
struct MemoryStruct {
  char *memory;
  size_t size;
  size_t read;
};


// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ServerAdd(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ServerStatusDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ServerBrowserDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
char *FetchStatus(char *ServerIP,unsigned long port);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	WSADATA wsa;
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SERVERBROWSER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERVERBROWSER));
	
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	//if(WSAStartup(MAKEWORD(2,0),&wsa)!=0)
	return (int) msg.wParam;
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVERBROWSER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_SERVERBROWSER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
      0, 0, 300, 60, NULL, NULL, hInstance, NULL);

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
		case ID_GAME:
			system("obse_loader");
			break;
		case ID_EDITOR:
			system("obse_loader -editor");
			break;
		case ID_SERVER_ADD:
			DialogBox(hInst,MAKEINTRESOURCE(IDD_SERVER_ADD),hWnd,ServerAdd);
			break;
		case ID_SERVERBROWSER:
			DialogBox(hInst,MAKEINTRESOURCE(IDD_SERVERBROWSER),hWnd,ServerBrowserDlg);
			break;
		case IDM_ABOUT:
			DialogBox(hInst,MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
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

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
//message Handler for "Add server" dialog
INT_PTR CALLBACK ServerAdd(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		INITCOMMONCONTROLSEX iccs;
		iccs.dwSize = sizeof(INITCOMMONCONTROLSEX);
		iccs.dwICC = ICC_INTERNET_CLASSES;
		return (INT_PTR)InitCommonControlsEx(&iccs);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK )
		{
			FILE *realmfile;
			DWORD dwTempAddress;
			int bFailure;
			unsigned int Port; //so we do not have an overflow. 
			SendDlgItemMessage(hDlg,IDC_SERVERIP,IPM_GETADDRESS,0,(LPARAM)&dwTempAddress);
			Port = GetDlgItemInt(hDlg,IDC_PORT,&bFailure,false);
			if(bFailure) //it worked, we got a number
			{
				if(Port <= 65535)
				{
					realmfile = fopen("realmlist.wth","w");
					if(realmfile)
					{
						fprintf(realmfile,"1 %d.%d.%d.%d %u",FIRST_IPADDRESS(dwTempAddress),SECOND_IPADDRESS(dwTempAddress),THIRD_IPADDRESS(dwTempAddress),FOURTH_IPADDRESS(dwTempAddress),Port);
						fclose(realmfile);
					}
					else
					{
						MessageBox(NULL,"Error - couldn't write realmlist.wth","ERROR",NULL);
					}
					EndDialog(hDlg, LOWORD(wParam));
				}
				else
				{
					MessageBoxA(NULL,"Port out of range, specify a port lower than 65536","ERROR",NULL);
				}
			}
			else
			{
				MessageBoxA(NULL,"Please enter a number","ERROR",NULL);
			}
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg,LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)TRUE;
}
// Message handler for Server Browser
INT_PTR CALLBACK ServerBrowserDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	
  CURL *curl_handle;


	int iNumServers, iServersReceived;
	FILE *TempFile; 
	switch (message)
	{
	case WM_INITDIALOG:
		if(TempFile)
		{

		  MessageBox(NULL,"Starting to download servers from Supertreks list. Programm may freeze.","Download",0);
		  curl_global_init(CURL_GLOBAL_ALL);

		  /* init the curl session */
		  curl_handle = curl_easy_init();

		  /* specify URL to get */
		  curl_easy_setopt(curl_handle, CURLOPT_URL, "http://71.164.181.152/list/raw.php");
		   curl_easy_setopt(curl_handle, CURLOPT_PORT, 82);
			
		  /* With a .dll we have to specify the pointer to fwrite ourselves */
		   curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, TempFile);
		  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, fwrite);
			
		  /* some servers don't like requests that are made without a user-agent
			 field, so we provide one */
		  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "OblivionOnlineLauncher/4.4");
		  /* We want no stupid output in a console :) */
		   curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1);
		  
		   
		  TempFile = fopen("ootemp1","a");;
		  /* get it! */
		  
			curl_easy_perform(curl_handle);
		  
		fclose(TempFile);
		  /* cleanup curl stuff */
		  curl_easy_cleanup(curl_handle);
		  
		 
		iServersReceived = 0;
		 // here we can read the data from the memory heap ...
		
		//SendDlgItemMessage(hDlg,IDC_SERVERS,LB_ADDSTRING,0,(LPARAM)"Showing Data from official server list by supertrek and stinger357");

		
		}
		else
		{
			MessageBoxA(NULL,"Could not open ootemp1","File Error",NULL);
		}
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
bool bNetAuthenticate(SOCKADDR_IN Server)
{
	WSADATA wsa;
	char szPath[MAX_PATH];
	FILE *Plugins_txt;
	WSAStartup(MAKEWORD(2,0),&wsa);
	SOCKET sock;
	short ModCount = 0;
	std::queue <std::string> ModQueue;
	Server.sin_family = AF_INET;
	sock = socket(AF_INET,SOCK_STREAM,0);

	if(SUCCEEDED(SHGetFolderPath(NULL, 
                             CSIDL_LOCAL_APPDATA, 
                             NULL, 
                             0, 
                             szPath))) 
	{
		PathAppend(szPath, TEXT("Oblivion\\Plugins.txt"));
		Plugins_txt=fopen(szPath,"r");
		if(Plugins_txt)
		{
			//read in Plugins.txt
			while(!feof(Plugins_txt))
			{
				char ModName[64];
				std::string TempString;
				fscanf(Plugins_txt,"%64s",ModName);
				TempString = ModName;
				ModQueue.push(TempString);
				ModCount++;					
			}
			if(connect(sock,(SOCKADDR *)&Server,sizeof(SOCKADDR_IN)) != SOCKET_ERROR)
			{
				ServerAuthPacket AuthPkg;
				ServerModAuthPacket ModAuthPkg;
				AuthPkg.PacketID = 65556; // the packet ID for this packet should be constant
				strcpy(AuthPkg.ClientName,"OblivionOnline Nick goes here"); // has to be implemented
				AuthPkg.ModCount = ModCount; 
				// NOTE:
				// WE IGNORE THE CRC32
				ModAuthPkg.PacketID = 65555; 
				send(sock,(char *)&AuthPkg,sizeof(ServerAuthPacket),0);
				for(unsigned int i = 1 ; i < ModQueue.size() ;i++) 
				{
					std::string TempString = ModQueue.front();
					sprintf(ModAuthPkg.EspName,"%64s",TempString.c_str());
					ModQueue.pop();
					ModAuthPkg.ModOrder = i;
					send(sock,(char *)&ModAuthPkg,sizeof(ServerModAuthPacket),0);
				}
				//Somehow await the data
			}
			else
			{
				MessageBoxA(NULL,"Could not connect to server","Authentication Error",NULL);
			}
			//Authenticate on the network
		}
		else
		{
			MessageBoxA(NULL,"Could not open Plugins.txt , check that Oblivion is installed","Authentication Error",NULL);
		}

	}
	return false;// by default we could not authenticate
}