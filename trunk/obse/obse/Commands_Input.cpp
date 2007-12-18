#include "Commands_Input.h"
#include "ParamInfos.h"
#include "GameForms.h"

#include <set>
#include <map>
#include <ctime>
#include "Commands_Console.h"

// 32	spacebar
// 48	0
// ...
// 57	9
// 65	A
// ...
// 90	Z
// 160	left shift
// 161	right shift
// 162	left control
// 163	right control

#if OBLIVION

#include "GameAPI.h"
#include "Hooks_DirectInput8Create.h"
#include <shlobj.h>

#define CONTROLSMAPPED 29
static bool IsKeycodeValid(UInt32 id)		{ return id < kMaxMacros - 2; }

UINT*  InputControls=0;
UINT*  AltInputControls=0;

static int _httoi(const char *value)
{
	unsigned int result = 0;
	sscanf_s(value, "%x", &result);
	return result;
}


static void ParseKey(char* iniPath, char* key,UINT ctrl)
{
	const char* section="Controls";
	char Ret[9];
	char In[5];
	DWORD size;
	size=GetPrivateProfileStringA(section,key,0,Ret,9,iniPath);
	if(size>=8) {
		strcpy_s(In, sizeof(In), &Ret[4]);
		AltInputControls[ctrl]=_httoi(In);
	} else AltInputControls[ctrl]=0xFFFF;
	if(size>=4) {
		Ret[4]=0;
		InputControls[ctrl]=_httoi(Ret);
	} else InputControls[ctrl]=0xFFFF;
}

static void GetControlMap()
{
	char iniPath[MAX_PATH];
	HRESULT hr=SHGetFolderPathA(0,CSIDL_PERSONAL,0,SHGFP_TYPE_CURRENT,iniPath);
	strcat_s(iniPath, sizeof(iniPath), "\\My Games\\Oblivion\\oblivion.ini");

	InputControls=new UINT[CONTROLSMAPPED];
	AltInputControls=new UINT[CONTROLSMAPPED];
	
	ParseKey(iniPath,"Forward",0);
	ParseKey(iniPath,"Back",1);
	ParseKey(iniPath,"SlideLeft",2);
	ParseKey(iniPath,"SlideRight",3);
	ParseKey(iniPath,"Use",4);
	ParseKey(iniPath,"Activate",5);
	ParseKey(iniPath,"Block",6);
	ParseKey(iniPath,"Cast",7);
	ParseKey(iniPath,"Ready Item",8);
	ParseKey(iniPath,"Crouch/Sneak",9);
	ParseKey(iniPath,"Run",10);
	ParseKey(iniPath,"Always Run",11);
	ParseKey(iniPath,"Auto Move",12);
	ParseKey(iniPath,"Jump",13);
	ParseKey(iniPath,"Toggle POV",14);
	ParseKey(iniPath,"Menu Mode",15);
	ParseKey(iniPath,"Rest",16);
	ParseKey(iniPath,"Quick Menu",17);
	ParseKey(iniPath,"Quick1",18);
	ParseKey(iniPath,"Quick2",19);
	ParseKey(iniPath,"Quick3",20);
	ParseKey(iniPath,"Quick4",21);
	ParseKey(iniPath,"Quick5",22);
	ParseKey(iniPath,"Quick6",23);
	ParseKey(iniPath,"Quick7",24);
	ParseKey(iniPath,"Quick8",25);
	ParseKey(iniPath,"QuickSave",26);
	ParseKey(iniPath,"QuickLoad",27);
	ParseKey(iniPath,"Grab",28);
}

static bool Cmd_GetControl_Execute(COMMAND_ARGS)
{
	*result=0xFFFF;

	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;

	if(keycode>=CONTROLSMAPPED) return true;

	if(!InputControls) GetControlMap();

	*result = InputControls[keycode];
	return true;
}

static bool Cmd_GetAltControl_Execute(COMMAND_ARGS)
{
	*result=0xFFFF;

	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;

	if(keycode>=CONTROLSMAPPED) return true;

	if(!InputControls) GetControlMap();

	*result = AltInputControls[keycode];
	return true;
}
static bool Cmd_IsKeyPressed_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;
	if(GetAsyncKeyState(keycode) & 0x8000) *result = 1;

	return true;
}

static bool Cmd_IsKeyPressed2_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;
	if(keycode%256==255&&keycode<2048) keycode=255+(keycode+1)/256;

	//Using IsKeycodeValid here stops it from detecting the mouse wheel!
	if(keycode<kMaxMacros) {
		if(DI_data.LastBytes[keycode]) *result=1;
	}

	return true;
}

