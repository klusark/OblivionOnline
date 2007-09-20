/*

Copyright 2007  Julian aka masterfreek64, Joseph Pearson aka chessmaster42 and Joel Teichroeb aka bobjr777

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
#include "OONetwork.h"
#include <hash_map>

// Global variables
IDebugLog gLog("OblivionOnline.log");

bool bIsConnected = false;
bool PlayerConnected[MAXCLIENTS];
int LocalPlayer;
int TotalPlayers;

UInt32 SpawnID[MAXCLIENTS];

SOCKET ServerSocket;

HANDLE hRecvThread;
HANDLE hPredictionEngine;

PlayerStatus Players[MAXCLIENTS];
TESObjectREFR* PlayerActorList[MAXCLIENTS];

int BadPackets[PACKET_COUNT];	//Keeps track of # of bad packets of each type

DWORD PacketTime[PACKET_COUNT]; //System time when this packet was received.'
DWORD VelocityTime[MAXCLIENTS], VelocityOldTime[MAXCLIENTS];	//Timers to calculate NPC velocity

UInt8 ModList[MAXCLIENTS][256];	//List of supported mods from each client

// Prototypes
extern void RunScriptLine(const char *buf, bool IsTemp);
extern int GetActorID(UInt32 refID);
extern float GetStat(Actor *ActorBuf, int statNum);

extern bool NetActorUpdate(PlayerStatus *Player, int PlayerID, bool Initial, bool IsPC);
extern bool NetWelcome();
extern bool NetDisconnect();
extern bool NetChat(char *Message);
extern bool NetFullStatUpdate(PlayerStatus *Player, int PlayerID, bool Initial, bool IsPC);
extern bool NetReadBuffer(char *acReadBuffer, int Length);
extern bool NetEquipped(PlayerStatus *Player, int PlayerID, bool Initial);
extern bool NetSendModList(void);

extern bool FindEquipped(TESObjectREFR* thisObj, UInt32 slotIdx, FoundEquipped* foundEquippedFunctor, double* result);

int OO_Initialize()
{
	long rc;
	WSADATA wsa;
	rc = WSAStartup(MAKEWORD(2,0),&wsa);
	ServerSocket = socket(AF_INET,SOCK_STREAM,0);
	_MESSAGE("Welcome to OblivionOnline");

	// Initialize the arrays
	for(int i=0; i<MAXCLIENTS; i++)
	{
		Players[i].PosX = 0;
		Players[i].PosY = 0;
		Players[i].PosZ = -196;
		Players[i].RotX = 0;
		Players[i].RotY = 0;
		Players[i].RotZ = 0;
		Players[i].VelX = 0;
		Players[i].VelY = 0;
		Players[i].VelZ = 0;
		Players[i].CellID = 0;
		Players[i].Health = 1;
		Players[i].bStatsInitialized = false;
		Players[i].bIsInInterior = true;
		Players[i].head = 0;
		Players[i].hair = 0;
		Players[i].upper_body = 0;
		Players[i].lower_body = 0;
		Players[i].hand = 0;
		Players[i].foot = 0;
		Players[i].right_ring = 0;
		Players[i].left_ring = 0;
		Players[i].amulet = 0;
		Players[i].shield = 0;
		Players[i].tail = 0;
		Players[i].weapon = 0;
		Players[i].ammo = 0;
		Players[i].robes = 0;

		PlayerConnected[i] = false;

		SpawnID[i] = 0;

		for(int j=0; j<256; j++)
			ModList[i][j] = 0;
	}
	return rc;
}

int OO_Deinitialize ()
{
	for(int i=0; i<MAXCLIENTS; i++)
	{
		Players[i].PosX = 0;
		Players[i].PosY = 0;
		Players[i].PosZ = -196;
		Players[i].RotX = 0;
		Players[i].RotY = 0;
		Players[i].RotZ = 0;
		Players[i].VelX = 0;
		Players[i].VelY = 0;
		Players[i].VelZ = 0;
		Players[i].CellID = 0;
		Players[i].Health = 1;
		Players[i].bStatsInitialized = false;
		Players[i].bIsInInterior = true;
		Players[i].head = 0;
		Players[i].hair = 0;
		Players[i].upper_body = 0;
		Players[i].lower_body = 0;
		Players[i].hand = 0;
		Players[i].foot = 0;
		Players[i].right_ring = 0;
		Players[i].left_ring = 0;
		Players[i].amulet = 0;
		Players[i].shield = 0;
		Players[i].tail = 0;
		Players[i].weapon = 0;
		Players[i].ammo = 0;
		Players[i].robes = 0;

		PlayerConnected[i] = false;
	}
	TotalPlayers = 0;
	TerminateThread(hRecvThread, 0);
	CloseHandle(hRecvThread);
	TerminateThread(hPredictionEngine, 0);
	CloseHandle(hPredictionEngine);
	closesocket(ServerSocket);
	ServerSocket = INVALID_SOCKET;
	WSACleanup();
	return 1;
}

DWORD WINAPI RecvThread(LPVOID Params)
{
	char buf[512];
	int rc;

	while(bIsConnected)
	{
		rc = recv(ServerSocket,buf,512,0);
		NetReadBuffer(buf, rc);
	}
	return 0;
}

DWORD WINAPI PredictionEngine(LPVOID Params)
{
	/*
	while(bIsConnected)
	{
		for(int i=0; i<MAXCLIENTS; i++)
		{
			//Keep the NPC moving between packets
			//Console_Print("VelX: %f, VelY: %f, VelZ: %f", Players[i].VelX, Players[i].VelY, Players[i].VelZ);
			//Players[i].PosX += Players[i].VelX;
			//Players[i].PosY += Players[i].VelY;
			//Players[i].PosZ += Players[i].VelZ;
		}
		//Wait a bit to allow other processes work
		Sleep(20);
	}
	*/
	return 0;
	
}

