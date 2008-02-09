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


static bool Cmd_GetActorValueC_Execute(COMMAND_ARGS)
{
	UInt32	type = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &type)) return true;

	if(thisObj && thisObj->IsActor())
	{
		Actor	* actor = (Actor *)thisObj;
		*result = actor->GetActorValue(type);
	}

	return true;
}

static bool Cmd_GetBaseActorValueC_Execute(COMMAND_ARGS)
{
	UInt32	type = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &type)) return true;

	if(thisObj && thisObj->IsActor())
	{
		Actor	* actor = (Actor *)thisObj;
		*result = actor->GetBaseActorValue(type);
	}

	return true;
}

static bool Cmd_SetActorValueC_Execute(COMMAND_ARGS)
{
	UInt32	type = 0;
	int		amount = 0;

	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &type, &amount)) return true;

	if(thisObj && thisObj->IsActor())
	{
		Actor	* actor = (Actor *)thisObj;
		actor->SetActorValue(type, amount);
	}

	return true;
}


static bool Cmd_ModActorValue2_Execute(COMMAND_ARGS)
{
	UInt32	type = 0;
	int		amount = 0;

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


class PrintAnimation
{
public:
	PrintAnimation() : index(0) {}
	UInt32 index;

	bool Accept(char* animName)
	{
		Console_Print("%d> %s", index, animName);
		_MESSAGE("%d> %s", index, animName);
		++index;
		return true;
	}
};

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
//		AnimationVisitor visitor(&creature->animation.data);
//		UInt32 animationCount = visitor.Count();
//		Console_Print("%s has %d animations", actorBase->GetEditorName(), animationCount);
//		PrintAnimation printer;
//		visitor.Visit(printer);
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
	kCreature_Walks,
	kCreature_Swims,
	kCreature_Flies,
	kCreature_Biped,
	kCreature_WeaponAndShield,
	kCreature_NoHead,
	kCreature_NoLArm,
	kCreature_NoRArm,
	kCreature_NoCombatInWater,
	kCreature_NoMovement,
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
				if (creature) {
					*result = creature->soulLevel;
				} else {
					TESNPC* npc = (TESNPC*)Oblivion_DynamicCast(actorBase, 0, RTTI_TESActorBase, RTTI_TESNPC, 0);
					if (npc) *result = 5;
				}
				break;
			}

		case kCreature_Walks:
			{
				*result = (creature && creature->actorBaseData.CreatureWalks()) ? 1 : 0;
				break;
			}

		case kCreature_Swims:
			{
				*result = (creature && creature->actorBaseData.CreatureSwims()) ? 1 : 0;
				break;
			}
		case kCreature_Flies:
			{
				*result = (creature && creature->actorBaseData.CreatureFlies()) ? 1 : 0;
				break;
			}
		case kCreature_Biped:
			{
				*result = (creature && creature->actorBaseData.IsCreatureBiped()) ? 1 : 0;
				break;
			}
		case kCreature_WeaponAndShield:
			{
				*result = (creature && creature->actorBaseData.CreatureHasWeaponAndShield()) ? 1 : 0;
				break;
			}
		case kCreature_NoHead:
			{
				*result = (creature && creature->actorBaseData.CreatureHasNoHead()) ? 1 : 0;
				break;
			}
		case kCreature_NoLArm:
			{
				*result = (creature && creature->actorBaseData.CreatureHasNoLeftArm()) ? 1 : 0;
				break;
			}
		case kCreature_NoRArm:
			{
				*result = (creature && creature->actorBaseData.CreatureHasNoRightArm()) ? 1 : 0;
				break;
			}
		case kCreature_NoCombatInWater:
			{
				*result = (creature && creature->actorBaseData.CreatureNoCombatInWater()) ? 1 : 0;
				break;
			}
		case kCreature_NoMovement:
			{
				*result = (creature && creature->actorBaseData.CreatureHasNoMovement()) ? 1 : 0;
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

static bool Cmd_GetCreatureWalks_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_Walks);
}

static bool Cmd_GetCreatureSwims_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_Swims);
}

static bool Cmd_GetCreatureFlies_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_Flies);
}

static bool Cmd_IsCreatureBiped_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_Biped);
}

static bool Cmd_CreatureUsesWeaponAndShield_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_WeaponAndShield);
}

static bool Cmd_CreatureHasNoHead_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_NoHead);
}

