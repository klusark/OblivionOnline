/*

Copyright(c) 2007-2008   Julian Bangert aka masterfreek64 and Joseph Pearson
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
#include "MobSynch.h"
#include "OONetwork.h"
#include "UserInterface.h"
#include "ChatTalker.h"
//Prototypes
extern void RunScriptLine(const char *buf, bool IsTemp);
bool OOPWelcome_Handler(char *Packet);
bool OOPDisconnect_Handler(char *Packet);
bool OOPActorUpdate_Handler(char *Packet);
bool OOPChat_Handler(char *Packet);
bool OOPEvent_Handler(char *Packet);
bool OOPEventRegister_Handler(char *Packet);
bool OOPFullStatUpdate_Handler(char *Packet);
bool OOPTimeUpdate_Handler(char *Packet);
bool OOPEquipped_Handler(char *Packet);

inline void PacketErrorMsg(OOPacketType type, char *Name)
{
	_ERROR("A %64s package arrived marked with the Type ID %u - Ignoring \n",Name,type);
}
void PacketError(OOPacketType type, size_t len)
{
	//TODO: find indicators for Collisions : Disconnect has one , and Race as well
	switch (len)
	{
	case sizeof(OOPkgWelcome):
		PacketErrorMsg(type,"OOPkgWelcome");
		break;
	case sizeof(OOPkgEvent):
		PacketErrorMsg(type,"OOPkgEvent");
		break;
	case sizeof(OOPkgEventRegister):
		PacketErrorMsg(type,"OOPkgEventRegister");
		break;
	case sizeof(OOPkgActorUpdate):
		PacketErrorMsg(type,"OOPkgActorUpdate");
		break;
	case sizeof(OOPkgChat):
		PacketErrorMsg(type,"OOPkgEvent");
		break;
	case sizeof(OOPkgFullStatUpdate):
		PacketErrorMsg(type,"OOPkgFullStatUpdate");
		break;
	case sizeof(OOPkgDisconnect):
		PacketErrorMsg(type,"OOPkgDisconnect");
		break;
	case sizeof(OOPkgEquipped):
		PacketErrorMsg(type,"OOPkgEquipped");
		break;
	case sizeof(OOPkgName):
		PacketErrorMsg(type,"OOPkgName");
		break;
		/*
	case sizeof(OOPkgRace):
		PacketErrorMsg(type,"OOPkgRace");
		break;*/
	default:
		PacketErrorMsg(type,"bad or unknown");
		break;
	}
	return;
}
//----------------------------------
//--Begin Incoming packet handlers--
//----------------------------------

bool NetWelcome()
{
	BYTE data[]=
	{
		VERSION_SUPER,
		VERSION_MAJOR,
		VERSION_MINOR
	};
	outnet.AddChunk(0,true,GetMinChunkSize(PkgChunk::Version),PkgChunk::Version,data);
	outnet.Send();
	return false;
}

bool NetDisconnect()
{
	//NO Bye-Bye package
	return true;
}

bool NetActorUpdate(PlayerStatus *Player, int PlayerID, bool IsPC, bool Initial) 
{
	static PlayerStatus LastPlayer;
	OOPkgActorUpdate pkgBuf;
	DWORD tickBuf;
	tickBuf=GetTickCount();
	if((tickBuf - PacketTime[OOPActorUpdate]) > NET_POSUPDATE_RESEND)
	{
		if(memcmp(&LastPlayer,Player,sizeof(PlayerStatus))) //changed since last package
		{
			memcpy(&LastPlayer,Player,sizeof(PlayerStatus));
			pkgBuf.etypeID = OOPActorUpdate;
			pkgBuf.Flags = IsPC | 2 | (!((int)Player->bIsInInterior) << 2) | (Initial << 3);
			pkgBuf.fPosX = Player->PosX;
			pkgBuf.fPosY = Player->PosY;
			pkgBuf.fPosZ = Player->PosZ;
			pkgBuf.fRotX = Player->RotX;
			pkgBuf.fRotY = Player->RotY;
			pkgBuf.fRotZ = Player->RotZ;
			pkgBuf.CellID = Player->CellID;
			pkgBuf.refID = PlayerID;
			pkgBuf.Health = Player->Health;
			pkgBuf.Magika = Player->Magika;
			pkgBuf.Fatigue = Player->Fatigue;
			if(Player->InCombat)
				pkgBuf.Flags ^= 32;
			send(ServerSocket,(char *)&pkgBuf,sizeof(OOPkgActorUpdate),0);
			PacketTime[OOPActorUpdate] = tickBuf;
		}
	}
	return true;
}

