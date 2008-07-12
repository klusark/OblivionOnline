#include "Commands_Magic.h"
#include "ParamInfos.h"
#include "GameAPI.h"
#include "GameForms.h"
#include "GameObjects.h"
#include "Utilities.h"


#if OBLIVION

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
	*result = magicItem->Type();
	return true;
}

static bool GetMagicItemEffectCount(EffectItemList::Entry* entry, double* result)
{
	if (!entry || !result) return true;

	EffectItemVisitor visitor(entry);
	*result = visitor.Count();
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
				if (!enchantItem->IsAutoCalc()) {
					*result = enchantItem->cost;
				} else {
					*result = enchantItem->magicItem.list.GetMagickaCost();
				}
				break;
			}
		default:
			break;
	}
	return true;
}

static bool GetNthEffectItemValue(EffectItemList::Entry* entry, UInt32 valueType, UInt32 whichEffect, double* result)
{
	if (!entry || !result) return true;
	EffectItemVisitor visitor(entry);
	EffectItem* pEffectItem = visitor.GetNthInfo(whichEffect);
	return GetEffectItemValue(pEffectItem, valueType, result);		
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
			return GetMagicItemEffectCount(&magicItem->list.effectList, result);
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
							*result = ingredient->IsAutoCalc() ? 1 : 0;
						}
						break;
					}
				case kFormType_AlchemyItem:
					{
						AlchemyItem* alchemy = (AlchemyItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_AlchemyItem, 0);
						if (alchemy) {
							*result = alchemy->IsAutoCalc() ? 1 : 0;
						}
						break;
					}
				case kFormType_Enchantment:
					{
						EnchantmentItem* enchantment = (EnchantmentItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_EnchantmentItem, 0);
						if(enchantment) {
							*result = enchantment->IsAutoCalc() ? 1 : 0;
						}
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
			return GetNthEffectItemValue(&magicItem->list.effectList, valueType, whichEffect, result);	
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

class EffectCodeFinder
{
	UInt32 m_effectCode;
	UInt32 m_count;
	UInt32 m_avOrRefID;

	enum {
		eCodeOnly = 0,
		eCodeAndAV,
		eCodeAndScript,
	};

	UInt8 m_mode;
public:
	EffectCodeFinder(UInt32 effectCode) : m_effectCode(effectCode), m_count(0), m_avOrRefID(0), m_mode(eCodeOnly) {}
	EffectCodeFinder(UInt32 effectCode, UInt32 av) : m_effectCode(effectCode), m_count(0), m_avOrRefID(0), m_mode(eCodeOnly) {
		if (av != kActorVal_OblivionMax) {
			m_avOrRefID = av;
			m_mode = eCodeAndAV;
		}
	}

	EffectCodeFinder(UInt32 effectCode, Script* script) : m_effectCode(effectCode), m_count(0), m_avOrRefID(script->refID), m_mode(eCodeAndScript) {}
	~EffectCodeFinder() {}

	inline bool MatchesAV(EffectItem* effectItem) {
		return m_mode == eCodeAndAV && effectItem->HasActorValue() && effectItem->GetActorValue() == m_avOrRefID;
	}

	inline bool MatchesScript(EffectItem* effectItem) {
		return m_mode == eCodeAndScript && effectItem->IsScriptedEffect() && effectItem->scriptEffectInfo && effectItem->scriptEffectInfo->scriptRefID == m_avOrRefID;
	}

	bool Accept(EffectItem* effectItem) {
		if (effectItem && effectItem->effectCode == m_effectCode) {
			if  (m_mode == eCodeOnly || MatchesAV(effectItem) || MatchesScript(effectItem)) {
				m_count++;
			}
		}
		return true;
	}

	UInt32 Found() { return m_count; }
};




static bool MagicItemHasEffectCode(EffectItemList::Entry* entry, EffectCodeFinder& finder, bool bReturnCount, double* result)
{
	if (!entry || ! result) return true;

	EffectItemVisitor visitor(entry);
	visitor.Visit(finder);
	UInt32 count = finder.Found();
	if (bReturnCount) {
		*result = count;
	} else {
		*result = count != 0;
	}
	return true;
}


const bool bReturnCountT = true;
const bool bReturnCountF = false;
const bool bUsingCodeT = true;
const bool bUsingCodeF = false;

EffectItemList* GetEffectList(TESForm* form)
{
	EffectItemList* list = NULL;
	MagicItem* magicItem = (MagicItem*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_MagicItem, 0);
	if (magicItem) {
		list = &magicItem->list;
	} else {
		TESSigilStone* sigilStone = (TESSigilStone*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESSigilStone, 0);
		if (sigilStone) {
			list = &sigilStone->effectList;
		}
	}
	return list;
}

static bool MagicItemHasEffect_Execute(COMMAND_ARGS, bool bReturnCount, bool bUsingCode)
{
	*result = 0;
	UInt32 effectCode = 0;
	EffectSetting* magic = NULL;
	TESForm* form = NULL;
	UInt32 actorVal = kActorVal_OblivionMax;

	if (bUsingCode) {
		ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &effectCode, &form, &actorVal);
	} else {
		ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &magic, &form, &actorVal);
		if (magic) effectCode = magic->effectCode;
	}
	if (!form || effectCode == 0) return true;
	EffectItemList* list = GetEffectList(form);
	if (!list) return true;
	EffectCodeFinder finder(effectCode, actorVal);
	return MagicItemHasEffectCode(&list->effectList, finder, bReturnCount, result);
}

