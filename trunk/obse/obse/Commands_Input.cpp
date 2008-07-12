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
#include "GameOSDepend.h"
#include "StringVar.h"
#include "GameMenus.h"
#include "GameTiles.h"
#include "GameObjects.h"
#include <string>

#define CONTROLSMAPPED 29
static bool IsKeycodeValid(UInt32 id)		{ return id < kMaxMacros - 2; }

//Roundabout way of getting a pointer to the array containing control map
//Not sure what CtrlMapBaseAddr points to (no RTTI) so use brute pointer arithmetic
#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
static const UInt32* CtrlMapBaseAddr = (UInt32*)0x00AEAAB8;
static const UInt32	 CtrlMapOffset   = 0x00000020;
static const UInt32  CtrlMapOffset2  = 0x00001B7E;
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
static const UInt32* CtrlMapBaseAddr = (UInt32*)0x00B33398;
static const UInt32	 CtrlMapOffset   = 0x00000020;
static const UInt32  CtrlMapOffset2  = 0x00001B7E;
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
static const UInt32* CtrlMapBaseAddr = (UInt32*)0x00B33398;
static const UInt32	 CtrlMapOffset   = 0x00000020;
static const UInt32  CtrlMapOffset2  = 0x00001B7E;
#endif

UInt8*  InputControls=0;
UInt8*  AltInputControls=0;

static void GetControlMap()
{
	UInt32 addr = *CtrlMapBaseAddr + CtrlMapOffset;
	addr = *(UInt32*)addr + CtrlMapOffset2;
	InputControls = (UInt8*)addr;
	AltInputControls = InputControls + CONTROLSMAPPED;
}
static UInt32 GetCurrentControl(UInt32 keycode, bool bUseAlt)
{	//return the control to which keycode is currently mapped
	UInt8* controls = InputControls;
	if (bUseAlt)
		controls = AltInputControls;

	for (UInt32 i = 0; i < CONTROLSMAPPED; i++)
	{
		if (controls[i] == keycode)
			return i;
	}
	return -1;	//not mapped
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

static bool Cmd_GetAltControl2_Execute(COMMAND_ARGS)
{
	*result = 0xFFFF;
	UInt32 keycode = 0;

	if (ExtractArgs(EXTRACT_ARGS, &keycode) && keycode < CONTROLSMAPPED)
	{
		if (!InputControls)
			GetControlMap();
		if (AltInputControls[keycode] != 0xFF)	//0xFF = unassigned
			*result = AltInputControls[keycode] + 256;
	}

	return true;
}

static bool Cmd_SetControl_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 keycode = 0;
	UInt32 whichControl = 0;

	if (ExtractArgs(PASS_EXTRACT_ARGS, &whichControl, &keycode) && whichControl < CONTROLSMAPPED)
	{
		if (!InputControls)
			GetControlMap();

		UInt32 curControl = GetCurrentControl(keycode, false);
		if (curControl != -1)
			InputControls[curControl] = InputControls[whichControl];	//swap control mappings

		InputControls[whichControl] = keycode;
	}
	return true;
}

static bool Cmd_SetAltControl_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 keycode = 0;
	UInt32 whichControl = 0;

	if (ExtractArgs(PASS_EXTRACT_ARGS, &whichControl, &keycode) && whichControl < CONTROLSMAPPED && keycode > 255)
	{
		if (!InputControls)
			GetControlMap();

		UInt32 curControl = GetCurrentControl(keycode, true);
		if (curControl != -1)
			AltInputControls[curControl] = AltInputControls[whichControl];

		AltInputControls[whichControl] = keycode - 256;
	}
	return true;
}

//deprecated
static bool Cmd_GetAltControl_Execute(COMMAND_ARGS)
{
	*result=0xFFFF;

	UInt32	keycode = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &keycode)) return true;

	if(keycode>=CONTROLSMAPPED) return true;

	if(!InputControls) GetControlMap();

	*result = ((AltInputControls[keycode] * 256) + 255);
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
#define NOKEY 0xFF

//MapVirtualKey() doesn't map arrowpad keys/some numpad keys
//macro isn't pretty but shortens a long switch block
#define DX2VK(keyConstant) case DIK_ ## keyConstant: vkCode = VK_ ## keyConstant; break;

