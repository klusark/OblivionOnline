#pragma once

#include "GameTypes.h"

/*** class hierarchy
 *	
 *	yet again taken from rtti information
 *	ni doesn't seem to use multiple inheritance
 *	
 *	NiObject derives from NiRefObject, an Oblivion class
 *	
 *	BSFaceGenMorphData - derived from NiRefObject
 *		BSFaceGenMorphDataHead
 *		BSFaceGenMorphDataHair
 *	
 *	BSTempEffect - derived from NiObject
 *		BSTempEffectDecal
 *		BSTempEffectGeometryDecal
 *		BSTempEffectParticle
 *		MagicHitEffect
 *			MagicModelHitEffect
 *			MagicShaderHitEffect
 *	
 *	NiDX92DBufferData - derived from NiRefObject and something else
 *		NiDX9DepthStencilBufferData
 *			NiDX9SwapChainDepthStencilBufferData
 *			NiDX9ImplicitDepthStencilBufferData
 *			NiDX9AdditionalDepthStencilBufferData
 *		NiDX9TextureBufferData
 *		NiDX9OnscreenBufferData
 *			NiDX9SwapChainBufferData
 *			NiDX9ImplicitBufferData
 *	
 *	NiObject
 *		NiObjectNET
 *			NiProperty
 *				NiTexturingProperty
 *				NiVertexColorProperty
 *				NiWireframeProperty
 *				NiZBufferProperty
 *				NiMaterialProperty
 *				NiAlphaProperty
 *				NiStencilProperty
 *				NiRendererSpecificProperty
 *				NiShadeProperty
 *					BSShaderProperty
 *						SkyShaderProperty
 *						ParticleShaderProperty
 *						BSShaderLightingProperty
 *							DistantLODShaderProperty
 *							TallGrassShaderProperty
 *							BSShaderPPLightingProperty
 *								SpeedTreeShaderPPLightingProperty
 *									SpeedTreeBranchShaderProperty
 *								Lighting30ShaderProperty
 *								HairShaderProperty
 *							SpeedTreeShaderLightingProperty
 *								SpeedTreeLeafShaderProperty
 *								SpeedTreeFrondShaderProperty
 *							GeometryDecalShaderProperty
 *						PrecipitationShaderProperty
 *						BoltShaderProperty
 *						WaterShaderProperty
 *				NiSpecularProperty
 *				NiFogProperty
 *					BSFogProperty
 *				NiDitherProperty
 *			NiTexture
 *				NiDX9Direct3DTexture
 *				NiSourceTexture
 *					NiSourceCubeMap
 *				NiRenderedTexture
 *					NiRenderedCubeMap
 *			NiAVObject
 *				NiDynamicEffect
 *					NiLight
 *						NiDirectionalLight
 *						NiPointLight
 *							NiSpotLight
 *						NiAmbientLight
 *					NiTextureEffect
 *				NiNode
 *					SceneGraph
 *					BSTempNodeManager
 *					BSTempNode
 *					BSCellNode
 *					BSClearZNode
 *					BSFadeNode
 *					BSScissorNode
 *					BSTimingNode
 *					BSFaceGenNiNode
 *					NiBillboardNode
 *					NiSwitchNode
 *						NiLODNode
 *							NiBSLODNode
 *					NiSortAdjustNode
 *					NiBSPNode
 *					ShadowSceneNode
 *					BSCubeMapCamera
 *				NiCamera
 *					NiScreenSpaceCamera
 *				NiGeometry
 *					NiLines
 *					NiTriBasedGeom
 *						NiTriShape
 *							BSScissorTriShape
 *							NiScreenElements
 *							NiScreenGeometry
 *							TallGrassTriShape
 *						NiTriStrips
 *							TallGrassTriStrips
 *					NiParticles
 *						NiParticleSystem
 *							NiMeshParticleSystem
 *						NiParticleMeshes
 *			NiSequenceStreamHelper
 *		NiRenderer
 *			NiDX9Renderer
 *		NiPixelData
 *		NiCollisionObject
 *			NiCollisionData
 *			bhkNiCollisionObject
 *				bhkCollisionObject
 *					bhkBlendCollisionObject
 *						WeaponObject
 *						bhkBlendCollisionObjectAddRotation
 *				bhkPCollisionObject
 *					bhkSPCollisionObject
 *		NiControllerSequence
 *			BSAnimGroupSequence
 *		NiTimeController
 *			BSDoorHavokController
 *			BSPlayerDistanceCheckController
 *			NiD3DController
 *			NiControllerManager
 *			NiInterpController
 *				NiSingleInterpController
 *					NiTransformController
 *					NiPSysModifierCtlr
 *						NiPSysEmitterCtlr
 *						NiPSysModifierBoolCtlr
 *							NiPSysModifierActiveCtlr
 *						NiPSysModifierFloatCtlr
 *							NiPSysInitialRotSpeedVarCtlr
 *							NiPSysInitialRotSpeedCtlr
 *							NiPSysInitialRotAngleVarCtlr
 *							NiPSysInitialRotAngleCtlr
 *							NiPSysGravityStrengthCtlr
 *							NiPSysFieldMaxDistanceCtlr
 *							NiPSysFieldMagnitudeCtlr
 *							NiPSysFieldAttenuationCtlr
 *							NiPSysEmitterSpeedCtlr
 *							NiPSysEmitterPlanarAngleVarCtlr
 *							NiPSysEmitterPlanarAngleCtlr
 *							NiPSysEmitterLifeSpanCtlr
 *							NiPSysEmitterInitialRadiusCtlr
 *							NiPSysEmitterDeclinationVarCtlr
 *							NiPSysEmitterDeclinationCtlr
 *							NiPSysAirFieldSpreadCtlr
 *							NiPSysAirFieldInheritVelocityCtlr
 *							NiPSysAirFieldAirFrictionCtlr
 *					NiFloatInterpController
 *						NiFlipController
 *						NiAlphaController
 *						NiTextureTransformController
 *						NiLightDimmerController
 *					NiBoolInterpController
 *						NiVisController
 *					NiPoint3InterpController
 *						NiMaterialColorController
 *						NiLightColorController
 *					NiExtraDataController
 *						NiFloatsExtraDataPoint3Controller
 *						NiFloatsExtraDataController
 *						NiFloatExtraDataController
 *						NiColorExtraDataController
 *				NiMultiTargetTransformController
 *				NiGeomMorpherController
 *			bhkBlendController
 *			bhkForceController
 *			NiBSBoneLODController
 *			NiUVController
 *			NiPathController
 *			NiLookAtController
 *			NiKeyframeManager
 *			NiBoneLODController
 *			NiPSysUpdateCtlr
 *			NiPSysResetOnLoopCtlr
 *			NiFloatController
 *				NiRollController
 *		bhkRefObject
 *			bhkWorld
 *				bhkWorldM
 *			bhkSerializable
 *				bhkAction
 *					bhkUnaryAction
 *						bhkMouseSpringAction
 *						bhkMotorAction
 *					bhkBinaryAction
 *						bhkSpringAction
 *						bhkAngularDashpotAction
 *						bhkDashpotAction
 *				bhkWorldObject
 *					bhkPhantom
 *						bhkShapePhantom
 *							bhkSimpleShapePhantom
 *							bhkCachingShapePhantom
 *						bhkAabbPhantom
 *							bhkAvoidBox
 *					bhkEntity
 *						bhkRigidBody
 *							bhkRigidBodyT
 *				bhkConstraint
 *					bhkLimitedHingeConstraint
 *					bhkMalleableConstraint
 *					bhkBreakableConstraint
 *					bhkWheelConstraint
 *					bhkStiffSpringConstraint
 *					bhkRagdollConstraint
 *					bhkPrismaticConstraint
 *					bhkHingeConstraint
 *					bhkBallAndSocketConstraint
 *					bhkGenericConstraint
 *						bhkFixedConstraint
 *					bhkPointToPathConstraint
 *					bhkPoweredHingeConstraint
 *				bhkShape
 *					bhkTransformShape
 *					bhkSphereRepShape
 *						bhkConvexShape
 *							bhkSphereShape
 *							bhkCapsuleShape
 *							bhkBoxShape
 *							bhkTriangleShape
 *							bhkCylinderShape
 *							bhkConvexVerticesShape
 *								bhkCharControllerShape
 *							bhkConvexTransformShape
 *							bhkConvexSweepShape
 *						bhkMultiSphereShape
 *					bhkBvTreeShape
 *						bhkTriSampledHeightFieldBvTreeShape
 *						bhkMoppBvTreeShape
 *					bhkShapeCollection
 *						bhkListShape
 *						bhkPackedNiTriStripsShape
 *						bhkNiTriStripsShape
 *					bhkHeightFieldShape
 *						bhkPlaneShape
 *				bhkCharacterProxy
 *		NiExtraData
 *			TESObjectExtraData
 *			BSFaceGenAnimationData
 *			BSFaceGenModelExtraData
 *			BSFaceGenBaseMorphExtraData
 *			DebugTextExtraData
 *			NiStringExtraData
 *			NiFloatExtraData
 *				FadeNodeMaxAlphaExtraData
 *			BSFurnitureMarker
 *			NiBinaryExtraData
 *			BSBound
 *			NiSCMExtraData
 *			NiTextKeyExtraData
 *			NiVertWeightsExtraData
 *			bhkExtraData
 *			PArrayPoint
 *			NiIntegerExtraData
 *				BSXFlags
 *			NiFloatsExtraData
 *			NiColorExtraData
 *			NiVectorExtraData
 *			NiSwitchStringExtraData
 *			NiStringsExtraData
 *			NiIntegersExtraData
 *			NiBooleanExtraData
 *		NiAdditionalGeometryData
 *			BSPackedAdditionalGeometryData
 *		NiGeometryData
 *			NiLinesData
 *			NiTriBasedGeomData
 *				NiTriStripsData
 *					NiTriStripsDynamicData
 *				NiTriShapeData
 *					NiScreenElementsData
 *					NiTriShapeDynamicData
 *					NiScreenGeometryData
 *			NiParticlesData
 *				NiPSysData
 *					NiMeshPSysData
 *				NiParticleMeshesData
 *		NiTask
 *			BSTECreateTask
 *			NiParallelUpdateTaskManager::SignalTask
 *			NiGeomMorpherUpdateTask
 *			NiPSysUpdateTask
 *		NiSkinInstance
 *		NiSkinPartition
 *		NiSkinData
 *		NiRenderTargetGroup
 *		Ni2DBuffer
 *			NiDepthStencilBuffer
 *		NiUVData
 *		NiStringPalette
 *		NiSequence
 *		NiRotData
 *		NiPosData
 *		NiMorphData
 *		NiTransformData
 *		NiFloatData
 *		NiColorData
 *		NiBSplineData
 *		NiBSplineBasisData
 *		NiBoolData
 *		NiTaskManager
 *			NiParallelUpdateTaskManager
 *		hkPackedNiTriStripsData
 *		NiInterpolator
 *			NiBlendInterpolator
 *				NiBlendTransformInterpolator
 *				NiBlendAccumTransformInterpolator
 *				NiBlendFloatInterpolator
 *				NiBlendQuaternionInterpolator
 *				NiBlendPoint3Interpolator
 *				NiBlendColorInterpolator
 *				NiBlendBoolInterpolator
 *			NiLookAtInterpolator
 *			NiKeyBasedInterpolator
 *				NiFloatInterpolator
 *				NiTransformInterpolator
 *				NiQuaternionInterpolator
 *				NiPoint3Interpolator
 *				NiPathInterpolator
 *				NiColorInterpolator
 *				NiBoolInterpolator
 *					NiBoolTimelineInterpolator
 *			NiBSplineInterpolator
 *				NiBSplineTransformInterpolator
 *					NiBSplineCompTransformInterpolator
 *				NiBSplinePoint3Interpolator
 *					NiBSplineCompPoint3Interpolator
 *				NiBSplineFloatInterpolator
 *					NiBSplineCompFloatInterpolator
 *				NiBSplineColorInterpolator
 *					NiBSplineCompColorInterpolator
 *		NiAVObjectPalette
 *			NiDefaultAVObjectPalette
 *		BSReference
 *		BSNodeReferences
 *		NiPalette
 *		NiLODData
 *			NiRangeLODData
 *			NiScreenLODData
 *		NiPSysModifier
 *			BSWindModifier
 *			NiPSysMeshUpdateModifier
 *			NiPSysRotationModifier
 *			NiPSysEmitter
 *				NiPSysMeshEmitter
 *				NiPSysVolumeEmitter
 *					NiPSysCylinderEmitter
 *					NiPSysSphereEmitter
 *					NiPSysBoxEmitter
 *					BSPSysArrayEmitter
 *			NiPSysGravityModifier
 *			NiPSysSpawnModifier
 *			BSParentVelocityModifier
 *			NiPSysPositionModifier
 *			NiPSysGrowFadeModifier
 *			NiPSysDragModifier
 *			NiPSysColorModifier
 *			NiPSysColliderManager
 *			NiPSysBoundUpdateModifier
 *			NiPSysBombModifier
 *			NiPSysAgeDeathModifier
 *			NiPSysFieldModifier
 *				NiPSysVortexFieldModifier
 *				NiPSysTurbulenceFieldModifier
 *				NiPSysRadialFieldModifier
 *				NiPSysGravityFieldModifier
 *				NiPSysDragFieldModifier
 *				NiPSysAirFieldModifier
 *		NiPSysEmitterCtlrData
 *		NiAccumulator
 *			NiBackToFrontAccumulator
 *				NiAlphaAccumulator
 *					BSShaderAccumulator
 *		NiScreenPolygon
 *		NiScreenTexture
 *		NiPSysCollider
 *			NiPSysSphericalCollider
 *			NiPSysPlanarCollider
 *	
 *	NiShader
 *		NiD3DShaderInterface
 *			NiD3DShader
 *				NiD3DDefaultShader
 *					SkyShader
 *					ShadowLightShader
 *						ParallaxShader
 *						SkinShader
 *						HairShader
 *						SpeedTreeBranchShader
 *					WaterShaderHeightMap
 *					WaterShader
 *					WaterShaderDisplacement
 *					ParticleShader
 *					TallGrassShader
 *					PrecipitationShader
 *					SpeedTreeLeafShader
 *					BoltShader
 *					Lighting30Shader
 *					GeometryDecalShader
 *					SpeedTreeFrondShader
 *					DistantLODShader
 *	
 *	NiD3DShaderConstantMap
 *		NiD3DSCM_Vertex
 *		NiD3DSCM_Pixel
 *	
 ****/

