#include "Hooks_Gameplay.h"
#include "obse_common/SafeWrite.h"
#include "Hooks_Memory.h"

static void HandleMainLoopHook(void);

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

static const UInt32 kLoadGameHookPatchAddr = 0x0045EA62;
static const UInt32 kLoadGameHookRetnAddr = 0x0045EA67;
static const UInt32	kLoadGameHookPushValue = 0x00977417;

static const UInt32 kMainLoopHookPatchAddr = 0x0040EC8E;
static const UInt32 kMainLoopHookRetnAddr = 0x0040EC94;

static const UInt32	kRefIDBugfixRetnAddr = 0x00443E6D;

static __declspec(naked) void RefIDBugfix(void)
{
	__asm
	{
		mov	edx, [ebx+0x08B8]		// fetch the last-generated refid
		inc	edx						// increment it
		or	edx, 0xFF000000			// make sure we wrap from FFFFFFFF -> FF000000 instead of 00000000
		jmp	[kRefIDBugfixRetnAddr]	// done
	}
}

static void InstallRefIDBugfix(void)
{
	WriteRelJump(0x00443E66, (UInt32)&RefIDBugfix);
}

static __declspec(naked) void MainLoopHook(void)
{
	__asm
	{
		pushad
		call	HandleMainLoopHook
		popad
		mov		ecx, [edx + 0x280]
		jmp		[kMainLoopHookRetnAddr]
	}
}

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2

static const UInt32 kLoadGameHookPatchAddr = 0x00465762;
static const UInt32 kLoadGameHookRetnAddr = 0x00465767;
static const UInt32	kLoadGameHookPushValue = 0x009AE597;

static const UInt32 kMainLoopHookPatchAddr = 0x0040F16D;
static const UInt32 kMainLoopHookRetnAddr = 0x0040F173;

static const UInt32	kRefIDBugfixRetnAddr = 0x00448F76;

static __declspec(naked) void RefIDBugfix(void)
{
	__asm
	{
		inc	dword ptr [ebx+0x8C0]					// increment the current refid
		or	dword ptr [ebx+0x8C0], 0xFF000000		// make sure we wrap from FFFFFFFF -> FF000000 instead of 00000000
		jmp	[kRefIDBugfixRetnAddr]					// done
	}
}

static void InstallRefIDBugfix(void)
{
	WriteRelJump(0x00448F70, (UInt32)&RefIDBugfix);
}

static __declspec(naked) void MainLoopHook(void)
{
	__asm
	{
		pushad
		call	HandleMainLoopHook
		popad
		mov		eax, [edx + 0x280]			// ### check this
		jmp		[kMainLoopHookRetnAddr]
	}
}

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416

static const UInt32 kLoadGameHookPatchAddr = 0x00465862;
static const UInt32 kLoadGameHookRetnAddr = 0x00465867;
static const UInt32	kLoadGameHookPushValue = 0x009AE797;

static const UInt32 kMainLoopHookPatchAddr = 0x0040F19D;
static const UInt32 kMainLoopHookRetnAddr = 0x0040F1A3;

static __declspec(naked) void MainLoopHook(void)
{
	__asm
	{
		pushad
		call	HandleMainLoopHook
		popad
		mov		eax, [edx + 0x280]
		jmp		[kMainLoopHookRetnAddr]
	}
}

#else
#error unsupported oblivion version
#endif

UInt32	g_gameLoaded = 0;

static __declspec(naked) void LoadGameHook(void)
{
	__asm
	{
		mov		g_gameLoaded, 1
		push	kLoadGameHookPushValue	// the code we overwrote
		jmp		[kLoadGameHookRetnAddr]	// workaround for inability to specify immediate jumps in ms' assembler
	}
}

static void HandleMainLoopHook(void)
{
	// nothing yet

	Hook_Memory_CheckAllocs();
}

void Hook_Gameplay_Init(void)
{
	// entry to "LoadGame" function
	WriteRelJump(kLoadGameHookPatchAddr, (UInt32)&LoadGameHook);

	// game main loop
	// this address was chosen because it is only run when oblivion is in the foreground
	// ### we don't need this yet, disable to remove a potential bug source
//	WriteRelJump(kMainLoopHookPatchAddr, (UInt32)&MainLoopHook);

	// patch enchanted cloned item check
#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
	SafeWrite8(0x004590ED + 1, 0x20);	// more accurate to branch to 0045DED7
	WriteRelJump(0x004591EC, 0x00459275);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
	SafeWrite8(0x0045DEBD + 1, 0x20);	// more accurate to branch to 0045DED7
	WriteRelJump(0x0045DFBF, 0x0045E04A);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	SafeWrite8(0x0045DEAD + 1, 0x20);	// more accurate to branch to 0045DED7
	WriteRelJump(0x0045DFAF, 0x0045E03A);
#else
#error unsupported oblivion version
#endif

#if OBLIVION_VERSION < OBLIVION_VERSION_1_2_410
	// patch dynamic refid rollover bug
	InstallRefIDBugfix();
#endif
}
