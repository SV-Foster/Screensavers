/***

Copyright 2023, SV Foster. All rights reserved.

License:
	This program is free for personal, educational and/or non-profit usage

Revision History:

***/

#pragma once


class ScreensaverCmdLineParser
{
public:
	static BOOL Parse(LPCTSTR CommandLine, ScreensaverBase::COMMAND* cmd, HWND* parent) noexcept;

private:
	static UINT64 atoui(LPCTSTR str) noexcept;
};