static bool Cmd_MagicItemHasEffect_Execute(COMMAND_ARGS)
{
	return MagicItemHasEffect_Execute(PASS_COMMAND_ARGS, bReturnCountF, bUsingCodeF);
}

static bool Cmd_MagicItemHasEffectCount_Execute(COMMAND_ARGS)
{
	return MagicItemHasEffect_Execute(PASS_COMMAND_ARGS, bReturnCountT, bUsingCodeF);
}


static bool Cmd_MagicItemHasEffectCode_Execute(COMMAND_ARGS)
{
	return MagicItemHasEffect_Execute(PASS_COMMAND_ARGS, bReturnCountF, bUsingCodeT);
}

static bool Cmd_MagicItemHasEffectCountCode_Execute(COMMAND_ARGS)
{
	return MagicItemHasEffect_Execute(PASS_COMMAND_ARGS, bReturnCountT, bUsingCodeT);
}

class ScriptFinder
{
	UInt32 m_refID;
public:
	ScriptFinder(UInt32 refID) : m_refID(refID) {}
	bool Accept(EffectItem* effectItem) {
		return (effectItem->ScriptEffectRefId() == m_refID);
	}
};

static bool Cmd_MagicItemHasEffectItemScript_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESForm* scriptItemVar = NULL;
	TESForm* form = NULL;
	bool bArgsExtracted = ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &scriptItemVar, &form);
	if (!bArgsExtracted || !scriptItemVar || !form) return true;
	
	EffectItemList* list = GetEffectList(form);
	if (!list) return true;

	EffectItemVisitor visitor(&list->effectList);
	ScriptFinder finder(scriptItemVar->refID);
	if (visitor.Find(finder) != NULL) {
		*result = 1;
	}
	return true;
}


static bool GetNthEffectItemValue_Execute(COMMAND_ARGS, UInt32 whichValue)
{
	*result = 0;
	UInt32 whichEffect = 0;
	TESForm* form = NULL;

	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &form, &whichEffect);
	if (!form) return true;
	EffectItemList* list = GetEffectList(form);
	if (!list) return true;
	return GetNthEffectItemValue(&list->effectList, whichValue, whichEffect, result);
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
	TESForm* form = NULL;

	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &form);
	if (!form) return true;
	EffectItemList* list = GetEffectList(form);
	return GetMagicItemEffectCount(&list->effectList, result);
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

