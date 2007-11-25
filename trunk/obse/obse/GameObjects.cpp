#include "obse/GameObjects.h"
#include "obse/GameAPI.h"

typedef Sky * (* _Sky_GetSingleton)(void);

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

PlayerCharacter ** g_thePlayer = (PlayerCharacter **)0x00AEAAE4;

void Actor::EquipItem(TESObjectREFR * objType, UInt32 unk1, UInt32 unk2, UInt32 unk3, bool lockEquip)
{
	ThisCall(0x005E6380, this, objType, unk1, unk2, unk3, lockEquip);
}

bool PlayerCharacter::SetActiveSpell(MagicItem * item)
{
	return ThisStdCall(0x00650C30, this, item) != 0;
}

static const _Sky_GetSingleton	Sky_GetSingleton = (_Sky_GetSingleton)0x00537420;
static const UInt32				s_Sky_RefreshClimate = 0x00537C00;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2

PlayerCharacter ** g_thePlayer = (PlayerCharacter **)0x00B333C4;

void Actor::EquipItem(TESObjectREFR * objType, UInt32 unk1, UInt32 unk2, UInt32 unk3, bool lockEquip)
{
	ThisCall(0x005FACE0, this, objType, unk1, unk2, unk3, lockEquip);
}

bool PlayerCharacter::SetActiveSpell(MagicItem * item)
{
	return ThisStdCall(0x006641B0, this, item) != 0;
}

static const _Sky_GetSingleton	Sky_GetSingleton = (_Sky_GetSingleton)0x00542F10;
static const UInt32				s_Sky_RefreshClimate = 0x00543270;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416

PlayerCharacter ** g_thePlayer = (PlayerCharacter **)0x00B333C4;

void Actor::EquipItem(TESObjectREFR * objType, UInt32 unk1, UInt32 unk2, UInt32 unk3, bool lockEquip)
{
	ThisCall(0x005FAEA0, this, objType, unk1, unk2, unk3, lockEquip);
}

bool PlayerCharacter::SetActiveSpell(MagicItem * item)
{
	return ThisStdCall(0x00664700, this, item) != 0;
}

static const _Sky_GetSingleton	Sky_GetSingleton = (_Sky_GetSingleton)0x00542EA0;
static const UInt32				s_Sky_RefreshClimate = 0x00543200;

#else

#error unsupported version of oblivion

#endif

Sky * Sky::GetSingleton(void)
{
	return Sky_GetSingleton();
}

void Sky::RefreshClimate(TESClimate * climate, UInt32 unk1)
{
	ThisStdCall(s_Sky_RefreshClimate, this, climate, unk1);
}
