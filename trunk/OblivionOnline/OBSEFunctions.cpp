#include "main.h"
/* FROM OBSE */
inline bool IsRingSlot(UInt32 slot)
{
	return slot == kSlot_RightRing || slot == kSlot_LeftRing;
}

inline bool IsOppositeRing(UInt32 slot, UInt32 slotIdx) {
	return (slot == kSlot_RightRing && slotIdx == kSlot_LeftRing) || (slot == kSlot_LeftRing && slotIdx == kSlot_RightRing);
}
UInt32 GetItemSlot(TESForm* type)
{
	if (type) {
		TESBipedModelForm	* bip = (TESBipedModelForm *)Oblivion_DynamicCast(type, 0, RTTI_TESForm, RTTI_TESBipedModelForm, 0);
		if (bip) {
			return bip->GetSlot();
		} else if (Oblivion_DynamicCast(type, 0, RTTI_TESForm, RTTI_TESObjectWEAP, 0) != 0) {
			return kSlot_Weapon;
		} else if (Oblivion_DynamicCast(type, 0, RTTI_TESForm, RTTI_TESAmmo, 0) != 0) {
			return kSlot_Ammo;
		}
	}
	return kSlot_None;
}
bool FindEquipped(TESObjectREFR* thisObj, UInt32 slotIdx, FoundEquipped* foundEquippedFunctor, double* result)
{
	ExtraContainerChanges	* containerChanges = static_cast <ExtraContainerChanges *>(thisObj->baseExtraList.GetByType(kExtraData_ContainerChanges));
	if(containerChanges && containerChanges->data && containerChanges->data->objList)
	{
		for(ExtraContainerChanges::Entry * entry = containerChanges->data->objList; entry; entry = entry->next)
		{
			// do the fast check first (an object must have extend data to be equipped)
			if(entry->data && entry->data->extendData && entry->data->type)
			{
				UInt32 slot = GetItemSlot(entry->data->type);
				if (slot != kSlot_None) {
					// ok, it's the right type, now is it equipped?
					for(ExtraContainerChanges::EntryExtendData * extend = entry->data->extendData; extend; extend = extend->next)
					{
						if (extend->data) {
							// handle rings
							bool bFound = false;
							if (IsRingSlot(slotIdx) && IsRingSlot(slot)) {
								if (slotIdx == kSlot_LeftRing && extend->data->HasType(kExtraData_WornLeft)) {
									bFound = true;
								} else if (slotIdx == kSlot_RightRing && extend->data->HasType(kExtraData_Worn)) {
									bFound = true;
								}
							} else if (slot == slotIdx && extend->data->HasType(kExtraData_Worn)) {
								bFound = true;
							}
							
							if (bFound) {
								return foundEquippedFunctor->Found(entry->data, result);
							}
						}
					}
				}
			}
		}
	}
	return false;
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

void RunScriptLine(const char *buf, bool IsTemp)
{
	// create a Script object
	UInt8	scriptObjBuf[sizeof(Script)];
	Script	* tempScriptObj = (Script *)scriptObjBuf;

	void	* scriptState = GetGlobalScriptStateObj();

	tempScriptObj->Constructor();
	if (IsTemp)
		tempScriptObj->MarkAsTemporary();
	tempScriptObj->SetText(buf);
	tempScriptObj->CompileAndRun(*((void **)scriptState), 1, NULL);
	tempScriptObj->StaticDestructor();
}