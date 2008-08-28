
/*

Copyright(c) 2007-2008   Julian Bangert aka masterfreek64

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
#include "Entity.h"
#include "NetSend.h"
#include <math.h>
extern bool FindEquipped(TESObjectREFR* thisObj, UInt32 slotIdx, FoundEquipped* foundEquippedFunctor, double* result);
bool Cmd_MPSendActor_Execute (COMMAND_ARGS)
{
	
	if(!bIsInitialized)
		return true;
	// A heavy command xD
	// 1 - send local player data up .
	// 2 - send health magicka and fatigue  + equip up.
	// if MC :
	// 2 - send up position , stat equip , etc of NPCs
	//(*g_thePlayer) is ignored
	feGetObject getObject;		
	UINT32 i;
	UINT32 ActorValue;
	double itemResult;
	UInt32* itemRef = (UInt32*)&itemResult;
	Entity *ent = Entities.GetEntity((*g_thePlayer)->refID); //Start off with the player
	Actor *actor;
	BYTE Status;
	if(ent == NULL)
		ent = new Entity((*g_thePlayer)->refID);	
	outnet.Send(); // Prevent Lag
	/*if((*g_thePlayer)->parentCell->refID != ent->CellID)
	{
		ent->CellID = (*g_thePlayer)->parentCell->refID;
		NetSendCellID((*g_thePlayer)->refID,STATUS_PLAYER,ent->CellID);
	} */
	if(abs((*g_thePlayer)->posX - ent->PosX)> 1 || abs((*g_thePlayer)->posY - ent->PosY) > 1
		|| abs((*g_thePlayer)->posZ - ent->PosZ) > 1 || abs((*g_thePlayer)->rotZ - ent->RotZ) > 1
		|| abs((*g_thePlayer)->rotX - ent->RotX)>1 ||abs((*g_thePlayer)->rotY - ent->RotY) > 1)
	{
		
		ent->PosX = (*g_thePlayer)->posX;
		ent->PosY = (*g_thePlayer)->posY;
		ent->PosZ = (*g_thePlayer)->posZ;
		ent->RotX = (*g_thePlayer)->rotX;
		ent->RotY = (*g_thePlayer)->rotY;
		ent->RotZ = (*g_thePlayer)->rotZ;
		NetSendPosition((*g_thePlayer)->refID,STATUS_PLAYER,ent->PosX,ent->PosY,ent->PosZ,ent->RotX,ent->RotY,ent->RotZ);
		ent->CellID = (*g_thePlayer)->parentCell->refID;
		NetSendCellID((*g_thePlayer)->refID,STATUS_PLAYER,ent->CellID);
	}
	
	// Health , Magicka , Fatigue
	ActorValue = (*g_thePlayer)->GetActorValue(8);
	if(ActorValue != ent->Health)
	{
		ent->Health = ActorValue;
		NetSendActorValue((*g_thePlayer)->refID,STATUS_PLAYER,8,ActorValue);
	}
	ActorValue = (*g_thePlayer)->GetActorValue(9);
	if(ActorValue != ent->Magicka)
	{
		ent->Magicka = ActorValue;
		NetSendActorValue((*g_thePlayer)->refID,STATUS_PLAYER,9,ActorValue);
	}
	ActorValue = (*g_thePlayer)->GetActorValue(10);
	if(ActorValue != ent->Fatigue)
	{
		ent->Fatigue = ActorValue;
		NetSendActorValue((*g_thePlayer)->refID,STATUS_PLAYER,10,ActorValue);
	}	
	//Now synch animations
	ActorAnimData *animdata = GetActorAnimData(*g_thePlayer);
	for(UInt32 i = 0;i < 43u;i++)
	{
		bool newValue = animdata->FindAnimInRange(i);
		if(newValue != ent->m_AnimationStatus[i])
		{
			ent->m_AnimationStatus[i] = newValue;
		}
		NetSendAnimation((*g_thePlayer)->refID,STATUS_PLAYER,i,newValue);
	}
	//player equip
		

	//TODO: Unroll loop maybe
	for(i = 0; i <= 20; i++) // traverse Slots
	{
		if(i == 18 || i == 19 || i== 9 || i == 10 || i == 11 || i == 12 || i == 14) // These do not exist 
			continue;
		if (!FindEquipped((*g_thePlayer), i, &getObject, &itemResult))
		{
			*itemRef = 0;
		}
		if( ent->Equip[i] != *itemRef)
		{
			NetSendEquip((*g_thePlayer)->refID,STATUS_PLAYER,i,*itemRef);				
			ent->Equip[i] = *itemRef;
		}
	}
	//Health of the other players
	for(i = 0;i < MAXCLIENTS ;i++)
	{
		if(SpawnID[i] != 0)
		{
			ent = Entities.GetEntity(SpawnID[i]); 		
			if(ent == NULL)
				ent = new Entity(SpawnID[i]);
			actor = (Actor *)LookupFormByID(SpawnID[i]);
			ActorValue = actor->GetActorValue(8);
			if(ActorValue != ent->Health)
			{
				ent->Health = ActorValue;
				NetSendActorValue(i,STATUS_PLAYER,8,ActorValue);
			}
			ActorValue = actor->GetActorValue(9);
			if(ActorValue != ent->Magicka)
			{
				ent->Magicka = ActorValue;
				NetSendActorValue(i,STATUS_PLAYER,9,ActorValue);
			}
			ActorValue = actor->GetActorValue(10);
			if(ActorValue != ent->Fatigue)
			{
				ent->Fatigue = ActorValue;
				NetSendActorValue(i,STATUS_PLAYER,10,ActorValue);
			}	
		}
	}
	if(bIsMasterClient)
	{

		//rewritten
		//Just check up on the cells all other players are in
		std::list <TESObjectCELL *> CellStack;
		CellStack.push_back((*g_thePlayer)->parentCell);
		for(i = 0 ; i < MAXCLIENTS;i++)
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
		//now we process each cell...
		for(std::list <TESObjectCELL *>::iterator i = CellStack.begin();i != CellStack.end(); i++)
		{
			TESObjectCELL * Cell = *i;
			TESObjectCELL::ObjectListEntry * ListIterator = &(Cell->objectList);		

			while(ListIterator && ListIterator->refr) // Iterate the loop
			{
				if(ListIterator->refr->IsActor())
				{
					Status = STATUS_NPC; // We ignore player objects - so
				
					if(GetPlayerNumberFromRefID(ListIterator->refr->refID) == -1) // Do not synchronise objects used by OblivionOnline
					{
						ent = Entities.GetEntity(ListIterator->refr->refID);
						
						if(ent == NULL)
							ent = new Entity(ListIterator->refr->refID);
						//Sync that object too
						/*
						if(ListIterator->refr->parentCell->refID != ent->CellID)
						{
							ent->CellID = ListIterator->refr->parentCell->refID;
							NetSendCellID(ListIterator->refr->refID,Status,ent->CellID);
						} */
						if(abs(ListIterator->refr->posX - ent->PosX) > 1||
							abs(ListIterator->refr->posY - ent->PosY) > 1 ||
							abs(ListIterator->refr->posZ - ent->PosZ) > 1 ||
							abs(ListIterator->refr->rotZ - ent->RotZ) > 1 ||
							abs(ListIterator->refr->rotX - ent->RotX) > 1 ||
							abs(ListIterator->refr->rotY - ent->RotY) > 1)
						{
							ent->PosX = ListIterator->refr->posX;
							ent->PosY = ListIterator->refr->posY;
							ent->PosZ = ListIterator->refr->posZ;
							ent->RotX = ListIterator->refr->rotX;
							ent->RotY = ListIterator->refr->rotY;
							ent->RotZ = ListIterator->refr->rotZ;
							NetSendPosition(ListIterator->refr->refID, Status,ent->PosX,ent->PosY,ent->PosZ,ent->RotX,ent->RotY,ent->RotZ);
							ent->CellID = ListIterator->refr->parentCell->refID;
							NetSendCellID(ListIterator->refr->refID,Status,ent->CellID);
						}
						
						if(Status == STATUS_NPC)
						{
							actor = (Actor *)LookupFormByID(ListIterator->refr->refID);
							ActorValue = actor->GetActorValue(8);
							if(ActorValue != ent->Health)
							{
								ent->Health = ActorValue;
								NetSendActorValue(ListIterator->refr->refID,Status,8,ActorValue);
							}
							ActorValue = actor->GetActorValue(9);
							if(ActorValue != ent->Magicka)
							{
								ent->Magicka = ActorValue;
								NetSendActorValue(ListIterator->refr->refID,Status,9,ActorValue);
							}
							ActorValue = actor->GetActorValue(10);
							if(ActorValue != ent->Fatigue)
							{
								ent->Fatigue = ActorValue;
								NetSendActorValue(ListIterator->refr->refID,Status,10,ActorValue);
							}	
							animdata = GetActorAnimData(ListIterator->refr);
							for(UInt32 i = 0;i < 43u;i++)
							{
								bool newValue = animdata->FindAnimInRange(i);
								if(newValue != ent->m_AnimationStatus[i])
								{
									ent->m_AnimationStatus[i] = newValue;
								}
								NetSendAnimation((*g_thePlayer)->refID,STATUS_PLAYER,i,newValue);
							}
						}
					}
				}
				ListIterator = ListIterator->next;
			}
		}
	}
	outnet.Send();
	return true;
}


CommandInfo kMPSendActorCommand =
{
	"MPSendActor",
	"MPSP",
	0,
	"Sends out data - long long routine",
	0,		// requires parent obj
	0,		// doesn't have params
	NULL,	// no param table
	Cmd_MPSendActor_Execute
};