/*

Copyright 2007   Julian Bangert aka masterfreek64 and Joseph Pearson aka chessmaster42 and Joel Teichroeb aka bobjr777

This file is part of OblivionOnline.

    OblivionOnline is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    OblivionOnline is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See theOOHashTable.h:33: error: initializer expression list treated as compound expression
OOHashTable.h:34: error: expected ‘,’ or ‘;’ before ‘{’ token
OOHashTable.h:39: error: ‘UINT32’ does not name a type
OOHashTable.h:53: error: expected ‘;’ before ‘(’ token

    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ConsoleServer_h
#define ConsoleServer_h

#define UInt32 unsigned long
#define UInt8 unsigned char

#ifdef WINDOWS
//Win32 includes
#include "Windows.h"
#include <process.h>
#include <winsock.h>
typedef int socklen_t; 
#else
//POSIX includes
#include <errno.h>
#include <pthread.h>
//Berkeley Sockets
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


//map types ...
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef int32_t SOCKET;
typedef unsigned int UINT32;
typedef sockaddr_in  SOCKADDR_IN;
typedef sockaddr  SOCKADDR;
typedef fd_set FD_SET;

//map constants and macros
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define WSAGetLastError() (errno)
#define closesocket(x) close(x)
#define Sleep(x) usleep(1000 * x)
#ifndef MAKEWORD
	#define MAKEWORD(b1,b2) ((WORD)( ((BYTE)(b1)) | ((WORD)((BYTE)(b2))) << 8))
#endif
// WE DO NO implement GUID with UUID , but rather make a GUID structure
struct GUID //wikipedia, MSDN
{
	unsigned long Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char  Data4[8];
	bool operator==(const GUID &other) const
	{
		return (Data1 == other.Data1) && (Data2 == other.Data2) && (Data3 == other.Data3) && (Data4 == other.Data4);
	}
};
#endif
//Generic Includes  ( ANSI and ISO)
#include <iostream>
#include <time.h>

#include "OOStructs.h"
#include "OOHashTable.h"
#include "IOSystem.h"
extern "C"
{
	#include "lua/lua.h"
	#include "lua/lualib.h"
	#include "lua/lauxlib.h"
}
#define MAXCLIENTS 12
#define PORT 41805
#define ADMINPORT (PORT - 1)

#define SUPER_VERSION 0 /*not used yet*/
#define MAIN_VERSION 4
#define SUB_VERSION 5 
//The 2 below here are nothing but UI . Not on the netcode
#define RELEASE_CODENAME "Fix Release" // The Name . Can be emp to `Players' follow
#define RELEASE_COMMENT "" // For betas and special builds only

#define EXPECTED_MOBS 3000

extern int TotalClients;
extern bool bServerAlive;
extern SOCKET clients[MAXCLIENTS];
extern SOCKET adminSocket;
extern sockaddr_in ConnectionInfo[MAXCLIENTS];
extern PlayerStatus Players[MAXCLIENTS];
extern PlayerStatus PlayersInitial[MAXCLIENTS];

extern FILE *easylog;

extern char ServerPassword[32];
extern char AdminPassword[32];

extern int StartNet(void);
extern int ScanBuffer(char *acReadBuffer, short LocalPlayer, short nBytesRead);
extern bool BroadcastMessage(char *Message, int Player);
extern bool Kick(int Player);

extern class OOHashTable MobTable;
extern class IOSystem GenericLog;

#endif