static bool Cmd_TapKey_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;
	if(keycode%256==255&&keycode<2048) keycode=255+(keycode+1)/256;
    if(IsKeycodeValid(keycode)) DI_data.TapStates[keycode]=0x80;

	return true;
}

static bool Cmd_MenuTapKey_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;
    if(keycode<256) DInput_FakeBufferedKeyTap(keycode);

	return true;
}

static bool Cmd_HoldKey_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;
	if(keycode%256==255&&keycode<2048) keycode=255+(keycode+1)/256;
    if(IsKeycodeValid(keycode)) DI_data.FakeStates[keycode]=0x80;

	return true;
}

static bool Cmd_ReleaseKey_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;
	if(keycode%256==255&&keycode<2048) keycode=255+(keycode+1)/256;
    if(IsKeycodeValid(keycode)) DI_data.FakeStates[keycode]=0x00;

	return true;
}

static bool Cmd_MenuHoldKey_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;
    if(keycode<256) DInput_FakeBufferedKeyPress(keycode);

	return true;
}

static bool Cmd_MenuReleaseKey_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;
    if(keycode<256) DInput_FakeBufferedKeyRelease(keycode);

	return true;
}

static bool Cmd_HammerKey_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;
	if(keycode%256==255&&keycode<2048) keycode=255+(keycode+1)/256;
    if(IsKeycodeValid(keycode)) DI_data.HammerStates[keycode]=0x80;

	return true;
}

static bool Cmd_AHammerKey_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;
	if(keycode%256==255&&keycode<2048) keycode=255+(keycode+1)/256;
    if(IsKeycodeValid(keycode)) DI_data.AHammerStates[keycode]=0x80;

	return true;
}

static bool Cmd_UnHammerKey_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;
	if(keycode%256==255&&keycode<2048) keycode=255+(keycode+1)/256;
	if(IsKeycodeValid(keycode)) {
		DI_data.HammerStates[keycode]=0x00;
		DI_data.AHammerStates[keycode]=0x00;
	}

	return true;
}

static bool Cmd_DisableKey_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;
	if(keycode%256==255&&keycode<2048) keycode=255+(keycode+1)/256;
    if(IsKeycodeValid(keycode)) DI_data.DisallowStates[keycode]=0x00;

	return true;
}

static bool Cmd_EnableKey_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;
	if(keycode%256==255&&keycode<2048) keycode=255+(keycode+1)/256;
    if(IsKeycodeValid(keycode)) DI_data.DisallowStates[keycode]=0x80;

	return true;
}

static bool Cmd_GetNumKeysPressed_Execute(COMMAND_ARGS)
{
	DWORD count=0;
	for(DWORD d=0;d<256;d++) if(IsKeycodeValid(d)&&DI_data.LastBytes[d]) count++;
	*result = count;
	return true;
}
static bool Cmd_GetKeyPress_Execute(COMMAND_ARGS)
{
	*result = 0xFFFF;
	UInt32 count=0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &count)) return true;
	for(DWORD d=0;d<256;d++) if(IsKeycodeValid(d)&&DI_data.LastBytes[d]&&(!count--)) {
		*result = d;
		break;
	}
	return true;
}
static bool Cmd_GetNumMouseButtonsPressed_Execute(COMMAND_ARGS)
{
	DWORD count=0;
	//Include mouse wheel? Probably not...
	for(DWORD d=256;d<kMaxMacros -2;d++) if(IsKeycodeValid(d)&&DI_data.LastBytes[d]) count++;
	*result = count;
	return true;
}
static bool Cmd_GetMouseButtonPress_Execute(COMMAND_ARGS)
{
	*result = 0xFFFF;
	UInt32 count=0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &count)) return true;
	for(DWORD d=256;d<kMaxMacros - 2;d++) if(DI_data.LastBytes[d]&&(!count--)) {
		*result = d;
		break;
	}
	return true;
}
static bool Cmd_MoveMouseX_Execute(COMMAND_ARGS)
{
	*result = 0;
	int pixels = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &pixels)) return true;
    DI_data.MouseXMov+=pixels;

	return true;
}

static bool Cmd_MoveMouseY_Execute(COMMAND_ARGS)
{
	*result = 0;
	int pixels = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &pixels)) return true;
    DI_data.MouseYMov+=pixels;

	return true;
}


