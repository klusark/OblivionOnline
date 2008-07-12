#include "Hooks_Gameplay.h"
#include "obse_common/SafeWrite.h"
#include "Hooks_Memory.h"
#include "Serialization.h"
#include "GameAPI.h"
#include "GameForms.h"
#include <share.h>

static void HandleMainLoopHook(void);

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

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

static const UInt32	kNewGamePatchAddr = 0x005A7727;

static const UInt32 QUIMsgPatchAddr = 0x0056DF90;
static const UInt8  QUIMsgData = 0x51;	//code overwritten by retn
static const UInt32 QUIMsg_2PatchAddr = 0x0056E0A0;
static const UInt8 QUIMsg_2Data = 0xD9;

static const UInt32	kOriginalLoadCreatedObjectsAddr = 0x00461350;
static const UInt32	kLoadCreatedObjectsHookAddr = 0x0046347B;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2

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

static const UInt32	kNewGamePatchAddr = 0x005B5D0D;

static const UInt32 QUIMsgPatchAddr = 0x0057ABC0;
static const UInt8  QUIMsgData = 0x51;	//code overwritten by retn
static const UInt32 QUIMsg_2PatchAddr = 0x0057ACD0;
static const UInt8 QUIMsg_2Data = 0xD9;

static const UInt32	kOriginalLoadCreatedObjectsAddr = 0x00461350;
static const UInt32	kLoadCreatedObjectsHookAddr = 0x0046347B;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416

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

static const UInt32	kNewGamePatchAddr = 0x005B5EFD;

static const UInt32	QUIMsgPatchAddr = 0x0057ACC0;
static const UInt8	QUIMsgData = 0x51;	//code overwritten by retn
static const UInt32	QUIMsg_2PatchAddr = 0x0057ADD0;
static const UInt8	QUIMsg_2Data = 0xD9;

static const UInt32	kOriginalLoadCreatedObjectsAddr = 0x00461310;
static const UInt32	kLoadCreatedObjectsHookAddr = 0x0046344B;

#else
#error unsupported oblivion version
#endif

static void HandleNewGameHook(void)
{
	_MESSAGE("HandleNewGameHook");

	Serialization::HandleNewGame();
}

static void HandleMainLoopHook(void)
{
	// nothing yet

	Hook_Memory_CheckAllocs();
}

// workaround for inability to take address of __thiscall functions
static __declspec(naked) void _TESSaveLoadGame_LoadCreatedObjectsHook(void)
{
	__asm
	{
		jmp	TESSaveLoadGame::LoadCreatedObjectsHook
	}
}

class CallPostFixup
{
public:
	bool	Accept(UInt32 formID)
	{
		TESForm	* form = LookupFormByID(formID);

		if(form)
		{
			// this could be done with dynamic_cast, but this is faster
			switch(form->typeID)
			{
				case 0x25:	// TESLevCreature
				{
					TESLevCreature	* _form = (TESLevCreature *)form;

					FixupModIDs(&_form->leveledList);
				}
				break;

				case 0x2B:	// TESLevItem
				{
					TESLevItem	* _form = (TESLevItem *)form;

					FixupModIDs(&_form->leveledList);
				}
				break;

				case 0x40:	// TESLevSpell
				{
					TESLevSpell	* _form = (TESLevSpell *)form;

					FixupModIDs(&_form->leveledList);
				}
				break;
			}

			form->DoPostFixup();
		}

		return true;
	}

private:
	class FixupModID
	{
	public:
		bool	Accept(TESLeveledList::ListData * data)
		{
			UInt32	newRefID;

			if(!Serialization::ResolveRefID(data->formID, &newRefID))
				newRefID = 0;	// invalid refid

			data->formID = newRefID;

			return true;
		}
	};

	void FixupModIDs(TESLeveledList * levList)
	{
		LeveledListVisitor	visitor(&levList->list);

		visitor.Visit(FixupModID());
	}
};

void TESSaveLoadGame::LoadCreatedObjectsHook(UInt32 unk0)
{
	// run the original code
	ThisStdCall(kOriginalLoadCreatedObjectsAddr, this, unk0);

	// iterate the linked list, call DoPostFixup
	CreatedObjectVisitor	visitor(&createdObjectList);

	visitor.Visit(CallPostFixup());
}

