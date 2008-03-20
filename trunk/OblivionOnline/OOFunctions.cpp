/*

Copyright(c) 2007-2008   Julian Bangert aka masterfreek64
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
UINT32 GetPlayerNumberFromRefID(UInt32 refID) // retrieves a player number from a refID in spawn
{
	bool foundID = false;
	if (refID == (*g_thePlayer)->refID)
	{
		return  LocalPlayer;
		foundID = true;
	}else{
		/*
		modus operandi:
		We check the spawn list , leaving the localplayer out
		*/
		// The player the current spawnID respresents
		//(0 == LocalPlayer) ? 1 : 0; // If LocalPlayer is 0 we start at 1 ... , because we have to leave him out 
		// if not at 0
		
		//If not the player, check the SpawnID list
		//currentspawn += ((i == LocalPlayer)?0:1)
		//A really complex but fast statement , selectíng the proper player id
		//translates into 
		//if(i == LocalPlayer) 
		//	LocalPlayer ++
		// more safe than any evaluation of the type currentspawn += (LocalPlayer == i) , this is not dependent on bool int conversion
		//,currentplayer += ((i == LocalPlayer)?0:1)
		int currentplayer = ((0 == LocalPlayer) ? 1 : 0);
		for (UINT32 currentspawn = 0 ;  currentspawn<MAXCLIENTS; currentplayer++ ,
			currentspawn += ((currentplayer == LocalPlayer)?0:1))
		{
			if(SpawnID[currentspawn] == refID) // we find it quite fast. 12 cmp cycles 12 add cycles
			{
				if(PlayerConnected[currentplayer])
				{
					return currentplayer;
				}
			}
		}
	}
	return -1;
}
/*
float GetStat(Actor* ActorBuf, int statNum)
{
	float statValue = -1;
	int PlayerNum = GetPlayerNumberFromRefID(ActorBuf->refID);

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
*/