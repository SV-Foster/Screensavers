/***

Copyright 2023, SV Foster. All rights reserved.

License:
	This program is free for personal, educational and/or non-profit usage

Revision History:

***/

#include <windows.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <cstdlib>
#include <tchar.h>
#include "SharedHeaders.h"
#include "ScreensaverBase.h"
#include "ScreensaverCmdLineParser.h"

#pragma comment (lib, "Shlwapi.lib")


//
// https://learn.microsoft.com/en-us/troubleshoot/windows/win32/screen-saver-command-line
// 
// Windows executes the screensaver binary file with specified command line parameters:
//
// "" - Show the Settings dialog box
// /C:<HWND> - Show the Settings dialog box, optionally modal to the foreground window <HWND>
// /P <HWND> - preview mode. The operating system uses this switch exclusively to render the screensaver preview in the 
//             Screen Saver Settings dialog. The "<HWND>" parameter provides the handle of the preview window.
//             You need create the preview window as a child of this window. It should cover the parent's entire client area
// /S - Run the Screen Saver in full screen
//
// HWND presented on the command line as an unsigned decimal number
//
BOOL ScreensaverCmdLineParser::Parse(LPCTSTR CommandLine, ScreensaverBase::COMMAND* cmd, HWND* parent) noexcept
{
	BOOL Result;
	LPWSTR* ArgsList = NULL;
	int ArgsNumber;


	if (!cmd)
		ExitFunction(FALSE);
	if (!parent)
		ExitFunction(FALSE);

	// defaults, no params
	*cmd = ScreensaverBase::COMMAND::CMD_CONFIG;
	*parent = 0;

	if (!_tcslen(CommandLine))
		ExitFunction(TRUE);

	// get an array from the command line
	ArgsList = CommandLineToArgvW(CommandLine, &ArgsNumber);
	if (NULL == ArgsList)
		ExitFunction(FALSE);

	if ((ArgsNumber <= 0) || (ArgsNumber >= 3))
		ExitFunction(FALSE);

	
	// get parameters from the array
	if (_tcsicmp(ArgsList[0], TEXT("/C")) == 0)
		ExitFunction(TRUE);

	if (StrStrI(ArgsList[0], TEXT("/C:")) == ArgsList[0])
	{
		*parent = (HWND)atoui(ArgsList[0] + 3);

		ExitFunction(TRUE);
	}

	if (_tcsicmp(ArgsList[0], TEXT("/P")) == 0)
	{
		*cmd = ScreensaverBase::COMMAND::CMD_PREVIEW;
		if (ArgsNumber >= 2)
			*parent = (HWND)atoui(ArgsList[1]);

		ExitFunction(TRUE);
	}

	if (_tcsicmp(ArgsList[0], TEXT("/S")) == 0)
	{
		*cmd = ScreensaverBase::COMMAND::CMD_FULLSCREEN;

		ExitFunction(TRUE);
	}

	// other, unknown parameter
	Result = FALSE;

function_end:
	LocalFree(ArgsList);
	return Result;
}

// helper to convert text to unsigned int, the simple way
// no need to make it fully safe, anyway this param will be checked later w/IsWindow() call
UINT64 ScreensaverCmdLineParser::atoui(LPCTSTR str) noexcept
{
	UINT64 Result = 0;


	while ((*str >= TEXT('0')) && (*str <= TEXT('9')))
		Result = ((Result * 10) + (*str++ - TEXT('0')));

	return Result;
}
