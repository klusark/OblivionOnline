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

#include "OONetwork.h"

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
bool OOPModOffsetList_Handler(char *Packet);

//----------------------------------
//--Begin Incoming packet handlers--
//----------------------------------

bool NetWelcome()
{
	OOPkgWelcome pkgBuf;
	pkgBuf.etypeID = OOPWelcome;
	pkgBuf.Flags = 0;
	pkgBuf.PlayerID = 0;
	pkgBuf.wVersion = MAKEWORD(MAIN_VERSION,SUB_VERSION);
	pkgBuf.guidOblivionOnline = gcOOGUID;
	send(ServerSocket,(char *)&pkgBuf,sizeof(OOPkgWelcome),0);
	return true;
}

bool NetDisconnect()
{
	OOPkgDisconnect pkgBuf;
	pkgBuf.etypeID = OOPDisconnect;
	pkgBuf.Flags = 0;
	pkgBuf.PlayerID = LocalPlayer;
	send(ServerSocket,(char *)&pkgBuf,sizeof(OOPkgDisconnect),0);
	return true;
}

bool NetActorUpdate(PlayerStatus *Player, int PlayerID, bool Initial)
{
	static PlayerStatus LastPlayer;
	
	OOPkgActorUpdate pkgBuf;
	DWORD tickBuf;
	tickBuf=GetTickCount();
	if((tickBuf - PacketTime[OOPActorUpdate]) > NET_POSUPDATE_RESEND) //just send it every 30 ms 
	{
		if(memcmp(&LastPlayer,Player,sizeof(PlayerStatus))) //changed since last package
		{
			memcpy(&LastPlayer,Player,sizeof(PlayerStatus));
			pkgBuf.etypeID = OOPActorUpdate;
			if(Player->bIsInInterior)
				pkgBuf.Flags = 1 | 2;
			else
				pkgBuf.Flags = 1 | 2 | 4;
			if(Initial)
				pkgBuf.Flags = pkgBuf.Flags | 8;
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
			send(ServerSocket,(char *)&pkgBuf,sizeof(OOPkgActorUpdate),0);
			PacketTime[OOPActorUpdate] = tickBuf;
		}
	}
	return true;
}

bool NetEquipped(PlayerStatus *Player, int PlayerID, bool Initial)
{
	static PlayerStatus EquipLastPlayer;
	
	OOPkgEquipped pkgBuf;
	DWORD tickBuf;
	tickBuf=GetTickCount();
	if((tickBuf - PacketTime[OOPEquipped]) > NET_EQUIPUPDATE_RESEND)
	{
		if(memcmp(&EquipLastPlayer,Player,sizeof(PlayerStatus))) //changed since last package
		{
			pkgBuf.etypeID = OOPEquipped;
			pkgBuf.refID = PlayerID;
			pkgBuf.Flags = 0;
			if (Initial)
				pkgBuf.Flags = pkgBuf.Flags | 8;
			pkgBuf.head = Player->head;
			pkgBuf.hair=Player->hair;
			pkgBuf.upper_body=Player->upper_body;
			pkgBuf.lower_body=Player->lower_body;
			pkgBuf.hand=Player->hand;
			pkgBuf.foot=Player->foot;
			pkgBuf.right_ring=Player->right_ring;
			pkgBuf.left_ring=Player->left_ring;
			pkgBuf.amulet=Player->amulet;
			pkgBuf.shield=Player->shield;
			pkgBuf.tail=Player->tail;
			pkgBuf.weapon=Player->weapon;
			pkgBuf.ammo=Player->ammo;
			send(ServerSocket,(char *)&pkgBuf,sizeof(OOPkgEquipped),0);
			PacketTime[OOPEquipped] = tickBuf;
		}
	}
	return true;
}

bool NetChat(char *Message)
{
	OOPkgChat pkgBuf;
	char *SendBuf;
	void *MessageDest;
	pkgBuf.etypeID = OOPChat;
	pkgBuf.Length = strlen(Message);
	pkgBuf.Flags = 0;
	SendBuf = (char *)malloc(sizeof(OOPkgChat)+pkgBuf.Length);
	memcpy(SendBuf,&pkgBuf,sizeof(OOPkgChat));
	MessageDest=(SendBuf+sizeof(OOPkgChat));
	memcpy(MessageDest,Message,pkgBuf.Length);
	send(ServerSocket,SendBuf,sizeof(OOPkgChat)+pkgBuf.Length,0);
	free(SendBuf);
	return true;
}

bool NetFullStatUpdate(PlayerStatus *Player, int PlayerID, bool Initial)
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
			pkgBuf.Flags = 1 | 2;
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

