#pragma once
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
	HANDLE *hDLL;
};
struct EventHook
{
	Plugin *plugin;
	bool (*callback)(eEvent evt,DWORD Param1,DWORD Param2);
};
struct Event
{
	enum eEvent EventId; // so we can find it ...
	std::list<EventHook> Hooks;
};
class EventSystem
{
public:
	bool LoadPlugins();
	bool AddPlugin();
	bool TriggerEvent(eEvent evt,DWORD Param1,DWORD Param2);
	EventSystem(void);
	~EventSystem(void);
private:	
	Event EventList[(int)eEvent::eMaxEvents]; // this contains the events, one by one :D ... Note the there is NO eMaxEvents event ..
};
#endif
#endif
