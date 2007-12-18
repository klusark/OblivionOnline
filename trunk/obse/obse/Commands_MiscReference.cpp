#include "Commands_MiscReference.h"
#include "GameObjects.h"
#include "GameExtraData.h"
#include "GameAPI.h"
#include "GameForms.h"
#include "GameProcess.h"
#include "ParamInfos.h"
#include <set>
#include <map>

#if OBLIVION

static bool Cmd_GetTravelHorse_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32* refResult = (UInt32*)result;
	if (!thisObj) return true;

	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_TravelHorse);
	if (xData) {
		ExtraTravelHorse* xHorse = (ExtraTravelHorse*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraTravelHorse, 0);
		if (xHorse) {
			*refResult = xHorse->horseRef->refID;
		}
	}
	return true;
}

static bool Cmd_SetTravelHorse_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!thisObj) return true;
	TESObjectREFR* objectRef = NULL;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &objectRef))
		return true;
	if (!thisObj) return true;

	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_TravelHorse);
	if (xData) {
		ExtraTravelHorse* xHorse = (ExtraTravelHorse*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraTravelHorse, 0);
		if (xHorse) {
			*refResult = xHorse->horseRef->refID;
			xHorse->horseRef = objectRef;			
		}
	}
	else		//add
	{
		ExtraTravelHorse* xHorse = ExtraTravelHorse::Create();
		xHorse->horseRef = objectRef;
		if (!thisObj->baseExtraList.Add(xHorse))
		{
			FormHeap_Free(xHorse);
			xHorse = NULL;
		}
	}

	return true;
}

static bool Cmd_GetOpenKey_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!thisObj)	return true;
	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_Lock);
	if (!xData)
		return true;

	ExtraLock* xLock = (ExtraLock*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraLock, 0);
	if (xLock && xLock->data && xLock->data->key)
		*refResult = xLock->data->key->refID;

	return true;
}

static bool Cmd_SetOpenKey_Execute(COMMAND_ARGS)
{
	TESForm* form;
	*result = 0;
	
	if (!thisObj)	
		return true;

	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &form);
	if (!form)	
		return true;

	TESKey* key = (TESKey*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESKey, 0);
	if (!key)	
		return true;

	ExtraLock* xLock = NULL;
	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_Lock);
	if (!xData)			//create and add to baseExtraList
	{
		xLock = ExtraLock::Create();
		if (!thisObj->baseExtraList.Add(xLock))
		{
			FormHeap_Free(xLock->data);
			FormHeap_Free(xLock);
			return true;
		}
	}
	else
		xLock = (ExtraLock*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraLock, 0);

	if (xLock)
	{
		xLock->data->key = key;
		*result = 1;
	}

	return true;
}

static TESForm* GetOwner(BaseExtraList* xDataList)
{
	BSExtraData* xData = xDataList->GetByType(kExtraData_Ownership);
	ExtraOwnership* xOwner = NULL;
	TESForm* owner = NULL;

	if (xData)
	{
		xOwner = (ExtraOwnership*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraOwnership, 0);
		if (xOwner)
			owner = xOwner->owner;
	}

	return owner;
}

static UInt32 GetOwningFactionRequiredRank(BaseExtraList* xDataList)
{
	BSExtraData* xData = xDataList->GetByType(kExtraData_Rank);
	if (xData)
	{
		ExtraRank* xRank = (ExtraRank*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraRank, 0);
		if (xRank)
			return xRank->rank;
	}

	return 0;
}

static bool Cmd_GetParentCellOwner_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!thisObj)
		return true;

	TESForm* owner = GetOwner(&(thisObj->parentCell->extraData));
	if (owner)
		*refResult = owner->refID;

	return true;
}

static bool Cmd_GetOwner_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!thisObj)
		return true;

	TESForm* owner = GetOwner(&(thisObj->baseExtraList));
	if (owner)
		*refResult = owner->refID;

	return true;
}

static bool Cmd_GetOwningFactionRequiredRank_Execute(COMMAND_ARGS)
{
	*result = 0;

	if (!thisObj)
		return true;

	*result = GetOwningFactionRequiredRank(&(thisObj->baseExtraList));

	return true;
}

static bool Cmd_GetParentCellOwningFactionRequiredRank_Execute(COMMAND_ARGS)
{
	*result = 0;

	if (!thisObj)
		return true;

	*result = GetOwningFactionRequiredRank(&(thisObj->parentCell->extraData));

	return true;
}

