/*

Copyright 2007  Julian aka masterfreek64 and Joseph Pearson aka chessmaster42 

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

	Linking OblivionOnline statically or dynamically with other modules is making a combined work based
	on OblivionOnline. Thus, the terms and conditions of the GNU General Public License cover 
	the whole combination.

    In addition, as a special exception, the copyright holders of  OblivionOnline give you permission to 
	combine OblivionOnline program with free software programs or libraries that are released under
	the GNU LGPL and with code included in the standard release of Oblivion Script Extender by Ian Patterson (OBSE)
	under the OBSE license (or modified versions of such code, with unchanged license). You may copy and distribute such a system 
	following the terms of the GNU GPL for  OblivionOnline and the licenses of the other code concerned,
	provided that you include the source code of that other code when and as the GNU GPL
	requires distribution of source code.

	Note that people who make modified versions of  OblivionOnline are not obligated to grant this special exception
	for their modified versions; it is their choice whether to do so. 
	The GNU General Public License gives permission to release a modified version without this
	exception; this exception also makes it possible to release a modified version which carries 
	forward this exception.
*/
//perhaps a template. DO NOT USE stdext:::hash_map. It is such bullshit....+
//not to be used yet
#pragma once
typedef void * ObjectPointer;
typedef char * (*NameFunction) (ObjectPointer obj);
typedef void (*ReleaseFunction) (ObjectPointer obj);
typedef char *(*HashFunction) (char *Name,size_t size);
struct HashEntry
{
	HashEntry *nxt; //In case of hash collision
	void *obj;
};
class HashTable
{
public:
   HashTable(NameFunction namef,size_t ElementSize,int size , HashFunction hashf);
   int HashInsert(ObjectPointer obj);
   int HashRemove(ObjectPointer obj);
   ~HashTable(HashMemory s);
   ObjectPointer Get(char*name);
private:
	NameFunction name;
	// ReleaseFunction release; //not rly needed, we use sizeof... 
	size_t ElementSize; // when we free something
	HashFunction hash;
	unsigned int size;
	HashEntry **hashtable;
}


typedef HashTable * HashMemory;