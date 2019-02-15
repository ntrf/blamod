
#include "cbase.h"
#include "ammodef.h"
#include "../shared/hl2/basehlcombatweapon_shared.h"

#include "../shared/hl2/hl2_player_shared.h"

#include "../blamod/blamodvar.h"

#include "../shared/beam_shared.h"

#include "soundenvelope.h"

#ifndef CLIENT_DLL
#include "../server/hl2/cbasehelicopter.h"
#endif

#if defined( CLIENT_DLL )
#define CWeaponBrGauss C_WeaponBrGauss
#endif

#define GAUSS_BEAM_SPRITE "sprites/laserbeam.vmt"

BlaConVar gauss_maxpunch("blamod_gauss_maxpass", "300", FCVAR_REPLICATED | FCVAR_HIDDEN);
BlaConVar gauss_damage("blamod_gauss_damagemod", "200.0", FCVAR_REPLICATED | FCVAR_HIDDEN);
BlaConVar gauss_knockback("blamod_gauss_knockback", "6.0", FCVAR_REPLICATED | FCVAR_HIDDEN);
BlaConVar gauss_shot_damage("blamod_gauss_damage", "20.0", FCVAR_REPLICATED | FCVAR_HIDDEN);

class CWeaponBrGauss : public CBaseHLCombatWeapon
{
public:
	DECLARE_CLASS(CWeaponBrGauss, CBaseHLCombatWeapon);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CWeaponBrGauss();

	void PrimaryAttack() override;
	void SecondaryAttack() override;
	virtual bool Reload();
	virtual void WeaponIdle();

#ifdef GAME_DLL
	void SendReloadEvents();
#endif

	bool PlayEmptySound();
	CHL2_Player* GetPlayerOwner() const;

private:

	CWeaponBrGauss(const CWeaponBrGauss &);

	// original

	float GetFullChargeTime(void);

	void Precache() override;
	void Spawn() override;

	bool Deploy() override;
	bool Holster(CBaseCombatWeapon *pSwitchingTo) override;

	void StartFire();
	void Fire(Vector vecOrigSrc, Vector vecDir, float flDamage);

	void DrawBeam(const Vector &startPos, const Vector &endPos, float width, bool attachMuzzle, bool primary);

	void StopChargeSound(void);


	float m_flStartCharge = 0;
	float m_flEndCharge = 0;
	float m_flNextAmmoBurn = 0;
	int m_fPrimaryFire = 0;
	int m_nBulletType = 0;

	int m_fInAttack = 0;

	CSoundPatch * spin_sound = nullptr;
	CSoundPatch * fire_sound = nullptr;

	HSOUNDSCRIPTHANDLE spin_sound_handle = 0;
};

IMPLEMENT_NETWORKCLASS_ALIASED(WeaponBrGauss, DT_WeaponBrGauss)

BEGIN_NETWORK_TABLE(CWeaponBrGauss, DT_WeaponBrGauss)
/*
#ifdef CLIENT_DLL
RecvPropInt( RECVINFO( m_fInSpecialReload ) )
#else
SendPropInt( SENDINFO( m_fInSpecialReload ), 2, SPROP_UNSIGNED )
#endif
*/
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CWeaponBrGauss)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(weapon_brgauss, CWeaponBrGauss);
PRECACHE_WEAPON_REGISTER(weapon_brgauss);


bool CWeaponBrGauss::PlayEmptySound()
{
	CPASAttenuationFilter filter(this);
	filter.UsePredictionRules();

	EmitSound(filter, entindex(), "Default.ClipEmpty_Rifle");

	return 0;
}

CHL2_Player* CWeaponBrGauss::GetPlayerOwner() const
{
	return dynamic_cast< CHL2_Player* >(GetOwner());
}

CWeaponBrGauss::CWeaponBrGauss()
{ }

bool CWeaponBrGauss::Reload()
{
	return false;
}

//---------------------------------------------
//  Original

