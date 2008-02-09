#include "obse/GameObjects.h"
#include "obse/GameAPI.h"

typedef Sky * (* _Sky_GetSingleton)(void);

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

PlayerCharacter ** g_thePlayer = (PlayerCharacter **)0x00AEAAE4;

static const UInt32	s_Actor_EquipItem =					0x005E6380;
static const UInt32	s_Actor_GetBaseActorValue =			0x005E4D30;

static const UInt32	s_PlayerCharacter_SetActiveSpell =	0x00650C30;

static const _Sky_GetSingleton	Sky_GetSingleton = (_Sky_GetSingleton)0x00537420;
static const UInt32				s_Sky_RefreshClimate = 0x00537C00;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2

PlayerCharacter ** g_thePlayer = (PlayerCharacter **)0x00B333C4;

static const UInt32	s_Actor_EquipItem =					0x005FACE0;
static const UInt32	s_Actor_GetBaseActorValue =			0x005F1750;

static const UInt32	s_PlayerCharacter_SetActiveSpell =	0x006641B0;

static const _Sky_GetSingleton	Sky_GetSingleton = (_Sky_GetSingleton)0x00542F10;
static const UInt32				s_Sky_RefreshClimate = 0x00543270;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416

PlayerCharacter ** g_thePlayer = (PlayerCharacter **)0x00B333C4;

static const UInt32	s_Actor_EquipItem =					0x005FAEA0;
static const UInt32	s_Actor_GetBaseActorValue =			0x005F1910;

static const UInt32	s_PlayerCharacter_SetActiveSpell =	0x00664700;

static const _Sky_GetSingleton	Sky_GetSingleton = (_Sky_GetSingleton)0x00542EA0;
static const UInt32				s_Sky_RefreshClimate = 0x00543200;

#else

#error unsupported version of oblivion

#endif

void Actor::EquipItem(TESObjectREFR * objType, UInt32 unk1, UInt32 unk2, UInt32 unk3, bool lockEquip)
{
	ThisStdCall(s_Actor_EquipItem, this, objType, unk1, unk2, unk3, lockEquip);
}

UInt32 Actor::GetBaseActorValue(UInt32 value)
{
	return ThisStdCall(s_Actor_GetBaseActorValue, this, value);
}

bool PlayerCharacter::SetActiveSpell(MagicItem * item)
{
	return ThisStdCall(s_PlayerCharacter_SetActiveSpell, this, item) != 0;
}

Sky * Sky::GetSingleton(void)
{
	return Sky_GetSingleton();
}

void Sky::RefreshClimate(TESClimate * climate, UInt32 unk1)
{
	ThisStdCall(s_Sky_RefreshClimate, this, climate, unk1);
}

ScriptEventList* TESObjectREFR::GetEventList() const
{
	BSExtraData* xData = baseExtraList.GetByType(kExtraData_Script);
	if (xData)
	{
		ExtraScript* xScript = (ExtraScript*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraScript, 0);
		if (xScript)
			return xScript->eventList;
	}

	return 0;
}