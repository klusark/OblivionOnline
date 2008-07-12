#include "obse/GameForms.h"
#include "obse/GameData.h"
#include <map>
#include <functional>
#include <ctime>			//for time()
#include <string>			//for strcpy_s()
#include "InternalSerialization.h"

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
static const UInt32 TESCreature_vtbl = 0x00A1FFEC;
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
static const UInt32 TESCreature_vtbl = 0x00A5340C;
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
static const UInt32 TESCreature_vtbl = 0x00A5324C;
#else
#error unsupported Oblivion version
#endif


static const UInt32 kSEFF = Swap32('SEFF');

enum {
	kSlot_Head =		0x1 << TESBipedModelForm::kPart_Head,
	kSlot_Hair =		0x1 << TESBipedModelForm::kPart_Hair,
	kSlot_UpperBody =	0x1 << TESBipedModelForm::kPart_UpperBody,
	kSlot_LowerBody =	0x1 << TESBipedModelForm::kPart_LowerBody,
	kSlot_Hand =		0x1 << TESBipedModelForm::kPart_Hand,
	kSlot_Foot =		0x1 << TESBipedModelForm::kPart_Foot,
	kSlot_RightRing =	0x1 << TESBipedModelForm::kPart_RightRing,
	kSlot_LeftRing =	0x1 << TESBipedModelForm::kPart_LeftRing,
	kSlot_Amulet =		0x1 << TESBipedModelForm::kPart_Amulet,
	kSlot_Weapon =		0x1 << TESBipedModelForm::kPart_Weapon,
	kSlot_BackWeapon =	0x1 << TESBipedModelForm::kPart_BackWeapon,
	kSlot_SideWeapon =	0x1 << TESBipedModelForm::kPart_SideWeapon,
	kSlot_Quiver =		0x1 << TESBipedModelForm::kPart_Quiver,
	kSlot_Shield =		0x1 << TESBipedModelForm::kPart_Shield,
	kSlot_Torch =		0x1 << TESBipedModelForm::kPart_Torch,
	kSlot_Tail =		0x1 << TESBipedModelForm::kPart_Tail,
	kSlot_UpperLower =  kSlot_UpperBody | kSlot_LowerBody,
	kSlot_UpperLowerFoot = kSlot_UpperLower | kSlot_Foot,
	kSlot_UpperLowerHandFoot = kSlot_UpperLowerFoot | kSlot_Hand,
	kSlot_None = 0,
};
	
enum {
	kBit_HeavyArmor = 0x1 << TESBipedModelForm::kFlags_HeavyArmor,
	kBit_NotPlayable = 0x1 << TESBipedModelForm::kFlags_NotPlayable,
};

UInt32 TESBipedModelForm::GetSlot() const {
	switch (partMask) {
		case kSlot_Head: return kPart_Head;
		case kSlot_Hair: return kPart_Hair;
		case kSlot_UpperBody: return kPart_UpperBody;
		case kSlot_LowerBody: return kPart_LowerBody;
		case kSlot_Hand: return kPart_Hand;
		case kSlot_Foot: return kPart_Foot;
		case kSlot_RightRing: return kPart_RightRing;
		case kSlot_LeftRing: return kPart_LeftRing;
		case kSlot_Amulet: return kPart_Amulet;
		case kSlot_Weapon: return kPart_Weapon;
		case kSlot_BackWeapon: return kPart_BackWeapon;
		case kSlot_SideWeapon: return kPart_SideWeapon;
		case kSlot_Quiver: return kPart_Quiver;
		case kSlot_Shield: return kPart_Shield;
		case kSlot_Torch: return kPart_Torch;
		case kSlot_Tail: return kPart_Tail;
		// combinations
		case kSlot_UpperLower: return 18;
		case kSlot_UpperLowerFoot: return 19;
		case kSlot_UpperLowerHandFoot: return 20;
		case kSlot_None: return 255;
		default: return 0;
	}
}

void TESBipedModelForm::SetSlot(UInt32 slot)
{
	switch(slot) {
		case kPart_Head: partMask = kSlot_Head; break;
		case kPart_Hair: partMask = kSlot_Hair; break;
		case kPart_UpperBody: partMask = kSlot_UpperBody; break;
		case kPart_LowerBody: partMask = kSlot_LowerBody; break;
		case kPart_Hand: partMask = kSlot_Hand; break;
		case kPart_Foot: partMask = kSlot_Foot; break;
		case kPart_RightRing: partMask = kSlot_RightRing; break;
		case kPart_LeftRing: partMask = kSlot_LeftRing; break;
		case kPart_Amulet: partMask = kSlot_Amulet; break;
		case kPart_Weapon: partMask = kSlot_Weapon; break;
		case kPart_BackWeapon: partMask = kSlot_BackWeapon; break;
		case kPart_SideWeapon: partMask = kSlot_SideWeapon; break;
		case kPart_Quiver: partMask = kSlot_Quiver; break;
		case kPart_Shield: partMask = kSlot_Shield; break;
		case kPart_Torch: partMask = kSlot_Torch; break;
		case kPart_Tail: partMask = kSlot_Tail; break;
		case 18: partMask = kSlot_UpperLower; break;
		case 19: partMask = kSlot_UpperLowerFoot; break;
		case 20: partMask = kSlot_UpperLowerHandFoot; break;
		case 255: partMask = kSlot_None; break;
		default: break;
	}
}

bool TESBipedModelForm::IsPlayable() const
{
	return (flags & kBit_NotPlayable) == 0;
}

void TESBipedModelForm::SetPlayable(bool bPlayable)
{
	// flag is not playable, so if bPlayable clear flag
	if (bPlayable) {
		flags &= ~kBit_NotPlayable;
	} else {
		flags |= kBit_NotPlayable;
	}
}

bool TESForm::IsQuestItem() const
{
	return (flags & kFormFlags_QuestItem) != 0;
}

bool TESForm::IsCloned() const
{
	return IsClonedForm(refID);
}

UInt8 TESForm::GetModIndex()
{
	return (refID >> 24);
}

bool TESForm::SupportsSimpleModel() const
{
	switch (typeID) {
		case kFormType_NPC:
		case kFormType_Creature:
		case kFormType_LeveledCreature:
			return false;

		case kFormType_Activator:
		case kFormType_Apparatus:
		case kFormType_Armor:
		case kFormType_Book:
		case kFormType_Clothing:
		case kFormType_Container:
		case kFormType_Door:
		case kFormType_Ingredient:
		case kFormType_Light:
		case kFormType_Misc:
		case kFormType_Stat:
		case kFormType_Grass:
		case kFormType_Tree:
		case kFormType_Flora:
		case kFormType_Furniture:
		case kFormType_Weapon:
		case kFormType_Ammo:
		case kFormType_SoulGem:
		case kFormType_Key:
		case kFormType_AlchemyItem:
		case kFormType_SigilStone:
		case kFormType_LeveledItem:
		case kFormType_ANIO:
			return true;
		default:
			return false;
	}

}

void TESForm::MarkAsTemporary(void)
{
	// only tested for Script objects in 'con_bat'

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
	ThisStdCall(0x004658C0, this);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
	ThisStdCall(0x0046B490, this);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	ThisStdCall(0x0046B590, this);
#else
#error unsupported oblivion version
#endif
}

TESForm * TESForm::TryGetREFRParent(void)
{
	TESForm			* result = this;

	if(result)
	{
		TESObjectREFR	* refr = (TESObjectREFR *)Oblivion_DynamicCast(this, 0, RTTI_TESForm, RTTI_TESObjectREFR, 0);

		if(refr && refr->baseForm)
			result = refr->baseForm;
	}

	return result;
}

bool TESObjectARMO::IsHeavyArmor() const
{
	return (bipedModel.flags & kBit_HeavyArmor) != 0;
}

void TESObjectARMO::SetHeavyArmor(bool bHeavyArmor) {
	
	if (bHeavyArmor) {
		bipedModel.flags |= kBit_HeavyArmor;
	} else {
		bipedModel.flags &= ~kBit_HeavyArmor;
	}
}

MagicItem::EType MagicItem::Type() const 
{
	void* pVoid = (void*)this;
	if (Oblivion_DynamicCast(pVoid, 0, RTTI_MagicItem, RTTI_SpellItem, 0)) {
		return kType_Spell;
	} else if (Oblivion_DynamicCast(pVoid, 0, RTTI_MagicItem, RTTI_EnchantmentItem, 0)) {
		return kType_Enchantment;
	} else if (Oblivion_DynamicCast(pVoid, 0, RTTI_MagicItem, RTTI_AlchemyItem, 0)) {
		return kType_Alchemy;
	} else if (Oblivion_DynamicCast(pVoid, 0, RTTI_MagicItem, RTTI_IngredientItem, 0)) {
		return kType_Ingredient;
	}
	return kType_None;
}

