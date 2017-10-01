//========= Copyright Valve Corporation, All rights reserved. ============//
#include "cbase.h"

#ifdef HL2_EPISODIC

#include "hl2_gamerules.h"
#include "ammodef.h"
#include "hl2_shareddefs.h"
#include "filesystem.h"
#include <KeyValues.h>

#ifdef CLIENT_DLL

#else
#include "player.h"
#include "game.h"
#include "gamerules.h"
#include "teamplay_gamerules.h"
#include "hl2_player.h"
#include "voice_gamemgr.h"
#include "globalstate.h"
#include "ai_basenpc.h"
#include "weapon_physcannon.h"
#include "ammodef.h"
#endif

#ifdef CLIENT_DLL
#define CBlamodChallenge C_BlamodChallenge
#define CBlamodChallengeProxy C_BlamodChallengeProxy
#endif

ConVar blamod_challengemode( "blamod_challengemode", "0", FCVAR_REPLICATED );

class CBlamodChallengeProxy : public CGameRulesProxy
{
public:
	DECLARE_CLASS( CBlamodChallengeProxy, CGameRulesProxy );
	DECLARE_NETWORKCLASS();
};

class CBlamodChallenge : public CHalfLife2
{
public:
	DECLARE_CLASS( CBlamodChallenge, CHalfLife2 );

#ifdef CLIENT_DLL

	DECLARE_CLIENTCLASS_NOBASE(); // This makes datatables able to access our private vars.

#else

	DECLARE_SERVERCLASS_NOBASE(); // This makes datatables able to access our private vars.

	CBlamodChallenge();
	~CBlamodChallenge() override {}

	void Think() override;
	void PlayerSpawn(CBasePlayer *pPlayer) override;
	bool IsAllowedToSpawn(CBaseEntity *pEntity) override;
	void CreateStandardEntities() override;

	bool IsCoOp() override { return true; }
	//bool IsMultiplayer() override { return true; }

private:
	bool			  m_bActive;
#endif

};

//-----------------------------------------------------------------------------
// Gets us at the Half-Life 2 game rules
//-----------------------------------------------------------------------------
inline CBlamodChallenge* BlamodChallengeGameRules()
{
	return static_cast<CBlamodChallenge*>(g_pGameRules);
}

REGISTER_GAMERULES_CLASS( CBlamodChallenge );

BEGIN_NETWORK_TABLE_NOBASE( CBlamodChallenge, DT_BlamodChallengeGameRules )
END_NETWORK_TABLE()


LINK_ENTITY_TO_CLASS( blamod_challenge_gamerules, CBlamodChallengeProxy );
IMPLEMENT_NETWORKCLASS_ALIASED( BlamodChallengeProxy, DT_BlamodChallengeProxy )

#ifdef CLIENT_DLL
	void RecvProxy_BlamodChallengeGameRules( const RecvProp *pProp, void **pOut, void *pData, int objectID )
	{
		CBlamodChallenge *pRules = BlamodChallengeGameRules();
		Assert( pRules );
		*pOut = pRules;
	}

	BEGIN_RECV_TABLE( CBlamodChallengeProxy, DT_BlamodChallengeProxy )
	RecvPropDataTable( "blamod_challenge_gamerules_data", 0, 0, &REFERENCE_RECV_TABLE( DT_BlamodChallengeGameRules ), RecvProxy_BlamodChallengeGameRules )
	END_RECV_TABLE()
	#else
	void* SendProxy_BlamodChallengeGameRules( const SendProp *pProp, const void *pStructBase, const void *pData, CSendProxyRecipients *pRecipients, int objectID )
	{
		CBlamodChallenge *pRules = BlamodChallengeGameRules();
		Assert( pRules );
		pRecipients->SetAllRecipients();
		return pRules;
	}

	BEGIN_SEND_TABLE( CBlamodChallengeProxy, DT_BlamodChallengeProxy )
	SendPropDataTable( "blamod_challenge_gamerules_data", 0, &REFERENCE_SEND_TABLE( DT_BlamodChallengeGameRules ), SendProxy_BlamodChallengeGameRules )
	END_SEND_TABLE()
#endif

#ifndef CLIENT_DLL

CBlamodChallenge::CBlamodChallenge()
{
	m_bActive = true;
}

void CBlamodChallenge::Think( void )
{

}

bool CBlamodChallenge::IsAllowedToSpawn( CBaseEntity *pEntity )
{
/*	if ( !m_bActive )
		return BaseClass::IsAllowedToSpawn( pEntity );

	const char *pPickups = STRING( m_SurvivalSettings.m_szPickups );
	if ( !pPickups )
		return false;

	if ( Q_stristr( pPickups, "everything" ) )
		return true;

	if ( Q_stristr( pPickups, pEntity->GetClassname() ) ||  Q_stristr( pPickups, STRING( pEntity->GetEntityName() ) ) )
		return true;

	return false;*/
	return true;
}

void CBlamodChallenge::PlayerSpawn( CBasePlayer *pPlayer )
{
	BaseClass::PlayerSpawn( pPlayer );

	if ( !m_bActive )
		return;

	pPlayer->EquipSuit();
}

void CBlamodChallenge::CreateStandardEntities( void )
{
}

#endif

#endif