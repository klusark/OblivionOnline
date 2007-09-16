#pragma once
/*

Copyright 2007  Julian aka masterfreek64 and Joseph Pearson aka chessmaster42 

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

#include "resource.h"

struct ServerInformation
{
	int ListIndex; // in the ServerBrowser ListBox
	char IPAddress[16]; //server IP
	char Name[256];
};
#pragma pack(push,1)
struct ServerAuthPacket
{
	int PacketID; //65556
	short Flags; //none
	char ClientName[64]; // 64 chars
	DWORD crc32_PluginsTxt; // CRC 32 of plugins.txt
	DWORD crc32_OblivionOnline; //CRC 32 of OblivionOnline.dll
	short ModCount;
};
struct ServerModAuthPacket
{
	int PacketID; //65555
	short Flags; //
	char EspName[128]; //Name to file
	char ModName[256]; //http link to d/
	short ModOrder; // 0 - reserved , 1- 255 Mods
	//after that it is: 255-2*255 - bsas for mod 1 etc ( mod 0 has no BSAs - it is a default namespace and not used) 
};

#pragma pack(pop)