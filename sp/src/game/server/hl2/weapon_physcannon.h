//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef WEAPON_PHYSCANNON_H
#define WEAPON_PHYSCANNON_H
#ifdef _WIN32
#pragma once
#endif

#include "basehlcombatweapon.h"

class CWeaponPhysCannon;

//-----------------------------------------------------------------------------
// Do we have the super-phys gun?
//-----------------------------------------------------------------------------
bool PlayerHasMegaPhysCannon();

// force the physcannon to drop an object (if carried)
void PhysCannonForceDrop( CBaseCombatWeapon *pActiveWeapon, CBaseEntity *pOnlyIfHoldingThis );
void PhysCannonBeginUpgrade( CBaseAnimating *pAnim );

bool PlayerPickupControllerIsHoldingEntity( CBaseEntity *pPickupController, CBaseEntity *pHeldEntity );
float PlayerPickupGetHeldObjectMass( CBaseEntity *pPickupControllerEntity, IPhysicsObject *pHeldObject );
float PhysCannonGetHeldObjectMass( CBaseCombatWeapon *pActiveWeapon, IPhysicsObject *pHeldObject );

CBaseEntity *PhysCannonGetHeldEntity( CBaseCombatWeapon *pActiveWeapon );
CBaseEntity *GetPlayerHeldEntity( CBasePlayer *pPlayer );

bool PhysCannonAccountableForObject( CBaseCombatWeapon *pPhysCannon, CBaseEntity *pObject );
bool PhysCannonEntityAllowsPunts(CWeaponPhysCannon * pc, CBaseEntity *pEntity);

void PhysCannon_PuntConcussionNonVPhysics(CWeaponPhysCannon * pc, CBaseEntity *pEntity, const Vector &forward, trace_t &tr);
void PhysCannon_PuntVPhysics(CWeaponPhysCannon * pc, CBaseEntity *pEntity, const Vector &forward, trace_t &tr);
void PhysCannon_PuntRagdoll(CWeaponPhysCannon * pc, CBaseEntity *pEntity, const Vector &forward, trace_t &tr);

#endif // WEAPON_PHYSCANNON_H
