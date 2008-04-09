/*
Copyright 2008   Julian Bangert aka masterfreek64
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
#include "InPacket.h"
#include "ChunkHandler.h"
size_t HandleGenderChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE Status)
{ 
	//TODO: Handle this
	if(Status == STATUS_PLAYER)
	{
		if(*(chunkdata +2) != 0)
			Console_Print("Player %u should be female",FormID);
		else
			Console_Print("Player %u should be male",FormID);
	}
	else
	{
		if(*(chunkdata +2) != 0)
			Console_Print("Actor %u should be female",FormID);
		else
			Console_Print("Actor %u should be male",FormID);
	}
	return GetMinChunkSize(Gender) + sizeof(unsigned short);

}