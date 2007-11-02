/*

Copyright 2007  Julian aka masterfreek64, Joseph Pearson aka chessmaster42 and Joel Teichroeb aka bobjr777

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
#include "ConsoleServer.h"

extern bool Connected[MAXCLIENTS];
extern int MasterClient;
bool OOPWelcome_Handler(char *Packet,short LocalPlayer)
{
	OOPkgWelcome InPkgBuf;
	OOPkgWelcome OutPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgWelcome));

	//If this is an "I'm ready for init data" welcome, send init data
	if(InPkgBuf.Flags & 1)
	{
		for(int i=0; i<MAXCLIENTS; i++)
		{
			//If the player is not localPlayer and is connected
			if (i != LocalPlayer && Connected[i])
			{
				//And if the player is initialised continue
				if (Players[i].bStatsInitialized)
				{
					printf("  Init: Sending player %i info to player %i\n", i, LocalPlayer);
					if(adminSocket){
						char message[256];
						sprintf(message,"  Init: Sending player %i info to player %i\n", i, LocalPlayer);
						SendAdminMessage(message);
					}
					OOPkgActorUpdate SendPkgBuf;
					SendPkgBuf.etypeID = OOPActorUpdate;
					if (Players[i].bIsInInterior)
						SendPkgBuf.Flags = 1 | 2 | 8;
					else
						SendPkgBuf.Flags = 1 | 2 | 4 | 8;
					SendPkgBuf.refID = Players[i].RefID;
					SendPkgBuf.CellID = Players[i].CellID;
					SendPkgBuf.Health = Players[i].Health; // we send the full values now , s that the client can make sense out of the differences
					SendPkgBuf.Magika = Players[i].Magika;
					SendPkgBuf.Fatigue = Players[i].Fatigue;
					send(clients[LocalPlayer],(char *)&SendPkgBuf,sizeof(OOPkgActorUpdate),0);
				}
			}
		}
		return true;
	}

	//If player is already connected, ignore welcome
	if (Connected[LocalPlayer])
		return true;
	if(InPkgBuf.guidOblivionOnline == gcOOGUID)
	{
		if(InPkgBuf.wVersion == MAKEWORD(MAIN_VERSION,SUB_VERSION))
		{
			char *SendBuf;
			OutPkgBuf.etypeID = OOPWelcome;
			// Passwords do not really work !!! it just wastes EVERything
			//Replace with database check later
			/*
			if(!strcmp(InPkgBuf.Password, ServerPassword))
			{
				Authenticated[LocalPlayer] = true;
				OutPkgBuf.Flags = 1;
			}else{
				printf("Player%2d tried to use wrong password: %s\n", LocalPlayer, InPkgBuf.Password);
				if(adminSocket){
					char message[256];
					sprintf(message,"Player%2d tried to use wrong password: %s", LocalPlayer, InPkgBuf.Password);
					SendAdminMessage(message);
				}
				return false;
			}
			*/
				Authenticated[LocalPlayer] = true;
				OutPkgBuf.Flags = 1;
			if(MasterClient == -1)
			{
				// This one will be master client
				OutPkgBuf.Flags = OutPkgBuf.Flags | 2;
				MasterClient = LocalPlayer;
				printf("Client %i was selected as master client", LocalPlayer);
			}
			OutPkgBuf.guidOblivionOnline = gcOOGUID;
			OutPkgBuf.PlayerID = LocalPlayer;
			OutPkgBuf.wVersion = MAKEWORD(MAIN_VERSION,SUB_VERSION); 
			sprintf(OutPkgBuf.NickName,"Player%2d",LocalPlayer);
			SendBuf = (char *)malloc(sizeof(OOPkgWelcome));
			memcpy(SendBuf,&OutPkgBuf,sizeof(OOPkgWelcome));
			send(clients[LocalPlayer],SendBuf,sizeof(OOPkgWelcome),0);
			printf("Welcoming Player%2d\n",LocalPlayer);
			if(adminSocket){
				char message[256];
				sprintf(message,"Welcoming Player%2d",LocalPlayer);
				SendAdminMessage(message);
			}
			//printf("Player %2i - %s:%u\n",LocalPlayer,inet_ntoa(ConnectionInfo[LocalPlayer].sin_addr),ntohs(ConnectionInfo[LocalPlayer].sin_port));
			free(SendBuf);
			Connected[LocalPlayer] = true;
		}
		else
		{
			time_t TimeStamp;
			time(&TimeStamp);
			int Seconds = (int)TimeStamp % 60;
			int Minutes = (int)(TimeStamp / 60) % 60;
			int Hours = (int)(TimeStamp / 3600) % 24;
			char MyTime[10];
			sprintf(MyTime, "%2i:%2i:%2i", Hours, Minutes, Seconds);

			TotalClients--;
			int MinorVersion = (InPkgBuf.wVersion & 0xff00) >> 8;
			int MajorVersion = InPkgBuf.wVersion & 0x00ff;
			printf("%s - Client tried to authenticate with wrong client version(%i.%i.%i)\n", MyTime, 0, MajorVersion, MinorVersion);
			if(adminSocket){
				char message[256];
				sprintf(message,"%s - Client tried to authenticate with wrong client version(%i.%i.%i)", MyTime, 0, MajorVersion, MinorVersion);
				SendAdminMessage(message);
			}
			printf("%s - Client %d was removed due to version missmatch\n", MyTime, LocalPlayer);
			if(adminSocket){
				char message[256];
				sprintf(message,"%s - Client %d was removed due to version missmatch", MyTime, LocalPlayer);
				SendAdminMessage(message);
			}
			fprintf(easylog,"%s - Client %d closed the Connection\n", MyTime, LocalPlayer);
			fprintf(easylog,"%s - Client %d had a version mismatch\n", MyTime, LocalPlayer);
			fprintf(easylog,"%s - We now have %d connections", MyTime, TotalClients);
			closesocket(clients[LocalPlayer]); 
			clients[LocalPlayer]=INVALID_SOCKET;
		}
	}
	else
	{
		TotalClients--;
		printf("Client %d tried to connect with a pre-version 4 OblivionOnline , or another software.\n",LocalPlayer);
		if(adminSocket){
			char message[256];
			sprintf(message,"Client %d tried to connect with a pre-version 4 OblivionOnline , or another software.",LocalPlayer);
			SendAdminMessage(message);
		}
		fprintf(easylog,"Client %d closed the Connection\n",LocalPlayer);
		fprintf(easylog,"Client %d had a non-compatible client\n",LocalPlayer);
		fprintf(easylog,"We now have %d connections",TotalClients);
		closesocket(clients[LocalPlayer]);
		clients[LocalPlayer]=INVALID_SOCKET;
	}
	return true;
}

