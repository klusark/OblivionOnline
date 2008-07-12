#pragma once

#include "GameRTTI.h"
#include "Utilities.h"

class TESForm;
class TESObjectREFR;
class Script;
struct ParamInfo;
class DataHandler;
class MemoryHeap;
class Tile;
class SceneGraph;
class NiNode;
class Tile;
class Menu;

// only records individual objects if there's a block that matches it
// ### how can it tell?
struct ScriptEventList
{
	struct Event
	{
		TESForm	* object;
		UInt32	eventMask;
	};

	struct EventEntry
	{
		Event		* event;
		EventEntry	* next;
	};

	struct VarEntry;

	struct Var
	{
		UInt32		id;
		VarEntry	* nextEntry;
		double		data;
	};

	struct VarEntry
	{
		Var			* var;
		VarEntry	* next;
	};

	Script		* m_script;
	UInt32		m_unk1;
	EventEntry	* m_eventList;
	VarEntry	* m_vars;

	void	Dump(void);
	Var *	GetVariable(UInt32 id);
};

typedef void (* _Console_Print)(const char * buf, ...);
extern const _Console_Print Console_Print;

typedef bool (* _ExtractArgs)(ParamInfo * paramInfo, void * arg1, UInt32 * arg2, TESObjectREFR * arg3, UInt32 arg4, Script * script, ScriptEventList * eventList, ...);
extern const _ExtractArgs ExtractArgs;

// convenience macro for common arguments to ExtractArgs
#define PASS_EXTRACT_ARGS	paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList

bool ExtractArgsEx(ParamInfo * paramInfo, void * scriptData, UInt32 * scriptDataOffset, Script * scriptObj, ScriptEventList * eventList, ...);
bool ExtractFormatStringArgs(UInt32 fmtStringPos, char* buffer, ParamInfo * paramInfo, void * scriptDataIn, UInt32 * scriptDataOffset, Script * scriptObj, ScriptEventList * eventList, UInt32 maxParams, ...);
bool ExtractSetStatementVar(Script* script, ScriptEventList* eventList, void* scriptDataIn, double* outVarData, UInt8* outModIndex = NULL);

typedef TESForm * (* _CreateFormInstance)(UInt8 type);
extern const _CreateFormInstance CreateFormInstance;

typedef TESForm * (* _LookupFormByID)(UInt32 id);
extern const _LookupFormByID LookupFormByID;

typedef void * (* _FormHeap_Allocate)(UInt32 size);
extern const _FormHeap_Allocate FormHeap_Allocate;

typedef void (* _FormHeap_Free)(void * ptr);
extern const _FormHeap_Free FormHeap_Free;

typedef void * (* _GetGlobalScriptStateObj)(void);
extern const _GetGlobalScriptStateObj GetGlobalScriptStateObj;

typedef void (* _ShowMessageBox_Callback)(void);
extern const _ShowMessageBox_Callback ShowMessageBox_Callback;

// message is messageBox string, unk2 is 0, var args are char* for buttons
// pass at least one button string or the messagebox can't be closed
typedef bool (* _ShowMessageBox)(const char * message, _ShowMessageBox_Callback callback, UInt32 unk2, ...);
extern const _ShowMessageBox ShowMessageBox;

// set to scriptObj->refID after calling ShowMessageBox()
// GetButtonPressed checks this before returning a value, if it doesn't match it returns -1
typedef UInt32* _ShowMessageBox_pScriptRefID;
extern const _ShowMessageBox_pScriptRefID ShowMessageBox_pScriptRefID;
typedef UInt8* _ShowMessageBox_button;
extern const _ShowMessageBox_button ShowMessageBox_button;

// unk1 = 0, unk2 = 1
typedef bool (* _QueueUIMessage)(const char * string, UInt32 unk1, UInt32 unk2, float duration);
extern const _QueueUIMessage QueueUIMessage;
const UInt32 kMaxMessageLength = 4096;

//displays icon and plays sound (used by Additem, Addspell, etc...)
//ddsPath relative to Textures\Menus\...  soundID as defined in the CS
typedef bool (* _QueueUIMessage_2)(const char * string, float duration, const char * ddsPath, const char * soundID);
extern const _QueueUIMessage_2 QueueUIMessage_2;

