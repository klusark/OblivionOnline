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
#include "OOPacketHandler.h"

//Prototypes
extern void RunScriptLine(const char *buf, bool IsTemp);

bool OOPWelcome_Handler(char *Packet)
{
	OOPkgWelcome InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgWelcome));
	sscanf(InPkgBuf.NickName, "Player%2d", &LocalPlayer);
	_MESSAGE("Received Player ID %u",LocalPlayer);
	Console_Print(InPkgBuf.NickName);
	return true;
}

bool OOPPosUpdate_Handler(char *Packet)
{
	OOPkgPosUpdate InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgPosUpdate));
	if ((InPkgBuf.refID < MAXCLIENTS) && (InPkgBuf.refID != LocalPlayer))
	{
		OtherPlayer = InPkgBuf.refID;
		Players[OtherPlayer].PosX = InPkgBuf.fPosX;
		Players[OtherPlayer].PosY = InPkgBuf.fPosY;
		Players[OtherPlayer].PosZ = InPkgBuf.fPosZ;
		Players[OtherPlayer].RotX = InPkgBuf.fRotX;
		Players[OtherPlayer].RotY = InPkgBuf.fRotY;
		Players[OtherPlayer].RotZ = InPkgBuf.fRotZ;
	}
	return true;
}

bool OOPZone_Handler(char *Packet)
{
	OOPkgZone InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgZone));

	if ((InPkgBuf.refID < MAXCLIENTS) && (InPkgBuf.refID != LocalPlayer))
	{
		OtherPlayer = InPkgBuf.refID;
		Players[OtherPlayer].PosX = InPkgBuf.fPosX;
		Players[OtherPlayer].PosY = InPkgBuf.fPosY;
		Players[OtherPlayer].PosZ = InPkgBuf.fPosZ;
		Players[OtherPlayer].RotX = InPkgBuf.fRotX;
		Players[OtherPlayer].RotY = InPkgBuf.fRotY;
		Players[OtherPlayer].RotZ = InPkgBuf.fRotZ;
		Players[OtherPlayer].CellID = InPkgBuf.cellID;
		strcpy(Players[OtherPlayer].Zone, InPkgBuf.ZoneName);
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
	sprintf(chatScript, "Message \"\%s\", %s", MessageDest);
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
		OtherPlayer = InPkgBuf.refID;
		Players[OtherPlayer].Agility = InPkgBuf.Agility;
		Players[OtherPlayer].Encumbrance = InPkgBuf.Encumbrance;
		Players[OtherPlayer].Endurance = InPkgBuf.Endurance;
		Players[OtherPlayer].Intelligence = InPkgBuf.Intelligence;
		Players[OtherPlayer].Luck = InPkgBuf.Luck;
		Players[OtherPlayer].Personality = InPkgBuf.Personality;
		Players[OtherPlayer].Speed = InPkgBuf.Speed;
		Players[OtherPlayer].Strength = InPkgBuf.Strength;
		Players[OtherPlayer].Willpower = InPkgBuf.Willpower;
		Players[OtherPlayer].Health = InPkgBuf.Health;
		Players[OtherPlayer].Magika = InPkgBuf.Magika;
		Players[OtherPlayer].Fatigue = InPkgBuf.Fatigue;
	}
	return true;
}

bool OOPStatUpdate_Handler(char *Packet)
{
	OOPkgStatUpdate InPkgBuf;
	memcpy(&InPkgBuf,Packet,sizeof(OOPkgStatUpdate));
	if ((InPkgBuf.refID < MAXCLIENTS) && (InPkgBuf.refID != LocalPlayer))
	{
		OtherPlayer = InPkgBuf.refID;
		Players[OtherPlayer].Health = InPkgBuf.Health;
		Players[OtherPlayer].Magika = InPkgBuf.Magika;
		Players[OtherPlayer].Fatigue = InPkgBuf.Fatigue;
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