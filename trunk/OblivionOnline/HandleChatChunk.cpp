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
size_t HandleChatChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE Status)
{ 
	Console_Print("Player %i: %s", FormID, (char *)(chunkdata +2 + sizeof(unsigned short)));
	QueueUIMessage((char *)(chunkdata +2 + sizeof(unsigned short)),0,1, 5);
	return (2 + sizeof(unsigned short) + (len < *(unsigned short *)(chunkdata +2) ) ? (len) : *(unsigned short *)(chunkdata +2));
}