#pragma once

#include "obse/GameForms.h"
#include "obse/GameExtraData.h"
#include "obse/GameProcess.h"

/*** class hierarchy
 *	
 *	this information comes from the RTTI information embedded in the exe
 *	so no, I don't have magical inside information
 *	
 *	sadly bethesda decided to use /lots/ of multiple inheritance, so this is
 *	going to be very difficult to access externally.
 *	
 *	it'll probably be best to expose all of the rtti structs and the dynamic cast interface I guess
 *	
 *	TESObjectREFR can be directly cast to Character
 *	
 *													total vtbl size
 *	BaseFormComponent
 *		TESForm										0x37
 *			TESObjectREFR							0x69
 *				MobileObject						0x81
 *					Actor							0xEF
 *						Character					0xEF
 *							PlayerCharacter			0xEF
 *						Creature					0xEF
 *					ArrowProjectile					0x81
 *					MagicProjectile					0x89
 *						MagicBallProjectile			0x89
 *						MagicBoltProjectile			0x89
 *						MagicFogProjectile			0x89
 *						MagicSprayProjectile		0x89
 *			TESPackage
 *				FleePackage
 *				DialogPackage
 *				CombatController
 *				AlarmPackage
 *				SpectatorPackage
 *				TresspassPackage
 *			TESTopicInfo
 *			TESTopic
 *			TESSkill
 *			TESRace
 *			TESQuest
 *			TESObject
 *				TESBoundObject
 *					TESBoundAnimObject
 *						TESActorBase
 *							TESNPC
 *							TESCreature
 *						TESObjectCONT
 *					TESObjectMISC
 *					TESObjectBOOK
 *					TESLevItem
 *			Script
 *		TESFullName
 *			TESTopic
 *			TESRace
 *			TESQuest
 *			TESObject
 *		TESDescription
 *			TESSkill
 *			TESRace
 *		TESTexture
 *			TESSkill
 *			TESIcon
 *				TESQuest
 *		TESSpellList
 *			TESRace
 *			TESObject
 *		TESReactionForm
 *			TESRace
 *		TESScriptableForm
 *			TESQuest
 *			TESObject
 *		TESActorBaseData
 *			TESObject
 *		TESContainer
 *			TESObject
 *		TESAIForm
 *			TESObject
 *		TESHealthForm
 *			TESObject
 *		TESAttributes
 *			TESObject
 *		TESAnimation
 *			TESObject
 *		TESModel
 *			TESObject
 *		TESRaceForm
 *			TESObject
 *		TESGlobal
 *	
 *	TESMemContextForm
 *		TESObjectREFR
 *			...
 *	
 *	TESChildCell
 *		MobileObject
 *			...
 *	
 *	MagicCaster
 *		Actor
 *			...
 *		NonActorMagicCaster
 *			BSExtraData
 *	
 *	MagicTarget
 *		Character
 *			...
 *		NonActorMagicTarget
 *			BSExtraData
 *	
 *	BaseProcess
 *		LowProcess
 *			MiddleLowProcess
 *				MiddleHighProcess
 *					HighProcess
 *	
 *	IOTask
 *		LipTask
 *		SkyTask
 *	
 *	NiRefObject
 *		NiObject
 *			NiTimeController
 *				BSPlayerDistanceCheckController
 *				BSDoorHavokController
 *			NiExtraData
 *				Tile::Extra
 *				DebugTextExtraData
 *				BSFaceGenBaseMorphExtraData
 *				BSFaceGenModelExtraData
 *				BSFaceGenAnimationData
 *			BSTempEffect
 *				BSTempEffectParticle
 *				BSTempEffectGeometryDecal
 *				BSTempEffectDecal
 *				MagicHitEffect
 *					MagicModelHitEffect
 *					MagicShaderHitEffect
 *			NiSkinInstance
 *			NiTask
 *				BSTECreateTask
 *			bhkRefObject
 *				bhkSerializable
 *					bhkShape
 *						bhkSphereRepShape
 *							bhkConvexShape
 *								bhkBoxShape
 *								bhkCapsuleShape
 *							bhkMultiSphereShape
 *						bhkTransformShape
 *			NiObjectNET
 *				NiAVObject
 *					NiNode
 *						BSTreeNode
 *						BSFaceGenNiNode
 *			NiCollisionObject
 *				bhkNiCollisionObject
 *					bhkPCollisionObject
 *						bgkSPCollisionObject
 *					bhkCollisionObject
 *						bhkBlendCollisionObject
 *							WeaponObject
 *		BSTreeModel
 *		BSFaceGenMorphDataHair
 *			BSFaceGenMorphDataHead
 *		BSFaceGenModel
 *	
 *	BSFaceGenMorph
 *		BSFaceGenMorphStatistical
 *		BSFaceGenMorphDifferential
 *	
 *	Menu
 *		VideoDisplayMenu
 *		TrainingMenu
 *		StatsMenu
 *		SpellPurchaseMenu
 *		SpellMakingMenu
 *		SleepWaitMenu
 *		SkillsMenu
 *		SigilStoneMenu
 *		SaveMenu
 *		RepairMenu
 *		RechargeMenu
 *		RaceSexMenu
 *		QuickKeysMenu
 *		QuantityMenu
 *		PersuasionMenu
 *		PauseMenu
 *		OptionsMenu
 *		NegotiateMenu
 *		MessageMenu
 *		MapMenu
 *		MainMenu
 *		MagicPopupMenu
 *		MagicMenu
 *		LockPickMenu
 *		LoadgameMenu
 *		LoadingMenu
 *		LevelUpMenu
 *		InventoryMenu
 *		HUDSubtitleMenu
 *		HUDReticle
 *		HUDMainMenu
 *		HUDInfoMenu
 *		GenericMenu
 *		GameplayMenu
 *		EnchantmentMenu
 *		EffectSettingMenu
 *		DialogMenu
 *		CreditsMenu
 *		ContainerMenu
 *		ClassMenu
 *		BreathMenu
 *		BookMenu
 *		AudioMenu
 *		AlchemyMenu
 *		VideoMenu
 *		TextEditMenu
 *		ControlsMenu
 *	
 *	Tile
 *		TileWindow
 *		TileRect
 *			TileMenu
 *		Tile3D
 *		TileText
 *		TileImage
 *	
 *	BackgroundLoader
 *		BSFaceGenManager
 *	
 *	BSFaceGenKeyframe
 *		BSFaceGenKeyframeMultiple
 *	
 *	SkyObject
 *		Sun
 *		Stars
 *		Moon
 *		Clouds
 *		Atmosphere
 *	
 *	Sky
 *	
 *	PathLow
 *		PathMiddleHigh
 *			PathHigh
 *	
 *	ActiveEffect
 *		ValueModifierEffect
 *			AbsorbEffect
 *			CalmEffect
 *			ChameleonEffect
 *			DarknessEffect
 *			DetectLifeEffect
 *			FrenzyEffect
 *			InvisibilityEffect
 *			NightEyeEffect
 *			ParalysisEffect
 *			ShieldEffect
 *			TelekinesisEffect
 *		AssociatedItemEffect
 *			BoundItemEffect
 *			SummonCreatureEffect
 *		CommandEffect
 *			CommandCreatureEffect
 *			CommandHumanoidEffect
 *		CureEffect
 *		DemoralizeEffect
 *		DisintegrateArmorEffect
 *		DisintegrateWeaponEffect
 *		DispelEffect
 *		LightEffect
 *		LockEffect
 *		OpenEffect
 *		ReanimateEffect
 *		ScriptEffect
 *		SoulTrapEffect
 *		SunDamageEffect
 *		TurnUndeadEffect
 *		VampirismEffect
 *	
 *	[ havok stuff ]
 *		bhkCharacterListenerArrow
 *	
 *	Menu vtbl + 0x0C = event handler
 *	
 ***/

