#include "Commands_Player.h"
#include "GameObjects.h"
#include "GameExtraData.h"
#include "GameAPI.h"
#include "GameForms.h"
#include "GameProcess.h"
#include "ParamInfos.h"
#include <set>
#include <map>

#if OBLIVION

//class TESMagicTargetForm : public TESForm
//{
//	UInt32 unk[20];
//};

static bool Cmd_GetActiveSpell_Execute(COMMAND_ARGS)
{
	UInt32			* refResult = (UInt32 *)result;
	MagicItem		* activeMagicItem = (*g_thePlayer)->activeMagicItem;


	*refResult = 0;
	
#if 0
	// spell walking code to help decode spells and effects
	PlayerCharacter* pc = *g_thePlayer;
	DumpClass (pc);

	TESMagicTargetForm* mtForm = (TESMagicTargetForm*)Oblivion_DynamicCast(pc, 0, RTTI_PlayerCharacter, RTTI_TESMagicTargetForm, 0);
	if (mtForm) {
		DumpClass(mtForm);
	}

	MagicTarget* mt = (MagicTarget*)Oblivion_DynamicCast(pc, 0, RTTI_PlayerCharacter, RTTI_MagicTarget, 0);
	if (mt) {
		DumpClass(mt);
	}


	TESNPC* npc = (TESNPC *)Oblivion_DynamicCast(pc->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
	
	TESSpellList& spellList = npc->spellList;
	TESSpellList::Entry* curEntry = &spellList.spellList;
	while (curEntry && curEntry->type != NULL) {
		SpellItem* spell = (SpellItem*)Oblivion_DynamicCast(curEntry->type, 0, RTTI_TESForm, RTTI_SpellItem, 0);
		EffectItemList* spellEffectList = (EffectItemList*)Oblivion_DynamicCast(spell, 0, RTTI_SpellItem, RTTI_EffectItemList, 0);
		curEntry = curEntry->next;
	} 
#endif

	if(activeMagicItem)
	{
		TESForm	* activeMagicItemForm = (TESForm *)Oblivion_DynamicCast(activeMagicItem, 0, RTTI_MagicItem, RTTI_TESForm, 0);
		if(activeMagicItemForm)
		{
			//Console_Print("GetPlayerSpell: %08X", activeMagicItemForm->refID);
			*refResult = activeMagicItemForm->refID;
		}
	}

	return true;
}

static bool Cmd_SetActiveSpell_Execute(COMMAND_ARGS)
{
	TESForm	* spell = NULL;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &spell)) return true;

	SpellItem	* spellItem = (SpellItem *)Oblivion_DynamicCast(spell, 0, RTTI_TESForm, RTTI_SpellItem, 0);
	if(spellItem)
	{
		(*g_thePlayer)->SetActiveSpell(&spellItem->magicItem);
	}

	return true;
}

static bool Cmd_GetParentCell_Execute(COMMAND_ARGS)
{
	UInt32	* refResult = (UInt32 *)result;
	*refResult = 0;

	if(!thisObj) return true;

	TESForm	* currentCell = (TESForm *)Oblivion_DynamicCast(thisObj->parentCell, 0, RTTI_TESObjectCELL, RTTI_TESForm, 0);

	if(currentCell) {
		//Console_Print("Cell: %08x", currentCell->refID);
		*refResult = currentCell->refID;
	}

	return true;
}

static bool Cmd_GetParentWorldspace_Execute(COMMAND_ARGS)
{
	*result = 0;

	if(!thisObj || !thisObj->parentCell || !thisObj->parentCell->worldSpace) return true;

	UInt32	* refResult = (UInt32 *)result;
	*refResult = thisObj->parentCell->worldSpace->refID;

	return true;
}

static bool Cmd_HasSpell_Execute(COMMAND_ARGS)
{
	*result = 0;

	if (!thisObj) return true;

	TESNPC* npc = (TESNPC *)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
	if (!npc) return true;

	TESForm	* form = NULL;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &form)) return true;

	if(form)
	{
		SpellItem* spell = (SpellItem*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_SpellItem, 0);

#if 1

		EffectItemList* spellEffectList = (EffectItemList*)Oblivion_DynamicCast(spell, 0, RTTI_SpellItem, RTTI_EffectItemList, 0);
		if (spellEffectList) {
			EffectItemList::Entry* entry = &(spellEffectList->effectList);
			while (entry) {
				EffectItem* effect = entry->effectItem;
				entry = entry->next;
			}
		}

//		MagicItem* magicItem = (MagicItem*)Oblivion_DynamicCast(spell, 0, RTTI_SpellItem, RTTI_MagicItem, 0);
//		DumpClass(magicItem);
#endif

		TESSpellList& spellList = npc->spellList;
		TESSpellList::Entry* curEntry = &spellList.spellList;
		while (curEntry && curEntry->type != NULL) {
			TESForm* spellForm = curEntry->type;
			if (form == spellForm) {
				*result = 1;
				return true;
			}
			curEntry = curEntry->next;
		} 
	}

	return true;
}

