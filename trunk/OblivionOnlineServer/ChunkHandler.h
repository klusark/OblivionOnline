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
#include "GlobalDefines.h"
#include "Packets.h"
class InPacket;
class GameServer;
size_t HandleObjectChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE Status);
size_t HandlePositionChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleChatChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleCellIDChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleVersionChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleNameChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleClassChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len,UINT32 FormID,BYTE status);
size_t HandleRaceChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleGenderChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleSkillChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleStatChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleFatigueChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleMagickaChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleHealthChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleEquipChunk(GameServer *gs,InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);