bool NetEquipped(UINT32 slot, int PlayerID)
{
	
	return true;
}

bool NetChat(char *Message)
{
	
	return true;
}

bool NetFullStatUpdate(PlayerStatus *Player, int PlayerID, bool Initial, bool IsPC)
{
	static PlayerStatus StatLastPlayer;
	
	DWORD tickBuf;
	tickBuf=GetTickCount();
	
	// If we have a full update, don't send it too quickly
	if((tickBuf - PacketTime[OOPFullStatUpdate]) > NET_FULLSTATUPDATE_RESEND)
	{
		if(memcmp(&StatLastPlayer,Player,sizeof(PlayerStatus))) //changed since last package
		{
			OOPkgFullStatUpdate pkgBuf;
			pkgBuf.etypeID = OOPFullStatUpdate;
			if (IsPC)
				pkgBuf.Flags = 1 | 2;
			else
				pkgBuf.Flags = 2;
			if (Initial)
				pkgBuf.Flags = pkgBuf.Flags | 8;
			pkgBuf.Agility = Player->Agility;
			pkgBuf.Encumbrance = Player->Encumbrance;
			pkgBuf.Endurance = Player->Endurance;
			pkgBuf.Intelligence = Player->Intelligence;
			pkgBuf.Luck = Player->Luck;
			pkgBuf.Personality = Player->Personality;
			pkgBuf.Speed = Player->Speed;
			pkgBuf.Strength = Player->Strength;
			pkgBuf.Willpower = Player->Willpower;
			pkgBuf.Health = Player->Health;
			pkgBuf.Magika = Player->Magika;
			pkgBuf.Fatigue = Player->Fatigue;
			pkgBuf.TimeStamp = Player->Time;
			pkgBuf.refID = PlayerID;
			send(ServerSocket,(char *)&pkgBuf,sizeof(OOPkgFullStatUpdate),0);
			PacketTime[OOPFullStatUpdate] = tickBuf;
		}
	}
	return true;
}

//---------------------------
//--End Outgoing Handlers----
//---------------------------

bool NetReadBuffer(char *acReadBuffer, int Length)
{
	OOPacketType ePacketType = SelectType(acReadBuffer);


	switch (ePacketType)
	{
	case OOPWelcome:
		if(VERIFYPACKETSIZE(Length,OOPkgWelcome))
			OOPWelcome_Handler(acReadBuffer);		
		else
			PacketError(ePacketType,Length);
		break;
	case OOPDisconnect:
		if(VERIFYPACKETSIZE(Length,OOPkgWelcome))
			OOPDisconnect_Handler(acReadBuffer);
		else
			PacketError(ePacketType,Length);
		break;
	case OOPActorUpdate:
		if(VERIFYPACKETSIZE(Length,OOPkgWelcome))
			OOPActorUpdate_Handler(acReadBuffer);
		else
			PacketError(ePacketType,Length);
		break;
	case OOPChat:
			OOPChat_Handler(acReadBuffer);
	case OOPEvent:
		if(VERIFYPACKETSIZE(Length,OOPkgWelcome))
			OOPEvent_Handler(acReadBuffer);
		else
			PacketError(ePacketType,Length);
		break;
	
	case OOPEventRegister:		
		if(VERIFYPACKETSIZE(Length,OOPkgWelcome))
			OOPEventRegister_Handler(acReadBuffer);
		else
			PacketError(ePacketType,Length);
		break;
	case OOPFullStatUpdate:
		if(VERIFYPACKETSIZE(Length,OOPkgWelcome))
			OOPFullStatUpdate_Handler(acReadBuffer);
		else
			PacketError(ePacketType,Length);
		break;
	case OOPTimeUpdate:
		if(VERIFYPACKETSIZE(Length,OOPkgWelcome))
			OOPTimeUpdate_Handler(acReadBuffer);
		else
			PacketError(ePacketType,Length);		
		break;
	case OOPEquipped:
		if(VERIFYPACKETSIZE(Length,OOPkgWelcome))
			OOPEquipped_Handler(acReadBuffer);	
		else
			PacketError(ePacketType,Length);	
		break;
	default: 
		break;
	}
	return true;
}

