//This is just to synchronize Actors. It is highly experimental and NOT completed
#define MOBSYNCH
#ifdef MOBSYNCH
#include "MobSynch.h"
std::list<MCActorBuf> MCCache;
DWORD MobResynchTimer ; // seperate , because no seperate packet
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
bool NetSynchNPC(Actor *Actor)
{
	OOPkgActorUpdate pkgBuf;
	pkgBuf.etypeID = OOPActorUpdate;
	
	pkgBuf.fPosX = Actor->posX ; //Actor
	pkgBuf.fPosY = Actor->posY;
	pkgBuf.fPosZ = Actor->posZ;
	pkgBuf.fRotX = Actor->rotX;
	pkgBuf.fRotY = Actor->rotY;
	pkgBuf.fRotZ = Actor->rotZ;
	pkgBuf.Health = Actor->GetActorValue(8);
	pkgBuf.Magika = Actor->GetActorValue(9);
	pkgBuf.Fatigue = Actor->GetActorValue(10);
	if(Actor->parentCell->worldSpace)
	{
				pkgBuf.Flags = 2|4; //Exterior
				pkgBuf.CellID = Actor->parentCell->worldSpace->refID;
	}
	else
	{
				pkgBuf.Flags = 2; //Interior
				pkgBuf.CellID = Actor->parentCell->refID;
	}
	pkgBuf.refID = Actor->refID;
	send(ServerSocket,(char *)&pkgBuf,sizeof(OOPkgActorUpdate),0);
	return true;
}
bool MCbSynchActors() //called every frame , so extremely important
{
	DWORD tickBuf;
	tickBuf=GetTickCount();
	if((tickBuf - MobResynchTimer) > MC_MOBRESYNCH) //just synch every 50 ms 
	{
	std::list<MCActorBuf>::iterator ActorIterator;
	std::list<MCActorBuf>::iterator EndIterator = MCCache.end();
	for(ActorIterator = MCCache.begin();ActorIterator!= EndIterator;ActorIterator++)
	{
		if((ActorIterator->Actor->posX != ActorIterator->LastStatus.PosX)
			|| (ActorIterator->Actor->posY != ActorIterator->LastStatus.PosY)
			|| (ActorIterator->Actor->posZ != ActorIterator->LastStatus.PosZ))
		{
			//Update this Actor on the net 
			NetSynchNPC(ActorIterator->Actor);
			//Update LastPos
			ActorIterator->LastStatus.PosX = ActorIterator->Actor->posX;
			ActorIterator->LastStatus.PosY = ActorIterator->Actor->posY;
			ActorIterator->LastStatus.PosZ = ActorIterator->Actor->posZ;
		}
	}
	MobResynchTimer = tickBuf;
	}
}
#endif