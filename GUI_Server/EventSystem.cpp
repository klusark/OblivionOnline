#include "EventSystem.h"

EventSystem::EventSystem(void)
{
	//initialise all EventIDs ... 
	// nothing more to do here
	for(int i = 0;i<eEvent::eMaxEvents;i++)
	{
		EventList[i].EventId = (eEvent) i;
	}
}

EventSystem::~EventSystem(void)
{
}
bool EventSystem::AddPlugin()
{
	return true;
}
bool EventSystem::LoadPlugins()
{
	std::string PluginName , DllName;
	std::ifstream PluginsCfg("Plugins.cfg");
	/*
	if(Plugins_Cfg)
	{
		
	}
	else
	{
		//Do some error handling here
		printf ("Couldn't open Plugins.cfg");
	}*/
	return false;

}
bool EventSystem::TriggerEvent(eEvent evt, DWORD Param1, DWORD Param2)
{
	return false;
}