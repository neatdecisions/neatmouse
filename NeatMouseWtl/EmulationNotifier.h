//
// Copyright © 2016 Neat Decisions. All rights reserved.
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
	EmulationNotifier(HWND mainWindow);
	virtual void Notify(bool enabled);
	virtual void RefreshOverlay(bool enabled);

private:
	HWND hwndMainWindow;
};

} // namespace neatmouse