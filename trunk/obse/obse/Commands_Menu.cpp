#include "GameObjects.h"
#include "GameTiles.h"
#include "GameMenus.h"
#include "Commands_Menu.h"
#include "GameAPI.h"
#include "GameForms.h"
#include "ParamInfos.h"


#if OBLIVION

typedef void (* _CloseAllMenus)(void);
#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

static _CloseAllMenus CloseAllMenus = (_CloseAllMenus)0x0056C910;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2

static _CloseAllMenus CloseAllMenus = (_CloseAllMenus)0x00579670;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416

static _CloseAllMenus CloseAllMenus = (_CloseAllMenus)0x00579770;

#else

#error unsupported oblivion version

#endif

static bool Cmd_GetActiveMenuMode_Execute(COMMAND_ARGS)
{
	*result = 0;
	InterfaceManager* intfc = InterfaceManager::GetSingleton();
	if (intfc && intfc->activeMenu)
		*result = intfc->activeMenu->id;

	return true;
}

static enum eMenuValue {
	//no params
	kMenu_Soulgem,
	kMenu_EnchantItem,
	kMenu_Barter,
	kMenu_ContainerView,

	//one optional int param
	kMenu_Selection,
	kMenu_Object,
	kMenu_Ref,
	kMenu_Filter,

	//one int param
	kMenu_Ingredient,
	kMenu_IngredientCount,
	kMenu_Apparatus
};

union MenuInfo {
	const TESForm* form;
	UInt32	 integer;
};

