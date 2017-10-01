
#include "cbase.h"
#include "baseentity_shared.h"

#include "hl2_player_shared.h"

class CBlamodTimer : public CLogicalEntity
{
public:
	DECLARE_CLASS(CBlamodTimer, CLogicalEntity);
	
	DECLARE_DATADESC();

	// Constructor
	CBlamodTimer();

public:

	// Input function
	void InputStart(inputdata_t &inputData);
	void InputFinish(inputdata_t &inputData);
	void InputFail(inputdata_t &inputData);

	float start_time;
	bool running = false;

	void UpdateTime(CBasePlayer * pl);
};

LINK_ENTITY_TO_CLASS(blamod_timer, CBlamodTimer);

BEGIN_DATADESC(CBlamodTimer)

	DEFINE_INPUTFUNC(FIELD_VOID, "Start", InputStart),
	DEFINE_INPUTFUNC(FIELD_VOID, "Finish", InputFinish),
	DEFINE_INPUTFUNC(FIELD_VOID, "Fail", InputFail)

END_DATADESC()

CBlamodTimer::CBlamodTimer()
{

}


void CBlamodTimer::InputStart(inputdata_t &inputData)
{
	if (!inputData.pActivator || !inputData.pActivator->IsPlayer()) return;
	
	auto player = dynamic_cast<CBasePlayer *>(inputData.pActivator);
	if (!player) return;

	start_time = gpGlobals->curtime;
	running = true;

	player->StartTimer(this);
}

void CBlamodTimer::InputFinish(inputdata_t &inputData)
{
	if (!running) return;

	if (!inputData.pActivator || !inputData.pActivator->IsPlayer()) return;

	auto player = dynamic_cast<CBasePlayer *>(inputData.pActivator);
	if (!player) return;

	if (running) {
		// ...
	}

	player->StopTimer();
	UpdateTime(player);
	running = false;
}

void CBlamodTimer::InputFail(inputdata_t &inputData)
{
	if (!running) return;

	if (!inputData.pActivator || !inputData.pActivator->IsPlayer()) return;

	auto player = dynamic_cast<CBasePlayer *>(inputData.pActivator);
	if (!player) return;

	player->StopTimer();
	UpdateTime(player);
	running = false;
}

void CBlamodTimer::UpdateTime(CBasePlayer * pl)
{
	float diff = gpGlobals->curtime - start_time;

	CSingleUserRecipientFilter user(pl);
	user.MakeReliable();
	UserMessageBegin(user, "BlaTimer_Time");
	WRITE_FLOAT(diff);
	MessageEnd();
}

namespace Blamod
{
	void DispatchTimerMessage(class CBlamodTimer * timer, class CBasePlayer * player)
	{
		if (!timer || !player || !timer->running)
			return;

		timer->UpdateTime(player);
	}
}