enum gauss_e
{
	GAUSS_IDLE = 0,
	GAUSS_IDLE2,
	GAUSS_FIDGET,
	GAUSS_SPINUP,
	GAUSS_SPIN,
	GAUSS_FIRE,
	GAUSS_FIRE2,
	GAUSS_HOLSTER,
	GAUSS_DRAW
};

float CWeaponBrGauss::GetFullChargeTime(void)
{
	if (g_pGameRules->IsMultiplayer())
	{
		return 1.5f;
	}

	return 2.0f;
}

#ifdef CLIENT_DLL
int g_irunninggausspred;
#endif

const int GAUSS_DEFAULT_GIVE = 20;

void CWeaponBrGauss::Spawn()
{
	CBaseHLCombatWeapon::Spawn();

	//### move this to some other place
	//### otherwise loading ignores this function
	m_nBulletType = GetAmmoDef()->Index("GaussEnergy");

	//this->SetModel("models/w_gauss.mdl");
	//this->SetPrimaryAmmoCount(GAUSS_DEFAULT_GIVE);
}

void CWeaponBrGauss::Precache(void)
{
	CBaseCombatWeapon::Precache();

	//PrecacheModel("models/w_gauss.mdl");
	//PrecacheModel("models/v_gauss.mdl");
	//PrecacheModel("models/p_gauss.mdl");

	//	PrecacheSound("items/9mmclip1.wav");

	PrecacheScriptSound("PropJeep.FireCannon");
	PrecacheScriptSound("PropJeep.FireChargedCannon");
	spin_sound_handle = PrecacheScriptSound("Jeep.GaussCharge");

	PrecacheSound("ambient/energy/zap8.wav");
	//PrecacheSound("weapons/electro5.wav");
	//PrecacheSound("weapons/electro6.wav");
	//PrecacheSound("ambience/pulsemachine.wav");

	//m_iGlow = PRECACHE_MODEL("sprites/hotglow.spr");
	//m_iBalls = PRECACHE_MODEL("sprites/hotglow.spr");
	//m_iBeam = PRECACHE_MODEL("sprites/smoke.spr");

	PrecacheModel(GAUSS_BEAM_SPRITE);
}

bool CWeaponBrGauss::Deploy()
{
	auto player = GetPlayerOwner();

	//player->m_flPlayAftershock = 0.0;
	return CBaseHLCombatWeapon::Deploy();
}

bool CWeaponBrGauss::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	//	PLAYBACK_EVENT_FULL(FEV_RELIABLE | FEV_GLOBAL, m_pPlayer->edict(), m_usGaussFire, 0.01, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, 0.0, 0.0, 0, 0, 0, 1);

	GetPlayerOwner()->m_flNextAttack = gpGlobals->curtime + 0.5;

	SendWeaponAnim(ACT_VM_HOLSTER);
	
	// make sure spinning sound is stopped
	StopChargeSound();

	m_fInAttack = 0;

	return true;
}

void CWeaponBrGauss::PrimaryAttack()
{
	auto player = GetPlayerOwner();
	if (!player) return; // rip player

	// don't fire underwater
	if (player->GetWaterLevel() == 3) {
		PlayEmptySound();
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + 0.15f;
		return;
	}

	// Out of ammo?
	// this gun needs two cells to shoot
	if (player->GetAmmoCount(GetPrimaryAmmoType()) < 2) {
		PlayEmptySound();
		player->m_flNextAttack = gpGlobals->curtime + 0.5;
		return;
	}

	//player->m_iWeaponVolume = GAUSS_PRIMARY_FIRE_VOLUME;
	m_fPrimaryFire = true;

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);

	player->RemoveAmmo(2, GetPrimaryAmmoType());
	StartFire();

	if (player->GetAmmoCount(GetPrimaryAmmoType()) <= 0) {
		// HEV suit - indicate out of ammo condition
		player->SetSuitUpdate("!HEV_AMO0", false, 0);
	}

	player->m_flNextAttack = gpGlobals->curtime + 0.2;

	m_flNextPrimaryAttack = gpGlobals->curtime + 0.2;
	m_flNextSecondaryAttack = gpGlobals->curtime + 0.2;
	SetWeaponIdleTime(gpGlobals->curtime + 0.2);

	/*
	// Update punch angles.
	QAngle angle = pPlayer->GetPunchAngle();

	if ( pPlayer->GetFlags() & FL_ONGROUND )
	{
	angle.x -= SharedRandomInt( "ShotgunPunchAngleGround", 4, 6 );
	}
	else
	{
	angle.x -= SharedRandomInt( "ShotgunPunchAngleAir", 8, 11 );
	}

	pPlayer->SetPunchAngle( angle );
	*/
}

