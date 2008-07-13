#pragma once
#include "Serialization.h"
#include "GameForms.h"
#include "GameObjects.h"

// Form changes layout:
//
//	FMCS - empty chunk indicating start of form changes block
//		FMCR
//			UInt32 modified form refID
//			FMCH
//				UInt16 changeType
//				data (varies by changeType)
//			[FMCH]
//			...
//		[FMCR]
//		...
//	FMCE - empty chunk indicating end of form changes block
//				

enum {
	kChangeForm_Name,
	kChangeForm_TrainerSkill,
	kChangeForm_TrainerLevel,
	kChangeForm_MerchantContainer,
	kChangeForm_TravelHorse,
	kChangeForm_BipedModel
};

void Init_CoreSerialization_Callbacks();

//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	FormChangesMap stores data about changes to base forms and references made via OBSE commands.
//	Each change type needs an associated class derived from FormChangeEntry implementing the following
//	methods:
//		c'tor	- caches original state of form, initializes pointer to modified form
//		Save	- saves current (modified) status of form to .obse file only if change has not been reverted
//		Load	- loads changes from .obse file
//				-> if valid data, apply changes to form and return true
//				-> otherwise, return false to indicate changes should be discarded
//		Modify	- registers additional changes to a previously created change entry
//		d'tor 	- reverts any changes to modified form, cleans up local resources (virtual)
//
//	Since the data required by each change type will vary, the c'tor and Modify() methods take a reference to a
//	FormChangeInfo object (or an object derived from it) containing the change type code and any extra data. 
//	To register a change entry, pass a FormChangeInfo to FormChangesMap::Add(). This must be
//	done _BEFORE_ modifying the form so that its original state can be stored.
//	
//	Changes to non-persistent references (or with non-persistent refs as data) will be saved but
//	are currently discarded on load. This may change in future versions.
//
//	Each individual change should be saved with the mod index of the mod from which the change originated. This
//	allows us to discard changes from mods which are no longer present. ChangeFormEntries should take care to
//	keep one mod's changes isolated from another mod's.
//
//	One other important consideration is to avoid saving "temporary" changes in which a script modifies a form
//	and then reverts it back to its original state. Verifying that the form was not reverted before saving prevents
//  leftover data from persisting in the savegame. While that can be done in the Save() method it may be better 
//	handled by Modify().
//
//////////////////////////////////////////////////////////////////////////////////////////////////

class FormChangeInfo
{
	//basic container for passing data to FormChangesMap and FormChangeEntry methods
public:
	FormChangeInfo(UInt16 in_changeType, TESForm* in_Form, Script* owningScript);
	virtual ~FormChangeInfo();

	TESForm	* modifiedForm;
	UInt16	changeType;
	UInt8	owningModIndex;
};

class FormChangeEntry
{
public:
	FormChangeEntry(FormChangeInfo &info);	
	virtual ~FormChangeEntry();							//deletes alloc'd memory, reverts modified form to original state
	virtual void Modify(FormChangeInfo &info);							//modifies a previously created change entry
	virtual void Save(OBSESerializationInterface* intfc);	//writes changes to .obse file
	virtual bool Load(OBSESerializationInterface* intfc);	//loads changes from .obse file, returns false to discard changes
	static FormChangeEntry* Create(FormChangeInfo &info);	//instantiates a derived object based on in_type
	UInt8 GetFixedModIndex(UInt8 modIndex);
	bool IsModIndexValid(UInt8 modIndex);

	UInt16	changeType;
};

class FormChangesMap
{
	typedef std::multimap<UInt32, FormChangeEntry*> ChangeMap;
	typedef std::pair<UInt32, FormChangeEntry*> ChangeMapValue;

	ChangeMap changes;		//key is formID of modified TESForm

	void Clear();
public:
	FormChangesMap();
	~FormChangesMap();


	void Save(OBSESerializationInterface* intfc);
	void Load(OBSESerializationInterface* intfc);
	void Reset(OBSESerializationInterface* intfc);
	void Add(FormChangeInfo &info);
};

extern FormChangesMap g_FormChangesMap;

class ChangeNameEntry : public FormChangeEntry
{
	//UInt8  modIndex
	//UInt16 dataLength
	//char	 newName[dataLength]

	TESForm	* namedForm;
	char	* prevName;
	UInt8	owningModIndex;

	TESFullName* GetNamedForm(TESForm* form);
public:
	ChangeNameEntry(FormChangeInfo &info);
	virtual ~ChangeNameEntry();
	virtual void Modify(FormChangeInfo &info);
	virtual void Save(OBSESerializationInterface* intfc);
	virtual bool Load(OBSESerializationInterface* intfc);
};

