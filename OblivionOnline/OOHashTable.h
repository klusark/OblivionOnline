/*

Copyright 2007  Julian aka masterfreek64, Joseph Pearson aka chessmaster42 and Joel Teichroeb aka bobjr777

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
//bobjr777 doesnt think we need this code
// this was an experiment , delete (mf64)



//hmmm , it does not cover collisions , and it cannot store pointers or more than 90 elements...
#pragma once
int find_slot(UINT32 formid);
char * lookup(UINT32 formid);
int set(int formid, char * value);
unsigned int hashs(UINT32 str, unsigned int len);
//I changed this for collisions
struct pair{
	UINT32 formid;
	char * value;
	size_t size;
	pair *next;
}slot[90];
int find_slot(UINT32 formid)
{
	int i;
	i = hashs(formid,90) % 90;
	// search until we either find the key, or find an empty slot.
	while (slot[i].formid && slot[i].formid != formid){
		i = (i + 1) % 90;
	}
	return i;
}
char * lookup(UINT32 formid)
{
	int i;
	i = find_slot(formid);
	if (slot[i].value)  // key is in table
		return slot[i].value;
	else                     // key is not in table
		return "not found";
}
int set(UINT32 formid, char * value)
{
	int i;
	i = find_slot(formid);
	if (slot[i].formid)
		slot[i].value = value;
	else{
         slot[i].formid = formid;
         slot[i].value = value;
	}
	return 1;
}
unsigned int hashs(UINT32 str, unsigned int len)
{
   unsigned int hash = 5381;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = ((hash << 5) + hash) + (str);
   }

   return hash;
}
