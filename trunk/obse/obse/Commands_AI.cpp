#include "Commands_AI.h"
#include "GameObjects.h"
#include "GameExtraData.h"
#include "GameAPI.h"
#include "GameForms.h"
#include "GameProcess.h"
#include "ParamInfos.h"

#if OBLIVION

#include "InternalSerialization.h"

static bool Cmd_GetNumFollowers_Execute(COMMAND_ARGS)
{
	*result = 0;

	if (!thisObj)
		return true;

	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_Follower);
	if (!xData)
		return true;
	
	ExtraFollower* xFollower = (ExtraFollower*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraFollower, 0);
	if (xFollower)
		*result = ExtraFollowerVisitor(xFollower->followers).Count();

	return true;
}

static bool Cmd_GetNthFollower_Execute(COMMAND_ARGS)
{
	UInt32 idx = 0;
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &idx))
		return true;

	else if (!thisObj)
		return true;

	BSExtraData* xData = thisObj->baseExtraList.GetByType(kExtraData_Follower);
	if (!xData)
		return true;
	
	ExtraFollower* xFollowers = (ExtraFollower*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraFollower, 0);
	if (xFollowers)
	{
		Character* follower = ExtraFollowerVisitor(xFollowers->followers).GetNthInfo(idx);
		if (follower)
			*refResult = follower->refID;
	}

	return true;
}

static bool Cmd_GetNumDetectedActors_Execute(COMMAND_ARGS)
{
	*result = 0;
	HighProcess* hiProc = NULL;
	Actor* actor = (Actor*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_Actor, 0);
	if (actor)
		hiProc = (HighProcess*)Oblivion_DynamicCast(actor->process, 0, RTTI_BaseProcess, RTTI_HighProcess, 0);

	if (hiProc && hiProc->detectionList)
		*result = HighProcess::DetectionListVisitor(hiProc->detectionList).Count();

	if (IsConsoleMode())
		Console_Print("Num Detected Actors: %.0f", *result);

	return true;
}

static bool Cmd_GetNthDetectedActor_Execute(COMMAND_ARGS)
{
	UInt32 whichN = 0;
	HighProcess* hiProc = NULL;
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &whichN))
		return true;

	Actor* actor = (Actor*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_Actor, 0);
	if (actor)
		hiProc = (HighProcess*)Oblivion_DynamicCast(actor->process, 0, RTTI_BaseProcess, RTTI_HighProcess, 0);

	if (hiProc && hiProc->detectionList)
	{
		HighProcess::DetectionList::Data* data = HighProcess::DetectionListVisitor(hiProc->detectionList).GetNthInfo(whichN);
		if (data && data->actor)
		{
			*refResult = data->actor->refID;
			if (IsConsoleMode())
				PrintItemType(data->actor->baseForm);
		}
	}

	return true;
}

class DetectedActorFinder
{
public:
	Actor* m_actorToFind;
	DetectedActorFinder(Actor* actor) : m_actorToFind(actor)
		{ }
	bool Accept(HighProcess::DetectionList::Data* data)
	{
		if (data->actor && data->actor->refID == m_actorToFind->refID)
			return true;
	
		return false;
	}
};

static bool Cmd_SetDetectionState_Execute(COMMAND_ARGS)
{
	HighProcess* hiProc = NULL;
	Actor* actor = NULL;
	UInt32 detectLevel = 0;
	*result = 0;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &actor, &detectLevel))
		return true;

	Actor* callingActor = (Actor*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_Actor, 0);
	if (callingActor)
		hiProc = (HighProcess*)Oblivion_DynamicCast(callingActor->process, 0, RTTI_BaseProcess, RTTI_HighProcess, 0);

	if (hiProc && hiProc->detectionList && detectLevel < HighProcess::kDetectionState_Max)
	{
		const HighProcess::DetectionList* dList = HighProcess::DetectionListVisitor(hiProc->detectionList).Find(DetectedActorFinder(actor));
		if (dList)
			dList->data->detectionState = detectLevel;
	}

	return true;
}

static bool GetServiceFlag_Execute(COMMAND_ARGS, UInt32 whichService)
{
	TESNPC* npc = NULL;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &npc))
		return false;
	if (!npc && thisObj)
		npc = (TESNPC*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);

	if (npc)
		return npc->aiForm.OffersServices(whichService);
	else
		return false;
}