bool IngredientItem::_IsFlagSet(UInt32 mask) const
{
	return (ingredFlags & mask) != 0;
}

void IngredientItem::_SetFlag(UInt32 flag, bool bSet)
{
	if (bSet) {
		ingredFlags |= flag;
	} else {
		ingredFlags &= ~flag;
	}
}

bool EnchantmentItem::MatchesType(TESForm* form)
{
	switch(enchantType) {
		case kEnchant_Scroll:
			{
				if ((TESObjectBOOK*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESObjectBOOK, 0) != NULL) {
					return true;
				}
			}
			break;
		case kEnchant_Staff:
			{
				TESObjectWEAP* weapon = (TESObjectWEAP*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESObjectWEAP, 0);
				if (weapon && weapon->type == TESObjectWEAP::kType_Staff) return true;
			}
			break;

		case kEnchant_Weapon:
			{
				TESObjectWEAP* weapon = (TESObjectWEAP*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESObjectWEAP, 0);
				if (weapon && weapon->type != TESObjectWEAP::kType_Staff) return true;
				TESAmmo* ammo = (TESAmmo*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESAmmo, 0);
				if (ammo)
					return true;
			}
			break;
		case kEnchant_Apparel:
			{
				if (Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESObjectCLOT, 0) != NULL ||
					Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESObjectARMO, 0) != NULL) {
					return true;
				}
			}
			break;

	}
	return false;
}

bool EnchantmentItem::IsAutoCalc() const 
{
	return (flags040 & kEnchant_NoAutoCalc) == 0;
}

void EnchantmentItem::SetAutoCalc(bool bAutoCalc) {
	if (bAutoCalc) {
		flags040 &= ~kEnchant_NoAutoCalc;
	} else {
		flags040 |= kEnchant_NoAutoCalc;
	}
}

bool SpellItem::IsAutoCalc() const 
{
	return (spellFlags & kFlag_NoAutoCalc) == 0;
}

void SpellItem::SetAutoCalc(bool bAutoCalc) {
	if (bAutoCalc) {
		spellFlags &= ~kFlag_NoAutoCalc;
	} else {
		spellFlags |= kFlag_NoAutoCalc;
	}
}

bool SpellItem::TouchExplodesWithNoTarget() const
{
	return (spellFlags & kFlag_TouchExplodesWithNoTarget) != 0;
}

void SpellItem::SetTouchExplodes(bool bExplodesWithNoTarget)
{
	if (bExplodesWithNoTarget) {
		spellFlags |= kFlag_TouchExplodesWithNoTarget;
	} else {
		spellFlags &= ~kFlag_TouchExplodesWithNoTarget;
	}
}

UInt32 SpellItem::GetSchool() const
{
	EffectItem* highest = magicItem.list.ItemWithHighestMagickaCost();
	if (!highest) return 0;
	return (highest->IsScriptedEffect()) ? highest->ScriptEffectSchool() : highest->setting->school;
}

static float SkillFactor(TESForm* actorCasting, UInt32 school);

UInt32 SpellItem::GetMagickaCost(TESForm *form) const
{
	bool bIsSpellType = (spellType == kType_Spell);
	UInt32 spellCost = 0;
	if (IsAutoCalc()) {
		spellCost = magicItem.list.GetMagickaCost(bIsSpellType ? form : NULL);
	} else {
		float skillFactor = 1.0;
		if (form && bIsSpellType) {
			skillFactor = SkillFactor(form, GetSchool());
			spellCost = magickaCost * skillFactor;
		} else {
			spellCost = magickaCost;
		}
	}
	return spellCost;
}

static float GetSpellLevelMin(short whichLevel)
{
	float fLevel = 0.0;
	char* settingString = NULL;
	switch(whichLevel) {
		case SpellItem::kLevel_Apprentice:
			settingString = "fMagicSpellLevelApprenticeMin";
			break;
		case SpellItem::kLevel_Journeyman:
			settingString = "fMagicSpellLevelApprenticeMin";
			break;
		case SpellItem::kLevel_Expert:
			settingString = "fMagicSpellLevelApprenticeMin";
			break;
		case SpellItem::kLevel_Master:
			settingString = "fMagicSpellLevelApprenticeMin";
			break;
		case SpellItem::kLevel_Novice:
		default:
			return fLevel;
	}
	SettingInfo* setting = NULL;
	if (GetGameSetting(settingString, &setting)) {
		fLevel = setting->f;
	}
	return fLevel;
}

UInt32 SpellItem::GetMasteryLevel() const
{
	bool bIsSpellItem = (spellType == kType_Spell);
	if (IsAutoCalc() && IsClonedForm(refID)) {
		UInt32 magickaCost = GetMagickaCost();
		if (magickaCost < GetSpellLevelMin(kLevel_Apprentice)) {
			return kLevel_Novice;
		} else if (magickaCost < GetSpellLevelMin(kLevel_Journeyman)) {
			return kLevel_Apprentice;
		} else if (magickaCost < GetSpellLevelMin(kLevel_Expert)) {
			return kLevel_Journeyman;
		} else if (magickaCost < GetSpellLevelMin(kLevel_Master)) {
			return kLevel_Expert;
		} else {
			return kLevel_Master;
		}
	} else {
		return masteryLevel;
	}
}


TESSkill* TESSkill::SkillForActorVal(UInt32 valSkill)
{
	return &(*g_dataHandler)->skills[valSkill-kSkill_Armorer];
}


Script::RefVariable	* Script::GetVariable(UInt32 reqIdx)
{
	UInt32	idx = 1;	// yes, really starts at 1
	for(RefListEntry * entry = &refList; entry; entry = entry->next)
	{
		if(idx == reqIdx)
			return entry->var;

		idx++;
	}

	return NULL;
}

UInt32 Script::AddVariable(TESForm * form)
{
	UInt32		resultIdx = 1;

	RefVariable	* var = new RefVariable;

	var->unk0 = 0;
	var->unk1 = 0;
	var->form = form;
	var->varIdx = 0;

	if(!refList.var)
	{
		// adding the first object
		refList.var = var;
		refList.next = NULL;
	}
	else
	{
		resultIdx++;

		// find the last RefListEntry
		RefListEntry	* entry;
		for(entry = &refList; entry->next; entry = entry->next, resultIdx++) ;

		RefListEntry	* newEntry = new RefListEntry;

		newEntry->var = var;
		newEntry->next = NULL;

		entry->next = newEntry;
	}

	info.numRefs = resultIdx + 1;

	return resultIdx;
}

void Script::CleanupVariables(void)
{
	delete refList.var;

	RefListEntry	* entry = refList.next;
	while(entry)
	{
		RefListEntry	* next = entry->next;

		delete entry->var;
		delete entry;

		entry = next;
	}
}

void Script::RefVariable::Resolve(ScriptEventList * eventList)
{
	if(varIdx && eventList)
	{
		ScriptEventList::Var	* var = eventList->GetVariable(varIdx);
		if(var)
		{
			UInt32	refID = *((UInt32 *)&var->data);
			form = LookupFormByID(refID);
		}
	}
}

void Script::Constructor(void)
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
	ThisStdCall(0x004F05D0, this);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
	ThisStdCall(0x004FBBF0, this);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	ThisStdCall(0x004FBAA0, this);
#else
#error unsupported oblivion version
#endif
}

void Script::StaticDestructor(void)
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
	ThisStdCall(0x004F24B0, this);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
	ThisStdCall(0x004FCAD0, this);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	ThisStdCall(0x004FC980, this);
#else
#error unsupported oblivion version
#endif
}

void Script::SetText(const char * buf)
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
	ThisStdCall(0x004EFA60, this, buf);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
	ThisStdCall(0x004F9EE0, this, buf);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	ThisStdCall(0x004F9E50, this, buf);
#else
#error unsupported oblivion version
#endif
}

void Script::CompileAndRun(void * unk0, UInt32 unk1, void * unk2)
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
	ThisStdCall(0x004F1A20, this, unk0, unk1, unk2);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
	ThisStdCall(0x004FC050, this, unk0, unk1, unk2);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	ThisStdCall(0x004FBF00, this, unk0, unk1, unk2);
#else
#error unsupported oblivion version
#endif
}

TESForm* TESSpellList::GetNthSpell(UInt32 whichSpell) const
{
	SpellListVisitor visitor(&spellList);
	return visitor.GetNthInfo(whichSpell);
}

