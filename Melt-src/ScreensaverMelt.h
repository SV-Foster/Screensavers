/***

Copyright 2023, SV Foster. All rights reserved.

License:
	This program is free for personal, educational and/or non-profit usage

Revision History:

***/

#pragma once


#define MeltFrameTimerInterval 133


class ScreensaverMelt :
	public ScreensaverBase
{
public:
	ScreensaverMelt(HINSTANCE hInst, int nCmdShow);
	virtual ~ScreensaverMelt() noexcept;
	ScreensaverMelt(const ScreensaverMelt& oth) = delete;
	ScreensaverMelt& operator=(const ScreensaverMelt& oth) = delete;
	ScreensaverMelt(ScreensaverMelt&& oth) = delete;
	ScreensaverMelt& operator=(ScreensaverMelt&& oth) = delete;

private:
	HBITMAP DesktopBitmap;
	std::mt19937 RandomGen;

	UINT OnConfigDialogShow() override;
	VOID OnBeforeWindowOperableShow() override;
	VOID OnWindowOperableShow() override;
	VOID OnWindowOperableHide() override;
	VOID OnNextFrame(HDC dc) override;
	VOID OnRedrawRegion(LPPAINTSTRUCT ps) override;
	LRESULT OnMessage(UINT message, WPARAM wParam, LPARAM lParam) override;
	int  RandomValueGet(const int min, const int max);
};
