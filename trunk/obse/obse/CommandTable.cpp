#include "CommandTable.h"
#include "Commands_Inventory.h"
#include "Commands_Input.h"
#include "Commands_FileIO.h"
#include "Commands_Console.h"
#include "Commands_Math.h"
#include "Commands_Player.h"
#include "Commands_Game.h"
#include "Commands_Magic.h"
#include "Commands_MagicEffect.h"
#include "Commands_Weather.h"
#include "Commands_ActiveEffect.h"
#include "Commands_MiscReference.h"
#include "ParamInfos.h"
#include "PluginManager.h"
#include "Hooks_Memory.h"
#include "obse_common/SafeWrite.h"
#include "obse_common/obse_version.h"
#include <string>
#include "Utilities.h"

/*

// arg0 - ParamInfo *
// arg1 - void * scriptData - contains argument data
// arg2 - TESObjectREFR * to the 'this' object (confirmed)
// arg3 - script instance state (contains local vars)
// arg4 - Script * to the script form
// arg5 - object event linked list
// arg6 - floating point result output (double!)
// arg7 - UInt16 * scriptData offset - offset in to scriptData
// opcodeOffsetPtr - UInt16 *, offset in to scriptData
bool Command_GetPos(unk arg0, unk arg1, unk arg2, unk arg3, unk arg4, unk arg5, unk arg6, unk opcodeOffsetPtr)
{
	bool	result = false;
	
	char	axis;
	
	if(ExtractArgs(arg0, arg1, opcodeOffsetPtr, arg2, arg3, arg4, arg5, &axis))
	{
		result = HandleGetPos(arg2, axis, 0, arg6);
	}
	
	return result;
}

*/

#ifdef OBLIVION

#include "GameAPI.h"
#include "GameObjects.h"
#include "GameForms.h"
#include "GameMagicEffects.h"
#include "GameTiles.h"

#include "common/IFileStream.h"

bool Cmd_TestExtractArgs_Execute(COMMAND_ARGS)
{
	*result = 0;

	UInt8	* scriptData = (UInt8 *)arg1;
	UInt8	* scriptDataBase = scriptData;

	scriptData += *opcodeOffsetPtr;

	scriptData -= 2;

	UInt32	opcodeDataLen = *((UInt16 *)scriptData);
	scriptData += 2;

	UInt32	numArgs = *((UInt16 *)scriptData);
	scriptData += 2;

	_MESSAGE("len = %04X numArgs = %04X", opcodeDataLen, numArgs);

	{
		static int	exportID = 0;
		char		name[64];

		sprintf_s(name, sizeof(name), "arg_%d", exportID);
		exportID++;

		IFileStream	out;
		if(out.Create(name))
			out.WriteBuf(scriptData - 2, opcodeDataLen);
	}

	for(UInt32 i = 0; i < numArgs; i++)
	{
		ParamInfo	* info = &paramInfo[i];

		switch(info->typeID)
		{
			case kParamType_InventoryObject:
			{
				UInt8	unk0 = *scriptData++;
				UInt16	varIdx = *((UInt16 *)scriptData);
				Script::RefVariable	* var = scriptObj->GetVariable(varIdx);
				ASSERT(var);

				var->Resolve(eventList);

				_MESSAGE("inventory object %02X %04X %08X (%08X)", unk0, varIdx, var->form,
					var->form ? var->form->refID : 0);
			}
			break;

			default:
				_MESSAGE("unknown type %02X", info->typeID);
				break;
		}
	}

	*opcodeOffsetPtr += opcodeDataLen;

	return true;
}

bool Cmd_GetOBSEVersion_Execute(COMMAND_ARGS)
{
	*result = OBSE_VERSION_INTEGER;

	return true;
}

static void DumpExtraDataList(ExtraDataList * list)
{
	for(BSExtraData * traverse = list->m_data; traverse; traverse = traverse->next)
	{
		Console_Print("%s", GetObjectClassName(traverse));

		if(traverse->type == kExtraData_Worn)
		{
			Console_Print("worn = %02X %02X %02X", traverse->pad[0], traverse->pad[1], traverse->pad[2]);
		}
	}
}

static const UInt32 kMaxSavedIPs = 0x100;
static const UInt32	kMaxSavedIPStack = 20;	// twice the supposed limit
struct SavedIPInfo
{
	UInt32	ip;
	UInt32	stackDepth;
	UInt32	stack[kMaxSavedIPStack];
};

struct ScriptExecutionState
{
	UInt8	pad[0x20];
	UInt32	stackDepth;
	UInt32	stack[1];
};

static SavedIPInfo s_savedIPTable[kMaxSavedIPs] = { { 0 } };

bool Cmd_SaveIP_Execute(COMMAND_ARGS)
{
	UInt32	_esi;

	// ### assume nothing modifies esi before we get here
	// ### MAKE SURE THIS IS THE FIRST CODE TO RUN
	// ### the alternative is a __declspec(naked) and __asm wrapper
	__asm { mov _esi, esi }

	// make sure this is only called from the main execution loop
	ASSERT_STR(arg1 == scriptObj->data, SaveIP may not be called inside a set or if statement);

	UInt32	idx = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &idx)) return true;

	// this must happen after extractargs updates opcodeOffsetPtr so it points to the next instruction
	if(idx < kMaxSavedIPs)
	{
		SavedIPInfo				* info = &s_savedIPTable[idx];
		ScriptExecutionState	* state = (ScriptExecutionState *)_esi;

		info->ip = *opcodeOffsetPtr;
		info->stackDepth = state->stackDepth;
		ASSERT((info->stackDepth + 1) < kMaxSavedIPStack);
		memcpy(info->stack, state->stack, (info->stackDepth + 1) * sizeof(UInt32));
	}

	return true;
}

bool Cmd_RestoreIP_Execute(COMMAND_ARGS)
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
	static const UInt32 kDataDeltaStackOffset = 480;
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
	static const UInt32 kDataDeltaStackOffset = 482;
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const UInt32 kDataDeltaStackOffset = 482;
#else
#error unsupported oblivion version
#endif

	UInt32	_esi;

	// ### assume nothing modifies esi before we get here
	__asm { mov _esi, esi }

	// make sure this is only called from the main execution loop
	ASSERT_STR(arg1 == scriptObj->data, RestoreIP may not be called inside a set or if statement);

	UInt32	idx = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &idx)) return true;

	if(idx < kMaxSavedIPs)
	{
		SavedIPInfo				* info = &s_savedIPTable[idx];
		ScriptExecutionState	* state = (ScriptExecutionState *)_esi;

		// ### this is major stack abuse
		// ### the variable storing the number of data bytes happens to be here
		// ### it will be added to the current instruction pointer after we return
		// ### we change it to fake branching
		opcodeOffsetPtr[kDataDeltaStackOffset] += info->ip - opcodeOffsetPtr[0];

		// restore the if/else/endif stack
		state->stackDepth = info->stackDepth;
		memcpy(state->stack, info->stack, (info->stackDepth + 1) * sizeof(UInt32));
	}

	return true;
}

bool Cmd_Test_Execute(COMMAND_ARGS)
{
	_MESSAGE("Cmd_Test_Execute: %08X %08X %08X (%s) %08X %08X (%s) %08X %08X %08X",
		paramInfo, arg1, thisObj, GetObjectClassName((void *)thisObj), arg3, scriptObj, GetObjectClassName((void *)scriptObj), eventList, result, opcodeOffsetPtr);

	InterfaceManager	* interfaceManager = InterfaceManager::GetSingleton();
	Sky					* sky = Sky::GetSingleton();

	sky->RefreshClimate(sky->firstClimate, 1);

	return true;
}

bool Cmd_TestArgs_Execute(COMMAND_ARGS)
{
	_MESSAGE("Cmd_TestArgs_Execute: %08X %08X %08X (%s) %08X %08X (%s) %08X %08X %08X",
		paramInfo, arg1, thisObj, GetObjectClassName((void *)thisObj), arg3, scriptObj, GetObjectClassName((void *)scriptObj), eventList, result, opcodeOffsetPtr);

	UInt32	arg;

	if(ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &arg))
	{
		Console_Print("testargcommand: %d", arg);
	}
	else
	{
		Console_Print("testargcommand: couldn't extract args");
	}

	return true;
}

bool Cmd_DumpVarInfo_Execute(COMMAND_ARGS)
{
	if(scriptObj)
	{
		Console_Print("script vars %08X", scriptObj);

		UInt32	idx = 0;

		for(Script::RefListEntry * traverse = &scriptObj->refList; traverse; traverse = traverse->next, idx++)
		{
			if(traverse->var)
			{
				Console_Print("%d: %08X %08X %08X (%08X%s%s) %08X",
					idx,
					traverse->var->unk0,
					traverse->var->unk1,
					traverse->var->form,
					traverse->var->form ? traverse->var->form->refID : 0,
					traverse->var->form ? " " : "",
					traverse->var->form ? GetFullName(traverse->var->form) : "",
					traverse->var->varIdx);

				_MESSAGE("%d: %08X %08X %08X (%08X%s%s) %08X",
					idx,
					traverse->var->unk0,
					traverse->var->unk1,
					traverse->var->form,
					traverse->var->form ? traverse->var->form->refID : 0,
					traverse->var->form ? " " : "",
					traverse->var->form ? GetFullName(traverse->var->form) : "",
					traverse->var->varIdx);
			}
		}
	}

	if(eventList)
	{
		Console_Print("event list %08X", eventList);

		UInt32	idx = 0;

		for(ScriptEventList::VarEntry * traverse = eventList->m_vars; traverse; traverse = traverse->next, idx++)
		{
			if(traverse->var)
			{
				Console_Print("%d: %08X %08X %f %016I64X",
					idx,
					traverse->var->id,
					traverse->var->unk1,
					traverse->var->data,
					*((UInt64 *)&traverse->var->data));

				_MESSAGE("%d: %08X %08X %f %016I64X",
					idx,
					traverse->var->id,
					traverse->var->unk1,
					traverse->var->data,
					*((UInt64 *)&traverse->var->data));
			}
		}
	}

	return true;
}
#endif

