//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#include "StdAfx.h"


#include "logic/HookThread.h"
#include "logic/KeyboardUtils.h"
#include "logic/MainSingleton.h"
#include "logic/MouseActioner.h"
#include "logic/MouseUtils.h"

namespace neatmouse {
namespace logic {

//---------------------------------------------------------------------------------------------------------------------
MouseActioner::MouseActioner()
{
	// to ensure that overlay icon moves together with cursor when ramp-up movement is triggered
	_rampUpCursorMover.setMoveCallback([](){ MainSingleton::Instance().UpdateOverlay(); });
}


//---------------------------------------------------------------------------------------------------------------------
MouseActioner::~MouseActioner(void)
{
	resetStickyButton();
}


//---------------------------------------------------------------------------------------------------------------------
bool
MouseActioner::checkModifierButtonDown(int vk, int modifier, bool isKeyUp, bool isNumlockSpecial, bool & oValue)
{
	if (abs(vk) == modifier)
	{
		oValue = !isKeyUp;
		return true;
	}
	else
	{
		if (isNumlockSpecial && ((modifier == VK_LSHIFT) || (modifier == VK_RSHIFT)))
		{
			oValue = (( (_lastShift == LastShift_t::kLeft) && (modifier == VK_LSHIFT)) ||
			          ( (_lastShift == LastShift_t::kRight) && (modifier == VK_RSHIFT)));
		}
	}
	return false;
}


//---------------------------------------------------------------------------------------------------------------------
std::pair<KeyboardUtils::VirtualKey_t, bool>
MouseActioner::preprocessKey(const KBDLLHOOKSTRUCT & event)
{
	KeyboardUtils::VirtualKey_t vk = static_cast<KeyboardUtils::VirtualKey_t>(event.vkCode);
	KeyboardUtils::ScanCode_t sc = static_cast<KeyboardUtils::ScanCode_t>(event.scanCode);

	if ((event.flags & LLKHF_EXTENDED))
	{
		sc |= 0x100;
		vk = -vk;
	}
	constexpr KeyboardUtils::ScanCode_t SC_RSHIFT = 0x136;
	constexpr KeyboardUtils::ScanCode_t SC_RCONTROL = 0x11D;
	constexpr KeyboardUtils::ScanCode_t SC_RALT = 0x138;

	switch (abs(vk))
	{
		case VK_SHIFT:   vk = (sc == SC_RSHIFT) ? VK_RSHIFT : VK_LSHIFT; break;
		case VK_CONTROL: vk = (sc == SC_RCONTROL) ? VK_RCONTROL : VK_LCONTROL; break;
		case VK_MENU:    vk = (sc == SC_RALT) ? VK_RMENU : VK_LMENU; break;
	}

	if (abs(vk) == VK_LSHIFT)
	{
		_lastShift = LastShift_t::kLeft;
	}
	else if (abs(vk) == VK_RSHIFT)
	{
		_lastShift = LastShift_t::kRight;
	}

	bool isNumlockSpecialHandling = false;

	KeyboardUtils::VirtualKey_t vk1 = KeyboardUtils::TransformNumpadWithShift(abs(vk), sc);

	if (vk1 != abs(vk))
	{
		isNumlockSpecialHandling = true;
		vk = vk1;
	}

	return std::make_pair(vk, isNumlockSpecialHandling);
}


//---------------------------------------------------------------------------------------------------------------------
bool
MouseActioner::processAction(const KBDLLHOOKSTRUCT & event, bool isKeyUp)
{
	// ignore injected events
	if (event.flags & LLKHF_INJECTED)
	{
		reset();
		return false;
	}

	// if we're processing "Key Up" event and the key is our enabler (one of the locks), reset everything and return
	if (isKeyUp &&
	    (static_cast<logic::KeyboardUtils::VirtualKey_t>(event.vkCode) == _mouseParams.VKEnabler))
	{
		_isEmulationActivated = (GetKeyState(_mouseParams.VKEnabler) & 1);
		MainSingleton::Instance().NotifyEnabling(_isEmulationActivated);
		if (!_isEmulationActivated) reset();
		return false;
	}

	// if emulation is not activated, reset and return
	if (!isEmulationActivated())
	{
		reset();
		return false;
	}

	// retrieve an actual virtual key code of the key which is being processed
	const std::pair<KeyboardUtils::VirtualKey_t, bool> & aKeyPair = preprocessKey(event);
	const KeyboardUtils::VirtualKey_t vk = aKeyPair.first;
	const bool isNumlockSpecialHandling = aKeyPair.second;

	// if Activation Modifier has been set up, check whether is is pressed
	if (_mouseParams.VKActivationMod != MouseParams::kVKNone)
	{
		// if Activation Modifier has been pressed and it is the button we're currently processing, return true
		// to block further event processing
		if (checkModifierButtonDown(vk, _mouseParams.VKActivationMod, isKeyUp, isNumlockSpecialHandling, _isActivationButtonPressed))
		{
			if (!_isActivationButtonPressed && (_mouseParams.VKActivationMod != VK_RSHIFT) && ((_mouseParams.VKActivationMod != VK_LSHIFT)))
			{
				reset();
			}
			return false;
		}

		// if Activation Modifier is a Shift and we're working with the numerical keyboard, isNumlockSpecialHandling will indicate
		// that Shift is pressed - there is no other way of deducing it here since Windows send Shift's Key Up in this case
		if (isNumlockSpecialHandling &&
		    ( ( (_mouseParams.VKActivationMod == VK_RSHIFT) && (_lastShift == LastShift_t::kRight) ) ||
		      ( (_mouseParams.VKActivationMod == VK_LSHIFT) && (_lastShift == LastShift_t::kLeft) ) ) )
		{
			_isActivationButtonPressed = true;
		}

		// if Activation Modifier is not pressed, return false indicating that the processing should be done by the OS
		if (!_isActivationButtonPressed)
		{
			reset();
			return false;
		}
	}
	else
	{
		// no Activation Modifier: mark it as not pressed anyway
		_isActivationButtonPressed = false;
	}

	// update the status of Alternative Speed Modifier; if we're currently processing its event, nothing more to do - exit
	if (checkModifierButtonDown(vk, _mouseParams.VKAccelerated, isKeyUp, isNumlockSpecialHandling, _isAlternativeSpeedButtonPressed))
	{
		return false;
	}

	// updade the status of Sticky Key Modifier and check if anything else should be done
	if (checkModifierButtonDown(vk, _mouseParams.VKStickyKey, isKeyUp, isNumlockSpecialHandling, _isStickyButtonPressed))
	{
		// Special processing of numerical keyboard and Shift modifiers:
		// When a Shift key is pressed together with a key from a numerical keyboard, a Key Down even for Shift is sent twice.
		// Se want to skip the first keydown event so that the Sticky Key mode isn't reset immediately
		if (_isStickyButtonPressed && _ignoreNextStickyKeyDown)
		{
			_ignoreNextStickyKeyDown = false;
			return true;
		}

		// if we had Sticky Mode activated and a Sticky Modifier was pressed, this means a user wants to stop Sticky Mode
		if (_isStickyButtonPressed)
		{
			if (_stickyButton != NMB_None)
			{
				resetStickyButton();
			}
		}
		return false;
	}
	else
	{
		// special Shift + Numerical Keyboard  handling (see above)
		// if checkModifierButtonDown() returned false, and both _isStickyButtonPressed and isNumlockSpecialHandling are true,
		// this means that we're processing an event from the numerical keyboard with a Shift pressed; we want to ignore the
		// next Shift Key Down event since it will be fake one generated by Windows
		if (isNumlockSpecialHandling && _isStickyButtonPressed && (_mouseParams.VKStickyKey == VK_RSHIFT || _mouseParams.VKStickyKey == VK_LSHIFT))
		{
			_ignoreNextStickyKeyDown = true;
		}
	}

	const KeyboardButtonsStatus oldStatus = _keyboardStatus;
	const bool result = isKeyUp ? processKeyUp(vk) : processKeyDown(vk);

	// figure out the movement vector
	const LONG d = _isAlternativeSpeedButtonPressed ? _mouseParams.adelta : _mouseParams.delta;
	const LONG dx =
		((_keyboardStatus.isLeftPressed || _keyboardStatus.isLeftUpPressed || _keyboardStatus.isLeftDownPressed) ? - d : 0) +
		((_keyboardStatus.isRightPressed || _keyboardStatus.isRightUpPressed || _keyboardStatus.isRightDownPressed) ? d : 0);

	const LONG dy =
		((_keyboardStatus.isUpPressed || _keyboardStatus.isLeftUpPressed || _keyboardStatus.isRightUpPressed) ? - d : 0) +
		((_keyboardStatus.isDownPressed || _keyboardStatus.isLeftDownPressed || _keyboardStatus.isRightDownPressed) ? d : 0);

	// if at least one of the keys was already pressed, stop ramp-up cursor mover
	if ((_keyboardStatus.isLeftPressed && oldStatus.isLeftPressed) ||
	    (_keyboardStatus.isRightPressed && oldStatus.isRightPressed) ||
	    (_keyboardStatus.isUpPressed && oldStatus.isUpPressed) ||
	    (_keyboardStatus.isDownPressed && oldStatus.isDownPressed) ||
	    (_keyboardStatus.isLeftDownPressed && oldStatus.isLeftDownPressed) ||
	    (_keyboardStatus.isRightDownPressed && oldStatus.isRightDownPressed) ||
	    (_keyboardStatus.isLeftUpPressed && oldStatus.isLeftUpPressed) ||
	    (_keyboardStatus.isRightUpPressed && oldStatus.isRightUpPressed) )
	{
		_rampUpCursorMover.stopMove();
	}

	// if at least one key changed its status to Pressed right now, invoke ramp-up cursor mover
	// to avoid keyboard repeat delay
	if ((_keyboardStatus.isLeftPressed && !oldStatus.isLeftPressed) ||
	    (_keyboardStatus.isRightPressed && !oldStatus.isRightPressed) ||
	    (_keyboardStatus.isUpPressed && !oldStatus.isUpPressed) ||
	    (_keyboardStatus.isDownPressed && !oldStatus.isDownPressed) ||
	    (_keyboardStatus.isLeftDownPressed && !oldStatus.isLeftDownPressed) ||
	    (_keyboardStatus.isRightDownPressed && !oldStatus.isRightDownPressed) ||
	    (_keyboardStatus.isLeftUpPressed && !oldStatus.isLeftUpPressed) ||
	    (_keyboardStatus.isRightUpPressed && !oldStatus.isRightUpPressed))
	{
		if ((dx != 0) || (dy != 0)) _rampUpCursorMover.moveAsync(dx, dy);
	}
	else
	{
		if ((dx != 0) || (dy != 0))
		{
			MouseUtils::MouseMove(dx, dy);
			MainSingleton::Instance().UpdateOverlay();
		}
	}
	return result;
}


//---------------------------------------------------------------------------------------------------------------------
bool
MouseActioner::processKeyUp(KeyboardUtils::VirtualKey_t vk)
{
	if (vk == _mouseParams.VKMoveRight)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isRightPressed = false;
	} else
	if (vk == _mouseParams.VKMoveLeft)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isLeftPressed = false;
	} else
	if (vk == _mouseParams.VKMoveUp)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isUpPressed = false;
	} else
	if (vk == _mouseParams.VKMoveDown)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isDownPressed = false;
	} else
	if (vk == _mouseParams.VKMoveLeftDown)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isLeftDownPressed = false;
	} else
	if (vk == _mouseParams.VKMoveRightDown)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isRightDownPressed = false;
	} else
	if (vk == _mouseParams.VKMoveLeftUp)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isLeftUpPressed = false;
	} else
	if (vk == _mouseParams.VKMoveRightUp)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isRightUpPressed = false;
	}
	else
	// left button up -------------------------------------------------------
	if ( (_stickyButton != NMB_Left) && (_mouseParams.VKPressLB == vk) )
	{
		if (_keyboardStatus.isLeftBtnPressed)
		{
			MouseUtils::MousePressLB(true);
			_keyboardStatus.isLeftBtnPressed = false;
		}
	} else
	// right button up ------------------------------------------------------
	if ( (_stickyButton != NMB_Right) && (_mouseParams.VKPressRB == vk) )
	{
		if (_keyboardStatus.isRightBtnPressed)
		{
			MouseUtils::MousePressRB(true);
			_keyboardStatus.isRightBtnPressed = false;
		}
	} else
	// middle button up -----------------------------------------------------
	if ( (_stickyButton != NMB_Middle) && (_mouseParams.VKPressMB == vk) )
	{
		if (_keyboardStatus.isMiddleBtnPressed)
		{
			MouseUtils::MousePressMB(true);
			_keyboardStatus.isMiddleBtnPressed = false;
		}
	} else
	{
		return false;
	}

	return true;
}


