//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//


#include "StdAfx.h"
#include "CursorOverlay.h"
#include "logic/MainSingleton.h"
#include "neatcommon/ui/CustomizedControls.h"
#include "resource.h"

namespace neatmouse {

namespace {
	HINSTANCE overlayInstance = NULL;
	HWND overlayHwnd = NULL;
	HHOOK mouseHook = NULL;
	HANDLE threadHandle = NULL;
	HBITMAP overlayBitmap = NULL;
	constexpr LPWSTR OVERLAY_WINDOW_NAME = L"NeatOverlay";
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if ((nCode < 0) || (wParam != WM_MOUSEMOVE)) CallNextHookEx(mouseHook, nCode, wParam, lParam);

	POINT pt;
	const PMOUSEHOOKSTRUCT pMouseStruct = reinterpret_cast<PMOUSEHOOKSTRUCT>(lParam);
	if (pMouseStruct != NULL)
	{
		pt = pMouseStruct->pt;
	}
	else
	{
		// should never happen...
		GetCursorPos(&pt);
	}

	const int dx = GetSystemMetrics(SM_CXCURSOR) / 2;
	const int dy = GetSystemMetrics(SM_CYCURSOR) / 2;

	SetWindowPos(overlayHwnd, HWND_TOPMOST, pt.x + dx, pt.y + dy, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}


//---------------------------------------------------------------------------------------------------------------------
void DrawOverlay(HWND hwndSplash, HBITMAP hbmpSplash)
{
	HDC	hdc = GetDC(hwndSplash);

	CDC dcImage;
	dcImage.CreateCompatibleDC(hdc);

	HBITMAP hBmpOld = dcImage.SelectBitmap(hbmpSplash);
	BLENDFUNCTION BlendFunction;
	BlendFunction.BlendOp = AC_SRC_OVER;
	BlendFunction.BlendFlags = 0;
	BlendFunction.SourceConstantAlpha = 0xFF;   // half transparent
	BlendFunction.AlphaFormat = AC_SRC_ALPHA;   // use bitmap alpha

	POINT pt = { 0, 0 };
	SIZE sz = { 16, 16 };
	UpdateLayeredWindow(hwndSplash, hdc, &pt, &sz, dcImage, &pt, RGB(0, 0, 0), &BlendFunction, ULW_ALPHA);

	dcImage.SelectBitmap(hBmpOld);

	ReleaseDC(hwndSplash, hdc);
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_CREATE:
		mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, overlayInstance, 0);
		if (!mouseHook)
		{
			PostMessage(hWnd, WM_DESTROY, 0, 0);
			break;
		}
		DrawOverlay(hWnd, overlayBitmap);
		break;
	case WM_CLOSE:
		UnhookWindowsHookEx(mouseHook);
		mouseHook = NULL;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
void InitOverlay(HINSTANCE hinst)
{
	overlayInstance = hinst;

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = overlayInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = OVERLAY_WINDOW_NAME;
	wcex.hIconSm = NULL;

	RegisterClassEx(&wcex);

	overlayBitmap = neatcommon::ui::AtlLoadGdiplusImage(IDB_PNG_NEATMOUSE, _T("PNG"));
}


//---------------------------------------------------------------------------------------------------------------------
void UninitOverlay()
{
	DisableIconOverlay();
	if (overlayBitmap) DeleteObject(overlayBitmap);
}


//---------------------------------------------------------------------------------------------------------------------
unsigned int WINAPI ThreadProc(void *)
{
	const int dx = GetSystemMetrics(SM_CXCURSOR) / 2;
	const int dy = GetSystemMetrics(SM_CYCURSOR) / 2;
	POINT p;
	GetCursorPos(&p);
	overlayHwnd = CreateWindowEx(
		WS_EX_NOACTIVATE | WS_EX_LAYERED,
		OVERLAY_WINDOW_NAME,
		NULL,
		WS_POPUP | WS_VISIBLE,
		p.x + dx,
		p.y + dy,
		16,
		16,
		NULL,
		NULL,
		overlayInstance,
		NULL);
	if (!overlayHwnd)
	{
		return 1;
	}

	ShowWindow(overlayHwnd, SW_SHOWNORMAL);
	UpdateWindow(overlayHwnd);

	SetWindowPos(overlayHwnd, HWND_TOPMOST, p.x + dx, p.y + dy, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
void EnableIconOverlay()
{
	if (overlayHwnd || threadHandle) DisableIconOverlay();
	threadHandle = (HANDLE)_beginthreadex(0, 0, ThreadProc, 0, 0, 0);
}


//---------------------------------------------------------------------------------------------------------------------
void DisableIconOverlay()
{
	SendMessage(overlayHwnd, WM_CLOSE, 0, 0);
	CloseHandle(threadHandle);

	overlayHwnd = NULL;
	threadHandle = 0;
}

} // namespace neatmouse