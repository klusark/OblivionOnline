#include "obse/Commands_MagicEffect.h"
#include "ParamInfos.h"
#include "GameAPI.h"
#include "GameForms.h"
#include "GameObjects.h"
#include "Utilities.h"

#if OBLIVION

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
	ExtractArgsEx(paramInfo, arg1, opcodeOffsetPtr, scriptObj, eventList, &magic);
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


#endif

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
	HANDLER(Cmd_GetMagicEffectUsedObject_Execute),
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
	HANDLER(Cmd_GetMagicEffectUsedObjectC_Execute),
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
