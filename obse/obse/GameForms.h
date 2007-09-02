#pragma once

#include "obse/GameExtraData.h"
#include "obse/GameTypes.h"
#include "obse/GameAPI.h"

/****
 *	id	name	size	class
 *	00	NONE	
 *	01	TES4	
 *	02	GRUP	
 *	03	GMST	
 *	04	GLOB	28		TESGlobal
 *	05	CLAS	6C		TESClass
 *	06	FACT	44		TESFaction
 *	07	HAIR	4C		TESHair
 *	08	EYES	34		TESEyes
 *	09	RACE	318		TESRace
 *	0A	SOUN	44		TESSound
 *	0B	SKIL	60		TESSkill
 *	0C	MGEF	A8		EffectSetting
 *	0D	SCPT	50		Script
 *	0E	LTEX	34		TESLandTexture
 *	0F	ENCH	44		EnchantmentItem
 *	10	SPEL	44		SpellItem
 *	11	BSGN	4C		BirthSign
 *	12	ACTI	58		TESObjectACTI
 *	13	APPA	7C		TESObjectAPPA
 *	14	ARMO	E8		TESObjectARMO
 *	15	BOOK	8C		TESObjectBOOK
 *	16	CLOT	DC		TESObjectCLOT
 *	17	CONT	7C		TESObjectCONT
 *	18	DOOR	70		TESObjectDOOR
 *	19	INGR	80		IngredientItem
 *	1A	LIGH	90		TESObjectLIGH
 *	1B	MISC	70		TESObjectMISC
 *	1C	STAT	3C		TESObjectSTAT
 *	1D	GRAS	5C		TESGrass
 *	1E	TREE	80		TESObjectTREE
 *	1F	FLOR	64		TESFlora
 *	20	FURN	5C		TESFurniture
 *	21	WEAP	A0		TESObjectWEAP
 *	22	AMMO	84		TESAmmo
 *	23	NPC_	200		TESNPC
 *	24	CREA	138		TESCreature (patch 1.1)
 *				140		TESCreature (patch 1.2) - changed due to TESModelList
 *	25	LVLC	44		TESLevCreature
 *	26	SLGM	74		TESSoulGem
 *	27	KEYM	70		TESKey
 *	28	ALCH	80		AlchemyItem
 *	29	SBSP	30		TESSubSpace
 *	2A	SGST	88		TESSigilStone
 *	2B	LVLI	34		TESLevItem
 *	2C	SNDG	
 *	2D	WTHR	148		TESWeather
 *	2E	CLMT	58		TESClimate
 *	2F	REGN	2C		TESRegion
 *	30	CELL	58		TESObjectCELL
 *	31	REFR	58		TESObjectREFR
 *	32	ACHR				used in save file for NPC_ placement
 *	33	ACRE				used in save file for CREA placement
 *	34	PGRD	54		TESPathGrid
 *	35	WRLD	E0		TESWorldSpace
 *	36	LAND	28		TESObjectLAND
 *	37	TLOD	
 *	38	ROAD	30		TESRoad
 *	39	DIAL	
 *	3A	INFO	
 *	3B	QUST	68		TESQuest
 *	3C	IDLE	48		TESIdleForm
 *	3D	PACK	3C		TESPackage
 *	3E	CSTY	98		TESCombatStyle
 *	3F	LSCR	3C		TESLoadScreen
 *	40	LVSP	34		TESLevSpell
 *	41	ANIO	34		TESObjectANIO
 *	42	WATR	AC		TESWaterForm
 *	43	EFSH	110		TESEffectShader
 *	44	TOFT	
 ***/

/**** change flags
 *	npc/creature
 *	00000004	CHANGE_ACTOR_BASE_HEALTH
 *	00000008	CHANGE_ACTOR_BASE_ATTRIBUTES
 *	00000010	CHANGE_ACTOR_BASE_DATA
 *	00000020	CHANGE_ACTOR_BASE_SPELLLIST
 *	00000040	CHANGE_ACTOR_BASE_FACTIONS
 *	10000000	CHANGE_ACTOR_BASE_MODIFIERS
 *	00000100	CHANGE_ACTOR_BASE_AIDATA
 *	00000080	CHANGE_ACTOR_BASE_FULLNAME
 *	
 *	npc
 *	00000200	CHANGE_NPC_SKILLS
 *	00000400	CHANGE_NPC_COMBATSTYLE
 *	00000800	CHANGE_NPC_FAME
 *	
 *	creature
 *	00000200	CHANGE_CREATURE_SKILLS
 *	00000400	CHANGE_CREATURE_COMBATSTYLE
 *	
 *	dialog
 *	10000000	CHANGE_TOPIC_SAIDONCE
 *	
 *	package
 *	10000000	CHANGE_PACKAGE_NEVER_RUN
 *	08000000	CHANGE_PACKAGE_WAITING
 *	
 *	cell
 *	00000008	CHANGE_CELL_FLAGS
 *	00000006	CHANGE_CELL_CREATED	### note: two flags?!
 *	10000000	CHANGE_CELL_SEENDATA
 *	08000000	CHANGE_CELL_DETACHTIME
 *	00000020	CHANGE_CELL_OWNERSHIP
 *	00000010	CHANGE_CELL_FULLNAME
 *	01000000	CHANGE_CELL_PATHGRID_MODIFIED
 *	
 *	faction
 *	00000004	CHANGE_FACTION_FLAGS
 *	00000008	CHANGE_FACTION_REACTIONS
 *	
 *	book
 *	00000004	CHANGE_BOOK_TEACHES_SKILL
 *	
 *	refr (object, character or creature)
 *	00000002	CHANGE_CREATED_NEW_REFERENCE
 *	80000000	CHANGEFLAG_REFR_CELL_CHANGED
 *	00000004	CHANGE_REFR_MOVE
 *	00000008	CHANGE_REFR_HAVOK_MOVE
 *	00000800	CHANGEFLAG_REFR_HAD_HAVOK_MOVE_FLAG
 *	00800000	CHANGE_REFR_OBLIVION_FLAG
 *	02000000	CHANGE_REFR_ANIMATION
 *	00000010	CHANGE_REFR_SCALE
 *	00000020	CHANGE_REFR_ALL_EXTRA
 *	40000000	CHANGE_REFR_DISABLE_STATE
 *	10000000	CHANGE_REFR_EXTRA_LEVELED_CREATURE
 *	08000000	CHANGE_REFR_INVENTORY
 *	04000000	CHANGE_REFR_EXTRA_SCRIPT
 *	01000000	CHANGE_REFR_EXTRA_SAVEDMOVEMENTDATA
 *	
 *	object refr
 *	00000400	CHANGE_MAPMARKER_EXTRA_FLAGS
 *	00400000	CHANGE_OBJECT_EXTRA_FURNITURE_MARKER
 *	00200000	CHANGE_OBJECT_EXTRA_MAGIC
 *	00000040	CHANGE_OBJECT_EXTRA_LOCK
 *	00000080	CHANGE_OBJECT_EXTRA_OWNER	
 *	00000100	CHANGE_OBJECT_EXTRA_GLOBAL
 *	00000200	CHANGE_OBJECT_EXTRA_RANK
 *	00020000	CHANGEFLAG_OBJECT_DROPPED_NON_QUEST_ITEM
 *	00010000	CHANGE_OBJECT_EMPTY
 *	
 *	object refr to DOOR
 *	00100000	CHANGE_DOOR_EXTRA_TELEPORT
 *	00080000	CHANGE_DOOR_OPEN_STATE
 *	00040000	CHANGE_DOOR_OPEN_DEFAULT_STATE
 ***/

enum FormType
{
	kFormType_None = 0,
	kFormType_TES4,
	kFormType_Group,
	kFormType_GMST,
	kFormType_Global,
	kFormType_Class,
	kFormType_Faction,
	kFormType_Hair,
	kFormType_Eyes,
	kFormType_Race,
	kFormType_Sound,
	kFormType_Skill,
	kFormType_Effect,
	kFormType_Script,
	kFormType_LandTexture,
	kFormType_Enchantment,
	kFormType_Spell,		// 10
	kFormType_BirthSign,
	kFormType_Activator,
	kFormType_Apparatus,
	kFormType_Armor,
	kFormType_Book,
	kFormType_Clothing,
	kFormType_Container,
	kFormType_Door,
	kFormType_Ingredient,
	kFormType_Light,
	kFormType_Misc,
	kFormType_Stat,	// ???
	kFormType_Grass,
	kFormType_Tree,
	kFormType_Flora,
	kFormType_Furniture,	// 20
	kFormType_Weapon,
	kFormType_Ammo,
	kFormType_NPC,
	kFormType_Creature,
	kFormType_LeveledCreature,
	kFormType_SoulGem,
	kFormType_Key,
	kFormType_AlchemyItem,
	kFormType_SubSpace,
	kFormType_SigilStone,
	kFormType_LeveledItem,
	kFormType_SNDG,
	kFormType_Weather,
	kFormType_Climate,
	kFormType_Region,
	kFormType_Cell,			// 30
	kFormType_REFR,
	kFormType_ACHR,
	kFormType_ACRE,
	kFormType_PathGrid,
	kFormType_WorldSpace,
	kFormType_Land,
	kFormType_TLOD,	// tile low lod?
	kFormType_Road,
	kFormType_Dialog,
	kFormType_DialogInfo,
	kFormType_Quest,
	kFormType_Idle,
	kFormType_Package,
	kFormType_CombatStyle,
	kFormType_LoadScreen,
	kFormType_LeveledSpell,	// 40
	kFormType_ANIO,
	kFormType_WaterForm,
	kFormType_EffectShader,
	kFormType_TOFT
};

