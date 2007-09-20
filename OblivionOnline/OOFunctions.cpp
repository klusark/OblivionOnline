/*

Copyright 2007  Julian aka masterfreek64, Joseph Pearson aka chessmaster42 and Joel Teichroeb aka bobjr777

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

void RunScriptLine(const char *buf, bool IsTemp)
{
	// create a Script object
	UInt8	scriptObjBuf[sizeof(Script)];
	Script	* tempScriptObj = (Script *)scriptObjBuf;

	void	* scriptState = GetGlobalScriptStateObj();

	tempScriptObj->Constructor();
	if (IsTemp)
		tempScriptObj->MarkAsTemporary();
	tempScriptObj->SetText(buf);
	tempScriptObj->CompileAndRun(*((void **)scriptState), 1, NULL);
	tempScriptObj->StaticDestructor();
}

int GetActorID(UInt32 refID)
{
	refID = refID & 0x00ffffff;	//Mask off the mod offset
	// Compare reference ID's and determine which actor we have selected in-game
	int retVal = -1;
	bool foundID = false;
	if (refID == 20)
	{
		retVal =  LocalPlayer;
		foundID = true;
	}else{

		//If not the player, check the SpawnID list
		for (int i=0; i<MAXCLIENTS; i++)
		{
			if(PlayerConnected[i] && (i != LocalPlayer))
			{
				for(int j=0; j<=i; j++)
				{
					if ((refID == SpawnID[j]) && !foundID)
					{
						retVal = i;
						foundID = true;
					}
				}
			}
		}
	}

	//Is only 1 player connected? and is ref valid?
	if ((TotalPlayers <= 1) && (retVal != -1))
	{
		retVal = -2;
	}

	return retVal;
}

float GetStat(Actor* ActorBuf, int statNum)
{
	float statValue = -1;
	int PlayerNum = GetActorID(ActorBuf->refID);

	//If actorID is valid and stats have been initialized, retrieve the stat we want
	if (PlayerNum != -1 && PlayerNum != -2)
	{
		if (!Players[PlayerNum].bStatsInitialized)
			return -1;

		switch(statNum)
		{
		case 0:
			statValue = Players[PlayerNum].Strength;
			break;
		case 1:
			statValue = Players[PlayerNum].Intelligence;
			break;
		case 2:
			statValue = Players[PlayerNum].Willpower;
			break;
		case 3:
			statValue = Players[PlayerNum].Agility;
			break;
		case 4:
			statValue = Players[PlayerNum].Speed;
			break;
		case 5:
			statValue = Players[PlayerNum].Endurance;
			break;
		case 6:
			statValue = Players[PlayerNum].Personality;
			break;
		case 7:
			statValue = Players[PlayerNum].Luck;
			break;
		case 8:
			statValue = Players[PlayerNum].Health;
			break;
		case 9:
			statValue = Players[PlayerNum].Magika;
			break;
		case 10:
			statValue = Players[PlayerNum].Fatigue;
			break;
		case 11:
			statValue = Players[PlayerNum].Encumbrance;
			break;
		default:
			statValue = -1;
			break;
		};
	}
	return statValue;
}