
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
#include <iostream>
#include <cstdio>
#include <cstdarg>
#include <ctime>
//defines input , output and logging methods .
using namespace std;

IOSystem::IOSystem(std::string FileName,unsigned int FileLogLevel,unsigned int ConsoleLogLevel)
{
	mFileName = FileName; // we do store the filename ...
	mFileLogLevel = FileLogLevel;
	mConsoleLogLevel = ConsoleLogLevel;
	std::fstream File(FileName.c_str(),std::ios_base::out);
	if(File.is_open())
	{
		File<<"Log File opened \n" ;
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

bool IOSystem::DoOutput(unsigned int LogLevel,char *Message,... )
{
	va_list arglist;
	/* this actually just does the full debug message*/
	if(LogLevel >= mConsoleLogLevel)
	{
		va_start(arglist,Message);
		vprintf(Message,arglist);
		va_end(arglist);
	}
	if((LogLevel >= mFileLogLevel) && mbFile)
	{
		FILE *File = fopen(mFileName.c_str(),"a");
		if(File)
		{
			time_t timestamp = time(NULL);
			tm *UTCtime = gmtime(&timestamp);
			char * tempString  = asctime(UTCtime); // Saves time ... - should be memory safe
			fwrite(tempString,(strlen(tempString)-1),1,File);
			fputc((int)' ',File);
			va_start(arglist,Message);
			vfprintf(File,Message,arglist);
			va_end(arglist);
		}
		else
		{
			printf("Log file could not be opened anymore \n");
		}
		fclose(File);
	}
	return false;
}
/*
bool IOSystem::Warning(std::string text)
{
	return DoOutput(text,LOG_WARNING);
}
bool IOSystem::Error(std::string text)
{
	return DoOutput(text,LOG_ERROR);
}
bool IOSystem::FatalError(std::string text)
{
	return DoOutput(text,LOG_FATALERROR);
}
bool IOSystem::Message(std::string text)
{
	return DoOutput(text,LOG_MESSAGE);
}
bool IOSystem::DebugMessage(std::string text)
{
	return DoOutput(text,LOG_DEBUGMESSAGE);
}
bool IOSystem::DebugWarning(std::string text)
{
	return DoOutput(text,LOG_DEBUGWARNING);
}*/
void IOSystem::RMAThread(void *Parameter)
{
	RMAThreadParameters *params = (RMAThreadParameters *)Parameter;
	IOSystem *IOSys = params->IOSys;
	IOSys->DoOutput(LOG_MESSAGE,"Remote Administrator started");
	//TODO: Finish Implementatio
	return;
}