static UInt8 _dx2vk(UINT dx){
	if (dx >= VK_TABLE_SIZE) 
		return NOKEY;

	UInt8 vkCode = NOKEY;
	HKL kbLayout = GetKeyboardLayout(0);
	vkCode = MapVirtualKeyEx(dx, 3, kbLayout);
	if (!vkCode)
	{
		switch (dx)
		{
			DX2VK(DIVIDE);
			DX2VK(RCONTROL);
			DX2VK(RMENU);
			DX2VK(HOME);
			DX2VK(PRIOR);
			DX2VK(UP);
			DX2VK(DOWN);
			DX2VK(LEFT);
			DX2VK(RIGHT);
			DX2VK(END);
			DX2VK(NEXT);
			DX2VK(INSERT);
			DX2VK(DELETE);

			case DIK_NUMPADENTER:
				vkCode = VK_SEPARATOR;
				break;
			default:
				vkCode = NOKEY;
		}
	}

	return vkCode;
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
	
	if (_isKeyPressed(InputControls[ctrl]))	
		return true;
	if (AltInputControls[ctrl] != NOKEY && _isKeyPressed(AltInputControls[ctrl] + 256))	
		return true;

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

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &ctrl))
		return true;
	
	if (!InputControls)
		GetControlMap();

	if (IsKeycodeValid(InputControls[ctrl]))	
		DI_data.DisallowStates[InputControls[ctrl]] = 0x00;

	ctrl = AltInputControls[ctrl] + 256;
	if (IsKeycodeValid(ctrl))	
		DI_data.DisallowStates[ctrl] = 0x00;

	return true;
}

static bool Cmd_EnableControl_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32	ctrl = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &ctrl)) return true;

	if (!InputControls)
		GetControlMap();

	if (IsKeycodeValid(InputControls[ctrl]))	
		DI_data.DisallowStates[InputControls[ctrl]] = 0x80;

	ctrl = AltInputControls[ctrl] + 256;
	if (IsKeycodeValid(ctrl))	
		DI_data.DisallowStates[ctrl] = 0x80;

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
		keyCode = AltInputControls[ctrl] + 256;
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
	//DEPRECATED now that we're looking up control map directly
	return true;
}

class TextInputBox
{
	TextInputBox(char* fmtString, UInt16 maxLen, Script* callingScript, TESObjectREFR* callingObj);

	static TextInputBox		* s_singleton;
	UInt16					m_maxInputLen;
	UInt16					m_cursorPos;
	DWORD					m_cursorTimer;
	DWORD					m_cursorDelay;
	char					m_cursorChar;
	std::string				m_promptText;
	std::string				m_inputText;
	BYTE					m_prevKeyboardState[256];
	bool					bBackgroundKeyboardInitialState;

	bool	BlinkCursor();		//returns true if cursor char changed
	bool	IsValidInputKey(UInt32 keyCode);

public:
	static UInt32	s_MAX_LEN;

	static TextInputBox * GetTextBox()	
		{	return s_singleton;	}
	static bool Create(char * fmtString, UInt16 maxLen, Script* callingScript, TESObjectREFR* callingObj);
	void GetInputString(char* outString);	
	void Update();
	void Close();
};

TextInputBox* TextInputBox::s_singleton = NULL;
UInt32 TextInputBox::s_MAX_LEN = 4096;			//TODO: prevent text from running off bottom of screen (check menu height?)

void TextInputBox::Close()
{
	delete s_singleton;
	s_singleton = NULL;

	//Reset adjusted ini setting bBackgroundKeyboard
}

bool TextInputBox::BlinkCursor()
{
	if (!m_cursorTimer)
	{
		m_cursorTimer = GetTickCount();
		m_cursorChar = '|';
	}

	DWORD elapsedTime = GetTickCount();
	if (elapsedTime - m_cursorTimer > m_cursorDelay)
	{
		m_cursorTimer = elapsedTime;
		m_cursorChar = (m_cursorChar == '|') ? 127 : '|';
		m_inputText.replace(m_cursorPos, 1, 1, m_cursorChar);

		return true;
	}
	else
		return false;
}

