#pragma once
#ifndef _EVENTS_H
#define _EVENTS_H
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

