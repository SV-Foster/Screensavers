/***

Copyright 2023, SV Foster. All rights reserved.

License:
	This program is free for personal, educational and/or non-profit usage

Revision History:

***/

#include <windows.h>
#include <commctrl.h>
#include <algorithm>
#include "SharedHeaders.h"
#include "ScreensaverBase.h"
// Stub module that fakes multiple monitor apis on Win32 OSes without them:
// #define COMPILE_MULTIMON_STUBS
// #include <multimon.h>


ScreensaverBase::ScreensaverBase(HINSTANCE hInst, int nCmdShow) noexcept:
	OperatingMode(COMMAND::CMD_FULLSCREEN),
	ProcessInstance(hInst),
	BackgroundColorRGB(DefaultBackgroundColorRGB),
	FrameTimerInterval(DefaultFrameTimerInterval),
	MouseSence(DefaultMouseSense),
	WindowOperableShowState(nCmdShow),
	WindowOperableHandle(NULL),
	WindowOperableRect({ 0 }),
	WindowParentHandle(NULL),
	WindowParentRect({ 0,0,152,112 }),
	ScrVirtRect({0}),
	ScrVirtX1(0),
	ScrVirtY1(0),
	ScrVirtX2(0),
	ScrVirtY2(0),
	ScrVirtWidth(0),
	SrcVirtHeight(0),
	MousePositionStart({0,0}),
	WaitForCPL(FALSE),
	ApplicationIconResourceID(0)
{
	INITCOMMONCONTROLSEX icce = { 0 };


	icce.dwSize = sizeof(icce);
	icce.dwICC = ICC_TAB_CLASSES;
	InitCommonControlsEx(&icce);

	return;
}

ScreensaverBase::~ScreensaverBase() noexcept
{
	return;
}