static bool Cmd_OffersWeapons_Execute(COMMAND_ARGS)
{
	*result = GetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Weapons) ? 1 : 0;
	return true;
}

static bool Cmd_OffersArmor_Execute(COMMAND_ARGS)
{
	*result = GetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Armor) ? 1 : 0;
	return true;
}

static bool Cmd_OffersBooks_Execute(COMMAND_ARGS)
{
	*result = GetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Books) ? 1 : 0;
	return true;
}

static bool Cmd_OffersClothing_Execute(COMMAND_ARGS)
{
	*result = GetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Clothing) ? 1 : 0;
	return true;
}

static bool Cmd_OffersIngredients_Execute(COMMAND_ARGS)
{
	*result = GetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Ingredients) ? 1 : 0;
	return true;
}

static bool Cmd_OffersLights_Execute(COMMAND_ARGS)
{
	*result = GetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Lights) ? 1 : 0;
	return true;
}

static bool Cmd_OffersApparatus_Execute(COMMAND_ARGS)
{
	*result = GetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Apparatus) ? 1 : 0;
	return true;
}

static bool Cmd_OffersMiscItems_Execute(COMMAND_ARGS)
{
	*result = GetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Misc) ? 1 : 0;
	return true;
}

static bool Cmd_OffersSpells_Execute(COMMAND_ARGS)
{
	*result = GetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Spells) ? 1 : 0;
	return true;
}

static bool Cmd_OffersMagicItems_Execute(COMMAND_ARGS)
{
	*result = GetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_MagicItems) ? 1 : 0;
	return true;
}

static bool Cmd_OffersPotions_Execute(COMMAND_ARGS)
{
	*result = GetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Potions) ? 1 : 0;
	return true;
}

static bool Cmd_OffersTraining_Execute(COMMAND_ARGS)
{
	*result = GetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Training) ? 1 : 0;
	return true;
}

static bool Cmd_OffersRecharging_Execute(COMMAND_ARGS)
{
	*result = GetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Recharge) ? 1 : 0;
	return true;
}

static bool Cmd_OffersRepair_Execute(COMMAND_ARGS)
{
	*result = GetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Repair) ? 1 : 0;
	return true;
}

static bool Cmd_GetTrainerSkill_Execute(COMMAND_ARGS)
{
	TESNPC* npc = NULL;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &npc))
		return true;
	if (!npc && thisObj)
		npc = (TESNPC*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);

	if (npc && npc->aiForm.OffersServices(TESAIForm::kService_Training))
		*result = npc->aiForm.trainingSkill + kActorVal_Armorer;

	return true;
}

static bool Cmd_GetTrainerLevel_Execute(COMMAND_ARGS)
{
	TESNPC* npc = NULL;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &npc))
		return true;
	if (!npc && thisObj)
		npc = (TESNPC*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);

	if (npc && npc->aiForm.OffersServices(TESAIForm::kService_Training))
		*result = npc->aiForm.trainingLevel;

	return true;
}

static bool Cmd_OffersServicesC_Execute(COMMAND_ARGS)
{
	TESNPC* npc = NULL;
	UInt32 serviceMask = 0;
	if (!ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &serviceMask, &npc))
		return true;

	if (!npc && thisObj)
		npc = (TESNPC*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);

	if (serviceMask && npc && npc->aiForm.OffersServices(serviceMask))
		*result = 1;
	else
		*result = 0;

	return true;
}

static void SetServiceFlag_Execute(COMMAND_ARGS, UInt32 whichService)
{
	TESNPC* npc = NULL;
	UInt32 bSetFlag = 0;
	if (ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &bSetFlag, &npc))
	{
		if (!npc && thisObj)
			npc = (TESNPC*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);

		if (npc)
		{
			npc->aiForm.SetOffersServices(whichService, bSetFlag ? true : false);
		}
	}
}

static bool Cmd_SetOffersWeapons_Execute(COMMAND_ARGS)
{
	SetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Weapons);
	return true;
}

static bool Cmd_SetOffersArmor_Execute(COMMAND_ARGS)
{
	SetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Armor);
	return true;
}

