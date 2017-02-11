#include "cbase.h"
#include "hudelement.h"
#include "hud_numericdisplay.h"
#include "hud_macros.h"
#include "iclientmode.h"
#include "view.h"

using namespace vgui;

#include <vgui_controls/Panel.h>
#include <vgui_controls/Frame.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <vgui_controls/AnimationController.h>

#include "vgui_helpers.h"

#define BUFSIZE 45

ConVar bla_hud_name("bla_hud_name", "1",
						FCVAR_CLIENTDLL | FCVAR_ARCHIVE,
						"Turn the runner name display on/off");

ConVar bla_runner_name("bla_name", "",
							   FCVAR_CLIENTDLL | FCVAR_ARCHIVE,
							   "Override displayed runner name");

class CHudName : public CHudElement, public Panel
{
	DECLARE_CLASS_SIMPLE(CHudName, Panel);

public:
	CHudName(const char *pElementName);
	virtual void Init() override;
	virtual void VidInit() override
    {
        Reset();
    }
	virtual void Reset() override;
	virtual bool ShouldDraw() override
	{
		return bla_hud_name.GetInt() > 0 && CHudElement::ShouldDraw();
	}
	virtual void Paint() override;

protected:
	//void ApplySchemeSettings(vgui::IScheme *pScheme) override;
	void OnThink() override;

private:
	wchar_t m_pwValue[BUFSIZE];
	char m_pszString[BUFSIZE];

protected:
	CPanelAnimationVar(float, m_flBlur, "Blur", "0");
	CPanelAnimationVar(Color, m_TextColor, "TextColor", "FgColor");
	CPanelAnimationVar(Color, m_Ammo2Color, "Ammo2Color", "FgColor");

	CPanelAnimationVar(HFont, m_hLargeFont, "LargeFont", "RunnerNameLabel");
	CPanelAnimationVar(HFont, m_hSmallFont, "SmallFont", "HudHintTextSmall");
	CPanelAnimationVar(HFont, m_hTextFont, "TextFont", "Default");

	CPanelAnimationVarAliasType(float, text_xpos, "text_xpos", "8", "proportional_float");
	CPanelAnimationVarAliasType(float, text_ypos, "text_ypos", "20", "proportional_float");
	CPanelAnimationVarAliasType(float, digit_xpos, "digit_xpos", "50", "proportional_float");
	CPanelAnimationVarAliasType(float, digit_ypos, "digit_ypos", "2", "proportional_float");
	CPanelAnimationVarAliasType(float, digit2_xpos, "digit2_xpos", "98", "proportional_float");
	CPanelAnimationVarAliasType(float, digit2_ypos, "digit2_ypos", "16", "proportional_float");
};

DECLARE_HUDELEMENT(CHudName);

CHudName::CHudName(const char *pElementName) :
	CHudElement(pElementName), Panel(NULL, "HudName")
{
	SetParent(g_pClientMode->GetViewport());
	SetHiddenBits(HIDEHUD_PLAYERDEAD);
}

void CHudName::Init()
{
	Reset();
}

void CHudName::Reset()
{

}

void CHudName::OnThink()
{
	auto name = bla_runner_name.GetString();
/*	if (!name || !name[0]) {
		C_BasePlayer *local = C_BasePlayer::GetLocalPlayer();
		if (local) {
			name = local->GetPlayerName();
		}
	}*/

	if (name && name[0]) {
		V_strncpy(m_pszString, name, BUFSIZE - 1);
		SetVisible(true);
	} else {
		SetVisible(false);
	}
}

void CHudName::Paint(void)
{
	// Convert the current time to a string.

	// msg.ReadString(m_pszString, sizeof(m_pszString));
	g_pVGuiLocalize->ConvertANSIToUnicode(m_pszString, m_pwValue, sizeof(m_pwValue));

	int wide = 0, tall = 0;

	surface()->GetTextSize(m_hLargeFont, m_pwValue, wide, tall);

	SetWide(wide + 32);

	// Draw the name
	surface()->DrawSetTextColor(GetFgColor());
	surface()->DrawSetTextFont(m_hLargeFont);
	surface()->DrawSetTextPos(digit_xpos, digit_ypos);
	surface()->DrawPrintText(m_pwValue, wcslen(m_pwValue));
}
