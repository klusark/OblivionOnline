#include "obse/GameAPI.h"
#include "obse/GameObjects.h"
#include "obse/CommandTable.h"
#include <cstdarg>

#include <float.h>
#include "obse/Commands_Input.h"

/***
 *	opcodes
 *	
 *	tokens
 *	0010	begin
 *	0011	end
 *	0012	short		int
 *	0013	long
 *	0014	float
 *	0015	set
 *	0016	if
 *	0017	else
 *	0018	elseif
 *	0019	endif
 *	001A	while_DEPRECATED
 *	001B	endwhile_DEPRECATED
 *	001C	.
 *	001D	scriptname	scn
 *	001E	return
 *	001F	reference	ref
 *	
 ***/

void ScriptEventList::Dump(void)
{
	for(EventEntry * entry = m_eventList; entry; entry = entry->next)
	{
		if(entry->event)
		{
			Event	* event = entry->event;

			Console_Print("%08X (%s) %08X", event->object, GetObjectClassName(event->object), event->eventMask);
		}
	}
}

ScriptEventList::Var * ScriptEventList::GetVariable(UInt32 id)
{
	for(VarEntry * entry = m_vars; entry; entry = entry->next)
		if(entry->var && entry->var->id == id)
			return entry->var;

	return NULL;
}

// arg1 = 1, ignored if canCreateNew is false, passed to 'init' function if a new object is created
typedef void * (* _GetSingleton)(bool canCreateNew, bool arg1);

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

const _Console_Print Console_Print = (_Console_Print)0x0056CDAD;
const _ExtractArgs ExtractArgs = (_ExtractArgs)0x004F0F10;
const _CreateFormInstance CreateFormInstance = (_CreateFormInstance)0x00442040;
const _LookupFormByID LookupFormByID = (_LookupFormByID)0x00465750;
const _FormHeap_Allocate FormHeap_Allocate = (_FormHeap_Allocate)0x00401FA0;
const _FormHeap_Free FormHeap_Free = (_FormHeap_Free)0x00401FC0;
const _GetGlobalScriptStateObj GetGlobalScriptStateObj = (_GetGlobalScriptStateObj)0x005790C0;
const _GetSingleton InterfaceManager_GetSingleton = (_GetSingleton)0x00575230;
const _ShowMessageBox ShowMessageBox = (_ShowMessageBox)0x0056CE20;
const _ShowMessageBox_Callback ShowMessageBox_Callback = (_ShowMessageBox_Callback)0x004F6520;
const _ShowMessageBox_pScriptRefID ShowMessageBox_pScriptRefID = (_ShowMessageBox_pScriptRefID)0x00AF3F40;
const _QueueUIMessage QueueUIMessage = (_QueueUIMessage)0x0056DF90;
MemoryHeap * g_formHeap = (MemoryHeap *)0x00AC3EC0;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2

const _Console_Print Console_Print = (_Console_Print)0x00579A9B;
const _ExtractArgs ExtractArgs = (_ExtractArgs)0x004FAF30;
const _CreateFormInstance CreateFormInstance = (_CreateFormInstance)0x00448000;
const _LookupFormByID LookupFormByID = (_LookupFormByID)0x0046B150;
const _FormHeap_Allocate FormHeap_Allocate = (_FormHeap_Allocate)0x00401F10;
const _FormHeap_Free FormHeap_Free = (_FormHeap_Free)0x00401F30;
const _GetGlobalScriptStateObj GetGlobalScriptStateObj = (_GetGlobalScriptStateObj)0x00585BE0;
const _GetSingleton InterfaceManager_GetSingleton = (_GetSingleton)0x005820B0;
const _ShowMessageBox ShowMessageBox = (_ShowMessageBox)0x00579B10;
const _ShowMessageBox_Callback ShowMessageBox_Callback = (_ShowMessageBox_Callback)0x00500530;
const _ShowMessageBox_pScriptRefID ShowMessageBox_pScriptRefID = (_ShowMessageBox_pScriptRefID)0x00B361C8;
const _QueueUIMessage QueueUIMessage = (_QueueUIMessage)0x0057ABC0;
MemoryHeap * g_formHeap = (MemoryHeap *)0x00000000;	// ### TODO

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416