static SInt8 IsOffLimits(BaseExtraList* xDataList, TESNPC* actor)
{
	SInt8 offLimits = -1;					//return -1 if ownership is ambiguous

	BSExtraData* xData;
	TESForm* owner = GetOwner(xDataList);
	if (owner)
	{
		if (owner->typeID == kFormType_NPC)			
		{
			if (owner->refID == actor->refID)			//owned by this actor
				offLimits = 0;
			else
			{
				xData = xDataList->GetByType(kExtraData_Global);
				if (xData)
				{
					ExtraGlobal* global = (ExtraGlobal*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraGlobal, 0);
					if (global->globalVar->data)
						offLimits = 0;
					else
						offLimits = 1;
				}
				else
					offLimits = 1;
			}
		}
		else if (owner->typeID == kFormType_Faction)
		{
			xData = xDataList->GetByType(kExtraData_Rank);
			SInt8 reqRank = 0;
			if (xData)					// ExtraRank only present if required rank > 0
			{
				ExtraRank* rank = (ExtraRank*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraRank, 0);
				reqRank = rank->rank;
			}
			if (actor->actorBaseData.GetFactionRank((TESFaction*)owner) >= reqRank)
				offLimits = 0;
			else
				offLimits = 1;
		}
	}
	return offLimits;
}

static bool Cmd_IsOffLimits_Execute(COMMAND_ARGS)
{
	TESNPC* actor = NULL;
	*result = 0;

	if (!thisObj)
		return true;
	else if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &actor))
		return true;

	SInt8 offLimits = IsOffLimits(&thisObj->baseExtraList, actor);
	if (offLimits != -1)
		*result = offLimits;
	else
	{
		offLimits = IsOffLimits(&thisObj->parentCell->extraData, actor);
		if (offLimits == 1)
			*result = 1;
	}

	return true;
}

static bool Cmd_IsLoadDoor_Execute(COMMAND_ARGS)
{
	*result = 0;

	if (!thisObj)
		return true;

	if (thisObj->baseExtraList.GetByType(kExtraData_Teleport) ||
		thisObj->baseExtraList.GetByType(kExtraData_RandomTeleportMarker))
		*result = 1;

	return true;
}

static bool Cmd_GetLinkedDoor_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!thisObj)
		return true;

	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_Teleport);
	if (xData)
	{
		ExtraTeleport* xTele = (ExtraTeleport*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraTeleport, 0);
		if (xTele)
			*refResult = xTele->data->linkedDoor->refID;
	}

	return true;
}

static bool Cmd_GetTeleportCell_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!thisObj)
		return true;

	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_Teleport);
	if (xData)
	{
		ExtraTeleport* xTele = (ExtraTeleport*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraTeleport, 0);
		if (xTele)
			*refResult = xTele->data->linkedDoor->parentCell->refID;
	}

	return true;
}

struct CellScanInfo
{
	const	TESObjectCELL::ObjectListEntry *	prev;	//last ref returned to script
	const	TESObjectCELL * curCell;					//cell currently being scanned
	const	TESObjectCELL * pcCell;						//player's current cell
	const	TESWorldSpace * world;
	SInt8	curX;										//offsets of curCell from player's cell
	SInt8	curY;
	UInt8	formType;									//form type to scan for
	UInt8	cellDepth;									//depth of adjacent cells to scan

	CellScanInfo()
	{	}

	CellScanInfo(UInt8 _cellDepth, UInt8 _formType) : cellDepth(_cellDepth), formType(_formType), prev(NULL)
	{
		pcCell = (*g_thePlayer)->parentCell;
		world = pcCell->worldSpace;

		if (world && cellDepth)		//exterior, cell depth > 0
		{
			curX = pcCell->coords->x - cellDepth;
			curY = pcCell->coords->y - cellDepth;
			UInt32 key = (curX << 16) + ((curY << 16) >> 16);
			curCell = world->cellMap->Lookup(key);
		}
		else
		{
			cellDepth = 0;
			curCell = pcCell;
			curX = pcCell->coords->x;
			curY = pcCell->coords->y;
		}
	}

	bool NextCell()		//advance to next cell in area
	{
		if (!world || !cellDepth)
		{
			curCell = NULL;
			return false;
		}

		do
		{
			if (curX - pcCell->coords->x == cellDepth)
			{
				if (curY - pcCell->coords->y == cellDepth)
				{
					curCell = NULL;
					return false;
				}
				else
				{
					curY++;
					curX -= cellDepth * 2;
					UInt32 key = (curX << 16) + ((curY << 16) >> 16);
					curCell = world->cellMap->Lookup(key);
				}
			}
			else
			{
				curX++;
				UInt32 key = (curX << 16) + ((curY << 16) >> 16);
				curCell = world->cellMap->Lookup(key);
			}
		}while (!curCell);
		
		return true;
	}

	void FirstCell()	//init curCell to point to first valid cell
	{
		if (!curCell)
			NextCell();
	}
};

