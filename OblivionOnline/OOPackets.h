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

#pragma once

/* defines for Flags */
const GUID gcOOGUID = 
{ 0x2b09e144, 0x4976, 0x44f6, { 0xaa, 0x8f, 0xb6, 0x27, 0x99, 0x24, 0x32, 0xaf } }; // so we can copy it over later
enum OOPacketType
{
	OOPWelcome = 0,		//Send to synchronise versions , 
	OOPPosUpdate,		//Sends a position of actors , objects and players
	OOPZone,			//A player,actor or object changes zone.
	OOPChat,			//Someone sends a chat message
	OOPEvent,			//An Event is triggered by a plugin
	OOPEventRegister,	//An Event is registered, server only
	OOPFullStatUpdate,	//Send all actor, mob, player stats
	OOPStatUpdate,		//Just send HP, MP, and Fatigue
	OOPTimeUpdate		//Send the time to all clients
};
#pragma pack(push,1)
struct OOPkgWelcome //THIS PACKET IS NOT CHANGEABLE ; IT STAYS LIKE THIS BECAUSE IT HAS TO WORK WITH ALL VERSIONS!!!!
{
	//4 bytes(most common enum) is a lot .... but better than a short and a hell of problems later
	OOPacketType etypeID;  // has to be OOPWelcome
	short Flags; //none yet , this can be changed
	short PlayerID; // 0 for client
	WORD wVersion; // LowerByte contains subversion ( or bugfix ) Higher Byte contains major release
	GUID guidOblivionOnline; // contains OblivionOnline GUID , this is once defined by me and never to be changed.
	char NickName[32]; // ignored when sent by client
};
/*
OblivionOnline GUID 
// {2B09E144-4976-44f6-AA8F-B627992432AF}
static const GUID OO = 
{ 0x2b09e144, 0x4976, 0x44f6, { 0xaa, 0x8f, 0xb6, 0x27, 0x99, 0x24, 0x32, 0xaf } };
- I googled it and it is unique:) - and it is a GUID
*/
struct OOPkgPosUpdate
{
	OOPacketType etypeID;
	short Flags;	// will be declared as enum or defines soon. atm it is 1 - player 2 - actor ( 3 equals both , aka player) 
	float fPosX,fPosY,fPosZ;
	float fRotX,fRotY,fRotZ;
	UInt32 refID;	// It is the reference ID if it is a NPC or object , player number when a player
};
struct OOPkgZone
{
	OOPacketType etypeID;
	short Flags;	//1 - Exterior Cell 
	char ZoneName[128]; // \0 terminated string containing the Zone if it is an interior , or the Worldspace if it is an exterior
	float fPosX,fPosY,fPosZ;
	float fRotX,fRotY,fRotZ;
	UInt32 cellID;	// It is the reference ID of the current cell
	UInt32 refID;	// It is the reference ID if it is a NPC or object , player number when a player
};
struct OOPkgChat //THIS PACKAGE IS NOT DIRECTLY MAPPED , but has to be converted
{
	OOPacketType etypeID;
	short Flags; // none yet
	int Length; // attention here!!!!!!!!
	//The actual message body is contained after this data. It is manually written as ANSI string
};
struct OOPkgEvent // This package is for Plugin Events
{
	OOPacketType etypeID;
	short Flags;	// none yet
	DWORD Param1;
	DWORD Param2;
	int EventID;
};
struct OOPkgEventRegister // This package is for Plugin Events
{
	OOPacketType etypeID;
	short Flags;	// none yet
	char EventString[64]; //better than variable length here
	int EventID;
};
struct OOPkgFullStatUpdate // This package is for full stat updates
{
	OOPacketType etypeID;
	short Flags; 	//1 - player, 2 - actor ( 3 equals both , aka player)
	int Strength, Intelligence, Willpower;
	int Agility, Speed, Endurance;
	int Personality, Luck, Encumbrance;
	int Health, Magika, Fatigue;
	float TimeStamp;	// This is so that the server can sort data
	UInt32 refID;	// It is the reference ID if it is a NPC or object , player number when a player
};
struct OOPkgStatUpdate	// This package is for quick HP, MP, Fatigue updates
{
	OOPacketType etypeID;
	short Flags; 	//1 - player, 2 - actor ( 3 equals both , aka player)
	int Health, Magika, Fatigue;
	float TimeStamp;	// This is so that the server can sort data
	UInt32 refID;	// It is the reference ID if it is a NPC or object , player number when a player
};
struct OOPkgTimeUpdate
{
	OOPacketType etypeID;
	short Flage;	// none so far
	unsigned int Hours, Minutes, Seconds;
};
#pragma pack(pop)
inline OOPacketType SelectType(char *Packet)
{
	return *((OOPacketType *)Packet);
}