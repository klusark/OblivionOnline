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
*/

#ifndef BasicServer_h
#define BasicServer_h

#pragma once
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <process.h>

#define MAXCLIENTS 2
#define PORT 41805

#define MAIN_VERSION 3
#define SUB_VERSION 5	//Release 3, bugfix 5
#define UInt32 unsigned long

struct ObjectStatus
{
	float PosX,PosY,PosZ;
	float RotX,RotY,RotZ;
	UInt32 RefID;
	UInt32 CellID;
};

struct ActorStatus : ObjectStatus
{
	int Strength, Intelligence, Willpower;
	int Agility, Speed, Endurance;
	int Personality, Luck, Encumbrance;
	int Health, Magika, Fatigue;
};

struct PlayerStatus : ActorStatus
{
	float Time;
	int Day, Month, Year;
};


extern int TotalClients;
extern bool bServerAlive;
extern SOCKET clients[MAXCLIENTS];
extern char acSendBuffer[512];
extern PlayerStatus Players[MAXCLIENTS];
extern FILE *easylog;
extern int StartNet(void);
extern int ScanBuffer(char *acReadBuffer, short LocalPlayer);

#endif