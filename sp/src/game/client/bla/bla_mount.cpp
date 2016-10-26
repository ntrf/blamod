
#include "cbase.h"

#include "icvar.h"
#include "filesystem.h"
#include "datacache/idatacache.h"
#include "datacache/imdlcache.h"
#include "scenefilecache/ISceneFileCache.h"

#include "steam/steam_api.h"

#include "memdbgon.h"

extern ISceneFileCache *scenefilecache;
extern IMDLCache *mdlcache;

void bla_mount_f(const CCommand & cmd)
{
	Warning("Not implemented\n");

	auto sapps = steamapicontext->SteamApps();

	const char * language = sapps->GetCurrentGameLanguage();

	char name[2049];
	auto namelen = sapps->GetAppInstallDir(420, name, 2048);
	
	filesystem->AddSearchPath("../Half-Life 2/episodic/ep1_sound_vo_english.vpk", "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath("../Half-Life 2/episodic/ep1_pak.vpk", "GAME", PATH_ADD_TO_HEAD);
	filesystem->AddSearchPath("../Half-Life 2/episodic", "GAME", PATH_ADD_TO_HEAD);

	datacache->Flush();
	mdlcache->Flush();

	scenefilecache->Reload();
}
static ConCommand bla_mount("bla_mount", bla_mount_f, "Mount episodes data", FCVAR_CLIENTDLL);



void bla_getmountslist_f(const CCommand & cmd)
{
	char name[2049];


	filesystem->GetSearchPath("GAME", true, name, 2048);
}
static ConCommand bla_getmounts("bla_getmounts", bla_getmountslist_f, "Gets mounts", FCVAR_CLIENTDLL);