//-----------------------------
//---Begin Command Functions---
//-----------------------------

bool Cmd_MPConnect_Execute(COMMAND_ARGS)
{
	if(!bIsConnected)
	{
		_MESSAGE("Initializing Connection... \n");
		OO_Initialize();
		SOCKADDR_IN ServerAddr;
		FILE *Realmlist = fopen("realmlist.wth","r");
		if(!Realmlist)
		{
			_ERROR("File realmlist.wth could not be found");
			Console_Print("File realmlist.wth could not be found");
			return true;
		}
		int TotalServers = 0;
		fscanf(Realmlist,"%i",&TotalServers);
		_MESSAGE("Found %i servers in realmlist.wth",TotalServers);
		char IP[MAXSERVERS][15];
		unsigned short ClientPort[MAXSERVERS];
		long rc;
		for(int i=0; i<TotalServers; i++)
		{
			fscanf(Realmlist,"%s",IP[i]);
			if(!fscanf(Realmlist,"%i",&ClientPort[i]))
				ClientPort[i] = 41805;
			_MESSAGE("Connecting to %s:%i",IP[i],ClientPort[i]);
			memset(&ServerAddr,NULL,sizeof(SOCKADDR_IN));
			ServerAddr.sin_addr.s_addr = inet_addr(IP[i]);
			ServerAddr.sin_port = htons(ClientPort[i]);
			ServerAddr.sin_family = AF_INET;
			rc = connect(ServerSocket,(SOCKADDR *)&ServerAddr,sizeof(SOCKADDR));
			if(rc == SOCKET_ERROR)
			{
				_ERROR("Couldn't connect to %s:%i %u",IP[i],ClientPort[i],WSAGetLastError());
				Console_Print("Oblivion not connected to %s",IP[i]);
			}
			else 
			{
				_MESSAGE("Successfully Connected to %s",IP[i]);
				bIsConnected = true;
				hRecvThread = CreateThread(NULL,NULL,RecvThread,NULL,NULL,NULL);
				//hPredictionEngine = CreateThread(NULL,NULL,PredictionEngine,NULL,NULL,NULL);
				if(!NetWelcome()) return true;
				Console_Print("Oblivion connected to %s",IP[i]);
				TotalPlayers = 1;
				break;
			}
		}
		fclose(Realmlist);
	}
	return true;
}

