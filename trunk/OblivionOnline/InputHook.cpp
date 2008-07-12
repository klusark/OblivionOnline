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
//these do not work .... probably ... we need to thunk DI8
#include <windows.h>
#include "UserInterface.h"
#include "BugSlayerUtil.h"
#include "main.h"
#include "inputhook.h"
#include "FakeDI8.h"
extern "C" HINSTANCE hDll; /* we need this for hooking the keyboard */
HHOOK hook;
typedef HRESULT (WINAPI *DirectInput8Create_t)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf,
										LPVOID * ppvOut, LPUNKNOWN punkOuter);
HRESULT WINAPI MyDirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut,
									LPUNKNOWN punkOuter);
DirectInput8Create_t old_DirectInput8Create;
bool bTypeMode;
WCHAR cIgnoredChars[20];
HOOKFUNCDESC DirectInput8CreateHook[2] = 
{
{
	"DirectInput8Create",
	(PROC)MyDirectInput8Create,
}

};

HOOKFUNCDESC RestoreHook2[2] = 
{
{
	"DirectInput8Create",
	(PROC)old_DirectInput8Create,
}
};

LRESULT CALLBACK kbdhook( int ncode,WPARAM wparam,LPARAM lparam)
{
	_MESSAGE("Win32 Hook called");
if(ncode>=0)
{
	BYTE KeyState[256];
	GetKeyboardState(KeyState);
	wchar_t Keys[20];
	for(int i=0;i<ToUnicode(wparam,(WORD)(lparam >> 16),KeyState,Keys,20,0);i++)
	{
		CEGUI::System::getSingleton().injectChar(Keys[i]);
	}

  }
  //pass control to next hook.
  return ( CallNextHookEx(hook,ncode,wparam,lparam) );
}
HRESULT WINAPI MyDirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, void ** ppvOut,
									LPUNKNOWN punkOuter)
{
	UInt32 ProtectionBuffer = 0;
	if(!ppvOut)
		return DIERR_INVALIDPARAM;
	
	VirtualProtect((void *)ppvOut,sizeof(void *),PAGE_EXECUTE_READWRITE,&ProtectionBuffer);
	VirtualProtect(*ppvOut,sizeof(void *),PAGE_EXECUTE_READWRITE,&ProtectionBuffer);
	void* temp;
	*ppvOut = NULL;
	_MESSAGE( "DirectInput-Hook: MyDirectInput8Create called.\n" );
	_MESSAGE( " Address for DirectInput8Create FAR %lu",old_DirectInput8Create);
	HRESULT hr =old_DirectInput8Create(hinst, dwVersion, riidltf, &temp, punkOuter);
	_MESSAGE("Created original device");
	if(SUCCEEDED(hr) && temp)
	{
		temp = new MyDirectInput8(temp);	
		VirtualProtect((void *)ppvOut,sizeof(void *),PAGE_EXECUTE_READWRITE,&ProtectionBuffer);
		VirtualProtect(*ppvOut,sizeof(void *),PAGE_EXECUTE_READWRITE,&ProtectionBuffer);
		_MESSAGE("Fixed memory protection");
		*ppvOut = temp;
		_MESSAGE("thunked pointer");
	}
	_MESSAGE("leaving myDirectInput8Create");
	return hr;
}
extern "C" int SetInputHooks()
{
	_MESSAGE("Setting up Input hooks");
	UINT HookedFunctions;
	HookImportedFunctionsByNameA(GetModuleHandle(0),"DINPUT8.DLL",1,DirectInput8CreateHook,(PROC *)old_DirectInput8Create,&HookedFunctions);
	//hook = SetWindowsHookEx(WH_KEYBOARD,kbdhook,hDll,0);
	return false;
}
int UnSetInputHooks()
{
	UINT HookedFunctions;
	 HookImportedFunctionsByNameA(GetModuleHandle(0),"DINPUT8.DLL",1,RestoreHook2,(PROC *)old_DirectInput8Create,&HookedFunctions);
	//UnhookWindowsHookEx(hook);
	return false;
}
CEGUI::utf32 keycodeToUTF32( unsigned int scanCode)
{
	CEGUI::utf32 utf = 0;

	BYTE keyboardState[256];
	unsigned char ucBuffer[3];
	static WCHAR deadKey = '\0';

	// Retrieve the keyboard layout in order to perform the necessary convertions
	HKL hklKeyboardLayout = GetKeyboardLayout(0); // 0 means current thread 
	// This seemingly cannot fail 
	// If this value is cached then the application must respond to WM_INPUTLANGCHANGE 

	// Retrieve the keyboard state
	// Handles CAPS-lock and SHIFT states
	if (GetKeyboardState(keyboardState) == FALSE)
		return utf;

	/* 0. Convert virtual-key code into a scan code
	1. Convert scan code into a virtual-key code
	Does not distinguish between left- and right-hand keys.
	2. Convert virtual-key code into an unshifted character value
	in the low order word of the return value. Dead keys (diacritics)
	are indicated by setting the top bit of the return value.
	3. Windows NT/2000/XP: Convert scan code into a virtual-key
	Distinguishes between left- and right-hand keys.*/
	UINT virtualKey = MapVirtualKeyEx(scanCode, 3, hklKeyboardLayout);
	if (virtualKey == 0) // No translation possible
		return utf;

	/* Parameter 5:
	0. No menu is active
	1. A menu is active
	Return values:
	Negative. Returned a dead key
	0. No translation available
	1. A translation exists 
	2. Dead-key could not be combined with character 	*/
	int ascii = ToAsciiEx(virtualKey, scanCode, keyboardState, (LPWORD) ucBuffer, 0, hklKeyboardLayout);
	if(deadKey != '\0' && ascii == 1)
	{
		// A dead key is stored and we have just converted a character key
		// Combine the two into a single character
		WCHAR wcBuffer[3];
		WCHAR out[3];
		wcBuffer[0] = ucBuffer[0];
		wcBuffer[1] = deadKey;
		wcBuffer[2] = '\0';
		if( FoldStringW(MAP_PRECOMPOSED, (LPWSTR) wcBuffer, 3, (LPWSTR) out, 3) )
			utf = out[0];
		else
		{
			// FoldStringW failed
			DWORD dw = GetLastError();
			switch(dw)
			{
			case ERROR_INSUFFICIENT_BUFFER:
			case ERROR_INVALID_FLAGS:
			case ERROR_INVALID_PARAMETER:
				break;
			}
		}
		deadKey = '\0';
	}
	else if (ascii == 1)
	{
		// We have a single character
		utf = ucBuffer[0];
		deadKey = '\0';
	}
	else
	{
		// Convert a non-combining diacritical mark into a combining diacritical mark
		switch(ucBuffer[0])
		{
		case 0x5E: // Circumflex accent: â
			deadKey = 0x302;
			break;
		case 0x60: // Grave accent: à
			deadKey = 0x300;
			break;
		case 0xA8: // Diaeresis: ü
			deadKey = 0x308;
			break;
		case 0xB4: // Acute accent: é
			deadKey = 0x301;
			break;
		case 0xB8: // Cedilla: ç
			deadKey = 0x327;
			break;
		default:
			deadKey = ucBuffer[0];
		}
	}

	return utf;
}