typedef bool (* _IsGodMode)(void);
extern const _IsGodMode IsGodMode;

extern MemoryHeap	* g_formHeap;

UInt32 __cdecl ThisCall(UInt32 function, void * _this, ...);
UInt32 __cdecl ThisStdCall(UInt32 function, void * _this, ...);

UInt32 AddFormToDataHandler(DataHandler * dataHandler, TESForm * form);
extern DataHandler ** g_dataHandler;

// 1C8 (different between 1.1, 1.2)
class TESSaveLoadGame
{
public:
	TESSaveLoadGame();
	~TESSaveLoadGame();

#if OBLIVION_VERSION >= OBLIVION_VERSION_1_2
	UInt32	unk000[0x28 >> 2];	// 000 (1.2)
#else
	UInt32	unk000[0x2C >> 2];	// 000 (1.1)
#endif

	struct CreatedObject
	{
		UInt32			formID;
		CreatedObject	* next;

		UInt32			Info() const	{ return formID; }
		CreatedObject *	Next() const	{ return next; }
	};

	typedef Visitor <CreatedObject, UInt32>	CreatedObjectVisitor;

	CreatedObject	createdObjectList;				// 028 (1.2) 02C (1.1)
	UInt32			unk030[(0x048 - 0x030) >> 2];	// 030 (1.2) 034 (1.1)

	UInt8	numMods;			// 048 (1.2) 04C (1.1)
	UInt8	pad049[3];			// 049 (1.2) 04D (1.1)
	UInt8	* modRefIDTable;	// 04C (1.2) 050 (1.1)
								// table mapping stored mod indices to loaded mod indices

	void	LoadCreatedObjectsHook(UInt32 unk0);
};

#ifdef OBLIVION_VERSION
STATIC_ASSERT(offsetof(TESSaveLoadGame, numMods) == ((OBLIVION_VERSION >= OBLIVION_VERSION_1_2) ? 0x48 : 0x4C));
#endif

void AddFormToCreatedBaseObjectsList(TESSaveLoadGame * objList, TESForm * form);
extern TESSaveLoadGame ** g_createdBaseObjList;	// a TESSaveLoadGame
// TESSaveLoadGame + 0x14 is a void * to the current save/load buffer
// TESSaveLoadGame + 0x80 stores a UInt8 containing the version of the save?

UInt32 NiTPointerMap_Lookup(void * map, void * key, void ** data);
extern void * g_gameSettingsTable;

extern const bool * g_bConsoleMode;
bool IsConsoleMode();

const char * GetObjectClassName(void * obj);

const char * GetFullName(TESForm * baseForm);

enum {
	kActorVal_Strength = 0,		// 0x00
	kActorVal_Intelligence,
	kActorVal_Willpower,
	kActorVal_Agility,
	kActorVal_Speed,
	kActorVal_Endurance,		//0x05
	kActorVal_Personality,
	kActorVal_Luck,
	kActorVal_Health,
	kActorVal_Magicka,
	kActorVal_Fatigue,			// 0x0a
	kActorVal_Encumbrance,
	kActorVal_Armorer,
	kActorVal_Athletics,
	kActorVal_Blade,
	kActorVal_Block,			// 0x0f
	kActorVal_Blunt,			// 0x10
	kActorVal_HandToHand,
	kActorVal_HeavyArmor,
	kActorVal_Alchemy,			
	kActorVal_Alteration,
	kActorVal_Conjuration,		// 0x15
	kActorVal_Destruction,
	kActorVal_Illusion,
	kActorVal_Mysticism,
	kActorVal_Restoration,
	kActorVal_Acrobatics,		// 0x1a
	kActorVal_LightArmor,
	kActorVal_Marksman,
	kActorVal_Mercantile,
	kActorVal_Security,
	kActorVal_Sneak,			// 0x1f
	kActorVal_Speechcraft,		// 0x20
	kActorVal_Aggression,
	kActorVal_Confidence,
	kActorVal_Energy,
	kActorVal_Responsibility,
	kActorVal_Bounty,			// 0x25
	kActorVal_Fame,
	kActorVal_Infamy,
	kActorVal_MagickaMultiplier,
	kActorVal_NightEyeBonus,
	kActorVal_AttackBonus,		// 0x2a
	kActorVal_DefendBonus,
	kActorVal_CastingPenalty,
	kActorVal_Blindness,
	kActorVal_Chameleon,
	kActorVal_Invisibility,		// 0x2f
	kActorVal_Paralysis,		// 0x30
	kActorVal_Silence,
	kActorVal_Confusion,
	kActorVal_DetectItemRange,
	kActorVal_SpellAbsorbChance,
	kActorVal_SpellReflectChance,// 0x35
	kActorVal_SwimSpeedMultiplier,
	kActorVal_WaterBreathing,
	kActorVal_WaterWalking,
	kActorVal_StuntedMagicka,
	kActorVal_DetectLifeRange,	// 0x3a
	kActorVal_ReflectDamage,
	kActorVal_Telekinesis,
	kActorVal_ResistFire,
	kActorVal_ResistFrost,
	kActorVal_ResistDisease,	// 0x3f
	kActorVal_ResistMagic,		// 0x40
	kActorVal_ResistNormalWeapons,
	kActorVal_ResistParalysis,
	kActorVal_ResistPoison,
	kActorVal_ResistShock,
	kActorVal_Vampirism,		// 0x45
	kActorVal_Darkness,
	kActorVal_ResistWaterDamage,
	///
	kActorVal_OblivionMax,
};

