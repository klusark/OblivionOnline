#include "OOHashTable.h"
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
OOHashTable::OOHashTable(unsigned int size)
{
	table = (HashEntry **)calloc(size,sizeof(HashEntry *));
	tablesize = size;
}

OOHashTable::~OOHashTable(void)
{
	//here we have to free all elements
	unsigned int i;
	HashEntry *entry;
	for(i = 0; i < tablesize;i++)
	{
		while(entry = table[i])
		{
			table[i] = entry->next;
			free(entry->obj); // we need to free it somehow else as well...
			free(entry);
		}
	}
	free(table);
	//Auto cleanup fixes the rest
}

bool OOHashTable::Insert(void * object)
{
	unsigned int hash;
	UINT32 id;
	HashEntry **prev, *newelement;

	id = GetName(object);
	hash = Hash(id,tablesize);
	//find a memory location
	for(prev = table + hash;*prev;prev = &((*prev)->next)) //We advance thorugh the list...
	{
		if(id == GetName((*prev)->obj))
		{
			return false; // Not a unique hash ....
		}
		else if(id < GetName((*prev)->obj))
		{
			break; // we found a location 
		}
	}
	// We allocate the memory
	newelement = (HashEntry *)malloc(sizeof(HashEntry));
	newelement->next = *prev;
	newelement->obj = object;
	*prev = newelement;// we insert ourselves into the chains
	elements++;
	return true;
}

void * OOHashTable::Find(UINT32 id)
{
	unsigned int hash;
	HashEntry *entry;
	hash = Hash(id,tablesize);
	for(entry = table[hash];entry; entry = entry->next) //we go through the list
	{
		if(GetName(entry->obj) == id)
		{
			return entry->obj;
		}
	}
	return NULL;
}

bool OOHashTable::Remove(UINT32 id)
{
	// this is not yet necessary, as we cannot detect that
	// I will implement it at the given time
	unsigned int hash;
	HashEntry *entry, *prev;
	hash = Hash(id,tablesize);
	entry = table[hash];
	if(GetName(entry->obj) == id) // no collision , we do not need to fix it ....
	{
		free(entry->obj);
		free(entry);
		return true;
	}
	
	prev = entry;
	for(;entry; entry = entry->next) //we go through the list
	{
		if(GetName(entry->obj)==id)
		{
			if(entry->next)
			{
				prev->next = entry->next; // We fix the pointer
			}
			else
			{
				prev->next = NULL;
			}
			free(entry->obj);
			free(entry);
			return true;
		}
		prev = entry;
	}
	return false;
}
