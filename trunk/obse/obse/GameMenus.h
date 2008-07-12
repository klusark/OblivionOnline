#pragma once

class TESObjectREFR;
class EnchantmentItem;
class TESForm;
class TESObjectBOOK;
class TileMenu;
class TileText;
class TileImage;
class TileRect;
class SpellItem;
class Character;
class ValueModifierEffect;
struct BaseExtraList;
class TESClass;

enum {
	kMenuType_Message = 0x3E9,
	kMenuType_Inventory,
	kMenuType_Stats,
	kMenuType_HUDMain,
	kMenuType_HUDInfo,
	kMenuType_HUDReticle,
	kMenuType_Loading,
	kMenuType_Container,
	kMenuType_Dialog,
	kMenuType_HUDSubtitle,
	kMenuType_Generic,
	kMenuType_SleepWait,
	kMenuType_Pause,
	kMenuType_LockPick,
	kMenuType_Options,
	kMenuType_Quantity,
	kMenuType_Audio,
	kMenuType_Video,
	kMenuType_VideoDisplay,
	kMenuType_Gameplay,
	kMenuType_Controls,
	kMenuType_Magic,
	kMenuType_Map,
	kMenuType_MagicPopup,
	kMenuType_Negotiate,
	kMenuType_Book,
	kMenuType_LevelUp,
	kMenuType_Training,
	kMenuType_BirthSign,
	kMenuType_Class,
	kMenuType_unk407,
	kMenuType_Skills,
	kMenuType_unk409,
	kMenuType_Persuasion,
	kMenuType_Repair,
	kMenuType_RaceSex,
	kMenuType_SpellPurchase,
	kMenuType_Load,
	kMenuType_Save,
	kMenuType_Alchemy,
	kMenuType_Spellmaking,
	kMenuType_Enchantment,
	kMenuType_EffectSetting,
	kMenuType_Main,
	kMenuType_Breath,
	kMenuType_QuickKeys,
	kMenuType_Credits,
	kMenuType_SigilStone,
	kMenuType_Recharge,
	kMenuType_unk41A,
	kMenuType_TextEdit,

	kMenuType_Max
};

extern NiTArray<TileMenu*> * g_TileMenuArray;
Menu * GetMenuByType(UInt32 menuType);

// 028
class Menu
{
public:
	Menu();
	~Menu();

	virtual void	Destructor(void);
	virtual void	SetField(UInt32 idx, UInt32 value);
	virtual void	Unk_02(UInt32 arg0, UInt32 arg1);	// show menu?
	virtual void	Unk_03(UInt32 arg0, UInt32 arg1);	// handle event?
	virtual void	Unk_04(UInt32 arg0, Tile * activeTile);	//called on mouseover, activeTile is moused-over Tile
	virtual void	Unk_05(UInt32 arg0, UInt32 arg1);
	virtual void	Unk_06(UInt32 arg0, UInt32 arg1, UInt32 arg2);
	virtual void	Unk_07(UInt32 arg0, UInt32 arg1, UInt32 arg2);
	virtual void	Unk_08(UInt32 arg0, UInt32 arg1);
	virtual void	Unk_09(UInt32 arg0, UInt32 arg1);
	virtual void	Unk_0A(UInt32 arg0, UInt32 arg1);
	virtual void	Unk_0B(void);
	virtual bool	Unk_0C(UInt32 arg0);
	virtual UInt32	GetID(void);
	virtual bool	Unk_0E(UInt32 arg0, UInt32 arg1);

	void	RegisterTile(TileMenu * tileMenu);
	void	EnableMenu(bool unk);

//	void	** _vtbl;	// 00
	TileMenu* tile;		// 04
	UInt32	unk08;		// 08
	UInt32	unk0C;		// 0C
	UInt32	unk10;		// 10
	UInt32	unk14;		// 14
	UInt32	unk18;		// 18
	UInt32	unk1C;		// 1C - initialized to 1
	UInt32	id;			// 20 - uninitialized
	UInt32	unk24;		// 24 - initialized to 4
};

// 64
class MessageMenu : public Menu
{
public:
	MessageMenu();
	~MessageMenu();

	TileRect		* backGround;	// 28
	TileText		* messageText;	// 2C
	TileRect		* focusBox;		// 30
	TileImage		* buttons[10];	// 34..58 each has a child TileText
	UInt32			unk5C;			// 5C
	UInt8			unk60;			// 60
	UInt8			pad61[3];		// 61
};


enum {
	kFilterInv_Weapons	= 1 << 0,
	kFilterInv_Armor	= 1 << 1,
	kFilterInv_Alchemy	= 1 << 2,
	kFilterInv_Misc		= 1 << 3,
	kFilterInv_All		= 31
};