const _Console_Print Console_Print = (_Console_Print)0x00579B9B;
const _ExtractArgs ExtractArgs = (_ExtractArgs)0x004FAE80;
const _CreateFormInstance CreateFormInstance = (_CreateFormInstance)0x00447DF0;
const _LookupFormByID LookupFormByID = (_LookupFormByID)0x0046B250;
const _FormHeap_Allocate FormHeap_Allocate = (_FormHeap_Allocate)0x00401F00;
const _FormHeap_Free FormHeap_Free = (_FormHeap_Free)0x00401F20;
const _GetGlobalScriptStateObj GetGlobalScriptStateObj = (_GetGlobalScriptStateObj)0x00585C10;
const _GetSingleton InterfaceManager_GetSingleton = (_GetSingleton)0x00582160;
const _ShowMessageBox ShowMessageBox = (_ShowMessageBox)0x00579C10;
const _ShowMessageBox_Callback ShowMessageBox_Callback = (_ShowMessageBox_Callback)0x005003E0;
const _ShowMessageBox_pScriptRefID ShowMessageBox_pScriptRefID = (_ShowMessageBox_pScriptRefID)0x00B361C8;
const _QueueUIMessage QueueUIMessage = (_QueueUIMessage)0x0057ACC0;
MemoryHeap * g_formHeap = (MemoryHeap *)0x00B02020;

#else

#error unsupported version of oblivion

#endif

// make sure this doesn't get optimized
#pragma optimize("", off)

// ### TODO: these functions are really cheesy and not thread-safe, fix it

static UInt32 g_ThisCall_returnAddr = 0;	// this is kludgy but I want to test things with this

__declspec(naked) UInt32 __cdecl ThisCall(UInt32 function, void * _this, ...)
{
	// eax, ecx, edx volatile
	// ebx, ebp, esi, edi must be preserved

	__asm
	{
		//// stack on entry
		// ...
		// args
		// _this
		// function
		// return address <- esp

		pop eax		// eax = return address
		pop edx		// edx = function pointer
		pop ecx		// ecx = _this

		mov g_ThisCall_returnAddr, eax
		
		//// new stack
		// ...
		// args <- esp

		call edx

		push 0
		push 0
		push g_ThisCall_returnAddr

		retn
	}
}

static UInt32 g_ThisStdCall_returnAddr = 0;
static UInt32 g_ThisStdCall_stackAddr = 0;

// like stdcall (ie. called fn pops from stack) but with ecx set to this
__declspec(naked) UInt32 __cdecl ThisStdCall(UInt32 function, void * _this, ...)
{
	__asm
	{
		//// stack
		// return address <- esp
		// function
		// _this
		// args (unknown length)
		// ...

		//// target stack
		// return address <- esp
		// args (unknown length)

		pop	eax
		pop	edx
		pop	ecx

		mov g_ThisStdCall_returnAddr, eax
		mov g_ThisStdCall_stackAddr, esp

		call edx

		mov esp, g_ThisStdCall_stackAddr

		push 0
		push 0
		push g_ThisStdCall_returnAddr

		retn
	}
}

#pragma optimize("", on)

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

UInt32 AddFormToDataHandler(DataHandler * dataHandler, TESForm * form)
{
	return ThisStdCall(0x00448B50, dataHandler, form);
}

DataHandler ** g_dataHandler = (DataHandler **)0x00AED734;

void AddFormToCreatedBaseObjectsList(void * objList, TESForm * form)
{
	ThisStdCall(0x004559B0, objList, form);
}

void ** g_createdBaseObjList = (void **)0x00AED900;

UInt32 NiTPointerMap_Lookup(void * map, void * key, void ** data)
{
	return ThisStdCall(0x00469F90, map, key, data);
}