class Actor;
class NiNode;
class Atmosphere;
class Stars;
class Sun;
class Clouds;
class Moon;
class Precipitation;
class MagicTarget;
class MagicCaster;
class EffectItem;
class ActiveEffect;
class DialoguePackage;
class Creature;
class BoltShaderProperty;

// 00C
class MagicCaster
{
public:
	MagicCaster();
	~MagicCaster();

	virtual void	Unk_00(UInt32 arg0, UInt32 arg1);
	virtual void	Unk_01(UInt32 arg0, UInt32 arg1, UInt32 arg2);
	virtual void	Unk_02(UInt32 arg0, UInt32 arg1, UInt32 arg2);
	virtual void	Unk_03(void);
	virtual void	Unk_04(void);
	virtual void	Unk_05(void);
	virtual void	Unk_06(void);
	virtual void	Unk_07(UInt32 arg0, UInt32 arg1, UInt32 arg2, UInt32 arg3);
	virtual void	Unk_08(void);
	virtual void	Unk_09(void);
	virtual void	Unk_0A(void);
	virtual void	Unk_0B(void);
	virtual void	Unk_0C(void);
	virtual void	Unk_0D(void);
	virtual void	Unk_0E(void);
	virtual void	Unk_0F(void);

//	void	** _vtbl;	// 000
	UInt32	unk0;		// 004
	UInt32	unk1;		// 008
};

