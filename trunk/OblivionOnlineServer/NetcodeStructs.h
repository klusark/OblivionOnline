#pragma once
#include "GlobalDefines.h"
// netcode structures
struct Packet
{
	char *Data; 
	bool Reliable;
};
struct 