void * g_gameSettingsTable = (void *)0x00AF182C;
char*** g_baseActorValueNames = (char***)0x00AD33A0;
char** g_extraActorValueNames = (char**)0x00AD3550;

#include "obse/GameRTTI_1_1.inl"

const _Oblivion_DynamicCast Oblivion_DynamicCast = (_Oblivion_DynamicCast)0x009619CE;

CRITICAL_SECTION * g_extraListMutex = (CRITICAL_SECTION *)0x00AEB900;
CRITICAL_SECTION * g_pathingMutex = (CRITICAL_SECTION *)0x00AFE780;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2

UInt32 AddFormToDataHandler(DataHandler * dataHandler, TESForm * form)
{
	return ThisStdCall(0x0044DB30, dataHandler, form);
}

DataHandler ** g_dataHandler = (DataHandler **)0x00B33A98;

void AddFormToCreatedBaseObjectsList(void * objList, TESForm * form)
{
	ThisStdCall(0x004597F0, objList, form);
}

void ** g_createdBaseObjList = (void **)0x00B33B00;

UInt32 NiTPointerMap_Lookup(void * map, void * key, void ** data)
{
	return ThisStdCall(0x004E8670, map, key, data);
}

void * g_gameSettingsTable = (void *)0x00B35574;
char*** g_baseActorValueNames = (char***)0x00B12758;
char** g_extraActorValueNames = (char**)0x00B12908;

#include "obse/GameRTTI_1_2.inl"

const _Oblivion_DynamicCast Oblivion_DynamicCast = (_Oblivion_DynamicCast)0x0098AC46;

CRITICAL_SECTION * g_extraListMutex = (CRITICAL_SECTION *)0x00B33800;
CRITICAL_SECTION * g_pathingMutex = (CRITICAL_SECTION *)0x00B3BE80;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416

UInt32 AddFormToDataHandler(DataHandler * dataHandler, TESForm * form)
{
	return ThisStdCall(0x0044D950, dataHandler, form);
}

DataHandler ** g_dataHandler = (DataHandler **)0x00B33A98;

void AddFormToCreatedBaseObjectsList(void * objList, TESForm * form)
{
	ThisStdCall(0x00459800, objList, form);
}

void ** g_createdBaseObjList = (void **)0x00B33B00;

UInt32 NiTPointerMap_Lookup(void * map, void * key, void ** data)
{
	return ThisStdCall(0x0055E000, map, key, data);
}

void * g_gameSettingsTable = (void *)0x00B35574;
char*** g_baseActorValueNames = (char***)0x00B12758;
char** g_extraActorValueNames = (char**)0x00B12908;

#include "obse/GameRTTI_1_2_416.inl"

const _Oblivion_DynamicCast Oblivion_DynamicCast = (_Oblivion_DynamicCast)0x009832E6;

CRITICAL_SECTION * g_extraListMutex = (CRITICAL_SECTION *)0x00B33800;
CRITICAL_SECTION * g_pathingMutex = (CRITICAL_SECTION *)0x00B3BE80;

#else

#error unsupported version of oblivion

#endif

#pragma warning (push)
#pragma warning (disable : 4200)
struct RTTIType
{
	void	* typeInfo;
	UInt32	pad;
	char	name[0];
};

struct RTTILocator
{
	UInt32		sig, offset, cdOffset;
	RTTIType	* type;
};
#pragma warning (pop)