static bool GetActiveMenuElement(COMMAND_ARGS, eMenuValue whichValue, MenuInfo* out, UInt32 whichMenu = 0)
{
	InterfaceManager* intfc = InterfaceManager::GetSingleton();
	Menu* activeMenu = intfc->activeMenu;
	Tile* activeTile = intfc->activeTile;
	bool gotValue = false;
	UInt32 intArg = -1;

	//Extract arguments
	if (whichValue < kMenu_Ingredient && whichValue >= kMenu_Selection)	//optional int param specifies menu type
	{
		ExtractArgs(EXTRACT_ARGS, &intArg);
		if (intArg != -1)
			activeMenu = GetMenuByType(intArg);
	}
	else if (whichValue >= kMenu_Ingredient)
	{
		ExtractArgs(EXTRACT_ARGS, &intArg);
		if (intArg == -1)
			return false;
	}

	if (whichMenu)	//specific menu, so look it up directly
		activeMenu = GetMenuByType(whichMenu);

	if (!activeMenu)
		return false;

	//get element based on menu type
	switch (activeMenu->id)
	{
	case kMenuType_Message:
		{
			switch (whichValue)
			{
			case kMenu_Object:
				if (ShowMessageBox_pScriptRefID)
				{
					out->form = LookupFormByID(*ShowMessageBox_pScriptRefID);
					gotValue = true;
				}
				else
					out->form = 0;
			default:
				break;
			}
		}
		break;
	case kMenuType_Alchemy:
		{
			AlchemyMenu* menu = (AlchemyMenu*)activeMenu;
			switch (whichValue)
			{
			case kMenu_Object:
				out->form = menu->potion;
				gotValue = true;
				break;
			case kMenu_Ingredient:
				out->form = menu->GetIngredientItem(intArg);
				gotValue = true;
				break;
			case kMenu_IngredientCount:
				out->integer = menu->GetIngredientCount(intArg);
				gotValue = true;
				break;
			case kMenu_Apparatus:
				out->form = menu->GetApparatus(intArg);
				gotValue = true;
				break;
			default:
				break;
			}
		}
		break;
	case kMenuType_Container:
		{
			ContainerMenu* menu = (ContainerMenu*)activeMenu;
			switch (whichValue)
			{
			case kMenu_ContainerView:
				if (menu->isContainerContents)
					out->integer = 0;
				else
					out->integer = 1;
				gotValue = true;
				break;
			case kMenu_Ref:
				{
					out->form = menu->refr;
					gotValue = true;
					break;
				}
			case kMenu_Filter:
				out->integer = menu->filterType;
				gotValue = true;
				break;
			case kMenu_Selection:
				{
					if (activeTile)
					{
						float fIndex;
						if (activeTile->GetFloatValue(kTileValue_user11, &fIndex))
						{
							UInt32 index = fIndex;
							if (menu->isContainerContents)
								out->form = menu->refr->GetInventoryItem(index, menu->isBarter);
							else
								out->form = (*g_thePlayer)->GetInventoryItem(index, 0);
							gotValue = true;
						}
					}
				}
				break;
			case kMenu_Barter:
				out->integer = menu->isBarter;
				gotValue = true;
				break;
			default:
				break;
			}
			break;
		}
	case kMenuType_Magic:
		{
			MagicMenu* menu = (MagicMenu*)activeMenu;
			switch (whichValue)
			{
			case kMenu_Selection:
				{
					if (activeTile && menu->filterType != MagicMenu::kFilter_ActiveEffects)
					{
						float fIndex;
						if (activeTile->GetFloatValue(kTileValue_user11, &fIndex))
						{
							UInt32 index = fIndex;
							out->form = MenuSpellListVisitor(&menu->spells).GetNthInfo(index);
							gotValue = true;
						}
					}
				}
				break;
			case kMenu_Filter:
				out->integer = menu->filterType;
				gotValue = true;
				break;
			default:
				break;
			}
			break;
		}
	case kMenuType_SpellPurchase:
		{
			SpellPurchaseMenu* menu = (SpellPurchaseMenu*)activeMenu;
			switch (whichValue)
			{
			case kMenu_Selection:
				{
					if (activeTile)
					{
						float fIndex;
						if (activeTile->GetFloatValue(kTileValue_user11, &fIndex))
						{
							UInt32 index = fIndex;
							out->form = MenuSpellListVisitor(&menu->spells).GetNthInfo(index);
							gotValue = true;
						}
					}
					break;
				}
			case kMenu_Ref:
				{
					out->form = menu->spellMerchant;
					gotValue = true;
					break;
				}
			default:
				break;
			}
		}
		break;
	case kMenuType_Enchantment:
		{
			EnchantmentMenu* menu = (EnchantmentMenu*)activeMenu;
			switch (whichValue)
			{
			case kMenu_Soulgem:
				if (menu->soulGemInfo)
				{
					out->form = menu->soulGemInfo->form;
					gotValue = true;
				}
				break;
			case kMenu_EnchantItem:
				out->form = menu->enchantItem;
				gotValue = true;
				break;
			default:
				break;
			}
		}
		break;
	case kMenuType_Book:
		{
			BookMenu* menu = (BookMenu*)activeMenu;
			switch (whichValue)
			{
			case kMenu_Ref:
				out->form = menu->bookRef;
				gotValue = true;
				break;
			case kMenu_Object:
				out->form = menu->book;
				gotValue = true;
				break;
			default:
				break;
			}
		}
		break;
	case kMenuType_Inventory:
		{
			InventoryMenu* menu = (InventoryMenu*)activeMenu;
			switch (whichValue)
			{
			case kMenu_Selection:
				{
					if (activeTile)
					{
						float fIndex;
						if (activeTile->GetFloatValue(kTileValue_user11, &fIndex))
						{
							UInt32 index = fIndex;
							out->form = (*g_thePlayer)->GetInventoryItem(index, 0);
							gotValue = true;
						}
					}
				}
				break;
			case kMenu_Filter:
				out->integer = menu->filterType;
				gotValue = true;
				break;
			default:
				break;
			}
		}
		break;
	case kMenuType_Dialog:
		{
			DialogMenu* menu = (DialogMenu*)activeMenu;
			switch (whichValue)
			{
			case kMenu_Ref:
				out->form = menu->speaker;
				gotValue = true;
				break;
			default:
				break;
			}
		}
		break;
	default:
		break;
	}

	return gotValue;
}

static bool Cmd_GetEnchMenuEnchItem_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	MenuInfo info;
	if (GetActiveMenuElement(PASS_COMMAND_ARGS, kMenu_EnchantItem, &info, kMenuType_Enchantment) && info.form)
		*refResult = info.form->refID;

	return true;
}

static bool Cmd_GetEnchMenuSoulgem_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	MenuInfo info;
	if (GetActiveMenuElement(PASS_COMMAND_ARGS, kMenu_Soulgem, &info, kMenuType_Enchantment) && info.form)
		*refResult = info.form->refID;

	return true;
}