bool NetSendModList(void)
{
	char Mods[255];
	char *SendBuf;
	void *ListDest;
	OOPkgModOffsetList pkgBuf;
	pkgBuf.etypeID = OOPModOffsetList;
	pkgBuf.Flags = 0;
	pkgBuf.refID = LocalPlayer;

	//Modlist is hardcoded for now, will read a file later
	pkgBuf.NumOfMods = 1;
	Mods[0] = 0;
	Mods[1] = OOModOffset;

	SendBuf = (char *)malloc(sizeof(OOPkgModOffsetList)+pkgBuf.NumOfMods);
	memcpy(SendBuf,&pkgBuf,sizeof(OOPkgModOffsetList));
	ListDest=(SendBuf+sizeof(OOPkgModOffsetList));
	memcpy(ListDest,Mods,pkgBuf.NumOfMods);
	send(ServerSocket,SendBuf,sizeof(OOPkgModOffsetList)+pkgBuf.NumOfMods,0);
	free(SendBuf);
	return true;
}

//---------------------------
//--End Outgoing Handlers----
//---------------------------

bool NetReadBuffer(char *acReadBuffer)
{
	OOPacketType ePacketType = SelectType(acReadBuffer);

	switch (ePacketType)
	{
	case OOPWelcome:
		OOPWelcome_Handler(acReadBuffer);
		break;
	case OOPDisconnect:
		OOPDisconnect_Handler(acReadBuffer);
		break;
	case OOPActorUpdate:
		OOPActorUpdate_Handler(acReadBuffer);
		break;
	case OOPChat:
		OOPChat_Handler(acReadBuffer);
		break;
	case OOPEvent:
		OOPEvent_Handler(acReadBuffer);
		break;
	case OOPEventRegister:
		OOPEventRegister_Handler(acReadBuffer);
		break;
	case OOPFullStatUpdate:
		OOPFullStatUpdate_Handler(acReadBuffer);
		break;
	case OOPTimeUpdate:
		OOPTimeUpdate_Handler(acReadBuffer);
		break;
	case OOPEquipped:
		OOPEquipped_Handler(acReadBuffer);
		break;
	case OOPModOffsetList:
		OOPModOffsetList_Handler(acReadBuffer);
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
	OOPkgWelcome InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgWelcome));
	sscanf(InPkgBuf.NickName, "Player%2d", &LocalPlayer);
	_MESSAGE("Received Player ID %u",LocalPlayer);
	Console_Print(InPkgBuf.NickName);
	PlayerConnected[LocalPlayer] = true;
	return true;
}

bool OOPDisconnect_Handler(char *Packet)
{
	OOPkgDisconnect InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgDisconnect));
	PlayerConnected[InPkgBuf.PlayerID] = false;
	TotalPlayers--;
	char DCText[32];
	sprintf(DCText, "Player %i disconnected", InPkgBuf.PlayerID);
	Console_Print(DCText);
	return true;
}

bool OOPActorUpdate_Handler(char *Packet)
{
	OOPkgActorUpdate InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgActorUpdate));
	if (InPkgBuf.refID < MAXCLIENTS)
	{
		if(!PlayerConnected[InPkgBuf.refID])
		{
			TotalPlayers++;
			PlayerConnected[InPkgBuf.refID] = true;
			Console_Print("Player %i connected", InPkgBuf.refID);
		}

		Players[InPkgBuf.refID].PosX = InPkgBuf.fPosX;
		Players[InPkgBuf.refID].PosY = InPkgBuf.fPosY;
		Players[InPkgBuf.refID].PosZ = InPkgBuf.fPosZ;
		Players[InPkgBuf.refID].RotX = InPkgBuf.fRotX;
		Players[InPkgBuf.refID].RotY = InPkgBuf.fRotY;
		Players[InPkgBuf.refID].RotZ = InPkgBuf.fRotZ;
		Players[InPkgBuf.refID].CellID = InPkgBuf.CellID;
		if (InPkgBuf.Flags & 8)
		{
			Players[InPkgBuf.refID].Health = InPkgBuf.Health;
			Players[InPkgBuf.refID].Magika = InPkgBuf.Magika;
			Players[InPkgBuf.refID].Fatigue = InPkgBuf.Fatigue;
			Console_Print("Player %i basic stats initialized", InPkgBuf.refID);
			Players[InPkgBuf.refID].bStatsInitialized = true;
		}else{
			Players[InPkgBuf.refID].Health += InPkgBuf.Health;
			Players[InPkgBuf.refID].Magika += InPkgBuf.Magika;
			Players[InPkgBuf.refID].Fatigue += InPkgBuf.Fatigue;
		}
		if (InPkgBuf.Flags & 4) //Is in an exterior?
			Players[InPkgBuf.refID].bIsInInterior = false;
		else
			Players[InPkgBuf.refID].bIsInInterior = true;
	}
	return true;
}