static bool Cmd_GetSpellMagickaCost_Execute(COMMAND_ARGS)
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
					String& effectName = effect->scriptEffectInfo->effectName;
					if (cmv.ForMod()) {
						std::string strTextArg(cmv.StringVal());
						// look and see if the input has the pipe character
						std::string::size_type pipePos = strTextArg.find('|');
						if (pipePos != std::string::npos) {
							// we found the pipe
							// now look for the replacement string
							std::string toReplace(strTextArg.substr(0, pipePos));
							effectName.Replace(toReplace.c_str(), &strTextArg[pipePos+1]);
						}
					} else {
						effectName.Set(cmv.StringVal());
					}
				}
			}

		default:
			break;
	}
	return true;
}

static bool ChangeNthEffectItem(EffectItemList::Entry* entry, UInt32 whichEffect, ChangeMagicValue& cmv, double* result)
{
	EffectItemVisitor visitor(entry);
	EffectItem* pEffectItem = visitor.GetNthInfo(whichEffect);
	return ChangeEffectItemValue(pEffectItem, cmv, result);
}

static bool ChangeNthEffectItem_Execute(COMMAND_ARGS, UInt32 whichValue, bool bForMod)
{
	UInt32 nuVal = 0;
	TESForm* form = NULL;
	UInt32 whichEffect = 0;
	ChangeMagicValue cmv(whichValue, bForMod);
	bool bArgsExtracted = false;
	bool bChangeScript = (whichValue == kMagic_EffectScript);
	if (whichValue == kMagic_EffectScriptName) {
		bArgsExtracted = ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, cmv.StringPtr(), &form, &whichEffect);
	} else if (bChangeScript) {
		bArgsExtracted = ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, cmv.FormPtr(), &form, &whichEffect);
	} else if (bForMod) {
		bArgsExtracted = ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, cmv.FloatPtr(), &form, &whichEffect);
	} else {
		bArgsExtracted = ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, cmv.IntegerPtr(), &form, &whichEffect);
	}
	if (!bArgsExtracted && !bChangeScript) return true;
	EffectItemList* list = GetEffectList(form);
	if (!list) return true;
	return ChangeNthEffectItem(&list->effectList, whichEffect, cmv, result);
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

static bool Cmd_ModNthEffectItemScriptName_Execute(COMMAND_ARGS)
{
	return ChangeNthEffectItem_Execute(PASS_COMMAND_ARGS, kMagic_EffectScriptName, bForModT);
}


static bool SetNthEffectItemVisualEffect_Execute(COMMAND_ARGS, bool bUseCode)
{
	*result = 0;
	EffectSetting* magic = NULL;
	UInt32 effectCode = 0;
	TESForm* form = NULL;
	UInt32 whichEffect = 0;

	if (bUseCode) {
		ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &effectCode, &form, &whichEffect);
	} else {
		ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &magic, &form, &whichEffect);
		if (magic) effectCode = magic->effectCode;
	}
	if (!form || effectCode == 0) return true;

	EffectItemList* list = GetEffectList(form);
	if (!list) return true;
	EffectItemVisitor visitor(&list->effectList);
	EffectItem* pEffectItem = visitor.GetNthInfo(whichEffect);
	if (pEffectItem && pEffectItem->IsScriptedEffect() && pEffectItem->scriptEffectInfo) {
		pEffectItem->scriptEffectInfo->visualEffectCode = effectCode;
	}
	return true;
}

static bool Cmd_SetNthEffectItemScriptVisualEffect_Execute(COMMAND_ARGS)
{
	return SetNthEffectItemVisualEffect_Execute(PASS_COMMAND_ARGS, bUsingCodeF);
}

