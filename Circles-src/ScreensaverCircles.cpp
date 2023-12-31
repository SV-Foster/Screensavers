/***

Copyright 2023, SV Foster. All rights reserved.

License:
	This program is free for personal, educational and/or non-profit usage

Revision History:

***/

#include <windows.h>
#include <commctrl.h>
#include <memory>
#include <vector>
#include <random>
#include "resource.h"
#include "ScreensaverBase.h"
#include "ScreensaverCircles.h"

#pragma comment(lib, "comctl32.lib")


TScreensaverCircles::TScreensaverCircles(HINSTANCE hInst, int nCmdShow) noexcept:
	ScreensaverBase(hInst, nCmdShow), // superclass constructor call
	CirclesColorRGB(DefaultCircleColorRGB),
	CirclesCount(DefaultCirclesCount),
	Settings(nullptr),
	CirclesArray() // empty container constructor (default constructor)
{	
	return;
}

TScreensaverCircles::~TScreensaverCircles() noexcept
{
	return;
}

VOID TScreensaverCircles::SettingsRead()
{
	if (!this->Settings)
		return;

	this->BackgroundColorRGB = this->Settings->BackgroundColorRGBGet(DefaultBackgroundColorRGB);
	this->FrameTimerInterval = this->Settings->TimerIntervalGet(DefaultFrameTimerInterval);
	this->MouseSence = this->Settings->MouseSenceGet(DefaultMouseSense);
	this->CirclesCount = this->Settings->CirclesCountGet(DefaultCirclesCount);
	this->CirclesColorRGB = this->Settings->CirclesColorRGBGet(DefaultCircleColorRGB);
}

VOID TScreensaverCircles::SettingsSave() const
{
	if (!this->Settings)
		return;

	this->Settings->BackgroundColorRGBSet(this->BackgroundColorRGB);
	this->Settings->CirclesCountSet(this->CirclesCount);
	this->Settings->CirclesColorRGBSet(this->CirclesColorRGB);
}

VOID TScreensaverCircles::OnBeforeWindowOperableShow()
{
	DWORD cnt = this->CirclesCount;
	LONG Delta = DefaultDeltaMax;
	std::random_device rd; // initializing and destroying crypto API only once
	std::mt19937 RandomGen = std::mt19937(rd());


	if (OperatingModeGet() == COMMAND::CMD_PREVIEW)
	{
		cnt = CirclesCountPreview;
		Delta = DeltaMaxPreview;
	}

	this->CirclesArray.reserve(cnt);
	for (size_t i = 0; i < cnt; ++i)
	{
		this->CirclesArray.emplace_back(this->CirclesColorRGB, this->BackgroundColorRGB); // create new obj inplace in the dyn array
		this->CirclesArray.back().Reset(WindowOperableRectGet(), Delta, &RandomGen);
	}

}

VOID TScreensaverCircles::OnWindowOperableShow()
{
	return;
}

VOID TScreensaverCircles::OnWindowOperableHide()
{
	return;
}

VOID TScreensaverCircles::OnNextFrame(HDC dct)
{
	auto i = this->CirclesArray.begin();
	while (i != this->CirclesArray.end())
	{
		i->UpdateAndDraw(dct, WindowOperableRectGet());
		++i;
	}
}

VOID TScreensaverCircles::OnRedrawRegion(LPPAINTSTRUCT ps)
{
	return;
}

LRESULT TScreensaverCircles::OnMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	return EndOnMessage(message, wParam, lParam);
}

VOID TScreensaverCircles::SettingsWindowGet(COLORREF* c, COLORREF* b, PDWORD cnt)
{
	*b = this->BackgroundColorRGB;
	*c = this->CirclesColorRGB;
	*cnt = this->CirclesCount;
}

VOID TScreensaverCircles::SettingsWindowSet(COLORREF c, COLORREF b, DWORD cnt)
{
	this->BackgroundColorRGB = b;
	this->CirclesColorRGB = c;
	this->CirclesCount = cnt;
}

UINT TScreensaverCircles::OnConfigDialogShow()
{
	INT_PTR CallResult;
	LPARAM dwInitParam;
	
	CONST IScreenSaverCircleSettingsWindow* Interface = this; //reinterpret_cast<IScreenSaverCircleSettingsWindow*>(this);
	dwInitParam = reinterpret_cast<LPARAM>(Interface);

	CallResult = DialogBoxParam
	(
		this->ProcessInstanceGet(),
		MAKEINTRESOURCE(IDD_CONFIG),
		this->WindowParentHandleGet(),
		ConfigDialogProc, 
		dwInitParam
	);

	if (CallResult)
		SettingsSave();

	return EXIT_SUCCESS;
}