bool TextInputBox::Create(char* fmtString, UInt16 maxLen, Script* callingScript, TESObjectREFR* callingObj)
{
	if (s_singleton)		//already initialized
		return false;
	else
	{
		s_singleton = new TextInputBox(fmtString, maxLen, callingScript, callingObj);
		return true;
	}

	//Adjust some settings for text input
	//TODO: SetIniSetting bBackgroundKeyboard 1

	//Disable menu navigation via arrow keys
		//well bleh, DI_data.DisallowStates doesn't work...edit the settings in MessageText's ValueList?
}

TextInputBox::TextInputBox(char* fmtString, UInt16 maxLen, Script* callingScript, TESObjectREFR* callingObj)
	: m_maxInputLen(maxLen), m_cursorPos(0), m_promptText(""), m_inputText(""), m_cursorTimer(0), m_cursorDelay(500)
{
	//initialize keyboard state array
	for (UInt32 i = 0; i < 256; i++)
		m_prevKeyboardState[i] = GetAsyncKeyState(i);

	char separatorChar = '@';	// '|' doesn't work in console, substitute for testing
	char* buttons[10] = { NULL };
	UInt32 numButtons = 0;
	UInt32 fmtStringLen = strlen(fmtString);

	//separate prompt text and button text
	for (UInt32 strPos = 0; strPos < fmtStringLen && numButtons < 10; strPos++)
	{
		if (fmtString[strPos] == separatorChar && (strPos + 1 < fmtStringLen))
		{
			fmtString[strPos] = '\0';
			buttons[numButtons++] = fmtString + strPos + 1;
		}
	}

	m_promptText = fmtString;
	if (!buttons[0])		//supply default button
		buttons[0] = "Finished";

	UInt32 scriptRefID = callingScript->refID;
	if (callingObj && (callingObj->flags & 0x00004000))	//not a temporary script
		scriptRefID = callingObj->refID;

	ShowMessageBox(fmtString, ShowMessageBox_Callback, 0, buttons[0], buttons[1], buttons[2], buttons[3], buttons[4],
		buttons[5], buttons[6], buttons[7], buttons[8], buttons[9]);

	*ShowMessageBox_pScriptRefID = scriptRefID;
	*ShowMessageBox_button = -1;
}

void TextInputBox::GetInputString(char* outString)	
{ 
	std::string str = m_inputText;
	str.erase(m_cursorPos, 1);
	strcpy_s(outString, str.length() + 1, str.c_str());
}

void TextInputBox::Update()
{
	//TODO: prevent this running while console is open

	static bool bGotKeyboardLayout = false;
	static HKL keyboardLayout = 0;

	if (!bGotKeyboardLayout)
	{
		keyboardLayout = GetKeyboardLayout(0);
		bGotKeyboardLayout = true;
	}

	bool bUpdateText = false;

	//Synthesize lpKeyboardState param to pass to ToAsciiEx, and simultaneously check for keypresses
	UInt32 keyPressed = 0;
	BYTE keyboardState[256] = { 0 };
	for (UInt32 keyCode = 0; keyCode < 256; keyCode++)
	{
		UInt32 keyState = GetAsyncKeyState(keyCode);
		keyboardState[keyCode] = (keyState & 0x8000) ? 0x80 : 0x00;
		if (!keyPressed && (keyState & 0x8000))		//key is pressed
			if (!(m_prevKeyboardState[keyCode] & 0x80))	//and not previously pressed
				if (IsValidInputKey(keyCode))
					keyPressed = keyCode;
	}

	memcpy(m_prevKeyboardState, keyboardState, 256);

	switch (keyPressed)
	{
	case 0:
		break;
	case VK_RETURN:
		if (m_inputText.length() < m_maxInputLen)
		{
			m_inputText.insert(m_cursorPos++, 1, '\n');
			bUpdateText = true;
		}

		break;
	case VK_BACK:
		if (m_cursorPos > 0)
		{
			m_inputText.erase(--m_cursorPos, 1);
			bUpdateText = true;
		}

		break;
	case VK_DELETE:
		if (m_cursorPos < m_inputText.length())
		{
			m_inputText.erase(m_cursorPos + 1, 1);
			bUpdateText = true;
		}

		break;
	case VK_TAB:
		if (m_cursorPos + 5 < m_maxInputLen)
		{
			m_inputText.insert(m_cursorPos, 5, ' ');
			m_cursorPos += 5;
			bUpdateText = true;
		}

		break;
	case VK_LEFT:
		if (m_cursorPos > 0)
		{
			m_inputText.replace(m_cursorPos, 1, 1, m_inputText[m_cursorPos - 1]);
			m_inputText.replace(--m_cursorPos, 1, 1, m_cursorChar);
			bUpdateText = true;
		}
		break;
	case VK_RIGHT:
		if (m_cursorPos < m_inputText.length() - 1)
		{
			m_inputText.replace(m_cursorPos, 1, 1, m_inputText[m_cursorPos + 1]);
			m_inputText.replace(++m_cursorPos, 1, 1, m_cursorChar);
			bUpdateText = true;
		}
		break;
	default:		//map to ASCII if possible and insert char into input text
		{
			if (m_inputText.length() == m_maxInputLen)
				break;

			unsigned short asciiResult[2] = { 0 };
			UInt32 toAscii = ToAsciiEx(keyPressed, MapVirtualKeyEx(keyPressed, 0, keyboardLayout), 
				keyboardState, asciiResult, 0, keyboardLayout);

			if (toAscii == 1 && isprint(asciiResult[0]))
			{
				m_inputText.insert(m_cursorPos++, 1, asciiResult[0]);
				bUpdateText = true;
			}
		}
	}

	if (BlinkCursor())
		bUpdateText = true;

	if (bUpdateText)	//Update displayed text
	{
		std::string messageText = m_promptText + m_inputText;
		MessageMenu* msgBox = (MessageMenu*)GetMenuByType(kMenuType_Message);
		msgBox->messageText->SetStringValue(kTileValue_string, messageText.c_str());
		msgBox->messageText->UpdateField(kTileValue_string, 0.0, messageText.c_str());
	}
}