bool OOPChat_Handler(char *Packet)
{
	OOPkgChat InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgChat));
	char MessageDest[1024] = "\0";
	for(int i=0; i<InPkgBuf.Length; i++)
	{
		MessageDest[i] = Packet[i+sizeof(OOPkgChat)];
	}
	MessageDest[InPkgBuf.Length] = '\0';
	char chatScript[1024];
	sprintf(chatScript, "Message \"%s\", %s", MessageDest);
	RunScriptLine(chatScript, true);
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
	OOPkgEquipped InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgEquipped));
	if ((InPkgBuf.refID < MAXCLIENTS) && (InPkgBuf.refID != LocalPlayer))
	{
		Players[InPkgBuf.refID].head = InPkgBuf.head;
		Players[InPkgBuf.refID].hair = InPkgBuf.hair;
		Players[InPkgBuf.refID].upper_body = InPkgBuf.upper_body;
		Players[InPkgBuf.refID].lower_body = InPkgBuf.lower_body;
		Players[InPkgBuf.refID].hand = InPkgBuf.hand;
		Players[InPkgBuf.refID].foot = InPkgBuf.foot;
		Players[InPkgBuf.refID].right_ring = InPkgBuf.right_ring;
		Players[InPkgBuf.refID].left_ring = InPkgBuf.left_ring;
		Players[InPkgBuf.refID].amulet = InPkgBuf.amulet;
		Players[InPkgBuf.refID].shield = InPkgBuf.shield;
		Players[InPkgBuf.refID].tail = InPkgBuf.tail;
		Players[InPkgBuf.refID].weapon = InPkgBuf.weapon;
		Players[InPkgBuf.refID].ammo = InPkgBuf.ammo;
	}
	return true;
}

bool OOPFullStatUpdate_Handler(char *Packet)
{
	OOPkgFullStatUpdate InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgFullStatUpdate));
	if (InPkgBuf.refID < MAXCLIENTS)
	{
		if (InPkgBuf.Flags & 8)
		{
			Players[InPkgBuf.refID].Agility = InPkgBuf.Agility;
			Players[InPkgBuf.refID].Encumbrance = InPkgBuf.Encumbrance;
			Players[InPkgBuf.refID].Endurance = InPkgBuf.Endurance;
			Players[InPkgBuf.refID].Intelligence = InPkgBuf.Intelligence;
			Players[InPkgBuf.refID].Luck = InPkgBuf.Luck;
			Players[InPkgBuf.refID].Personality = InPkgBuf.Personality;
			Players[InPkgBuf.refID].Speed = InPkgBuf.Speed;
			Players[InPkgBuf.refID].Strength = InPkgBuf.Strength;
			Players[InPkgBuf.refID].Willpower = InPkgBuf.Willpower;
			Players[InPkgBuf.refID].Health = InPkgBuf.Health;
			Players[InPkgBuf.refID].Magika = InPkgBuf.Magika;
			Players[InPkgBuf.refID].Fatigue = InPkgBuf.Fatigue;
			Players[InPkgBuf.refID].bStatsInitialized = true;
		}else{
			Players[InPkgBuf.refID].Agility += InPkgBuf.Agility;
			Players[InPkgBuf.refID].Encumbrance += InPkgBuf.Encumbrance;
			Players[InPkgBuf.refID].Endurance += InPkgBuf.Endurance;
			Players[InPkgBuf.refID].Intelligence += InPkgBuf.Intelligence;
			Players[InPkgBuf.refID].Luck += InPkgBuf.Luck;
			Players[InPkgBuf.refID].Personality += InPkgBuf.Personality;
			Players[InPkgBuf.refID].Speed += InPkgBuf.Speed;
			Players[InPkgBuf.refID].Strength += InPkgBuf.Strength;
			Players[InPkgBuf.refID].Willpower += InPkgBuf.Willpower;
			Players[InPkgBuf.refID].Health += InPkgBuf.Health;
			Players[InPkgBuf.refID].Magika += InPkgBuf.Magika;
			Players[InPkgBuf.refID].Fatigue += InPkgBuf.Fatigue;
		}
	}
	return true;
}

bool OOPTimeUpdate_Handler(char *Packet)
{
	OOPkgTimeUpdate InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgTimeUpdate));
	Players[LocalPlayer].Time = InPkgBuf.Hours + (float)InPkgBuf.Minutes / 60.0 + (float)InPkgBuf.Seconds / 3600.0;
	return true;
}

bool OOPModOffsetList_Handler(char *Packet)
{
	OOPkgModOffsetList InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgModOffsetList));
	int PlayerList[MAXCLIENTS];
	if (InPkgBuf.NumOfMods >= 255)
		return false;
	for(int i=0; i<InPkgBuf.NumOfMods; i++)
	{
		ModList[InPkgBuf.refID][i] = Packet[i+sizeof(OOPkgModOffsetList)];
	}
	return true;
}