bool Cmd_MPSendActor_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPSendActor");
		return true;
	}
	if(thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		int actorNumber = GetActorID(ActorBuf->refID);

		if (actorNumber == -2)
			actorNumber = LocalPlayer;

		if (!PlayerConnected[actorNumber])
		{
			return true;
		}

		PlayerActorList[actorNumber] = thisObj;

		PlayerStatus DummyStatus;
		DummyStatus.RefID = ActorBuf->refID;
		DummyStatus.PosX = ActorBuf->posX;
		DummyStatus.PosY = ActorBuf->posY;
		DummyStatus.PosZ = ActorBuf->posZ;
		DummyStatus.RotX = ActorBuf->rotX;
		DummyStatus.RotY = ActorBuf->rotY;
		DummyStatus.RotZ = ActorBuf->rotZ;
		DummyStatus.Health = ActorBuf->GetActorValue(8);
		DummyStatus.Magika = ActorBuf->GetActorValue(9);
		DummyStatus.Fatigue = ActorBuf->GetActorValue(10);
		if(ActorBuf->parentCell->worldSpace)
		{
			DummyStatus.bIsInInterior = false;
			DummyStatus.CellID = ActorBuf->parentCell->worldSpace->refID;
		}
		else
		{
			DummyStatus.bIsInInterior = true;
			DummyStatus.CellID = ActorBuf->parentCell->refID;
		}
		if (actorNumber != LocalPlayer)
		{
			NetActorUpdate(&DummyStatus, actorNumber, false, false);
		}else{
			if (Players[actorNumber].bStatsInitialized)
				NetActorUpdate(&DummyStatus, actorNumber, false, true);
			else{
				Console_Print("Initializing stats ...");
				NetActorUpdate(&DummyStatus, actorNumber, true, true);
				Players[actorNumber].bStatsInitialized = true;
			}
		}
	}
	return true;
}

bool Cmd_MPSendFullStat_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPSendFullStat");
		return true;
	}
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		int actorNumber = GetActorID(ActorBuf->refID);

		if (actorNumber != -1 && actorNumber != -2)
		{
			if (!PlayerConnected[actorNumber])
			{
				return true;
			}
			PlayerStatus DummyStatus;
			DummyStatus.Strength = ActorBuf->GetActorValue(0);
			DummyStatus.Intelligence = ActorBuf->GetActorValue(1);
			DummyStatus.Willpower = ActorBuf->GetActorValue(2);
			DummyStatus.Agility = ActorBuf->GetActorValue(3);
			DummyStatus.Speed = ActorBuf->GetActorValue(4);
			DummyStatus.Endurance = ActorBuf->GetActorValue(5);
			DummyStatus.Personality = ActorBuf->GetActorValue(6);
			DummyStatus.Luck = ActorBuf->GetActorValue(7);
			DummyStatus.Health = ActorBuf->GetActorValue(8);
			DummyStatus.Magika = ActorBuf->GetActorValue(9);
			DummyStatus.Fatigue = ActorBuf->GetActorValue(10);
			DummyStatus.Encumbrance = ActorBuf->GetActorValue(11);
			if (actorNumber != LocalPlayer)
			{
				NetFullStatUpdate(&DummyStatus, actorNumber, false, false);
			}else{
				if (Players[actorNumber].bStatsInitialized)
				{
					NetFullStatUpdate(&DummyStatus, actorNumber, false, true);
				}else{
					NetFullStatUpdate(&DummyStatus, actorNumber, true, true);
					Players[actorNumber].bStatsInitialized = true;
				}
			}
		}
	}
	return true;
}

bool Cmd_MPSendChat_Execute (COMMAND_ARGS)
{
	if (!bIsConnected)
	{
		return true;
	}
	char message[512] = "\0";
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &message)) return true;
	NetChat(message);
	return true;
}