static bool Cmd_GetSpellCount_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 spellCount = 0;

	TESForm* baseForm = (thisObj) ? thisObj->baseForm : (*g_thePlayer)->baseForm;
	TESNPC* npc = (TESNPC *)Oblivion_DynamicCast(baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
	if (!npc) return true;

	TESSpellList& spellList = npc->spellList;
	TESSpellList::Entry* curEntry = &spellList.spellList;
	while (curEntry && curEntry->type != NULL) {
		++spellCount;
		curEntry = curEntry->next;
	} 
	*result = spellCount;
	return true;
}

static bool Cmd_GetNthSpell_Execute(COMMAND_ARGS)
{
	UInt32	* refResult = (UInt32 *)result;
	*refResult = 0;

	TESForm* baseForm = (thisObj) ? thisObj->baseForm : (*g_thePlayer)->baseForm;
	TESNPC* npc = (TESNPC *)Oblivion_DynamicCast(baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
	if (!npc) return true;

	UInt32 whichSpell = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &whichSpell)) return true;

	TESForm* spellForm = npc->spellList.GetNthSpell(whichSpell);
	if (spellForm) {
		*refResult = spellForm->refID;
	}
	return true;
}

static bool Cmd_RemoveAllSpells_Execute(COMMAND_ARGS)
{
	*result = 0;
	if (thisObj) {
		TESNPC* npc = (TESNPC *)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
		if (npc) {
			npc->spellList.RemoveAllSpells();
		}
	}
	return true;
}

static bool Cmd_IsMajor_Execute(COMMAND_ARGS)
{
	*result = 0;

	UInt32 skill = 0;
	TESClass* theClass = NULL;

	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &skill, &theClass);

	if (skill < kActorVal_Armorer || skill > kActorVal_Speechcraft) return true;
	if (!theClass) {
		if (!thisObj) return true;
		TESNPC* npc = (TESNPC *)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
		if (!npc || !npc->npcClass) return true;
		theClass = npc->npcClass;
	}
	
	for (int ix = 0; ix < 7; ++ix) {
		if (theClass->majorSkills[ix] == skill) {
			*result = 1;
			return true;
		}
	}

	return true;
}

static bool Cmd_IsClassAttribute_Execute(COMMAND_ARGS)
{
	*result = 0;

	UInt32 attribute = 0;
	TESClass* theClass = NULL;

	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &attribute, &theClass);

	if (attribute > kActorVal_Luck) return true;
	if (!theClass) {
		if (!thisObj) return true;
		TESNPC* npc = (TESNPC *)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
		if (!npc || !npc->npcClass) return true;
		theClass = npc->npcClass;
	}

	for (int ix = 0; ix < 2; ++ix) {
		if (theClass->attributes[ix] == attribute) {
			*result = 1;
			return true;
		}
	}

	return true;
}

static bool Cmd_GetClass_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if(!thisObj) return true;

	TESNPC* npc = (TESNPC *)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
	if (!npc || !npc->npcClass) return true;
	
	*refResult = npc->npcClass->refID;
	return true;
}

static bool Cmd_GetClassAttribute_Execute(COMMAND_ARGS)
{
	*result = 0;


	UInt32 which = 0;
	TESClass* theClass = NULL;

	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &which, &theClass);

	if (which > 1) return true;
	if (!theClass) {
		if(!thisObj) return true;
		TESNPC* npc = (TESNPC *)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
		if (!npc || !npc->npcClass) return true;
		theClass = npc->npcClass;
	}

	*result = theClass->attributes[which];
	return true;
}

static bool Cmd_GetClassSkill_Execute(COMMAND_ARGS)
{
	*result = 0;

	UInt32 which = 0;
	TESClass* theClass = NULL;

	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &which, &theClass);

	if (which > 6) return true;
	if (!theClass) {
		if(!thisObj) return true;
		TESNPC* npc = (TESNPC *)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
		if (!npc || !npc->npcClass) return true;
		theClass = npc->npcClass;
	}

	*result = theClass->majorSkills[which];
	return true;
}

static bool Cmd_GetClassSpecialization_Execute(COMMAND_ARGS)
{
	*result = 0;


	TESClass* theClass = NULL;

	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &theClass);

	if (!theClass) {
		if(!thisObj) return true;
		TESNPC* npc = (TESNPC *)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
		if (!npc || !npc->npcClass) return true;
		theClass = npc->npcClass;
	}

	*result = theClass->specialization;
	return true;
}

static bool Cmd_ModActorValue2_Execute(COMMAND_ARGS)
{
	UInt32	type;
	int		amount;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &type, &amount)) return true;

	if(thisObj && thisObj->IsActor())
	{
		Actor	* actor = (Actor *)thisObj;

		actor->ModActorBaseValue(type, amount, 0);
	}

	return true;
}