class TESObjectLAND;
class TESPathGrid;
class PlayerCharacter;
class TESWorldSpace;
class EnchantmentItem;
class BSFaceGenNiNode;
class TESObjectLIGH;
class TESEffectShader;
class TESSound;
class TESObjectACTI;
class Script;
class TESCombatStyle;
struct BoundObjectListHead;
class TESNPC;
class TESRace;
class Character;

/**** bases ****/

class BaseFormComponent
{
public:
	BaseFormComponent();
	~BaseFormComponent();

	virtual void	Destructor(void);	// 00
	virtual void	Unk_01(void);
	virtual void	CopyFromBase(BaseFormComponent * component);

//	void		** _vtbl;	// 000
	// TESFullName seems to limit the data to this size
};

class TESForm : public BaseFormComponent
{
public:
	TESForm();
	~TESForm();

	enum
	{
		kModified_FormFlags =	0x00000001,
			// CHANGE_FORM_FLAGS
			// can change 0x00112860 for TESObjectREFR, 0x0C20 for others
			// UInt32	flags;
	};

	enum
	{
		// ### need to figure out which objects these belong to
		kModified_GoldValue = 0x08,
		kModified_Name = 0x80
	};

	// TODO: determine which of these are in BaseFormComponent
	virtual void	Unk_03(void);
	virtual void	Unk_04(void);
	virtual void	Unk_05(void);
	virtual void	Unk_06(void);
	virtual void	Unk_07(void);	// load form?
	virtual void	Unk_08(void);
	virtual void	Unk_09(void);	// save form?
	virtual void	Unk_0A(void);
	virtual void	Unk_0B(void);
	virtual void	Unk_0C(void);
	virtual void	Unk_0D(void);
	virtual void	Unk_0E(void);
	virtual void	Unk_0F(void);
	virtual void	MarkAsModified(UInt32 mask);	// 10
	virtual void	Unk_11(void);
	virtual void	Unk_12(void);
	virtual UInt32	GetSaveSize(UInt32 modifiedMask);
	virtual void	SaveGame(UInt32 modifiedMask);	// output buffer not passed as param, accessed indirectly via g_createdBaseObjList
	virtual void	LoadGame(UInt32 modifiedMask, UInt32 unk);
	virtual void	Unk_16(void);
	virtual void	Unk_17(void);
	virtual void	Unk_18(void);
	virtual void	Unk_19(void);
	virtual void	Unk_1A(void);
	virtual void	Unk_1B(void);
	virtual void	Unk_1C(void);
	virtual void	Unk_1D(void);
	virtual void	Unk_1E(void);
	virtual void	Unk_1F(void);
	virtual void	Unk_20(void);	// 20
	virtual void	Unk_21(void);
	virtual void	Unk_22(void);
	virtual void	Unk_23(void);
	virtual void	Unk_24(void);
	virtual void	SetQuestItem(bool isQuestItem);
	virtual void	Unk_26(void);
	virtual void	Unk_27(void);
	virtual void	Unk_28(void);
	virtual void	Unk_29(void);
	virtual void	Unk_2A(void);
	virtual void	Unk_2B(void);
	virtual void	Unk_2C(void);
	virtual void	CopyFrom(TESForm * form);
	virtual bool	Compare(TESForm * form);
	virtual void	Unk_2F(void);
	virtual void	Unk_30(void);	// 30
	virtual void	Unk_31(void);
	virtual void	Unk_32(void);
	virtual void	Unk_33(void);	// activate?
	virtual const char *	GetName(void);	// not sure which objects this works on, doesn't seem to work on player or random objects
	virtual const char *	GetEditorName(void);
	virtual void	SetName(const char * name);

	enum {
		kFormFlags_QuestItem = 0x400,
	};

	struct Unk010
	{
		void	* data;
		Unk010	* next;
	};

	UInt8	typeID;					// 004
	UInt8	typeIDPad[3];			// 005
	UInt32	flags;					// 008
										// 00000040 - something (TESForm_Fn26)
										// 00000400 - quest item
										// 00004000 - temporary?
										// 00010000 - something (TESForm_Fn27)
										// 00020000 - something (TESForm_Fn28)
										// 00080000 - something (sub_464A60)
										// 00100000 - something (sub_464A30)
	UInt32	refID;					// 00C
	Unk010	unk0;					// 010
	// 018

	bool	IsQuestItem() const;
	bool	IsCloned() const;
	bool	SupportsSimpleModel() const;

	void	MarkAsTemporary(void);
};

class TESObject : public TESForm
{
public:
	TESObject();
	~TESObject();

	virtual void	Unk_37(void);
	virtual void	Unk_38(void);
	virtual void	Unk_39(void);
	virtual void	Unk_3A(void);
	virtual void	Unk_3B(void);
	virtual void	Unk_3C(void);
	virtual void	Unk_3D(void);
	virtual void	Unk_3E(void);
	virtual void	Unk_3F(void);
	virtual void	Unk_40(void);
	virtual void	Unk_41(void);
	virtual void	Unk_42(void);
	virtual void	Unk_43(void);
	virtual void	Unk_44(void);

	// 018
};

// 24
class TESBoundObject : public TESObject
{
public:
	TESBoundObject();
	~TESBoundObject();

	virtual void	Unk_45(void);
	virtual void	Unk_46(void);
	virtual void	Unk_47(void);

	BoundObjectListHead*	head;		// 018
	TESBoundObject*			prev;		// 01C
	TESBoundObject*			next;		// 020
};

// 24
class TESBoundTreeObject : public TESBoundObject
{
public:
	TESBoundTreeObject();
	~TESBoundTreeObject();
};

// 24
class TESBoundAnimObject : public TESBoundObject
{
public:
	TESBoundAnimObject();
	~TESBoundAnimObject();

	// size limited by TESSound
};

class TESSoundFile : public BaseFormComponent
{
public:
	TESSoundFile();
	~TESSoundFile();

	// 004
};

/**** components ****/

// C
class TESFullName : public BaseFormComponent
{
public:
	TESFullName();
	~TESFullName();

	String	name;		// 004
};

// 18
class TESModel : public BaseFormComponent
{
public:
	TESModel();
	~TESModel();

	String	nifPath;	// 004
	UInt32	unk2;		// 00C
	UInt32	unk3;		// 010
	UInt32	unk4;		// 014
};

// C
class TESScriptableForm : public BaseFormComponent
{
public:
	TESScriptableForm();
	~TESScriptableForm();

	Script	* script;	// 004
	UInt8	unk1;		// 008
	UInt8	pad[3];		// 009
};

// 008
class TESValueForm : public BaseFormComponent
{
public:
	enum
	{
		kModified_GoldValue =	0x00000008,
			// UInt32	value
	};

	TESValueForm();
	~TESValueForm();

	UInt32	value;
};

// 008
class TESWeightForm : public BaseFormComponent
{
public:
	TESWeightForm();
	~TESWeightForm();

	float	weight;
};

// 008
class TESHealthForm : public BaseFormComponent
{
public:
	TESHealthForm();
	~TESHealthForm();

	UInt32	health;
};

// 008
class TESAttackDamageForm : public BaseFormComponent
{
public:
	TESAttackDamageForm();
	~TESAttackDamageForm();

	UInt16	damage;
	UInt16	unk0;	// bitmask? perhaps 2 UInt8s?
};

// 008
class TESDescription : public BaseFormComponent
{
public:
	TESDescription();
	~TESDescription();

	char	* desc;
};

// 00C
class TESTexture : public BaseFormComponent
{
public:
	TESTexture();
	~TESTexture();

//	char	* path;	// 004 - might be a String? ### verify
//	UInt16	width;	// 008 - not confirmed which is width/height
//	UInt16	height;	// 00A
	String ddsPath;
};

// 00C
class TESIcon : public TESTexture
{
public:
	TESIcon();
	~TESIcon();
};

// 10
class TESReactionForm : public BaseFormComponent
{
public:
	enum
	{
		kModified_Reactions =	0x00000008,
			// CHANGE_FACTION_REACTIONS
			//	UInt16	numChanges;
			//	struct
			//	{
			//		UInt32	unk;
			//		UInt32	unk;
			//	} changes[numChanges];
	};

	struct Data
	{
		UInt32	faction;
		SInt32	reaction;
	};

	struct Entry
	{
		Data	* data;
		Entry	* next;
	};

