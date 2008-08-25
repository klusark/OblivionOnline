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
#include "ChunkHandler.h"
#include "InPacket.h"
#include "Entity.h"
size_t HandlePositionChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status)
{
	Entity *ent = gs->GetEntities()->GetEntity(status,FormID);
	float *floats = (float *)(chunkdata + 2);
	if(ent == NULL)
	{
		gs->GetIO()<<Error<<"Error handling position chunk: Entity not registered"<< endl;
		return 0;
	}	
	gs->GetIO()<<GameMessage<<"Entity "<<FormID<<" updated its position to "<<floats[0]<<' '<<floats[1]<<' '<<floats[2]<<' '<<floats[3]<<' '<<floats[4]<<' '<<floats[5]<<"in cell "<<ent->CellID()<<endl;
	ent->MoveNRot(floats[0],floats[1],floats[2],floats[3],floats[4],floats[5]);
	return GetMinChunkSize(PkgChunk::Position) + sizeof(unsigned short);
}
size_t HandleCellIDChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status)
{
	Entity *ent = gs->GetEntities()->GetEntity(status,FormID);
	if(ent == NULL)
	{
		gs->GetIO()<<Error<<"Error handling CellID chunk: Entity not registered"<< endl;
		return 0;
	}	
	ent->SetCell(*(UINT32*)(chunkdata +2));
	return GetMinChunkSize(PkgChunk::CellID) + sizeof(unsigned short);
}

size_t HandleAnimationChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status)
{
	BYTE AnimationID,AnimStatus;
	AnimationID = *(chunkdata + 2);
	AnimStatus = *(chunkdata +3);
	Entity *ent = gs->GetEntities()->GetEntity(status,FormID);
	if(ent == NULL)
	{
		gs->GetIO()<<Error<<"Error handling Animation chunk: Entity not registered"<< endl;
		return 0;
	}	
	ent->SetAnimation(AnimationID,AnimStatus);
	gs->GetIO()<<LogLevel::SystemMessage<<"Animation State changed Entity : " << FormID<<" Status "<< (unsigned short)status<< "animation "<< AnimationID << " On /Off " << AnimStatus;  
	return GetMinChunkSize(PkgChunk::Animation);
}