void CWeaponBrGauss::SecondaryAttack()
{
	auto player = GetPlayerOwner();
	if (!player) return; // rip player

	// don't fire underwater
	if (player->GetWaterLevel() == 3) {
		if (m_fInAttack != 0) {
			// make sure sound is stopped
			StopChargeSound();
			//EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0, 0x3f));
			SendWeaponAnim(ACT_VM_IDLE);
			m_fInAttack = 0;
		} else {
			PlayEmptySound();
		}

		m_flNextSecondaryAttack = m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;
		return;
	}

	if (m_fInAttack == 0) {
		if (player->GetAmmoCount(GetPrimaryAmmoType()) <= 0) {
			//EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8, ATTN_NORM);
			PlayEmptySound();
			player->m_flNextAttack = gpGlobals->curtime + 0.5f;
			return;
		}

		m_fPrimaryFire = false;

		// take one ammo just to start the spin
		player->RemoveAmmo(1, GetPrimaryAmmoType());
		m_flNextAmmoBurn = gpGlobals->curtime;

		// spin up
		//m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;

		SendWeaponAnim(ACT_VM_SECONDARYATTACK);
		m_fInAttack = 1;
		m_flTimeWeaponIdle = gpGlobals->curtime + 0.5f;
		m_flStartCharge = gpGlobals->curtime;
		m_flEndCharge = gpGlobals->curtime + GetFullChargeTime();

		if (spin_sound == nullptr) {
			CPASAttenuationFilter sound_filter(this);
			spin_sound = (CSoundEnvelopeController::GetController()).SoundCreate(sound_filter, entindex(), CHAN_WEAPON, "Jeep.GaussCharge", ATTN_NORM);
		}

		if (spin_sound != nullptr) {
			(CSoundEnvelopeController::GetController()).Play(spin_sound, 1.0f, 50);
			(CSoundEnvelopeController::GetController()).SoundChangePitch(spin_sound, 110, 0.0f);
		}

		//PLAYBACK_EVENT_FULL(FEV_NOTHOST, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 110, 0, 0, 0);

		//m_iSoundState = SND_CHANGE_PITCH;
	} else if (m_fInAttack == 1) {
		if (m_flTimeWeaponIdle < gpGlobals->curtime) {
			m_flNextSecondaryAttack = gpGlobals->curtime;
			SendWeaponAnim(ACT_VM_RELOAD);
			m_flTimeWeaponIdle = gpGlobals->curtime;
			m_fInAttack = 2;
		}
	} else {
		// during the charging process, eat one bit of ammo every once in a while
		if (gpGlobals->curtime >= m_flNextAmmoBurn && gpGlobals->curtime < m_flEndCharge) {
			player->RemoveAmmo(1, GetPrimaryAmmoType());
			m_flNextAmmoBurn = gpGlobals->curtime + ((g_pGameRules->IsMultiplayer()) ? 0.1f : 0.3f);
		}

		if (player->GetAmmoCount(GetPrimaryAmmoType()) <= 0) {
			// out of ammo! force the gun to fire
			StartFire();
			m_fInAttack = 0;
			m_flTimeWeaponIdle = gpGlobals->curtime + 1.0f;
			player->m_flNextAttack = gpGlobals->curtime + 1.0f;
			return;
		}

		m_flNextSecondaryAttack = gpGlobals->curtime;

		int pitch = (int)((gpGlobals->curtime - m_flStartCharge) * (150.0f / GetFullChargeTime())) + 100;
		if (pitch > 250)
			pitch = 250;

		// ALERT( at_console, "%d %d %d\n", m_fInAttack, m_iSoundState, pitch );

		//if (m_iSoundState == 0)
		//	ALERT(at_console, "sound state %d\n", m_iSoundState);

		//PLAYBACK_EVENT_FULL(FEV_NOTHOST, m_pPlayer->edict(), m_usGaussSpin, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, pitch, 0, (m_iSoundState == SND_CHANGE_PITCH) ? 1 : 0, 0);

		if (spin_sound != nullptr)
			(CSoundEnvelopeController::GetController()).SoundChangePitch(spin_sound, pitch, 0.0f);

		//m_iSoundState = SND_CHANGE_PITCH; // hack for going through level transitions

		//m_pPlayer->m_iWeaponVolume = GAUSS_PRIMARY_CHARGE_VOLUME;

		// m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.1;
		if (10.0f < gpGlobals->curtime - m_flStartCharge) {
			// Player charged up too long. Zap him.

			player->SetSuitUpdate("!HEV_SHOCK", false, 0);

			EmitSound("ambient/energy/zap8.wav");
			//EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 80 + RANDOM_LONG(0, 0x3f));
			//EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/electro6.wav", 1.0, ATTN_NORM, 0, 75 + RANDOM_LONG(0, 0x3f));

			m_fInAttack = 0;
			m_flTimeWeaponIdle = gpGlobals->curtime + 1.0f;
			player->m_flNextAttack = gpGlobals->curtime + 1.0f;

#ifndef CLIENT_DLL
			player->TakeDamage(CTakeDamageInfo(nullptr, nullptr, 50, DMG_SHOCK));
			UTIL_ScreenFade(player, color32{ 255, 128, 0, 128 }, 1.5f, 0.5f, FFADE_IN);
#endif
			SendWeaponAnim(ACT_VM_IDLE);

			// make sure sound is stopped
			StopChargeSound();

			// Player may have been killed and this weapon dropped, don't execute any more code after this!
			return;
		}
	}
}