class NiAVObject;
class BSFadeNode;

// 8
struct NiRTTI
{
	const char	* name;
	NiRTTI		* parent;
};

// C
struct NiVector3
{
	float	x, y, z;
};

// 24
struct NiMatrix33
{
	float	data[9];
};

// 10
struct NiSphere
{
	float	x, y, z, radius;
};

// 1C
struct NiFrustum
{
	float	l;
	float	r;
	float	t;
	float	b;
	float	n;
	float	f;
	UInt8	o;
};

// 10
struct NiViewport
{
	float	l;
	float	r;
	float	t;
	float	b;
};

// 10
template <typename T>
struct NiTArray
{
	void	* _vtbl;	// 00
	T		* data;		// 04
	UInt16	unk08;		// 08 - init'd to size of preallocation
	UInt16	length;		// 0A - init'd to 0
	UInt16	unk0C;		// 0C - init'd to 0
	UInt16	unk0E;		// 0E - init'd to size of preallocation
};

// 8+
class NiObject
{
public:
	NiObject();
	~NiObject();

	virtual void		Destructor(void);	// 00
	virtual NiRTTI *	GetType(void);
	virtual void		Unk_02(void);
	virtual void		Unk_03(void);
	virtual void		Unk_04(void);
	virtual void		Unk_05(void);
	virtual void		Unk_06(void);
	virtual void		Unk_07(void);
	virtual void		Unk_08(void);
	virtual void		Unk_09(void);
	virtual void		Unk_0A(void);
	virtual void		Unk_0B(void);
	virtual void		DumpAttributes(NiTArray <char *> * dst);
	virtual void		Unk_0D(void);
	virtual void		Unk_0E(void);
	virtual void		Unk_0F(void);
	virtual void		Unk_10(void);		// 10
	virtual void		Unk_11(void);
	virtual void		Unk_12(void);

//	void		** _vtbl;		// 000
	UInt32		m_uiRefCount;	// 004
};