// 008+
class MagicTarget
{
public:
	MagicTarget();
	~MagicTarget();

	// 8
	struct EffectNode
	{
		ActiveEffect	* data;
		EffectNode		* next;

		ActiveEffect* Info() const { return data; }
		EffectNode* Next() const { return next; }
	};

	virtual void	Destructor(void);
	virtual TESObjectREFR *	GetParent(void);
	virtual EffectNode *	GetEffectList(void);

//	void	** _vtbl;	// 000
	UInt32	unk0;		// 004
};

/*

	virtual void	Unk_0(void) = 0;
	virtual void	Unk_1(void) = 0;
	virtual void	Unk_2(void) = 0;
	virtual void	Unk_3(void) = 0;
	virtual void	Unk_4(void) = 0;
	virtual void	Unk_5(void) = 0;
	virtual void	Unk_6(void) = 0;
	virtual void	Unk_7(void) = 0;
	virtual void	Unk_8(void) = 0;
	virtual void	Unk_9(void) = 0;
	virtual void	Unk_A(void) = 0;
	virtual void	Unk_B(void) = 0;
	virtual void	Unk_C(void) = 0;
	virtual void	Unk_D(void) = 0;
	virtual void	Unk_E(void) = 0;
	virtual void	Unk_F(void) = 0;

*/

// 058
class TESObjectREFR : public TESForm
{
public:
	enum
	{
		//	TESForm flags
		//	if(!IsActor())
				kChanged_IsEmpty =		0x00010000,
					// CHANGE_OBJECT_EMPTY
					// no data?
		
		kChanged_Inventory =			0x08000000,
			// CHANGE_REFR_INVENTORY
			// ### todo: see 0048BA40

		//	if((changed & ((version < 0x43) ? 0x177577F0 : 0x177577E0))) || IsActor())
		//	{
		//		// this is all part of its own function
		//		
		//	}

		//	if(!IsActor())
				kChanged_Animation =	0x02000000,
					// CHANGE_REFR_ANIMATION
					// UInt16	dataLen;
					// UInt8	data[dataLen];

		kChanged_HavokMove =			0x00000008,
			// CHANGE_REFR_HAVOK_MOVE
		kChanged_HadHavokMoveFlag =		0x00000800,
			// CHANGEFLAG_REFR_HAD_HAVOK_MOVE_FLAG
			// if either of these are set
			// UInt16	dataLen;
			// UInt8	data[dataLen];

		//	if(version > 0x43)
				kChanged_Scale =		0x00000010,
					// CHANGE_REFR_SCALE
					// float	scale;

		kChanged_DoorOpenDefaultState =	0x00040000,
			// CHANGE_DOOR_OPEN_DEFAULT_STATE
			// no data

		kChanged_DoorOpenState =		0x00080000,
			// CHANGE_DOOR_OPEN_STATE
			// no data
	};

	enum
	{
		kFlags_Persistent	= 0x00000400,		//shared bit with kFormFlags_QuestItem
		kFlags_Taken		= 0x00000022,		//2 bits, both set when picked up by NPC/PC - need to figure out what each means
	};

	TESObjectREFR();
	~TESObjectREFR();