void CWeaponBrGauss::StopChargeSound(void)
{
	if (spin_sound != nullptr) {
		(CSoundEnvelopeController::GetController()).SoundFadeOut(spin_sound, 0.1f);
	}

/*	auto player = GetPlayerOwner();
	if (player) {
		player->RumbleEffect(RUMBLE_FLAT_LEFT, 0, RUMBLE_FLAG_STOP);
	}*/
}

//=========================================================
// StartFire- since all of this code has to run and then 
// call Fire(), it was easier at this point to rip it out 
// of weaponidle() and make its own function then to try to
// merge this into Fire(), which has some identical variable names 
//=========================================================
void CWeaponBrGauss::StartFire()
{
	auto player = GetPlayerOwner();

	Vector vecAiming;
	player->EyeVectors(&vecAiming);

	float damage;

	Vector vecSrc = player->Weapon_ShootPosition();

	float chargemod = (gpGlobals->curtime - m_flStartCharge) / GetFullChargeTime();

	if (chargemod > 1.0f) {
		chargemod = 1.0f;
	}
	damage = gauss_damage.GetFloat() * chargemod;

	if (m_fPrimaryFire) {
		// fixed damage on primary attack
#ifdef CLIENT_DLL
		damage = 20.0f;
#else 
		damage = gauss_shot_damage.GetFloat();
#endif
	}

	if (m_fInAttack != 3) {
		//ALERT ( at_console, "Time:%f Damage:%f\n", gpGlobals->time - m_pPlayer->m_flStartCharge, flDamage );

#ifndef CLIENT_DLL
		if (!m_fPrimaryFire) {
			Vector velocity = player->GetAbsVelocity();
			velocity -= vecAiming * damage * gauss_knockback.GetFloat();

			// in deathmatch, gauss can pop you up into the air. Not in single play.
			if (!g_pGameRules->IsMultiplayer())
				velocity.z = 0;

			player->SetAbsVelocity(velocity);
		}
#endif
		// player "shoot" animation
		player->SetAnimation(PLAYER_ATTACK1);
	}

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);

	// time until aftershock 'static discharge' sound
	//player->m_flPlayAftershock = gpGlobals->time + UTIL_SharedRandomFloat(player->random_seed, 0.3, 0.8);

	Fire(vecSrc, vecAiming, damage);
}

