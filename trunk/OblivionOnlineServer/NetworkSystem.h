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

#include "OutPacket.h"
#include "GlobalDefines.h"
#include <vector>
#include <map>

#include "ChunkHandler.h"
class NetworkSystem
{
private:
	GameServer *m_GS;
	SOCKET m_UDPSock;
	DWORD m_resendinterval;
	DWORD m_lastsent;
	OO_THREAD m_TCPThread;
	OO_THREAD m_UDPThread;
	UINT32 m_MasterClient;
	//TODO : Defer packet processing to some other threads -use boost thread queue
	std::map<UINT32,OutPacket *> m_OutPackets; // All packets the server is currently writing to
	std::map<UINT32,SOCKET> m_TCPSockets;
	std::map<UINT32,SOCKADDR_IN> m_PlayerAddresses;
	std::map<u_long,UINT32> m_AddressPlayer; // This is the type of INET_ADDR.sin_addr.s_addr
	BYTE m_MasterClientDefined;
	static bool ValidatePacketLength(BYTE *data,size_t reportedSize)
	{
		return (*(UINT16 *)((BYTE *)data+1)) < reportedSize;
	}
	
public:
	NetworkSystem(GameServer *Server);
	~NetworkSystem(void);
	GameServer * GetGS()
	{
		return m_GS;
	}
	UINT32 AddNewPlayer(SOCKADDR_IN addr,SOCKET TCPSock);
	bool PlayerDisconnect(UINT32 ID);
	inline bool SendChunk(UINT32 PlayerID,UINT32 FormID,BYTE status,size_t ChunkSize,PkgChunk ChunkType,BYTE *data)
	{
		OutPacket *packet = m_OutPackets[PlayerID];
		if(packet->AddChunk(FormID,status,ChunkSize,ChunkType,data) == true)
			return true;
		else
		{
			if(packet->Reliable())
				SendReliableStream(PlayerID,packet->Size(),packet->GetData());
			else
				SendUnreliableStream(PlayerID,packet->Size(),packet->GetData());
		}
	}
	inline bool Send(UINT32 PlayerID)
	{
		OutPacket *packet = m_OutPackets[PlayerID];
		if(packet->Reliable())
			return SendReliableStream(PlayerID,packet->Size(),packet->GetData());
		else
			return SendUnreliableStream(PlayerID,packet->Size(),packet->GetData());
	}
	bool RegisterTraffic(UINT32 PlayerID,size_t size,BYTE *data,bool reliable);
	//TODO : Place these in  a callback ?
	bool SendUnreliableStream(UINT32 PlayerID,size_t length,BYTE *len);
	bool SendReliableStream(UINT32 PlayerID,size_t length,BYTE *len);
	bool StartReceiveThreads();
	UINT32 GetPlayerFromAddr(SOCKADDR_IN addr)
	{
		std::map<u_long,UINT32>::iterator iter = m_AddressPlayer.find(addr.sin_addr.S_un.S_addr);
		if(iter != m_AddressPlayer.end())
			return iter->second;
		return 0;
	}
	UINT32 GetPlayerCount()
	{
		return  m_AddressPlayer.size();
	}
	static OO_TPROC_RET UDPProc(void *thisptr);
	static OO_TPROC_RET TCPProc(void *thisptr);
};
