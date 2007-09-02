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

// Global variables
IDebugLog gLog("OblivionOnline.log");

bool bIsConnected = false;
int LocalPlayer;
int OtherPlayer;

UInt32 SpawnID[MAXCLIENTS];

SOCKET ServerSocket;
HANDLE hRecvThread;

PlayerStatus Players[MAXCLIENTS];
PlayerStatus PlayersInitial[MAXCLIENTS];

// Prototypes
extern void RunScriptLine(const char *buf, bool IsTemp);
extern int GetActorID(UInt32 refID);
extern float GetStat(Actor *ActorBuf, int statNum);
extern bool NetPlayerPosUpdate(PlayerStatus *Player,int PlayerID);
extern bool NetWelcome();
extern bool NetPlayerZone(PlayerStatus *Player,char *ZoneName,int PlayerID, bool bIsInterior);
extern bool NetChat(char *Message);
extern bool NetStatUpdate(PlayerStatus *Player, int PlayerID, bool FullUpdate);
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
		PlayersInitial[i].PosX = 0;
		PlayersInitial[i].PosY = 0;
		PlayersInitial[i].PosZ = -196;
		PlayersInitial[i].RotX = 0;
		PlayersInitial[i].RotY = 0;
		PlayersInitial[i].RotZ = 0;
		PlayersInitial[i].CellID = 0;
		PlayersInitial[i].Health = 42;
		PlayersInitial[i].bStatsInitialized = false;

		SpawnID[i] = 0;
	}

	return rc;
}

int OO_Deinitialize ()
{
	closesocket(ServerSocket);
	WSACleanup();
	return 1;
}

DWORD WINAPI RecvThread(LPVOID Params)
{
	char buf[512];
	int rc;

	while(1)
	{
		rc = recv(ServerSocket,buf,512,0);
		//Console_Print("Received data");
		NetReadBuffer(buf);
	}
}

//-----------------------------
//---Begin Command Functions---
//-----------------------------

bool Cmd_MPConnect_Execute(COMMAND_ARGS)
{
	_MESSAGE("Connecting \n");
	if(!bIsConnected)
	{
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
			hRecvThread = CreateThread(NULL,NULL,RecvThread,NULL,NULL,NULL);
			if(!NetWelcome()) return true;
			bIsConnected = true;
			Console_Print("Oblivion connected to server");
		}
	}
	return true;
}

bool Cmd_MPSendPos_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for command");
		return true;
	}
	if(thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		Players[LocalPlayer].RefID = ActorBuf->refID;
		Players[LocalPlayer].CellID = ActorBuf->parentCell->refID;
		Players[LocalPlayer].PosX = ActorBuf->posX;
		Players[LocalPlayer].PosY = ActorBuf->posY;
		Players[LocalPlayer].PosZ = ActorBuf->posZ;
		Players[LocalPlayer].RotX = ActorBuf->rotX;
		Players[LocalPlayer].RotY = ActorBuf->rotY;
		Players[LocalPlayer].RotZ = ActorBuf->rotZ;
		NetPlayerPosUpdate(&Players[LocalPlayer], LocalPlayer);
	}
	return true;
}

bool Cmd_MPSendWorld_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for command");
		return true;
	}
	if(thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		Players[LocalPlayer].RefID = ActorBuf->refID;
		Players[LocalPlayer].PosX = ActorBuf->posX;
		Players[LocalPlayer].PosY = ActorBuf->posY;
		Players[LocalPlayer].PosZ = ActorBuf->posZ;
		Players[LocalPlayer].RotX = ActorBuf->rotX;
		Players[LocalPlayer].RotY = ActorBuf->rotY;
		Players[LocalPlayer].RotZ = ActorBuf->rotZ;
		if (!ActorBuf->parentCell->worldSpace)
		{
			Players[LocalPlayer].CellID = ActorBuf->parentCell->refID;
			strcpy(Players[LocalPlayer].Zone, "");
			NetPlayerZone(&Players[LocalPlayer], Players[LocalPlayer].Zone, LocalPlayer, true);
		}else{
			Players[LocalPlayer].CellID = ActorBuf->parentCell->worldSpace->refID;
			strcpy(Players[LocalPlayer].Zone, ActorBuf->parentCell->worldSpace->GetEditorName());
			NetPlayerZone(&Players[LocalPlayer], Players[LocalPlayer].Zone, LocalPlayer, false);
		}
	}
	return true;
}

