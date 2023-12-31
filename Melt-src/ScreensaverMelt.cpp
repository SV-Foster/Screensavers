/***

Copyright 2023, SV Foster. All rights reserved.

License:
    This program is free for personal, educational and/or non-profit usage

Revision History:

***/

#include <windows.h>
#include <commctrl.h>
#include <memory>
#include <random>
#include <string>
#include "resource.h"
#include "ScreensaverBase.h"
#include "ScreensaverMelt.h"

#pragma comment(lib, "comctl32.lib")


ScreensaverMelt::ScreensaverMelt(HINSTANCE hInst, int nCmdShow):
	ScreensaverBase(hInst, nCmdShow) // superclass constructor call
{
	this->FrameTimerInterval = MeltFrameTimerInterval;
    ZeroMemory(&(this->DesktopBitmap), sizeof(HBITMAP));

    // Initialize random generator
    std::random_device rd;
    this->RandomGen = std::mt19937(rd());
}

ScreensaverMelt::~ScreensaverMelt() noexcept
{
	return;
}

VOID ScreensaverMelt::OnBeforeWindowOperableShow()
{
    HDC Desktop;
    HDC Buffer;

    Desktop = GetDC(HWND_DESKTOP);
    Buffer = CreateCompatibleDC(Desktop);
    this->DesktopBitmap = CreateCompatibleBitmap(Desktop, this->ScrVirtWidthGet(), this->SrcVirtHeightGet());
    SelectObject(Buffer, this->DesktopBitmap);
   
    // make screenshot of the desktop right before screensaver window appears
    BitBlt
    (
        Buffer,                 // to
        0,
        0,
        this->ScrVirtWidthGet(),
        this->SrcVirtHeightGet(),
        Desktop,              // from
        this->ScrVirtX1Get(),
        this->ScrVirtY1Get(),
        SRCCOPY
    );

    ReleaseDC(HWND_DESKTOP, Desktop);
    DeleteDC(Buffer);
}

VOID ScreensaverMelt::OnWindowOperableShow()
{
    HDC hdcWindow;
    HDC Buffer;


    hdcWindow = GetDC(this->WindowOperableHandleGet());
    Buffer = CreateCompatibleDC(hdcWindow);
    SelectObject(Buffer, this->DesktopBitmap);
    
    if (this->OperatingModeGet() == ScreensaverBase::COMMAND::CMD_FULLSCREEN)
    {
        // copy saved in OnBeforeWindowOperableShow() screenshot to the screensaver window
        BitBlt
        (
            hdcWindow,              // to
            0,
            0,
            this->ScrVirtWidthGet(),
            this->SrcVirtHeightGet(),
            Buffer,                 // from
            0,
            0,
            SRCCOPY
        );
    }
    else
    {
        // This is the best stretch mode
        SetStretchBltMode(hdcWindow, HALFTONE);

        // The source DC is the entire screen, and the destination DC is the current window (HWND)
        StretchBlt
        (
            hdcWindow,
            WindowOperableRectGet().left,
            WindowOperableRectGet().top,
            WindowOperableRectGet().right,
            WindowOperableRectGet().bottom,
            Buffer,
            0,
            0,
            ScrVirtWidthGet(),
            SrcVirtHeightGet(),
            SRCCOPY
        );
    }

    DeleteDC(Buffer);
    ReleaseDC(this->WindowOperableHandleGet(), hdcWindow);
}

VOID ScreensaverMelt::OnWindowOperableHide()
{
    DeleteObject(this->DesktopBitmap);
}

VOID ScreensaverMelt::OnNextFrame(HDC dc)
{
    int x, y, w;


    if (this->OperatingModeGet() == ScreensaverBase::COMMAND::CMD_FULLSCREEN)
    {
        x = RandomValueGet(0, this->ScrVirtWidthGet() - 2);
        y = RandomValueGet(5, 20);
        w = RandomValueGet(5, 200);
    }
    else
    {
        x = RandomValueGet(0, RectToWidth(this->WindowOperableRectGet()) - 2);
        y = RandomValueGet(4, 10);
        w = RandomValueGet(4, 10);
    }

	BitBlt(dc, x, y, w, this->SrcVirtHeightGet(), dc, x, 0, SRCCOPY);
}

VOID ScreensaverMelt::OnRedrawRegion(LPPAINTSTRUCT ps)
{
    return;
}

LRESULT ScreensaverMelt::ScreensaverMelt::OnMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    return EndOnMessage(message, wParam, lParam);
}

UINT ScreensaverMelt::OnConfigDialogShow()
{
    TCHAR Text[256] = { 0 };
    TCHAR Caption[256] = { 0 };


    LoadString(this->ProcessInstanceGet(), UIMSG_101_NO_OPTIONS, &Text[0], ARRAYSIZE(Text));
    LoadString(this->ProcessInstanceGet(), UIMSG_100_SCR_FNAME, &Caption[0], ARRAYSIZE(Caption));

    MessageBox(this->WindowParentHandleGet(), &Text[0], &Caption[0], MB_OK | MB_ICONEXCLAMATION);

	return EXIT_SUCCESS;
}

int ScreensaverMelt::RandomValueGet(const int min, const int max)
{
    std::uniform_int_distribution<LONG> distribution(min, max);
    return distribution(this->RandomGen);
}
