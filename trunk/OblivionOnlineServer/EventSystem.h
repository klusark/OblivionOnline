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
#pragma once
#include <map>
#include "GlobalDefines.h"
class Event;
class GameServer;
class EventSystem
{
public:
	EventSystem(GameServer *server);
	~EventSystem(void);
	Event *GetEventFromID(UINT32);
	bool RegisterEvent(Event *evt);
	UINT32 GetEventID()
	{
		return m_CurrentEventID++;
	}
protected:
	GameServer *m_server;	
private:
	UINT32 m_CurrentEventID;
};