//---------------------------------------------------------------------------------------------------------------------
bool
MouseActioner::processKeyDown(KeyboardUtils::VirtualKey_t vk)
{
	const bool isStickyModifierOn = _isStickyButtonPressed;

	// left button down -----------------------------------------------------
	if (_mouseParams.VKPressLB == vk)
	{
		if (_stickyButton == NMB_Left)
		{
			resetStickyButton();
		} else
		if (!_keyboardStatus.isLeftBtnPressed)
		{
			MouseUtils::MousePressLB(false);
			_keyboardStatus.isLeftBtnPressed = true;
			if (isStickyModifierOn)
			{
				_stickyButton = NMB_Left;
			}
		}
	} else
	// right button down -----------------------------------------------------
	if (_mouseParams.VKPressRB == vk)
	{
		if (_stickyButton == NMB_Right)
		{
			resetStickyButton();
		} else
		if (!_keyboardStatus.isRightBtnPressed)
		{
			MouseUtils::MousePressRB(false);
			_keyboardStatus.isRightBtnPressed = true;
			if (isStickyModifierOn)
			{
				_stickyButton = NMB_Right;
			}
		}
	} else
	// middle button down ---------------------------------------------------
	if (_mouseParams.VKPressMB == vk)
	{
		if (_stickyButton == NMB_Middle)
		{
			resetStickyButton();
		} else
		if (!_keyboardStatus.isMiddleBtnPressed)
		{
			MouseUtils::MousePressMB(false);
			_keyboardStatus.isMiddleBtnPressed = true;
			if (isStickyModifierOn)
			{
				_stickyButton = NMB_Middle;
			}
		}
	} else
	// up -------------------------------------------------------------------
	if (_mouseParams.VKMoveUp == vk)
	{
		_keyboardStatus.isUpPressed = true;
	} else
	// down -----------------------------------------------------------------
	if (_mouseParams.VKMoveDown == vk)
	{
		_keyboardStatus.isDownPressed = true;
	} else
	// left -----------------------------------------------------------------
	if (_mouseParams.VKMoveLeft == vk)
	{
		_keyboardStatus.isLeftPressed = true;
	} else
	// right ----------------------------------------------------------------
	if (_mouseParams.VKMoveRight == vk)
	{
		_keyboardStatus.isRightPressed = true;
	} else
	// left down ------------------------------------------------------------
	if (_mouseParams.VKMoveLeftDown == vk)
	{
		_keyboardStatus.isLeftDownPressed = true;
	} else
	// right down -----------------------------------------------------------
	if (_mouseParams.VKMoveRightDown == vk)
	{
		_keyboardStatus.isRightDownPressed = true;
	} else
	// left up --------------------------------------------------------------
	if (_mouseParams.VKMoveLeftUp == vk)
	{
		_keyboardStatus.isLeftUpPressed = true;
	} else
	// right up -------------------------------------------------------------
	if (_mouseParams.VKMoveRightUp == vk)
	{
		_keyboardStatus.isRightUpPressed = true;
	}
	else
	// wheel up -------------------------------------------------------------
	if (_mouseParams.VKWheelUp == vk)
	{
		MouseUtils::MouseWheel(false);
	} else
	// wheel down -----------------------------------------------------------
	if (_mouseParams.VKWheelDown == vk)
	{
		MouseUtils::MouseWheel(true);
	} else
	{
		return false;
	}

	return true;
}


