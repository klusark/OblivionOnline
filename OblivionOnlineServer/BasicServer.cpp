/*

Copyright 2007  Julian aka masterfreek64 and Joseph Pearson aka chessmaster42 

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
bool Connected[MAXCLIENTS];
unsigned short serverPort = 0;
FILE *easylog;
FILE *serverSettings;

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

	SOCKET acceptSocket;
	SOCKADDR_IN addr;
	FD_SET fdSet;

	
	easylog = fopen("Log.txt","w");

	serverSettings = fopen("ServerSettings.ini","r");
	if (serverSettings)
	{
		char portLine[100];
		fgets(portLine, 100, serverSettings);
		sscanf(portLine, "Port %u", &serverPort);
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
		printf("Listening for incoming connections\n");
		fprintf(easylog,"Server Up and running\n");
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
					sockaddr_in NewAddr;
					int nAddrSize = sizeof(NewAddr);
					clients[LocalPlayer]=accept(acceptSocket, (sockaddr*)&NewAddr, &nAddrSize);
					TotalClients++;
					printf("Accepted new connection #(%d) from %s:%u\n",LocalPlayer,inet_ntoa(NewAddr.sin_addr),ntohs(NewAddr.sin_port));
					fprintf(easylog,"Accepted new connection #(%d) from %s:%u\n",LocalPlayer,inet_ntoa(NewAddr.sin_addr),ntohs(NewAddr.sin_port));
					fprintf(easylog,"We now have %d connections\n",TotalClients);
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
					TotalClients--;
					printf("Client %d closed the Connection\n",LocalPlayer);
					fprintf(easylog,"Client %d closed the Connection\n",LocalPlayer);
					fprintf(easylog,"We now have %d connections\n",TotalClients);
					closesocket(clients[LocalPlayer]); 
					clients[LocalPlayer]=INVALID_SOCKET;
					Connected[LocalPlayer] = false;
				} 
				else
				{
					acReadBuffer[rc]='\0';	// Create a null-terminated string from buffer
					ScanBuffer(acReadBuffer, LocalPlayer);
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
	default: 
		OOPTimeUpdate_Handler(acReadBuffer,LocalPlayer);
		break;
	}
	return true;
}


void info(void *arg)
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2,0), &WSAData);

	SOCKET sock;
	SOCKADDR_IN sin;

	FILE *settings = fopen("settings.txt","r");
	char IP[16];
	char FILE[16];
	char NAME[16];
	fscanf(settings,"%s",IP);
	fscanf(settings,"%s",FILE);
	fscanf(settings,"%s",NAME);
	char * hi = "hi";
	char srequest[256];
	sprintf_s(srequest,256, "GET /%s?name=%s&port=%i&players=%i&maxplayers=%i HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", FILE,NAME,PORT,TotalClients,MAXCLIENTS,IP);

	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	sin.sin_addr.s_addr = inet_addr("192.168.0.5");
	sin.sin_family = AF_INET;
	sin.sin_port = htons(80);

	connect(sock, (SOCKADDR *)&sin, sizeof(sin)); 
	send(sock, srequest, strlen(srequest), 0); 

	closesocket(sock); 
	WSACleanup();
}
