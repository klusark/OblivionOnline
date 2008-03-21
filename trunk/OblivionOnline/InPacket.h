#pragma once
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
#include "../OblivionOnlineServer/GlobalDefines.h"
#include "../OblivionOnlineServer/Packets.h"
#include "ChunkHandler.h"
#include "main.h"
class NetworkSystem;
class InPacket
{
private:
	BYTE *m_stream;
	BYTE *m_current;
	unsigned short m_currentchunk;
	unsigned short m_supposedsize;
	BYTE ChunkCount;
	size_t m_streamlen;
public:
	UINT32 ObjectIDs[16];
	BYTE Status[16];
	UINT8 GetNumberOfChunks()
	{
		return *(UINT8*)(m_stream);
	}
	InPacket(BYTE * stream,size_t streamlen)
	{
		m_stream = stream;
		m_current = m_stream;
		m_currentchunk = 0;
		m_streamlen = streamlen;
	} 
	inline size_t HandleChunk(BYTE* chunkdata,BYTE* EndPtr )
	{
		PkgChunk chunk = GetChunkType(chunkdata);
		unsigned long maxsize = (unsigned long)EndPtr - (unsigned long)chunkdata;
		if(GetMinChunkSize(chunk) < maxsize )
			return false;
		UINT32 FormID = this->ObjectIDs[GetObject(chunkdata)];
		BYTE status = this->Status[GetObject(chunkdata)];
		_MESSAGE("Found %u chunk",chunk);
		switch(chunk)
		{
		case	Object:
			return HandleObjectChunk(this,chunkdata,maxsize,FormID,status);
		case	Position:
			return HandlePositionChunk(this,chunkdata,maxsize,FormID,status);
		case CellID:
			return HandleCellIDChunk(this,chunkdata,maxsize,FormID,status);
		case	Health:
			return HandleHealthChunk(this,chunkdata,maxsize,FormID,status);
		case Magicka:
			return HandleMagickaChunk(this,chunkdata,maxsize,FormID,status);
		case Fatigue:
			return HandleFatigueChunk(this,chunkdata,maxsize,FormID,status);
		case Gender:
			return HandleGenderChunk(this,chunkdata,maxsize,FormID,status);
		case Race:
			return HandleRaceChunk(this,chunkdata,maxsize,FormID,status);
		case Class:
			return HandleClassChunk(this,chunkdata,maxsize,FormID,status);
		case Name:
			return HandleNameChunk(this,chunkdata,maxsize,FormID,status);
		case Stat:
			return HandleStatChunk(this,chunkdata,maxsize,FormID,status);
		case Skill:
			return HandleSkillChunk(this,chunkdata,maxsize,FormID,status);
		case Chat:
			return HandleChatChunk(this,chunkdata,maxsize,FormID,status);
		case Auth:
			//return HandleAuthChunk(this,chunkdata,maxsize,FormID,status);
			return false;
		case ClientType:
			return HandleClientTypeChunk(this,chunkdata,maxsize,FormID,status);
		case Version:
			return HandleVersionChunk(this,chunkdata,maxsize,FormID,status);
		case PlayerID:
			return HandlePlayerIDChunk(this,chunkdata,maxsize,FormID,status);
		default:
			return false;
		}
	} 
	void HandlePacket()
	{
		size_t retval;
		int i = 0;
		BYTE *m_end = m_stream + m_streamlen;
		ChunkCount = m_stream[0];//The Data
		m_current += 3;
		_MESSAGE("Handling Packet");
		for(;i < ChunkCount;i++)
		{
			retval = HandleChunk(m_current,m_end);
			if(retval == 0)
			{
				_WARNING("Bad chunk : Ignoring rest of packet ");
				return;
			}
			m_current += retval;
			if(m_current > m_end)
			{
				//m_GS->GetIO() << Warning<< "Bad Packet: too less data"<<endl;
			}
			if(m_current == m_end)
			{
				break;
			}
		}
		if(i != ChunkCount)
		{
			//m_GS->GetIO()<< Warning << "Bad Packet: Not all data encountered"<<endl;
		}

	}
	~InPacket(void)
	{
		free(m_stream);
	}
};