static bool Cmd_CreatureHasNoLeftArm_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_NoLArm);
}

static bool Cmd_CreatureHasNoRightArm_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_NoRArm);
}

static bool Cmd_CreatureNoCombatInWater_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_NoCombatInWater);
}

static bool Cmd_CreatureHasNoMovement_Execute(COMMAND_ARGS)
{
	return GetCreatureValue(PASS_COMMAND_ARGS, kCreature_NoMovement);
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
	kRace_NthSpell,
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
		case kRace_NthSpell:
			{
				UInt32* refResult = (UInt32*)result;
				SpellListVisitor visitor(&race->spells.spellList);
				TESForm* form = visitor.GetNthInfo(intVal);
				SpellItem* spell = (SpellItem*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_SpellItem, 0);
				if (spell) {
					*refResult = spell->refID;
				}
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

static bool Cmd_GetRaceSpellCount_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESRace* race = 0;
	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &race);
	if (!race) {
		if (!thisObj) return true;
		TESNPC* npc = (TESNPC*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
		if (!npc) return true;
		race = npc->race.race;
		if (!race) return true;
	}
	SpellListVisitor visitor(&race->spells.spellList);
	*result = visitor.Count();
	return true;
}

static bool Cmd_GetNthRaceSpell_Execute(COMMAND_ARGS)
{
	return GetRaceValue_Execute(PASS_COMMAND_ARGS, kRace_NthSpell);
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

static bool Cmd_GetNumFollowers_Execute(COMMAND_ARGS)
{
	*result = 0;

	if (!thisObj)
		return true;

	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_Follower);
	if (!xData)
		return true;
	
	ExtraFollower* xFollower = (ExtraFollower*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraFollower, 0);
	if (xFollower)
		*result = ExtraFollowerVisitor(xFollower->followers).Count();

	return true;
}

static bool Cmd_GetNthFollower_Execute(COMMAND_ARGS)
{
	UInt32 idx = 0;
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &idx))
		return true;

	else if (!thisObj)
		return true;

	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_Follower);
	if (!xData)
		return true;
	
	ExtraFollower* xFollowers = (ExtraFollower*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraFollower, 0);
	if (xFollowers)
	{
		Character* follower = ExtraFollowerVisitor(xFollowers->followers).GetNthInfo(idx);
		if (follower)
			*refResult = follower->refID;
	}

	return true;
}

static bool Cmd_GetCellMusicType_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESObjectCELL* curCell = (*g_thePlayer)->parentCell;

	BSExtraData* xData = curCell->extraData.GetByType(kExtraData_CellMusicType);
	if (xData)
	{
		ExtraCellMusicType* xMusic = (ExtraCellMusicType*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraCellMusicType, 0);
		if (xMusic)
			*result = xMusic->musicType;
	}

	return true;
}

static TESActorBase* ExtractActorBase(COMMAND_ARGS)
{
	TESActorBase* actorBase = NULL;
	TESForm* actorForm = NULL;

	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &actorForm);
	if (!actorForm)
		if (thisObj)
			actorForm = thisObj->baseForm;

	if (actorForm)
	{
		actorBase = (TESActorBase*)Oblivion_DynamicCast(actorForm, 0, RTTI_TESForm, RTTI_TESActorBase, 0);
	}

	return actorBase;
}

static bool Cmd_IsActorRespawning_Execute(COMMAND_ARGS)
{
	*result = 0;

	TESActorBase* actorBase = ExtractActorBase(PASS_COMMAND_ARGS);
	if (actorBase && actorBase->actorBaseData.IsRespawning())
		*result = 1;

	return true;
}

static bool Cmd_IsPCLevelOffset_Execute(COMMAND_ARGS)
{
	*result = 0;

	TESActorBase* actorBase = ExtractActorBase(PASS_COMMAND_ARGS);
	if (actorBase && actorBase->actorBaseData.IsPCLevelOffset())
		*result = 1;

	return true;
}

static bool Cmd_GetActorMinLevel_Execute(COMMAND_ARGS)
{
	*result = 0;

	TESActorBase* actorBase = ExtractActorBase(PASS_COMMAND_ARGS);
	if (actorBase && actorBase->actorBaseData.IsPCLevelOffset())
	{
		*result = actorBase->actorBaseData.minLevel;
	}

	return true;
}