// nop command handler for script editor
bool Cmd_Default_Execute(COMMAND_ARGS)
{
	return true;
}

// called from 004F90A5
bool Cmd_Default_Parse(UInt32 arg0, UInt32 arg1, UInt32 arg2, UInt32 arg3)
{
#ifdef _DEBUG
	_MESSAGE("Cmd_Default_Parse: %08X %08X %08X %08X",
		arg0, arg1, arg2, arg3);
#endif

	#ifdef OBLIVION

	#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
	static const Cmd_Parse g_defaultParseCommand = (Cmd_Parse)0x004F38C0;
	#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
	static const Cmd_Parse g_defaultParseCommand = (Cmd_Parse)0x004FDF80;
	#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	static const Cmd_Parse g_defaultParseCommand = (Cmd_Parse)0x004FDE30;
	#else
	#error unsupported version of oblivion
	#endif

	#else

	#if CS_VERSION == CS_VERSION_1_0
	static const Cmd_Parse g_defaultParseCommand = (Cmd_Parse)0x004F69C0;
	#elif CS_VERSION == CS_VERSION_1_2
	static const Cmd_Parse g_defaultParseCommand = (Cmd_Parse)0x00500FF0;
	#else
	#error unsupported cs version
	#endif

	#endif

	// arg0 = idx?
	// arg1 = ParamInfo *
	// arg2 = ptr to line to parse, skip UInt32 header first
	// arg3 = ptr to script info? first UInt32 is ptr to script data

	return g_defaultParseCommand(arg0, arg1, arg2, arg3);
}

CommandTable::CommandTable()
{
	//
}

CommandTable::~CommandTable()
{
	//
}

static CommandInfo kCommandInfo_DumpVarInfo =
{
	"DumpVarInfo",
	"",
	0,
	"",
	0,
	0,
	NULL,

	HANDLER(Cmd_DumpVarInfo_Execute),
	Cmd_Default_Parse,
	NULL,
	NULL
};

static ParamInfo kTestExtractArgs_Params[] =
{
	{	"TESForm",	kParamType_InventoryObject,	0 },
};

static CommandInfo kCommandInfo_TestExtractArgs =
{
	"TestExtractArgs",
	"",
	0,
	"",
	0,
	1,
	kTestExtractArgs_Params,

	HANDLER(Cmd_TestExtractArgs_Execute),
	Cmd_Default_Parse,
	NULL,
	NULL
};

static CommandInfo kCommandInfo_GetOBSEVersion =
{
	"GetOBSEVersion",
	"",
	0,
	"returns the installed version of OBSE",
	0,
	0,
	NULL,

	HANDLER(Cmd_GetOBSEVersion_Execute),
	Cmd_Default_Parse,
	NULL,
	NULL
};

static CommandInfo kTestCommand =
{
	"testcommand",
	"tcmd",
	0,
	"test command for obse",
	0,		// doesn't require parent obj
	0,		// doesn't have params
	NULL,	// no param table

	HANDLER(Cmd_Test_Execute),
	Cmd_Default_Parse,
	NULL,
	NULL
};

static ParamInfo kTestArgCommand_Params[] =
{
	{	"int", kParamType_Integer, 0 }
};

static CommandInfo kTestArgCommand =
{
	"testargcommand",
	"tacmd",
	0,
	"test argument command for obse",
	0,
	1,
	kTestArgCommand_Params,

	HANDLER(Cmd_TestArgs_Execute),
	Cmd_Default_Parse,
	NULL,
	NULL
};

static CommandInfo kCommandInfo_SaveIP =
{
	"SaveIP",
	"Label",
	0,
	"",
	0,
	1,
	kParams_OneOptionalInt,

	HANDLER(Cmd_SaveIP_Execute),
	Cmd_Default_Parse,
	NULL,
	NULL
};

static CommandInfo kCommandInfo_RestoreIP =
{
	"RestoreIP",
	"Goto",
	0,
	"",
	0,
	1,
	kParams_OneOptionalInt,

	HANDLER(Cmd_RestoreIP_Execute),
	Cmd_Default_Parse,
	NULL,
	NULL
};

// this was defined in oblivion in a patch, but the CS wasn't updated to include it
static CommandInfo kCommandInfo_PurgeCellBuffers =
{
	"PurgeCellBuffers", "pcb",
	0x016B,
	"Forcibly unloads all unattached cells in cell buffers.",
	0,
	0,
	NULL,

	Cmd_Default_Execute,
	Cmd_Default_Parse,
	NULL,
	NULL
};

//0000116C 0000 SetPlayerInSEWorld SPInSE 
//	00000001 00000000 Integer
static CommandInfo kCommandInfo_SetPlayerInSEWorld =
{
	"SetPlayerInSEWorld", "SPInSE",
	0x0000116C,
	"",
	0,
	1,
	kParams_OneInt,
	Cmd_Default_Execute,
	Cmd_Default_Parse,
	NULL,
	NULL
};


//0000116D 0000 GetPlayerInSEWorld gpInSE 
static CommandInfo kCommandInfo_GetPlayerInSEWorld =
{
	"GetPlayerInSEWorld", "gpInSE",
	0x0000116D,
	"",
	0,
	0,
	NULL,

	Cmd_Default_Execute,
	Cmd_Default_Parse,
	NULL,
	NULL
};

//0000116E 0001 PushActorAway  
//	00000004 00000000 ObjectReferenceID
//	00000001 00000000 Integer
static ParamInfo kParams_PushActorAway[2] =
{
	{	"objectRef", kParamType_ObjectRef, 0},
	{	"int", kParamType_Integer, 0}
};

static CommandInfo kCommandInfo_PushActorAway =
{
	"PushActorAway", "",
	0x0000116E,
	"",
	1,
	2,
	kParams_PushActorAway,

	Cmd_Default_Execute,
	Cmd_Default_Parse,
	NULL,
	NULL
};

//0000116F 0001 SetActorsAI  
//	00000001 00000000 Integer
static CommandInfo kCommandInfo_SetActorsAI =
{
	"SetActorsAI", "",
	0x0000116F,
	"",
	1,
	1,
	kParams_OneInt,

	Cmd_Default_Execute,
	Cmd_Default_Parse,
	NULL,
	NULL
};


static CommandInfo kPaddingCommand =
{
	"", "",
	0,
	"command used for padding",
	0,
	0,
	NULL,

	Cmd_Default_Execute,
	Cmd_Default_Parse,
	NULL,
	NULL
};

struct PatchLocation
{
	UInt32	ptr;
	UInt32	offset;
	UInt32	type;
};

#ifdef OBLIVION

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

// ### TODO: command dumper at 004F848F
// asserts at 004F25C0

static const PatchLocation kPatch_ScriptCommands_Start[] =
{
	{	0x004F263C + 3,	0x00 },
	{	0x004F2693 + 1,	0x04 },
	{	0x004F272A + 3,	0x08 },
	{	0x004F5ED7 + 3,	0x00 },
	{	0x0055E199 + 2,	0x20 },
	{	0x0055E1A7 + 2,	0x10 },
	{	0x0055E5DE + 3,	0x20 },
	{	0x0055E6F3 + 3,	0x00 },
	{	0x0055E8D1 + 4,	0x12 },
	{	0x0055E8F9 + 3,	0x12 },
	{	0x0055E905 + 2,	0x14 },
	{	0x0055E939 + 3,	0x12 },
	{	0x0055E945 + 2,	0x14 },
	{	0x0055E980 + 3,	0x12 },
	{	0x0055E98B + 2,	0x14 },
	{	0x0055EA56 + 4,	0x12 },
	{	0x0055EAA8 + 3,	0x12 },
	{	0x0055EABB + 2,	0x14 },
	{	0x0055EB72 + 3,	0x12 },
	{	0x0055EB81 + 2,	0x14 },
	{	0 },
};

static const PatchLocation kPatch_ScriptCommands_End[] =
{
	{	0x004F26BD + 2, 4 },
	{	0 },
};

// search for 16Ch, stopped at 0x005FA012

static const PatchLocation kPatch_ScriptCommands_MaxIdx[] =
{
	{	0x004E8DB9 + 3,	1, 1 },
//	{	0x004F25C0 + 6,	1 },	// assert
	{	0x004F262D + 2, 1 },
	{	0x004F5EC9 + 1,	1 },
	{	0x0055E33D + 1,	1 },
	{	0x0055E354 + 1, (UInt32)(-0x1000) + 1 },
	{	0x0055E368 + 2, (UInt32)(-0x1000) + 1 },
	{	0x0055E8C6 + 2, (UInt32)(-0x1000) + 1 },
	{	0x0055E8E6 + 2, (UInt32)(-0x1000) + 1 },
	{	0x0055E926 + 2, (UInt32)(-0x1000) + 1 },
	{	0x0055E96B + 1, (UInt32)(-0x1000) + 1 },
	{	0x0055EA48 + 1, (UInt32)(-0x1000) + 1 },
	{	0x0055EA97 + 1,	(UInt32)(-0x1000) + 1 },
	{	0x0055EB61 + 1,	(UInt32)(-0x1000) + 1 },

	{	0 },
};

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2

