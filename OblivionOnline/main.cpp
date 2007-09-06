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
int LocalPlayer;
int OtherPlayer;
bool PlayerConnected[MAXCLIENTS];
int TotalPlayers;

UInt32 SpawnID[MAXCLIENTS];

SOCKET ServerSocket;
HANDLE hRecvThread;

PlayerStatus Players[MAXCLIENTS];

DWORD PacketTime[PACKET_COUNT]; //System time when this packet was received.

// Prototypes
extern void RunScriptLine(const char *buf, bool IsTemp);
extern int GetActorID(UInt32 refID);
extern float GetStat(Actor *ActorBuf, int statNum);

extern bool NetActorUpdate(PlayerStatus *Player,int PlayerID);
extern bool NetWelcome();
extern bool NetDisconnect();
extern bool NetChat(char *Message);
extern bool NetFullStatUpdate(PlayerStatus *Player, int PlayerID);
extern bool NetReadBuffer(char *acReadBuffer);


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
		Players[i].CellID = 0;
		Players[i].Health = 1;
		Players[i].bStatsInitialized = false;
		Players[i].bIsInInterior = true;

		SpawnID[i] = 0;
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
		Players[i].CellID = 0;
		Players[i].Health = 1;
		Players[i].bStatsInitialized = false;
		Players[i].bIsInInterior = true;

		SpawnID[i] = 0;
	}
	TerminateThread(hRecvThread, 0);
	CloseHandle(hRecvThread);
	closesocket(ServerSocket);
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
		NetReadBuffer(buf);
	}
	return 0;
}

//-----------------------------
//---Begin Command Functions---
//-----------------------------

bool Cmd_MPConnect_Execute(COMMAND_ARGS)
{
	if(!bIsConnected)
	{
		_MESSAGE("Connecting \n");
		OO_Initialize();
		SOCKADDR_IN ServerAddr;
		FILE *Realmlist = fopen("realmlist.wth","r");
		char IP[15];
		long rc;
		fscanf(Realmlist,"%s",IP);
		memset(&ServerAddr,NULL,sizeof(SOCKADDR_IN));
		ServerAddr.sin_addr.s_addr = inet_addr(IP);
		ServerAddr.sin_port = htons(41805);
		ServerAddr.sin_family = AF_INET;
		rc = connect(ServerSocket,(SOCKADDR *)&ServerAddr,sizeof(SOCKADDR));
		if(rc == SOCKET_ERROR)
		{
			_ERROR("Couldn't connect %u",WSAGetLastError());
			Console_Print("Oblivion not connected to server");
		}
		else 
		{
			_MESSAGE("Successfully Connected");
			bIsConnected = true;
			hRecvThread = CreateThread(NULL,NULL,RecvThread,NULL,NULL,NULL);
			if(!NetWelcome()) return true;
			Console_Print("Oblivion connected to server");
			TotalPlayers = 1;
		}
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

		if (actorNumber != -1)
		{
			if (actorNumber == -2)
				actorNumber = LocalPlayer;
			Players[actorNumber].RefID = ActorBuf->refID;
			Players[actorNumber].PosX = ActorBuf->posX;
			Players[actorNumber].PosY = ActorBuf->posY;
			Players[actorNumber].PosZ = ActorBuf->posZ;
			Players[actorNumber].RotX = ActorBuf->rotX;
			Players[actorNumber].RotY = ActorBuf->rotY;
			Players[actorNumber].RotZ = ActorBuf->rotZ;
			Players[actorNumber].Health = ActorBuf->GetActorValue(8);
			Players[actorNumber].Magika = ActorBuf->GetActorValue(9);
			Players[actorNumber].Fatigue = ActorBuf->GetActorValue(10);
			if(ActorBuf->parentCell->worldSpace)
			{
				Players[actorNumber].bIsInInterior = false;
				Players[actorNumber].CellID = ActorBuf->parentCell->worldSpace->refID;
			}else{
				Players[actorNumber].bIsInInterior = true;
				Players[actorNumber].CellID = ActorBuf->parentCell->refID;
			}
			NetActorUpdate(&Players[actorNumber], actorNumber);
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
			Players[actorNumber].Strength = ActorBuf->GetActorValue(0);
			Players[actorNumber].Intelligence = ActorBuf->GetActorValue(1);
			Players[actorNumber].Willpower = ActorBuf->GetActorValue(2);
			Players[actorNumber].Agility = ActorBuf->GetActorValue(3);
			Players[actorNumber].Speed = ActorBuf->GetActorValue(4);
			Players[actorNumber].Endurance = ActorBuf->GetActorValue(5);
			Players[actorNumber].Personality = ActorBuf->GetActorValue(6);
			Players[actorNumber].Luck = ActorBuf->GetActorValue(7);
			Players[actorNumber].Health = ActorBuf->GetActorValue(8);
			Players[actorNumber].Magika = ActorBuf->GetActorValue(9);
			Players[actorNumber].Fatigue = ActorBuf->GetActorValue(10);
			Players[actorNumber].Encumbrance = ActorBuf->GetActorValue(11);
			
			// Check to see if we're trying to send the NPC default values
			if (Players[actorNumber].Personality != 1)
			{
				NetFullStatUpdate(&Players[actorNumber], actorNumber);
			}
		}else{
			if (actorNumber == -2)
				Console_Print("Cannot update, only one player is connected");
		}
	}
	return true;
}

bool Cmd_MPSendChat_Execute (COMMAND_ARGS)
{
	char message[512] = "\0";
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &message)) return true;
	NetChat(message);
	return true;
}

bool Cmd_MPSyncTime_Execute (COMMAND_ARGS)
{
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
		*result = statAmount;
	}
	return true;
}

bool Cmd_MPGetOtherPlayer_Execute (COMMAND_ARGS)
{
	*result = OtherPlayer;
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
		for(int i=0; i<MAXCLIENTS; i++)
		{
			if (!SpawnID[i])
			{
				SpawnID[i] = actorNumber;
				//Temp
				char tempData[64];
				sprintf(tempData, "Spawn %i ID: %u", i, SpawnID[i]);
				Console_Print(tempData);
				//End Temp
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

//---------------------------
//---End Command Functions---
//---------------------------

//-------------------------------
//---Begin Command Enumeration---
//-------------------------------

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

static CommandInfo kMPGetOtherPlayer =
{
	"MPGetOtherPlayer",
	"MPGOP",
	0,
	"Gets other player number",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetOtherPlayer_Execute
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

//-----------------------------
//---End Command Enumeration---
//-----------------------------

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

	//Data sync
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
	obse->RegisterCommand(&kMPGetOtherPlayer);
	obse->RegisterCommand(&kMPGetSpawnedRefCommand);

	//Misc.
	obse->RegisterCommand(&kMPSpawnedCommand);
	obse->RegisterCommand(&kMPTotalPlayersCommand);
	//should be under connections but it causes a bug then. 
	obse->RegisterCommand(&kMPDisconnectCommand);
	//Debug commands

	_MESSAGE("Done loading OO Commands");
	return true;
}

};