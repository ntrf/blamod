#ifndef TIMER_H_
#define TIMER_H_

#include "cbase.h"
#include "fasttimer.h"
#include "filesystem.h"
#include "utlbuffer.h"

extern IFileSystem *filesystem;

#ifdef WIN32
#define NEWLINE "\r\n"
#else
#define NEWLINE "\n"
#endif

class BlaTimer
{
public:
	BlaTimer() {}
	~BlaTimer() {}

	void Init()
	{
		m_bIsRunning = false;
	}

	bool IsRunning()
	{
		return m_bIsRunning;
	}

	float GetCurrentTime()
	{
		return gpGlobals->curtime;
	}

	void DispatchTimeToBeatMessage()
	{
/*
		CSingleUserRecipientFilter user(UTIL_GetLocalPlayer());
		user.MakeReliable();
		UserMessageBegin(user, "BlaTimer_TimeToBeat");
			WRITE_FLOAT(m_flSecondsRecord);
		MessageEnd();
*/
	}

	void DispatchTimeMessage()
	{
/*
		CSingleUserRecipientFilter user(UTIL_GetLocalPlayer());
		user.MakeReliable();
		UserMessageBegin(user, "BlaTimer_Time");
			WRITE_FLOAT(GetCurrentTime());
		MessageEnd();
*/
	}

	void SetStartPosition(Vector start)
	{
/*
		UTIL_GetLocalPlayer()->SetStartPosition(start);
		m_vStart = start;
		WriteMapFile();
*/
	}

	void SetGoalPosition(Vector goal)
	{
/*
		UTIL_GetLocalPlayer()->SetGoalPosition(goal);
		m_vGoal = goal;
		WriteMapFile();
*/
	}

private:
	bool m_bIsRunning;
	bool m_IsPaused;
	float m_flSecondsRecord;

	// Inform the HUD about status changes of the timer so it can fire up some
	// fancy animation effects.
	void DispatchStateChangeMessage()
	{
		CSingleUserRecipientFilter user(UTIL_GetLocalPlayer());
		user.MakeReliable();

		UserMessageBegin(user, "BlaTimer_StateChange");
			WRITE_BOOL(m_bIsRunning);
		MessageEnd();
	}

	void WriteMapFile()
	{
#if 0
		char *pszPath = GetMapFilePath();
		FileHandle_t fh = filesystem->Open(pszPath, "w", "MOD");
		if (fh)
		{
			filesystem->FPrintf(fh, "%f %f %f%s", 
			                    m_vStart.x, m_vStart.y, m_vStart.z, NEWLINE);
			filesystem->FPrintf(fh, "%f %f %f%s", 
			                    m_vGoal.x, m_vGoal.y, m_vGoal.z, NEWLINE);
			filesystem->FPrintf(fh, "%f", m_flSecondsRecord);
			filesystem->Close(fh);
		}
		delete[] pszPath;
#endif
	}

	// Caller is responsible for delete[]'ing the array.
	char *GetMapFilePath()
	{
		const char *pszMapname = (gpGlobals->mapname).ToCStr();
		size_t sz = sizeof("maps/") + strlen(pszMapname) + sizeof(".bla") + 1;
		char *pszPath = new char[sz];
		Q_strncpy(pszPath, "maps/", sz);
		Q_strncat(pszPath, pszMapname, sz);
		Q_strncat(pszPath, ".bla", sz);
		Q_FixSlashes(pszPath);
		return pszPath;
	}
public:
	static BlaTimer * timer();
};

#undef NEWLINE

#endif // TIMER_H_
