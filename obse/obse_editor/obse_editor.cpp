#include "obse/obse.h"
#include "obse/CommandTable.h"
#include "EditorHookWindow.h"
#include "common/IThread.h"
#include "obse_common/SafeWrite.h"
#include "richedit.h"

IDebugLog	gLog("obse_editor.log");

IThread	hookWindowThread;

static void HookWindowThread(void * param)
{
	_MESSAGE("create hook window");
	EditorHookWindow	* window = new EditorHookWindow;

	window->PumpEvents();

	_MESSAGE("hook window closed");

	delete window;
}

static void CreateHookWindow(void)
{
	hookWindowThread.Start(HookWindowThread);
}

static HANDLE	fontHandle;
static LOGFONT	fontInfo;
static bool		userSetFont = false;

static void DoModScriptWindow(HWND wnd)
{
	SendMessage(wnd, EM_EXLIMITTEXT, 0, 0x00FFFFFF);

	bool	setFont = false;

	if(GetAsyncKeyState(VK_F11))
	{
		LOGFONT		newFontInfo = fontInfo;
		CHOOSEFONT	chooseInfo = { sizeof(chooseInfo) };
		
		chooseInfo.lpLogFont = &newFontInfo;
		chooseInfo.Flags = CF_INITTOLOGFONTSTRUCT | CF_NOVERTFONTS | CF_SCREENFONTS;

		if(ChooseFont(&chooseInfo))
		{
			HANDLE	newFont = CreateFontIndirect(&newFontInfo);
			if(newFont)
			{
				DeleteObject(fontHandle);

				fontInfo = newFontInfo;
				fontHandle = newFont;
			}
			else
				_WARNING("couldn't create font");
		}

		setFont = true;
	}

	if(GetAsyncKeyState(VK_F12) || setFont || userSetFont)
	{
		userSetFont = true;
		SendMessage(wnd, EM_SETTEXTMODE, TM_PLAINTEXT, 0);
		SendMessage(wnd, WM_SETFONT, (WPARAM)fontHandle, 1);

		UInt32	tabStopSize = 16;
		SendMessage(wnd, EM_SETTABSTOPS, 1, (LPARAM)&tabStopSize);	// one tab stop every 16 dialog units
	}
}

static __declspec(naked) void ModScriptWindow() {
	__asm {
		pushad
		push eax
		call DoModScriptWindow
		add esp, 4
		popad
		pop ecx
		push 0x030000
		push 0x00
		push ecx
		ret
	}
}

static UInt32 pModScriptWindow = (UInt32)&ModScriptWindow;

// copied from a call to ChooseFont
static const LOGFONT kLucidaConsole9 =
{
	-12,	// height
	0,		// width
	0,		// escapement
	0,		// orientation
	400,	// weight
	0,		// italic
	0,		// underline
	0,		// strikeout
	0,		// charset
	3,		// out precision
	2,		// clip precision
	1,		// quality
	49,		// pitch and family
	"Lucida Console"
};

static void FixEditorFont(void)
{
	// try something nice, otherwise fall back on SYSTEM_FIXED_FONT
	fontHandle = CreateFontIndirect(&kLucidaConsole9);
	if(fontHandle)
	{
		fontInfo = kLucidaConsole9;
	}
	else
	{
		fontHandle = GetStockObject(SYSTEM_FIXED_FONT);
		GetObject(fontHandle, sizeof(fontInfo), &fontInfo);
	}

#if CS_VERSION == CS_VERSION_1_0
	UInt32	basePatchAddr = 0x004F4491;
#elif CS_VERSION == CS_VERSION_1_2
	UInt32	basePatchAddr = 0x004FEAB9;
#else
#error unhandled cs version
#endif

	SafeWrite8(basePatchAddr + 0,	0xFF);
	SafeWrite8(basePatchAddr + 1,	0x15);
	SafeWrite32(basePatchAddr + 2,	(UInt32)&pModScriptWindow);
	SafeWrite8(basePatchAddr + 6,	0x90);
}

extern "C" {

void OBSE_Initialize(void)
{
	__try {
		_MESSAGE("OBSE editor: initialize (version = %d %08X)", OBSE_VERSION_INTEGER, CS_VERSION);

		FixEditorFont();

		CommandTable::Init();

		// this is very very helpful for debugging and analyzing data formats
		// but completely useless for end users, so it's #if 0'd out
#if 0
		CreateHookWindow();
#endif
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		_ERROR("exception");
	}
}

void OBSE_DeInitialize(void)
{
	DeleteObject(fontHandle);

	_MESSAGE("OBSE: deinitialize");
}

};
