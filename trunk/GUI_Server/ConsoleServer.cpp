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
#include "Lua_Binding.h"
// Global Server Variables
bool bServerAlive = true;
int TotalClients = 0;
SOCKET clients[MAXCLIENTS];
SOCKET adminSocket;
sockaddr_in ConnectionInfo[MAXCLIENTS];
PlayerStatus Players[MAXCLIENTS];
PlayerStatus PlayersInitial[MAXCLIENTS];

bool Connected[MAXCLIENTS];
int serverPort = 0;
int adminPort = 0;
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
OOHashTable MobTable(EXPECTED_MOBS);  // This value generates a good spread, especially with 16 potences ... also we should about double the number of values necessary to avoid collisions. 
//The hashing algorithm here relies on the fact that we do a) not have a string hash , but a good modulo hash
// Therefore we need sequential FormIDs to create a good spread
// b) lots and lots of memory- time tradeoffs

IOSystem GenericLog("Server.log",LOG_WARNING,LOG_WARNING);


// Prototypes
int StartNet(void);
int ScanBuffer(char *acReadBuffer, short LocalPlayer, short nBytesRead);
#ifdef WINDOWS
void info(void *);
#else
void *info(void *);
#endif

typedef std::pair< std::string, std::string > UserPasswordPair;


//Main entry procedure
int  main(void)
{
	
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
#ifndef WINDOWS
//POSIX 
pthread_t threads;
#endif
	GenericLog.DoOutput(LOG_MESSAGE,"######################################################################################################\n");
	GenericLog.DoOutput(LOG_MESSAGE,"OblivionOnline Basic Server, v.%i.%i.%i\"%s \" %s \n \n",SUPER_VERSION,MAIN_VERSION,SUB_VERSION,RELEASE_CODENAME,RELEASE_COMMENT);
	GenericLog.DoOutput(LOG_MESSAGE,"Main Developers : Written by masterfreek64 aka  Julian Bangert Bangert , bobjr 777 aka Joel Teichroeb \n\n");
	GenericLog.DoOutput(LOG_MESSAGE,"Former Developers : Chessmaster42 aka Joseph Pearson \n");
	GenericLog.DoOutput(LOG_MESSAGE,"######################################################################################################\n");

	SOCKET acceptSocket;
	SOCKADDR_IN addr;
	FD_SET fdSet;

	//Clean out the server log
	easylog = fopen("Log.txt","w");
	fclose(easylog);
	g_LuaInstance = lua_open();
	// first we sett default values:
	lua_pushnumber(g_LuaInstance,PORT);
	LuaRegisterBinding(g_LuaInstance);
	lua_setglobal(g_LuaInstance,"ServicePort");
	lua_pushstring(g_LuaInstance,"");
	lua_setglobal(g_LuaInstance,"ServerListURI");
	lua_pushstring(g_LuaInstance,"Another OblivionOnline Server");
	lua_setglobal(g_LuaInstance,"ServerName");
	luaL_dofile(g_LuaInstance,"ServerLaunch.lua");
	//get the port
	lua_getglobal(g_LuaInstance,"ServicePort");
	serverPort = lua_tointeger(g_LuaInstance,lua_gettop(g_LuaInstance));
	if((serverPort < 1) || (serverPort > 65356))
	{
		
		GenericLog.DoOutput(LOG_WARNING,"Service port %u out of range, selecting %i\n",serverPort,PORT);
		serverPort = PORT;
	}
	//Get the name
	lua_getglobal(g_LuaInstance,"ServerName");
	if(lua_isstring(g_LuaInstance,lua_gettop(g_LuaInstance)))
	{
		strncpy(ServerName,lua_tostring(g_LuaInstance,lua_gettop(g_LuaInstance)),32);
	}
	else
	{
		strncpy(ServerName,"Another OblivionOnline Server",32);
	}
	GenericLog.DoOutput(LOG_MESSAGE,"Server was called %s \n",ServerName);

	lua_getglobal(g_LuaInstance,"ServerListURI");
	if(lua_isstring(g_LuaInstance,lua_gettop(g_LuaInstance)))
	{
		strncpy(ListURI,lua_tostring(g_LuaInstance,lua_gettop(g_LuaInstance)),256);
	}
	else
	{
		ListURI[0] = '\0';
	}
	GenericLog.DoOutput(LOG_MESSAGE,"Server List was set to : %s \n",ListURI);
	
#ifndef WINDOWS
	//POSIX 
	pthread_t thread;
	pthread_create(&thread,NULL,&info,NULL);
#else

	_beginthread(info,0,NULL);
#endif	
	// start WinSock
	rc=StartNet();
	// create Socket
	acceptSocket=socket(AF_INET,SOCK_STREAM,0);

	if(acceptSocket==INVALID_SOCKET)
	{
		GenericLog.DoOutput(LOG_ERROR,"The AcceptSocket couldn't be created: %d\n",WSAGetLastError());
		return 1;
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
		GenericLog.DoOutput(LOG_MESSAGE,"Opened server on port %u \n",serverPort);
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
					socklen_t nAddrSize = sizeof(NewAddr);
					clients[LocalPlayer]=accept(acceptSocket, (sockaddr*)&NewAddr, &nAddrSize);
					ConnectionInfo[LocalPlayer] = NewAddr;
					TotalClients++;
					GenericLog.DoOutput(LOG_MESSAGE,"Accepted new connection #%d from %s:%u\n",LocalPlayer,inet_ntoa(NewAddr.sin_addr),ntohs(NewAddr.sin_port));
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
#ifdef WINDOWS
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

#else
	bServerAlive = true;
	return 1;
#endif
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
//Thread 
#ifdef WINDOWS
void info(void *arg)
#else
void *info(void *arg)
#endif
{
	if(strlen(ListURI))
	{
		char ListHost [512];
		char ListFile [256];
		unsigned short ListPort;
		while(serverPort==0) // We wait for initialisation
			Sleep(50);
#ifdef WINDOWS
		WSADATA WSAData;
		WSAStartup(MAKEWORD(2,0), &WSAData);
#endif
		SOCKET sock;
		SOCKADDR_IN sin;
		
		
		

		long rcs;
		bool HasPassword;
		HasPassword = false;
		

		while(true){
			sscanf(ListURI,"http://%512s",ListHost); // direct parsing won't work in all implementations
			char *ptr = strstr(ListHost,":"); // Port delimiter
			sscanf(ptr,":%u%512s",&ListPort,ListFile); 
			*ptr = '\0';
			// THIS IS SOOOO UGLY !!!! Replace with a proper libcurl implementation
			char srequest[384];
			struct hostent *he;
			while((he = gethostbyname(ListHost)) == NULL)
			{
				GenericLog.DoOutput(LOG_WARNING,"Error resolving serverlist hostname. Retrying in 60 seconds.\n");
				Sleep(60000);	//Sleep for 60 seconds and then try again
			}
			sprintf(srequest, "GET /%s?name=%s&port=%u&players=%i&maxplayers=%i&VersionMajor=%i&VersionMinor=%i&HasPassword=%i HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", ListFile, ServerName, serverPort, TotalClients, MAXCLIENTS, MAIN_VERSION, SUB_VERSION, HasPassword, ListHost);
			
			sock = socket(PF_INET, SOCK_STREAM, 0);
			memcpy(&sin.sin_addr, he->h_addr_list[0], he->h_length);
			//sin.sin_addr.s_addr = inet_addr(IP);
			sin.sin_family = AF_INET;
			sin.sin_port = htons(ListPort);
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
#ifdef WINDOWS
		WSACleanup();
#endif
	}
	else
	{
		GenericLog.DoOutput(LOG_ERROR,"No ListURI set. This server will not be listed online.\n");
	}	
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
