/***

Copyright 2023, SV Foster. All rights reserved.

License:
	This program is free for personal, educational and/or non-profit usage

Revision History:

***/

#include <windows.h>
#include <tchar.h>
#include <memory>
#include <random>
#include "Resource.h"
#include "ScreensaverBase.h"
#include "ScreensaverMoire.h"
#include "ScreensaverCmdLineParser.h"
#include "SharedSettingsDefaults.h"
#include "Main.h"


int WINAPI _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	ScreensaverBase::COMMAND cmd;
	HWND handle;


	// app init
	SetErrorMode(SEM_FAILCRITICALERRORS); // Don't popup on floppy query and etc.
	SetProcessShutdownParameters(ShutdownPriority, SHUTDOWN_NORETRY);
	SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);

	// get parameters out of command line
	if (!(ScreensaverCmdLineParser::Parse(lpCmdLine, &cmd, &handle)))
		return EXIT_FAILURE;

	// create objects
	ScreensaverMoire App(hInstance, nCmdShow);

	// start!
	return App.Run(cmd, handle, IDI_ICON);
}