bool OOPDisconnect_Handler(char *Packet,short LocalPlayer)
{
	if (Connected[LocalPlayer])
	{
		OOPkgDisconnect InPkgBuf;
		memcpy(&InPkgBuf,Packet,sizeof(OOPkgDisconnect));
		printf("Received disconnect from %i\n", InPkgBuf.PlayerID);
		if(adminSocket){
			char message[256];
			sprintf(message,"Received disconnect from %i", InPkgBuf.PlayerID);
			SendAdminMessage(message);
		}
		for(int cx=0;cx<MAXCLIENTS;cx++)
		{
			if (cx != LocalPlayer)
				send(clients[cx],(char *)&InPkgBuf,sizeof(OOPkgDisconnect),0);
		}
	}
	return true;
}

bool OOPActorUpdate_Handler(char *Packet,short LocalPlayer)
{
	OOPkgActorUpdate InPkgBuf;
	OOPkgActorUpdate OutPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgActorUpdate));
	if(InPkgBuf.Flags | 1)
	{
	if (InPkgBuf.refID < MAXCLIENTS)
	{
		OutPkgBuf.etypeID = OOPActorUpdate;
		OutPkgBuf.Flags = InPkgBuf.Flags;
		OutPkgBuf.refID = InPkgBuf.refID;
		OutPkgBuf.CellID = InPkgBuf.CellID;
		OutPkgBuf.fPosX = InPkgBuf.fPosX;
		OutPkgBuf.fPosY = InPkgBuf.fPosY;
		OutPkgBuf.fPosZ = InPkgBuf.fPosZ;
		OutPkgBuf.fRotX = InPkgBuf.fRotX;
		OutPkgBuf.fRotY = InPkgBuf.fRotY;
		OutPkgBuf.fRotZ = InPkgBuf.fRotZ;
		OutPkgBuf.InCombat = InPkgBuf.InCombat;

		UInt32 tempCell = Players[InPkgBuf.refID].CellID;
		Players[InPkgBuf.refID].CellID = OutPkgBuf.CellID;
		if (Players[InPkgBuf.refID].CellID != tempCell){
			printf("  Player %i moved from cell %x to cell %x\n", InPkgBuf.refID, tempCell, Players[InPkgBuf.refID].CellID);
			if(adminSocket){
				char message[256];
				sprintf(message,"  Player %i moved from cell %x to cell %x", InPkgBuf.refID, tempCell, Players[InPkgBuf.refID].CellID);
				SendAdminMessage(message);
			}
		}

		//If the intialize flag is set, set init to true
		if(!Players[InPkgBuf.refID].bStatsInitialized)
		{
			printf("  Initializing player %i basic stats", InPkgBuf.refID);
			if(adminSocket){
				char message[256];
				sprintf(message,"  Initializing player %i basic stats", InPkgBuf.refID);
				SendAdminMessage(message);
			}
			Players[InPkgBuf.refID].bStatsInitialized = true;
			OutPkgBuf.Flags = OutPkgBuf.Flags | 8;
		}

		//Now check for changes in the basic stats
		if(Players[InPkgBuf.refID].Health != InPkgBuf.Health)
		{
			OutPkgBuf.Health = InPkgBuf.Health - Players[InPkgBuf.refID].Health;
			Players[InPkgBuf.refID].Health += OutPkgBuf.Health;
			if (Players[InPkgBuf.refID].Health < 0)
			{
				OutPkgBuf.Health += Players[InPkgBuf.refID].Health;
				Players[InPkgBuf.refID].Health = 0;
			}else{
				printf("  Player %i HP is %i (change of %i)\n", InPkgBuf.refID, Players[InPkgBuf.refID].Health, OutPkgBuf.Health);
				/*if(adminSocket){
					char message[256];
					sprintf(message,"  Player %i HP is %i (change of %i)", InPkgBuf.refID, Players[InPkgBuf.refID].Health, OutPkgBuf.Health);
					SendAdminMessage(message);
				}*/
			}
		}else
			OutPkgBuf.Health = 0;
		if(Players[InPkgBuf.refID].Magika != InPkgBuf.Magika)
		{
			OutPkgBuf.Magika = InPkgBuf.Magika - Players[InPkgBuf.refID].Magika;
			Players[InPkgBuf.refID].Magika += OutPkgBuf.Magika;
			if (Players[InPkgBuf.refID].Magika < 0)
			{
				OutPkgBuf.Magika += Players[InPkgBuf.refID].Magika;
				Players[InPkgBuf.refID].Magika = 0;
			}
			//printf("  Player %i MP is %i (change of %i)\n", InPkgBuf.refID, Players[InPkgBuf.refID].Magika, OutPkgBuf.Magika);
		}else
			OutPkgBuf.Magika = 0;
		if(Players[InPkgBuf.refID].Fatigue != InPkgBuf.Fatigue)
		{
			OutPkgBuf.Fatigue = InPkgBuf.Fatigue - Players[InPkgBuf.refID].Fatigue;
			Players[InPkgBuf.refID].Fatigue += OutPkgBuf.Fatigue;
			if (Players[InPkgBuf.refID].Fatigue < 0)
			{
				OutPkgBuf.Fatigue += Players[InPkgBuf.refID].Fatigue;
				Players[InPkgBuf.refID].Fatigue = 0;
			}
			//printf("  Player %i Fatigue is %i (change of %i)\n", InPkgBuf.refID, Players[InPkgBuf.refID].Fatigue, OutPkgBuf.Fatigue);
		}else
			OutPkgBuf.Fatigue = 0;

	//Now send out the data
	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		if (cx != LocalPlayer)
			send(clients[cx],(char *)&OutPkgBuf,sizeof(OOPkgActorUpdate),0);
	}
	}
	}




	else// It is an NPC
	{
		OOMobHashTableEntry *ptr;
		ptr = (OOMobHashTableEntry *)MobTable.Find(InPkgBuf.refID);
		if(!ptr) // We got to insert it
		{
			//Make one , insert it....
			ptr = (OOMobHashTableEntry *)malloc(sizeof(OOMobHashTableEntry)); // We free that in the cleanup code ...
			ptr->ID = InPkgBuf.refID;
			ptr->Status.RefID = InPkgBuf.refID;
			ptr->Status.CellID = InPkgBuf.CellID;
			ptr->Status.PosX = InPkgBuf.fPosX;
			ptr->Status.PosY = InPkgBuf.fPosY;
			ptr->Status.PosZ = InPkgBuf.fPosZ;
			ptr->Status.RotX = InPkgBuf.fRotX;
			ptr->Status.RotY = InPkgBuf.fRotY;
			ptr->Status.RotZ = InPkgBuf.fRotZ;
			ptr->Status.InCombat = InPkgBuf.InCombat;

			ptr->Status.Fatigue = InPkgBuf.Fatigue;
			ptr->Status.Magika = InPkgBuf.Magika;
			
			if(!MobTable.Insert((void *)ptr))
			{
				printf("Could not store actor %u in Database ... hash not unique \n",ptr->ID);
				// This message would mean your CPU has too much alcohol ... if there was a non unique hash we would have found it
			}
			else
			{
				//We give a message
				printf("Actor %u spawned with %i Health in cell %u  \n",ptr->ID,ptr->Status.Health,ptr->Status.CellID);
			}
		}
		// Health Magicka and Fatigue -....
		if(ptr->Status.Health != InPkgBuf.Health)
		{
				OutPkgBuf.Health = InPkgBuf.Health - ptr->Status.Health;
				ptr->Status.Health += OutPkgBuf.Health;
				// Handle dying here ... we should remove all mobs dying from the DB
				if(ptr->Status.Health < 0)
				{
					printf("Actor %u died in cell %u \n",ptr->ID,ptr->Status.CellID);
					// Handle despawning here
				}
		}
		if(ptr->Status.Magika != InPkgBuf.Magika)
		{
				OutPkgBuf.Magika = InPkgBuf.Magika - ptr->Status.Magika;
				ptr->Status.Magika += OutPkgBuf.Magika;
		}
		if(ptr->Status.Fatigue != InPkgBuf.Fatigue)
		{
				OutPkgBuf.Fatigue = InPkgBuf.Fatigue - ptr->Status.Fatigue;
				ptr->Status.Fatigue += OutPkgBuf.Fatigue;
		}
		//Position
		OutPkgBuf.etypeID = OOPActorUpdate;
		OutPkgBuf.Flags = InPkgBuf.Flags;
		OutPkgBuf.refID = InPkgBuf.refID;
		OutPkgBuf.CellID = InPkgBuf.CellID;
		OutPkgBuf.fPosX = InPkgBuf.fPosX;
		OutPkgBuf.fPosY = InPkgBuf.fPosY;
		OutPkgBuf.fPosZ = InPkgBuf.fPosZ;
		OutPkgBuf.fRotX = InPkgBuf.fRotX;
		OutPkgBuf.fRotY = InPkgBuf.fRotY;
		OutPkgBuf.fRotZ = InPkgBuf.fRotZ;
		OutPkgBuf.InCombat = InPkgBuf.InCombat;
		for(int cx=0;cx<MAXCLIENTS;cx++)
		{
			if (cx != LocalPlayer)
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
		if (cx != LocalPlayer)
			send(clients[cx],Packet,sizeof(OOPkgChat)+InPkgBuf.Length,0);
	}
	if (InPkgBuf.Length < 1024)
	{
		char MessageDest[1024] = "\0";
		for(int i=0; i<InPkgBuf.Length; i++)
		{
			MessageDest[i] = Packet[i+sizeof(OOPkgChat)];
		}
		printf("Player %i: %s\n", LocalPlayer, MessageDest);
		if(adminSocket){
			char message[256];
			sprintf(message,"Player %i: %s", LocalPlayer, MessageDest);
			SendAdminMessage(message);
		}
	}
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
	OOPkgFullStatUpdate OutPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgFullStatUpdate));
	if (InPkgBuf.refID < MAXCLIENTS)
	{
		OutPkgBuf.etypeID = OOPFullStatUpdate;
		OutPkgBuf.refID = InPkgBuf.refID;
		OutPkgBuf.Flags = InPkgBuf.Flags;
		//Are we setting initial values?
		if (InPkgBuf.Flags & 8)
		{
			OutPkgBuf.Health = InPkgBuf.Health;
			OutPkgBuf.Magika = InPkgBuf.Magika;
			OutPkgBuf.Fatigue = InPkgBuf.Fatigue;
			Players[InPkgBuf.refID].Health = InPkgBuf.Health;
			Players[InPkgBuf.refID].Magika = InPkgBuf.Magika;
			Players[InPkgBuf.refID].Fatigue = InPkgBuf.Fatigue;
			PlayersInitial[InPkgBuf.refID].Health = InPkgBuf.Health;
			PlayersInitial[InPkgBuf.refID].Magika = InPkgBuf.Magika;
			PlayersInitial[InPkgBuf.refID].Fatigue = InPkgBuf.Fatigue;
			printf("Client %i full stats initialized\n", InPkgBuf.refID);
			if(adminSocket){
				char message[256];
				sprintf(message,"Client %i full stats initialized", InPkgBuf.refID);
				SendAdminMessage(message);
			}
			printf(" HP: %i\n", Players[InPkgBuf.refID].Health);
			if(adminSocket){
				char message[256];
				sprintf(message," HP: %i", Players[InPkgBuf.refID].Health);
				SendAdminMessage(message);
			}
			printf(" MP: %i\n", Players[InPkgBuf.refID].Magika);
			if(adminSocket){
				char message[256];
				sprintf(message," MP: %i", Players[InPkgBuf.refID].Magika);
				SendAdminMessage(message);
			}
		}else{
			OutPkgBuf.Health = InPkgBuf.Health - Players[InPkgBuf.refID].Health;
			OutPkgBuf.Magika = InPkgBuf.Magika - Players[InPkgBuf.refID].Magika;
			OutPkgBuf.Fatigue = InPkgBuf.Fatigue - Players[InPkgBuf.refID].Fatigue;
			//printf("Player HP: %i | Incoming HP: %i (FSU)\n", Players[InPkgBuf.refID].Health, InPkgBuf.Health);
			Players[InPkgBuf.refID].Health += OutPkgBuf.Health;
			Players[InPkgBuf.refID].Magika += OutPkgBuf.Magika;
			Players[InPkgBuf.refID].Fatigue += OutPkgBuf.Fatigue;
			if (OutPkgBuf.Health != 0)
				printf("  Player %i HP is %i (change of %i)(FSU)\n", LocalPlayer, InPkgBuf.refID, Players[InPkgBuf.refID].Health, OutPkgBuf.Health);
				if(adminSocket){
					char message[256];
					sprintf(message,"  Player %i HP is %i (change of %i)(FSU)", LocalPlayer, InPkgBuf.refID, Players[InPkgBuf.refID].Health, OutPkgBuf.Health);
					SendAdminMessage(message);
				}
		}
		for(int cx=0;cx<MAXCLIENTS;cx++)
		{
			if (cx != LocalPlayer)
				send(clients[cx],(char *)&OutPkgBuf,sizeof(OOPkgFullStatUpdate),0);
		}
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

bool OOPEquipped_Handler(char *Packet,short LocalPlayer)
{
	OOPkgEquipped InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgEquipped));
	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		if (cx != LocalPlayer)
			send(clients[cx],(char *)&InPkgBuf,sizeof(OOPkgEquipped),0);
	}
	return true;
}

