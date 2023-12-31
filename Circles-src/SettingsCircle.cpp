/***

Copyright 2023, SV Foster. All rights reserved.

License:
	This program is free for personal, educational and/or non-profit usage

Revision History:

***/

#include <Windows.h>
#include <memory>
#include <string>
#include <random>
#include "ScreensaverBase.h"
#include "ScreensaverCircles.h"
#include "SettingsBase.h"
#include "SettingsCircle.h"


SettingsCircle::SettingsCircle(CONST std::wstring& Path):
	SettingsBase(Path)
{
	return;
}

SettingsCircle::~SettingsCircle() noexcept
{
	// virtual, auto call to SettingsBase::~SettingsBase();
	return;
}

VOID SettingsCircle::BackgroundColorRGBSet(COLORREF n) const
{
	WriteRGB(RegValueBackgroundColor, n);
}

COLORREF SettingsCircle::BackgroundColorRGBGet(COLORREF Default) const
{
	return ReadRGB(RegValueBackgroundColor, Default);
}

VOID SettingsCircle::TimerIntervalSet(DWORD n) const
{
	WriteDWORD(RegValueTimerInterval, n);
}

DWORD SettingsCircle::TimerIntervalGet(DWORD Default) const
{
	return ReadDWORD(RegValueTimerInterval, Default);
}

VOID SettingsCircle::MouseSenceSet(DWORD n) const
{
	WriteDWORD(RegValueMouseSence, n);
}

DWORD SettingsCircle::MouseSenceGet(DWORD Default) const
{
	return ReadDWORD(RegValueMouseSence, Default);
}

VOID SettingsCircle::CirclesCountSet(DWORD n) const
{
	WriteDWORD(RegValueCirclesCount, n);
}

DWORD SettingsCircle::CirclesCountGet(DWORD Default) const
{
	return ReadDWORD(RegValueCirclesCount, Default);
}

VOID SettingsCircle::CirclesColorRGBSet(COLORREF n) const
{
	WriteRGB(RegValueCirclesColor, n);
}

COLORREF SettingsCircle::CirclesColorRGBGet(COLORREF Default) const
{
	return ReadRGB(RegValueCirclesColor, Default);
}
