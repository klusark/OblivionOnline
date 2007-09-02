#include "Commands_Magic.h"
#include "ParamInfos.h"
#include "GameAPI.h"
#include "GameForms.h"
#include "GameObjects.h"
#include "Utilities.h"


#if OBLIVION

class EffectVisitor
{
public:
	EffectVisitor() {}
	virtual ~EffectVisitor() {}

	// visit each 
	virtual bool Accept(EffectItem* effectItem) = 0;
};

class EffectCounter : public EffectVisitor
{
	UInt32 count;
public:
	EffectCounter() : count(0) {}
	~EffectCounter() {}

	bool Accept(EffectItem* effectItem) {
		if (effectItem) {
			count++;
		}
		return true;	// always continue
	}

	UInt32 Count() { return count; }
};

class EffectFinder : public EffectVisitor
{
	UInt32 which;
	UInt32 count;
	EffectItem* found;
public:
	EffectFinder(UInt32 toFind) : which(toFind), count(0), found(NULL) {}
	~EffectFinder() {}

	bool Accept(EffectItem* effectItem) {
		if (which == count) {
			found = effectItem;
			return false;
		} else {
			count++;
			return true;
		}
	}
	EffectItem* Found() { return found; }
};

static bool VisitEffects(MagicItem* magicItem, EffectVisitor& visitor) 
{
	UInt32 count = 0;
	if (magicItem) {
		EffectItemList* spellEffectList = &(magicItem->list);
		if (spellEffectList) {
			EffectItemList::Entry* entry = &(spellEffectList->effectList);
			bool bContinue = true;
			while (entry && bContinue) {
				bContinue = visitor.Accept(entry->effectItem);				
				entry = entry->next;
			}
			return true;
		}
	}
	return false;
};


enum {
	kMagic_ItemType = 0,
	kMagic_EffectCount,
	kMagic_IsAutoCalc,
	kMagic_EffectCode = 50,
	kMagic_EffectMagnitude,
	kMagic_EffectArea,
	kMagic_EffectDuration,
	kMagic_EffectRange,
	kMagic_EffectActorValue,
	kMagic_EffectIsScripted,
	kMagic_EffectScript = 75,
	kMagic_EffectScriptVisualEffect,
	kMagic_EffectScriptSchool,
	kMagic_EffectScriptHostile,
	kMagic_EffectScriptName,
	kMagic_SpellType = 100,
	kMagic_SpellMagickaCost,
	kMagic_SpellMasteryLevel,
	kMagic_SpellSchool,
	kMagic_SpellExplodes,
	kMagic_EnchantmentType = 150,
	kMagic_EnchantmentCharge,
	kMagic_EnchantmentCost,
};

static bool GetMagicItemType(MagicItem* magicItem, double* result)
{
	if (!magicItem || !result) return true;
	*result = 0;
	if (Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_SpellItem, 0)) {
		*result = 1;
	} else if (Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_EnchantmentItem, 0)) {
		*result = 2;
	} else if (Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_AlchemyItem, 0)) {
		*result = 3;
	} else if (Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_IngredientItem, 0)) {
		* result = 4;
	}
	return true;
}

static bool GetMagicItemEffectCount(MagicItem* magicItem, double* result)
{
	if (!magicItem || !result) return true;
	EffectCounter counter;
	if (VisitEffects(magicItem, counter)) {
		*result = counter.Count();
		return true;
	}
	return true;
}

static bool GetEffectItemValue(EffectItem* effect, UInt32 valueType, double* result)
{
	if (!effect || !result) return true;

	switch(valueType) {
		case kMagic_EffectCode:
			{
				*result = effect->effectCode;
				break;
			}
		case kMagic_EffectMagnitude:	
			{
				*result = effect->magnitude;
				break;
			}
		case kMagic_EffectArea:			
			{
				*result = effect->area;
				break;
			}
		case kMagic_EffectDuration:		
			{
				*result = effect->duration;
				break;
			}
		case kMagic_EffectRange:		
			{
				*result = effect->range;
				break;
			}
		case kMagic_EffectActorValue:
			{
				*result = effect->GetActorValue();
				break;
			}
		case kMagic_EffectIsScripted:
			{
				*result = (effect->IsScriptedEffect()) ? 1 : 0;
				break;
			}
		case kMagic_EffectScript:
			{
				UInt32* refResult = (UInt32*)result;
				*refResult = effect->ScriptEffectRefId();
				break;
			}

		case kMagic_EffectScriptVisualEffect:
			{
				*result = effect->ScriptEffectVisualEffectCode();
				break;
			}
		case kMagic_EffectScriptSchool:
			{
				*result = effect->ScriptEffectSchool();
				break;
			}
		case kMagic_EffectScriptHostile:
			{
				*result = (effect->IsScriptEffectHostile()) ? 1 : 0;
				break;
			}

		default:
			break;
	}
	return true;
}

static bool GetSpellItemValue(SpellItem* spell, UInt32 valueType, double* result, TESForm* form = NULL)
{
	if (!spell || !result) return true;

	switch(valueType) {
		case kMagic_SpellType:
			{
				*result = spell->spellType;
				break;
			}

		case kMagic_SpellMagickaCost:
			{
				*result = spell->GetMagickaCost(form);
				break;
			}

		case kMagic_SpellMasteryLevel:
			{
				*result = spell->masteryLevel;
				break;
			}

		case kMagic_SpellSchool:
			{
				*result = spell->GetSchool();
				break;
			}

		case kMagic_SpellExplodes:
			{
				*result = spell->TouchExplodesWithNoTarget();
				break;
			}
		default:
			break;
	}
	return true;
}

static bool GetEnchantmentItemValue(EnchantmentItem* enchantItem, UInt32 valueType, double* result)
{
	if (!enchantItem || !result) return true;

	switch(valueType)
	{
		case kMagic_EnchantmentType:
			{
				*result = enchantItem->enchantType;
				break;
			}

		case kMagic_EnchantmentCharge:
			{
				*result = enchantItem->charge;
				break;
			}

		case kMagic_EnchantmentCost:
			{
				*result = enchantItem->cost;
				break;
			}
		default:
			break;
	}
	return true;
}

static bool GetNthEffectItemValue(MagicItem* magicItem, UInt32 valueType, UInt32 whichEffect, double* result)
{
	if (!magicItem || !result) return true;
	EffectFinder finder(whichEffect);
	if (VisitEffects(magicItem, finder)) {
		return GetEffectItemValue(finder.Found(), valueType, result);		
	}
	return true;
}

static bool GetMagicItemValue(MagicItem* magicItem, UInt32 valueType, UInt32 whichEffect, double* result)
{
	if (!magicItem) return true;


	switch(valueType) {
		case kMagic_ItemType:
			{
				return GetMagicItemType(magicItem, result);
			}

		case kMagic_EffectCount:
		{
			return GetMagicItemEffectCount(magicItem, result);
			break;
		}

		case kMagic_IsAutoCalc:
		{
			TESForm* form = (TESForm*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_TESForm, 0);
			switch(form->typeID) {
				case kFormType_Spell:
					{
						SpellItem* spell = (SpellItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_SpellItem, 0);
						if (spell) {
							*result = spell->IsAutoCalc() ? 1 : 0;
						}
						break;
					}

				case kFormType_Ingredient:
					{
						IngredientItem* ingredient = (IngredientItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_IngredientItem, 0);
						if (ingredient) {
							*result = ingredient->IsAutocalc() ? 1 : 0;
						}
						break;
					}
				case kFormType_AlchemyItem:
					{
						AlchemyItem* alchemy = (AlchemyItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_AlchemyItem, 0);
						if (alchemy) {
							*result = alchemy->IsAutocalc() ? 1 : 0;
						}
						break;
					}
				case kFormType_Enchantment:
					{
						EnchantmentItem* enchantment = (EnchantmentItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_EnchantmentItem, 0);
						if(enchantment) {
							int debug = 4;
						}
						// haven't decode this yet
					}
					break;
				default:
					break;
			}
			
			break;
		}

		case kMagic_EffectCode:
		case kMagic_EffectMagnitude:
		case kMagic_EffectArea:
		case kMagic_EffectDuration:
		case kMagic_EffectRange:
		{
			return GetNthEffectItemValue(magicItem, valueType, whichEffect, result);	
		}

		case kMagic_SpellType:
		case kMagic_SpellMagickaCost:
		case kMagic_SpellMasteryLevel:
		case kMagic_SpellSchool:
		case kMagic_SpellExplodes:
		{
			SpellItem* spell = (SpellItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_SpellItem, 0);
			if (spell) {
				return GetSpellItemValue(spell, valueType, result);
			}
			break;
		}

		case kMagic_EnchantmentType:
		case kMagic_EnchantmentCharge:
		case kMagic_EnchantmentCost:
		{
			EnchantmentItem* enchant = (EnchantmentItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_EnchantmentItem, 0);
			if (enchant) {
				return GetEnchantmentItemValue(enchant, valueType, result);
			}
			break;
		}

	}		
	return true;
}


static bool Cmd_GetMagicItemValue_Execute(COMMAND_ARGS)
{
	*result = 0;

	UInt32 valueType = 0;
	MagicItem* magicItem = NULL;
	UInt32 whichEffect = 0;
	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &valueType, &magicItem, &whichEffect);

	return GetMagicItemValue(magicItem, valueType, whichEffect, result);
}


enum {
	kMagicEffect_EffectCode = 0,
	kMagicEffect_BaseCost,
	kMagicEffect_School,
	kMagicEffect_ProjectileSpeed,
	kMagicEffect_EnchantFactor,
	kMagicEffect_BarterFactor,
	kMagicEffect_EffectShader,
	kMagicEffect_EnchantEffect,
	kMagicEffect_Light,
	kMagicEffect_CastingSound,
	kMagicEffect_BoltSound,
	kMagicEffect_HitSound,
	kMagicEffect_AreaSound,
	kMagicEffect_IsHostile,
	kMagicEffect_CanRecover,
	kMagicEffect_IsDetrimental,
	kMagicEffect_MagnitudePercent,
	kMagicEffect_OnSelfAllowed,
	kMagicEffect_OnTouchAllowed,
	kMagicEffect_OnTargetAllowed,
	kMagicEffect_NoDuration,
	kMagicEffect_NoMagnitude,
	kMagicEffect_NoArea,
	kMagicEffect_FXPersists,
	kMagicEffect_ForSpellmaking,
	kMagicEffect_ForEnchanting,
	kMagicEffect_NoIngredient,
	kMagicEffect_UseWeapon,
	kMagicEffect_UseArmor,
	kMagicEffect_UseCreature,
	kMagicEffect_UseSkill,
	kMagicEffect_UseAttribute,
	kMagicEffect_UseActorValue,
	kMagicEffect_NoHitEffect,
	kMagicEffect_OtherActorValue,
	kMagicEffect_UsedObject,
};

