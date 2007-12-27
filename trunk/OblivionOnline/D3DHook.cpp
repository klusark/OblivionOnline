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
FARPROC WINAPI MyGetProcAddress( HMODULE hModule,LPCSTR lpProcName)
{
	FARPROC retVal = NULL;
	if(_strnicmp(lpProcName, "Direct3DCreate9", 15) == 0)
	{
		// We are loading Direct3DCreate9
		retVal = (FARPROC)MyDirect3DCreate9;
		
		OldDirect3DCreate9 = RealGetProcAddress(hModule,lpProcName);		
	}
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

}
