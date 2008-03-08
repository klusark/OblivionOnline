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
#include "ioprovider.h"
#include <fstream>
#include <ios>
#include <ctime>
class LogIOProvider :
	public IOProvider
{
public:
	LogIOProvider(IOSystem *parent,LogLevel threshold, std::string FileName): IOProvider(parent)
	{
		FILE *File = fopen(FileName.c_str(),"w");
		if(File)
		{
			time_t timestamp = time(NULL);
			fwrite(ctime(&timestamp),24,1,File);
			fputc((int)'\t',File);
			fprintf(File,"Opened Log File");
		}
		m_filename = FileName;
		m_threshold = threshold;
	}
	~LogIOProvider(void)
	{
				FILE *File = fopen(m_filename.c_str(),"a");
		if(File)
		{
			time_t timestamp = time(NULL);
			fwrite(ctime(&timestamp),24,1,File);
			fputc((int)'\t',File);
			fprintf(File,"Closing Log");
			fputc((int)'\n',File);
		}
	}
	bool DoOutput(LogLevel LogLevel,std::string Message)
	{
		if(LogLevel < m_threshold)
			return false;
		FILE *File = fopen(m_filename.c_str(),"a");
		if(File)
		{
			fwrite(Message.c_str(),Message.length(),1,File);
			fputc((int)'\n',File);
		}
		return true;
	}
private:
	std::string m_filename;
	LogLevel m_threshold;
};
