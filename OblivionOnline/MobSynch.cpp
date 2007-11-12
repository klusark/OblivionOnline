/*

Copyright 2007   Julian Bangert aka masterfreek64, Joseph Pearson aka chessmaster42 and Joel Teichroeb aka bobjr777

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

//This is just to synchronize Actors. It is highly experimental and NOT completed
#define MOBSYNCH
#ifdef MOBSYNCH
#include "MobSynch.h"

extern void RunScriptLine(const char *buf, bool IsTemp);


bool MCbWritten;
std::list<MCActorBuf> MCCache;
DWORD MobResynchTimer ; // seperate , because no seperate packet
bool bIsMasterClient = false;
bool bCacheBuilt = false;
bool ActorSynchCalled = false;
// Here we send an NPC over the net
// The bug persists ....
bool NetSynchNPC(Actor *Actor)
{
	try
	{
	OOPkgActorUpdate pkgBuf;
	pkgBuf.etypeID = OOPActorUpdate;
	pkgBuf.fPosX = Actor->posX ; //Actor
	pkgBuf.fPosY = Actor->posY;
	pkgBuf.fPosZ = Actor->posZ;
	pkgBuf.fRotX = Actor->rotX;
	pkgBuf.fRotY = Actor->rotY;
	pkgBuf.fRotZ = Actor->rotZ;
	pkgBuf.Health = Actor->GetActorValue(8);
	pkgBuf.Magika = Actor->GetActorValue(9);
	pkgBuf.Fatigue = Actor->GetActorValue(10);
	_MESSAGE("Synchronising : %u %s",Actor->refID,Actor->GetEditorName());
	if(Actor->parentCell->worldSpace)
	{
				pkgBuf.Flags = 2|4; //Exterior
				pkgBuf.CellID = Actor->parentCell->worldSpace->refID;
	}
	else
	{
				pkgBuf.Flags = 2; //Interior
				pkgBuf.CellID = Actor->parentCell->refID;
	}
	pkgBuf.refID = Actor->refID;
	send(ServerSocket,(char *)&pkgBuf,sizeof(OOPkgActorUpdate),0);
	return true;
	}
	catch(...)
	{
		_MESSAGE("Failed Actor Network Synch");
		throw 1;
		return false;
	}
}
// We load an .ooc file
bool MCAddClientCache(char *FileName) // add this file
{
	_MESSAGE("Starting to build Cache");
	FILE *CacheFile;
	FILE *LogFile;
	MCActorBuf tempBuf;
	char RefName[256];
	UINT32 RefID;
	CacheFile = fopen(FileName,"r");
	LogFile = fopen("Cache.log","w");

	while(!feof(CacheFile))
	{
		fscanf(CacheFile,"%s", RefName);
		fscanf(CacheFile,"%u",&RefID);
		tempBuf.RefID = RefID;
		MCCache.push_back(tempBuf); //ok we pushed him in
		fprintf(LogFile,"%s %u was injected\n",RefName,RefID);		
	}
	Console_Print("Cache build completed, %i References found",MCCache.size());
	_MESSAGE("Cache Built");
	bCacheBuilt = true;
	fclose(LogFile);
	return true;
}
// We load all OOCs. And make us a Master Client
bool MCBuildCache()
{
	Console_Print("Building Master Client Cache");
	if(!bCacheBuilt)
	{
		MCAddClientCache("OblivionOnline/Oblivion.ooc");
	}
	bIsMasterClient = true;
	return true;
}
// We set us to passive. We still keep the cache if present
bool MCMakePassive()		//changes client mode to passive
{
	bIsMasterClient = false;
	return true;
}

// This is used in a command to go thorugh the cache and look for mobs that changed position
 bool MCbSynchActors() //called nearly every frame , so extremely important
{

	#if 1 
	
	try
	{
	if(!ActorSynchCalled)
	{
		Console_Print("Actor Synchronisation working");
		_MESSAGE("WORKING MCBActorSynch");
		ActorSynchCalled = true;
	}
	DWORD tickBuf;
	tickBuf=GetTickCount();
	if((tickBuf - MobResynchTimer) > MC_MOBRESYNCH) //just synch every 50 ms 
	{
	
	std::list<MCActorBuf>::iterator ActorIterator;
	std::list<MCActorBuf>::iterator EndIterator = MCCache.end();
	for(ActorIterator = MCCache.begin();ActorIterator!= EndIterator;ActorIterator++)
	{
		// This is more secure than SVN 354. If there are performance problems with that solution , try to revert to SVN 354
		Actor * TempActor = reinterpret_cast<Actor *> (LookupFormByID(ActorIterator->RefID));
		if(TempActor)
		{
			if((TempActor->posX != ActorIterator->LastStatus.PosX)
					|| (TempActor->posY != ActorIterator->LastStatus.PosY)
					|| (TempActor->posZ != ActorIterator->LastStatus.PosZ))
				{
					//Update this Actor on the net 
					NetSynchNPC(TempActor);
					//Update LastPos
					ActorIterator->LastStatus.PosX = TempActor->posX;
					ActorIterator->LastStatus.PosY = TempActor->posY;
					ActorIterator->LastStatus.PosZ = TempActor->posZ;
				}
		}
		
		else
		{
			Console_Print("Found a Mob not present . Removing it");
				MCCache.erase(ActorIterator);
		}

	}
	MobResynchTimer = tickBuf;
		return true;
	}
	}
	catch (...)
	{
		_MESSAGE("Failed to Synchronise Actors");
		return false;
	}
	return false;
	#endif
	//brand new testing stuff, DO NOT USE. Always comment this before building
#if 0
	char OutputString[1200];
	sprintf(OutputString,"Data :%512s %512s",
	(*g_thePlayer)->parentCell->objectList->unk0.data, //some pointer. Probably an Actor Pointer- will dump data and see,
	(*g_thePlayer)->parentCell->objectList->unk0.next);
	Console_Print(OutputString);
	return true;
#endif	
}
 // This just calls MCbSynchActors
bool Cmd_MPSynchActors_Execute (COMMAND_ARGS)
{

	if(bIsMasterClient && bIsConnected)
	{
	if(bCacheBuilt)
		MCbSynchActors();
	}
	
	return true;
}
//Used manually to build the cache .... SHOULD NOT BE USED atm.... we use the welcome code for that
bool Cmd_MPBuildCache_Execute(COMMAND_ARGS)
{
	if(bIsConnected)
		MCBuildCache();
	return true;
}



 CommandInfo kMPSynchActorsCommand =
{
	"MPSynchActors",
	"MPSA",
	0,
	"Synchs Objects",
	0,		 // well it NEEDS one...
	0,		
	NULL,	// one string
	Cmd_MPSynchActors_Execute
};
 CommandInfo kMPBuildCacheCommand =
{
	"MPBuildCache",
	"MPBC",
	0,
	"Adds an Object to the OO master cache . Not to be used manually",
	0,		 // well it NEEDS one...
	0,		
	NULL,	// one string
	Cmd_MPBuildCache_Execute
};


bool NetHandleMobUpdate(OOPkgActorUpdate pkgBuf) // called from the packet Handler
{
	_MESSAGE("Received Mob Update"); 
	std::string ScriptString;
	if(!bIsMasterClient)
	{
		Actor * Object;
		Object = (Actor *)LookupFormByID(pkgBuf.refID);
		if(Object)
		{
		ScriptString = Object->GetEditorName();
		ScriptString += ".MoveTo" ;
		ScriptString += pkgBuf.fPosX;
		ScriptString += ",";
		ScriptString += pkgBuf.fPosY;
		ScriptString += ",";
		ScriptString += pkgBuf.fPosZ;
		ScriptString += ",";
		ScriptString += pkgBuf.fRotZ;
		ScriptString += ",";
		ScriptString += pkgBuf.CellID;
		RunScriptLine(ScriptString.c_str(),true);
		_MESSAGE("Injected script: \" %s! \"",ScriptString.c_str());
		if(pkgBuf.Flags & 8)
		{
			Object->ModActorBaseValue(8,(pkgBuf.Health -Object->GetActorValue(8)),0);
			Object->ModActorBaseValue(9,(pkgBuf.Magika -Object->GetActorValue(9)),0);
			Object->ModActorBaseValue(10,(pkgBuf.Fatigue -Object->GetActorValue(10)),0);
		}
		else
		{
		if(pkgBuf.Health)
			Object->ModActorBaseValue(8,pkgBuf.Health,0);
		if(pkgBuf.Magika)
			Object->ModActorBaseValue(9,pkgBuf.Magika,0);
		if(pkgBuf.Fatigue)
			Object->ModActorBaseValue(10,pkgBuf.Fatigue,0);
		}

	}
	}
	// Do Health here....
	return true;
}

#endif