extern char*** g_baseActorValueNames;		//those with an associated game setting string
extern char** g_extraActorValueNames;		//MagickaMultiplier .. ResistWaterDamage (unchangeable)
const char* GetActorValueString(UInt32 actorValue);
bool IsValidActorValue(UInt32 actorValue);

UInt32 SafeModUInt32(UInt32 originalVal, float modBy);
float SafeChangeFloat(float originalVal, float changeVal, bool bMod, bool bNegativeAllowed);

extern CRITICAL_SECTION * g_extraListMutex;
extern CRITICAL_SECTION * g_pathingMutex;

struct SettingInfo
{
	union
	{
		int		i;
		UInt32	u;
		float	f;
		char	* s;
	};
	
	char	* name;
	
	enum EType {
		kSetting_Bool = 0,
		kSetting_c,
		kSetting_h,
		kSetting_Integer,
		kSetting_Unsigned,
		kSetting_Float,
		kSetting_String,
		kSetting_r,
		kSetting_a,
		kSetting_Other
	};

	EType Type() const;
};

bool GetGameSetting(char *settingName, SettingInfo** setting);

// 134
class InterfaceManager
{
public:
	InterfaceManager();
	~InterfaceManager();

	static InterfaceManager *	GetSingleton(void);

	SceneGraph*		unk000;							// 000
	SceneGraph*		unk004;							// 004
	UInt32			unk008[(0x018 - 0x008) >> 2];	// 008
	void*			unk018;							// 018 NiDirectionalLight *
	Tile			* cursor;						// 01C
	UInt32			unk020[(0x054 - 0x020) >> 2];	// 020
	NiNode*			unk054[(0x064 - 0x054) >> 2];	// 054 
	NiNode*			unk064;							// 064 ShadowSceneNode *
	Tile			* menuRoot;						// 068
	Tile			* strings;						// 06C
	NiNode*			unk070;							// 070
	UInt32			unk074;							// 074
	void*			unk078;							// 078 NiAlphaProperty*
	UInt32			unk07C;							// 07C
	Tile			* hudReticule;					// 080
	UInt32			unk084;							// 084
	Tile			* unk088;						// 088
	UInt32			unk08C;							// 08C
	UInt32			unk090;							// 090
	UInt32			unk094;							// 094
	Tile			* activeTile;					// 098 - moused-over tile
	Menu			* activeMenu;					// 09C - menu over which the mouse cursor is placed
	UInt32			unk0A0[(0x0BC - 0x0A0) >> 2];	// 0A0
	TESObjectREFR	* debugSelection;				// 0BC
	UInt32			unk0C0[(0x134 - 0x0C0) >> 2];	// 0C0

	bool CreateTextEditMenu(const char* promptText, const char* defaultText);
};

STATIC_ASSERT(offsetof(InterfaceManager, activeMenu) == 0x09C);
STATIC_ASSERT(sizeof(InterfaceManager) == 0x134);

#define FORMAT_STRING_PARAMS 	\
	{"format string",	kParamType_String, 0}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1}, \
	{"variable",		kParamType_Float, 1} 