UINT ScreensaverBase::Run(COMMAND Command, HWND WindowParent, WORD AppIconID)
{
	UINT Result = EXIT_SUCCESS;
	DWORD dwStyle;
	DWORD dwExStyle;
	MSG msg = { 0 };
	WNDCLASSEX wcex = { 0 };
	ATOM ClassReg;
	BOOL CallResult;
	LPTSTR WindowTitle;


	// save data in class members
	this->OperatingMode = Command;
	this->WindowParentHandle = WindowParent;
	this->ApplicationIconResourceID = AppIconID;

	// get virtual screen (combination of all monitors in the system) dimensions from Windows
	this->ScrVirtX1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
	this->ScrVirtY1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
	this->ScrVirtWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	this->SrcVirtHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	this->ScrVirtX2 = this->ScrVirtWidth - std::abs(this->ScrVirtX1);
	this->ScrVirtY2 = this->SrcVirtHeight - std::abs(this->ScrVirtY1);

	// if value is NULL, GetDC retrieves the DC for the entire screen
	HDC hdc = GetDC(NULL);
	GetClipBox(hdc, &this->ScrVirtRect);
	ReleaseDC(NULL, hdc);

	// set some variables for following operations
	switch(this->OperatingMode)
	{
	case COMMAND::CMD_CONFIG:
		if (!WindowParentHandleCheck())
			return EXIT_FAILURE;

		return OnConfigDialogShow();

	case COMMAND::CMD_PREVIEW:
		if (!WindowParentHandleCheck())
			return EXIT_FAILURE;

		dwStyle = WS_CHILD | WS_CLIPCHILDREN; // not WS_VISIBLE for now
		dwExStyle = 0;
		WindowTitle = (LPTSTR)WindowTitlePreview;
		GetClientRect(this->WindowParentHandle, &this->WindowParentRect);
		AdjustWindowRect(&this->WindowParentRect, dwStyle, FALSE);
		
		break;

	case COMMAND::CMD_FULLSCREEN:
		dwStyle = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS; // not WS_VISIBLE for now
#ifndef _DEBUG
		dwExStyle = WS_EX_TOPMOST;
#else
		dwExStyle = 0;
#endif
		WindowTitle = (LPTSTR)WindowTitleOperatiing;
		this->WindowParentHandle = HWND_DESKTOP;
		this->WindowParentRect = this->ScrVirtRect;

		break;

	default:
		return EXIT_FAILURE;

	};

	// window class
	wcex.cbSize = sizeof(wcex);
	if (this->ApplicationIconResourceID)
		wcex.hIcon = LoadIcon(this->ProcessInstance, MAKEINTATOM(this->ApplicationIconResourceID));
	// If hIconSm is NULL, the system searches the icon resource specified by the hIcon member for an icon of
	// the appropriate size to use as the small icon
	// wcex.hIconSm = LoadIcon(this->ProcessInstance, MAKEINTATOM(this->ApplicationIconResourceID));
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
	wcex.lpfnWndProc = WindowOperableProc;
	wcex.hInstance = this->ProcessInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(this->BackgroundColorRGB);
	wcex.lpszClassName = WindowClassName;

	ClassReg = RegisterClassEx(&wcex);
	if (ClassReg == NULL)
		return EXIT_FAILURE;

	// create window, but don't show
	this->WindowOperableHandle = CreateWindowEx
	(
		dwExStyle,
		WindowClassName,
		WindowTitle,
		dwStyle,
		this->WindowParentRect.left,
		this->WindowParentRect.top,
		RectToWidth(this->WindowParentRect),
		RectToHeight(this->WindowParentRect),
		this->WindowParentHandle,
		NULL,
		this->ProcessInstance,
		NULL
	);
	if (this->WindowOperableHandle == NULL)
		return EXIT_FAILURE;

	// set/get more window options
	SetProp(this->WindowOperableHandle, WindowObjPtrName, this); // reinterpret_cast<HANDLE>(this)
	GetClientRect(this->WindowOperableHandle, &this->WindowOperableRect);

	// let subclass create all structures required for a job
	OnBeforeWindowOperableShow();

	// parameter this->WindowOperableShowState is ignored the first time an application calls ShowWindow, 
	// if the program that launched the application provides a STARTUPINFO structure. Otherwise, the first 
	// time ShowWindow is called, the value should be the value obtained by the WinMain function in its nCmdShow parameter.
	ShowWindow(this->WindowOperableHandle, this->WindowOperableShowState);
	UpdateWindow(this->WindowOperableHandle);
	if (this->OperatingMode == COMMAND::CMD_FULLSCREEN)
	{
		ShowCursor(FALSE);
		GetCursorPos(&this->MousePositionStart);
		SetForegroundWindow(this->WindowOperableHandle);
	}

	// in preview mode, wait a little by entring a limited message loop, before proceeding
	if (this->OperatingMode == COMMAND::CMD_PREVIEW)
	{
		this->WaitForCPL = TRUE;

		// message to mark the end of the initial group of messages for our window
		PostMessage(this->WindowOperableHandle, WM_INIT_END, 0, 0);

		while (this->WaitForCPL)
		{
			CallResult = GetMessage(&msg, this->WindowOperableHandle, 0, 0);

			// if GetMessage returns FALSE, it's quitting time
			if (!CallResult)
			{
				// in case of an error post the quit message to handle it later
				PostQuitMessage(EXIT_FAILURE);
				break;
			}

			// If there is an error, the return value is -1
			if (CallResult == -1)
				PostQuitMessage(EXIT_FAILURE);

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	// notify subclass the window is ready and appeared on the screen
	OnWindowOperableShow();

	// main message loop
	while (TRUE)
	{
		CallResult = GetMessage(&msg, this->WindowOperableHandle, 0, 0);

		// function retrieves the WM_QUIT message, the return value is zero
		if (!CallResult)
			ExitFunction(static_cast<UINT>(msg.wParam));

		// If there is an error, the return value is -1
		if (CallResult == -1)
			ExitFunction(EXIT_FAILURE);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
function_end:
	// exitting
	OnWindowOperableHide();
	ShowCursor(TRUE);
	return Result;
}

LRESULT CALLBACK ScreensaverBase::WindowOperableProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IMsgTarget* pMsgTarget;


	// get the window object back
	pMsgTarget = static_cast<IMsgTarget*>(GetProp(hWnd, WindowObjPtrName));
	if (pMsgTarget)
		return pMsgTarget->OnWindowMessage(uMsg, wParam, lParam);

	// not our window
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT ScreensaverBase::OnWindowMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	//
	// window lifecycle
	//

	// a window is about to be hidden or shown
	case WM_SHOWWINDOW:
	{
		// If wParam is TRUE, the window is being shown. If wParam is FALSE, the window is being hidden.
		if (wParam)
			SetTimer(this->WindowOperableHandle, TimerIDEvent, this->FrameTimerInterval, NULL);
		else
			KillTimer(this->WindowOperableHandle, TimerIDEvent);

		break;
	}

	// a window is being destroyed. It is sent to the window procedure of the window being destroyed after the window is removed from the screen.
	case WM_DESTROY:
		RemoveProp(this->WindowOperableHandle, WindowObjPtrName);
		PostQuitMessage(EXIT_SUCCESS);

		break;

	// the last message in the init queue (our mark)
	case WM_INIT_END:
		// 0.5sec of idle time, then proceed
		SetTimer(this->WindowOperableHandle, TimerPreviewIDEvent, 500, NULL);

		return 0;

	// a timer expires. The message is posted by the GetMessage or PeekMessage function.
	case WM_TIMER:
		if ((wParam == TimerIDEvent) && (!this->WaitForCPL))
		{
			HDC hDC;

			hDC = GetDC(this->WindowOperableHandle);
			OnNextFrame(hDC);
			ReleaseDC(this->WindowOperableHandle, hDC);
			Sleep(0);
			
			return 0;
		}

		if (wParam == TimerPreviewIDEvent)
		{
			this->WaitForCPL = FALSE;
			KillTimer(this->WindowOperableHandle, TimerPreviewIDEvent);

			return 0;
		}

		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		BeginPaint(this->WindowOperableHandle, &ps);
		OnRedrawRegion(&ps);
		EndPaint(this->WindowOperableHandle, &ps);
		Sleep(0);

		break;
	}

	// mouse causes the cursor to move within a window and mouse input is not captured
	case WM_SETCURSOR:
		SetCursor(NULL);

		return TRUE; // return TRUE to halt further processing

	//
	// the things that kill the screensaver
	//
	
	// the cursor moves
	case WM_MOUSEMOVE:
		if(this->OperatingMode != COMMAND::CMD_FULLSCREEN)
			break;

		POINT MousePosition;
		GetCursorPos(&MousePosition);

		if(
		   (std::abs(MousePosition.x - this->MousePositionStart.x) > static_cast<long>(this->MouseSence)) ||
		   (std::abs(MousePosition.y - this->MousePositionStart.y) > static_cast<long>(this->MouseSence))
		  )
			PostCloseMessage();

		break;

	// mouse click
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONDBLCLK:

	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
	case WM_NCMBUTTONDOWN:
	case WM_NCMBUTTONDBLCLK:

	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONDBLCLK:

	case WM_XBUTTONDOWN:
	case WM_XBUTTONDBLCLK:
	case WM_NCXBUTTONDOWN:
	case WM_NCXBUTTONDBLCLK:

	case WM_MOUSEWHEEL:
	case WM_MOUSEHWHEEL:
		if (this->OperatingMode != COMMAND::CMD_FULLSCREEN)
			break;

		PostCloseMessage();

		break;

	// window being activated or the window being deactivated
	case WM_ACTIVATE:
		if (this->OperatingMode != COMMAND::CMD_FULLSCREEN)
			break;

		if (LOWORD(wParam) == WA_INACTIVE)
			PostCloseMessage();

		break;

	// a window belonging to a different application than the active window is about to be activated
	case WM_ACTIVATEAPP:
		// TRUE if the window is being activated; it is FALSE if the window is being deactivated
		if(wParam == FALSE)
			PostCloseMessage();

		break;

	// system/nonsystem key is pressed. A nonsystem key is a key that is pressed when the ALT key is not pressed.
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		if(this->OperatingMode != COMMAND::CMD_FULLSCREEN)
			break;

		PostCloseMessage();

		break;

	case WM_SYSCOMMAND:
		if (this->OperatingMode != COMMAND::CMD_FULLSCREEN)
			break;

		switch (wParam)
		{
		case SC_NEXTWINDOW: // Alt-tab
		case SC_PREVWINDOW: // shift-alt-tab
		case SC_SCREENSAVE: // more screensavers wants to run
		case SC_CLOSE:

			return FALSE;

		// monitor is shutting down or just plugged in
		case SC_MONITORPOWER:
			PostCloseMessage();

			break;
		}

		break;

	case WM_HELP:
	case WM_CONTEXTMENU:
		if (this->OperatingMode != COMMAND::CMD_PREVIEW)
			break;

		// pass to owner window
		if (this->WindowParentHandle)
			PostMessage(this->WindowParentHandle, message, reinterpret_cast<WPARAM>(this->WindowParentHandle), lParam);

		return TRUE;

	default:
		
		break;
   }

   return OnMessage(message, wParam, lParam);   
}