static bool GetMagicEffectValue(EffectSetting* effect, UInt32 valueType, double* result)
{
	if (!effect || !result) return true;

	*result = 0;

	switch(valueType) {
		case kMagicEffect_EffectCode:
			{
				*result = effect->effectCode;
				break;
			}

		case kMagicEffect_BaseCost:
			{
				*result = effect->baseCost;
				break;
			}

		case kMagicEffect_School:
			{
				*result = effect->school;
				break;
			}

		case kMagicEffect_ProjectileSpeed:
			{
				*result = effect->projSpeed;
				break;
			}

		case kMagicEffect_EnchantFactor:
			{
				*result = effect->enchantFactor;
				break;
			}

		case kMagicEffect_BarterFactor:
			{
				*result = effect->barterFactor;
				break;
			}

		case kMagicEffect_IsHostile:
			{
				*result = effect->IsHostile() ? 1 : 0;
				break;
			}

		case kMagicEffect_CanRecover:
			{
				*result = effect->CanRecover() ? 1 : 0;
				break;
			}

		case kMagicEffect_IsDetrimental:
			{
				*result = effect->IsDetrimental() ? 1 : 0;
				break;
			}

		case kMagicEffect_MagnitudePercent:
			{
				*result = effect->MagnitudeIsPercent() ? 1 : 0;
				break;
			}

		case kMagicEffect_OnSelfAllowed:
			{
				*result = effect->OnSelfAllowed() ? 1 : 0;
				break;
			}

		case kMagicEffect_OnTouchAllowed:
			{
				*result = effect->OnTouchAllowed() ? 1 : 0;
				break;
			}

		case kMagicEffect_OnTargetAllowed:
			{
				*result = effect->OnTargetAllowed() ? 1 : 0;
				break;
			}

		case kMagicEffect_NoDuration:
			{
				*result = effect->NoDuration() ? 1 : 0;
				break;
			}

		case kMagicEffect_NoMagnitude:
			{
				*result = effect->NoMagnitude() ? 1 : 0;
				break;
			}

		case kMagicEffect_NoArea:
			{
				*result = effect->NoArea() ? 1 : 0;
				break;
			}

		case kMagicEffect_FXPersists:
			{
				*result = effect->FXPersists() ? 1 : 0;
				break;
			}

		case kMagicEffect_ForSpellmaking:
			{
				*result = effect->ForSpellmaking() ? 1 : 0;
				break;
			}

		case kMagicEffect_ForEnchanting:
			{
				*result = effect->ForEnchanting() ? 1 : 0;
				break;
			}


		case kMagicEffect_NoIngredient:
			{
				*result = effect->NoIngredient() ? 1 : 0;
				break;
			}

		case kMagicEffect_UseWeapon:
			{
				*result = effect->UseWeapon() ? 1 : 0;
				break;
			}

		case kMagicEffect_UseArmor:
			{
				*result = effect->UseArmor() ? 1 : 0;
				break;
			}

		case kMagicEffect_UseCreature:
			{
				*result = effect->UseCreature() ? 1 : 0;
				break;
			}

		case kMagicEffect_UseSkill:
			{
				*result = effect->UseSkill() ? 1 : 0;
				break;
			}

		case kMagicEffect_UseAttribute:
			{
				*result = effect->UseAttribute() ? 1 : 0;
				break;
			}

		case kMagicEffect_UseActorValue:
			{
				*result = effect->UseOtherActorValue() ? 1 : 0;
				break;
			}

		case kMagicEffect_NoHitEffect:
			{
				*result = effect->NoHitEffect() ? 1 : 0;
				break;
			}

		case kMagicEffect_OtherActorValue:
			{
				*result = effect->data;
				break;
			}

		case kMagicEffect_UsedObject:
			{
				UInt32* refResult = (UInt32*)result;
				*refResult = effect->data;
				break;
			}

		case kMagicEffect_EffectShader:
		case kMagicEffect_EnchantEffect:
		case kMagicEffect_Light:
		case kMagicEffect_CastingSound:
		case kMagicEffect_BoltSound:
		case kMagicEffect_HitSound:
		case kMagicEffect_AreaSound:
		default:
			break;
	}
	return true;
}

static bool Cmd_GetMagicEffectValue_Execute(COMMAND_ARGS)
{
	*result = 0;

	UInt32 whichValue = 0;
	EffectSetting* magic = NULL;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &whichValue, &magic)) return true;

	if (magic) {
		return GetMagicEffectValue(magic, whichValue, result);
	}

	return true;
}

static bool GetMagicEffectValue_Execute(COMMAND_ARGS, UInt32 whichValue)
{
	*result = 0;
	EffectSetting* magic = NULL;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &magic)) return true;
	if (magic) {
		return GetMagicEffectValue(magic, whichValue, result);
	}
	return true;
}

static bool GetMagicEffectValueC(UInt32 effectCode, UInt32 whichValue, double* result)
{
	*result = 0;
	EffectSetting *magicEffect = EffectSetting::EffectSettingForC(effectCode);
	if (magicEffect) {
		return GetMagicEffectValue(magicEffect, whichValue, result);
	}
	return true;
}

static bool Cmd_GetMagicEffectCodeValue_Execute(COMMAND_ARGS)
{
	*result = 0;

	UInt32 whichValue = 0;
	UInt32 effectCode = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &whichValue, &effectCode)) return true;
	return GetMagicEffectValueC(effectCode, whichValue, result);
}

static bool GetMagicEffectValueC_Execute(COMMAND_ARGS, UInt32 whichValue)
{
	*result = 0;
	UInt32 effectCode = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &effectCode)) return true;
	return GetMagicEffectValueC(effectCode, whichValue, result);
}

class EffectCodeFinder : public EffectVisitor
{
	UInt32 m_effectCode;
	UInt32 m_count;
public:
	EffectCodeFinder(UInt32 effectCode) : m_effectCode(effectCode), m_count(0) {}
	~EffectCodeFinder() {}

	bool Accept(EffectItem* effectItem) {
		if (effectItem->effectCode == m_effectCode) {
			m_count++;
		}
		return true;
	}

	UInt32 Found() { return m_count; }
};

static bool MagicItemHasEffectCode(MagicItem* magicItem, UInt32 effectCode, bool bReturnCount, double* result)
{
	if (!magicItem || ! result) return true;

	EffectCodeFinder codeFinder(effectCode);
	if (VisitEffects(magicItem, codeFinder)) {
		UInt32 count = codeFinder.Found();
		if (bReturnCount) {
			*result = count;
		} else {
			*result = count != 0;
		}
	}
	return true;
}

const bool bReturnCountT = true;
const bool bReturnCountF = false;

static bool MagicItemHasEffect_Execute(COMMAND_ARGS, bool bReturnCount)
{
	*result = 0;
	EffectSetting* magic = 0;
	MagicItem* magicItem = NULL;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &magic, &magicItem)) return true;

	if (magicItem && magic) {
		return MagicItemHasEffectCode(magicItem, magic->effectCode, bReturnCount, result);
	}
	return true;
}

static bool Cmd_MagicItemHasEffect_Execute(COMMAND_ARGS)
{
	return MagicItemHasEffect_Execute(PASS_COMMAND_ARGS, bReturnCountF);
}

static bool Cmd_MagicItemHasEffectCount_Execute(COMMAND_ARGS)
{
	return MagicItemHasEffect_Execute(PASS_COMMAND_ARGS, bReturnCountT);
}


static bool MagicItemHasEffectCode_Execute(COMMAND_ARGS, bool bReturnCount)
{
	*result = 0;

	UInt32 effectCode = 0;
	MagicItem* magicItem = NULL;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &effectCode, &magicItem)) return true;

	return MagicItemHasEffectCode(magicItem, effectCode, bReturnCount, result);
}

static bool Cmd_MagicItemHasEffectCode_Execute(COMMAND_ARGS)
{
	return MagicItemHasEffectCode_Execute(PASS_COMMAND_ARGS, bReturnCountF);
}

static bool Cmd_MagicItemHasEffectCountCode_Execute(COMMAND_ARGS)
{
	return MagicItemHasEffectCode_Execute(PASS_COMMAND_ARGS, bReturnCountT);
}


static bool GetNthEffectItemValue_Execute(COMMAND_ARGS, UInt32 whichValue)
{
	*result = 0;
	UInt32 whichEffect = 0;
	MagicItem* magicItem = NULL;
	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &magicItem, &whichEffect);
	return GetNthEffectItemValue(magicItem, whichValue, whichEffect, result);
}

static bool GetSpellItemValue_Execute(COMMAND_ARGS, UInt32 whichValue)
{
	*result = 0;
	SpellItem* spell = NULL;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &spell)) return true;
	
	if (spell) {
		return GetSpellItemValue(spell, whichValue, result, (thisObj) ? thisObj->baseForm : NULL);
	}
	return true;
}

static bool GetEnchantmentItemValue_Execute(COMMAND_ARGS, UInt32 whichValue)
{
	*result = 0;
	MagicItem* magicItem = NULL;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &magicItem)) return true;
	
	EnchantmentItem* enchantmentItem = (EnchantmentItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_EnchantmentItem, 0);
	if (enchantmentItem) {
		return GetEnchantmentItemValue(enchantmentItem, whichValue, result);
	}
	return true;
}


static bool Cmd_GetMagicItemType_Execute(COMMAND_ARGS)
{
	*result = 0;
	MagicItem* magicItem = NULL;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &magicItem)) return true;
	return GetMagicItemType(magicItem, result);
}

static bool Cmd_GetMagicItemEffectCount_Execute(COMMAND_ARGS)
{
	*result = 0;
	MagicItem* magicItem = NULL;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &magicItem)) return true;
	return GetMagicItemEffectCount(magicItem, result);
}

static bool Cmd_GetNthEffectItemCode_Execute(COMMAND_ARGS)
{
	return GetNthEffectItemValue_Execute(PASS_COMMAND_ARGS, kMagic_EffectCode);
}

static bool Cmd_GetNthEffectItemMagnitude_Execute(COMMAND_ARGS)
{
	return GetNthEffectItemValue_Execute(PASS_COMMAND_ARGS, kMagic_EffectMagnitude);
}

static bool Cmd_GetNthEffectItemArea_Execute(COMMAND_ARGS)
{
	return GetNthEffectItemValue_Execute(PASS_COMMAND_ARGS, kMagic_EffectArea);
}

static bool Cmd_GetNthEffectItemDuration_Execute(COMMAND_ARGS)
{
	return GetNthEffectItemValue_Execute(PASS_COMMAND_ARGS, kMagic_EffectDuration);
}

static bool Cmd_GetNthEffectItemRange_Execute(COMMAND_ARGS)
{
	return GetNthEffectItemValue_Execute(PASS_COMMAND_ARGS, kMagic_EffectRange);
}

static bool Cmd_GetNthEffectItemActorValue_Execute(COMMAND_ARGS)
{
	return GetNthEffectItemValue_Execute(PASS_COMMAND_ARGS, kMagic_EffectActorValue);
}

static bool Cmd_GetSpellType_Execute(COMMAND_ARGS)
{
	return GetSpellItemValue_Execute(PASS_COMMAND_ARGS, kMagic_SpellType);
}

static bool Cmd_GetSpellMagikaCost_Execute(COMMAND_ARGS)
{
	return GetSpellItemValue_Execute(PASS_COMMAND_ARGS, kMagic_SpellMagickaCost);
}

static bool Cmd_GetSpellMasteryLevel_Execute(COMMAND_ARGS)
{
	return GetSpellItemValue_Execute(PASS_COMMAND_ARGS, kMagic_SpellMasteryLevel);
}

static bool Cmd_GetSpellSchool_Execute(COMMAND_ARGS)
{
	return GetSpellItemValue_Execute(PASS_COMMAND_ARGS, kMagic_SpellSchool);
}

static bool Cmd_GetSpellExplodes_Execute(COMMAND_ARGS)
{
	return GetSpellItemValue_Execute(PASS_COMMAND_ARGS, kMagic_SpellExplodes);
}

static bool Cmd_GetEnchantmentType_Execute(COMMAND_ARGS)
{
	return GetEnchantmentItemValue_Execute(PASS_COMMAND_ARGS, kMagic_EnchantmentType);
}

static bool Cmd_GetEnchantmentCharge_Execute(COMMAND_ARGS)
{
	return GetEnchantmentItemValue_Execute(PASS_COMMAND_ARGS, kMagic_EnchantmentCharge);
}

