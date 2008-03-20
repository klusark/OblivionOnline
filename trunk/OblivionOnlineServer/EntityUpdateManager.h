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


You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
class NetworkSystem;
class Entity;
class EntityManager;
class GameServer;
class EntityUpdateManager
{
private:
	EntityManager *m_mgr;
	NetworkSystem *m_net;
public:
	EntityUpdateManager(EntityManager *mgr,NetworkSystem *netsys)
	{
		m_mgr = mgr;
		m_net = netsys;
	};
	void OnPositionUpdate(Entity *ent);//Triggers Events and network code;
	void OnHealthUpdate(Entity *ent);
	void OnFatigueUpdate(Entity *ent);
	void OnMagickaUpdate(Entity *ent);
	void GlobalSend(Entity *ent);
	void OnNameUpdate(Entity *ent);
	void OnEquipUdate(Entity *ent,unsigned char slot);
	void OnClassUpdate(Entity *ent);
	void OnCellChange(Entity *ent);
	void OnRaceUpdate(Entity *ent);
	void OnGenderUpdate(Entity *ent);
};