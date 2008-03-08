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
#pragma once
#include <iostream>
#include <string>
#include "ioprovider.h"
#include "GlobalDefines.h"
class ScreenIOProvider :
	public IOProvider
{
public:
	ScreenIOProvider(IOSystem *parent,LogLevel LogTreshold);
	~ScreenIOProvider(void);
	static OO_TPROC_RET QueryProc(ScreenIOProvider *thisptr);
	virtual bool DoOutput(LogLevel LogLevel,std::string Message)
	{
		if(LogLevel < m_threshold)
			return false;
		std::cout << Message ;
		return true;
	}
	bool RegisterInput(std::string * Message)
	{
		m_system->RegisterInput(Message);
	}
		
protected:
	OO_THREAD QueryThread; // just repeats sscanf all the time ...
	LogLevel m_threshold;  //Minimum Log Level for output
};