static bool Cmd_IsRefEssential_Execute(COMMAND_ARGS)
{
	if (!thisObj) return true;

	*result = 0;

	TESActorBaseData* actorBaseData = (TESActorBaseData*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESActorBaseData, 0);
	if (actorBaseData) {
		*result = actorBaseData->IsEssential();
	}

	return true;
}

static bool Cmd_SetRefEssential_Execute(COMMAND_ARGS)
{
	if (!thisObj) return true;

	*result = 0;

	UInt32 setEssential;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &setEssential)) return true;

	TESActorBaseData* actorBaseData = (TESActorBaseData*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESActorBaseData, 0);
	if (actorBaseData) {
		bool bEssential = (setEssential != 0) ? true : false;
		actorBaseData->SetEssential(bEssential);
	}

	return true;
}

static bool Cmd_GetActorLightAmount_Execute(COMMAND_ARGS)
{
	*result = 100.0f;

	if(!thisObj) return true;
	if(!thisObj->IsActor()) return true;

	Actor	* actor = (Actor *)thisObj;

	if(!actor->process) return true;

	*result = actor->process->GetLightAmount(actor, 0);

	//Console_Print("light amount = %f", (float)*result);

	return true;
}

static bool Cmd_IsCreature_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESActorBase* actorBase = NULL;
	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &actorBase);

	if (!actorBase) {
		if (!thisObj) return true;
		actorBase = (TESActorBase*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESActorBase, 0);
		if (!actorBase) return true;
	}

	TESCreature* creature = (TESCreature*)Oblivion_DynamicCast(actorBase, 0, RTTI_TESActorBase, RTTI_TESCreature, 0);
	if (creature) {
		*result = 1;
	}
	return true;
}

enum {
	kCreature_Type = 0,
	kCreature_CombatSkill,
	kCreature_MagicSkill,
	kCreature_StealthSkill,
	kCreature_Reach,
	kCreature_BaseScale,
	kCreature_SoulLevel,
};

static bool GetCreatureValue(COMMAND_ARGS, UInt32 whichVal)
{
	*result = 0;
	TESActorBase* actorBase = NULL;
	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &actorBase);

	if (!actorBase) {
		if (!thisObj) return true;
		actorBase = (TESActorBase*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESActorBase, 0);
		if (!actorBase) return true;
	}

	TESCreature* creature = (TESCreature*)Oblivion_DynamicCast(actorBase, 0, RTTI_TESActorBase, RTTI_TESCreature, 0);
	switch(whichVal) {
		case kCreature_Type: 
			{
				*result = (creature) ? creature->type : -1;
				break;
			}

		case kCreature_CombatSkill:
			{
				*result = (creature) ? creature->stealthSkill : 0;
				break;
			}

		case kCreature_MagicSkill:
			{
				*result = (creature) ? creature->stealthSkill : 0;
				break;
			}

		case kCreature_StealthSkill:
			{
				*result = (creature) ? creature->stealthSkill : 0;
				break;
			}
		case kCreature_Reach:
			{
				*result = (creature) ? creature->attackReach : 0;
				break;
			}

		case kCreature_BaseScale:
			{
				*result = (creature) ? creature->baseScale : 1.0;
				break;
			}
		case kCreature_SoulLevel:
			{
				*result = (creature) ? creature->soulLevel : 0;
				break;
			}
		default:
			*result = 0;
	}
	return true;
}

static bool Cmd_GetCreatureType_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_Type);
}

static bool Cmd_GetCreatureCombatSkill_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_CombatSkill);
}

static bool Cmd_GetCreatureMagicSkill_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_MagicSkill);
}

static bool Cmd_GetCreatureStealthSkill_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_StealthSkill);
}

static bool Cmd_GetCreatureReach_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_Reach);
}

static bool Cmd_GetCreatureBaseScale_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_BaseScale);
}

static bool Cmd_GetCreatureSoulLevel_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_SoulLevel);
}

static bool Cmd_IsThirdPerson_Execute(COMMAND_ARGS)
{
	// g_thePlayer guaranteed to be non-null
	*result = (*g_thePlayer && (*g_thePlayer)->isThirdPerson) ? 1 : 0;

//	Console_Print("IsThirdPerson = %f", *result);

	return true;
}

// (expValue:float) IncrementPlayerSkillUse skill:actor value whichTrigger
static bool Cmd_IncrementPlayerSkillUse_Execute(COMMAND_ARGS)
{
	*result = 0;
	
	UInt32 valSkill = 0;
	UInt32 whichUse = 0;
	float howManyTimes = 1.0;

	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &valSkill, &whichUse, &howManyTimes))
		return true;
	if (valSkill < kActorVal_Armorer || valSkill > kActorVal_Speechcraft) return true;

	(*g_thePlayer)->ModExperience(valSkill, whichUse, howManyTimes);
	
	*result = (*g_thePlayer)->skillAdv[valSkill-kActorVal_Armorer];
	return true;
}

