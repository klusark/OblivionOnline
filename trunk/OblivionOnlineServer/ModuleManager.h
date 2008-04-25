#pragma once
extern class Module;
#include <list>
class ModuleManager
{
public:
	ModuleManager(void);
	~ModuleManager(void);
	void AddModule(Module *m_mod)
	{
		m_Modules.push_back(m_mod);
	}
private:
	std::list<Module *> m_Modules;
};