static bool Cmd_SetOffersClothing_Execute(COMMAND_ARGS)
{
	SetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Clothing);
	return true;
}

static bool Cmd_SetOffersBooks_Execute(COMMAND_ARGS)
{
	SetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Books);
	return true;
}

static bool Cmd_SetOffersIngredients_Execute(COMMAND_ARGS)
{
	SetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Ingredients);
	return true;
}

static bool Cmd_SetOffersLights_Execute(COMMAND_ARGS)
{
	SetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Lights);
	return true;
}

static bool Cmd_SetOffersApparatus_Execute(COMMAND_ARGS)
{
	SetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Apparatus);
	return true;
}

static bool Cmd_SetOffersMiscItems_Execute(COMMAND_ARGS)
{
	SetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Misc);
	return true;
}

static bool Cmd_SetOffersPotions_Execute(COMMAND_ARGS)
{
	SetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Potions);
	return true;
}

static bool Cmd_SetOffersSpells_Execute(COMMAND_ARGS)
{
	SetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Spells);
	return true;
}

static bool Cmd_SetOffersMagicItems_Execute(COMMAND_ARGS)
{
	SetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_MagicItems);
	return true;
}

static bool Cmd_SetOffersTraining_Execute(COMMAND_ARGS)
{
	SetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Training);
	return true;
}

static bool Cmd_SetOffersRecharging_Execute(COMMAND_ARGS)
{
	SetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Recharge);
	return true;
}

static bool Cmd_SetOffersRepair_Execute(COMMAND_ARGS)
{
	SetServiceFlag_Execute(PASS_COMMAND_ARGS, TESAIForm::kService_Repair);
	return true;
}

static bool Cmd_GetServicesMask_Execute(COMMAND_ARGS)
{
	TESNPC* npc = NULL;
	*result = 0;
	if (ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &npc))
	{
		if (!npc && thisObj)
			npc = (TESNPC*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
		if (npc)
			*result = npc->aiForm.serviceFlags;
	}
	return true;
}

static bool Cmd_SetServicesMask_Execute(COMMAND_ARGS)
{
	TESNPC* npc = NULL;
	UInt32 serviceMask = 0;

	*result = 0;
	if (ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &serviceMask, &npc))
	{
		if (!npc && thisObj)
			npc = (TESNPC*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
		if (npc)
		{
			npc->aiForm.SetOffersServices(serviceMask, true, true);
		}
	}
	return true;
}

static bool Cmd_SetTrainerSkill_Execute(COMMAND_ARGS)
{
	TESNPC* npc = NULL;
	UInt32 actorVal = 0;

	*result = 0;
	if (ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &actorVal, &npc))
	{
		if (!npc && thisObj)
			npc = (TESNPC*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
		if (npc && actorVal >= kActorVal_Armorer && actorVal <= kActorVal_Speechcraft)
		{
			FormChangeInfo info(kChangeForm_TrainerSkill, npc, scriptObj);
			g_FormChangesMap.Add(info);
			npc->aiForm.trainingSkill = actorVal - kActorVal_Armorer;
		}
	}
	return true;
}

static bool Cmd_SetTrainerLevel_Execute(COMMAND_ARGS)
{
	TESNPC* npc = NULL;
	UInt32 nuLevel = 0;

	*result = 0;
	if (ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &nuLevel, &npc))
	{
		if (!npc && thisObj)
			npc = (TESNPC*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESNPC, 0);
		if (npc)
		{
			FormChangeInfo info(kChangeForm_TrainerLevel, npc, scriptObj);
			g_FormChangesMap.Add(info);
			npc->aiForm.trainingLevel = nuLevel;
		}
	}
	return true;
}

static bool Cmd_GetNumPackages_Execute(COMMAND_ARGS)
{
	TESActorBase* actorBase = NULL;
	*result = 0;

	if (ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &actorBase))
	{
		if (!actorBase && thisObj)
			actorBase = (TESActorBase*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESActorBase, 0);
		if (actorBase)
			*result = PackageListVisitor(&actorBase->aiForm.packageList).Count();
	}
	return true;
}

