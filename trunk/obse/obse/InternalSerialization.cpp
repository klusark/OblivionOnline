#include <map>
#include "InternalSerialization.h"
#include "StringVar.h"
#include <algorithm>
#include <string>

/*******************************
*	Callbacks
*******************************/
void Core_SaveCallback(void * reserved)
{
	//g_StringMap.Save(&g_OBSESerializationInterface);
	//g_FormChangesMap.Save(&g_OBSESerializationInterface);
}

void Core_LoadCallback(void * reserved)
{
/*	v0016
	OBSESerializationInterface* intfc = &g_OBSESerializationInterface;
	UInt32 type, version, length;

	while (intfc->GetNextRecordInfo(&type, &version, &length))
	{
		switch (type)
		{
		case 'STVS':
			g_StringMap.Load(intfc);
			break;
		case 'FMCS':
			g_FormChangesMap.Load(intfc);
			break;
		default:
			_MESSAGE("Unhandled chunk type in LoadCallback: %d", type);
			continue;
		}
	}
*/
}

void Core_NewGameCallback(void * reserved)
{
	//g_StringMap.Reset(&g_OBSESerializationInterface);
	//g_FormChangesMap.Reset(&g_OBSESerializationInterface);
}

void Init_CoreSerialization_Callbacks()
{
	Serialization::InternalSetSaveCallback(0, Core_SaveCallback);
	Serialization::InternalSetLoadCallback(0, Core_LoadCallback);
	Serialization::InternalSetNewGameCallback(0, Core_NewGameCallback);
}

FormChangesMap::FormChangesMap()
{
	//
}

FormChangesMap::~FormChangesMap()
{
	//
}

void FormChangesMap::Add(FormChangeInfo &info)
{
	bool bEntryExists = false;

	std::pair<ChangeMap::iterator, ChangeMap::iterator> range = changes.equal_range(info.modifiedForm->refID);
	if (range.first != range.second)
	{
		for (ChangeMap::iterator iter = range.first; iter != range.second; ++iter)
		{
			if (iter->second->changeType == info.changeType)
			{
				iter->second->Modify(info);
				bEntryExists = true;
				break;
			}
		}
	}
	if (!bEntryExists)
	{
		FormChangeEntry* nuEntry = FormChangeEntry::Create(info);
		changes.insert(ChangeMapValue(info.modifiedForm->refID, nuEntry));
	}
}

/*
class CountUniqueKeysPredicate
{
public:
	CountUniqueKeysPredicate() : m_curKey(0)
		{	}
	bool operator()(FormChangesMap::ChangeMapValue val)
	{
		if (val.first != m_curKey)
		{
			m_curKey = val.first;
			return true;
		}
		else
			return false;
	}

	UInt32 m_numKeys;
	UInt32 m_curKey;
};
*/

void FormChangesMap::Save(OBSESerializationInterface* intfc)
{
	intfc->OpenRecord('FMCS', 0);

	UInt8 numChanges = 0;
	UInt32 curFormID = 0;
	for (ChangeMap::iterator iter = changes.begin(); iter != changes.end(); ++iter)
	{
		if (iter->first != curFormID)
		{
			curFormID = iter->first;
			intfc->OpenRecord('FMCR', 0);
			intfc->WriteRecordData(&curFormID, sizeof(UInt32));
		}
		iter->second->Save(intfc);
	}
	intfc->OpenRecord('FMCE', 0);
}

void FormChangesMap::Load(OBSESerializationInterface* intfc)
{
	UInt32 recType, recVersion, recLength, formID, nuFormID;
	UInt16 changeType;
	TESForm* curForm = NULL;

	Clear();

	_MESSAGE("Loading form changes");
	bool bContinue = true;
	bool bSkipCurrentRecord = false;
	while (bContinue && intfc->GetNextRecordInfo(&recType, &recVersion, &recLength))
	{
		switch (recType)
		{
		case 'FMCE':			//end of data
			bContinue = false;
			break;
		case 'FMCR':			//new form
			bSkipCurrentRecord = false;
			intfc->ReadRecordData(&formID, sizeof(formID));
			bSkipCurrentRecord = !intfc->ResolveRefID(formID, &nuFormID);	//if can't resolve refID, skip changes

			curForm = LookupFormByID(nuFormID);
			if (!curForm)							//probably non-persistent reference; skip
				bSkipCurrentRecord = true;
			
			break;
		case 'FMCH':			//change entry for current form
			if (!bSkipCurrentRecord)
			{
				intfc->ReadRecordData(&changeType, sizeof(changeType));
				FormChangeInfo info(changeType, curForm, NULL);
				FormChangeEntry* changeEntry = FormChangeEntry::Create(info);
				if (changeEntry->Load(intfc))
					changes.insert(ChangeMapValue(curForm->refID, changeEntry));
			}
			break;
		default:
			_MESSAGE("Error loading form changes: unhandled chunk type %d", recType);
		}
	}
}

