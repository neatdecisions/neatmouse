//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#pragma once


// Change these values to use different versions
#define WINVER        0x0501
#define _WIN32_WINNT  0x0501
#define _WIN32_IE     0x0600
#define _RICHEDIT_VER 0x0300
#define OEMRESOURCE

#define ID_TOOLBAR_START            10000

#define ID_TOOLBAR_COMBOPRESETS     ID_TOOLBAR_START + 2
#define ID_TOOLBAR_ADDPRESET        ID_TOOLBAR_START + 3
#define ID_TOOLBAR_REMOVEPRESET     ID_TOOLBAR_START + 4
#define ID_TOOLBAR_SAVEPRESET       ID_TOOLBAR_START + 5
#define ID_TOOLBAR_ADVANCEDVIEW     ID_TOOLBAR_START + 6

#define ID_TOOLBAR_LANGUAGE         ID_TOOLBAR_START + 15
#define ID_TOOLBAR_HELP             ID_TOOLBAR_START + 16

#define ID_TOOLBAR_SEP1             ID_TOOLBAR_START + 17
#define ID_TOOLBAR_SEP2             ID_TOOLBAR_START + 18

#define ID_COMBO_PRESETS            ID_TOOLBAR_START + 100

#define NEAT_TRAY_CALLBACK          WM_USER + 1000

#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <Gdiplus.h>

#include <atlmisc.h>
#include <atlctrls.h>
#include <atlcrack.h>
#include <atlctrlx.h>
#include <atltheme.h>
#include <atlframe.h>

#include <assert.h>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

#include "logic/MainSingleton.h"
#include "neatcommon/system/Helpers.h"

#define ASSERT(x) assert(x)

HBITMAP SafeLoadPng(UINT id);

LPCTSTR _(const std::string & name);