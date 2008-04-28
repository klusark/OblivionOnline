#include "Module.h"
#include "ModuleManager.h"
Module::Module(ModuleManager*mm, GameServer * gs,std::string Filename )
{
	//TODO: Add implementations
	InitialiseCallback callback;
	
#ifdef WIN32
	//load module
	//(sizeof(void *) == sizeof(HMODULE)); //static assert to verify the Win32 define of HMODULE
	m_data = (void *)LoadLibraryA(Filename.c_str());
	callback = (InitialiseCallback) GetProcAddress((HMODULE)m_data,"OnLoad");
#endif
	callback(gs,VERSION_SUPER,VERSION_MAJOR,VERSION_MINOR);
	mm->AddModule(this);
}
Module::~Module(void)
{
#ifdef WIN32
	FreeLibrary((HMODULE)m_data);
#endif
}
