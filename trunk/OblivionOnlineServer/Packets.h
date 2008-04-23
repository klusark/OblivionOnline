/*
Copyright 2008   Julian Bangert aka masterfreek64
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
#pragma once
#include "GlobalDefines.h"
/*
OO Advanced Packet System:
Packets are streams of bytes transported either over an unreliable (UDP ) or a reliable (TCP) carrier:
OO Packets consist of multiple data segments , called chunks
Every chunk contains 1 data , such as position on an axis , health , equipment in a slot , etc
Chunks that require reliability are sent over TCP , others can be sent on both , but usually are sent over UDP
Every packet can contain data for up to 16 objects - identified by formID. Object chunks must be sent before the data.

The Packet Layout
BYTE	Data
0		Number of chunks as unsigned char
1-2		Size of Packet in bytes as unsigned short
3-end	chunks 


Chunk Base Layout
BYTE	DATA
0-1		Chunk ID :the first 4 bits ( && 0xff000000 ) Specify the object this is related to , the others ( && 0x00ffffff) the chunk type


Strings in Packets:
ANSI strings are saved as follows:
unsigned short Length 
raw data 

Important Chunks
TYPE#	Description
0		Bad packet
1		ObjectID chunk - contains a single UINT32 field which specifies the FormID of this object. After that 1 byte to specify the type: 0 object, 1 actor , 2 player
2		Position . 6 floats : PosX, PosY, PosZ ,RotX,RotY,RotZ
3		CellID . One UINT32
		EMPTY(old Health)
		EMPTY(old Magicka)
		EMPTY(old Fatigue)

7		Gender . One Byte 1 if female
8		Race   . UINT32
9		Class  . ANSI string
10		Name   . ANSI string
11		ActorValue. BYTE AV Code and then short Value
12		EMPTY
13		Equip .  BYTE slot and UINT32 Equip
14		Chat	. ANSI string
15		Auth	. ANSI string Username , 512 bit SHA-512 
16		Client Type. BYTE : 0 if passive , 1 if master client. Passive is assumed until further notice ;)
17		Version .	SUPER , MAJOR AND MINOR as bytes
18		PlayerID . UINT32 Player ID - sent only by server
*/
#define PACKET_SIZE 1024
#define PACKET_HEADER_SIZE 3
#define MAX_OBJECTS_PER_PACKET 16
#define OBJECTMASK 0xff000000
#define CHUNKMASK 0x00ffffff
enum PkgChunk
{
	Object	 = 1,
	Position = 2,
	CellID	 = 3,
	Gender   = 7,
	Race     = 8,
	Class    = 9,
	Name     = 10,
	ActorValue= 11,
	Equip	 = 13,
	Chat     = 14,
	Auth	 = 15,
	ClientType = 16,
	Version	= 17,
	PlayerID = 18
};
inline bool RequiresReliable(PkgChunk type)
{
	switch (type) // No breaks here

	{
	case	Object:
		return false;
	case	Position:
		return false;
	case CellID:
		return true;
	case Gender:
		return true;
	case Race:
		return true;
	case Class:
		return true;
	case Name:
		return true;
	case ActorValue:
		return true;
	case Equip:
		return true;
	case Chat:
		return true;
	case Auth:
		return true;
	case ClientType:
		return true;
	case Version:
		return true;
	case PlayerID:
		return true;
	default:
		return false;
	}
}
inline size_t GetMinChunkSize(PkgChunk type)
{
	switch(type)
	{
	case	Object:
		return sizeof(UINT32)+sizeof(BYTE);
	case	Position:
		return 6*sizeof(float);
	case CellID:
		return sizeof(UINT32);
	case Gender:
		return sizeof(BYTE);
	case Race:
		return sizeof(UINT32);
	case Class:
		return sizeof(unsigned short);
	case Name:
		return sizeof(unsigned short);
	case ActorValue:
		return sizeof(BYTE)+sizeof(unsigned short);
	case Chat:
		return sizeof(unsigned short);
	case Auth:
		return sizeof(unsigned short);
	case ClientType:
		return sizeof(BYTE);
	case Version:
		return 3*sizeof(BYTE);
	case PlayerID:
		return sizeof(UINT32);
	default:
		return sizeof(unsigned short);
	}
}
inline PkgChunk GetChunkType(BYTE *stream)
{
	return (PkgChunk)(*((INT16 *)(stream)) & CHUNKMASK);
}

inline UINT8 GetObject(BYTE *stream)
{
	return (PkgChunk)(*((INT16 *)(stream)) & OBJECTMASK);
}
inline std::string ReadANSIString(BYTE* BaseStream,size_t maxlen)
{
	return std::string((char *)(BaseStream + sizeof(unsigned short)), ((maxlen < *(unsigned short *)BaseStream) ? (maxlen) : *(unsigned short *)BaseStream));
}
#define STATUS_OBJECT 0
#define STATUS_NPC 1
#define STATUS_PLAYER 2