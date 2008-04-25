#include "ModuleManager.h"

ModuleManager::ModuleManager(void)
{
}

ModuleManager::~ModuleManager(void)
{
	for(std::list<Module *>::iterator m_iter = m_Modules.begin() ; m_iter != m_Modules.end(); m_iter++)
	{
		delete *m_iter;
	}
}