static const PatchLocation kPatch_ScriptCommands_Start[] =
{
	// 004FCB80
	{	0x004FCBB8 + 3,	0x00 },

	// 004FCBC0
	{	0x004FCC09 + 1,	0x04 },
	{	0x004FCCA2 + 3,	0x08 },

	// 004FDC40
	{	0x004FDDC9 + 3,	0x00 },

	// 004FF860
	{	0x005000BE + 3,	0x00 },

	// 00501690 - print console/script command help
	{	0x00501724 + 2,	0x0C },
	{	0x00501740 + 2,	0x04 },
	{	0x0050175F + 2,	0x00 },
	{	0x00501775 + 2,	0x00 },

	// 0056ABD0
	{	0x0056AC25 + 3,	0x20 },
	{	0x0056AC36 + 2,	0x10 },

	// 0056ADA0
	{	0x0056AE02 + 3,	0x20 },
	{	0x0056AF16 + 3,	0x00 },

	// 0056B0F0
	{	0x0056B101 + 4,	0x12 },

	// 0056B110
	{	0x0056B12C + 3,	0x12 },
	{	0x0056B138 + 2,	0x14 },

	// 0056B150
	{	0x0056B16C + 3,	0x12 },
	{	0x0056B178 + 2,	0x14 },
	
	// 0056B1A0
	{	0x0056B1C1 + 4,	0x12 },
	{	0x0056B1CF + 2,	0x14 },

	// 0056B260
	{	0x0056B2B2 + 4,	0x12 },
	{	0x0056B305 + 4,	0x12 },
	{	0x0056B317 + 2,	0x14 },
	{	0x0056B3CD + 3,	0x14 },
	{	0x0056B471 + 4,	0x12 },
	{	0x0056B483 + 2,	0x14 },

	{	0 },
};

static const PatchLocation kPatch_ScriptCommands_End[] =
{
	// 004FCBC0
	{	0x004FCC37 + 2,	0x04 },

	{	0 },
};

// check 00863EF2, unlikely

static const PatchLocation kPatch_ScriptCommands_MaxIdx[] =
{
	// 004F3380
	{	0x004F3404 + 3,	1,	1 },

	// 004FCB80
	{	0x004FCBA9 + 2,	(UInt32)(-0x1000) },

	// 004FCE30 - assert
//	{	0x004FCE30 + 6,	1 },

	// 004FDC40
	{	0x004FDDBB + 1,	(UInt32)(-0x1000) },

	// 004FF860
	{	0x005000B3 + 2,	(UInt32)(-0x1000) },

	// 0056B0F0
	{	0x0056B0F6 + 2, (UInt32)(-0x1000) + 1 },

	// 0056B110
	{	0x0056B116 + 2,	(UInt32)(-0x1000) + 1 },

	// 0056B150
	{	0x0056B156 + 2,	(UInt32)(-0x1000) + 1 },

	// 0056B1A0
	{	0x0056B1AB + 1, (UInt32)(-0x1000) + 1 },

	// 0056B260
	{	0x0056B2A4 + 1, (UInt32)(-0x1000) + 1 },
	{	0x0056B2F3 + 1, (UInt32)(-0x1000) + 1 },
	{	0x0056B45F + 1, (UInt32)(-0x1000) + 1 },

	{	0 },
};

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416

static const PatchLocation kPatch_ScriptCommands_Start[] =
{
	// 004FCA30
	{	0x004FCA68 + 3,	0x00 },

	// 004FCA70
	{	0x004FCAB9 + 1, 0x04 },
	{	0x004FCB52 + 3,	0x08 },

	// 004FDAF0
	{	0x004FDC79 + 3,	0x00 },

	// 004FF710
	{	0x004FFF6E + 3,	0x00 },

	// 00501540 - print console/script command help
	{	0x005015D4 + 2,	0x0C },
	{	0x005015F0 + 2,	0x04 },
	{	0x0050160F + 2,	0x00 },
	{	0x00501625 + 2,	0x00 },

	// 0056AC50
	{	0x0056ACA5 + 3,	0x20 },
	{	0x0056ACB6 + 2,	0x10 },

	// 0056AE20
	{	0x0056AE82 + 3,	0x20 },
	{	0x0056AF96 + 3,	0x00 },

	// 0056B170
	{	0x0056B181 + 4,	0x12 },

	// 0056B190
	{	0x0056B1AC + 3,	0x12 },
	{	0x0056B1B8 + 2,	0x14 },

	// 0056B1D0
	{	0x0056B1EC + 3,	0x12 },
	{	0x0056B1F8 + 2,	0x14 },
	
	// 0056B220
	{	0x0056B241 + 4,	0x12 },
	{	0x0056B24F + 2,	0x14 },

	// 0056B2E0
	{	0x0056B332 + 4,	0x12 },
	{	0x0056B385 + 4,	0x12 },
	{	0x0056B397 + 2,	0x14 },
	{	0x0056B44D + 3,	0x14 },
	{	0x0056B4F1 + 4,	0x12 },
	{	0x0056B503 + 2,	0x14 },

	{	0 },
};

static const PatchLocation kPatch_ScriptCommands_End[] =
{
	// 004FCA70
	{	0x004FCAE7 + 2,	0x04 },

	{	0 },
};

// check 00854AC2, unlikely

static const PatchLocation kPatch_ScriptCommands_MaxIdx[] =
{
	// 004F3320
	{	0x004F33A4 + 3,	1,	1 },

	// 004FCA30
	{	0x004FCA59 + 2,	(UInt32)(-0x1000) },

	// 004FCCE0 - assert
//	{	0x004FCCE0 + 6,	1 },

	// 004FDAF0
	{	0x004FDC6B + 1,	(UInt32)(-0x1000) },

	// 004FF710
	{	0x004FFF63 + 2,	(UInt32)(-0x1000) },

	// 0056B170
	{	0x0056B176 + 2, (UInt32)(-0x1000) + 1 },

	// 0056B190
	{	0x0056B196 + 2,	(UInt32)(-0x1000) + 1 },

	// 0056B1D0
	{	0x0056B1D6 + 2,	(UInt32)(-0x1000) + 1 },

	// 0056B220
	{	0x0056B22B + 1, (UInt32)(-0x1000) + 1 },

	// 0056B2E0
	{	0x0056B324 + 1, (UInt32)(-0x1000) + 1 },
	{	0x0056B373 + 1, (UInt32)(-0x1000) + 1 },
	{	0x0056B4DF + 1, (UInt32)(-0x1000) + 1 },

	{	0 },
};

#else

#error unsupported oblivion version

#endif

#else // OBLIVION (CS stuff goes here)

#if CS_VERSION == CS_VERSION_1_0

// could patch the stuff around 004F5620 but those are just asserts

// weird things going on at sub_456A20

//	00	longName
//	04	shortName
//	08	opcode
//	0C	helpText
//	10	unk0
//	12	numParams
//	14	params
//	18	unk1
//	1C	unk2
//	20	unk3
//	24	flags

// original = C0 D8 9D 00
static const PatchLocation kPatch_ScriptCommands_Start[] =
{
	{	0x00454E99 + 1,	0 },

	{	0x00457023 + 3,	0 },
	{	0x004575EC + 3,	0 },
	{	0x004577F2 + 3,	0 },
	{	0x004579A1 + 4,	0x12 },
	{	0x004579C9 + 3,	0x12 },
	{	0x004579D5 + 2,	0x14 },
	{	0x00457A09 + 3, 0x12 },
	{	0x00457A15 + 2, 0x14 },
	{	0x00457A50 + 3,	0x12 },
	{	0x00457A5B + 2,	0x14 },
	{	0x00457B06 + 4, 0x12 },
	{	0x00457B31 + 3,	0x12 },
	{	0x00457B3C + 2,	0x14 },
	{	0x00457B8E + 4,	0x12 },
	{	0x00457BB4 + 3,	0x12 },
	{	0x00457BBF + 2,	0x14 },

	{	0x004583AD + 4,	0x12 },
	{	0x004583E3 + 3,	0x12 },
	{	0x004583EE + 2,	0x14 },
	{	0x00458496 + 4,	0x12 },

	{	0x004584D4 + 3,	0x12 },
	{	0x004584DF + 2,	0x14 },
	{	0x004585D6 + 4, 0x12 },
	{	0x00458628 + 3,	0x12 },
	{	0x0045863B + 2,	0x14 },

	{	0x0045880C + 3,	0x12 },
	{	0x0045881B + 2,	0x14 },
	{	0x00458A74 + 4,	0x12 },
	{	0x00458BDE + 3,	0x14 },
	{	0x00458C20 + 3,	0x14 },

	{	0x00458E07 + 4,	0x12 },
	{	0x00458F13 + 4,	0x12 },
	{	0x00458F5F + 3,	0x14 },
	{	0x00459006 + 3,	0x14 },
	{	0x004590EF + 4,	0x12 },
	{	0x004592BC + 3,	0x14 },

	{	0x004F569C + 3,	0 },
	{	0x004F56F3 + 1,	4 },
	{	0x004F578A + 3,	8 },
	{	0x004F8FF0 + 3,	0 },
	{	0 },
};

// original = 78 11 9E 00
static const PatchLocation kPatch_ScriptCommands_End[] =
{
	{	0x00454EBA + 2,	0 },
	{	0x004F571D + 2,	4 },
	{	0 },
};

// original = 6B 11 00 00
static const PatchLocation kPatch_ScriptCommands_MaxIdx[] =
{
	{	0x00456A2D + 1, 1 },
	{	0x00456A44 + 1, (UInt32)(-0x1000) + 1 },
	{	0x00456A58 + 2, (UInt32)(-0x1000) + 1 },

	{	0x00457996 + 2,	(UInt32)(-0x1000) + 1 },
	{	0x004579B6 + 2, (UInt32)(-0x1000) + 1 },
	{	0x004579F6 + 2, (UInt32)(-0x1000) + 1 },
	{	0x00457A3B + 1, (UInt32)(-0x1000) + 1 },
	{	0x00457AFC + 1,	(UInt32)(-0x1000) + 1 },
	{	0x00457B24 + 1,	(UInt32)(-0x1000) + 1 },
	{	0x00457B84 + 1,	(UInt32)(-0x1000) + 1 },
	{	0x00457BA7 + 1,	(UInt32)(-0x1000) + 1 },

	{	0x0045839F + 1,	(UInt32)(-0x1000) + 1 },
	{	0x004583D6 + 1,	(UInt32)(-0x1000) + 1 },
	{	0x00458488 + 1,	(UInt32)(-0x1000) + 1 },
	{	0x004584C3 + 1,	(UInt32)(-0x1000) + 1 },
	{	0x004585C8 + 1,	(UInt32)(-0x1000) + 1 },
	{	0x00458617 + 1,	(UInt32)(-0x1000) + 1 },
	{	0x004587FB + 1,	(UInt32)(-0x1000) + 1 },
	{	0x00458A64 + 2,	(UInt32)(-0x1000) + 1 },
	{	0x00458DFD + 1,	(UInt32)(-0x1000) + 1 },
	{	0x00458F05 + 1,	(UInt32)(-0x1000) + 1 },
	{	0x004590E5 + 1,	(UInt32)(-0x1000) + 1 },

	{	0x004EF049 + 3,	1, 1 },
	{	0x004F568D + 2,	1 },
	{	0x004F8FE2 + 1,	1 },
	{	0 }
};