// (expValue:float) GetPlayerSkillUse skill:actor value
static bool Cmd_GetPlayerSkillUse_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 valSkill = 0;

	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &valSkill))
		return true;
	if (valSkill < kActorVal_Armorer || valSkill > kActorVal_Speechcraft) return true;

	*result = (*g_thePlayer)->skillAdv[valSkill - kActorVal_Armorer];
	return true;
}

static bool Cmd_GetSkillUseIncrement_Execute(COMMAND_ARGS)
{
	*result = 0;

	UInt32 valSkill = 0;
	UInt32 whichUse = 0;
	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &valSkill, &whichUse);
	if (valSkill < kActorVal_Armorer || valSkill > kActorVal_Speechcraft) return true;

	TESSkill *skill = TESSkill::SkillForActorVal(valSkill);
	if (skill) {
		*result = (whichUse == 1) ? skill->useValue1 : skill->useValue0;
	}
	return true;
}

static bool Cmd_SetSkillUseIncrement_Execute(COMMAND_ARGS)
{
	*result = 0;
	float nuVal = 0.0;
	UInt32 valSkill = 0;
	UInt32 whichUse = 0;
	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &nuVal, &valSkill, &whichUse);
	if (valSkill < kActorVal_Armorer || valSkill > kActorVal_Speechcraft) return true;

	TESSkill *skill = TESSkill::SkillForActorVal(valSkill);
	if (skill) {
		if (whichUse == 1) {
			skill->useValue1 = nuVal;
		} else skill->useValue0 = nuVal;
	}
	return true;
}

enum {
	kRace_Attribute = 0,
	kRace_BonusSkill,
	kRace_IsBonusSkill,
	kRace_NthBonusSkill,
};

static bool GetRaceValue_Execute(COMMAND_ARGS, UInt32 whichVal)
{
	*result = 0;
	UInt32 intVal = 0;
	TESRace* race = 0;
	UInt32 isFemaleArg = 0; // male
	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &intVal, &race, &isFemaleArg);

	bool bIsFemale = (isFemaleArg == 0) ? false : true;
	if (!race) {
		if (!thisObj) return true;
		TESNPC* npc = (TESNPC*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
		if (!npc) return true;
		race = npc->race.race;
		if (!race) return true;
		bIsFemale = npc->actorBaseData.IsFemale();
	}

	switch (whichVal) {
		case kRace_Attribute: 
			{
				if (intVal > kActorVal_Luck) return true;
				*result = race->GetBaseAttribute(intVal, bIsFemale);
				break;
			}
		case kRace_BonusSkill: 
			{
				if (intVal < kActorVal_Armorer || intVal > kActorVal_Speechcraft) return true;
				*result = race->GetSkillBonus(intVal);
				break;
			}
		case kRace_IsBonusSkill:
			{
				if (intVal < kActorVal_Armorer || intVal > kActorVal_Speechcraft) return true;
				*result = race->IsBonusSkill(intVal);
				break;
			}
		case kRace_NthBonusSkill:
			{
				if (intVal > 6) return true;
				*result = race->GetNthBonusSkill(intVal);
				break;
			}

	}
	return true;
}

static bool Cmd_GetRaceAttribute_Execute(COMMAND_ARGS)
{
	return GetRaceValue_Execute(PASS_COMMAND_ARGS, kRace_Attribute);
}

static bool Cmd_GetRaceSkillBonus_Execute(COMMAND_ARGS)
{
	return GetRaceValue_Execute(PASS_COMMAND_ARGS, kRace_BonusSkill);
}

static bool Cmd_IsRaceBonusSkill_Execute(COMMAND_ARGS)
{
	return GetRaceValue_Execute(PASS_COMMAND_ARGS, kRace_IsBonusSkill);
}

static bool Cmd_GetNthRaceBonusSkill_Execute(COMMAND_ARGS)
{
	return GetRaceValue_Execute(PASS_COMMAND_ARGS, kRace_NthBonusSkill);
}

static bool Cmd_GetMerchantContainer_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32* refResult = (UInt32*)result;
	if (!thisObj) return true;

	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_MerchantContainer);
	if (xData) {
		ExtraMerchantContainer* xContainer = (ExtraMerchantContainer*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraMerchantContainer, 0);
		if (xContainer) {
			*refResult = xContainer->containerRef->refID;
		}
	}
	return true;
}

static bool Cmd_SetMerchantContainer_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!thisObj) return true;
	TESObjectREFR* objectRef = NULL;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &objectRef))
		return true;
	if (!thisObj) return true;

	TESNPC* npc = (TESNPC*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_TESNPC, 0);
	if (!npc) return true;
	
	if (objectRef->baseForm->typeID != kFormType_Container) return true;

	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_MerchantContainer);
	if (xData) {
		ExtraMerchantContainer* xContainer = (ExtraMerchantContainer*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraMerchantContainer, 0);
		if (xContainer) {
			*refResult = xContainer->containerRef->refID;
			xContainer->containerRef = objectRef;			
		}
	}
	return true;
}

