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
#include "EntityUpdateManager.h"
#include "Entity.h"
#include <map>
using namespace std;
void EntityUpdateManager::OnRaceUpdate( Entity *ent )
{
	UINT race = ent->Race();
	for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
	{
		if(i->second != ent)
		{

			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Player(),GetMinChunkSize(PkgChunk::Race),PkgChunk::Race,(BYTE*)&race);
		}
	}
}

void EntityUpdateManager::OnGenderUpdate( Entity *ent )
{
	BYTE Gender = ent->Gender();
	for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
	{
		if(i->second != ent)
	{

			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Player(),GetMinChunkSize(PkgChunk::Gender),PkgChunk::Gender,&Gender);
		}
	}
}

void EntityUpdateManager::OnClassUpdate( Entity *ent )
{
	for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
	{
		if(i->second != ent)
		{
			//Send out data
		}
	}
}

void EntityUpdateManager::OnNameUpdate( Entity *ent )
{
	for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
	{
		if(i->second != ent)
		{
			//Send out data
		}
	}
}

void EntityUpdateManager::GlobalSend( Entity *ent )
{
	float ChunkData[6] =
	{
		ent->PosX(),
		ent->PosY(),
		ent->PosZ(),
		ent->RotX(),
		ent->RotY(),
		ent->RotZ()
	};
	signed short Health = ent->Fatigue()
		,Magicka = ent->Magicka()
		,Fatigue = ent->Fatigue();
	for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
	{
		if(i->second != ent)
		{
			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Player(),GetMinChunkSize(PkgChunk::Position),PkgChunk::Position,(BYTE *)&ChunkData);
			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Player(),GetMinChunkSize(PkgChunk::Health),PkgChunk::Health,(BYTE *) &Health);
			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Player(),GetMinChunkSize(PkgChunk::Fatigue),PkgChunk::Fatigue,(BYTE *) &Magicka);
			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Player(),GetMinChunkSize(PkgChunk::Magicka),PkgChunk::Magicka,(BYTE *) &Fatigue);

		}
	}
}

void EntityUpdateManager::OnMagickaUpdate( Entity *ent )
{
	short Magicka = ent->Magicka();
	for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
	{
		if(i->second != ent)
		{
			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Player(),GetMinChunkSize(PkgChunk::Magicka),PkgChunk::Magicka,(BYTE *)&Magicka);
		}
	}
}

void EntityUpdateManager::OnFatigueUpdate( Entity *ent )
{
	short Fatigue = ent->Fatigue();
	for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
	{
		if(i->second != ent)
		{
			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Player(),GetMinChunkSize(PkgChunk::Fatigue),PkgChunk::Fatigue,(BYTE *)Fatigue);
		}
	}
}

void EntityUpdateManager::OnHealthUpdate( Entity *ent )
{
	short Health = ent->Health();
	for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
	{
		if(i->second != ent)
		{
			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Player(),GetMinChunkSize(PkgChunk::Health),PkgChunk::Health,(BYTE *)Health);
		}
	}
}

void EntityUpdateManager::OnPositionUpdate( Entity *ent ) /*Triggers Events and network code */
{
	float ChunkData[6] =
	{
		ent->PosX(),
		ent->PosY(),
		ent->PosZ(),
		ent->RotX(),
		ent->RotY(),
		ent->RotZ()
	};
	for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
	{
		if(i->second->CellID() == ent->CellID() && ( i->second != ent))
		{
			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Player(),GetMinChunkSize(PkgChunk::Position),PkgChunk::Position,(BYTE*)&ChunkData);
		}
	}
}