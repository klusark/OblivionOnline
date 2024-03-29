#pragma once

class TESForm;
class TESObjectREFR;
class Script;
struct ParamInfo;
class DataHandler;
class MemoryHeap;

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

	struct Var
	{
		UInt32	id;
		UInt32	unk1;	// align?
		double	data;
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

bool ExtractArgsEx(ParamInfo * paramInfo, void * scriptData, UInt32 * scriptDataOffset, Script * scriptObj, ScriptEventList * eventList, ...);

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

extern MemoryHeap	* g_formHeap;

UInt32 __cdecl ThisCall(UInt32 function, void * _this, ...);
UInt32 __cdecl ThisStdCall(UInt32 function, void * _this, ...);

UInt32 AddFormToDataHandler(DataHandler * dataHandler, TESForm * form);
extern DataHandler ** g_dataHandler;

void AddFormToCreatedBaseObjectsList(void * objList, TESForm * form);
extern void ** g_createdBaseObjList;	// a TESSaveLoadGame
// TESSaveLoadGame + 0x14 is a void * to the current save/load buffer
// TESSaveLoadGame + 0x80 stores a UInt8 containing the version of the save?

UInt32 NiTPointerMap_Lookup(void * map, void * key, void ** data);
extern void * g_gameSettingsTable;

// RTTI
// pass 0 to arg1 and arg4
typedef void * (* _Oblivion_DynamicCast)(void * srcObj, UInt32 arg1, const void * fromType, const void * toType, UInt32 arg4);
extern const _Oblivion_DynamicCast Oblivion_DynamicCast;

extern const void * RTTI_AStarNodeList;
extern const void * RTTI_AStarWorldNodeList;
extern const void * RTTI_AbsorbEffect;
extern const void * RTTI_ActiveEffect;
extern const void * RTTI_Actor;
extern const void * RTTI_AlarmPackage;
extern const void * RTTI_AlchemyItem;
extern const void * RTTI_AlchemyMenu;
extern const void * RTTI_AnimSequenceBase;
extern const void * RTTI_AnimSequenceMultiple;
extern const void * RTTI_AnimSequenceSingle;
extern const void * RTTI_Archive;
extern const void * RTTI_ArchiveFile;
extern const void * RTTI_ArrowProjectile;
extern const void * RTTI_AssociatedItemEffect;
extern const void * RTTI_Atmosphere;
extern const void * RTTI_AudioMenu;
extern const void * RTTI_AverageEntry;
extern const void * RTTI_BSAnimGroupSequence;
extern const void * RTTI_BSArchive;
extern const void * RTTI_BSArchiveHeader;
extern const void * RTTI_BSClearZNode;
extern const void * RTTI_BSCullingProcess;
extern const void * RTTI_BSDoorHavokController;
extern const void * RTTI_BSExtraData;
extern const void * RTTI_BSFaceGenAnimationData;
extern const void * RTTI_BSFaceGenBaseMorphExtraData;
extern const void * RTTI_BSFaceGenImage;
extern const void * RTTI_BSFaceGenKeyframe;
extern const void * RTTI_BSFaceGenKeyframeMultiple;
extern const void * RTTI_BSFaceGenManager;
extern const void * RTTI_BSFaceGenModel;
extern const void * RTTI_BSFaceGenModelExtraData;
extern const void * RTTI_BSFaceGenModelMap;
extern const void * RTTI_BSFaceGenMorph;
extern const void * RTTI_BSFaceGenMorphData;
extern const void * RTTI_BSFaceGenMorphDataHair;
extern const void * RTTI_BSFaceGenMorphDataHead;
extern const void * RTTI_BSFaceGenMorphDifferential;
extern const void * RTTI_BSFaceGenMorphStatistical;
extern const void * RTTI_BSFaceGenNiNode;
extern const void * RTTI_BSFadeNode;
extern const void * RTTI_BSFile;
extern const void * RTTI_BSFileLoader;
extern const void * RTTI_BSPackedAdditionalGeometryData;
extern const void * RTTI_BSPlayerDistanceCheckController;
extern const void * RTTI_BSScissorNode;
extern const void * RTTI_BSScissorTriShape;
extern const void * RTTI_BSSearchPath;
extern const void * RTTI_BSStream;
extern const void * RTTI_BSTECreateTask;
extern const void * RTTI_BSTempEffect;
extern const void * RTTI_BSTempEffectDecal;
extern const void * RTTI_BSTempEffectGeometryDecal;
extern const void * RTTI_BSTempEffectParticle;
extern const void * RTTI_BSTempNode;
extern const void * RTTI_BSTempNodeManager;
extern const void * RTTI_BSTexturePalette;
extern const void * RTTI_BSThread;
extern const void * RTTI_BSTreeManager;
extern const void * RTTI_BSTreeManagerIOTask;
extern const void * RTTI_BSTreeModel;
extern const void * RTTI_BSTreeNode;
extern const void * RTTI_BackgroundLoader;
extern const void * RTTI_BackgroundLoaderThread;
extern const void * RTTI_BaseExtraList;
extern const void * RTTI_BaseFormComponent;
extern const void * RTTI_BaseProcess;
extern const void * RTTI_BirthSign;
extern const void * RTTI_BlendSettingCollection;
extern const void * RTTI_BookMenu;
extern const void * RTTI_BoundItemEffect;
extern const void * RTTI_BreathMenu;
extern const void * RTTI_CBillboardLeaf;
extern const void * RTTI_CIdvCamera;
extern const void * RTTI_CTreeEngine;
extern const void * RTTI_CalmEffect;
extern const void * RTTI_CellMopp;
extern const void * RTTI_ChameleonEffect;
extern const void * RTTI_ChangesMap;
extern const void * RTTI_Character;
extern const void * RTTI_ClassMenu;
extern const void * RTTI_Clouds;
extern const void * RTTI_CombatController;
extern const void * RTTI_CommandCreatureEffect;
extern const void * RTTI_CommandEffect;
extern const void * RTTI_CommandHumanoidEffect;
extern const void * RTTI_CompressedArchiveFile;
extern const void * RTTI_ContainerMenu;
extern const void * RTTI_ControlsMenu;
extern const void * RTTI_Creature;
extern const void * RTTI_CreditsMenu;
extern const void * RTTI_CureEffect;
extern const void * RTTI_DarknessEffect;
extern const void * RTTI_DebugTextExtraData;
extern const void * RTTI_DefaultCombatStyle;
extern const void * RTTI_DemoralizeEffect;
extern const void * RTTI_DetectLifeEffect;
extern const void * RTTI_DialogMenu;
extern const void * RTTI_DialoguePackage;
extern const void * RTTI_DisintegrateArmorEffect;
extern const void * RTTI_DisintegrateWeaponEffect;
extern const void * RTTI_DispelEffect;
extern const void * RTTI_DistantLODLoader;
extern const void * RTTI_DistantLODLoaderTask;
extern const void * RTTI_EffectItemList;
extern const void * RTTI_EffectSetting;
extern const void * RTTI_EffectSettingCollection;
extern const void * RTTI_EffectSettingMenu;
extern const void * RTTI_EnchantmentItem;
extern const void * RTTI_EnchantmentMenu;
extern const void * RTTI_ExteriorCellLoader;
extern const void * RTTI_ExteriorCellLoaderTask;
extern const void * RTTI_ExteriorCellNewReferencesMap;
extern const void * RTTI_ExtraAction;
extern const void * RTTI_ExtraAnim;
extern const void * RTTI_ExtraBoundArmor;
extern const void * RTTI_ExtraCannotWear;
extern const void * RTTI_ExtraCell3D;
extern const void * RTTI_ExtraCellCanopyShadowMask;
extern const void * RTTI_ExtraCellClimate;
extern const void * RTTI_ExtraCellMusicType;
extern const void * RTTI_ExtraCellWaterType;
extern const void * RTTI_ExtraCharge;
extern const void * RTTI_ExtraContainerChanges;
extern const void * RTTI_ExtraCount;
extern const void * RTTI_ExtraCrimeGold;
extern const void * RTTI_ExtraDataList;
extern const void * RTTI_ExtraDetachTime;
extern const void * RTTI_ExtraDistantData;
extern const void * RTTI_ExtraDroppedItemList;
extern const void * RTTI_ExtraEditorID;
extern const void * RTTI_ExtraEditorRefMoveData;
extern const void * RTTI_ExtraEnableStateChildren;
extern const void * RTTI_ExtraEnableStateParent;
extern const void * RTTI_ExtraFollower;
extern const void * RTTI_ExtraFriendHitList;
extern const void * RTTI_ExtraGhost;
extern const void * RTTI_ExtraGlobal;
extern const void * RTTI_ExtraHaggleAmount;
extern const void * RTTI_ExtraHasNoRumors;
extern const void * RTTI_ExtraHavok;
extern const void * RTTI_ExtraHeadingTarget;
extern const void * RTTI_ExtraHealth;
extern const void * RTTI_ExtraInfoGeneralTopic;
extern const void * RTTI_ExtraInvestmentGold;
extern const void * RTTI_ExtraItemDropper;
extern const void * RTTI_ExtraLastFinishedSequence;
extern const void * RTTI_ExtraLevCreaModifier;
extern const void * RTTI_ExtraLeveledCreature;
extern const void * RTTI_ExtraLeveledItem;
extern const void * RTTI_ExtraLight;
extern const void * RTTI_ExtraLock;
extern const void * RTTI_ExtraMapMarker;
extern const void * RTTI_ExtraMerchantContainer;
extern const void * RTTI_ExtraNorthRotation;
extern const void * RTTI_ExtraOblivionEntry;
extern const void * RTTI_ExtraOriginalReference;
extern const void * RTTI_ExtraOwnership;
extern const void * RTTI_ExtraPackage;
extern const void * RTTI_ExtraPackageStartLocation;
extern const void * RTTI_ExtraPersistentCell;
extern const void * RTTI_ExtraPersuasionPercent;
extern const void * RTTI_ExtraPoison;
extern const void * RTTI_ExtraProcessMiddleLow;
extern const void * RTTI_ExtraQuickKey;
extern const void * RTTI_ExtraRagDollData;
extern const void * RTTI_ExtraRandomTeleportMarker;
extern const void * RTTI_ExtraRank;
extern const void * RTTI_ExtraReferencePointer;
extern const void * RTTI_ExtraRefractionProperty;
extern const void * RTTI_ExtraRegionList;
extern const void * RTTI_ExtraRunOncePacks;
extern const void * RTTI_ExtraSavedMovementData;
extern const void * RTTI_ExtraScale;
extern const void * RTTI_ExtraScript;
extern const void * RTTI_ExtraSeed;
extern const void * RTTI_ExtraSeenData;
extern const void * RTTI_ExtraSoul;
extern const void * RTTI_ExtraSound;
extern const void * RTTI_ExtraStartingPosition;
extern const void * RTTI_ExtraStartingWorldOrCell;
extern const void * RTTI_ExtraTeleport;
extern const void * RTTI_ExtraTimeLeft;
extern const void * RTTI_ExtraTravelHorse;
extern const void * RTTI_ExtraTresPassPackage;
extern const void * RTTI_ExtraUsedMarkers;
extern const void * RTTI_ExtraUses;
extern const void * RTTI_ExtraWaterHeight;
extern const void * RTTI_ExtraWorn;
extern const void * RTTI_ExtraWornLeft;
extern const void * RTTI_ExtraXTarget;
extern const void * RTTI_FadeNodeMaxAlphaExtraData;
extern const void * RTTI_FileFinder;
extern const void * RTTI_FleePackage;
extern const void * RTTI_FrenzyEffect;
extern const void * RTTI_GameSettingCollection;
extern const void * RTTI_GameplayMenu;
extern const void * RTTI_GenericMenu;
extern const void * RTTI_GridArray;
extern const void * RTTI_GridCellArray;
extern const void * RTTI_GridDistantArray;
extern const void * RTTI_HUDInfoMenu;
extern const void * RTTI_HUDMainMenu;
extern const void * RTTI_HUDReticle;
extern const void * RTTI_HUDSubtitleMenu;
extern const void * RTTI_HavokError;
extern const void * RTTI_HavokFileStreambufReader;
extern const void * RTTI_HavokFileStreambufWriter;
extern const void * RTTI_HavokStreambufFactory;
extern const void * RTTI_HavokStreambufWriter;
extern const void * RTTI_HighProcess;
extern const void * RTTI_INISettingCollection;
extern const void * RTTI_IOManager;
extern const void * RTTI_IOTask;
extern const void * RTTI_IconArray;
extern const void * RTTI_IdvFileError;
extern const void * RTTI_IngredientItem;
extern const void * RTTI_IntSeenData;
extern const void * RTTI_InteriorCellNewReferencesMap;
extern const void * RTTI_InventoryMenu;
extern const void * RTTI_InvisibilityEffect;
extern const void * RTTI_LevelUpMenu;
extern const void * RTTI_LightEffect;
extern const void * RTTI_LipTask;
extern const void * RTTI_LoadgameMenu;
extern const void * RTTI_LoadingMenu;
extern const void * RTTI_LockEffect;
extern const void * RTTI_LockPickMenu;
extern const void * RTTI_LowProcess;
extern const void * RTTI_MagicBallProjectile;
extern const void * RTTI_MagicBoltProjectile;
extern const void * RTTI_MagicCaster;
extern const void * RTTI_MagicFogProjectile;
extern const void * RTTI_MagicHitEffect;
extern const void * RTTI_MagicItem;
extern const void * RTTI_MagicItemForm;
extern const void * RTTI_MagicItemObject;
extern const void * RTTI_MagicMenu;
extern const void * RTTI_MagicModelHitEffect;
extern const void * RTTI_MagicPopupMenu;
extern const void * RTTI_MagicProjectile;
extern const void * RTTI_MagicShaderHitEffect;
extern const void * RTTI_MagicSprayProjectile;
extern const void * RTTI_MagicTarget;
extern const void * RTTI_MainMenu;
extern const void * RTTI_MapMenu;
extern const void * RTTI_MemoryHeap;
extern const void * RTTI_Menu;
extern const void * RTTI_MessageHandler;
extern const void * RTTI_MessageMenu;
extern const void * RTTI_MiddleHighProcess;
extern const void * RTTI_MiddleLowProcess;
extern const void * RTTI_MobileObject;
extern const void * RTTI_ModelLoaderCloneTask;
extern const void * RTTI_ModelLoaderStreamTask;
extern const void * RTTI_ModelLoaderTask;
extern const void * RTTI_Moon;
extern const void * RTTI_NegotiateMenu;
extern const void * RTTI_NiAVObject;
extern const void * RTTI_NiAdditionalGeometryData;
extern const void * RTTI_NiAlphaProperty;
extern const void * RTTI_NiBSLODNode;
extern const void * RTTI_NiBillboardNode;
extern const void * RTTI_NiBinaryExtraData;
extern const void * RTTI_NiBinaryStream;
extern const void * RTTI_NiCollisionObject;
extern const void * RTTI_NiControllerSequence;
extern const void * RTTI_NiCullingProcess;
extern const void * RTTI_NiDynamicEffect;
extern const void * RTTI_NiExtraData;
extern const void * RTTI_NiFile;
extern const void * RTTI_NiFloatExtraData;
extern const void * RTTI_NiFormArray;
extern const void * RTTI_NiGeometry;
extern const void * RTTI_NiLODNode;
extern const void * RTTI_NiLight;
extern const void * RTTI_NiNode;
extern const void * RTTI_NiObject;
extern const void * RTTI_NiObjectNET;
extern const void * RTTI_NiPointLight;
extern const void * RTTI_NiProperty;
extern const void * RTTI_NiRefObject;
extern const void * RTTI_NiSearchPath;
extern const void * RTTI_NiSkinInstance;
extern const void * RTTI_NiStencilProperty;
extern const void * RTTI_NiStream;
extern const void * RTTI_NiSwitchNode;
extern const void * RTTI_NiTask;
extern const void * RTTI_NiTexturePalette;
extern const void * RTTI_NiThreadProcedure;
extern const void * RTTI_NiTimeController;
extern const void * RTTI_NiTriBasedGeom;
extern const void * RTTI_NiTriShape;
extern const void * RTTI_NiVertexColorProperty;
extern const void * RTTI_NiWireframeProperty;
extern const void * RTTI_NiZBufferProperty;
extern const void * RTTI_NightEyeEffect;
extern const void * RTTI_NonActorMagicCaster;
extern const void * RTTI_NonActorMagicTarget;
extern const void * RTTI_Normal;
extern const void * RTTI_NumericIDBufferMap;
extern const void * RTTI_OpenEffect;
extern const void * RTTI_OptionsMenu;
extern const void * RTTI_ParalysisEffect;
extern const void * RTTI_PathBuilder;
extern const void * RTTI_PathHigh;
extern const void * RTTI_PathLow;
extern const void * RTTI_PathMiddleHigh;
extern const void * RTTI_PauseMenu;
extern const void * RTTI_PersuasionMenu;
extern const void * RTTI_PlayerCharacter;
extern const void * RTTI_PosGen;
extern const void * RTTI_Precipitation;
extern const void * RTTI_QuantityMenu;
extern const void * RTTI_QuickKeysMenu;
extern const void * RTTI_RaceSexMenu;
extern const void * RTTI_Random;
extern const void * RTTI_ReanimateEffect;
extern const void * RTTI_RechargeMenu;
extern const void * RTTI_RegSettingCollection;
extern const void * RTTI_RepairMenu;
extern const void * RTTI_SaveGameFile;
extern const void * RTTI_SaveMenu;
extern const void * RTTI_SceneGraph;
extern const void * RTTI_Script;
extern const void * RTTI_ScriptEffect;
extern const void * RTTI_SeenData;
extern const void * RTTI_ShieldEffect;
extern const void * RTTI_SigilStoneMenu;
extern const void * RTTI_SkillsMenu;
extern const void * RTTI_Sky;
extern const void * RTTI_SkyObject;
extern const void * RTTI_SkyTask;
extern const void * RTTI_SleepWaitMenu;
extern const void * RTTI_SoulTrapEffect;
extern const void * RTTI_SoundCollisionListener;
extern const void * RTTI_SpecificItemCollector;
extern const void * RTTI_SpectatorPackage;
extern const void * RTTI_SpellItem;
extern const void * RTTI_SpellMakingMenu;
extern const void * RTTI_SpellPurchaseMenu;
extern const void * RTTI_Stars;
extern const void * RTTI_StatsMenu;
extern const void * RTTI_StreamHelper;
extern const void * RTTI_SummonCreatureEffect;
extern const void * RTTI_Sun;
extern const void * RTTI_SunDamageEffect;
extern const void * RTTI_SymGen;
extern const void * RTTI_TES;
extern const void * RTTI_TESAIForm;
extern const void * RTTI_TESActorBase;
extern const void * RTTI_TESActorBaseData;
extern const void * RTTI_TESAmmo;
extern const void * RTTI_TESAnimGroup;
extern const void * RTTI_TESAnimation;
extern const void * RTTI_TESArrowTargetListener;
extern const void * RTTI_TESAttackDamageForm;
extern const void * RTTI_TESAttributes;
extern const void * RTTI_TESBipedModelForm;
extern const void * RTTI_TESBoundAnimObject;
extern const void * RTTI_TESBoundObject;
extern const void * RTTI_TESBoundTreeObject;
extern const void * RTTI_TESChildCell;
extern const void * RTTI_TESClass;
extern const void * RTTI_TESClimate;
extern const void * RTTI_TESCombatStyle;
extern const void * RTTI_TESContainer;
extern const void * RTTI_TESCreature;
extern const void * RTTI_TESDescription;
extern const void * RTTI_TESEffectShader;
extern const void * RTTI_TESEnchantableForm;
extern const void * RTTI_TESEyes;
extern const void * RTTI_TESFaction;
extern const void * RTTI_TESFlora;
extern const void * RTTI_TESForm;
extern const void * RTTI_TESFullName;
extern const void * RTTI_TESFurniture;
extern const void * RTTI_TESGlobal;
extern const void * RTTI_TESGrass;
extern const void * RTTI_TESHair;
extern const void * RTTI_TESHealthForm;
extern const void * RTTI_TESIcon;
extern const void * RTTI_TESIconTree;
extern const void * RTTI_TESIdleForm;
extern const void * RTTI_TESKey;
extern const void * RTTI_TESLandTexture;
extern const void * RTTI_TESLevCreature;
extern const void * RTTI_TESLevItem;
extern const void * RTTI_TESLevSpell;
extern const void * RTTI_TESLeveledList;
extern const void * RTTI_TESLoadScreen;
extern const void * RTTI_TESMagicCasterForm;
extern const void * RTTI_TESMagicTargetForm;
extern const void * RTTI_TESMemContextForm;
extern const void * RTTI_TESModel;
extern const void * RTTI_TESModelAnim;
extern const void * RTTI_TESModelList;
extern const void * RTTI_TESModelTree;
extern const void * RTTI_TESNPC;
extern const void * RTTI_TESObject;
extern const void * RTTI_TESObjectACTI;
extern const void * RTTI_TESObjectANIO;
extern const void * RTTI_TESObjectAPPA;
extern const void * RTTI_TESObjectARMO;
extern const void * RTTI_TESObjectBOOK;
extern const void * RTTI_TESObjectCELL;
extern const void * RTTI_TESObjectCLOT;
extern const void * RTTI_TESObjectCONT;
extern const void * RTTI_TESObjectDOOR;
extern const void * RTTI_TESObjectExtraData;
extern const void * RTTI_TESObjectLAND;
extern const void * RTTI_TESObjectLIGH;
extern const void * RTTI_TESObjectMISC;
extern const void * RTTI_TESObjectREFR;
extern const void * RTTI_TESObjectSTAT;
extern const void * RTTI_TESObjectTREE;
extern const void * RTTI_TESObjectWEAP;
extern const void * RTTI_TESPackage;
extern const void * RTTI_TESPathGrid;
extern const void * RTTI_TESProduceForm;
extern const void * RTTI_TESQualityForm;
extern const void * RTTI_TESQuest;
extern const void * RTTI_TESRace;
extern const void * RTTI_TESRaceForm;
extern const void * RTTI_TESReactionForm;
extern const void * RTTI_TESRegion;
extern const void * RTTI_TESRegionData;
extern const void * RTTI_TESRegionDataGrass;
extern const void * RTTI_TESRegionDataLandscape;
extern const void * RTTI_TESRegionDataManager;
extern const void * RTTI_TESRegionDataMap;
extern const void * RTTI_TESRegionDataSound;
extern const void * RTTI_TESRegionDataWeather;
extern const void * RTTI_TESRegionGrassObject;
extern const void * RTTI_TESRegionGrassObjectList;
extern const void * RTTI_TESRegionList;
extern const void * RTTI_TESRegionObjectBase;
extern const void * RTTI_TESRoad;
extern const void * RTTI_TESScriptableForm;
extern const void * RTTI_TESSigilStone;
extern const void * RTTI_TESSkill;
extern const void * RTTI_TESSoulGem;
extern const void * RTTI_TESSound;
extern const void * RTTI_TESSoundFile;
extern const void * RTTI_TESSpellList;
extern const void * RTTI_TESSubSpace;
extern const void * RTTI_TESTerrainLODQuadRoot;
extern const void * RTTI_TESTexture;
extern const void * RTTI_TESTopic;
extern const void * RTTI_TESTopicInfo;
extern const void * RTTI_TESTrapListener;
extern const void * RTTI_TESUsesForm;
extern const void * RTTI_TESValueForm;
extern const void * RTTI_TESWaterCullingProcess;
extern const void * RTTI_TESWaterForm;
extern const void * RTTI_TESWaterListener;
extern const void * RTTI_TESWeather;
extern const void * RTTI_TESWeightForm;
extern const void * RTTI_TESWindListener;
extern const void * RTTI_TESWorldSpace;
extern const void * RTTI_TelekinesisEffect;
extern const void * RTTI_TextEditMenu;
extern const void * RTTI_Tile;
extern const void * RTTI_Tile3D;
extern const void * RTTI_TileImage;
extern const void * RTTI_TileMenu;
extern const void * RTTI_TileRect;
extern const void * RTTI_TileText;
extern const void * RTTI_TileWindow;
extern const void * RTTI_TopicInfoArray;
extern const void * RTTI_TrainingMenu;
extern const void * RTTI_TrespassPackage;
extern const void * RTTI_TurnUndeadEffect;
extern const void * RTTI_Uniform;
extern const void * RTTI_ValueModifierEffect;
extern const void * RTTI_VampirismEffect;
extern const void * RTTI_VideoDisplayMenu;
extern const void * RTTI_VideoMenu;
extern const void * RTTI_WeaponObject;
extern const void * RTTI___non_rtti_object;
extern const void * RTTI_bad_cast;
extern const void * RTTI_bad_typeid;
extern const void * RTTI_bhkAabbPhantom;
extern const void * RTTI_bhkAction;
extern const void * RTTI_bhkArrowTargetListener;
extern const void * RTTI_bhkBackfaceCollisionListener;
extern const void * RTTI_bhkBlendCollisionObject;
extern const void * RTTI_bhkBoxShape;
extern const void * RTTI_bhkBvTreeShape;
extern const void * RTTI_bhkCapsuleShape;
extern const void * RTTI_bhkCharacterController;
extern const void * RTTI_bhkCharacterListener;
extern const void * RTTI_bhkCharacterListenerArrow;
extern const void * RTTI_bhkCharacterListenerSpell;
extern const void * RTTI_bhkCharacterProxy;
extern const void * RTTI_bhkCollisionListener;
extern const void * RTTI_bhkCollisionObject;
extern const void * RTTI_bhkConstraint;
extern const void * RTTI_bhkConvexShape;
extern const void * RTTI_bhkEntity;
extern const void * RTTI_bhkEntityListener;
extern const void * RTTI_bhkLimitedHingeConstraint;
extern const void * RTTI_bhkMouseSpringAction;
extern const void * RTTI_bhkMultiSphereShape;
extern const void * RTTI_bhkNiCollisionObject;
extern const void * RTTI_bhkPCollisionObject;
extern const void * RTTI_bhkPhantom;
extern const void * RTTI_bhkRefObject;
extern const void * RTTI_bhkRigidBody;
extern const void * RTTI_bhkSPCollisionObject;
extern const void * RTTI_bhkSerializable;
extern const void * RTTI_bhkShape;
extern const void * RTTI_bhkShapePhantom;
extern const void * RTTI_bhkSimpleShapePhantom;
extern const void * RTTI_bhkSphereRepShape;
extern const void * RTTI_bhkSphereShape;
extern const void * RTTI_bhkTelekinesisListener;
extern const void * RTTI_bhkTransformShape;
extern const void * RTTI_bhkTrapListener;
extern const void * RTTI_bhkTriSampledHeightFieldBvTreeShape;
extern const void * RTTI_bhkUnaryAction;
extern const void * RTTI_bhkWaterListener;
extern const void * RTTI_bhkWindListener;
extern const void * RTTI_bhkWorldObject;
extern const void * RTTI_exception;
extern const void * RTTI_hkAllCdBodyPairCollector;
extern const void * RTTI_hkAllCdPointCollector;
extern const void * RTTI_hkAllRayHitCollector;
extern const void * RTTI_hkBaseObject;
extern const void * RTTI_hkBroadPhaseCastCollector;
extern const void * RTTI_hkCdBodyPairCollector;
extern const void * RTTI_hkCdPointCollector;
extern const void * RTTI_hkCharacterContext;
extern const void * RTTI_hkCharacterProxyListener;
extern const void * RTTI_hkClosestRayHitCollector;
extern const void * RTTI_hkCollisionListener;
extern const void * RTTI_hkEntityActivationListener;
extern const void * RTTI_hkEntityListener;
extern const void * RTTI_hkError;
extern const void * RTTI_hkMoppCode;
extern const void * RTTI_hkPhantomListener;
extern const void * RTTI_hkRayHitCollector;
extern const void * RTTI_hkReferencedObject;
extern const void * RTTI_hkSerializableCinfo;
extern const void * RTTI_hkStreamReader;
extern const void * RTTI_hkStreamWriter;
extern const void * RTTI_hkStreambufFactory;
extern const void * RTTI_hkWorldRayCaster;
extern const void * RTTI_type_info;

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