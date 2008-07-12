#pragma once

#include "obse/GameForms.h"

// 10
struct BoundObjectListHead
{
	UInt32			boundObjectCount;	// 0
	TESBoundObject	* first;			// 4
	TESBoundObject	* last;				// 8
	UInt32			unkC;				// C
};

// 8
class TESRegionDataManager
{
public:
	TESRegionDataManager();
	~TESRegionDataManager();

	virtual void	Destructor(void) = 0;	// 0
	virtual void	Unk_1(void) = 0;
	virtual void	Unk_2(void) = 0;
	virtual void	Unk_3(void) = 0;
	virtual void	Unk_4(void) = 0;
	virtual void	Unk_5(void) = 0;
	virtual void	Unk_6(void) = 0;
	virtual void	Unk_7(void) = 0;
	virtual void	Unk_8(void) = 0;

//	void	** _vtbl;
	UInt32	unk4;
};

// 10
class TESRegionList
{
public:
	TESRegionList();
	~TESRegionList();

	virtual void	Destructor(void) = 0;	// 0
	// no other virtual fns

//	void	** _vtbl;	// 0
	UInt32	unk4;		// 4
	UInt32	unk8;		// 8
	UInt8	unkC;		// C
	UInt8	padD[3];	// D
};

// note: this list contains all esm/esp files in the Data folder, even ones that are not loaded
struct ModEntry
{
	// 41C
	struct Data
	{
		enum
		{
			kFlag_IsMaster =	1 << 0,
			kFlag_Loaded =		1 << 2
		};

		UInt32	unk000[(0x01C - 0x000) >> 2];	// 000
		char	name[0x20];						// 01C - size?
		UInt32	unk03C[(0x3DC - 0x03C) >> 2];	// 03C
		UInt32	flags;							// 3DC
		UInt32	unk3E0[(0x3F0 - 0x3E0) >> 2];	// 3E0
		UInt32	idx;							// 3F0
		UInt32	unk3F4[(0x41C - 0x3F4) >> 2];	// 3F4
	};

	Data		* data;
	ModEntry	* next;

	ModEntry * Next() const	{	return next;	}
	Data * Info() const		{	return data;	}
	bool IsLoaded()	const	{	return (data && (data->flags & Data::kFlag_Loaded)) ? true : false;	}
};

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

// ### duplication of this class definition is ugly but several offsets changed between 1.1 and 1.2

// CDC
class DataHandler
{
public:
	DataHandler();
	~DataHandler();

	BoundObjectListHead		* boundObjects;					// 000
	UInt32					unk004[(0x0BC - 0x004) >> 2];	// 004
	TESRegionList			* regionList;					// 0BC
	NiTArray <TESObjectCELL *>	cellArray;					// 0C0
	TESSkill				skills[0x15];					// 0D0
	UInt32					unk8B0[(0x8C0 - 0x8B0) >> 2];	// 8B0
	ModEntry				modList;						// 8C0
	UInt32					numLoadedMods;					// 8C8
	ModEntry::Data			* modsByID[0xFF];				// 8CC
	UInt32					unkCC8[(0xCD4 - 0xCC8) >> 2];	// CC8
	TESRegionDataManager	* regionDataManager;			// CD4
	UInt32					unkCD8;							// CD8

	const ModEntry * LookupModByName(const char * modName);
	const ModEntry ** GetActiveModList();		// returns array of modEntry* corresponding to loaded mods sorted by mod index
	UInt8 GetModIndex(const char* modName);
	UInt8 GetActiveModCount();
};

STATIC_ASSERT(sizeof(DataHandler) == 0xCDC);

#elif OBLIVION_VERSION >= OBLIVION_VERSION_1_2

// CE0
class DataHandler
{
public:
	DataHandler();
	~DataHandler();

	BoundObjectListHead		* boundObjects;					// 000
	UInt32					unk004[(0x0BC - 0x004) >> 2];	// 004
	TESRegionList			* regionList;					// 0BC
	NiTArray <TESObjectCELL *>	cellArray;					// 0C0
	UInt32					unk0D0[2];						// 0D0
	TESSkill				skills[0x15];					// 0D8
	UInt32					unk8B8[(0x8C8 - 0x8B8) >> 2];	// 8B8
	ModEntry				modList;						// 8C8
	UInt32					numLoadedMods;					// 8D0
	ModEntry::Data			* modsByID[0xFF];				// 8D4
	UInt32					unkCD0[(0xCD8 - 0xCD0) >> 2];	// CD0
	TESRegionDataManager	* regionDataManager;			// CD8
	UInt32					unkCDC;							// CDC

	const ModEntry * LookupModByName(const char * modName);
	const ModEntry ** GetActiveModList();		// returns array of modEntry* corresponding to loaded mods sorted by mod index
	UInt8 GetModIndex(const char* modName);
	UInt8 GetActiveModCount();
};

STATIC_ASSERT(sizeof(DataHandler) == 0xCE0);

#else

#error unsupported oblivion version

#endif

typedef Visitor<ModEntry, ModEntry::Data> ModEntryVisitor;

class ChangesMap;
class InteriorCellNewReferencesMap;
class ExteriorCellNewReferencesMap;
class NumericIDBufferMap;
class NiTLargeArray;

// 1C8
class ChangeHandler
{
	ChangesMap						* changesMap;			// 000
	UInt32							unk0;					// 004
	InteriorCellNewReferencesMap	* interiorCellMap;		// 008
	ExteriorCellNewReferencesMap*	exteriorCellMaps[2];	// 00C
	UInt32							unk1[2];				// 014
	void*							unk2;					// 01C
	UInt32							unk3[3];				// 020

	struct RefIDList {
		UInt32	refID;
		RefIDList* next;
	};
	RefIDList						baseFormNewList;		// 02C
	UInt32							unk4[5];				// 034
	UInt32							unk5[2];				// 048
	void*							unk6;					// 050
	UInt32							unk7;					// 054
	NumericIDBufferMap*				bufferMaps[4];			// 058
	UInt32							unk8[4];				// 068
	NiTLargeArray*					largeArrays[2];			// 078
	UInt32							unk9[83];				// 080
};

class FileFinder
{
public:
	FileFinder();
	~FileFinder();

	virtual void Unk_00(void) = 0;
	virtual UInt32 FindFile(const char* filePath, UInt32 arg1, UInt32 arg2, UInt32 arg3) = 0;	//seen (char*, 0, 0, -1)
	virtual void Unk_02(void) = 0;	
	virtual UInt32 Unk_03(const char* filePath, UInt8 arg1, UInt32 arg2) = 0; //seen (char*,0xC1, 0xFFFF)
	virtual void Unk_04(void) = 0;

	enum {
		kFileStatus_NotFound = 0,
		kFileStatus_Unpacked,
		kFileStatus_Packed
	};	//return values for FindFile(), Unk_03()

	//vtbl				00
	NiTArray<UInt32>	array;	//NiTArray@PBD@@ - Single entry containing filepath "Data\"
};

extern FileFinder** g_FileFinder;