class RefMatcherAnyForm
{
public:
	RefMatcherAnyForm() { }

	bool Accept(const TESObjectREFR* refr)
	{
		return true;
	}
};

class RefMatcherFormType
{
	UInt32 m_formType;
public:
	RefMatcherFormType(UInt32 formType) : m_formType(formType) { }

	bool Accept(const TESObjectREFR* refr)
	{
		if (refr->baseForm->typeID == m_formType && refr->baseForm->refID != 7)	//exclude player for kFormType_NPC
			return true;
		else
			return false;
	}
};

class RefMatcherActor
{
public:
	RefMatcherActor() { }

	bool Accept(const TESObjectREFR* refr)
	{
		if (refr->baseForm->typeID == kFormType_Creature)
			return true;
		else if (refr->baseForm->typeID == kFormType_NPC && refr->baseForm->refID != 7) //exclude the player
			return true;
		else
			return false;
	}
};

class RefMatcherItem
{
public:
	RefMatcherItem() { }

	bool Accept(const TESObjectREFR* refr)
	{
		switch (refr->baseForm->typeID)
		{
			case kFormType_Apparatus:
			case kFormType_Armor:
			case kFormType_Book:
			case kFormType_Clothing:
			case kFormType_Ingredient:
			case kFormType_Misc:
			case kFormType_Weapon:
			case kFormType_Ammo:
			case kFormType_SoulGem:
			case kFormType_Key:
			case kFormType_AlchemyItem:
			case kFormType_SigilStone:
				return true;

			case kFormType_Light:
				TESObjectLIGH* light = (TESObjectLIGH*)Oblivion_DynamicCast(refr->baseForm, 0, RTTI_TESForm, RTTI_TESObjectLIGH, 0);
				if (light)
					if (light->icon.ddsPath.m_dataLen)	//temp hack until I find canCarry flag on TESObjectLIGH
						return true;
		}
		return false;
	}
};

static const TESObjectCELL::ObjectListEntry* GetCellRefEntry(CellListVisitor visitor, UInt32 formType, const TESObjectCELL::ObjectListEntry* prev)
{
	const TESObjectCELL::ObjectListEntry* entry = NULL;
	switch(formType)
	{
	case 0:		//Any type
		entry = visitor.Find(RefMatcherAnyForm(), prev);
		break;
	case 69:	//Actor
		entry = visitor.Find(RefMatcherActor(), prev);
		break;
	case 70:	//Inventory Item
		entry = visitor.Find(RefMatcherItem(), prev);
		break;
	default:
		entry = visitor.Find(RefMatcherFormType(formType), prev);
	}

	return entry;
}

static TESObjectREFR* CellScan(Script* scriptObj, UInt32 formType = 0, UInt32 cellDepth = 0, bool getFirst = false)
{
	static std::map<UInt32, CellScanInfo> scanScripts;
	UInt32 idx = scriptObj->refID;

	if (getFirst)
		scanScripts.erase(idx);

	if (scanScripts.find(idx) == scanScripts.end())
	{
		scanScripts[idx] = CellScanInfo(cellDepth, formType);
		scanScripts[idx].FirstCell();
	}

	CellScanInfo* info = &(scanScripts[idx]);

	bool bContinue = true;
	while (bContinue)
	{
		info->prev = GetCellRefEntry(CellListVisitor(&info->curCell->objectList), info->formType, info->prev);
		if (!info->prev || !info->prev->refr)				//no ref found
		{
			if (!info->NextCell())			//check next cell if possible
				bContinue = false;
		}
		else
			bContinue = false;			//found a ref
	}

	if (info->prev)
		return info->prev->refr;
	else
	{
		scanScripts.erase(idx);
		return NULL;
	}

}

static bool Cmd_GetFirstRef_Execute(COMMAND_ARGS)
{
	UInt32 formType = 0;
	UInt32 cellDepth = -1;
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &formType, &cellDepth);

	if (cellDepth == -1)
		cellDepth = 0;

	TESObjectREFR* refr = CellScan(scriptObj, formType, cellDepth, true);
	if (refr)
		*refResult = refr->refID;

	return true;
}

static bool Cmd_GetNextRef_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	TESObjectREFR* refr = CellScan(scriptObj);
	if (refr)
		*refResult = refr->refID;

	return true;
}

static bool Cmd_GetNumRefs_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 formType = 0;
	UInt32 cellDepth = -1;

	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &formType, &cellDepth);

	if (cellDepth == -1)
		cellDepth = 0;

	CellScanInfo info(cellDepth, formType);
	info.FirstCell();

	while (info.curCell)
	{
		CellListVisitor visitor(&info.curCell->objectList);
		switch (formType)
		{
		case 0:
			*result += visitor.CountIf(RefMatcherAnyForm());
			break;
		case 69:
			*result += visitor.CountIf(RefMatcherActor());
			break;
		case 70:
			*result += visitor.CountIf(RefMatcherItem());
			break;
		default:
			*result += visitor.CountIf(RefMatcherFormType(formType));
		}
		info.NextCell();
	}

	return true;
}