#elif CS_VERSION == CS_VERSION_1_2

// 10 36 9F 00
static const PatchLocation kPatch_ScriptCommands_Start[] =
{
	// 00455FF0
	{	0x00456009 + 1,	0x00 },

	// 004578B0
	{	0x00457905 + 3,	0x20 },
	{	0x00457916 + 2,	0x10 },

	// 004581B0
	{	0x00458212 + 3,	0x20 },
	{	0x00458326 + 3,	0x00 },

	// 004588A0
	{	0x0045892B + 3,	0x00 },

	// 00458AE0
	{	0x00458B4C + 3,	0x00 },

	// 00458CF0
	{	0x00458D01 + 4,	0x12 },

	// 00458D10
	{	0x00458D33 + 3,	0x14 },

	// 00458D50
	{	0x00458D73 + 3,	0x14 },

	// 00458DA0
	{	0x00458DC3 + 3,	0x14 },

	// 004592B0
	{	0x004592D0 + 3,	0x14 },
	{	0x00459604 + 3,	0x14 },

	// 00459CB0
	{	0x00459E35 + 3,	0x14 },
	{	0x00459E77 + 3,	0x14 },

	// 0045A130
	{	0x0045A1C0 + 3,	0x14 },
	{	0x0045A275 + 3,	0x14 },

	// 0045A380
	{	0x0045A529 + 3,	0x14 },

	// 004F59B0
	{	0x004F5A37 + 1,	0x24 },

	// 004FFBE0
	{	0x004FFC18 + 3,	0x00 },

	// 0x004FFC20
	{	0x004FFC69 + 1,	0x04 },
	{	0x004FFD02 + 3,	0x08 },

	{	0 },
};

// B8 6F 9F 00
static const PatchLocation kPatch_ScriptCommands_End[] =
{
	// 00455FF0
	{	0x0045602B + 2,	0x00 },

	// 004F59B0
	{	0x004F5A64 + 2,	0x24 },

	// 004FFBA0 - assert

	// 004FFC20
	{	0x004FFC97 + 2,	0x04 },

	{	0 },
};

// 1170 / 170 / 171
static const PatchLocation kPatch_ScriptCommands_MaxIdx[] =
{
	// 00457DA0 - suspicious, but seems to be unrelated?

	// 00458CF0
	{	0x00458CF6 + 2,	(UInt32)(-0x1000) + 1 },

	// 00458D10
	{	0x00458D17 + 2,	(UInt32)(-0x1000) + 1 },

	// 00458D50
	{	0x00458D57 + 2,	(UInt32)(-0x1000) + 1 },

	// 00458DA0
	{	0x00458DA7 + 2,	(UInt32)(-0x1000) + 1 },

	// 004F92A0
	{	0x004F9324 + 3,	1, 1 },

	// 004FFBE0
	{	0x004FFC09 + 2,	(UInt32)(-0x1000) },

	{	0 },
};

#else

#error unsupported cs version

#endif

#endif

static void ApplyPatch(const PatchLocation * patch, UInt32 newData)
{
	for(; patch->ptr; ++patch)
	{
		switch(patch->type)
		{
			case 0:
				SafeWrite32(patch->ptr, newData + patch->offset);
				break;

			case 1:
				SafeWrite16(patch->ptr, newData + patch->offset);
				break;
		}
	}
}

void ImportConsoleCommand(const char * name)
{
	CommandInfo	* info = g_consoleCommands.GetByName(name);
	if(info)
	{
		CommandInfo	infoCopy = *info;

		std::string	newName;

		newName = std::string("con_") + name;

		infoCopy.shortName = "";
		infoCopy.longName = _strdup(newName.c_str());	// this leaks but meh

		g_scriptCommands.Add(&infoCopy);

//		_MESSAGE("imported console command %s", name);
	}
	else
	{
		_WARNING("couldn't find console command (%s)", name);

		// pad it
		g_scriptCommands.Add(&kPaddingCommand);
	}
}