bool TextInputBox::IsValidInputKey(UInt32 keyCode)
{
	switch (keyCode)
	{
	case VK_SPACE:
		return true;
	case VK_RETURN:
		return true;
	case VK_BACK:
		return true;
	case VK_LEFT:
		return true;
	case VK_RIGHT:
		return true;
	case VK_TAB:
		return true;
	case VK_DELETE:
		return true;
	default:
		if (keyCode >= 0x41 && keyCode <= 0x5A)		//A..Z
			return true;
		else if (keyCode >= 0x30 && keyCode <= 0x39)	//0..9
			return true;
		else if (keyCode >= VK_OEM_1 && keyCode <= VK_OEM_3)	//various punctuation
			return true;
		else if (keyCode >= VK_OEM_4 && keyCode <= VK_OEM_7)	//various punctuation
			return true;
	}

	return false;
}

static bool Cmd_ShowTextInputBox_Execute(COMMAND_ARGS)
{
	*result = 0;

	if (TextInputBox::GetTextBox())
		return true;					//already in use

	char promptText[kMaxMessageLength] = { 0 };
	UInt32 maxLen = TextInputBox::s_MAX_LEN;

	if (ExtractFormatStringArgs(0, promptText, paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, 
		kCommandInfo_ShowTextInputBox.numParams, &maxLen))
	{
		if (TextInputBox::Create(promptText, maxLen, scriptObj, thisObj))
			*result = 1;
	}

	return true;
}

static bool Cmd_GetInputText_Execute(COMMAND_ARGS)
{
	*result = 0;
	double strID = 0;
	UInt8 strModIndex = 0;

	if (ExtractSetStatementVar(scriptObj, eventList, arg1, &strID, &strModIndex))
	{
		char inputText[kMaxMessageLength] = { 0 };
		TextInputBox* textBox = TextInputBox::GetTextBox();
		if (textBox)
			textBox->GetInputString(inputText);			

		StringVar* strVar = g_StringMap.Get(strID);
		if (strVar)
			strVar->Set(inputText);
		else
			strID = g_StringMap.Add(strModIndex, inputText);
	}

	*result = strID;
	return true;
}

static bool Cmd_UpdateTextInputBox_Execute(COMMAND_ARGS)
{
	TextInputBox* textBox = TextInputBox::GetTextBox();
	if (textBox)
		textBox->Update();

	return true;
}

static bool Cmd_CloseTextInputBox_Execute(COMMAND_ARGS)
{
	TextInputBox* textBox = TextInputBox::GetTextBox();
	if (textBox)
		textBox->Close();

	return true;
}

