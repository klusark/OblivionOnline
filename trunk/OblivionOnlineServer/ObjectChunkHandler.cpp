/*
This file is part of OblivionOnline Server- An open source game server for the OblivionOnline mod
Copyright (C)  2008   Julian Bangert

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.


You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Gameserver.h"
#include "Entity.h"
#include "ChunkHandler.h"
#include "InPacket.h"
size_t HandleObjectChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE Status)
{
	//DO not advance the header - instead write the object data
	FormID = pkg->ObjectIDs[GetObject(chunkdata)] = *((UINT32 *)(chunkdata + 2));
	Status = pkg->Status[GetObject(chunkdata)] = *(chunkdata + 2 +sizeof(UINT32));
	if(gs->GetEntities()->GetEntity(Status == STATUS_PLAYER,FormID) == NULL)
	{
		new Entity(gs->GetEntities(),FormID,false,false,(Status == 2),Status);
	}
	return GetMinChunkSize(PkgChunk::Object) + sizeof(unsigned short);
}