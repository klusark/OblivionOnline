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

// BasicServer.cpp : Defines the entry point for the console application.

#include "BasicServer.h"
#include "OOPackets.h"
#include "PacketHandler.h"

// Globals
int TotalClients;
bool bServerAlive;
SOCKET clients[MAXCLIENTS];
char acSendBuffer[512];
PlayerStatus Players[MAXCLIENTS];
bool Connected[MAXCLIENTS];
unsigned short serverPort = 0;
FILE *easylog;
FILE *serverSettings;

// Prototypes
int StartNet(void);
int ScanBuffer(char *acReadBuffer, short LocalPlayer);
void info( void * );

//Main entry procedure
int main()
{
	_beginthread(info,0,(void*)1);

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

	// In den listen Modus
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
					Players[LocalPlayer].Health = 0;
					Players[LocalPlayer].Magika = 0;
					Players[LocalPlayer].Fatigue = 0;
				} 
				else
				{
					acReadBuffer[rc]='\0';	// Create a null-terminated string from buffer
					ScanBuffer(acReadBuffer, LocalPlayer);
					acSendBuffer[0] = '\0';
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
	default: 
		OOPTimeUpdate_Handler(acReadBuffer,LocalPlayer);
		break;
	}
	return true;
}


void info( void *arg )
{
	long rcs;
	rcs=StartNet();
	SOCKET server;
    SOCKADDR_IN local;
	server=socket(AF_INET,SOCK_STREAM,0);
	memset(&local,0,sizeof(SOCKADDR_IN));
    local.sin_family=AF_INET; //Address family
    local.sin_addr.s_addr=INADDR_ANY; //Wild card IP address
    local.sin_port=htons(serverPort-1); //port to use
	rcs=bind(server,(SOCKADDR*)&local,sizeof(SOCKADDR_IN));
		if(rcs==SOCKET_ERROR)
	{
		printf("Error calling bind: %d\n",WSAGetLastError());
	}
	rcs=listen(server,10);
		if(rcs==SOCKET_ERROR)
	{
		printf("Error calling listen: %d\n",WSAGetLastError());
	}
    SOCKET client;
	while(true)
	{
		sockaddr_in NewAddr;
		int nAddrSize = sizeof(NewAddr);
		client=accept(server, (sockaddr*)&NewAddr, &nAddrSize);
		char * temp="Text being sent back.";
		send( client, temp, strlen(temp), 0 );
		closesocket(client);
	}
}

