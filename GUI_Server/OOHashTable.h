#pragma once // this saves performance when in the first line .... 
#include "ConsoleServer.h"
/*

Copyright 2007  Julian aka masterfreek64 and Joseph Pearson aka chessmaster42 and Joel Teichroeb aka bobjr777

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
#ifndef _OOHASHTABLE_H
#define _OOHASHTABLE_H
//Masterfreek64:
//in theory we would need naming functions now ... I define these as inlines directly here ... All names are defined as UINT32 s because Oblivion has them all over the place
// Also I directly define the structure we will be hashing with. This is not THAT generic ;).
//Should we need to reuse it I can change it quickly
inline UINT32 GetName(void *obj)
{
	return *(UINT32 *)obj; // The old SelectType method. Extremely fast here
}
// I already define the hash here .... 
inline unsigned int Hash(UINT32 value, unsigned int size)
{
	return value % size; // This is easy and efficient. We are not dealing with strings here ....
}
struct OOMobHashTableEntry
{
	UINT32 ID;
	ActorStatus Status;
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
private:
	HashEntry **table;
	unsigned int tablesize;
	unsigned int elements;
};
#endif