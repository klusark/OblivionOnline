/*

Copyright(c) 2007-2008   Julian Bangert aka masterfreek64
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

Linking OblivionOnline statically or dynamically with other modules is making a combined work based
on OblivionOnline. Thus, the terms and conditions of the GNU General Public License cover 
the whole combination.

In addition, as a special exception, the copyright holders of  OblivionOnline give you permission to 
combine OblivionOnline program with free software programs or libraries that are released under
the GNU LGPL and with code included in the standard release of Oblivion Script Extender by Ian Patterson (OBSE)
under the OBSE license (or modified versions of such code, with unchanged license). You may copy and distribute such a system 
following the terms of the GNU GPL for  OblivionOnline and the licenses of the other code concerned,
provided that you include the source code of that other code when and as the GNU GPL
requires distribution of source code.

Note that people who make modified versions of  OblivionOnline are not obligated to grant this special exception
for their modified versions; it is their choice whether to do so. 
The GNU General Public License gives permission to release a modified version without this
exception; this exception also makes it possible to release a modified version which carries 
forward this exception.
*/
#include "InPacket.h"
#include "ChunkHandler.h"
#include "UserInterface.h"
size_t HandleVersionChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE Status)
{
	if(*(chunkdata + 2) == VERSION_SUPER && *(chunkdata+3) == VERSION_MAJOR && *(chunkdata+4) == VERSION_MINOR )
		_MESSAGE("Server using the same version as the client");
	else
	{
		Console_Print("Incorrect Server Version %u.%u.%u",*(chunkdata + 2),*(chunkdata +3),*(chunkdata + 4));
		_ERROR("Incorrect Server Version %u.%u.%u",*(chunkdata + 2),*(chunkdata +3),*(chunkdata + 4));
	}
	return GetMinChunkSize(PkgChunk::Version);
}
size_t HandlePlayerIDChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE Status)
{
	LocalPlayer = *(UINT32 *)(chunkdata + 2);
	Console_Print("Received Player ID %u",LocalPlayer);
	_MESSAGE("Received Player ID %u",LocalPlayer);
	bIsInitialized = true;
	if(bUIInitialized)
		SetConnectionMessage("Good to go");
	return GetMinChunkSize(PlayerID);
}
size_t HandleClientTypeChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE Status)
{
	if((chunkdata + 2) > 0)
	{
		bIsMasterClient = true;
		_MESSAGE("Master Client");
		Console_Print("Master Client");
	}
	else
	{
		bIsMasterClient = false;
		_MESSAGE("Passive Client");
		Console_Print("Passive Client");
	}
	return GetMinChunkSize(ClientType);
}