	TESReactionForm();
	~TESReactionForm();

	Entry	root;		// 004
	UInt8	unk2;		// 00C
	UInt8	pad[3];		// 00D
};

// 14
class TESSpellList : public BaseFormComponent
{
public:
	enum
	{
		kModified_BaseSpellList =	0x00000020,
			// CHANGE_ACTOR_BASE_SPELLLIST
			//	UInt16	numSpells;
			//	UInt32	spells[numSpells];
	};

	TESSpellList();
	~TESSpellList();

	struct Entry
	{
		TESForm	* type;
		Entry	* next;
	};

	Entry	spellList;	// 004
	UInt32	unk2;		// 00C
	UInt32	unk3;		// 010

	// return the nth spell
	TESForm* GetNthSpell(UInt32 whichSpell) const;

	// removes all spells and returns how many spells were removed
	UInt32 RemoveAllSpells();


};

// C?
class TESAttributes : public BaseFormComponent
{
public:
	enum
	{
		kModified_BaseAttributes =		0x00000008,
			// CHANGE_ACTOR_BASE_ATTRIBUTES
			// UInt8	attr[8];
	};

	TESAttributes();
	~TESAttributes();

	UInt8	attr[8];
};

// 10
class TESContainer : public BaseFormComponent
{
public:
	TESContainer();
	~TESContainer();

	struct Data
	{
		SInt32	count;
		TESForm	* type;
	};

	struct Entry
	{
		Data	* data;
		Entry	* next;
	};

	UInt8	type;					// 004
	UInt8	typePad[3];				// 005
	Entry	list;					// 008
};

// 20
class TESActorBaseData : public BaseFormComponent
{
public:
	enum
	{
		kModified_ActorBaseFlags =	0x00000010,
			// CHANGE_ACTOR_BASE_DATA
			// UInt32 flags;

		kModified_BaseFactions =	0x00000040,
			// CHANGE_ACTOR_BASE_FACTIONS
			//	UInt16	numFactions;
			//	struct
			//	{
			//		UInt32	unk0;
			//		UInt32	unk1;
			//	} factions[numFactions];
	};

	TESActorBaseData();
	~TESActorBaseData();

	enum
	{
		kFlag_IsFemale =	0x00000001,
		kFlag_IsEssential = 0x00000002
	};

	UInt32	flags;	// 004 - has flags
						// 00000002 - is essential
	UInt16	unk1;	// 008
	UInt16	unk2;	// 00A
	UInt16	unk3;	// 00C
	UInt16	unk4;	// 00E
	UInt16	unk5;	// 010
	UInt16	unk6;	// 012
	UInt32	unk7;	// 014
	UInt32	unk8;	// 018
	UInt32	unk9;	// 01C

	bool IsEssential() {
		return (flags & kFlag_IsEssential) != 0;
	}

	void SetEssential(bool bEssential) {
		if (bEssential) {
			flags |= kFlag_IsEssential;
		} else {
			flags &= ~kFlag_IsEssential;
		}
	}
	bool IsFemale() {
		return (flags & kFlag_IsFemale) != 0;
	}
};

// 018
class TESAIForm : public BaseFormComponent
{
public:
	enum
	{
		kModified_BaseAIData =	0x00000100,
			// CHANGE_ACTOR_BASE_AIDATA
			// UInt8	unk0;	// +0x4
			// UInt8	unk1;	// +0x5
			// UInt8	unk2;	// +0x6
			// UInt8	unk3;	// +0x7
	};

	TESAIForm();
	~TESAIForm();

	struct Unk
	{
		UInt32	unk0;
		UInt32	unk1;
		UInt32	unk2;
	};

	struct Unk2
	{
		UInt32	unk0;
		UInt32	unk1;
	};

	Unk		unk0;	// 004
	Unk2	unk1;	// 010
};

// C
class TESAnimation : public BaseFormComponent
{
public:
	TESAnimation();
	~TESAnimation();

	UInt32	unk0;	// 04
	UInt32	unk1;	// 08
};

// 00C (1.1)
// 014 (1.2)
class TESModelList : public BaseFormComponent
{
public:
	TESModelList();
	~TESModelList();

	UInt32	unk04;	// 004
	UInt32	unk08;	// 008

#if OBLIVION_VERSION >= OBLIVION_VERSION_1_2
	UInt32	unk0C;	// 00C
	UInt32	unk10;	// 010
#endif
};

// 8
class TESRaceForm : public BaseFormComponent
{
public:
	TESRaceForm();
	~TESRaceForm();

	TESRace* race;	// 004
};

// 8
class TESQualityForm : public BaseFormComponent
{
public:
	TESQualityForm();
	~TESQualityForm();

	float	quality;	// 004
};

// 10
class TESEnchantableForm : public BaseFormComponent
{
public:
	TESEnchantableForm();
	~TESEnchantableForm();

	EnchantmentItem* enchantItem;	// 04
	UInt16	enchantment;			// 08
	UInt16	unk1;					// 0A
	UInt32	unk2;					// 0C
};

// C
class TESProduceForm : public BaseFormComponent
{
public:
	TESProduceForm();
	~TESProduceForm();

	UInt32	unk004;	// 004
	UInt32	unk008;	// 008
};

// 10
class TESLeveledList : public BaseFormComponent
{
public:
	TESLeveledList();
	~TESLeveledList();

	UInt32	unk004;
	UInt32	unk008;
	UInt8	unk00C;
	UInt8	unk00D;
	UInt16	pad00E;
};

/**** bases with components ****/

// 080
class TESBipedModelForm : public BaseFormComponent
{
public:
	TESBipedModelForm();
	~TESBipedModelForm();

	// bit indices starting from lsb
	enum
	{
		kPart_Head =	0,
		kPart_Hair,
		kPart_UpperBody,
		kPart_LowerBody,
		kPart_Hand,
		kPart_Foot,
		kPart_RightRing,
		kPart_LeftRing,
		kPart_Amulet,
		kPart_Weapon,
		kPart_BackWeapon,
		kPart_SideWeapon,
		kPart_Quiver,
		kPart_Shield,
		kPart_Torch,
		kPart_Tail,

		kPart_Max
	};
	enum 
	{
		kFlags_HidesRings = 0,
		kFlags_HidesAmulets,
		kFlags_Unk2,
		kFlags_Unk3,
		kFlags_Unk4,
		kFlags_Unk5,
		kFlags_NotPlayable,
		kFlags_HeavyArmor,
		kFlags_Unk8,
	};

	enum
	{
		kMale = 0,
		kFemale
	};

	UInt16		partMask;			// 004
	UInt16		flags;				// 006

	// male and female models and icons
	TESModel	bipedModel[2];		// 008
	TESModel	groundModel[2];		// 038
	TESIcon		icon[2];			// 068

	UInt32 GetSlot() const;
	void SetSlot(UInt32 slot);
	bool		IsPlayable() const;
	void		SetPlayable(bool bPlayable);
};

// 0E4
class TESActorBase : public TESBoundAnimObject
{
public:
	enum
	{
		//	if(version >= 0x6D)
		//		TESForm flags
		//	TESAttributes flags
		//	TESActorBaseData flags
		//	TESSpellList flags
		//	TESAIForm flags

		kModified_BaseHealth =		0x00000004,
			// CHANGE_ACTOR_BASE_HEALTH
			// UInt32	health;

		kModified_BaseModifiers =	0x10000000,
			// CHANGE_ACTOR_BASE_MODIFIERS
			// stuff with +0xD0

		kModified_FullName =		0x00000080,
			// CHANGE_ACTOR_BASE_FULLNAME
			//	UInt8	strLen;
			//	char	name[strLen];
	};

	TESActorBase();
	~TESActorBase();

	// base classes
	TESActorBaseData	actorBaseData;	// 024
	TESContainer		container;		// 044
	TESSpellList		spellList;		// 054
	TESAIForm			aiForm;			// 068
	TESHealthForm		health;			// 080
	TESAttributes		attributes;		// 088
	TESAnimation		animation;		// 094
	TESFullName			fullName;		// 0A0
	TESModel			model;			// 0AC
	TESScriptableForm	scriptable;		// 0C4

	// 8
	struct Unk2
	{
		UInt8	unk0;	// 000
		UInt8	pad[3];	// 001
		UInt32	unk1;	// 004
	};

	struct Unk
	{
		UInt32	unk0;	// 000
		UInt32	unk1;	// 004
		Unk2	* unk2;	// 008 - alloc'd in ctor
		Unk2	* unk3;	// 00C - alloc'd in ctor
		UInt32	unk4;	// 010
	};

	// Unk::unk2 and Unk::unk3 may not be the same type, they're just the same size and have the same member sizes

	// members
	Unk		unk0;	// 0D0
};

// A8
class EffectSetting : public TESForm
{
public:
	// no changed flags (TESForm flags)

	EffectSetting();
	~EffectSetting();

