//
// Copyright © 2016 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//

#include "StdAfx.h"

#include "logic/MouseUtils.h"

namespace neatmouse {
namespace logic {

//---------------------------------------------------------------------------------------------------------------------
void MouseUtils::MouseMove(LONG dx, LONG dy)
{
	MOUSEINPUT mouseInput;
	mouseInput.dx = dx;
	mouseInput.dy = dy;
	mouseInput.dwFlags = MOUSEEVENTF_MOVE;
	mouseInput.mouseData = 0;
	mouseInput.time = 0;
	mouseInput.dwExtraInfo = 0;
	INPUT Input;
	Input.type = INPUT_MOUSE;
	Input.mi = mouseInput;
	SendInput(1, &Input, sizeof(Input));
}


//---------------------------------------------------------------------------------------------------------------------
void MouseUtils::MousePressMB(bool doUp)
{
	MOUSEINPUT mouseInput;
	mouseInput.dx = 0;
	mouseInput.dy = 0;
	mouseInput.dwFlags = doUp ? MOUSEEVENTF_MIDDLEUP : MOUSEEVENTF_MIDDLEDOWN;
	mouseInput.time = 0;
	INPUT Input;
	Input.type = INPUT_MOUSE;
	Input.mi = mouseInput;
	SendInput(1, &Input, sizeof(Input));
}


//---------------------------------------------------------------------------------------------------------------------
void MouseUtils::MousePressLB(bool doUp)
{
	MOUSEINPUT mouseInput;
	mouseInput.dx = 0;
	mouseInput.dy = 0;
	mouseInput.dwFlags = doUp ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_LEFTDOWN;
	mouseInput.time = 0;
	INPUT Input;
	Input.type = INPUT_MOUSE;
	Input.mi = mouseInput;
	SendInput(1, &Input, sizeof(Input));
}


//---------------------------------------------------------------------------------------------------------------------
void MouseUtils::MousePressRB(bool doUp)
{
	MOUSEINPUT mouseInput;
	mouseInput.dx = 0;
	mouseInput.dy = 0;
	mouseInput.dwFlags = doUp ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_RIGHTDOWN;
	mouseInput.time = 0;
	INPUT Input;
	Input.type = INPUT_MOUSE;
	Input.mi = mouseInput;
	SendInput(1, &Input, sizeof(Input));
}


//---------------------------------------------------------------------------------------------------------------------
void MouseUtils::MouseWheel(bool toUser)
{
	MOUSEINPUT mouseInput;
	mouseInput.mouseData = toUser ? -WHEEL_DELTA : WHEEL_DELTA;
	mouseInput.dwFlags = MOUSEEVENTF_WHEEL;
	mouseInput.time = 0;
	INPUT Input;
	Input.type = INPUT_MOUSE;
	Input.mi = mouseInput;
	SendInput(1, &Input, sizeof(Input));
}

}}