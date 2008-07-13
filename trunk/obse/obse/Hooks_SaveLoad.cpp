#include "Hooks_SaveLoad.h"
#include "obse_common/SafeWrite.h"
#include "GameAPI.h"
#include "NiNodes.h"
#include "Serialization.h"

UInt32 g_gameLoaded = 0;

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

static const UInt32	kLoadGamePatchAddr =	0x0045FAA5;
static const UInt32	kLoadGameRetnAddr =		0x0045FAAA;
#define	kLoadGameEBPOffset					0x44

static const UInt32	kSaveGamePatchAddr =	0x00460813;
static const UInt32	kSaveGameRetnAddr =		0x00460818;

static const UInt32	kDeleteGamePatchAddr =	0x00453ED6;

static const UInt32	kRenameGamePatchAddr =	0x0045C1DA;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2

static const UInt32	kLoadGamePatchAddr =	0x00466833;
static const UInt32	kLoadGameRetnAddr =		0x00466838;
#define	kLoadGameEBPOffset					0x40

static const UInt32	kSaveGamePatchAddr =	0x004656D8;
static const UInt32	kSaveGameRetnAddr =		0x004656DD;

static const UInt32	kDeleteGamePatchAddr =	0x00453496;

static const UInt32	kRenameGamePatchAddr =	0x0045F600;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416

static const UInt32	kLoadGamePatchAddr =	0x00466995;
static const UInt32	kLoadGameRetnAddr =		0x0046699A;
#define	kLoadGameEBPOffset					0x40

static const UInt32	kSaveGamePatchAddr =	0x004657D6;
static const UInt32	kSaveGameRetnAddr =		0x004657DB;

static const UInt32	kDeleteGamePatchAddr =	0x004534A6;

static const UInt32	kRenameGamePatchAddr =	0x0045F5C0;

#else

#error unsupported oblivion version

#endif

// stdcall to make it easier to work with in asm
static void __stdcall DoLoadGameHook(BSFile * file)
{
	g_gameLoaded = 1;

	_MESSAGE("DoLoadGameHook: %s", file->m_path);

	Serialization::HandleLoadGame(file->m_path);
}

static __declspec(naked) void LoadGameHook(void)
{
	__asm
	{
		pushad
		push		esi		// esi = BSFile *
		call		DoLoadGameHook
		popad

		// overwritten code
		mov			ecx, [ebp + kLoadGameEBPOffset]
		test		ecx, ecx
		jmp			[kLoadGameRetnAddr]
	}
}

static void __stdcall DoSaveGameHook(BSFile * file)
{
	_MESSAGE("DoSaveGameHook: %s", file->m_path);

	Serialization::HandleSaveGame(file->m_path);
}

static __declspec(naked) void SaveGameHook(void)
{
	__asm
	{
		pushad
		push		esi		// esi = BSFile *
		call		DoSaveGameHook
		popad

		// overwritten code
		mov			ecx, [ebp + kLoadGameEBPOffset]
		test		ecx, ecx	// not all versions do exactly this, but they are all equivalent
		jmp			[kSaveGameRetnAddr]
	}
}

// overwriting a call to DeleteFileA
static void __stdcall DeleteGameHook(const char * path)
{
	_MESSAGE("DeleteGameHook: %s", path);

	Serialization::HandleDeleteGame(path);

	// overwritten code
	DeleteFile(path);
}

static void RenameGameHook(const char * oldPath, const char * newPath)
{
	_MESSAGE("RenameGameHook: %s -> %s", oldPath, newPath);

	Serialization::HandleRenameGame(oldPath, newPath);

	rename(oldPath, newPath);
}

void Hook_SaveLoad_Init(void)
{
	WriteRelJump(kLoadGamePatchAddr, (UInt32)&LoadGameHook);
	WriteRelJump(kSaveGamePatchAddr, (UInt32)&SaveGameHook);
	WriteRelCall(kDeleteGamePatchAddr, (UInt32)&DeleteGameHook);
	SafeWrite8(kDeleteGamePatchAddr + 5, 0x90);	// nop out the 6th byte of the patched instruction
	WriteRelCall(kRenameGamePatchAddr, (UInt32)&RenameGameHook);
}
