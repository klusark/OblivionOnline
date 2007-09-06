/*

Copyright 2007  Julian aka masterfreek64 and Joseph Pearson aka chessmaster42 

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

#include "PacketHandler.h"
#include "BasicServer.h"

extern bool Connected[MAXCLIENTS];

bool OOPWelcome_Handler(char *Packet,short LocalPlayer)
{
	if (Connected[LocalPlayer])
		return true;
	OOPkgWelcome InPkgBuf;
	OOPkgWelcome OutPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgWelcome));
	if(InPkgBuf.guidOblivionOnline == gcOOGUID)
	{
		if(InPkgBuf.wVersion == MAKEWORD(MAIN_VERSION,SUB_VERSION))
		{
			char *SendBuf;
			OutPkgBuf.etypeID = OOPWelcome;
			OutPkgBuf.Flags = 0;
			OutPkgBuf.guidOblivionOnline = gcOOGUID;
			OutPkgBuf.PlayerID = LocalPlayer;
			OutPkgBuf.wVersion = MAKEWORD(MAIN_VERSION,SUB_VERSION); 
			sprintf(OutPkgBuf.NickName,"Player%2d",LocalPlayer);
			SendBuf = (char *)malloc(sizeof(OOPkgWelcome));
			memcpy(SendBuf,&OutPkgBuf,sizeof(OOPkgWelcome));
			send(clients[LocalPlayer],SendBuf,sizeof(OOPkgWelcome),0);
			printf("Welcoming Player%2d\n",LocalPlayer);
			free(SendBuf);
			Connected[LocalPlayer] = true;
		}
		else
		{
			TotalClients--;
			printf("Client tried to authenticate with wrong client Version\n");
			printf("Client %d was removed due to version missmatch\n",LocalPlayer);
			fprintf(easylog,"Client %d closed the Connection\n",LocalPlayer);
			fprintf(easylog,"Client %d had a version mismatch\n",LocalPlayer);
			fprintf(easylog,"We now have %d connections",TotalClients);
			closesocket(clients[LocalPlayer]); 
			clients[LocalPlayer]=INVALID_SOCKET; 
		}
	}
	else
	{
		TotalClients--;
		printf("Client %d tried to connect with a pre-version 3 OblivionOnline , or another software.\n",LocalPlayer);
		fprintf(easylog,"Client %d closed the Connection\n",LocalPlayer);
		fprintf(easylog,"Client %d had a non-compatible client\n",LocalPlayer);
		fprintf(easylog,"We now have %d connections",TotalClients);
		closesocket(clients[LocalPlayer]);
		clients[LocalPlayer]=INVALID_SOCKET;
	}
	return true;
}
bool OOPActorUpdate_Handler(char *Packet,short LocalPlayer)
{
	OOPkgActorUpdate InPkgBuf;
	OOPkgActorUpdate OutPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgActorUpdate));
	if (InPkgBuf.refID < MAXCLIENTS)
	{
		OutPkgBuf.etypeID = OOPActorUpdate;
		OutPkgBuf.refID = InPkgBuf.refID;
		OutPkgBuf.CellID = InPkgBuf.CellID;
		OutPkgBuf.Flags = InPkgBuf.Flags;
		OutPkgBuf.fPosX = InPkgBuf.fPosX;
		OutPkgBuf.fPosY = InPkgBuf.fPosY;
		OutPkgBuf.fPosZ = InPkgBuf.fPosZ;
		OutPkgBuf.fRotX = InPkgBuf.fRotX;
		OutPkgBuf.fRotY = InPkgBuf.fRotY;
		OutPkgBuf.fRotZ = InPkgBuf.fRotZ;
		OutPkgBuf.Health = InPkgBuf.Health - Players[InPkgBuf.refID].Health;
		OutPkgBuf.Magika = InPkgBuf.Magika - Players[InPkgBuf.refID].Magika;
		OutPkgBuf.Fatigue = InPkgBuf.Fatigue - Players[InPkgBuf.refID].Fatigue;
		Players[InPkgBuf.refID].Health += OutPkgBuf.Health;
		Players[InPkgBuf.refID].Magika += OutPkgBuf.Magika;
		Players[InPkgBuf.refID].Fatigue += OutPkgBuf.Fatigue;
		//Temp
		if (OutPkgBuf.Health != 0)
		{
			printf("Player %i HP: %i (change of %i)\n", InPkgBuf.refID,
				Players[InPkgBuf.refID].Health, OutPkgBuf.Health);
		}
		//End Temp
		for(int cx=0;cx<MAXCLIENTS;cx++)
		{
			send(clients[cx],(char *)&OutPkgBuf,sizeof(OOPkgActorUpdate),0);
		}
	}
	return true;
}
bool OOPChat_Handler(char *Packet,short LocalPlayer)
{
	OOPkgChat InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgChat));
	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		send(clients[cx],Packet,sizeof(OOPkgChat)+InPkgBuf.Length,0);
	}
	//Temp
	char MessageDest[1024] = "\0";
	for(int i=0; i<InPkgBuf.Length; i++)
	{
		MessageDest[i] = Packet[i+sizeof(OOPkgChat)];
	}
	printf("Player %i: %s\n", LocalPlayer, MessageDest);
	//End Temp
	return true;
}
bool OOPEvent_Handler(char *Packet,short LocalPlayer)
{
	//EVENTS ARE IGNORED UNTIL PLUGIN SYSTEM IS THERE
	return true;
}
bool OOPEventRegister_Handler(char *Packet,short LocalPlayer)
{
	//EVENTS ARE IGNORED UNTIL PLUGIN SYSTEM IS THERE
	return true;
}
bool OOPFullStatUpdate_Handler(char *Packet,short LocalPlayer)
{
	OOPkgFullStatUpdate InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgFullStatUpdate));
	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		send(clients[cx],(char *)&InPkgBuf,sizeof(OOPkgFullStatUpdate),0);
	}
	return true;
}
bool OOPTimeUpdate_Handler(char *Packet,short LocalPlayer)
{
	time_t ServerTime;
	OOPkgTimeUpdate OutPkgBuf;

	time(&ServerTime);
	OutPkgBuf.etypeID = OOPTimeUpdate;
	OutPkgBuf.Seconds = (int)ServerTime % 60;
	OutPkgBuf.Minutes = (int)(ServerTime / 60) % 60;
	OutPkgBuf.Hours = (int)(ServerTime / 3600) % 24;
	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		send(clients[cx],(char *)&OutPkgBuf,sizeof(OOPkgTimeUpdate),0);
	}
	return true;
}
bool OOPPlayerList_Handler(char *Packet,short LocalPlayer)
{
	char *SendBuf;
	OOPkgPlayerList OutPkgBuf;
	OutPkgBuf.etypeID = OOPPlayerList;
	SendBuf = (char *)malloc(sizeof(OOPkgPlayerList) + MAXCLIENTS);
	memcpy(SendBuf, &OutPkgBuf, sizeof(OOPkgPlayerList));
	for(int cx=0; cx<MAXCLIENTS; cx++)
	{
		if (Connected[cx])
			SendBuf[cx + sizeof(OOPkgPlayerList)] = cx;
		else
			SendBuf[cx + sizeof(OOPkgPlayerList)] = 255;
	}
	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		send(clients[cx],SendBuf,sizeof(OOPkgPlayerList),0);
	}
	free(SendBuf);
	return true;
}
