#include "cbase.h"
#include "blamodvar.h"

extern ConVar blamod_sandbox;
extern ConCommand blamod_category;

BlaConVar * BlaConVar::bv_chain = nullptr;

static bool disable_callback = false;

static void ResetSandboxVariables(IConVar *var, const char *pOldValue, float flOldValue)
{
	// Don't do anything when entering sandbox
	if (blamod_sandbox.GetInt() != 0) return;

	disable_callback = true;

	// Reset all vars to their default values
	for (BlaConVar * v = BlaConVar::bv_chain; !!v; v = v->bv_next) {
		v->Revert();
	}

	disable_callback = false;
}

// This will be called every time blamod variable gets modified
void BlaConVar::VariableChangeCallback(IConVar *var, const char *pOldValue, float flOldValue)
{
	if (disable_callback || blamod_sandbox.GetBool()) return;

	auto bv = static_cast<BlaConVar*>(var);

	auto val = bv->GetString(), def = bv->GetDefault();

	// New value is the default - don't change anything
	if (val == def || (val && def && strcmp(val, def) == 0)) return;

	// Enable the sandbox
	DevWarning("Protected variable changed. Entering sandbox mode");
	blamod_sandbox.SetValue(1);
}

void BlaConVar::InitBlamodVars()
{
	auto bv = bv_chain;

	// Reset all vars to their default values
	for (; bv; bv = bv->bv_next) {
		bv->Revert();
		bv->InstallChangeCallback(&BlaConVar::VariableChangeCallback);
	}
}


static void SetCategoryCommand(const CCommand &command)
{
	
}

void BlaConVar::BvInit()
{
	if (!bv_registered) {
		bv_registered = true;
		bv_next = BlaConVar::bv_chain;
		BlaConVar::bv_chain = this;
	}
}

BlaConVar::BlaConVar(const char *pName, const char *pDefaultValue, int flags) :
ConVar(pName, pDefaultValue, flags)
{
	BvInit();
}

BlaConVar::BlaConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString) :
ConVar(pName, pDefaultValue, flags, pHelpString)
{
	BvInit();
}
BlaConVar::BlaConVar(const char *pName, const char *pDefaultValue, int flags,
		  const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax) :
		  ConVar(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax)
{
	BvInit();
}


////////////////////////
///  CONSOLE VARS

ConVar blamod_sandbox("blamod_sandbox", "1",
					  FCVAR_REPLICATED | FCVAR_NOTIFY,
					  "Enables sandbox rules.\n"
					  "When sandbox rules are disabled all blamod variables\n"
					  "will be reset to their original value.", &ResetSandboxVariables);

ConCommand blamod_category("blamod_category", &SetCategoryCommand,
						   "Creates file with current blamod variables captured\n"
						   "Args: <category name>\n");
