/*

Copyright 2007   Julian Bangert aka masterfreek64, Joseph Pearson aka chessmaster42 

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
	OOPkgWelcome * InPkgBuf = (OOPkgWelcome *)Packet;
	OOPkgWelcome OutPkgBuf;

	//If this is an "I'm ready for init data" welcome, send init data
	if(InPkgBuf->Flags & 1)
	{
		for(int i=0; i<MAXCLIENTS; i++)
		{
			//If the player is not localPlayer and is connected
			if (i != LocalPlayer && Connected[i])
			{
				//And if the player is initialised continue
				if (Players[i].bStatsInitialized)
				{
					GenericLog.DoOutput(LOG_MESSAGE,"  Init: Sending player %i info to player %i\n", i, LocalPlayer);
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
	if(InPkgBuf->guidOblivionOnline == gcOOGUID)
	{
		if(InPkgBuf->wVersion == MAKEWORD(MAIN_VERSION,SUB_VERSION))
		{
			char *SendBuf;
			OutPkgBuf.etypeID = OOPWelcome;
			// Passwords do not really work !!! it just wastes EVERything
			//Replace with database check later
			/*
			if(!strcmp(InPkgBuf->Password, ServerPassword))
			{
				Authenticated[LocalPlayer] = true;
				OutPkgBuf.Flags = 1;
			}else{
				printf("Player%2d tried to use wrong password: %s\n", LocalPlayer, InPkgBuf->Password);
				return false;
			}
			*/
				
				OutPkgBuf.Flags = 1;
			if(MasterClient == -1)
			{
				// This one will be master client
				OutPkgBuf.Flags = OutPkgBuf.Flags | 4;
				MasterClient = LocalPlayer;
				GenericLog.DoOutput(LOG_MESSAGE,"Client %i was selected as master client", LocalPlayer);
			}
			OutPkgBuf.guidOblivionOnline = gcOOGUID;
			OutPkgBuf.PlayerID = LocalPlayer;
			OutPkgBuf.wVersion = MAKEWORD(MAIN_VERSION,SUB_VERSION); 
			sprintf(OutPkgBuf.NickName,"Player%2d",LocalPlayer);
			SendBuf = (char *)malloc(sizeof(OOPkgWelcome));
			memcpy(SendBuf,&OutPkgBuf,sizeof(OOPkgWelcome));
			send(clients[LocalPlayer],SendBuf,sizeof(OOPkgWelcome),0);
			GenericLog.DoOutput(LOG_MESSAGE,"Welcoming Player%2d\n",LocalPlayer);
			//printf("Player %2i - %s:%u\n",LocalPlayer,inet_ntoa(ConnectionInfo[LocalPlayer].sin_addr),ntohs(ConnectionInfo[LocalPlayer].sin_port));
			free(SendBuf);
			Connected[LocalPlayer] = true;
		}
		else
		{
			

			TotalClients--;
			int MinorVersion = (InPkgBuf->wVersion & 0xff00) >> 8;
			int MajorVersion = InPkgBuf->wVersion & 0x00ff;
			GenericLog.DoOutput(LOG_WARNING,"%s - Client tried to authenticate with wrong client version(%i.%i.%i)\n", 0, MajorVersion, MinorVersion);

			GenericLog.DoOutput(LOG_ERROR,"%s - Client %d was removed due to version missmatch\n", LocalPlayer);
			closesocket(clients[LocalPlayer]); 
			clients[LocalPlayer]=INVALID_SOCKET;
		}
	}
	else
	{
		TotalClients--;
		GenericLog.DoOutput(LOG_ERROR,"Client %d tried to connect with a pre-version 3 OblivionOnline , or another software.\n",LocalPlayer);
		closesocket(clients[LocalPlayer]);
		clients[LocalPlayer]=INVALID_SOCKET;
	}
	return true;
}

bool OOPDisconnect_Handler(char *Packet,short LocalPlayer)
{
	if (Connected[LocalPlayer])
	{
		OOPkgDisconnect  * InPkgBuf = (OOPkgDisconnect *) Packet;
		GenericLog.DoOutput(LOG_MESSAGE,"Received disconnect from %i\n", InPkgBuf->PlayerID);
		for(int cx=0;cx<MAXCLIENTS;cx++)
		{
			if (cx != LocalPlayer&&clients[cx])
				send(clients[cx],(char *)&InPkgBuf,sizeof(OOPkgDisconnect),0);
		}
	}
	return true;
}

