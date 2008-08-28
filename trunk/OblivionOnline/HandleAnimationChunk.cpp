/*
Copyright 2008   Julian Bangert aka masterfreek64
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
#include "InPacket.h"
#include "ChunkHandler.h"
#include "Entity.h"
string GetAnimationGroupScriptName(BYTE AnimationGroup)
{
	switch(AnimationGroup)
	{
	case 0:   return "Idle";
			case 1:    return "DynamicIdle";
			case 2:    return "SpecialIdle";
			case 3:    return "Forward";
			case 4:    return "Backward";
			case 5:    return "Left";
			case 6:    return "Right";
			case 7:    return "FastForward";
			case 8:    return "FastBackward";
			case 9:    return "FastLeft";
			case 10:   return "FastRight";
			case 11:   return "DodgeForward";
			case 12:   return "DodgeBack";
			case 13:   return "DodgeLeft";
			case 14:   return "DodgeRight";
			case 15:   return "TurnLeft";
			case 16:   return "TurnRight";
			case 17:   return "Equip";
			case 18:   return "Unequip";
			case 19:   return "AttackBow";
			case 20:   return "AttackLeft";
			case 21:   return "AttackRight";
			case 22:   return "AttackPower";
			case 23:   return "AttackForwardPower";
			case 24:   return "AttackBackPower";
			case 25:   return "AttackLeftPower";
			case 26:   return "AttackRightPower";
			case 27:   return "BlockIdle";
			case 28:   return "BlockHit";
			case 29:   return "BlockAttack";
			case 30:   return "Recoil";
			case 31:   return "Stagger";
			case 32:   return "Death";
			case 33:   return "TorchIdle";
			case 34:   return "CastSelf";
			case 35:   return "CastTouch";
			case 36:   return "CastTarget";
			case 37:   return "CastSelfAlt";
			case 38:   return "CastTouchAlt";
			case 39:   return "CastTargetAlt";
			case 40:   return "JumpStart";
			case 41:   return "JumpLoop";
			case 42:   return "JumpLand";
	}
}
void PlayGroup(UInt32 FormID,BYTE groupID)
{
	string Command;
	Command += FormID;
	Command += ".PlayGroup ";
	Command += GetAnimationGroupScriptName(groupID);
	Command += " 1";
	RunScriptLine(Command.c_str(),true);
}
size_t HandleAnimationChunk(InPacket *pkg, BYTE* chunkdata,size_t len ,UINT32 FormID,BYTE Status)
{ 
	TESObjectREFR *obj;
	BYTE AnimationID,Value;
	AnimationID = *(chunkdata + 2);
	Value     = *(chunkdata + 3);
	Entity * ent = Entities.GetEntity(FormID);
	if(ent == NULL)
		ent = new Entity(FormID);
	obj = (TESObjectREFR *)LookupFormByID(FormID);
	if(Value == 0)
	{
		PlayGroup(FormID,0);
	}
	if(AnimationID < 40 ) // Jump Animations are not synchronised -> They are in the game
	{
		PlayGroup(FormID,AnimationID);
	}
	return GetMinChunkSize(Animation) + sizeof(unsigned short);
}