	virtual void	Unk_37(void) = 0;
	virtual void	Unk_38(void) = 0;	// 38
	virtual void	Unk_39(void) = 0;
	virtual void	Unk_3A(void) = 0;
	virtual float	GetScale(void) = 0;
	virtual void	GetStartingAngle(float * pos) = 0;
	virtual void	GetStartingPos(float * pos) = 0;
	virtual void	Unk_3E(void) = 0;
	virtual void	Unk_3F(void) = 0;
	virtual void	Unk_40(void) = 0;	// 40
	virtual void	Unk_41(void) = 0;
	virtual void	Unk_42(void) = 0;
	virtual void	Unk_43(void) = 0;
	virtual void	Unk_44(void) = 0;
	virtual void	Unk_45(void) = 0;
	virtual void	Unk_46(void) = 0;
	virtual void	Unk_47(void) = 0;
	virtual void	Unk_48(void) = 0;
	virtual MagicTarget *	GetMagicTarget(void) = 0;
	virtual void	Unk_4A(void) = 0;
	virtual void	Unk_4B(void) = 0;
	virtual void	Unk_4C(void) = 0;
	virtual void	Unk_4D(void) = 0;
	virtual void	Unk_4E(void) = 0;
	virtual void	Unk_4F(void) = 0;
	virtual void	Unk_50(void) = 0;	// 50
	virtual void	Unk_51(void) = 0;
	virtual void	Unk_52(void) = 0;
	virtual void	Unk_53(void) = 0;
	virtual void	Unk_54(void * object) = 0;	// Set3D? object is a NiNode* ?
	virtual NiNode *	GetNiNode(void) = 0;
	virtual void	Unk_56(void) = 0;
	virtual void	Unk_57(UInt32 arg0) = 0;
	virtual void	Unk_58(void) = 0;
	virtual void	Unk_59(void) = 0;
	virtual void	Unk_5A(void) = 0;
	virtual void	Unk_5B(void) = 0;
	virtual TESForm *	GetBaseForm(void) = 0;	// returns type this object references
	virtual float *	GetPos(void) = 0;
	virtual void	Unk_5E(void) = 0;
	virtual void	Unk_5F(void) = 0;
	virtual void	Unk_60(void) = 0;	// 60
	virtual void	Unk_61(void) = 0;
	virtual void	Unk_62(void) = 0;
	virtual void	Unk_63(void) = 0;
	virtual bool	IsActor(void) = 0;
	virtual void	ChangeCell(TESObjectCELL * newCell) = 0;
	virtual void	Unk_66(void) = 0;
	virtual void	Unk_67(void) = 0;
	virtual void	Unk_68(void) = 0;
	virtual void	Unk_69(void) = 0;

	TESChildCell	childCell;		// 018
	TESForm	* baseForm;				// 01C
										// U8(typeInfo + 4) == 0x23 is true if the object is a character
	float	rotX, rotY, rotZ;		// 020 - either public or accessed via simple inline accessor common to all child classes
	float	posX, posY, posZ;		// 02C - seems to be private
	float	scale;					// 038 
	NiNode	* niNode;				// 03C
	TESObjectCELL	* parentCell;	// 040
	BaseExtraList	baseExtraList;	// 044

	ScriptEventList* GetEventList() const;
	bool IsTaken() const
		{	return ((flags & kFlags_Taken) == kFlags_Taken) ? true : false;	}
};

// 05C+
class MobileObject : public TESObjectREFR
{
public:
	enum
	{
		//	UInt8	processLevel;
		//		FF - none
		//		00 - high
		//		01 - medium high
		//		02 - medium low
		//		03 - low
	};

	MobileObject();
	~MobileObject();

	virtual void	Unk_6A(void) = 0;	// 6A
	virtual void	Unk_6B(void) = 0;
	virtual void	Unk_6C(void) = 0;
	virtual void	Unk_6D(void) = 0;
	virtual void	Unk_6E(void) = 0;
	virtual void	Unk_6F(void) = 0;
	virtual void	Unk_70(void) = 0;	// 70
	virtual void	Unk_71(void) = 0;
	virtual void	Unk_72(void) = 0;
	virtual void	Unk_73(void) = 0;
	virtual void	Unk_74(void) = 0;
	virtual void	Unk_75(void) = 0;
	virtual void	Unk_76(void) = 0;
	virtual void	Unk_77(void) = 0;
	virtual void	Unk_78(void) = 0;
	virtual void	Unk_79(void) = 0;
	virtual void	Unk_7A(void) = 0;
	virtual void	Unk_7B(void) = 0;
	virtual void	Unk_7C(void) = 0;
	virtual void	Unk_7D(void) = 0;
	virtual void	Unk_7E(void) = 0;
	virtual void	Unk_7F(void) = 0;
	virtual void	Unk_80(void) = 0;	// 80
	virtual void	Unk_81(void) = 0;