	// bases
	TESModel			model;			// 018
	TESDescription		description;	// 030
	TESFullName			fullName;		// 038
	TESIcon				texture;		// 044
	UInt32				unk0[2];		// 050
	UInt32				effectFlags;	// 058
	float				baseCost;		// 05C
	UInt32				data;			// 060 bound weapon, actor value
	UInt32				school;			// 064
	UInt32				resistValue;	// 068
	UInt16				numCounters;	// 06C 0: count of counters
	UInt16				unk3;			// 06E
	TESObjectLIGH*		light;			// 070
	float				projSpeed;		// 074
	TESEffectShader*	effectShader;	// 078
	TESEffectShader*	enchantEffect;	// 07C
	TESSound*			castingSound;	// 080
	TESSound*			boltSound;		// 084
	TESSound*			hitSound;		// 088
	TESSound*			areaSound;		// 08C
	float				enchantFactor;	// 090
	float				barterFactor;	// 094
	UInt32				effectCode;		// 098
	void*				counterArray;	// 09C
	UInt32				unk4[2];		// 0A0
	// 0A8

	enum {
		kEffect_IsHostile =			0x1,
		kEffect_CanRecover =		0x1 << 1,
		kEffect_IsDetrimental =		0x1 << 2,
		kEffect_MagnitudePercent =	0x1 << 3,
		kEffect_OnSelfAllowed =		0x1 << 4,
		kEffect_OnTouchAllowed =	0x1 << 5,
		kEffect_OnTargetAllowed =	0x1 << 6,
		kEffect_NoDuration =		0x1 << 7,
		kEffect_NoMagnitude =		0x1 << 8,
		kEffect_NoArea =			0x1 << 9,
		kEffect_FXPersists =		0x1 << 10,
		kEffect_ForSpellmaking =	0x1 << 11,
		kEffect_ForEnchanting =		0x1 << 12,
		kEffect_NoIngredient =		0x1 << 13,
		kEffect_UnknownF =			0x1 << 14,
		kEffect_UnknownG =			0x1 << 15,
		kEffect_UseWeapon =			0x1 << 16,
		kEffect_UseArmor =			0x1 << 17,
		kEffect_UseCreature =		0x1 << 18,
		kEffect_UseSkill =			0x1 << 19,
		kEffect_UseAttribute =		0x1 << 20,
		kEffect_UnknownM =			0x1 << 21, // ABHE, FIDG, FOAT, FOMA, FRDG, OPEN, RSHE, RSMA, SEFF, LISH, SHLD
		kEffect_UnknownN =			0x1 << 22,
		kEffect_UknownO =			0x1 << 23, // POSN
		kEffect_UseActorValue =		0x1 << 24,
		kEffect_UnknownQ =			0x1 << 25,
		kEffect_UnknownR =			0x1 << 26, // FRDG, SEFF
		kEffect_NoHitEffect =		0x1 << 27,
		kEffect_UnknownT =			0x1 << 28, // REAN, VAMP
		kEffect_UnknownU =			0x1 << 29, // DGHE, FIDG, LIDG
		kEffect_UnknownV =			0x1 << 30, // CALM, COCR, COHU, DEMO, FRNZ, TURN
		kEffect_UnknownW =			0x1 << 31, // DARK, DTCT, LGHT, TELE
//		kEffect_NoRecast,
	};

	bool ForSpellmaking() const;
	bool ForEnchanting() const;
	bool IsHostile() const;
	bool IsDetrimental() const;
	bool CanRecover() const;
	bool MagnitudeIsPercent() const;
	bool FXPersists() const;
	bool OnSelfAllowed() const;
	bool OnTouchAllowed() const;
	bool OnTargetAllowed() const;
	bool NoDuration() const;
	bool NoMagnitude() const;
	bool NoArea() const;
	bool NoIngredient() const;
	bool UseWeapon() const;
	bool UseArmor() const;
	bool UseCreature() const;
	bool UseSkill() const;
	bool UseAttribute() const;
	bool UseOtherActorValue() const;
	bool NoRecast() const;
	bool NoHitEffect() const;

	static UInt32 RefIdForC(UInt32 effectCode);
	static EffectSetting* EffectSettingForC(UInt32 effectCode);

	enum {
		kEffect_Alteration = 0,
		kEffect_Conjuration,
		kEffect_Destruction,
		kEffect_Illusion,
		kEffect_Mysticism,
		kEffect_Restoration
	};
};

// 24
class EffectItem {
public:
	EffectItem();
	~EffectItem();

	// mising flags
	UInt32	effectCode;	
	UInt32	magnitude;
	UInt32	area;
	UInt32	duration;

	enum {
		kRange_Self = 0,
		kRange_Touch,
		kRange_Target,
	};

	UInt32	range;
	UInt32	actorValueOrOther;

	struct ScriptEffectInfo
	{
		UInt32 scriptRefID;
		UInt32 school;
		String effectName;
		UInt32 visualEffectCode;
		UInt32 isHostile;

		void SetName(const char* name);
		void SetSchool(UInt32 school);
		void SetVisualEffectCode(UInt32 code);
		void SetIsHostile(bool bIsHostile);
		bool IsHostile() const;
		void SetScriptRefID(UInt32 refID);

		ScriptEffectInfo* Clone() const;
		void CopyFrom(const ScriptEffectInfo* from);
		static ScriptEffectInfo* Create();
	};

	ScriptEffectInfo*	scriptEffectInfo;
	EffectSetting* setting;		
	float unk1;

	bool HasActorValue() const;
	UInt32 GetActorValue() const;
	bool IsValidActorValue(UInt32 actorValue) const;
	void SetActorValue(UInt32 actorValue);

	bool IsScriptedEffect() const;
	UInt32 ScriptEffectRefId() const;
	UInt32 ScriptEffectSchool() const;
	UInt32 ScriptEffectVisualEffectCode() const;
	bool IsScriptEffectHostile() const;

	EffectItem* Clone() const;
	void CopyFrom(const EffectItem* from);
	static EffectItem* Create();
	static EffectItem* ProxyEffectItemFor(UInt32 effectCode);
	
	bool operator<(EffectItem*rhs) const;
	// return the magicka cost of this effect item
	// adjust for skill level if actorCasting is used
	float MagickaCost(TESForm* actorCasting = NULL) const;

	void SetMagnitude(UInt32 magnitude);
	void ModMagnitude(float modBy);
	void SetArea(UInt32 area);
	void ModArea(float modBy);
	void SetDuration(UInt32 duration);
	void ModDuration(float modBy);
	void SetRange(UInt32 range);
	bool IsHostile() const;
};

// 10
class EffectItemList
{
public:
	EffectItemList();
	~EffectItemList();

	struct Entry {
		EffectItem* effectItem;
		Entry* next;
	};

	void	** _vtbl;		// 000 so far this seems to always be a SpellItem vtable*
	Entry		effectList; // 004

	EffectItem* ItemAt(UInt32 whichItem);
	UInt32 AddItem(EffectItem* effectItem);
	UInt32 AddItemCopy(EffectItem* effectItem);
	UInt32 CopyItemFrom(EffectItemList& fromList, UInt32 whichItem);
	bool RemoveItem(UInt32 whichItem);
	bool HasNonHostileItem() const;
	UInt32 CountItems() const;
	UInt32 CountHostileItems() const;
	EffectItem* ItemWithHighestMagickaCost() const;
	UInt32 GetMagickaCost(TESForm* form = NULL) const;
};

// 1C
class MagicItem : public TESFullName
{
public:
	MagicItem();
	~MagicItem();

	EffectItemList	list;	// 00C
};

// 040
class MagicItemObject : public TESBoundObject
{
public:
	MagicItemObject();
	~MagicItemObject();

	// base
	MagicItem	magicItem;	// 024
};

// 034
class MagicItemForm : public TESForm
{
public:
	MagicItemForm();
	~MagicItemForm();

	// base
	MagicItem	magicItem;	// 018
};

/**** forms ****/

class TESGlobal : public TESForm
{
public:
	// no changed flags (TESForm flags)

	TESGlobal();
	~TESGlobal();

	enum
	{
		kType_Short =	's',
		kType_Long =	'l',
		kType_Float =	'f'
	};

	String	name;		// 018
	UInt8	type;		// 020
	UInt8	pad21[3];	// 021
	float	data;		// 024
};

class TESClass : public TESForm
{
public:
	// no changed flags (TESForm flags)

	enum
	{
		kFlag_Playable =	1 << 0,
		kFlag_Guard =		1 << 1,
	};

	enum
	{
		kBuySell_Weapons =		1 << 0,
		kBuySell_Armor =		1 << 1,
		kBuySell_Clothing =		1 << 2,
		kBuySell_Books =		1 << 3,
		kBuySell_Ingredients =	1 << 4,
		kBuySell_Unk5 =			1 << 5,
		kBuySell_Unk6 =			1 << 6,
		kBuySell_Lights =		1 << 7,
		kBuySell_Apparatus =	1 << 8,
		kBuySell_Unk9 =			1 << 9,
		kBuySell_Misc =			1 << 10,
		kBuySell_Spells =		1 << 11,
		kBuySell_MagicItems =	1 << 12,
		kBuySell_Potions =		1 << 13,
		