static bool Cmd_GetActiveMenuSelection_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	MenuInfo info;
	if (GetActiveMenuElement(PASS_COMMAND_ARGS, kMenu_Selection, &info) && info.form)
	{
		*refResult = info.form->refID;
		if (IsConsoleMode())
			Console_Print("GetActiveMenuSelection >> %s (%08x)", GetFullName(const_cast<TESForm*>(info.form)), info.form->refID);
	}

	return true;
}

static bool Cmd_GetActiveMenuObject_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	MenuInfo info;
	if (GetActiveMenuElement(PASS_COMMAND_ARGS, kMenu_Object, &info) && info.form)
		*refResult = info.form->refID;

	return true;
}

static bool Cmd_GetActiveMenuRef_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	MenuInfo info;
	if (GetActiveMenuElement(PASS_COMMAND_ARGS, kMenu_Ref, &info) && info.form)
		*refResult = info.form->refID;

	return true;
}

static bool Cmd_GetActiveMenuFilter_Execute(COMMAND_ARGS)
{
	*result = 0;

	MenuInfo info;
	if (GetActiveMenuElement(PASS_COMMAND_ARGS, kMenu_Filter, &info))
		*result = info.integer;

	return true;
}

static bool Cmd_IsBarterMenuActive_Execute(COMMAND_ARGS)
{
	*result = 0;

	MenuInfo info;
	if (GetActiveMenuElement(PASS_COMMAND_ARGS, kMenu_Barter, &info))
		*result = (info.integer) ? 1 : 0;

	return true;
}

static bool Cmd_GetAlchMenuIngredient_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	MenuInfo info;
	if (GetActiveMenuElement(PASS_COMMAND_ARGS, kMenu_Ingredient, &info, kMenuType_Alchemy) && info.form)
		*refResult = info.form->refID;
	
	return true;
}

static bool Cmd_GetAlchMenuIngredientCount_Execute(COMMAND_ARGS)
{
	*result = -1;

	MenuInfo info;
	if (GetActiveMenuElement(PASS_COMMAND_ARGS, kMenu_IngredientCount, &info, kMenuType_Alchemy))
		*result = info.integer;

	return true;
}

static bool Cmd_GetAlchMenuApparatus_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	MenuInfo info;
	if (GetActiveMenuElement(PASS_COMMAND_ARGS, kMenu_Apparatus, &info, kMenuType_Alchemy) && info.form)
		*refResult = info.form->refID;

	return true;
}


/*	Need to figure out what BaseExtraList* and unk4 do in ApparatusInfo struct first
static bool Cmd_SetAlchMenuApparatus_Execute(COMMAND_ARGS)
{
	UInt32 whichAppa = 0;
	TESForm* newAppa = NULL;
	*result = 0;

	if (ExtractArgs(PASS_EXTRACT_ARGS, &whichAppa, &newAppa))
	{
		Menu* menu = GetMenuByType(kMenuType_Alchemy);
		if (alchMenu)
		{
			AlchemyMenu* alchMenu = (AlchemyMenu*)menu;
			alchMenu->
*/

static bool Cmd_CloseAllMenus_Execute(COMMAND_ARGS)
{
	CloseAllMenus();
	return true;
}


static bool Cmd_GetContainerMenuView_Execute(COMMAND_ARGS)
{
	*result = -1;

	MenuInfo info;
	if (GetActiveMenuElement(PASS_COMMAND_ARGS, kMenu_ContainerView, &info, kMenuType_Container))
		*result = info.integer;

	return true;
}

static bool Cmd_GetFloatMenuTrait_Execute(COMMAND_ARGS)
{
	UInt32 menuCode = 0;
	eTileValue valueID;
	*result = 0;

	if (ExtractArgs(PASS_EXTRACT_ARGS, &menuCode, &valueID))
	{
		Menu* menu = GetMenuByType(menuCode);
		if (menu && menu->tile)
		{
			float floatVal = 0;
			menu->tile->GetFloatValue(valueID, &floatVal);
			*result = floatVal;
		}
	}

	return true;
}