	BaseProcess	* process;			// 058
};

// 104+
class Actor : public MobileObject
{
public:
	Actor();
	~Actor();

	virtual SInt32	GetInfamy(void) = 0;	// 82
	virtual void	Unk_83(void) = 0;
	virtual void	Unk_84(void) = 0;
	virtual void	Unk_85(void) = 0;
	virtual void	Unk_86(void) = 0;
	virtual void	Unk_87(void) = 0;
	virtual void	Unk_88(void) = 0;
	virtual void	Unk_89(void) = 0;
	virtual void	Unk_8A(void) = 0;	// handles input for PlayerCharacter
	virtual void	Unk_8B(void) = 0;
	virtual void	Unk_8C(void) = 0;
	virtual void	Unk_8D(void) = 0;
	virtual void	Unk_8E(void) = 0;
	virtual void	Unk_8F(void) = 0;
	virtual void	Unk_90(void) = 0;	// 90
	virtual void	Unk_91(void) = 0;
	virtual void	Unk_92(void) = 0;	// SendTrespassAlarm
	virtual void	Unk_93(void) = 0;
	virtual void	Unk_94(void) = 0;
	virtual void	Unk_95(void) = 0;
	virtual void	Unk_96(void) = 0;
	virtual void	Unk_97(void) = 0;
	virtual void	Unk_98(void) = 0;
	virtual void	Unk_99(void) = 0;
	virtual void	Unk_9A(void) = 0;
	virtual void	Unk_9B(void) = 0;
	virtual void	Unk_9C(void) = 0;
	virtual void	Unk_9D(void) = 0;
	virtual void	Unk_9E(void) = 0;
	virtual void	Unk_9F(void) = 0;
	virtual void	Unk_A0(void) = 0;	// A0
	virtual UInt32	GetActorValue(UInt32 id) = 0;
	virtual void	Unk_A2(void) = 0;
	virtual void	Unk_A3(void) = 0;
	virtual void	SetActorValue(UInt32 value, UInt32 amount) = 0;
	virtual void	Unk_A5(void) = 0;
	virtual void	Unk_A6(void) = 0;
	virtual void	Unk_A7(void) = 0;
	virtual void	ModActorValue(UInt32 value, UInt32 amount, UInt32 unk) = 0;
	virtual void	Unk_A9(void) = 0;
	virtual void	ModActorBaseValue(UInt32 value, UInt32 amount, UInt32 unk) = 0;
	virtual void	Unk_AB(void) = 0;
	virtual void	Unk_AC(void) = 0;
	virtual void	Unk_AD(void) = 0;
	virtual void	Unk_AE(void) = 0;
	virtual void	Unk_AF(void) = 0;
	virtual void	Unk_B0(void) = 0;	// B0
	virtual void	Unk_B1(void) = 0;
	virtual void	Unk_B2(void) = 0;
	virtual void	Unk_B3(void) = 0;
	virtual void	Unk_B4(void) = 0;
	virtual void	Unk_B5(void) = 0;
	virtual void	Unk_B6(void) = 0;
	virtual void	Unk_B7(void) = 0;	// AddSpell?
	virtual void	Unk_B8(void) = 0;	// RemoveSpell?
	virtual void	Unk_B9(void) = 0;
	virtual void	Unk_BA(void) = 0;
	virtual void	Unk_BB(void) = 0;
	virtual void	Unk_BC(void) = 0;
	virtual void	Unk_BD(void) = 0;
	virtual void	Unk_BE(void) = 0;
	virtual void	Unk_BF(void) = 0;
	virtual void	Unk_C0(void) = 0;	// C0
	virtual void	Unk_C1(void) = 0;
	virtual void	Unk_C2(void) = 0;
	virtual void	Unk_C3(void) = 0;
	virtual void	Unk_C4(void) = 0;
	virtual void	Unk_C5(void) = 0;
	virtual void	Unk_C6(void) = 0;
	virtual void	Unk_C7(void) = 0;
	virtual void	Unk_C8(void) = 0;
	virtual void	Unk_C9(void) = 0;
	virtual void	Unk_CA(void) = 0;
	virtual void	Unk_CB(void) = 0;
	virtual void	Unk_CC(void) = 0;	// get combat controller
	virtual bool	IsInCombat(bool unk) = 0;	// is in combat?
	virtual TESForm *	GetCombatTarget(void) = 0;
	virtual void	Unk_CF(void) = 0;
	virtual void	Unk_D0(void) = 0;	// D0
	virtual void	Unk_D1(void) = 0;
	virtual void	Unk_D2(void) = 0;
	virtual void	Unk_D3(void) = 0;
	virtual bool	IsYielding(void) = 0;
	virtual void	Unk_D5(void) = 0;
	virtual void	Unk_D6(void) = 0;
	virtual void	Unk_D7(void) = 0;
	virtual void	Unk_D8(void) = 0;
	virtual void	Unk_D9(void) = 0;
	virtual void	Unk_DA(void) = 0;
	virtual void	Unk_DB(void) = 0;
	virtual void	Unk_DC(void) = 0;
	virtual void	Unk_DD(void) = 0;
	virtual void	Unk_DE(void) = 0;
	virtual void	Unk_DF(void) = 0;
	virtual void	Unk_E0(void) = 0;	// E0
	virtual void	Unk_E1(void) = 0;
	virtual void	Unk_E2(void) = 0;
	virtual void	Unk_E3(void) = 0;
	virtual void	Unk_E4(void) = 0;
	virtual void	Unk_E5(void) = 0;
	virtual void	Unk_E6(void) = 0;
	virtual void	ModExperience(UInt32 actorValue, UInt32 scaleIndex, float baseDelta) = 0;
	virtual void	Unk_E8(void) = 0;
	virtual void	Unk_E9(void) = 0;
	virtual void	Unk_EA(void) = 0;
	virtual void	Unk_EB(UInt32 arg0, UInt32 arg1, TESObjectREFR * target) = 0;	// attack handling, 'this' is attacker
	virtual void	Unk_EC(void) = 0;
	virtual void	Unk_ED(void) = 0;
	virtual void	Unk_EE(void) = 0;

