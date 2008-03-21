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
#include "OutboundNetwork.h"
#include "main.h"

bool OutboundNetwork::Send()
{
	EnterCriticalSection(&m_criticalsection);
	if(m_Reliable)
	{
		send(ServerSocket,(const char *)m_Data,*m_Bytes_written,0);
	}
	else
	{
		sendto(m_UDP,(const char *)m_Data,*m_Bytes_written,0,(SOCKADDR *)&m_addr,sizeof(SOCKADDR_IN));
	}
	_MESSAGE("Sent out data : %u Bytes , reliable %1u",*m_Bytes_written,m_Reliable);
	memset(m_Data,0,PACKET_SIZE);
	memset(m_ObjectID,0,sizeof(UINT32)*MAX_OBJECTS_PER_PACKET);
	m_Dataptr = m_Data + PACKET_HEADER_SIZE;
	m_Bytes_written =  (UINT16 *)(m_Data + 1); 
	m_Chunks_written = (UINT8 *)m_Data;
	m_Reliable = false;
	LeaveCriticalSection(&m_criticalsection);
	
	return true; //TODO: Error Handling - Fragmentation Handling
}

void OutboundNetwork::SendThread( void * ARG )
{
	while(1)
	{
		WaitForSingleObject(hEvtSendPacket,INFINITE); // We wait for the send code
		if(bIsConnected)
			outnet.Send();
	}
	return ;
}
OutboundNetwork outnet;