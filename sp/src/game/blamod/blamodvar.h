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