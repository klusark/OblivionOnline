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

#include "InternalSerialization.h"

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
	if (!thisObj || thisObj->typeID != kFormType_ACHR) return true;	//only NPCs may have travel horses

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
		thisObj->baseExtraList.Add(xHorse);
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
			TESFaction* owningFaction = (TESFaction*)Oblivion_DynamicCast(owner, 0, RTTI_TESForm, RTTI_TESFaction, 0);
			if (owningFaction && !(owningFaction->IsEvil()))		//no crime to steal from evil factions
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
	bool	includeTakenRefs;

	CellScanInfo()
	{	}

	CellScanInfo(UInt8 _cellDepth, UInt8 _formType, bool _includeTaken) 
					:	cellDepth(_cellDepth), formType(_formType), includeTakenRefs(_includeTaken), prev(NULL)
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
	bool m_includeTaken;
public:
	RefMatcherAnyForm(bool includeTaken) : m_includeTaken(includeTaken)
		{ }

	bool Accept(const TESObjectREFR* refr)
	{
		if (m_includeTaken || !(refr->IsTaken()))
			return true;
		else
			return false;
	}
};

class RefMatcherFormType
{
	UInt32 m_formType;
	bool m_includeTaken;
public:
	RefMatcherFormType(UInt32 formType, bool includeTaken) : m_formType(formType), m_includeTaken(includeTaken)
		{ }

	bool Accept(const TESObjectREFR* refr)
	{
		if (!m_includeTaken && refr->IsTaken())
			return false;
		else if (refr->baseForm->typeID == m_formType && refr->baseForm->refID != 7)	//exclude player for kFormType_NPC
			return true;
		else
			return false;
	}
};

class RefMatcherActor
{
public:
	RefMatcherActor()
		{ }

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
	bool m_includeTaken;
public:
	RefMatcherItem(bool includeTaken) : m_includeTaken(includeTaken)
		{ }