		kService_Training =		1 << 14,
		kService_Unk15 =		1 << 15,
		kService_Recharge =		1 << 16,
		kService_Repair =		1 << 17,
	};

	TESClass();
	~TESClass();

	// child classes
	TESFullName			fullName;			// 018
	TESDescription		description;		// 024
	TESTexture			texture;			// 02C

	// members
	UInt32				attributes[2];		// 038
	enum {
		eSpec_Combat = 0,
		eSpec_Magic,
		eSpec_Stealth,
	};
	UInt32				specialization;		// 040
	UInt32				majorSkills[7];		// 044
	UInt32				flags;				// 060
	UInt32				buySellServices;	// 064
	UInt8				skillTrained;		// 068
	UInt8				trainingLevel;		// 069
	UInt8				pad6A[2];			// 06A
};

class TESFaction : public TESForm
{
public:
	enum
	{
		// TESForm flags

		// TESReactionForm flags

		kModified_FactionFlags =	0x00000004
			// CHANGE_FACTION_FLAGS
			// UInt8	flags;
	};

	struct RankData
	{
		String		maleRank;
		String		femaleRank;
		TESTexture	insignia;
	};

	struct RankEntry
	{
		RankData	* data;
		RankEntry	* next;
	};

	TESFaction();
	~TESFaction();

	// child classes
	TESFullName		fullName;	// 018
	TESReactionForm	reaction;	// 024

	// members
	UInt8		factionFlags;		// 034
	UInt8		pad35[3];			// 035
	float		crimeGoldMultiplier;// 038
	RankEntry	ranks;				// 03C
};

// 04C
class TESHair : public TESForm
{
public:
	// no changed flags (TESForm flags)

	enum
	{
		kFlag_Playable =	1 << 0,
		kFlag_NotMale =		1 << 1,
		kFlag_NotFemale =	1 << 2,
		kFlag_FixedColor =	1 << 3,
	};

	TESHair();
	~TESHair();

	// child classes
	TESFullName	fullName;	// 018
	TESModel	model;		// 024
	TESTexture	texture;	// 03C

	// members
	UInt8	flags;	// 048
	UInt8	pad[3];	// 049
};

// 34
class TESEyes : public TESForm
{
public:
	// no changed flags (TESForm flags)

	enum
	{
		kFlag_Playable =	1 << 0
	};

	TESEyes();
	~TESEyes();

	// child classes
	TESFullName	fullName;	// 018
	TESTexture	texture;	// 024

	// members
	UInt8	flags;	// 030
	UInt8	pad[3];	// 031
};

// 318
class TESRace : public TESForm
{
public:
	// no changed flags (TESForm flags)

	TESRace();
	~TESRace();

	// child classes
	TESFullName		fullName;	// 018
	TESDescription	desc;		// 024
	TESSpellList	spells;		// 02C
	TESReactionForm	reaction;	// 040

	struct Unk
	{
		UInt32	unk0;
		UInt32	unk1;
		UInt32	unk2;
		UInt32	unk3;
		UInt32	unk4;
		UInt32	unk5;
	};
	struct BonusSkillInfo
	{
		UInt8	skill;
		UInt8	bonus;
	};

	// members
	BonusSkillInfo	bonusSkills[7];// 050
	UInt16			pad0;
	UInt32			unk0[5];	// 050
	TESAttributes	maleAttr;	// 074
	TESAttributes	femaleAttr;	// 080
	UInt32			unk3;		// 08C
	UInt32			unk4;		// 090
	UInt32			unk5[5];	// 094
	UInt32			unk6;		// 0A8
	UInt32			unk7;		// 0AC
	TESModel		unk8[2];	// 0B0
	TESModel		unk9[9];	// 0E0
	TESTexture		unk10[9];	// 1B8
	TESTexture		unk11[10];	// 224
	Unk				unk12[4];	// 29C
	UInt32			unk13[4];	// 2FC
	UInt32			unk14;		// 30C
	UInt16			unk15;		// 310
	UInt16			unk16;		// 312
	UInt16			unk17;		// 314
	UInt16			pad;		// 316

	UInt32 GetBaseAttribute(UInt32 attribute, bool bForFemale) const;
	UInt32 GetSkillBonus(UInt32 skill) const;
	bool IsBonusSkill(UInt32 skill) const;
	UInt32 GetNthBonusSkill(UInt32 n) const;
};

// 8
class TESSound : public TESBoundAnimObject
{
public:
	// no changed flags (TESForm flags)

	TESSound();
	~TESSound();

	// bases
	TESSoundFile	soundFile;	// 024

	// members - may be part of TESSoundFile
	UInt32	unk0;	// 028
	UInt16	unk1;	// 02C
	UInt16	unk2;	// 02E
	UInt32	unk3;	// 030
	UInt16	unk4;	// 034
	UInt16	unk5;	// 036
	UInt8	unk6;	// 038
	UInt8	unk7;	// 039
	UInt8	unk8;	// 03A
	UInt8	unk9;	// 03B
	UInt32	unk10;	// 03C
	UInt16	unk11;	// 040
	UInt16	unk12;	// 042
};

// 60
class TESSkill : public TESForm
{
public:
	// no changed flags (TESForm flags)

	enum
	{
		kSkill_Armorer =	0x0C,
		kSkill_Athletics,
		kSkill_Blade,
		kSkill_Block,
		kSkill_Blunt,		// 10
		kSkill_HandToHand,
		kSkill_HeavyArmor,
		kSkill_Alchemy,
		kSkill_Alteration,
		kSkill_Conjuration,
		kSkill_Destruction,
		kSkill_Illusion,
		kSkill_Mysticism,	// 18
		kSkill_Restoration,
		kSkill_Acrobatics,
		kSkill_LightArmor,
		kSkill_Marksman,
		kSkill_Mercantile,
		kSkill_Security,
		kSkill_Sneak,
		kSkill_Speechcraft,	// 20
	};

	TESSkill();
	~TESSkill();

	// bases
	TESDescription	description;	// 018
	TESTexture		texture;		// 020

	// members
	UInt32			skill;			// 02C
	UInt32			attribute;		// 030
	UInt32			specialization;	// 034
	float			useValue0;		// 038 - amount to learn on successful action
	float			useValue1;		// 03C - action is different for each skill
	TESDescription	levelQuote[4];	// 040

	static TESSkill* SkillForActorVal(UInt32 actorVal);
};

// 50
class Script : public TESForm
{
public:
	// no changed flags (TESForm flags)

	Script();
	~Script();

	// members

	struct RefVariable
	{
		UInt32	unk0;
		UInt32	unk1;
		TESForm	* form;
		UInt32	varIdx;

		void	Resolve(ScriptEventList * eventList);
	};

	struct RefListEntry
	{
		RefVariable		* var;
		RefListEntry	* next;
	};

	struct VariableInfo
	{
		UInt32	idx;		// 00
		UInt32	pad04;		// 04
		double	data;		// 08
		UInt8	type;		// 10
		UInt8	pad11[3];	// 11
		UInt32	unk14;		// 14
		UInt32	unk18;		// 18
		UInt16	unk1C;		// 1C
		UInt16	unk1E;		// 1E
	};

	struct VarInfoEntry
	{
		VariableInfo	* data;
		VarInfoEntry	* next;
	};

	// 14
	struct ScriptInfo
	{
		UInt32	unk0;		// 00 (18)
		UInt32	numRefs;	// 04 (1C)
		UInt32	dataLength;	// 08 (20)
		UInt32	varCount;	// 0C (24)
		UInt32	type;		// 10 (28)
	};

	enum {
		eType_Object = 0,
		eType_Quest = 1,
		eType_Magic = 0x100
	};

	ScriptInfo		info;					// 018
	UInt32			unk1;					// 02C
	void			* data;					// 030
	UInt32			unk3;					// 034
	float			questDelayTimeCounter;	// 038 - init'd to fQuestDelayTime, decremented by frametime each frame
	float			secondsPassed;			// 03C - only if you've modified fQuestDelayTime
	RefListEntry	refList;				// 040 - ref variables and immediates
	VarInfoEntry	varList;				// 048 - local variable list

	RefVariable *	GetVariable(UInt32 reqIdx);

	UInt32			AddVariable(TESForm * form);
	void			CleanupVariables(void);

	void			Constructor(void);
	void			StaticDestructor(void);

	void			SetText(const char * buf);

	// unk0 = some ptr
	// unk1 = 1
	// unk2 = NULL or thisObj?
	void			CompileAndRun(void * unk0, UInt32 unk1, void * unk2);
	UInt32			Type() const { return info.type; }
	bool			IsObjectScript() const {return info.type == eType_Object; }
	bool			IsQuestScript() const { return info.type == eType_Quest; }
	bool			IsMagicScript() const { return info.type == eType_Magic; }
};

STATIC_ASSERT(sizeof(Script) == 0x50);

// 34
class TESLandTexture : public TESForm
{
public:
	// no changed flags (TESForm flags)
 
