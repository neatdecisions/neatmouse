//
// Copyright © 2016 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include <map>
#include <WTypes.h>

namespace neatmouse {
namespace logic {

#define VK_NUMPADENTER -VK_RETURN

/**
 * Utility class for interactions with the keyboard
 */
class KeyboardUtils
{
public:
	using ScanCode_t   = unsigned short;
	using VirtualKey_t = int;
	
	/** Return a scan code of the key using its virtual key code
	 *  A special processing of the numerical keyboard is taken into account
	 */
	static ScanCode_t VirtualKeyToScanCode(VirtualKey_t vk);

	/** Return a virtual key code of the key using its scan code
	 *  A special processing of the numerical keyboard is taken into account
	 */
	static VirtualKey_t ScanCodeToVirtualKey(ScanCode_t sc);

	/** Return a textual name of the key using its virtual key code and scan code
	 */
	static std::wstring GetKeyName(VirtualKey_t vk, ScanCode_t sc);

	/** Generate a key down / key up event
	 */
	static void KeyPress(VirtualKey_t vk, bool doUp);

	/** Check if key is pressed
	 */
	static bool IsKeyDown(VirtualKey_t vk);

	/** Return a virtual key code of the key taking into account NumLock status and assuming that Shift is pressed
	 */
	static VirtualKey_t TransformNumpadWithShift(VirtualKey_t vk, ScanCode_t sc);	
	
	/** Return the keyboard repeat delay (in ms).
	 *  Normally this should correspond to the delay between the first and 
	 *  the second WM_KEYDOWN messages which are sent when a button is pressed.
	 */
	static unsigned long GetKeyboardInitialDelay();

	/** Return the delay (in ms) between two consecutive WM_KEYDOWN messages 
	  * (starting from the 2nd message; to get the delay between 1st and 2nd,
		* GetKeyboardInitialDelay() should be used instead.
	  */
	static unsigned long GetKeyboardRepeatPeriod();

private:
	KeyboardUtils() = delete;
	~KeyboardUtils() = delete;

	static const std::map<VirtualKey_t, std::wstring> kVirtualKeyToNameMap;
};

}}