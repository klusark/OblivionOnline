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
#include "Gameserver.h"
#include "Entity.h"
#include "ChunkHandler.h"
#include "InPacket.h"
size_t HandleObjectChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE Status)
{
	//DO not advance the header - instead write the object data
	FormID = pkg->ObjectIDs[GetObject(chunkdata)] = *((UINT32 *)(chunkdata + 2));
	Status = pkg->Status[GetObject(chunkdata)] = *(chunkdata + 2 +sizeof(UINT32));
	if(gs->GetEntities()->GetEntity((Status == 2),FormID) == NULL)
	{
		new Entity(gs->GetEntities(),FormID,false,false,(Status == 2),Status);
	}
	return GetMinChunkSize(PkgChunk::Object);
}