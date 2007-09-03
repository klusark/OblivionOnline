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
*/

#pragma once
#include "BasicServer.h"
#include "OOPackets.h"
extern bool OOPWelcome_Handler(char *Packet,short LocalPlayer);
extern bool OOPPosUpdate_Handler(char *Packet,short LocalPlayer);
extern bool OOPZone_Handler(char *Packet,short LocalPlayer);
extern bool OOPChat_Handler(char *Packet,short LocalPlayer);
extern bool OOPEvent_Handler(char *Packet,short LocalPlayer);
extern bool OOPFullStatUpdate_Handler(char *Packet,short LocalPlayer);
extern bool OOPStatUpdate_Handler(char *Packet,short LocalPlayer);
extern bool OOPEventRegister_Handler(char *Packet,short LocalPlayer);
extern bool OOPTimeUpdate_Handler(char *Packet,short LocalPlayer);