bool Cmd_MPSendFullStat_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for command");
		return true;
	}
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		int actorNumber = GetActorID(ActorBuf->refID);

		if (actorNumber != -1)
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
			if (!Players[actorNumber].bStatsInitialized)
			{
				PlayersInitial[actorNumber].Strength = ActorBuf->GetActorValue(0);
				PlayersInitial[actorNumber].Intelligence = ActorBuf->GetActorValue(1);
				PlayersInitial[actorNumber].Willpower = ActorBuf->GetActorValue(2);
				PlayersInitial[actorNumber].Agility = ActorBuf->GetActorValue(3);
				PlayersInitial[actorNumber].Speed = ActorBuf->GetActorValue(4);
				PlayersInitial[actorNumber].Endurance = ActorBuf->GetActorValue(5);
				PlayersInitial[actorNumber].Personality = ActorBuf->GetActorValue(6);
				PlayersInitial[actorNumber].Luck = ActorBuf->GetActorValue(7);
				PlayersInitial[actorNumber].Health = ActorBuf->GetActorValue(8);
				PlayersInitial[actorNumber].Magika = ActorBuf->GetActorValue(9);
				PlayersInitial[actorNumber].Fatigue = ActorBuf->GetActorValue(10);
				PlayersInitial[actorNumber].Encumbrance = ActorBuf->GetActorValue(11);
				PlayersInitial[actorNumber].bStatsInitialized = true;
			}

			// Check to see if we're trying to send the NPC default values
			if (Players[actorNumber].Personality != 1)
			{
				NetStatUpdate(&Players[actorNumber], actorNumber, true);
			}
		}
	}
	return true;
}

bool Cmd_MPSendStat_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPSendStat");
		return true;
	}
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
		int actorNumber = GetActorID(ActorBuf->refID);
		Console_Print("IsActor");

		if (actorNumber != -1)
		{
			Console_Print("Valid actor");
			if (ActorBuf->refID == 20)
				actorNumber = LocalPlayer;
			Players[actorNumber].Health = ActorBuf->GetActorValue(8);
			Players[actorNumber].Magika = ActorBuf->GetActorValue(9);
			Players[actorNumber].Fatigue = ActorBuf->GetActorValue(10);
			if (!Players[actorNumber].bStatsInitialized)
			{
				PlayersInitial[actorNumber].Health = ActorBuf->GetActorValue(8);
				PlayersInitial[actorNumber].Magika = ActorBuf->GetActorValue(9);
				PlayersInitial[actorNumber].Fatigue = ActorBuf->GetActorValue(10);
				Players[actorNumber].bStatsInitialized = true;
				Console_Print("Stat's initialized");
			}

			// Check to see if we're trying to send the NPC default values
			if (Players[actorNumber].Personality != 1)
			{
				NetStatUpdate(&Players[actorNumber], actorNumber, false);
			}
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
	send(ServerSocket, "TIME", 4, 0);
	return true;
}

bool Cmd_MPSyncStat_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for command");
		return true;
	}
	if (thisObj->IsActor())
	{
		Actor *ActorBuf = (Actor *)thisObj;
/*
		char tempLine[64];
		if (Players[LocalPlayer].bStatsInitialized)
		{
			if (!Players[LocalPlayer].bInitialSet)
			{
				// Set HP to initial value
				sprintf(tempLine, "player.setav Health %i", (int)PlayersInitial[LocalPlayer].Health);
				RunScriptLine(tempLine, true);
				Players[LocalPlayer].bInitialSet = true;
			}else{
				// Mod HP by difference between internal and ingame values
				int hpDiff = (int)Players[LocalPlayer].Health - (int)ActorBuf->GetActorValue(8);
				sprintf(tempLine, "player.modav Health %i", hpDiff);
				RunScriptLine(tempLine, true);
			}
		}else{
			Console_Print("Cannot sync stats, no stats have been initialized");
		}
*/
	}
	return true;
}