#define DEBUG_PRINT_CHANNEL(idx)								\
																\
static UInt32 __stdcall DebugPrint##idx(const char * str)		\
{																\
	static FILE	* dst = NULL;									\
	if(!dst) dst = _fsopen("obse_debugprint" #idx ".log", "w", _SH_DENYWR);	\
	if(dst) fputs(str, dst);									\
	return 0;													\
}

DEBUG_PRINT_CHANNEL(0)	// used to exit
DEBUG_PRINT_CHANNEL(1)	// ignored
DEBUG_PRINT_CHANNEL(2)	// ignored
// 3 - program flow
DEBUG_PRINT_CHANNEL(4)	// ignored
// 5 - stack trace?
DEBUG_PRINT_CHANNEL(6)	// ignored
// 7 - ingame
// 8 - ingame

// these are all ignored in-game
static void Hook_DebugPrint(void)
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
	const UInt32	kMessageHandlerVtblBase = 0x00A0A468;
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
	const UInt32	kMessageHandlerVtblBase = 0x00A3DB18;
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	const UInt32	kMessageHandlerVtblBase = 0x00A3DA08;
#else
#error unsupported oblivion version
#endif
	
	SafeWrite32(kMessageHandlerVtblBase + (0 * 4), (UInt32)DebugPrint0);
	SafeWrite32(kMessageHandlerVtblBase + (1 * 4), (UInt32)DebugPrint1);
	SafeWrite32(kMessageHandlerVtblBase + (2 * 4), (UInt32)DebugPrint2);
	SafeWrite32(kMessageHandlerVtblBase + (4 * 4), (UInt32)DebugPrint4);
	SafeWrite32(kMessageHandlerVtblBase + (6 * 4), (UInt32)DebugPrint6);
}

void Hook_Gameplay_Init(void)
{
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

	WriteRelCall(kNewGamePatchAddr, (UInt32)&HandleNewGameHook);

	// objects in the 'created objects' list are not loaded in the same way as other objects
	// normally they are created, loaded from disk, then once they have all been loaded and added
	// to the form table a post-load callback is called to allow refids to be turned in to pointers
	// the 'created objects' code calls the post-load callback immediately after loading the form
	// so bugs occur when the callback expects to be able to find a cloned object that hasn't been
	// created yet
	// 
	// to fix this, we hook the function loading the created objects and disable the call to the
	// post-fixup function. once the function is completed, we walk the created objects linked list
	// and call the callback ourselves

	// nop out post-load callback
#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
	SafeWrite8(0x0045B27F + 0, 0x90);
	SafeWrite8(0x0045B27F + 1, 0x90);
	SafeWrite8(0x0045B27F + 2, 0x90);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
	SafeWrite8(0x004614F9 + 0, 0x90);	
	SafeWrite8(0x004614F9 + 1, 0x90);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	SafeWrite8(0x004614B9 + 0, 0x90);
	SafeWrite8(0x004614B9 + 1, 0x90);
#else
#error unsupported oblivion version
#endif

	// hook the loader function
	WriteRelCall(kLoadCreatedObjectsHookAddr, (UInt32)&_TESSaveLoadGame_LoadCreatedObjectsHook);

	// this seems stable and helps in debugging, but it makes large files during gameplay
#if defined(_DEBUG) && 0
	Hook_DebugPrint();
#endif
}

//toggle messages on or off in upper left  corner of screen
//meant to be toggled off immediately before calling a spam-generating function and toggled back on again
void ToggleUIMessages(bool enableSpam)
{
	static bool msgsOn = true;
	if (msgsOn != enableSpam)
	{
		msgsOn = enableSpam;
		if (!msgsOn)
		{
			SafeWrite8(QUIMsgPatchAddr, 0xC3);		//write immediate retn at function entry
			SafeWrite8(QUIMsg_2PatchAddr, 0xC3);
		}
		else
		{
			SafeWrite8(QUIMsgPatchAddr, QUIMsgData);	//restore code overwritten above
			SafeWrite8(QUIMsg_2PatchAddr, QUIMsg_2Data);
		}
	}
}