	bool Accept(const TESObjectREFR* refr)
	{
		if (!m_includeTaken && refr->IsTaken())
			return false;

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

static const TESObjectCELL::ObjectListEntry* GetCellRefEntry(CellListVisitor visitor, UInt32 formType, const TESObjectCELL::ObjectListEntry* prev, bool includeTaken)
{
	const TESObjectCELL::ObjectListEntry* entry = NULL;
	switch(formType)
	{
	case 0:		//Any type
		entry = visitor.Find(RefMatcherAnyForm(includeTaken), prev);
		break;
	case 69:	//Actor
		entry = visitor.Find(RefMatcherActor(), prev);
		break;
	case 70:	//Inventory Item
		entry = visitor.Find(RefMatcherItem(includeTaken), prev);
		break;
	default:
		entry = visitor.Find(RefMatcherFormType(formType, includeTaken), prev);
	}

	return entry;
}

static TESObjectREFR* CellScan(Script* scriptObj, UInt32 formType = 0, UInt32 cellDepth = 0, bool getFirst = false, bool includeTaken = false)
{
	static std::map<UInt32, CellScanInfo> scanScripts;
	UInt32 idx = scriptObj->refID;

	if (getFirst)
		scanScripts.erase(idx);

	if (scanScripts.find(idx) == scanScripts.end())
	{
		scanScripts[idx] = CellScanInfo(cellDepth, formType, includeTaken);
		scanScripts[idx].FirstCell();
	}

	CellScanInfo* info = &(scanScripts[idx]);

	bool bContinue = true;
	while (bContinue)
	{
		info->prev = GetCellRefEntry(CellListVisitor(&info->curCell->objectList), info->formType, info->prev, info->includeTakenRefs);
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
	UInt32 bIncludeTakenRefs = 0;
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;
	
	PlayerCharacter* pc = *g_thePlayer;
	if (!pc || !(pc->parentCell))
		return true;						//avoid crash when these functions called in main menu before parentCell instantiated

	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &formType, &cellDepth, &bIncludeTakenRefs);

	if (cellDepth == -1)
		cellDepth = 0;

	TESObjectREFR* refr = CellScan(scriptObj, formType, cellDepth, true, bIncludeTakenRefs ? true : false);
	if (refr)
		*refResult = refr->refID;

	return true;
}

static bool Cmd_GetNextRef_Execute(COMMAND_ARGS)
{
	
	PlayerCharacter* pc = *g_thePlayer;
	if (!pc || !(pc->parentCell))
		return true;						//avoid crash when these functions called in main menu before parentCell instantiated

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
	UInt32 includeTakenRefs = 0;

	PlayerCharacter* pc = *g_thePlayer;
	if (!pc || !(pc->parentCell))
		return true;						//avoid crash when these functions called in main menu before parentCell instantiated

	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &formType, &cellDepth, &includeTakenRefs);

	bool bIncludeTakenRefs = includeTakenRefs ? true : false;
	if (cellDepth == -1)
		cellDepth = 0;

	CellScanInfo info(cellDepth, formType, bIncludeTakenRefs);
	info.FirstCell();

	while (info.curCell)
	{
		CellListVisitor visitor(&info.curCell->objectList);
		switch (formType)
		{
		case 0:
			*result += visitor.CountIf(RefMatcherAnyForm(bIncludeTakenRefs));
			break;
		case 69:
			*result += visitor.CountIf(RefMatcherActor());
			break;
		case 70:
			*result += visitor.CountIf(RefMatcherItem(bIncludeTakenRefs));
			break;
		default:
			*result += visitor.CountIf(RefMatcherFormType(formType, bIncludeTakenRefs));
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

static bool Cmd_GetNumChildRefs_Execute(COMMAND_ARGS)
{
	*result = 0;

	if (!thisObj)
		return true;

	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_EnableStateChildren);
	if (!xData)
		return true;
	
	ExtraEnableStateChildren* xKids = (ExtraEnableStateChildren*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraEnableStateChildren, 0);
	if (xKids)
		*result = EnableStateChildrenVisitor(&xKids->childList).Count();

	return true;
}

static bool Cmd_GetNthChildRef_Execute(COMMAND_ARGS)
{
	UInt32 idx = 0;
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &idx))
		return true;

	else if (!thisObj)
		return true;

	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_EnableStateChildren);
	if (!xData)
		return true;
	
	ExtraEnableStateChildren* xKids = (ExtraEnableStateChildren*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraEnableStateChildren, 0);
	if (xKids)
	{
		TESObjectREFR* kid = EnableStateChildrenVisitor(&xKids->childList).GetNthInfo(idx);
		if (kid)
			*refResult = kid->refID;
	}

	return true;
}

static bool Cmd_SetScaleEX_Execute(COMMAND_ARGS)
{
	float newScale = 0;
	*result = 0;

	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &newScale))
		return true;
	else if (!thisObj)
		return true;

	thisObj->scale = newScale;
	thisObj->MarkAsModified(thisObj->kChanged_Scale);
	*result = 1;

	return true;
}

static bool Cmd_IsActivatable_Execute(COMMAND_ARGS)
{
	*result = 0;

	if (!thisObj)
		return true;
	
	UInt32 type = thisObj->baseForm->typeID;
	if (type >= kFormType_Activator && type <= kFormType_Ingredient)
		*result = 1;
	else if (type == kFormType_Light)
		*result = 1;
	else if (type == kFormType_Misc)
		*result = 1;
	else if (type >= kFormType_Flora && type <= kFormType_Creature)
		*result = 1;
	else if (type >= kFormType_SoulGem && type <= kFormType_AlchemyItem)
		*result = 1;
	else if (type == kFormType_SigilStone)
		*result = 1;

	return true;
}

