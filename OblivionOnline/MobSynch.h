#pragma once
#include "main.h"

//MASTER CLIENT STUFF
//Everything has MC prefix
struct MCActorBuf
{
	Actor *Actor;
	ActorStatus LastStatus; //to compare it...
};
std::list<MCActorBuf> MCCache;
