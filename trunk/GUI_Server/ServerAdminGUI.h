/*

Copyright 2007   Julian Bangert aka masterfreek64 and Joseph Pearson aka chessmaster42 and Joel Teichroeb aka bobjr777

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

#ifndef GUI_Server_h
#define GUI_Server_h

#define UInt32 unsigned long
#define UInt8 unsigned char

#include "resource.h"
#include "stdafx.H"
#include <winsock.h>

#define ADMINPORT 41804

#define SUPER_VERSION 0 //Not used yet
#define MAIN_VERSION 4
#define SUB_VERSION 5 
//The 2 below here are nothing but UI . Not on the netcode
#define RELEASE_CODENAME "Release V" // The Name . Can be empty
#define ADDITIONAL_VERSION_COMMENT "Internal Alpha") // For betas and special builds only

//Defines for remote admin control
#define CONNECTMSG 0
#define DISCONNECTMSG 1
#define WELCOMEMSG 2

#define MSGCONTROL 0
#define AUTHCONTROL 1
#define CHATCONTROL 2
#define KICKCONTROL 3

#endif