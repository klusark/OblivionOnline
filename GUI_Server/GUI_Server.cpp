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

#include "GUI_Server.h"
#include "OOPackets.h"
#include "PacketHandler.h"

//-------------------------
//----Begin Dialog Code----
//-------------------------

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Global Server Variables
uintptr_t hServerThread;
bool bServerAlive = true;
char commandString[256];
int TotalClients = 0;
SOCKET clients[MAXCLIENTS];
sockaddr_in ConnectionInfo[MAXCLIENTS];
PlayerStatus Players[MAXCLIENTS];
PlayerStatus PlayersInitial[MAXCLIENTS];
UInt8 ModList[MAXCLIENTS][255];
bool Connected[MAXCLIENTS];
unsigned short serverPort = 0;
FILE *easylog;
FILE *serverSettings;
char serverMsg[256];
HWND hServerDlg;

//Prototypes for GUI functions
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	ServerConsoleDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

//Prototypes for main server
void serverThread(void *arg);
int server_main(HWND hDlg);
bool BroadcastMessage(char *Message, int Player);
bool Kick(int Player);

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
			bServerAlive = true;
			break;
		case ID_SERVER_STOP:
			bServerAlive = false;
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
		hServerDlg = hDlg;

		//If server is not started, start it up
		if (!hServerThread)
		{
			hServerThread = _beginthread(serverThread, 0, hServerDlg);
		}

		//Fill up the player list
		for(int i=0; i<MAXCLIENTS; i++)
			if (Connected[i])
			{
				sprintf(serverMsg, "Player %2i - %s:%u",i,inet_ntoa(ConnectionInfo[i].sin_addr),ntohs(ConnectionInfo[i].sin_port));
				SendDlgItemMessageA(hServerDlg,IDC_PLAYERLIST,LB_ADDSTRING,0,(LPARAM)serverMsg);
			}
		return true;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_KICK:
			{int KPlayer = (int)SendDlgItemMessageA(hServerDlg,IDC_PLAYERLIST,LB_GETCURSEL, 0, 0);
			char KPlayerName[64];
			SendDlgItemMessageA(hServerDlg,IDC_PLAYERLIST,LB_GETTEXT,KPlayer,(LPARAM)KPlayerName);
			sscanf(KPlayerName, "Player %i", &KPlayer);
			Kick(KPlayer);}
			return true;
		case IDC_CLOSE:
			EndDialog(hDlg, LOWORD(wParam));
			return true;
		case IDC_SEND:
			GetDlgItemTextA(hDlg, IDC_COMMAND_ENTER, (LPSTR)commandString, 256);
			bool AllChat = IsDlgButtonChecked(hDlg, IDC_CHATALL);
			if (AllChat)
				BroadcastMessage(commandString, -1);
			else{
				int bSuccess;
				int PlayerNum = GetDlgItemInt(hDlg, IDC_CHATPLAYERNUMBER, &bSuccess, false);
				if (bSuccess)
				{
					if (PlayerNum < TotalClients && PlayerNum >= 0)
					{
						BroadcastMessage(commandString, PlayerNum);
					}else{
						MessageBoxA(NULL, "Player number out of range", "Error", NULL);
						return true;
					}
				}
			}
			sprintf(serverMsg, "Server: %s", commandString);
			SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
			SetDlgItemTextA(hDlg, IDC_COMMAND_ENTER, NULL);
			return true;
		};
		break;
	}
	return false;
}

//-------------------------
//----End Dialog Code------
//-------------------------

//-------------------------
//----Begin Server Code----
//-------------------------

//Serverlist settings variables
char LISTHOST[32];
char LISTFILE[16];
char LISTNAME[32];
char ServerPassword[32];

// Prototypes
int StartNet(void);
int ScanBuffer(char *acReadBuffer, short LocalPlayer);
void info(void *);

//Main server thread
void serverThread(void *arg)
{
	hServerDlg = (HWND)arg;
	server_main(hServerDlg);
}