static bool Cmd_GetNthPackage_Execute(COMMAND_ARGS)
{
	TESActorBase* actorBase = NULL;
	UInt32 whichPackage = 0;
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (ExtractArgs(paramInfo, arg1, opcodeOffsetPtr, thisObj, arg3, scriptObj, eventList, &whichPackage, &actorBase))
	{
		if (!actorBase && thisObj)
			actorBase = (TESActorBase*)Oblivion_DynamicCast(thisObj->baseForm, 0, RTTI_TESForm, RTTI_TESActorBase, 0);
		if (actorBase)
		{
			TESPackage* pkg = PackageListVisitor(&actorBase->aiForm.packageList).GetNthInfo(whichPackage);
			if (pkg)
				*refResult = pkg->refID;
		}
	}
	return true;
}

static HighProcess* ExtractHighProcess(TESObjectREFR* thisObj)
{
	HighProcess* hiProc = NULL;
	MobileObject* mob = (MobileObject*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_MobileObject, 0);
	if (mob)
		hiProc = (HighProcess*)Oblivion_DynamicCast(mob->process, 0, RTTI_BaseProcess, RTTI_HighProcess, 0);

	return hiProc;
}

static bool Cmd_IsAttacking_Execute(COMMAND_ARGS)
{
	*result = 0;
	HighProcess* hiProc = ExtractHighProcess(thisObj);
	if (hiProc)
		*result = hiProc->IsAttacking() ? 1 : 0;

	return true;
}

static bool Cmd_IsBlocking_Execute(COMMAND_ARGS)
{
	*result = 0;
	HighProcess* hiProc = ExtractHighProcess(thisObj);
	if (hiProc)
		*result = hiProc->IsBlocking() ? 1 : 0;

	return true;
}

static bool Cmd_IsRecoiling_Execute(COMMAND_ARGS)
{
	*result = 0;
	HighProcess* hiProc = ExtractHighProcess(thisObj);
	if (hiProc)
		*result = hiProc->IsRecoiling() ? 1 : 0;

	return true;
}

static bool Cmd_IsDodging_Execute(COMMAND_ARGS)
{
	*result = 0;
	HighProcess* hiProc = ExtractHighProcess(thisObj);
	if (hiProc)
		*result = hiProc->IsDodging() ? 1 : 0;

	return true;
}

static bool Cmd_IsStaggered_Execute(COMMAND_ARGS)
{
	*result = 0;
	HighProcess* hiProc = ExtractHighProcess(thisObj);
	if (hiProc)
		*result = hiProc->IsStaggered() ? 1 : 0;

	return true;
}

static bool IsMovementFlagSet_Execute(TESObjectREFR* thisObj, UInt32 flag)
{
	HighProcess* hiProc = ExtractHighProcess(thisObj);
	if (hiProc && hiProc->IsMovementFlagSet(flag))
		return true;
	else
		return false;
}

static bool Cmd_IsMovingForward_Execute(COMMAND_ARGS)
{
	*result = IsMovementFlagSet_Execute(thisObj, HighProcess::kMovement_Forward) ? 1 : 0;
	return true;
}

static bool Cmd_IsMovingBackward_Execute(COMMAND_ARGS)
{
	*result = IsMovementFlagSet_Execute(thisObj, HighProcess::kMovement_Backward) ? 1 : 0;
	return true;
}

static bool Cmd_IsMovingLeft_Execute(COMMAND_ARGS)
{
	*result = IsMovementFlagSet_Execute(thisObj, HighProcess::kMovement_Left) ? 1 : 0;
	return true;
}

static bool Cmd_IsMovingRight_Execute(COMMAND_ARGS)
{
	*result = IsMovementFlagSet_Execute(thisObj, HighProcess::kMovement_Right) ? 1 : 0;
	return true;
}

static bool Cmd_IsTurningLeft_Execute(COMMAND_ARGS)
{
	*result = IsMovementFlagSet_Execute(thisObj, HighProcess::kMovement_TurnLeft) ? 1 : 0;
	return true;
}

static bool Cmd_IsTurningRight_Execute(COMMAND_ARGS)
{
	*result = IsMovementFlagSet_Execute(thisObj, HighProcess::kMovement_TurnRight) ? 1 : 0;
	return true;
}

