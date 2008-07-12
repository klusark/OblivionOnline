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
#include "BugslayerUtil.h"
#include <d3d9.h>
#include "FakeD3D.H"
extern  "C" HINSTANCE hDll;
// From the Direct3D hooking tutorial modified
// Function pointer types.
typedef IDirect3D9* (WINAPI *Direct3DCreate9_t)(UINT sdk_version);
typedef FARPROC (WINAPI *GetProcAddress_Type)(HMODULE HModule,LPCSTR ProcName);


void * OldDirect3DCreate9 = NULL;

#pragma data_seg (".HookSection")		// This segment is shared by all instances of this .DLL
// Shared instance for all processes.
HHOOK hHook = NULL;	
#pragma data_seg ()
IDirect3D9 *OblivionDirect3D9 = NULL;
IDirect3DDevice9 *OblivionDirect3D9Device = NULL;
// Function prototypes.
IDirect3D9* WINAPI MyDirect3DCreate9(UINT sdk_version);
FARPROC WINAPI MyGetProcAddress( HMODULE hModule,LPCSTR lpProcName);
GetProcAddress_Type RealGetProcAddress;
UINT HookedFunctions;
// Hook structure.

HOOKFUNCDESC GetProcAddressHook[2] = 
{
{
	"GetProcAddress",
	(PROC)MyGetProcAddress,
}

};

HOOKFUNCDESC RestoreHook[2] = 
{
{
	"GetProcAddress",
	(PROC)RealGetProcAddress,
}
};

IDirect3D9* WINAPI MyDirect3DCreate9(UINT sdk_version)
{
	_MESSAGE("Direct3D interface was created via the hook");
	MyDirect3D9 *nd3d;
	Direct3DCreate9_t old_func = (Direct3DCreate9_t) OldDirect3DCreate9;
	IDirect3D9* d3d = old_func(sdk_version);
	nd3d =  d3d? new MyDirect3D9(d3d) : 0;
	OblivionDirect3D9 = nd3d;
	return nd3d;	
}
typedef HRESULT (WINAPI *DirectInput8Create_t)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf,
											   LPVOID * ppvOut, LPUNKNOWN punkOuter);
extern HRESULT WINAPI MyDirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut,
										   LPUNKNOWN punkOuter);
extern DirectInput8Create_t old_DirectInput8Create;
FARPROC WINAPI MyGetProcAddress( HMODULE hModule,LPCSTR lpProcName)
{
	FARPROC retVal = NULL;
	if(_strnicmp(lpProcName, "Direct3DCreate9", 15) == 0)
	{
		// We are loading Direct3DCreate9
		retVal = (FARPROC)MyDirect3DCreate9;
		
		OldDirect3DCreate9 = RealGetProcAddress(hModule,lpProcName);		
	}
	/*
	else if(_strnicmp(lpProcName,"DirectInput8Create",15))
	{
		retVal = (FARPROC)MyDirectInput8Create;
		old_DirectInput8Create = (DirectInput8Create_t)RealGetProcAddress(hModule,lpProcName);
	}*/
	else if(_strnicmp(lpProcName,"GetProcAddress",15) == 0)
	{
		retVal = (FARPROC) MyGetProcAddress;
	}
	else
	{
		// the process is trying to load a different function ... we give it out
		retVal = RealGetProcAddress(hModule,lpProcName);
	}
	return retVal;
}
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
    return CallNextHookEx( hHook, nCode, wParam, lParam); 
}

void D3DInstallHook()
{
	printf("Hooking");
	hHook = SetWindowsHookEx( WH_CBT, HookProc, hDll, 0 ); 
}

 void  D3DUnHook()
{
	printf("Unhooking");
    UnhookWindowsHookEx( hHook );
}
 
extern "C" void D3DHookInit()
{
	 _MESSAGE("Initializing Direct3D hook");
	HookImportedFunctionsByName(GetModuleHandle(0),"KERNEL32.DLL",1,GetProcAddressHook,(PROC *)&RealGetProcAddress,&HookedFunctions);

}


void D3DHookDeInit()
{
	 _MESSAGE("Deinitializing Direct3D hook");
	HookImportedFunctionsByName(GetModuleHandle(0),"KERNEL32.DLL",1,RestoreHook,(PROC *)&RealGetProcAddress,&HookedFunctions);
	RealGetProcAddress = GetProcAddress;
}