//Main entry procedure
int server_main(HWND hDlg)
{
	_beginthread(info,0,NULL);
	
	// Setup our local variables
	short LocalPlayer;
	long rc;
	bool bSendPackage;
	char acReadBuffer[512];
	for(int i=0; i<MAXCLIENTS; i++)
	{
		Players[i].PosX = 0;
		Players[i].PosY = 0;
		Players[i].PosZ = -196;
		Players[i].RotX = 0;
		Players[i].RotY = 0;
		Players[i].RotZ = 0;
		Players[i].CellID = 0;

		Players[i].Health = 1;
		Players[i].Magika = 0;
		Players[i].Fatigue = 0;

		Players[i].hair = 0;

		Connected[i] = false;
	}

	Sleep(100);

	sprintf(serverMsg, "OblivionOnline Basic Server, v.%i.%i.%i",SUPER_VERSION,MAIN_VERSION,SUB_VERSION);
	SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
	sprintf(serverMsg, "Wrtten by masterfreek64 aka Julian Bangert, Chessmaster42 aka Joseph Pearson");
	SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
	sprintf(serverMsg, "and bobjr777 aka Joel Teichroeb");
	SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
	sprintf(serverMsg, "--------------------------");
	SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
	serverMsg[0] = '\0';
	SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);

	SOCKET acceptSocket;
	SOCKADDR_IN addr;
	FD_SET fdSet;

	//Clean out the server log
	easylog = fopen("Log.txt","w");
	fclose(easylog);

	serverSettings = fopen("ServerSettings.ini","r");
	if (serverSettings)
	{
		bool portFound = false;
		char settingLine[128];
		while(!portFound)
		{
			fscanf(serverSettings, "%s", settingLine);
			if (!strcmp(settingLine, "#PORT"))
				portFound = true;
		}
		fscanf(serverSettings, "%u", &serverPort);
		if (!serverPort || serverPort < 1024)
			serverPort = PORT;
		sprintf(serverMsg, "Opening on port %u", serverPort);
		SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
	}else{
		serverPort = PORT;
		sprintf(serverMsg, "ServerSettings.ini not found. Using default port.");
		SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
	}

	bool passwordFound = false;
	char settingLine[128];
	while(!passwordFound)
	{
		fscanf(serverSettings, "%s", settingLine);
		if (!strcmp(settingLine, "#PASSWORD"))
			passwordFound = true;
	}
	fscanf(serverSettings, "%s", &ServerPassword);

	//If no pw was in the file, use default
	if(!strlen(ServerPassword))
		strcpy(ServerPassword, "nopassword");

	// start WinSock
	rc=StartNet();
	// create Socket
	acceptSocket=socket(AF_INET,SOCK_STREAM,0);

	if(acceptSocket==INVALID_SOCKET)
	{
		sprintf(serverMsg, "Error the AcceptSocket couldn't be created: %d",WSAGetLastError());
		SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
		return 1;
	}
	else
	{
		sprintf(serverMsg, "Created Socket-");
		SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
	}

	memset(&addr,0,sizeof(SOCKADDR_IN));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(serverPort);
	addr.sin_addr.s_addr=INADDR_ANY;
	rc=bind(acceptSocket,(SOCKADDR*)&addr,sizeof(SOCKADDR_IN));
	if(rc==SOCKET_ERROR)
	{
		sprintf(serverMsg, "Error calling bind: %d",WSAGetLastError());
		SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
		return 1;
	}
	else
	{
		sprintf(serverMsg, "Socket bound-");
		SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
	}

	// Start listener
	rc=listen(acceptSocket,10);
	if(rc==SOCKET_ERROR)
	{
		sprintf(serverMsg, "Error calling listen: %d",WSAGetLastError());
		SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
		return 1;
	}
	else
	{
		time_t TimeStamp;
		time(&TimeStamp);
		int Seconds = (int)TimeStamp % 60;
		int Minutes = (int)(TimeStamp / 60) % 60;
		int Hours = (int)(TimeStamp / 3600) % 24;
		char MyTime[8];
		sprintf(MyTime, "%2i:%2i:%2i", Hours, Minutes, Seconds);

		sprintf(serverMsg, "Listening for incoming connections");
		SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
		easylog = fopen("Log.txt","a");
		fprintf(easylog,"%s - Server Up and running\n",MyTime);
		fclose(easylog);
	}
	for(LocalPlayer=0;LocalPlayer<MAXCLIENTS;LocalPlayer++) 
	{
		clients[LocalPlayer]=INVALID_SOCKET;
	}
	while(bServerAlive) 
	{
		bSendPackage = false;

		FD_ZERO(&fdSet); // clean
		FD_SET(acceptSocket,&fdSet); // Add acceptsocket

		for(LocalPlayer=0;LocalPlayer<MAXCLIENTS;LocalPlayer++) 
		{
			if(clients[LocalPlayer]!=INVALID_SOCKET)
			{
				FD_SET(clients[LocalPlayer],&fdSet);
			}
		}
		rc=select(0,&fdSet,NULL,NULL,NULL); 
		if(rc==SOCKET_ERROR) 
		{
			//printf("Error: select, error code: %s\n",WSAGetLastError());
			return 1;
		}
    
    
		if(FD_ISSET(acceptSocket,&fdSet))
		{
			for(LocalPlayer=0;LocalPlayer<MAXCLIENTS;LocalPlayer++)
			{
				if(clients[LocalPlayer]==INVALID_SOCKET) 
				{
					time_t TimeStamp;
					time(&TimeStamp);
					int Seconds = (int)TimeStamp % 60;
					int Minutes = (int)(TimeStamp / 60) % 60;
					int Hours = (int)(TimeStamp / 3600) % 24;
					char MyTime[8];
					sprintf(MyTime, "%2i:%2i:%2i", Hours, Minutes, Seconds);

					sockaddr_in NewAddr;
					int nAddrSize = sizeof(NewAddr);
					clients[LocalPlayer]=accept(acceptSocket, (sockaddr*)&NewAddr, &nAddrSize);
					ConnectionInfo[LocalPlayer] = NewAddr;
					TotalClients++;
					sprintf(serverMsg, "%s - Accepted new connection #%d from %s:%u",MyTime,LocalPlayer,inet_ntoa(NewAddr.sin_addr),ntohs(NewAddr.sin_port));
					SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
					easylog = fopen("Log.txt","a");
					fprintf(easylog,"%s - Accepted new connection #(%d) from %s:%u\n",MyTime,LocalPlayer,inet_ntoa(NewAddr.sin_addr),ntohs(NewAddr.sin_port));
					fprintf(easylog,"%s - We now have %d connections\n",MyTime,TotalClients);
					fclose(easylog);
					break;
				}
			}
		}

		// Check which Sockets are in the FDS
		for(LocalPlayer=0;LocalPlayer<MAXCLIENTS;LocalPlayer++) 
		{
			if(clients[LocalPlayer]==INVALID_SOCKET)
			{
				continue; 
			}
			if(FD_ISSET(clients[LocalPlayer],&fdSet))
			{
				acReadBuffer[0] = '\0';
				rc=recv(clients[LocalPlayer],acReadBuffer,256,0);
		
				if(rc==0 || rc==SOCKET_ERROR)
				{
					OOPkgDisconnect OutPkgBuf;
					OutPkgBuf.etypeID = OOPDisconnect;
					OutPkgBuf.PlayerID = LocalPlayer;
					for(int cx=0;cx<MAXCLIENTS;cx++)
					{
						if (cx != LocalPlayer)
							send(clients[cx],(char *)&OutPkgBuf,sizeof(OOPkgDisconnect),0);
					}

					time_t TimeStamp;
					time(&TimeStamp);
					int Seconds = (int)TimeStamp % 60;
					int Minutes = (int)(TimeStamp / 60) % 60;
					int Hours = (int)(TimeStamp / 3600) % 24;
					char MyTime[8];
					sprintf(MyTime, "%2i:%2i:%2i", Hours, Minutes, Seconds);

					TotalClients--;
					sprintf(serverMsg, "%s - Client %d closed the Connection",MyTime,LocalPlayer);
					SendDlgItemMessageA(hDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
					for(int i=0; i<MAXCLIENTS; i++)
					{
						// Now search for the player in the dialog list
						sprintf(serverMsg, "Player %2i",LocalPlayer);
						int PlayerIdx = SendDlgItemMessageA(hDlg,IDC_PLAYERLIST,LB_FINDSTRING,-1,(LPARAM)serverMsg);
						if (PlayerIdx != LB_ERR)
							SendDlgItemMessageA(hDlg,IDC_PLAYERLIST,LB_DELETESTRING,PlayerIdx,0);
					}
					easylog = fopen("Log.txt","a");
					fprintf(easylog,"%s - Client %d closed the Connection\n",MyTime,LocalPlayer);
					fprintf(easylog,"%s - We now have %d connection(s)\n",MyTime,TotalClients);
					fclose(easylog);
					closesocket(clients[LocalPlayer]); 
					clients[LocalPlayer]=INVALID_SOCKET;
					Connected[LocalPlayer] = false;
				} 
				else
					ScanBuffer(acReadBuffer, LocalPlayer);
			}
		}
	}
	fclose(easylog);
}