void FormChangesMap::Reset(OBSESerializationInterface *intfc)
{
	Clear();
}

void FormChangesMap::Clear()
{
	for (FormChangesMap::ChangeMap::iterator iter = changes.begin(); iter != changes.end(); ++iter)
	{
		delete iter->second;
	}
	changes.clear();
}

FormChangesMap g_FormChangesMap;

/**********************************
*	Change Entries
**********************************/

FormChangeEntry* FormChangeEntry::Create(FormChangeInfo &info)
{
	switch (info.changeType)
	{
	case kChangeForm_Name:
		return new ChangeNameEntry(info);
	case kChangeForm_MerchantContainer:
		return new ChangeMerchantContainerEntry(info);
	case kChangeForm_TravelHorse:
		return new ChangeTravelHorseEntry(info);
	case kChangeForm_TrainerLevel:
		return new ChangeTrainerLevelEntry(info);
	case kChangeForm_TrainerSkill:
		return new ChangeTrainerSkillEntry(info);
	default:
		_MESSAGE("Error initializing change entry: invalid change type %d", info.changeType);
	}

	return NULL;
}

FormChangeEntry::FormChangeEntry(FormChangeInfo &info) : changeType(info.changeType)
{	
	//
}

FormChangeEntry::~FormChangeEntry()
{
	//
}

void FormChangeEntry::Modify(FormChangeInfo &info)
{
	_MESSAGE("Error: base class Modify() function called during serialization");
}

void FormChangeEntry::Save(OBSESerializationInterface *intfc)
{
	_MESSAGE("Error: base class Save() function called during serialization");
}

bool FormChangeEntry::Load(OBSESerializationInterface *intfc)
{
	_MESSAGE("Error: base class Load() function called during serialization");
	return false;
}

UInt8 FormChangeEntry::GetFixedModIndex(UInt8 modIndex)
{
	UInt32 refID = modIndex << 24;
	if (g_OBSESerializationInterface.ResolveRefID(refID, &refID))
		return refID >> 24;
	else
		return 0xFF;	//mod no longer loaded
}

bool FormChangeEntry::IsModIndexValid(UInt8 modIndex)
{
	UInt32 dummyRefID;
	return g_OBSESerializationInterface.ResolveRefID(modIndex << 24, &dummyRefID);
}

FormChangeInfo::FormChangeInfo(UInt16 in_changeType, TESForm* in_Form, Script* owningScript)
					: changeType(in_changeType), modifiedForm(in_Form)
{
	if (owningScript)
		owningModIndex = owningScript->refID >> 24;
}

FormChangeInfo::~FormChangeInfo()
{
	//
}

TESFullName* ChangeNameEntry::GetNamedForm(TESForm* form)
{
	if (form->typeID == kFormType_REFR)	//mapmarker reference
	{
		TESObjectREFR* refr = (TESObjectREFR*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESObjectREFR, 0);
		ExtraMapMarker* mapMarker = (ExtraMapMarker*)refr->baseExtraList.GetByType(kExtraData_MapMarker);
		if (mapMarker)
			return mapMarker->fullName;
		else
		{
			_MESSAGE("Error: ChangeNameEntry unable to extract MapMarker name");
			return NULL;
		}
	}
	else
		return (TESFullName*)Oblivion_DynamicCast(form, 0, RTTI_TESForm, RTTI_TESFullName, 0);
}
		
