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