//---------------------------
//--Begin Incoming Handlers--
//---------------------------

bool OOPWelcome_Handler(char *Packet)
{
	OOPkgWelcome * InPkgBuf = (OOPkgWelcome *)Packet;
	if(!(InPkgBuf->Flags & 2)) // Ignore Data Flag is not set
		{		
		sscanf(InPkgBuf->NickName, "Player%2d", &LocalPlayer);
		_MESSAGE("Received Player ID %u",LocalPlayer);
		Console_Print(InPkgBuf->NickName);
		_MESSAGE("Server: %s running OblivionOnline %u.%u", InPkgBuf->ServerName,HIBYTE(InPkgBuf->wVersion),LOBYTE(InPkgBuf->wVersion));

		PlayerConnected[LocalPlayer] = true;
		PlayersInitial[LocalPlayer].Health = (*g_thePlayer)->GetActorValue(8);
		PlayersInitial[LocalPlayer].Magika = (*g_thePlayer)->GetActorValue(9);
		PlayersInitial[LocalPlayer].Fatigue = (*g_thePlayer)->GetActorValue(10);
		Players[LocalPlayer].Health = (*g_thePlayer)->GetActorValue(8);
		Players[LocalPlayer].Magika = (*g_thePlayer)->GetActorValue(9);
		Players[LocalPlayer].Fatigue = (*g_thePlayer)->GetActorValue(10);
		//Tell server that we're ready to get init data from other clients
		OOPkgWelcome pkgBuf;
		pkgBuf.etypeID = OOPWelcome;
		pkgBuf.Flags = 1 ;
	//TODO : TUNE THAT HERE !!!!
		UpdateExternalInfo((*g_thePlayer)->GetName(),0,"Unknown",(*g_thePlayer)->classForm->GetName(),(*g_thePlayer)->parentCell->GetName(),InPkgBuf->ServerName,ServerIP,0);
		send(ServerSocket, (char *)&pkgBuf, sizeof(OOPkgWelcome), 0);
	}
// Here we hande the so -called "Mode Flags " from 4 (MC ) upwards.

// Flag 4 - Master Client ...
	if(InPkgBuf->Flags & 4 & 2) // Master Client
	{
		_MESSAGE("Received master client");
		MCMakeMC();
	}
	else if( InPkgBuf->Flags & 2)
	{
		_MESSAGE("Received passive client");
		MCMakePassive();
	}
// other flags below here 

	return true;
}

