//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include <memory>
#include <neatcommon/system/IniFiles.h>
#include "KeyboardUtils.h"

namespace neatmouse {
namespace logic {


struct MouseParams
{
public:
	MouseParams();
	explicit MouseParams(const std::wstring & name);

	LONG delta  = 20;
	LONG adelta = 1;
	std::vector<int> accelerationCurve = { 10, 20, 30, 50, 70, 90 };

	KeyboardUtils::VirtualKey_t VKEnabler         = VK_SCROLL;
	KeyboardUtils::VirtualKey_t VKMoveUp          = VK_NUMPAD8;
	KeyboardUtils::VirtualKey_t VKMoveDown        = VK_NUMPAD2;
	KeyboardUtils::VirtualKey_t VKMoveLeft        = VK_NUMPAD4;
	KeyboardUtils::VirtualKey_t VKMoveRight       = VK_NUMPAD6;
	KeyboardUtils::VirtualKey_t VKMoveLeftUp      = VK_NUMPAD7;
	KeyboardUtils::VirtualKey_t VKMoveRightUp     = VK_NUMPAD9;
	KeyboardUtils::VirtualKey_t VKMoveLeftDown    = VK_NUMPAD1;
	KeyboardUtils::VirtualKey_t VKMoveRightDown   = VK_NUMPAD3;
	KeyboardUtils::VirtualKey_t VKAccelerated     = kVKNone;
	KeyboardUtils::VirtualKey_t VKPressLB         = VK_NUMPAD0;
	KeyboardUtils::VirtualKey_t VKPressRB         = VK_NUMPADENTER;
	KeyboardUtils::VirtualKey_t VKPressMB         = VK_NUMPAD5;
	KeyboardUtils::VirtualKey_t VKWheelUp         = -VK_DIVIDE;
	KeyboardUtils::VirtualKey_t VKWheelDown       = VK_MULTIPLY;
	KeyboardUtils::VirtualKey_t VKActivationMod   = kVKNone;
	KeyboardUtils::VirtualKey_t VKStickyKey       = kVKNone;

	UINT modHotkey = VK_F10;
	UINT VKHotkey  = MOD_CONTROL | MOD_ALT;

	bool activateOnStartup = false;
	bool minimizeOnStartup = false;
	bool changeCursor      = false;
	bool showNotifications = true;

	const static int kVKNone = 0;

	std::wstring GetName() const;
	std::wstring GetFilePath() const;
	bool UseHotkey() const;
	bool IsEqual(const MouseParams & mouseParams) const;

	bool BindingExists(int keyCode);
	bool Save();
	bool Save(const std::wstring & fileName);
	bool Load(const std::wstring & fileName);

	bool isModifierTaken(DWORD modifierId) const;

private:
	std::wstring m_name;
	std::wstring m_filePath;
};

}}