bool Cmd_MPGetPosX_Execute (COMMAND_ARGS)
{
	if (OtherPlayer == LocalPlayer)
	{
		*result = 0;
		return true;
	}
	*result = Players[OtherPlayer].PosX;
	return true;
}

bool Cmd_MPGetPosY_Execute (COMMAND_ARGS)
{
	if (OtherPlayer == LocalPlayer)
	{
		*result = 0;
		return true;
	}
	*result = Players[OtherPlayer].PosY;
	return true;
}

bool Cmd_MPGetPosZ_Execute (COMMAND_ARGS)
{
	if (OtherPlayer == LocalPlayer)
	{
		*result = -196;
		return true;
	}
	*result = Players[OtherPlayer].PosZ;
	return true;
}

bool Cmd_MPGetRotZ_Execute (COMMAND_ARGS)
{
	if (OtherPlayer == LocalPlayer)
	{
		*result = 0;
		return true;
	}
	*result = Players[OtherPlayer].RotZ;
	return true;
}

bool Cmd_MPGetCell_Execute (COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;
	if (OtherPlayer == LocalPlayer)
	{
		*result = 0;
		return true;
	}
	*refResult = Players[OtherPlayer].CellID;
	return true;
}

bool Cmd_MPGetTime_Execute (COMMAND_ARGS)
{
	*result = Players[LocalPlayer].Time;
	return true;
}

bool Cmd_MPGetDay_Execute (COMMAND_ARGS)
{
	if (OtherPlayer == LocalPlayer)
	{
		*result = 0;
		return true;
	}
	*result = Players[OtherPlayer].Day;
	return true;
}

bool Cmd_MPGetMonth_Execute (COMMAND_ARGS)
{
	if (OtherPlayer == LocalPlayer)
	{
		*result = 0;
		return true;
	}
	*result = Players[OtherPlayer].Month;
	return true;
}

bool Cmd_MPGetYear_Execute (COMMAND_ARGS)
{
	if (OtherPlayer == LocalPlayer)
	{
		*result = 0;
		return true;
	}
	*result = Players[OtherPlayer].Year;
	return true;
}

bool Cmd_MPGetWorldspace_Execute (COMMAND_ARGS)
{
	char testSum[128];
	strcpy(testSum, Players[OtherPlayer].Zone);
	int checkSum = 0;
	for(int i=0; i<strlen(testSum); i++)
	{
		checkSum += int(testSum[i]);
	}
	*result = checkSum;
	return true;
}

bool Cmd_MPGetStat_Execute (COMMAND_ARGS)
{
	int statNumber = 0;
	if (!thisObj)
	{
		Console_Print("Error, no reference given for command");
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

bool Cmd_MPGetSpawnedRef_Execute (COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;
	int spawnNumber = 0;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &spawnNumber)) return true;
	for(int i=0; i<MAXCLIENTS; i++)
	{
		if (spawnNumber == i)
		{
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
		Console_Print("Error, no reference given for command");
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
				sprintf(tempData, "Spawned ID: %u", SpawnID[i]);
				Console_Print(tempData);
				//End Temp
				break;
			}
		}
	}
	return true;
}

