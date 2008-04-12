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
#ifndef _GLOBAL_DEFINES_H
#define _GLOBAL_DEFINES_H
#include <string>
#define VERSION_SUPER 0
#define VERSION_MAJOR 5
#define VERSION_MINOR 0
#define VERSION_CODENAME "Release V"
#define VERSION_COMMENT "Server Rewrite"
#define VERSION_STREAM VERSION_SUPER<<"."<<VERSION_MAJOR<<"."<<VERSION_MINOR<<" \""<< VERSION_CODENAME << " \" "<< VERSION_COMMENT << " "  
#ifdef WIN32
#include <Windows.h>
typedef HANDLE OO_THREAD;
typedef void OO_TPROC_RET;
#else
#ifndef UINT32
#define UINT32 unsigned long
#endif

typedef int32_t OO_THREAD 
typedef void * OO_TPROC_RET
typedef HANDLE SEMAPHORE
#endif

#endif
using namespace std;
#define MAX_EQUIPSLOTS 18
#define MAX_PLAYERS 12