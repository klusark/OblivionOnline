#pragma once

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
#ifndef _EVENTSYSTEM_H
#define _EVENTSYSTEM_H

#ifndef SERVER_NO_EVENTS
#include "Windows.h"
#include <deque>
#include <list>
#include "Events.h"
#include <fstream>
struct Plugin
{
	unsigned int PluginID;
	HANDLE *hDLL;
	std::list<EventHook *> hooks;
};
struct EventHook
{
	Plugin *plugin;
	bool (*callback)(eEvent evt,DWORD Param1,DWORD Param2);
};
struct Event
{
	enum eEvent EventId; // so we can find it ...
	std::list<EventHook *> Hooks; // we store pointers and manage the memory ourselves
};
class EventSystem
{
public:
	bool LoadPlugins();
	bool AddPlugin();
	bool TriggerEvent(eEvent evt,DWORD Param1,DWORD Param2);
	//bHandlerCallback(eEvent evt,DWORD Param1,DWORD Param2);
	bool HookEvent(eEvent evt,unsigned int PluginID,bool (*bHandlerCallback)(eEvent,DWORD,DWORD,unsigned int PluginID));
	EventSystem(void);
	~EventSystem(void);
private:	
	Event EventList[(int)eEvent::eMaxEvents]; // this contains the events, one by one :D ... Note the there is NO eMaxEvents event ..
	std::list<Plugin *> PluginList;
};
#endif
#endif