int StartNet()
{
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,0),&wsa) == 0)
	{
		//printf("WinSock2 started \n");
		bServerAlive = true;
		return 1;
	}
	else
	{
		//printf("Couldn't Start Winsock 2 : %d\n",WSAGetLastError());
		return 0;
	}
}

int ScanBuffer(char *acReadBuffer, short LocalPlayer)
{
	OOPacketType ePacketType = SelectType(acReadBuffer);

	switch (ePacketType)
	{
	case OOPWelcome:
		if (!Connected[LocalPlayer])
			OOPWelcome_Handler(acReadBuffer,LocalPlayer);
		break;
	case OOPDisconnect:
		OOPDisconnect_Handler(acReadBuffer,LocalPlayer);
		break;
	case OOPActorUpdate:
		OOPActorUpdate_Handler(acReadBuffer,LocalPlayer);
		break;
	case OOPChat:
		OOPChat_Handler(acReadBuffer,LocalPlayer);
		break;
	case OOPEvent:
		OOPEvent_Handler(acReadBuffer,LocalPlayer);
		break;
	case OOPEventRegister:
		OOPEventRegister_Handler(acReadBuffer,LocalPlayer);
		break;
	case OOPFullStatUpdate:
		OOPFullStatUpdate_Handler(acReadBuffer,LocalPlayer);
		break;
	case OOPEquipped:
		OOPEquipped_Handler(acReadBuffer,LocalPlayer);
		break;
	case OOPModOffsetList:
		OOPModOffsetList_Handler(acReadBuffer,LocalPlayer);
		break;
	default: 
		OOPTimeUpdate_Handler(acReadBuffer,LocalPlayer);
		break;
	}
	return true;
}

