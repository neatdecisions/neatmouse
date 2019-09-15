//
// Copyright © 2016–2019 Neat Decisions. All rights reserved.
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
MouseActioner::MouseActioner() :
	_lastShift(kUnknown),
	_isEmulationActivated(false),
	_stickyButton(NMB_None),
	_isStickyButtonPressed(false),
	_isActivationButtonPressed(false),
	_isAlternativeSpeedButtonPressed(false),
	_ignoreNextStickyKeyDown(false)
{
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
			oValue = (( (_lastShift == kLeft) && (modifier == VK_LSHIFT)) ||
			          ( (_lastShift == kRight) && (modifier == VK_RSHIFT)));
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
		_lastShift = kLeft;
	else if (abs(vk) == VK_RSHIFT)
		_lastShift = kRight;

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
	    (static_cast<logic::KeyboardUtils::VirtualKey_t>(event.vkCode) == MainSingleton::Instance().GetMouseParams()->VKEnabler))
	{
		_isEmulationActivated = (GetKeyState(MainSingleton::Instance().GetMouseParams()->VKEnabler) & 1);
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
	const MouseParams & mouseParams = *(MainSingleton::Instance().GetMouseParams());
	const std::pair<KeyboardUtils::VirtualKey_t, bool> & aKeyPair = preprocessKey(event);
	const KeyboardUtils::VirtualKey_t vk = aKeyPair.first;
	const bool isNumlockSpecialHandling = aKeyPair.second;

	// if Activation Modifier has been set up, check whether is is pressed
	if (mouseParams.VKActivationMod != MouseParams::kVKNone)
	{
		// if Activation Modifier has been pressed and it is the button we're currently processing, return true 
		// to block further event processing 
		if (checkModifierButtonDown(vk, mouseParams.VKActivationMod, isKeyUp, isNumlockSpecialHandling, _isActivationButtonPressed))
		{
			if (!_isActivationButtonPressed && (mouseParams.VKActivationMod != VK_RSHIFT) && ((mouseParams.VKActivationMod != VK_LSHIFT))) 
			{
				reset();
			}
			return true;
		}

		// if Activation Modifier is a Shift and we're working with the numerical keyboard, isNumlockSpecialHandling will indicate
		// that Shift is pressed - there is no other way of deducing it here since Windows send Shift's Key Up in this case
		if (isNumlockSpecialHandling && 
		    ( ( (mouseParams.VKActivationMod == VK_RSHIFT) && (_lastShift == kRight) ) || 
		      ( (mouseParams.VKActivationMod == VK_LSHIFT) && (_lastShift == kLeft) ) ) )
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
	if (checkModifierButtonDown(vk, mouseParams.VKAccelerated, isKeyUp, isNumlockSpecialHandling, _isAlternativeSpeedButtonPressed))
	{
		return true;
	}

	// updade the status of Sticky Key Modifier and check if anything else should be done
	if (checkModifierButtonDown(vk, mouseParams.VKStickyKey, isKeyUp, isNumlockSpecialHandling, _isStickyButtonPressed))
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
		return true;
	}
	else
	{
		// special Shift + Numerical Keyboard  handling (see above)
		// if checkModifierButtonDown() returned false, and both _isStickyButtonPressed and isNumlockSpecialHandling are true,
		// this means that we're processing an event from the numerical keyboard with a Shift pressed; we want to ignore the
		// next Shift Key Down event since it will be fake one generated by Windows
		if (isNumlockSpecialHandling && _isStickyButtonPressed && (mouseParams.VKStickyKey == VK_RSHIFT || mouseParams.VKStickyKey == VK_LSHIFT))
		{
			_ignoreNextStickyKeyDown = true;
		}
	}

	const KeyboardButtonsStatus oldStatus = _keyboardStatus;
	const bool result = isKeyUp ? processKeyUp(vk) : processKeyDown(vk);

	// figure out the movement vector
	LONG _d = _isAlternativeSpeedButtonPressed ? mouseParams.adelta : mouseParams.delta;
	LONG dx =
		((_keyboardStatus.isLeftPressed || _keyboardStatus.isLeftUpPressed || _keyboardStatus.isLeftDownPressed) ? -_d : 0) +
		((_keyboardStatus.isRightPressed || _keyboardStatus.isRightUpPressed || _keyboardStatus.isRightDownPressed) ? _d : 0);

	LONG dy =
		((_keyboardStatus.isUpPressed || _keyboardStatus.isLeftUpPressed || _keyboardStatus.isRightUpPressed) ? -_d : 0) +
		((_keyboardStatus.isDownPressed || _keyboardStatus.isLeftDownPressed || _keyboardStatus.isRightDownPressed) ? _d : 0);

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
		if ((dx != 0) || (dy != 0)) MouseUtils::MouseMove(dx, dy);
	}
	return result;
}


