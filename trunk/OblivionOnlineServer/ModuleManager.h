#pragma once
#include "Module.h"
#include <list>
class ModuleManager
{
public:
	ModuleManager(GameServer *gs);
	~ModuleManager(void);
	void AddModule(Module *m_mod)
	{
		m_Modules.push_back(m_mod);
	}
	void LoadModule(std::string Filename)
	{
		AddModule(new Module(this,m_gs,Filename));
	}
private:
	std::list<Module *> m_Modules;
	GameServer *m_gs;
};