void info(void *arg)
{
	FILE *settings = fopen("ServerSettings.ini","r");
	if (settings)
	{
		while(serverPort==0)
			Sleep(50);
		WSADATA WSAData;
		WSAStartup(MAKEWORD(2,0), &WSAData);
		SOCKET sock;
		SOCKADDR_IN sin;

		bool listSettingsFound = false;
		char settingLine[128];
		while(!listSettingsFound)
		{
			fscanf(settings, "%s", settingLine);
			if (!strcmp(settingLine, "#LISTSETTINGS"))
				listSettingsFound = true;
		}
		fscanf(settings,"%s",LISTHOST);
		fscanf(settings,"%s",LISTFILE);
		fscanf(settings,"%s",LISTNAME);

		struct hostent *he;
		while((he = gethostbyname(LISTHOST)) == NULL)
		{
			//printf("Error resolving serverlist hostname. Retrying in 60 seconds.");
			Sleep(60000);	//Sleep for 60 seconds and then try again
		}

		long rcs;
		while(true){
			char srequest[384];
			sprintf_s(srequest,384, "GET /%s?name=%s&port=%u&players=%i&maxplayers=%i&VersionMajor=%i&VersionMinor=%i HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", LISTFILE, LISTNAME, serverPort, TotalClients, MAXCLIENTS, MAIN_VERSION, SUB_VERSION, LISTHOST);
			
			sock = socket(AF_INET, SOCK_STREAM, 0);
			memcpy(&sin.sin_addr, he->h_addr_list[0], he->h_length);
			//sin.sin_addr.s_addr = inet_addr(IP);
			sin.sin_family = AF_INET;
			sin.sin_port = htons(80);

			rcs=connect(sock, (SOCKADDR *)&sin, sizeof(sin)); 
			if(rcs==SOCKET_ERROR) 
			{
				//printf("Error: Serverlist connect error, code: %i\n",WSAGetLastError());
			}
			rcs=send(sock, srequest, 384, 0);
			if(rcs==SOCKET_ERROR) 
			{
				//printf("Error: Serverlist send error, code: %i\n",WSAGetLastError());
			}
			closesocket(sock); 
			Sleep(120000);
		}
		WSACleanup();
	}else{
		//printf("ServerSettings.ini not found. This server will not be listed online.\n");
	}	
}

