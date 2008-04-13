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
#include "EntityUpdateManager.h"
#include "Entity.h"
#include "NetworkSystem.h"
#include <map>
using namespace std;
void EntityUpdateManager::OnRaceUpdate( Entity *ent )
{
	UINT race = ent->Race();
	for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
	{
		if(i->second != ent)
		{
			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Race),PkgChunk::Race,(BYTE*)&race);
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

			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Gender),PkgChunk::Gender,&Gender);
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
			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Position),PkgChunk::Position,(BYTE *)&ChunkData);
			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Health),PkgChunk::Health,(BYTE *) &Health);
			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Fatigue),PkgChunk::Fatigue,(BYTE *) &Magicka);
			m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Magicka),PkgChunk::Magicka,(BYTE *) &Fatigue);

		}
	}
}

void EntityUpdateManager::OnMagickaUpdate( Entity *ent )
{
	short Magicka = ent->Magicka();
	if(ent->Status() < STATUS_PLAYER)
	{
		for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
		{
			if(i->first != m_net->GetMasterClient())
			{
				m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Magicka),PkgChunk::Magicka,(BYTE *)&Magicka);
			}
		}
	}
	else
	{
		for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
		{
			if(i->first != ent->RefID())
			{
				m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Magicka),PkgChunk::Magicka,(BYTE *)&Magicka);
			}
		}
	}
}

void EntityUpdateManager::OnFatigueUpdate( Entity *ent )
{
	short Fatigue = ent->Fatigue();
	if(ent->Status() < STATUS_PLAYER)
	{
		for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
		{
			if(i->first != m_net->GetMasterClient())
			{
				m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Fatigue),PkgChunk::Fatigue,(BYTE *)&Fatigue);
			}
		}
	}
	else
	{
		for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
		{
			if(i->first != ent->RefID())
			{
				m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Fatigue),PkgChunk::Fatigue,(BYTE *)&Fatigue);
			}
		}
	}
}

void EntityUpdateManager::OnHealthUpdate( Entity *ent )
{
	short Health = ent->Health();
	if(ent->Status() < STATUS_PLAYER)
	{
		for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
		{
			if(i->first != m_net->GetMasterClient())
			{
				m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Health),PkgChunk::Health,(BYTE *)&Health);
			}
		}
	}
	else
	{
		for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
		{
			if(i->first != ent->RefID())
			{
				m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Health),PkgChunk::Health,(BYTE *)&Health);
			}
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
	if(ent->Status() < STATUS_PLAYER)
	{
		for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
		{			
			if(m_net->GetMasterClient() != i->first && i->second->CellID() == ent->CellID())
			{
				m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Position),PkgChunk::Position,(BYTE*)&ChunkData);
			}
		}
	}
	else
	{
		for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
		{			
			if(ent->RefID() != i->first && i->second->CellID() == ent->CellID())
			{
				m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Position),PkgChunk::Position,(BYTE*)&ChunkData);
			}
		}
	}
}
void EntityUpdateManager::OnCellChange( Entity *ent )
{
	UINT32 ChunkData = ent->CellID();
	if(ent->Status() < STATUS_PLAYER)
	{
		for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
		{
			if( i->first != m_net->GetMasterClient() )
			{
				m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::CellID),PkgChunk::CellID,(BYTE*)&ChunkData);
			}
		}
	}
	else
	{
		for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
		{
			if( i->first != ent->RefID() )
			{
				m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::CellID),PkgChunk::CellID,(BYTE*)&ChunkData);
			}
		}
	}
}

void EntityUpdateManager::OnEquipUdate( Entity *ent,BYTE slot )
{
	BYTE ChunkData[5];
	ChunkData[0] = slot;
	*(UINT32 *)(ChunkData +1) = ent->Equip(slot);
	if(ent->Status() < STATUS_PLAYER)
	{
		for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
		{
			if( i->first != m_net->GetMasterClient())
			{
				m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Equip),PkgChunk::Equip,(BYTE*)&ChunkData);
			}
		}
	}
	else
	{
		for(map<UINT32,Entity *>::const_iterator i =  m_mgr->GetPlayerList().begin(); i != m_mgr->GetPlayerList().end() ; i++)
		{
			if(i->first != ent->RefID())
			{
				m_net->SendChunk(i->second->RefID(),ent->RefID(),ent->Status(),GetMinChunkSize(PkgChunk::Equip),PkgChunk::Equip,(BYTE*)&ChunkData);
			}
		}
	}
}