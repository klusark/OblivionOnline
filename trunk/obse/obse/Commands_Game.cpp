#include "obse/Commands_Game.h"
#include "obse/ParamInfos.h"
#include "obse/GameObjects.h"

#ifdef OBLIVION

#include "GameAPI.h"
#include "Hooks_Gameplay.h"
#include "Hooks_DirectInput8Create.h"
#include "GameForms.h"
#include <set>
#include <string>
#include "obse/Commands_Input.h"
#include "obse/GameMenus.h"
#include "GameData.h"

// first character in name mapped to type ID
//	b	0
//	c	1
//	h	2
//	i	3
//	u	4
//	f	5
//	S	6
//	s	6
//	r	7
//	a	8
//	anything else is 9

static bool Cmd_SetNumericGameSetting_Execute(COMMAND_ARGS)
{
	*result = 0;
	
	char	settingName[256] = { 0 };
	float	settingData = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &settingName, &settingData))
		return true;

	SettingInfo	* setting = NULL;

	if(GetGameSetting(settingName, &setting))
	{
		if(setting && setting->name)
		{
			*result = 1;

			switch(setting->Type())
			{
				case SettingInfo::kSetting_Integer:
					setting->i = settingData;
					break;

				case SettingInfo::kSetting_Float:
					setting->f = settingData;
					break;

				case SettingInfo::kSetting_Unsigned:
					setting->u = settingData;
					break;

				default:
					*result = 0;
					break;
			}
		}
	}

	return true;
}

// ### TODO: collapse this and SettingInfo
struct INISettingEntry
{
	struct Data
	{
		union
		{
			bool	b;
			float	f;
			int		i;
			char	* s;
			UInt32	u;
		} data;
		char	* name;
	};

	Data			* data;
	INISettingEntry	* next;
};

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
static const UInt32 g_INISettingList = 0x00AF1898;
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
static const UInt32 g_INISettingList = 0x00B07BF0;
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
static const UInt32 g_INISettingList = 0x00B07BF0;
#else
#error unsupported version of oblivion
#endif

static bool Cmd_SetNumericINISetting_Execute(COMMAND_ARGS)
{
	*result = 0;
	
	char	settingName[256] = { 0 };
	float	settingData = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &settingName, &settingData))
		return true;

	for(INISettingEntry * entry = (INISettingEntry *)(g_INISettingList + 0x10C); entry; entry = entry->next)
	{
		INISettingEntry::Data	* data = entry->data;
		if(data && data->name && !_stricmp(settingName, data->name))
		{
			*result = 1;

			switch(data->name[0])
			{
				case 'i':	// int
					data->data.i = settingData;
					break;

				case 'f':	// float
					data->data.f = settingData;
					break;

				case 'u':	// unsigned
					data->data.u = settingData;
					break;

				case 'b':	// bool
					data->data.b = (settingData != 0);
					break;

				default:
					*result = 0;
					break;
			}

			break;
		}
	}

	return true;
}

bool Cmd_GetNumericINISetting_Execute(COMMAND_ARGS)
{
	*result = 0;
	
	char	settingName[256] = { 0 };

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &settingName))
		return true;

	for(INISettingEntry * entry = (INISettingEntry *)(g_INISettingList + 0x10C); entry; entry = entry->next)
	{
		INISettingEntry::Data	* data = entry->data;
		if(data && data->name && !_stricmp(settingName, data->name))
		{
			switch(data->name[0])
			{
				case 'i':	// int
					*result = data->data.i;
					break;

				case 'f':	// float
					*result = data->data.f;
					break;

				case 'u':	// unsigned
					*result = data->data.u;
					break;

				case 'b':	// bool
					*result = data->data.b ? 1 : 0;
					break;

				default:
					*result = 0;
					break;
			}

			break;
		}
	}

	return true;
}

typedef std::set <UInt32> ScriptSetType;

static bool Cmd_GetGameLoaded_Execute(COMMAND_ARGS)
{
	static ScriptSetType	informedScripts;

	*result = 0;

	// was a game loaded?
	if(g_gameLoaded)
	{
		// yes, clear the list of scripts we've informed and reset the 'game loaded' flag
		informedScripts.clear();

		g_gameLoaded = false;
	}

	if(scriptObj)
	{
		// have we returned 'true' to this script yet?
		if(informedScripts.find(scriptObj->refID) == informedScripts.end())
		{
			// no, return true and add to the set
			*result = 1;

			informedScripts.insert(scriptObj->refID);
		}
	}

	return true;
}

static bool Cmd_GetGameRestarted_Execute(COMMAND_ARGS)
{
	static std::set<UInt32> regScripts;

	*result = 0;

	if (scriptObj && (regScripts.find(scriptObj->refID) == regScripts.end()))
	{
		*result = 1;
		regScripts.insert(scriptObj->refID);
	}
	
	return true;
}

