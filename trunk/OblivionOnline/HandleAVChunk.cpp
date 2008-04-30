/*
Copyright 2008   Julian Bangert aka masterfreek64
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
#include "InPacket.h"
#include "ChunkHandler.h"
size_t HandleAVChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE Status)
{ 
	signed short Value = *(unsigned short*)(chunkdata + 2);
	Actor *Npc;// = Oblivion_DynamicCast(LookupFormByID(FormID),0,RTTI_TESObjectREFR,RTTI_TESNPC,0);
	if(Status < STATUS_NPC)
	{
		_ERROR("Health packet received for non-actor entity");
		Console_Print("Health packet received for non-actor entity");
		return GetMinChunkSize(ActorValue);
	}
	if(Status == STATUS_PLAYER)
		FormID = GetPlayerFormID(FormID); // Get a player representation
	Npc = (Actor *)LookupFormByID(FormID);
	if(Npc != NULL && Npc->IsActor())
		Npc->SetActorValue( *(chunkdata +2), *(short *)(chunkdata+3));
	return GetMinChunkSize(ActorValue) + sizeof(unsigned short);
}