bool OOPDisconnect_Handler(char *Packet)
{
	OOPkgDisconnect * InPkgBuf = (OOPkgDisconnect *)Packet;
	if (InPkgBuf->PlayerID == LocalPlayer && InPkgBuf->Flags == 1)
	{
		TESFullName* first = (TESFullName*)Oblivion_DynamicCast(*g_thePlayer, 0, RTTI_TESForm, RTTI_TESFullName, 0);
		UpdateXFireInfo(first->name.m_data,0,"Unknown",(*g_thePlayer)->classForm->GetName(),(*g_thePlayer)->parentCell->GetName(),"Offline","",0);
		bIsConnected = false;
		Console_Print("You have been disconnected");
		for(int i=0; i<MAXCLIENTS; i++)
		{
			PlayerConnected[i] = false;
		}
		TotalPlayers = 1;
		TerminateThread(hRecvThread, 0);
		CloseHandle(hRecvThread);
		//TerminateThread(hPredictionEngine, 0);
		//CloseHandle(hPredictionEngine);
		closesocket(ServerSocket);
		ServerSocket = INVALID_SOCKET;
		WSACleanup();
	}else{
		PlayerConnected[InPkgBuf->PlayerID] = false;
		TotalPlayers--;
		Console_Print("Player %i disconnected", InPkgBuf->PlayerID);
		Players[InPkgBuf->PlayerID].PosX = 3200; // In testcave 1.  a location that is in every esm ...
		Players[InPkgBuf->PlayerID].PosY = 0;
		Players[InPkgBuf->PlayerID].PosZ = 0;
		Players[InPkgBuf->PlayerID].RotX = 0;
		Players[InPkgBuf->PlayerID].RotY = 0;
		Players[InPkgBuf->PlayerID].RotZ = 0;
		Players[InPkgBuf->PlayerID].CellID = 0x34E1D; // TestCave01
		Players[InPkgBuf->PlayerID].Health = 1;
		Players[InPkgBuf->PlayerID].bStatsInitialized = false;
		Players[InPkgBuf->PlayerID].bIsInInterior = true;
		Players[InPkgBuf->PlayerID].head = 0;
		Players[InPkgBuf->PlayerID].hair = 0;
		Players[InPkgBuf->PlayerID].upper_body = 0;
		Players[InPkgBuf->PlayerID].lower_body = 0;
		Players[InPkgBuf->PlayerID].hand = 0;
		Players[InPkgBuf->PlayerID].foot = 0;
		Players[InPkgBuf->PlayerID].right_ring = 0;
		Players[InPkgBuf->PlayerID].left_ring = 0;
		Players[InPkgBuf->PlayerID].amulet = 0;
		Players[InPkgBuf->PlayerID].shield = 0;
		Players[InPkgBuf->PlayerID].tail = 0;
		Players[InPkgBuf->PlayerID].weapon = 0;
		Players[InPkgBuf->PlayerID].ammo = 0;
		Players[InPkgBuf->PlayerID].robes = 0;
	}
	return true;
}

