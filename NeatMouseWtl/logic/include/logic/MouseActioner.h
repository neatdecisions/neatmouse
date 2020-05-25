//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include "logic/MouseEntities.h"
#include "logic/RampUpCursorMover.h"

namespace neatmouse {
namespace logic {

/**
 * Descriptors of the currently pressed keyboard buttons
 */
struct KeyboardButtonsStatus
{
	volatile bool isLeftPressed = false;
	volatile bool isRightPressed = false;
	volatile bool isUpPressed = false;
	volatile bool isDownPressed = false;;
	volatile bool isLeftUpPressed = false;
	volatile bool isRightUpPressed = false;
	volatile bool isLeftDownPressed = false;
	volatile bool isRightDownPressed = false;
	bool isLeftBtnPressed = false;
	bool isRightBtnPressed = false;
	bool isMiddleBtnPressed = false;
	bool isUnbindBtnPressed = false;
};

/**
 * Class containing the main logic of transfering keyboard events into mouse actions
 */
class MouseActioner
{
public:
	MouseActioner();
	~MouseActioner();

	/**
	 * Process an event received from the keyboard hook callback
	 *
	 * @param event    KBDLLHOOKSTRUCT received in LowLevelKeyboardProc
	 * @param isKeyUp  event type received in LowLevelKeyboardProc
	 *
	 * @return  A boolean indicating whether the processing was successful and should not be chained to the system
	 */
	bool processAction(const KBDLLHOOKSTRUCT & event, bool isKeyUp);

	void activateEmulation(bool activate);
	bool isEmulationActivated();
	void reset();
	void setMouseParams(const MouseParams& mouseParams);

private:
	/**
	 * Terminate "sticky button" (click & drag) mode if (leads to the generation of "Mouse Up" even if the mode was on)
	 */
	void resetStickyButton();

	/**
	 * Process a Key Up event
	 *
	 * @param vk  Virtual key code to process (after preprocessKey)
	 *
	 * @return  True if the event was processed, false if it wasn't and its processing should be delegated to the system
	 */
	bool processKeyUp(KeyboardUtils::VirtualKey_t vk);

	/**
	 * Process a Key Down event
	 *
	 * @param vk  Virtual key code to process (after preprocessKey)
	 *
	 * @return  True if the event was processed, false if it wasn't and its processing should be delegated to the system
	 */
	bool processKeyDown(KeyboardUtils::VirtualKey_t vk);

	/**
	 * Check the pressed status of the provided modifier button and indicates whether the current keyboard event should be
	 * processed further.
	 *
	 * @param vk                Virtual key code which is being currently processed (after preprocessKey)
	 * @param modifier          Virtual key code of a modifier which should be checked (ex. Left Alt)
	 * @param isKeyUp           Flag indicating whether we're processing Key Up (true) or Key Down (false) event
	 * @param isNumlockSpecial  KBDLLHOOKSTRUCT received by the LowLevelKeyboardProc
	 * @param oValue            Output value indicating whether the key defined by [modifier] is pressed.
	 *                          Updated only it is possible to deduce from the provided data.
   *
	 * @return  True if the current event should be processed further (i.e. [vk] is not a [modifier]), false otherwise
	 */
	bool checkModifierButtonDown(int vk, int modifier, bool isKeyUp, bool isNumlockSpecial, bool & oValue);

	/**
	 * Deduce a virtual key code of key being processed from the information provided in the KBDLLHOOKSTRUCT.
	 *
	 * @param event  KBDLLHOOKSTRUCT received by the LowLevelKeyboardProc
   *
	 * @return  A pair containing virtual key code and a boolean indicating if we're processing a key from the numerical
	 *          keyboard with a Shift key pressed.
	 */
	std::pair<KeyboardUtils::VirtualKey_t, bool> preprocessKey(const KBDLLHOOKSTRUCT & event);

	RampUpCursorMover _rampUpCursorMover;
	KeyboardButtonsStatus _keyboardStatus;
	MouseParams _mouseParams;

	enum class LastShift_t
	{
		kUnknown,
		kLeft,
		kRight
	};

	LastShift_t _lastShift = LastShift_t::kUnknown;
	bool _isEmulationActivated = false;
	NeatMouseButton _stickyButton = NMB_None;

	bool _isStickyButtonPressed = false;
	bool _isActivationButtonPressed = false;
	bool _isAlternativeSpeedButtonPressed = false;
	bool _ignoreNextStickyKeyDown = false;
};

}}