static bool Cmd_IsTextInputBoxInUse_Execute(COMMAND_ARGS)
{
	*result = TextInputBox::GetTextBox() ? 1 : 0;
	return true;
}

/*  TextEdit menu works, but has some limitations which will require editing the .xml file (bad) or
	hooking ReadXML to apply changes before rendering the menu (only slightly better); doesn't allow use of the Enter key or
	multiple line input either.
	Probably better to just to roll our own. Hooking ReadXML() will be useful later, though.

static const UInt32 s_MaxLineInputPromptLength = 256;

static bool Cmd_ShowLineInputBox_Execute(COMMAND_ARGS)
{
	if (GetMenuByType(kMenuType_TextEdit))
		return true;

	char promptText[s_MaxLineInputPromptLength] = { 0 };
	if (ExtractArgs(PASS_EXTRACT_ARGS, promptText))
	{
		if (InterfaceManager::GetSingleton()->CreateTextEditMenu(promptText, "DEFAULT"))
			Console_Print("Created text edit menu");
		else
			Console_Print("Creation failed");
		TextEditMenu* menu = (TextEditMenu*)GetMenuByType(kMenuType_TextEdit);
		const char* inputText = 0;
		menu->text->GetStringValue(kTileValue_string, &inputText);
		Console_Print("text = %08x %s InterfaceManager = %08x TileText = %08x", 
			inputText, inputText, InterfaceManager::GetSingleton(), menu->text);

	}

	return true;
}

static bool Cmd_GetInputLine_Execute(COMMAND_ARGS)
{
	TextEditMenu* menu = (TextEditMenu*)GetMenuByType(kMenuType_TextEdit);
	if (!menu)
		return true;

	*result = 0;
	double strID = 0;
	UInt8 strModIndex = 0;
	if(ExtractSetStatementVar(scriptObj, eventList, arg1, &strID, &strModIndex))
	{
		const char* text = NULL;
		menu->text->GetStringValue(kTileValue_string, &text);
		std::string textStr(text);
		UInt32 cursorPos = textStr.find('|');
		if (cursorPos == -1)
			cursorPos = textStr.find(127);
		
		if (cursorPos != -1)
		{
			textStr.erase(cursorPos, 1);
			text = textStr.c_str();
		}

		Console_Print("Text = %s", text);

		StringVar* strVar = g_StringMap.Get(strID);
		if (strVar)
			strVar->Set(text);
		else
			strID = g_StringMap.Add(strModIndex, text);

		*result = strID;
	}

	return true;
}
*/

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

DEFINE_COMMAND(GetAltControl2,
			   returns the mouse button code assigned to the specified control,
			   0,
			   1,
			   kParams_OneInt);

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

DEFINE_COMMAND(SetControl,
			   assigns a new keycode to the specified keyboard control,
			   0,
			   2,
			   kParams_TwoInts);

DEFINE_COMMAND(SetAltControl,
			   assigns a new mouse button code to the specified mouse control,
			   0,
			   2,
			   kParams_TwoInts);

/*
DEFINE_COMMAND(ShowLineInputBox,
			   opens a text input box for a single line of input,
			   0,
			   1,
			   kParams_OneString);

DEFINE_COMMAND(GetInputLine,
			   returns the users input,
			   0,
			   0,
			   NULL);
*/

static ParamInfo kParams_ShowTextInputBox[22] = 
{
	FORMAT_STRING_PARAMS,
	{	"maxLength",	kParamType_Integer,	1	},
};

DEFINE_COMMAND(ShowTextInputBox,
			   displays a text box for user input,
			   0,
			   22,
			   kParams_ShowTextInputBox);

DEFINE_COMMAND(UpdateTextInputBox,
			   updates text input box,
			   0,
			   0,
			   NULL);

DEFINE_COMMAND(GetInputText,
			   returns the users text input,
			   0,
			   0,
			   NULL);

DEFINE_COMMAND(CloseTextInputBox,
			   releases the text input box for other scripts to use,
			   0,
			   0,
			   NULL);

DEFINE_COMMAND(IsTextInputBoxInUse,
			   returns true if another script is using the text input box,
			   0,
			   0,
			   NULL);