static bool Cmd_GetEnchantmentCost_Execute(COMMAND_ARGS)
{
	return GetEnchantmentItemValue_Execute(PASS_COMMAND_ARGS, kMagic_EnchantmentCost);
}

static bool Cmd_GetMagicEffectCode_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_EffectCode);
}

static bool Cmd_GetMagicEffectBaseCost_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_BaseCost);
}

static bool Cmd_GetMagicEffectSchool_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_School);
}

static bool Cmd_GetMagicEffectProjectileSpeed_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_ProjectileSpeed);
}

static bool Cmd_GetMagicEffectEnchantFactor_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_EnchantFactor);
}

static bool Cmd_GetMagicEffectBarterFactor_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_BarterFactor);
}

static bool Cmd_GetMagicEffectBaseCostC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_BaseCost);
}

static bool Cmd_GetMagicEffectSchoolC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_School);
}

static bool Cmd_GetMagicEffectProjectileSpeedC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_ProjectileSpeed);
}

static bool Cmd_GetMagicEffectEnchantFactorC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_EnchantFactor);
}

static bool Cmd_GetMagicEffectBarterFactorC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_BarterFactor);
}

static bool Cmd_IsMagicEffectHostile_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_IsHostile);
}

static bool Cmd_IsMagicEffectHostileC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_IsHostile);
}

const bool bForModT = true;
const bool bForModF = false;

class ChangeMagicValue
{
public:
	ChangeMagicValue(UInt32 whichValue, bool bForMod) 
		: m_form(NULL), m_whichValue(whichValue), m_floatVal(0), m_integerVal(0), m_bForMod(bForMod){}
	~ChangeMagicValue() {}


	UInt32 WhichValue() const { return m_whichValue; }
	float FloatVal() const { return m_floatVal; }
	UInt32 IntegerVal() const { return m_integerVal; }
	TESForm* FormVal() const { return m_form; }
	const char* StringVal() const { return (const char*) m_stringVal; }
	float* FloatPtr() { return &m_floatVal; }
	UInt32* IntegerPtr() { return &m_integerVal; }
	TESForm** FormPtr() { return &m_form; }
	char** StringPtr() { return (char**)&m_stringVal; }
	bool ForMod() const { return m_bForMod; }

private:
	char m_stringVal[256];
	TESForm* m_form;
	UInt32 m_whichValue;
	float m_floatVal;
	UInt32 m_integerVal;
	bool m_bForMod;
};


static bool ChangeEffectItemValue(EffectItem* effect, ChangeMagicValue& cmv, double* result)
{
	if (!effect || !result) return true;
	switch(cmv.WhichValue()) {
		case kMagic_EffectMagnitude:
			{
				cmv.ForMod() ? effect->ModMagnitude(cmv.FloatVal()) : 
					effect->SetMagnitude(cmv.IntegerVal());
				break;
			}
		case kMagic_EffectArea:			
			{
				cmv.ForMod() ? effect->ModArea(cmv.FloatVal()) : 
					effect->SetArea(cmv.IntegerVal());
				break;
			}
		case kMagic_EffectDuration:		
			{
				cmv.ForMod() ? effect->ModDuration(cmv.FloatVal()) : 
					effect->SetDuration(cmv.IntegerVal());
				break;
			}
		case kMagic_EffectRange:
			{
				effect->SetRange(cmv.IntegerVal());
				break;
			}
		case kMagic_EffectActorValue:
			{
				effect->SetActorValue(cmv.IntegerVal());
				break;
			}
		case kMagic_EffectScript:
			{
				if (effect->IsScriptedEffect() && effect->scriptEffectInfo) {
					Script* script = (Script*)Oblivion_DynamicCast(cmv.FormVal(), 0, RTTI_TESForm, RTTI_Script, 0);
					if (script && script->IsMagicScript()) {
						effect->scriptEffectInfo->SetScriptRefID(cmv.FormVal()->refID);
					}
				}
				break;
			}
		case kMagic_EffectScriptSchool:
			{
				if (effect->IsScriptedEffect() && effect->scriptEffectInfo) {
					effect->scriptEffectInfo->SetSchool(cmv.IntegerVal());
				}
				break;
			}
		case kMagic_EffectScriptHostile:
			{
				if (effect->IsScriptedEffect() && effect->scriptEffectInfo) {
					effect->scriptEffectInfo->SetIsHostile(cmv.IntegerVal() == 1);
				}
				break;
			}
		case kMagic_EffectScriptName:
			{
				if (effect->IsScriptedEffect() && effect->scriptEffectInfo) {
					effect->scriptEffectInfo->SetName(cmv.StringVal());
				}
			}

		default:
			break;
	}
	return true;
}

static bool ChangeNthEffectItem(MagicItem* magicItem, UInt32 whichEffect, ChangeMagicValue& cmv, double* result)
{
	EffectFinder finder(whichEffect);
	if (VisitEffects(magicItem, finder)) {
		return ChangeEffectItemValue(finder.Found(), cmv, result);
	}
	return true;
}

static bool ChangeNthEffectItem_Execute(COMMAND_ARGS, UInt32 whichValue, bool bForMod)
{
	UInt32 nuVal = 0;
	MagicItem* magicItem = NULL;
	UInt32 whichEffect = 0;
	ChangeMagicValue cmv(whichValue, bForMod);
	bool bArgsExtracted = false;
	bool bChangeScript = (whichValue == kMagic_EffectScript);
	if (whichValue == kMagic_EffectScriptName) {
		bArgsExtracted = ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, cmv.StringPtr(), &magicItem, &whichEffect);
	} else if (bChangeScript) {
		TESForm* magicForm = NULL;
		bArgsExtracted = ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, cmv.FormPtr(), &magicForm, &whichEffect);
		magicItem = (MagicItem*)Oblivion_DynamicCast(magicForm, 0, RTTI_TESForm, RTTI_MagicItem, 0);
	} else if (bForMod) {
		bArgsExtracted = ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, cmv.FloatPtr(), &magicItem, &whichEffect);
	} else {
		bArgsExtracted = ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, cmv.IntegerPtr(), &magicItem, &whichEffect);
	}
	if (!bArgsExtracted && !bChangeScript) return true;
	return ChangeNthEffectItem(magicItem, whichEffect, cmv, result);
}

static bool Cmd_SetNthEffectItemMagnitude_Execute(COMMAND_ARGS)
{
	return ChangeNthEffectItem_Execute(PASS_COMMAND_ARGS, kMagic_EffectMagnitude, bForModF);
}

static bool Cmd_ModNthEffectItemMagnitude_Execute(COMMAND_ARGS)
{
	return ChangeNthEffectItem_Execute(PASS_COMMAND_ARGS, kMagic_EffectMagnitude, bForModT);
}

static bool Cmd_SetNthEffectItemArea_Execute(COMMAND_ARGS)
{
	return ChangeNthEffectItem_Execute(PASS_COMMAND_ARGS, kMagic_EffectArea, bForModF);
}

static bool Cmd_ModNthEffectItemArea_Execute(COMMAND_ARGS)
{
	return ChangeNthEffectItem_Execute(PASS_COMMAND_ARGS, kMagic_EffectArea, bForModT);
}

static bool Cmd_SetNthEffectItemDuration_Execute(COMMAND_ARGS)
{
	return ChangeNthEffectItem_Execute(PASS_COMMAND_ARGS, kMagic_EffectDuration, bForModF);
}

static bool Cmd_ModNthEffectItemDuration_Execute(COMMAND_ARGS)
{
	return ChangeNthEffectItem_Execute(PASS_COMMAND_ARGS, kMagic_EffectDuration, bForModT);
}

static bool Cmd_SetNthEffectItemRange_Execute(COMMAND_ARGS)
{
	return ChangeNthEffectItem_Execute(PASS_COMMAND_ARGS, kMagic_EffectRange, bForModF);
}

static bool Cmd_SetNthEffectItemActorValue_Execute(COMMAND_ARGS)
{
	return ChangeNthEffectItem_Execute(PASS_COMMAND_ARGS, kMagic_EffectActorValue, bForModF);
}

static bool Cmd_IsNthEffectItemScripted_Execute(COMMAND_ARGS)
{
	return GetNthEffectItemValue_Execute(PASS_COMMAND_ARGS, kMagic_EffectIsScripted);
}

static bool Cmd_GetNthEffectItemScript_Execute(COMMAND_ARGS)
{
	return GetNthEffectItemValue_Execute(PASS_COMMAND_ARGS, kMagic_EffectScript);
}

static bool Cmd_GetNthEffectItemScriptVisualEffect_Execute(COMMAND_ARGS)
{
	return GetNthEffectItemValue_Execute(PASS_COMMAND_ARGS, kMagic_EffectScriptVisualEffect);
}

static bool Cmd_GetNthEffectItemScriptSchool_Execute(COMMAND_ARGS)
{
	return GetNthEffectItemValue_Execute(PASS_COMMAND_ARGS, kMagic_EffectScriptSchool);
}

static bool Cmd_IsNthEffectItemScriptHostile_Execute(COMMAND_ARGS)
{
	return GetNthEffectItemValue_Execute(PASS_COMMAND_ARGS, kMagic_EffectScriptHostile);
}

static bool Cmd_SetNthEffectItemScript_Execute(COMMAND_ARGS)
{
	return ChangeNthEffectItem_Execute(PASS_COMMAND_ARGS, kMagic_EffectScript, bForModF);
}

static bool Cmd_SetNthEffectItemScriptName_Execute(COMMAND_ARGS)
{
	return ChangeNthEffectItem_Execute(PASS_COMMAND_ARGS, kMagic_EffectScriptName, bForModF);
}


static bool SetNthEffectItemVisualEffect(MagicItem* magicItem, UInt32 effectCode, UInt32 whichEffect, double* result)
{
	if (!magicItem || !result) return true;

	EffectFinder finder(whichEffect);
	if (VisitEffects(magicItem, finder)) {
		EffectItem* found = finder.Found();
		if (found && found->IsScriptedEffect() && found->scriptEffectInfo) {
			found->scriptEffectInfo->visualEffectCode = effectCode;
		}
	}
	return true;
}

static bool Cmd_SetNthEffectItemScriptVisualEffect_Execute(COMMAND_ARGS)
{
	*result = 0;
	EffectSetting* magic = 0;
	MagicItem* magicItem = NULL;
	UInt32 whichEffect = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &magic, &magicItem, &whichEffect)) return true;

	if (magicItem && magic) {
		return SetNthEffectItemVisualEffect(magicItem, magic->effectCode, whichEffect, result);
	}
	return true;
}

static bool Cmd_SetNthEffectItemScriptVisualEffectC_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 effectCode = 0;
	MagicItem* magicItem = NULL;
	UInt32 whichEffect = 0;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &effectCode, &magicItem, &whichEffect)) return true;

	if (magicItem) {
		return SetNthEffectItemVisualEffect(magicItem, effectCode, whichEffect, result);
	}
	return true;
}

static bool Cmd_SetNthEffectItemScriptSchool_Execute(COMMAND_ARGS)
{
	return ChangeNthEffectItem_Execute(PASS_COMMAND_ARGS, kMagic_EffectScriptSchool, bForModF);
}

static bool Cmd_SetNthEffectItemScriptHostile_Execute(COMMAND_ARGS)
{
	return ChangeNthEffectItem_Execute(PASS_COMMAND_ARGS, kMagic_EffectScriptHostile, bForModF);
}