static bool Cmd_SetPCAMurderer_Execute(COMMAND_ARGS)
{
	*result = (*g_thePlayer)->isAMurderer != 0;

	int	value = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &value)) return true;

	(*g_thePlayer)->isAMurderer = value ? 1 : 0;

	return true;
}


static bool Cmd_CellHasWater_Execute(COMMAND_ARGS)
{
	*result = 0;

	if(!thisObj) return true;
	TESObjectCELL* cell = thisObj->parentCell;
	if (!cell) return true;

	*result = cell->HasWater() ? 1 : 0;

	return true;
}

static bool Cmd_GetCellWaterHeight_Execute(COMMAND_ARGS)
{
	*result = 0;
	if(!thisObj) return true;
	TESObjectCELL* cell = thisObj->parentCell;
	if (!cell) return true;

	if (!cell->HasWater()) return true;

	float waterHeight = cell->GetWaterHeight();
	*result = waterHeight;
	return true;
}

static bool Cmd_IsUnderWater_Execute(COMMAND_ARGS)
{
	*result = 0;
	if (!thisObj) return true;
	TESObjectCELL* cell = thisObj->parentCell;
	if (!cell) return true;

	if (!cell->HasWater()) return true;

	float waterHeight = cell->GetWaterHeight();
	float bottom = thisObj->posZ;
	bool bIsSwimming = false;
	bool bIsSneaking = false;
	MobileObject* mobile = (MobileObject*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_MobileObject, 0);
	if (mobile && mobile->process) {
		UInt32 moveFlags = mobile->process->GetMovementFlags();
		bIsSwimming = (moveFlags & BaseProcess::kMovementFlag_Swimming) != 0;
	}

	float scale = thisObj->GetScale();
	float standingHeight = scale * 128.0;	// base height = 128
	float factor = (bIsSwimming) ? .9 : .73;
	float height = standingHeight * factor;
	float top = bottom + height;
	*result = (top < waterHeight) ? 1 : 0;

	return true;
}

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

static bool Cmd_CalcLeveledItem_Execute(COMMAND_ARGS)
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

	TESForm* item = levList->CalcElement(level, (useChanceNone ? true : false), levelDiff);
	if (item)
		*refResult = item->refID;

	return true;
}

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
	if (xLock)
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

static bool Cmd_CopyEyes_Execute(COMMAND_ARGS)
{
	TESNPC* copyFrom = NULL;
	TESNPC* copyTo = NULL;
	*result = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &copyFrom, &copyTo))
		return true;

	if (!copyFrom)
		return true;

	if (!copyTo)
	{	
		if (!thisObj)
			return true;
		copyTo = (TESNPC*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
		if (!copyTo)
			return true;
	}

	copyTo->eyes = copyFrom->eyes;
	*result = 1;

	return true;
}

static bool Cmd_SetEyes_Execute(COMMAND_ARGS)
{
	TESForm* form = NULL;
	TESForm*  npcF = NULL;
	*result = 0;

	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &form, &npcF);
	if (!form)
		return true;

	TESEyes* eyes = (TESEyes*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESEyes, 0);

	if (!eyes)
		return true;
	else if (!npcF)
	{	
		if (!thisObj)
			return true;
		npcF = thisObj->baseForm;
	}

	TESNPC* npc = (TESNPC*)Oblivion_DynamicCast(npcF, 0, RTTI_TESForm, RTTI_TESNPC, 0);

	if (!npc)
		return true;

	npc->eyes = eyes;

	*result = 1;
	return true;
}

static bool Cmd_SetHair_Execute(COMMAND_ARGS)
{
	TESForm* form = NULL;
	TESForm*  npcF = NULL;
	*result = 0;

	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &form, &npcF);
	if (!form)
		return true;

	TESHair* hair = (TESHair*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESHair, 0);
	if (!hair)
		return true;
	else if (!npcF)
	{	
		if (!thisObj)
			return true;
		npcF = thisObj->baseForm;
	}

	TESNPC* npc = (TESNPC*)Oblivion_DynamicCast(npcF, 0, RTTI_TESForm, RTTI_TESNPC, 0);

	if (!npc)
		return true;
	npc->hair = hair;

	*result = 1;
	return true;
}