bool BroadcastMessage(char *Message, int Player)
{
	//Send out the chat message
	OOPkgChat pkgBuf;
	char *SendBuf;
	void *MessageDest;
	pkgBuf.etypeID = OOPChat;
	pkgBuf.refID = 1337;
	pkgBuf.Length = strlen(Message);
	pkgBuf.Flags = 0;
	SendBuf = (char *)malloc(sizeof(OOPkgChat)+pkgBuf.Length);
	memcpy(SendBuf,&pkgBuf,sizeof(OOPkgChat));
	MessageDest=(SendBuf+sizeof(OOPkgChat));
	memcpy(MessageDest,Message,pkgBuf.Length);
	if (Player == -1)
	{
		for(int cx=0;cx<MAXCLIENTS;cx++)
		{
			send(clients[cx],SendBuf,sizeof(OOPkgChat)+pkgBuf.Length,0);
		}
	}else
		send(clients[Player],SendBuf,sizeof(OOPkgChat)+pkgBuf.Length,0);
	free(SendBuf);
	return true;
}

bool Kick(int Player)
{
	if (Connected[Player])
	{
		OOPkgDisconnect OutPkgBuf;
		OutPkgBuf.PlayerID = Player;
		OutPkgBuf.etypeID = OOPDisconnect;
		OutPkgBuf.Flags = 1;
		sprintf(serverMsg, "Kicking Player %i ...", OutPkgBuf.PlayerID);
		SendDlgItemMessageA(hServerDlg,IDC_SERVEROUTPUT,LB_ADDSTRING,0,(LPARAM)serverMsg);
		for(int cx=0;cx<MAXCLIENTS;cx++)
		{
			send(clients[cx],(char *)&OutPkgBuf,sizeof(OOPkgDisconnect),0);
		}
		for(int i=0; i<MAXCLIENTS; i++)
		{
			// Now search for the player in the dialog list
			sprintf(serverMsg, "Player %2i",Player);
			int PlayerIdx = SendDlgItemMessageA(hServerDlg,IDC_PLAYERLIST,LB_FINDSTRING,-1,(LPARAM)serverMsg);
			if (PlayerIdx != LB_ERR)
				SendDlgItemMessageA(hServerDlg,IDC_PLAYERLIST,LB_DELETESTRING,PlayerIdx,0);
		}
	}
	return true;
}
