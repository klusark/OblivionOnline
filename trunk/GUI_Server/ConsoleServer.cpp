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
#include "IOSystem.h"
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
unsigned short adminPort = 0;
FILE *easylog;
FILE *serverSettings;

//Serverlist settings variables
char ListURI[128];

char ServerName[32];
char ServerPassword[32];
char AdminPassword[32];


lua_State* g_LuaInstance = NULL;
//Mob Synch Variables
int MasterClient = -1;
//Initially we are just storing 2 pointers , so this can be 24kb.... it autosizes quite well
OOHashTable MobTable(3000);  // This value generates a good spread, especially with 16 potences ... also we should about double the number of values necessary to avoid collisions. 
//The hashing algorithm here relies on the fact that we do a) not have a string hash , but a good modulo hash
// Therefore we need sequential FormIDs to create a good spread
// b) lots and lots of memory- time tradeoffs

IOSystem GenericLog("Server.log",LOG_WARNING,LOG_WARNING);

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
		Players[i].CellID = 0x2C16E;

		Players[i].Health = 1;
		Players[i].Magika = 0;
		Players[i].Fatigue = 0;
		Connected[i] = false;
	}

	Sleep(100);
	GenericLog.DoOutput(LOG_MESSAGE,"OblivionOnline Basic Server, v.%i.%i.%i\"%s \" %s \n \n",SUPER_VERSION,MAIN_VERSION,SUB_VERSION,RELEASE_CODENAME,RELEASE_COMMENT);
	GenericLog.DoOutput(LOG_MESSAGE,"Main Developers : Written by masterfreek64 aka  Julian Bangert Bangert , bobjr 777 aka Joel Teichroeb \n\n");
	GenericLog.DoOutput(LOG_MESSAGE,"Former Developers : Chessmaster42 aka Joseph Pearson \n");
	GenericLog.DoOutput(LOG_MESSAGE,"--------------------------\n");

	SOCKET acceptSocket;
	SOCKADDR_IN addr;
	FD_SET fdSet;

	//Clean out the server log
	easylog = fopen("Log.txt","w");
	fclose(easylog);
	g_LuaInstance = lua_open();
	// UGGGHHH - moving to LUA
	// first we sett default values:
	lua_pushnumber(g_LuaInstance,PORT);
	lua_setglobal(g_LuaInstance,"OO_Port");
	lua_pushstring(g_LuaInstance,"NONE");
	lua_setglobal(g_LuaInstance,"OOServerListURI");
	lua_pushstring(g_LuaInstance,"Unnamed_Server");
	lua_setglobal(g_LuaInstance,"OOServerName");
	luaL_dofile(g_LuaInstance,"ServerLaunch.lua");
	//get the port
	lua_getglobal(g_LuaInstance,"OO_Port");
	serverPort = lua_tointeger(g_LuaInstance,lua_gettop(g_LuaInstance));
	if(serverPort < 1 || serverPort > 65356)
	{
		serverPort = PORT;
		GenericLog.DoOutput(LOG_WARNING,"Service port out of range, selecting %u\n",serverPort);
	}
	//Get the name
	lua_getglobal(g_LuaInstance,"OO_ServerName");
	if(lua_isstring(g_LuaInstance,lua_gettop(g_LuaInstance)))
	{
		strncpy(ServerName,lua_tostring(g_LuaInstance,lua_gettop(g_LuaInstance)),32);
	}
	else
	{
		strncpy(ServerName,"Another OblivionOnline Server",32);
	}
	GenericLog.DoOutput(LOG_MESSAGE,"Server was called %s \n",ServerName);

	lua_getglobal(g_LuaInstance,"OO_ListURI");
	if(lua_isstring(g_LuaInstance,lua_gettop(g_LuaInstance)))
	{
		strncpy(ListURI,lua_tostring(g_LuaInstance,lua_gettop(g_LuaInstance)),256);
	}
	else
	{
		ListURI[0] = '\0';
	}
	GenericLog.DoOutput(LOG_MESSAGE,"Server List was set to : %s \n",ListURI);
	
	// start WinSock
	rc=StartNet();
	// create Socket
	acceptSocket=socket(AF_INET,SOCK_STREAM,0);

	if(acceptSocket==INVALID_SOCKET)
	{
		GenericLog.DoOutput(LOG_ERROR,"The AcceptSocket couldn't be created: %d\n",WSAGetLastError());
		return 1;
	}
	else
	{
		GenericLog.DoOutput(LOG_MESSAGE,"Created Socket-\n");
	}

	memset(&addr,0,sizeof(SOCKADDR_IN));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(serverPort);
	addr.sin_addr.s_addr=INADDR_ANY;
	rc=bind(acceptSocket,(SOCKADDR*)&addr,sizeof(SOCKADDR_IN));
	if(rc==SOCKET_ERROR)
	{
		GenericLog.DoOutput(LOG_ERROR,"Error calling bind: %d\n",WSAGetLastError());
		return 1;
	}
	else
	{
		GenericLog.DoOutput(LOG_MESSAGE,"Socket bound-\n");
	}

	// Start listener
	rc=listen(acceptSocket,10);
	if(rc==SOCKET_ERROR)
	{
		GenericLog.DoOutput(LOG_ERROR,"Error calling listen: %d\n",WSAGetLastError());
		return 1;
	}
	else
	{
		GenericLog.DoOutput(LOG_MESSAGE,"Listening for incoming connections\n");		
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
			GenericLog.DoOutput(LOG_ERROR,"select, error code: %s\n",WSAGetLastError());
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
					ConnectionInfo[LocalPlayer] = NewAddr;
					TotalClients++;
					GenericLog.DoOutput(LOG_MESSAGE,"%s - Accepted new connection #%d from %s:%u\n",LocalPlayer,inet_ntoa(NewAddr.sin_addr),ntohs(NewAddr.sin_port));
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
					TotalClients--;
					GenericLog.DoOutput(LOG_MESSAGE,"%s - Client %d closed the Connection\n",LocalPlayer);
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
	lua_close(g_LuaInstance); // Not thread safe...
}

