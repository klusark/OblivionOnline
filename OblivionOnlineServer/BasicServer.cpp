/*

Copyright 2007  Julian aka masterfreek64, Joseph Pearson aka chessmaster42 and Joel Teichroeb aka bobjr777

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

#include "BasicServer.h"
#include "OOPackets.h"
#include "PacketHandler.h"

// Globals
int TotalClients = 0;
bool bServerAlive;
SOCKET clients[MAXCLIENTS];
PlayerStatus Players[MAXCLIENTS];
PlayerStatus PlayersInitial[MAXCLIENTS];
UInt8 ModList[MAXCLIENTS][255];
bool Connected[MAXCLIENTS];
unsigned short serverPort = 0;
FILE *easylog;
FILE *serverSettings;

//Server settings variables
char LISTHOST[32];
char LISTFILE[16];
char LISTNAME[32];


// Prototypes
int StartNet(void);
int ScanBuffer(char *acReadBuffer, short LocalPlayer);
void info(void *);

//Main entry procedure
int main()
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
	printf("OblivionOnline Basic Server, v.%i.%i.%i\n",SUPER_VERSION,MAIN_VERSION,SUB_VERSION);
	printf("Wrtten by masterfreek64 aka Julian Bangert, Chessmaster42 aka Joseph Pearson and bobjr777 aka Joel Teichroeb \n");
	printf("--------------------------\n\n");
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
		printf("Opening on port %u\n", serverPort);
	}
	else
		serverPort = PORT;

	// start WinSock
	rc=StartNet();
	// create Socket
	acceptSocket=socket(AF_INET,SOCK_STREAM,0);

	if(acceptSocket==INVALID_SOCKET)
	{
		printf("Error the AcceptSocket couldn't be created: %d\n",WSAGetLastError());
		return 1;
	}
	else
	{
		printf("Created Socket-");
	}

	memset(&addr,0,sizeof(SOCKADDR_IN));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(serverPort);
	addr.sin_addr.s_addr=INADDR_ANY;
	rc=bind(acceptSocket,(SOCKADDR*)&addr,sizeof(SOCKADDR_IN));
	if(rc==SOCKET_ERROR)
	{
		printf("Error calling bind: %d\n",WSAGetLastError());
		return 1;
	}
	else
	{
		printf("Socket bound-");
	}

	// Start listener
	rc=listen(acceptSocket,10);
	if(rc==SOCKET_ERROR)
	{
		printf("Error calling listen: %d\n",WSAGetLastError());
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

		printf("Listening for incoming connections\n");
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
			printf("Error: select, error code: %s\n",WSAGetLastError());
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
					TotalClients++;
					printf("%s - Accepted new connection #%d from %s:%u\n",MyTime,LocalPlayer,inet_ntoa(NewAddr.sin_addr),ntohs(NewAddr.sin_port));
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
					printf("%s - Client %d closed the Connection\n",MyTime,LocalPlayer);
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
		printf("WinSock2 started \n");
		bServerAlive = true;
		return 1;
	}
	else
	{
		printf("Couldn't Start Winsock 2 : %d\n",WSAGetLastError());
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
			printf("Error resolving serverlist hostname. Retrying in 60 seconds.");
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
				printf("Error: Serverlist connect error, code: %i\n",WSAGetLastError());
			}
			rcs=send(sock, srequest, 384, 0);
			if(rcs==SOCKET_ERROR) 
			{
				printf("Error: Serverlist send error, code: %i\n",WSAGetLastError());
			}
			closesocket(sock); 
			Sleep(120000);
		}
		WSACleanup();
	}else{
		printf("ServerSettings.ini not found. This server will not be listed online.\n");
	}
	
}

bool BroadcastMessage(char *Message);
{
	OOPkgChat pkgBuf;
	char *SendBuf;
	void *MessageDest;
	pkgBuf.etypeID = OOPChat;
	pkgBuf.refID = LocalPlayer;
	pkgBuf.Length = strlen(Message);
	pkgBuf.Flags = 0;
	SendBuf = (char *)malloc(sizeof(OOPkgChat)+pkgBuf.Length);
	memcpy(SendBuf,&pkgBuf,sizeof(OOPkgChat));
	MessageDest=(SendBuf+sizeof(OOPkgChat));
	memcpy(MessageDest,Message,pkgBuf.Length);
	for(int cx = 0;cx< MAXCLIENTS;cx++)

	send(ServerSocket,SendBuf,sizeof(OOPkgChat)+pkgBuf.Length,0);
	free(SendBuf);
	return true;
}