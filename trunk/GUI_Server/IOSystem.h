
/*

Copyright 2007   Julian Bangert aka masterfreek64

This file is part of OblivionOnline.

    OblivionOnline is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    OblivionOnline is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _IO_SYSTEM_H
#define _IO_SYSTEM_H
#include <string>
#define LOG_DEBUGMESSAGE 0
#define LOG_DEBUGWARNING 1
#define LOG_WARNING 2
#define LOG_ERROR 3
#define LOG_MESSAGE 4
#define LOG_FATALERROR 5
class IOSystem
{
	bool mbFile;
	unsigned int mFileLogLevel, mConsoleLogLevel;
	std::string mFileName;
public:
	IOSystem(std::string FileName,unsigned int FileLogLevel,unsigned int ConsoleLogLevel);
	~IOSystem(void);
	bool DoOutput(unsigned int LogLevel,char *Message,... );
};

#endif