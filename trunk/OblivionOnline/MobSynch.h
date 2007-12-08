#pragma once

#include "main.h"
#include <hash_map>
#include <stack>
#include <queue>
//time in MS in which the MC resynchs the npcs

//MASTER CLIENT STUFF
#define MC_MOBRESYNCH 50
//Everything has MC prefix
extern bool MCMakeMC();
extern bool MCMakePassive();
extern bool Cmd_MPSynchActors_Execute (COMMAND_ARGS);
extern bool NetHandleMobUpdate(OOPkgActorUpdate pkgBuf); // called from the packet Handler

extern  CommandInfo kMPSynchActorsCommand;
extern  CommandInfo kMPStopStackCommand;
extern  CommandInfo kMPAdvanceStackCommand;

extern bool MCStackMode;
