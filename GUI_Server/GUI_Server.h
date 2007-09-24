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

#ifndef GUI_Server_h
#define GUI_Server_h

#define UInt32 unsigned long
#define UInt8 unsigned char

#include "resource.h"
#include <winsock.h>
#include <iostream>
#include <time.h>
#include <process.h>
#include "OOStructs.h"

#define MAXCLIENTS 4
#define PORT 41805

#define SUPER_VERSION 0	//Not used yet
#define MAIN_VERSION 4
#define SUB_VERSION 2	//Release 4, bugfix 2

extern int TotalClients;
extern bool bServerAlive;
extern SOCKET clients[MAXCLIENTS];
extern PlayerStatus Players[MAXCLIENTS];
extern PlayerStatus PlayersInitial[MAXCLIENTS];
extern UInt8 ModList[MAXCLIENTS][255];
extern FILE *easylog;
extern char serverMsg[256];
extern HWND hServerDlg;

extern int StartNet(void);
extern int ScanBuffer(char *acReadBuffer, short LocalPlayer);

#endif