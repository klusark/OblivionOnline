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
bool OOPPosUpdate_Handler(char *Packet,short LocalPlayer)
{
	char *SendBuf;
	OOPkgPosUpdate InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgPosUpdate));
	SendBuf = (char *)malloc(sizeof(OOPkgPosUpdate));
	memcpy(SendBuf,&InPkgBuf,sizeof(OOPkgPosUpdate));
	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		send(clients[cx],SendBuf,sizeof(OOPkgPosUpdate),0);
	}
	free(SendBuf);
	return true;
}
bool OOPZone_Handler(char *Packet,short LocalPlayer)
{
	char *SendBuf;
	OOPkgZone InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgZone));
	SendBuf = (char *)malloc(sizeof(OOPkgZone));
	memcpy(SendBuf,&InPkgBuf,sizeof(OOPkgZone));
	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		send(clients[cx],SendBuf,sizeof(OOPkgZone),0);
	}
	free(SendBuf);
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
	//CHEATER:::: phear ---- also ignored
	return true;
}
bool OOPFullStatUpdate_Handler(char *Packet,short LocalPlayer)
{
	char *SendBuf;
	OOPkgFullStatUpdate InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgFullStatUpdate));
	SendBuf = (char *)malloc(sizeof(OOPkgFullStatUpdate));
	memcpy(SendBuf,&InPkgBuf,sizeof(OOPkgFullStatUpdate));
	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		send(clients[cx],SendBuf,sizeof(OOPkgFullStatUpdate),0);
	}
	free(SendBuf);
	return true;
}
bool OOPStatUpdate_Handler(char *Packet,short LocalPlayer)
{
	char *SendBuf;
	OOPkgStatUpdate InPkgBuf;
	OOPkgStatUpdate OutPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgStatUpdate));
	printf("Stat from %u with HP of %i\n", InPkgBuf.refID, InPkgBuf.Health);
	if (InPkgBuf.refID < MAXCLIENTS)
	{
		OutPkgBuf.etypeID = InPkgBuf.etypeID;
		OutPkgBuf.refID = InPkgBuf.refID;
		OutPkgBuf.Flags = InPkgBuf.Flags;
		OutPkgBuf.TimeStamp = InPkgBuf.TimeStamp;
		OutPkgBuf.Health = InPkgBuf.Health - Players[InPkgBuf.refID].Health;
		OutPkgBuf.Magika = InPkgBuf.Magika - Players[InPkgBuf.refID].Magika;
		OutPkgBuf.Fatigue = InPkgBuf.Fatigue - Players[InPkgBuf.refID].Fatigue;
		Players[InPkgBuf.refID].Health += OutPkgBuf.Health;
		Players[InPkgBuf.refID].Magika += OutPkgBuf.Magika;
		Players[InPkgBuf.refID].Fatigue += OutPkgBuf.Fatigue;
	}
	SendBuf = (char *)malloc(sizeof(OOPkgStatUpdate));
	memcpy(SendBuf,&OutPkgBuf,sizeof(OOPkgStatUpdate));
	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		send(clients[cx],SendBuf,sizeof(OOPkgStatUpdate),0);
	}
	free(SendBuf);
	return true;
}
bool OOPTimeUpdate_Handler(char *Packet,short LocalPlayer)
{
	char *SendBuf;
	time_t ServerTime;
	OOPkgTimeUpdate OutPkgBuf;

	time(&ServerTime);
	OutPkgBuf.etypeID = OOPTimeUpdate;
	OutPkgBuf.Seconds = (int)ServerTime % 60;
	OutPkgBuf.Minutes = (int)(ServerTime / 60) % 60;
	OutPkgBuf.Hours = (int)(ServerTime / 3600) % 24;
	//Temp - Disables Time sync for testing
	OutPkgBuf.Seconds = 0;
	OutPkgBuf.Minutes = 0;
	OutPkgBuf.Hours = 10;
	//End Temp
	SendBuf = (char *)malloc(sizeof(OOPkgTimeUpdate));
	memcpy(SendBuf,&OutPkgBuf,sizeof(OOPkgTimeUpdate));
	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		send(clients[cx],SendBuf,sizeof(OOPkgTimeUpdate),0);
	}
	free(SendBuf);
	return true;
}