UInt32 TESSpellList::RemoveAllSpells()
{
	UInt32 nRemoved = 0;

	Entry* curEntry = NULL;
	Entry* nextEntry = spellList.next;
	spellList.next = NULL;
	while (nextEntry) {
		// cache the current entry
		curEntry = nextEntry;
		// figure out the next entry
		nextEntry = nextEntry->next;
		// get rid of the current entry
		FormHeap_Free(curEntry);
		nRemoved++;
	}
	if (spellList.type) {
		spellList.type = NULL;
		nRemoved++;
	}
	return nRemoved;
}


bool EffectSetting::ForSpellmaking() const
{
	return (effectFlags & kEffect_ForSpellmaking) != 0;
}

bool EffectSetting::ForEnchanting() const
{
	return (effectFlags & kEffect_ForEnchanting) != 0;
}

bool EffectSetting::IsHostile() const
{
	return (effectFlags & kEffect_IsHostile) != 0;
}

bool EffectSetting::IsDetrimental() const
{
	return (effectFlags & kEffect_IsDetrimental) != 0;
}

bool EffectSetting::CanRecover() const
{
	return (effectFlags & kEffect_CanRecover) != 0;
}

bool EffectSetting::MagnitudeIsPercent() const
{
	return (effectFlags & kEffect_MagnitudePercent) != 0;
}

bool EffectSetting::FXPersists() const
{
	return (effectFlags & kEffect_FXPersists) != 0;
}

bool EffectSetting::OnSelfAllowed() const
{
	return (effectFlags & kEffect_OnSelfAllowed) != 0;
}

bool EffectSetting::OnTouchAllowed() const
{
	return (effectFlags & kEffect_OnTouchAllowed) != 0;
}

bool EffectSetting::OnTargetAllowed() const
{
	return (effectFlags & kEffect_OnTargetAllowed) != 0;
}

bool EffectSetting::NoDuration() const
{
	return (effectFlags & kEffect_NoDuration) != 0;
}

bool EffectSetting::NoMagnitude() const
{
	return (effectFlags & kEffect_NoMagnitude) != 0;
}

bool EffectSetting::NoArea() const
{
	return (effectFlags & kEffect_NoArea) != 0;
}

bool EffectSetting::NoIngredient() const
{
	return (effectFlags & kEffect_NoIngredient) != 0;
}

bool EffectSetting::UseWeapon() const
{
	return (effectFlags & kEffect_UseWeapon) != 0;
}

bool EffectSetting::UseArmor() const
{
	return (effectFlags & kEffect_UseArmor) != 0;
}

bool EffectSetting::UseCreature() const
{
	return (effectFlags & kEffect_UseCreature) != 0;
}

bool EffectSetting::UseSkill() const
{
	return (effectFlags & kEffect_UseSkill) != 0;
}

bool EffectSetting::UseAttribute() const
{
	return (effectFlags & kEffect_UseAttribute) != 0;
}

bool EffectSetting::UseOtherActorValue() const
{
	return (effectFlags & kEffect_UseActorValue) != 0;
}

bool EffectSetting::NoRecast() const
{
	return false;
}

bool EffectSetting::NoHitEffect() const
{
	return (effectFlags & kEffect_NoHitEffect) != 0;
}

// static
UInt32 EffectSetting::RefIdForC(UInt32 effectCode)
{
	EffectSetting* magicEffect = EffectSettingForC(effectCode);
	return (magicEffect) ? magicEffect->refID : 0;
}

// static
EffectSetting* EffectSetting::EffectSettingForC(UInt32 effectCode)
{
	EffectSetting* magicEffect = g_EffectSettingCollection->Lookup(effectCode);
	return magicEffect;
}

EffectItem::EffectItem() {}
EffectItem::~EffectItem() {}

bool EffectItem::HasActorValue() const
{
	return setting->UseAttribute() || setting->UseSkill();
}

bool EffectItem::IsValidActorValue(UInt32 actorValue) const
{
	if (setting->UseAttribute()) {
		return actorValue <= kActorVal_Luck;
	} else if (setting->UseSkill()) {
		return actorValue >= kActorVal_Armorer && actorValue <= kActorVal_Speechcraft; 
	} else return false;
}

UInt32 EffectItem::GetActorValue() const
{
	return (HasActorValue()) ? actorValueOrOther : 256;
}

void EffectItem::SetActorValue(UInt32 actorValue)
{
	if (HasActorValue() && IsValidActorValue(actorValue)) {
		actorValueOrOther = actorValue;
	}
}

void EffectItem::CopyFrom(const EffectItem* copyFrom)
{
	effectCode = copyFrom->effectCode;
	magnitude = copyFrom->magnitude;
	area = copyFrom->area;
	duration = copyFrom->duration;
	range = copyFrom->range;
	actorValueOrOther = copyFrom->actorValueOrOther;
	scriptEffectInfo = NULL;
	if (copyFrom->IsScriptedEffect()) {

		scriptEffectInfo = copyFrom->scriptEffectInfo->Clone();
	}
	setting = copyFrom->setting;
	cost = copyFrom->cost;
}

EffectItem* EffectItem::Clone() const
{
	EffectItem* clone = NULL;
	//if (effectCode != kSEFF) {
		clone = EffectItem::Create();
		if (clone) {
			clone->CopyFrom(this);
		}
	//}
	return clone;
}

// static
EffectItem* EffectItem::Create()
{
	UInt32 size = sizeof(EffectItem);
	void* memory = FormHeap_Allocate(size);
	memset(memory, 0, size);
	// assume no vtabl
	EffectItem* effectItem = (EffectItem*)memory;
	return effectItem;
}

EffectItem::ScriptEffectInfo* EffectItem::ScriptEffectInfo::Clone() const
{
	ScriptEffectInfo* clone = Create();
	if (clone) {
		clone->CopyFrom(this);
	}
	return clone;
}

EffectItem::ScriptEffectInfo* EffectItem::ScriptEffectInfo::Create()
{
	UInt32 size = sizeof(ScriptEffectInfo);
	void* memory = FormHeap_Allocate(size);
	memset(memory, 0, size);
	// assume no vtable
	ScriptEffectInfo* scriptEffectInfo = (ScriptEffectInfo*)memory;
	scriptEffectInfo->SetName("Unknown");
	return scriptEffectInfo;
}

void EffectItem::ScriptEffectInfo::CopyFrom(const EffectItem::ScriptEffectInfo* copyFrom)
{
	if (copyFrom) {
		scriptRefID = copyFrom->scriptRefID;
		school = copyFrom->school;
		effectName.Set(copyFrom->effectName.m_data);
		isHostile = copyFrom->isHostile;
	}
}

void EffectItem::ScriptEffectInfo::SetName(const char*name)
{
	effectName.Set(name);
}

void EffectItem::ScriptEffectInfo::SetSchool(UInt32 nuSchool)
{
	// both the actor value enums and the effect setting enums should work
	// if we're given something in or above the actor value numbers, adjust them
	// into the effect setting range
	if (nuSchool >= kActorVal_Alteration) {
		nuSchool -= kActorVal_Alteration;
	}
	// only set if the value is one of the schools
	if (nuSchool <= EffectSetting::kEffect_Restoration) {
		school = nuSchool;
	}
}

void EffectItem::ScriptEffectInfo::SetVisualEffectCode(UInt32 code)
{
	visualEffectCode = code;
}

void EffectItem::ScriptEffectInfo::SetScriptRefID(UInt32 refID)
{
	scriptRefID = refID;
}

void EffectItem::ScriptEffectInfo::SetIsHostile(bool bIsHostile)
{
	isHostile = (bIsHostile) ? 1 : 0;
}

bool EffectItem::ScriptEffectInfo::IsHostile() const
{
	return (isHostile == 1) ? true : false;
}

bool EffectItem::operator<(EffectItem* rhs) const
{
	return effectCode < rhs->effectCode;
}

typedef std::map<UInt32, EffectItem> EffectProxyMap;
typedef std::pair<UInt32, EffectItem> EffectProxyPair;
static EffectProxyMap gProxyMap;

void InitProxy(UInt32 effectCode, UInt32 magnitude, UInt32 area, UInt32 duration, UInt32 range, UInt32 actorValueOrOther, float cost)
{
	EffectSetting* magicEffect = EffectSetting::EffectSettingForC(effectCode);
	if (magicEffect) {
		EffectItem effectItem;
		effectItem.effectCode = effectCode;
		effectItem.magnitude = magnitude;
		effectItem.area = area;
		effectItem.duration = duration;
		effectItem.range = range;
		effectItem.actorValueOrOther = actorValueOrOther;
		effectItem.scriptEffectInfo = NULL;
		effectItem.setting = magicEffect;
		effectItem.cost = cost;

		if (effectCode == Swap32('SEFF')) {
			effectItem.scriptEffectInfo = EffectItem::ScriptEffectInfo::Create();
		}
		gProxyMap.insert(EffectProxyPair(effectCode, effectItem));
	}
}