//10
class NiBaseObject
{
public:
	NiBaseObject();
	~NiBaseObject();

	virtual void		Unk_00(void);

	//void**		vtbl;		//000
	UInt16			unk004;		//004
	UInt16			unk006;		//006 refcount?
	UInt32			unk008;		//008 init to c'tr arg0
	UInt32			unk00C;		//00C init to c'tr arg4
};

// 100+
class NiObjectNET : public NiObject
{
public:
	NiObjectNET();
	~NiObjectNET();

	const char			* m_pcName;						// 008
	UInt32				unk00C;							// 00C
	UInt32				unk010;							// 010
	UInt16				unk014;							// 014
	UInt16				unk016;							// 016
	UInt32				unk018[(0x080 - 0x018) >> 2];	// 018
	CRITICAL_SECTION	critSection;					// 080
	UInt32				unk098[(0x0AC - 0x098) >> 2];	// 098
	NiTArray <NiAVObject>	unk0AC;						// 0AC
	UInt32				unk0BC[(0x0F8 - 0x0BC) >> 2];	// 0BC
	UInt32				unk0F8;							// 0F8
	UInt32				unk0FC;							// 0FC
};

// 194+
class NiAVObject : public NiObjectNET
{
public:
	NiAVObject();
	~NiAVObject();
	
