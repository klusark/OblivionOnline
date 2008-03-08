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
#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <streambuf>
#include <ctime>
#include <ostream>
#include <iostream>

#include "EventSystem.h"

class IOProvider;
enum LogLevel
{
	BootMessage =1 ,
	SystemMessage,
	GameMessage,
	Warning,
	Error,
	FatalError
};
class IOSystem : public std::streambuf
{
public:
	LogLevel DefaultLogLevel;
	IOSystem(EventSystem* evt)
	{
		m_evt = evt;
		DefaultLogLevel = LogLevel::BootMessage;
		m_buf = new char[1024];
		m_buflen = 1024;
		setp(m_buf, m_buf + 1024);
	}
	~IOSystem(void);
	bool DoOutput(LogLevel Level,std::string Message);
	bool DoOutput(LogLevel Level,char * Message)
	{
		return DoOutput(Level,std::string(Message));
	} 
	bool DoOutput(LogLevel Level,char *Message,size_t MessageLen)
	{
		return DoOutput(Level,std::string(Message,MessageLen));
	}
	/*
	bool BootMessage(std::string Message)
	{
		return DoOutput(LogLevel::BootMessage,Message);
	};
	bool SystemMessage(std::string Message)
	{
		return DoOutput(LogLevel::SystemMessage,Message);
	};
	bool GameMessage(std::string Message)
	{
		return DoOutput(LogLevel::GameMessage,Message);
	};
	bool Warning(std::string Message)
	{
		return DoOutput(LogLevel::Warning,Message);
	};
	bool Error(std::string Message)
	{
		return DoOutput(LogLevel::Error,Message);
	};
	bool FatalError(std::string Message)
	{
		return DoOutput(LogLevel::FatalError,Message);
	};
	*/
	bool RegisterIOProvider(IOProvider *provider);
	bool RemoveIOProvider(IOProvider *system);
	bool RegisterInput(std::string  *Message);//TODO : tune performance here , the input still locks 
protected:
	std::list <IOProvider *> m_providers;
	EventSystem *m_evt;
private:
	char *m_buf;
	size_t m_buflen;
	virtual int sync (void) 
	{
		time_t timestamp = time(NULL);
		std::string Message(ctime(&timestamp),24);
		switch(DefaultLogLevel)
		{
		case BootMessage:
			Message +="[BootMessage]";
			break;
		case Error:
			Message +="[Error]";
			break;
		case SystemMessage:
			Message +="[SystemMessage]";
			break;
		case GameMessage:
			Message +="[GameMessage]";
			break;
		case Warning:
			Message +="[Warning]";
			break;
		case FatalError:
			Message +="[FatalError]";
			break;
		default:
			Message +="[unknown]";
			break;
		}
		Message.append(pbase(),pptr() - pbase());
		int ret = (int) DoOutput(DefaultLogLevel,Message);
		DefaultLogLevel = LogLevel::SystemMessage;
		// reset the buffer
		m_buf[0] = '\0';
		setp(pbase(), epptr());
		return 0;
	}
	int overflow (int c) {
		// allocate a new buffer and copy
		// our current data into it, then swap
		// it with the old buffer
		m_buflen += 1024;
		char * newbuf = new char[m_buflen];
		memcpy(newbuf, m_buf, m_buflen-1024);
		delete[] m_buf;
		m_buf = newbuf;
		setp(m_buf, m_buf + m_buflen);
		// now we need to stuff c into the buffer
		sputc(c);
		return 0;
	}
};
class IOStream : public std::ostream {
public:
	// we initialize the ostream to use our logbuf
	IOStream(IOSystem *sys)
		:std::ostream(sys)
	{
		m_system = sys;
	}

	inline bool RegisterIOProvider(IOProvider *provider)
	{
		return m_system->RegisterIOProvider(provider);
	}
	inline bool RemoveIOProvider(IOProvider *provider)
	{
		return m_system->RemoveIOProvider(provider);
	}
	inline bool RegisterInput(std::string  *Message)
	{
		return m_system->RegisterInput(Message);
	}
	  // set priority
	  void SetLogLevel (LogLevel lvl) {
		  m_system->DefaultLogLevel = lvl; 
	  }

private:
	IOSystem *m_system;
};
inline IOStream& operator<< (IOStream& ls,
					   LogLevel lvl) 
{
						   ls.SetLogLevel(lvl);
						   return ls;
}