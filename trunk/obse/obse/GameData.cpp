#include "GameData.h"
#include <string>

class LoadedModFinder
{
	const char * m_stringToFind;

public:
	LoadedModFinder(const char * str) : m_stringToFind(str) { }

	bool Accept(DataHandler::ModEntry::Data* data)
	{
		return _stricmp(data->name, m_stringToFind) == 0;
	}
};

const DataHandler::ModEntry * DataHandler::LookupModByName(const char * modName)
{
	return ModEntryVisitor(&modList).Find(LoadedModFinder(modName));
}