static bool Cmd_IsHarvested_Execute(COMMAND_ARGS)
{
	*result = 0;
	if (!thisObj)
		return true;
	else if (thisObj->baseForm->typeID != kFormType_Flora)
		return true;

	if (thisObj->flags & TESFlora::kFloraFlags_Harvested)
		*result = 1;

	return true;
}

static bool Cmd_SetHarvested_Execute(COMMAND_ARGS)
{
	UInt32 bHarvested = 0;
	if (!thisObj || thisObj->baseForm->typeID != kFormType_Flora)
		return true;
	else if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &bHarvested))
		return true;

	if (bHarvested)
	{
		thisObj->flags |= TESFlora::kFloraFlags_Harvested;
		thisObj->MarkAsModified(TESFlora::kModified_Empty);
	}
	else
		thisObj->flags &= ~TESFlora::kFloraFlags_Harvested;

	return true;
}

static bool Cmd_HasBeenPickedUp_Execute(COMMAND_ARGS)
{
	*result = 0;
	if (thisObj && thisObj->IsTaken())
		*result = 1;

	return true;
}

static bool Cmd_SetHasBeenPickedUp_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 bPickedUp = 0;

	if (!thisObj)
		return true;
	else if (!ExtractArgs(EXTRACT_ARGS, &bPickedUp))
		return true;

	thisObj->SetTaken(bPickedUp ? true : false);
	return true;
}

UInt32 GetProjectileData(MobileObject* mob, bool getMagicData = false, MagicProjectileData** dataOut = NULL)
{
	//is there a better way to do this?
	UInt32 type = -1;
	MagicProjectileData* data;

	if (mob->baseForm->typeID != kFormType_Ammo)
	{
		data = NULL;
		return -1;
	}

	MagicBallProjectile* ball = (MagicBallProjectile*)Oblivion_DynamicCast(mob, 0, RTTI_MobileObject, RTTI_MagicBallProjectile, 0);
	if (ball)
	{
		data = &ball->data;
		type = kProjectileType_Ball;
	}
	else
	{
		MagicBoltProjectile* bolt = (MagicBoltProjectile*)Oblivion_DynamicCast(mob, 0, RTTI_MobileObject, RTTI_MagicBoltProjectile, 0);
		if (bolt)
		{
			data = &bolt->data;
			type = kProjectileType_Bolt;
		}
		else
		{
			MagicFogProjectile* fog = (MagicFogProjectile*)Oblivion_DynamicCast(mob, 0, RTTI_MobileObject, RTTI_MagicFogProjectile, 0);
			if (fog)
			{
				data = &fog->data;
				type = kProjectileType_Fog;
			}
			else
			{
				type = kProjectileType_Arrow;
				data = NULL;
			}
		}
	}

	if (getMagicData)
		*dataOut = data;

	return type;
}

static bool Cmd_GetProjectileType_Execute(COMMAND_ARGS)
{
	*result = -1;

	if (thisObj)
	{
		MobileObject* mob = (MobileObject*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_MobileObject, 0);
		if (mob)
			*result = GetProjectileData(mob);
	}

	return true;
}

static bool Cmd_GetMagicProjectileSpell_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (thisObj)
	{
		MobileObject* mob = (MobileObject*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_MobileObject, 0);
		if (mob)
		{
			MagicProjectileData* data = NULL;
			GetProjectileData(mob, true, &data);
			if (data && data->magicItem)
			{
				SpellItem* spell = (SpellItem*)Oblivion_DynamicCast(data->magicItem, 0, RTTI_MagicItem, RTTI_SpellItem, 0);
				if (spell)
					*refResult = spell->refID;
			}
		}
	}

	return true;
}

static bool Cmd_GetArrowProjectileEnchantment_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (thisObj)
	{
		ArrowProjectile* arrow = (ArrowProjectile*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_ArrowProjectile, 0);
		if (arrow && arrow->arrowEnch)
			*refResult = arrow->arrowEnch->refID;
	}

	return true;
}