	TESLandTexture();
	~TESLandTexture();

	// bases
	TESTexture	texture;	// 018

	// members
	UInt32	unk0;	// 024
	UInt8	unk1;	// 028
	UInt8	unk2;	// 029
	UInt8	unk3;	// 02A
	UInt8	unk4;	// 02B
	UInt32	unk5;	// 02C
	UInt32	unk6;	// 030
};

// 44
class EnchantmentItem : public MagicItemForm
{
public:
	// no changed flags (TESForm flags)

	EnchantmentItem();
	~EnchantmentItem();

	enum {
		kEnchant_Scroll = 0,
		kEnchant_Staff,
		kEnchant_Weapon,
		kEnchant_Apparel
	};

	// members
	UInt32	unk0;			// 034 - initialized to 2
	UInt32	enchantType;	// 038 - initialized to FFFFFFFF
	UInt32	charge;			// 03C - initialized to FFFFFFFF
	UInt32	cost;			// 040

	bool MatchesType(TESForm* form);
};

// 44
class SpellItem : public MagicItemForm
{
public:
	// no changed flags (TESForm flags)

	SpellItem();
	~SpellItem();

	enum {
		kType_Spell = 0,
		kType_Disease,
		kType_Power,
		kType_LesserPower,
		kType_Ability,
	};

	enum {
		kLevel_Novice = 0,
		kLevel_Apprentice,
		kLevel_Journeyman,
		kLevel_Expert,
		kLevel_Master,
	};

	enum {
		kFlag_NoAutoCalc = 0x1,
		kFlag_TouchExplodesWithNoTarget = 0x80,
	};

	// members
	UInt32	unk0;			// 034
	UInt32	spellType;		// 038 - init'd to FFFFFFFF
	UInt32	magickaCost;	// 03C
	UInt32	masteryLevel;	// 040
	UInt32	spellFlags;		// 044

	bool IsAutoCalc() const;
	void SetAutoCalc(bool bAutoCalc);
	UInt32 GetSchool() const;
	UInt32 GetMagickaCost(TESForm* form = NULL) const;
	UInt32 GetMasteryLevel() const;
	bool TouchExplodesWithNoTarget() const;
	void SetTouchExplodes(bool bExplodesWithNoTarget);
};

// 4C
class BirthSign : public TESForm
{
public:
	// no changed flags (TESForm flags)

	BirthSign();
	~BirthSign();

	// bases
	TESFullName		fullName;	// 018
	TESTexture		texture;	// 024
	TESDescription	desc;		// 030
	TESSpellList	spellList;	// 038
};

// 58
class TESObjectACTI : public TESBoundAnimObject
{
public:
	// no changed flags (TESForm flags)

	TESObjectACTI();
	~TESObjectACTI();

	// bases
	TESFullName			fullName;	// 024
	TESModel			model;		// 030
	TESScriptableForm	scriptable;	// 048

	// members
	UInt32	unk0;	// 054
};

// 7C
class TESObjectAPPA : public TESBoundObject
{
public:
	// TESForm flags
	// TESValueForm flags

	TESObjectAPPA();
	~TESObjectAPPA();

	// bases
	TESFullName			fullName;	// 024
	TESModel			model;		// 030
	TESIcon				icon;		// 048
	TESScriptableForm	scriptable;	// 054
	TESValueForm		value;		// 060
	TESWeightForm		weight;		// 068
	TESQualityForm		quality;	// 070

	// members
	enum {
		eApparatus_MortarPestle = 0,
		eApparatus_Alembic,
		eApparatus_Calcinator,
		eApparatus_Retort,
	};
	UInt8	appaType;	// 078
	UInt8	pad[3];	// 079

	void SetType(UInt8 appaType);
};

// E8
class TESObjectARMO : public TESBoundObject
{
public:
	// TESForm flags
	// TESValueForm flags

	TESObjectARMO();
	~TESObjectARMO();

	// children
	TESFullName			fullName;		// 024
	TESScriptableForm	scriptable;		// 030
	TESEnchantableForm	enchantable;	// 03C
	TESValueForm		value;			// 04C
	TESWeightForm		weight;			// 054
	TESHealthForm		health;			// 05C
	TESBipedModelForm	bipedModel;		// 064

	// members
	UInt16	armorRating;			// 0E4
	UInt16	pad;					// 0E6

	bool IsHeavyArmor() const;
	void SetHeavyArmor(bool bHeavyArmor);
};

// 8C
class TESObjectBOOK : public TESBoundObject
{
public:
	enum
	{
		// TESForm flags

		// TESValueForm flags

		kModified_TeachesSkill =	0x00000004,
			// CHANGE_BOOK_TEACHES_SKILL
			// UInt8	teachesSkill
	};

	TESObjectBOOK();
	~TESObjectBOOK();

	// children
	TESFullName			fullName;		// 024
	TESModel			model;			// 030
	TESIcon				icon;			// 048
	TESScriptableForm	scriptable;		// 054
	TESEnchantableForm	enchantable;	// 060
	TESValueForm		value;			// 070
	TESWeightForm		weight;			// 078
	TESDescription		description;	// 080

	// members
	enum {
		kBook_IsScroll = 1,
		kBook_CantBeTaken = 1 << 1,
	};

	UInt8				bookFlags;		// 088
	UInt8				teachesSkill;	// 089
	UInt8	pad[2];	// 08A

	bool CantBeTaken() const;
	void SetCantBeTaken(bool bCantBeTaken);
	bool IsScroll() const;
	void SetIsScroll(bool bIsScroll);
	float Teaches() const;
	void SetTeaches(UInt32 skill);
};

// DC
class TESObjectCLOT : public TESBoundObject
{
public:
	// TESForm flags
	// TESValueForm flags

	TESObjectCLOT();
	~TESObjectCLOT();

	// children
	TESFullName			fullName;		// 024
	TESScriptableForm	scriptable;		// 030
	TESEnchantableForm	enchantable;	// 03C
	TESValueForm		value;			// 04C
	TESWeightForm		weight;			// 054
	TESBipedModelForm	bipedModel;		// 05C
};

// 7C
class TESObjectCONT : public TESBoundAnimObject
{
public:
	// no changed flags (TESForm flags)

	TESObjectCONT();
	~TESObjectCONT();

	// child classes
	TESContainer		container;	// 024
	TESFullName			fullName;	// 034
	TESModel			model;		// 040
	TESScriptableForm	scriptable;	// 058
	TESWeightForm		weight;		// 064
	UInt32				unk0;		// 06C
	UInt32				unk1;		// 070
	UInt32				unk2;		// 074
	UInt8				unk3;		// 078
	UInt8				pad[3];		// 079
};

// 70
class TESObjectDOOR : public TESBoundAnimObject
{
public:
	// no changed flags (TESForm flags)

	TESObjectDOOR();
	~TESObjectDOOR();

	// bases
	TESFullName			fullName;	// 024
	TESModel			model;		// 030
	TESScriptableForm	scriptable;	// 048
	// 054 - TESMagicCasterForm
	// 055 - TESMagicTargetForm
	UInt32				basePad;

	// members
	UInt32	unk0;	// 058
	UInt32	unk1;	// 05C
	UInt32	unk2;	// 060
	UInt8	unk3;	// 064
	UInt8	pad[3];	// 065
	UInt32	unk4;	// 068
	UInt32	unk5;	// 06C
};

// 80
class IngredientItem : public MagicItemObject
{
public:
	// no changed flags (TESForm flags)

	IngredientItem();
	~IngredientItem();
	// bases
	UInt32	unk0;
	TESModel			model;		// 040
	TESIcon				icon;		// 058
	TESScriptableForm	scriptable;	// 064
	TESWeightForm		weight;		// 070
	
	// members
	UInt32	value;	// 078 - init'd to FFFFFFFF
	enum {
		kIngred_NoAutocalc = 0x1,
		kIngred_Food = 0x2,
	};
	UInt32	ingredFlags;

	bool IsFood() const { return _IsFlagSet(kIngred_Food); }
	void SetIsFood(bool bFood) { _SetFlag(kIngred_Food, bFood); }
	bool IsAutocalc() const { return !_IsFlagSet(kIngred_NoAutocalc); }
	void SetIsAutocalc(bool bAutocalc) { _SetFlag(kIngred_NoAutocalc, !bAutocalc); }
private:
	bool _IsFlagSet(UInt32 mask) const;
	void _SetFlag(UInt32 flag, bool bSet);
};

// 90
class TESObjectLIGH : public TESBoundAnimObject
{
public:
	// TESForm flags
	// TESValueForm flags

	TESObjectLIGH();
	~TESObjectLIGH();

	// bases
	TESFullName			fullName;	// 024
	TESModel			model;		// 030
	TESIcon				icon;		// 048
	TESScriptableForm	scriptable;	// 054
	TESWeightForm		weight;		// 060
	TESValueForm		value;		// 068

	// members

