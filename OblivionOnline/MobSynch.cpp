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
bool NetSynchNPC(Actor *Actor)
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
bool MCAddClientCache(char *FileName) // add this file
{
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
		tempBuf.Actor = (Actor *) LookupFormByID(RefID); // here we look it up
		if(tempBuf.Actor)
		{
			if(tempBuf.Actor->refID)
			{
				
				MCCache.push_back(tempBuf); //ok we pushed him in
				fprintf(LogFile,"%s %u was detected at %ul\n",RefName,RefID,tempBuf.Actor);
			}
			else
			{
				fprintf(LogFile,"%s %u failed detection. Invalid REFID",RefName,RefID);
				Console_Print("Couldn't find reference %s",RefName);
			}
		}
		else
		{
			fprintf(LogFile,"%s %u failed detection.Not present",RefName,RefID);
			Console_Print("Couldn't find reference %s",RefName);
		}
	}
	Console_Print("Cache build completed, %i References found",MCCache.size());
	bCacheBuilt = true;
	fclose(LogFile);
	return true;
}
 bool MCbSynchActors() //called nearly every frame , so extremely important
{
	DWORD tickBuf;
	tickBuf=GetTickCount();
	if((tickBuf - MobResynchTimer) > MC_MOBRESYNCH) //just synch every 50 ms 
	{
	std::list<MCActorBuf>::iterator ActorIterator;
	std::list<MCActorBuf>::iterator EndIterator = MCCache.end();
	for(ActorIterator = MCCache.begin();ActorIterator!= EndIterator;ActorIterator++)
	{
		if(ActorIterator->Actor)
		{
		if((ActorIterator->Actor->posX != ActorIterator->LastStatus.PosX)
			|| (ActorIterator->Actor->posY != ActorIterator->LastStatus.PosY)
			|| (ActorIterator->Actor->posZ != ActorIterator->LastStatus.PosZ))
		{
			//Update this Actor on the net 
			NetSynchNPC(ActorIterator->Actor);
			//Update LastPos
			ActorIterator->LastStatus.PosX = ActorIterator->Actor->posX;
			ActorIterator->LastStatus.PosY = ActorIterator->Actor->posY;
			ActorIterator->LastStatus.PosZ = ActorIterator->Actor->posZ;
		}
		}
		else
		{
			Console_Print("Detected failed injection !");
		}
	}
	MobResynchTimer = tickBuf;
	}
	return true;
}
bool Cmd_MPSynchActors_Execute (COMMAND_ARGS)
{

	if(bIsMasterClient)
	{
	if(bCacheBuilt)
		MCbSynchActors();
	}
	
	return true;
}
bool Cmd_MPBuildCache_Execute(COMMAND_ARGS)
{
	if(bIsConnected)
	{
	MCAddClientCache("Oblivion.ooc");
	bIsMasterClient = true;
	}
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

typedef std::pair< UINT32 , std::string > PCPair;
stdext::hash_map<UINT32,std::string>  PCList;//fix up the hash code in itself
bool PCAddFile(char *FileName)
{
	FILE *CacheFile;
	
	char RefName[256];
	UINT32 RefID;
	CacheFile = fopen(FileName,"r");
	while(!feof(CacheFile))
	{
		fscanf(CacheFile,"%s", RefName);
		fscanf(CacheFile,"%u",&RefID);
		PCList.insert(PCPair(RefID,RefName) );
	}
	return true;
}
bool NetHandleMobUpdate(OOPkgActorUpdate pkgBuf) // called from the packet Handler
{
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
	}
	}
	// Do Health here....
	return true;
}

#endif