static bool Cmd_GetActorMaxLevel_Execute(COMMAND_ARGS)
{
	*result = 0;

	TESActorBase* actorBase = ExtractActorBase(PASS_COMMAND_ARGS);
	if (actorBase && actorBase->actorBaseData.IsPCLevelOffset())
	{
		*result = actorBase->actorBaseData.maxLevel;
	}

	return true;
}

static bool Cmd_HasLowLevelProcessing_Execute(COMMAND_ARGS)
{
	*result = 0;

	TESActorBase* actorBase = ExtractActorBase(PASS_COMMAND_ARGS);
	if (actorBase && actorBase->actorBaseData.HasLowLevelProcessing())
		*result = 1;

	return true;
}

static bool Cmd_IsSummonable_Execute(COMMAND_ARGS)
{
	*result = 0;

	TESActorBase* actorBase = ExtractActorBase(PASS_COMMAND_ARGS);
	if (actorBase && actorBase->actorBaseData.IsSummonable())
		*result = 1;

	return true;
}

static bool Cmd_HasNoPersuasion_Execute(COMMAND_ARGS)
{
	*result = 0;

	TESActorBase* actorBase = ExtractActorBase(PASS_COMMAND_ARGS);
	if (actorBase && actorBase->actorBaseData.HasNoPersuasion())
		*result = 1;

	return true;
}

static bool Cmd_CanCorpseCheck_Execute(COMMAND_ARGS)
{
	*result = 0;

	TESActorBase* actorBase = ExtractActorBase(PASS_COMMAND_ARGS);
	if (actorBase && actorBase->actorBaseData.CanCorpseCheck())
		*result = 1;

	return true;
}

static bool Cmd_IsFemale_Execute(COMMAND_ARGS)
{
	*result = 0;

	TESActorBase* actorBase = ExtractActorBase(PASS_COMMAND_ARGS);
	if (actorBase && actorBase->actorBaseData.IsFemale())
		*result = 1;

	return true;
}

static TESActorBase* ExtractSetActorBase(COMMAND_ARGS, UInt32* bMod)
{
	TESActorBase* actorBase = NULL;
	TESForm* actorForm = NULL;
	*bMod = 0;

	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, bMod, &actorForm);
	if (!actorForm)
		if (thisObj)
			actorForm = thisObj->baseForm;

	if (actorForm)
	{
		actorBase = (TESActorBase*)Oblivion_DynamicCast(actorForm, 0, RTTI_TESForm, RTTI_TESActorBase, 0);
	}
	return actorBase;
}

static bool Cmd_SetFemale_Execute(COMMAND_ARGS)
{
	UInt32 bMod = 0;
	*result = 0;
	TESActorBase* actorBase = ExtractSetActorBase(PASS_COMMAND_ARGS, &bMod);
	if (actorBase)
	{
		actorBase->actorBaseData.SetFemale(bMod ? true : false);
		actorBase->MarkAsModified(TESActorBaseData::kModified_ActorBaseFlags);
		*result = 1;
	}

	return true;
}

static bool Cmd_SetActorRespawns_Execute(COMMAND_ARGS)
{
	UInt32 bMod = 0;
	*result = 0;
	TESActorBase* actorBase = ExtractSetActorBase(PASS_COMMAND_ARGS, &bMod);
	if (actorBase)
	{
		actorBase->actorBaseData.SetRespawning(bMod ? true : false);
		actorBase->MarkAsModified(TESActorBaseData::kModified_ActorBaseFlags);
		*result = 1;
	}

	return true;
}

static bool Cmd_SetLowLevelProcessing_Execute(COMMAND_ARGS)
{
	UInt32 bMod = 0;
	*result = 0;
	TESActorBase* actorBase = ExtractSetActorBase(PASS_COMMAND_ARGS, &bMod);
	if (actorBase)
	{
		actorBase->actorBaseData.SetLowLevelProcessing(bMod ? true : false);
		actorBase->MarkAsModified(TESActorBaseData::kModified_ActorBaseFlags);
		*result = 1;
	}

	return true;
}

static bool Cmd_SetNoPersuasion_Execute(COMMAND_ARGS)
{
	UInt32 bMod = 0;
	*result = 0;
	TESActorBase* actorBase = ExtractSetActorBase(PASS_COMMAND_ARGS, &bMod);
	if (actorBase)
	{
		actorBase->actorBaseData.SetNoPersuasion(bMod ? true : false);
		actorBase->MarkAsModified(TESActorBaseData::kModified_ActorBaseFlags);
		*result = 1;
	}

	return true;
}