	// 018
	UInt32	unk0;	// 070
	UInt32	unk1;	// 074
	UInt32	unk2;	// 078
	UInt32	unk3;	// 07C
	UInt32	unk4;	// 080
	float	unk5;	// 084
	float	unk6;	// 088
	UInt32	unk7;	// 08C
};

// 70
class TESObjectMISC : public TESBoundObject
{
public:
	// TESForm flags
	// TESValueForm flags

	TESObjectMISC();
	~TESObjectMISC();

	// bases
	TESFullName			fullName;	// 024
	TESModel			model;		// 030
	TESTexture			texture;	// 048
	TESScriptableForm	scriptable;	// 054
	TESValueForm		value;		// 060
	TESWeightForm		weight;		// 068
};

// 3C
class TESObjectSTAT : public TESBoundObject
{
public:
	// no changed flags (TESForm flags)

	TESObjectSTAT();
	~TESObjectSTAT();

	// children
	TESModel	model;	// 024
};

// 5C
class TESGrass : public TESBoundObject
{
public:
	// no changed flags (TESForm flags)

	TESGrass();
	~TESGrass();

	// bases
	TESModel	model;		// 024

	// members
	UInt8		unk03C;		// 03C
	UInt8		unk03D;		// 03D
	UInt8		unk03E;		// 03E
	UInt8		pad03F;		// 03F
	UInt16		unk040;		// 040
	UInt16		pad042;		// 042
	UInt32		unk044;		// 044
	float		unk048;		// 048
	float		unk04C;		// 04C
	float		unk050;		// 050
	float		unk054;		// 054
	UInt8		unk058;		// 058
	UInt8		pad059[3];	// 059
};

// 80
class TESObjectTREE : public TESBoundTreeObject
{
public:
	// no changed flags (TESForm flags)

	TESObjectTREE();
	~TESObjectTREE();

	// bases
	TESModel	model;		// 024
	TESTexture	texture;	// 03C

	// members
	// NiTArray <uint>
	NiTArray	unk048;		// 048 - 
	UInt16		unk050;		// 050
	UInt16		unk052;		// 052
	UInt16		unk054;		// 054
	UInt16		unk056;		// 056
	float		unk058;		// 058
	float		unk05C;		// 05C
	float		unk060;		// 060
	float		unk064;		// 064
	float		unk068;		// 068
	UInt32		pad06C;		// 06C
	float		unk070;		// 070
	float		unk074;		// 074
	void		* unk078;	// 078
	void		* unk07C;	// 07C
};

// 64
class TESFlora : public TESProduceForm
{
public:
	// no changed flags (no TESForm)

	TESFlora();
	~TESFlora();

	// bases
	TESObjectACTI	activator;	// 00C
};

// 5C
class TESFurniture : public TESObjectACTI
{
public:
	// no changed flags (TESForm flags)

	TESFurniture();
	~TESFurniture();

	// members
	UInt32	unk058;	// 058
};

class TESObjectWEAP : public TESBoundObject
{
public:
	// TESForm flags
	// TESValueForm flags

	TESObjectWEAP();
	~TESObjectWEAP();

	// child classes (exposed for easy access)
	TESFullName			fullName;		// 024
	TESModel			model;			// 030
	TESIcon				icon;			// 048
	TESScriptableForm	scriptable;		// 054
	TESEnchantableForm	enchantable;	// 060
	TESValueForm		value;			// 070
	TESWeightForm		weight;			// 078
	TESHealthForm		health;			// 080
	TESAttackDamageForm	attackDmg;		// 088
	// 090

	enum
	{
		kType_BladeOneHand = 0,
		kType_BladeTwoHand,
		kType_BluntOneHand,
		kType_BluntTwoHand,
		kType_Staff,
		kType_Bow,

		kType_Max,
	};

	// member vars
	UInt32	type;						// 090
	float	speed;						// 094
	float	reach;						// 098
	UInt32	ignoreNormalWeaponResist;	// 09C
};

// 9E
class TESAmmo : public TESBoundObject
{
public:
	// TESForm flags
	// TESValueForm flags

	TESAmmo();
	~TESAmmo();
	// child classes (exposed for easy access)
	TESFullName			fullName;					// 024
	TESModel			model;						// 030
	TESIcon				icon;						// 048
	TESEnchantableForm	enchantable;				// 054
	TESValueForm		value;						// 064
	TESWeightForm		weight;						// 072
	TESAttackDamageForm	attackDmg;					// 080
	float				speed;						// 088
	UInt32				ignoreNormalWeaponResist;	// 092
	UInt32				unk1;						// 096	// seems to be a pointer
	UInt32				unk2;						// 09A	// seems to be flags?  values 0x84 and 0x200
};

// 200
class TESNPC : public TESActorBase
{
public:
	enum
	{
		// TESActorBase flags

		kChanged_TESNPC_EC_15 =	0x00000200,
			// unknown, 0x15 bytes at +0xEC
			// CHANGE_NPC_SKILLS?

		kChanged_TESNPC_1E4 =	0x00000400,
			// calls through the other unknown write/read fn with something at +0x1E4
			// CHANGE_NPC_COMBATSTYLE?
	};

	TESNPC();
	~TESNPC();

	// base classes
	TESRaceForm		race;	// 0E4
	// 0EC

	// 1C
	struct Unk
	{
		UInt32	unk0;	// 000
		UInt32	unk1;	// 004
		UInt32	unk2;	// 008
		UInt32	unk3;	// 00C
		UInt32	unk4;	// 010
		UInt32	unk5;	// 014
	};

	// members
	UInt8		skillLevels[0x15];	// 0EC
	UInt8		pad101[3];			// 101
	TESClass*	npcClass;			// 104
	Unk			unk1[4];			// 108
	Unk			unk2[4];			// 168
	TESHair*	hair;				// 1C8
	UInt32		unk3;				// 1CC
	TESEyes*	eyes;				// 1D0
	BSFaceGenNiNode* face0;			// 1D4 male and female unknown order
	BSFaceGenNiNode* face1;			// 1D8
	UInt32		unk6;				// 1DC
	UInt32		unk7[3];			// 1E0
	UInt32		unk8;				// 1EC
	NiTArray	faceGenUndo;		// 1F0 - NiTArray <FaceGenUndo *>
};

// 138 (1.1)
// 140 (1.2) - changed due to TESModelList
class TESCreature : public TESActorBase
{
public:
	enum
	{
		// TESActorBase flags

		kChanged_CreatureSkills =	0x00000200,
			// CHANGE_CREATURE_SKILLS
			// UInt8	combatSkill;
			// UInt8	magicSkill;
			// UInt8	stealthSkill;

		kChanged_CombatStyle =		0x00000400,
			// SaveGameData2 +0x110
			// CHANGE_CREATURE_COMBATSTYLE
			// UInt32	refIdx;
	};

	TESCreature();
	~TESCreature();

	enum {
		eCreatureType_Creature = 0,
		eCreatureType_Daedra,
		eCreatureType_Undead,
		eCreatureType_Humanoid,
		eCreatureType_Horse,
		eCreatureType_Giant,
	};

	// base classes
	TESAttackDamageForm	attackDamage;	// 0E4
	TESModelList		modelList;		// 0EC - changed size in 1.2

	// members
	void*		sometimesACreature;		// 0F8 / 100
	UInt8		type;					// 0FC / 104
	UInt8		combatSkill;			// 0FD / 105
	UInt8		magicSkill;				// 0FE / 106
	UInt8		stealthSkill;			// 0FF / 107
	UInt32		unk0[2];				// 100 / 108

	float		footWeight;				// 108 / 110
	float		baseScale;				// 10C / 114
	TESCombatStyle*	combatStyle;		// 110 / 118

	TESModel	unk1;					// 114 / 11C
	TESTexture	unk2;					// 12C / 134
};

// 44
class TESLevCreature : public TESBoundObject
{
public:
	// no changed flags (TESForm flags)

	TESLevCreature();
	~TESLevCreature();

	// bases
	TESLeveledList		leveledList;	// 024
	TESScriptableForm	scriptable;		// 034

	// members
	UInt32	unk044;						// 044
};

class TESSoulGem : public TESBoundObject
{
public:
	// TESForm flags
	// TESValueForm flags

	TESSoulGem();
	~TESSoulGem();

	TESFullName			fullName;					// 024
	TESModel			model;						// 030
	TESIcon				icon;						// 048
	TESScriptableForm	scriptable;		// 054
	TESValueForm		value;						// 064
	TESWeightForm		weight;						// 072

	enum {
		kSoul_None = 0,
		kSoul_Petty,
		kSoul_Lesser,
		kSoul_Common,
		kSoul_Greater,
		kSoul_Grand,
	};

	UInt8				soul;
	UInt8				capacity;
	UInt8				padding[2];
};

// 70
class TESKey : public TESObjectMISC
{
public:
	// TESForm flags
	// TESValueForm flags

	TESKey();
	~TESKey();
};

class AlchemyItem : public MagicItemObject
{
public:
	// no changed flags (TESForm flags)

	AlchemyItem();
	~AlchemyItem();