// 58
class InventoryMenu : public Menu
{
public:
	InventoryMenu();
	~InventoryMenu();

	TileRect		* unk028;		//028
	TileRect		* unk02C;		//02C
	TileRect		* unk030;		//030
	TileImage		* unk034;		//034
	TileRect		* unk038;		//038
	UInt32			unk03C;			//03C
	UInt8			filterType;		//040 init'd to 1F (all), 1=weapons, 2=armor, ...
	UInt8			pad041[3];
	UInt8			unk044;			//044 init'd to FF
	UInt8			unk045[3];
	float			unk048;			//048
	UInt32			unk04C;			//04C
	UInt32			unk050;			//050
	UInt32			unk054;			//054 uninitialized
};

//68
class ContainerMenu : public Menu
{
public:
	ContainerMenu();
	~ContainerMenu();

	TileImage			* unk028;		//028
	TileImage			* unk02C;		//02C
	TileRect			* unk030;		//030
	TileRect			* unk034;		//034
	TileRect			* unk038;		//038
	UInt32				unk03C;			//03C
	UInt8				filterType;		//040
	UInt8				pad041[3];
	TESObjectREFR		* refr;			//044
	UInt32				unk048;			//048
	UInt32				unk04C;			//04C
	float				unk050;			//050
	UInt8				unk054[3];		//054
	UInt8				pad057;
	UInt32				unk058;			//058
	UInt32				unk05C;			//05C
	UInt8				unk060;			//060
	bool				isBarter;		//061 1 if bartering with merchant
	UInt8				unk062;
	UInt8				pad063;
	bool				isContainerContents; //init'd to 1. 0 when switched to player's inventory view
	UInt8				pad065[3];
};

// 5C
class HUDInfoMenu : public Menu
{
public:
	HUDInfoMenu();
	~HUDInfoMenu();

	UInt32			unk028[10];			// 028 .. 04C	- TileText
	UInt32			unk050;				// 050			- TileImage
	TESObjectREFR	* crosshairRef;		// 054
	UInt32			unk058;				//058
};

extern HUDInfoMenu**	g_HUDInfoMenu;

// 034
class TextEditMenu : public Menu
{
public:
	TextEditMenu();
	~TextEditMenu();

	// fields
	TileText*	text;				// 028
	TileImage*	unk02C;				// 02C
	TileImage*	unk030;				// 030
};

// 0A0
class EnchantmentMenu : public Menu
{
public:
	EnchantmentMenu();
	~EnchantmentMenu();

	struct Unk0
	{
		void*		unk0;
		UInt32		unk1;	// 1
		TESForm*	form;
	};

	// fields
	EnchantmentItem*	enchantItem;	// 028
	Unk0*				soulGemInfo;	// 02C
	Unk0*				enchantableInfo;// 030
	void*				unk034;			// 034
	UInt32				cost;			// 038
	TileText*			enchantName;
	TileRect*			unk040;
	TileImage*			unk044;
	TileText*			goldValue;	
	TileText*			playerGoldValue;
	TileImage*			unk050;
	TileRect*			unk054;
	TileRect*			unk058;
	TileRect*			unk05C;
	TileImage*			unk060;
	TileImage*			unk064;
	TileImage*			unk068;
	TileImage*			unk06C;
	TileImage*			unk070;
	TileImage*			unk074;
	TileText*			applicableEffectsLabel;
	TileText*			addedEffectsLabel;
	TileRect*			unk080;
	TileRect*			unk084;
	TileImage*			unk088;
	TileImage*			unk08C;
	UInt32				unk090[4];
};
STATIC_ASSERT(sizeof(EnchantmentMenu) == 0x0A0);

//3C
class BookMenu : public Menu
{
public:
	BookMenu();
	~BookMenu();

	UInt32			unk028;			//028 init'd to 0
	UInt32			unk02C;			//02C ditto
	TESObjectREFR	* bookRef;		//030
	TESObjectBOOK	* book;			//034
};

struct MenuSpellList
{
	SpellItem		* spell;
	MenuSpellList	* next;

	SpellItem* Info() const	{ return spell;	}
	MenuSpellList* Next() const	{ return next;	}
};

typedef Visitor<MenuSpellList, SpellItem> MenuSpellListVisitor;

//5C
class MagicMenu : public Menu
{
public:
	MagicMenu();
	~MagicMenu();

	enum {
		kFilter_Target	= 1 << 0,
		kFilter_Touch	= 1 << 1,
		kFilter_Self	= 1 << 2,
		kFilter_Active  = 1 << 3,
		kFilter_All		= 7,
		kFilter_ActiveEffects = 8
	};