static bool ReflectRay(CBaseEntity * ent)
{
	return ent->IsBSPModel();
}

void CWeaponBrGauss::Fire(Vector vecOrigSrc, Vector vecDir, float flDamage)
{
	auto player = GetPlayerOwner();

	//player->m_iWeaponVolume = GAUSS_PRIMARY_FIRE_VOLUME;

	Vector vecSrc = vecOrigSrc;
	Vector vecDest = vecSrc + vecDir * 8192;

	trace_t tr, beam_tr;
	float flMaxFrac = 1.0;
	bool hasPunched = false;
	bool firstBeam = true;
	int	nMaxHits = 10;

#ifdef CLIENT_DLL
	if (m_fPrimaryFire == false)
		g_irunninggausspred = true;
#endif

	// make sure sound is stopped
	StopChargeSound();

	// The main firing event is sent unreliably so it won't be delayed.
	//PLAYBACK_EVENT_FULL(FEV_NOTHOST, m_pPlayer->edict(), m_usGaussFire, 0.0, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, flDamage, 0.0, 0, 0, m_fPrimaryFire ? 1 : 0, 0);

	WeaponSound(WeaponSound_t::SINGLE);

	// This reliable event is used to stop the spinning sound
	// It's delayed by a fraction of second to make sure it is delayed by 1 frame on the client
	// It's sent reliably anyway, which could lead to other delays

	//PLAYBACK_EVENT_FULL(FEV_NOTHOST | FEV_RELIABLE, m_pPlayer->edict(), m_usGaussFire, 0.01, (float *)&m_pPlayer->pev->origin, (float *)&m_pPlayer->pev->angles, 0.0, 0.0, 0, 0, 0, 1);


	/*ALERT( at_console, "%f %f %f\n%f %f %f\n",
	vecSrc.x, vecSrc.y, vecSrc.z,
	vecDest.x, vecDest.y, vecDest.z );*/


	//	ALERT( at_console, "%f %f\n", tr.flFraction, flMaxFrac );

	CTraceFilterSimple filter(player, COLLISION_GROUP_NONE);
	CTraceFilter * ignoreEntity = &filter;

	while (flDamage > 10 && nMaxHits > 0) {
		nMaxHits--;

		// ALERT( at_console, "." );
		UTIL_TraceLine(vecSrc, vecDest, MASK_SHOT_HULL, ignoreEntity, &tr);

		if (tr.allsolid)
			break;

		auto pEntity = tr.m_pEnt;

		if (pEntity == NULL)
			break;

		DrawBeam(vecSrc, tr.endpos, 3.0f, firstBeam, !!m_fPrimaryFire);

		if (firstBeam) {
			player->DoMuzzleFlash();
			firstBeam = false;
		}

#ifndef CLIENT_DLL
		ClearMultiDamage();

		Vector force = vecDir * flDamage * 40.0f;

		// Damage anything flying with gauss
		CBaseHelicopter * heli = dynamic_cast<CBaseHelicopter *>(pEntity);
		int dmgType = ((heli == nullptr) ? DMG_BULLET : DMG_BLAST) | DMG_ENERGYBEAM;

		CTakeDamageInfo dmg(player, player, this, force, tr.endpos, flDamage, dmgType);

		pEntity->DispatchTraceAttack(dmg, vecDir, &tr);
		ApplyMultiDamage();
#endif

		CPVSFilter expl_filter(tr.endpos);
		te->GaussExplosion(expl_filter, 0.0f, tr.endpos, tr.plane.normal, 0);

		if (ReflectRay(pEntity) && !(tr.surface.flags & SURF_SKY)) {
			float n;

			// Allow player to be self-damaged
			ignoreEntity = NULL;

			n = -DotProduct(tr.plane.normal, vecDir);

			if (n < 0.5f) // 60 degrees
			{
				Vector r;

				r = 2.0 * tr.plane.normal * n + vecDir;
				flMaxFrac = flMaxFrac - tr.fraction;
				vecDir = r;
				vecSrc = tr.endpos + vecDir * 8;
				vecDest = vecSrc + vecDir * 8192;

#ifndef CLIENT_DLL
				// explode a bit
				CTakeDamageInfo dmg_blast(player, player, this, flDamage * n, DMG_BLAST);
				::RadiusDamage(dmg_blast, tr.endpos, flDamage * 2.5f, CLASS_NONE, player);
#endif

				// lose energy
				if (n == 0) n = 0.1f;
				flDamage = flDamage * (1.0f - n);
			} else {
				// Primary attack can't pass through walls
				if (m_fPrimaryFire) {
					//ALERT( at_console, "blocked solid\n" );
					flDamage = 0;
					break;
				}

				// limit it to one hole punch
				if (hasPunched)
					break;

				hasPunched = true;

				/*

				I'm tyring to understand how it works....

				So ray passes through the wall

				.                  | / / / / |
				-------------------X  / / /  |
				.                  | / / / / |

				First step is to try to continue trace with a slight offset. But the end point of trace (8192 units from start)
				is the same

				.                  | / / / / |                     |
				-------------------X  O------>---------------------X
				.                  | / / / / |                     |

				Then it hits something else. Now this is the point where i'm starting to loose the picture.

				I'm assuming power of the explosion on the other side should be located at the exit point and
				be reverse-proportional to wall thickness. But it looks like it's not the case.


				*/

				// try punching through wall if secondary attack (primary is incapable of breaking through)
				UTIL_TraceLine(tr.endpos + vecDir * 8, vecDest, MASK_SHOT_HULL, ignoreEntity, &beam_tr);
				if (beam_tr.allsolid) {
					//ALERT( at_console, "blocked %f\n", n );
					flDamage = 0;
					break;
				}

				// trace backwards to find exit point
				// ntrf: can compute exit point form `beam_tr.fractionleftsolid`
				UTIL_TraceLine(beam_tr.endpos, tr.endpos, MASK_SHOT_HULL, ignoreEntity, &beam_tr);

				float dist = (beam_tr.endpos - tr.endpos).Length();

				//ntrf: this is attempt to fix self-gauss bug
				//
				//      in hl1 you need to shoot through a wall, that is thick enough not to be punched through, 
				//      but does have enough space on the other side. if exit point exists, but wall it too thick
				//      hl1 will, instead of blocking the shot, shoot again from previous position. only this time
				//      gun trace against player will not be blocked and player will ge damage.
				//
				//      in hl2 however outside of the map is void and considered enmpty for traces (but solid for
				//      stationary contents checks). this results in EVERY shot through the skybox outside wall, 
				//      that is thick enough, to trigger self-gauss.
				//
				//      this statement will block shots through walls that are too thick
				if (dist > gauss_maxpunch.GetFloat()) {
					// block this shot
					flDamage = 0;
					break;
				}

				if (dist < flDamage) {
					if (dist == 0) dist = 1;
					flDamage -= dist;

					// ALERT( at_console, "punch %f\n", n );

#ifndef CLIENT_DLL
					// exit blast damage
					//m_pPlayer->RadiusDamage( beam_tr.vecEndPos + vecDir * 8, pev, m_pPlayer->pev, flDamage, CLASS_NONE, DMG_BLAST );
					float damage_radius = flDamage * (g_pGameRules->IsMultiplayer() ? 1.75f : 2.5f);

					CTakeDamageInfo dmg_blast(player, player, this, flDamage, DMG_BLAST);

					::RadiusDamage(dmg_blast, beam_tr.endpos + vecDir * 8, damage_radius, CLASS_NONE, nullptr);

					//CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME, 3.0);
#endif
					vecSrc = beam_tr.endpos + vecDir;
				}
			}
		} else {
			vecSrc = tr.endpos + vecDir;
			filter.SetPassEntity(pEntity);
			ignoreEntity = &filter;
		}
	}
}

