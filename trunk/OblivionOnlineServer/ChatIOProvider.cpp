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
#include "ChatIOProvider.h"
#include "GameServer.h"
#include "NetworkSystem.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Packets.h"
ChatIOProvider::~ChatIOProvider(void)
{
}

bool ChatIOProvider::DoOutput( LogLevel LogLevel,std::string Message )
{
	BYTE * data = new BYTE[Message.length()+ 2];
	*(UINT16 *)data = (UINT16)Message.length(); 
	memcpy(data + 2,Message.c_str(),Message.length());
	for(map<UINT32,Entity *>::const_iterator i =  m_gs->GetEntities()->GetPlayerList().begin(); i != m_gs->GetEntities()->GetPlayerList().end() ; i++)
	{
		//m_gs->GetNetwork()->SendChunk(i->second->RefID(),i->second->RefID(),true,Message.length() + 2,PkgChunk::Chat,data);
		//m_gs->GetNetwork()->Send(i->second->RefID());
	}
	return true;
}