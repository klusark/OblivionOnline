
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
//Reworked 3/19/2008
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
		Entity *ent = Entities.GetEntity(ActorBuf->refID);
		BYTE i = 0;
		if(ent ==NULL)
			ent = new Entity(thisObj->refID);
		for(;i < MAX_EQUIPSLOTS;i++)
		{
			if(ent->EquipChanged[i])
			{
				ent->EquipChanged[i] = false;
				*((UINT32 *)result) = ent->Equip[i];
				return true;
			}
		}
		*result = 0;
	}

	return true;
}
//TODO: Tweak this
extern bool FindEquipped(TESObjectREFR* thisObj, UInt32 slotIdx, FoundEquipped* foundEquippedFunctor, double* result);
bool Cmd_MPSendEquipped_Execute (COMMAND_ARGS)
{
	if (!thisObj)
	{
		Console_Print("Error, no reference given for MPSendEquipped");
		return true;
	}
	if (thisObj->IsActor())
	{
		double itemResult;
		UInt32* itemRef = (UInt32*)&itemResult;
		Entity * ent = Entities.GetEntity(thisObj->refID);
		feGetObject getObject;
		if(ent ==NULL)
			ent = new Entity(thisObj->refID);				
		//TODO: Unroll loop maybe
		for(int i = 0; i <= 20; i++) // traverse Slots
		{
			if(i == 18 || i == 19 || i== 9 || i == 10 || i == 11 || i == 12 || i == 14) // These do not exist 
				continue;
			if (!FindEquipped(thisObj, i, &getObject, &itemResult))
			{
				*itemRef = 0;
			}
			if( ent->Equip[i] != *itemRef)
			{
				NetSendEquip(thisObj->refID,ent->status,i,*itemRef);				
				ent->Equip[i] = *itemRef;
			}
		}
	}
	return true;
}
CommandInfo kMPSendEquippedCommand =
{
	"MPSendEquipped",
	"MPSE",
	0,
	"Sends an actors equipment",
	0,		// requires parent obj
	0,		// no params
	NULL,	// no param table
	Cmd_MPSendEquipped_Execute
};

CommandInfo kMPGetEquipmentCommand =
{
	"MPGetEquipment",
	"MPGE",
	0,
	"Gets an actors equipment in the specifiec slot",
	0,		// requires parent obj
	1,		// 1 param
	kParams_OneInt,	// int param table
	Cmd_MPGetEquipment_Execute
};