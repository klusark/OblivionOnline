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
#include <process.h>
#include "GameObjects.h"
#include "../OblivionOnlineServer/GlobalDefines.h"
#include "../OblivionOnlineServer/Packets.h"
extern UINT32 LocalPlayer;
class OutboundNetwork
{
private:
	BYTE m_Data[PACKET_SIZE];
	UINT32 m_ObjectID[MAX_OBJECTS_PER_PACKET];
	BYTE m_Status[MAX_OBJECTS_PER_PACKET];
	
	UINT32 m_Player;
	UINT16 *m_Bytes_written;
	UINT8 *m_Chunks_written;
	SOCKADDR_IN m_addr;
	
	bool m_Reliable;
	BYTE m_ObjectsWritten;
	SOCKET  m_UDP;
	CRITICAL_SECTION m_criticalsection;
	inline bool WriteByte(BYTE data)
	{
		if(*m_Bytes_written  < PACKET_SIZE) // m_Bytes_written + 1 <= tuned
		{
			*(m_Data + *m_Bytes_written) = data;
			(*m_Bytes_written)++;
			return true;
		}
		return false;
	}
	inline bool WriteWord(WORD data)
	{
		if(*m_Bytes_written + 1 < PACKET_SIZE)// m_Bytes_written + 2 <= tuned
		{
			*(WORD *)(m_Data + *m_Bytes_written) = data;
			*m_Bytes_written += sizeof(UINT16);
			return true;
		}
		return false;
	}
	inline bool WriteUINT32(UINT32 data)
	{
		if(*m_Bytes_written + sizeof(UINT32) <= PACKET_SIZE)
		{
			*(UINT32 *)(m_Data + *m_Bytes_written) = data;
			(*m_Bytes_written) += sizeof(UINT32);
			return true;
		}
		return false;
	}
	inline bool Write(size_t len,BYTE *data)
	{
		if(*m_Bytes_written + len <= PACKET_SIZE)
		{
			memcpy(m_Data + *m_Bytes_written,data,len);
			(*m_Bytes_written) += len;
			return true;
		}
		
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
		for(i = 0;i < m_ObjectsWritten; i++)
		{
			if((m_Status[i] == STATUS_PLAYER) == (Status == STATUS_PLAYER) && m_ObjectID[i] == FormID)
				return i;
		}
		//write it
		if( RemainingDataSize() < (sizeof(UINT32) + 3))
			return MAX_OBJECTS_PER_PACKET;
			//Search for a new slot
		if(m_ObjectsWritten == MAX_OBJECTS_PER_PACKET )
			return MAX_OBJECTS_PER_PACKET; // We found no empty slot
		i = m_ObjectsWritten;
		m_ObjectID[i] = FormID;
		m_Status[i] =  Status;		
		WriteWord((   ((UINT16)PkgChunk::Object)   & CHUNKMASK)|(i & OBJECTMASK) );
		(*m_Chunks_written)++;
		WriteUINT32(FormID);
		WriteByte(Status);
		m_ObjectsWritten++;
		
		return i;
	}
public:	
	OutboundNetwork()
	{
		WSADATA data;
		WSAStartup(MAKEWORD(2,2),&data);
		memset(m_ObjectID,0xffffffff,sizeof(UINT32)*MAX_OBJECTS_PER_PACKET);
		memset(m_Status,255,sizeof(BYTE)*MAX_OBJECTS_PER_PACKET);
		m_Bytes_written =  (UINT16 *)(m_Data + 1); 
		m_Chunks_written = (UINT8 *)m_Data;
		m_ObjectsWritten = 0;
		m_Reliable = false;
		m_UDP = socket(AF_INET,SOCK_DGRAM,0);
		*m_Chunks_written = 0;
		*m_Bytes_written = PACKET_HEADER_SIZE;
		InitializeCriticalSection(&m_criticalsection);
	}
	~OutboundNetwork(void)
	{
		DeleteCriticalSection(&m_criticalsection);
	}
	void SetAddress(SOCKADDR_IN addr)
	{
		m_addr =  addr;
	}
	void Reset()
	{
		memset(m_ObjectID,0xffffffff,sizeof(UINT32)*MAX_OBJECTS_PER_PACKET);
		memset(m_Status,255,sizeof(BYTE)*MAX_OBJECTS_PER_PACKET);
		memset(m_Data,0,PACKET_SIZE);
		m_Bytes_written =  (UINT16 *)(m_Data + 1); 
		m_Chunks_written = (UINT8 *)m_Data;
		m_ObjectsWritten = 0;
		*m_Chunks_written = 0;
		*m_Bytes_written = PACKET_HEADER_SIZE;
		m_Reliable = false;
	}
	inline bool AddChunk(UINT32 FormID,BYTE Status,size_t ChunkSize,PkgChunk ChunkType,BYTE *data)
	{
		if(Status == STATUS_PLAYER && FormID == (*g_thePlayer)->refID)
			FormID = LocalPlayer;
		if(*m_Chunks_written >= UCHAR_MAX)
			Send();
		if(RemainingDataSize() < (ChunkSize + 2))  // Chunk Header
		{
			Send();
		}
		BYTE ObjectID = GetObjectID(FormID,Status);
		if( ObjectID == MAX_OBJECTS_PER_PACKET)
		{
			Send();
			ObjectID = GetObjectID(FormID,Status);
		}
		EnterCriticalSection(&m_criticalsection);
		WriteWord((ChunkType & CHUNKMASK)|(ObjectID & OBJECTMASK));
		Write(ChunkSize,data);
		(*m_Chunks_written)++;
		if(RequiresReliable(ChunkType))
			m_Reliable = true;
		LeaveCriticalSection(&m_criticalsection);
		if(*m_Bytes_written == PACKET_SIZE)
			Send();
		return true;
	}
	bool Send();
};