static bool Cmd_RemoveNthEffectItem_Execute(COMMAND_ARGS)
{
	*result = 0;
	UInt32 whichEffect = 0;
	MagicItem* magicItem = NULL;
	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &magicItem, &whichEffect);
	if (!magicItem || ! result) return true;
	TESForm* magicForm = (TESForm*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_TESForm, 0);
	if (!magicForm) return true;

	EffectItemList& spellEffectList = magicItem->list;
	bool bFound = spellEffectList.RemoveItem(whichEffect);
	if (bFound) {
		AlchemyItem* potion = (AlchemyItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_AlchemyItem, 0);
		if (potion) {
			potion->UpdateHostileEffectCount();
		}
	}
	return true;
}

//static bool RemoveEffectItems_Execute(COMMAND_ARGS, bool bHostile)
//{
//	*result = 0;
//	MagicItem* magicItem = NULL;
//	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &magicItem);
//	if (!magicItem || ! result) return true;
//	TESForm* magicForm = (TESForm*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_TESForm, 0);
//	if (!magicForm || !IsClonedForm(magicForm->refID)) return true;
//
//	EffectItemList* spellEffectList = &(magicItem->list);
//	if (spellEffectList) {
//		bool bFound = spellEffectList->RemoveItems(bHostile);
//	}
//	return true;
//}

//static bool Cmd_RemoveHostileEffectItems_Execute(COMMAND_ARGS)
//{
//	const bool bHostileT = true;
//	return RemoveEffectItems_Execute(PASS_COMMAND_ARGS, bHostileT);
//}
//
//static bool Cmd_RemoveNonHostileEffectItems_Execute(COMMAND_ARGS)
//{
//	const bool bHostileF = false;
//	return RemoveEffectItems_Execute(PASS_COMMAND_ARGS, bHostileF);
//}

static bool ChangeMagicItem(MagicItem* magicItem, ChangeMagicValue& cmv, double* result)
{
	if (!magicItem || !result) return true;
	switch (cmv.WhichValue()) {
		case kMagic_IsAutoCalc:
			{
				UInt32 autoCalc = cmv.IntegerVal();
				SpellItem* spell = (SpellItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_SpellItem, 0);
				if (spell) {
					spell->SetAutoCalc(autoCalc==0 ? false : true);
				}
				return true;
			}
		default:
			break;
	}
	return true;
}


static bool ChangeSpellItem(SpellItem* spell, ChangeMagicValue& cmv, double* result)
{
	if (!spell || !result) return true;
	switch(cmv.WhichValue()) {
		case kMagic_SpellType:
			{
				UInt32 nuType = cmv.IntegerVal();
				if (nuType <= SpellItem::kType_Ability) {
					spell->spellType = nuType;
				}
				return true;
			}
		case kMagic_SpellMagickaCost:
			{
				spell->magickaCost = cmv.ForMod() ? SafeModUInt32(spell->magickaCost, cmv.FloatVal()) : cmv.IntegerVal();
				return true;
			}

		case kMagic_SpellMasteryLevel:
			{
				UInt32 nuLevel = cmv.IntegerVal();
				if (nuLevel <= SpellItem::kLevel_Master) {
					spell->masteryLevel = nuLevel;
				}
				return true;
			}
		case kMagic_SpellExplodes:
			{
				UInt32 spellExplodes = cmv.IntegerVal();
				bool bSpellExplodes = (spellExplodes == 0) ? false : true;
				spell->SetTouchExplodes(bSpellExplodes);
				return true;
			}
		default:
			break;
	}
	return true;
}

static bool ChangeMagicItem_Execute(COMMAND_ARGS, UInt32 whichValue, bool bForMod)
{
	if (!result) return true;
	*result = 0;
	MagicItem* magicItem = NULL;
	ChangeMagicValue cmv(whichValue, bForMod);
	bool bArgsExtracted = false;
	if (bForMod) {
		bArgsExtracted = ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, cmv.FloatPtr(), &magicItem);
	} else {
		bArgsExtracted = ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, cmv.IntegerPtr(), &magicItem);
	}
	if (!bArgsExtracted) return true;
	return ChangeMagicItem(magicItem, cmv, result);
}

static bool ChangeSpellItem_Execute(COMMAND_ARGS, UInt32 whichValue, bool bForMod)
{
	if (!result) return true;
	*result = 0;
	SpellItem* spell = NULL;
	ChangeMagicValue cmv(whichValue, bForMod);
	bool bArgsExtracted = false;
	if (bForMod) {
		bArgsExtracted = ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, cmv.FloatPtr(), &spell);
	} else {
		bArgsExtracted = ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, cmv.IntegerPtr(), &spell);
	}
	if (!bArgsExtracted) return true;
	return ChangeSpellItem(spell, cmv, result);
}

static bool ChangeEnchantmentItem(EnchantmentItem* enchantItem, ChangeMagicValue& cmv, double* result)
{
	if (!enchantItem || !result) return true;
	switch(cmv.WhichValue()) {
		case kMagic_EnchantmentType:
			{
				UInt32 nuType = cmv.IntegerVal();
				if (nuType <= EnchantmentItem::kEnchant_Apparel) {
					enchantItem->enchantType = nuType;
				}
			}
		case kMagic_EnchantmentCharge:
			{
				enchantItem->charge = cmv.ForMod() ? SafeModUInt32(enchantItem->charge, cmv.FloatVal()) : cmv.IntegerVal();
				return true;
			}
		case kMagic_EnchantmentCost:
			{
				enchantItem->cost = cmv.ForMod() ? SafeModUInt32(enchantItem->cost, cmv.FloatVal()) : cmv.IntegerVal();
				return true;
			}
		default:
			break;
	}
	return true;
}

static bool ChangeEnchantmentItem_Execute(COMMAND_ARGS, UInt32 whichValue, bool bForMod)
{
	if (!result) return true;
	*result = 0;
	MagicItem* magicItem = NULL;
	ChangeMagicValue cmv(whichValue, bForMod);
	bool bArgsExtracted = false;
	if (bForMod) {
		bArgsExtracted = ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, cmv.FloatPtr(), &magicItem);
	} else {
		bArgsExtracted = ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, cmv.IntegerPtr(), &magicItem);
	}
	if (!bArgsExtracted) return true;
	EnchantmentItem* enchantmentItem = (EnchantmentItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_EnchantmentItem, 0);
	if (enchantmentItem) {
		return ChangeEnchantmentItem(enchantmentItem, cmv, result);
	}
	return true;
}

static bool Cmd_SetSpellType_Execute(COMMAND_ARGS)
{
	return ChangeSpellItem_Execute(PASS_COMMAND_ARGS, kMagic_SpellType, bForModF);
}

static bool Cmd_SetSpellMagickaCost_Execute(COMMAND_ARGS)
{
	return ChangeSpellItem_Execute(PASS_COMMAND_ARGS, kMagic_SpellMagickaCost, bForModF);
}

static bool Cmd_ModSpellMagickaCost_Execute(COMMAND_ARGS)
{
	return ChangeSpellItem_Execute(PASS_COMMAND_ARGS, kMagic_SpellMagickaCost, bForModT);
}

static bool Cmd_SetSpellMasteryLevel_Execute(COMMAND_ARGS)
{
	return ChangeSpellItem_Execute(PASS_COMMAND_ARGS, kMagic_SpellMasteryLevel, bForModF);
}

static bool Cmd_SetSpellExplodes_Execute(COMMAND_ARGS)
{
	return ChangeSpellItem_Execute(PASS_COMMAND_ARGS, kMagic_SpellExplodes, bForModF);
}

static bool Cmd_SetEnchantmentType_Execute(COMMAND_ARGS)
{
	return ChangeEnchantmentItem_Execute(PASS_COMMAND_ARGS, kMagic_EnchantmentType, bForModF);
}

static bool Cmd_SetEnchantmentCharge_Execute(COMMAND_ARGS)
{
	return ChangeEnchantmentItem_Execute(PASS_COMMAND_ARGS, kMagic_EnchantmentCharge, bForModF);
}

static bool Cmd_ModEnchantmentCharge_Execute(COMMAND_ARGS)
{
	return ChangeEnchantmentItem_Execute(PASS_COMMAND_ARGS, kMagic_EnchantmentCharge, bForModT);
}

static bool Cmd_SetEnchantmentCost_Execute(COMMAND_ARGS)
{
	return ChangeEnchantmentItem_Execute(PASS_COMMAND_ARGS, kMagic_EnchantmentCost, bForModF);
}

static bool Cmd_ModEnchantmentCost_Execute(COMMAND_ARGS)
{
	return ChangeEnchantmentItem_Execute(PASS_COMMAND_ARGS, kMagic_EnchantmentCost, bForModT);
}

static bool Cmd_CopyNthEffectItem_Execute(COMMAND_ARGS)
{
	*result = 0;
	MagicItem* from = NULL;
	MagicItem* to = NULL;
	UInt32 whichEffect = 0;
	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &from, &to, &whichEffect);
	TESForm* toForm = (TESForm*)Oblivion_DynamicCast(to, 0, RTTI_MagicItem, RTTI_TESForm, 0);
	if (!from || !to) return true;
	
	UInt32 nuIndex = to->list.CopyItemFrom(from->list, whichEffect);
	*result = nuIndex;

	AlchemyItem* potion = (AlchemyItem*)Oblivion_DynamicCast(to, 0, RTTI_MagicItem, RTTI_AlchemyItem, 0);
	if (potion) {
		potion->UpdateHostileEffectCount();
	}

	return true;
}

static bool Cmd_CopyAllEffectItems_Execute(COMMAND_ARGS)
{
	*result = 0;
	MagicItem* from = NULL;
	MagicItem* to = NULL;
	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &from, &to);
	TESForm* toForm = (TESForm*)Oblivion_DynamicCast(to, 0, RTTI_MagicItem, RTTI_TESForm, 0);
	if (!from || !to) return true;
	
	UInt32 count = from->list.CountItems();
	for (UInt32 n = 0; n < count; n++) {
		to->list.CopyItemFrom(from->list, n);
	}
	AlchemyItem* potion = (AlchemyItem*)Oblivion_DynamicCast(to, 0, RTTI_MagicItem, RTTI_AlchemyItem, 0);
	if (potion) {
		potion->UpdateHostileEffectCount();
	}

	return true;
}

static bool Cmd_RemoveAllEffectItems_Execute(COMMAND_ARGS)
{
	*result = 0;
	MagicItem* from = NULL;
	ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &from);
	if (!from) return true;

	EffectItemList& spellEffectList = from->list;
	UInt32 count = spellEffectList.CountItems();
	// remove from the back
	while (count > 0) {
		spellEffectList.RemoveItem(--count);
	}
	return true;
}

static void AddEffectItem(MagicItem* magicItem, UInt32 magicEffectCode, double* result)
{
	EffectItem* proxyEffect = EffectItem::ProxyEffectItemFor(magicEffectCode);
	if (proxyEffect) {
		EffectItem* effectItem = proxyEffect->Clone();
		if (effectItem) {
			UInt32 nuIndex = magicItem->list.AddItem(effectItem);
			*result = nuIndex;
		}
	}

	AlchemyItem* potion = (AlchemyItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_AlchemyItem, 0);
	if (potion) {
		potion->UpdateHostileEffectCount();
	}
}

static bool Cmd_AddEffectItem_Execute(COMMAND_ARGS)
{
	*result = -1;
	EffectSetting* magicEffect = NULL;
	MagicItem* to = NULL;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &magicEffect, &to))
		return true;

	TESForm* toForm = (TESForm*)Oblivion_DynamicCast(to, 0, RTTI_MagicItem, RTTI_TESForm, 0);
	if (!to) return true;
	AddEffectItem(to, magicEffect->effectCode, result);
	return true;
}