static bool Cmd_SetNthEffectItemScriptVisualEffectC_Execute(COMMAND_ARGS)
{
	return SetNthEffectItemVisualEffect_Execute(PASS_COMMAND_ARGS, bUsingCodeT);
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
	TESForm* form = NULL;

	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &form, &whichEffect);
	if (!form) return NULL;


	EffectItemList* list = GetEffectList(form);
	if (!list) return true;
	bool bFound = list->RemoveItem(whichEffect);
	if (bFound) {
		AlchemyItem* potion = (AlchemyItem*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_AlchemyItem, 0);
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
				TESForm* form = (TESForm*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_TESForm, 0);
				switch(form->typeID) {
					case kFormType_Spell:
						{
							SpellItem* spell = (SpellItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_SpellItem, 0);
							if (spell) {
								spell->SetAutoCalc(autoCalc==0 ? false : true);
							}
							break;
						}

					case kFormType_Ingredient:
						{
							IngredientItem* ingredient = (IngredientItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_IngredientItem, 0);
							if (ingredient) {
								ingredient->SetAutoCalc(autoCalc==0 ? false : true);
							}
							break;
						}
					case kFormType_AlchemyItem:
						{
							AlchemyItem* alchemy = (AlchemyItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_AlchemyItem, 0);
							if (alchemy) {
								alchemy->SetAutoCalc(autoCalc==0 ? false : true);
							}
							break;
						}
					case kFormType_Enchantment:
						{
							EnchantmentItem* enchantment = (EnchantmentItem*)Oblivion_DynamicCast(magicItem, 0, RTTI_MagicItem, RTTI_EnchantmentItem, 0);
							if(enchantment) {
								enchantment->SetAutoCalc(autoCalc==0 ? false : true);
							}
						}
						break;
					default:
						break;
				}
				break;
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
	TESForm* from = NULL;
	TESForm* to = NULL;
	UInt32 whichEffect = 0;
	
	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &from, &to, &whichEffect);
	if (!from || !to) return true;
	
	EffectItemList* fromList = GetEffectList(from);
	EffectItemList* toList = GetEffectList(to);
	if (!fromList || ! toList) return true;

	UInt32 nuIndex = toList->CopyItemFrom(*fromList, whichEffect);
	*result = nuIndex;

	AlchemyItem* potion = (AlchemyItem*)Oblivion_DynamicCast(to, 0, RTTI_TESForm, RTTI_AlchemyItem, 0);
	if (potion) {
		potion->UpdateHostileEffectCount();
	}

	return true;
}

static bool Cmd_CopyAllEffectItems_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESForm* from = NULL;
	TESForm* to = NULL;
	UInt32 whichEffect = 0;
	
	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &from, &to, &whichEffect);
	if (!from || !to) return true;
	
	EffectItemList* fromList = GetEffectList(from);
	EffectItemList* toList = GetEffectList(to);
	if (!fromList || ! toList) return true;
	
	UInt32 count = fromList->CountItems();
	for (UInt32 n = 0; n < count; n++) {
		toList->CopyItemFrom(*fromList, n);
	}
	AlchemyItem* potion = (AlchemyItem*)Oblivion_DynamicCast(to, 0, RTTI_TESForm, RTTI_AlchemyItem, 0);
	if (potion) {
		potion->UpdateHostileEffectCount();
	}

	return true;
}

static bool Cmd_RemoveAllEffectItems_Execute(COMMAND_ARGS)
{
	*result = 0;
	TESForm* from = NULL;
	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &from);
	if (!from) return true;

	
	EffectItemList* spellEffectList = GetEffectList(from);
	UInt32 count = spellEffectList->CountItems();
	// remove from the back
	while (count > 0) {
		spellEffectList->RemoveItem(--count);
	}
	return true;
}

static void AddEffectItem(EffectItemList* list, UInt32 magicEffectCode, double* result)
{
	EffectItem* proxyEffect = EffectItem::ProxyEffectItemFor(magicEffectCode);
	if (proxyEffect) {
		EffectItem* effectItem = proxyEffect->Clone();
		if (effectItem) {
			UInt32 nuIndex = list->AddItem(effectItem);
			*result = nuIndex;
		}
	}
}