bool OOPActorUpdate_Handler(char *Packet)
{
	
	OOPkgActorUpdate * InPkgBuf = (OOPkgActorUpdate *) Packet;
	
	if(InPkgBuf->Flags & 1)
	{
	if (InPkgBuf->refID < MAXCLIENTS)
	{
		if(!PlayerConnected[InPkgBuf->refID])
		{
			TotalPlayers++;
			PlayerConnected[InPkgBuf->refID] = true;
			Console_Print("Player %i connected", InPkgBuf->refID);
		}
		Players[InPkgBuf->refID].InCombat = InPkgBuf->Flags & 32; 
		Players[InPkgBuf->refID].PosX = InPkgBuf->fPosX;
		Players[InPkgBuf->refID].PosY = InPkgBuf->fPosY;
		Players[InPkgBuf->refID].PosZ = InPkgBuf->fPosZ;
		Players[InPkgBuf->refID].RotX = InPkgBuf->fRotX;
		Players[InPkgBuf->refID].RotY = InPkgBuf->fRotY;
		Players[InPkgBuf->refID].RotZ = InPkgBuf->fRotZ;
		UInt32 oldCell = Players[InPkgBuf->refID].CellID;
		Players[InPkgBuf->refID].CellID = InPkgBuf->CellID;
		

		//Is this a set of initial data?
		if (InPkgBuf->Flags & 8)
		{
			if(!Players[InPkgBuf->refID].bStatsInitialized)
			{
				Players[InPkgBuf->refID].Health = InPkgBuf->Health;
				Players[InPkgBuf->refID].Magika = InPkgBuf->Magika;
				Players[InPkgBuf->refID].Fatigue = InPkgBuf->Fatigue;
				Console_Print("Player %i basic stats initialized", (int)InPkgBuf->refID);
				Players[InPkgBuf->refID].bStatsInitialized = true;
			}else{
				Console_Print("Player %i reinialized basic stats. Error perhaps?", (int)InPkgBuf->refID);
			}
		}else{
			Players[InPkgBuf->refID].Health += InPkgBuf->Health;
			Players[InPkgBuf->refID].Magika += InPkgBuf->Magika;
			Players[InPkgBuf->refID].Fatigue += InPkgBuf->Fatigue;
			// If we know the mem location of the NPC, mod the stats
			if (InPkgBuf->refID == LocalPlayer)
			{
				if (Players[InPkgBuf->refID].Health >= 0)
					(*g_thePlayer)->ModActorBaseValue(8, InPkgBuf->Health, 0);
				else
					Players[InPkgBuf->refID].Health = 0;
				if (Players[InPkgBuf->refID].Magika >= 0)
					(*g_thePlayer)->ModActorBaseValue(9, InPkgBuf->Magika, 0);
				else
					Players[InPkgBuf->refID].Magika = 0;
				if (Players[InPkgBuf->refID].Fatigue >= 0)
					(*g_thePlayer)->ModActorBaseValue(10, InPkgBuf->Fatigue, 0);
				else
					Players[InPkgBuf->refID].Fatigue = 0;

				if(InPkgBuf->Flags & 16) // We have to overwrite the local player - aka we are being moved
				{
					// We change the cell
					(*g_thePlayer)->ChangeCell((TESObjectCELL *)LookupFormByID(InPkgBuf->refID));
					char Script [32];
					//optimise this !!!
					sprintf(Script,"player.setpos x , %7.6f",InPkgBuf->fPosX);
					RunScriptLine(Script,true);
					sprintf(Script,"player.setpos y , %7.6f",InPkgBuf->fPosY);
					RunScriptLine(Script,true);
					sprintf(Script,"player.setpos z , %7.6f",InPkgBuf->fPosZ);
					RunScriptLine(Script,true);
					sprintf(Script,"player.setrot z , %7.6f",InPkgBuf->fRotZ);
					RunScriptLine(Script,true);
				}					
			}else{
				if (PlayerActorList[InPkgBuf->refID])
				{
					Actor *ActorBuf = (Actor*)PlayerActorList[InPkgBuf->refID];
					if (Players[InPkgBuf->refID].Health >= 0)
						ActorBuf->ModActorBaseValue(8, InPkgBuf->Health, 0);
					else
						Players[InPkgBuf->refID].Health = 0;
					if (Players[InPkgBuf->refID].Magika >= 0)
						ActorBuf->ModActorBaseValue(9, InPkgBuf->Magika, 0);
					else
						Players[InPkgBuf->refID].Magika = 0;
					if (Players[InPkgBuf->refID].Fatigue >= 0)
						ActorBuf->ModActorBaseValue(10, InPkgBuf->Fatigue, 0);
					else
						Players[InPkgBuf->refID].Fatigue = 0;
				}
			}
		}
		if (InPkgBuf->Flags & 4) //Is in an exterior?
		{
			Players[InPkgBuf->refID].bIsInInterior = false;
		}else{
			Players[InPkgBuf->refID].bIsInInterior = true;
		}

		
	}
	}
	else //Mob
	{
		NetHandleMobUpdate(InPkgBuf);
	}
	return true;
}
#define max(a,b)            (((a) > (b)) ? (a) : (b))
bool OOPChat_Handler(char *Packet)
{
	OOPkgChat * InPkgBuf = (OOPkgChat *)Packet;

	char MessageDest[1024] = "\0";
	for(int i=0; i<max(InPkgBuf->Length,1024); i++)
	{
		MessageDest[i] = Packet[i+sizeof(OOPkgChat)];
	}
	MessageDest[InPkgBuf->Length] = '\0';
	char chatScript[1024];
	sprintf(chatScript, "Message \"Player %i: %s\"", InPkgBuf->refID, MessageDest);
	Console_Print("Player %i: %s", InPkgBuf->refID, MessageDest);
	RunScriptLine(chatScript, true);
	std::string Sender = "Player "+InPkgBuf->refID;
	RegisterChatMessage(MessageDest);
	return true;
}

