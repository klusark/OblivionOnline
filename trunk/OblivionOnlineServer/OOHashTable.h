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
#pragma once // this saves performance when in the first line .... 
#include "GameServer.h"
#ifndef _OOHASHTABLE_H
#define _OOHASHTABLE_H

// I already define the hash here .... 
inline unsigned int Hash(UINT32 value, unsigned int size)
{
	return value % size; // This is easy and efficient. We are not dealing with strings here ....
};
class OOHashTable
{
public:
	struct HashEntry
	{
		HashEntry *next; //if there is a collision
		void *obj;
	};
	OOHashTable(unsigned int size);
	~OOHashTable(void);
	bool Insert(void * object);
	void * Find(UINT32 id);
	bool Remove(UINT32 id);
	bool Erase();
private:
	HashEntry **table;
	unsigned int tablesize;
	unsigned int elements;
};
#endif

