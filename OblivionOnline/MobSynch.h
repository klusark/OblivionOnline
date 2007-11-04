#pragma once
#include "main.h"
#include <hash_map>
//time in MS in which the MC resynchs the npcs

//MASTER CLIENT STUFF
#define MC_MOBRESYNCH 50
//Everything has MC prefix
struct MCActorBuf
{
	UINT32 RefID;
	ActorStatus LastStatus; //to compare it...
};

extern std::list<MCActorBuf> MCCache;
extern bool MCBuildCache();
extern bool MCMakePassive();
extern bool Cmd_MPPushNPC_Execute (COMMAND_ARGS);
extern bool Cmd_MPSynchActors_Execute (COMMAND_ARGS);
extern bool Cmd_MPBuildCache_Execute(COMMAND_ARGS);
extern bool NetHandleMobUpdate(OOPkgActorUpdate pkgBuf); // called from the packet Handler

extern  CommandInfo kMPPushNPCCommand ;
extern  CommandInfo kMPSynchActorsCommand;
extern  CommandInfo kMPBuildCacheCommand;
extern  CommandInfo kMPBuildPassiveCacheCommand;