	void	EquipItem(TESObjectREFR * objType, UInt32 unk1, UInt32 unk2, UInt32 unk3, bool lockEquip);
	UInt32	GetBaseActorValue(UInt32 value);

	// 8
	struct Unk09C
	{
		// C+
		struct Data
		{
			UInt32		unk0;
			UInt32		unk4;
			SpellItem	* unk8;
		};

		Data	* data;
		Unk09C	* next;
	};

	// 8
	struct Unk0A4
	{
		// 8+
		struct Data
		{
			UInt32	unk0;
			Actor	* unk4;
		};

		Data	* data;
		Unk0A4	* next;
	};

	// bases
	MagicCaster		magicCaster;					// 05C
	MagicTarget		magicTarget;					// 068

	UInt32			unk070[(0x07C - 0x070) >> 2];	// 070
	Actor			* unk07C;						// 07C
	UInt32			unk080[(0x09C - 0x080) >> 2];	// 080
	Unk09C			unk09C;							// 09C
	Unk0A4			unk0A4;							// 0A4
	UInt32			unk0AC[(0x0CC - 0x0AC) >> 2];	// 0AC
	TESObjectREFR	* unk0CC;						// 0CC
	UInt32			unk0D0;							// 0D0
	Actor			* horseOrRider;					// 0D4 - For Character, currently ridden horse
														 //- For horse (Creature), currently riding Character	
	UInt32			unk0D8[(0x0E4 - 0x0D8) >> 2];	// 0D8
	Actor			* unk0E4;						// 0E4
	UInt32			unk0E8[(0x104 - 0x0E8) >> 2];	// 0E8
	// 104
};

// 104+
class Character : public Actor
{
public:
	Character();
	~Character();
};

