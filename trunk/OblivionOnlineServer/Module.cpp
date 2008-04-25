#include "Module.h"
#include "ModuleManager.h"
Module::Module(ModuleManager*mm, GameServer * gs,std::string Filename )
{
	//TODO: Add implementations
	int (*InitialiseCallback)(GameServer *gs,BYTE,BYTE,BYTE);
	
#ifdef WIN32
	//load module
	STATIC_ASSERT(sizeof(void *) == sizeof(HMODULE)); //static assert to verify the Win32 define of HMODULE
	m_data = (void *)LoadLibrary(Filename.c_str());
	InitialiseCallback = GetProcAddress(m_data,"OnLoad");
#endif
	InitialiseCallback(gs,VERSION_SUPER,VERSION_MAJOR,VERSION_MINOR);
	mm->AddModule(this);
}
Module::~Module(void)
{
#ifdef WIN32
	FreeLibrary(m_data);
#endif
}