bool Cmd_MPSyncTime_Execute (COMMAND_ARGS)
{
	if (!bIsConnected)
	{
		return true;
	}
	OOPkgTimeUpdate pkgBuf;
	DWORD tickBuf;
	tickBuf=GetTickCount();
	if((tickBuf - PacketTime[OOPTimeUpdate]) > NET_TIMEUPDATE_RESEND)
	{
		pkgBuf.etypeID = OOPTimeUpdate;
		pkgBuf.Flags = 1;	//1 means blank data (i.e. requesting time from server)
		send(ServerSocket, (char *)&pkgBuf, sizeof(OOPkgTimeUpdate), 0);
		PacketTime[OOPTimeUpdate] = tickBuf;
	}
	return true;
}

bool Cmd_MPGetPosX_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPGetPosX");
		return true;
	}
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		int actorNumber = GetActorID(ActorBuf->refID);

		if (actorNumber != -1 && actorNumber != -2)
		{
			*result = Players[actorNumber].PosX;
		}else{
			//Console_Print("Error: Couldn't find actor for MPGetPosX");
			*result = 0;
		}
	}
	return true;
}

bool Cmd_MPGetPosY_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPGetPosY");
		return true;
	}
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		int actorNumber = GetActorID(ActorBuf->refID);

		if (actorNumber != -1 && actorNumber != -2)
		{
			*result = Players[actorNumber].PosY;
		}else{
			//Console_Print("Error: Couldn't find actor for MPGetPosY");
			*result = 0;
		}
	}
	return true;
}

bool Cmd_MPGetPosZ_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPGetPosZ");
		return true;
	}
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		int actorNumber = GetActorID(ActorBuf->refID);

		if (actorNumber != -1 && actorNumber != -2)
		{
			*result = Players[actorNumber].PosZ;
		}else{
			//Console_Print("Error: Couldn't find actor for MPGetPosZ");
			*result = -196;
		}
	}
	return true;
}

bool Cmd_MPGetRotZ_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPGetRotZ");
		return true;
	}
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		int actorNumber = GetActorID(ActorBuf->refID);

		if (actorNumber != -1 && actorNumber != -2)
		{
			//Now convert the angle from radians to degrees
			int angleDegrees = (int)(Players[actorNumber].RotZ * (180.0/3.1415));
			*result = angleDegrees;
		}else{
			//Console_Print("Error: Couldn't find actor for MPGetRotZ");
			*result = 0;
		}
	}
	return true;
}

bool Cmd_MPGetCell_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPGetCell");
		return true;
	}
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		int actorNumber = GetActorID(ActorBuf->refID);

		if (actorNumber != -1 && actorNumber != -2)
		{
			UInt32* refResult = (UInt32*)result;
			*refResult = Players[actorNumber].CellID;
		}else{
			//Console_Print("Error: Couldn't find actor for MPGetCell");
		}
	}
	return true;
}

bool Cmd_MPGetTime_Execute (COMMAND_ARGS)
{
	*result = Players[LocalPlayer].Time;
	return true;
}

bool Cmd_MPGetDay_Execute (COMMAND_ARGS)
{
	*result = Players[LocalPlayer].Day;
	return true;
}

bool Cmd_MPGetMonth_Execute (COMMAND_ARGS)
{
	*result = Players[LocalPlayer].Month;
	return true;
}

bool Cmd_MPGetYear_Execute (COMMAND_ARGS)
{
	*result = Players[LocalPlayer].Year;
	return true;
}

bool Cmd_MPGetIsInInterior_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPGetIsInInterior");
		return true;
	}
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		int actorNumber = GetActorID(ActorBuf->refID);

		if (actorNumber != -1 && actorNumber != -2)
		{
			*result = (int)Players[actorNumber].bIsInInterior;
		}else{
			//Console_Print("Error: Couldn't find actor for MPGetIsInInterior");
		}
	}
	return true;
}

bool Cmd_MPGetStat_Execute (COMMAND_ARGS)
{
	int statNumber = 0;
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPGetStat");
		return true;
	}
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &statNumber)) return true;
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		float statAmount = GetStat(ActorBuf, statNumber);
		//if (statAmount == -1)
			//Console_Print("Tried to get stat of non-initialized actor");
		*result = statAmount;
	}
	return true;
}

