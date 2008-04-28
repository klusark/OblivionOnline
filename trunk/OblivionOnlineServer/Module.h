#pragma once
#include "GlobalDefines.h"
#ifdef WIN32
#include <Windows.h>
#endif
extern class GameServer;
extern class ModuleManager;
typedef int(*InitialiseCallback) (GameServer *gs,BYTE,BYTE,BYTE);
class Module
{
public:
	Module(ModuleManager *mm,GameServer * gs,std::string Filename); // Module has to contain an exported procedure called "OnLoad"
	~Module(void);
private:
	void *m_data; // implementation specific - WIN32:  Hmodule is a void* ...
};
 
