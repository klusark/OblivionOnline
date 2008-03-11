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
HRESULT WINAPI MyDirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut,
									LPUNKNOWN punkOuter)
{
	_MESSAGE( "DirectInput-Hook: MyDirectInput8Create called.\n" );
	HRESULT hr =old_DirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
	if(SUCCEEDED(hr))
		*ppvOut = new MyDirectInput8(reinterpret_cast<IDirectInput8*>(*ppvOut));
	return hr;
}
extern "C" int SetInputHooks()
{
	_MESSAGE("Setting up Input hooks");
	UINT HookedFunctions;
	HookImportedFunctionsByNameA(GetModuleHandle(0),"DINPUT8.DLL",1,DirectInput8CreateHook,(PROC *)old_DirectInput8Create,&HookedFunctions);
	hook = SetWindowsHookEx(WH_KEYBOARD,kbdhook,hDll,0);
	return false;
}
int UnSetInputHooks()
{
	UINT HookedFunctions;
	 HookImportedFunctionsByNameA(GetModuleHandle(0),"DINPUT8.DLL",1,RestoreHook2,(PROC *)old_DirectInput8Create,&HookedFunctions);
	 UnhookWindowsHookEx(hook);
	return false;
}
