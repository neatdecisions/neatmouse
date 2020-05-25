//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//


#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>

#include "resource.h"

#include "NeatMouseWtlView.h"
#include "aboutdlg.h"
#include "MainFrm.h"
#include "CursorOverlay.h"
#include "EmulationNotifier.h"

#include "logic/HookThread.h"
#include "logic/MainSingleton.h"


CAppModule _Module;


//---------------------------------------------------------------------------------------------------------------------
int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	neatmouse::InitOverlay(_Module.m_hInst);

	// prepare supported translations
	const std::vector<neatcommon::system::LocaleUiDescriptor> locales
	{
		{ "cn", IDR_LANG_CHINESESIMPLIFIED, IDB_PNG_LANG_CN, ID_LANGUAGE_CN },
		{ "en", IDR_LANG_ENGLISH,           IDB_PNG_LANG_EN, ID_LANGUAGE_EN },
		{ "de", IDR_LANG_GERMAN,            IDB_PNG_LANG_DE, ID_LANGUAGE_DE },
		{ "fr", IDR_LANG_FRENCH,            IDB_PNG_LANG_FR, ID_LANGUAGE_FR },
		{ "it", IDR_LANG_ITALIAN,           IDB_PNG_LANG_IT, ID_LANGUAGE_IT },
		{ "pl", IDR_LANG_POLISH,            IDB_PNG_LANG_PL, ID_LANGUAGE_PL },
		{ "ua", IDR_LANG_UKRAINIAN,         IDB_PNG_LANG_UA, ID_LANGUAGE_UA },
		{ "gr", IDR_LANG_GREEK,             IDB_PNG_LANG_GR, ID_LANGUAGE_GR },
		{ "ro", IDR_LANG_ROMANIAN,          IDB_PNG_LANG_RO, ID_LANGUAGE_RO },
		{ "ru", IDR_LANG_RUSSIAN,           IDB_PNG_LANG_RU, ID_LANGUAGE_RU }
	};

	// check whether a copy of NeatMouse is already running
	switch (neatmouse::logic::MainSingleton::Instance().Init(locales))
	{
	case 1:
		AtlMessageBox(0, _("errors.mutex-msg"), _("errors.mutex-caption"), MB_ICONERROR | MB_OK);
		return 1;
	default:
		break;
	}

	neatmouse::CMainFrame wndMain;
	RECT rect{ 0, 0, 640, 480 };
	if (wndMain.CreateEx(0, rect, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX ) == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}
	neatmouse::logic::MainSingleton::Instance().SetEmulationNotifier(std::make_shared<neatmouse::EmulationNotifier>(wndMain));
	neatmouse::logic::MainSingleton::Instance().TriggerOverlay();
	const CIcon icon = AtlLoadIconImage(IDI_MAUSEMUL);

	wndMain.SetIcon(icon);

	WCHAR a[64] = L"NeatMouse";
	NOTIFYICONDATA nd{};
	nd.cbSize = sizeof(NOTIFYICONDATA);
	nd.hWnd = wndMain;
	wcscpy_s(nd.szTip, wcslen(a) + 1, a);
	nd.uCallbackMessage = NEAT_TRAY_CALLBACK;
	nd.uID = 10;
	nd.hIcon = icon;
	nd.uFlags = NIF_TIP | NIF_ICON | NIF_MESSAGE;

	if (neatmouse::logic::MainSingleton::Instance().GetMouseParams().minimizeOnStartup)
	{
		wndMain.ToggleVisible();
		nd.uFlags |= NIF_INFO;
		nd.dwInfoFlags = NIIF_INFO;
		wcscpy_s(nd.szInfo, 255, _("notify.balloon-start"));
		wcscpy_s(nd.szInfoTitle, 63, L"NeatMouse");
	}
	else
	{
		wndMain.ShowWindow(nCmdShow);
	}

	Shell_NotifyIcon(NIM_ADD, &nd);
	neatmouse::logic::HookThread::Initialize(_Module.m_hInst);

	int nRet = theLoop.Run();

	Shell_NotifyIcon(NIM_DELETE, &nd);
	neatmouse::UninitOverlay();
	_Module.RemoveMessageLoop();

	return nRet;
}


//---------------------------------------------------------------------------------------------------------------------
int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE /*hPrevInstance*/,
	_In_ LPWSTR lpstrCmdLine,
	_In_ int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));

	neatcommon::ui::CGdiPlusInitializer	gdiplusInit;
	gdiplusInit.Init();

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	#ifdef _DEBUG
		_CrtDumpMemoryLeaks();
	#endif

	return nRet;
}
