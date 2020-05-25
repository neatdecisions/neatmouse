//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//


#include "stdafx.h"
#include "EmulationNotifier.h"

#include "CursorOverlay.h"

namespace neatmouse {

//---------------------------------------------------------------------------------------------------------------------
EmulationNotifier::EmulationNotifier(HWND mainWindow) : hwndMainWindow(mainWindow)
{}


//---------------------------------------------------------------------------------------------------------------------
void EmulationNotifier::Notify(bool enabled)
{
	NOTIFYICONDATA nd{};
	nd.cbSize = sizeof(NOTIFYICONDATA);
	nd.hWnd = hwndMainWindow;
	nd.uID = 10;
	nd.uFlags = NIF_INFO;
	nd.dwInfoFlags = NIIF_INFO;
	if (enabled)
	{
		wcscpy_s(nd.szInfo, 255, _("notify.balloon-enabled"));
	}	else
	{
		wcscpy_s(nd.szInfo, 255, _("notify.balloon-disabled"));
	}
	wcscpy_s(nd.szInfoTitle, 63, L"NeatMouse");

	Shell_NotifyIcon(NIM_MODIFY, &nd);
}


//---------------------------------------------------------------------------------------------------------------------
void EmulationNotifier::TriggerOverlay(bool enabled)
{
	if (enabled)
	{
		EnableIconOverlay();
	} else
	{
		DisableIconOverlay();
	}
}


//---------------------------------------------------------------------------------------------------------------------
void EmulationNotifier::UpdateOverlay()
{
	PostRedrawOverlay();
}


} // namespace neatmouse