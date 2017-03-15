#pragma once

#include "convar.h"

//----------------
//-- blamod run category switch
//--

class BlaConVar : public ConVar
{
public:
	BlaConVar(const char *pName, const char *pDefaultValue, int flags = 0);

	BlaConVar(const char *pName, const char *pDefaultValue, int flags,
		   const char *pHelpString);
	BlaConVar(const char *pName, const char *pDefaultValue, int flags,
		   const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax);

protected:
	void BvInit();
	static void VariableChangeCallback(IConVar *var, const char *pOldValue, float flOldValue);

public:
	bool bv_registered = false;
	BlaConVar * bv_next = nullptr;
	static BlaConVar * bv_chain;

	static void InitBlamodVars();
};