static bool Cmd_IsInAir_Execute(COMMAND_ARGS)
{
	*result = 0;
	HighProcess* hiProc = ExtractHighProcess(thisObj);
	if (hiProc)
	{
		bhkCharacterController* ctrlr = hiProc->GetCharacterController();
		if (ctrlr)
		{
			if (ctrlr->baseObject.IsInAir())
				*result = 1;
		}
	}

	return true;
}

static bool Cmd_IsJumping_Execute(COMMAND_ARGS)
{
	*result = 0;
	HighProcess* hiProc = ExtractHighProcess(thisObj);
	if (hiProc)
	{
		bhkCharacterController* ctrlr = hiProc->GetCharacterController();
		if (ctrlr && ctrlr->baseObject.IsJumping())
			*result = 1;
	}

	return true;
}

static bool Cmd_IsOnGround_Execute(COMMAND_ARGS)
{
	*result = 0;
	HighProcess* hiProc = ExtractHighProcess(thisObj);
	if (hiProc)
	{
		bhkCharacterController* ctrlr = hiProc->GetCharacterController();
		if (ctrlr && ctrlr->baseObject.IsOnGround())
			*result = 1;
	}

	return true;
}

static bool Cmd_IsFlying_Execute(COMMAND_ARGS)
{
	*result = 0;
	HighProcess* hiProc = ExtractHighProcess(thisObj);
	if (hiProc)
	{
		bhkCharacterController* ctrlr = hiProc->GetCharacterController();
		if (ctrlr && ctrlr->baseObject.IsFlying())
			*result = 1;
	}

	return true;
}

static bool Cmd_GetFallTimer_Execute(COMMAND_ARGS)
{
	*result = 0;

	HighProcess* hiProc = ExtractHighProcess(thisObj);
	if (hiProc)
	{
		bhkCharacterController* ctrlr = hiProc->GetCharacterController();
		if (ctrlr)
			*result = ctrlr->fallDamageTimer;
	}

	return true;
}

static MiddleHighProcess* ExtractMiddleHighProcess(TESObjectREFR* thisObj)
{
	MiddleHighProcess* proc = NULL;
	MobileObject* mob = (MobileObject*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_MobileObject, 0);
	if (mob)
		proc = (MiddleHighProcess*)Oblivion_DynamicCast(mob->process, 0, RTTI_BaseProcess, RTTI_MiddleHighProcess, 0);

	return proc;
}

ActorAnimData* GetActorAnimData(TESObjectREFR* callingObj)
{
	if (callingObj == *g_thePlayer && (*g_thePlayer)->isThirdPerson == 0)
		return (*g_thePlayer)->firstPersonAnimData;
	else
	{
		MiddleHighProcess* proc = ExtractMiddleHighProcess(callingObj);
		if (proc)
			return proc->animData;
	}

	return NULL;
}

static bool Cmd_IsPowerAttacking_Execute(COMMAND_ARGS)
{
	*result = 0;
	ActorAnimData* animData = GetActorAnimData(thisObj);
	if (animData)
	{
		if (animData->FindAnimInRange(TESAnimGroup::kAnimGroup_AttackPower, TESAnimGroup::kAnimGroup_AttackRightPower))
			*result = 1;
	}
	return true;
}

static bool Cmd_IsCasting_Execute(COMMAND_ARGS)
{
	*result = 0;
	ActorAnimData* animData = GetActorAnimData(thisObj);
	if (animData)
	{
		if (animData->FindAnimInRange(TESAnimGroup::kAnimGroup_CastSelf, TESAnimGroup::kAnimGroup_CastTargetAlt))
			*result = 1;
	}
	return true;
}

static bool Cmd_IsAnimGroupPlaying_Execute(COMMAND_ARGS)
{
	UInt32 whichGroup = -1;
	*result = 0;

	if (!ExtractArgs(EXTRACT_ARGS, &whichGroup))
		return true;
	else if (whichGroup > TESAnimGroup::kAnimGroup_Max)
		return true;

	ActorAnimData* animData = GetActorAnimData(thisObj);
	if (animData)
		*result = (animData->FindAnimInRange(whichGroup)) ? 1 : 0;

	return true;
}