// use the RTTI information to return an object's class name
const char * GetObjectClassName(void * objBase)
{
	const char	* result = "<no rtti>";

	__try
	{
		void		** obj = (void **)objBase;
		RTTILocator	** vtbl = (RTTILocator **)obj[0];
		RTTILocator	* rtti = vtbl[-1];
		RTTIType	* type = rtti->type;

		// starts with ,?
		if((type->name[0] == '.') && (type->name[1] == '?'))
		{
			// is at most 100 chars long
			for(UInt32 i = 0; i < 100; i++)
			{
				if(type->name[i] == 0)
				{
					// remove the .?AV
					result = type->name + 4;
					break;
				}
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// return the default
	}

	return result;
}

const char * GetFullName(TESForm * baseForm)
{
	if(baseForm)
	{
		TESFullName		* fullName = (TESFullName *)Oblivion_DynamicCast(baseForm, 0, RTTI_TESForm, RTTI_TESFullName, 0);
		if(fullName)
		{
			if(fullName->name.m_data)
			{
				return fullName->name.m_data;
			}
		}
	}

	return "<no name>";
}

// ### this doesn't handle variables correctly yet
bool ExtractArgsEx(ParamInfo * paramInfo, void * scriptDataIn, UInt32 * scriptDataOffset, Script * scriptObj, ScriptEventList * eventList, ...)
{
	va_list	args;
	va_start(args, eventList);

	UInt8	* scriptData = ((UInt8 *)scriptDataIn) + *scriptDataOffset;

	UInt32	numArgs = *((UInt16 *)scriptData);
	scriptData += 2;

	for(UInt32 i = 0; i < numArgs; i++)
	{
		ParamInfo	* info = &paramInfo[i];

		switch(info->typeID)
		{
			case kParamType_String:
			{
				char	* out = va_arg(args, char *);

				UInt16	len = *((UInt16 *)scriptData);
				scriptData += 2;

				memcpy(out, scriptData, len);
				scriptData += len;

				out[len] = 0;
			}
			break;

			case kParamType_Integer:
			{
				UInt32	* out = va_arg(args, UInt32 *);

				UInt8	type = *scriptData++;
				switch(type)
				{
					case 0x72: // "r"
					{
						double data = 0;
						if (ExtractFloat(data, --scriptData, scriptObj, eventList))
							*out = data;
						else
							return false;
					}
					break;

					case 0x6E: // "n"
						*out = *((UInt32 *)scriptData);
						scriptData += sizeof(UInt32);
						break;
					
					case 0x66: // "f"
					case 0x73: // "s"
					{
						UInt16	varIdx = *((UInt16 *)scriptData);
						scriptData += 2;

						ScriptEventList::Var	* var = eventList->GetVariable(varIdx);
						if(!var) return false;

						*out = var->data;
					}
					break;

					case 0x47: // "G"
					{
						UInt16 varIndx = *((UInt16 *)scriptData);
						scriptData += 2;
						Script::RefVariable* refVar = scriptObj->GetVariable(varIndx);
						if (!refVar || !refVar) return false;
						TESGlobal* global = (TESGlobal*)Oblivion_DynamicCast(refVar->form, 0, RTTI_TESForm, RTTI_TESGlobal, 0);
						if (!global) return false;
						*out = global->data;
					}
					break;

					default:
						return false;
				}
			}
			break;

			case kParamType_Float:
			{
				float	* out = va_arg(args, float *);

				UInt8	type = *scriptData++;
				switch(type)
				{
					case 0x7A:
						*out = *((double *)scriptData);
						scriptData += sizeof(double);
						break;
					case 0x72: // "r"
					{
						double data = 0;
						if (ExtractFloat(data, --scriptData, scriptObj, eventList))
							*out = data;
						else
							return false;
					}
					break;

					case 0x66: // "f"
					case 0x73: // "s"
					{
						UInt16	varIdx = *((UInt16 *)scriptData);
						scriptData += 2;

						ScriptEventList::Var	* var = eventList->GetVariable(varIdx);
						if(!var) return false;

						*out = var->data;
					}
					break;

					case 0x47: // "G"
					{
						UInt16 varIndx = *((UInt16 *)scriptData);
						scriptData += 2;
						Script::RefVariable* refVar = scriptObj->GetVariable(varIndx);
						if (!refVar || !refVar) return false;
						TESGlobal* global = (TESGlobal*)Oblivion_DynamicCast(refVar->form, 0, RTTI_TESForm, RTTI_TESGlobal, 0);
						if (!global) return false;
						*out = global->data;
					}
					break;
					default:
						return false;
				}
			}
			break;

			case kParamType_InventoryObject:
			case kParamType_ObjectRef:
			case kParamType_Actor:
			case kParamType_SpellItem:
			case kParamType_Cell:
			case kParamType_MagicItem:
			case kParamType_Sound:
			case kParamType_Topic:
			case kParamType_Quest:
			case kParamType_Race:
			case kParamType_Class:
			case kParamType_Faction:
			case kParamType_Global:
			case kParamType_Furniture:
			case kParamType_TESObject:
			case kParamType_MapMarker:
			case kParamType_ActorBase:
			case kParamType_Container:
			case kParamType_WorldSpace:
			case kParamType_AIPackage:
			case kParamType_CombatStyle:
			case kParamType_MagicEffect:
			case kParamType_Birthsign:
			case kParamType_WeatherID:
			case kParamType_NPC:
			case kParamType_Owner:
			case kParamType_EffectShader:
			{
				TESForm	** out = va_arg(args, TESForm **);

				UInt8	type = *scriptData++;
				if(type != 0x72) return false;

				UInt16	varIdx = *((UInt16 *)scriptData);
				scriptData += 2;

				Script::RefVariable	* var = scriptObj->GetVariable(varIdx);
				if(!var) return false;

				var->Resolve(eventList);

				TESForm	* result = var->form;

				// ### temp hack to replicate the behavior of ExtractArgs (TESObjectREFR -> referring object)
				if(info->typeID == kParamType_InventoryObject)
				{
					// TESForm::Unk_2A returns if the form is a reference?
					TESObjectREFR	* refr = (TESObjectREFR *)Oblivion_DynamicCast(result, 0, RTTI_TESForm, RTTI_TESObjectREFR, 0);

					if(refr && refr->baseForm)
						result = refr->baseForm;
				}

				*out = result;
			}
			break;

			case kParamType_ActorValue:
			case kParamType_AnimationGroup:
			case kParamType_Sex:
			case kParamType_CrimeType:
			{
				UInt32	* out = va_arg(args, UInt32 *);

				*out = *((UInt16 *)scriptData);
				scriptData += 2;
			}
			break;

			default:
				HALT(unhandled type);
				break;
		}
	}

	va_end(args);

	return true;
}

// g_baseActorValueNames is only filled in after oblivion's global initializers run
const char* GetActorValueString(UInt32 actorValue)
{
	char* name = 0;
	if (actorValue <= kActorVal_Infamy)
		name = *g_baseActorValueNames[actorValue];
	else if (actorValue < kActorVal_OblivionMax)
		name = g_extraActorValueNames[actorValue - kActorVal_MagickaMultiplier];
	
	if (!name)
		name = "unknown";

	return name;
}

bool IsValidActorValue(UInt32 actorValue)
{
	return actorValue < kActorVal_OblivionMax;
}

UInt32 SafeModUInt32(UInt32 originalVal, float modBy) {
	double val = originalVal;
	val += modBy;
	if (val > ULONG_MAX)	{return ULONG_MAX;}
	else if (val < 0)		{return 0;}
	else					{return (UInt32) val;}					
}

float SafeChangeFloat(float originalVal, float changeVal, bool bMod, bool bNegativeAllowed) {
	double val = (bMod) ? originalVal : changeVal;
	if (bMod) { val+= changeVal; }
	if (val > FLT_MAX)			{return FLT_MAX;}
	else if (val >= 0)			{return (float) val;}
	else if (!bNegativeAllowed) { return 0;}
	else if (val > FLT_MIN)		{return (float) val;}
	else						{ return FLT_MIN; }
}

SettingInfo::EType SettingInfo::Type() const
{
	if (!name) return kSetting_Other;
	switch(name[0]) {
		case 'b': return kSetting_Bool;
		case 'c': return kSetting_c;
		case 'i': return kSetting_Integer;
		case 'u': return kSetting_Unsigned;
		case 'f': return kSetting_Float;
		case 's':
		case 'S':
			return kSetting_String;
		case 'r': return kSetting_r;
		case 'a': return kSetting_a;
		default:
			return kSetting_Other;
	}
}

bool GetGameSetting(char* settingName, SettingInfo** setting)
{
	return (NiTPointerMap_Lookup(g_gameSettingsTable, settingName, (void **)setting) != 0);
}

InterfaceManager * InterfaceManager::GetSingleton(void)
{
	return (InterfaceManager *)InterfaceManager_GetSingleton(false, true);
}

bool ExtractFloat(double& out, UInt8* &scriptData, Script* scriptObj, ScriptEventList* eventList)
{
	//extracts one float arg, used by ExtractFormattedString

	bool ret = false;
	if (*scriptData == 'r')		//reference to var in another script
	{
		UInt16 varIdx = *((UInt16*)++scriptData);
		scriptData += 2;
		Script::RefVariable* refVar = scriptObj->GetVariable(varIdx);
		if (refVar)
		{
			refVar->Resolve(eventList);
			TESForm* refObj = refVar->form;
			if (refObj)
			{
				ScriptEventList* refEventList = 0;
				if (refObj->typeID == kFormType_REFR)
				{
					TESObjectREFR* refr = (TESObjectREFR*)Oblivion_DynamicCast(refObj, 0, RTTI_TESForm, RTTI_TESObjectREFR, 0);
					if (refr)
						refEventList = refr->GetEventList();
				}
				else if (refObj->typeID == kFormType_Quest)
				{
					TESQuest* quest = (TESQuest*)Oblivion_DynamicCast(refObj, 0, RTTI_TESForm, RTTI_TESQuest, 0);
					if (quest)
						refEventList = quest->scriptEventList;
				}
				
				if (refEventList)
				{
					UInt16 varIdx = *((UInt16*)++scriptData);
					scriptData += 2;
					ScriptEventList::Var* var = refEventList->GetVariable(varIdx);
					if (var)
					{
						out = var->data;
						return true;
					}
				}
			}
		}
		//invalid reference
		return false;
	}		

	switch (*scriptData)
	{
	case 'G':		//global var
	{
		UInt16 varIdx = *((UInt16*)++scriptData);
		scriptData += 2;

		Script::RefVariable* globalRef = scriptObj->GetVariable(varIdx);
		if (globalRef)
		{
			TESGlobal* global = (TESGlobal*)Oblivion_DynamicCast(globalRef->form, 0, RTTI_TESForm, RTTI_TESGlobal, 0);
			if (global)
			{
				out = global->data;
				ret = true;
			}
		}
		break;
	}
	case 'z':		//literal double
	{
		out = *((double*)++scriptData);
		scriptData += sizeof(double);
		ret = true;
		break;
	}
	case 'f':
	case 's':		//local var
	{
		UInt16 varIdx = *((UInt16*)++scriptData);
		scriptData += 2;
		ScriptEventList::Var* var = eventList->GetVariable(varIdx);
		if (var)
		{
			out = var->data;
			ret = true;
		}
		break;
	}
	}
	return ret;
}

const char* DXDescriptions[221] =
{
	0,		"ESC",	"1",	"2",	"3",	"4",	"5",	"6",	"7",	"8",	"9",	"0",
	"-",	"=",	"BKSPC","TAB",	"Q",	"W",	"E",	"R",	"T",	"Y",	"U",	"I",
	"O",	"P",	"[",	"]",	"ENTER","LCTRL","A",	"S",	"D",	"F",	"G",	"H",
	"J",	"K",	"L",	";",	"'",	"~",	"LSHFT","\\",	"Z",	"X",	"C",	"V",
	"B",	"N",	"M",	",",	".",	"/",	"RSHFT","NUM*",	"LALT",	"SPACE","CAPS",	"F1",
	"F2",	"F3",	"F4",	"F5",	"F6",	"F7",	"F8",	"F9",	"F10",	"NUMLK","SCRLK","NUM7",
	"NUM8",	"NUM9",	"NUM-",	"NUM4",	"NUM5",	"NUM6",	"NUM+",	"NUM1",	"NUM2",	"NUM3",	"NUM0",	"NUM.",
	0,	0,	0,		
	"F11",	"F12",	
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	"F13",	"F14",	"F15",	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	"NUMENTER", "RCTRL",	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	
	"NUM/",	0, 0, "RALT",	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	"HOME", "UP",	"PGUP",	0,	"LEFT",	0,	"RIGHT",	0,	"END",	"DOWN",	"PGDN",	"INS",	"DEL",
	0, 0, 0, 0, 0, 0, 0, 
	"LWIN",	"RWIN"
};

const char* DXMouseDescriptions[9] = 
{
	"LMB",	"LMB",	"RMB",	"MMB",	"XMB1",	"XMB2",	"XMB3",	"XMB4",	"XMB5"
};

const char* GetDXDescription(UInt32 keycode)
{
	const char* ret = 0;
	if (keycode <= 220)
		ret =  DXDescriptions[keycode];
	else if (255 <= keycode && keycode <= 263)
		ret = DXMouseDescriptions[keycode - 255];
	
	if (!ret)
		ret = "Nothing";

	return ret;
}
bool ExtractFormattedString(char* buffer, void* arg1, Script* scriptObj, ScriptEventList* eventList, UInt32* opcodeOffsetPtr)
{
	//extracts args based on format string, prints formatted string to buffer (max 512 chars)
	static const int maxArgs = 20;
	double f[maxArgs];
	UInt8* scriptData = ((UInt8*)arg1) + *opcodeOffsetPtr;
	UInt32 numArgs = *((UInt16*)scriptData);
	scriptData += 2;
	UInt32 argIdx = 0;
	
	//extract format string
	UInt16 len = *((UInt16*)scriptData);
	char* szFmt = new char[len+1];
	scriptData += 2;
	memcpy(szFmt, scriptData, len);
	szFmt[len] = '\0';

	scriptData += len;
	std::string fmtString(szFmt);
	delete szFmt;

	UInt32 strIdx = 0;

	//extract args
	while ((strIdx = fmtString.find('%', strIdx)) != -1 && numArgs > 0)
	{
		char argType = fmtString.at(strIdx+1);
		switch (argType)
		{
		case '%':										//literal %
			strIdx += 2;	//was ++
			break;
		case 'r':										//newline
		case 'R':
			fmtString.erase(strIdx, 2);
			fmtString.insert(strIdx, "\n");
			break;
		case 'n':										// name of obj/ref
		case 'N':
			{
				numArgs--;
				fmtString.erase(strIdx, 2);
				scriptData++;
				UInt16 varIdx = *((UInt16*)scriptData);
				
				ScriptEventList::Var* var = eventList->GetVariable(varIdx);
				if (!var)
					fmtString.insert(strIdx, "NULL");
				else
				{
					TESForm* form = LookupFormByID(*((UInt64 *)&var->data));
					if (!form)
						fmtString.insert(strIdx, "NULL");
					else
					{			
						TESObjectREFR* refr = (TESObjectREFR*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESObjectREFR, 0);
						if (refr)
							form = refr->baseForm;

						TESFullName* name = (TESFullName*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESFullName, 0);
						if (name && name->name.m_data && name->name.m_dataLen > 0)
							fmtString.insert(strIdx, name->name.m_data);
						else
							fmtString.insert(strIdx, "<no name>");
					}
				}
			}
			scriptData += 2;
			break;
		case 'i':											//formID
		case 'I':
			{
				numArgs--;
				fmtString.erase(strIdx, 2);
				scriptData++;
				UInt16 varIdx = *((UInt16*)scriptData);
				
				ScriptEventList::Var* var = eventList->GetVariable(varIdx);
				if (!var)
					fmtString.insert(strIdx, "0");
				else
				{
					TESForm* form = LookupFormByID(*((UInt64 *)&var->data)); 
					if (!form)
						fmtString.insert(strIdx, "0");
					else
					{			
						char formID[9];
						sprintf_s(formID, 9, "%08X", form->refID);
						fmtString.insert(strIdx, formID);
					}
				}
				scriptData += 2;
				break;
			}
		case 'c':											//named component of another object
		case 'C':											//2 args - object and index
			{
				numArgs -= 2;
				fmtString.erase(strIdx, 2);
				scriptData++;
				UInt16 varIdx = *((UInt16*)scriptData);
				scriptData += 2;

				ScriptEventList::Var* var = eventList->GetVariable(varIdx);
				if (!var)
					fmtString.insert(strIdx, "NULL");
				else
				{
					TESForm* form = LookupFormByID(*((UInt64*)&var->data));
					if (!form)
						fmtString.insert(strIdx, "NULL");
					else
					{
						double objIdx = 0;
						if (!ExtractFloat(objIdx, scriptData, scriptObj, eventList))
							fmtString.insert(strIdx, "NULL");
						else
						{
							switch (form->typeID)
							{
							case kFormType_Spell:
							case kFormType_Enchantment:
							case kFormType_Ingredient:
							case kFormType_AlchemyItem:
								{
									MagicItem* magItm = (MagicItem*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_MagicItem, 0);
									if (!magItm)
										fmtString.insert(strIdx, "NULL");
									else
										fmtString.insert(strIdx, magItm->list.GetNthEIName(objIdx));

									break;
								}
							case kFormType_SigilStone:
								{
									TESSigilStone* stone = (TESSigilStone*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESSigilStone, 0);
									if (!stone)
										fmtString.insert(strIdx, "NULL");
									else
										fmtString.insert(strIdx, stone->effectList.GetNthEIName(objIdx));

									break;
								}
							case kFormType_Faction:
								{
									TESFaction* fact = (TESFaction*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESFaction, 0);
									if (!fact)
										fmtString.insert(strIdx, "NULL");
									else
									{
										fmtString.insert(strIdx, fact->GetNthRankMaleName(objIdx));
									}
									break;
								}
							default:
								fmtString.insert(strIdx, "unknown");
							}
						}
					}
				}
				break;
			}
		case 'k':
		case 'K':											//DX code
			{
				numArgs--;
				double keycode = 0;
				fmtString.erase(strIdx, 2);
				if (ExtractFloat(keycode, scriptData, scriptObj, eventList))
				{
					const char* desc = GetDXDescription(keycode);
					fmtString.insert(strIdx, desc);
				}
				else
					fmtString.insert(strIdx, "0");
			}
			break;
		case 'v':
		case 'V':
			{
				numArgs--;
				double actorVal = kActorVal_OblivionMax;
				fmtString.erase(strIdx, 2);
				if (ExtractFloat(actorVal, scriptData, scriptObj, eventList))
				{
					std::string valStr(GetActorValueString(actorVal));
					if (valStr.length())
					{
						for (UInt32 idx = 1; idx < valStr.length(); idx++)
							if (isupper(valStr[idx]))
							{								//insert spaces to make names more presentable
								valStr.insert(idx, " ");
								idx += 2;
							}
					}
					fmtString.insert(strIdx, valStr);
				}
				else
					fmtString.insert(strIdx, "unknown");
			}
			break;
		default:											//float
			{
				numArgs--;
				double data = 0;
				ExtractFloat(data, scriptData, scriptObj, eventList);
				f[argIdx++] = data;
				strIdx++;
			}
		}
	}
	
	while (argIdx < maxArgs)
		f[argIdx++] = 0;

	if (sprintf_s(buffer, 511, fmtString.c_str(), f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8], f[9], f[10], f[11], f[12], f[13], f[14], f[15], f[16], f[17], f[18], f[19]) > 0)
		return true;
	else
		return false;
}
