#include "ParamInfos.h"
#include "obse\Commands_LeveledList.h"
#include "GameForms.h"
#include "GameObjects.h"

#if OBLIVION

static bool Cmd_AddToLeveledList_Execute(COMMAND_ARGS)
{
	TESForm*	list;
	TESForm*	form;
	UInt32		level = 1;	
	UInt32		count = 1;

	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &list, &form, &level, &count);
	TESLeveledList*	levList = (TESLeveledList*)Oblivion_DynamicCast(list, 0, RTTI_TESForm, RTTI_TESLeveledList, 0);
	if (!levList || !form)
	{
		return true;
	}

	levList->AddItem(form, level, count);

	return true;
}

static bool Cmd_RemoveFromLeveledList_Execute(COMMAND_ARGS)
{	
	TESForm*	list;
	TESForm*	form;
	*result = 0;

	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &list, &form);
	TESLeveledList*	levList = (TESLeveledList*)Oblivion_DynamicCast(list, 0, RTTI_TESForm, RTTI_TESLeveledList, 0);
	if (!levList || !form)
	{
		return true;
	}

	*result = (double)(levList->RemoveItem(form));
	return true;
}

static bool CalcLevItem_Execute(COMMAND_ARGS, bool noRecurse)
{
	TESForm* list;
	SInt32 level = -1;
	UInt32 useChanceNone = 1;
	SInt32 levelDiff = -1;
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &list, &level, &useChanceNone, &levelDiff);
	if (level == -1)	//param omitted
		return true;

	TESLeveledList* levList = (TESLeveledList*)Oblivion_DynamicCast(list, 0, RTTI_TESForm, RTTI_TESLeveledList, 0);
	if (!list || level == -1)
		return true;

	if (levelDiff == -1)	//param omitted so use default
	{
		SettingInfo * info = NULL;
		if (GetGameSetting("iLevItemLevelDifferenceMax", &info))
			levelDiff = info->i;
	}

	TESForm* item = levList->CalcElement(level, (useChanceNone ? true : false), levelDiff, noRecurse);
	if (item)
		*refResult = item->refID;

	return true;
}

static bool Cmd_CalcLeveledItem_Execute(COMMAND_ARGS)
{
	CalcLevItem_Execute(PASS_COMMAND_ARGS, false);
	return true;
}

static bool Cmd_CalcLeveledItemNR_Execute(COMMAND_ARGS)
{
	CalcLevItem_Execute(PASS_COMMAND_ARGS, true);
	return true;
}

static bool Cmd_GetLevItemByLevel_Execute(COMMAND_ARGS)
{
	UInt32 lev = 0;
	TESForm* form = NULL;
	TESLeveledList* levList = NULL;
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &lev, &form))
		levList = (TESLeveledList*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESLeveledList, 0);

	if (levList)
	{
		form = levList->GetElementByLevel(lev);
		if (form)
			*refResult = form->refID;
	}

	return true;
}

static bool Cmd_RemoveLevItemByLevel_Execute(COMMAND_ARGS)
{
	UInt32 lev = 0;
	TESForm* form = NULL;
	TESLeveledList* levList = NULL;
	*result = 0;

	if (ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &lev, &form))
		levList = (TESLeveledList*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESLeveledList, 0);

	if (levList)
		*result = levList->RemoveByLevel(lev);

	return true;
}

static TESLeveledList* ExtractLeveledList(COMMAND_ARGS)
{
	TESForm* form = NULL;
	TESLeveledList* levList = NULL;
	if (ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &form))
		levList = (TESLeveledList*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESLeveledList, 0);

	return levList;
}

static bool Cmd_GetChanceNone_Execute(COMMAND_ARGS)
{
	*result = -1;

	TESLeveledList* levList = ExtractLeveledList(PASS_COMMAND_ARGS);
	if (levList)
		*result = levList->chanceNone;

	return true;
}

static bool Cmd_GetCalcAllLevels_Execute(COMMAND_ARGS)
{
	*result = 0;

	TESLeveledList* levList = ExtractLeveledList(PASS_COMMAND_ARGS);
	if (levList && (levList->flags & levList->kFlags_CalcAllLevels))
		*result = 1;

	return true;
}

static bool Cmd_GetCalcEachInCount_Execute(COMMAND_ARGS)
{
	*result = 0;

	TESLeveledList* levList = ExtractLeveledList(PASS_COMMAND_ARGS);
	if (levList && (levList->flags & levList->kFlags_CalcEachInCount))
		*result = 1;

	return true;
}

static bool Cmd_GetNumLevItems_Execute(COMMAND_ARGS)
{
	*result = 0;

	TESLeveledList* levList = ExtractLeveledList(PASS_COMMAND_ARGS);
	if (levList)
		*result = LeveledListVisitor(&levList->list).Count();

	return true;
}

static bool Cmd_GetNthLevItem_Execute(COMMAND_ARGS)
{
	UInt32 index = 0;
	TESForm* form = NULL;
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &index, &form))
		return true;

	TESLeveledList* levList = (TESLeveledList*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESLeveledList, 0);
	if (levList)
	{
		TESLeveledList::ListData* data = LeveledListVisitor(&levList->list).GetNthInfo(index);
		if (data && data->form)
			*refResult = data->form->refID;
	}
	return true;
}