// 800
class PlayerCharacter : public Character
{
public:
	enum
	{
		kMiscStat_DaysInPrison = 0,
		kMiscStat_DaysPassed,
		kMiscStat_SkillIncreases,
		kMiscStat_TrainingSessions,
		kMiscStat_LargestBounty,
		kMiscStat_CreaturesKilled,
		kMiscStat_PeopleKilled,
		kMiscStat_PlacesDiscovered,
		kMiscStat_LocksPicked,
		kMiscStat_LockpicksBroken,
		kMiscStat_SoulsTrapped,	// 10
		kMiscStat_IngredientsEaten,
		kMiscStat_PotionsMade,
		kMiscStat_OblivionGatesShut,
		kMiscStat_HorsesOwned,
		kMiscStat_HousesOwned,
		kMiscStat_StoresInvestedIn,
		kMiscStat_BooksRead,
		kMiscStat_SkillBooksRead,
		kMiscStat_ArtifactsFound,
		kMiscStat_HoursSlept,	// 20
		kMiscStat_HoursWaited,
		kMiscStat_DaysAsAVampire,
		kMiscStat_LastDayAsAVampire,
		kMiscStat_PeopleFedOn,
		kMiscStat_JokesTold,
		kMiscStat_DiseasesContracted,
		kMiscStat_NirnrootsFound,
		kMiscStat_ItemsStolen,
		kMiscStat_ItemsPickpocketed,
		kMiscStat_Trespasses,	// 30
		kMiscStat_Assaults,
		kMiscStat_Murders,
		kMiscStat_HorsesStolen,

		kMiscStat_Max			// 34
	};

	PlayerCharacter();
	~PlayerCharacter();

	// [ vtbl ]
	// +000 = PlayerCharacter
	// +018 = TESChildCell
	// +05C = MagicCaster
	// +068 = MagicTarget
	// +784 = NiTMapBase

	// [ objects ]
    // +01C TESNPC *
    // +03C BSFadeNode *
    // +040 TESObjectCELL *
    // +048 ExtraContainerChanges *
    // +058 HighProcess *
    // +1F4 hkAllCdPointCollector *
    // +570 TESObjectREFR *
    // +5E4 TESTopic *
    // +5F4 TESQuest *
    // +614    float amountFenced
    // +624 SpellItem *
    // +644 BirthSign *
    // +650 TESClass *
    // +6E8 TESRegion *
    // +700 TESObjectREFR *
    // +728 TESWorldSpace *
    // +740 TESWorldSpace *

	// [ data ]
	// +11C haggle amount?
	// +588 UInt8, bit 0x01 is true if we're in third person?
	// +590 UInt8, is time passing?
	// +5A9 UInt8, fast travel disabled
	// +658	UInt32, misc stat array
	// +70C	'initial state' buffer

	UInt32		unk104[(0x118 - 0x104) >> 2];	// 104
	DialoguePackage	* dialoguePackage;			// 118
	UInt32		unk11C[(0x130 - 0x11C) >> 2];	// 11C
	float		skillAdv[21];					// 130
	UInt32		unk184[(0x1E0 - 0x184) >> 2];	// 184
	Creature	* lastRiddenHorse;				// 1E0
	UInt32		unk1E4[(0x570 - 0x1E4) >> 2];	// 1E4
	TESObjectREFR	* lastActivatedLoadDoor;	// 570	-most recently activated load door
	UInt32		unk574[(0x588 - 0x574) >> 2];	// 574
	UInt8		isThirdPerson;					// 588
	UInt8		pad589[3];						// 589
	UInt32		unk58C[(0x610 - 0x58C) >> 2];	// 58C
	UInt8		unk610;							// 610
	UInt8		isAMurderer;					// 611
	UInt8		pad612[2];						// 612
	UInt32		unk614[(0x624 - 0x614) >> 2];	// 614
	MagicItem	* activeMagicItem;				// 624
	TESObjectBOOK	* book;						// 628
	UInt32		unk62C[(0x644 - 0x62C) >> 2];	// 62C
	BirthSign	* birthSign;					// 644
	UInt32		unk648[(0x650 - 0x648) >> 2];	// 648
	TESClass	* classForm;					// 650
	UInt32		unk654;							// 654
	UInt32		miscStats[kMiscStat_Max];		// 658
	AlchemyItem	* alchemyItem;					// 6E0
	UInt32		unk6E4[(0x800 - 0x6E4) >> 2];	// 6E4
	// 800

	bool	SetActiveSpell(MagicItem * item);
};

STATIC_ASSERT(sizeof(PlayerCharacter) == 0x800);
STATIC_ASSERT(offsetof(PlayerCharacter, isAMurderer) == 0x611);
STATIC_ASSERT(offsetof(PlayerCharacter, activeMagicItem) == 0x624);

