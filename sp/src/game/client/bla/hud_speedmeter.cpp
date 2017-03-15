#include "cbase.h"
#include "hudelement.h"
#include "hud_numericdisplay.h"
#include "iclientmode.h"

#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>

using namespace vgui;

static ConVar bla_speedmeter("blamod_speedmeter", "1", 
                             FCVAR_CLIENTDLL | FCVAR_ARCHIVE, 
                             "Turn the speedmeter on/off\nSet it to 2 to include vertical speed");

class CHudSpeedMeter : public CHudElement, public CHudNumericDisplay
{
    DECLARE_CLASS_SIMPLE(CHudSpeedMeter, CHudNumericDisplay);

public:
    CHudSpeedMeter(const char *pElementName);
    
	void Init() override
    {
        Reset();
    }
    
	void VidInit() override
    {
        Reset();
    }

	void Reset() override;

    bool ShouldDraw() override
    {
        return bla_speedmeter.GetInt() > 0 && CHudElement::ShouldDraw();
    }

	void OnThink() override;

	void Paint() override;

	float tick_pos = 0.0f;
	float verticalVelocity = 0.0f;
};

DECLARE_HUDELEMENT(CHudSpeedMeter);

CHudSpeedMeter::CHudSpeedMeter(const char *pElementName) :
    CHudElement(pElementName), CHudNumericDisplay(NULL, "HudSpeedmeter")
{
    SetParent(g_pClientMode->GetViewport());
    SetHiddenBits(HIDEHUD_PLAYERDEAD);
}

void CHudSpeedMeter::Reset()
{
	SetLabelText(L"UPS");
	SetDisplayValue(0);

	tick_pos = 0;
}

void CHudSpeedMeter::OnThink()
{
    Vector velocity(0, 0, 0);
    C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	if (player) {
		velocity = player->GetLocalVelocity();

		// update vertical tick position
		tick_pos += (gpGlobals->frametime * velocity.z) / 100.0f;
		verticalVelocity = velocity.z;

		if (bla_speedmeter.GetInt() < 2) {
			velocity.z = 0;
		}
	}
    SetDisplayValue((int)velocity.Length());
}

void CHudSpeedMeter::Paint()
{
	CHudNumericDisplay::Paint();

	float w = (float)this->GetWide();
	float hc = (float)this->GetTall() * 0.5f;

	float offset = tick_pos - floorf(tick_pos);

	float step = (hc - 3.0f) / 3.0f;

	vgui::surface()->DrawSetColor(Color(255, 255, 255, 64));

	for (float i = -2; i <= 2; i += 1.0f) {
		float y = hc + (offset - (float)i - 0.5f) * step;
		vgui::surface()->DrawLine(w - 17, y, w - 5, y);
		vgui::surface()->DrawLine(5, y, 17, y);
	}
#if 0
	float xpos = w - 30;
	float ypos = hc;

	wchar_t unicode[40];

	if (verticalVelocity > 0)
		V_snwprintf(unicode, ARRAYSIZE(unicode), L"+%d", (int)verticalVelocity);
	else
		V_snwprintf(unicode, ARRAYSIZE(unicode), L"%d", (int)verticalVelocity);

	surface()->DrawSetTextPos(xpos, ypos);
	surface()->DrawUnicodeString(unicode);
#endif
}