//---------------------------------------------------------------------------------------------------------------------
void
MouseActioner::activateEmulation(bool activate)
{
	if (_mouseParams.UseHotkey())
	{
		_isEmulationActivated = activate;
	} else
	{
		if (activate)
		{
			if (!(GetKeyState(_mouseParams.VKEnabler) & 1))
			{
				keybd_event(static_cast<BYTE>(_mouseParams.VKEnabler), static_cast<BYTE>(KeyboardUtils::VirtualKeyToScanCode(_mouseParams.VKEnabler)), KEYEVENTF_EXTENDEDKEY, 0);
				keybd_event(static_cast<BYTE>(_mouseParams.VKEnabler), static_cast<BYTE>(KeyboardUtils::VirtualKeyToScanCode(_mouseParams.VKEnabler)), KEYEVENTF_EXTENDEDKEY |  KEYEVENTF_KEYUP, 0);
			}
		} else
		{
			if (GetKeyState(_mouseParams.VKEnabler) & 1)
			{
				keybd_event(static_cast<BYTE>(_mouseParams.VKEnabler), static_cast<BYTE>(KeyboardUtils::VirtualKeyToScanCode(_mouseParams.VKEnabler)), KEYEVENTF_EXTENDEDKEY, 0);
				keybd_event(static_cast<BYTE>(_mouseParams.VKEnabler), static_cast<BYTE>(KeyboardUtils::VirtualKeyToScanCode(_mouseParams.VKEnabler)), KEYEVENTF_EXTENDEDKEY |  KEYEVENTF_KEYUP, 0);
			}
		}
	}

	if (!activate)
	{
		resetStickyButton();
		_isActivationButtonPressed = false;
		_isAlternativeSpeedButtonPressed = false;
	}
}