bool Cmd_MPGetDebugData_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPGetDebugData");
		return true;
	}
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		int actorNumber = GetActorID(ActorBuf->refID);

		if (actorNumber != -1)
		{
			if (actorNumber == -2)
				actorNumber = LocalPlayer;
			//Console_Print("VelX: %f, VelY: %f, VelZ: %f", Players[actorNumber].VelX, Players[actorNumber].VelY, Players[actorNumber].VelZ);
			Console_Print("Badpackets: ActorUpdate(%i), FSU(%i)", BadPackets[OOPActorUpdate], BadPackets[OOPFullStatUpdate]);
		}
	}
	return true;
}

bool Cmd_MPGetSpawnedRef_Execute (COMMAND_ARGS)
{
	int spawnNumber = 0;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &spawnNumber)) return true;
	for(int i=0; i<MAXCLIENTS; i++)
	{
		if (spawnNumber == i)
		{
			UInt32* refResult = (UInt32*)result;
			*refResult = SpawnID[i];
			return true;
		}
	}
	return true;
}

bool Cmd_MPSpawned_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPSpawned");
		return true;
	}
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		UInt32 actorNumber = ActorBuf->refID;
		for(int i=0; i<TotalPlayers-1; i++)
		{
			if (!SpawnID[i])
			{
				SpawnID[i] = actorNumber & 0x00ffffff;	//Mask off the mod offset for OblivionOnline
				ModList[LocalPlayer][1] = (actorNumber & 0xff000000) >> 24;	//Read off the mod offset for OblivionOnline
				Console_Print("Spawn %i ID: %u", i, SpawnID[i]);
				break;
			}
		}
	}
	return true;
}

bool Cmd_MPTotalPlayers_Execute (COMMAND_ARGS)
{
	*result = (float)TotalPlayers;
	return true;
}

bool Cmd_MPDisconnect_Execute (COMMAND_ARGS)
{
	if(bIsConnected)
	{
		NetDisconnect();
		OO_Deinitialize();
		bIsConnected = false;
		Console_Print("You have disconnected");
	}else{
		Console_Print("You are not connected");
	}
	return true;
}

bool Cmd_MPClearSpawn_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPClearSpawn");
		return true;
	}
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		for(int i=0; i<MAXCLIENTS; i++)
		{
			if (SpawnID[i] == (ActorBuf->refID & 0x00ffffff))
			{
				SpawnID[i] = 0;
				//Console_Print("SpawnID cleared");
			}
		}
	}
	return true;
}
bool Cmd_MPSendEquipped_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPSendEquipped");
		return true;
	}
	if (thisObj->IsActor())
	{
		int actorNumber = GetActorID(thisObj->refID);

		if (actorNumber != -1)
		{
			//If the player is the only one connected, go ahead and store the data
			if (actorNumber == -2)
				actorNumber = LocalPlayer;

			double itemResult;
			UInt32* itemRef = (UInt32*)&itemResult;
			feGetObject getObject;

			if (FindEquipped(thisObj, 0, &getObject, &itemResult))
				Players[actorNumber].head = *itemRef;
			else
				Players[actorNumber].head = 0;
			if (FindEquipped(thisObj, 1, &getObject, &itemResult))
				Players[actorNumber].hair = *itemRef;
			else
				Players[actorNumber].hair = 0;
			if (FindEquipped(thisObj, 2, &getObject, &itemResult))
				Players[actorNumber].upper_body = *itemRef;
			else
				Players[actorNumber].upper_body = 0;
			if (FindEquipped(thisObj, 3, &getObject, &itemResult))
				Players[actorNumber].lower_body = *itemRef;
			else
				Players[actorNumber].lower_body = 0;
			if (FindEquipped(thisObj, 4, &getObject, &itemResult))
				Players[actorNumber].hand = *itemRef;
			else
				Players[actorNumber].hand = 0;
			if (FindEquipped(thisObj, 5, &getObject, &itemResult))
				Players[actorNumber].foot = *itemRef;
			else
				Players[actorNumber].foot = 0;
			if (FindEquipped(thisObj, 6, &getObject, &itemResult))
				Players[actorNumber].right_ring = *itemRef;
			else
				Players[actorNumber].right_ring = 0;
			if (FindEquipped(thisObj, 7, &getObject, &itemResult))
				Players[actorNumber].left_ring = *itemRef;
			else
				Players[actorNumber].left_ring = 0;
			if (FindEquipped(thisObj, 8, &getObject, &itemResult))
				Players[actorNumber].amulet = *itemRef;
			else
				Players[actorNumber].amulet = 0;
			if (FindEquipped(thisObj, 13, &getObject, &itemResult))
				Players[actorNumber].shield = *itemRef;
			else
				Players[actorNumber].shield = 0;
			if (FindEquipped(thisObj, 15, &getObject, &itemResult))
				Players[actorNumber].tail = *itemRef;
			else
				Players[actorNumber].tail = 0;
			if (FindEquipped(thisObj, 16, &getObject, &itemResult))
				Players[actorNumber].weapon = *itemRef;
			else
				Players[actorNumber].weapon = 0;
			if (FindEquipped(thisObj, 17, &getObject, &itemResult))
				Players[actorNumber].ammo = *itemRef;
			else
				Players[actorNumber].ammo = 0;
			if (FindEquipped(thisObj, 20, &getObject, &itemResult))	//Robes are 20 not 18 (CS Wiki is wrong)
				Players[actorNumber].robes = *itemRef;
			else
				Players[actorNumber].robes = 0;

			NetEquipped(&Players[actorNumber], actorNumber, false);
		}
	}
	return true;
}