void CommandTable::Init(void)
{
	g_consoleCommands.SetBaseID(0x0100);
	g_scriptCommands.SetBaseID(0x1000);

#ifdef OBLIVION

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
	g_consoleCommands.Read((CommandInfo *)0x00ACC4E0, (CommandInfo *)0x00ACD930);
	g_scriptCommands.Read((CommandInfo *)0x00ACD958, (CommandInfo *)0x00AD1238);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
	g_consoleCommands.Read((CommandInfo *)0x00B0B420, (CommandInfo *)0x00B0C898);
	g_scriptCommands.Read((CommandInfo *)0x00B0C8C0, (CommandInfo *)0x00B10268);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	g_consoleCommands.Read((CommandInfo *)0x00B0B420, (CommandInfo *)0x00B0C898);
	g_scriptCommands.Read((CommandInfo *)0x00B0C8C0, (CommandInfo *)0x00B10268);
#else
#error unsupported oblivion version
#endif

#else

#if CS_VERSION == CS_VERSION_1_0

	g_consoleCommands.Read((CommandInfo *)0x009DC420, (CommandInfo *)0x009DD898);
	g_scriptCommands.Read((CommandInfo *)0x009DD8C0, (CommandInfo *)0x009E1178);
//	g_tokenCommands.Read((CommandInfo *)0x009DB2D4, (CommandInfo *)0x009DB554);	// keeping around just for addresses, format is different

	// extra commands in 1.1
	g_scriptCommands.Add(&kCommandInfo_PurgeCellBuffers);

	// extra commands in 1.2
	g_scriptCommands.Add(&kCommandInfo_SetPlayerInSEWorld);
	g_scriptCommands.Add(&kCommandInfo_GetPlayerInSEWorld);
	g_scriptCommands.Add(&kCommandInfo_PushActorAway);
	g_scriptCommands.Add(&kCommandInfo_SetActorsAI);

#elif CS_VERSION == CS_VERSION_1_2

	g_consoleCommands.Read((CommandInfo *)0x009F2170, (CommandInfo *)0x009F35E8);
	g_scriptCommands.Read((CommandInfo *)0x009F3610, (CommandInfo *)0x009F6FB8);

#else
#error unsupported cs version
#endif

#endif

	// pad to opcode 0x1400 to give Bethesda lots of room
	g_scriptCommands.PadTo(0x1400);

	// add the new commands
	g_scriptCommands.Add(&kTestCommand);
	g_scriptCommands.Add(&kTestArgCommand);
	g_scriptCommands.Add(&kCommandInfo_GetNumItems);
	g_scriptCommands.Add(&kCommandInfo_GetInventoryItemType);
	g_scriptCommands.Add(&kCommandInfo_IsKeyPressed);
	g_scriptCommands.Add(&kCommandInfo_GetEquipmentSlotType);
	g_scriptCommands.Add(&kCommandInfo_PrintToConsole);
	g_scriptCommands.Add(&kCommandInfo_GetActiveSpell);
	g_scriptCommands.Add(&kCommandInfo_SetActiveSpell);
	g_scriptCommands.Add(&kCommandInfo_SquareRoot);
	g_scriptCommands.Add(&kCommandInfo_Sin);
	g_scriptCommands.Add(&kCommandInfo_Cos);
	g_scriptCommands.Add(&kCommandInfo_Tan);
	g_scriptCommands.Add(&kCommandInfo_ASin);
	g_scriptCommands.Add(&kCommandInfo_ACos);
	g_scriptCommands.Add(&kCommandInfo_ATan);
	g_scriptCommands.Add(&kCommandInfo_Log);
	g_scriptCommands.Add(&kCommandInfo_Exp);
	g_scriptCommands.Add(&kCommandInfo_GetParentCell);
	g_scriptCommands.Add(&kCommandInfo_Log10);
	g_scriptCommands.Add(&kCommandInfo_Floor);
	g_scriptCommands.Add(&kCommandInfo_Ceil);
	g_scriptCommands.Add(&kCommandInfo_Abs);
	g_scriptCommands.Add(&kCommandInfo_Rand);
	g_scriptCommands.Add(&kCommandInfo_Pow);
	g_scriptCommands.Add(&kCommandInfo_ATan2);
	g_scriptCommands.Add(&kCommandInfo_Sinh);
	g_scriptCommands.Add(&kCommandInfo_Cosh);
	g_scriptCommands.Add(&kCommandInfo_Tanh);
	g_scriptCommands.Add(&kCommandInfo_dSin);
	g_scriptCommands.Add(&kCommandInfo_dCos);
	g_scriptCommands.Add(&kCommandInfo_dTan);
	g_scriptCommands.Add(&kCommandInfo_dASin);
	g_scriptCommands.Add(&kCommandInfo_dACos);
	g_scriptCommands.Add(&kCommandInfo_dATan);
	g_scriptCommands.Add(&kCommandInfo_dATan2);
	g_scriptCommands.Add(&kCommandInfo_dSinh);
	g_scriptCommands.Add(&kCommandInfo_dCosh);
	g_scriptCommands.Add(&kCommandInfo_dTanh);
	g_scriptCommands.Add(&kCommandInfo_GetInventoryObject);
	g_scriptCommands.Add(&kCommandInfo_GetEquippedObject);
	g_scriptCommands.Add(&kCommandInfo_IsKeyPressed2);
	g_scriptCommands.Add(&kCommandInfo_TapKey);
	g_scriptCommands.Add(&kCommandInfo_HoldKey);
	g_scriptCommands.Add(&kCommandInfo_ReleaseKey);
	g_scriptCommands.Add(&kCommandInfo_HammerKey);
	g_scriptCommands.Add(&kCommandInfo_AHammerKey);
	g_scriptCommands.Add(&kCommandInfo_UnHammerKey);
	g_scriptCommands.Add(&kCommandInfo_DisableKey);
	g_scriptCommands.Add(&kCommandInfo_EnableKey);
	g_scriptCommands.Add(&kCommandInfo_MoveMouseX);
	g_scriptCommands.Add(&kCommandInfo_MoveMouseY);
	g_scriptCommands.Add(&kCommandInfo_SetMouseSpeedX);
	g_scriptCommands.Add(&kCommandInfo_SetMouseSpeedY);
	g_scriptCommands.Add(&kCommandInfo_DisableMouse);
	g_scriptCommands.Add(&kCommandInfo_EnableMouse);
	g_scriptCommands.Add(&kCommandInfo_GetOBSEVersion);
	ImportConsoleCommand("SetGameSetting");
	ImportConsoleCommand("SetINISetting");
	ImportConsoleCommand("GetINISetting");
	ImportConsoleCommand("SetFog");
	ImportConsoleCommand("SetClipDist");
	ImportConsoleCommand("SetImageSpaceGlow");
	ImportConsoleCommand("ToggleDetection");
	ImportConsoleCommand("SetCameraFOV");
	ImportConsoleCommand("SexChange");
	ImportConsoleCommand("RefreshINI");
	ImportConsoleCommand("HairTint");
	ImportConsoleCommand("SetTargetRefraction");
	ImportConsoleCommand("SetTargetRefractionFire");
	ImportConsoleCommand("SetSkyParam");
	ImportConsoleCommand("RunMemoryPass");
	ImportConsoleCommand("ModWaterShader");
	ImportConsoleCommand("WaterShallowColor");
	ImportConsoleCommand("WaterDeepColor");
	ImportConsoleCommand("WaterReflectionColor");
	ImportConsoleCommand("SetGamma");
	ImportConsoleCommand("SetHDRParam");
	g_scriptCommands.Add(&kCommandInfo_GetCurrentValue);
	g_scriptCommands.Add(&kCommandInfo_GetObjectValue);
	g_scriptCommands.Add(&kCommandInfo_GetBaseObject);
	g_scriptCommands.Add(&kCommandInfo_GetType);
	g_scriptCommands.Add(&kCommandInfo_IsWeapon);
	g_scriptCommands.Add(&kCommandInfo_IsAmmo);
	g_scriptCommands.Add(&kCommandInfo_IsArmor);
	g_scriptCommands.Add(&kCommandInfo_IsClothing);
	g_scriptCommands.Add(&kCommandInfo_IsBook);
	g_scriptCommands.Add(&kCommandInfo_IsIngredient);
	g_scriptCommands.Add(&kCommandInfo_IsContainer);
	g_scriptCommands.Add(&kCommandInfo_IsKey);
	g_scriptCommands.Add(&kCommandInfo_IsAlchemyItem);
	g_scriptCommands.Add(&kCommandInfo_IsApparatus);
	g_scriptCommands.Add(&kCommandInfo_IsSoulGem);
	g_scriptCommands.Add(&kCommandInfo_IsSigilStone);
	g_scriptCommands.Add(&kCommandInfo_IsDoor);
	g_scriptCommands.Add(&kCommandInfo_IsActivator);
	g_scriptCommands.Add(&kCommandInfo_IsLight);
	g_scriptCommands.Add(&kCommandInfo_IsFurniture);
	g_scriptCommands.Add(&kCommandInfo_HasSpell);
	g_scriptCommands.Add(&kCommandInfo_GetClass);
	g_scriptCommands.Add(&kCommandInfo_IsClassSkill);
	g_scriptCommands.Add(&kCommandInfo_GetClassAttribute);
	g_scriptCommands.Add(&kCommandInfo_GetClassSkill);
	g_scriptCommands.Add(&kCommandInfo_GetClassSpecialization);
	g_scriptCommands.Add(&kCommandInfo_ModActorValue2);
	g_scriptCommands.Add(&kCommandInfo_SetNumericGameSetting);
	g_scriptCommands.Add(&kCommandInfo_GetControl);
	g_scriptCommands.Add(&kCommandInfo_GetAltControl);
	g_scriptCommands.Add(&kCommandInfo_GetNumKeysPressed);
	g_scriptCommands.Add(&kCommandInfo_GetKeyPress);
	g_scriptCommands.Add(&kCommandInfo_GetEquippedCurrentValue);
	g_scriptCommands.Add(&kCommandInfo_GetEquippedObjectValue);
	g_scriptCommands.Add(&kCommandInfo_GetMagicItemValue);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectValue);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectCodeValue);
	g_scriptCommands.Add(&kCommandInfo_MagicItemHasEffect);
	g_scriptCommands.Add(&kCommandInfo_MagicItemHasEffectCode);
	g_scriptCommands.Add(&kCommandInfo_GetNumMouseButtonsPressed);
	g_scriptCommands.Add(&kCommandInfo_GetMouseButtonPress);

	// v0009
	g_scriptCommands.Add(&kCommandInfo_IsRefEssential);
	g_scriptCommands.Add(&kCommandInfo_SetRefEssential);
	g_scriptCommands.Add(&kCommandInfo_GetWeight);
	g_scriptCommands.Add(&kCommandInfo_SetWeight);
	g_scriptCommands.Add(&kCommandInfo_ModWeight);
	g_scriptCommands.Add(&kCommandInfo_GetGoldValue);
	g_scriptCommands.Add(&kCommandInfo_SetGoldValue);
	g_scriptCommands.Add(&kCommandInfo_ModGoldValue);
	g_scriptCommands.Add(&kCommandInfo_GetObjectHealth);
	g_scriptCommands.Add(&kCommandInfo_SetObjectHealth);
	g_scriptCommands.Add(&kCommandInfo_ModObjectHealth);
	g_scriptCommands.Add(&kCommandInfo_GetEquipmentSlot);
	g_scriptCommands.Add(&kCommandInfo_SetEquipmentSlot);
	g_scriptCommands.Add(&kCommandInfo_GetObjectCharge);
	g_scriptCommands.Add(&kCommandInfo_SetObjectCharge);
	g_scriptCommands.Add(&kCommandInfo_ModObjectCharge);
	g_scriptCommands.Add(&kCommandInfo_IsQuestItem);
	g_scriptCommands.Add(&kCommandInfo_SetQuestItem);
	g_scriptCommands.Add(&kCommandInfo_GetEnchantment);
	g_scriptCommands.Add(&kCommandInfo_SetEnchantment);
	g_scriptCommands.Add(&kCommandInfo_RemoveEnchantment);
	g_scriptCommands.Add(&kCommandInfo_GetAttackDamage);
	g_scriptCommands.Add(&kCommandInfo_SetAttackDamage);
	g_scriptCommands.Add(&kCommandInfo_ModAttackDamage);
	g_scriptCommands.Add(&kCommandInfo_GetWeaponReach);
	g_scriptCommands.Add(&kCommandInfo_SetWeaponReach);
	g_scriptCommands.Add(&kCommandInfo_ModWeaponReach);
	g_scriptCommands.Add(&kCommandInfo_GetWeaponSpeed);
	g_scriptCommands.Add(&kCommandInfo_SetWeaponSpeed);
	g_scriptCommands.Add(&kCommandInfo_ModWeaponSpeed);
	g_scriptCommands.Add(&kCommandInfo_GetWeaponType);
	g_scriptCommands.Add(&kCommandInfo_SetWeaponType);
	g_scriptCommands.Add(&kCommandInfo_GetIgnoresResistance);
	g_scriptCommands.Add(&kCommandInfo_SetIgnoresResistance);
	g_scriptCommands.Add(&kCommandInfo_GetArmorAR);
	g_scriptCommands.Add(&kCommandInfo_SetArmorAR);
	g_scriptCommands.Add(&kCommandInfo_ModArmorAR);
	g_scriptCommands.Add(&kCommandInfo_GetArmorType);
	g_scriptCommands.Add(&kCommandInfo_SetArmorType);
	g_scriptCommands.Add(&kCommandInfo_SoulLevel);
	g_scriptCommands.Add(&kCommandInfo_GetSoulGemCapacity);
	g_scriptCommands.Add(&kCommandInfo_IsFood);
	g_scriptCommands.Add(&kCommandInfo_SetIsFood);
	g_scriptCommands.Add(&kCommandInfo_IsPoison);
	g_scriptCommands.Add(&kCommandInfo_SetName);
	g_scriptCommands.Add(&kCommandInfo_SetModelPath);
	g_scriptCommands.Add(&kCommandInfo_SetIconPath);
	g_scriptCommands.Add(&kCommandInfo_SetMaleBipedPath);
	g_scriptCommands.Add(&kCommandInfo_SetFemaleBipedPath);
	g_scriptCommands.Add(&kCommandInfo_SetMaleGroundPath);
	g_scriptCommands.Add(&kCommandInfo_SetFemaleGroundPath);
	g_scriptCommands.Add(&kCommandInfo_SetMaleIconPath);
	g_scriptCommands.Add(&kCommandInfo_SetFemaleIconPath);
	g_scriptCommands.Add(&kCommandInfo_GetEquippedCurrentHealth);
	g_scriptCommands.Add(&kCommandInfo_SetEquippedCurrentHealth);
	g_scriptCommands.Add(&kCommandInfo_ModEquippedCurrentHealth);
	g_scriptCommands.Add(&kCommandInfo_GetEquippedCurrentCharge);
	g_scriptCommands.Add(&kCommandInfo_SetEquippedCurrentCharge);
	g_scriptCommands.Add(&kCommandInfo_ModEquippedCurrentCharge);
	g_scriptCommands.Add(&kCommandInfo_GetEquippedWeaponPoison);
	g_scriptCommands.Add(&kCommandInfo_SetEquippedWeaponPoison);
	g_scriptCommands.Add(&kCommandInfo_RemoveEquippedWeaponPoison);
	g_scriptCommands.Add(&kCommandInfo_CloneForm);
	g_scriptCommands.Add(&kCommandInfo_IsClonedForm);
	g_scriptCommands.Add(&kCommandInfo_SetNumericINISetting);
	g_scriptCommands.Add(&kCommandInfo_GetMagicItemType);
	g_scriptCommands.Add(&kCommandInfo_GetMagicItemEffectCount);
	g_scriptCommands.Add(&kCommandInfo_GetNthEffectItemCode);
	g_scriptCommands.Add(&kCommandInfo_GetNthEffectItemMagnitude);
	g_scriptCommands.Add(&kCommandInfo_GetNthEffectItemArea);
	g_scriptCommands.Add(&kCommandInfo_GetNthEffectItemDuration);
	g_scriptCommands.Add(&kCommandInfo_GetNthEffectItemRange);
	g_scriptCommands.Add(&kCommandInfo_GetNthEffectItemActorValue);
	g_scriptCommands.Add(&kCommandInfo_GetSpellType);
	g_scriptCommands.Add(&kCommandInfo_GetSpellMagickaCost);
	g_scriptCommands.Add(&kCommandInfo_GetSpellMasteryLevel);
	g_scriptCommands.Add(&kCommandInfo_GetEnchantmentType);
	g_scriptCommands.Add(&kCommandInfo_GetEnchantmentCharge);
	g_scriptCommands.Add(&kCommandInfo_GetEnchantmentCost);	
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectCode);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectBaseCost);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectSchool);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectProjectileSpeed);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectEnchantFactor);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectBarterFactor);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectBaseCostC);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectSchoolC);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectProjectileSpeedC);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectEnchantFactorC);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectBarterFactorC);
	g_scriptCommands.Add(&kCommandInfo_SetNthEffectItemMagnitude);
	g_scriptCommands.Add(&kCommandInfo_ModNthEffectItemMagnitude);
	g_scriptCommands.Add(&kCommandInfo_SetNthEffectItemArea);
	g_scriptCommands.Add(&kCommandInfo_ModNthEffectItemArea);
	g_scriptCommands.Add(&kCommandInfo_SetNthEffectItemDuration);
	g_scriptCommands.Add(&kCommandInfo_ModNthEffectItemDuration);
	g_scriptCommands.Add(&kCommandInfo_SetNthEffectItemRange);
	g_scriptCommands.Add(&kCommandInfo_SetNthEffectItemActorValue);
	g_scriptCommands.Add(&kCommandInfo_SetNthEffectItemActorValueC);
	g_scriptCommands.Add(&kCommandInfo_RemoveNthEffectItem);
	g_scriptCommands.Add(&kCommandInfo_SetSpellType);
	g_scriptCommands.Add(&kCommandInfo_SetSpellMagickaCost);
	g_scriptCommands.Add(&kCommandInfo_ModSpellMagickaCost);
	g_scriptCommands.Add(&kCommandInfo_SetSpellMasteryLevel);
	g_scriptCommands.Add(&kCommandInfo_SetEnchantmentCharge);
	g_scriptCommands.Add(&kCommandInfo_ModEnchantmentCharge);
	g_scriptCommands.Add(&kCommandInfo_SetEnchantmentCost);
	g_scriptCommands.Add(&kCommandInfo_ModEnchantmentCost);
	g_scriptCommands.Add(&kCommandInfo_SetEnchantmentType);
	g_scriptCommands.Add(&kCommandInfo_SetNumericINISetting);
	g_scriptCommands.Add(&kCommandInfo_GetNumericINISetting);
	g_scriptCommands.Add(&kCommandInfo_GetActorLightAmount);
	g_scriptCommands.Add(&kCommandInfo_GetGameLoaded);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectHostile);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectHostileC);
	g_scriptCommands.Add(&kCommandInfo_SaveIP);
	g_scriptCommands.Add(&kCommandInfo_RestoreIP);

	// v0010
	g_scriptCommands.Add(&kCommandInfo_CopyNthEffectItem);
	g_scriptCommands.Add(&kCommandInfo_CopyAllEffectItems);
	g_scriptCommands.Add(&kCommandInfo_AddEffectItem);
	g_scriptCommands.Add(&kCommandInfo_AddEffectItemC);
	g_scriptCommands.Add(&kCommandInfo_IsMagicItemAutoCalc);
	g_scriptCommands.Add(&kCommandInfo_SetMagicItemAutoCalc);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectForSpellmaking);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectForSpellmakingC);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectForEnchanting);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectForEnchantingC);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectDetrimental);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectDetrimentalC);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectCanRecover);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectCanRecoverC);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectMagnitudePercent);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectMagnitudePercentC);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectFXPersists);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectFXPersistsC);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectOnSelfAllowed);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectOnSelfAllowedC);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectOnTouchAllowed);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectOnTouchAllowedC);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectOnTargetAllowed);
	g_scriptCommands.Add(&kCommandInfo_IsMagicEffectOnTargetAllowedC);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectHasNoDuration);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectHasNoDurationC);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectHasNoMagnitude);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectHasNoMagnitudeC);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectHasNoArea);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectHasNoAreaC);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectHasNoIngredient);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectHasNoIngredientC);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectHasNoHitEffect);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectHasNoHitEffectC);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectUsesWeapon);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectUsesWeaponC);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectUsesArmor);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectUsesArmorC);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectUsesCreature);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectUsesCreatureC);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectUsesSkill);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectUsesSkillC);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectUsesAttribute);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectUsesAttributeC);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectUsesOtherActorValue);
	g_scriptCommands.Add(&kCommandInfo_MagicEffectUsesOtherActorValueC);
	g_scriptCommands.Add(&kCommandInfo_GetCurrentHealth);
	g_scriptCommands.Add(&kCommandInfo_GetCurrentCharge);
	g_scriptCommands.Add(&kCommandInfo_GetCurrentSoulLevel);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectOtherActorValue);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectOtherActorValueC);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectUsedObject);
	g_scriptCommands.Add(&kCommandInfo_GetMagicEffectUsedObjectC);
	g_scriptCommands.Add(&kCommandInfo_IsCreature);
	g_scriptCommands.Add(&kCommandInfo_GetCreatureType);
	g_scriptCommands.Add(&kCommandInfo_GetCreatureCombatSkill);
	g_scriptCommands.Add(&kCommandInfo_GetCreatureMagicSkill);
	g_scriptCommands.Add(&kCommandInfo_GetCreatureStealthSkill);
	g_scriptCommands.Add(&kCommandInfo_GetBookCantBeTaken);
	g_scriptCommands.Add(&kCommandInfo_GetBookIsScroll);
	g_scriptCommands.Add(&kCommandInfo_GetBookSkillTaught);
	g_scriptCommands.Add(&kCommandInfo_SetBookCantBeTaken);
	g_scriptCommands.Add(&kCommandInfo_SetBookIsScroll);
	g_scriptCommands.Add(&kCommandInfo_SetBookSkillTaught);
	g_scriptCommands.Add(&kCommandInfo_GetApparatusType);
	g_scriptCommands.Add(&kCommandInfo_SetApparatusType);
	g_scriptCommands.Add(&kCommandInfo_GetQuality);
	g_scriptCommands.Add(&kCommandInfo_SetQuality);
	g_scriptCommands.Add(&kCommandInfo_ModQuality);
	g_scriptCommands.Add(&kCommandInfo_SetSoulLevel);
	g_scriptCommands.Add(&kCommandInfo_SetSoulGemCapacity);
	g_scriptCommands.Add(&kCommandInfo_ModModelPath);
	g_scriptCommands.Add(&kCommandInfo_ModIconPath);
	g_scriptCommands.Add(&kCommandInfo_ModMaleBipedPath);
	g_scriptCommands.Add(&kCommandInfo_ModFemaleBipedPath);
	g_scriptCommands.Add(&kCommandInfo_ModMaleGroundPath);
	g_scriptCommands.Add(&kCommandInfo_ModFemaleGroundPath);
	g_scriptCommands.Add(&kCommandInfo_ModMaleIconPath);
	g_scriptCommands.Add(&kCommandInfo_ModFemaleIconPath);
	g_scriptCommands.Add(&kCommandInfo_CompareModelPath);
	g_scriptCommands.Add(&kCommandInfo_CompareIconPath);
	g_scriptCommands.Add(&kCommandInfo_CompareMaleBipedPath);
	g_scriptCommands.Add(&kCommandInfo_CompareFemaleBipedPath);
	g_scriptCommands.Add(&kCommandInfo_CompareMaleGroundPath);
	g_scriptCommands.Add(&kCommandInfo_CompareFemaleGroundPath);
	g_scriptCommands.Add(&kCommandInfo_CompareMaleIconPath);
	g_scriptCommands.Add(&kCommandInfo_CompareFemaleIconPath);
	g_scriptCommands.Add(&kCommandInfo_GetPlayerSpellCount);
	g_scriptCommands.Add(&kCommandInfo_GetNthPlayerSpell);
	g_scriptCommands.Add(&kCommandInfo_CopyModelPath);
	g_scriptCommands.Add(&kCommandInfo_CopyIconPath);
	g_scriptCommands.Add(&kCommandInfo_CopyMaleBipedPath);
	g_scriptCommands.Add(&kCommandInfo_CopyFemaleBipedPath);
	g_scriptCommands.Add(&kCommandInfo_CopyMaleGroundPath);
	g_scriptCommands.Add(&kCommandInfo_CopyFemaleGroundPath);
	g_scriptCommands.Add(&kCommandInfo_CopyMaleIconPath);
	g_scriptCommands.Add(&kCommandInfo_CopyFemaleIconPath);
	ImportConsoleCommand("TCL");
	ImportConsoleCommand("Save");
	ImportConsoleCommand("ToggleAI");
	ImportConsoleCommand("ToggleCombatAI");
	ImportConsoleCommand("ToggleMenus");
	g_scriptCommands.Add(&kCommandInfo_IsScripted);
	g_scriptCommands.Add(&kCommandInfo_CompareName);
	g_scriptCommands.Add(&kCommandInfo_MenuTapKey);
	g_scriptCommands.Add(&kCommandInfo_MenuHoldKey);
	g_scriptCommands.Add(&kCommandInfo_MenuReleaseKey);
	g_scriptCommands.Add(&kCommandInfo_GetEquipmentSlotMask);
	g_scriptCommands.Add(&kCommandInfo_LeftShift);
	g_scriptCommands.Add(&kCommandInfo_RightShift);
	g_scriptCommands.Add(&kCommandInfo_LogicalAnd);
	g_scriptCommands.Add(&kCommandInfo_LogicalOr);
	g_scriptCommands.Add(&kCommandInfo_LogicalXor);
	g_scriptCommands.Add(&kCommandInfo_LogicalNot);
	g_scriptCommands.Add(&kCommandInfo_GetSpellSchool);
	g_scriptCommands.Add(&kCommandInfo_IsNthEffectItemScripted);
	g_scriptCommands.Add(&kCommandInfo_GetNthEffectItemScript);
	g_scriptCommands.Add(&kCommandInfo_GetNthEffectItemScriptVisualEffect);
	g_scriptCommands.Add(&kCommandInfo_GetNthEffectItemScriptSchool);
	g_scriptCommands.Add(&kCommandInfo_IsNthEffectItemScriptHostile);
	g_scriptCommands.Add(&kCommandInfo_SetNthEffectItemScriptVisualEffect);
	g_scriptCommands.Add(&kCommandInfo_SetNthEffectItemScriptVisualEffectC);
	g_scriptCommands.Add(&kCommandInfo_SetNthEffectItemScriptSchool);
	g_scriptCommands.Add(&kCommandInfo_SetNthEffectItemScriptHostile);
	ImportConsoleCommand("CAL");
	g_scriptCommands.Add(&kCommandInfo_Fmod);
	g_scriptCommands.Add(&kCommandInfo_CopyName);
	g_scriptCommands.Add(&kCommandInfo_GetFPS);
	g_scriptCommands.Add(&kCommandInfo_IsThirdPerson);

	// v0011
	g_scriptCommands.Add(&kCommandInfo_IsGlobalCollisionDisabled);
	g_scriptCommands.Add(&kCommandInfo_SetDisableGlobalCollision);
	g_scriptCommands.Add(&kCommandInfo_AddFullEffectItem);
	g_scriptCommands.Add(&kCommandInfo_AddFullEffectItemC);
	g_scriptCommands.Add(&kCommandInfo_GetPlayerSkillUse);
	g_scriptCommands.Add(&kCommandInfo_RunBatchScript);
	g_scriptCommands.Add(&kCommandInfo_GetSkillUseIncrement);
	g_scriptCommands.Add(&kCommandInfo_SetSkillUseIncrement);
	g_scriptCommands.Add(&kCommandInfo_IncrementPlayerSkillUse);
	g_scriptCommands.Add(&kCommandInfo_IsClassAttribute);
	ImportConsoleCommand("SaveINI");
	ImportConsoleCommand("QuitGame");
	g_scriptCommands.Add(&kCommandInfo_ModName);
	ImportConsoleCommand("TGM");
	g_scriptCommands.Add(&kCommandInfo_AppendToName);

	// v0012
	g_scriptCommands.Add(&kCommandInfo_GetRaceAttribute);
	g_scriptCommands.Add(&kCommandInfo_GetRaceAttributeC);
	g_scriptCommands.Add(&kCommandInfo_GetRaceSkillBonus);
	g_scriptCommands.Add(&kCommandInfo_IsRaceBonusSkillC);
	g_scriptCommands.Add(&kCommandInfo_IsRaceBonusSkill);
	g_scriptCommands.Add(&kCommandInfo_GetRaceSkillBonusC);
	g_scriptCommands.Add(&kCommandInfo_GetNthRaceBonusSkill);
	g_scriptCommands.Add(&kCommandInfo_GetMerchantContainer);
	g_scriptCommands.Add(&kCommandInfo_GetCurrentWeatherID);
	g_scriptCommands.Add(&kCommandInfo_GetCurrentClimateID);
	g_scriptCommands.Add(&kCommandInfo_GetClimateSunriseBegin);
	g_scriptCommands.Add(&kCommandInfo_GetClimateSunriseEnd);
	g_scriptCommands.Add(&kCommandInfo_GetClimateSunsetBegin);
	g_scriptCommands.Add(&kCommandInfo_GetClimateSunsetEnd);
	ImportConsoleCommand("TFC");
	g_scriptCommands.Add(&kCommandInfo_GetSpellExplodesWithNoTarget);
	g_scriptCommands.Add(&kCommandInfo_SetSpellExplodesWithNoTarget);
	g_scriptCommands.Add(&kCommandInfo_RemoveAllEffectItems);
	g_scriptCommands.Add(&kCommandInfo_SetNthEffectItemScript);
	g_scriptCommands.Add(&kCommandInfo_SetNthEffectItemScriptName);
	g_scriptCommands.Add(&kCommandInfo_SetMerchantContainer);
	g_scriptCommands.Add(&kCommandInfo_MagicItemHasEffectCount);
	g_scriptCommands.Add(&kCommandInfo_MagicItemHasEffectCountCode);
	g_scriptCommands.Add(&kCommandInfo_GetScript);
	g_scriptCommands.Add(&kCommandInfo_RemoveScript);
	g_scriptCommands.Add(&kCommandInfo_SetScript);
	g_scriptCommands.Add(&kCommandInfo_IsPlayable);
	g_scriptCommands.Add(&kCommandInfo_SetPlayable);

	// v0013
	g_scriptCommands.Add(&kCommandInfo_SetPCAMurderer);
	g_scriptCommands.Add(&kCommandInfo_GetClimateMoonPhaseLength);
	g_scriptCommands.Add(&kCommandInfo_GetClimateHasMasser);
	g_scriptCommands.Add(&kCommandInfo_GetClimateHasSecunda);
	g_scriptCommands.Add(&kCommandInfo_SetClimateSunriseBegin);
	g_scriptCommands.Add(&kCommandInfo_SetClimateSunriseEnd);
	g_scriptCommands.Add(&kCommandInfo_SetClimateSunsetBegin);
	g_scriptCommands.Add(&kCommandInfo_SetClimateSunsetEnd);
	g_scriptCommands.Add(&kCommandInfo_SetClimateMoonPhaseLength);
	g_scriptCommands.Add(&kCommandInfo_SetClimateHasMasser);
	g_scriptCommands.Add(&kCommandInfo_SetClimateHasSecunda);
	g_scriptCommands.Add(&kCommandInfo_GetClimateVolatility);
	g_scriptCommands.Add(&kCommandInfo_SetClimateVolatility);
	g_scriptCommands.Add(&kCommandInfo_IsKeyPressed3);
	g_scriptCommands.Add(&kCommandInfo_IsControlPressed);
	g_scriptCommands.Add(&kCommandInfo_DisableControl);
	g_scriptCommands.Add(&kCommandInfo_EnableControl);
	g_scriptCommands.Add(&kCommandInfo_OnKeyDown);
	g_scriptCommands.Add(&kCommandInfo_OnControlDown);
	g_scriptCommands.Add(&kCommandInfo_GetActiveEffectCount);
	g_scriptCommands.Add(&kCommandInfo_GetNthActiveEffectCode);
	g_scriptCommands.Add(&kCommandInfo_GetNthActiveEffectMagnitude);
	g_scriptCommands.Add(&kCommandInfo_GetNthActiveEffectDuration);
	g_scriptCommands.Add(&kCommandInfo_GetNthActiveEffectTimeElapsed);
	g_scriptCommands.Add(&kCommandInfo_GetNthActiveEffectMagicItem);
	g_scriptCommands.Add(&kCommandInfo_GetNthActiveEffectCaster);
	g_scriptCommands.Add(&kCommandInfo_GetNthActiveEffectMagicItemIndex);
	g_scriptCommands.Add(&kCommandInfo_GetTotalActiveEffectMagnitude);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAENonAbilityMagnitude);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAEAbilityMagnitude);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAESpellMagnitude);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAEDiseaseMagnitude);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAELesserPowerMagnitude);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAEPowerMagnitude);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAEAllSpellsMagnitude);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAEEnchantmentMagnitude);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAEAlchemyMagnitude);
	ImportConsoleCommand("LoadGame");
	g_scriptCommands.Add(&kCommandInfo_IsPluginInstalled);
	g_scriptCommands.Add(&kCommandInfo_GetPluginVersion);
	g_scriptCommands.Add(&kCommandInfo_GetNthActiveEffectData);
	g_scriptCommands.Add(&kCommandInfo_GetTotalActiveEffectMagnitudeC);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAENonAbilityMagnitudeC);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAEAbilityMagnitudeC);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAESpellMagnitudeC);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAEDiseaseMagnitudeC);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAELesserPowerMagnitudeC);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAEPowerMagnitudeC);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAEAllSpellsMagnitudeC);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAEEnchantmentMagnitudeC);
	g_scriptCommands.Add(&kCommandInfo_GetTotalAEAlchemyMagnitudeC);
	g_scriptCommands.Add(&kCommandInfo_ParentCellHasWater);
	g_scriptCommands.Add(&kCommandInfo_GetParentCellWaterHeight);
	g_scriptCommands.Add(&kCommandInfo_IsUnderWater);
	g_scriptCommands.Add(&kCommandInfo_GetDebugSelection);
	g_scriptCommands.Add(&kCommandInfo_AddToLeveledList);
	g_scriptCommands.Add(&kCommandInfo_RemoveFromLeveledList);
	g_scriptCommands.Add(&kCommandInfo_GetTravelHorse);
	g_scriptCommands.Add(&kCommandInfo_SetTravelHorse);
	g_scriptCommands.Add(&kCommandInfo_CompareNames);
	g_scriptCommands.Add(&kCommandInfo_GetGameRestarted);
	g_scriptCommands.Add(&kCommandInfo_TapControl);
	g_scriptCommands.Add(&kCommandInfo_GetWeatherWindSpeed);
	g_scriptCommands.Add(&kCommandInfo_GetWeatherCloudSpeedLower);
	g_scriptCommands.Add(&kCommandInfo_GetWeatherCloudSpeedUpper);
	g_scriptCommands.Add(&kCommandInfo_GetWeatherTransDelta);
	g_scriptCommands.Add(&kCommandInfo_GetWeatherSunGlare);
	g_scriptCommands.Add(&kCommandInfo_GetWeatherSunDamage);
	g_scriptCommands.Add(&kCommandInfo_GetWeatherFogDayNear);
	g_scriptCommands.Add(&kCommandInfo_GetWeatherFogDayFar);
	g_scriptCommands.Add(&kCommandInfo_GetWeatherFogNightNear);
	g_scriptCommands.Add(&kCommandInfo_GetWeatherFogNightFar);
	g_scriptCommands.Add(&kCommandInfo_GetWeatherHDRValue);
	g_scriptCommands.Add(&kCommandInfo_SetWeatherHDRValue);
	g_scriptCommands.Add(&kPaddingCommand);	// used to be SetCurrentClimate
	g_scriptCommands.Add(&kCommandInfo_GetWeatherColor);
	g_scriptCommands.Add(&kCommandInfo_SetWeatherColor);
	g_scriptCommands.Add(&kCommandInfo_SetWeatherLightningFrequency);
	g_scriptCommands.Add(&kCommandInfo_SetWeatherWindSpeed);
	g_scriptCommands.Add(&kCommandInfo_SetWeatherCloudSpeedLower);
	g_scriptCommands.Add(&kCommandInfo_SetWeatherCloudSpeedUpper);
	g_scriptCommands.Add(&kCommandInfo_SetWeatherTransDelta);
	g_scriptCommands.Add(&kCommandInfo_SetWeatherSunGlare);
	g_scriptCommands.Add(&kCommandInfo_SetWeatherSunDamage);
	g_scriptCommands.Add(&kCommandInfo_SetWeatherFogDayNear);
	g_scriptCommands.Add(&kCommandInfo_SetWeatherFogDayFar);
	g_scriptCommands.Add(&kCommandInfo_SetWeatherFogNightNear);
	g_scriptCommands.Add(&kCommandInfo_SetWeatherFogNightFar);
	g_scriptCommands.Add(&kCommandInfo_RefreshCurrentClimate);
	g_scriptCommands.Add(&kCommandInfo_CalcLeveledItem);
	g_scriptCommands.Add(&kCommandInfo_GetOpenKey);
	g_scriptCommands.Add(&kCommandInfo_SetOpenKey);
	g_scriptCommands.Add(&kCommandInfo_GetWeatherLightningFrequency);
	g_scriptCommands.Add(&kCommandInfo_SetNthActiveEffectMagnitude);
	g_scriptCommands.Add(&kCommandInfo_ModNthActiveEffectMagnitude);
	g_scriptCommands.Add(&kCommandInfo_GetScriptActiveEffectIndex);
	g_scriptCommands.Add(&kCommandInfo_GetParentCellOwner);
	g_scriptCommands.Add(&kCommandInfo_GetOwner);
	g_scriptCommands.Add(&kCommandInfo_GetOwningFactionRequiredRank);
	g_scriptCommands.Add(&kCommandInfo_GetParentCellOwningFactionRequiredRank);
	g_scriptCommands.Add(&kCommandInfo_SetHair);
	g_scriptCommands.Add(&kCommandInfo_CopyHair);
	g_scriptCommands.Add(&kCommandInfo_SetEyes);
	g_scriptCommands.Add(&kCommandInfo_CopyEyes);
	g_scriptCommands.Add(&kCommandInfo_GetContainerRespawns);
	g_scriptCommands.Add(&kCommandInfo_SetContainerRespawns);
	g_scriptCommands.Add(&kCommandInfo_GetCreatureReach);
	g_scriptCommands.Add(&kCommandInfo_GetCreatureBaseScale);
	g_scriptCommands.Add(&kCommandInfo_GetCreatureSoulLevel);
	g_scriptCommands.Add(&kCommandInfo_IsLoadDoor);
	g_scriptCommands.Add(&kCommandInfo_GetLinkedDoor);
	g_scriptCommands.Add(&kCommandInfo_GetTeleportCell);
	g_scriptCommands.Add(&kCommandInfo_GetFirstRef);
	g_scriptCommands.Add(&kCommandInfo_GetNextRef);
	g_scriptCommands.Add(&kCommandInfo_GetNumRefs);
	g_scriptCommands.Add(&kCommandInfo_RefreshControlMap);

	// v0014
