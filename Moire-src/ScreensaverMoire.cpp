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
#include "resource.h"
#include "ScreensaverBase.h"
#include "ScreensaverMoire.h"

#pragma comment(lib, "comctl32.lib")


ScreensaverMoire::ScreensaverMoire(HINSTANCE hInst, int nCmdShow) :
    ScreensaverBase(hInst, nCmdShow), // superclass constructor call
	NextFrameBitmap(0),
	NextFrameDC(0),
	RandomGen((std::random_device())()),
	Red(0),
	Green(0),
	Blue(0),
	TailErase(FALSE),
	CurrentColor(TColors::toRed),
	x1(0),
	x1Erase(0),
	x2(0),
	x2Erase(0),
	y1(0),
	y1Erase(0),
	y2(0),
	y2Erase(0),
	x1inc(0),
	x1incErase(0),
	x2inc(0),
	x2incErase(0),
	y1inc(0),
	y1incErase(0),
	y2inc(0),
	y2incErase(0)
{
	return;
}

ScreensaverMoire::~ScreensaverMoire() noexcept
{
    return;
}

VOID ScreensaverMoire::OnBeforeWindowOperableShow()
{
    HDC hdc;
    CONST HBRUSH hBrush = CreateSolidBrush(this->BackgroundColorRGB);
    CONST RECT BufferRect = this->WindowOperableRectGet();


	// create and fill memory frame buffer
	hdc = GetDC(this->WindowOperableHandleGet());
	this->NextFrameDC = CreateCompatibleDC(hdc);
    this->NextFrameBitmap = CreateCompatibleBitmap(hdc, RectToWidth(BufferRect), RectToHeight(BufferRect));

	SelectObject(this->NextFrameDC, this->NextFrameBitmap);
	FillRect(this->NextFrameDC, &BufferRect, hBrush);

	ReleaseDC(this->WindowOperableHandleGet(), hdc);
}

VOID ScreensaverMoire::OnWindowOperableShow()
{
	Reset();
}

VOID ScreensaverMoire::OnWindowOperableHide()
{
	DeleteObject(this->NextFrameBitmap);
	DeleteDC(this->NextFrameDC);
}

VOID ScreensaverMoire::Reset()
{
	CurrentColor = static_cast<TColors>(RandomValueGet(static_cast<int>(TColors::toRed), static_cast<int>(TColors::toRedFromPurple)));

	x1 = x1Erase = RandomValueGet(DefaultScreenBorderDelta, RectToWidth(this->WindowOperableRectGet()) - DefaultScreenBorderDelta);
	x2 = x2Erase = RandomValueGet(DefaultScreenBorderDelta, RectToWidth(this->WindowOperableRectGet()) - DefaultScreenBorderDelta);
	y1 = y1Erase = RandomValueGet(DefaultScreenBorderDelta, RectToHeight(this->WindowOperableRectGet()) - DefaultScreenBorderDelta);
	y2 = y2Erase = RandomValueGet(DefaultScreenBorderDelta, RectToHeight(this->WindowOperableRectGet()) - DefaultScreenBorderDelta);

	x1inc = x1incErase = RandomValueGet(-DefaultIncABS, DefaultIncABS);
	x2inc = x2incErase = RandomValueGet(-DefaultIncABS, DefaultIncABS);
	y1inc = y1incErase = RandomValueGet(-DefaultIncABS, DefaultIncABS);
	y2inc = y2incErase = RandomValueGet(-DefaultIncABS, DefaultIncABS);
}

