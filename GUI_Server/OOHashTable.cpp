#include "OOHashTable.h"

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
#pragma warning (" OOHashTable::Remove not yet implemented. Masterfreek64 truly is a lazy fellow :) ");
	return false;
}