bool OOPActorUpdate_Handler(char *Packet,short LocalPlayer)
{

	OOPkgActorUpdate * InPkgBuf = (OOPkgActorUpdate *) Packet;
	OOPkgActorUpdate OutPkgBuf;
	if(InPkgBuf->Flags & 1)
	{
	if (InPkgBuf->refID < MAXCLIENTS)
	{
		OutPkgBuf.etypeID = OOPActorUpdate;
		OutPkgBuf.Flags = InPkgBuf->Flags;
		OutPkgBuf.refID = InPkgBuf->refID;
		OutPkgBuf.CellID = InPkgBuf->CellID;
		OutPkgBuf.fPosX = InPkgBuf->fPosX;
		OutPkgBuf.fPosY = InPkgBuf->fPosY;
		OutPkgBuf.fPosZ = InPkgBuf->fPosZ;
		OutPkgBuf.fRotX = InPkgBuf->fRotX;
		OutPkgBuf.fRotY = InPkgBuf->fRotY;
		OutPkgBuf.fRotZ = InPkgBuf->fRotZ;
		UInt32 tempCell = Players[InPkgBuf->refID].CellID;
		Players[InPkgBuf->refID].CellID = OutPkgBuf.CellID;
		if (Players[InPkgBuf->refID].CellID != tempCell){
			GenericLog.DoOutput(LOG_MESSAGE,"  Player %i moved from cell %x to cell %x\n", InPkgBuf->refID, tempCell, Players[InPkgBuf->refID].CellID);
		}

		//If the intialize flag is set, set init to true
		if(!Players[InPkgBuf->refID].bStatsInitialized)
		{
			GenericLog.DoOutput(LOG_MESSAGE,"  Initializing player %i basic stats", InPkgBuf->refID);
			Players[InPkgBuf->refID].bStatsInitialized = true;
			OutPkgBuf.Flags = OutPkgBuf.Flags | 8;
		}

		//Now check for changes in the basic stats
		if(Players[InPkgBuf->refID].Health != InPkgBuf->Health)
		{
			OutPkgBuf.Health = InPkgBuf->Health - Players[InPkgBuf->refID].Health;
			Players[InPkgBuf->refID].Health += OutPkgBuf.Health;
			if (Players[InPkgBuf->refID].Health < 0)
			{
				OutPkgBuf.Health += Players[InPkgBuf->refID].Health;
				Players[InPkgBuf->refID].Health = 0;
			}else{
				GenericLog.DoOutput(LOG_MESSAGE,"  Player %i HP is %i (change of %i)\n", InPkgBuf->refID, Players[InPkgBuf->refID].Health, OutPkgBuf.Health);
				}
		}else
			OutPkgBuf.Health = 0;
		if(Players[InPkgBuf->refID].Magika != InPkgBuf->Magika)
		{
			OutPkgBuf.Magika = InPkgBuf->Magika - Players[InPkgBuf->refID].Magika;
			Players[InPkgBuf->refID].Magika += OutPkgBuf.Magika;
			if (Players[InPkgBuf->refID].Magika < 0)
			{
				OutPkgBuf.Magika += Players[InPkgBuf->refID].Magika;
				Players[InPkgBuf->refID].Magika = 0;
			}
			//printf("  Player %i MP is %i (change of %i)\n", InPkgBuf->refID, Players[InPkgBuf->refID].Magika, OutPkgBuf.Magika);
		}else
			OutPkgBuf.Magika = 0;
		if(Players[InPkgBuf->refID].Fatigue != InPkgBuf->Fatigue)
		{
			OutPkgBuf.Fatigue = InPkgBuf->Fatigue - Players[InPkgBuf->refID].Fatigue;
			Players[InPkgBuf->refID].Fatigue += OutPkgBuf.Fatigue;
			if (Players[InPkgBuf->refID].Fatigue < 0)
			{
				OutPkgBuf.Fatigue += Players[InPkgBuf->refID].Fatigue;
				Players[InPkgBuf->refID].Fatigue = 0;
			}
			//printf("  Player %i Fatigue is %i (change of %i)\n", InPkgBuf->refID, Players[InPkgBuf->refID].Fatigue, OutPkgBuf.Fatigue);
		}else
			OutPkgBuf.Fatigue = 0;

	//Now send out the data
	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		if (cx != LocalPlayer&&clients[cx])
			send(clients[cx],(char *)&OutPkgBuf,sizeof(OOPkgActorUpdate),0);
	}
	}
	}




	else// It is an NPC
	{
		OOMobHashTableEntry *ptr;
		ptr = (OOMobHashTableEntry *)MobTable.Find(InPkgBuf->refID);
		if(!ptr) // We got to insert it
		{
			//Make one , insert it....
			ptr = (OOMobHashTableEntry *)malloc(sizeof(OOMobHashTableEntry)); // We free that in the cleanup code ...
			ptr->ID = InPkgBuf->refID;
			ptr->Status.RefID = InPkgBuf->refID;
			ptr->Status.CellID = InPkgBuf->CellID;
			ptr->Status.PosX = InPkgBuf->fPosX;
			ptr->Status.PosY = InPkgBuf->fPosY;
			ptr->Status.PosZ = InPkgBuf->fPosZ;
			ptr->Status.RotX = InPkgBuf->fRotX;
			ptr->Status.RotY = InPkgBuf->fRotY;
			ptr->Status.RotZ = InPkgBuf->fRotZ;
			ptr->Status.Health = InPkgBuf->Health;
			ptr->Status.Fatigue = InPkgBuf->Fatigue;
			ptr->Status.Magika = InPkgBuf->Magika;
			
			if(!MobTable.Insert((void *)ptr))
			{
				GenericLog.DoOutput(LOG_ERROR,"Could not store actor %u in Database ... hash not unique \n",ptr->ID);
				// This message would mean your CPU has too much alcohol ... if there was a non unique hash we would have found it
			}
			else
			{
				//We give a message
				GenericLog.DoOutput(LOG_MESSAGE,"Actor %u spawned with %i Health in cell %u  \n",ptr->ID,ptr->Status.Health,ptr->Status.CellID);
			}
		}
		// Health Magicka and Fatigue -....
		if(ptr->Status.Health != InPkgBuf->Health)
		{
				OutPkgBuf.Health = InPkgBuf->Health - ptr->Status.Health;
				ptr->Status.Health += OutPkgBuf.Health;
				// Handle dying here ... we should remove all mobs dying from the DB
				if(ptr->Status.Health < 0)
				{
					GenericLog.DoOutput(LOG_MESSAGE,"Actor %u died in cell %u \n",ptr->ID,ptr->Status.CellID);
					// Handle despawning here
				}
		}
		if(ptr->Status.Magika != InPkgBuf->Magika)
		{
				OutPkgBuf.Magika = InPkgBuf->Magika - ptr->Status.Magika;
				ptr->Status.Magika += OutPkgBuf.Magika;
		}
		if(ptr->Status.Fatigue != InPkgBuf->Fatigue)
		{
				OutPkgBuf.Fatigue = InPkgBuf->Fatigue - ptr->Status.Fatigue;
				ptr->Status.Fatigue += OutPkgBuf.Fatigue;
		}
		//Position
		OutPkgBuf.etypeID = OOPActorUpdate;
		OutPkgBuf.Flags = InPkgBuf->Flags;
		OutPkgBuf.refID = InPkgBuf->refID;
		OutPkgBuf.CellID = InPkgBuf->CellID;
		OutPkgBuf.fPosX = InPkgBuf->fPosX;
		OutPkgBuf.fPosY = InPkgBuf->fPosY;
		OutPkgBuf.fPosZ = InPkgBuf->fPosZ;
		OutPkgBuf.fRotX = InPkgBuf->fRotX;
		OutPkgBuf.fRotY = InPkgBuf->fRotY;
		OutPkgBuf.fRotZ = InPkgBuf->fRotZ;
	
		for(int cx=0;cx<MAXCLIENTS;cx++)
		{
			if (cx != LocalPlayer&&clients[cx])
				send(clients[cx],(char *)&OutPkgBuf,sizeof(OOPkgActorUpdate),0);
		}
	}
	
	return true;
}