void InitProxyMap() 
{
//	InitProxy(Swap32('Code'),	Mag,Area,Dur,Range,	AVorOther,	Unk1);	// RestoreHealth

	InitProxy(Swap32('ABAT'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Luck, -1);	//	Absorb Attribute
	InitProxy(Swap32('ABFA'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Fatigue, -1);	//  Absorb Fatigue
	InitProxy(Swap32('ABHE'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Health, -1);	//  Absorb Health
	InitProxy(Swap32('ABSK'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Acrobatics, -1);	//  Absorb Skill
	InitProxy(Swap32('ABSP'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Magicka, -1);	//  Absorb Spell Points
	InitProxy(Swap32('BA01'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BA02'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BA03'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BA04'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BA05'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BA06'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BA07'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BA08'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BA09'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BA10'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BABO'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Bound Boots
	InitProxy(Swap32('BACU'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Bound Cuirass
	InitProxy(Swap32('BAGA'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Bound Gauntlets
	InitProxy(Swap32('BAGR'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Bound Greaves
	InitProxy(Swap32('BAHE'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Bound Helmet
	InitProxy(Swap32('BASH'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Bound Shield
	InitProxy(Swap32('BRDN'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Encumbrance, -1);	//  Burden
	InitProxy(Swap32('BW01'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BW02'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BW03'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BW04'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BW05'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BW06'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BW07'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BW08'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('BWAX'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Bound Axe
	InitProxy(Swap32('BWBO'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Bound Bow
	InitProxy(Swap32('BWDA'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Bound Dagger
	InitProxy(Swap32('BWMA'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Bound Mace
	InitProxy(Swap32('BWSW'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Bound Sword
	InitProxy(Swap32('CALM'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Aggression, -1);	//  Calm
	InitProxy(Swap32('CHML'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_Chameleon, -1);	//  Chameleon
	InitProxy(Swap32('CHRM'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Personality, -1);	//  Charm
	InitProxy(Swap32('COCR'),	0, 0, 0,	EffectItem::kRange_Touch, 0x8, -1);	//  Command Creature
	InitProxy(Swap32('COHU'),	0, 0, 0,	EffectItem::kRange_Touch, 0x8, -1);	//  Command Humanoid
	InitProxy(Swap32('CUDI'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Cure Disease
	InitProxy(Swap32('CUPA'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Cure Paralysis
	InitProxy(Swap32('CUPO'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Cure Poison
	InitProxy(Swap32('DARK'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_Darkness, -1);	//  Darkness - DO NOT USE
	InitProxy(Swap32('DEMO'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Confidence, -1);	//  Demoralize
	InitProxy(Swap32('DGAT'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Luck, -1);	//  Damage Attribute
	InitProxy(Swap32('DGFA'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Fatigue, -1);	//  Damage Fatigue
	InitProxy(Swap32('DGHE'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Health, -1);	//  Damage Health
	InitProxy(Swap32('DGSP'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Magicka, -1);	//  Damage Spell Points
	InitProxy(Swap32('DIAR'),	0, 0, 0,	EffectItem::kRange_Touch, 0x8, -1);	//  Disintegrate Armor
	InitProxy(Swap32('DIWE'),	0, 0, 0,	EffectItem::kRange_Touch, 0x8, -1);	//  Disintegrate Weapons
	InitProxy(Swap32('DRAT'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Luck, -1);	//  Drain Attribute
	InitProxy(Swap32('DRFA'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Fatigue, -1);	//  Drain Fatigue
	InitProxy(Swap32('DRHE'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Health, -1);	//  Drain Health
	InitProxy(Swap32('DRSK'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Acrobatics, -1);	//  Drain Skill
	InitProxy(Swap32('DRSP'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Magicka, -1);	//  Drain Spell Points
	InitProxy(Swap32('DSPL'),	0, 0, 0,	EffectItem::kRange_Touch, 0x8, -1);	//  Dispel
	InitProxy(Swap32('DTCT'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_DetectLifeRange, -1);	//  Detect Life
	InitProxy(Swap32('DUMY'), 0, 0, 0, EffectItem::kRange_Self, 0x8, -1);	//  Mehrunes Dagon Custom Effect
	InitProxy(Swap32('FIDG'),	0, 0, 0,	EffectItem::kRange_Touch, 0x8, -1);	//  Fire Damage
	InitProxy(Swap32('FISH'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_ResistFire, -1);	//  Fire Shield
	InitProxy(Swap32('FOAT'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_Luck, -1);	//  Fortify Attribute
	InitProxy(Swap32('FOFA'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_Fatigue, -1);	//  Fortify Fatigue
	InitProxy(Swap32('FOHE'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_Health, -1);	//  Fortify Health
	InitProxy(Swap32('FOMM'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_MagickaMultiplier, -1);	//  Fortify Magicka Multiplier
	InitProxy(Swap32('FOSK'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_Acrobatics, -1);	//  Fortify Skill
	InitProxy(Swap32('FOSP'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_Magicka, -1);	//  Fortify Spell Points
	InitProxy(Swap32('FRDG'),	0, 0, 0,	EffectItem::kRange_Touch, 0x8, -1);	//  Frost Damage
	InitProxy(Swap32('FRNZ'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Aggression, -1);	//  Frenzy
	InitProxy(Swap32('FRSH'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_ResistFrost, -1);	//  Frost Shield
	InitProxy(Swap32('FTHR'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_Encumbrance, -1);	//  Feather
	InitProxy(Swap32('INVI'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_Invisibility, -1);	//  Invisibility
	InitProxy(Swap32('LGHT'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Light
	InitProxy(Swap32('LISH'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_ResistShock, -1);	//  Lightning Shield
	InitProxy(Swap32('LOCK'),	0, 0, 0,	EffectItem::kRange_Touch, 0x8, -1);	//  Lock - DO NOT USE
	InitProxy(Swap32('MYHL'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Mythic Dawn Helm
	InitProxy(Swap32('MYTH'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Mythic Dawn Armor
	InitProxy(Swap32('NEYE'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_NightEyeBonus, -1);	//  Night-Eye
	InitProxy(Swap32('OPEN'),	0, 0, 0,	EffectItem::kRange_Target, 0x8, -1);	//  Open
	InitProxy(Swap32('PARA'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Paralysis, -1);	//  Paralyze
	InitProxy(Swap32('POSN'), 0, 0, 0, EffectItem::kRange_Touch, 0x8, -1);	//  Poison Info
	InitProxy(Swap32('RALY'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Confidence, -1);	//  Rally
	InitProxy(Swap32('REAN'),	0, 0, 0,	EffectItem::kRange_Touch, 0x8, -1);	//  Reanimate
	InitProxy(Swap32('REAT'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_Luck, -1);	//  Restore Attribute
	InitProxy(Swap32('REDG'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_ReflectDamage, -1);	//  Reflect Damage
	InitProxy(Swap32('REFA'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_Fatigue, -1);	//  Restore Fatigue
	InitProxy(Swap32('REHE'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	// RestoreHealth
	InitProxy(Swap32('RESP'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_Magicka, -1);	//  Restore Spell Points
	InitProxy(Swap32('RFLC'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_SpellReflectChance, -1);	//  Reflect
	InitProxy(Swap32('RSDI'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_ResistDisease, -1);	//  Resist Disease
	InitProxy(Swap32('RSFI'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_ResistFire, -1);	//  Resist Fire
	InitProxy(Swap32('RSFR'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_ResistFrost, -1);	//  Resist Frost
	InitProxy(Swap32('RSMA'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_ResistMagic, -1);	//  Resist Magic
	InitProxy(Swap32('RSNW'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_ResistNormalWeapons, -1);	//  Resist Normal Weapons
	InitProxy(Swap32('RSPA'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_ResistParalysis, -1);	//  Resist Paralysis
	InitProxy(Swap32('RSPO'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_ResistPoison, -1);	//  Resist Poison
	InitProxy(Swap32('RSSH'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_ResistShock, -1);	//  Resist Shock
	InitProxy(Swap32('RSWD'), 0, 0, 0, EffectItem::kRange_Self, 0x8, -1);	//  Resist Water Damage
	InitProxy(Swap32('SABS'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_SpellAbsorbChance, -1);	//  Spell Absorption
	InitProxy(Swap32('SEFF'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Script Effect
	InitProxy(Swap32('SHDG'),	0, 0, 0,	EffectItem::kRange_Touch, 0x8, -1);	//  Shock Damage
	InitProxy(Swap32('SHLD'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_DefendBonus, -1);	//  Shield
	InitProxy(Swap32('SLNC'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_Silence, -1);	//  Silence
	InitProxy(Swap32('STMA'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_StuntedMagicka, -1);	//  Stunted Magicka
	InitProxy(Swap32('STRP'),	0, 0, 0,	EffectItem::kRange_Touch, 0x8, -1);	//  Soul Trap
	InitProxy(Swap32('SUDG'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Sun Damage
	InitProxy(Swap32('TELE'),	0, 0, 0,	EffectItem::kRange_Target, 0x8, -1);	//  Telekinesis
	InitProxy(Swap32('TURN'),	0, 0, 0,	EffectItem::kRange_Touch, 0x8, -1);	//  Turn Undead
	InitProxy(Swap32('VAMP'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Vampirism
	InitProxy(Swap32('WABR'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_WaterBreathing, -1);	//  Water Breathing
	InitProxy(Swap32('WAWA'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_WaterWalking, -1);	//  Water Walking
	InitProxy(Swap32('WKDI'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_ResistDisease, -1);	//  Weakness to Disease
	InitProxy(Swap32('WKFI'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_ResistFire, -1);	//  Weakness to Fire
	InitProxy(Swap32('WKFR'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_ResistFrost, -1);	//  Weakness to Frost
	InitProxy(Swap32('WKMA'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_ResistMagic, -1);	//  Weakness to Magic
	InitProxy(Swap32('WKNW'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_ResistNormalWeapons, -1);	//  Weakness to Normal Weapons
	InitProxy(Swap32('WKPO'),	0, 0, 0,	EffectItem::kRange_Touch, kActorVal_ResistPoison, -1);	//  Weakness to Poison
	InitProxy(Swap32('WKSH'),	0, 0, 0,	EffectItem::kRange_Self, kActorVal_ResistShock, -1);	//  Weakness to Shock
	InitProxy(Swap32('Z001'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Rufio's Ghost (Extra Summon 01)
	InitProxy(Swap32('Z002'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Ancestor Guardian (Extra Summon 02)
	InitProxy(Swap32('Z003'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Extra Summon 03
	InitProxy(Swap32('Z004'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Extra Summon 04
	InitProxy(Swap32('Z005'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Bear (Extra Summon 05)
	InitProxy(Swap32('Z006'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('Z007'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('Z008'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('Z009'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('Z010'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('Z011'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('Z012'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('Z013'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('Z014'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('Z015'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('Z016'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('Z017'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('Z018'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('Z019'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('Z020'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  
	InitProxy(Swap32('ZCLA'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Clannfear
	InitProxy(Swap32('ZDAE'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Daedroth
	InitProxy(Swap32('ZDRE'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Dremora
	InitProxy(Swap32('ZDRL'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Dremora Lord
	InitProxy(Swap32('ZFIA'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Fire Atronach
	InitProxy(Swap32('ZFRA'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Frost Atronach
	InitProxy(Swap32('ZGHO'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Ghost
	InitProxy(Swap32('ZHDZ'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Headless Zombie
	InitProxy(Swap32('ZLIC'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Lich
	InitProxy(Swap32('ZSCA'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Scamp
	InitProxy(Swap32('ZSKA'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Skeleton
	InitProxy(Swap32('ZSKC'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Skeleton Archer
	InitProxy(Swap32('ZSKE'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Skeleton Champion
	InitProxy(Swap32('ZSKH'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Skeleton Hero
	InitProxy(Swap32('ZSPD'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Spider Daedra
	InitProxy(Swap32('ZSTA'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Storm Atronach
	InitProxy(Swap32('ZWRA'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Wraith
	InitProxy(Swap32('ZWRL'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Wraith Lord
	InitProxy(Swap32('ZXIV'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Xivilai
	InitProxy(Swap32('ZZOM'),	0, 0, 0,	EffectItem::kRange_Self, 0x8, -1);	//  Summon Zombie		

	//Currently unavailable magic effects
	//InitProxy(Swap32('MODB'), 0, 0, 0, EffectItem::kRange_Self, 0x8, -1);	//  
	//InitProxy(Swap32('MODL'), 0, 0, 0, EffectItem::kRange_Self, 0x8, -1);	//  
	//InitProxy(Swap32('MODT'), 0, 0, 0, EffectItem::kRange_Self, 0x8, -1);	//  

}

EffectItem* EffectItem::ProxyEffectItemFor(UInt32 effectCode) {
	if (gProxyMap.empty()) {
		InitProxyMap();
	}
	EffectProxyMap::iterator it = gProxyMap.find(effectCode);
	if (it != gProxyMap.end()) {
		return &it->second;
	} else {
		return NULL;
	}
}

bool EffectItem::IsScriptedEffect() const
{
	return effectCode == kSEFF;
}

UInt32 EffectItem::ScriptEffectRefId() const
{
	return (scriptEffectInfo) ? scriptEffectInfo->scriptRefID : 0;
}

UInt32 EffectItem::ScriptEffectSchool() const
{
	return (scriptEffectInfo) ? scriptEffectInfo->school : 0;
}

UInt32 EffectItem::ScriptEffectVisualEffectCode() const
{
	return (scriptEffectInfo) ? scriptEffectInfo->visualEffectCode : 0;
}

bool EffectItem::IsScriptEffectHostile() const
{
	return (scriptEffectInfo) ? scriptEffectInfo->IsHostile() : false;
}

bool EffectItem::IsHostile() const
{
	if (IsScriptedEffect()) {
		return IsScriptEffectHostile();
	} else if (setting) {
		return setting->IsHostile();
	} else return false;
}

static float AreaFactor(UInt32 area)
{
	float areaFactor = 1.0;
	SettingInfo* setting = NULL;
	if (GetGameSetting("fMagicAreaBaseCostMult", &setting)) {
		float areaMult = setting->f;
		areaFactor = areaMult * area;
		if (areaFactor < 1.0) {
			areaFactor = 1.0;
		}
	}
	return areaFactor;
}

static float DurationModifier(UInt32 duration)
{
	if (duration == 0) {
		duration = 1.0;
	}
	float durationModifier = 1.0;
	SettingInfo* setting = NULL;
	if (GetGameSetting("fMagicDurMagBaseCostMult", &setting)) {
		durationModifier = setting->f * duration;
	}
	return durationModifier;
}

static float EffectBaseCost(UInt32 effectCode)
{
	float baseCost = 1.0;
	EffectSetting* effect = EffectSetting::EffectSettingForC(effectCode);
	if (effect) {
		baseCost = effect->baseCost;
	}
	return baseCost;
}

static float DurationFactor(UInt32 duration, UInt32 magnitude, float effectBaseCost)
{
	float durationFactor = 1.0;
	float durationModifier = DurationModifier(duration);
	float magnitudeModifier = 1.0;
	SettingInfo* setting = NULL;
	if (GetGameSetting("fMagicCostScale", &setting)) {
		if (magnitude == 0) {
			magnitude = 1;
		}
		magnitudeModifier = pow(magnitude,setting->f);
	}
	durationFactor = effectBaseCost * durationModifier * magnitudeModifier;
	return durationFactor;
}

static float RangeFactor(UInt32 range) {
	float rangeFactor = 1.0;
	if (range == EffectItem::kRange_Target) {
		SettingInfo* setting = NULL;
		if (GetGameSetting("fMagicRangeTargetCostMult", &setting)) {
			rangeFactor = setting->f;
		}
	}
	return rangeFactor;
}

static float SkillFactor(TESForm* actorCasting, UInt32 school)
{
	if (!actorCasting) return 1.0;
	
	float skillFactor = 1.0;
	UInt8 skillLevel = 0;
	UInt8 luck = 0;

	switch(actorCasting->typeID) {
		case kFormType_Creature:
			{
				TESCreature* creature = (TESCreature*)(EffectSetting*)Oblivion_DynamicCast(actorCasting, 0, RTTI_TESForm, RTTI_TESCreature, 0);
				if (creature) {
					skillLevel = creature->magicSkill;
					luck = creature->attributes.attr[kActorVal_Luck];
				}
				break;
			}
		case kFormType_NPC:
			{
				TESNPC* npc = (TESNPC*)(EffectSetting*)Oblivion_DynamicCast(actorCasting, 0, RTTI_TESForm, RTTI_TESNPC, 0);
				if (npc) {
					static UInt8 offset = kActorVal_Alteration - kActorVal_Armorer;
					skillLevel = npc->skillLevels[school + offset];
					luck = npc->attributes.attr[kActorVal_Luck];
				}
				break;
			}
		default:
			return 1.0;
	}

	float fMagicCasterSkillCostBase = 1.0;
	float fMagicCasterSkillCostMult = 1.0;
	float fActorLuckSkillMult = .4;	// default value in the CS

	SettingInfo* setting = NULL;
	if (GetGameSetting("fMagicCasterSkillCostBase", &setting)) {
		fMagicCasterSkillCostBase = setting->f;
	}

	if (GetGameSetting("fMagicCasterSkillCostMult", &setting)) {
		fMagicCasterSkillCostMult = setting->f;
	}
	
	if (GetGameSetting("fActorLuckSkillMult", &setting)) {
		fActorLuckSkillMult = setting->f;
	}

	// adjust skill level for luck, but cap at 100
	skillLevel += (SInt8)((luck - 50) * fActorLuckSkillMult);
	if (skillLevel > 100) skillLevel = 100;

	skillFactor = fMagicCasterSkillCostBase + ((1.0 - skillLevel*.01) * fMagicCasterSkillCostMult);
	return skillFactor;
}

float EffectItem::MagickaCost(TESForm *actorCasting) const
{
	float areaFactor = AreaFactor(area);
	float durationFactor = DurationFactor(duration, magnitude, setting->baseCost);
	float rangeFactor = RangeFactor(range);
	float skillFactor = SkillFactor(actorCasting, setting->school);
	float magickaCost = areaFactor * durationFactor * rangeFactor * skillFactor;
	if (actorCasting && magickaCost < 1) magickaCost = 1;
	return magickaCost;
}

void EffectItem::SetMagnitude(UInt32 mag)
{
	magnitude = mag;
}

void EffectItem::ModMagnitude(float modBy)
{
	magnitude = SafeModUInt32(magnitude, modBy);
}

void EffectItem::SetArea(UInt32 _area)
{
	area = _area;
}

void EffectItem::ModArea(float modBy)
{
	area = SafeModUInt32(area, modBy);
}

void EffectItem::SetDuration(UInt32 dur)
{
	duration = dur;
}

void EffectItem::ModDuration(float modBy)
{
	duration = SafeModUInt32(duration, modBy);
}

void EffectItem::SetRange(UInt32 _range)
{
	if (_range <= kRange_Target) {
		range = _range;
	}
}

bool EffectItemList::RemoveItem(UInt32 whichItem)
{
	Entry* entry = &effectList;
	Entry* entryToDelete = NULL;
	EffectItem* effectItemToDelete = NULL;
	UInt32 prevItem = whichItem-1;
	bool bFound = false;
	if (whichItem == 0) {
		effectItemToDelete = entry->effectItem;
		entryToDelete = entry->next;
		entry->effectItem = (entryToDelete) ? entryToDelete->effectItem : NULL;
		entry->next = (entryToDelete) ? entryToDelete->next : NULL;
		bFound = true;
	} else {
		UInt32 count = 0;
		while (entry && count < prevItem) {
			entry = entry->next;
			count++;
		}
		if (count < prevItem || entry->next == NULL) {
			// there were not enough items in the list
			bFound = false;
		} else {
			//we found item before the one to remove and it does have a next
			entryToDelete = entry->next;
			effectItemToDelete = entryToDelete->effectItem;
			entry->next = entryToDelete->next;
			bFound = true;
		}
	}
	if (bFound) {
		if (entryToDelete) {
			FormHeap_Free(entryToDelete);
		}
		if (effectItemToDelete) {
			FormHeap_Free(effectItemToDelete);
		}
	}
	return bFound;
}

static const bool kFindHostile = true;
static const bool kFindNonHostile = false;
class HostileItemCounter {
	UInt32 m_count;
	bool m_bFindHostile;
	bool m_bStopAtFirst;
public:
	HostileItemCounter(bool bFindHostile, bool bStopAtFirst) : 
		m_count(0), m_bFindHostile(bFindHostile), m_bStopAtFirst(bStopAtFirst) { }
	
	bool Accept(EffectItem* pEffectItem) {
		if (pEffectItem && pEffectItem->IsHostile() == m_bFindHostile) {
			m_count++;
			if (m_bStopAtFirst) return false;
		}
		return true;
	}

	UInt32 Count() const { return m_count; }
};

bool EffectItemList::HasNonHostileItem() const
{
	EffectItemVisitor visitor(&effectList);
	HostileItemCounter countNonHostile(kFindNonHostile, true);
	visitor.Visit(countNonHostile);
	return countNonHostile.Count() != 0;
}

UInt32 EffectItemList::CountItems() const
{
	EffectItemVisitor visitor(&effectList);
	return visitor.Count();
}

UInt32 EffectItemList::CountHostileItems() const
{
	EffectItemVisitor visitor(&effectList);
	HostileItemCounter counter(kFindHostile, false);
	visitor.Visit(counter);
	return counter.Count();
}

EffectItem* EffectItemList::ItemAt(UInt32 whichItem)
{
	EffectItemVisitor visitor(&effectList);
	return visitor.GetNthInfo(whichItem);
}

class HighMagickaCostFinder {
	EffectItem* m_pEffectItem;
	UInt32 m_highCost;
public:
	HighMagickaCostFinder() : m_pEffectItem(NULL), m_highCost(0) {}
	bool Accept(EffectItem* pEffectItem) {
		if (pEffectItem) {
			UInt32 cost = pEffectItem->MagickaCost();
			if (cost > m_highCost) {
				m_highCost = cost;
				m_pEffectItem = pEffectItem;
			}
		}
		return true;
	}
	EffectItem* HighestItem() const { return m_pEffectItem; }
};

EffectItem* EffectItemList::ItemWithHighestMagickaCost() const
{
	const Entry* entry = &effectList;

	// quick exit for the common case with only one effect item
	if (entry && entry->effectItem && !entry->next) {
		return entry->effectItem;
	}

	EffectItemVisitor visitor(entry);
	HighMagickaCostFinder finder;
	visitor.Visit(finder);
	return finder.HighestItem();
}

class MagickaCostCounter
{
	TESForm* m_pForm;
	UInt32 m_cost;
public:
	MagickaCostCounter(TESForm* pForm) : m_pForm(pForm), m_cost(0) {}
	bool Accept(EffectItem* pEffectItem) {
		if (pEffectItem) {
			m_cost += pEffectItem->MagickaCost(m_pForm);
		}
		return true;
	}

	UInt32 Cost() const { return m_cost; }
};

UInt32 EffectItemList::GetMagickaCost(TESForm* form) const
{
	EffectItemVisitor visitor(&effectList);
	MagickaCostCounter counter(form);
	visitor.Visit(counter);
	return counter.Cost();
}

UInt32 EffectItemList::AddItem(EffectItem* effectItem)
{
	UInt32 nuIndex = 0;
	if (effectItem) {
		Entry* entry = &effectList;
		while (entry && entry->effectItem && entry->next) {
			entry = entry->next;
			nuIndex++;
		}
		if (entry) {
			if (!entry->effectItem) {
				entry->effectItem = effectItem;
				return nuIndex;
			} else if (!entry->next) {
				// create a new entry
				Entry* nuEntry = (Entry*)FormHeap_Allocate(sizeof(Entry));
				if (nuEntry) {
					entry->next = nuEntry;
					nuEntry->effectItem = effectItem;
					nuEntry->next = NULL;
					return ++nuIndex;
				}
			}
		}
	}
	return -1;
}

UInt32 EffectItemList::AddItemCopy(EffectItem* effectItem)
{
	// make a copy of the incoming item
	EffectItem* nuEffectItem = effectItem->Clone();
	return AddItem(nuEffectItem);
}

UInt32 EffectItemList::CopyItemFrom(EffectItemList& fromList, UInt32 whichEffect)
{
	UInt32 nuIndex = -1;
	EffectItem* effectItem = fromList.ItemAt(whichEffect);
	if (effectItem) {
		nuIndex = AddItemCopy(effectItem);		
	}
	return nuIndex;
}

const char* EffectItemList::GetNthEIName(UInt32 whichEffect) const
{
	EffectItemVisitor visitor(&effectList);
	EffectItem* effItem = visitor.GetNthInfo(whichEffect);
	if (effItem->scriptEffectInfo)
		return effItem->scriptEffectInfo->effectName.m_data;
	else if (effItem->setting)
		return GetFullName(effItem->setting);
	else
		return "<no name>";
}

bool AlchemyItem::IsPoison() const
{
	return !magicItem.list.HasNonHostileItem();
}

bool AlchemyItem::_IsFlagSet(UInt32 flag) const
{
	return (moreFlags & flag) != 0;
}

void AlchemyItem::_SetFlag(UInt32 flag, bool bSet)
{
	if (bSet) {
		moreFlags |= flag;
	} else {
		moreFlags &= ~flag;
	}
}

void AlchemyItem::UpdateHostileEffectCount()
{
	hostileEffectCount = magicItem.list.CountHostileItems();
}

bool TESObjectBOOK::CantBeTaken() const
{
	return (bookFlags & kBook_CantBeTaken) != 0;
}

void TESObjectBOOK::SetCantBeTaken(bool bCantBeTaken)
{
	if (bCantBeTaken) {
		bookFlags |= kBook_CantBeTaken;
	} else {
		bookFlags &= ~kBook_CantBeTaken;
	}
}

bool TESObjectBOOK::IsScroll() const
{
	return (bookFlags & kBook_IsScroll) != 0;
}

void TESObjectBOOK::SetIsScroll(bool bIsScroll)
{
	if (bIsScroll) {
		bookFlags |= kBook_IsScroll;
	} else {
		bookFlags &= ~kBook_IsScroll;
	}
}

float TESObjectBOOK::Teaches() const
{
	if (teachesSkill == 255) return -1;
	else return (teachesSkill + kActorVal_Armorer);
}

void TESObjectBOOK::SetTeaches(UInt32 skill)
{
	if (skill >= kActorVal_Armorer && skill <= kActorVal_Speechcraft) {
		teachesSkill = (skill - kActorVal_Armorer);
	} else {
		teachesSkill = 255;
	}
}

bool IsClonedForm(UInt32 formID) {
	return (formID & 0xff000000) == 0xff000000;
}

/*
TESObjectWEAP* CloneWeapon(TESObjectWEAP* weaponToCopy)
{
	if(weaponToCopy)
	{
		TESObjectWEAP	* weap = (TESObjectWEAP *)CreateFormInstance(kFormType_Weapon);
		if(weap)
		{
			Console_Print("created weapon %08X", weap->refID);
			weap->CopyFrom(weaponToCopy);
			AddFormToDataHandler(*g_dataHandler, weap);
			AddFormToCreatedBaseObjectsList(*g_createdBaseObjList, weap);
			return weap;
		}
	}
	return NULL;
}
*/

TESForm* CloneForm(TESForm* formToClone)
{
	TESForm* clone = NULL;
	if (formToClone) {
		clone = (TESForm*)CreateFormInstance(formToClone->typeID);
		if (clone) {
			clone->CopyFrom(formToClone);
			AddFormToDataHandler(*g_dataHandler, clone);
			AddFormToCreatedBaseObjectsList(*g_createdBaseObjList, clone);
		}
	}
	return clone;
}

UInt32 TESRace::GetBaseAttribute(UInt32 attribute, bool bForFemale) const
{
	if (attribute > kActorVal_Luck) return 0;
	return (bForFemale) ? femaleAttr.attr[attribute] : maleAttr.attr[attribute];
}

UInt32 TESRace::GetSkillBonus(UInt32 skill) const
{
	for (UInt32 ix = 0; ix < 7; ++ix) {
		const BonusSkillInfo& skillInfo = bonusSkills[ix];
		if (skillInfo.skill == skill) {
			return skillInfo.bonus;
		}	
	}
	return 0;
}

bool TESRace::IsBonusSkill(UInt32 skill) const
{
	for (UInt32 ix = 0; ix < 7; ++ix) {
		const BonusSkillInfo& skillInfo = bonusSkills[ix];
		if (skillInfo.skill == skill) return true;
	}
	return false;
}

UInt32 TESRace::GetNthBonusSkill(UInt32 n) const
{
	if (n > 6) return 0;
	const BonusSkillInfo& skillInfo = bonusSkills[n];
	return skillInfo.skill;
}

bool TESClimate::HasMasser() const
{
	return (moonInfo & kClimate_Masser) == kClimate_Masser;
};

bool TESClimate::HasSecunda() const
{
	return (moonInfo & kClimate_Secunda) == kClimate_Secunda;
}

UInt8 TESClimate::GetPhaseLength() const
{
	return moonInfo & kClimate_PhaseLengthMask;
}

void TESClimate::SetPhaseLength(UInt8 nuVal) 
{
	moonInfo = (moonInfo & ~kClimate_PhaseLengthMask) | (nuVal & kClimate_PhaseLengthMask);
}

void TESClimate::SetHasMasser(bool bHasMasser)
{
	if (bHasMasser) {
		moonInfo |= kClimate_Masser;
	} else {
		moonInfo &= ~kClimate_Masser;
	}
}

void TESClimate::SetHasSecunda(bool bHasSecunda)
{
	if (bHasSecunda) {
		moonInfo |= kClimate_Secunda;
	} else {
		moonInfo &= ~kClimate_Secunda;
	}
}

void TESClimate::SetSunriseBegin(UInt8 nuVal)
{
	if (nuVal < sunriseEnd) {
		sunriseBegin = nuVal;
	}
}

void TESClimate::SetSunriseEnd(UInt8 nuVal)
{
	if (nuVal > sunriseBegin && nuVal < sunsetBegin) {
		sunriseEnd = nuVal;
	}
}

void TESClimate::SetSunsetBegin(UInt8 nuVal) 
{
	if (nuVal > sunriseEnd && nuVal < sunsetEnd) {
		sunsetBegin = nuVal;
	}
}

void TESClimate::SetSunsetEnd(UInt8 nuVal)
{
	if (nuVal > sunsetBegin) {
		sunsetEnd = nuVal;
	}
}

bool TESObjectCELL::HasWater() const
{
	return IsInterior() ? ((flags0 & kFlags0_HasWater) != 0) : true;
}

bool TESObjectCELL::IsInterior() const
{
	return worldSpace == NULL;
}

float TESObjectCELL::GetWaterHeight() const
{
	float waterHeight = 0;
	if (HasWater()) {
		BSExtraData* xData = extraData.GetByType(kExtraData_WaterHeight);
		if (xData) {
			ExtraWaterHeight* xHeight = (ExtraWaterHeight*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraWaterHeight, 0);
			if (xHeight) {
				waterHeight = xHeight->waterHeight;
			}	
		}
	}
	return waterHeight;
}

TESLeveledList::ListData*	TESLeveledList::CreateData(TESForm* form, UInt16 level, UInt16 count)
{
	ListData*	newData = (ListData*)FormHeap_Allocate(sizeof(ListData));
	newData->form = form;
	newData->level = level;
	newData->count = count;

	return newData;
}
TESLeveledList::ListEntry*	TESLeveledList::CreateEntry(TESLeveledList::ListData* data)
{
	ListEntry*	newEntry = (ListEntry*)FormHeap_Allocate(sizeof(ListEntry));
	newEntry->data = data;
	newEntry->next = NULL;

	return newEntry;
}

void TESLeveledList::AddItem(TESForm* form, UInt16 level, UInt16 count)
{
	ListEntry*	oldEntry = &list;
	ListData*	newData = CreateData(form, level, count);

	if (!list.data)		//empty list
	{
		list.data = newData;
		return;
	}
	
	ListEntry*	newEntry = CreateEntry(newData);

	while (oldEntry->data->level < level)
	{
		if (!oldEntry->next)		//add at end of list
		{
			oldEntry->next = newEntry;
			return;
		}
		oldEntry = oldEntry->next;
	}

	ListData*	swapData = oldEntry->data;
	ListEntry*	swapNext = oldEntry->next;
	oldEntry->data = newData;
	oldEntry->next = newEntry;
	newEntry->data = swapData;
	newEntry->next = swapNext;

	return;
}

UInt32 TESLeveledList::RemoveItem(TESForm* form)
{
	UInt32 numRemoved = 0;
	TESLeveledList::ListData*	curData;
	TESLeveledList::ListEntry*	curEntry;

	if (!(list.data))		//empty list
		return 0;
	
	while (list.data && list.data->form == form)	//removal from head requires shifting next element
	{
		numRemoved++;
		curData = list.data;
		curEntry = list.next;
		if (!curEntry)
		{
			FormHeap_Free(list.data);
			list.data = NULL;
			return numRemoved;
		}
		list.data = curEntry->data;
		list.next = curEntry->next;
		FormHeap_Free(curData);
		FormHeap_Free(curEntry);
	}

	curEntry = list.next;
	TESLeveledList::ListEntry*	prevEntry = &list;
	while (curEntry)
	{
		if (curEntry->data && curEntry->data->form == form)
		{
			numRemoved++;
			prevEntry->next = curEntry->next;
			TESLeveledList::ListEntry* temp = curEntry->next;
			FormHeap_Free(curEntry->data);
			FormHeap_Free(curEntry);
			curEntry = prevEntry->next;
		}
		else
		{
			prevEntry = curEntry;
			curEntry = curEntry->next;
		}
	}

	return numRemoved;
}

class LeveledListEntryDumper
{
public:
	bool Accept(TESLeveledList::ListData* data)
	{
		if (data)
			Console_Print("%32s (%0x) Level: %d Count: %d",	GetFullName(data->form),
															data->form->refID,
															data->level,
															data->count);
			_MESSAGE("%32s (%0x) Level: %d Count: %d",	GetFullName(data->form),
															data->form->refID,
															data->level,
															data->count);
		
			return true;
	}
};

void TESLeveledList::Dump()
{
	LeveledListVisitor visitor(&list);
	LeveledListEntryDumper dumper;
	visitor.Visit(dumper);
}

TESForm* TESLeveledList::CalcElement(UInt32 cLevel, bool useChanceNone, UInt32 levelDiff, bool noRecurse)
{
	ListEntry* curEntry = &list;
	UInt32 minLevel = 0;
	UInt32 maxLevel = 0;
	TESForm* item = NULL;

	if (useChanceNone && MersenneTwister::genrand_int32() % 100 < chanceNone)
		return NULL;

	//find max/min for level range
	if (!(flags & kFlags_CalcAllLevels) && !noRecurse)
	{
		while (curEntry && curEntry->data && curEntry->data->level <= cLevel)
		{
			maxLevel = curEntry->data->level;
			curEntry = curEntry->next;
		}
		if ( maxLevel > levelDiff)
			minLevel = maxLevel - levelDiff;
	}
	else
		maxLevel = cLevel;

	//skip entries below minLevel
	curEntry = &list;
	while (curEntry && curEntry->data && curEntry->data->level < minLevel)
		curEntry = curEntry->next;

	//pick an item with 1/numMatches probability
	UInt32 numMatches = 0;
	UInt32 curIndex = 0;
	while (curEntry && curEntry->data && curEntry->data->level <= maxLevel)
	{
		if (MersenneTwister::genrand_int32() % ++numMatches == 0)
		{
			item = curEntry->data->form;
			curIndex = numMatches - 1;
		}
		curEntry = curEntry->next;
	}

	//Recurse if a nested leveled item was chosen, unless otherwise specified
	if (!noRecurse)	
	{
		TESLeveledList* nestedList = (TESLeveledList*)Oblivion_DynamicCast(item, 0, RTTI_TESForm, RTTI_TESLeveledList, 0);
		if (nestedList)
			item = nestedList->CalcElement(cLevel, useChanceNone, levelDiff);
	}

	return item;
}

class LevListFinderByLevel
{
	UInt32 m_whichLevel;

public:
	LevListFinderByLevel(UInt32 whichLevel) : m_whichLevel(whichLevel)
		{	}
	bool Accept(TESLeveledList::ListData* data)
	{
		if (data->level == m_whichLevel)
			return true;
		else
			return false;
	}
};

UInt32 TESLeveledList::RemoveByLevel(UInt32 whichLevel)
{
	UInt32 numRemoved = 0;
	if (list.data)
		numRemoved = LeveledListVisitor(&list).RemoveIf(LevListFinderByLevel(whichLevel));

	return numRemoved;
}

void TESLeveledList::ListEntry::Delete() {
	FormHeap_Free(data);
	FormHeap_Free(this);
}
void TESLeveledList::ListEntry::DeleteHead(TESLeveledList::ListEntry* replaceWith) {
	FormHeap_Free(data);
	if (replaceWith)
	{
		data = replaceWith->data;
		next = replaceWith->next;
		FormHeap_Free(replaceWith);
	}
	else
		memset(this, 0, sizeof(ListEntry));
}

TESForm* TESLeveledList::GetElementByLevel(UInt32 whichLevel)
{
	TESForm* foundForm = NULL;
	LeveledListVisitor visitor(&list);
	const TESLeveledList::ListEntry* foundEntry = visitor.Find(LevListFinderByLevel(whichLevel));
	if (foundEntry && foundEntry->data)
		foundForm = foundEntry->data->form;

	return foundForm;
}

TESWeather::RGBA& TESWeather::GetColor(UInt32 whichColor, UInt8 time)
{
	static RGBA bogus;
	if (whichColor > eColor_Lightning || time > eTime_Night) return bogus;
	if (whichColor == eColor_Lightning)	{
		return lightningColor;
	} else {
		return colors[whichColor].colors[time];
	}
}

SInt8 TESActorBaseData::GetFactionRank(TESFaction* faction)
{
	FactionListEntry* entry = &factionList;
	while (entry && entry->data)
	{
		if (entry->data->faction == faction)
			return entry->data->rank;
		entry = entry->next;
	}
	return -1;
}

class ContainerFormFinder
{
	TESForm* m_formToFind;
public:
	ContainerFormFinder(TESForm* form) : m_formToFind(form) {}

	bool Accept(TESContainer::Data* pData) const
	{
		return (pData && pData->type == m_formToFind);
	}
};

TESContainer::Data* TESContainer::DataByType(TESForm *type) const
{
	ContainerVisitor visitor(&list);
	const Entry* entry = visitor.Find(ContainerFormFinder(type));
	return (entry) ? entry->data : NULL;
}

const char* TESFaction::GetNthRankMaleName(UInt32 whichRank)
{
	TESFaction::RankData* rankData = FactionRankVisitor(&ranks).GetNthInfo(whichRank);
	if (!rankData)
		return NULL;
	else
		return rankData->maleRank.m_data;
}

const TESModelList::Entry* TESModelList::FindNifPath(char* path)
{
	return ModelListVisitor(&modelList).FindString(path);
}

TESCreature* TESCreature::GetSoundBase()
{
	TESCreature* base = NULL;
	if (soundBase && (*((UInt32*)soundBase) == TESCreature_vtbl))
	{
		TESForm* form = (TESForm*)Oblivion_DynamicCast((TESCreature*)soundBase, 0, RTTI_TESCreature, RTTI_TESForm, 0);
		base = (TESCreature*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESCreature, 0);
	}
	else
		base = this;

	return base;
}

TESSound* TESCreature::GetSound(UInt32 whichSound)
{
	TESSound* sound = NULL;
	if (whichSound < eCreatureSound_MAX && soundBase)
	{
		CreatureSoundEntry** sndTbl = GetSoundBase()->soundBase;
		if (sndTbl[whichSound] && sndTbl[whichSound]->data)
			sound = sndTbl[whichSound]->data->sound;
	}
	return sound;
}

UInt32 TESCreature::GetSoundChance(UInt32 whichSound)
{
	UInt32 chance = -1;
	if (whichSound < eCreatureSound_MAX && soundBase)
	{
		CreatureSoundEntry** sndTbl = GetSoundBase()->soundBase;
		if (sndTbl[whichSound] && sndTbl[whichSound]->data)
			chance = sndTbl[whichSound]->data->chance;
	}
	return chance;
}

bool TESModelList::RemoveEntry(char* nifToRemove)
{
	Entry* toRemove = const_cast<Entry*>(ModelListVisitor(&modelList).FindString(nifToRemove));
	if (!toRemove)
		return false;

	if (&modelList == toRemove)	//remove from head, shift elements down
	{
		FormHeap_Free(modelList.nifPath);
		if (modelList.next)
		{
			toRemove = modelList.next;
			modelList.nifPath = modelList.next->nifPath;
			modelList.next = modelList.next->next;
			FormHeap_Free(toRemove);
		}
		else
			modelList.nifPath = NULL;

		return true;
	}
	//otherwise, find within list and remove
	Entry* curEntry = &modelList;
	while (curEntry)
	{
		if (curEntry->next == toRemove)
		{
			toRemove = curEntry->next;
			curEntry->next = toRemove->next;
			FormHeap_Free(toRemove->nifPath);
			FormHeap_Free(toRemove);
			return true;
		}
		curEntry = curEntry->next;
	}
	return false;
}

bool TESModelList::AddEntry(char* nifToAdd)
{
	if (ModelListVisitor(&modelList).FindString(nifToAdd))	//already present
		return false;

	UInt32 newLen = strlen(nifToAdd);
	char* newStr = (char*)FormHeap_Allocate(newLen);
	strcpy_s(newStr, newLen + 1, nifToAdd);
	if (!modelList.nifPath)
		modelList.nifPath = newStr;
	else
	{
		Entry* lastEntry = const_cast<Entry*>(ModelListVisitor(&modelList).GetLastNode());
		Entry* newEntry = (Entry*)FormHeap_Allocate(sizeof(Entry));
		lastEntry->next = newEntry;
		newEntry->nifPath = newStr;
		newEntry->next = NULL;
	}
	return true;
}

class ScriptVarFinder
{
public:
	const char* m_varName;
	ScriptVarFinder(const char* varName) : m_varName(varName)
		{	}
	bool Accept(Script::VariableInfo* varInfo)
	{
		if (!_stricmp(m_varName, varInfo->name))
			return true;
		else
			return false;
	}
};

Script::VariableInfo* Script::GetVariableByName(const char* varName)
{
	VarListVisitor visitor(&varList);
	const VarInfoEntry* varEntry = visitor.Find(ScriptVarFinder(varName));
	if (varEntry)
		return varEntry->data;
	else
		return NULL;
}

bool SpellItem::IsHostile()
{
	return hostileEffectCount ? true : false;
}

void SpellItem::SetHostile(bool bHostile)
{
	hostileEffectCount = bHostile ? 1 : 0;
}

