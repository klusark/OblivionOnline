#pragma once
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
#include "IOSystem.h"
#include "GlobalDefines.h"
#include "EntityManager.h"
class LuaSystem;
class NetworkSystem;

class GameServer
{
public:
	GameServer(void);
	~GameServer(void);
	void RunServer();
	IOStream & GetIO()
	{
		return *m_IO;
	}
	EventSystem *GetEventSys()
	{
		return m_Evt;
	}
	LuaSystem *GetLua()
	{
		return m_script;
	}
	EntityManager *GetEntities()
	{
		return m_Entities;
	}
	NetworkSystem *GetNetwork()
	{
		return m_Netsystem;
	}
	void DisplayBootupMessage();
	void AdvertiseGameServer();
protected:
	LuaSystem *m_script;
	IOStream *m_IO;
	IOSystem *m_IOSys;
	EventSystem *m_Evt;
	EntityManager *m_Entities;
	NetworkSystem *m_Netsystem;
};