bool OOPChat_Handler(char *Packet,short LocalPlayer)
{
	OOPkgChat *InPkgBuf = (OOPkgChat *)Packet;
	
	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		if (cx != LocalPlayer&&clients[cx])
			send(clients[cx],Packet,sizeof(OOPkgChat)+InPkgBuf->Length,0);
	}
	if (InPkgBuf->Length < 1024)
	{
		char MessageDest[1024] = "\0";
		for(int i=0; i<InPkgBuf->Length; i++)
		{
			MessageDest[i] = Packet[i+sizeof(OOPkgChat)];
		}
		GenericLog.DoOutput(LOG_MESSAGE,"Player %i: %s\n", LocalPlayer, MessageDest);
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
	OOPkgFullStatUpdate *InPkgBuf = (OOPkgFullStatUpdate *) Packet;
	OOPkgFullStatUpdate OutPkgBuf;

	if (InPkgBuf->refID < MAXCLIENTS)
	{
		OutPkgBuf.etypeID = OOPFullStatUpdate;
		OutPkgBuf.refID = InPkgBuf->refID;
		OutPkgBuf.Flags = InPkgBuf->Flags;
		//Are we setting initial values?
		if (InPkgBuf->Flags & 8)
		{
			OutPkgBuf.Health = InPkgBuf->Health;
			OutPkgBuf.Magika = InPkgBuf->Magika;
			OutPkgBuf.Fatigue = InPkgBuf->Fatigue;
			Players[InPkgBuf->refID].Health = InPkgBuf->Health;
			Players[InPkgBuf->refID].Magika = InPkgBuf->Magika;
			Players[InPkgBuf->refID].Fatigue = InPkgBuf->Fatigue;
			PlayersInitial[InPkgBuf->refID].Health = InPkgBuf->Health;
			PlayersInitial[InPkgBuf->refID].Magika = InPkgBuf->Magika;
			PlayersInitial[InPkgBuf->refID].Fatigue = InPkgBuf->Fatigue;
			GenericLog.DoOutput(LOG_MESSAGE,"Client %i full stats initialized\n", InPkgBuf->refID);
			GenericLog.DoOutput(LOG_MESSAGE," HP: %i\n", Players[InPkgBuf->refID].Health);
			GenericLog.DoOutput(LOG_MESSAGE," MP: %i\n", Players[InPkgBuf->refID].Magika);
		}else{
			OutPkgBuf.Health = InPkgBuf->Health - Players[InPkgBuf->refID].Health;
			OutPkgBuf.Magika = InPkgBuf->Magika - Players[InPkgBuf->refID].Magika;
			OutPkgBuf.Fatigue = InPkgBuf->Fatigue - Players[InPkgBuf->refID].Fatigue;
			//printf("Player HP: %i | Incoming HP: %i (FSU)\n", Players[InPkgBuf->refID].Health, InPkgBuf->Health);
			Players[InPkgBuf->refID].Health += OutPkgBuf.Health;
			Players[InPkgBuf->refID].Magika += OutPkgBuf.Magika;
			Players[InPkgBuf->refID].Fatigue += OutPkgBuf.Fatigue;
			if (OutPkgBuf.Health != 0)
				GenericLog.DoOutput(LOG_MESSAGE,"  Player %i HP is %i (change of %i)\n", LocalPlayer, InPkgBuf->refID, Players[InPkgBuf->refID].Health, OutPkgBuf.Health);
		}
		for(int cx=0;cx<MAXCLIENTS;cx++)
		{
			if (cx != LocalPlayer&&clients[cx])
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
		if(clients[cx])
			send(clients[cx],(char *)&OutPkgBuf,sizeof(OOPkgTimeUpdate),0);
	}
	return true;
}

bool OOPEquipped_Handler(char *Packet,short LocalPlayer)
{
	OOPkgEquipped *InPkgBuf = (OOPkgEquipped *) Packet;

	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		if (cx != LocalPlayer&&clients[cx])
			send(clients[cx],(char *)InPkgBuf,sizeof(OOPkgEquipped),0);
	}
	return true;
}
bool OOPName_Handler(char *Packet,short LocalPlayer)
{
	OOPkgName *InPkgBuf = (OOPkgName *) Packet;
	for(int cx=0;cx<MAXCLIENTS;cx++)
	{
		if (cx != LocalPlayer&&clients[cx])
			send(clients[cx],(char *)InPkgBuf,sizeof(OOPkgName),0);
	}
	GenericLog.DoOutput(LOG_MESSAGE,"Player %i changed name to %s",LocalPlayer,InPkgBuf->Name);
	return true;
}