static bool Cmd_GetNthLevItemCount_Execute(COMMAND_ARGS)
{
	UInt32 index = 0;
	TESForm* form = NULL;
	*result = 0;

	if (!ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &index, &form))
		return true;

	TESLeveledList* levList = (TESLeveledList*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESLeveledList, 0);
	if (levList)
	{
		TESLeveledList::ListData* data = LeveledListVisitor(&levList->list).GetNthInfo(index);
		if (data)
			*result = data->count;
	}
	return true;
}

static bool Cmd_GetNthLevItemLevel_Execute(COMMAND_ARGS)
{
	UInt32 index = 0;
	TESForm* form = NULL;
	*result = 0;

	if (!ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &index, &form))
		return true;

	TESLeveledList* levList = (TESLeveledList*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESLeveledList, 0);
	if (levList)
	{
		TESLeveledList::ListData* data = LeveledListVisitor(&levList->list).GetNthInfo(index);
		if (data)
			*result = data->level;
	}
	return true;
}

static bool Cmd_ClearLeveledList_Execute(COMMAND_ARGS)
{
	TESForm* form = NULL;
	if (!ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &form))
		return true;

	TESLeveledList* levList = (TESLeveledList*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESLeveledList, 0);
	if (levList && levList->list.data)
		LeveledListVisitor(&levList->list).RemoveAll();

	return true;
}

#endif

static ParamInfo kParams_TwoInventoryObjects_TwoOptionalInts[4] =
{
	{	"leveled list", kParamType_InventoryObject, 0 },
	{	"item to add",	kParamType_InventoryObject, 0 },
	{	"level",		kParamType_Integer,			1 },
	{	"count",		kParamType_Integer,			1},
};

CommandInfo kCommandInfo_AddToLeveledList =
{
	"AddToLeveledList",
	"AddLevList",
	0,
	"adds an object to a leveled list",
	0,
	4,
	kParams_TwoInventoryObjects_TwoOptionalInts,
	HANDLER(Cmd_AddToLeveledList_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_TwoInventoryObjects[2] =
{
	{	"leveled list",		kParamType_InventoryObject, 0 },
	{	"item to remove",	kParamType_InventoryObject, 0 },
};

CommandInfo kCommandInfo_RemoveFromLeveledList =
{
	"RemoveFromLeveledList",
	"RemLevList",
	0,
	"removes all occurrences of an object from a leveled list",
	0,
	2,
	kParams_TwoInventoryObjects,
	HANDLER(Cmd_RemoveFromLeveledList_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_CalcLeveledItem[4] = 
{
	{	"leveled list",			kParamType_InventoryObject, 0	},
	{	"level",				kParamType_Integer,			0	},
	{	"chance none flag",		kParamType_Integer,			1	},
	{	"min level difference",	kParamType_Integer,			1	},
};

CommandInfo kCommandInfo_CalcLeveledItem = 
{
	"CalcLeveledItem",
	"CalcLevItem",
	0,
	"chooses a random item for a given level from the list",
	0,
	4,
	kParams_CalcLeveledItem,
	HANDLER(Cmd_CalcLeveledItem_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_CalcLeveledItemNR = 
{
	"CalcLeveledItemNR",
	"CalcLevItemNR",
	0,
	"returns the index of an randomly chosen element from a leveled list",
	0,
	4,
	kParams_CalcLeveledItem,
	HANDLER(Cmd_CalcLeveledItemNR_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetChanceNone =
{
	"GetChanceNone",
	"",
	0,
	"returns the chance a leveled list will return nothing",
	0,
	1,
	kParams_OneInventoryObject,
	HANDLER(Cmd_GetChanceNone_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCalcAllLevels =
{
	"GetCalcAllLevels",
	"",
	0,
	"returns 1 if the Calc All Levels flag is set on the leveled list",
	0,
	1,
	kParams_OneInventoryObject,
	HANDLER(Cmd_GetCalcAllLevels_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCalcEachInCount =
{
	"GetCalcEachInCount",
	"",
	0,
	"returns 1 if the Calc Each In Count flag is set for the leveled list",
	0,
	1,
	kParams_OneInventoryObject,
	HANDLER(Cmd_GetCalcEachInCount_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNumLevItems =
{
	"GetNumLevItems",
	"",
	0,
	"returns the number of elements in a leveled list",
	0,
	1,
	kParams_OneInventoryObject,
	HANDLER(Cmd_GetNumLevItems_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_GetNthLevItem[2] =
{
	{	"index",		kParamType_Integer,			0	},
	{	"leveled list",	kParamType_InventoryObject,	0	},
};

CommandInfo kCommandInfo_GetNthLevItem =
{
	"GetNthLevItem",
	"",
	0,
	"returns the nth element of a leveled list",
	0,
	2,
	kParams_GetNthLevItem,
	HANDLER(Cmd_GetNthLevItem_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

DEFINE_COMMAND(ClearLeveledList,
			   clears all data from a leveled list,
			   0,
			   1,
			   kParams_OneInventoryObject);

DEFINE_COMMAND(GetNthLevItemCount,
			   returns the count of the nth element of a leveled list,
			   0,
			   2,
			   kParams_GetNthLevItem);
DEFINE_COMMAND(GetNthLevItemLevel,
			   returns the level of the nth element of a leveled list,
			   0,
			   2,
			   kParams_GetNthLevItem);

DEFINE_COMMAND(GetLevItemByLevel,
			   returns the first element of the specified level,
			   0,
			   2,
			   kParams_GetNthLevItem);

DEFINE_COMMAND(RemoveLevItemByLevel,
			   removes all elements of the specified level,
			   0,
			   2,
			   kParams_GetNthLevItem);

