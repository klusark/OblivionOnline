#pragma once
#include <d3d9.h>

extern IDirect3D9 *OblivionDirect3D9 ;
extern IDirect3DDevice9 *OblivionDirect3D9Device;
#ifdef DIRECT3DHOOK_EXPORTS
	#define D3DHOOK_API __declspec(dllexport)
#else
	#define D3DHOOK_API __declspec(dllimport)
#endif
#ifdef __cplusplus
extern "C"
{
#endif
void D3DHookInit();
#ifdef __cplusplus
}
#endif
extern void D3DHookDeInit();

extern HHOOK hHook;	