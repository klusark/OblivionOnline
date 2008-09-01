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


You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Gameserver.h"
#include "ChunkHandler.h"
#include "Entity.h"
#include "NetworkSystem.h"
#include "InPacket.h"
size_t HandleChatChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status)
{
	size_t retval;
	UINT16 supposedlength = *(unsigned short *)(chunkdata + 2);
	Entity *ent = gs->GetEntities()->GetEntity(status,FormID);
	std::string message = ent->Name();
	if(supposedlength > len - 4)
		retval = len;
	else
		retval = supposedlength;
	message.append(" :");
	message.append((char *)(chunkdata + 4),retval);
	BYTE *buf = (BYTE *)malloc(message.length() + 2);
	*(UINT16 *) buf = retval;
	memcpy(buf + 2,message.c_str(),message.length());
	gs->GetIO()<<PlayerChat<<ent->Name()<<" :"<<message<<endl;
	for(map<UINT32,Entity *>::const_iterator i =  gs->GetEntities()->GetPlayerList().begin(); i != gs->GetEntities()->GetPlayerList().end() ; i++)
	{
		gs->GetNetwork()->SendChunk(i->second->RefID(),FormID,status,message.length() + 2,PkgChunk::Chat,buf);		
	}
	free(buf);
	return retval + sizeof(unsigned short);
}
size_t HandleVersionChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status)
{
	if(*(chunkdata + 2) == VERSION_SUPER && *(chunkdata+3) == VERSION_MAJOR && *(chunkdata+4) == VERSION_MINOR )
		gs->GetIO()<<SystemMessage<<"Client "<<FormID <<" authenticated with the correct version" << endl;
	else
	{
		//TODO: kick him
		gs->GetIO()<<SystemMessage<<"Client "<<FormID <<" tried to authenticate with an incorrect version:"<<*chunkdata<<*(chunkdata+1)<<*(chunkdata+2)<< endl;
	}
	return GetMinChunkSize(PkgChunk::Version) + sizeof(unsigned short);
}