	virtual void		Unk_12(void);
	virtual void		Unk_13(void);
	virtual void		Unk_14(void);
	virtual void		Unk_15(void);
	virtual void		Unk_16(void);
	virtual void		Unk_17(void);
	virtual void		Unk_18(void);
	virtual void		Unk_19(void);
	virtual void		Unk_1A(void);
	virtual void		Unk_1B(void);
	virtual void		Unk_1C(void);
	virtual void		Unk_1D(void);
	virtual void		Unk_1E(void);
	virtual void		Unk_1F(void);
	virtual void		Unk_20(void);

	enum
	{
		kFlag_AppCulled =					1 << 0,	// originally named m_bAppCulled but they appear to have used bitfields
		kFlag_SelUpdate =					1 << 1,
		kFlag_SelUpdateTransforms =			1 << 2,
		kFlag_SelUpdatePropControllers =	1 << 3,
		kFlag_SelUpdateRigid =				1 << 4,
	};

	UInt8		m_flags;				// 100
	UInt8		pad101;					// 101
	UInt8		pad102;					// 102
	UInt8		pad103;					// 103
	UInt32		unk104;					// 104
	NiSphere	m_kWorldBound;			// 108
	NiMatrix33	m_localRotate;			// 118
	NiVector3	m_localTranslate;		// 13C
	float		m_fLocalScale;			// 148
	NiMatrix33	m_worldRotate;			// 14C
	NiVector3	m_worldTranslate;		// 170
	float		m_worldScale;			// 17C
	UInt32		unk180;					// 180 - vtable

