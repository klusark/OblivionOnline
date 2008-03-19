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

