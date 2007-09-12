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

#ifndef OOStructs_h
#define OOStructs_h

//--------------------------
//---Begin Status Structs---
//--------------------------

struct ObjectStatus
{
	float PosX,PosY,PosZ;
	float RotX,RotY,RotZ;
	float VelX,VelY,VelZ;		//Linear velocities
	float RVelX,RVelY,RVelZ;	//Rotaional velocities
	float Velocity;				//Absolute linear velocity
	UInt32 RefID;
	UInt32 CellID;
};

struct ActorStatus : ObjectStatus
{
	int Strength, Intelligence, Willpower;
	int Agility, Speed, Endurance;
	int Personality, Luck, Encumbrance;
	int Health, Magika, Fatigue;
	
	UInt32 head,hair,upper_body,lower_body;
	UInt32 hand,foot,right_ring,left_ring;
	UInt32 amulet,shield,tail,weapon,ammo;
	UInt32 robes;

	bool bStatsInitialized;
	bool bIsInInterior;
};

struct PlayerStatus : ActorStatus
{
	float Time;
	int Day, Month, Year;
};

//------------------------
//---End Status Structs---
//------------------------

//-------------------------
//---Begin Misc. Structs---
//-------------------------

//-----------------------
//---End Misc. Structs---
//-----------------------

#endif