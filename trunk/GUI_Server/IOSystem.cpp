
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
#include "IOSystem.h"
#include <fstream>
//defines input , output and logging methods .


IOSystem::IOSystem(std::string FileName,unsigned int FileLogLevel,unsigned int ConsoleLogLevel)
{
	mFileName = FileName; // we do store the filename ...
	mFileLogLevel = FileLogLevel;
	mConsoleLogLevel = ConsoleLogLevel;
	std::fstream File(FileName.c_str(),std::ios_base::out);
	if(File.is_open())
	{
		File<<" Log File opened \n" ;
		printf("Log File opened \n");
		mbFile = true;
	}
	else
	{
		printf("Could not open log file \n");
		mbFile = false;
	}
	File.close();
}

IOSystem::~IOSystem(void)
{
}

bool IOSystem::DoOutput(std::string Message, unsigned int LogLevel)
{
	/* this actually just does the full debug message*/
	if(LogLevel >= mConsoleLogLevel)
	{
		cout << Message << endl;
	}
	if(LogLevel >= mFileLogLevel && mbFile)
	{
		std::fstream File(mFileName,std::ios_base::app);
		if(File.is_open())
		{
			File<<Message;
			mbFile = true;
		}
		else
		{
			cout << "Log file could not be opened anymore" << endl;
		}
	}
	else
	{
		printf("Could not open log file \n");
		mbFile = false;
	}
	}
	return false;
}