static bool Cmd_AddEffectItemC_Execute(COMMAND_ARGS)
{
	*result = -1;
	UInt32 effectCode = 0;
	MagicItem* to = NULL;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &effectCode, &to))
		return true;

	TESForm* toForm = (TESForm*)Oblivion_DynamicCast(to, 0, RTTI_MagicItem, RTTI_TESForm, 0);
	if (!to) return true;
	AddEffectItem(to, effectCode, result);
	return true;
}

static void AddFullEffectItem(MagicItem* magicItem, UInt32 magicEffectCode, UInt32 magnitude, UInt32 area, UInt32 duration, UInt32 range, double* result)
{
	EffectItem* proxyEffect = EffectItem::ProxyEffectItemFor(magicEffectCode);
	if (proxyEffect) {
		EffectItem* effectItem = proxyEffect->Clone();
		if (effectItem) {
			effectItem->SetMagnitude(magnitude);
			effectItem->SetArea(area);
			effectItem->SetDuration(duration);
			effectItem->SetRange(range);

			UInt32 nuIndex = magicItem->list.AddItem(effectItem);
			*result = nuIndex;
		}
	}
}

static bool Cmd_AddFullEffectItem_Execute(COMMAND_ARGS)
{
	*result = -1;
	EffectSetting* magicEffect = NULL;
	UInt32 magnitude = 0;
	UInt32 area = 0;
	UInt32 duration = 0;
	UInt32 range = 0;
	MagicItem* to = NULL;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &magicEffect, &magnitude, &area, &duration, &range, &to))
		return true;
	TESForm* toForm = (TESForm*)Oblivion_DynamicCast(to, 0, RTTI_MagicItem, RTTI_TESForm, 0);
	if (!toForm) return true;
	AddFullEffectItem(to, magicEffect->effectCode, magnitude, area, duration, range, result);
	return true;
}

static bool Cmd_AddFullEffectItemC_Execute(COMMAND_ARGS)
{
	*result = -1;
	UInt32 effectCode = 0;
	UInt32 magnitude = 0;
	UInt32 area = 0;
	UInt32 duration = 0;
	UInt32 range = 0;
	MagicItem* to = NULL;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &effectCode, &magnitude, &area, &duration, &range, &to))
		return true;
	TESForm* toForm = (TESForm*)Oblivion_DynamicCast(to, 0, RTTI_MagicItem, RTTI_TESForm, 0);
	if (!toForm) return true;
	AddFullEffectItem(to, effectCode, magnitude, area, duration, range, result);
	return true;
}

// this function is not exposed and is a work in progress
static bool Cmd_AddScriptedEffectItem_Execute(COMMAND_ARGS)
{
	*result = -1;
	TESForm* scriptArg = NULL;
	char name[256];
	MagicItem* to = NULL;

	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &scriptArg, (char**)&name, &to);

	if (!scriptArg || !to) return true;

	Script* script = (Script*)Oblivion_DynamicCast(scriptArg, 0, RTTI_TESForm, RTTI_Script, 0);
	
	EffectItem* proxyEffect = EffectItem::ProxyEffectItemFor(Swap32('SEFF'));
	if (proxyEffect) {
		EffectItem* scriptedEffect = proxyEffect->Clone();
		if (scriptedEffect) {
			if (!scriptedEffect->scriptEffectInfo) {
				scriptedEffect->scriptEffectInfo = EffectItem::ScriptEffectInfo::Create();
			}
			EffectItem::ScriptEffectInfo* scriptInfo = scriptedEffect->scriptEffectInfo;
			if (scriptInfo) {
				scriptInfo->SetScriptRefID(script->refID);
				scriptInfo->SetName(name);
				UInt32 nuIndex = to->list.AddItem(scriptedEffect);
				*result = nuIndex;
			} else {
				// we need to get rid of the scriptedEffect so it doesn't leak
				FormHeap_Free(scriptedEffect);
				scriptedEffect = NULL;
			}
		}
	}
	return true;
}

static bool Cmd_IsMagicItemAutoCalc_Execute(COMMAND_ARGS)
{
	*result = 0;
	MagicItem* magicItem = NULL;
	if(!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &magicItem)) return true;
	return GetMagicItemValue(magicItem, kMagic_IsAutoCalc, 0, result);
}

static bool Cmd_SetMagicItemAutoCalc_Execute(COMMAND_ARGS)
{
	return ChangeMagicItem_Execute(PASS_COMMAND_ARGS, kMagic_IsAutoCalc, bForModF);
}

static bool Cmd_IsMagicEffectForSpellmaking_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_ForSpellmaking);
}

static bool Cmd_IsMagicEffectForSpellmakingC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_ForSpellmaking);
}

static bool Cmd_IsMagicEffectForEnchanting_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_ForEnchanting);
}

static bool Cmd_IsMagicEffectForEnchantingC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_ForEnchanting);
}

static bool Cmd_IsMagicEffectDetrimental_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_IsDetrimental);
}

static bool Cmd_IsMagicEffectDetrimentalC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_IsDetrimental);
}

static bool Cmd_IsMagicEffectCanRecover_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_CanRecover);
}

static bool Cmd_IsMagicEffectCanRecoverC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_CanRecover);
}

static bool Cmd_IsMagicEffectMagnitudePercent_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_MagnitudePercent);
}

static bool Cmd_IsMagicEffectMagnitudePercentC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_MagnitudePercent);
}

static bool Cmd_MagicEffectFXPersists_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_FXPersists);
}

static bool Cmd_MagicEffectFXPersistsC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_FXPersists);
}

static bool Cmd_IsMagicEffectOnSelfAllowed_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_OnSelfAllowed);
}

static bool Cmd_IsMagicEffectOnSelfAllowedC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_OnSelfAllowed);
}

static bool Cmd_IsMagicEffectOnTouchAllowed_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_OnTouchAllowed);
}

static bool Cmd_IsMagicEffectOnTouchAllowedC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_OnTouchAllowed);
}

static bool Cmd_IsMagicEffectOnTargetAllowed_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_OnTargetAllowed);
}

static bool Cmd_IsMagicEffectOnTargetAllowedC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_OnTargetAllowed);
}

static bool Cmd_MagicEffectHasNoDuration_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_NoDuration);
}

static bool Cmd_MagicEffectHasNoDurationC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_NoDuration);
}

static bool Cmd_MagicEffectHasNoMagnitude_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_NoMagnitude);
}

static bool Cmd_MagicEffectHasNoMagnitudeC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_NoMagnitude);
}

static bool Cmd_MagicEffectHasNoArea_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_NoArea);
}

static bool Cmd_MagicEffectHasNoAreaC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_NoArea);
}

static bool Cmd_MagicEffectHasNoIngredient_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_NoIngredient);
}

static bool Cmd_MagicEffectHasNoIngredientC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_NoIngredient);
}

static bool Cmd_MagicEffectHasNoHitEffect_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_NoHitEffect);
}

static bool Cmd_MagicEffectHasNoHitEffectC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_NoHitEffect);
}

static bool Cmd_MagicEffectUsesWeapon_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_UseWeapon);
}

static bool Cmd_MagicEffectUsesWeaponC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_UseWeapon);
}

static bool Cmd_MagicEffectUsesArmor_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_UseArmor);
}

static bool Cmd_MagicEffectUsesArmorC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_UseArmor);
}

static bool Cmd_MagicEffectUsesCreature_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_UseCreature);
}

static bool Cmd_MagicEffectUsesCreatureC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_UseCreature);
}

static bool Cmd_MagicEffectUsesSkill_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_UseSkill);
}

static bool Cmd_MagicEffectUsesSkillC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_UseSkill);
}

static bool Cmd_MagicEffectUsesAttribute_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_UseAttribute);
}

static bool Cmd_MagicEffectUsesAttributeC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_UseAttribute);
}

static bool Cmd_MagicEffectUsesOtherActorValue_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_UseActorValue);
}

static bool Cmd_MagicEffectUsesOtherActorValueC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_UseActorValue);
}

static bool Cmd_GetMagicEffectOtherActorValue_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_OtherActorValue);
}

static bool Cmd_GetMagicEffectOtherActorValueC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_OtherActorValue);
}

static bool Cmd_GetMagicEffectUsedObject_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValue_Execute(PASS_COMMAND_ARGS, kMagicEffect_UsedObject);
}

static bool Cmd_GetMagicEffectUsedObjectC_Execute(COMMAND_ARGS)
{
	return GetMagicEffectValueC_Execute(PASS_COMMAND_ARGS, kMagicEffect_UsedObject);
}


#endif // OBLIVION

static ParamInfo kParams_GetMagicItemValue[3] = 
{
	{	"value", kParamType_Integer, 0 }, 
	{	"magic item", kParamType_MagicItem, 0 },
	{	"which effect", kParamType_Integer, 1 },
};

