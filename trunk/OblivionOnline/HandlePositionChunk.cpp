/*

Copyright(c) 2007-2008   Julian Bangert aka masterfreek64
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
#include "InPacket.h"
#include "ChunkHandler.h"
#include "Entity.h"
size_t HandlePositionChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE Status)
{
	Entity *ent;
	float *floats = (float *)(chunkdata + 2);
	_MESSAGE("FormID %u status %uh Position: %f %f %f %f %f %f",floats[0],floats[1],floats[2],floats[3],floats[4],floats[5]);
	if(Status == STATUS_PLAYER)
		FormID = GetPlayerFormID(FormID); // Get a player representation
	ent = Entities.GetEntity(FormID);
	if(ent == NULL)
		ent = new Entity(FormID);
	ent->PosX = floats[0];
	ent->PosY = floats[1];
	ent->PosZ = floats[2];
	ent->RotX = floats[3];
	ent->RotY = floats[4];
	ent->RotZ = floats[5];
	TESObjectREFR *obj = (TESObjectREFR *)LookupFormByID(FormID);
	if(obj != NULL)
		SafeAddUpdateQueue(ent);
	return GetMinChunkSize(Position) + sizeof(unsigned short);
}