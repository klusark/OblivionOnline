/*

Copyright 2007   Julian Bangert aka masterfreek64, Joseph Pearson aka chessmaster42 and Joel Teichroeb aka bobjr777

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

#ifndef OOStructs_h
#define OOStructs_h

//--------------------------
//---Begin Status Structs---
//--------------------------

struct ObjectStatus
{
	float PosX,PosY,PosZ;
	float RotX,RotY,RotZ;
	UInt32 RefID;
	UInt32 CellID;
};

struct ActorStatus : ObjectStatus
{
	int Health, Magika, Fatigue;
	bool bIsInInterior;
	bool InCombat;
};
struct ActorStatusEx : ActorStatus
{
	int Strength, Intelligence, Willpower;
	int Agility, Speed, Endurance;
	int Personality, Luck, Encumbrance;
	
	UInt32 head,hair,upper_body,lower_body;
	UInt32 hand,foot,right_ring,left_ring;
	UInt32 amulet,shield,tail,weapon,ammo;
	UInt32 robes;

	bool bStatsInitialized;
};

struct PlayerStatus : ActorStatusEx
{
	float Time;
	int Day, Month, Year;
};

//------------------------
//---End Status Structs---
//------------------------

//-------------------------
//---Begin Mob Synch Structs---
//-------------------------

//-----------------------
//---End Mob Synch Structs---
//-----------------------
//-------------------------
//---Begin Misc. Structs---
//-------------------------

//-----------------------
//---End Misc. Structs---
//-----------------------

#endif