static bool Cmd_CopyHair_Execute(COMMAND_ARGS)
{
	TESNPC* copyFrom = NULL;
	TESNPC* copyTo = NULL;
	*result = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &copyFrom, &copyTo))
		return true;

	if (!copyFrom)
		return true;

	if (!copyTo)
	{	
		if (!thisObj)
			return true;
		copyTo = (TESNPC*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
		if (!copyTo)
			return true;
	}

	copyTo->hair = copyFrom->hair;
	copyTo->hairLength = copyFrom->hairLength;
	copyTo->hairColorRGB = copyFrom->hairColorRGB;
	*result = 1;

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
			return false;

		do
		{
			if (curX - pcCell->coords->x == cellDepth)
			{
				if (curY - pcCell->coords->y == cellDepth)
				{
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
		if (refr->baseForm->typeID == m_formType)
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
	const TESObjectCELL::ObjectListEntry* entry = NULL;
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
	{
//		PrintItemType(info->prev->refr->baseForm);
		return info->prev->refr;
	}
	else
	{
		scanScripts.erase(idx);
		return NULL;
	}
//	return info->prev ? info->prev->refr : NULL;
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

	while (CellScan(scriptObj, formType, cellDepth))
	{
		(*result)++;
	}

	return true;
}

#endif

CommandInfo kCommandInfo_GetActiveSpell =
{
	"GetPlayerSpell",
	"GetActiveSpell",
	0,
	"returns the base spell object for the player's active spell",
	0,
	0,
	NULL,
	HANDLER(Cmd_GetActiveSpell_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetActiveSpell =
{
	"SetActiveSpell",
	"sspl",
	0,
	"sets the active spell to the argument",
	0,
	1,
	kParams_OneSpellItem,
	HANDLER(Cmd_SetActiveSpell_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetParentCell =
{
	"GetParentCell",
	"gcel",
	0,
	"returns a ref to the cell the object is in",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetParentCell_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetParentWorldspace =
{
	"GetParentWorldspace",
	"",
	0,
	"returns a ref to the worldspace the object is in",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetParentWorldspace_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_HasSpell =
{
	"HasSpell",
	"hspl",
	0,
	"returns 1 if the actor has the spell",
	1,
	1,
	kParams_OneSpellItem,
	HANDLER(Cmd_HasSpell_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetPlayerSpellCount =
{
	"GetPlayerSpellCount",
	"GetSpellCount",
	0,
	"returns the number of spells in the player's spell list",
	0,
	0,
	NULL,
	HANDLER(Cmd_GetSpellCount_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNthPlayerSpell =
{
	"GetNthPlayerSpell",
	"GetNthSpell",
	0,
	"returns the spell at the specified slot in the list",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_GetNthSpell_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_RemoveAllSpells =
{
	"RemoveAllSpells",
	"RemSpells",
	0,
	"removes all of the spells from the reference",
	1,
	0,
	0,
	HANDLER(Cmd_RemoveAllSpells_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetClass =
{
	"GetClass",
	"gclass",
	0,
	"returns the ref to the class of the calling actor",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetClass_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_IsMajor[2] =
{
	{	"skill", kParamType_ActorValue, 0 },
	{	"class", kParamType_Class, 1 },
};


CommandInfo kCommandInfo_IsClassSkill =
{
	"IsClassSkill",
	"IsMajor",
	0,
	"returns 1 if the skill is a major skill of the class",
	0,
	2,
	kParams_IsMajor,
	HANDLER(Cmd_IsMajor_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsClassAttribute =
{
	"IsClassAttribute",
	"",
	0,
	"returns 1 if the attribute is part of the class",
	0,
	2,
	kParams_IsMajor,
	HANDLER(Cmd_IsClassAttribute_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


static ParamInfo kParams_ClassInfo[2] =
{
	{	"index", kParamType_Integer, 0 },
	{	"class", kParamType_Class, 1 },
};

CommandInfo kCommandInfo_GetClassAttribute =
{
	"GetClassAttribute",
	"GetAttrib",
	0,
	"returns a code for the specified index for a class attribute",
	0,
	2,
	kParams_ClassInfo,
	HANDLER(Cmd_GetClassAttribute_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetClassSkill =
{
	"GetClassSkill",
	"GetSkill",
	0,
	"returns a code for the specified index of the skills for the given class",
	0,
	2,
	kParams_ClassInfo,
	HANDLER(Cmd_GetClassSkill_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_OneOptionalClass[1] =
{
	{	"class", kParamType_Class, 1 },
};


CommandInfo kCommandInfo_GetClassSpecialization =
{
	"GetClassSpecialization",
	"GetSpec",
	0,
	"returns a code for the given classes specialization",
	0,
	1,
	kParams_OneOptionalClass,
	HANDLER(Cmd_GetClassSpecialization_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_ModActorValue2[2] = 
{
	{	"actor value", kParamType_ActorValue, 0 }, 
	{	"amount", kParamType_Integer, 0 },
};

CommandInfo kCommandInfo_ModActorValue2 =
{
	"ModActorValue2",
	"ModAV2",
	0,
	"Modify an actor's value in a non-permanent fashion. [player.modabv luck, -10]",
	1,
	2,
	kParams_ModActorValue2,
	HANDLER(Cmd_ModActorValue2_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsRefEssential =
{
	"IsRefEssential",
	"",
	0,
	"returns 1 if the calling reference is essential",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsRefEssential_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetRefEssential =
{
	"SetRefEssential",
	"",
	0,
	"changes the calling reference to esential",
	1,
	1,
	kParams_OneInt,
	HANDLER(Cmd_SetRefEssential_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetActorLightAmount =
{
	"GetActorLightAmount",
	"",
	0,
	"returns a float describing the amount of light falling on an actor, or 100 if the actor is not in high/medium-high process",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetActorLightAmount_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_OneOptionalActorBase[1] =
{
	{	"index", kParamType_ActorBase, 1 },
};

CommandInfo kCommandInfo_IsCreature =
{
	"IsCreature",
	"",
	0,
	"returns 1 if the passed actor base is a creature",
	0,
	1,
	kParams_OneOptionalActorBase,
	HANDLER(Cmd_IsCreature_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCreatureType =
{
	"GetCreatureType",
	"",
	0,
	"returns the type of the calling creature or passed refID",
	0,
	1,
	kParams_OneOptionalActorBase,
	HANDLER(Cmd_GetCreatureType_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCreatureCombatSkill =
{
	"GetCreatureCombatSkill",
	"GetCreatureCombat",
	0,
	"returns the combat skill of the calling creature or passed refID",
	0,
	1,
	kParams_OneOptionalActorBase,
	HANDLER(Cmd_GetCreatureCombatSkill_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCreatureMagicSkill =
{
	"GetCreatureMagicSkill",
	"GetCreatureMagic",
	0,
	"returns the magic skill of the calling creature or passed refID",
	0,
	1,
	kParams_OneOptionalActorBase,
	HANDLER(Cmd_GetCreatureMagicSkill_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCreatureStealthSkill =
{
	"GetCreatureStealthSkill",
	"GetCreatureStealth",
	0,
	"returns the stealth skill of the calling creature or passed refID",
	0,
	1,
	kParams_OneOptionalActorBase,
	HANDLER(Cmd_GetCreatureStealthSkill_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCreatureBaseScale =
{
	"GetCreatureBaseScale",
	"GetCreatureScale",
	0,
	"returns the base scale of the calling creature or passed refID",
	0,
	1,
	kParams_OneOptionalActorBase,
	HANDLER(Cmd_GetCreatureBaseScale_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCreatureReach =
{
	"GetCreatureReach",
	"",
	0,
	"returns the reach of the calling creature or passed refID",
	0,
	1,
	kParams_OneOptionalActorBase,
	HANDLER(Cmd_GetCreatureReach_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCreatureSoulLevel =
{
	"GetCreatureSoulLevel",
	"",
	0,
	"returns the soul level of the calling creature or passed refID",
	0,
	1,
	kParams_OneOptionalActorBase,
	HANDLER(Cmd_GetCreatureSoulLevel_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


CommandInfo kCommandInfo_IsThirdPerson =
{
	"IsThirdPerson",
	"",
	0,
	"returns if the player is using a third-person camera",
	0,
	0,
	NULL,
	HANDLER(Cmd_IsThirdPerson_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetPlayerSkillUse =
{
	"GetPlayerSkillUse",
	"",
	0,
	"returns the player's skills",
	0,
	1,
	kParams_OneActorValue,
	HANDLER(Cmd_GetPlayerSkillUse_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


static ParamInfo kParams_SkillUseIncrement[2] =
{
	{	"skill", kParamType_ActorValue, 0 },
	{	"index", kParamType_Integer, 1 },
};

CommandInfo kCommandInfo_GetSkillUseIncrement =
{
	"GetSkillUseIncrement",
	"",
	0,
	"returns the skill experience use increment for the specified skill",
	0,
	2,
	kParams_SkillUseIncrement,
	HANDLER(Cmd_GetSkillUseIncrement_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_SetSkillUseIncrement[3] =
{
	{	"nuVal", kParamType_Float, 0 },
	{	"skill", kParamType_ActorValue, 0 },
	{	"index", kParamType_Integer, 1 },
};

CommandInfo kCommandInfo_SetSkillUseIncrement =
{
	"SetSkillUseIncrement",
	"",
	0,
	"sets the skill experience use increment for the specified skill",
	0,
	3,
	kParams_SetSkillUseIncrement,
	HANDLER(Cmd_SetSkillUseIncrement_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_IncrementPlayerSkillUse[3] =
{
	{	"skill", kParamType_ActorValue, 0 },
	{	"index", kParamType_Integer, 1 },
	{	"howManyTimes", kParamType_Float, 1},
};


CommandInfo kCommandInfo_IncrementPlayerSkillUse =
{
	"IncrementPlayerSkillUse",
	"",
	0,
	"increments the player's skill as if the appropriate action occured",
	0,
	3,
	kParams_IncrementPlayerSkillUse,
	HANDLER(Cmd_IncrementPlayerSkillUse_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_GetRaceAttribute[3] =
{
	{	"attribute", kParamType_ActorValue, 0 },
	{	"race", kParamType_Race, 1 },
	{	"sex", kParamType_Sex, 1},
};

static ParamInfo kParams_GetRaceAttributeC[3] =
{
	{	"which", kParamType_Integer, 0 },
	{	"race", kParamType_Race, 1 },
	{	"sex", kParamType_Sex, 1},
};

CommandInfo kCommandInfo_GetRaceAttribute =
{
	"GetRaceAttribute",
	"",
	0,
	"returns the specified attibute for the race",
	0,
	2,
	kParams_GetRaceAttribute,
	HANDLER(Cmd_GetRaceAttribute_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetRaceAttributeC =
{
	"GetRaceAttributeC",
	"",
	0,
	"returns the specified attibute for the race",
	0,
	2,
	kParams_GetRaceAttributeC,
	HANDLER(Cmd_GetRaceAttribute_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_GetRaceValue[2] =
{
	{	"attribute", kParamType_ActorValue, 0 },
	{	"race", kParamType_Race, 1 },
};

static ParamInfo kParams_GetRaceValueC[2] =
{
	{	"attribute", kParamType_Integer, 0 },
	{	"race", kParamType_Race, 1 },
};

CommandInfo kCommandInfo_GetRaceSkillBonus =
{
	"GetRaceSkillBonus",
	"",
	0,
	"returns the specified skill bonus for the race",
	0,
	2,
	kParams_GetRaceValue,
	HANDLER(Cmd_GetRaceSkillBonus_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetRaceSkillBonusC =
{
	"GetRaceSkillBonusC",
	"",
	0,
	"returns the specified skill bonus for the race",
	0,
	2,
	kParams_GetRaceValueC,
	HANDLER(Cmd_GetRaceSkillBonus_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsRaceBonusSkill =
{
	"IsRaceBonusSkill",
	"",
	0,
	"returns whether the specified skill has a bonus for the race",
	0,
	2,
	kParams_GetRaceValue,
	HANDLER(Cmd_IsRaceBonusSkill_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsRaceBonusSkillC =
{
	"IsRaceBonusSkillC",
	"",
	0,
	"returns whether the specified skill has a bonus for the race",
	0,
	2,
	kParams_GetRaceValueC,
	HANDLER(Cmd_IsRaceBonusSkill_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNthRaceBonusSkill =
{
	"GetNthRaceBonusSkill",
	"",
	0,
	"returns the nth bonus skill of the race",
	0,
	2,
	kParams_GetRaceValue,
	HANDLER(Cmd_GetNthRaceBonusSkill_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


CommandInfo kCommandInfo_GetMerchantContainer =
{
	"GetMerchantContainer",
	"",
	0,
	"returns the merchant container of the reference",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetMerchantContainer_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_OneObjectRef[1] =
{
	{	"container", kParamType_ObjectRef, 0 },
};

CommandInfo kCommandInfo_SetMerchantContainer =
{
	"SetMerchantContainer",
	"",
	0,
	"sets the merchant container of the reference",
	1,
	1,
	kParams_OneObjectRef,
	HANDLER(Cmd_SetMerchantContainer_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetPCAMurderer =
{
	"SetPCAMurderer",
	"",
	0,
	"sets whether or not the PC has ever killed an NPC",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_SetPCAMurderer_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_ParentCellHasWater =
{
	"ParentCellHasWater",
	"HasWater",
	0,
	"returns whether the cell has water",
	1,
	0,
	NULL,
	HANDLER(Cmd_CellHasWater_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetParentCellWaterHeight =
{
	"GetParentCellWaterHeight",
	"GetWaterHeight",
	0,
	"returns the height of the cell's water",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetCellWaterHeight_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsUnderWater =
{
	"IsUnderWater",
	"",
	0,
	"returns 1 if the object is under the cell's water level",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsUnderWater_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

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

//In progress - need to decode faction list on TESActorBase
/*CommandInfo kCommandInfo_IsLegalToActivateFor =
{
	"IsLegalToActivateFor",
	"IsLegalFor",
	0,
	"returns true if the calling reference can be legally activated by the passed actor reference",
	1,
	1,
	kParams_OneActor,
	HANDLER(Cmd_IsLegalToActivateFor_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};
*/

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

static ParamInfo kParams_CopyNPCBodyData[2] = 
{
	{	"copy from",	kParamType_NPC,		0	},
	{	"copy to",		kParamType_NPC,		1	},
};


CommandInfo kCommandInfo_CopyHair =
{
	"CopyHair",
	"",
	0,
	"copies hair from source to target",
	0,
	2,
	kParams_CopyNPCBodyData,
	HANDLER(Cmd_CopyHair_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


CommandInfo kCommandInfo_CopyEyes =
{
	"CopyEyes",
	"",
	0,
	"copies eyes from source to target",
	0,
	2,
	kParams_CopyNPCBodyData,
	HANDLER(Cmd_CopyEyes_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_SetNPCBodyData[2] =
{
	{	"body data",	kParamType_InventoryObject,	0	},
	{	"base NPC",		kParamType_NPC,				1	},
};

CommandInfo kCommandInfo_SetHair =
{
	"SetHair",
	"",
	0,
	"sets the hair of the NPC",
	0,
	2,
	kParams_SetNPCBodyData,
	HANDLER(Cmd_SetHair_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetEyes =
{
	"SetEyes",
	"",
	0,
	"sets the hair of the NPC",
	0,
	2,
	kParams_SetNPCBodyData,
	HANDLER(Cmd_SetEyes_Execute),
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