bool Cmd_MPGetEquipment_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPGetEquipment");
		return true;
	}
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		int actorNumber = GetActorID(ActorBuf->refID);
		int SlotID;
		if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &SlotID)) return true;
		if (actorNumber != -1 && actorNumber != -2)
		{
			UInt32* refResult = (UInt32*)result;
			switch (SlotID)
			{
			case 0:
				*refResult = Players[actorNumber].head;
				break;
			case 1:
				*refResult = Players[actorNumber].hair;
				break;
			case 2:
				*refResult = Players[actorNumber].upper_body;
				break;
			case 3:
				*refResult = Players[actorNumber].lower_body;
				break;
			case 4:
				*refResult = Players[actorNumber].hand;
				break;
			case 5:
				*refResult = Players[actorNumber].foot;
				break;
			case 6:
				*refResult = Players[actorNumber].right_ring;
				break;
			case 7:
				*refResult = Players[actorNumber].left_ring;
				break;
			case 8:
				*refResult = Players[actorNumber].amulet;
				break;
			case 13:
				*refResult = Players[actorNumber].shield;
				break;
			case 15:
				*refResult = Players[actorNumber].tail;
				break;
			case 16:
				*refResult = Players[actorNumber].weapon;
				break;
			case 17:
				*refResult = Players[actorNumber].ammo;
				break;
			case 18:
				*refResult = Players[actorNumber].robes;
				break;
			default: 
				break;
			}
		}
	}
	return true;
}

bool Cmd_MPSendModList_Execute (COMMAND_ARGS)
{
	NetSendModList();
	return true;
}

bool Cmd_MPLogModOffset_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPLogModOffset");
		return true;
	}
	int ModID;	//0 - Oblivion, 1 - OblivionOnline.esp, 2 - OOArenaMod.esp
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &ModID)) return true;
	if (ModID > 255)
		return true;
	ModList[LocalPlayer][ModID] = (thisObj->refID & 0xff000000) >> 24;
	Console_Print("Offset of %i set for modID %i", ModList[LocalPlayer][ModID], ModID);
	return true;
}

bool Cmd_MPGetMyID_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPGetEquipment");
		return true;
	}
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		int actorNumber = GetActorID(ActorBuf->refID);
		*result = (float)actorNumber;
	}
	return true;
}

//---------------------------
//---End Command Functions---
//---------------------------

//-----------------------------------
//---Begin CommandInfo Enumeration---
//-----------------------------------

static CommandInfo kMPConnectCommand =
{
	"MPConnect",
	"MPCN",
	0,
	"Connects OO to server",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPConnect_Execute
};