	struct ActiveEffectData {
		ValueModifierEffect	* effectMod;
		UInt32				magnitude;
		//might be 1 more
	};

	struct ActiveEffectEntry {
		ActiveEffectData	* data;
		ActiveEffectEntry	* next;

		ActiveEffectData* Info() const;
		ActiveEffectEntry* Next() const;
	};

	TileRect			* unk028;			//028
	TileRect			* unk02C;			//02C
	TileImage			* unk030;			//030
	TileImage			* unk034;			//034
	MenuSpellList		spells;			//038
	ActiveEffectEntry	activeEffects;		//040
	UInt32				unk048;				//048
	TileRect			* unk04C;			//04C
	UInt32				filterType;			//050 init'd to 7
	UInt32				unk054;				//054
	float				unk058;				//058
};

typedef Visitor<MagicMenu::ActiveEffectEntry, MagicMenu::ActiveEffectData> MenuActiveEffectVisitor;

//68
class SpellPurchaseMenu : public Menu
{
public:
	SpellPurchaseMenu();
	~SpellPurchaseMenu();

	TileImage		* unk028;		//028
	TileRect		* unk02C;		//02C
	TileImage		* unk030[4];	//030 .. 03C
	TileText		* unk040;		//040
	TileRect		* unk044;		//044
	TileRect		* unk048;		//048
	UInt32			unk04C;			//04C
	Character		* spellMerchant;//050
	UInt32			unk054;			//054
	UInt32			unk058;			//058
	UInt8			unk05C;			//05C
	UInt8			pad05D[3];	
	MenuSpellList	spells;			//060
};

template <typename T>
class RepairMenuList : public NiTPointerListBase <T>
{		//list of repair item and index
public:
	RepairMenuList();
	~RepairMenuList();
};

//78
class RepairMenu : public Menu
{
public:
	RepairMenu();
	~RepairMenu();

	struct RepairMenuItemAndIndex {
		BaseExtraList	* extraList;
		UInt32			unk04;		//index?
		TESForm			* object;
	};

	UInt32			unk028;		//028
	TileImage		* unk02C;	//02C
	TileImage		* unk030;	//030
	TileText		* unk034;	//034
	TileImage		* unk038;	//038
	TileRect		* unk03C;	//03C
	TileImage		* unk040;	//040
	TileRect		* unk044;	//044
	UInt32			unk048;		//048
	TileImage		* unk04C;	//04C
	TileImage		* unk050;	//050
	UInt32			unk054;		//054
	UInt32			unk058;		//058 init'd to 1
	UInt32			unk05C;		//05C
	Character		* armorer;	//060
	UInt8			unk064;		//064
	UInt8			unk065;
	UInt8			pad066[2];
	RepairMenuList<RepairMenuItemAndIndex>	repairMenuList;	//068
};

//54
class RechargeMenu : public Menu
{
public:
	RechargeMenu();
	~RechargeMenu();

	TileRect		* unk028;		//028
	TileImage		* unk02C[3];	//02C .. 034
	TileRect		* unk038;		//038
	UInt32			unk03C;			//03C
	UInt32			unk040;			//040
	UInt32			unk044;			//044
	Character		* recharger;	//048
	UInt32			unk04C;			//04C
	UInt8			unk050;			//050 init'd to FF
	UInt8			pad051[3];
};

//98+
class DialogMenu : public Menu
{
public:
	DialogMenu();
	~DialogMenu();

	TileRect			* unk028;		//028
	TileText			* unk02C;		//02C
	TileImage			* unk030;		//030
	TileRect			* unk034[3];	//034 .. 03C
	TileImage			* unk040[8];	//040 .. 05C
	Character			* speaker;		//060
	UInt32				unk064;			//064
	UInt32				unk068;			//068
	UInt32				unk06C;			//06C init'd to 1
	UInt32				unk070;			//070
	UInt32				unk074;			//074
	UInt32				unk078;			//078
	UInt8				unk07C;			//07C
	UInt8				pad07D[3];
	UInt32				unk080;			//080 init'd to 1
	UInt32				unk084;			//084
	UInt8				unk088;			//088
	UInt8				pad087[3];		
	UInt32				unk08C;			//08C
	UInt16				unk090;			//090
	UInt16				unk092;			//092
	UInt8				unk094[3];		//094
	UInt8				pad097;	
};

class AlchemyItem;
class TESObjectAPPA;
class IngredientItem;

//C0
class AlchemyMenu : public Menu
{
public:
	AlchemyMenu();
	~AlchemyMenu();