INT_PTR CALLBACK TScreensaverCircles::ConfigDialogProc
(
	HWND hwndDlg,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	static IScreenSaverCircleSettingsWindow* App = NULL;
	static COLORREF rgbClr;
	static COLORREF rgbBKClr;
	CHOOSECOLOR cc;
	LPDRAWITEMSTRUCT lpdis;
	COLORREF rgbCust[16] = { 0 };
	UINT bnFlags;
	DWORD nNumCircles;


	switch (uMsg) 
	{
	case WM_INITDIALOG:
		App = reinterpret_cast<IScreenSaverCircleSettingsWindow*>(lParam);
		if (App)
		{			
			App->SettingsWindowGet(&rgbClr, &rgbBKClr, &nNumCircles);
			SetDlgItemInt(hwndDlg, IDC_NUM_CIRCLES, nNumCircles, FALSE);

			return 1;
		}

		return 0;

	case WM_DRAWITEM:
		lpdis = reinterpret_cast<LPDRAWITEMSTRUCT>(lParam);
		bnFlags = DFCS_BUTTONPUSH;
		if (lpdis->itemState & ODS_SELECTED)
			bnFlags |= DFCS_PUSHED;

		DrawFrameControl(lpdis->hDC, &(lpdis->rcItem), DFC_BUTTON, bnFlags);
		InflateRect(&(lpdis->rcItem), -2, -2);
		switch (lpdis->CtlID)
		{
		case IDC_COLOR:
			SetBkColor(lpdis->hDC, rgbClr);
			break;

		case IDC_BKCOLOR:
			SetBkColor(lpdis->hDC, rgbBKClr);
			break;
			
		default:
			break;
		}
		
		ExtTextOut(lpdis->hDC, 0, 0, ETO_OPAQUE, &(lpdis->rcItem), NULL, 0, NULL);

		return 1;

	case WM_COMMAND:
		switch (wParam) 
		{
		case IDC_COLOR:
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hwndDlg;
			cc.hInstance = NULL;
			cc.rgbResult = rgbClr;
			cc.lpCustColors = &rgbCust[0];
			cc.Flags = CC_RGBINIT;
			cc.lCustData = NULL;
			cc.lpfnHook = NULL;
			cc.lpTemplateName = NULL;

			if (ChooseColor(&cc)) 
			{
				rgbClr = cc.rgbResult;
				InvalidateRect(hwndDlg, NULL, TRUE);
			}

			break;

		case IDC_BKCOLOR:
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hwndDlg;
			cc.hInstance = NULL;
			cc.rgbResult = rgbBKClr;
			cc.lpCustColors = &rgbCust[0];
			cc.Flags = CC_RGBINIT;
			cc.lCustData = NULL;
			cc.lpfnHook = NULL;
			cc.lpTemplateName = NULL;

			if (ChooseColor(&cc))
			{
				rgbBKClr = cc.rgbResult;
				InvalidateRect(hwndDlg, NULL, TRUE);
			}

			break;

		case IDOK:
			App->SettingsWindowSet(rgbClr, rgbBKClr, GetDlgItemInt(hwndDlg, IDC_NUM_CIRCLES, NULL, FALSE));
			EndDialog(hwndDlg, TRUE);
			
			break;

		case IDCANCEL:
			EndDialog(hwndDlg, FALSE);

			break;

		default:

			break;
		}

	default:

		break;
	}

	return 0;
}

TCircle::TCircle(COLORREF rgb, COLORREF backgr):
	PenHandle(NULL),
	PenErase(NULL),
	DeltaX(0),
	DeltaY(0),
	PositionX(0),
	PositionY(0),
	Radius(0)
{
	this->PenHandle = CreatePen(PS_SOLID, 1, rgb);
	this->PenErase = CreatePen(PS_SOLID, 1, backgr);
};

TCircle::~TCircle() noexcept
{
	DeleteObject(this->PenErase);
	DeleteObject(this->PenHandle);
};

VOID TCircle::Reset(CONST RECT rctScreen, LONG DeltaMax, std::mt19937* PRnd)
{
	int ScreenWidth;


	ScreenWidth = rctScreen.right - rctScreen.left;
	this->PositionX = RandomValueGet(this->Radius * 2, this->Radius * 10, PRnd);
	this->PositionY = RandomValueGet(this->Radius * 2, this->Radius * 10, PRnd);
	this->Radius = max(ScreenWidth / 200, CircleRadiusMin);
	this->DeltaX = RandomValueGet(1, DeltaMax, PRnd);
	this->DeltaY = RandomValueGet(1, DeltaMax, PRnd);
}

VOID TCircle::UpdateAndDraw(HDC dc, CONST RECT Screen)
{
	// fill old drowing with backgound color	
	SelectObject(dc, GetStockObject(NULL_BRUSH));
	SelectObject(dc, this->PenErase);
	DrawCircle(dc);

	// calc
	MoveByAxis(this->PositionX, this->DeltaX, this->Radius, Screen.right, Screen.left);
	MoveByAxis(this->PositionY, this->DeltaY, this->Radius, Screen.bottom, Screen.top);

	// draw new circle
	SelectObject(dc, this->PenHandle);
	DrawCircle(dc);
}

VOID TCircle::MoveByAxis(LONG& Pos, LONG& Delta, LONG R, LONG BorderHi, LONG BorderLo)
{
	// move it
	Pos += Delta;

	// collision w/borders detection
	if ((Pos + R) >= BorderHi)
	{
		Delta *= -1;
		Pos = BorderHi - R;

		return;
	}

	if ((Pos - R) <= BorderLo)
	{
		Delta *= -1;
		Pos = BorderLo + R;

		return;
	}
}

BOOL TCircle::DrawCircle(HDC dc)
{
	return Ellipse
	(
		dc,
		this->PositionX - this->Radius,
		this->PositionY - this->Radius,
		this->PositionX + this->Radius,
		this->PositionY + this->Radius
	);
}

LONG TCircle::RandomValueGet(CONST LONG min, CONST LONG max, std::mt19937* PRnd)
{
	std::uniform_int_distribution<LONG> distribution(min, max);
	return distribution(*PRnd);
}
