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

#ifndef ConsoleServer_h
#define ConsoleServer_h

#define UInt32 unsigned long
#define UInt8 unsigned char

#include <winsock.h>
#include <iostream>
#include <time.h>
#include <process.h>
#include "OOStructs.h"
#include "OOHashTable.h"
#include "../common/IDebugLog.h"
#define MAXCLIENTS 12
#define PORT 41805
#define ADMINPORT (PORT - 1)

#define SUPER_VERSION 0 //Not used yet
#define MAIN_VERSION 4
#define SUB_VERSION 5 
//The 2 below here are nothing but UI . Not on the netcode
#define RELEASE_CODENAME "Release V" // The Name . Can be empty
#define RELEASE_COMMENT "Internal Alpha" // For betas and special builds only

//Defines for remote admin control
#define CONNECTMSG 0
#define DISCONNECTMSG 1
#define WELCOMEMSG 2

#define MSGCONTROL 0
#define AUTHCONTROL 1
#define CHATCONTROL 2
#define KICKCONTROL 3

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
extern int SendAdminMessage(char message[256]);
extern class OOHashTable MobTable;

#endif