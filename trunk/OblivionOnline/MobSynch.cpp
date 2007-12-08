/*

Copyright 2007   Julian Bangert aka masterfreek64
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
using namespace std; // hash_map "interoperability"
using namespace stdext;
extern void RunScriptLine(const char *buf, bool IsTemp);

bool MCStackMode = false;
bool MCbWritten;

static std::stack <ActorStatus> MobStack;
hash_map<UINT32,ActorStatus> MobList;
typedef std::pair<UINT32,ActorStatus> MobPair; 
DWORD MobResynchTimer ; // seperate , because no seperate packet
bool bIsMasterClient = false;
// Here we send an NPC over the net
// The bug persists ....
bool NetSynchObject(TESObjectREFR *Refr)
{
	try
	{
	OOPkgActorUpdate pkgBuf;
	pkgBuf.etypeID = OOPActorUpdate;
	pkgBuf.fPosX = Refr->posX ; //Actor
	pkgBuf.fPosY = Refr->posY;
	pkgBuf.fPosZ = Refr->posZ;
	pkgBuf.fRotX = Refr->rotX;
	pkgBuf.fRotY = Refr->rotY;
	pkgBuf.fRotZ = Refr->rotZ;
	pkgBuf.Health = (Refr->IsActor() ? ((Actor *)Refr)->GetActorValue(8) : -1);
	pkgBuf.Magika = (Refr->IsActor() ? ((Actor *)Refr)->GetActorValue(9) : -1);
	pkgBuf.Fatigue =(Refr->IsActor() ? ((Actor *)Refr)->GetActorValue(10) : -1);
	_MESSAGE("Synchronising : %u %s",Refr->refID,Refr->GetEditorName());
	if(Refr->parentCell->worldSpace)
	{
				pkgBuf.Flags = 4; //Exterior
				pkgBuf.CellID = Refr->parentCell->worldSpace->refID;
	}
	else
	{
				pkgBuf.Flags = 0; //Interior
				pkgBuf.CellID = Refr->parentCell->refID;
	}
	if(Refr->IsActor())
	{
		pkgBuf.Flags |= 2; //Actor
	}
	pkgBuf.refID = Refr->refID;
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
// We set us to passive. We still keep the cache if present
bool MCMakePassive()		//changes client mode to passive
{
	bIsMasterClient = false;
	return true;
}
bool MCMakeMC()
{
	bIsMasterClient = true;
	return true;
}
// This is used in a command to go thorugh the cache and look for mobs that changed position
 bool MCbSynchActors() //called nearly every frame , so extremely important
{
	//rewritten
	//Just check up on the cells all other players are in
	std::stack<TESObjectCELL *> CellStack;
	CellStack.push((*g_thePlayer)->parentCell);
	for(int i = 0 ; i < MAXCLIENTS;i++)
	{
		TESForm *form = LookupFormByID(SpawnID[i]);
		if(form)
		{
			CellStack.push(((TESObjectREFR *)form)->parentCell);
		}
	}
	//now we process each cell...
	while(!CellStack.empty())
	{
		TESObjectCELL * Cell = CellStack.top();
		CellStack.pop();
		TESObjectCELL::ObjectListEntry * ListIterator = &Cell->objectList;		
				
		while(ListIterator->next) // Iterate the loop
		{
			ListIterator = ListIterator->next;
			//Lookup in hash_map
			hash_map<UINT32,ActorStatus>::iterator iter =  MobList.find(ListIterator->refr->refID);
			if(iter == MobList.end())
			{
				ActorStatus temp;
				MobList.insert(MobPair(ListIterator->refr->refID,temp));
				//optimisation here?
				iter =  MobList.find(ListIterator->refr->refID);
			}
			if((iter->second.PosX != ListIterator->refr->posX ||		//position was changed
				iter->second.PosY != ListIterator->refr->posY ||
				iter->second.PosZ != ListIterator->refr->posZ ||
				iter->second.RotZ != ListIterator->refr->rotZ )||
				iter->second.Health != (ListIterator->refr->IsActor() ? ((Actor *)ListIterator->refr)->GetActorValue(8) : -1)) // Health
			{
				iter->second.PosX = ListIterator->refr->posX;
				iter->second.PosY = ListIterator->refr->posY;
				iter->second.PosZ = ListIterator->refr->posZ;
				iter->second.RotZ = ListIterator->refr->rotZ;				
				iter->second.Health = (ListIterator->refr->IsActor() ? ((Actor *)ListIterator->refr)->GetActorValue(8) : -1);
				NetSynchObject(ListIterator->refr);
			}
		}
	}
	return false;	
}
 // This just calls MCbSynchActors
bool Cmd_MPSynchActors_Execute (COMMAND_ARGS)
{

	if(bIsMasterClient && bIsConnected)
	{
		MCbSynchActors();		
	}	
	//generate the stack
	if(bIsConnected)
		MCStackMode = true;
	return true;
}
bool Cmd_MPAdvanceStack_Execute (COMMAND_ARGS)
{
	if(MCStackMode)
	{
			
	}
	return true;
}
bool Cmd_MPStopStack_Execute (COMMAND_ARGS)
{
	if(MCStackMode)
	{
		MCStackMode = false;
	}
	return true;
}

//Used manually to build the cache .... SHOULD NOT BE USED atm.... we use the welcome code for that




 CommandInfo kMPSynchActorsCommand =
{
	"MPSynchActors",
	"MPSA",
	0,
	"Synchs Objects and resets +enables the stack",
	0,		 
	0,		
	NULL,	
	Cmd_MPSynchActors_Execute
};
  CommandInfo kMPStopStackCommand =
{
	"MPStopStack",
	"MPSTS",
	0,
	"Exits stack processing mode",
	0,		 
	0,		
	NULL,	
	Cmd_MPStopStack_Execute
};
 CommandInfo kMPAdvanceStackCommand =
{
	"MPAdvanceStack",
	"MPAS",
	0,
	"Advances the Stack by 1 ",
	0,		 // well it NEEDS one...
	0,		
	NULL,	// one string
	Cmd_MPAdvanceStack_Execute
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