	// C+
	struct Unk184
	{
		Unk184		* next;	// 000
		UInt32		unk004;	// 004
		NiObject	* obj;	// 008
	};

	Unk184		* unk184;				// 184 - property list?
	UInt32		unk188;					// 188
	UInt32		unk18C;					// 18C - possibly number of links in Unk184?
	void		* m_spCollisionObject;	// 190
};

// 220+
// 280? see 005743F1
class NiNode : public NiAVObject
{
public:
	NiNode();
	~NiNode();

	virtual void		Unk_21(void);
	virtual void		Unk_22(void);
	virtual void		Unk_23(void);
	virtual void		Unk_24(void);
	virtual void		Unk_25(void);
	virtual void		Unk_26(void);

	UInt8					unk194[0x6C];	// 194
	NiTArray <NiObject *>	children;		// 200
	void					* unk210;		// 210 - vtbl
	UInt32					unk214;			// 214
	UInt32					unk218;			// 218
	UInt32					unk21C;			// 21C
};

class NiCamera : public NiAVObject
{
public:
	NiCamera();
	~NiCamera();

	// ### needs padding

	NiFrustum	m_kViewFrustum;			// 240
	UInt8		unk254[8];				// 254
	float		m_fMinNearPlaneDist;	// 25C
	float		m_fMaxFarNearRatio;		// 260
	NiViewport	m_kPort;				// 264
};