/*
	g_scriptCommands.Add(&kCommandInfo_IsPersistent);
	g_scriptCommands.Add(&kCommandInfo_GetFormID);
	g_scriptCommands.Add(&kCommandInfo_IsOffLimits);
	ImportConsoleCommand("PlayerSpellBook");
	ImportConsoleCommand("ToggleMapMarkers");
*/

	/* to add later if problems can be solved
	g_scriptCommands.Add(&kCommandInfo_SetCurrentClimate); // too many problems
	g_scriptCommands.Add(&kCommandInfo_SetWorldspaceClimate);
	g_scriptCommands.Add(&kCommandInfo_GetParentWorldspace);
	g_scriptCommands.Add(&kCommandInfo_FloatFromFile);
	g_scriptCommands.Add(&kCommandInfo_FloatToFile);
	g_scriptCommands.Add(&kCommandInfo_PrintToFile);
	*/

	/* to be implemented later
	g_scriptCommands.Add(&kCommandInfo_RemoveHostileEffectItems);
	g_scriptCommands.Add(&kCommandInfo_RemoveNonHostileEffectItems);
	g_scriptCommands.Add(&kCommandInfo_SetBookSkillTaughtC);
	g_scriptCommands.Add(&kCommandInfo_RemoveAllSpells); // had some crashing problems
	g_scriptCommands.Add(&kCommandInfo_SetScriptedEffectItem);
	*/