bool OOPEvent_Handler(char *Packet)
{
	return true;
}

bool OOPEventRegister_Handler(char *Packet)
{
	return true;
}

bool OOPEquipped_Handler(char *Packet)
{
	OOPkgEquipped * InPkgBuf = (OOPkgEquipped *)Packet;
	if ((InPkgBuf->refID < MAXCLIENTS) && (InPkgBuf->refID != LocalPlayer))
	{
		Players[InPkgBuf->refID].head = InPkgBuf->head;
		Players[InPkgBuf->refID].hair = InPkgBuf->hair;
		Players[InPkgBuf->refID].upper_body = InPkgBuf->upper_body;
		Players[InPkgBuf->refID].lower_body = InPkgBuf->lower_body;
		Players[InPkgBuf->refID].hand = InPkgBuf->hand;
		Players[InPkgBuf->refID].foot = InPkgBuf->foot;
		Players[InPkgBuf->refID].right_ring = InPkgBuf->right_ring;
		Players[InPkgBuf->refID].left_ring = InPkgBuf->left_ring;
		Players[InPkgBuf->refID].amulet = InPkgBuf->amulet;
		Players[InPkgBuf->refID].shield = InPkgBuf->shield;
		Players[InPkgBuf->refID].tail = InPkgBuf->tail;
		Players[InPkgBuf->refID].weapon = InPkgBuf->weapon;
		Players[InPkgBuf->refID].ammo = InPkgBuf->ammo;
		Players[InPkgBuf->refID].robes = InPkgBuf->robes;
	}
	return true;
}

