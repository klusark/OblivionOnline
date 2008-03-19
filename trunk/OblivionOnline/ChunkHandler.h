#pragma once
/*
Copyright 2008   Julian Bangert aka masterfreek64
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
// Client chunk handler
#include "OOFunctions.h"
class InPacket;
size_t HandleObjectChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE Status);
size_t HandlePositionChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleChatChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleCellIDChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleVersionChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleNameChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleClassChunk(InPacket *pkg, BYTE* chunkdata,size_t len,UINT32 FormID,BYTE status);
size_t HandleRaceChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleGenderChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleSkillChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleStatChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleFatigueChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleMagickaChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandleHealthChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);
size_t HandlePlayerIDChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE status);