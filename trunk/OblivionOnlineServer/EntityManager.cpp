/*
This file is part of OblivionOnline Server- An open source game server for the OblivionOnline mod
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
#include "EntityManager.h"
#include "EventSystem.h"
#include "Entity.h"
bool EntityManager::RegisterEntity(Entity *Entity)
{
#ifndef OO_USE_HASHMAP
	if(Entity->Status() == STATUS_PLAYER)
		m_players.insert(IDEntityPair(Entity->RefID(),Entity));
	else
		m_objects.insert(IDEntityPair(Entity->RefID(),Entity));
#else
	if(Entity->Status())
		m_players.Insert(Entity);
	else
		m_objects.Insert(Entity);
#endif	
	return true;
}
bool EntityManager::DeleteEntity(Entity *Entity)
{
	delete Entity;
	return true;
}
bool EntityManager::DeleteEntities()
{
	#ifndef OO_USE_HASHMAP

	for(std::map<UINT32,Entity *>::iterator i = m_objects.begin();i != m_objects.end();i++)
	{
		delete i->second;
	}
	m_objects.clear();
	for(std::map<UINT32,Entity *>::iterator i = m_players.begin();i != m_players.end();i++)
	{
		delete i->second;
	}
	m_players.clear();
	#else
	// TODO
	printf("SOMEBODY TOLD YOU NOT TO MESS WITH DEVELOPMENT CODE !!! BETTER LISTEN");
	#endif	
	return true;
}
bool EntityManager::DeRegisterEntity(Entity *Entity)
{

#ifndef OO_USE_HASHMAP
	if(Entity->Status())
		m_players.erase(Entity->RefID());
	else
		m_objects.erase(Entity->RefID());
#else
	if(Entity->Status())
		m_players.Remove(Entity);
	else
		m_objects.Remove(Entity);
#endif	
	return true;	
}