#pragma once
#ifndef _EVENTS_H
#define _EVENTS_H
/*

Copyright 2007   Julian Bangert aka masterfreek64 and Joseph Pearson aka chessmaster42 

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
enum eEvent
{
	/* Event Name:
	ePlayerJoins
	Description :
	A player joins the server
	Param 1:
	Player ID 
	Param 2:
	Pointer to a CSTRING with the name
	*/
	ePlayerJoins,
	/* Event Name:
	ePlayerLeaves
	Description :
	A player leaves the server
	Param 1:
	Player ID 
	Param 2:
	Pointer to a CSTRING with the name
	*/
	ePlayerLeaves,
	/* Event Name:
	ePlayerDies
	Description :
	A Player dies
	Param 1:
	Player ID 
	Param 2:
	Last Attacker - not yet used
	*/
	ePlayerDies,
	/* Event Name:
	ePlayerRespawns
	Description :
	A Player dies
	Param 1:
	Player ID 
	Param 2:
	*/
	ePlayerRespawns,
	/* ****

	unused .. for calculation purposes ONLY 
	*/
	eMaxEvents
};
#endif

