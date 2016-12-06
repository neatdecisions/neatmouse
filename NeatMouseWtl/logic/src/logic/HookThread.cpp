//
// Copyright © 2016 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//


#include "stdafx.h"

#include "logic/HookThread.h"
#include "logic/KeyboardUtils.h"
#include "logic/MainSingleton.h"

#include <thread>

namespace neatmouse {
namespace logic {

//---------------------------------------------------------------------------------------------------------------------
void HookThread::Initialize(HINSTANCE hInst)
{
	std::thread(HookThread(), hInst).detach();
}


//---------------------------------------------------------------------------------------------------------------------
void HookThread::operator() (HINSTANCE hInst)
{
	KeyboardUtils::KeyPress(VK_CONTROL, false);
	KeyboardUtils::KeyPress(VK_CONTROL, true);

	HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, &KeyboardProc, hInst, 0);
	MSG msg;
	BOOL bRet = -1;
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			// Handle Error
		}
		else
		{ 
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	UnhookWindowsHookEx(hook);
	KeyboardUtils::KeyPress(VK_CONTROL, false);
	KeyboardUtils::KeyPress(VK_CONTROL, true);
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT CALLBACK HookThread::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	// MSDN docs specify that both LL keybd & mouse hook should return in this case.
	if (nCode != HC_ACTION) return CallNextHookEx(NULL, nCode, wParam, lParam);
	
	const KBDLLHOOKSTRUCT &event = *(PKBDLLHOOKSTRUCT)lParam;

	if (!MainSingleton::Instance().GetMouseActioner().processAction(event, (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)))
	{
		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	return 1;
}

}}