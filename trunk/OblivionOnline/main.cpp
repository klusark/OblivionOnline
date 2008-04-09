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
#include "main.h"
#include "UserInterface.h"
#include "D3Dhook.h"
#include "InPacket.h"
#include "Commands.h"
#include "Entity.h"
// Global variables
extern "C" HINSTANCE OODll;
bool bIsConnected = false;
bool bIsInitialized = false;
IDebugLog gLog;
bool PlayerConnected[MAXCLIENTS];
UINT32 LocalPlayer;
UINT32 TotalPlayers;
bool bIsMasterClient = false;
UInt32 SpawnID[MAXCLIENTS];

SOCKET ServerSocket;

HANDLE hRecvThread;
HANDLE hPredictionEngine;

TESObjectREFR* PlayerActorList[MAXCLIENTS];
//UserInterface usrInterface;
char ServerIP[15];

bool bFrameRendered = false;
std::queue<Entity *> UpdateQueue;
// Prototypes

EntityManager Entities;
extern bool FindEquipped(TESObjectREFR* thisObj, UInt32 slotIdx, FoundEquipped* foundEquippedFunctor, double* result);

extern  "C" void OpenLog(int bOblivion)
{
	if(bOblivion)
		gLog.Open("OblivionOnline.log");
	else
		gLog.Open("OblivionOnline_Hook.log");
}
int OO_Initialize()
{
	
	long rc;
	WSADATA wsa;
	rc = WSAStartup(MAKEWORD(2,0),&wsa);
	ServerSocket = socket(AF_INET,SOCK_STREAM,0);
	_MESSAGE("Welcome to OblivionOnline");
	_MESSAGE("Initializing GUI");
	InitialiseUI();
	Entities.DeleteEntities();
	TotalPlayers = 0;
	for(int i=0; i<MAXCLIENTS; i++)
	{
		PlayerConnected[i] = false;
	}
	return rc;
}

int OO_Deinitialize ()
{
	for(int i=0; i<MAXCLIENTS; i++)
	{
		PlayerConnected[i] = false;
	}
	TotalPlayers = 0;
	TerminateThread(hRecvThread, 0);
	CloseHandle(hRecvThread);
	TerminateThread(hPredictionEngine, 0);
	
	closesocket(ServerSocket);
	ServerSocket = INVALID_SOCKET;
	WSACleanup();
	DeinitialiseUI();
	D3DHookDeInit();

	return 1;
}

DWORD WINAPI RecvThread(LPVOID Params)
{
	char buf[PACKET_SIZE];
	int rc;
	InPacket * pkg;
	_MESSAGE("Receive thread started");
	while(bIsConnected)
	{
		_MESSAGE("Waiting for incoming traffic...");
		rc = recv(ServerSocket,buf,PACKET_SIZE,0);
		_MESSAGE("Received new packet");
		pkg = new InPacket((BYTE *)buf,rc);
		pkg->HandlePacket();
		delete pkg;
	}
	return 0;
}

//-----------------------------
//---Begin Command Functions---
//-----------------------------




//---------------------------
//---End Command Functions---
//---------------------------

//-----------------------------------
//---Begin CommandInfo Enumeration---
//-----------------------------------



//---------------------------------
//---End CommandInfo Enumeration---
//---------------------------------

extern "C" {

bool OBSEPlugin_Query(const OBSEInterface * obse, PluginInfo * info)
{
	_MESSAGE("Starting plugin query ...");

	// fill out the info structure
	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = "OblivionOnline";
	info->version = (SUPER_VERSION << 16) &&MAKEWORD(MAIN_VERSION,SUB_VERSION);

	// version checks
	if(!obse->isEditor)
	{
		if(obse->obseVersion < OBSE_VERSION_INTEGER)
		{
			_ERROR("OBSE version too old (got %08X expected at least %08X)", obse->obseVersion, OBSE_VERSION_INTEGER);
			return false;
		}

		if(obse->oblivionVersion != OBLIVION_VERSION)
		{
			_ERROR("Incorrect Oblivion version (got %08X need %08X)", obse->oblivionVersion, OBLIVION_VERSION);
			return false;
		}
	}
	else
	{
		// no version checks needed for editor
	}

	// version checks pass

	return true;
}

bool OBSEPlugin_Load(const OBSEInterface * obse)
{
	_MESSAGE("Loading OO Commands");

	obse->SetOpcodeBase(0x22D0); // Our codebase

	//Connection commands
	obse->RegisterCommand(&kMPConnectCommand);
	

	//Data sending
	obse->RegisterCommand(&kMPSendActorCommand);
	obse->RegisterCommand(&kMPSendChatCommand);


	//Data injecting
	obse->RegisterCommand(&kMPGetPosXCommand);
	obse->RegisterCommand(&kMPGetPosYCommand);
	obse->RegisterCommand(&kMPGetPosZCommand);
	obse->RegisterCommand(&kMPGetRotZCommand);
	obse->RegisterCommand(&kMPGetCellCommand);
	obse->RegisterCommand(&kMPGetIsInInteriorCommand);
	//Debug
	obse->RegisterCommand(&kMPGetDebugDataCommand);

	//Misc.
	obse->RegisterCommand(&kMPGetSpawnedRefCommand);
	obse->RegisterCommand(&kMPSpawnedCommand);
	obse->RegisterCommand(&kMPTotalPlayersCommand);
	obse->RegisterCommand(&kMPDisconnectCommand);
	obse->RegisterCommand(&kMPClearSpawnCommand);

	//Equipment
	obse->RegisterCommand(&kMPGetEquipmentCommand);


	obse->RegisterCommand(&kMPGetNewObjectCommand);
	obse->RegisterCommand(&kMPGetMyIDCommand);
	
	obse->RegisterCommand(&kMPShowGUICommand);
	obse->RegisterCommand(&kGetParentCellCommand);
	_MESSAGE("Done loading OO Commands");
	return true;
}
};
