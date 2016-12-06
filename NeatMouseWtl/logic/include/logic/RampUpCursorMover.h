//
// Copyright © 2016 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include <condition_variable>
#include <mutex>

namespace neatmouse {
namespace logic {

class RampUpCursorMover
{
public:
	void moveAsync(LONG dx, LONG dy);
	void stopMove();
	void operator() (LONG dx, LONG dy);

private:
	std::condition_variable _condition;
	std::mutex _mutex;
};

}}