ChangeNameEntry::ChangeNameEntry(FormChangeInfo &info) 
	: FormChangeEntry(info), owningModIndex(info.owningModIndex), namedForm(info.modifiedForm)
{
	TESFullName* name = GetNamedForm(info.modifiedForm);
	
	prevName = new char[name->name.m_dataLen + 1];
	memcpy(prevName, name->name.m_data, name->name.m_dataLen);
	prevName[name->name.m_dataLen] = 0;
}

ChangeNameEntry::~ChangeNameEntry()
{
	TESFullName* name = GetNamedForm(namedForm);
	name->name.Set(prevName);
	delete[] prevName;
}

void ChangeNameEntry::Save(OBSESerializationInterface* intfc)
{
	TESFullName* name = GetNamedForm(namedForm);

	if (std::string(name->name.m_data) != std::string(prevName))	//only save if name has not been reverted
	{
		intfc->OpenRecord('FMCH', sizeof(UInt32));
		intfc->WriteRecordData(&changeType, sizeof(changeType));

		intfc->WriteRecordData(&owningModIndex, sizeof(owningModIndex));
		intfc->WriteRecordData(&name->name.m_dataLen, sizeof(UInt16));
		intfc->WriteRecordData(name->name.m_data, name->name.m_dataLen);
	}
}

bool ChangeNameEntry::Load(OBSESerializationInterface* intfc)
{
	UInt8 modIndex;
	intfc->ReadRecordData(&modIndex, sizeof(modIndex));
	if (!IsModIndexValid(modIndex))
	{
		_MESSAGE("Mod Index %d not valid", (UInt32)modIndex);
		return false;
	}
	else
		owningModIndex = GetFixedModIndex(modIndex);

	UInt16 dataLength;
	intfc->ReadRecordData(&dataLength, sizeof(dataLength));

	char* buffer = new char[dataLength + 1];
	intfc->ReadRecordData(buffer, dataLength);
	buffer[dataLength] = 0;

	TESFullName* name = GetNamedForm(namedForm);
	name->name.Set(buffer);
	delete[] buffer;

	return true;
}

void ChangeNameEntry::Modify(FormChangeInfo &info)
{
	owningModIndex = info.owningModIndex;
}

ChangeMerchantContainerEntry::ChangeMerchantContainerEntry(FormChangeInfo &info)
	: FormChangeEntry(info), owningModIndex(info.owningModIndex)
{
	merchant = (Actor*)Oblivion_DynamicCast(info.modifiedForm, 0, RTTI_TESForm, RTTI_Actor, 0);
	ExtraMerchantContainer* xCont = GetMerchantContainer();
	if (xCont)
		prevMerchantContainer = xCont->containerRef;
	else
		prevMerchantContainer = NULL;
}

ChangeMerchantContainerEntry::~ChangeMerchantContainerEntry()
{
	ExtraMerchantContainer* xCont = GetMerchantContainer();
	if (prevMerchantContainer && xCont)
		xCont->containerRef = prevMerchantContainer;
	else if (xCont)
		merchant->baseExtraList.Remove(xCont);
}

ExtraMerchantContainer* ChangeMerchantContainerEntry::GetMerchantContainer()
{
	ExtraMerchantContainer* xCont = NULL;
	BSExtraData* xData = merchant->baseExtraList.GetByType(kExtraData_MerchantContainer);
	if (xData)
		xCont = (ExtraMerchantContainer*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraMerchantContainer, 0);

	return xCont;
}
void ChangeMerchantContainerEntry::Save(OBSESerializationInterface* intfc)
{
	ExtraMerchantContainer* xCont = GetMerchantContainer();
	if (xCont && xCont->containerRef && xCont->containerRef != prevMerchantContainer)
	{
		UInt32 refID = xCont->containerRef->refID;
		intfc->OpenRecord('FMCH', 0);
		intfc->WriteRecordData(&changeType, sizeof(changeType));

		intfc->WriteRecordData(&owningModIndex, sizeof(owningModIndex));
		intfc->WriteRecordData(&refID, sizeof(refID));
	}
}