#ifdef _DEBUG
	// only for testing, don't use in scripts
	g_scriptCommands.Add(&kCommandInfo_TestExtractArgs);
	g_scriptCommands.Add(&kCommandInfo_DumpVarInfo);

#ifdef OBLIVION
	g_scriptCommands.Add(&kCommandInfo_DebugMemDump);
#endif

#endif

	if(!g_pluginManager.Init())
		_WARNING("couldn't load plugins");

#ifdef _DEBUG
#if 1
	_MESSAGE("console commands");
	g_consoleCommands.Dump();
	_MESSAGE("script commands");
	g_scriptCommands.Dump();
#endif
#endif

	// patch the code
	ApplyPatch(kPatch_ScriptCommands_Start, (UInt32)g_scriptCommands.GetStart());
	ApplyPatch(kPatch_ScriptCommands_End, (UInt32)g_scriptCommands.GetEnd());
	ApplyPatch(kPatch_ScriptCommands_MaxIdx, g_scriptCommands.GetMaxID());

	_MESSAGE("patched");
}

void CommandTable::Read(CommandInfo * start, CommandInfo * end)
{
	UInt32	numCommands = end - start;
	m_commands.reserve(m_commands.size() + numCommands);

	for(; start != end; ++start)
		Add(start);
}

void CommandTable::Add(CommandInfo * info)
{
	UInt32	backCommandID = m_baseID + m_commands.size();	// opcode of the next command to add

	info->opcode = m_curID;

	if(m_curID == backCommandID)
	{
		// adding at the end?
		m_commands.push_back(*info);
	}
	else if(m_curID < backCommandID)
	{
		// adding to existing data?
		ASSERT(m_curID >= m_baseID);

		m_commands[m_curID - m_baseID] = *info;
	}
	else
	{
		HALT(CommandTable::Add: adding past the end);
	}

	m_curID++;
}

