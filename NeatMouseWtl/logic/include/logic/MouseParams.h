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
	LONG delta;
	LONG adelta;

	KeyboardUtils::VirtualKey_t VKEnabler;
	KeyboardUtils::VirtualKey_t VKMoveUp;
	KeyboardUtils::VirtualKey_t VKMoveDown;
	KeyboardUtils::VirtualKey_t VKMoveLeft;
	KeyboardUtils::VirtualKey_t VKMoveRight;
	KeyboardUtils::VirtualKey_t VKMoveLeftUp;
	KeyboardUtils::VirtualKey_t VKMoveRightUp;
	KeyboardUtils::VirtualKey_t VKMoveLeftDown;
	KeyboardUtils::VirtualKey_t VKMoveRightDown;
	KeyboardUtils::VirtualKey_t VKAccelerated;
	KeyboardUtils::VirtualKey_t VKPressLB;
	KeyboardUtils::VirtualKey_t VKPressRB;
	KeyboardUtils::VirtualKey_t VKPressMB;
	KeyboardUtils::VirtualKey_t VKWheelUp;
	KeyboardUtils::VirtualKey_t VKWheelDown;

	UINT modHotkey;
	UINT VKHotkey;

	KeyboardUtils::VirtualKey_t VKActivationMod;
	KeyboardUtils::VirtualKey_t VKStickyKey;

	bool activateOnStartup;
	bool minimizeOnStartup;
	bool changeCursor;
	bool showNotifications;
	std::wstring Name;
	std::wstring FileName;

	const static int kVKNone = 0;

	bool UseHotkey() const;
	bool IsPreset() const;
	bool IsEqual(const MouseParams & mouseParams) const;
	explicit MouseParams(bool iIsPreset = false);
	bool BindingExists(int keyCode);
	bool Save();
	bool Save(const std::wstring & fileName);
	bool Load(const std::wstring & fileName);

	bool isModifierTaken(DWORD modifierId) const;

protected:
	bool isPreset;
};

}}