static bool Cmd_GetArrowProjectileBowEnchantment_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (thisObj)
	{
		ArrowProjectile* arrow = (ArrowProjectile*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_ArrowProjectile, 0);
		if (arrow && arrow->bowEnch)
			*refResult = arrow->bowEnch->refID;
	}

	return true;
}

static bool Cmd_GetArrowProjectilePoison_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (thisObj)
	{
		ArrowProjectile* arrow = (ArrowProjectile*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_ArrowProjectile, 0);
		if (arrow && arrow->poison)
			*refResult = arrow->poison->refID;
	}

	return true;
}

static bool Cmd_GetProjectileSource_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (thisObj)
	{
		MobileObject* mob = (MobileObject*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_MobileObject, 0);
		if (mob)
		{
			MagicProjectileData* data = NULL;
			UInt32 type = GetProjectileData(mob, true, &data);
			if (type == kProjectileType_Arrow)
			{
				ArrowProjectile* arrow = (ArrowProjectile*)Oblivion_DynamicCast(mob, 0, RTTI_MobileObject, RTTI_ArrowProjectile, 0);
				if (arrow && arrow->shooter)
					*refResult = arrow->shooter->refID;
			}
			else if (type != -1)
				if (data && data->caster)
				{
					Actor* caster = (Actor*)Oblivion_DynamicCast(data->caster, 0, RTTI_MagicCaster, RTTI_Actor, 0);
					if (caster)
						*refResult = caster->refID;
					else
					{
						NonActorMagicCaster* caster = (NonActorMagicCaster*)Oblivion_DynamicCast(data->caster, 0, RTTI_MagicCaster, RTTI_NonActorMagicCaster, 0);
						if (caster && caster->caster)
							*refResult = caster->caster->refID;
					}
				}
		}
	}

	return true;
}

static bool Cmd_SetMagicProjectileSpell_Execute(COMMAND_ARGS)
{
	if (!thisObj)
		return true;

	SpellItem* spell = NULL;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &spell))
		return true;
	
	MobileObject* mob = (MobileObject*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_MobileObject, 0);
	if (mob)
	{
		MagicProjectileData* data = NULL;
		GetProjectileData(mob, true, &data);
		if (data && spell)
			data->magicItem = &(spell->magicItem);
	}

	return true;
}

static bool Cmd_SetPlayerProjectile_Execute(COMMAND_ARGS)
{
	if (!thisObj)
		return true;

	MobileObject* mob = (MobileObject*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_MobileObject, 0);
	if (mob)
	{
		MagicProjectileData* data = NULL;
		UInt32 projType = GetProjectileData(mob, true, &data);
		if (projType == kProjectileType_Arrow)
		{
			ArrowProjectile* arrow = (ArrowProjectile*)Oblivion_DynamicCast(mob, 0, RTTI_MobileObject, RTTI_ArrowProjectile, 0);
			if (arrow)
				arrow->shooter = *g_thePlayer;
		}
		else if (projType != -1)
		{
			if (data)
			{
				MagicCaster* caster = (MagicCaster*)Oblivion_DynamicCast(*g_thePlayer, 0, RTTI_Actor, RTTI_MagicCaster, 0);
				if (caster)
					data->caster = caster;
			}
		}
	}

	return true;
}

//NULL for projectile source = BAD.
static bool Cmd_ClearProjectileSource_Execute(COMMAND_ARGS)
{
	if (thisObj)
	{
		MobileObject* mob = (MobileObject*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_MobileObject, 0);
		if (mob)
		{
			MagicProjectileData* data = NULL;
			UInt32 projType = GetProjectileData(mob, true, &data);
			if (projType == kProjectileType_Arrow)
			{
				ArrowProjectile* arrow = (ArrowProjectile*)Oblivion_DynamicCast(mob, 0, RTTI_MobileObject, RTTI_ArrowProjectile, 0);
				if (arrow)
					arrow->shooter = NULL;
			}
			else if (projType != -1)
			{
				if (data)
					data->caster = NULL;
			}
		}
	}

	return true;
}

