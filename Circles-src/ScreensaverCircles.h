/***

Copyright 2023, SV Foster. All rights reserved.

License:
	This program is free for personal, educational and/or non-profit usage

Revision History:

***/

#pragma once


#define DefaultCircleColorRGB RGB(0xff, 0xbf, 0x00)
#define DefaultCirclesCount 25
#define DefaultDeltaMax 30
#define DeltaMaxPreview 5
#define CircleRadiusMin 5
#define CirclesCountPreview 3


class TCircle
{
public:
	TCircle(COLORREF rgb, COLORREF backgr);
	~TCircle() noexcept;
	TCircle(const TCircle& oth) = default;
	TCircle& operator=(const TCircle& oth) = default;
	TCircle(TCircle&& oth) = default;
	TCircle& operator=(TCircle&& oth) = default;


	VOID Reset(CONST RECT rctScreen, LONG DeltaMax, std::mt19937* PRnd);
	VOID UpdateAndDraw(HDC dc, CONST RECT Screen);

private:
	HPEN PenHandle;
	HPEN PenErase;
	LONG DeltaX;
	LONG DeltaY;
	LONG PositionX;
	LONG PositionY;
	LONG Radius;


	BOOL DrawCircle(HDC dc);
	VOID MoveByAxis(LONG& Pos, LONG& Delta, LONG R, LONG BorderHi, LONG BorderLo);
	LONG RandomValueGet(CONST LONG min, CONST LONG max, std::mt19937* PRnd);
};

class IScreenSaverCirclesSettings
{
public:
	// BackgroundColorRGB
	virtual VOID BackgroundColorRGBSet(COLORREF n) const abstract;
	virtual COLORREF BackgroundColorRGBGet(COLORREF Default) const abstract;
	// FrameTimerInterval
	virtual VOID TimerIntervalSet(DWORD n) const abstract;
	virtual DWORD TimerIntervalGet(DWORD Default) const abstract;
	// MouseSence
	virtual VOID MouseSenceSet(DWORD n) const abstract;
	virtual DWORD MouseSenceGet(DWORD Default) const abstract;
	// CirclesCount
	virtual VOID CirclesCountSet(DWORD n) const abstract;
	virtual DWORD CirclesCountGet(DWORD Default) const abstract;
	// CirclesColorRGB
	virtual VOID CirclesColorRGBSet(COLORREF n) const abstract;
	virtual COLORREF CirclesColorRGBGet(COLORREF Default) const abstract;
};

class IScreenSaverCircleSettingsWindow
{
public:
	virtual VOID SettingsWindowGet(COLORREF* c, COLORREF* b, PDWORD cnt) abstract;
	virtual VOID SettingsWindowSet(COLORREF c, COLORREF b, DWORD cnt) abstract;	
};

class TScreensaverCircles:
	public ScreensaverBase,
	public IScreenSaverCircleSettingsWindow
{
public:
	COLORREF CirclesColorRGB;
	DWORD CirclesCount;
	IScreenSaverCirclesSettings* Settings;


	TScreensaverCircles(HINSTANCE hInst, int nCmdShow) noexcept;
	virtual ~TScreensaverCircles() noexcept;
	TScreensaverCircles(const TScreensaverCircles& oth) = delete;
	TScreensaverCircles& operator=(const TScreensaverCircles& oth) = delete;
	TScreensaverCircles(TScreensaverCircles&& oth) = delete;
	TScreensaverCircles& operator=(TScreensaverCircles&& oth) = delete;

	VOID SettingsRead();
	VOID SettingsSave() const;

private:
	std::vector<TCircle> CirclesArray;


	UINT OnConfigDialogShow() override;
	VOID OnBeforeWindowOperableShow() override;
	VOID OnWindowOperableShow() override;
	VOID OnWindowOperableHide() override;
	VOID OnNextFrame(HDC dc) override;
	VOID OnRedrawRegion(LPPAINTSTRUCT ps) override;
	LRESULT OnMessage(UINT message, WPARAM wParam, LPARAM lParam) override;
	VOID SettingsWindowGet(COLORREF* c, COLORREF* b, PDWORD cnt) override;
	VOID SettingsWindowSet(COLORREF c, COLORREF b, DWORD cnt) override;
	static INT_PTR CALLBACK ConfigDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
