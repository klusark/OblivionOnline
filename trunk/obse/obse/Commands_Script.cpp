#include "Commands_Script.h"
#include "GameAPI.h"
#include "GameForms.h"
#include "GameObjects.h"
#include "ParamInfos.h"

#if OBLIVION

enum EScriptMode {
	eScript_HasScript,
	eScript_Get,
	eScript_Remove,
};


static bool GetScript_Execute(COMMAND_ARGS, EScriptMode eMode)
{
	*result = 0;
	TESForm* form = 0;

	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &form);
	form = form->TryGetREFRParent();
	if (!form) {
		if (!thisObj) return true;
		form = thisObj->baseForm;
	}

	TESScriptableForm* scriptForm = (TESScriptableForm*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESScriptableForm, 0);
	Script* script = (scriptForm) ? scriptForm->script : NULL;

	if (eMode == eScript_HasScript) {
		*result = (script != NULL) ? 1 : 0;
	} else {
		if (script) {
			UInt32* refResult = (UInt32*)result;
			*refResult = script->refID;		
		}
		if (eMode == eScript_Remove) {
			scriptForm->script = NULL;
		}
	}

	return true;
}

static bool Cmd_IsScripted_Execute(COMMAND_ARGS)
{
	return GetScript_Execute(PASS_COMMAND_ARGS, eScript_HasScript);
}

static bool Cmd_GetScript_Execute(COMMAND_ARGS)
{
	return GetScript_Execute(PASS_COMMAND_ARGS, eScript_Get);
}

static bool Cmd_RemoveScript_Execute(COMMAND_ARGS)
{
	return GetScript_Execute(PASS_COMMAND_ARGS, eScript_Remove);
}


static bool Cmd_SetScript_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32* refResult = (UInt32*)result;

	TESForm* form = NULL;
	TESForm* scriptArg = NULL;

	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &scriptArg, &form);
	form = form->TryGetREFRParent();
	if (!form) {
		if (!thisObj) return true;
		form = thisObj->baseForm;
	}

	TESScriptableForm* scriptForm = (TESScriptableForm*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESScriptableForm, 0);
	if (!scriptForm) return true;

	Script* script = (Script*)Oblivion_DynamicCast(scriptArg, 0, RTTI_TESForm, RTTI_Script, 0);
	if (!script) return true;

	// we can't get a magic script here
	if (script->IsMagicScript()) return true;

	if (script->IsQuestScript() && form->typeID == kFormType_Quest) {
		*refResult = scriptForm->script->refID;
		scriptForm->script = script;
	} else if (script->IsObjectScript()) {
		if (scriptForm->script) {
			*refResult = scriptForm->script->refID;
		}
		scriptForm->script = script;
	}
	return true;
}

static bool Cmd_IsFormValid_Execute(COMMAND_ARGS)
{
	TESForm* form = NULL;
	*result = 0;
	if (ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &form) && form)
		*result = 1;
	return true;
}

static bool Cmd_IsReference_Execute(COMMAND_ARGS)
{
	TESObjectREFR* refr = NULL;
	*result = 0;
	if (ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &refr))
		*result = 1;

	return true;
}

static enum {
	eScriptVar_Get = 1,
	eScriptVar_GetRef,
	eScriptVar_Has,
};

static bool GetVariable_Execute(COMMAND_ARGS, UInt32 whichAction)
{
	char varName[256] = { 0 };
	TESQuest* quest = NULL;
	Script* targetScript = NULL;
	ScriptEventList* targetEventList = NULL;
	*result = 0;

	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &varName, &quest))
		return true;
	if (quest)
	{
		TESScriptableForm* scriptable = (TESScriptableForm*)Oblivion_DynamicCast(quest, 0, RTTI_TESQuest, RTTI_TESScriptableForm, 0);
		targetScript = scriptable->script;
		targetEventList = quest->scriptEventList;
	}
	else if (thisObj)
	{
		TESScriptableForm* scriptable = (TESScriptableForm*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESScriptableForm, 0);
		if (scriptable)
		{
			targetScript = scriptable->script;
			targetEventList = thisObj->GetEventList();
		}
	}

	if (targetScript && targetEventList)
	{
		Script::VariableInfo* varInfo = targetScript->GetVariableByName(varName);
		if (whichAction == eScriptVar_Has)
			return varInfo ? true : false;
		else if (varInfo)
		{
			ScriptEventList::Var* var = targetEventList->GetVariable(varInfo->idx);
			if (var)
			{
				if (whichAction == eScriptVar_Get)
					*result = var->data;
				else if (whichAction == eScriptVar_GetRef)
				{
					UInt32* refResult = (UInt32*)result;
					*refResult = (*(UInt64*)&var->data);
				}
				return true;
			}
		}
	}

	return false;
}