static bool Cmd_GetRefCount_Execute(COMMAND_ARGS)
{
	*result = 1;
	if (!thisObj)
		return true;

	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_Count);
	if (xData)
	{
		ExtraCount* xCount = (ExtraCount*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraCount, 0);
		if (xCount)
			*result = xCount->count;
	}
	return true;
}

static bool Cmd_SetRefCount_Execute(COMMAND_ARGS)
{
	UInt32 newCount = 0;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &newCount))
		return true;
	else if (!thisObj)
		return true;

	ExtraCount* xCount = NULL;
	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_Count);
	if (xData)
		xCount = (ExtraCount*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraCount, 0);
	else
	{
		xCount = ExtraCount::Create();
		xCount->count = newCount;
		thisObj->baseExtraList.Add(xCount);
	}

	if (xCount)
		xCount->count = newCount;

	return true;
}

static bool GetSound_Execute(COMMAND_ARGS, UInt32 whichSound)
{
	TESForm* form = NULL;
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (whichSound > 2)
		return false;

	if (!ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &form))
		return false;

	form = form->TryGetREFRParent(); 
	if (!form)
	{
		if (thisObj)
			form = thisObj->baseForm;
		else
			return false;
	}

	switch (form->typeID)
	{
	case kFormType_Door:
		{
			TESObjectDOOR* door = (TESObjectDOOR*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESObjectDOOR, 0);
			if (door->animSounds[whichSound])
				*refResult = door->animSounds[whichSound]->refID;
		}
		break;
	case kFormType_Container:
		{
			if (whichSound == 2)
				return false;
			TESObjectCONT* cont = (TESObjectCONT*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESObjectCONT, 0);
			if (cont->animSounds[whichSound])
				*refResult = cont->animSounds[whichSound]->refID;
		}
		break;
	case kFormType_Activator:
		{
			if (whichSound != 2)
				return false;
			TESObjectACTI* acti = (TESObjectACTI*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESObjectACTI, 0);

			if (acti->loopSound)
				*refResult = acti->loopSound->refID;
		}
		break;
	case kFormType_Light:
		{
			if (whichSound != 2)
				return false;
			TESObjectLIGH* light = (TESObjectLIGH*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESObjectLIGH, 0);

			if (light->loopSound)
				*refResult = light->loopSound->refID;
		}
		break;
	default:
		return false;
	}

	return true;
}

static bool Cmd_GetOpenSound_Execute(COMMAND_ARGS)
{
	*result = 0;
	GetSound_Execute(PASS_COMMAND_ARGS, 0);
	return true;
}

static bool Cmd_GetCloseSound_Execute(COMMAND_ARGS)
{
	*result = 0;
	GetSound_Execute(PASS_COMMAND_ARGS, 1);
	return true;
}

static bool Cmd_GetLoopSound_Execute(COMMAND_ARGS)
{
	*result = 0;
	GetSound_Execute(PASS_COMMAND_ARGS, 2);
	return true;
}

static bool SetSound_Execute(COMMAND_ARGS, UInt32 whichSound)
{
	TESForm* form = NULL;
	TESSound* sound = NULL;
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (whichSound > 2)
		return false;

	if (!ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &sound, &form))
		return false;

	form = form->TryGetREFRParent(); 
	if (!form)
	{
		if (thisObj)
			form = thisObj->baseForm;
		else
			return false;
	}

	switch (form->typeID)
	{
	case kFormType_Door:
		{
			TESObjectDOOR* door = (TESObjectDOOR*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESObjectDOOR, 0);
			door->animSounds[whichSound] = sound;
		}
		break;
	case kFormType_Container:
		{
			if (whichSound == 2)
				return false;
			TESObjectCONT* cont = (TESObjectCONT*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESObjectCONT, 0);
			cont->animSounds[whichSound] = sound;
		}
		break;
	case kFormType_Activator:
		{
			if (whichSound != 2)
				return false;
			TESObjectACTI* acti = (TESObjectACTI*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESObjectACTI, 0);
			acti->loopSound = sound;
		}
		break;
	case kFormType_Light:
		{
			if (whichSound != 2)
				return false;
			TESObjectLIGH* light = (TESObjectLIGH*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESObjectLIGH, 0);
			light->loopSound = sound;
		}
		break;
	default:
		return false;
	}

	return true;
}

