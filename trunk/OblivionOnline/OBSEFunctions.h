#ifndef OBSEFunctions_h
#define OBSEFunctions_h

enum
{
	kSlot_None = -1,
	// lower slots are TESBipedModelForm::kPart_*
	kSlot_RightRing = TESBipedModelForm::kPart_RightRing,
	kSlot_LeftRing,
	kSlot_Weapon = TESBipedModelForm::kPart_Max,	// 16
	kSlot_Ammo,
	kSlot_RangedWeapon,
};

class FoundEquipped
{
public:
	virtual bool Found(ExtraContainerChanges::EntryData* entryData, double *result) = 0;
};

class feGetObject : public FoundEquipped
{
public:
	feGetObject() {}
	~feGetObject() {}
	virtual bool Found(ExtraContainerChanges::EntryData* entryData, double *result) {
		UInt32* refResult = (UInt32*) result;
		if (entryData) {
			// cool, we win
			*refResult = entryData->type->refID;
			//Console_Print("refID = %08X (%s)", *refResult, GetFullName(entryData->type));
			return true;
		}
		return false;
	}
};

ActorAnimData* GetActorAnimData(TESObjectREFR* callingObj);
#endif