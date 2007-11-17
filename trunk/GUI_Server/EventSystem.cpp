
/*

Copyright 2007   Julian Bangert aka masterfreek64

This file is part of OblivionOnline.

    OblivionOnline is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    OblivionOnline is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "EventSystem.h"


EventSystem::EventSystem(void)
{
	//initialise all EventIDs ... 
	// nothing more to do here
	for(int i = 0;i<eEvent::eMaxEvents;i++)
	{
		EventList[i].EventId = (eEvent) i;
	}
	CurrentPluginID = 0;
}

EventSystem::~EventSystem(void)
{
}

bool EventSystem::LoadPlugins()
{
	std::string  DllName;
	Plugin *TempPlugin;
	std::ifstream PluginsCfg("Plugins.cfg");
	if(PluginsCfg.is_open)
	{
		while(!PluginsCfg.eof())
		{
			PluginsCfg>>DllName;	// Trigger this Plugin here
			if(DllName.size() > 0)
			{
				TempPlugin = new Plugin(); // c++ initialises all subconstructors this way
				TempPlugin->PluginID = CurrentPluginID++; // afterwards we increase it by 1
				TempPlugin->hDLL = LoadLibrary(DllName);
				if(!TempPlugin->hDLL)
				{
					delete TempPlugin;
				}
			}
		}
	}
	else
	{
		printf("Could not open plugins.cfg");
	}
	return false;

}
bool EventSystem::TriggerEvent(eEvent evt, DWORD Param1, DWORD Param2)
{
	return false;
}
bool EventSystem::HookEvent(eEvent evt, unsigned int PluginID, bool (*bHandlerCallback)(eEvent, DWORD, DWORD))
{
	return false;
}