VOID ScreensaverMoire::OnNextFrame(HDC dc)
{
	//
	// calculate next frame
	//
	// calculate color
	switch (this->CurrentColor)
	{
	case TColors::toRed:
		if (Red == 0xFF)
			CurrentColor = TColors::toYellow;
		else
			Red++;

		break;

	case TColors::toYellow:
		if (Green == 0xFF)
			CurrentColor = TColors::toGreen;
		else
			Green++;

		break;

	case TColors::toGreen:
		if (Red == 0)
			CurrentColor = TColors::toBluegreen;
		else
			Red--;

		break;

	case TColors::toBluegreen:
		if (Blue == 0xFF)
			CurrentColor = TColors::toBlue;
		else
			Blue++;

		break;

	case TColors::toBlue:
		if (Green == 0)
			CurrentColor = TColors::toPurple;
		else
			Green--;

		break;

	case TColors::toPurple:
		if (Red == 0xFF)
			CurrentColor = TColors::toRedFromPurple;
		else
			Red++;

		break;

	case TColors::toRedFromPurple:
		if (Blue == 0)
			CurrentColor = TColors::toYellow;
		else 
		{
			if (Blue == 0x80)
				TailErase = TRUE;
			Blue--;
		}

	}

	// draw new line
	DrawUpdateLine(&x1, &x2, &y1, &y2, &x1inc, &x2inc, &y1inc, &y2inc, RGB(Red, Green, Blue));

	// erase tail
	if (TailErase)
		DrawUpdateLine(&x1Erase, &x2Erase, &y1Erase, &y2Erase, &x1incErase, &x2incErase, &y1incErase, &y2incErase, this->BackgroundColorRGB);

	// call to redraw the window
	RedrawWindow(this->WindowOperableHandleGet(), NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

VOID ScreensaverMoire::DrawUpdateLine(int* x1i, int* x2i, int* y1i, int* y2i, int* x1inci, int* x2inci, int* y1inci, int* y2inci, COLORREF c) noexcept
{
	HPEN hPen;


	hPen = CreatePen(PS_SOLID, DefaultLineWidth, c);
	SelectObject(this->NextFrameDC, hPen);
	MoveToEx(this->NextFrameDC, *x1i, *y1i, NULL);
	LineTo(this->NextFrameDC, *x2i, *y2i);
	DeleteObject(hPen);

	*x1i += *x1inci;
	*x2i += *x2inci;
	*y1i += *y1inci;
	*y2i += *y2inci;
	if ((*x1i < DefaultScreenBorderDelta) || (*x1i > RectToWidth(this->WindowOperableRectGet()) - DefaultScreenBorderDelta))
		*x1inci = -*x1inci;
	if ((*x2i < DefaultScreenBorderDelta) || (*x2i > RectToWidth(this->WindowOperableRectGet()) - DefaultScreenBorderDelta))
		*x2inci = -*x2inci;
	if ((*y1i < DefaultScreenBorderDelta) || (*y1i > RectToHeight(this->WindowOperableRectGet()) - DefaultScreenBorderDelta))
		*y1inci = -*y1inci;
	if ((*y2i < DefaultScreenBorderDelta) || (*y2i > RectToHeight(this->WindowOperableRectGet()) - DefaultScreenBorderDelta))
		*y2inci = -*y2inci;
}

VOID ScreensaverMoire::OnRedrawRegion(LPPAINTSTRUCT ps)
{
	// copy new frame to screen
	BitBlt
	(
		ps->hdc,
		ps->rcPaint.left,
		ps->rcPaint.top,
		ps->rcPaint.right,
		ps->rcPaint.bottom,
		this->NextFrameDC,
		ps->rcPaint.left,
		ps->rcPaint.top,
		SRCCOPY
	);
}

LRESULT ScreensaverMoire::OnMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	return EndOnMessage(message, wParam, lParam);
}

UINT ScreensaverMoire::OnConfigDialogShow()
{
    TCHAR Text[256] = { 0 };
    TCHAR Caption[256] = { 0 };


    LoadString(this->ProcessInstanceGet(), UIMSG_101_NO_OPTIONS, &Text[0], ARRAYSIZE(Text));
    LoadString(this->ProcessInstanceGet(), UIMSG_100_SCR_FNAME, &Caption[0], ARRAYSIZE(Caption));

    MessageBox(this->WindowParentHandleGet(), &Text[0], &Caption[0], MB_OK | MB_ICONEXCLAMATION);

    return EXIT_SUCCESS;
}

int ScreensaverMoire::RandomValueGet(const int min, const int max)
{
    std::uniform_int_distribution<int> distribution(min, max);	
    return distribution(this->RandomGen);
}
