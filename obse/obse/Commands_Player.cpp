#include "Commands_Player.h"
#include "GameObjects.h"
#include "GameExtraData.h"
#include "GameAPI.h"
#include "GameForms.h"
#include "Utilities.h"
#include "ParamInfos.h"


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

static bool Cmd_SetMerchantContainer_Exectute(COMMAND_ARGS)
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
	HANDLER(Cmd_SetMerchantContainer_Exectute),
	Cmd_Default_Parse,
	NULL,
	0
};