LRESULT ScreensaverBase::EndOnMessage(UINT message, WPARAM wParam, LPARAM lParam) const noexcept
{
	return DefWindowProc(this->WindowOperableHandle, message, wParam, lParam);
}

BOOL ScreensaverBase::PostCloseMessage() const noexcept
{
	return PostMessage(this->WindowOperableHandle, WM_CLOSE, 0, 0);
}

int ScreensaverBase::RectToWidth(RECT r) const noexcept
{
	return r.right - r.left;
}

int ScreensaverBase::RectToHeight(RECT r) const noexcept
{
	return r.bottom - r.top;
}

BOOL ScreensaverBase::WindowParentHandleCheck() noexcept
{
	if (!this->WindowParentHandle) // not set by the user
	{
		this->WindowParentHandle = GetForegroundWindow();
		return TRUE;
	}

	// was set, check window exists
	if (!IsWindow(this->WindowParentHandle))
		return FALSE;

	return TRUE;
}

ScreensaverBase::COMMAND ScreensaverBase::OperatingModeGet() noexcept { return this->OperatingMode; };
CONST ScreensaverBase::COMMAND ScreensaverBase::OperatingModeGet() const noexcept { return this->OperatingMode; };
HINSTANCE ScreensaverBase::ProcessInstanceGet() noexcept { return this->ProcessInstance; };
CONST HINSTANCE ScreensaverBase::ProcessInstanceGet() const noexcept { return this->ProcessInstance; };
int  ScreensaverBase::WindowOperableShowStateGet() noexcept { return this->WindowOperableShowState; };
CONST int  ScreensaverBase::WindowOperableShowStateGet() const noexcept { return this->WindowOperableShowState; };
HWND ScreensaverBase::WindowOperableHandleGet() noexcept { return this->WindowOperableHandle; };
CONST HWND ScreensaverBase::WindowOperableHandleGet() const noexcept { return this->WindowOperableHandle; };
RECT ScreensaverBase::WindowOperableRectGet() noexcept { return this->WindowOperableRect; };
CONST RECT ScreensaverBase::WindowOperableRectGet() const noexcept { return this->WindowOperableRect; };
HWND ScreensaverBase::WindowParentHandleGet() noexcept { return this->WindowParentHandle; };
CONST HWND ScreensaverBase::WindowParentHandleGet() const noexcept { return this->WindowParentHandle; };
RECT ScreensaverBase::WindowParentRectGet() noexcept { return this->WindowParentRect; };
CONST RECT ScreensaverBase::WindowParentRectGet() const noexcept { return this->WindowParentRect; };
int ScreensaverBase::ScrVirtX1Get() noexcept { return this->ScrVirtX1; };
CONST int ScreensaverBase::ScrVirtX1Get() const noexcept { return this->ScrVirtX1; };
int ScreensaverBase::ScrVirtY1Get() noexcept { return this->ScrVirtY1; };
CONST int ScreensaverBase::ScrVirtY1Get() const noexcept { return this->ScrVirtY1; };
int ScreensaverBase::ScrVirtX2Get() noexcept { return this->ScrVirtX2; };
CONST int ScreensaverBase::ScrVirtX2Get() const noexcept { return this->ScrVirtX2; };
int ScreensaverBase::ScrVirtY2Get() noexcept { return this->ScrVirtY2; };
CONST int ScreensaverBase::ScrVirtY2Get() const noexcept { return this->ScrVirtY2; };
int ScreensaverBase::ScrVirtWidthGet() noexcept { return this->ScrVirtWidth; };
CONST int ScreensaverBase::ScrVirtWidthGet() const noexcept { return this->ScrVirtWidth; };
int ScreensaverBase::SrcVirtHeightGet() noexcept { return this->SrcVirtHeight; };
CONST int ScreensaverBase::SrcVirtHeightGet() const noexcept { return this->SrcVirtHeight; };
