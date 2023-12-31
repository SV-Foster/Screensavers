/***

Copyright 2023, SV Foster. All rights reserved.

License:
	This program is free for personal, educational and/or non-profit usage

Revision History:

***/

#pragma once


#define DefaultLineWidth 1
#define DefaultScreenBorderDelta 0
#define DefaultIncABS 4


enum class TColors : int
{
	toRed,
	toYellow,
	toGreen,
	toBluegreen,
	toBlue,
	toPurple,
	toRedFromPurple
};

class ScreensaverMoire :
	public ScreensaverBase
{
public:
	ScreensaverMoire(HINSTANCE hInst, int nCmdShow);
	virtual ~ScreensaverMoire() noexcept;
	ScreensaverMoire(const ScreensaverMoire& oth) = delete;
	ScreensaverMoire& operator=(const ScreensaverMoire& oth) = delete;
	ScreensaverMoire(ScreensaverMoire&& oth) = delete;
	ScreensaverMoire& operator=(ScreensaverMoire&& oth) = delete;

	VOID Reset();
	VOID DrawUpdateLine(int* x1i, int* x2i, int* y1i, int* y2i, int* x1inci, int* x2inci, int* y1inci, int* y2inci, COLORREF c) noexcept;

private:
	HBITMAP NextFrameBitmap;
	HDC NextFrameDC;
	std::mt19937 RandomGen;

	TColors CurrentColor;
	BYTE Red;
	BYTE Green;
	BYTE Blue;
	
	int x1; 
	int x2;
	int y1;
	int y2;
	int x1inc;
	int x2inc;
	int y1inc;
	int y2inc;

	BOOL TailErase;
	int x1Erase;
	int x2Erase;
	int y1Erase;
	int y2Erase;
	int x1incErase;
	int	x2incErase;
	int y1incErase;
	int y2incErase;


	UINT OnConfigDialogShow() override;
	VOID OnBeforeWindowOperableShow() override;
	VOID OnWindowOperableShow() override;
	VOID OnWindowOperableHide() override;
	VOID OnNextFrame(HDC dc) override;
	VOID OnRedrawRegion(LPPAINTSTRUCT ps) override;
	LRESULT OnMessage(UINT message, WPARAM wParam, LPARAM lParam) override;
	int  RandomValueGet(const int min, const int max);
};