static bool Cmd_SetMouseSpeedX_Execute(COMMAND_ARGS)
{
	*result = 0;
	float speed = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &speed)) return true;
    DI_data.MouseXSpeed=speed;

	return true;
}

static bool Cmd_SetMouseSpeedY_Execute(COMMAND_ARGS)
{
	*result = 0;
	float speed = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &speed)) return true;
    DI_data.MouseYSpeed=speed;

	return true;
}

static bool Cmd_DisableMouse_Execute(COMMAND_ARGS)
{
	*result=0;
    DI_data.MouseDisable=true;

	return true;
}

static bool Cmd_EnableMouse_Execute(COMMAND_ARGS)
{
	*result=0;
    DI_data.MouseDisable=false;

	return true;
}

#define VK_TABLE_SIZE 212
#define NOKEY 0xFFFF

static const UINT VKTable[VK_TABLE_SIZE] = {	NOKEY, 27, 49, 50, 51, 52, 53, 54, 55, 56, 57, 48, 189, 187, 8, 9, 81, 87, 69, 
								   		82, 84, 89, 85, 73, 79, 80, 219, 221, 13, 162, 65, 83, 68, 70, 71, 72, 74, 
          								75, 76, 186, 222, 192, 160, 220, 90, 88, 67, 86, 66, 78, 77, 188, 190, 191, 
            							161, 106, 164, 32, 20, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 144, 
										145, 103, 104, 105, 109, 100, 101, 102, 107, 97, 98, 99, 96, 110, NOKEY, NOKEY,
										NOKEY, 122, 123, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY,
										NOKEY, NOKEY, 124, 125, 126, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, 
										NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY,
										NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY,
										NOKEY, NOKEY, NOKEY, NOKEY,	NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY,
										NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY,
										NOKEY, NOKEY, 13, 163, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY,
										NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY,
										NOKEY, NOKEY, 108, NOKEY, 111, NOKEY, NOKEY, 165, NOKEY, NOKEY, NOKEY, NOKEY,
										NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, NOKEY, 36, 38,
										33, NOKEY, 37, NOKEY, 39, NOKEY, 35, 40, 34, 45, 46};


static UINT _dx2vk(UINT dx){
	if (dx >= VK_TABLE_SIZE) return NOKEY;
	return VKTable[dx];
}

static bool _isKeyPressed(UINT keyCode)
{
	if (keyCode < 255)	//use IsKeyPressed
	{
		keyCode = _dx2vk(keyCode);
		if (!(keyCode == NOKEY))		return (GetAsyncKeyState(keyCode) & 0x8000) ? true : false;
	}
	else	//use IsKeyPressed2
	{
		//code below recognizes 255 and 256 for LMB - intentional?
		if (keyCode % 256 == 255 && keyCode < 2048)	keyCode = 255 + (keyCode + 1) / 256;
		if (keyCode < kMaxMacros)	return DI_data.LastBytes[keyCode] ? true : false;
	}

	return false;
}

static bool _isControlPressed(UINT ctrl)
{
	if (ctrl >= CONTROLSMAPPED)	return false;
	if (!InputControls)		GetControlMap();
	
	if (_isKeyPressed(InputControls[ctrl]))	return true;
	if (_isKeyPressed(AltInputControls[ctrl]))	return true;

	return false;
}

static bool Cmd_IsKeyPressed3_Execute(COMMAND_ARGS)
{
	*result = 0;
	UINT keyCode = NOKEY;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keyCode)) return true;
	if (_isKeyPressed(keyCode))	*result = 1;
	return true;
}

static bool Cmd_IsControlPressed_Execute(COMMAND_ARGS)
{
	*result = 0;
	UINT ctrl;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &ctrl))	return true;
	if (_isControlPressed(ctrl))	*result = 1;
	return true;
}

static bool Cmd_DisableControl_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	ctrl = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &ctrl)) return true;

	
	if (!InputControls)
		GetControlMap();

	if (IsKeycodeValid(InputControls[ctrl]))	DI_data.DisallowStates[InputControls[ctrl]] = 0x00;
	ctrl = AltInputControls[ctrl];
	if (ctrl%256==255 && ctrl < 2048)	ctrl = 255 + (ctrl + 1) / 256;
	if (IsKeycodeValid(ctrl))	DI_data.DisallowStates[ctrl] = 0x00;

	return true;
}

