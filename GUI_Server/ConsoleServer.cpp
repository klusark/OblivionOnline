/*

Copyright 2007   Julian Bangert aka masterfreek64 and Joseph Pearson aka chessmaster42 and Joel Teichroeb aka bobjr777

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

#include "ConsoleServer.h"
#include "OOPackets.h"
#include "PacketHandler.h"

// Global Server Variables
bool bServerAlive = true;
int TotalClients = 0;
SOCKET clients[MAXCLIENTS];
SOCKET adminSocket;
sockaddr_in ConnectionInfo[MAXCLIENTS];
PlayerStatus Players[MAXCLIENTS];
PlayerStatus PlayersInitial[MAXCLIENTS];

bool Connected[MAXCLIENTS];
unsigned short serverPort = 0;
FILE *easylog;
FILE *serverSettings;

//Serverlist settings variables
char LISTHOST[32];
char LISTFILE[16];
char LISTNAME[32];
char ServerPassword[32];
char AdminPassword[32];

//Mob Synch Variables
int MasterClient = -1;
//Initially we are just storing 2 pointers , so this can be 24kb.... it autosizes quite well
OOHashTable MobTable(3000);  // This value generates a good spread, especially with 16 potences ... also we should about double the number of values necessary to avoid collisions. 
//The hashing algorithm here relies on the fact that we do a) not have a string hash , but a good modulo hash
// Therefore we need sequential FormIDs to create a good spread
// b) lots and lots of memory- time tradeoffs


// Prototypes
int StartNet(void);
int ScanBuffer(char *acReadBuffer, short LocalPlayer, short nBytesRead);
void info(void *);
void adminthread(void *);
typedef std::pair< std::string, std::string > UserPasswordPair;


//Main entry procedure
int main(void)
{
	_beginthread(info,0,NULL);
	_beginthread(adminthread,0,NULL);
	
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

	printf("OblivionOnline Basic Server, v.%i.%i.%i\"%s \" %s \n \n",SUPER_VERSION,MAIN_VERSION,SUB_VERSION,RELEASE_CODENAME,RELEASE_COMMENT);
	printf("Wrtten by masterfreek64 aka  Julian Bangert Bangert, Chessmaster42 aka Joseph Pearson\n");
	printf("and bobjr777 aka Joel Teichroeb\n");
	printf("--------------------------\n");

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
		bool passwordFound = false;
		while(!passwordFound)
		{
			fscanf(serverSettings, "%s", settingLine);
			if (!strcmp(settingLine, "#PASSWORD"))
				passwordFound = true;
		}
		fscanf(serverSettings, "%s", &ServerPassword);
		bool adminPasswordFound = false;
		while(!adminPasswordFound)
		{
			fscanf(serverSettings, "%s", settingLine);
			if (!strcmp(settingLine, "#ADMINPASSWORD"))
				adminPasswordFound = true;
		}
		fscanf(serverSettings, "%s", &AdminPassword);
	}else{
		serverPort = PORT;
		printf("ServerSettings.ini not found. Using default port.\n");
	}


	//If no pw was in the file, use default
	if(!strlen(ServerPassword))
		strcpy(ServerPassword, "");

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
					ConnectionInfo[LocalPlayer] = NewAddr;
					TotalClients++;
					printf("%s - Accepted new connection #%d from %s:%u\n",MyTime,LocalPlayer,inet_ntoa(NewAddr.sin_addr),ntohs(NewAddr.sin_port));

					if(adminSocket){
						char message[256];
						sprintf(message,"%s - Accepted new connection #%d from %s:%u",MyTime,LocalPlayer,inet_ntoa(NewAddr.sin_addr),ntohs(NewAddr.sin_port));
						SendAdminMessage(message);
					}
					

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
			}else{
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
					if(adminSocket){
						char message[256];
						sprintf(message,"%s - Client %d closed the Connection",MyTime,LocalPlayer);
						SendAdminMessage(message);
					}
					if(LocalPlayer == MasterClient)
					{
						// Remove him and select a new master client
						for(int i = 0;i < MAXCLIENTS;i++)
						{
							if(Connected[i])
							{
								MasterClient = i; // update that on the net
								OOPkgWelcome OutPkgBuf;
								OutPkgBuf.etypeID = OOPWelcome;
								OutPkgBuf.Flags = 2 | 4;
								send(clients[LocalPlayer],(char *)&OutPkgBuf,sizeof(OOPkgWelcome),0);
							}
						}
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
					ScanBuffer(acReadBuffer, LocalPlayer, rc);
			}
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

int ScanBuffer(char *acReadBuffer, short LocalPlayer, short nBytesRead)
{
	printf("%u",*acReadBuffer);
	OOPacketType ePacketType = SelectType(acReadBuffer);
	//If this is run from the admin thread
	/*if(LocalPlayer == -1)
	{
		switch (ePacketType)
		{
		case OOPAdminInfo:
			OOPAdminInfo_Handler(acReadBuffer, nBytesRead);
			break;
		default:
			break;
		};
		return true;
	}*/
	/*
	//Only welcome is allowed for non-auth clients
	/*
	if(!Authenticated[LocalPlayer])
	{
		switch (ePacketType)
		{
		case OOPWelcome:
			OOPWelcome_Handler(acReadBuffer,LocalPlayer);
			break;
		default:
			break;
		};
		return true;
	}
	*/
	switch (ePacketType)
	{
	case OOPWelcome:
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
			printf("Error resolving serverlist hostname. Retrying in 60 seconds.\n");
			Sleep(60000);	//Sleep for 60 seconds and then try again
		}

		long rcs;
		bool HasPassword;
		if(!strcmp(ServerPassword, "nopassword"))
			HasPassword = false;
		else
			HasPassword = true;

		while(true){

			// THIS IS SOOOO UGLY !!!! Replace y a proper libcurl implementation
			char srequest[384];
			sprintf(srequest, "GET /%s?name=%s&port=%u&players=%i&maxplayers=%i&VersionMajor=%i&VersionMinor=%i&HasPassword=%i HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", LISTFILE, LISTNAME, serverPort, TotalClients, MAXCLIENTS, MAIN_VERSION, SUB_VERSION, HasPassword, LISTHOST);
			
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
			rcs=send(sock, srequest, strlen(srequest), 0);
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

void adminthread(void *arg)
{
	//Wait until we have a valid server port
	while(serverPort == 0)
		Sleep(50);

	//Initialize winsock
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2,0), &WSAData);

	//Setup socket
	SOCKET acceptSocket;
	SOCKADDR_IN sinLocal;
	acceptSocket=socket(AF_INET,SOCK_STREAM,0);

	//Setup bind and listen on socket
	memset(&sinLocal,0,sizeof(SOCKADDR_IN));
	sinLocal.sin_family=AF_INET;
	sinLocal.sin_port=htons(serverPort-1);//This should be fixed and customised. On most NATs this port won' be routed. Instead we should remove all Winsock Code, add the packages to the main packet stream , and route them through a std::queue with Mutex
	sinLocal.sin_addr.s_addr=INADDR_ANY;
	int rc = bind(acceptSocket,(SOCKADDR*)&sinLocal,sizeof(SOCKADDR_IN));
	if (rc == SOCKET_ERROR)
	{
		printf("Error on bind socket for remote admin.\n");
		return;
	}
	rc = listen(acceptSocket,10);
	if (rc == SOCKET_ERROR)
	{
		printf("Error on listen socket for remote admin.\n");
		return;
	}

	while(true)
	{
		//Wait for remote connection
		SOCKADDR_IN sinRemote;
		int nAddrSize = sizeof(SOCKADDR_IN);
		adminSocket = accept(acceptSocket, (sockaddr*)&sinRemote, &nAddrSize);

		time_t TimeStamp;
		time(&TimeStamp);
		int Seconds = (int)TimeStamp % 60;
		int Minutes = (int)(TimeStamp / 60) % 60;
		int Hours = (int)(TimeStamp / 3600) % 24;
		char MyTime[8];
		sprintf(MyTime, "%2i:%2i:%2i", Hours, Minutes, Seconds);
		printf("%s - Admin connected from %s:%i\n", MyTime, inet_ntoa(sinRemote.sin_addr), ntohs(sinRemote.sin_port));
		easylog = fopen("Log.txt","a");
		fprintf(easylog,"%s - Admin connected from %s:%i\n", MyTime, inet_ntoa(sinRemote.sin_addr), ntohs(sinRemote.sin_port));
		fclose(easylog);

		//Enter the send / receive loop
		char acReadBuffer[512];
		int nReadBytes;
		do {
			//Read in data from client
			nReadBytes = recv(adminSocket, acReadBuffer, 512, 0);
			if (nReadBytes > 0)
			{
				ScanBuffer(acReadBuffer, -1, nReadBytes);
			}else
				break;
		} while (nReadBytes != 0);
		time(&TimeStamp);
		Seconds = (int)TimeStamp % 60;
		Minutes = (int)(TimeStamp / 60) % 60;
		Hours = (int)(TimeStamp / 3600) % 24;
		sprintf(MyTime, "%2i:%2i:%2i", Hours, Minutes, Seconds);
		printf("%s - Admin disconnected from %s:%i\n", MyTime, inet_ntoa(sinRemote.sin_addr), ntohs(sinRemote.sin_port));
		easylog = fopen("Log.txt","a");
		fprintf(easylog,"%s - Admin disconnected from %s:%i\n", MyTime, inet_ntoa(sinRemote.sin_addr), ntohs(sinRemote.sin_port));
		fclose(easylog);
	}

	//Clean up
	closesocket(adminSocket);
	WSACleanup();
}


