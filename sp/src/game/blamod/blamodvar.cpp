/*
Copyright 2016 - 2017 Nesterov A.

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
#include "blamodvar.h"

#include "blamod_config.h"

#include "filesystem.h"

extern ConVar blamod_sandbox;
extern ConCommand blamod_category;

BlaConVar * BlaConVar::bv_chain = nullptr;

static bool disable_callback = false;

static void ResetSandboxVariables(IConVar *var, const char *pOldValue, float flOldValue)
{
	// Don't do anything when entering sandbox
	if (blamod_sandbox.GetInt() != 0) return;

#if 0
	disable_callback = true;

	// Reset all vars to their default values
	for (BlaConVar * v = BlaConVar::bv_chain; !!v; v = v->bv_next) {
		v->Revert();
	}

	disable_callback = false;
#endif
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
	DevWarning("Protected variable changed. Entering sandbox mode\n");
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


#if CLIENT_DLL
CUtlString BlamodMountName;
CUtlString BlamodCategoryName;

static void ApplyCategory()
{
	// Don't reset sandbox vars
	if (blamod_sandbox.GetBool())
		return;

	// if there is no category - drop into sandbox
	if (BlamodMountName.IsEmpty() || BlamodCategoryName.IsEmpty()) {
		// drop into sandbox
		blamod_sandbox.SetValue(1);
		return;
	}

	Msg(">>>> Category : %s %s\n", BlamodMountName.String(), BlamodCategoryName.String());

	CUtlString cmd;
	cmd.Format("exec \"cat/%s.%s.cfg\"", BlamodMountName.String(), BlamodCategoryName.String());

	// make sure we can safely reconfigure
	blamod_sandbox.SetValue(1);

	disable_callback = true;

	engine->ExecuteClientCmd(cmd);
	//engine->ExecuteClientCmd("disconnect");

	// register as out-of-sandbox
	blamod_sandbox.SetValue(0);

	disable_callback = false;
}

static void SetCategoryCommand(const CCommand &command)
{
	if (command.ArgC() <= 1) {
		Msg("Usage: blamod_category <category name>\n");
		return;
	}

	const char * arg = command.Arg(1);

	CUtlString path_out;

	path_out.Format("cfg/cat/%s.%s.cfg", BlamodMountName.String(), arg);

	if (!filesystem->FileExists(path_out.String(), "MOD")) {
		Warning("Category file [%s] is not found\n", path_out.String());
		return;
	}

	BlamodCategoryName = arg;

	ApplyCategory();
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

ConCommand blamod_category_apply("blamod_category_apply", &ApplyCategory, nullptr, FCVAR_HIDDEN);

#else

ConVar blamod_sandbox("blamod_sandbox", "1",
					  FCVAR_REPLICATED | FCVAR_NOTIFY,
					  "Enables sandbox rules.");

#endif

ConVar blamod_category_name("blamod_category_name", "", FCVAR_DEMO | FCVAR_NOTIFY,
							"Category name. Used only as displayed name.");

static void BlamodVersion(const CCommand &command)
{
	Msg("Blamod Reborn\n version %s\n", BLAMOD_VERSION);
}
ConCommand blamod_version("blamod_version", &BlamodVersion);
