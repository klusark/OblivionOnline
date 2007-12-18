#include "obse/GameAPI.h"
#include "obse/GameObjects.h"
#include "obse/CommandTable.h"
#include <cstdarg>

#include <float.h>

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
