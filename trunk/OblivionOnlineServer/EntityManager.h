/*
OblivionOnline Server- An open source game server for the OblivionOnline mod
Copyright (C)  2008   Julian Bangert

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.
*/
#pragma once
#include "GlobalDefines.h"
#ifndef OO_USE_HASHMAP	//Slower and more secure 
#include <map>
#else
#endif
class Entity;
class EventSystem;
typedef std::pair<UINT32,Entity *> IDEntityPair;
class EntityManager
{	
private:
#ifndef OO_USE_HASHMAP	
	std::map<UINT32,Entity *> m_objects; //Includes actors
	std::map<UINT32,Entity *> m_players;
#else
	OOHashTable m_objects
	OOHashTable m_players;

#endif
	EventSystem *m_evt;
public:
	inline const std::map<UINT32,Entity *>& GetPlayerList() // TODO : evaluate if this is necessary
	{
		return m_players;
	}
	EntityManager(EventSystem *evt)
	{
		m_evt = evt;
	}
	~EntityManager(void)
	{
		DeleteEntities();
	}
	bool RegisterEntity(Entity *Entity);
	bool DeleteEntity(Entity *Entity);
	bool DeRegisterEntity(Entity *Entity);
	bool DeleteEntities();
	inline Entity * GetEntity(bool IsPlayer,UINT32 RefID)
	{
		std::map<UINT32,Entity *>::iterator iter;
#ifndef OO_USE_HASHMAP
	if(IsPlayer)
	{
		iter =  m_players.find(RefID);
		if(iter != m_players.end())
			return iter->second;
	}
	else
	{
		iter =  m_objects.find(RefID);
		if(iter != m_objects.end())
			return iter->second;
	}
	return NULL;
#else
	if(IsPlayer)
		return m_players.Find(RefID);
	else
		return m_objects.Find(RefID);
#endif
	}
	inline EventSystem *GetEventSystem()
	{
		return m_evt;
	}
};

