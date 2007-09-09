//This is just to synchronize Actors. It is highly experimental and NOT completed
#define MOBSYNCH
#ifdef MOBSYNCH
#include "MobSynch.h"
bool MCbFlushCache()
{
	MCCache.erase(MCCache.begin(),MCCache.end()); // flushes it
	return true;
}
bool MCbEnterZone (UINT32 Zone)  // the MC enters a new Zone , the client cache is rebuilt
{
	MCbFlushCache();
	//build client cache here
	return true;
}
bool MCbSynchActors() //called every frame , so extremely important
{
	std::list<MCActorBuf>::iterator ActorIterator;
	std::list<MCActorBuf>::iterator EndIterator = MCCache.end();
	for(ActorIterator = MCCache.begin();ActorIterator!= EndIterator;ActorIterator++)
	{
		if((ActorIterator->Actor->posX != ActorIterator->LastStatus.PosX)
			|| (ActorIterator->Actor->posY != ActorIterator->LastStatus.PosY)
			|| (ActorIterator->Actor->posZ != ActorIterator->LastStatus.PosZ))
		{
			//Update this Actoron the net , todo

			//Update LastPos
			ActorIterator->LastStatus.PosX = ActorIterator->Actor->posX;
			ActorIterator->LastStatus.PosY = ActorIterator->Actor->posY;
			ActorIterator->LastStatus.PosZ = ActorIterator->Actor->posZ;
		}
	}
}
#endif