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
#pragma once
#include <string>
#include "../OblivionOnlineServer/GlobalDefines.h"
#include "../OblivionOnlineServer/Packets.h"

class OutboundNetwork
{
private:
	BYTE m_Data[PACKET_SIZE];
	UINT32 m_ObjectID[MAX_OBJECTS_PER_PACKET];
	BYTE m_Status[MAX_OBJECTS_PER_PACKET];
	
	UINT32 m_Player;
	BYTE *m_Dataptr; 
	UINT16 *m_Bytes_written;
	UINT8 *m_Chunks_written;
	SOCKADDR_IN m_addr;
	
	bool m_Reliable;

	SOCKET  m_UDP;
	CRITICAL_SECTION m_criticalsection;
	inline bool WriteByte(BYTE data)
	{
		if(*m_Bytes_written  < PACKET_SIZE) // m_Bytes_written + 1 <= tuned
		{
			*m_Dataptr = data;
			*m_Bytes_written++;
			return true;
		}
		return false;
	}
	inline bool WriteWord(WORD data)
	{
		if(*m_Bytes_written + 1 < PACKET_SIZE)// m_Bytes_written + 2 <= tuned
		{
			*((WORD *)m_Dataptr) = data;
			*m_Bytes_written++; // faster here
			*m_Bytes_written++;
			return true;
		}
		return false;
	}
	inline bool WriteUINT32(UINT32 data)
	{
		if(*m_Bytes_written + sizeof(UINT32) <= PACKET_SIZE)
		{
			*((UINT32 *)m_Dataptr) = data;
			*m_Bytes_written += sizeof(UINT32);
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
		*m_Bytes_written += len;
		return false;
	}
	inline BYTE FindObjectID(UINT32 FormID,BYTE Status)
	{	
		for(BYTE i = 0;i < MAX_OBJECTS_PER_PACKET; i++)
		{
			if(m_Status[i] == Status && m_ObjectID[i] == FormID)
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
			if(m_Status[i] == Status && m_ObjectID[i] == FormID)
				return i;
		}
		//write it
		if( RemainingDataSize() < (sizeof(UINT32) + 3))
			return MAX_OBJECTS_PER_PACKET;		//packet full
		if(i == MAX_OBJECTS_PER_PACKET )
			return MAX_OBJECTS_PER_PACKET; // We found no empty slot
		m_ObjectID[i] = FormID;
		m_Status[i] = Status;		
		WriteWord((PkgChunk::Object & CHUNKMASK)|(i & OBJECTMASK));
		WriteUINT32(FormID);
		WriteByte(Status);
		return i;
	}
public:
	OutboundNetwork()
	{
		WSADATA data;
		WSAStartup(MAKEWORD(2,2),&data);
		memset(m_ObjectID,0,sizeof(UINT32)*MAX_OBJECTS_PER_PACKET);
		memset(m_Status,false,sizeof(BYTE)*MAX_OBJECTS_PER_PACKET);
		m_Dataptr = m_Data + PACKET_HEADER_SIZE;
		m_Bytes_written =  (UINT16 *)((UINT8 *)m_Data + 1); 
		m_Chunks_written = (UINT8 *)m_Data;
		m_Reliable = false;
		m_UDP = socket(AF_INET,SOCK_DGRAM,0);
		InitializeCriticalSection(&m_criticalsection);
	}
	~OutboundNetwork(void)
	{
	}
	void SetAddress(SOCKADDR_IN addr)
	{
		m_addr =  addr;
	}
	void Reset()
	{
		memset(m_ObjectID,0,sizeof(UINT32)*MAX_OBJECTS_PER_PACKET);
		memset(m_Status,false,sizeof(BYTE)*MAX_OBJECTS_PER_PACKET);
		m_Dataptr = m_Data + PACKET_HEADER_SIZE;
		m_Bytes_written =  (UINT16 *)((UINT8 *)m_Data + 1); 
		m_Chunks_written = (UINT8 *)m_Data;
		m_Reliable = false;
		m_UDP = socket(AF_INET,SOCK_DGRAM,0);
	}
	inline bool AddChunk(UINT32 FormID,bool IsPlayer,size_t ChunkSize,PkgChunk ChunkType,BYTE *data)
	{
		if(RemainingDataSize() < ChunkSize + 2)  // Chunk Header
			return false; // No more space
		BYTE ObjectID = GetObjectID(FormID,IsPlayer);
		if( ObjectID == MAX_OBJECTS_PER_PACKET)
			return false; // TOO many objects or too less space
		EnterCriticalSection(&m_criticalsection);
		WriteWord((ChunkType & CHUNKMASK)|(ObjectID & OBJECTMASK));
		Write(ChunkSize,data);
		if(RequiresReliable(ChunkType))
			m_Reliable = true;
		LeaveCriticalSection(&m_criticalsection);
		if(*m_Bytes_written >= PACKET_SIZE)
			Send();
		return true;
	}
	bool Send();
};