bool OOPModOffsetList_Handler(char *Packet,short LocalPlayer)
{
	OOPkgModOffsetList InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgModOffsetList));
	printf("Received mod list (%i mods) from %i:\n", InPkgBuf.NumOfMods, InPkgBuf.refID);
	if(adminSocket){
		char message[256];
		sprintf(message,"Received mod list (%i mods) from %i:", InPkgBuf.NumOfMods, InPkgBuf.refID);
		SendAdminMessage(message);
	}
	for(int cx=0;cx<InPkgBuf.NumOfMods;cx++)
	{
		ModList[LocalPlayer][cx] = Packet[cx+sizeof(OOPkgModOffsetList)];
		if (ModList[LocalPlayer][cx])
			printf("  Mod %i: %i\n", cx, ModList[LocalPlayer][cx]);
			if(adminSocket){
				char message[256];
				sprintf(message,"  Mod %i: %i\n", cx, ModList[LocalPlayer][cx]);
				SendAdminMessage(message);
			}
	}
	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		if (cx != LocalPlayer)
			send(clients[cx],Packet,sizeof(OOPkgModOffsetList)+InPkgBuf.NumOfMods,0);
	}
	return true;
}

bool AdminMsg_Handler(char *Data, short Length)
{
	return true;
}

bool AdminAuth_Handler(char *Data, short Length)
{
	if(!strcmp(Data, AdminPassword))
		printf("Admin authentication good.\n");
	else{
		printf("Admin authentication bad: %s.\n", Data);
		printf("Closing connection.\n");
		closesocket(adminSocket);
	}
	return true;
}

