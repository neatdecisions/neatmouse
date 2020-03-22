//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>

namespace neatmouse {
namespace logic {

class RampUpCursorMover
{
public:
	using MoveCallback_t = std::function<void()>;
	void moveAsync(LONG dx, LONG dy);
	void stopMove();
	void operator() (LONG dx, LONG dy);
	void setMoveCallback(const MoveCallback_t & callbackFn);

private:
	std::condition_variable m_condition;
	std::mutex m_mutex;
	MoveCallback_t m_moveCallback;
};

}}