#pragma once

#include "CommandTable.h"

extern CommandInfo kCommandInfo_GetNumFollowers;
extern CommandInfo kCommandInfo_GetNthFollower;
extern CommandInfo kCommandInfo_GetNumDetectedActors;
extern CommandInfo kCommandInfo_GetNthDetectedActor;
extern CommandInfo kCommandInfo_SetDetectionState;

extern CommandInfo kCommandInfo_OffersWeapons;
extern CommandInfo kCommandInfo_OffersArmor;
extern CommandInfo kCommandInfo_OffersClothing;
extern CommandInfo kCommandInfo_OffersBooks;
extern CommandInfo kCommandInfo_OffersLights;
extern CommandInfo kCommandInfo_OffersIngredients;
extern CommandInfo kCommandInfo_OffersApparatus;
extern CommandInfo kCommandInfo_OffersMiscItems;
extern CommandInfo kCommandInfo_OffersMagicItems;
extern CommandInfo kCommandInfo_OffersSpells;
extern CommandInfo kCommandInfo_OffersPotions;
extern CommandInfo kCommandInfo_OffersTraining;
extern CommandInfo kCommandInfo_OffersRecharging;
extern CommandInfo kCommandInfo_OffersRepair;
extern CommandInfo kCommandInfo_OffersServicesC;
extern CommandInfo kCommandInfo_GetTrainerSkill;
extern CommandInfo kCommandInfo_GetTrainerLevel;

extern CommandInfo kCommandInfo_SetOffersWeapons;
extern CommandInfo kCommandInfo_SetOffersArmor;
extern CommandInfo kCommandInfo_SetOffersClothing;
extern CommandInfo kCommandInfo_SetOffersBooks;
extern CommandInfo kCommandInfo_SetOffersIngredients;
extern CommandInfo kCommandInfo_SetOffersSpells;
extern CommandInfo kCommandInfo_SetOffersLights;
extern CommandInfo kCommandInfo_SetOffersMiscItems;
extern CommandInfo kCommandInfo_SetOffersMagicItems;
extern CommandInfo kCommandInfo_SetOffersApparatus;
extern CommandInfo kCommandInfo_SetOffersPotions;
extern CommandInfo kCommandInfo_SetOffersTraining;
extern CommandInfo kCommandInfo_SetOffersRecharging;
extern CommandInfo kCommandInfo_SetOffersRepair;
extern CommandInfo kCommandInfo_SetOffersServicesC;
extern CommandInfo kCommandInfo_GetServicesMask;
extern CommandInfo kCommandInfo_SetTrainerSkill;
extern CommandInfo kCommandInfo_SetTrainerLevel;

extern CommandInfo kCommandInfo_GetNumPackages;
extern CommandInfo kCommandInfo_GetNthPackage;

extern CommandInfo kCommandInfo_IsBlocking;
extern CommandInfo kCommandInfo_IsAttacking;
extern CommandInfo kCommandInfo_IsRecoiling;
extern CommandInfo kCommandInfo_IsDodging;
extern CommandInfo kCommandInfo_IsStaggered;

extern CommandInfo kCommandInfo_IsMovingForward;
extern CommandInfo kCommandInfo_IsMovingBackward;
extern CommandInfo kCommandInfo_IsMovingLeft;
extern CommandInfo kCommandInfo_IsMovingRight;
extern CommandInfo kCommandInfo_IsTurningLeft;
extern CommandInfo kCommandInfo_IsTurningRight;

extern CommandInfo kCommandInfo_IsInAir;
extern CommandInfo kCommandInfo_IsJumping;
extern CommandInfo kCommandInfo_IsOnGround;
extern CommandInfo kCommandInfo_IsFlying;
extern CommandInfo kCommandInfo_GetFallTimer;

extern CommandInfo kCommandInfo_IsPowerAttacking;
extern CommandInfo kCommandInfo_IsCasting;
extern CommandInfo kCommandInfo_IsAnimGroupPlaying;
extern CommandInfo kCommandInfo_AnimPathIncludes;

extern CommandInfo kCommandInfo_GetProcessLevel;