bool ChangeMerchantContainerEntry::Load(OBSESerializationInterface* intfc)
{
	UInt8 modIndex;
	intfc->ReadRecordData(&modIndex, sizeof(modIndex));
	if (!IsModIndexValid(modIndex))
		return false;
	else
		owningModIndex = GetFixedModIndex(modIndex);

	UInt32 refID, nuRefID;
	intfc->ReadRecordData(&refID, sizeof(refID));
	if (!intfc->ResolveRefID(refID, &nuRefID))
		return false;

	TESForm* cont = LookupFormByID(nuRefID);
	if (!cont)
		return false;		//discard change entry (non-persistent reference)

	ExtraMerchantContainer* xCont = GetMerchantContainer();
	if (xCont)
	{
		TESObjectREFR* contRef = (TESObjectREFR*)Oblivion_DynamicCast(cont, 0, RTTI_TESForm, RTTI_TESObjectREFR, 0);
		xCont->containerRef = contRef;
		return true;
	}
	else
		return false;		//SetMerchantContainer cmd doesn't allow creating a new ExtraMerchantContainer so discard
}

void ChangeMerchantContainerEntry::Modify(FormChangeInfo &info)
{
	owningModIndex = info.owningModIndex;
}

ExtraTravelHorse* ChangeTravelHorseEntry::GetTravelHorse()
{
	ExtraTravelHorse* xHorse = NULL;
	BSExtraData* xData = npc->baseExtraList.GetByType(kExtraData_TravelHorse);
	if (xData)
		ExtraTravelHorse* xHorse = (ExtraTravelHorse*)Oblivion_DynamicCast(xData, 0, RTTI_BSExtraData, RTTI_ExtraTravelHorse, 0);
	
	return xHorse;
}

ChangeTravelHorseEntry::ChangeTravelHorseEntry(FormChangeInfo &info) 
	: FormChangeEntry(info), owningModIndex(info.owningModIndex)
{
	npc = (Actor*)Oblivion_DynamicCast(info.modifiedForm, 0, RTTI_TESForm, RTTI_Actor, 0);
	ExtraTravelHorse* xHorse = GetTravelHorse();
	if (xHorse)
		prevTravelHorse = xHorse->horseRef;
	else
		prevTravelHorse = NULL;
}

ChangeTravelHorseEntry::~ChangeTravelHorseEntry()
{
	ExtraTravelHorse* xHorse = GetTravelHorse();
	if (prevTravelHorse)
		xHorse->horseRef = prevTravelHorse;
	else
		npc->baseExtraList.Remove(xHorse);
}

void ChangeTravelHorseEntry::Save(OBSESerializationInterface *intfc)
{
	UInt32 refID = 0;

	ExtraTravelHorse* xHorse = GetTravelHorse();
	if (xHorse->horseRef != prevTravelHorse)
	{
		intfc->OpenRecord('FMCH', 0);
		intfc->WriteRecordData(&changeType, sizeof(changeType));
		intfc->WriteRecordData(&owningModIndex, sizeof(owningModIndex));

		if (xHorse->horseRef)
			refID = xHorse->horseRef->refID;

		intfc->WriteRecordData(&refID, sizeof(refID));
	}
}

bool ChangeTravelHorseEntry::Load(OBSESerializationInterface* intfc)
{
	UInt8 modIndex;
	intfc->ReadRecordData(&modIndex, sizeof(modIndex));
	if (!IsModIndexValid(modIndex))
		return false;
	else
		owningModIndex = GetFixedModIndex(modIndex);

	UInt32 refID;
	UInt32 nuRefID;

	intfc->ReadRecordData(&refID, sizeof(refID));
	if (!intfc->ResolveRefID(refID, &nuRefID))
		return false;

	TESForm* horse = LookupFormByID(nuRefID);
	if (!horse)
		return false;

	TESObjectREFR* horseRef = (TESObjectREFR*)Oblivion_DynamicCast(horse, 0, RTTI_TESForm, RTTI_TESObjectREFR, 0);
	ExtraTravelHorse* xHorse = GetTravelHorse();
	if (xHorse)
	{
		xHorse->horseRef = horseRef;
		return true;
	}
	else	//SetTravelHorse allows creation of new ExtraTravelHorse, so create if it doesn't exist
	{
		xHorse = ExtraTravelHorse::Create();
		xHorse->horseRef = horseRef;
		npc->baseExtraList.Add(xHorse);
		return true;
	}
}

void ChangeTravelHorseEntry::Modify(FormChangeInfo &info)
{
	owningModIndex = info.owningModIndex;
}

