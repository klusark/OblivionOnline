#include "Utilities.h"
#include "GameTiles.h"
#include "GameMenus.h"
#include "GameAPI.h"

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

HUDInfoMenu	** g_HUDInfoMenu = (HUDInfoMenu**)0x00AFC094;
NiTArray<TileMenu*> * g_TileMenuArray = (NiTArray<TileMenu*> *)0x00AD4458;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2

HUDInfoMenu	** g_HUDInfoMenu = (HUDInfoMenu**)0x00B3B33C;
NiTArray<TileMenu*> * g_TileMenuArray = (NiTArray<TileMenu*> *)0x00B13970;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416

HUDInfoMenu	** g_HUDInfoMenu = (HUDInfoMenu**)0x00B3B33C;
NiTArray<TileMenu*> * g_TileMenuArray = (NiTArray<TileMenu*> *)0x00B13970;

#else

#error unsupported version of oblivion

#endif

Menu* GetMenuByType(UInt32 menuType)
{
	if (menuType >= kMenuType_Message && menuType < kMenuType_Max)
	{
		TileMenu* tileMenu = g_TileMenuArray->data[menuType - kMenuType_Message];
		if (tileMenu)
			return tileMenu->menu;
	}

	return NULL;
}

void Menu::RegisterTile(TileMenu * tileMenu)
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
	ThisStdCall(0x005779A0, this, tileMenu);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
	ThisStdCall(0x00584870, this, tileMenu);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	ThisStdCall(0x00584880, this, tileMenu);
#else
#error unsupported Oblivion version
#endif
}

void Menu::EnableMenu(bool unk)
{
#if OBLIVION_VERSION == OBLIVION_VERSION_1_1
	ThisStdCall(0x005785D0, this, unk);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2
	ThisStdCall(0x00585160, this, unk);
#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416
	ThisStdCall(0x00585190, this, unk);
#else
#error unsupported Oblivion version
#endif
}

IngredientItem* AlchemyMenu::GetIngredientItem(UInt32 whichIngred)
{
	if (whichIngred < 4 && ingreds[whichIngred])
		return ingreds[whichIngred]->ingredient;
	else
		return NULL;
}

TESObjectAPPA* AlchemyMenu::GetApparatus(UInt32 whichAppa)
{
	if (whichAppa < 4 && apparatus[whichAppa])
		return apparatus[whichAppa]->apparatus;
	else
		return NULL;
}

UInt32 AlchemyMenu::GetIngredientCount(UInt32 whichIngred)
{
	if (whichIngred < 4 && ingreds[whichIngred])
		return ingreds[whichIngred]->count;
	else
		return -1;
}