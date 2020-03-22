//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#include "stdafx.h"

#include <thread>
#include "logic/KeyboardUtils.h"
#include "logic/MouseUtils.h"
#include "logic/RampUpCursorMover.h"

namespace neatmouse {
namespace logic {

//---------------------------------------------------------------------------------------------------------------------
void RampUpCursorMover::moveAsync(LONG dx, LONG dy)
{
	stopMove();
	std::thread(std::ref(*this), dx, dy).detach();
}


//---------------------------------------------------------------------------------------------------------------------
void RampUpCursorMover::stopMove()
{
	m_condition.notify_all();
}


//---------------------------------------------------------------------------------------------------------------------
void RampUpCursorMover::operator() (LONG dx, LONG dy)
{
	const unsigned long kMinDelay = KeyboardUtils::GetKeyboardInitialDelay();
	const unsigned long kTick = KeyboardUtils::GetKeyboardRepeatPeriod();
	std::unique_lock<std::mutex> lk(m_mutex);

	const auto & waitTime = std::chrono::milliseconds(kTick);
	for (unsigned long i = 0; i < kMinDelay; i += kTick)
	{
		MouseUtils::MouseMove(dx, dy);
		if (m_moveCallback) m_moveCallback();
		if (m_condition.wait_for(lk, waitTime) == std::cv_status::no_timeout)
		{
			break;
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
void RampUpCursorMover::setMoveCallback(const MoveCallback_t & callbackFn)
{
	m_moveCallback = callbackFn;
}

}}