static bool Cmd_EnableControl_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	ctrl = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &ctrl)) return true;

	if (!InputControls)
		GetControlMap();

	if (IsKeycodeValid(InputControls[ctrl]))	DI_data.DisallowStates[InputControls[ctrl]] = 0x80;
	ctrl = AltInputControls[ctrl];
	if (ctrl%256==255 && ctrl < 2048)	ctrl = 255 + (ctrl + 1) / 256;
	if (IsKeycodeValid(ctrl))	DI_data.DisallowStates[ctrl] = 0x80;

	return true;
}

static bool Cmd_OnKeyDown_Execute(COMMAND_ARGS)
{
	// key is refID, data is a set of key events that have been returned for that script
	static std::map< UINT, std::set<UINT> > KeyListeners;
	UINT keyCode = 0;
	*result = 0;

	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keyCode))	return true;

	if (scriptObj)
	{
		std::set<UINT>	* keyList = &KeyListeners[scriptObj->refID];

		if (_isKeyPressed(keyCode))
		{
			if (keyList->find(keyCode) == keyList->end())
			{
				keyList->insert(keyCode);
				*result = 1;
			}
		}
		else if (keyList->find(keyCode) != keyList->end())
		{
			keyList->erase(keyCode);
		}
	}

	return true;
}

static bool Cmd_OnControlDown_Execute(COMMAND_ARGS)
{
	// key is refID, data is a set of key events that have been returned for that script
	static std::map< UINT, std::set<UINT> > CtrlListeners;
	UINT ctrl = 0;
	*result = 0;

	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &ctrl))	return true;

	if (scriptObj)
	{
		std::set<UINT> *	ctrlList = &CtrlListeners[scriptObj->refID];

		if (_isControlPressed(ctrl))
		{
			if (ctrlList->find(ctrl) == ctrlList->end())
			{
				ctrlList->insert(ctrl);
				*result = 1;
			}
		}
		else if (ctrlList->find(ctrl) != ctrlList->end())
		{
			ctrlList->erase(ctrl);
		}
	}

	return true;
}

static bool Cmd_TapControl_Execute(COMMAND_ARGS)
{
	//returns false if control is not assigned
	*result = 0;
	UINT ctrl = 0;
	UINT keyCode = 0;

	if (!(ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &ctrl)))	return true;

	if (ctrl >= CONTROLSMAPPED)	return true;
	if (!InputControls)			GetControlMap();

	keyCode = InputControls[ctrl];
    if (IsKeycodeValid(keyCode))
	{
		DI_data.TapStates[keyCode] = 0x80;
		*result = 1;
	}
	else
	{
		keyCode = AltInputControls[ctrl];
		if (keyCode % 256 == 255 && keyCode < 2048) keyCode = 255 + (keyCode + 1) / 256;
		if (IsKeycodeValid(keyCode))
		{
			DI_data.TapStates[keyCode] = 0x80;
			*result = 1;
		}
	}

	return true;
}

static bool Cmd_RefreshControlMap_Execute(COMMAND_ARGS)
{
	//This is a MAJOR hack. Need to find control map in memory to do it properly

	static const UInt32 timeOut = 90;		//time-out between function calls, in seconds
	static clock_t lastCallTime = 0;

	if(!lastCallTime || (clock() - lastCallTime) / CLOCKS_PER_SEC > timeOut || clock() < lastCallTime)
	{
		lastCallTime = clock();
		RunScriptLine("con_SaveIni");
		GetControlMap();
	}

	return true;
}

#endif