static CommandInfo kMPSendActorCommand =
{
	"MPSendActor",
	"MPSP",
	0,
	"Sends local player's data to server",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPSendActor_Execute
};

static CommandInfo kMPSendFullStatCommand =
{
	"MPSendFullStat",
	"MPSFS",
	0,
	"Send all stats of calling ref",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPSendFullStat_Execute
};

static CommandInfo kMPSendChatCommand =
{
	"MPSendChat",
	"Chat",
	0,
	"Sends a chat message",
	0,		// requires parent obj
	1,		// has 1 param
	kParams_OneString,	// string param table
	Cmd_MPSendChat_Execute
};

static CommandInfo kMPSyncTimeCommand =
{
	"MPSyncTime",
	"MPSTM",
	0,
	"Requests server time sync",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPSyncTime_Execute
};

static CommandInfo kMPGetPosXCommand =
{
	"MPGetPosX",
	"MPGPX",
	0,
	"Gets other players X position",
	0,		// requires parent obj
	0,		// has no param
	NULL,	// has no param table
	Cmd_MPGetPosX_Execute
};

static CommandInfo kMPGetPosYCommand =
{
	"MPGetPosY",
	"MPGPY",
	0,
	"Gets other players Y position",
	0,		// requires parent obj
	0,		// has no param
	NULL,	// has no param table
	Cmd_MPGetPosY_Execute
};

static CommandInfo kMPGetPosZCommand =
{
	"MPGetPosZ",
	"MPGPZ",
	0,
	"Gets other players Z position",
	0,		// requires parent obj
	0,		// has no param
	NULL,	// has no param table
	Cmd_MPGetPosZ_Execute
};

static CommandInfo kMPGetRotZCommand =
{
	"MPGetRotZ",
	"MPGRZ",
	0,
	"Gets other players Z rotation",
	0,		// requires parent obj
	0,		// has no param
	NULL,	// has no param table
	Cmd_MPGetRotZ_Execute
};

static CommandInfo kMPGetCellCommand =
{
	"MPGetCell",
	"MPGCL",
	0,
	"Gets other players cell",
	0,		// requires parent obj
	0,		// has no param
	NULL,	// has no param table
	Cmd_MPGetCell_Execute
};

static CommandInfo kMPGetTimeCommand =
{
	"MPGetTime",
	"MPGTM",
	0,
	"Gets players time",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetTime_Execute
};

static CommandInfo kMPGetDayCommand =
{
	"MPGetDay",
	"MPGDY",
	0,
	"Gets players day",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetDay_Execute
};

static CommandInfo kMPGetMonthCommand =
{
	"MPGetMonth",
	"MPGMN",
	0,
	"Gets players month",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetMonth_Execute
};

static CommandInfo kMPGetYearCommand =
{
	"MPGetYear",
	"MPGYR",
	0,
	"Gets players year",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetYear_Execute
};

static CommandInfo kMPGetIsInInteriorCommand =
{
	"MPGetIsInInterior",
	"MPGIII",
	0,
	"Get IsInInterior of other player",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetIsInInterior_Execute
};

static CommandInfo kMPGetStatCommand =
{
	"MPGetStat",
	"MPGST",
	0,
	"Gets stats",
	0,		// requires parent obj
	1,		// has 1 param
	kParams_OneInt,	// int param table
	Cmd_MPGetStat_Execute
};

static CommandInfo kMPGetDebugDataCommand =
{
	"MPGetDebugData",
	"MPGDD",
	0,
	"Gets debug data",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetDebugData_Execute
};

static CommandInfo kMPGetSpawnedRefCommand =
{
	"MPGetSpawnedRef",
	"MPGSPR",
	0,
	"Returns the refID of the spawn # passed in",
	0,		// requires parent obj
	1,		// has 1 param
	kParams_OneInt,	// int param table
	Cmd_MPGetSpawnedRef_Execute
};

static CommandInfo kMPSpawnedCommand =
{
	"MPSpawned",
	"MPSPW",
	0,
	"Triggered when new player NPC is spawned",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPSpawned_Execute
};