int StartNet()
{
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,0),&wsa) == 0)
	{
		GenericLog.DoOutput(LOG_MESSAGE,"WinSock2 started \n");
		bServerAlive = true;
		return 1;
	}
	else
	{
		GenericLog.DoOutput(LOG_ERROR,"Couldn't Start Winsock 2 : %d\n",WSAGetLastError());
		return 0;
	}
}

int ScanBuffer(char *acReadBuffer, short LocalPlayer, short nBytesRead)
{
	OOPacketType ePacketType = SelectType(acReadBuffer);

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
	case OOPName:
		OOPName_Handler(acReadBuffer,LocalPlayer);
		break;	
	}
	return true;
}

void info(void *arg)
{
	if(strlen(ListURI))
	{
		char ListHost [256];
		char ListFile [256];
		unsigned short ListPort;
		while(serverPort==0) // We wait for initialisation
			Sleep(50);
		WSADATA WSAData;
		WSAStartup(MAKEWORD(2,0), &WSAData);
		SOCKET sock;
		SOCKADDR_IN sin;
		
		sscanf(ListURI,"http://%256s:%u/%256s",ListHost,&ListPort,ListFile);
		struct hostent *he;
		while((he = gethostbyname(ListHost)) == NULL)
		{
			GenericLog.DoOutput(LOG_WARNING,"Error resolving serverlist hostname. Retrying in 60 seconds.\n");
			Sleep(60000);	//Sleep for 60 seconds and then try again
		}

		long rcs;
		bool HasPassword;
		HasPassword = false;
		

		while(true){

			// THIS IS SOOOO UGLY !!!! Replace with a proper libcurl implementation
			char srequest[384];
			sprintf(srequest, "GET /%s?name=%s&port=%u&players=%i&maxplayers=%i&VersionMajor=%i&VersionMinor=%i&HasPassword=%i HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", ListFile, ServerName, serverPort, TotalClients, MAXCLIENTS, MAIN_VERSION, SUB_VERSION, HasPassword, ListHost);
			
			sock = socket(AF_INET, SOCK_STREAM, 0);
			memcpy(&sin.sin_addr, he->h_addr_list[0], he->h_length);
			//sin.sin_addr.s_addr = inet_addr(IP);
			sin.sin_family = AF_INET;
			sin.sin_port = htons(80);

			rcs=connect(sock, (SOCKADDR *)&sin, sizeof(sin)); 
			if(rcs==SOCKET_ERROR) 
			{
				GenericLog.DoOutput(LOG_ERROR,"Error: Serverlist connect error, code: %i\n",WSAGetLastError());
			}
			rcs=send(sock, srequest, strlen(srequest), 0);
			if(rcs==SOCKET_ERROR) 
			{
				GenericLog.DoOutput(LOG_ERROR,"Error: Serverlist send error, code: %i\n",WSAGetLastError());
			}
			closesocket(sock); 
			Sleep(120000);
		}
		WSACleanup();
	}
	else
	{
		GenericLog.DoOutput(LOG_ERROR,"No ListURI set. This server will not be listed online.\n");
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
		GenericLog.DoOutput(LOG_ERROR,"Error on bind socket for remote admin.\n");
		return;
	}
	rc = listen(acceptSocket,10);
	if (rc == SOCKET_ERROR)
	{
		GenericLog.DoOutput(LOG_ERROR,"Error on listen socket for remote admin.\n");
		return;
	}

	while(true)
	{
		//Wait for remote connection
		SOCKADDR_IN sinRemote;
		int nAddrSize = sizeof(SOCKADDR_IN);
		adminSocket = accept(acceptSocket, (sockaddr*)&sinRemote, &nAddrSize);

		
		GenericLog.DoOutput(LOG_MESSAGE,"%s - Admin connected from %s:%i\n",inet_ntoa(sinRemote.sin_addr), ntohs(sinRemote.sin_port));
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
		GenericLog.DoOutput(LOG_MESSAGE,"%s - Admin disconnected from %s:%i\n",inet_ntoa(sinRemote.sin_addr), ntohs(sinRemote.sin_port));
	}

	//Clean up
	closesocket(adminSocket);
	WSACleanup();
}


bool BroadcastMessage(char *Message, int Player)
{
	//Print message on console
	GenericLog.DoOutput(LOG_MESSAGE,"Server: %s\n", Message);

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
		GenericLog.DoOutput(LOG_MESSAGE,"Kicking Player %i ...\n", OutPkgBuf.PlayerID);
		for(int cx=0;cx<MAXCLIENTS;cx++)
		{
			send(clients[cx],(char *)&OutPkgBuf,sizeof(OOPkgDisconnect),0);
		}
	}
	return true;
}
