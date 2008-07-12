#include "GameData.h"
#include <string>

class LoadedModFinder
{
	const char * m_stringToFind;

public:
	LoadedModFinder(const char * str) : m_stringToFind(str) { }

	bool Accept(ModEntry::Data* data)
	{
		return _stricmp(data->name, m_stringToFind) == 0;
	}
};

const ModEntry * DataHandler::LookupModByName(const char * modName)
{
	return ModEntryVisitor(&modList).Find(LoadedModFinder(modName));
}

const ModEntry ** DataHandler::GetActiveModList()
{
	static const ModEntry* activeModList[0x100] = { 0 };

	if (!(*activeModList))
	{
		UInt8 index = 0;
		for (ModEntry* entry = &(*g_dataHandler)->modList; entry; entry = entry->next)
		{
			if (entry->IsLoaded())
				activeModList[index++] = entry;
		}
	}

	return activeModList;
}

UInt8 DataHandler::GetModIndex(const char* modName)
{
	UInt8 modIndex = 0xFF;
	const ModEntry** activeModList = GetActiveModList();

	for (UInt8 idx = 0; idx < 0x100 && activeModList[idx] && modIndex == 0xFF; idx++)
		if (!_stricmp(activeModList[idx]->data->name, modName))
			modIndex = idx;

	return modIndex;
}

UInt8 DataHandler::GetActiveModCount()
{
	UInt8 count = 0;
	const ModEntry** activeModList = GetActiveModList();

	while (activeModList[count])
		count++;

	return count;
}

#if OBLIVION_VERSION == OBLIVION_VERSION_1_1

FileFinder** g_FileFinder = (FileFinder**)0xAEBE0C;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2

FileFinder** g_FileFinder = (FileFinder**)0xB33A04;

#elif OBLIVION_VERSION == OBLIVION_VERSION_1_2_416

FileFinder** g_FileFinder = (FileFinder**)0xB33A04;

#else

#error unsupported Oblivion version

#endif
