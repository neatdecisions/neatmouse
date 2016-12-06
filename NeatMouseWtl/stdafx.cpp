//
// Copyright © 2016 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//


#include "stdafx.h"

#if (_ATL_VER < 0x0700)
#include <atlimpl.cpp>
#endif //(_ATL_VER < 0x0700)

CDPI gMetrics;


//-----------------------------------------------------------------------------
HBITMAP SafeLoadPng(UINT id)
{
	return neatmouse::logic::MainSingleton::Instance().GetImageManager().GetBitmapFromPng(id);
}


//-----------------------------------------------------------------------------
LPCTSTR _(const std::string & name)
{
	return neatmouse::logic::MainSingleton::Instance().GetLocalizer().GetValue(name).c_str();
}