	struct ApparatusInfo {
		BaseExtraList	** extraList;
		UInt8			unk4;			// quality?
		TESObjectAPPA	* apparatus;
	};

	struct EffectData {
		UInt32			effectCode;	
		UInt32			unk4;			// magnitude?
	};

	struct EffectEntry {
		EffectData	* data;
		EffectEntry	* next;
	};


	struct IngredientInfo {
		BaseExtraList		** extraList;
		UInt32				count;
		IngredientItem		* ingredient;
	};

	struct Unk0A0 {
		UInt8	unk00;
		UInt8	pad01[3];
		UInt32	unk04;
		UInt8	unk08;
		UInt8	pad09[3];
		UInt32	unk0C;			//init'd to -1
		UInt32	unk10;
		UInt32	unk14;			//init'd to 1
		char	* effectName18;
		UInt16	unk1C;
		UInt16  unk1E;
		char	* effectName20;
		UInt16	unk24;
		UInt16	unk26;
	};

	TileRect			* unk028;		//028
	TileText			* unk02C;		//02C
	TileImage			* unk030[4];	//030 .. 03C
	TileRect			* unk040[6];	//040 .. 054
	TileImage			* unk058[8];	//058 .. 074
	ApparatusInfo		* apparatus[4];	//078 .. 084
	float				unk088;			//088
	UInt32				unk08C;			//08C
	TileRect			* unk090;		//090
	AlchemyItem			* potion;		//094
	float				unk098;			//098
	UInt32				unk09C;			//09C
	Unk0A0				* unk0A0;		//0A0
	UInt8				unk0A4;			//0A4
	UInt8				unk0A5;			//	  - not initialized
	UInt8				unk0A6;			//    - init'd to 0xFF
	UInt8				unk0A7;
	EffectEntry			effects;		//0A8 .. 0AC
	IngredientInfo		* ingreds[4];	//0B0 .. 0BC

	IngredientItem* GetIngredientItem(UInt32 whichIngred);
	TESObjectAPPA* GetApparatus(UInt32 whichAppa);
	UInt32 GetIngredientCount(UInt32 whichIngred);
};

STATIC_ASSERT(sizeof(AlchemyMenu) == 0xC0);

class TESWorldSpace;

//100
class MapMenu : public Menu
{
public:
	MapMenu();
	~MapMenu();

	struct MapMarkerEntry {
		TESObjectREFR	* mapMarker;
		MapMarkerEntry	* next;

		TESObjectREFR	* Info() const { return mapMarker;	}
		MapMarkerEntry	* Next() const { return next;	}
	};

	TileImage			* unk028;		//028
	TileRect			* unk02C[5];	//02C .. 3C
	TileImage			* unk040;		//03C
	TileImage			* unk044;		//040
	TileRect			* unk048;		//044
	TileRect			* unk04C;		//048
	TileImage			* unk050[5];	//050 .. 060
	TileRect			* unk064[2];	//064 .. 068
	TileImage			* unk06C;		//06C
	TileImage			* unk070;		//070
	TileRect			* unk074;		//074
	UInt32				unk078[3];		//078 .. 080
	UInt8				unk084;			//084 - init'd to -1
	UInt8				pad085[3];
	UInt32				unk088[4];		//088 .. 094
	UInt32				unk098[11];		//098 .. C0
	MapMarkerEntry		* mapMarkers;	//0C4
	void				* unk0C8;		//0C8
	UInt32				unk0CC;			//0CC
	TESWorldSpace		* worldSpace;	//0D0
	UInt32				unk0D4;			//0D4
	UInt32				unk0D8;			//0D8
	UInt32				unk0DC;			//0DC
	TileImage			* unk0E0;		//0E0
	UInt32				unk0E4[5];		//0E4 .. 0F4
	TileImage			* unk0F8;		//0F8
	TileImage			* unk0FC;		//0FC
};

typedef Visitor<MapMenu::MapMarkerEntry, TESObjectREFR> MapMarkerEntryVisitor;
STATIC_ASSERT(sizeof(MapMenu) == 0x100);

//08C
class ClassMenu : public Menu
{
public:
	ClassMenu();
	~ClassMenu();

	TileRect		* unk028;					//028
	TileImage		* unk02C;					//02C
	TileImage		* unk030;					//030
	TileRect		* unk034;					//034
	UInt32			unk038;						//038
	TESClass		* class03C;					//03C
	TESClass		* class040;					//040
	UInt32			unk044[(0x8C - 0x44) >> 2];	//044..088
};

STATIC_ASSERT(sizeof(ClassMenu) == 0x8C);