static bool AddEffectItem_Execute(COMMAND_ARGS, bool bUsingCode)
{
	*result = -1;
	EffectSetting* magicEffect = NULL;
	UInt32 effectCode = 0;	
	TESForm* form = NULL;
	if (bUsingCode) {
		ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &effectCode, &form);
	} else {
		ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &magicEffect, &form);
		if (magicEffect) effectCode = magicEffect->effectCode;
	}
	if (!form || effectCode == 0) return true;

	EffectItemList* list = GetEffectList(form);
	if (!list) return true;
	AddEffectItem(list, effectCode, result);

	AlchemyItem* potion = (AlchemyItem*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_AlchemyItem, 0);
	if (potion) {
		potion->UpdateHostileEffectCount();
	}


	return true;
}

static bool Cmd_AddEffectItem_Execute(COMMAND_ARGS)
{
	return AddEffectItem_Execute(PASS_COMMAND_ARGS, bUsingCodeF);
}

static bool Cmd_AddEffectItemC_Execute(COMMAND_ARGS)
{
	return AddEffectItem_Execute(PASS_COMMAND_ARGS, bUsingCodeT);
}

static bool AddFullEffectItem_Execute(COMMAND_ARGS, bool bUsingCode)
{
	*result = -1;
	EffectSetting* magicEffect = NULL;
	UInt32 effectCode = 0;
	UInt32 magnitude = 0;
	UInt32 area = 0;
	UInt32 duration = 0;
	UInt32 range = 0;
	TESForm* to = NULL;
	if (bUsingCode) {
		ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &effectCode, &magnitude, &area, &duration, &range, &to);
	} else {
		ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &magicEffect, &magnitude, &area, &duration, &range, &to);
		if (magicEffect) effectCode = magicEffect->effectCode;
	}
	if (!to || effectCode == 0) return true;
	EffectItemList* list = GetEffectList(to);
	if (!list) return true;

	EffectItem* proxyEffect = EffectItem::ProxyEffectItemFor(effectCode);
	if (proxyEffect) {
		EffectItem* effectItem = proxyEffect->Clone();
		if (effectItem) {
			effectItem->SetMagnitude(magnitude);
			effectItem->SetArea(area);
			effectItem->SetDuration(duration);
			effectItem->SetRange(range);

			UInt32 nuIndex = list->AddItem(effectItem);
			*result = nuIndex;
		}
	}
	return true;
}

static bool Cmd_AddFullEffectItem_Execute(COMMAND_ARGS)
{
	return AddFullEffectItem_Execute(PASS_COMMAND_ARGS, bUsingCodeF);
}

static bool Cmd_AddFullEffectItemC_Execute(COMMAND_ARGS)
{
	return AddFullEffectItem_Execute(PASS_COMMAND_ARGS, bUsingCodeT);
}

// this function is not exposed and is a work in progress
static bool Cmd_AddScriptedEffectItem_Execute(COMMAND_ARGS)
{
	*result = -1;
	TESForm* scriptArg = NULL;
	char name[256];
	TESForm* to = NULL;

	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &scriptArg, (char**)&name, &to);
	if (!scriptArg || !to) return true;

	Script* script = (Script*)Oblivion_DynamicCast(scriptArg, 0, RTTI_TESForm, RTTI_Script, 0);
	if (!script) return true;
	EffectItemList* list = GetEffectList(to);
	if (!list) return true;

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
				UInt32 nuIndex = list->AddItem(scriptedEffect);
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

static bool Cmd_IsSpellHostile_Execute(COMMAND_ARGS)
{
	*result = 0;
	SpellItem* spell = NULL;

	if (!ExtractArgs(EXTRACT_ARGS, &spell))
		return true;

	if (spell)
		*result = spell->IsHostile() ? 1 : 0;

	return true;
}

