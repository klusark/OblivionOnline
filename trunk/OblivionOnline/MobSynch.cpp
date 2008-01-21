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

bool QueueMode = false;
std::queue <std::pair<TESObjectREFR *,ActorStatus>> MobQueue;
typedef std::pair<TESObjectREFR *,ActorStatus> RefrStatusPair;
hash_map<UINT32,ActorStatus> MCMobList;
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
	_MESSAGE("MCbSynchActors called");
	//rewritten
	//Just check up on the cells all other players are in
	std::list <TESObjectCELL *> CellStack;
	CellStack.push_back((*g_thePlayer)->parentCell);
	_MESSAGE("Looking up Cells");
	for(int i = 0 ; i < MAXCLIENTS;i++)
	{
		bool bInsert = true;
		if(SpawnID[i])
		{
			TESObjectREFR *form = (TESObjectREFR *)LookupFormByID(SpawnID[i]);
			// Look through the list
			std::list <TESObjectCELL *>::iterator it = CellStack.begin();
			std::list <TESObjectCELL *>::iterator end = CellStack.end();
			for(;it != end; ++it)
			{
				if( (*it)->Compare(form->parentCell) )
				{
					bInsert = false;
					break;
				}
			}
			if(bInsert)
				CellStack.push_back(form->parentCell);
		}
	}
	_MESSAGE("%d Cells for mob synch",CellStack.size());
	//now we process each cell...
	for(std::list <TESObjectCELL *>::iterator i = CellStack.begin();i != CellStack.end(); i++)
	{
		TESObjectCELL * Cell = *i;
		_MESSAGE("Processing cell %u",Cell->refID);
		TESObjectCELL::ObjectListEntry * ListIterator = &Cell->objectList;		
				
		while(ListIterator->next) // Iterate the loop
		{
			ListIterator = ListIterator->next;
			//Lookup in hash_map
			hash_map<UINT32,ActorStatus>::iterator iter =  MCMobList.find(ListIterator->refr->refID);
			if(iter == MCMobList.end())
			{
				_MESSAGE("Inserting new object");
				ActorStatus temp;
				MCMobList.insert(MobPair(ListIterator->refr->refID,temp));
				//optimisation here?
				iter =  MCMobList.find(ListIterator->refr->refID);
			}
			if((iter->second.PosX != ListIterator->refr->posX ||		//position was changed
				iter->second.PosY != ListIterator->refr->posY ||
				iter->second.PosZ != ListIterator->refr->posZ ||
				iter->second.RotZ != ListIterator->refr->rotZ )||
				iter->second.Health != (ListIterator->refr->IsActor() ? ((Actor *)ListIterator->refr)->GetActorValue(8) : -1)) // Health
			{
				_MESSAGE("Net Synching object");
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
	_MESSAGE("MPSynchActors called");
	if(bIsMasterClient && bIsConnected)
	{
		_MESSAGE("Synching Actors");
		MCbSynchActors();		
	}	
	if(bIsConnected && !bIsMasterClient)
	{
		_MESSAGE("Starting queue");
		QueueMode = true;
	}
	return true;
}
bool Cmd_MPAdvanceStack_Execute (COMMAND_ARGS)
{	
	if(QueueMode&&MobQueue.size())
	{
		MobQueue.pop();
		*result = MobQueue.front().first->refID;
		_MESSAGE("Monster %u",MobQueue.front().first->refID);
	}
	else
	{

		*result = 0;
	}
	return true;
}
bool Cmd_MPStopStack_Execute (COMMAND_ARGS)
{
	if(QueueMode)
	{
		_MESSAGE("Stopping Queue");
		QueueMode = false;
	}
	return true;
}




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
	Console_Print("Received Mob Update"); 
	_MESSAGE("Received Mob Update");
	if(!bIsMasterClient)
	{
	TESObjectREFR * Object;
	Object = (TESObjectREFR *)LookupFormByID(pkgBuf.refID);
	if(Object)
	{
		
		Actor *act = (Actor *)Object;
		if((pkgBuf.Flags & 2) && act->IsActor())
		{
			_MESSAGE("Trying to set health");
			if(pkgBuf.Flags & 8)
			{
				act->ModActorBaseValue(8,(pkgBuf.Health -act->GetActorValue(8)),0);
				act->ModActorBaseValue(9,(pkgBuf.Magika -act->GetActorValue(9)),0);
				act->ModActorBaseValue(10,(pkgBuf.Fatigue -act->GetActorValue(10)),0);
			}
			else
			{
			if(pkgBuf.Health)
				act->ModActorBaseValue(8,pkgBuf.Health,0);
			if(pkgBuf.Magika)
				act->ModActorBaseValue(9,pkgBuf.Magika,0);
			if(pkgBuf.Fatigue)
				act->ModActorBaseValue(10,pkgBuf.Fatigue,0);
			}
			_MESSAGE("Set health");
		}
		//if((*g_thePlayer)->parentCell->refID == pkgBuf.CellID) // remove that , and see if the performance allows it
		{
			RefrStatusPair temp;
			temp.first = Object;
			if (pkgBuf.Flags & 4) //Is in an exterior?
			{
				temp.second.bIsInInterior = false;
			}else{
				temp.second.bIsInInterior = true;
			}
			temp.second.InCombat = pkgBuf.Flags & 32; 
			temp.second.PosX = pkgBuf.fPosX;
			temp.second.PosY = pkgBuf.fPosY;
			temp.second.PosZ = pkgBuf.fPosZ;
			temp.second.RotX = pkgBuf.fRotX;
			temp.second.RotY = pkgBuf.fRotY;
			temp.second.RotZ = pkgBuf.fRotZ;
			MobQueue.push(temp);
		}
	}
	}
	// Do Health here....
	return true;
}

#endif