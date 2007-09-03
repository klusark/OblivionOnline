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

#include "main.h"
#include "OONetwork.h"
#include "OOPacketHandler.h"

bool NetPlayerPosUpdate(PlayerStatus *Player,int PlayerID)
{
	OOPkgPosUpdate pkgBuf;
	char *SendBuf;
	pkgBuf.etypeID = OOPPosUpdate;
	pkgBuf.Flags = 1 | 2;
	pkgBuf.fPosX = Player->PosX;
	pkgBuf.fPosY = Player->PosY;
	pkgBuf.fPosZ = Player->PosZ;
	pkgBuf.fRotX = Player->RotX;
	pkgBuf.fRotY = Player->RotY;
	pkgBuf.fRotZ = Player->RotZ;
	pkgBuf.refID = PlayerID;
	SendBuf = (char *)malloc(sizeof(OOPkgPosUpdate));
	memcpy(SendBuf,&pkgBuf,sizeof(OOPkgPosUpdate));
	while(true)
	{
		if (!bSendBusy)
		{
			bSendBusy = true;
			send(ServerSocket,SendBuf,sizeof(OOPkgPosUpdate),0);
			bSendBusy = false;
			break;
		}
	}
	free(SendBuf);
	return true;
}

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
	while(true)
	{
		if (!bSendBusy)
		{
			bSendBusy = true;
			send(ServerSocket,SendBuf,sizeof(OOPkgWelcome),0);
			bSendBusy = false;
			break;
		}
	}
	free(SendBuf);
	return true;
}

bool NetPlayerZone(PlayerStatus *Player,char *ZoneName,int PlayerID, bool bIsInterior)
{
	OOPkgZone pkgBuf;
	char *SendBuf;
	pkgBuf.etypeID = OOPZone;
	if (!bIsInterior)
	{
		pkgBuf.Flags = 1 | 2 | 4; //Player and Actor | Exterior
	}else{
		pkgBuf.Flags = 1 | 2; //Player and Actor | Interior
	}
	pkgBuf.fPosX = Player->PosX;
	pkgBuf.fPosY = Player->PosY;
	pkgBuf.fPosZ = Player->PosZ;
	pkgBuf.fRotX = Player->RotX;
	pkgBuf.fRotY = Player->RotY;
	pkgBuf.fRotZ = Player->RotZ;
	strcpy(pkgBuf.ZoneName, ZoneName);
	pkgBuf.cellID = Player->CellID;
	pkgBuf.refID = PlayerID;
	SendBuf = (char *)malloc(sizeof(OOPkgZone));
	memcpy(SendBuf,&pkgBuf,sizeof(OOPkgZone));
	while(true)
	{
		if (!bSendBusy)
		{
			bSendBusy = true;
			send(ServerSocket,SendBuf,sizeof(OOPkgZone),0);
			bSendBusy = false;
			break;
		}
	}
	free(SendBuf);
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
	while(true)
	{
		if (!bSendBusy)
		{
			bSendBusy = true;
			send(ServerSocket,SendBuf,sizeof(OOPkgChat)+pkgBuf.Length+1,0);
			bSendBusy = false;
			break;
		}
	}
	free(SendBuf);
	return true;
}

bool NetStatUpdate(PlayerStatus *Player, int PlayerID, bool FullUpdate)
{
	char *SendBuf;
	if (FullUpdate)
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
		SendBuf = (char *)malloc(sizeof(OOPkgFullStatUpdate));
		memcpy(SendBuf,&pkgBuf,sizeof(OOPkgFullStatUpdate));
		while(true)
		{
			if (!bSendBusy)
			{
				bSendBusy = true;
				send(ServerSocket,SendBuf,sizeof(OOPkgFullStatUpdate),0);
				bSendBusy = false;
				break;
			}
		}
	}else{
		OOPkgStatUpdate pkgBuf;
		pkgBuf.etypeID = OOPStatUpdate;
		pkgBuf.Flags = 1 | 2;
		pkgBuf.Health = Player->Health;
		pkgBuf.Magika = Player->Magika;
		pkgBuf.Fatigue = Player->Fatigue;
		pkgBuf.TimeStamp = Player->Time;
		pkgBuf.refID = PlayerID;
		SendBuf = (char *)malloc(sizeof(OOPkgStatUpdate));
		memcpy(SendBuf,&pkgBuf,sizeof(OOPkgStatUpdate));
		while(true)
		{
			if (!bSendBusy)
			{
				bSendBusy = true;
				send(ServerSocket,SendBuf,sizeof(OOPkgStatUpdate),0);
				bSendBusy = false;
				break;
			}
		}
		//Temp
		char tempData[64];
		sprintf(tempData, "Player %u with HP of %i", pkgBuf.refID, pkgBuf.Health);
		Console_Print(tempData);
		//End Temp
	}
	free(SendBuf);
	return true;
}

bool NetReadBuffer(char *acReadBuffer)
{
	OOPacketType ePacketType = SelectType(acReadBuffer);

	switch (ePacketType)
	{
	case OOPWelcome:
		OOPWelcome_Handler(acReadBuffer);
		break;
	case OOPPosUpdate:
		OOPPosUpdate_Handler(acReadBuffer);
		break;
	case OOPZone:	
		OOPZone_Handler(acReadBuffer);
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
	case OOPStatUpdate:
		OOPStatUpdate_Handler(acReadBuffer);
		break;
	case OOPTimeUpdate:
		OOPTimeUpdate_Handler(acReadBuffer);
		break;
	default: 
		break;
	}
	return true;
}