/*

Copyright(c) 2007-2008   Julian Bangert aka masterfreek64, Joseph Pearson aka chessmaster42 

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

	Linking OblivionOnline statically or dynamically with other modules is making a combined work based
	on OblivionOnline. Thus, the terms and conditions of the GNU General Public License cover 
	the whole combination.

    In addition, as a special exception, the copyright holders of  OblivionOnline give you permission to 
	combine OblivionOnline program with free software programs or libraries that are released under
	the GNU LGPL and with code included in the standard release of Oblivion Script Extender by Ian Patterson (OBSE)
	under the OBSE license (or modified versions of such code, with unchanged license). You may copy and distribute such a system 
	following the terms of the GNU GPL for  OblivionOnline and the licenses of the other code concerned,
	provided that you include the source code of that other code when and as the GNU GPL
	requires distribution of source code.

	Note that people who make modified versions of  OblivionOnline are not obligated to grant this special exception
	for their modified versions; it is their choice whether to do so. 
	The GNU General Public License gives permission to release a modified version without this
	exception; this exception also makes it possible to release a modified version which carries 
	forward this exception.
*/

#ifndef main_h
#define main_h

#include <list>
#include <queue>
#include <stdio.h>
#include <Windows.h>
#include "../OblivionOnlineServer/GlobalDefines.h"
#include "obse/PluginAPI.h"
#include "obse/CommandTable.h"
#include "obse/GameAPI.h"
#include "obse/ParamInfos.h"
#include "obse/GameObjects.h"
#include "OBSEFunctions.h"
#include "OOFunctions.H"
#include "OutboundNetwork.h"
#include "EntityManager.h"
#define MAXCLIENTS 12
#define MAXSERVERS 8

#define SUPER_VERSION 0 //Not used yet
#define MAIN_VERSION 5
#define SUB_VERSION 0
//The 2 below here are nothing but UI . Not on the netcode
#define RELEASE_CODENAME "Release V" // The Name . Can be empty
#define ADDITIONAL_VERSION_COMMENT "Internal Alpha" // For betas and special builds only

extern std::queue<Entity *> UpdateQueue;
//Externals
extern IDebugLog gLog;
extern bool bIsConnected;
extern bool bIsMasterClient;
extern UINT32 LocalPlayer;
extern UINT32 TotalPlayers;
extern bool PlayerConnected[MAXCLIENTS];
extern SOCKET ServerSocket;
extern HANDLE hRecvThread;
extern TESObjectREFR* PlayerActorList[MAXCLIENTS];
extern UInt32 SpawnID[MAXCLIENTS];
extern char ServerIP[15];
extern OutboundNetwork outnet;

extern DWORD WINAPI RecvThread(LPVOID Params);
extern int OO_Initialize();
extern int OO_Deinitialize();
extern EntityManager Entities;

extern "C" HANDLE hEvtSendPacket;
#endif