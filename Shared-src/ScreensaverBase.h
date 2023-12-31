/***

Copyright 2023, SV Foster. All rights reserved.

License:
	This program is free for personal, educational and/or non-profit usage

Revision History:

***/

#pragma once

// MANDATORY string required in .RC file
// This string should contain a less than 25 char name/description of the
// screen saver.  This string is what will be seen by the user in the Control
// Panel's Desktop applet screen saver listbox
//#define IDS_DESCRIPTION      1

#define WindowClassName TEXT("WindowsScreenSaverClass")
#define WindowTitlePreview TEXT("Preview")
#define WindowTitleOperatiing TEXT("Screen Saver")
#define WindowObjPtrName TEXT("rgibmook")
#define TimerIDEvent 1
#define TimerPreviewIDEvent 2
#define DefaultFrameTimerInterval 16
#define DefaultBackgroundColorRGB RGB(0, 0, 0)
#define DefaultMouseSense 5
#define WM_INIT_END WM_USER


class IMsgTarget
{
public:
	virtual LRESULT OnWindowMessage(UINT message, WPARAM wParam, LPARAM lParam) abstract;
};

class ScreensaverBase:
	public IMsgTarget
{
public:
	enum class COMMAND
	{
		CMD_CONFIG,
		CMD_FULLSCREEN,
		CMD_PREVIEW
	};

	ScreensaverBase(HINSTANCE hInst, int nCmdShow) noexcept;
	virtual ~ScreensaverBase() noexcept;
	ScreensaverBase(const ScreensaverBase& oth) = delete;
	ScreensaverBase& operator=(const ScreensaverBase& oth) = delete;
	ScreensaverBase(ScreensaverBase&& oth) = delete;
	ScreensaverBase& operator=(ScreensaverBase&& oth) = delete;
	
	UINT Run(COMMAND Command, HWND WindowParent, WORD AppIconID = 0);
	BOOL PostCloseMessage() const noexcept;
	

protected:	
	COLORREF BackgroundColorRGB;
	DWORD FrameTimerInterval;
	DWORD MouseSence;

	virtual UINT OnConfigDialogShow() abstract;
	virtual VOID OnBeforeWindowOperableShow() abstract;
	virtual VOID OnWindowOperableShow() abstract;
	virtual VOID OnWindowOperableHide() abstract;
	virtual VOID OnNextFrame(HDC dc) abstract;
	virtual VOID OnRedrawRegion(LPPAINTSTRUCT ps) abstract;
	virtual LRESULT OnMessage(UINT message, WPARAM wParam, LPARAM lParam) abstract;
	LRESULT EndOnMessage(UINT message, WPARAM wParam, LPARAM lParam) const noexcept;

	int RectToWidth(RECT r) const noexcept;
	int RectToHeight(RECT r) const noexcept;

	BOOL WindowParentHandleCheck() noexcept;

	// R/O getters
	COMMAND OperatingModeGet() noexcept;
	CONST COMMAND OperatingModeGet() const noexcept;
	HINSTANCE ProcessInstanceGet() noexcept;
	CONST HINSTANCE ProcessInstanceGet() const noexcept;
	int  WindowOperableShowStateGet() noexcept;
	CONST int  WindowOperableShowStateGet() const noexcept;
	HWND WindowOperableHandleGet() noexcept;
	CONST HWND WindowOperableHandleGet() const noexcept;
	RECT WindowOperableRectGet() noexcept;
	CONST RECT WindowOperableRectGet() const noexcept;
	HWND WindowParentHandleGet() noexcept;
	CONST HWND WindowParentHandleGet() const noexcept;
	RECT WindowParentRectGet() noexcept;
	CONST RECT WindowParentRectGet() const noexcept;
	int ScrVirtX1Get() noexcept;
	CONST int ScrVirtX1Get() const noexcept;
	int ScrVirtY1Get() noexcept;
	CONST int ScrVirtY1Get() const noexcept;
	int ScrVirtX2Get() noexcept;
	CONST int ScrVirtX2Get() const noexcept;
	int ScrVirtY2Get() noexcept;
	CONST int ScrVirtY2Get() const noexcept;
	int ScrVirtWidthGet() noexcept;
	CONST int ScrVirtWidthGet() const noexcept;
	int SrcVirtHeightGet() noexcept;
	CONST int SrcVirtHeightGet() const noexcept;

private:
	COMMAND OperatingMode; // sets on Run()
	HINSTANCE ProcessInstance; // sets on Constructor()
	POINT MousePositionStart;	
	int  WindowOperableShowState; // sets on Constructor()
	HWND WindowOperableHandle;
	RECT WindowOperableRect;
	HWND WindowParentHandle; // sets on Run()
	RECT WindowParentRect;	
	int ScrVirtX1;
	int ScrVirtY1;
	int ScrVirtX2;
	int ScrVirtY2;
	int ScrVirtWidth;
	int SrcVirtHeight;
	RECT ScrVirtRect;
	BOOL WaitForCPL;
	WORD ApplicationIconResourceID;

	static LRESULT CALLBACK WindowOperableProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnWindowMessage(UINT message, WPARAM wParam, LPARAM lParam) final;
};
