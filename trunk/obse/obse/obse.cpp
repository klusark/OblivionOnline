#include "obse.h"
#include "CommandTable.h"
#include "Hooks_DirectInput8Create.h"
#include "Hooks_Gameplay.h"
#include "Hooks_Memory.h"
#include "Hooks_SaveLoad.h"
#include "Commands_Math.h"
#include "PluginManager.h"
#include "InternalSerialization.h"
//#include "Commands_Python.h"

IDebugLog	gLog("obse.log");

extern "C" {

void OBSE_Initialize(void)
{
#ifndef _DEBUG
	__try {
#endif
		_MESSAGE("OBSE: initialize (version = %d.%d %08X)", OBSE_VERSION_INTEGER, OBSE_VERSION_INTEGER_MINOR, OBLIVION_VERSION);

#ifdef _DEBUG
		SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);
#endif

		MersenneTwister::init_genrand(GetTickCount());

		CommandTable::Init();

		Hook_DirectInput8Create_Init();
		Hook_Gameplay_Init();
		Hook_Memory_Init();
		Hook_SaveLoad_Init();

		Init_CoreSerialization_Callbacks();

		FlushInstructionCache(GetCurrentProcess(), NULL, 0);

#if 0
		while(!IsDebuggerPresent())
		{
			Sleep(10);
		}

		Sleep(1000 * 2);
#endif

#ifndef _DEBUG
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		_ERROR("exception");
	}
#endif
}

void OBSE_DeInitialize(void)
{
	_MESSAGE("OBSE: deinitialize");

	g_pluginManager.DeInit();

	Hook_Memory_DeInit();

//	DeInitPython();
}

};
