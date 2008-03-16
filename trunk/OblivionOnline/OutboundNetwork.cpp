#include "OutboundNetwork.h"
#include "main.h"
bool OutboundNetwork::Send()
{
	if(m_Reliable)
	{
		send(ServerSocket,(const char *)m_Data,*m_Bytes_written,0);
	}
	else
	{
		sendto(m_UDP,(const char *)m_Data,*m_Bytes_written,0,(SOCKADDR *)&m_addr,sizeof(SOCKADDR_IN));
	}
	memset(m_Data,0,PACKET_SIZE);
	memset(m_ObjectID,0,sizeof(UINT32)*MAX_OBJECTS_PER_PACKET);
	m_Dataptr = m_Data + PACKET_HEADER_SIZE;
	m_Bytes_written =  (UINT16 *)((UINT8 *)m_Data + 1); 
	m_Chunks_written = (UINT8 *)m_Data;
	m_Reliable = false;
	return true; //TODO: Error Handling - Fragmentation Handling
}
OutboundNetwork outnet;