static CommandInfo kMPTotalPlayersCommand =
{
	"MPTotalPlayers",
	"MPTPS",
	0,
	"Returns number of players connected",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPTotalPlayers_Execute
};

static CommandInfo kMPDisconnectCommand =
{
	"MPDisconnect",
	"MPDC",
	0,
	"Disconnects the player",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPDisconnect_Execute
};

static CommandInfo kMPClearSpawnCommand =
{
	"MPClearSpawn",
	"MPCSP",
	0,
	"Clears the SpawnID for the calling ref",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPClearSpawn_Execute
};

static CommandInfo kMPSendEquippedCommand =
{
	"MPSendEquipped",
	"MPSE",
	0,
	"Sends the players equipment",
	0,		// requires parent obj
	0,		// no params
	NULL,	// no param table
	Cmd_MPSendEquipped_Execute
};

static CommandInfo kMPGetEquipmentCommand =
{
	"MPGetEquipment",
	"MPGE",
	0,
	"Gets the players equipment useing the int specified",
	0,		// requires parent obj
	1,		// 1 param
	kParams_OneInt,	// int param table
	Cmd_MPGetEquipment_Execute
};

static CommandInfo kMPSendModListCommand =
{
	"MPSendModList",
	"MPSML",
	0,
	"Sends the local mod list",
	0,		// requires parent obj
	0,		// no params
	NULL,	// no param table
	Cmd_MPSendModList_Execute
};

static CommandInfo kMPLogModOffsetCommand =
{
	"MPLogModOffset",
	"MPLMO",
	0,
	"Logs the mod offset of the calling ref",
	0,		// requires parent obj
	1,		// 1 param
	kParams_OneInt,	// int param table
	Cmd_MPLogModOffset_Execute
};

static CommandInfo kMPGetMyIDCommand =
{
	"MPGetMyID",
	"MPGMID",
	0,
	"Get's the player ID of the calling NPC (other player)",
	0,		// requires parent obj
	0,		// no params
	NULL,	// no param table
	Cmd_MPGetMyID_Execute
};

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
	info->version = 1;

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

	obse->SetOpcodeBase(0x2000);

	//Connection commands
	obse->RegisterCommand(&kMPConnectCommand);
	

	//Data sending
	obse->RegisterCommand(&kMPSendActorCommand);
	obse->RegisterCommand(&kMPSendFullStatCommand);
	obse->RegisterCommand(&kMPSendChatCommand);

	//Time sync
	obse->RegisterCommand(&kMPSyncTimeCommand);

	//Data injecting
	obse->RegisterCommand(&kMPGetPosXCommand);
	obse->RegisterCommand(&kMPGetPosYCommand);
	obse->RegisterCommand(&kMPGetPosZCommand);
	obse->RegisterCommand(&kMPGetRotZCommand);
	obse->RegisterCommand(&kMPGetCellCommand);
	obse->RegisterCommand(&kMPGetTimeCommand);
	obse->RegisterCommand(&kMPGetDayCommand);
	obse->RegisterCommand(&kMPGetMonthCommand);
	obse->RegisterCommand(&kMPGetYearCommand);
	obse->RegisterCommand(&kMPGetIsInInteriorCommand);
	obse->RegisterCommand(&kMPGetStatCommand);
	
	//Debug
	obse->RegisterCommand(&kMPGetDebugDataCommand);

	//Misc.
	obse->RegisterCommand(&kMPGetSpawnedRefCommand);
	obse->RegisterCommand(&kMPSpawnedCommand);
	obse->RegisterCommand(&kMPTotalPlayersCommand);
	obse->RegisterCommand(&kMPDisconnectCommand);
	obse->RegisterCommand(&kMPClearSpawnCommand);

	//Equipment
	obse->RegisterCommand(&kMPSendEquippedCommand);
	obse->RegisterCommand(&kMPGetEquipmentCommand);

	//Mod support
	obse->RegisterCommand(&kMPSendModListCommand);
	obse->RegisterCommand(&kMPLogModOffsetCommand);

	obse->RegisterCommand(&kMPGetMyIDCommand);

	_MESSAGE("Done loading OO Commands");
	return true;
}

};