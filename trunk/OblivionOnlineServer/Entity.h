
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
#pragma once
#include "Entity.h"
#include "EntityManager.h"
#include "EntityUpdateManager.h"
#include <vector>
class Entity
{
private:
	short m_ActorValues[72];
	UINT32 m_Equip[MAX_EQUIPSLOTS]; // Enuf 
	float m_PosX,m_PosY,m_PosZ,m_RotX,m_RotY,m_RotZ;
	UINT32 m_RefID,m_CellID,m_Race; 
	bool m_Actor,m_GlobalSynch,m_Female;//Player : is a player , Actor: is an actor , GlobalSynch: Is important for quests and must therefore always be synched
	bool m_TriggerEvents;
	BYTE  m_Status;
	EntityManager *m_mgr;
	Entity *m_MoveEvent,*m_ActorValueEvent,*CellChangeEvent,*m_DeathEvent;
	std::string m_Name; // A std::string should not waste TOO much space 
	std::string m_Class;
public:
	Entity(EntityManager *mgr,UINT32 refID,BYTE Status,bool TriggerEvents = false,bool GlobalSynch= false,
		float posX = 0 , float posY = 0 , float posZ = 0,UINT32 CellID = 0,
		float rotX = 0 , float rotY = 0 , float rotZ = 0,short health = 0,short magicka = 0 , short fatigue = 0 ,
		bool female = false,UINT32 race = 0,std::string name = std::string(""),std::string classname = std::string(""))
	{
		m_mgr = mgr;
		m_RefID = refID;
		m_Status = Status;
		m_PosX = posX;
		m_PosY = posY;
		m_PosZ = posZ;
		m_RotX = rotX;
		m_RotY = rotY;
		m_RotZ = rotZ;
		m_CellID = CellID;
		m_GlobalSynch = true;
		m_TriggerEvents = TriggerEvents;
		m_Female = female;
		m_Race = race;
		m_Name = name;
		m_Class = classname;
		memset(m_ActorValues,0,72*sizeof(short));
		m_mgr->RegisterEntity(this);
		m_mgr->GetEventSystem();
	}

	
	inline void Move(float PosX,float PosY,float PosZ)
	{
		if(m_PosX != PosX || m_PosY != PosY || m_PosZ != PosZ)
		{
			m_PosX = PosX;
			m_PosY = PosY;
			m_PosZ = PosZ;
			m_mgr->GetUpdateMgr()->OnPositionUpdate(this);
		}
	}
	inline void SetRotation(float RotX,float RotY,float RotZ)
	{
		if(m_RotX != RotX || m_RotY != RotY || m_RotZ != RotZ)
		{
			m_RotX = RotX;
			m_RotY = RotY;
			m_RotZ = RotZ;
			m_mgr->GetUpdateMgr()->OnPositionUpdate(this);
		}
	}
	inline void MoveNRot(float PosX,float PosY,float PosZ,float RotX,float RotY,float RotZ)
	{
		Move(PosX,PosY,PosZ);
		SetRotation(RotX,RotY,RotZ);
		m_mgr->GetUpdateMgr()->OnPositionUpdate(this);
	}
	inline void ChangeCell(UINT32 CellID)
	{
		m_CellID = CellID;
		m_mgr->GetUpdateMgr()->OnCellChange(this);
	}
	inline void SetFemale(bool value)
	{
		m_Female = value;
		m_mgr->GetUpdateMgr()->OnGenderUpdate(this);
	}
	inline void SetCell(UINT32 value)
	{
		m_CellID = value;
		m_mgr->GetUpdateMgr()->OnCellChange(this);
	}
	inline void SetGlobalSynch(bool value)
	{
		m_GlobalSynch  = value; 
		//TODO: Synch object out 
	}
	inline void SetRace(UINT32 value)
	{
		m_Race = value;
		m_mgr->GetUpdateMgr()->OnRaceUpdate(this);
	}
	inline void SetEquip(BYTE slot,UINT32 value)
	{
		if(slot < MAX_EQUIPSLOTS)
			m_Equip[slot ] = value;
		m_mgr->GetUpdateMgr()->OnEquipUdate(this,slot);
	}
	inline void SetName(std::string Name)
	{
		m_Name = Name;
		m_mgr->GetUpdateMgr()->OnNameUpdate(this);
	}
	inline void SetClassName(std::string Class)
	{
		m_Class = Class;
		m_mgr->GetUpdateMgr()->OnClassUpdate(this);
	}
	inline void SetActorValue(BYTE ActorValue,short Value)
	{
		if(ActorValue < 72)
			m_ActorValues[ActorValue] = Value;		
		m_mgr->GetUpdateMgr()->OnAVUpdate(this,ActorValue);
	}
	inline std::string Name()
	{
		return m_Name;
	}
	inline std::string ClassName()
	{
		return m_Class;
	}
	inline bool Female()
	{
		return m_Female;
	}
	inline BYTE Gender()
	{
		if(m_Female)
			return 1;
		return 0;
	}
	inline UINT32 RefID()
	{
		return m_RefID;
	}
	inline UINT32 Equip(UINT32 slot)
	{
		if(slot < MAX_EQUIPSLOTS)
			return m_Equip[slot];
		else
			return 0;
	}
	inline UINT32 CellID()
	{
		return m_CellID;
	}
	inline float PosX()
	{
		return m_PosX;
	}
	inline float PosY()
	{
		return m_PosY;
	}
	inline float PosZ()
	{
		return m_PosZ;
	}
	inline float RotX()
	{
		return m_RotX;
	}
	inline float RotY()
	{
		return m_RotY;
	}
	inline float RotZ()
	{
		return m_RotZ;
	};
	inline signed short ActorValue(BYTE statid)
	{
		return m_ActorValues[statid];
	}
	inline BYTE Status()
	{
		return m_Status;
	}
	inline bool GlobalSynch()
	{
		return m_GlobalSynch;
	}
	inline UINT32 Race()
	{
		return m_Race;
	}
	~Entity(void)
	{
		m_mgr->DeRegisterEntity(this);
	}
	bool TriggerEvents;//Used for script controlling NPCs

};
