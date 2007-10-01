#pragma once
#include "main.h"
#include <hash_map>
//time in MS in which the MC resynchs the npcs

//MASTER CLIENT STUFF
#define MC_MOBRESYNCH 50
//Everything has MC prefix
struct MCActorBuf
{
	Actor *Actor;
	ActorStatus LastStatus; //to compare it...
};
extern std::list<MCActorBuf> MCCache;