static bool Cmd_SetOpenSound_Execute(COMMAND_ARGS)
{
	*result = 0;
	SetSound_Execute(PASS_COMMAND_ARGS, 0);
	return true;
}

static bool Cmd_SetCloseSound_Execute(COMMAND_ARGS)
{
	*result = 0;
	SetSound_Execute(PASS_COMMAND_ARGS, 1);
	return true;
}

static bool Cmd_SetLoopSound_Execute(COMMAND_ARGS)
{
	*result = 0;
	SetSound_Execute(PASS_COMMAND_ARGS, 2);
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

static ParamInfo kParams_GetFirstRef[3] =
{
	{	"form type",			kParamType_Integer,	1	},
	{	"cell depth",			kParamType_Integer,	1	},
	{	"include taken refs",	kParamType_Integer,	1	},
};

CommandInfo kCommandInfo_GetFirstRef =
{
	"GetFirstRef", "",
	0,
	"returns the first reference of the specified type in the current cell",
	0,
	3,
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
	3,
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
	1,
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
	1,
	1,
	kParams_OneInt,
	HANDLER(Cmd_SetPersistent_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNumChildRefs =
{
	"GetNumChildRefs", "", 0,
	"returns the number of enable state children for the calling reference",
	1, 0, NULL,
	HANDLER(Cmd_GetNumChildRefs_Execute),
	Cmd_Default_Parse,
	NULL, 0
};

CommandInfo kCommandInfo_GetNthChildRef =
{
	"GetNthChildRef", "", 0,
	"returns the nth enable state child for the calling reference",
	1,
	1,
	kParams_OneInt,
	HANDLER(Cmd_GetNthChildRef_Execute),
	Cmd_Default_Parse,
	NULL, 0
};

CommandInfo kCommandInfo_SetScaleEX =
{
	"SetScaleEX", "", 0,
	"sets scale of the calling reference above or below limits of setScale",
	1,
	1,
	kParams_OneFloat,
	HANDLER(Cmd_SetScaleEX_Execute),
	Cmd_Default_Parse,
	NULL, 0
};

CommandInfo kCommandInfo_IsActivatable =
{
	"IsActivatable", "",
	0,
	"returns 1 if the calling reference can be activated",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsActivatable_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsHarvested =
{
	"IsHarvested", "",
	0,
	"returns 1 if the calling flora reference has been harvested",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsHarvested_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetHarvested =
{
	"SetHarvested", "",
	0,
	"sets the harvested flag on the calling flora reference",
	1,
	1,
	kParams_OneInt,
	HANDLER(Cmd_SetHarvested_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_HasBeenPickedUp =
{
	"HasBeenPickedUp",
	"IsTaken",
	0,
	"returns 1 if the calling reference has been placed in an inventory",
	1,
	0,
	NULL,
	HANDLER(Cmd_HasBeenPickedUp_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetProjectileType =
{
	"GetProjectileType",
	"",
	0,
	"returns the type of the calling projectile",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetProjectileType_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicProjectileSpell =
{
	"GetMagicProjectileSpell",
	"GetMPSpell",
	0,
	"returns the spell associated with the projectile",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetMagicProjectileSpell_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetArrowProjectileEnchantment =
{
	"GetArrowProjectileEnchantment",
	"GetAPEnch",
	0,
	"returns the enchantment on the calling arrow projectile",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetArrowProjectileEnchantment_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetArrowProjectileBowEnchantment =
{
	"GetArrowProjectileBowEnchantment",
	"GetAPBowEnch",
	0,
	"returns the bow enchantment on the calling arrow projectile",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetArrowProjectileBowEnchantment_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetArrowProjectilePoison =
{
	"GetArrowProjectilePoison",
	"GetAPPoison",
	0,
	"returns the poison on the calling arrow projectile",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetArrowProjectilePoison_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetProjectileSource =
{
	"GetProjectileSource",
	"",
	0,
	"returns the source of the calling projectile",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetProjectileSource_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetMagicProjectileSpell =
{
	"SetMagicProjectileSpell",
	"SetMPSpell",
	0,
	"sets the spell associated with the calling magic projectile",
	1,
	1,
	kParams_OneSpellItem,
	HANDLER(Cmd_SetMagicProjectileSpell_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

/*
CommandInfo kCommandInfo_SetProjectileSource =
{
	"SetProjectileSource",
	"",
	0,
	"sets the source of the calling projectile to the specified actor",
	1,
	1,
	kParams_OneActor,
	HANDLER(Cmd_SetProjectileSource_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};
*/

CommandInfo kCommandInfo_ClearProjectileSource =
{
	"ClearProjectileSource",
	"",
	0,
	"removes information about the source of the calling projectile",
	1,
	0,
	NULL,
	HANDLER(Cmd_ClearProjectileSource_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetRefCount =
{
	"GetRefCount",
	"",
	0,
	"returns the count in a stacked reference",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetRefCount_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetRefCount =
{
	"SetRefCount",
	"",
	0,
	"sets the count on a stacked reference",
	1,
	1,
	kParams_OneInt,
	HANDLER(Cmd_SetRefCount_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_SetSound[2] =
{
	{	"sound",	kParamType_Sound,			0	},
	{	"object",	kParamType_InventoryObject,	1	},
};

CommandInfo kCommandInfo_GetOpenSound =
{
	"GetOpenSound",
	"",
	0,
	"returns the open sound for a container or door",
	0,
	1,
	kParams_OneOptionalInventoryObject,
	HANDLER(Cmd_GetOpenSound_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCloseSound =
{
	"GetCloseSound",
	"",
	0,
	"returns the close sound for a container or door",
	0,
	1,
	kParams_OneOptionalInventoryObject,
	HANDLER(Cmd_GetCloseSound_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetLoopSound =
{
	"GetLoopSound",
	"",
	0,
	"returns the loop sound for an object",
	0,
	1,
	kParams_OneOptionalInventoryObject,
	HANDLER(Cmd_GetLoopSound_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOpenSound =
{
	"SetOpenSound",
	"",
	0,
	"sets the open sound for a container or door",
	0,
	2,
	kParams_SetSound,
	HANDLER(Cmd_SetOpenSound_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetCloseSound =
{
	"SetCloseSound",
	"",
	0,
	"sets the close sound for a container or door",
	0,
	2,
	kParams_SetSound,
	HANDLER(Cmd_SetCloseSound_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetLoopSound =
{
	"SetLoopSound",
	"",
	0,
	"sets the looping sound for an object",
	0,
	2,
	kParams_SetSound,
	HANDLER(Cmd_SetLoopSound_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

DEFINE_COMMAND(SetPlayerProjectile,
			   sets the player as the source of the projectile,
			   1,
			   0,
			   NULL);

CommandInfo kCommandInfo_SetHasBeenPickedUp =
{
	"SetHasBeenPickedUp",
	"SetTaken",
	0,
	"toggles the 'taken' flag on a reference",
	1,
	1,
	kParams_OneInt,
	HANDLER(Cmd_SetHasBeenPickedUp_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};