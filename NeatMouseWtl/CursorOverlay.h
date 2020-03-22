//
// Copyright © 2016 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#pragma once

namespace neatmouse {

void InitOverlay(HINSTANCE hinst);
void UninitOverlay();
void EnableIconOverlay();
void DisableIconOverlay();
void PostRedrawOverlay();

} // namespace neatmouse