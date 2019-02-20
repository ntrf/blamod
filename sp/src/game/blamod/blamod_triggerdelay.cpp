/*
Blamod Reborn

Copyright 2016 - 2018 Nesterov A.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "cbase.h"

#include "hl2_gamerules.h"
#include "ammodef.h"
#include "hl2_shareddefs.h"
#include "filesystem.h"
#include <KeyValues.h>

#include "blamodvar.h"

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

#include "CommandBuffer.h"
#include "utlbuffer.h"
#include "inputsystem/ButtonCode.h"

static BlaConVar blamod_triggerdelay("blamod_triggerdelay", "1", FCVAR_REPLICATED);

#ifdef CLIENT_DLL

//-------------------------------------------------------
class AccumCommandBuffer : public CCommandBuffer
{
public:
	AccumCommandBuffer() : CCommandBuffer()
	{
		SetWaitEnabled(true);
	}

	int accumulatedTick = 0;

	inline int GetDelay() const 
	{
		int d = accumulatedTick - m_nCurrentTick;
		return (d > 0) ? d : 0;
	}

	bool AddTextWithWaits(const char *pText, int nTickDelay = 0)
	{
		Assert(nTickDelay >= 0);

		// make sure we're using correct tick value
		if (accumulatedTick < m_nCurrentTick) {
			accumulatedTick = m_nCurrentTick;
		}

		int	nLen = Q_strlen(pText);
		int nTick = accumulatedTick + nTickDelay;

		// Parse the text into distinct commands
		const char *pCurrentCommand = pText;
		int nOffsetToNextCommand;
		for (; nLen > 0; nLen -= nOffsetToNextCommand + 1, pCurrentCommand += nOffsetToNextCommand + 1) {
			// find a \n or ; line break
			int nCommandLength;
			GetNextCommandLength(pCurrentCommand, nLen, &nCommandLength, &nOffsetToNextCommand);
			if (nCommandLength <= 0)
				continue;

			const char *pArgS;
			char *pArgV0 = (char*)_alloca(nCommandLength + 1);
			CUtlBuffer bufParse(pCurrentCommand, nCommandLength, CUtlBuffer::TEXT_BUFFER | CUtlBuffer::READ_ONLY);
			ParseArgV0(bufParse, pArgV0, nCommandLength + 1, &pArgS);
			if (pArgV0[0] == 0)
				continue;

			// Deal with the special 'wait' command
			if (!Q_stricmp(pArgV0, "wait") && IsWaitEnabled()) {
				int nDelay = pArgS ? atoi(pArgS) : m_nWaitDelayTicks;
				nTick += nDelay;
				continue;
			}

			if (!InsertCommand(pCurrentCommand, nCommandLength, nTick)) {
				accumulatedTick = nTick;
				return false;
			}
		}

		accumulatedTick = nTick;

		return true;
	}

	bool DequeueNextCommandText(CUtlString & res)
	{
		Assert(m_bIsProcessingCommands);
		if (m_Commands.Count() == 0)
			return false;

		int nHead = m_Commands.Head();
		Command_t command = m_Commands[nHead];
		if (command.m_nTick > m_nLastTickToProcess)
			return false;

		m_nCurrentTick = command.m_nTick;

		m_Commands.Remove(nHead);

		// Necessary to insert commands while commands are being processed
		m_hNextCommand = m_Commands.Head();

		res = (command.m_nBufferSize > 0) ? m_pArgSBuffer + command.m_nFirstArgS : "";
		return true;
	}
};

static AccumCommandBuffer cmd_buffer;

int BlamodTriggerDelay_HandleInput(int down, ButtonCode_t key, const char *kb)
{
	char cmd[1024];

	// Don't delay Escape key!
	if (key == KEY_ESCAPE || !blamod_triggerdelay.GetBool() || 
		engine->IsPlayingDemo() || engine->IsInEditMode() || 
		(!g_pGameRules || g_pGameRules->IsMultiplayer()))
		return 1;

	// Only keybinds
	if (!kb || !*kb)
		return 1;

	// Don't intercept console command if there are no pending waits
	if (cmd_buffer.GetDelay() <= 0 && strcmpi(kb, "toggleconsole") == 0)
		return 1;

	if (kb[0] == '+') {
		// Special handling for binds with press-release format
		if (down) {
			V_snprintf(cmd, sizeof(cmd), "%s %i\n", kb, key);
			cmd_buffer.AddTextWithWaits(cmd);
		} else {
			V_snprintf(cmd, sizeof(cmd), "-%s %i\n", kb + 1, key);
			cmd_buffer.AddTextWithWaits(cmd);
		}
	} else if (down) {
			cmd_buffer.AddTextWithWaits(kb);
	}

	return 0;
}

void BlamodTriggerDelay_TickFrame()
{
	// only continue commands when connected
	if (engine->IsDrawingLoadingImage()) return;

	cmd_buffer.BeginProcessingCommands(1);

	do {
		CUtlString cmd_out;
		if (!cmd_buffer.DequeueNextCommandText(cmd_out))
			break;

		engine->ExecuteClientCmd(cmd_out.Get());

	} while (true);

	cmd_buffer.EndProcessingCommands();
}

void BlamodTriggerDelay_HandleTransition(const CCommand &command)
{
	CUtlString cmd_out;
	cmd_out.Format("changelevel2 %s %s", command.Arg(1), command.Arg(2));

	cmd_buffer.AddTextWithWaits(cmd_out.Get());
}

static ConCommand blamod_changelevel2("blamod_changelevel2", &BlamodTriggerDelay_HandleTransition, "", FCVAR_HIDDEN);

#else

void BlamodTriggerDelay_DelayTransition(CBaseEntity *player, const char * level, const char * landmark)
{
	if (blamod_triggerdelay.GetBool()) {
		engine->ClientCommand(player->edict(), "blamod_changelevel2 %s %s", level, landmark);

		if (developer.GetInt() > 0) {
			Msg("Triggerdelay: queued transition to %s via %s\n", level, landmark);
		}

		return;
	}

	engine->ChangeLevel(level, landmark);
}
#endif

/*
Client implementation requires replacing the entire keybinding system 
with our own.

Here is how it should work:

- handle all inputs in replaced CHLClient::IN_KeyEvent
- for each keybind being executed - move it to our command buffer
- when commands are ready to be executed for-real - send them into the system command buffer
- when adding commands - keep previous accumulated wait value

Problems:

- this will not work if commands contain aliases with waits

Alias fix might require me to replace implementation of the alias command :(



Need to implement this whole thing on client-side

How to mix-up the changelevel trigger on client-side?!


 */