//---------------------------------------------------------------------------------------------------------------------
bool 
MouseActioner::processKeyUp(KeyboardUtils::VirtualKey_t vk)
{
	const MouseParams & mouseParams = *(MainSingleton::Instance().GetMouseParams());

	if (vk == mouseParams.VKMoveRight)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isRightPressed = false;
	} else
	if (vk == mouseParams.VKMoveLeft)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isLeftPressed = false;
	} else
	if (vk == mouseParams.VKMoveUp)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isUpPressed = false;
	} else
	if (vk == mouseParams.VKMoveDown)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isDownPressed = false;
	} else
	if (vk == mouseParams.VKMoveLeftDown)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isLeftDownPressed = false;
	} else
	if (vk == mouseParams.VKMoveRightDown)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isRightDownPressed = false;
	}
	if (vk == mouseParams.VKMoveLeftUp)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isLeftUpPressed = false;
	} else
	if (vk == mouseParams.VKMoveRightUp)
	{
		_rampUpCursorMover.stopMove();
		_keyboardStatus.isRightUpPressed = false;
	}
	else
	// left button up -------------------------------------------------------
	if ( (_stickyButton != NMB_Left) && (mouseParams.VKPressLB == vk) )
	{
		if (_keyboardStatus.isLeftBtnPressed)
		{
			MouseUtils::MousePressLB(true);
			_keyboardStatus.isLeftBtnPressed = false;
		}
	} else
	// right button up ------------------------------------------------------
	if ( (_stickyButton != NMB_Right) && (mouseParams.VKPressRB == vk) )
	{
		if (_keyboardStatus.isRightBtnPressed)
		{
			MouseUtils::MousePressRB(true);
			_keyboardStatus.isRightBtnPressed = false;
		}
	} else
	// middle button up -----------------------------------------------------
	if ( (_stickyButton != NMB_Middle) && (mouseParams.VKPressMB == vk) )
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
	const MouseParams & mouseParams = *(MainSingleton::Instance().GetMouseParams());
	
	const bool isStickyModifierOn = _isStickyButtonPressed;

	// left button down -----------------------------------------------------
	if (mouseParams.VKPressLB == vk)
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
	if (mouseParams.VKPressRB == vk)
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
	if (mouseParams.VKPressMB == vk)
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
	if (mouseParams.VKMoveUp == vk)
	{
		_keyboardStatus.isUpPressed = true;
	} else
	// down -----------------------------------------------------------------
	if (mouseParams.VKMoveDown == vk)
	{
		_keyboardStatus.isDownPressed = true;
	} else	
	// left -----------------------------------------------------------------
	if (mouseParams.VKMoveLeft == vk)
	{
		_keyboardStatus.isLeftPressed = true;
	} else		
	// right ----------------------------------------------------------------
	if (mouseParams.VKMoveRight == vk)
	{
		_keyboardStatus.isRightPressed = true;
	} else
	// left down ------------------------------------------------------------
	if (mouseParams.VKMoveLeftDown == vk)
	{
		_keyboardStatus.isLeftDownPressed = true;
	} else
	// right down -----------------------------------------------------------
	if (mouseParams.VKMoveRightDown == vk)
	{
		_keyboardStatus.isRightDownPressed = true;
	} else
	// left up --------------------------------------------------------------
	if (mouseParams.VKMoveLeftUp == vk)
	{
		_keyboardStatus.isLeftUpPressed = true;
	} else
	// right up -------------------------------------------------------------
	if (mouseParams.VKMoveRightUp == vk)
	{
		_keyboardStatus.isRightUpPressed = true;
	}
	else
	// wheel up -------------------------------------------------------------
	if (mouseParams.VKWheelUp == vk)
	{
		MouseUtils::MouseWheel(false);
	} else
	// wheel down -----------------------------------------------------------
	if (mouseParams.VKWheelDown == vk)
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
	const MouseParams & mouseParams = *(MainSingleton::Instance().GetMouseParams());
	if (mouseParams.UseHotkey())
	{
		_isEmulationActivated = activate;
	} else
	{
		if (activate)
		{
			if (!(GetKeyState(mouseParams.VKEnabler) & 1))
			{
				keybd_event(static_cast<BYTE>(mouseParams.VKEnabler), static_cast<BYTE>(KeyboardUtils::VirtualKeyToScanCode(mouseParams.VKEnabler)), KEYEVENTF_EXTENDEDKEY, 0);
				keybd_event(static_cast<BYTE>(mouseParams.VKEnabler), static_cast<BYTE>(KeyboardUtils::VirtualKeyToScanCode(mouseParams.VKEnabler)), KEYEVENTF_EXTENDEDKEY |  KEYEVENTF_KEYUP, 0);
			}
		} else
		{
			if (GetKeyState(mouseParams.VKEnabler) & 1)
			{
				keybd_event(static_cast<BYTE>(mouseParams.VKEnabler), static_cast<BYTE>(KeyboardUtils::VirtualKeyToScanCode(mouseParams.VKEnabler)), KEYEVENTF_EXTENDEDKEY, 0);
				keybd_event(static_cast<BYTE>(mouseParams.VKEnabler), static_cast<BYTE>(KeyboardUtils::VirtualKeyToScanCode(mouseParams.VKEnabler)), KEYEVENTF_EXTENDEDKEY |  KEYEVENTF_KEYUP, 0);
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
	const MouseParams & mouseParams = *(MainSingleton::Instance().GetMouseParams());
	if (mouseParams.UseHotkey())
		return _isEmulationActivated;
	else
		return (GetKeyState(mouseParams.VKEnabler) & 1);
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
		processKeyUp(MainSingleton::Instance().GetMouseParams()->VKPressLB);
		break;
	case NMB_Middle:
		_stickyButton = NMB_None;
		processKeyUp(MainSingleton::Instance().GetMouseParams()->VKPressMB);
		break;
	case NMB_Right:
		_stickyButton = NMB_None;
		processKeyUp(MainSingleton::Instance().GetMouseParams()->VKPressRB);
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
	_lastShift = kUnknown;
	_isActivationButtonPressed = false;
	_isAlternativeSpeedButtonPressed = false;
	_ignoreNextStickyKeyDown = false;
}

}}