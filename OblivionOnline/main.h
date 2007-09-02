/*

Copyright 2007 Joseph Pearson aka chessmaster42 and Julian aka masterfreek64

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

#ifndef main_h
#define main_h

#include <stdio.h>
#include <Windows.h>
#include "obse/PluginAPI.h"
#include "obse/CommandTable.h"
#include "obse/GameAPI.h"
#include "obse/ParamInfos.h"
#include "obse/GameObjects.h"

#define MAXCLIENTS 2
#define MAIN_VERSION 2
#define SUB_VERSION 1 // Beta 1 for release 2

struct ObjectStatus
{
	float PosX,PosY,PosZ;
	float RotX,RotY,RotZ;
	UInt32 RefID;
	UInt32 CellID;
	char Zone[128];
};

struct ActorStatus : ObjectStatus
{
	int Strength, Intelligence, Willpower;
	int Agility, Speed, Endurance;
	int Personality, Luck, Encumbrance;
	int Health, Magika, Fatigue;

	bool bStatsInitialized;
	bool bInitialSet;
};

struct PlayerStatus : ActorStatus
{
	float Time;
	int Day, Month, Year;
};

extern IDebugLog gLog;
extern bool bIsConnected;
extern int LocalPlayer;
extern int OtherPlayer;
extern SOCKET ServerSocket;
extern HANDLE hRecvThread;
extern PlayerStatus Players[MAXCLIENTS];
extern UInt32 SpawnID[MAXCLIENTS];

#endif