void CWeaponBrGauss::WeaponIdle()
{
	auto player = GetPlayerOwner();

	//ResetEmptySound();

	// play aftershock static discharge
	/*	if (player->m_flPlayAftershock && m_pPlayer->m_flPlayAftershock < gpGlobals->time)
	{
	switch (RANDOM_LONG(0, 3))
	{
	case 0:	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro4.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM); break;
	case 1:	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro5.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM); break;
	case 2:	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/electro6.wav", RANDOM_FLOAT(0.7, 0.8), ATTN_NORM); break;
	case 3:	break; // no sound
	}
	m_pPlayer->m_flPlayAftershock = 0.0;
	}
	*/
	if (m_flTimeWeaponIdle > gpGlobals->curtime)
		return;

	if (m_fInAttack != 0) {
		StartFire();
		m_fInAttack = 0;
		m_flTimeWeaponIdle = gpGlobals->curtime + 2.0;
	} else {
		/*
		int iAnim;
		float flRand = RandomFloat();
		if (flRand <= 0.5)
		{
		iAnim = GAUSS_IDLE;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
		}
		else if (flRand <= 0.75)
		{
		iAnim = GAUSS_IDLE2;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
		}
		else
		{
		iAnim = GAUSS_FIDGET;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 3;
		}

		return;
		SendWeaponAnim(iAnim);
		*/

		CBaseHLCombatWeapon::WeaponIdle();
	}
}