class NiControllerManager;
class NiStringPalette;
class NiTextKeyExtraData;

//068
class NiControllerSequence : public NiObject
{
public:
	NiControllerSequence();
	~NiControllerSequence();

	char				* filePath;		//008
	UInt16				unk00C;			//00C
	UInt16				unk00E;			//00E
	UInt16				unk010;			//010
	UInt16				unk012;			//012
	void				* unk014;		//014
	void				* unk018;		//018
	float				unk01C;			//01C
	NiTextKeyExtraData	* unk020;		//020
	UInt32				unk024;			//024
	float				unk028;			//028
	UInt32				unk02C[5];		//02C
	NiControllerManager * controllerManager;	//040
	UInt32				unk044;			//044
	float				unk048;			//048
	float				unk04C;			//04C - unk048 * -1?
	float				unk050;			//050
	UInt32				unk054;			//054
	UInt32				unk058;			//058
	void				* unk05C;		//05C - bone? (seen "Bip01")
	NiNode				* niNode060;	//060
	NiStringPalette		* unk064;		//064
};

//02C+
class TESAnimGroup
{
public:
	//derived from NiRefObject
	TESAnimGroup();
	~TESAnimGroup();

	virtual void Destructor(bool arg0);

	enum{
		kAnimGroup_Idle 		=	0,
		kAnimGroup_DynamicIdle,
		kAnimGroup_SpecialIdle,
		kAnimGroup_Forward,	
		kAnimGroup_Backward,
		kAnimGroup_Left,
		kAnimGroup_Right,
		kAnimGroup_FastForward,
		kAnimGroup_FastBackward,
		kAnimGroup_FastLeft,
		kAnimGroup_FastRight,
		kAnimGroup_DodgeForward,
		kAnimGroup_DodgeBack,
		kAnimGroup_DodgeLeft,
		kAnimGroup_DodgeRight,
		kAnimGroup_TurnLeft,
		kAnimGroup_TurnRight,
		kAnimGroup_Equip,
		kAnimGroup_Unequip,
		kAnimGroup_AttackBow,
		kAnimGroup_AttackLeft,
		kAnimGroup_AttackRight,
		kAnimGroup_AttackPower, 
		kAnimGroup_AttackForwardPower,
		kAnimGroup_AttackBackPower,
		kAnimGroup_AttackLeftPower,
		kAnimGroup_AttackRightPower,
		kAnimGroup_BlockIdle,
		kAnimGroup_BlockHit,
		kAnimGroup_BlockAttack,
		kAnimGroup_Recoil,
		kAnimGroup_Stagger,
		kAnimGroup_Death,
		kAnimGroup_TorchIdle,
		kAnimGroup_CastSelf,
		kAnimGroup_CastTouch,
		kAnimGroup_CastTarget,
		kAnimGroup_CastSelfAlt,
		kAnimGroup_CastTouchAlt,
		kAnimGroup_CastTargetAlt,
		kAnimGroup_JumpStart,
		kAnimGroup_JumpLoop,
		kAnimGroup_JumpLand,

