//
// Copyright © 2016 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include <string>

namespace neatcommon {
namespace system {


class AutorunManager
{
public:
	void enableAutostart(const std::wstring & iKey, bool value) const;
	bool getAutostartEnabled(const std::wstring & iKey) const;
};

}}