class ChangeTrainerLevelEntry : public FormChangeEntry
{
	//UInt8	modIndex
	//UInt8	trainerLevel

	TESActorBase	* actorBase;
	UInt8			prevTrainerLevel;
	UInt8			owningModIndex;
public:
	ChangeTrainerLevelEntry(FormChangeInfo &info);
	virtual ~ChangeTrainerLevelEntry();
	virtual void Modify(FormChangeInfo &info);
	virtual void Save(OBSESerializationInterface* intfc);
	virtual bool Load(OBSESerializationInterface* intfc);
};

class ChangeTrainerSkillEntry : public FormChangeEntry
{
	//UInt8	modIndex
	//UInt8	trainerSkill

	TESActorBase	* actorBase;
	UInt8			prevTrainerSkill;
	UInt8			owningModIndex;
public:
	ChangeTrainerSkillEntry(FormChangeInfo &info);
	virtual ~ChangeTrainerSkillEntry();
	virtual void Modify(FormChangeInfo &info);
	virtual void Save(OBSESerializationInterface* intfc);
	virtual bool Load(OBSESerializationInterface* intfc);
};

class ChangeMerchantContainerEntry : public FormChangeEntry
{
	//UInt8	 modIndex
	//UInt32 containerFormID

	Actor			* merchant;
	TESObjectREFR	* prevMerchantContainer;
	UInt8			owningModIndex;

	ExtraMerchantContainer* GetMerchantContainer();
public:
	ChangeMerchantContainerEntry(FormChangeInfo &info);
	virtual ~ChangeMerchantContainerEntry();
	virtual void Modify(FormChangeInfo &info);
	virtual void Save(OBSESerializationInterface* intfc);
	virtual bool Load(OBSESerializationInterface* intfc);
};

class ChangeTravelHorseEntry : public FormChangeEntry
{
	//UInt8	 modIndex
	//UInt32 horseFormID

	Actor			* npc;
	TESObjectREFR	* prevTravelHorse;
	UInt8			owningModIndex;

	ExtraTravelHorse* GetTravelHorse();

public:
	ChangeTravelHorseEntry(FormChangeInfo &info);
	virtual ~ChangeTravelHorseEntry();
	virtual void Modify(FormChangeInfo &info);
	virtual void Save(OBSESerializationInterface* intfc);
	virtual bool Load(OBSESerializationInterface* intfc);
};

/*
class FilePathChangeInfo : public FormChangeInfo
{
public:
	enum {
		kPath_SimpleModel,
		kPath_BipedWorldModel,
		kPath_BipedFemaleModel,
		kPath_BipedMaleModel,
		kPath_SimpleIcon,
		kPath_BipedFemaleIcon,
		kPath_BipedMaleIcon,
		kPath_SoundLoop,
		kPath_SoundOpen,
		kPath_SoundClose
	};
	FilePathChangeInfo(UInt16 in_changeType, TESForm* in_Form, Script* owningScript, UInt8 in_pathType);
	virtual ~FilePathChangeInfo();

	UInt8	pathType;
};

class ChangeFilePathEntry : public FormChangeEntry
{
	//UInt8 modIndex
	//UInt8 pathType
	//UInt16 pathLength
	//char path[pathLength]

	struct FilePathData {
		const char	* prevPath;
		UInt8		owningModIndex;
		UInt8		pathType;
	};

	struct FilePathEntry {
		FilePathData	* data;
		FilePathEntry	* next;

		FilePathData * Info() const;
		FilePathEntry * Next() const;
		void Delete();
		void DeleteHead(FilePathEntry* lastNode);
		void SetNext(FilePathEntry* lastNode);
	};

	typedef Visitor<FilePathEntry, FilePathData> FilePathEntryVisitor;

	TESForm			* form;
	FilePathEntry	changeList;

	const char* GetPath(UInt8 pathType);
	void SetPath(UInt8 pathType);
public:
	ChangeFilePathEntry(FilePathChangeInfo &info);
	virtual ~ChangeFilePathEntry();
	virtual void Modify(FilePathChangeInfo &info);
	virtual void Save(OBSESerializationInterface* intfc);
	virtual bool Load(OBSESerializationInterface* intfc);
};

class FlagChangeInfo : public FormChangeInfo
{
};

class ChangeFlagsEntry : public FormChangeEntry
{
	struct BitData {
		UInt8	owningModIndex;
		bool	bitIsSet;
	};

	BitData	* bits[32];
	TESForm	* form;
public:
	ChangeFlagsEntry(FlagChangeInfo &info);
	virtual ~ChangeFlagsEntry();
	virtual void Modify(FlagChangeInfo &info);
	virtual void Save(OBSESerializationInterface* intfc);
	virtual bool Load(OBSESerializationInterface* intfc);
};
*/