static bool Cmd_SetSpellHostile_Execute(COMMAND_ARGS)
{
	*result = 0;
	SpellItem* spell = NULL;
	UInt32 bHostile = 0;

	if (!ExtractArgs(EXTRACT_ARGS, &spell, &bHostile))
		return true;

	if (spell)
		spell->SetHostile(bHostile ? true : false);
	return true;
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
	"GetMagicItemValue", "GetMIV",
	0,
	"gets the specified value from the given magic item",
	0, 3, kParams_GetMagicItemValue,
	HANDLER(Cmd_GetMagicItemValue_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_MagicItemHasEffect[3] = 
{
	{	"effect code", kParamType_MagicEffect, 0 },
	{	"magic item", kParamType_MagicItem, 0 },
	{	"actor value", kParamType_ActorValue, 1},
};

CommandInfo kCommandInfo_MagicItemHasEffect =
{
	"MagicItemHasEffect", "MagicHasEffect",
	0,
	"returns true if the magic item has the specified effect",
	0, 3, kParams_MagicItemHasEffect,
	HANDLER(Cmd_MagicItemHasEffect_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicItemHasEffectCount =
{
	"MagicItemHasEffectCount", "MagicHasEffectCount",
	0,
	"returns the number of effect items of the magic item with the specified effect",
	0, 3, kParams_MagicItemHasEffect,
	HANDLER(Cmd_MagicItemHasEffectCount_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_MagicItemHasEffectCode[3] = 
{
	{	"effect code", kParamType_Integer, 0 },
	{	"magic item", kParamType_MagicItem, 0 },
	{	"actor value", kParamType_Integer, 1},
};

CommandInfo kCommandInfo_MagicItemHasEffectCode =
{
	"MagicItemHasEffectCode","MagicHasEffectC",
	0,
	"returns true if the magic item has the specified effect code",
	0, 3, kParams_MagicItemHasEffectCode,
	HANDLER(Cmd_MagicItemHasEffectCode_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_MagicItemHasEffectCountCode =
{
	"MagicItemHasEffectCountCode", "MagicHasEffectCountC",
	0,
	"returns the number of effect items of the magic item with the specified effect code",
	0, 3, kParams_MagicItemHasEffectCode,
	HANDLER(Cmd_MagicItemHasEffectCountCode_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_MagicItemHasEffectItemScript[2] = 
{
	{	"script", kParamType_MagicItem, 0 },
	{	"magic item", kParamType_MagicItem, 0 },
};

CommandInfo kCommandInfo_MagicItemHasEffectItemScript =
{
	"MagicItemHasEffectItemScript", "MagicHasEffectItemScript",
	0,
	"returns 1 id the magic effect has a scripted item with the given script",
	0, 2, kParams_MagicItemHasEffectItemScript,
	HANDLER(Cmd_MagicItemHasEffectItemScript_Execute),
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
	HANDLER(Cmd_GetSpellMagickaCost_Execute),
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
	"SetNthEffectItemScriptName", "SetNthEISName",
	0,
	"sets the name of the scripted effect item",
	0, 3, kParams_SetNthEffectItemScriptName,
	HANDLER(Cmd_SetNthEffectItemScriptName_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_ModNthEffectItemScriptName =
{
	"ModNthEffectItemScriptName", "ModNthEISName",
	0,
	"modifies the name of the scripted effect item",
	0, 3, kParams_SetNthEffectItemScriptName,
	HANDLER(Cmd_ModNthEffectItemScriptName_Execute),
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

DEFINE_COMMAND(IsSpellHostile,
			   returns 1 if the spell is flagged as hostile,
			   0,
			   1,
			   kParams_OneSpellItem);

static ParamInfo kParams_SetSpellHostile[2] =
{
	{	"spell",	kParamType_SpellItem,	0	},
	{	"bool",		kParamType_Integer,		0	},
};

DEFINE_COMMAND(SetSpellHostile,
			   changes the hostile flag on the spell,
			   0,
			   2,
			   kParams_SetSpellHostile);