//---------------------------------------------------------------------------------------------------------------------
bool
MouseActioner::isEmulationActivated()
{
	if (_mouseParams.UseHotkey())
		return _isEmulationActivated;
	else
		return (GetKeyState(_mouseParams.VKEnabler) & 1);
}


//---------------------------------------------------------------------------------------------------------------------
void
MouseActioner::resetStickyButton()
{
	switch (_stickyButton)
	{
	case NMB_None:
		break;
	case NMB_Left:
		_stickyButton = NMB_None;
		processKeyUp(_mouseParams.VKPressLB);
		break;
	case NMB_Middle:
		_stickyButton = NMB_None;
		processKeyUp(_mouseParams.VKPressMB);
		break;
	case NMB_Right:
		_stickyButton = NMB_None;
		processKeyUp(_mouseParams.VKPressRB);
		break;
	}
	_stickyButton = NMB_None;
}


//---------------------------------------------------------------------------------------------------------------------
void
MouseActioner::reset()
{
	resetStickyButton();
	_rampUpCursorMover.stopMove();
	_keyboardStatus = KeyboardButtonsStatus();
	_lastShift = LastShift_t::kUnknown;
	_isActivationButtonPressed = false;
	_isAlternativeSpeedButtonPressed = false;
	_ignoreNextStickyKeyDown = false;
}

//---------------------------------------------------------------------------------------------------------------------
void
MouseActioner::setMouseParams(const MouseParams& mouseParams)
{
	_mouseParams = mouseParams;
}

}}