bool BroadcastMessage(char *Message, int Player)
{
	//Print message on console
	printf("Server: %s\n", Message);

	//Send out the chat message
	OOPkgChat pkgBuf;
	char *SendBuf;
	void *MessageDest;
	pkgBuf.etypeID = OOPChat;
	pkgBuf.refID = 1337;
	pkgBuf.Length = (int)strlen(Message);
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
	if (Player >= MAXCLIENTS)
		return false;
	if (Connected[Player])
	{
		OOPkgDisconnect OutPkgBuf;
		OutPkgBuf.PlayerID = Player;
		OutPkgBuf.etypeID = OOPDisconnect;
		OutPkgBuf.Flags = 1;
		printf("Kicking Player %i ...\n", OutPkgBuf.PlayerID);
		for(int cx=0;cx<MAXCLIENTS;cx++)
		{
			send(clients[cx],(char *)&OutPkgBuf,sizeof(OOPkgDisconnect),0);
		}
	}
	return true;
}

int SendAdminMessage(char message[256])
{
	OOPkgAdminMessage AdminMessage;
	AdminMessage.etypeID=OOPAdminMessage;
	strcpy(AdminMessage.message,message);
	long rc;
	rc=send(adminSocket,(char *)&AdminMessage,sizeof(AdminMessage),0);
		if (rc == SOCKET_ERROR)
	{
		printf("Error on send for remote admin.\n");
		return 0;
	}
	return 1;
}