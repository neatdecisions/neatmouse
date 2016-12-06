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
namespace logic {

/**
 * Utility class for interactions with the mouse
 */
class MouseUtils
{
public:
	static void MouseMove(LONG dx, LONG dy);
	static void MousePressMB(bool doUp);
	static void MousePressLB(bool doUp);
	static void MousePressRB(bool doUp);
	static void MouseWheel(bool toUser);

private:
	MouseUtils() = delete;
	~MouseUtils() = delete;
};

}}