void CWeaponBrGauss::DrawBeam(const Vector &startPos, const Vector &endPos, float width, bool attachMuzzle, bool primary)
{
	auto muzzle = LookupAttachment("muzzle");

	//Tracer down the middle
	//UTIL_Tracer(startPos, endPos, 0, TRACER_DONT_USE_ATTACHMENT, 6500, false, "GaussTracer");

	//Draw the main beam shaft
	CBeam *pBeam = CBeam::BeamCreate(GAUSS_BEAM_SPRITE, 0.5f);

	if (!attachMuzzle) {
		pBeam->SetStartPos(startPos);
		pBeam->SetEndPos(endPos);
	} else {
		pBeam->PointEntInit(endPos, this);
		pBeam->SetEndAttachment(1);
	}
	pBeam->SetWidth(width);
	//pBeam->SetEndWidth(0.05f);
	pBeam->SetBrightness(255);
	if (primary) {
		pBeam->SetColor(255, 185 + random->RandomInt(-16, 16), 40);
	} else {
		pBeam->SetColor(255, 255, 150 + random->RandomInt(0, 64));
	}
	pBeam->RelinkBeam();
	pBeam->LiveForTime(0.1f);

	//Draw electric bolts along shaft
	pBeam = CBeam::BeamCreate(GAUSS_BEAM_SPRITE, 3.0f);

	if (!attachMuzzle) {
		pBeam->SetStartPos(startPos);
		pBeam->SetEndPos(endPos);
	} else {
		pBeam->PointEntInit(endPos, this);
		pBeam->SetEndAttachment(1);
	}
	pBeam->SetBrightness(random->RandomInt(64, 255));
	pBeam->SetColor(255, 255, 150 + random->RandomInt(0, 64));
	pBeam->RelinkBeam();
	pBeam->LiveForTime(0.1f);
	pBeam->SetNoise(1.2f);
	pBeam->SetEndWidth(0.1f);
}
