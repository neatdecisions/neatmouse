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

namespace neatmouse {
namespace logic {

struct IEmulationNotifier
{
	using Ptr = std::shared_ptr<IEmulationNotifier>;
	virtual void Notify(bool enabled) = 0;
	virtual void TriggerOverlay(bool enabled) = 0;
	virtual void UpdateOverlay() = 0;
	virtual ~IEmulationNotifier() = default;
};

}}