static bool Cmd_IsPersistent_Execute(COMMAND_ARGS)
{
	*result = 0;

	if (thisObj)
		*result = (thisObj->flags & thisObj->kFlags_Persistent) ? 1 : 0;

	return true;
}

//SetPersistent not exposed because changes are not saved
static bool Cmd_SetPersistent_Execute(COMMAND_ARGS)
{
	UInt32 persistent = 0;
	*result = 0;

	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &persistent);
	if (thisObj)
	{
		if (persistent)
			thisObj->flags |= thisObj->kFlags_Persistent;
		else
			thisObj->flags &= ~thisObj->kFlags_Persistent;
	}

	return true;
}

#endif

CommandInfo kCommandInfo_GetTravelHorse =
{
	"GetTravelHorse",
	"",
	0,
	"returns the travel horse of the reference",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetTravelHorse_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_OneObjectRef[1] =
{
	{	"container", kParamType_ObjectRef, 0 },
};

CommandInfo kCommandInfo_SetTravelHorse =
{
	"SetTravelHorse",
	"",
	0,
	"sets the travel horse of the reference",
	1,
	1,
	kParams_OneObjectRef,
	HANDLER(Cmd_SetTravelHorse_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetOpenKey =
{
	"GetOpenKey",
	"GetKey",
	0,
	"returns the key used to unlock the calling object",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetOpenKey_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOpenKey =
{
	"SetOpenKey",
	"SetKey",
	0,
	"sets the key used to unlock the calling object",
	1,
	1,
	kParams_OneInventoryObject,
	HANDLER(Cmd_SetOpenKey_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetParentCellOwner =
{
	"GetParentCellOwner",
	"GetCellOwner",
	0,
	"returns the owner of the calling reference's cell",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetParentCellOwner_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetOwner =
{
	"GetOwner",
	"",
	0,
	"returns the owner of the calling reference",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetOwner_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_OneActor[1] =
{
	{	"actor reference",	 kParamType_Actor,	0	},
};

CommandInfo kCommandInfo_GetOwningFactionRequiredRank =
{
	"GetOwningFactionRequiredRank",
	"GetOwningFactionRank",
	0,
	"returns the required rank for ownership of the calling reference",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetOwningFactionRequiredRank_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetParentCellOwningFactionRequiredRank =
{
	"GetParentCellOwningFactionRequiredRank",
	"GetCellFactionRank",
	0,
	"returns the required rank for ownership of the calling reference's cell",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetParentCellOwningFactionRequiredRank_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_OneNPC[1] =
{
	{	"NPC",	kParamType_NPC,	0	},
};

CommandInfo kCommandInfo_IsOffLimits =
{
	"IsOffLimits", "IsIllegal",
	0,
	"returns true if activating the calling reference would result in a crime for the actor",
	1,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_IsOffLimits_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsLoadDoor =
{
	"IsLoadDoor", "",
	0,
	"returns 1 if the calling reference is a load door",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsLoadDoor_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetLinkedDoor =
{
	"GetLinkedDoor", "",
	0,
	"returns the door to which the calling reference is linked",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetLinkedDoor_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetTeleportCell =
{
	"GetTeleportCell", "",
	0,
	"returns the cell to which the calling door reference teleports",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetTeleportCell_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_GetFirstRef[2] =
{
	{	"form type",	kParamType_Integer,	1	},
	{	"cell depth",	kParamType_Integer,	1	},
};

CommandInfo kCommandInfo_GetFirstRef =
{
	"GetFirstRef", "",
	0,
	"returns the first reference of the specified type in the current cell",
	0,
	2,
	kParams_GetFirstRef,
	HANDLER(Cmd_GetFirstRef_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNextRef =
{
	"GetNextRef", "",
	0,
	"returns the next reference of a given type in the  current cell",
	0,
	0,
	NULL,
	HANDLER(Cmd_GetNextRef_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNumRefs =
{
	"GetNumRefs", "",
	0,
	"returns the number of references of a given type in the current cell",
	0,
	2,
	kParams_GetFirstRef,
	HANDLER(Cmd_GetNumRefs_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsPersistent =
{
	"IsPersistent", "",
	0,
	"returns true if the reference is persistent",
	0,
	0,
	NULL,
	HANDLER(Cmd_IsPersistent_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetPersistent =
{
	"SetPersistent", "",
	0,
	"sets the persistence of the calling reference",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_SetPersistent_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};
