/*
OblivionOnline Server- An open source game server for the OblivionOnline mod
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
#include "GameServer.h"
#include "NetworkSystem.h"
#include "Entity.h"
size_t HandleHealthChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status)
{
	Entity *ent = gs->GetEntities()->GetEntity(status,FormID);
	if(ent == NULL)
	{
		gs->GetIO()<<Error<<"Error handling health chunk: Entity not registered"<< endl;
		return 0;
	}	
	ent->SetHealth(*(INT16 *)(chunkdata + 2));
	return GetMinChunkSize(PkgChunk::Health) + sizeof(unsigned short);
}
size_t HandleMagickaChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status)
{
	Entity *ent = gs->GetEntities()->GetEntity(status,FormID);
	if(ent == NULL)
	{
		gs->GetIO()<<Error<<"Error handling magicka chunk: Entity not registered"<< endl;
		return 0;
	}	
	ent->SetMagicka(*(INT16*)(chunkdata + 2));
	return GetMinChunkSize(PkgChunk::Magicka) + sizeof(unsigned short);
}
size_t HandleFatigueChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status)
{
	Entity *ent = gs->GetEntities()->GetEntity(status,FormID);
	if(ent == NULL)
	{
		gs->GetIO()<<Error<<"Error handling fatigue chunk: Entity not registered"<< endl;
		return 0;
	}	
	ent->SetFatigue(*(INT16 *)(chunkdata + 2));
	return GetMinChunkSize(PkgChunk::Fatigue) + sizeof(unsigned short);
}
size_t HandleStatChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status)
{
	Entity *ent = gs->GetEntities()->GetEntity(status,FormID);
	if(ent == NULL)
	{
		gs->GetIO()<<Error<<"Error handling stat chunk: Entity not registered"<< endl;
		return 0;
	}	
	ent->SetStat(*(chunkdata +2),*(INT16 *)(chunkdata+3));
	return GetMinChunkSize(PkgChunk::Stat) + sizeof(unsigned short);
}
size_t HandleSkillChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status)
{
	Entity *ent = gs->GetEntities()->GetEntity(status,FormID);
	if(ent == NULL)
	{
		gs->GetIO()<<Error<<"Error handling skill chunk: Entity not registered"<< endl;
		return 0;
	}	
	ent->SetSkill(*(chunkdata +2),*(INT16 *)(chunkdata+3));
	return GetMinChunkSize(PkgChunk::Skill) + sizeof(unsigned short);
}
size_t HandleGenderChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status)
{
	Entity *ent = gs->GetEntities()->GetEntity(status,FormID);
	if(ent == NULL)
	{
		gs->GetIO()<<Error<<"Error handling gender chunk: Entity not registered"<< endl;
		return 0;
	}	
	ent->SetFemale(*(chunkdata+2));
	return GetMinChunkSize(PkgChunk::Gender) + sizeof(unsigned short);
}
size_t HandleRaceChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status)
{
	Entity *ent = gs->GetEntities()->GetEntity(status,FormID);
	if(ent == NULL)
	{
		gs->GetIO()<<Error<<"Error handling race chunk: Entity not registered"<< endl;
		return 0;
	}	
	ent->SetRace(*(UINT32 *)(chunkdata + 2));
	return GetMinChunkSize(PkgChunk::Race) + sizeof(unsigned short);
}

size_t HandleClassChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len,UINT32 FormID,BYTE status)
{
	Entity *ent = gs->GetEntities()->GetEntity(status,FormID);
	if(ent == NULL)
	{
		gs->GetIO()<<Error<<"Error handling class chunk: Entity not registered"<< endl;
		return 0;
	}	
	ent->SetClassName(ReadANSIString(chunkdata + 2,len-2));
	return ent->ClassName().length() + 2 + sizeof(unsigned short);//TODO: think of more clever way to handle strings
}
size_t HandleNameChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status)
{
	Entity *ent = gs->GetEntities()->GetEntity(status,FormID);
	if(ent == NULL)
	{
		gs->GetIO()<<Error<<"Error handling name chunk: Entity not registered"<< endl;
		return 0;
	}	
	ent->SetName(ReadANSIString(chunkdata + 2,len-2));
	return ent->Name().length() + 2 + sizeof(unsigned short);
}
size_t HandleEquipChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status)
{
	Entity *ent = gs->GetEntities()->GetEntity(status,FormID);
	if(ent == NULL)
	{
		gs->GetIO()<<Error<<"Error handling Equip chunk: Entity not registered"<< endl;
		return 0;
	}	
	ent->SetEquip(*(BYTE*)(chunkdata + 2),*(UINT32 *)(chunkdata + 3));
	return GetMinChunkSize(PkgChunk::Equip)  + sizeof(unsigned short);
}