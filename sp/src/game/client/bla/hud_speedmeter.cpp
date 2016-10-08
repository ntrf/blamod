#include "cbase.h"
#include "hudelement.h"
#include "hud_numericdisplay.h"
#include "iclientmode.h"

#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>

using namespace vgui;

static ConVar bla_speedmeter("bla_speedmeter", "1", 
                             FCVAR_CLIENTDLL | FCVAR_ARCHIVE | FCVAR_DEMO, 
                             "Turn the speedmeter on/off\nSet it to 2 to include vertical speed");

class CHudSpeedMeter : public CHudElement, public CHudNumericDisplay
{
    DECLARE_CLASS_SIMPLE(CHudSpeedMeter, CHudNumericDisplay);

public:
    CHudSpeedMeter(const char *pElementName);
    virtual void Init()
    {
        Reset();
    }
    virtual void VidInit()
    {
        Reset();
    }
    virtual void Reset()
    {
        SetLabelText(L"UPS");
        SetDisplayValue(0);
    }
    bool ShouldDraw() override
    {
        return bla_speedmeter.GetInt() > 0 && CHudElement::ShouldDraw();
    }

	void OnThink() override;

	void Paint() override;
};

DECLARE_HUDELEMENT(CHudSpeedMeter);

CHudSpeedMeter::CHudSpeedMeter(const char *pElementName) :
    CHudElement(pElementName), CHudNumericDisplay(NULL, "HudSpeedmeter")
{
    SetParent(g_pClientMode->GetViewport());
    SetHiddenBits(HIDEHUD_PLAYERDEAD);
}

void CHudSpeedMeter::OnThink()
{
    Vector velocity(0, 0, 0);
    C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	if (player) {
		velocity = player->GetLocalVelocity();
		if (bla_speedmeter.GetInt() < 2) velocity.z = 0;
	}
    SetDisplayValue((int)velocity.Length());
}

void CHudSpeedMeter::Paint()
{
	BaseClass::Paint();

	//vgui::surface()->Draw
}