		kAnimGroup_Max,
	};

	//void**	vtbl		//000
	UInt8		unk004;		//004
	UInt8		unk005[3];
	UInt8		animGroup;	//008 init'ed to word arg in c'tor
	UInt8		unk009;		//009 does what?
	UInt16		unk00A;
	UInt32		numFrames;	//00C count of group frames (Start, Detach, Attack, End, etc)
	float		** frameData;//010 pointer to float array of group frame times (size numFrames)
	UInt32		unk014;		//014
	UInt32		unk018;		//018
	UInt32		unk01C;		//01C
	UInt8		unk020;		//020
	UInt8		unk021;
	UInt8		pad022[2];
	UInt32		unk024;		//024
	void		* unk028;	//028
};

//06C
class BSAnimGroupSequence : public NiControllerSequence
{
public:
	BSAnimGroupSequence();
	~BSAnimGroupSequence();

	virtual void Destructor(bool arg0);
	virtual NiRTTI* GetType(void);
	virtual void Unk_02(void);
	virtual void Unk_03(void);
	virtual void Unk_04(void);
	virtual void Unk_05(void);
	virtual void Unk_06(void); //allocates space for a NiControllerSequence
	virtual void Unk_07(UInt32 arg0);
	virtual void Unk_08(UInt32 arg0);
	virtual bool Unk_09(UInt32 arg0);
	virtual void Unk_0A(void);
	virtual bool Unk_0B(UInt32 arg0);
	virtual void Unk_0C(UInt32 arg0);	//prints AccumRoot from pointer at +05C
	virtual void Unk_0D(void);
	virtual void Unk_0E(void);
	virtual void Unk_0F(void);
	virtual void Unk_10(void);
	virtual void Unk_11(void);
	virtual void Unk_12(void);

	TESAnimGroup		* animGroup;	//068;
};

//03C
class NiTimeController : public NiObject
{
public:
	NiTimeController();
	~NiTimeController();

	virtual void		Unk_13(void);
	virtual void		Unk_14(void);
	virtual void		Unk_15(void);
	virtual void		Unk_16(void);
	virtual void		Unk_17(void);
	virtual void		Unk_18(void);
	virtual void		Unk_19(void);
	virtual void		Unk_1A(void);
	virtual void		Unk_1B(void);
	virtual void		Unk_1C(void);

	UInt32				unk008;		//008
	float				unk00C;		//00C
	UInt32				unk010;		//010
	UInt32				unk014;		//014
	UInt32				unk018;		//018
	UInt32				unk01C;		//01C
	UInt32				unk020;		//020
	UInt32				unk024;		//024
	UInt32				unk028;		//028
	UInt32				unk02C;		//02C
	NiNode				* unk030;	//030 seen BSFadeNode
	NiObject			* unk034;	//034 seen NiMultiTargetTransformController
	UInt32				unk038;		//038
};

//080
class NiControllerManager : public NiTimeController
{
public:
	NiControllerManager();
	~NiControllerManager();

	virtual void Unk_1D(void);