bool AdminChat_Handler(char *Data, short Length)
{
	UInt8 PlayerNum = Data[0];
	char tempData[512];
	memcpy(tempData, &Data[1], Length - 1);
	if (PlayerNum != 255)
		BroadcastMessage(tempData, PlayerNum);
	else
		BroadcastMessage(tempData, -1);
	return true;
}

bool AdminKick_Handler(char *Data, short Length)
{
	UInt8 PlayerNum = Data[0];
	Kick(PlayerNum);
	return true;
}

bool OOPAdminInfo_Handler(char *Packet, short Length)
{
	OOPkgAdminInfo InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgAdminInfo));
	char MessageDest[512];
	memcpy(&MessageDest, Packet + sizeof(OOPkgAdminInfo), Length - sizeof(OOPkgAdminInfo));
	MessageDest[Length - sizeof(OOPkgAdminInfo)] = '\0';
	switch(InPkgBuf.ControlCommand)
	{
	case MSGCONTROL:
		AdminMsg_Handler(MessageDest, Length);
		break;
	case AUTHCONTROL:
		AdminAuth_Handler(MessageDest, Length);
		break;
	case CHATCONTROL:
		AdminChat_Handler(MessageDest, Length);
		break;
	case KICKCONTROL:
		AdminKick_Handler(MessageDest, Length);
		break;
	default:
		break;
	};
	return true;
}