static bool Cmd_SetFloatMenuTrait_Execute(COMMAND_ARGS)
{
	UInt32 menuCode = 0;
	eTileValue valueID;
	float newValue = 0;
	*result = 0;

	if (ExtractArgs(PASS_EXTRACT_ARGS, &menuCode, &valueID, &newValue))
	{
		Menu* menu = GetMenuByType(menuCode);
		if (menu && menu->tile)
			if (menu->tile->SetFloatValue(valueID, newValue))
				*result = 1;
	}

	return true;
}

//this doesn't actually have an effect as xml is re-parsed each time menu instantiated
static bool Cmd_SetMessageMenuFade_Execute(COMMAND_ARGS)
{
	float newFadeTimer = 0;

	if (ExtractArgs(PASS_EXTRACT_ARGS, &newFadeTimer))
	{
		MessageMenu* msgMenu = (MessageMenu*)GetMenuByType(kMenuType_Message);
		if (msgMenu)
			DumpClass(msgMenu, sizeof(MessageMenu)/4);
	}
	return true;
}

#endif

DEFINE_COMMAND(GetActiveMenuMode,
			   returns the code for the menu over which the mouse is positioned,
			   0,
			   0,
			   NULL);

DEFINE_COMMAND(GetEnchMenuEnchItem,
			   returns the selected enchantment item,
			   0,
			   0,
			   NULL);

DEFINE_COMMAND(GetEnchMenuSoulgem,
			   returns the selected soulgem,
			   0,
			   0,
			   NULL);

DEFINE_COMMAND(GetActiveMenuObject,
			   returns the base object from which the active menu is derived,
			   0,
			   1,
			   kParams_OneOptionalInt);

DEFINE_COMMAND(GetActiveMenuRef,
			   returns the ref from which the active menu is derived,
			   0,
			   1,
			   kParams_OneOptionalInt);

DEFINE_COMMAND(GetActiveMenuSelection,
			   returns the selected item in the active menu,
			   0,
			   1,
			   kParams_OneOptionalInt);

DEFINE_COMMAND(GetActiveMenuFilter,
			   returns the filter code for the active menu,
			   0,
			   1,
			   kParams_OneOptionalInt);

DEFINE_COMMAND(IsBarterMenuActive,
			   returns 1 if the barter menu is active,
			   0,
			   0,
			   NULL);

DEFINE_COMMAND(GetAlchMenuIngredient,
			   returns the nth ingredient selected in the Alchemy interface,
			   0,
			   1,
			   kParams_OneInt);

DEFINE_COMMAND(GetAlchMenuIngredientCount,
			   returns the count of the nth ingredient selected in the Alchemy interface,
			   0,
			   1,
			   kParams_OneInt);

DEFINE_COMMAND(GetAlchMenuApparatus,
			   returns the nth apparatus selected in the alchemy interface,
			   0,
			   1,
			   kParams_OneInt);

DEFINE_COMMAND(GetContainerMenuView,
			   returns 1 if the player is looking at his own inventory,
			   0,
			   0,
			   NULL);

static ParamInfo kParams_SetAlchMenuApparatus[2] =
{
	{	"int",		 kParamType_Integer,		 0	},
	{	"apparatus", kParamType_InventoryObject, 0	},
};

/*
DEFINE_COMMAND(SetAlchMenuApparatus,
			   sets the apparatus being used for brewing potions,
			   0,
			   2,
			   kParams_SetAlchMenuApparatus);
*/

DEFINE_COMMAND(CloseAllMenus,
			   closes all open menus,
			   0,
			   0,
			   NULL);

static ParamInfo kParams_SetFloatMenuTrait[3] =
{
	{	"menuCode",	kParamType_Integer,	0	},
	{	"traitID",	kParamType_Integer,	0	},
	{	"newValue",	kParamType_Float,	0	},
};

DEFINE_COMMAND(SetFloatMenuTrait,
			   sets the value of a menu trait,
			   0,
			   3,
			   kParams_SetFloatMenuTrait);

DEFINE_COMMAND(GetFloatMenuTrait,
			   returns the value of a menu trait,
			   0,
			   2,
			   kParams_TwoInts);

DEFINE_COMMAND(SetMessageMenuFade,
			   adjust the fade time for the Message menu,
			   0,
			   1,
			   kParams_OneFloat);
