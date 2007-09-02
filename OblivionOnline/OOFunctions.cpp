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

	// Compare reference ID's and determine which actor we have selected in-game
	int retVal = -1;
	if (refID == 20)
		retVal = LocalPlayer;
	if (refID == SpawnID[0])
		retVal = OtherPlayer;

	// This is to prevent errors when only 1 player is connected
	if (LocalPlayer == OtherPlayer)
		retVal = -2;

	return retVal;
}

float GetStat(Actor* ActorBuf, int statNum)
{
	float statValue = 0;
	int PlayerNum = GetActorID(ActorBuf->refID);
	if (PlayerNum != -1)
	{
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