CommandInfo kCommandInfo_GetMagicItemValue =
{
	"GetMagicItemValue",
	"GetMIV",
	0,
	"gets the specified value from the given magic item",
	0,
	3,
	kParams_GetMagicItemValue,
	HANDLER(Cmd_GetMagicItemValue_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_GetMagicEffectValue[2] = 
{
	{	"value", kParamType_Integer, 0 },
	{	"magic effect", kParamType_MagicEffect, 0 },
};

CommandInfo kCommandInfo_GetMagicEffectValue =
{
	"GetMagicEffectValue",
	"GetMEV",
	0,
	"returns the specified value for the given magic effect",
	0,
	2,
	kParams_GetMagicEffectValue,
	HANDLER(Cmd_GetMagicEffectValue_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_GetMagicEffectCodeValue[2] = 
{
	{	"value", kParamType_Integer, 0 },
	{	"effect code", kParamType_Integer, 0 },
};

CommandInfo kCommandInfo_GetMagicEffectCodeValue =
{
	"GetMagicEffectCodeValue",
	"GetMECV",
	0,
	"returns the specified value for the given magic effect code",
	0,
	2,
	kParams_GetMagicEffectCodeValue,
	HANDLER(Cmd_GetMagicEffectCodeValue_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_MagicItemHasEffect[2] = 
{
	{	"effect code", kParamType_MagicEffect, 0 },
	{	"magic item", kParamType_MagicItem, 0 },
};

CommandInfo kCommandInfo_MagicItemHasEffect =
{
	"MagicItemHasEffect",
	"MagicHasEffect",
	0,
	"returns true if the magic item has the specified effect",
	0,
	2,
	kParams_MagicItemHasEffect,
	HANDLER(Cmd_MagicItemHasEffect_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


CommandInfo kCommandInfo_MagicItemHasEffectCount =
{
	"MagicItemHasEffectCount",
	"MagicHasEffectCount",
	0,
	"returns the number of effect items of the magic item with the specified effect",
	0,
	2,
	kParams_MagicItemHasEffect,
	HANDLER(Cmd_MagicItemHasEffectCount_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


static ParamInfo kParams_MagicItemHasEffectCode[2] = 
{
	{	"effect code", kParamType_Integer, 0 },
	{	"magic item", kParamType_MagicItem, 0 },
};

CommandInfo kCommandInfo_MagicItemHasEffectCode =
{
	"MagicItemHasEffectCode",
	"MagicHasEffectC",
	0,
	"returns true if the magic item has the specified effect code",
	0,
	2,
	kParams_MagicItemHasEffectCode,
	HANDLER(Cmd_MagicItemHasEffectCode_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicItemHasEffectCountCode =
{
	"MagicItemHasEffectCountCode",
	"MagicHasEffectCountC",
	0,
	"returns the number of effect items of the magic item with the specified effect code",
	0,
	2,
	kParams_MagicItemHasEffectCode,
	HANDLER(Cmd_MagicItemHasEffectCountCode_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicItemType =
{
	"GetMagicItemType",
	"GetMIType",
	0,
	"returns the type of the magic item",
	0,
	1,
	kParams_OneMagicItem,
	HANDLER(Cmd_GetMagicItemType_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicItemEffectCount =
{
	"GetMagicItemEffectCount",
	"GetMIEffectCount",
	0,
	"returns the effect item count of the magic item",
	0,
	1,
	kParams_OneMagicItem,
	HANDLER(Cmd_GetMagicItemEffectCount_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_GetNthEffectItem[2] = 
{
	{	"magic item", kParamType_MagicItem, 0 },
	{	"which effect", kParamType_Integer, 1 },
};

CommandInfo kCommandInfo_GetNthEffectItemCode =
{
	"GetNthEffectItemCode",
	"GetNthEICode",
	0,
	"returns the effect code of the nth effect item of the specified magic item",
	0,
	2,
	kParams_GetNthEffectItem,
	HANDLER(Cmd_GetNthEffectItemCode_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNthEffectItemMagnitude =
{
	"GetNthEffectItemMagnitude",
	"GetNthEIMagnitude",
	0,
	"returns the magnitude of the nth effect item of the specified magic item",
	0,
	2,
	kParams_GetNthEffectItem,
	HANDLER(Cmd_GetNthEffectItemMagnitude_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNthEffectItemArea =
{
	"GetNthEffectItemArea",
	"GetNthEIArea",
	0,
	"returns the area of the nth effect item of the specified magic item",
	0,
	2,
	kParams_GetNthEffectItem,
	HANDLER(Cmd_GetNthEffectItemArea_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNthEffectItemDuration =
{
	"GetNthEffectItemDuration",
	"GetNthEIDuration",
	0,
	"returns the duration of the nth effect item of the specified magic item",
	0,
	2,
	kParams_GetNthEffectItem,
	HANDLER(Cmd_GetNthEffectItemDuration_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNthEffectItemRange =
{
	"GetNthEffectItemRange",
	"GetNthEIRange",
	0,
	"returns the range of the nth effect item of the specified magic item",
	0,
	2,
	kParams_GetNthEffectItem,
	HANDLER(Cmd_GetNthEffectItemRange_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNthEffectItemActorValue =
{
	"GetNthEffectItemActorValue",
	"GetNthEIAV",
	0,
	"returns the actor value of the nth effect item of the specified magic item",
	0,
	2,
	kParams_GetNthEffectItem,
	HANDLER(Cmd_GetNthEffectItemActorValue_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetSpellType =
{
	"GetSpellType",
	"",
	0,
	"returns the type of the specified spell",
	0,
	1,
	kParams_OneSpellItem,
	HANDLER(Cmd_GetSpellType_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetSpellMagickaCost =
{
	"GetSpellMagickaCost",
	"GetSpellCost",
	0,
	"returns the magicka cost of the specified spell",
	0,
	1,
	kParams_OneSpellItem,
	HANDLER(Cmd_GetSpellMagikaCost_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetSpellMasteryLevel =
{
	"GetSpellMasteryLevel",
	"GetSpellLevel",
	0,
	"returns the mastery level of the specified spell",
	0,
	1,
	kParams_OneSpellItem,
	HANDLER(Cmd_GetSpellMasteryLevel_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetSpellSchool =
{
	"GetSpellSchool",
	"",
	0,
	"returns the school of the specified spell",
	0,
	1,
	kParams_OneSpellItem,
	HANDLER(Cmd_GetSpellSchool_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetSpellExplodesWithNoTarget =
{
	"GetSpellExplodesWithNoTarget",
	"GetSpellExplodes",
	0,
	"returns whether the spell explodes on touch with no target",
	0,
	1,
	kParams_OneSpellItem,
	HANDLER(Cmd_GetSpellExplodes_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetEnchantmentType =
{
	"GetEnchantmentType",
	"GetEnchType",
	0,
	"returns the type of the specified enchantment",
	0,
	1,
	kParams_OneMagicItem,
	HANDLER(Cmd_GetEnchantmentType_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetEnchantmentCharge =
{
	"GetEnchantmentCharge",
	"GetEnchCharge",
	0,
	"returns the charge of the specified enchantment",
	0,
	1,
	kParams_OneMagicItem,
	HANDLER(Cmd_GetEnchantmentCharge_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetEnchantmentCost =
{
	"GetEnchantmentCost",
	"GetEnchCost",
	0,
	"returns the cost of the specified enchantment",
	0,
	1,
	kParams_OneMagicItem,
	HANDLER(Cmd_GetEnchantmentCost_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicEffectCode =
{
	"GetMagicEffectCode",
	"GetMECode",
	0,
	"returns the effect code for the given magic effect",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_GetMagicEffectCode_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicEffectBaseCost =
{
	"GetMagicEffectBaseCost",
	"GetMEBaseCost",
	0,
	"returns the specified base cost for the given magic effect",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_GetMagicEffectBaseCost_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicEffectSchool =
{
	"GetMagicEffectSchool",
	"GetMESchool",
	0,
	"returns the magic school for the given magic effect",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_GetMagicEffectSchool_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicEffectProjectileSpeed =
{
	"GetMagicEffectProjectileSpeed",
	"GetMEProjSpeed",
	0,
	"returns the projectile speed for the given magic effect",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_GetMagicEffectProjectileSpeed_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicEffectEnchantFactor =
{
	"GetMagicEffectEnchantFactor",
	"GetMEEnchant",
	0,
	"returns the enchantment factor for the given magic effect",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_GetMagicEffectEnchantFactor_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicEffectBarterFactor =
{
	"GetMagicEffectBarterFactor",
	"GetMEBarter",
	0,
	"returns the barter factor for the given magic effect",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_GetMagicEffectBarterFactor_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


CommandInfo kCommandInfo_GetMagicEffectBaseCostC =
{
	"GetMagicEffectBaseCostC",
	"GetMEBaseCostC",
	0,
	"returns the specified base cost for the given magic effect code",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_GetMagicEffectBaseCostC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicEffectSchoolC =
{
	"GetMagicEffectSchoolC",
	"GetMESchoolC",
	0,
	"returns the magic school for the given magic effect code",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_GetMagicEffectSchoolC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicEffectProjectileSpeedC =
{
	"GetMagicEffectProjectileSpeedC",
	"GetMEProjSpeedC",
	0,
	"returns the projectile speed for the given magic effect code",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_GetMagicEffectProjectileSpeedC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicEffectEnchantFactorC =
{
	"GetMagicEffectEnchantFactorC",
	"GetMEEnchantC",
	0,
	"returns the enchantment factor for the given magic effect code",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_GetMagicEffectEnchantFactorC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicEffectBarterFactorC =
{
	"GetMagicEffectBarterFactorC",
	"GetMEBarterC",
	0,
	"returns the barter factor for the given magic effect code",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_GetMagicEffectBarterFactorC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_SetNthEffectItemValue[3] = 
{
	{	"value", kParamType_Integer, 0 },
	{	"magic item", kParamType_MagicItem, 0 },
	{	"which effect", kParamType_Integer, 1 },
};

static ParamInfo kParams_ModNthEffectItemValue[3] = 
{
	{	"value", kParamType_Float, 0 },
	{	"magic item", kParamType_MagicItem, 0 },
	{	"which effect", kParamType_Integer, 1 },
};

CommandInfo kCommandInfo_SetNthEffectItemMagnitude =
{
	"SetNthEffectItemMagnitude",
	"SetNthEIMagnitude",
	0,
	"sets the magnitude of the nth effect item of the specified magic item",
	0,
	3,
	kParams_SetNthEffectItemValue,
	HANDLER(Cmd_SetNthEffectItemMagnitude_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_ModNthEffectItemMagnitude =
{
	"ModNthEffectItemMagnitude",
	"ModNthEIMagnitude",
	0,
	"modifies the magnitude of the nth effect item of the specified magic item",
	0,
	3,
	kParams_ModNthEffectItemValue,
	HANDLER(Cmd_ModNthEffectItemMagnitude_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetNthEffectItemArea =
{
	"SetNthEffectItemArea",
	"SetNthEIArea",
	0,
	"sets the area of the nth effect item of the specified magic item",
	0,
	3,
	kParams_SetNthEffectItemValue,
	HANDLER(Cmd_SetNthEffectItemArea_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_ModNthEffectItemArea =
{
	"ModNthEffectItemArea",
	"ModNthEIArea",
	0,
	"modifies the area of the nth effect item of the specified magic item",
	0,
	3,
	kParams_ModNthEffectItemValue,
	HANDLER(Cmd_ModNthEffectItemArea_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetNthEffectItemDuration =
{
	"SetNthEffectItemDuration",
	"SetNthEIDuration",
	0,
	"sets the duration of the nth effect item of the specified magic item",
	0,
	3,
	kParams_SetNthEffectItemValue,
	HANDLER(Cmd_SetNthEffectItemDuration_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_ModNthEffectItemDuration =
{
	"ModNthEffectItemDuration",
	"ModNthEIDuration",
	0,
	"modifies the duration of the nth effect item of the specified magic item",
	0,
	3,
	kParams_ModNthEffectItemValue,
	HANDLER(Cmd_ModNthEffectItemDuration_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetNthEffectItemRange =
{
	"SetNthEffectItemRange",
	"SetNthEIRange",
	0,
	"sets the range of the nth effect item of the specified magic item",
	0,
	3,
	kParams_SetNthEffectItemValue,
	HANDLER(Cmd_SetNthEffectItemRange_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_SetNthEffectActorValue[3] = 
{
	{	"value", kParamType_ActorValue, 0 },
	{	"magic item", kParamType_MagicItem, 0 },
	{	"which effect", kParamType_Integer, 1 },
};

static ParamInfo kParams_SetNthEffectActorValueC[3] = 
{
	{	"actor value code", kParamType_Integer, 0 },
	{	"magic item", kParamType_MagicItem, 0 },
	{	"which effect", kParamType_Integer, 1 },
};

CommandInfo kCommandInfo_SetNthEffectItemActorValue =
{
	"SetNthEffectItemActorValue",
	"SetNthEIAV",
	0,
	"sets the actor value of the nth effect item of the specified magic item",
	0,
	3,
	kParams_SetNthEffectActorValue,
	HANDLER(Cmd_SetNthEffectItemActorValue_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetNthEffectItemActorValueC =
{
	"SetNthEffectItemActorValueC",
	"SetNthEIAVC",
	0,
	"sets the actor value of the nth effect item of the specified magic item",
	0,
	3,
	kParams_SetNthEffectActorValueC,
	HANDLER(Cmd_SetNthEffectItemActorValue_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsNthEffectItemScripted =
{
	"IsNthEffectItemScripted",
	"IsNthEIScripted",
	0,
	"returns 1 if the nth effect item of the specified magic item is scripted",
	0,
	2,
	kParams_GetNthEffectItem,
	HANDLER(Cmd_IsNthEffectItemScripted_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNthEffectItemScript =
{
	"GetNthEffectItemScript",
	"GetNthEIScript",
	0,
	"returns the object id of nth effect item's script of the specified magic item",
	0,
	2,
	kParams_GetNthEffectItem,
	HANDLER(Cmd_GetNthEffectItemScript_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNthEffectItemScriptVisualEffect =
{
	"GetNthEffectItemScriptVisualEffect",
	"GetNthEISVisualEffect",
	0,
	"returns the magic effect code of nth effect item's scripted visual effect of the specified magic item",
	0,
	2,
	kParams_GetNthEffectItem,
	HANDLER(Cmd_GetNthEffectItemScriptVisualEffect_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNthEffectItemScriptSchool =
{
	"GetNthEffectItemScriptSchool",
	"GetNthEISSchool",
	0,
	"returns the magic effect code of nth effect item's scripted visual effect of the specified magic item",
	0,
	2,
	kParams_GetNthEffectItem,
	HANDLER(Cmd_GetNthEffectItemScriptSchool_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsNthEffectItemScriptHostile =
{
	"IsNthEffectItemScriptHostile",
	"IsNthEISHostile",
	0,
	"returns 1 if the nth effect item of the specified magic item is hostile",
	0,
	2,
	kParams_GetNthEffectItem,
	HANDLER(Cmd_IsNthEffectItemScriptHostile_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_SetNthEffectItemScript[3] = 
{
	{	"script", kParamType_MagicItem, 0 },
	{	"magic item", kParamType_MagicItem, 0 },
	{	"which effect", kParamType_Integer, 1 },
};


CommandInfo kCommandInfo_SetNthEffectItemScript =
{
	"SetNthEffectItemScript",
	"SetNthEIScript",
	0,
	"sets the script for the nth effect item",
	0,
	3,
	kParams_SetNthEffectItemScript,
	HANDLER(Cmd_SetNthEffectItemScript_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_SetNthEffectItemEffect[3] = 
{
	{	"effect", kParamType_MagicEffect, 0 },
	{	"magic item", kParamType_MagicItem, 0 },
	{	"which effect", kParamType_Integer, 1 },
};

CommandInfo kCommandInfo_SetNthEffectItemScriptVisualEffect =
{
	"SetNthEffectItemScriptVisualEffect",
	"SetNthEISVisualEffect",
	0,
	"sets the magic effect code of nth effect item's scripted visual effect of the specified magic item",
	0,
	3,
	kParams_SetNthEffectItemEffect,
	HANDLER(Cmd_SetNthEffectItemScriptVisualEffect_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetNthEffectItemScriptVisualEffectC =
{
	"SetNthEffectItemScriptVisualEffectC",
	"SetNthEISVisualEffectC",
	0,
	"sets the magic effect code of nth effect item's scripted visual effect of the specified magic item",
	0,
	3,
	kParams_SetNthEffectItemValue,
	HANDLER(Cmd_SetNthEffectItemScriptVisualEffectC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetNthEffectItemScriptSchool =
{
	"SetNthEffectItemScriptSchool",
	"SetNthEISSchool",
	0,
	"sets the school of nth effect of the specified magic item",
	0,
	3,
	kParams_SetNthEffectItemValue,
	HANDLER(Cmd_SetNthEffectItemScriptSchool_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetNthEffectItemScriptHostile =
{
	"SetNthEffectItemScriptHostile",
	"SetNthEISHostile",
	0,
	"sets whether the nth item effect is hostile",
	0,
	3,
	kParams_SetNthEffectItemValue,
	HANDLER(Cmd_SetNthEffectItemScriptHostile_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_SetNthEffectItemScriptName[3] = 
{
	{	"name", kParamType_String, 0 },
	{	"magic item", kParamType_MagicItem, 0 },
	{	"which effect", kParamType_Integer, 1 },
};

CommandInfo kCommandInfo_SetNthEffectItemScriptName =
{
	"SetNthEffectItemScriptName",
	"SetNthEISName",
	0,
	"sets the name of the scripted effect item",
	0,
	3,
	kParams_SetNthEffectItemScriptName,
	HANDLER(Cmd_SetNthEffectItemScriptName_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


CommandInfo kCommandInfo_RemoveNthEffectItem =
{
	"RemoveNthEffectItem",
	"RemNthEffect",
	0,
	"removes the nth effect item of the specified magic item",
	0,
	2,
	kParams_GetNthEffectItem,
	HANDLER(Cmd_RemoveNthEffectItem_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_RemoveAllEffectItems =
{
	"RemoveAllEffectItems",
	"RemAllEffects",
	0,
	"removes all effect items from the specified magic item",
	0,
	1,
	kParams_OneMagicItem,
	HANDLER(Cmd_RemoveAllEffectItems_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_SetSpellItemVal[2] = 
{
	{	"value", kParamType_Integer, 0 },
	{	"spell", kParamType_SpellItem, 0 },
};

static ParamInfo kParams_ModSpellItemVal[2] = 
{
	{	"value", kParamType_Float, 0 },
	{	"spell", kParamType_SpellItem, 0 },
};

CommandInfo kCommandInfo_SetSpellType =
{
	"SetSpellType",
	"",
	0,
	"sets the spell to the specified type",
	0,
	2,
	kParams_SetSpellItemVal,
	HANDLER(Cmd_SetSpellType_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetSpellMagickaCost =
{
	"SetSpellMagickaCost",
	"",
	0,
	"sets the magicka cost of the spell",
	0,
	2,
	kParams_SetSpellItemVal,
	HANDLER(Cmd_SetSpellMagickaCost_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_ModSpellMagickaCost =
{
	"ModSpellMagickaCost",
	"",
	0,
	"modifies the magicka cost of the spell",
	0,
	2,
	kParams_ModSpellItemVal,
	HANDLER(Cmd_ModSpellMagickaCost_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetSpellMasteryLevel =
{
	"SetSpellMasteryLevel",
	"",
	0,
	"sets the mastery level of the spell",
	0,
	2,
	kParams_SetSpellItemVal,
	HANDLER(Cmd_SetSpellMasteryLevel_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetSpellExplodesWithNoTarget =
{
	"SetSpellExplodesWithNoTarget",
	"SetSpellExplodes",
	0,
	"sets the spell to the specified type",
	0,
	2,
	kParams_SetSpellItemVal,
	HANDLER(Cmd_SetSpellExplodes_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_SetMagicItemVal[2] = 
{
	{	"value", kParamType_Integer, 0 },
	{	"magic item", kParamType_MagicItem, 0 },
};

static ParamInfo kParams_ModMagicItemVal[2] = 
{
	{	"value", kParamType_Float, 0 },
	{	"magic item", kParamType_MagicItem, 0 },
};

CommandInfo kCommandInfo_SetEnchantmentType =
{
	"SetEnchantmentType",
	"",
	0,
	"sets the type of the enchantment",
	0,
	2,
	kParams_SetMagicItemVal,
	HANDLER(Cmd_SetEnchantmentType_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetEnchantmentCharge =
{
	"SetEnchantmentCharge",
	"",
	0,
	"sets the charge of the enchantment",
	0,
	2,
	kParams_SetMagicItemVal,
	HANDLER(Cmd_SetEnchantmentCharge_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_ModEnchantmentCharge =
{
	"ModEnchantmentCharge",
	"",
	0,
	"modifies the charge of the enchantment",
	0,
	2,
	kParams_ModMagicItemVal,
	HANDLER(Cmd_ModEnchantmentCharge_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetEnchantmentCost =
{
	"SetEnchantmentCost",
	"",
	0,
	"sets the cost of the enchantment",
	0,
	2,
	kParams_SetMagicItemVal,
	HANDLER(Cmd_SetEnchantmentCost_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_ModEnchantmentCost =
{
	"ModEnchantmentCost",
	"",
	0,
	"modifies the cost of the enchantment",
	0,
	2,
	kParams_ModMagicItemVal,
	HANDLER(Cmd_ModEnchantmentCost_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

//CommandInfo kCommandInfo_RemoveHostileEffectItems =
//{
//	"RemoveHostileEffectItems",
//	"RemHEffects",
//	0,
//	"removes all hostile effects from the magic item",
//	0,
//	1,
//	kParams_OneMagicItem,
//	HANDLER(Cmd_RemoveHostileEffectItems_Execute),
//	Cmd_Default_Parse,
//	NULL,
//	0
//};
//
//CommandInfo kCommandInfo_RemoveNonHostileEffectItems =
//{
//	"RemoveNonHostileEffectItems",
//	"RemNHEffects",
//	0,
//	"removes all hostile effects from the magic item",
//	0,
//	1,
//	kParams_OneMagicItem,
//	HANDLER(Cmd_RemoveNonHostileEffectItems_Execute),
//	Cmd_Default_Parse,
//	NULL,
//	0
//};

CommandInfo kCommandInfo_IsMagicEffectHostile =
{
	"IsMagicEffectHostile",
	"IsMEHostile",
	0,
	"returns 1 if the passed magic effect is hostile",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_IsMagicEffectHostile_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectHostileC =
{
	"IsMagicEffectHostileC",
	"IsMEHostileC",
	0,
	"returns 1 if the passed magic effect is hostile",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_IsMagicEffectHostileC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_CopyNthEffectItem[3] = 
{
	{	"from Magic Item", kParamType_MagicItem, 0 },
	{	"toMagicItem", kParamType_MagicItem, 0 },
	{	"which effect", kParamType_Integer, 1 },
};

CommandInfo kCommandInfo_CopyNthEffectItem =
{
	"CopyNthEffectItem",
	"CopyNthEI",
	0,
	"copies the specified effect item from the source to the destination and returns the index in the new magic item",
	0,
	3,
	kParams_CopyNthEffectItem,
	HANDLER(Cmd_CopyNthEffectItem_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_CopyAllEffectItems[2] = 
{
	{	"from Magic Item", kParamType_MagicItem, 0 },
	{	"toMagicItem", kParamType_MagicItem, 0 },
};

CommandInfo kCommandInfo_CopyAllEffectItems =
{
	"CopyAllEffectItems",
	"CopyAllEI",
	0,
	"copies all effect items from the source to the destination magic item",
	0,
	2,
	kParams_CopyAllEffectItems,
	HANDLER(Cmd_CopyAllEffectItems_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_AddEffectItem[2] = 
{
	{	"effect", kParamType_MagicEffect, 0 },
	{	"toMagicItem", kParamType_MagicItem, 0 },
};

static ParamInfo kParams_AddEffectItemC[2] = 
{
	{	"effect code", kParamType_Integer, 0 },
	{	"toMagicItem", kParamType_MagicItem, 0 },
};

CommandInfo kCommandInfo_AddEffectItem =
{
	"AddEffectItem",
	"AddEI",
	0,
	"adds the default effect item for the specified effect to the magic item",
	0,
	2,
	kParams_AddEffectItem,
	HANDLER(Cmd_AddEffectItem_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_AddEffectItemC =
{
	"AddEffectItemC",
	"AddEIC",
	0,
	"adds the default effect item for the specified effect to the magic item",
	0,
	2,
	kParams_AddEffectItemC,
	HANDLER(Cmd_AddEffectItemC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicItemAutoCalc =
{
	"IsMagicItemAutoCalc",
	"IsMIAutoCalc",
	0,
	"returns whether the given magic item is autocalculated",
	0,
	1,
	kParams_OneMagicItem,
	HANDLER(Cmd_IsMagicItemAutoCalc_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetMagicItemAutoCalc =
{
	"SetMagicItemAutoCalc",
	"SetMIAutoCalc",
	0,
	"specifies whether the given magic item is autocalculated",
	0,
	2,
	kParams_SetMagicItemVal,
	HANDLER(Cmd_SetMagicItemAutoCalc_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectForSpellmaking =
{
	"IsMagicEffectForSpellmaking",
	"IsMEForSpellmaking",
	0,
	"returns 1 if the passed magic effect is allowed for spellmaking",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_IsMagicEffectForSpellmaking_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectForSpellmakingC =
{
	"IsMagicEffectForSpellmakingC",
	"IsMEForSpell",
	0,
	"returns 1 if the passed magic effect is allowed for spellmaking",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_IsMagicEffectForSpellmakingC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectForEnchanting =
{
	"IsMagicEffectForEnchanting",
	"IsMEForEnchant",
	0,
	"returns 1 if the passed magic effect is allowed for enchanting",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_IsMagicEffectForEnchanting_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectForEnchantingC =
{
	"IsMagicEffectForEnchantingC",
	"IsMEForEnchantC",
	0,
	"returns 1 if the passed magic effect is allowed for enchanting",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_IsMagicEffectForEnchantingC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectDetrimental =
{
	"IsMagicEffectDetrmimental",
	"IsMEDetrimental",
	0,
	"returns 1 if the passed magic effect is detrimental",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_IsMagicEffectDetrimental_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectDetrimentalC =
{
	"IsMagicEffectDetrimentalC",
	"IsMEDetrimentalC",
	0,
	"returns 1 if the passed magic effect detrimental",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_IsMagicEffectDetrimentalC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectCanRecover =
{
	"IsMagicEffectCanRecover",
	"IsMECanRecover",
	0,
	"returns 1 if the passed magic effect is marked can recover",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_IsMagicEffectCanRecover_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectCanRecoverC =
{
	"IsMagicEffectCanRecoverC",
	"IsMECanRecoverC",
	0,
	"returns 1 if the passed magic effect is marked can recover",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_IsMagicEffectCanRecoverC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


CommandInfo kCommandInfo_IsMagicEffectMagnitudePercent =
{
	"IsMagicEffectMagnitudePercent",
	"IsMEMagnitudePercent",
	0,
	"returns 1 if the passed magic effect is marked can recover",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_IsMagicEffectMagnitudePercent_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectMagnitudePercentC =
{
	"IsMagicEffectMagnitudePercentC",
	"IsMEMagnitudePercentC",
	0,
	"returns 1 if the passed magic effect is marked can recover",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_IsMagicEffectMagnitudePercentC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectFXPersists =
{
	"MagicEffectFXPersists",
	"MEFXPersists",
	0,
	"returns 1 if the passed magic effect's graphic effects persist",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_MagicEffectFXPersists_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectFXPersistsC =
{
	"MagicEffectFXPersistsC",
	"MEFXPersistsC",
	0,
	"returns 1 if the passed magic effect's graphic effects persist",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MagicEffectFXPersistsC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectOnSelfAllowed =
{
	"IsMagicEffectOnSelfAllowed",
	"IsMEOnSelfAllowed",
	0,
	"returns 1 if the passed magic effect can target self",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_IsMagicEffectOnSelfAllowed_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectOnSelfAllowedC =
{
	"IsMagicEffectOnSelfAllowedC",
	"IsMEOnSelfAllowedC",
	0,
	"returns 1 if the passed magic effect can target self",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_IsMagicEffectOnSelfAllowedC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectOnTouchAllowed =
{
	"IsMagicEffectOnTouchAllowed",
	"IsMEOnTouchAllowed",
	0,
	"returns 1 if the passed magic effect can target touch",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_IsMagicEffectOnTouchAllowed_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectOnTouchAllowedC =
{
	"IsMagicEffectOnTouchAllowedC",
	"IsMEOnTouchAllowedC",
	0,
	"returns 1 if the passed magic effect can target ouch",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_IsMagicEffectOnTouchAllowedC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectOnTargetAllowed =
{
	"IsMagicEffectOnTargetAllowed",
	"IsMEOnTargetAllowed",
	0,
	"returns 1 if the passed magic effect can target target",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_IsMagicEffectOnTargetAllowed_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMagicEffectOnTargetAllowedC =
{
	"IsMagicEffectOnTargetAllowedC",
	"IsMEOnTargetAllowedC",
	0,
	"returns 1 if the passed magic effect can target arget",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_IsMagicEffectOnTargetAllowedC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectHasNoDuration =
{
	"MagicEffectHasNoDuration",
	"MEHasNoDuration",
	0,
	"returns 1 if the passed magic effect has no duration",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_MagicEffectHasNoDuration_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectHasNoDurationC =
{
	"MagicEffectHasNoDurationC",
	"MEHasNoDurationC",
	0,
	"returns 1 if the passed magic effect has no duration",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MagicEffectHasNoDurationC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectHasNoMagnitude =
{
	"MagicEffectHasNoMagnitude",
	"MEHasNoMagnitude",
	0,
	"returns 1 if the passed magic effect has no magnitude",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_MagicEffectHasNoMagnitude_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectHasNoMagnitudeC =
{
	"MagicEffectHasNoMagnitudeC",
	"MEHasNoMagnitudeC",
	0,
	"returns 1 if the passed magic effect has no magnitude",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MagicEffectHasNoMagnitudeC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectHasNoArea =
{
	"MagicEffectHasNoArea",
	"MEHasNoArea",
	0,
	"returns 1 if the passed magic effect has no area",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_MagicEffectHasNoArea_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectHasNoAreaC =
{
	"MagicEffectHasNoAreaC",
	"MEHasNoAreaC",
	0,
	"returns 1 if the passed magic effect has no area",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MagicEffectHasNoAreaC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectHasNoIngredient =
{
	"MagicEffectHasNoIngredient",
	"MEHasNoIngredient",
	0,
	"returns 1 if the passed magic effect has no ingredient",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_MagicEffectHasNoIngredient_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectHasNoIngredientC =
{
	"MagicEffectHasNoIngredientC",
	"MEHasNoIngredientC",
	0,
	"returns 1 if the passed magic effect has no ingredient",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MagicEffectHasNoIngredientC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectHasNoHitEffect =
{
	"MagicEffectHasNoHitEffect",
	"MEHasNoHitEffect",
	0,
	"returns 1 if the passed magic effect has no hit effect",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_MagicEffectHasNoHitEffect_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectHasNoHitEffectC =
{
	"MagicEffectHasNoHitEffectC",
	"MEHasNoHitEffectC",
	0,
	"returns 1 if the passed magic effect has no hit effect",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MagicEffectHasNoHitEffectC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectUsesWeapon =
{
	"MagicEffectUsesWeapon",
	"MEUsesWeapon",
	0,
	"returns 1 if the passed magic effect uses a weapon",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_MagicEffectUsesWeapon_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectUsesWeaponC =
{
	"MagicEffectUsesWeaponC",
	"MEUsesWeaponC",
	0,
	"returns 1 if the passed magic effect uses a weapon",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MagicEffectUsesWeaponC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectUsesArmor =
{
	"MagicEffectUsesArmor",
	"MEUsesArmor",
	0,
	"returns 1 if the passed magic effect uses armor",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_MagicEffectUsesArmor_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectUsesArmorC =
{
	"MagicEffectUsesArmorC",
	"MEUsesArmorC",
	0,
	"returns 1 if the passed magic effect uses armor",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MagicEffectUsesArmorC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectUsesCreature =
{
	"MagicEffectUsesCreature",
	"MEUsesCreature",
	0,
	"returns 1 if the passed magic effect uses a creature",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_MagicEffectUsesCreature_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectUsesCreatureC =
{
	"MagicEffectUsesCreatureC",
	"MEUsesCreatureC",
	0,
	"returns 1 if the passed magic effect uses a creature",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MagicEffectUsesCreatureC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


CommandInfo kCommandInfo_MagicEffectUsesSkill =
{
	"MagicEffectUsesSkill",
	"MEUsesSkill",
	0,
	"returns 1 if the passed magic effect uses a Skill",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_MagicEffectUsesSkill_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectUsesSkillC =
{
	"MagicEffectUsesSkillC",
	"MEUsesSkillC",
	0,
	"returns 1 if the passed magic effect uses a Skill",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MagicEffectUsesSkillC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};


CommandInfo kCommandInfo_MagicEffectUsesAttribute =
{
	"MagicEffectUsesAttribute",
	"MEUsesAttribute",
	0,
	"returns 1 if the passed magic effect uses an attribute",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_MagicEffectUsesAttribute_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectUsesAttributeC =
{
	"MagicEffectUsesAttributeC",
	"MEUsesAttributeC",
	0,
	"returns 1 if the passed magic effect uses an attribute",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MagicEffectUsesAttributeC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectUsesOtherActorValue =
{
	"MagicEffectUsesOtherActorValue",
	"MEUsesOtherAV",
	0,
	"returns 1 if the passed magic effect uses another actor value",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_MagicEffectUsesOtherActorValue_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicEffectUsesOtherActorValueC =
{
	"MagicEffectUsesOtherActorValueC",
	"MEUsesOtherAVC",
	0,
	"returns 1 if the passed magic effect uses another actor value",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MagicEffectUsesOtherActorValueC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicEffectOtherActorValue =
{
	"GetMagicEffectOtherActorValue",
	"GetMEOtherAV",
	0,
	"returns the magic effect's other actor value",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_MagicEffectUsesOtherActorValue_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicEffectOtherActorValueC =
{
	"GetMagicEffectOtherActorValueC",
	"GetMEOtherAVC",
	0,
	"returns the magic effect's other actor value",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MagicEffectUsesOtherActorValueC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicEffectUsedObject =
{
	"GetMagicEffectUsedObject",
	"GetMEUsedObject",
	0,
	"returns the magic effect's used object",
	0,
	1,
	kParams_OneMagicEffect,
	HANDLER(Cmd_MagicEffectUsesOtherActorValue_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetMagicEffectUsedObjectC =
{
	"GetMagicEffectUsedObjectC",
	"GetMEUsedObjectC",
	0,
	"returns the magic effect's used object",
	0,
	1,
	kParams_OneInt,
	HANDLER(Cmd_MagicEffectUsesOtherActorValueC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_AddFullEffectItem[6] = 
{
	{	"effect", kParamType_MagicEffect, 0 },
	{	"magnitude", kParamType_Integer, 0 },
	{	"area", kParamType_Integer, 0 },
	{	"duration", kParamType_Integer, 0 },
	{	"range", kParamType_Integer, 0 },
	{	"toMagicItem", kParamType_MagicItem, 0 },
};

CommandInfo kCommandInfo_AddFullEffectItem =
{
	"AddFullEffectItem",
	"AddFullEI",
	0,
	"adds a new effect item to the magic item and initializes the common settings",
	0,
	6,
	kParams_AddFullEffectItem,
	HANDLER(Cmd_AddFullEffectItem_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_AddFullEffectItemC[6] = 
{
	{	"effect code", kParamType_Integer, 0 },
	{	"magnitude", kParamType_Integer, 0 },
	{	"area", kParamType_Integer, 0 },
	{	"duration", kParamType_Integer, 0 },
	{	"range", kParamType_Integer, 0 },
	{	"toMagicItem", kParamType_MagicItem, 0 },
};

CommandInfo kCommandInfo_AddFullEffectItemC =
{
	"AddFullEffectItemC",
	"AddFullEIC",
	0,
	"adds a new effect item to the magic item and initializes the common settings",
	0,
	6,
	kParams_AddFullEffectItemC,
	HANDLER(Cmd_AddFullEffectItemC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_AddScriptedEffectItem[3] =
{
	{	"script", kParamType_MagicItem, 0 },
	{	"name", kParamType_String, 0 },
	{	"toMagicItem", kParamType_MagicItem, 0 },
};

CommandInfo kCommandInfo_AddScriptedEffectItem =
{
	"AddScriptedEffectItem",
	"AddScriptedEI",
	0,
	"adds a new scripted effect item to the magic item and initializes the common settings",
	0,
	3,
	kParams_AddScriptedEffectItem,
	HANDLER(Cmd_AddScriptedEffectItem_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};