void CommandTable::PadTo(UInt32 id, CommandInfo * info)
{
	if(!info) info = &kPaddingCommand;

	while(m_baseID + m_commands.size() < id) Add(info);
}

void CommandTable::Dump(void)
{
	for(CommandList::iterator iter = m_commands.begin(); iter != m_commands.end(); ++iter)
	{
		_MESSAGE("%08X %04X %s %s", iter->opcode, iter->unk0, iter->longName, iter->shortName);
//		_MESSAGE("%08X %08X %08X %08X %08X %s", iter->params, iter->unk1, iter->unk2, iter->unk3, iter->flags, iter->longName);
		gLog.Indent();

#if 0
		for(UInt32 i = 0; i < iter->numParams; i++)
		{
			ParamInfo	* param = &iter->params[i];
			_MESSAGE("%08X %08X %s", param->typeID, param->isOptional, param->typeStr);
		}
#endif

		gLog.Outdent();
	}
}

CommandInfo * CommandTable::GetStart(void)
{
	return &m_commands[0];
}

CommandInfo * CommandTable::GetEnd(void)
{
	return &m_commands[0] + m_commands.size();
}

CommandInfo * CommandTable::GetByName(const char * name)
{
	for(CommandList::iterator iter = m_commands.begin(); iter != m_commands.end(); ++iter)
		if(!_stricmp(name, iter->longName) || (iter->shortName && !_stricmp(name, iter->shortName)))
			return &(*iter);

	return NULL;
}

CommandTable	g_consoleCommands;
CommandTable	g_scriptCommands;
//CommandTable	g_tokenCommands;
