/*

Copyright 2007  Julian aka masterfreek64 and Joseph Pearson aka chessmaster42 and Joel Teichroeb aka bobjr777

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
#include <winsock.h>

#define ADMINPORT 41804

#define SUPER_VERSION 0	//Not used yet
#define MAIN_VERSION 4
#define SUB_VERSION 3	//Release 4, bugfix 3

//Defines for remote admin control
#define CONNECTMSG 0
#define DISCONNECTMSG 1
#define WELCOMEMSG 2

#define MSGCONTROL 0
#define AUTHCONTROL 1
#define CHATCONTROL 2
#define KICKCONTROL 3

#endif