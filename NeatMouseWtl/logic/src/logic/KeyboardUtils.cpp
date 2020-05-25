//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//


#include "StdAfx.h"

#include "logic/KeyboardUtils.h"

namespace neatmouse {
namespace logic {

//=====================================================================================================================
// special scan codes
//=====================================================================================================================

const KeyboardUtils::ScanCode_t SC_INSERT      = 0x152;
const KeyboardUtils::ScanCode_t SC_DELETE      = 0x153;
const KeyboardUtils::ScanCode_t SC_HOME        = 0x147;
const KeyboardUtils::ScanCode_t SC_END         = 0x14F;
const KeyboardUtils::ScanCode_t SC_UP          = 0x148;
const KeyboardUtils::ScanCode_t SC_DOWN        = 0x150;
const KeyboardUtils::ScanCode_t SC_LEFT        = 0x14B;
const KeyboardUtils::ScanCode_t SC_RIGHT       = 0x14D;
const KeyboardUtils::ScanCode_t SC_PGUP        = 0x149;
const KeyboardUtils::ScanCode_t SC_PGDN        = 0x151;

const KeyboardUtils::ScanCode_t SC_NUMPADDOT   = 0x53;
const KeyboardUtils::ScanCode_t SC_NUMPAD0     = 0x52;
const KeyboardUtils::ScanCode_t SC_NUMPAD1     = 0x4F;
const KeyboardUtils::ScanCode_t SC_NUMPAD7     = 0x47;
const KeyboardUtils::ScanCode_t SC_NUMPAD5     = 0x4C;
const KeyboardUtils::ScanCode_t SC_NUMPAD8     = 0x48;
const KeyboardUtils::ScanCode_t SC_NUMPAD2     = 0x50;
const KeyboardUtils::ScanCode_t SC_NUMPAD4     = 0x4B;
const KeyboardUtils::ScanCode_t SC_NUMPAD6     = 0x4D;
const KeyboardUtils::ScanCode_t SC_NUMPAD9     = 0x49;
const KeyboardUtils::ScanCode_t SC_NUMPAD3     = 0x51;
const KeyboardUtils::ScanCode_t SC_NUMPADDIV   = 0x135;
const KeyboardUtils::ScanCode_t SC_NUMPADMULT  = 0x037;
const KeyboardUtils::ScanCode_t SC_NUMPADSUB   = 0x04A;
const KeyboardUtils::ScanCode_t SC_NUMPADADD   = 0x04E;
const KeyboardUtils::ScanCode_t SC_NUMLOCK     = 0x145;
const KeyboardUtils::ScanCode_t SC_NUMPADENTER = 0x11C;


//=====================================================================================================================
// KeyboardUtils
//=====================================================================================================================

//  key labels - translation needed?
const std::map<KeyboardUtils::VirtualKey_t, std::wstring> KeyboardUtils::kVirtualKeyToNameMap
{
	{VK_NUMPAD0,             L"Num 0"},
	{VK_NUMPAD1,             L"Num 1"},
	{VK_NUMPAD2,             L"Num 2"},
	{VK_NUMPAD3,             L"Num 3"},
	{VK_NUMPAD4,             L"Num 4"},
	{VK_NUMPAD5,             L"Num 5"},
	{VK_NUMPAD6,             L"Num 6"},
	{VK_NUMPAD7,             L"Num 7"},
	{VK_NUMPAD8,             L"Num 8"},
	{VK_NUMPAD9,             L"Num 9"},
	{VK_MULTIPLY,            L"Num *"},
	{VK_DIVIDE,              L"Num /"},
	{VK_ADD,                 L"Num +"},
	{VK_SUBTRACT,            L"Num -"},
	{VK_DECIMAL,             L"Num ."},
	{VK_NUMLOCK,             L"Num Lock"},
	{VK_SCROLL,              L"Scroll Lock"},
	{VK_CAPITAL,             L"Caps Lock"},
	{VK_ESCAPE,              L"Escape"},
	{VK_TAB,                 L"Tab"},
	{VK_SPACE,               L"Space"},
	{VK_BACK,                L"Backspace"},
	{VK_RETURN,              L"Enter"},
	{VK_NUMPADENTER,         L"Num Enter"},
	{VK_DELETE,              L"Num Del"},
	{VK_INSERT,              L"Num Ins"},
	{VK_CLEAR,               L"Num Clear"},
	{VK_UP,                  L"Num Up"},
	{VK_DOWN,                L"Num Down"},
	{VK_LEFT,                L"Num Left"},
	{VK_RIGHT,               L"Num Right"},
	{VK_HOME,                L"Num Home"},
	{VK_END,                 L"Num End"},
	{VK_PRIOR,               L"Num PgUp"},
	{VK_NEXT,                L"Num PgDn"},
	{VK_SNAPSHOT,            L"Print Screen"},
	{VK_CANCEL,              L"Ctrl-Break"},
	{VK_PAUSE,               L"Pause"},
	{VK_HELP,                L"Help"},
	{VK_SLEEP,               L"Sleep"},
	{VK_APPS,                L"Menu"},
	{VK_LCONTROL,            L"Left Ctrl"},
	{VK_RCONTROL,            L"Right Ctrl"},
	{VK_LSHIFT,              L"Left Shift"},
	{VK_RSHIFT,              L"Right Shift"},
	{VK_LMENU,               L"Left Alt"},
	{VK_RMENU,               L"Right Alt"},
	{VK_LWIN,                L"Left Win"},
	{VK_RWIN,                L"Right Win"},
	{VK_F1,                  L"F1"},
	{VK_F2,                  L"F2"},
	{VK_F3,                  L"F3"},
	{VK_F4,                  L"F4"},
	{VK_F5,                  L"F5"},
	{VK_F6,                  L"F6"},
	{VK_F7,                  L"F7"},
	{VK_F8,                  L"F8"},
	{VK_F9,                  L"F9"},
	{VK_F10,                 L"F10"},
	{VK_F11,                 L"F11"},
	{VK_F12,                 L"F12"},
	{VK_F13,                 L"F13"},
	{VK_F14,                 L"F14"},
	{VK_F15,                 L"F15"},
	{VK_F16,                 L"F16"},
	{VK_F17,                 L"F17"},
	{VK_F18,                 L"F18"},
	{VK_F19,                 L"F19"},
	{VK_F20,                 L"F20"},
	{VK_F21,                 L"F21"},
	{VK_F22,                 L"F22"},
	{VK_F23,                 L"F23"},
	{VK_F24,                 L"F24"},
	{VK_BROWSER_BACK,        L"Browser Back"},
	{VK_BROWSER_FORWARD,     L"Browser Forward"},
	{VK_BROWSER_REFRESH,     L"Browser Refresh"},
	{VK_BROWSER_STOP,        L"Browser Stop"},
	{VK_BROWSER_SEARCH,      L"Browser Search"},
	{VK_BROWSER_FAVORITES,   L"Browser Favorites"},
	{VK_BROWSER_HOME,        L"Browser Home"},
	{VK_VOLUME_MUTE,         L"Volume Mute"},
	{VK_VOLUME_DOWN,         L"Volume Down"},
	{VK_VOLUME_UP,           L"Volume_Up"},
	{VK_MEDIA_NEXT_TRACK,    L"Next Track"},
	{VK_MEDIA_PREV_TRACK,    L"Previous Track"},
	{VK_MEDIA_STOP,          L"Stop"},
	{VK_MEDIA_PLAY_PAUSE,    L"Play / Pause"},
	{VK_LAUNCH_MAIL,         L"Mail"},
	{VK_LAUNCH_MEDIA_SELECT, L"Media"},
	{VK_LAUNCH_APP1,         L"App1"},
	{VK_LAUNCH_APP2,         L"App2"}
};


//---------------------------------------------------------------------------------------------------------------------
KeyboardUtils::ScanCode_t KeyboardUtils::VirtualKeyToScanCode(VirtualKey_t vk)
{
	switch (vk)
	{
		case VK_NUMPAD0:     return SC_NUMPAD0;
		case VK_NUMPAD1:     return SC_NUMPAD1;
		case VK_NUMPAD2:     return SC_NUMPAD2;
		case VK_NUMPAD3:     return SC_NUMPAD3;
		case VK_NUMPAD4:     return SC_NUMPAD4;
		case VK_NUMPAD5:     return SC_NUMPAD5;
		case VK_NUMPAD6:     return SC_NUMPAD6;
		case VK_NUMPAD7:     return SC_NUMPAD7;
		case VK_NUMPAD8:     return SC_NUMPAD8;
		case VK_NUMPAD9:     return SC_NUMPAD9;
		case VK_DECIMAL:     return SC_NUMPADDOT;
		case VK_NUMLOCK:     return SC_NUMLOCK;
		case VK_DIVIDE:      return SC_NUMPADDIV;
		case VK_MULTIPLY:    return SC_NUMPADMULT;
		case VK_SUBTRACT:    return SC_NUMPADSUB;
		case VK_ADD:         return SC_NUMPADADD;
		case VK_NUMPADENTER: return SC_NUMPADENTER;
	}

	ScanCode_t sc = static_cast<ScanCode_t>( MapVirtualKey(vk, MAPVK_VK_TO_VSC) );
	if ( (VK_APPS == vk) || (VK_CANCEL == vk) || (VK_SNAPSHOT == vk) || (VK_DIVIDE == vk) || (VK_NUMLOCK == vk) )
	{
		sc |= KF_EXTENDED;
	}

	return sc;
}


//---------------------------------------------------------------------------------------------------------------------
KeyboardUtils::VirtualKey_t KeyboardUtils::ScanCodeToVirtualKey(ScanCode_t sc)
{
	switch (sc)
	{
		case SC_NUMLOCK:     return VK_NUMLOCK;
		case SC_NUMPADDIV:   return VK_DIVIDE;
		case SC_NUMPADMULT:  return VK_MULTIPLY;
		case SC_NUMPADSUB:   return VK_SUBTRACT;
		case SC_NUMPADADD:   return VK_ADD;
		case SC_NUMPADENTER: return VK_NUMPADENTER;
		case SC_NUMPADDOT:   return VK_DELETE;
		case SC_NUMPAD0:     return VK_INSERT;
		case SC_NUMPAD1:     return VK_END;
		case SC_NUMPAD2:     return VK_DOWN;
		case SC_NUMPAD3:     return VK_NEXT;
		case SC_NUMPAD4:     return VK_LEFT;
		case SC_NUMPAD5:     return VK_CLEAR;
		case SC_NUMPAD6:     return VK_RIGHT;
		case SC_NUMPAD7:     return VK_HOME;
		case SC_NUMPAD8:     return VK_UP;
		case SC_NUMPAD9:     return VK_PRIOR;
	}

	return MapVirtualKey(sc, MAPVK_VSC_TO_VK);
}


//---------------------------------------------------------------------------------------------------------------------
std::wstring KeyboardUtils::GetKeyName(VirtualKey_t vk, ScanCode_t sc)
{
	static const std::wstring kDefaultName;
	if ( (vk == 0) && (sc == 0) ) return kDefaultName;

	if (vk == 0) vk = ScanCodeToVirtualKey(sc);
	if (sc == 0) sc = VirtualKeyToScanCode(abs(vk));

	if (vk < 0)
	{
		sc |= KF_EXTENDED;
		if (vk != VK_NUMPADENTER) vk = abs(vk);
	}

	static const std::set<ScanCode_t> ignoreSet {
		SC_INSERT, SC_DELETE, SC_HOME, SC_END, SC_UP, SC_DOWN, SC_LEFT, SC_RIGHT, SC_PGUP, SC_PGDN };
	if ( ignoreSet.find(sc) == ignoreSet.end() )
	{
		auto it = kVirtualKeyToNameMap.find(vk);
		if (it != kVirtualKeyToNameMap.end()) return it->second;
	}

	constexpr size_t kBufSize = 100;
	wchar_t buf[kBufSize];
	if ( GetKeyNameText(sc << 16, buf, kBufSize) ) return std::wstring(buf);

	return kDefaultName;
}


//---------------------------------------------------------------------------------------------------------------------
void KeyboardUtils::KeyPress(VirtualKey_t vk, bool doUp)
{
	vk = abs(vk);
	ScanCode_t asc = VirtualKeyToScanCode(vk);
	BYTE aSC_lobyte = LOBYTE(asc);
	DWORD event_flags = HIBYTE(asc) ? KEYEVENTF_EXTENDEDKEY : 0;
	if (doUp)
	{
		event_flags |= KEYEVENTF_KEYUP;
	}
	keybd_event(static_cast<BYTE>(vk), aSC_lobyte, event_flags, 0);
}


//---------------------------------------------------------------------------------------------------------------------
KeyboardUtils::VirtualKey_t KeyboardUtils::TransformNumpadWithShift(VirtualKey_t vk, ScanCode_t sc)
{
	bool isNumLockOn = (GetAsyncKeyState(VK_NUMLOCK) & 1) || (GetKeyState(VK_NUMLOCK) & 1);

	// map: { scan code -> pair: 1) numpad's VK with NumLock off, 2) numpad's VK with NumLock on }
	static const std::map<ScanCode_t, std::pair<VirtualKey_t, VirtualKey_t> > numpadKeyMap =
	{
		{ SC_NUMPAD8,   {VK_UP, VK_NUMPAD8}     },
		{ SC_NUMPAD2,   {VK_DOWN, VK_NUMPAD2}   },
		{ SC_NUMPAD4,   {VK_LEFT, VK_NUMPAD4}   },
		{ SC_NUMPAD6,   {VK_RIGHT, VK_NUMPAD6}  },
		{ SC_NUMPAD7,   {VK_HOME, VK_NUMPAD7}   },
		{ SC_NUMPAD9,   {VK_PRIOR, VK_NUMPAD9}  },
		{ SC_NUMPAD1,   {VK_END, VK_NUMPAD1}    },
		{ SC_NUMPAD3,   {VK_NEXT, VK_NUMPAD3}   },
		{ SC_NUMPAD5,   {VK_CLEAR, VK_NUMPAD5}  },
		{ SC_NUMPADDOT, {VK_DELETE, VK_DECIMAL} },
		{ SC_NUMPAD0,   {VK_INSERT, VK_NUMPAD0} }
	};

	auto it = numpadKeyMap.find(sc);
	if (it != numpadKeyMap.end())
	{
		const auto & aVkPair = it->second;
		if ( isNumLockOn && (aVkPair.first == vk) )
		{
			vk = aVkPair.second;
		} else
		if ( !isNumLockOn && (aVkPair.second == vk) )
		{
			vk = aVkPair.first;
		}
	}

	return vk;
}


//---------------------------------------------------------------------------------------------------------------------
bool KeyboardUtils::IsKeyDown(VirtualKey_t vk)
{
	return (GetAsyncKeyState(vk) & KF_UP) || ((GetKeyState(vk) & KF_UP));
}


//---------------------------------------------------------------------------------------------------------------------
unsigned long KeyboardUtils::GetKeyboardInitialDelay()
{
	const DWORD kMinDelay = 250;
	// 0 -> 250ms, 3 -> 1s
	DWORD result = 0;
	if (!SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &result, 0)) result = 0;
	return kMinDelay + result * 250;
}


//---------------------------------------------------------------------------------------------------------------------
unsigned long KeyboardUtils::GetKeyboardRepeatPeriod()
{
	// retrieve the keyboard repeat - speed setting which is a value in the range
	// from 0 (approximately 2.5 repetitions per second) through 31 (approximately 30 repetitions per second),
	// and deduce the repeat period from this value
	DWORD result = 0;
	if (!SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &result, 0)) result = 15;
	return static_cast<unsigned long>( std::round(1000.f / (2.5f + result * (30.f - 2.5f) / 32.f)) );
}

}}