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
#include "IOProvider.h"
class ChatIOProvider  : public IOProvider
{
protected:
	GameServer *m_gs;
public:
	ChatIOProvider(GameServer *gs,IOSystem *parent): IOProvider(parent)
	{
		m_gs = gs;
	}
	~ChatIOProvider(void);
	virtual bool DoOutput(LogLevel LogLevel,std::string Message);
};
