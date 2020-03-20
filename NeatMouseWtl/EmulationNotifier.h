//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include "logic/IEmulationNotifier.h"

namespace neatmouse {

struct EmulationNotifier : logic::IEmulationNotifier
{
	explicit EmulationNotifier(HWND mainWindow);
	void Notify(bool enabled) override;
	void RefreshOverlay(bool enabled) override;

private:
	HWND hwndMainWindow = NULL;
};

} // namespace neatmouse