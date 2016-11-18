
#include "cbase.h"

#include "icvar.h"
#include "filesystem.h"
#include "datacache/idatacache.h"
#include "datacache/imdlcache.h"
//#include "datacache
#include "scenefilecache/ISceneFileCache.h"

#include "steam/steam_api.h"

#include "memdbgon.h"

extern ISceneFileCache *scenefilecache;
extern IMDLCache *mdlcache;

extern void S_SoundEmitterSystemFlush(void);

static bool mount_inited = false;

static CUtlConstString game380path;
static CUtlConstString game420path;
static CUtlConstString sdkpath;
static CUtlConstString moddir;
static CUtlConstString original_paths;

void bla_mount_f(const CCommand & cmd)
{
	Warning("Not implemented\n");

	auto sapps = steamapicontext->SteamApps();

	const char * language = sapps->GetCurrentGameLanguage();

	char name[2049];
	int namelen = 0;

	if (!mount_inited) {

		namelen = sapps->GetAppInstallDir(380, name, sizeof(name));
		if (namelen > 0) game380path = name;

		namelen = sapps->GetAppInstallDir(420, name, sizeof(name));
		if (namelen > 0) game420path = name;

		namelen = sapps->GetAppInstallDir(243730, name, sizeof(name));
		if (namelen > 0) sdkpath = name;

		filesystem->GetSearchPath("MOD", false, name, sizeof(name));
		if (namelen > 0) moddir = name;

		// Get list of all current paths
		int res = filesystem->GetSearchPath("GAME", true, name, sizeof(name));
		if (res > 0) original_paths = name;
	}
	
	// Split the paths
	CUtlVector< char * > paths;
	V_SplitString(original_paths.Get(), ";", paths);

	
	// The path list will have the following structure:
	//
	// - mod path
	// - ep1 vpk's
	// - hl2 vpk's
	// - sdk hl2 path
	// - ep1 path
	// - hl2 path

	// Let's build:
	CUtlVector < CUtlConstString > new_paths;

	int i = 0;
	size_t lim = 0;

	// first copy everything we recognize as mod
	lim = strlen(moddir.Get());
	for (; i < paths.Count(); ++i) {
		// check if
		if (V_strnicmp(paths[i], moddir.Get(), lim) != 0) {
			break; // something else
		}
		new_paths.AddToTail(CUtlConstString(paths[i]));
	}

	// now tricky stuff - need to add all the stuff related to episodes
	CUtlString ep1_path;
	ep1_path.Format("%s\\episodic\\ep1_sound_vo_english.vpk", game380path.Get());
	new_paths.AddToTail(CUtlConstString(ep1_path));

	ep1_path.Format("%s\\episodic\\ep1_pak.vpk", game380path.Get());
	new_paths.AddToTail(CUtlConstString(ep1_path));

	// now copy everything until we find sdk folder
	lim = strlen(sdkpath.Get());
	for (; i < paths.Count(); ++i) {
		new_paths.AddToTail(CUtlConstString(paths[i]));
		if (V_strnicmp(paths[i], sdkpath.Get(), lim) == 0 && !V_stristr(paths[i], ".vpk")) {
			++i;
			break;
		}
	}

	// add loose episode files
	ep1_path.Format("%s\\episodic", game380path.Get());
	new_paths.AddToTail(CUtlConstString(ep1_path));

	// copy the rest
	for (; i < paths.Count(); ++i) {
		new_paths.AddToTail(CUtlConstString(paths[i]));
	}


	filesystem->RemoveSearchPaths("GAME");

	for (i = 0; i < new_paths.Count(); ++i) {
		DevMsg("[%d] => %s\n", i, new_paths[i]);

		filesystem->AddSearchPath(new_paths[i].Get(), "GAME");
	}

	// show result
	int res = filesystem->GetSearchPath("GAME", true, name, sizeof(name));

	S_SoundEmitterSystemFlush();

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