	enum {
		kAlchemy_NoAutocalc = 0x1,
		kAlchemy_IsFood = 0x2,	// in moreFlags
	};

	UInt32				hostileEffectCount;
	TESModel			model;		
	TESIcon				icon;		
	TESScriptableForm	scriptable;	
	TESWeightForm		weight;
	UInt32				unk0;

	UInt32				moreFlags;

	bool IsPoison() const;
	void UpdateHostileEffectCount();
	bool IsFood() const  { return _IsFlagSet(kAlchemy_IsFood); }
	void SetIsFood(bool bFood) { _SetFlag(kAlchemy_IsFood, bFood); }
	bool IsAutocalc() const { return !_IsFlagSet(kAlchemy_NoAutocalc); }
	void SetAutocalc(bool bSet) { return _SetFlag(kAlchemy_NoAutocalc, !bSet); }
private:
	bool _IsFlagSet(UInt32 flag) const;
	void _SetFlag(UInt32 flag, bool bSet);
};

// TESSubSpace
// TESSigilStone
// TESLevItem
// SNDG

// 148
class TESWeather : public TESForm
{
public:
	TESWeather();
	~TESWeather();

	struct FogInfo {
		float nearFog;
		float farFog;
	};


	TESTexture	upperLayer;		// 018
	TESTexture	lowerLayer;		// 024
	TESModel	model;			// 030
	UInt32		unk048[4];			// 048
	FogInfo		fogDay;			// 058
	FogInfo		fogNight;		// 060
	UInt32		unk068[(0x148-0x68) >> 2];		// 068
	// 148
};

// 058
class TESClimate : public TESForm
{
public:
	TESClimate();
	~TESClimate();

	struct WeatherEntry {
		TESWeather* weather;
		UInt32 chance;
	};

	struct WeatherList {
		WeatherEntry* entry;
		WeatherList* next;
	};


	TESModel	nightSky;		// 018
	WeatherList list;
	TESTexture	sun;			// 038
	TESTexture	sunGlare;		// 044
	UInt8		sunriseBegin;	// 050
	UInt8		sunriseEnd;		// these are the number of 10 minute increments past midnight
	UInt8		sunsetBegin;	// 6 increments per hour
	UInt8		sunsetEnd;
	UInt8		volatility;		// 054
	UInt8		moonInfo;
	UInt8		pad[2];
	// 058

	enum {
		kClimate_Masser = 0x80,
		kClimate_Secunda = 0x40,
		kClimate_PhaseLengthMask = 0x3F,
	};
	// moonInfo bits 0-6 define the moon length
	UInt8 GetPhaseLength() const;
	bool HasMasser() const; // moonInfo bit 8
	bool HasSecunda() const; // moonInfo bit 7
};

// TESRegion

// 58
class TESObjectCELL : public TESForm
{
public:
	enum
	{
		kModified_DetachTime = 0x08000000,
			// only present if version >= 0x5A
			// UInt32	detachTime;	// maybe float
			// stored as ExtraData chunk 0x10

		// TESForm bits

		kModified_CellFlags =	0x00000008,
			// CHANGE_CELL_FLAGS
			// UInt8	flags;
			// bits 0x60 stored in flags0
			// bits 0x9F stored in flags1

		kModified_SeenData =	0x10000000,
			// CHANGE_CELL_SEENDATA
			// no data, just adds a ExtraSeenData, then calls a virtual function passing 0xFFFF

		//	if(version < 0x5A)
		//		kModified_DetachTime
		//			(as above)

		kModified_FullName =	0x00000010,
			// CHANGE_CELL_FULLNAME
			// UInt8	strLen;
			// char		name[strLen];

		kModified_Ownership =	0x00000020,
			// CHANGE_CELL_OWNERSHIP
			// UInt32	formIdx;

		kModified_Pathgrid =	0x01000000,
			// CHANGE_CELL_PATHGRID_MODIFIED
			// ### todo: data
	};

	enum
	{
		kFlags0_CantTravelFromHere =	1 << 0,
		kFlags0_HasWater =				1 << 1,
		kFlags0_Unk2 =					1 << 2,
		kFlags0_ForceHideLand =			1 << 3,	// shared bit - for exterior
		kFlags0_OblivionInterior =		1 << 3,	// shared bit - for interior
		kFlags0_Unk4 =					1 << 4,
		kFlags0_Public =				1 << 5,
		kFlags0_HandChanged =			1 << 6,
		kFlags0_BehaveLikeExterior =	1 << 7,
	};

	TESObjectCELL();
	~TESObjectCELL();

	// base class
	TESFullName	fullName;	// 018

	// members
	UInt8			flags0;			// 024
	UInt8			flags1;			// 025
	UInt8			flags2;			// 026
	UInt8			pad27;			// 027
	ExtraDataList	extraData;		// 028
	UInt32			unk0;			// 03C
	TESObjectLAND	* land;			// 040
	TESPathGrid		* pathGrid;		// 044
	TESForm			* objectList;	// 048 - could be linked list of objects in cell
	UInt32			unk4;			// 04C
	TESWorldSpace	* worldSpace;	// 050
	UInt32			unk6;			// 054
};

// TESObjectREFR
// ACHR
// ACHE

// 54
class TESPathGrid : public TESForm
{
public:
	TESPathGrid();
	~TESPathGrid();

	// bases
	TESChildCell	childCell;		// 018

	// members
	UInt32			unk01C;			// 01C
	TESObjectCELL	* theChildCell;	// 020
	UInt32			unk024;			// 024
	UInt32			unk028;			// 028
	UInt32			unk02C;			// 02C
	UInt16			unk030;			// 030
	UInt16			pad032;			// 032

	// NiTPointerMap <class TESObjectREFR *, class BSSimpleList<class TESPathGridPoint *> *>
	NiTPointerMap <void>	objectToGrid;	// 034
	// NiTPointerMap <unsigned int, class BSSimpleList<class TESPathGridPoint *> *>
	NiTPointerMap <void>	refIDToGrid;	// 044
};

// E0
class TESWorldSpace : public TESForm
{
public:
	TESWorldSpace();

	// bases
	TESFullName	fullName;	// 018
	TESTexture	texture;	// 024

	// members
	UInt32	unk030;			// 030
	UInt32	unk034;			// 034

	// NiTPointerMap<int, class TESTerrainLODQuadRoot *>
	NiTPointerMap <void>	map;	// 038
	TESWorldSpace* worldSpace048;	// 048
	UInt32 unk04c[6];				// 04C

	// $NiTPointerMap@IPAV?$BSSimpleList@PAVTESObjectREFR@@@@@@
	NiTPointerMap <void> map064;	// 064
	Character*	character;			// 074
	void*		ptr078;				// 078
	TESWorldSpace* worldSpace07C;	// 07C
	UInt32 unk080[18];				// 080
	// ?$NiTPointerMap@I_N@@
	NiTPointerMap <void> map0C8;	// 0C8
	UInt32 unk0D8[4];				// 0D8
	// 0E0
};

// 28
class TESObjectLAND : public TESForm
{
public:
	TESObjectLAND();
	~TESObjectLAND();

	// base class
	TESChildCell	childCell;	// 018

	// members
	UInt32			unk0;		// 01C
	UInt32			unk1;		// 020
	UInt32			unk2;		// 024
};

// TLOD
// TESRoad
// DIAL
// INFO

// 68
class TESQuest : public TESForm
{
public:
	enum
	{
		// ### order may be wrong (haven't checked code yet)

		kModified_QuestFlags =	0x00000004,
			// CHANGE_QUEST_FLAGS

		kModified_QuestStage =	0x10000000,
			// CHANGE_QUEST_STAGES

		kModified_QuestScript =	0x08000000,
			// CHANGE_QUEST_SCRIPT
	};

	TESQuest();
	~TESQuest();

	// base classes
	TESScriptableForm	scriptable;	// 018
	TESIcon				icon;		// 024
	TESFullName			fullName;	// 030

	struct Unk50
	{
		UInt32	unk0;	// 000
		UInt32	unk1;	// 004
	};

	// members
	UInt8	running;	// 03C
	UInt8	priority;	// 03D
	UInt8	pad0[2];	// 03E
	UInt32	unk0;		// 040
	UInt32	unk1;		// 044
	UInt32	unk2;		// 048
	UInt32	unk3;		// 04C
	Unk50	unk4;		// 050
	UInt32	unk5;		// 058
	UInt8	unk6;		// 05C
	UInt8	pad1[3];	// 05D
	UInt32	unk7;		// 060
	UInt16	unk8;		// 064
	UInt16	unk9;		// 066
};

// +3C	UInt8	bitfield
//					0x00000001 = running
// +3D	UInt8	priority
// +5C	UInt8	stage

// TESIdleForm
// TESPackage
// TESCombatStyle
// TESLoadScreen
// TESLevSpell
// TESObjectANIO
// TESWaterForm
// TESEffectShader

bool IsClonedForm(UInt32 formID);
TESForm* CloneForm(TESForm* formToClone);
