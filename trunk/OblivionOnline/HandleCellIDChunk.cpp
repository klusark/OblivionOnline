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
#include "Entity.h"
#include "ChunkHandler.h"
size_t HandleCellIDChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE Status)
{ 
	UINT32 Value = *(UINT32*)(chunkdata + 2);
	bool IsInInterior = ( (*(chunkdata + 6)) == 1 );
	if(Status == STATUS_PLAYER)
		FormID = GetPlayerFormID(FormID); // Get a player representation
	Entity * ent = Entities.GetEntity(FormID);
	if(ent == NULL)
		ent = new Entity(FormID);
	if(IsInInterior)/* We cannot verify worldspaces */
	{
		TESObjectCELL *cell = (TESObjectCELL *)LookupFormByID(Value);
		if(cell == NULL)
		{
			Console_Print("Unknown Cell");
			_ERROR("Changed into unknown cell");
			return GetMinChunkSize(CellID) + sizeof(unsigned short);
		}
	}
	
	TESObjectREFR* obj = (TESObjectREFR *)LookupFormByID(FormID);
	if(obj != NULL)
	{
		if(ent->CellID != Value ||ent->IsInInterior != IsInInterior)
		{
			ent->IsInInterior = IsInInterior;
			ent->CellID = Value;
			SafeAddUpdateQueue(ent);	
		}
	}
	//TODO : See if this works
	return GetMinChunkSize(CellID) + sizeof(unsigned short);
}