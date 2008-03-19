/*
Copyright 2008   Julian Bangert aka masterfreek64 and Joseph Pearson aka chessmaster42
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
#include "main.h"

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
		for(int i=0; i< MAXCLIENTS ; i++)
		{
			if(SpawnID[i] == actorNumber)
			{
				// conflict here
				Console_Print("This reference is already used as a controlled actor");
				return false;
				break;
			}
		}
		for(int i=0; i<MAXCLIENTS; i++)
		{
			if (!SpawnID[i])
			{
				SpawnID[i] = actorNumber;
				Console_Print("Spawn %i ID: %u", i, SpawnID[i]);
				int actorNum = GetPlayerNumberFromRefID(ActorBuf->refID);
				PlayerActorList[actorNum] = thisObj;
				return true;
			}
		}
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
			if (SpawnID[i] == (ActorBuf->refID))
			{
				SpawnID[i] = 0;
				//Console_Print("SpawnID cleared");
			}
		}
	}
	return true;
}


CommandInfo kMPClearSpawnCommand =
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
CommandInfo kMPGetSpawnedRefCommand =
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

CommandInfo kMPSpawnedCommand =
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