CommandInfo kCommandInfo_GetControl =
{
	"GetControl",
	"gc",
	0,
	"Get the key which is used for a particular control",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_GetControl_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetAltControl =
{
	"GetAltControl",
	"gac",
	0,
	"Get the second key which is used for a particular control",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_GetAltControl_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsKeyPressed =
{
	"IsKeyPressed",
	"ikp",
	0,
	"return if a virtual keycode is down or up",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_IsKeyPressed_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsKeyPressed2 =
{
	"IsKeyPressed2",
	"ikp2",
	0,
	"return if a dx scancode is down or up",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_IsKeyPressed2_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_TapKey =
{
	"TapKey",
	"tk",
	0,
	"Fakes a key press for one frame",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_TapKey_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MenuTapKey =
{
	"MenuTapKey",
	"mtk",
	0,
	"Fakes a key press for one frame in menu mode",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MenuTapKey_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_HoldKey =
{
	"HoldKey",
	"hk",
	0,
	"Fakes a key press indefinately",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_HoldKey_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_ReleaseKey =
{
	"ReleaseKey",
	"rk",
	0,
	"Releases a key held down by HoldKey",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_ReleaseKey_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MenuHoldKey =
{
	"MenuHoldKey",
	"mhk",
	0,
	"Fakes a key press indefinately in menu mode",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MenuHoldKey_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MenuReleaseKey =
{
	"MenuReleaseKey",
	"mrk",
	0,
	"Releases a key held down by MenuHoldKey",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MenuReleaseKey_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_HammerKey =
{
	"HammerKey",
	"hk",	//Duplicate. Does it matter?
	0,
	"Fakes key presses in alternate frames",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_HammerKey_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_AHammerKey =
{
	"AHammerKey",
	"ahk",
	0,
	"Fakes key presses in alternate frames",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_AHammerKey_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_UnHammerKey =
{
	"UnhammerKey",
	"uhk",
	0,
	"Releases a key being hammered by HammerKey or AHammerKey",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_UnHammerKey_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_DisableKey =
{
	"DisableKey",
	"dk",
	0,
	"Prevents a player from using a key",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_DisableKey_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_EnableKey =
{
	"EnableKey",
	"ek",
	0,
	"Reenables a key previously disabled with DisableKey",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_EnableKey_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNumKeysPressed =
{
	"GetNumKeysPressed",
	"gnkp",
	0,
	"Returns how many keyboard keys are currently being held down",
	0,
	0,
	0,
	HANDLER(Cmd_GetNumKeysPressed_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetKeyPress =
{
	"GetKeyPress",
	"gkp",
	0,
	"Returns the scancode of the n'th key which is currently being held down",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_GetKeyPress_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNumMouseButtonsPressed =
{
	"GetNumMouseButtonsPressed",
	"gnmbp",
	0,
	"Returns how many mouse buttons are currently being held down",
	0,
	0,
	0,
	HANDLER(Cmd_GetNumMouseButtonsPressed_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMouseButtonPress =
{
	"GetMouseButtonPress",
	"gmbp",
	0,
	"Returns the code of the n'th mouse button which is currently being held down",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_GetMouseButtonPress_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MoveMouseX =
{
	"MoveMouseX",
	"mmx",
	0,
	"Fakes a mouse movement x pixels along the x axis",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MoveMouseX_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MoveMouseY =
{
	"MoveMouseY",
	"mmy",
	0,
	"Fakes a mouse movement x pixels along the y axis",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MoveMouseY_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetMouseSpeedX =
{
	"SetMouseSpeedX",
	"smsx",
	0,
	"Moves the mouse x pixels per second along the x axis",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_SetMouseSpeedX_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetMouseSpeedY =
{
	"SetMouseSpeedY",
	"smsy",
	0,
	"Moves the mouse x pixels per second along the y axis",
	0,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_SetMouseSpeedY_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_DisableMouse =
{
	"DisableMouse",
	"dm",
	0,
	"Disables mouse x/y axis movement",
	0,
	0,
	0,
	HANDLER(Cmd_DisableMouse_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_EnableMouse =
{
	"EnableMouse",
	"em",
	0,
	"Enables the mouse after it has been disabled by DisableMouse",
	0,
	0,
	0,
	HANDLER(Cmd_EnableMouse_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

/**********************************
New input functions params (scruggs)
**********************************/

CommandInfo kCommandInfo_IsKeyPressed3 =
{
	"IsKeyPressed3",
	"ikp3",
	0,
	"returns true if key/button pressed, even when disabled",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_IsKeyPressed3_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsControlPressed =
{
	"IsControlPressed",
	"ICP",
	0,
	"returns true if the key or button assigned to control is pressed",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_IsControlPressed_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_DisableControl =
{
	"DisableControl",
	"dc",
	0,
	"disables the key and button bound to a control",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_DisableControl_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_EnableControl =
{
	"EnableControl",
	"ec",
	0,
	"enables the key and button assigned to a control",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_EnableControl_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OnKeyDown =
{
	"OnKeyDown",
	"okd",
	0,
	"returns true each time the key is depressed",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_OnKeyDown_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OnControlDown =
{
	"OnControlDown",
	"ocd",
	0,
	"returns true each time the key or button assigned to control is depressed",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_OnControlDown_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_TapControl =
{
	"TapControl",
	"tc",
	0,
	"taps the key or mouse button assigned to control",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_TapControl_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_RefreshControlMap =
{
	"RefreshControlMap", "",
	0,
	"refreshes the control map from Oblivion.ini",
	0,
	0,
	NULL,
	HANDLER(Cmd_RefreshControlMap_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};