bool Cmd_MPGetMyChecksum_Execute (COMMAND_ARGS)
{
	char testData[128];
	strcpy(testData, Players[LocalPlayer].Zone);
	int checkSum = 0;
	for(int i=0; i<strlen(testData); i++)
	{
		checkSum += int(testData[i]);
	}
	sprintf(testData, "Checksum: %i", checkSum);
	Console_Print(testData);
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

static CommandInfo kMPSendPosCommand =
{
	"MPSendPos",
	"MPSP",
	0,
	"Sends local player's pos to server",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPSendPos_Execute
};

static CommandInfo kMPSendWorldCommand =
{
	"MPSendWorld",
	"MPSW",
	0,
	"Send worldspace of calling ref",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPSendWorld_Execute
};

static CommandInfo kMPSendFullStatCommand =
{
	"MPSendStat",
	"MPSFS",
	0,
	"Send all stats of calling ref",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPSendFullStat_Execute
};

static CommandInfo kMPSendStatCommand =
{
	"MPSendStat",
	"MPSST",
	0,
	"Sends basic stats (HP, MP, Fatigue)",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPSendStat_Execute
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

static CommandInfo kMPSyncStatCommand =
{
	"MPSyncStat",
	"MPSYST",
	0,
	"Set's calling ref's stat to the stored stats",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPSyncStat_Execute
};

static CommandInfo kMPGetPosXCommand =
{
	"MPGetPosX",
	"MPGPX",
	0,
	"Gets other players X position",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetPosX_Execute
};

static CommandInfo kMPGetPosYCommand =
{
	"MPGetPosY",
	"MPGPY",
	0,
	"Gets other players Y position",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetPosY_Execute
};

static CommandInfo kMPGetPosZCommand =
{
	"MPGetPosZ",
	"MPGPZ",
	0,
	"Gets other players Z position",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetPosZ_Execute
};

static CommandInfo kMPGetRotZCommand =
{
	"MPGetRotZ",
	"MPGRZ",
	0,
	"Gets other players Z rotation",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetRotZ_Execute
};

static CommandInfo kMPGetCellCommand =
{
	"MPGetCell",
	"MPGCL",
	0,
	"Gets other players cell",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetCell_Execute
};

static CommandInfo kMPGetTimeCommand =
{
	"MPGetTime",
	"MPGTM",
	0,
	"Gets other players time",
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
	"Gets other players day",
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
	"Gets other players month",
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
	"Gets other players year",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetYear_Execute
};

static CommandInfo kMPGetWorldspaceCommand =
{
	"MPGetWorldspace",
	"MPGW",
	0,
	"Get worldspace of other player",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetWorldspace_Execute
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

static CommandInfo kMPGetMyChecksumCommand =
{
	"MPGetMyChecksum",
	"MPGMCS",
	0,
	"Prints player's worldspace checksum to the console",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPGetMyChecksum_Execute
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
			_ERROR("incorrect Oblivion version (got %08X need %08X)", obse->oblivionVersion, OBLIVION_VERSION);
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

	obse->RegisterCommand(&kMPConnectCommand);

	obse->RegisterCommand(&kMPSendPosCommand);
	obse->RegisterCommand(&kMPSendWorldCommand);
	obse->RegisterCommand(&kMPSendFullStatCommand);
	obse->RegisterCommand(&kMPSendStatCommand);
	obse->RegisterCommand(&kMPSendChatCommand);

	obse->RegisterCommand(&kMPSyncTimeCommand);
	obse->RegisterCommand(&kMPSyncStatCommand);

	obse->RegisterCommand(&kMPGetPosXCommand);
	obse->RegisterCommand(&kMPGetPosYCommand);
	obse->RegisterCommand(&kMPGetPosZCommand);
	obse->RegisterCommand(&kMPGetRotZCommand);
	obse->RegisterCommand(&kMPGetCellCommand);
	obse->RegisterCommand(&kMPGetTimeCommand);
	obse->RegisterCommand(&kMPGetDayCommand);
	obse->RegisterCommand(&kMPGetMonthCommand);
	obse->RegisterCommand(&kMPGetYearCommand);
	obse->RegisterCommand(&kMPGetWorldspaceCommand);
	obse->RegisterCommand(&kMPGetStatCommand);
	obse->RegisterCommand(&kMPGetSpawnedRefCommand);

	obse->RegisterCommand(&kMPSpawnedCommand);

	obse->RegisterCommand(&kMPGetMyChecksumCommand);

	return true;
}

};