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
#include "GlobalDefines.h"
#include "GameServer.h"
#include "ChunkHandler.h"
class NetworkSystem;
class InPacket
{
private:
	NetworkSystem *m_sys;
	size_t m_streamlen;
	BYTE *m_stream;
	BYTE *m_current;
	GameServer *m_GS; 
	unsigned short m_currentchunk;
	unsigned short m_supposedsize;
	BYTE ChunkCount;
public:
	UINT32 ObjectIDs[16];
	BYTE Status[16];
	UINT8 GetNumberOfChunks()
	{
		return *(UINT8*)(m_stream);
	}
	InPacket(NetworkSystem *sys,GameServer *gs,BYTE * stream,size_t streamlen)
	{
		m_sys = sys;
		m_stream = (BYTE*)malloc(streamlen);
		memcpy(m_stream,stream,streamlen); // So the stream can be released
		m_stream = m_stream;
		m_current = m_stream;
		m_currentchunk = 0;
		m_supposedsize = *(UINT16*)(m_stream+1);
		m_streamlen = streamlen;
		m_GS = gs;
		memset(ObjectIDs,0xffff,16*sizeof(UINT32));
		memset(Status,255,16);
	} 
	inline size_t HandleChunk(BYTE* chunkdata,BYTE* EndPtr )
	{
		PkgChunk chunk = GetChunkType(chunkdata);
		
		unsigned long maxsize = (unsigned long)EndPtr - (unsigned long)chunkdata;
		/*if(GetMinChunkSize(chunk) < maxsize )
			return false;*/
		UINT32 FormID = this->ObjectIDs[GetObject(chunkdata)];
		BYTE status = this->Status[GetObject(chunkdata)];
		switch(chunk)
		{
		case	Object:
			return HandleObjectChunk(m_GS,this,chunkdata,maxsize,FormID,status);
		case	Position:
			return HandlePositionChunk(m_GS,this,chunkdata,maxsize,FormID,status);
		case CellID:
			return HandleCellIDChunk(m_GS,this,chunkdata,maxsize,FormID,status);
		
		case Gender:
			return HandleGenderChunk(m_GS,this,chunkdata,maxsize,FormID,status);
		case Race:
			return HandleRaceChunk(m_GS,this,chunkdata,maxsize,FormID,status);
		case Class:
			return HandleClassChunk(m_GS,this,chunkdata,maxsize,FormID,status);
		case Name:
			return HandleNameChunk(m_GS,this,chunkdata,maxsize,FormID,status);
		case ActorValue:
			return HandleActorValueChunk(m_GS,this,chunkdata,maxsize,FormID,status);
		case Equip:
			return HandleEquipChunk(m_GS,this,chunkdata,maxsize,FormID,status);
		case Chat:
			return HandleChatChunk(m_GS,this,chunkdata,maxsize,FormID,status);
		case Auth:
			//return HandleAuthChunk(m_GS,this,chunkdata,maxsize,FormID,status);
			return GetMinChunkSize(Auth);
		case ClientType:
			return GetMinChunkSize(ClientType);
		case Version:
			return HandleVersionChunk(m_GS,this,chunkdata,maxsize,FormID,status);
		default:
			return 0;
		}
	} 
	void HandlePacket()
	{
		size_t retval;
		int i = 0;
		BYTE *m_end = m_stream + min(m_streamlen,m_supposedsize);
		ChunkCount = m_stream[0];//The Data
		m_current += 3;
		for(;i < ChunkCount;i++)
		{
			if(m_current >= m_end)
			{
				
				return;
			}
			retval = HandleChunk(m_current,m_end);
			
			if(retval == 0)
			{
				//m_GS->GetIO()<<Warning<<"Ignoring rest of packet "<< endl;
				return;
			}
			m_current += retval;
		}
		if(i != ChunkCount)
		{
			m_GS->GetIO()<< Warning << "Bad Packet: Not all data encountered"<<endl;
		}

	}
	~InPacket(void)
	{
		free(m_stream);
	}
};