static bool Cmd_HasVariable_Execute(COMMAND_ARGS)
{
	*result = 0;
	if (GetVariable_Execute(PASS_COMMAND_ARGS, eScriptVar_Has))
		*result = 1;

	return true;
}

static bool Cmd_GetVariable_Execute(COMMAND_ARGS)
{
	GetVariable_Execute(PASS_COMMAND_ARGS, eScriptVar_Get);
	return true;
}

static bool Cmd_GetRefVariable_Execute(COMMAND_ARGS)
{
	GetVariable_Execute(PASS_COMMAND_ARGS, eScriptVar_GetRef);
	return true;
}

static bool Cmd_CompareScripts_Execute(COMMAND_ARGS)
{
	Script* script1 = NULL;
	Script* script2 = NULL;
	*result = 0;

	if (!ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &script1, &script2))
		return true;
	script1 = (Script*)Oblivion_DynamicCast(script1, 0, RTTI_TESForm, RTTI_Script, 0);
	script2 = (Script*)Oblivion_DynamicCast(script2, 0, RTTI_TESForm, RTTI_Script, 0);

	if (script1 && script2 && script1->info.dataLength == script2->info.dataLength)
	{
		if (script1 == script2)
			*result = 1;
		else if (!memcmp(script1->data, script2->data, script1->info.dataLength))
			*result = 1;
	}

	return true;
}

	
#endif

CommandInfo kCommandInfo_IsScripted =
{
	"IsScripted",
	"",
	0,
	"returns 1 if the object or reference has a script attached to it",
	0,
	1,
	kParams_OneOptionalInventoryObject,
	HANDLER(Cmd_IsScripted_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetScript =
{
	"GetScript",
	"",
	0,
	"returns the script of the referenced or passed object",
	0,
	1,
	kParams_OneOptionalInventoryObject,
	HANDLER(Cmd_GetScript_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_RemoveScript =
{
	"RemoveScript",
	"",
	0,
	"removes the script of the referenced or passed object",
	0,
	1,
	kParams_OneOptionalInventoryObject,
	HANDLER(Cmd_RemoveScript_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


ParamInfo kParamInfo_SetScript[2] = 
{
	{	"script", kParamType_MagicItem, 0 },
	{	"object", kParamType_InventoryObject, 1},
};

CommandInfo kCommandInfo_SetScript =
{
	"SetScript",
	"",
	0,
	"returns the script of the referenced or passed object",
	0,
	2,
	kParamInfo_SetScript,
	HANDLER(Cmd_SetScript_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsFormValid =
{
	"IsFormValid",
	"",
	0,
	"returns true if the ref variable contains a valid form",
	0,
	1,
	kParams_OneInventoryObject,
	HANDLER(Cmd_IsFormValid_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_OneObjectRef[1] =
{
	{	"ref",	kParamType_ObjectRef,	0	},
};

CommandInfo kCommandInfo_IsReference =
{
	"IsReference",
	"",
	0,
	"returns true if the ref variable contains a REFR",
	0,
	1,
	kParams_OneObjectRef,
	HANDLER(Cmd_IsReference_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_GetVariable[2] =
{
	{	"variable name",	kParamType_String,	0	},
	{	"quest",			kParamType_Quest,	1	},
};

CommandInfo kCommandInfo_GetVariable =
{
	"GetVariable",
	"GetVar",
	0,
	"looks up the value of a variable by name",
	0,
	2,
	kParams_GetVariable,
	HANDLER(Cmd_GetVariable_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_HasVariable =
{
	"HasVariable",
	"HasVar",
	0,
	"returns true if the script has a variable with the specified name",
	0,
	2,
	kParams_GetVariable,
	HANDLER(Cmd_HasVariable_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetRefVariable =
{
	"GetRefVariable",
	"GetRefVar",
	0,
	"looks up the value of a ref variable by name",
	0,
	2,
	kParams_GetVariable,
	HANDLER(Cmd_GetRefVariable_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_CompareScripts[2] =
{
	{	"script",	kParamType_InventoryObject,	0	},
	{	"script",	kParamType_InventoryObject,	0	},
};

CommandInfo kCommandInfo_CompareScripts =
{
	"CompareScripts",
	"",
	0,
	"returns true if the compiled scripts are identical",
	0,
	2,
	kParams_CompareScripts,
	HANDLER(Cmd_CompareScripts_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};