ChangeTrainerLevelEntry::ChangeTrainerLevelEntry(FormChangeInfo &info)
	: FormChangeEntry(info), owningModIndex(info.owningModIndex)
{
	actorBase = (TESActorBase*)Oblivion_DynamicCast(info.modifiedForm, 0, RTTI_TESForm, RTTI_TESActorBase, 0);
	prevTrainerLevel = actorBase->aiForm.trainingLevel;
}

ChangeTrainerLevelEntry::~ChangeTrainerLevelEntry()
{
	actorBase->aiForm.trainingLevel = prevTrainerLevel;
}

void ChangeTrainerLevelEntry::Modify(FormChangeInfo &info)
{
	owningModIndex = info.owningModIndex;
}

void ChangeTrainerLevelEntry::Save(OBSESerializationInterface* intfc)
{
	if (actorBase->aiForm.trainingLevel != prevTrainerLevel)
	{
		intfc->OpenRecord('FMCH', 0);
		intfc->WriteRecordData(&changeType, sizeof(changeType));
		intfc->WriteRecordData(&owningModIndex, sizeof(owningModIndex));

		intfc->WriteRecordData(&actorBase->aiForm.trainingLevel, sizeof(prevTrainerLevel));
	}
}

bool ChangeTrainerLevelEntry::Load(OBSESerializationInterface* intfc)
{
	UInt8 modIndex;
	intfc->ReadRecordData(&modIndex, sizeof(modIndex));
	if (!IsModIndexValid(modIndex))
		return false;
	else
		owningModIndex = GetFixedModIndex(modIndex);

	intfc->ReadRecordData(&actorBase->aiForm.trainingLevel, sizeof(actorBase->aiForm.trainingLevel));
	return true;
}

ChangeTrainerSkillEntry::ChangeTrainerSkillEntry(FormChangeInfo &info)
	: FormChangeEntry(info), owningModIndex(info.owningModIndex)
{
	actorBase = (TESActorBase*)Oblivion_DynamicCast(info.modifiedForm, 0, RTTI_TESForm, RTTI_TESActorBase, 0);
	prevTrainerSkill = actorBase->aiForm.trainingSkill;
}

ChangeTrainerSkillEntry::~ChangeTrainerSkillEntry()
{
	actorBase->aiForm.trainingSkill = prevTrainerSkill;
}

void ChangeTrainerSkillEntry::Modify(FormChangeInfo &info)
{
	owningModIndex = info.owningModIndex;
}

void ChangeTrainerSkillEntry::Save(OBSESerializationInterface* intfc)
{
	if (actorBase->aiForm.trainingSkill != prevTrainerSkill)
	{
		intfc->OpenRecord('FMCH', 0);
		intfc->WriteRecordData(&changeType, sizeof(changeType));
		intfc->WriteRecordData(&owningModIndex, sizeof(owningModIndex));

		intfc->WriteRecordData(&actorBase->aiForm.trainingSkill, sizeof(prevTrainerSkill));
	}
}

bool ChangeTrainerSkillEntry::Load(OBSESerializationInterface* intfc)
{
	UInt8 modIndex;
	intfc->ReadRecordData(&modIndex, sizeof(modIndex));
	if (!IsModIndexValid(modIndex))
		return false;
	else
		owningModIndex = GetFixedModIndex(modIndex);

	intfc->ReadRecordData(&actorBase->aiForm.trainingSkill, sizeof(actorBase->aiForm.trainingSkill));
	return true;
}

/*
FilePathChangeInfo::FilePathChangeInfo(UInt16 in_changeType, TESForm* in_Form, Script* owningScript, UInt8 in_pathType)
	: FormChangeInfo(in_changeType, in_Form, owningScript), pathType(in_pathType)
{
	//
}

FilePathChangeInfo::~FilePathChangeInfo()
{
	//
}

ChangeFilePathEntry::ChangeFilePathEntry(FilePathChangeInfo &info) : FormChangeEntry(info), 
	form(info.modifiedForm)
{
//	const char* tempPath = GetPath(pathType);
//	UInt32 pathLen = strlen(tempPath);
//	prevPath = new char[pathLen + 1];
//	strcpy(prevPath, tempPath);
}

ChangeFilePathEntry::~ChangeFilePathEntry()
{
//	SetPath(prevPath);
//	delete[] prevPath;
}

*/



