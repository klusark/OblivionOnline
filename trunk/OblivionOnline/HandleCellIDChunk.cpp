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
size_t HandleCellIDChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE Status)
{ 
	UINT32 Value = *(UINT32*)(chunkdata + 2);
	TESObjectREFR *obj;
	TESObjectCELL *cell;
	if(Status == STATUS_PLAYER)
		FormID = GetPlayerFormID(FormID); // Get a player representation
	obj = (TESObjectREFR *)LookupFormByID(FormID);
	cell = (TESObjectCELL *)Oblivion_DynamicCast(LookupFormByID(Value),0,RTTI_TESForm,RTTI_TESObjectCELL,0);
	if(cell == NULL)
	{
		if(Status == STATUS_PLAYER)
		{
			Console_Print("Player %u changed to unknown cell %u",FormID,Value);
		}
		else
		{
			Console_Print("Object %u changed to unknown cell %u",FormID,Value);			
		}
		return GetMinChunkSize(CellID);
	}
	obj->ChangeCell(cell);
	Console_Print("Entity %u changed to cell %u",FormID,Value);
	//TODO : See if this works
	return GetMinChunkSize(CellID);
}