struct TimeInfo
{
	UInt8	disableCount;	// 00
	UInt8	pad1[3];		// 01
	float	fpsClamp;		// 04 - in seconds
	float	unk08;			// 08
	float	frameTime;		// 0C - in seconds
	float	unk10;			// 10
	UInt32	gameStartTime;	// 14
};

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
TimeInfo	* g_timeInfo = (TimeInfo *)0x00AEEAB8;
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
TimeInfo	* g_timeInfo = (TimeInfo *)0x00B33E90;
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
TimeInfo	* g_timeInfo = (TimeInfo *)0x00B33E90;
#else
#error unsupported oblivion version
#endif

static bool Cmd_GetFPS_Execute(COMMAND_ARGS)
{
#if 0
	float			frameTime = g_timeInfo->frameTime;
#else
	float			frameTime = GetAverageFrameTime();
#endif

	// clamp to prevent weird behavior
	const float	kFPSCap = 10000.0f;	// 10K FPS ought to be enough for anybody
	const float kMinFrameTime = 1.0f / kFPSCap;

	if(frameTime < kMinFrameTime) frameTime = kMinFrameTime;

	*result = 1.0f / frameTime;

	return true;
}

typedef void (* _ToggleGlobalCollision)(void);

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

UInt8							* g_isCollisionOff = (UInt8 *)0x00AED364;
const _ToggleGlobalCollision	ToggleGlobalCollision = (_ToggleGlobalCollision)0x0043ED30;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2

UInt8							* g_isCollisionOff = (UInt8 *)0x00B33A34;
const _ToggleGlobalCollision	ToggleGlobalCollision = (_ToggleGlobalCollision)0x00444A90;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416

UInt8							* g_isCollisionOff = (UInt8 *)0x00B33A34;
const _ToggleGlobalCollision	ToggleGlobalCollision = (_ToggleGlobalCollision)0x004447F0;

#else
#error unsupported oblivion version
#endif

bool IsGlobalCollisionDisabled(void)
{
	return *g_isCollisionOff != 0;
}

static bool Cmd_IsGlobalCollisionDisabled_Execute(COMMAND_ARGS)
{
	*result = IsGlobalCollisionDisabled() ? 1 : 0;

	return true;
}

static bool Cmd_SetDisableGlobalCollision_Execute(COMMAND_ARGS)
{
	UInt32	disable = 0;
	UInt32	currentState = IsGlobalCollisionDisabled();

	*result = currentState;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &disable))
		return true;

	if(disable != currentState)
		ToggleGlobalCollision();

	return true;
}

static bool Cmd_GetDebugSelection_Execute(COMMAND_ARGS)
{
	*result = 0;

	InterfaceManager	* interfaceManager = InterfaceManager::GetSingleton();
	if(interfaceManager && interfaceManager->debugSelection)
	{
		UInt32	* refResult = (UInt32 *)result;

		*refResult = interfaceManager->debugSelection->refID;
	}

	return true;
}

static bool Cmd_MessageEX_Execute(COMMAND_ARGS)
{
	*result = 0;
	char buffer[512];

	if (ExtractFormattedString(buffer, arg1, scriptObj, eventList, opcodeOffsetPtr))
	{
		*result = 1;
		QueueUIMessage(buffer, 0, 1, 1);
	}

	return true;
}

static bool Cmd_MessageBoxEX_Execute(COMMAND_ARGS)
{
	*result = 0;
	char buffer[512];

	if (!ExtractFormattedString(buffer, arg1, scriptObj, eventList, opcodeOffsetPtr))
		return true;

	//extract the buttons
	char* b[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	UInt32 btnIdx = 0;
	
	for (char* ch = buffer; *ch && btnIdx < 10; ch++)
	{
		if (*ch == '|')
		{
			*ch = '\0';
			b[btnIdx++] = ch + 1;
		}
	}

	if (!btnIdx)				//supply default OK button
		b[0] = "Ok";

	ShowMessageBox(buffer, ShowMessageBox_Callback, 0, b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9], 0);

	if (thisObj && !(thisObj->flags & 0x00004000))
		*ShowMessageBox_pScriptRefID = thisObj->refID;
	else
		*ShowMessageBox_pScriptRefID = scriptObj->refID;


	return true;
}

static bool Cmd_GetCrosshairRef_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	TESObjectREFR* xRef = ((*g_HUDInfoMenu)->crosshairRef);
	if (xRef)
		if (Oblivion_DynamicCast(xRef, 0, RTTI_TESObjectREFR, RTTI_TESObjectREFR, 0))
			*refResult = xRef->refID;

	return true;
}