static bool Cmd_SetSummonable_Execute(COMMAND_ARGS)
{
	UInt32 bMod = 0;
	*result = 0;
	TESActorBase* actorBase = ExtractSetActorBase(PASS_COMMAND_ARGS, &bMod);
	if (actorBase)
	{
		actorBase->actorBaseData.SetSummonable(bMod ? true : false);
		actorBase->MarkAsModified(TESActorBaseData::kModified_ActorBaseFlags);
		*result = 1;
	}

	return true;
}

static bool Cmd_SetCanCorpseCheck_Execute(COMMAND_ARGS)
{
	UInt32 bMod = 0;
	*result = 0;
	TESActorBase* actorBase = ExtractSetActorBase(PASS_COMMAND_ARGS, &bMod);
	if (actorBase)
	{
		actorBase->actorBaseData.SetCanCorpseCheck(bMod ? true : false);
		actorBase->MarkAsModified(TESActorBaseData::kModified_ActorBaseFlags);
		*result = 1;
	}

	return true;
}

static bool Cmd_SetPCLevelOffset_Execute(COMMAND_ARGS)
{
	UInt32 minLevel = -1;
	UInt32 maxLevel = -1;
	UInt32 bMod = 0;
	TESActorBase* actorBase = NULL;
	TESForm* actorForm = NULL;

	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &bMod, &minLevel, &maxLevel, &actorForm);
	if (!actorForm)
		if (thisObj)
			actorForm = thisObj->baseForm;

	if (actorForm)
	{
		actorBase = (TESActorBase*)Oblivion_DynamicCast(actorForm, 0, RTTI_TESForm, RTTI_TESActorBase, 0);
	}

	if (actorBase)
	{
		actorBase->actorBaseData.SetPCLevelOffset((bMod ? true : false), minLevel, maxLevel);
		actorBase->MarkAsModified(TESActorBaseData::kModified_ActorBaseFlags);
	}

	return true;
}

static bool Cmd_GetPlayersLastRiddenHorse_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if ((*g_thePlayer)->lastRiddenHorse)
		*refResult = (*g_thePlayer)->lastRiddenHorse->refID;

	return true;
}

static bool Cmd_GetHorse_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (thisObj)
	{
		Character* actor = (Character*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_Character, 0);
		if (actor && actor->horseOrRider)
			*refResult = actor->horseOrRider->refID;
	}

	return true;
}

static bool Cmd_GetRider_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (thisObj)
	{
		Creature* horse = (Creature*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_Creature, 0);
		if (horse && horse->horseOrRider)
			*refResult = horse->horseOrRider->refID;
	}

	return true;
}

static bool Cmd_GetPlayersLastActivatedLoadDoor_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if ((*g_thePlayer)->lastActivatedLoadDoor)
		*refResult = (*g_thePlayer)->lastActivatedLoadDoor->refID;

	return true;
}

static bool Cmd_GetCreatureSoundBase_Execute(COMMAND_ARGS)
{
	TESActorBase* actorBase = 0;
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &actorBase))
		return true;

	if (!actorBase)
		if (thisObj)
			actorBase = (TESActorBase*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESActorBase, 0);

	TESCreature* crea = (TESCreature*)Oblivion_DynamicCast(actorBase, 0, RTTI_TESActorBase, RTTI_TESCreature, 0);
	if (crea && crea->soundBase)
		*refResult = crea->soundBase->refID;

	return true;
}

static bool Cmd_HasModel_Execute(COMMAND_ARGS)
{
	char nifPath[512];
	TESActorBase* actorBase = 0;
	*result = 0;

	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &nifPath, &actorBase))
		return true;

	if (!actorBase)
		if (thisObj)
			actorBase = (TESActorBase*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESActorBase, 0);

	TESCreature* crea = (TESCreature*)Oblivion_DynamicCast(actorBase, 0, RTTI_TESActorBase, RTTI_TESCreature, 0);
	if (crea && crea->modelList.FindNifPath(nifPath))
			*result = 1;


	return true;
}

static bool Cmd_GetRace_Execute(COMMAND_ARGS)
{
	TESNPC* npc = 0;
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &npc))
		return true;

	if (!npc)
	{
		if (thisObj && (thisObj->baseForm->typeID == kFormType_NPC))
			npc = (TESNPC*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
		else
			return true;
	}

	*refResult = npc->race.race->refID;
	return true;
}

