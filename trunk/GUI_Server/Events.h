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
	/* ****

	unused .. for calculation purposes ONLY 
	*/
	eMaxEvents
};
#endif