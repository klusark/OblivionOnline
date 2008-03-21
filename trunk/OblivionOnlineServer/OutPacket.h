/*
OblivionOnline Server- An open source game server for the OblivionOnline mod
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
#include <string>
#include "GlobalDefines.h"
#include "Packets.h"

class OutPacket
{
private:
	BYTE m_Data[PACKET_SIZE];
	UINT32 m_ObjectID[MAX_OBJECTS_PER_PACKET];
	bool m_IsPlayer[MAX_OBJECTS_PER_PACKET];
	
	UINT32 m_Player;
	BYTE *m_Dataptr; 
	UINT16 *m_Bytes_written;
	UINT8 *m_Chunks_written;
	
	BYTE m_ObjectsWritten;
	bool m_Reliable;


	inline bool WriteByte(BYTE data)
	{
		if(*m_Bytes_written  < PACKET_SIZE) // m_Bytes_written + 1 <= tuned
		{
			*m_Dataptr = data;
			(*m_Bytes_written)++;
			return true;
		}
		return false;
	}
	inline bool WriteWord(WORD data)
	{
		if(*m_Bytes_written + 1 < PACKET_SIZE)// m_Bytes_written + 2 <= tuned
		{
			*((WORD *)m_Dataptr) = data;
			(*m_Bytes_written)++; // faster here
			(*m_Bytes_written)++;
			return true;
		}
		return false;
	}
	inline bool WriteUINT32(UINT32 data)
	{
		if(*m_Bytes_written + sizeof(UINT32) <= PACKET_SIZE)
		{
			*((UINT32 *)m_Dataptr) = data;
			(*m_Bytes_written) += sizeof(UINT32);
			return true;
		}
		return false;
	}
	inline bool Write(size_t len,BYTE *data)
	{
		if(*m_Bytes_written + len <= PACKET_SIZE)
		{
			memcpy(m_Dataptr,data,len);
			return true;
		}
		(*m_Bytes_written) += len;
		return false;
	}
	inline BYTE FindObjectID(UINT32 FormID,BYTE Status)
	{	
		for(BYTE i = 0;i < MAX_OBJECTS_PER_PACKET; i++)
		{
			if(m_IsPlayer[i] == (Status == STATUS_PLAYER) && m_ObjectID[i] == FormID)
				return i;
		}	
		return MAX_OBJECTS_PER_PACKET;
	}
	inline size_t RemainingDataSize()
	{
		return PACKET_SIZE - *m_Bytes_written;
	}
	inline BYTE GetObjectID(UINT32 FormID,BYTE Status)
	{		
		BYTE i = 0;		
		for(i = 0;i < MAX_OBJECTS_PER_PACKET; i++)
		{
			if(m_IsPlayer[i] == (Status == STATUS_PLAYER) && m_ObjectID[i] == FormID)
				return i;
		}
		//write it
		if( RemainingDataSize() < (sizeof(UINT32) + 3))
			return MAX_OBJECTS_PER_PACKET;		//packet full
		//Search for a new slot
		if(m_ObjectsWritten == MAX_OBJECTS_PER_PACKET )
			return MAX_OBJECTS_PER_PACKET; // We found no empty slot
		i = m_ObjectsWritten;
		m_ObjectID[i] = FormID;
		m_IsPlayer[i] = (STATUS_PLAYER == Status);		
		WriteWord((PkgChunk::Object & CHUNKMASK)|(i & OBJECTMASK));
		WriteUINT32(FormID);
		WriteByte(Status);
		m_ObjectsWritten++;
		return i;
	}
public:
	OutPacket(UINT32 Player)
	{
		memset(m_ObjectID,0,sizeof(UINT32)*MAX_OBJECTS_PER_PACKET);
		memset(m_IsPlayer,false,sizeof(bool)*MAX_OBJECTS_PER_PACKET);
		m_Dataptr = m_Data + PACKET_HEADER_SIZE;
		m_Bytes_written =  (UINT16 *)((UINT8 *)m_Data + 1); 
		m_Chunks_written = (UINT8 *)m_Data;
		*m_Bytes_written = PACKET_HEADER_SIZE;
		*m_Chunks_written = 0;
		m_Reliable = false;
		m_Player = Player;
		m_ObjectsWritten = 0;
	}
	~OutPacket(void)
	{
	}
	inline bool AddChunk(UINT32 FormID,BYTE Status,size_t ChunkSize,PkgChunk ChunkType,BYTE *data)
	{
		if(RemainingDataSize() < ChunkSize + 2)  // Chunk Header
			return false; // No more space
		BYTE ObjectID = GetObjectID(FormID,Status);
		if( ObjectID == MAX_OBJECTS_PER_PACKET)
			return false; // TOO many objects or too less space
		WriteWord((ChunkType & CHUNKMASK)|(ObjectID & OBJECTMASK));
		Write(ChunkSize,data);
		(*m_Chunks_written)++;
		if(RequiresReliable(ChunkType))
			m_Reliable = true;
		return true;
	}
	inline BYTE* GetData()
	{
		return m_Data;
	}
	inline bool Reliable()
	{
		return m_Reliable;
	}
	inline size_t Size()
	{
		return *m_Bytes_written;
	}
};