//WIP
static bool Cmd_SetRace_Execute(COMMAND_ARGS)
{
	TESNPC* npc = NULL;
	TESForm* raceArg = NULL;

	if(!ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &raceArg, &npc))
		return true;

	if (!npc)
		if (thisObj)
			npc = (TESNPC*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);

	TESRace* newRace = (TESRace*)Oblivion_DynamicCast(raceArg, 0, RTTI_TESForm, RTTI_TESRace, 0);
	if (newRace && npc)
	{
		npc->race.race = newRace;
		Console_Print("Changed Race");
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
	"ModActorValue2", "ModAV2",
	0,
	"Modify an actor's value in a non-permanent fashion. [player.modabv luck, -10]",
	1, 2, kParams_ModActorValue2, 
	HANDLER(Cmd_ModActorValue2_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetActorValueC =
{
	"GetActorValueC", "GetAVC",
	0,
	"Return an actor's value by code",
	1, 1, kParams_OneInt, 
	HANDLER(Cmd_GetActorValueC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetBaseActorValueC =
{
	"GetBaseActorValueC", "GetBAVC",
	0,
	"Return an actor's base value by code",
	1, 1, kParams_OneInt, 
	HANDLER(Cmd_GetBaseActorValueC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_SetModActorValueC[2] = 
{
	{	"actor value", kParamType_Integer, 0 }, 
	{	"amount", kParamType_Integer, 0 },
};

CommandInfo kCommandInfo_SetActorValueC =
{
	"SetActorValueC", "SetAVC",
	0,
	"Set an actor's value by code. ",
	1, 2, kParams_SetModActorValueC, 
	HANDLER(Cmd_SetActorValueC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_ModActorValueC =
{
	"ModActorValueC", "ModAVC",
	0,
	"Modify an actor's value in a non-permanent fashion.",
	1, 2, kParams_SetModActorValueC, 
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
	"GetCreatureSoulLevel", "GetActorSoulLevel",
	0,
	"returns the soul level of the calling actor or passed refID",
	0, 1, kParams_OneOptionalActorBase,
	HANDLER(Cmd_GetCreatureSoulLevel_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCreatureWalks =
{
	"GetCreatureWalks", "CreatureWalks",
	0,
	"returns 1 if the calling creature or creature refID has the Walk flag",
	0, 1, kParams_OneOptionalActorBase,
	HANDLER(Cmd_GetCreatureWalks_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCreatureFlies =
{
	"GetCreatureFlies", "CreatureFlies",
	0,
	"returns 1 if the calling creature or creature refID has the Flies flag",
	0, 1, kParams_OneOptionalActorBase,
	HANDLER(Cmd_GetCreatureFlies_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCreatureSwims =
{
	"GetCreatureSwims", "CreatureSwims",
	0,
	"returns 1 if the calling creature or creature refID has the Swims flag",
	0, 1, kParams_OneOptionalActorBase,
	HANDLER(Cmd_GetCreatureSwims_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsCreatureBiped =
{
	"IsCreatureBiped", "IsBiped",
	0,
	"returns 1 if the calling creature or creature refID is marked as a biped",
	0, 1, kParams_OneOptionalActorBase,
	HANDLER(Cmd_IsCreatureBiped_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_CreatureHasNoMovement =
{
	"CreatureHasNoMovement", "",
	0,
	"returns 1 if the calling creature or creature refID has the None movement flag",
	0, 1, kParams_OneOptionalActorBase,
	HANDLER(Cmd_CreatureHasNoMovement_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_CreatureHasNoHead =
{
	"CreatureHasNoHead", "",
	0,
	"returns 1 if the calling creature or creature refID has the NoHead flag",
	0, 1, kParams_OneOptionalActorBase,
	HANDLER(Cmd_CreatureHasNoHead_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_CreatureHasNoLeftArm =
{
	"CreatureHasNoLeftArm", "",
	0,
	"returns 1 if the calling creature or creature refID has the NoLeftArm flag",
	0, 1, kParams_OneOptionalActorBase,
	HANDLER(Cmd_CreatureHasNoLeftArm_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_CreatureHasNoRightArm =
{
	"CreatureHasNoRightArm", "",
	0,
	"returns 1 if the calling creature or creature refID has the NoRightArm flag",
	0, 1, kParams_OneOptionalActorBase,
	HANDLER(Cmd_CreatureHasNoRightArm_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_CreatureNoCombatInWater =
{
	"CreatureNoCombatInWater", "",
	0,
	"returns 1 if the calling creature or creature refID has the NoCombatInWater flag",
	0, 1, kParams_OneOptionalActorBase,
	HANDLER(Cmd_CreatureNoCombatInWater_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_CreatureUsesWeaponAndShield =
{
	"CreatureUsesWeaponAndShield", "",
	0,
	"returns 1 if the calling creature or creature refID has the WeaponAndShield flag",
	0, 1, kParams_OneOptionalActorBase,
	HANDLER(Cmd_CreatureUsesWeaponAndShield_Execute),
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

static ParamInfo kParams_OneOptionalRace[1] =
{
	{	"race", kParamType_Race, 1 },
};


CommandInfo kCommandInfo_GetRaceSpellCount =
{
	"GetRaceSpellCount",
	"",
	0,
	"returns the number of spells provided by the race",
	0,
	1,
	kParams_OneOptionalRace,
	HANDLER(Cmd_GetRaceSpellCount_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNthRaceSpell =
{
	"GetNthRaceSpell",
	"",
	0,
	"returns the nth spell of the race",
	0,
	2,
	kParams_GetRaceValueC,
	HANDLER(Cmd_GetNthRaceSpell_Execute),
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

CommandInfo kCommandInfo_GetNumFollowers =
{
	"GetNumFollowers", "", 0,
	"returns the number of characters following the calling actor",
	1, 0, NULL,
	HANDLER(Cmd_GetNumFollowers_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNthFollower =
{
	"GetNthFollower", "", 0,
	"returns the nth actor following the calling actor",
	1,
	1,
	kParams_OneInt,
	HANDLER(Cmd_GetNthFollower_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCellMusicType =
{
	"GetCellMusicType", "GetMusicType", 0,
	"returns the music type associated with the player's current cell",
	0, 0, NULL,
	HANDLER(Cmd_GetCellMusicType_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_GetActorBaseFlag[1] =
{
	{	"base actor",	kParamType_ActorBase,	1	},
};

static ParamInfo kParams_SetActorBaseFlag[2] =
{
	{	"bool",			kParamType_Integer,		0	},
	{	"base actor",	kParamType_ActorBase,	1	},
};

static ParamInfo kParams_SetPCLevelOffset[4] =
{
	{	"bool",			kParamType_Integer,		0	},
	{	"min level",	kParamType_Integer,		1	},
	{	"max level",	kParamType_Integer,		1	},
	{	"base actor",	kParamType_ActorBase,	1	},
};

CommandInfo kCommandInfo_IsActorRespawning =
{
	"IsActorRespawning",
	"",
	0,
	"returns true if the actor respawns",
	0,
	1,
	kParams_GetActorBaseFlag,
	HANDLER(Cmd_IsActorRespawning_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsPCLevelOffset =
{
	"IsPCLevelOffset",
	"",
	0,
	"returns true if the actor's level is relative to the player's level",
	0,
	1,
	kParams_GetActorBaseFlag,
	HANDLER(Cmd_IsPCLevelOffset_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_HasLowLevelProcessing =
{
	"HasLowLevelProcessing",
	"HasLowLevProc",
	0,
	"returns true if the actor has low level processing",
	0,
	1,
	kParams_GetActorBaseFlag,
	HANDLER(Cmd_HasLowLevelProcessing_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsSummonable =
{
	"IsSummonable",
	"",
	0,
	"returns true if the actor is summonable",
	0,
	1,
	kParams_GetActorBaseFlag,
	HANDLER(Cmd_IsSummonable_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_HasNoPersuasion =
{
	"HasNoPersuasion",
	"",
	0,
	"returns true if the actor has no persuasion",
	0,
	1,
	kParams_GetActorBaseFlag,
	HANDLER(Cmd_HasNoPersuasion_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_CanCorpseCheck =
{
	"CanCorpseCheck",
	"",
	0,
	"returns true if the actor can corpse check",
	0,
	1,
	kParams_GetActorBaseFlag,
	HANDLER(Cmd_CanCorpseCheck_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsFemale =
{
	"IsFemale", "",
	0,
	"returns true if the actor is female",
	0,
	1,
	kParams_GetActorBaseFlag,
	HANDLER(Cmd_IsFemale_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetActorMinLevel =
{
	"GetActorMinLevel",
	"GetMinLevel",
	0,
	"returns the minimum level of the actor",
	0,
	1,
	kParams_GetActorBaseFlag,
	HANDLER(Cmd_GetActorMinLevel_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetActorMaxLevel =
{
	"GetActorMaxLevel",
	"GetMaxLevel",
	0,
	"returns the maximum level of the actor",
	0,
	1,
	kParams_GetActorBaseFlag,
	HANDLER(Cmd_GetActorMaxLevel_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetFemale =
{
	"SetFemale",
	"",
	0,
	"toggles female flag on the actor",
	0,
	2,
	kParams_SetActorBaseFlag,
	HANDLER(Cmd_SetFemale_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetActorRespawns =
{
	"SetActorRespawns",
	"",
	0,
	"toggles respawn flag on the actor",
	0,
	2,
	kParams_SetActorBaseFlag,
	HANDLER(Cmd_SetActorRespawns_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetLowLevelProcessing =
{
	"SetLowLevelProcessing",
	"SetLowLevProc",
	0,
	"toggles low level processing flag on the actor",
	0,
	2,
	kParams_SetActorBaseFlag,
	HANDLER(Cmd_SetLowLevelProcessing_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetSummonable =
{
	"SetSummonable",
	"",
	0,
	"toggles summonable flag on the actor",
	0,
	2,
	kParams_SetActorBaseFlag,
	HANDLER(Cmd_SetSummonable_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetNoPersuasion =
{
	"SetNoPersuasion",
	"",
	0,
	"toggles no persuasion flag on the actor",
	0,
	2,
	kParams_SetActorBaseFlag,
	HANDLER(Cmd_SetNoPersuasion_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetCanCorpseCheck =
{
	"SetCanCorpseCheck",
	"SetCorpseCheck",
	0,
	"toggles can corpse check flag on the actor",
	0,
	2,
	kParams_SetActorBaseFlag,
	HANDLER(Cmd_SetCanCorpseCheck_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetPCLevelOffset =
{
	"SetPCLevelOffset",
	"",
	0,
	"toggles PC level offset flag and optionally sets min/max levels",
	0,
	4,
	kParams_SetPCLevelOffset,
	HANDLER(Cmd_SetPCLevelOffset_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetHorse =
{
	"GetHorse", "",
	0,
	"returns a reference to the horse currently ridden by the calling actor",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetHorse_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetRider =
{
	"GetRider", "",
	0,
	"returns a reference to the actor currently riding the calling horse",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetRider_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetPlayersLastRiddenHorse =
{
	"GetPlayersLastRiddenHorse", "GetPCLastHorse",
	0,
	"returns the last horse ridden by the player",
	0,
	0,
	NULL,
	HANDLER(Cmd_GetPlayersLastRiddenHorse_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetPlayersLastActivatedLoadDoor =
{
	"GetPlayersLastActivatedLoadDoor", "GetPCLastLoadDoor",
	0,
	"returns the last load door activated by the player",
	0,
	0,
	NULL,
	HANDLER(Cmd_GetPlayersLastActivatedLoadDoor_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetCreatureSoundBase =
{
	"GetCreatureSoundBase", "",
	0,
	"returns the creature from which the specified creature's sounds are derived",
	0,
	1,
	kParams_OneOptionalActorBase,
	HANDLER(Cmd_GetCreatureSoundBase_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_OneStringOneOptionalActorBase[2] =
{
	{	"model path",	kParamType_String,		0	},
	{	"creature",		kParamType_ActorBase,	1	},
};

CommandInfo kCommandInfo_HasModel =
{
	"HasModel",
	"",
	0,
	"returns 1 if the creature has the specified model path",
	0,
	2,
	kParams_OneStringOneOptionalActorBase,
	HANDLER(Cmd_HasModel_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_OneNPC[1] =
{
	{	"NPC",	kParamType_NPC,	1	},
};

CommandInfo kCommandInfo_GetRace =
{
	"GetRace",
	"",
	0,
	"returns the race of the specified NPC",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_GetRace_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_SetRace[2] =
{
	{	"race",	kParamType_InventoryObject,	0	},
	{	"NPC",	kParamType_NPC,				1	},
};

CommandInfo kCommandInfo_SetRace =
{
	"SetRace",
	"",
	0,
	"sets the race of the specified NPC",
	0,
	2,
	kParams_SetRace,
	HANDLER(Cmd_SetRace_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