	NiTArray <NiControllerSequence*>		array03C;	//03C
	NiControllerSequence					** unk04C;	//04C appears to be array of pointers to dynamic idles
	UInt32									unk050;		//050
	UInt32									unk054;		//054
	NiTPointerMap <NiControllerSequence>	map058;		//058 NiTStringPointerMap
	UInt16									unk06C;		//06C
	UInt16									pad06E;		
	UInt16									unk070;		//070
	UInt16									unk072;
	UInt32									unk074;		//074
	UInt32									unk078;		//078
	UInt32									unk07C;		//07C
};

class ShadowSceneNode : public NiNode
{
public:
	ShadowSceneNode();
	~ShadowSceneNode();

	// ### needs padding
	// 28C - linked list of lights?
	UInt32	m_lightCount;	// 294
};

// derived from NiFile, which derives from NiBinaryStream
// 154
class BSFile
{
public:
	BSFile();
	~BSFile();

	virtual void	Destructor(bool freeMemory);				// 00
	virtual void	Unk_01(void);								// 04
	virtual void	Unk_02(void);								// 08
	virtual void	Unk_03(void);								// 0C
	virtual void	Unk_04(void);								// 10
	virtual void	DumpAttributes(NiTArray <char *> * dst);	// 14
	virtual UInt32	GetSize(void);								// 18
	virtual void	Unk_07(void);								// 1C
	virtual void	Unk_08(void);								// 20
	virtual void	Unk_09(void);								// 24
	virtual void	Unk_0A(void);								// 28
	virtual void	Unk_0B(void);								// 2C
	virtual void	Unk_0C(void);								// 30
	virtual void	Unk_Read(void);								// 34
	virtual void	Unk_Write(void);							// 38

//	void	** m_vtbl;		// 000
	void	* m_readProc;	// 004 - function pointer
	void	* m_writeProc;	// 008 - function pointer
	UInt32	m_bufSize;		// 00C
	UInt32	m_unk010;		// 010 - init'd to m_bufSize
	UInt32	m_unk014;		// 014
	void	* m_buf;		// 018
	FILE	* m_file;		// 01C
	UInt32	m_writeAccess;	// 020
	UInt8	m_good;			// 024
	UInt8	m_pad025[3];	// 025
	UInt8	m_unk028;		// 028
	UInt8	m_pad029[3];	// 029
	UInt32	m_unk02C;		// 02C
	UInt32	m_pos;			// 030
	UInt32	m_unk034;		// 034
	UInt32	m_unk038;		// 038
	char	m_path[0x104];	// 03C
	UInt32	m_unk140;		// 140
	UInt32	m_unk144;		// 144
	UInt32	m_pos2;			// 148 - used if m_pos is 0xFFFFFFFF
	UInt32	m_unk14C;		// 14C
	UInt32	m_fileSize;		// 150
};

class NiCamera;
class NiCullingProcess;

//F0
class SceneGraph
{
public:
	SceneGraph();
	~SceneGraph();

	struct Unk030 {
		float		unk00;
		UInt32		unk04;
		UInt32		unk08;
		UInt32		unk0C;
	};

	void ** vtbl;								//000
	UInt32				unk004[7];				//004 .. 01C
	float				unk020;					//020
	float				unk024;					//024
	UInt32				unk028;					//028
	UInt32				unk02C;					//02C
	Unk030				unk030[3];				//030 .. 05C
	float				unk060;					//060
	Unk030				unk064[3];				//064 .. 090
	float				unk094;					//094
	NiTPointerList<UInt32>	pointerList098;		//098 - NiProperty
	UInt32				unk0A8;					//0A8
	NiTArray<UInt32>	tArray;					//0AC - NiAVObject
	NiTPointerList<UInt32>	pointerList0BC;		//0BC - DynamicEffect
	UInt32				unk0CC[4];				//0CC .. D8
	NiCamera			* camera;				//0DC
	UInt32				unk0E0;					//0E0
	NiCullingProcess	* cullingProcess;		//0E4
	UInt32				unk0E8;					//0E8
	float				unk0EC;					//0EC
};

STATIC_ASSERT(sizeof(SceneGraph) == 0x0F0);