static bool Cmd_IsModLoaded_Execute(COMMAND_ARGS)
{
	char modName[512];
	*result = 0;

	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &modName))
		return true;

	const DataHandler::ModEntry* modEntry = (*g_dataHandler)->LookupModByName(modName);
	if (modEntry)
		if (modEntry->IsLoaded())
			*result = 1;

	return true;
}

// ModEntry::Data.idx doesn't appear to be mod index. Doesn't appear to be turning up elsewhere, either.

/*
static bool Cmd_GetModIndex_Execute(COMMAND_ARGS)
{
	char modName[512];
	*result = -1;

	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &modName))
		return true;
	for (DataHandler::ModEntry* mmm = &((*g_dataHandler)->modList); mmm; mmm = mmm->next)
	{
		Console_Print("%d %s", mmm->data->idx, mmm->data->name);
		_MESSAGE("%s", mmm->data->name);
		DumpClass(mmm->data, sizeof (DataHandler::ModEntry::Data)/4);
	}

	const DataHandler::ModEntry* modEntry = (*g_dataHandler)->LookupModByName(modName);
	if (modEntry && modEntry->data)
		*result = modEntry->data->idx;

	return true;
}
*/

#endif

static ParamInfo kParams_SetNumericGameSetting[] =
{
	{	"string",	kParamType_String,	0 },
	{	"float",	kParamType_Float,	0 },
};

CommandInfo kCommandInfo_SetNumericGameSetting =
{
	"SetNumericGameSetting",
	"setngs",
	0,
	"Set a game setting from a variable",
	0,
	2,
	kParams_SetNumericGameSetting,
	HANDLER(Cmd_SetNumericGameSetting_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetNumericINISetting =
{
	"SetNumericINISetting",
	"setnis",
	0,
	"Set an INI setting from a variable",
	0,
	2,
	kParams_SetNumericGameSetting,
	HANDLER(Cmd_SetNumericINISetting_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNumericINISetting =
{
	"GetNumericINISetting",
	"getnis",
	0,
	"Get an INI setting",
	0,
	1,
	kParams_OneString,
	HANDLER(Cmd_GetNumericINISetting_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetGameLoaded =
{
	"GetGameLoaded",
	"",
	0,
	"Returns true the first time the command is called (per script) after a game is loaded",
	0,
	0,
	NULL,
	HANDLER(Cmd_GetGameLoaded_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetGameRestarted =
{
	"GetGameRestarted",
	"OnRestart",
	0,
	"returns true once each time game is restarted",
	0,
	0,
	NULL,
	HANDLER(Cmd_GetGameRestarted_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetFPS =
{
	"GetFPS",
	"",
	0,
	"Returns the current rendering FPS",
	0,
	0,
	NULL,
	HANDLER(Cmd_GetFPS_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


CommandInfo kCommandInfo_IsGlobalCollisionDisabled =
{
	"IsGlobalCollisionDisabled",
	"",
	0,
	"Returns whether or not global collision is disabled (via the \'tcl\' console command)",
	0,
	0,
	NULL,
	HANDLER(Cmd_IsGlobalCollisionDisabled_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetDisableGlobalCollision =
{
	"SetDisableGlobalCollision",
	"",
	0,
	"Enables or disables global collision, returns the previous state",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_SetDisableGlobalCollision_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetDebugSelection =
{
	"GetDebugSelection",
	"",
	0,
	"returns the current selected object in the console",
	0,
	0,
	NULL,
	HANDLER(Cmd_GetDebugSelection_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_Message[21] =
{
	{"format string",	kParamType_String, 0},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
	{"variable",		kParamType_Float, 1},
};

CommandInfo kCommandInfo_MessageEX =
{
	"MessageEX", "MsgEX",
	0,
	"Prints a formatted message",
	0, 21, kParams_Message,
	HANDLER(Cmd_MessageEX_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MessageBoxEX =
{
	"MessageBoxEX", "MBoxEX",
	0,
	"Displays a formatted messagebox",
	0, 21, kParams_Message,
	HANDLER(Cmd_MessageBoxEX_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCrosshairRef =
{
	"GetCrosshairRef", "GetXRef",
	0,
	"returns the reference currently under the crosshair",
	0, 0, NULL,
	HANDLER(Cmd_GetCrosshairRef_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsModLoaded =
{
	"IsModLoaded", "",
	0,
	"returns 1 if the specified mod is currently loaded",
	0,
	1,
	kParams_OneString,
	HANDLER(Cmd_IsModLoaded_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

/*
CommandInfo kCommandInfo_GetModIndex =
{
	"GetModIndex", "",
	0,
	"returns the index of the specified mod",
	0,
	1,
	kParams_OneString,
	HANDLER(Cmd_GetModIndex_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};
*/