bool OOPFullStatUpdate_Handler(char *Packet)
{
	OOPkgFullStatUpdate *InPkgBuf = (OOPkgFullStatUpdate *)Packet;
	if (InPkgBuf->refID < MAXCLIENTS)
	{
		if (InPkgBuf->Flags & 8)
		{
			Players[InPkgBuf->refID].Agility = InPkgBuf->Agility;
			Players[InPkgBuf->refID].Encumbrance = InPkgBuf->Encumbrance;
			Players[InPkgBuf->refID].Endurance = InPkgBuf->Endurance;
			Players[InPkgBuf->refID].Intelligence = InPkgBuf->Intelligence;
			Players[InPkgBuf->refID].Luck = InPkgBuf->Luck;
			Players[InPkgBuf->refID].Personality = InPkgBuf->Personality;
			Players[InPkgBuf->refID].Speed = InPkgBuf->Speed;
			Players[InPkgBuf->refID].Strength = InPkgBuf->Strength;
			Players[InPkgBuf->refID].Willpower = InPkgBuf->Willpower;
			Players[InPkgBuf->refID].Health = InPkgBuf->Health;
			Players[InPkgBuf->refID].Magika = InPkgBuf->Magika;
			Players[InPkgBuf->refID].Fatigue = InPkgBuf->Fatigue;
			Players[InPkgBuf->refID].bStatsInitialized = true;
			Console_Print("Player %i full stats initialized", InPkgBuf->refID);
		}else{
			Players[InPkgBuf->refID].Agility += InPkgBuf->Agility;
			Players[InPkgBuf->refID].Encumbrance += InPkgBuf->Encumbrance;
			Players[InPkgBuf->refID].Endurance += InPkgBuf->Endurance;
			Players[InPkgBuf->refID].Intelligence += InPkgBuf->Intelligence;
			Players[InPkgBuf->refID].Luck += InPkgBuf->Luck;
			Players[InPkgBuf->refID].Personality += InPkgBuf->Personality;
			Players[InPkgBuf->refID].Speed += InPkgBuf->Speed;
			Players[InPkgBuf->refID].Strength += InPkgBuf->Strength;
			Players[InPkgBuf->refID].Willpower += InPkgBuf->Willpower;
			Players[InPkgBuf->refID].Health += InPkgBuf->Health;
			Players[InPkgBuf->refID].Magika += InPkgBuf->Magika;
			Players[InPkgBuf->refID].Fatigue += InPkgBuf->Fatigue;
			// If we know the mem location of the NPC, mod the stats
			if (InPkgBuf->refID == LocalPlayer)
			{
				(*g_thePlayer)->ModActorBaseValue(0, InPkgBuf->Strength, 0);
				(*g_thePlayer)->ModActorBaseValue(1, InPkgBuf->Intelligence, 0);
				(*g_thePlayer)->ModActorBaseValue(2, InPkgBuf->Willpower, 0);
				(*g_thePlayer)->ModActorBaseValue(3, InPkgBuf->Agility, 0);
				(*g_thePlayer)->ModActorBaseValue(4, InPkgBuf->Speed, 0);
				(*g_thePlayer)->ModActorBaseValue(5, InPkgBuf->Endurance, 0);
				(*g_thePlayer)->ModActorBaseValue(6, InPkgBuf->Personality, 0);
				(*g_thePlayer)->ModActorBaseValue(7, InPkgBuf->Luck, 0);
				(*g_thePlayer)->ModActorBaseValue(8, InPkgBuf->Health, 0);
				(*g_thePlayer)->ModActorBaseValue(9, InPkgBuf->Magika, 0);
				(*g_thePlayer)->ModActorBaseValue(10, InPkgBuf->Fatigue, 0);
				(*g_thePlayer)->ModActorBaseValue(11, InPkgBuf->Encumbrance, 0);
			}else{
				if (PlayerActorList[InPkgBuf->refID])
				{
					Actor *ActorBuf = (Actor*)PlayerActorList[InPkgBuf->refID];
					ActorBuf->ModActorBaseValue(0, InPkgBuf->Strength, 0);
					ActorBuf->ModActorBaseValue(1, InPkgBuf->Intelligence, 0);
					ActorBuf->ModActorBaseValue(2, InPkgBuf->Willpower, 0);
					ActorBuf->ModActorBaseValue(3, InPkgBuf->Agility, 0);
					ActorBuf->ModActorBaseValue(4, InPkgBuf->Speed, 0);
					ActorBuf->ModActorBaseValue(5, InPkgBuf->Endurance, 0);
					ActorBuf->ModActorBaseValue(6, InPkgBuf->Personality, 0);
					ActorBuf->ModActorBaseValue(7, InPkgBuf->Luck, 0);
					ActorBuf->ModActorBaseValue(8, InPkgBuf->Health, 0);
					ActorBuf->ModActorBaseValue(9, InPkgBuf->Magika, 0);
					ActorBuf->ModActorBaseValue(10, InPkgBuf->Fatigue, 0);
					ActorBuf->ModActorBaseValue(11, InPkgBuf->Encumbrance, 0);
				}
			}
		}
	}
	return true;
}

bool OOPTimeUpdate_Handler(char *Packet)
{
	OOPkgTimeUpdate * InPkgBuf = (OOPkgTimeUpdate *)Packet;
	Players[LocalPlayer].Time = InPkgBuf->Hours + (float)InPkgBuf->Minutes / 60.0 + (float)InPkgBuf->Seconds / 3600.0;
	return true;
}
bool OOPName_Handler(char *Packet)
{
	OOPkgName * InPkgBuf = (OOPkgName *)Packet;
	if(InPkgBuf->refID == LocalPlayer)
	{
		(*g_thePlayer)->SetName(InPkgBuf->Name);
	}
	else if(InPkgBuf->Flags & 1)
	{
		//retrieve the spawn ID
		UINT32 ActorID = SpawnID[GetSpawnIDFromPlayerID(InPkgBuf->refID)];
		{
			TESForm *actor = LookupFormByID(ActorID);
			if(actor)
				actor->SetName(InPkgBuf->Name);
		}
	}
	else
	{
		TESForm *actor = LookupFormByID(InPkgBuf->refID);
		if(actor)
			actor->SetName(InPkgBuf->Name);
	}
	return true;
}