static bool Cmd_AnimPathIncludes_Execute(COMMAND_ARGS)
{
	char subStr[512] = { 0 };
	*result = 0;

	if (!ExtractArgs(EXTRACT_ARGS, &subStr))
		return true;

	ActorAnimData* animData = GetActorAnimData(thisObj);
	if (animData)
		*result = (animData->PathsInclude(subStr)) ? 1 : 0;

	return true;
}

static bool Cmd_GetProcessLevel_Execute(COMMAND_ARGS)
{
	*result = -1;
	if (thisObj)
	{
		MobileObject* mob = (MobileObject*)Oblivion_DynamicCast(thisObj, 0, RTTI_TESObjectREFR, RTTI_MobileObject, 0);
		if (mob && mob->process)
			*result = mob->process->GetProcessLevel();
	}
	return true;
}
			
#endif


static ParamInfo kParams_OneNPC[1] =
{
	{	"NPC",	kParamType_NPC,	1	},
};

static ParamInfo kParams_OneIntOneOptionalNPC[2] =
{
	{	"RGB value",	kParamType_Integer,	0	},
	{	"NPC",			kParamType_NPC,		1	},
};

CommandInfo kCommandInfo_GetNumFollowers =
{
	"GetNumFollowers", "", 0,
	"returns the number of characters following the calling actor",
	1, 0, NULL,
	HANDLER(Cmd_GetNumFollowers_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNthFollower =
{
	"GetNthFollower", "", 0,
	"returns the nth actor following the calling actor",
	1,
	1,
	kParams_OneInt,
	HANDLER(Cmd_GetNthFollower_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNthDetectedActor =
{
	"GetNthDetectedActor",
	"",
	0,
	"returns the nth actor detected by the calling actor",
	1,
	1,
	kParams_OneInt,
	HANDLER(Cmd_GetNthDetectedActor_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNumDetectedActors =
{
	"GetNumDetectedActors",
	"",
	0,
	"returns the number of actors detected by the calling actor",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetNumDetectedActors_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_SetDetectionState[2] =
{
	{	"actor",	kParamType_Actor,	0	},
	{	"level",	kParamType_Integer,	0	},
};

CommandInfo kCommandInfo_SetDetectionState =
{
	"SetDetectionState", "",
	0,
	"sets the level at which the calling actor detects the specified actor",
	1, 2, kParams_SetDetectionState,
	HANDLER(Cmd_SetDetectionState_Execute),
	Cmd_Default_Parse,
	NULL, 0
};

CommandInfo kCommandInfo_OffersWeapons =
{
	"OffersWeapons",
	"",
	0,
	"returns 1 if the calling actor offers the specified service",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_OffersWeapons_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OffersArmor =
{
	"OffersArmor",
	"",
	0,
	"returns 1 if the calling actor offers the specified service",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_OffersArmor_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OffersBooks =
{
	"OffersBooks",
	"",
	0,
	"returns 1 if the calling actor offers the specified service",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_OffersBooks_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OffersClothing =
{
	"OffersClothing",
	"",
	0,
	"returns 1 if the calling actor offers the specified service",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_OffersClothing_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OffersIngredients =
{
	"OffersIngredients",
	"",
	0,
	"returns 1 if the calling actor offers the specified service",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_OffersIngredients_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OffersLights =
{
	"OffersLights",
	"",
	0,
	"returns 1 if the calling actor offers the specified service",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_OffersLights_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OffersApparatus =
{
	"OffersApparatus",
	"",
	0,
	"returns 1 if the calling actor offers the specified service",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_OffersApparatus_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OffersMiscItems =
{
	"OffersMiscItems",
	"",
	0,
	"returns 1 if the calling actor offers the specified service",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_OffersMiscItems_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OffersMagicItems =
{
	"OffersMagicItems",
	"",
	0,
	"returns 1 if the calling actor offers the specified service",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_OffersMagicItems_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OffersSpells =
{
	"OffersSpells",
	"",
	0,
	"returns 1 if the calling actor offers the specified service",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_OffersSpells_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OffersRecharging =
{
	"OffersRecharging",
	"",
	0,
	"returns 1 if the calling actor offers the specified service",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_OffersRecharging_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OffersTraining =
{
	"OffersTraining",
	"",
	0,
	"returns 1 if the calling actor offers the specified service",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_OffersTraining_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OffersPotions =
{
	"OffersPotions",
	"",
	0,
	"returns 1 if the calling actor offers the specified service",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_OffersPotions_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OffersRepair =
{
	"OffersRepair",
	"",
	0,
	"returns 1 if the calling actor offers the specified service",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_OffersRepair_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetTrainerSkill =
{
	"GetTrainerSkill",
	"",
	0,
	"returns the skill in which the actor offers training",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_GetTrainerSkill_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetTrainerLevel =
{
	"GetTrainerLevel",
	"",
	0,
	"returns the level at which the actor offers training",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_GetTrainerLevel_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_OffersServicesC =
{
	"OffersServicesC",
	"",
	0,
	"returns 1 if the calling actor offers the specified services",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_OffersServicesC_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOffersWeapons =
{
	"SetOffersWeapons",
	"",
	0,
	"sets the specified offers services flag for the NPC",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetOffersWeapons_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOffersArmor =
{
	"SetOffersArmor",
	"",
	0,
	"sets the specified offers services flag for the NPC",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetOffersArmor_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOffersClothing =
{
	"SetOffersClothing",
	"",
	0,
	"sets the specified offers services flag for the NPC",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetOffersClothing_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOffersBooks =
{
	"SetOffersBooks",
	"",
	0,
	"sets the specified offers services flag for the NPC",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetOffersBooks_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOffersLights =
{
	"SetOffersLights",
	"",
	0,
	"sets the specified offers services flag for the NPC",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetOffersLights_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOffersPotions =
{
	"SetOffersPotions",
	"",
	0,
	"sets the specified offers services flag for the NPC",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetOffersPotions_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOffersSpells =
{
	"SetOffersSpells",
	"",
	0,
	"sets the specified offers services flag for the NPC",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetOffersSpells_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOffersIngredients =
{
	"SetOffersIngredients",
	"",
	0,
	"sets the specified offers services flag for the NPC",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetOffersIngredients_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOffersApparatus =
{
	"SetOffersApparatus",
	"",
	0,
	"sets the specified offers services flag for the NPC",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetOffersApparatus_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOffersMiscItems =
{
	"SetOffersMiscItems",
	"",
	0,
	"sets the specified offers services flag for the NPC",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetOffersMiscItems_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOffersMagicItems =
{
	"SetOffersMagicItems",
	"",
	0,
	"sets the specified offers services flag for the NPC",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetOffersMagicItems_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOffersTraining =
{
	"SetOffersTraining",
	"",
	0,
	"sets the specified offers services flag for the NPC",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetOffersTraining_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOffersRepair =
{
	"SetOffersRepair",
	"",
	0,
	"sets the specified offers services flag for the NPC",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetOffersRepair_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOffersRecharging =
{
	"SetOffersRecharging",
	"",
	0,
	"sets the specified offers services flag for the NPC",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetOffersRecharging_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetTrainerLevel =
{
	"SetTrainerLevel",
	"",
	0,
	"sets the level at which the NPC offers training",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetTrainerLevel_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_SetTrainerSkill[2] =
{
	{	"skill",	kParamType_ActorValue,	0	},
	{	"npc",		kParamType_NPC,			1	},
};

CommandInfo kCommandInfo_SetTrainerSkill =
{
	"SetTrainerSkill",
	"",
	0,
	"sets the skill in which the NPC offers training",
	0,
	2,
	kParams_SetTrainerSkill,
	HANDLER(Cmd_SetTrainerSkill_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_SetOffersServicesC =
{
	"SetServicesMask",
	"SetOffersServicesC",
	0,
	"sets the services offered by the npc",
	0,
	2,
	kParams_OneIntOneOptionalNPC,
	HANDLER(Cmd_SetServicesMask_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetServicesMask =
{
	"GetServicesMask",
	"GetServicesC",
	0,
	"returns the code for all services offered by the npc",
	0,
	1,
	kParams_OneNPC,
	HANDLER(Cmd_GetServicesMask_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_OneOptionalActorBase[1] =
{
	{	"actor",	kParamType_ActorBase,	1	},
};

static ParamInfo kParams_OneIntOneOptionalActorBase[2] =
{
	{	"int",		kParamType_Integer,		0	},
	{	"actor",	kParamType_ActorBase,	1	},
};

CommandInfo kCommandInfo_GetNumPackages =
{
	"GetNumPackages",
	"",
	0,
	"returns the number of packages in the actor's package list",
	0,
	1,
	kParams_OneOptionalActorBase,
	HANDLER(Cmd_GetNumPackages_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetNthPackage =
{
	"GetNthPackage",
	"",
	0,
	"returns the nth package in the actor's package list",
	0,
	2,
	kParams_OneIntOneOptionalActorBase,
	HANDLER(Cmd_GetNthPackage_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsAttacking =
{
	"IsAttacking",
	"",
	0,
	"returns true if the calling actor is attacking",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsAttacking_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsBlocking =
{
	"IsBlocking",
	"",
	0,
	"returns true if the calling actor is Blocking",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsBlocking_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsDodging =
{
	"IsDodging",
	"",
	0,
	"returns true if the calling actor is Dodging",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsDodging_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsRecoiling =
{
	"IsRecoiling",
	"",
	0,
	"returns true if the calling actor is Recoiling",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsRecoiling_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsStaggered =
{
	"IsStaggered",
	"",
	0,
	"returns true if the calling actor is Staggered",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsStaggered_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMovingForward =
{
	"IsMovingForward",
	"",
	0,
	"returns 1 if the calling actor is performing the specified movement",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsMovingForward_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMovingBackward =
{
	"IsMovingBackward",
	"",
	0,
	"returns 1 if the calling actor is performing the specified movement",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsMovingBackward_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMovingLeft =
{
	"IsMovingLeft",
	"",
	0,
	"returns 1 if the calling actor is performing the specified movement",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsMovingLeft_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsMovingRight =
{
	"IsMovingRight",
	"",
	0,
	"returns 1 if the calling actor is performing the specified movement",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsMovingRight_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsTurningLeft =
{
	"IsTurningLeft",
	"",
	0,
	"returns 1 if the calling actor is performing the specified movement",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsTurningLeft_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsTurningRight =
{
	"IsTurningRight",
	"",
	0,
	"returns 1 if the calling actor is performing the specified movement",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsTurningRight_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsInAir =
{
	"IsInAir",
	"",
	0,
	"returns 1 if the calling actor is in the air",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsInAir_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsJumping =
{
	"IsJumping",
	"",
	0,
	"returns 1 if the calling actor is jumping",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsJumping_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsOnGround =
{
	"IsOnGround",
	"",
	0,
	"returns 1 if the calling actor is on the ground",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsOnGround_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsFlying =
{
	"IsFlying",
	"",
	0,
	"returns 1 if the calling actor is flying",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsFlying_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_GetFallTimer =
{
	"GetFallTimer",
	"GetFallDamageTimer",
	0,
	"returns the length of time for which the calling actor has been falling",
	1,
	0,
	NULL,
	HANDLER(Cmd_GetFallTimer_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

CommandInfo kCommandInfo_IsCasting =
{
	"IsCasting",
	"",
	0,
	"returns true if the calling actor is casting a spell",
	1,
	0,
	NULL,
	HANDLER(Cmd_IsCasting_Execute),
	Cmd_Default_Parse,
	NULL,
	0
};

static ParamInfo kParams_OneAnimGroup[1] =
{
	{	"anim group",	kParamType_AnimationGroup,	0	},
};

DEFINE_COMMAND(IsPowerAttacking,
			   returns true if the calling actor is executing a power attack,
			   1,
			   0,
			   NULL);

DEFINE_COMMAND(IsAnimGroupPlaying,
			   returns true if the actor is playing the specified anim group,
			   1,
			   1,
			   kParams_OneAnimGroup);

DEFINE_COMMAND(AnimPathIncludes, 
			   returns true if the actor is playing an anim containing the substring,
			   1,
			   1,
			   kParams_OneString);

static ParamInfo kParams_Package[1] =
{
	{	"package",	kParamType_AIPackage,	0	},
};

DEFINE_COMMAND(GetProcessLevel,
			   returns the process level of the calling reference,
			   1,
			   0,
			   NULL);