class Creature : public Character
{
public:
	Creature();
	~Creature();
};

extern PlayerCharacter ** g_thePlayer;

// 104
class Sky
{
public:
	Sky();
	~Sky();

	virtual void	Destructor(void);
	// no more virtual functions

	static Sky *	GetSingleton(void);

	void	RefreshClimate(TESClimate * climate, UInt32 unk1);	// pass 1 for unk1 to pick new random weather etc

//	void		** _vtbl;						// 000
	NiNode*		niNode004;						// 004
	NiNode*		niNode008;						// 008
	TESClimate	* firstClimate;					// 00C
	TESWeather	* firstWeather;					// 010
	UInt32		unk014;							// 014
	TESWeather*	weather018;						// 018
	UInt32		unk01C;							// 01C
	Atmosphere* atmosphere;						// 020
	Stars*		stars;							// 024
	Sun*		sun;							// 028
	Clouds*		clouds;							// 02C
	Moon*		masserMoon;						// 030
	Moon*		secundaMoon;					// 034
	Precipitation* precipitation;				// 038
	UInt32		unk03C[(0x104 - 0x03C) >> 2];	// 03C
};

STATIC_ASSERT(sizeof(Sky) == 0x104);

enum
{
	kProjectileType_Arrow,
	kProjectileType_Ball,
	kProjectileType_Fog,
	kProjectileType_Bolt,
};							//arbitrary

struct MagicProjectileData
{
	float			speed;				// base speed * GMST fMagicProjectileBaseSpeed
	float			distanceTraveled;	// speed * elapsedTime while in flight
	float			elapsedTime;		// length of time projectile has existed
	MagicCaster		* caster;			// whoever/whatever cast the spell
										// For NonActorMagicCaster, != casting reference
	MagicItem		* magicItem;		//can always cast to SpellItem?
	UInt32			unk070;				
	EffectSetting	* effectSetting;		
};

//90
class MagicBallProjectile : public MobileObject
{
public:
	MagicBallProjectile();
	~MagicBallProjectile();

	MagicProjectileData	data;				//05C
	float				unk078;				//078
	UInt32				unk07C;				//07C
	UInt32				unk080;				//080 - looks like flags - (1 in flight, 2 hit target?)
	float				unk084;				//084 - value changes after projectile hits something 
	UInt32				unk088;				//088
	UInt32				unk08C;				//08C
};

//9C
class MagicFogProjectile : public MobileObject
{
public:
	MagicFogProjectile();
	~MagicFogProjectile();

	MagicProjectileData	data;				//05C
	float				unk078;				//078
	float				unk07C;				//07C
	float				unk080;				//080
	float				unk084;				//084
	UInt32				unk088;				//088 - looks like flags - (0 in flight, 1 hit target?)
	float				unk08C;				//08C
	UInt32				unk090;				//090 - pointer?
	UInt32				unk094;				//094
	UInt32				unk098;				//098 - pointer?
};

//A4
class MagicBoltProjectile : public MobileObject
{
public:
	MagicBoltProjectile();
	~MagicBoltProjectile();

	MagicProjectileData	data;				//05C
	float				unk078;				//078
	BoltShaderProperty	* boltShaderProperty;//07C
	UInt32				unk080;				//080
	UInt32				unk084;				//084
	NiNode				* niNode088;		//088
	UInt32				unk08C;				//08C
	UInt32				unk090;				//090
	NiNode				* niNode094;		//094
	UInt32				unk098;				//098
	UInt32				unk09C;				//09C - pointer?
	UInt32				unk0A0;				//0A0
};

//9C
class ArrowProjectile : public MobileObject
{
public:
	ArrowProjectile();
	~ArrowProjectile();

	UInt32			unk05C;			//05C
	UInt32			unk060;			//060
	float			unk064;			//064
	float			unk068;			//068
	float			speed;			//06C - base speed * GMST fArrowSpeedMult
	float			unk070;			//070
	float			unk074;			//074
	Actor			* shooter;		//078;
	EnchantmentItem	* arrowEnch;	//07C
	EnchantmentItem	* bowEnch;		//080
	AlchemyItem		* poison;		//084
	float			unk088;			//088
	float			unk08C;			//08C
	float			unk090;			//090
	UInt32			unk094;			//094
	UInt32			unk098;			//098
};
