/***

Copyright 2023, SV Foster. All rights reserved.

License:
	This program is free for personal, educational and/or non-profit usage

Revision History:

***/

#pragma once


#define RegistryPath TEXT("SOFTWARE\\SV Foster\\Circles Screensaver")
#define RegValueBackgroundColor TEXT("BackgroundColor")
#define RegValueCirclesCount TEXT("CirclesCount")
#define RegValueCirclesColor TEXT("CirclesColor")
#define RegValueTimerInterval TEXT("FrameTimerInterval")
#define RegValueMouseSence TEXT("MouseSence")


class SettingsCircle:
	public SettingsBase,
	public IScreenSaverCirclesSettings
{
public:
	SettingsCircle(CONST std::wstring& Path);
	virtual ~SettingsCircle() noexcept;
	SettingsCircle(const SettingsCircle& oth) = delete;
	SettingsCircle& operator=(const SettingsCircle& oth) = delete;
	SettingsCircle(SettingsCircle&& oth) = delete;
	SettingsCircle& operator=(SettingsCircle&& oth) = delete;

	VOID BackgroundColorRGBSet(COLORREF n) const override;
	COLORREF BackgroundColorRGBGet(COLORREF Default) const override;
	VOID TimerIntervalSet(DWORD n) const override;
	DWORD TimerIntervalGet(DWORD Default) const override;
	VOID MouseSenceSet(DWORD n) const override;
	DWORD MouseSenceGet(DWORD Default) const override;
	VOID CirclesCountSet(DWORD n) const override;
	DWORD CirclesCountGet(DWORD Default) const override;
	VOID CirclesColorRGBSet(COLORREF n) const override;
	COLORREF CirclesColorRGBGet(COLORREF Default) const override;
};
