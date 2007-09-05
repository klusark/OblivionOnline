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
bool OOPActorUpdate_Handler(char *Packet);
bool OOPChat_Handler(char *Packet);
bool OOPEvent_Handler(char *Packet);
bool OOPEventRegister_Handler(char *Packet);
bool OOPFullStatUpdate_Handler(char *Packet);
bool OOPTimeUpdate_Handler(char *Packet);

bool NetWelcome()
{
	OOPkgWelcome pkgBuf;
	char *SendBuf;
	pkgBuf.etypeID = OOPWelcome;
	pkgBuf.Flags = 0;
	pkgBuf.PlayerID = 0;
	pkgBuf.wVersion = MAKEWORD(MAIN_VERSION,SUB_VERSION);
	pkgBuf.guidOblivionOnline = gcOOGUID;
	SendBuf = (char *)malloc(sizeof(OOPkgWelcome));
	memcpy(SendBuf,&pkgBuf,sizeof(OOPkgWelcome));
	send(ServerSocket,SendBuf,sizeof(OOPkgWelcome),0);
	free(SendBuf);
	return true;
}

bool NetActorUpdate(PlayerStatus *Player,int PlayerID)
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
			{
				pkgBuf.Flags = 1 | 2;
			}else{
				pkgBuf.Flags = 1 | 2 | 4;
			}
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
	send(ServerSocket,SendBuf,sizeof(OOPkgChat)+pkgBuf.Length+1,0);
	free(SendBuf);
	return true;
}

bool NetFullStatUpdate(PlayerStatus *Player, int PlayerID)
{
	DWORD tickBuf;
	tickBuf=GetTickCount();
	
	// If we have a full update, don't send it too quickly
	if((tickBuf - PacketTime[OOPFullStatUpdate]) > NET_FULLSTATUPDATE_RESEND)
	{
		OOPkgFullStatUpdate pkgBuf;
		pkgBuf.etypeID = OOPFullStatUpdate;
		pkgBuf.Flags = 1 | 2;
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
	return true;
}

bool OOPActorUpdate_Handler(char *Packet)
{
	OOPkgActorUpdate InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgActorUpdate));
	if ((InPkgBuf.refID < MAXCLIENTS) && (InPkgBuf.refID != LocalPlayer))
	{
		OtherPlayer = InPkgBuf.refID;
		Players[OtherPlayer].PosX = InPkgBuf.fPosX;
		Players[OtherPlayer].PosY = InPkgBuf.fPosY;
		Players[OtherPlayer].PosZ = InPkgBuf.fPosZ;
		Players[OtherPlayer].RotX = InPkgBuf.fRotX;
		Players[OtherPlayer].RotY = InPkgBuf.fRotY;
		Players[OtherPlayer].RotZ = InPkgBuf.fRotZ;
		Players[OtherPlayer].CellID = InPkgBuf.CellID;
		Players[OtherPlayer].Health += InPkgBuf.Health;
		Players[OtherPlayer].Magika += InPkgBuf.Magika;
		Players[OtherPlayer].Fatigue += InPkgBuf.Fatigue;
		if (InPkgBuf.Flags & 4) //Is in an exterior?
		{
			Players[OtherPlayer].bIsInInterior = false;
			Console_Print("Other player is in an exterior");
		}else{
			Players[OtherPlayer].bIsInInterior = true;
			Console_Print("Other player is in an interior");
		}
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

bool OOPFullStatUpdate_Handler(char *Packet)
{
	OOPkgFullStatUpdate InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgFullStatUpdate));
	if ((InPkgBuf.refID < MAXCLIENTS) && (InPkgBuf.refID != LocalPlayer))
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