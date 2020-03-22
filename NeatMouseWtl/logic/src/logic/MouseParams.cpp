//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#include "StdAfx.h"

#include "logic/MouseParams.h"
#include "neatcommon/system/IniFiles.h"

namespace neatmouse {
namespace logic {

//---------------------------------------------------------------------------------------------------------------------
std::wstring MouseParams::GetName() const
{
	return m_name;
}


//---------------------------------------------------------------------------------------------------------------------
std::wstring MouseParams::GetFilePath() const
{
	return m_filePath;
}


//---------------------------------------------------------------------------------------------------------------------
bool MouseParams::Save()
{
	return Save(m_filePath);
}


//---------------------------------------------------------------------------------------------------------------------
bool MouseParams::Save(const std::wstring & fileName)
{
	neatcommon::system::MyIniFile mif;

	mif.writeIntValue(L"General", L"Delta", this->delta);
	mif.writeIntValue(L"General", L"ADelta", this->adelta);

	mif.writeIntValue(L"General", L"VKEnabler", this->VKEnabler);
	mif.writeIntValue(L"General", L"VKAccelerated", this->VKAccelerated);

	mif.writeIntValue(L"General", L"VK_MoveUp", this->VKMoveUp);
	mif.writeIntValue(L"General", L"VK_MoveDown", this->VKMoveDown);
	mif.writeIntValue(L"General", L"VK_MoveLeft", this->VKMoveLeft);
	mif.writeIntValue(L"General", L"VK_MoveRight", this->VKMoveRight);

	mif.writeIntValue(L"General", L"VK_MoveLeftUp", this->VKMoveLeftUp);
	mif.writeIntValue(L"General", L"VK_MoveRightUp", this->VKMoveRightUp);
	mif.writeIntValue(L"General", L"VK_MoveLeftDown", this->VKMoveLeftDown);
	mif.writeIntValue(L"General", L"VK_MoveRightDown", this->VKMoveRightDown);

	mif.writeIntValue(L"General", L"VK_PressLB", this->VKPressLB);
	mif.writeIntValue(L"General", L"VK_PressRB", this->VKPressRB);
	mif.writeIntValue(L"General", L"VK_PressMB", this->VKPressMB);
	mif.writeIntValue(L"General", L"VK_WheelUp", this->VKWheelUp);
	mif.writeIntValue(L"General", L"VK_WheelDown", this->VKWheelDown);

	mif.writeUIntValue(L"General", L"VK_Hotkey", this->VKHotkey);
	mif.writeUIntValue(L"General", L"ModHotkey", this->modHotkey);

	mif.writeStringValue(L"General", L"Name", this->m_name);

	mif.writeUIntValue(L"General", L"VKActivationMod", this->VKActivationMod);
	mif.writeUIntValue(L"General", L"VKStickyKey", this->VKStickyKey);
	mif.writeBoolValue(L"General", L"MinimizeOnStartup", this->minimizeOnStartup);
	mif.writeBoolValue(L"General", L"ActivateOnStartup", this->activateOnStartup);
	mif.writeBoolValue(L"General", L"ChangeCursor", this->changeCursor);
	mif.writeBoolValue(L"General", L"ShowNotifications", this->showNotifications);

	m_filePath = fileName;
	return mif.save(fileName);
}


//---------------------------------------------------------------------------------------------------------------------
bool MouseParams::Load(const std::wstring & fileName)
{
	m_filePath = fileName;

	neatcommon::system::MyIniFile mif;
	bool res = mif.load(fileName);

	this->delta = mif.readIntValue(L"General", L"Delta", 20);
	this->adelta = mif.readIntValue(L"General", L"ADelta", 1);

	this->VKEnabler = mif.readIntValue(L"General", L"VKEnabler", VK_SCROLL);
	this->VKAccelerated = mif.readIntValue(L"General", L"VKAccelerated", kVKNone);

	this->VKMoveUp = mif.readIntValue(L"General", L"VK_MoveUp", VK_NUMPAD8);
	this->VKMoveDown = mif.readIntValue(L"General", L"VK_MoveDown", VK_NUMPAD2);
	this->VKMoveLeft = mif.readIntValue(L"General", L"VK_MoveLeft", VK_NUMPAD4);
	this->VKMoveRight = mif.readIntValue(L"General", L"VK_MoveRight", VK_NUMPAD6);

	this->VKMoveLeftUp = mif.readIntValue(L"General", L"VK_MoveLeftUp", VK_NUMPAD7);
	this->VKMoveRightUp = mif.readIntValue(L"General", L"VK_MoveRightUp", VK_NUMPAD9);
	this->VKMoveLeftDown = mif.readIntValue(L"General", L"VK_MoveLeftDown", VK_NUMPAD1);
	this->VKMoveRightDown = mif.readIntValue(L"General", L"VK_MoveRightDown", VK_NUMPAD3);

	this->VKPressLB = mif.readIntValue(L"General", L"VK_PressLB", VK_NUMPAD0);
	this->VKPressRB = mif.readIntValue(L"General", L"VK_PressRB", VK_NUMPADENTER);
	this->VKPressMB = mif.readIntValue(L"General", L"VK_PressMB", VK_NUMPAD5);
	this->VKWheelUp = mif.readIntValue(L"General", L"VK_WheelUp", -VK_DIVIDE);
	this->VKWheelDown = mif.readIntValue(L"General", L"VK_WheelDown", VK_MULTIPLY);

	this->VKHotkey = mif.readUIntValue(L"General", L"VK_Hotkey", VK_F10);
	this->modHotkey = mif.readUIntValue(L"General", L"ModHotkey", MOD_CONTROL | MOD_ALT);

	this->m_name = mif.readStringValue(L"General", L"Name", L"[Untitled]");

	this->minimizeOnStartup = mif.readBoolValue(L"General", L"MinimizeOnStartup", false);
	this->activateOnStartup = mif.readBoolValue(L"General", L"ActivateOnStartup", false);
	this->changeCursor = mif.readBoolValue(L"General", L"ChangeCursor", false);
	this->showNotifications = mif.readBoolValue(L"General", L"ShowNotifications", true);

	this->VKActivationMod = mif.readIntValue(L"General", L"VKActivationMod", kVKNone);
	this->VKStickyKey = mif.readIntValue(L"General", L"VKStickyKey", kVKNone);

	return res;
}


//---------------------------------------------------------------------------------------------------------------------
bool MouseParams::UseHotkey() const
{
	return (VKEnabler != VK_CAPITAL) && (VKEnabler != VK_SCROLL) && (VKEnabler != VK_NUMLOCK);
}


//---------------------------------------------------------------------------------------------------------------------
bool MouseParams::IsEqual(const MouseParams & mouseParams) const
{
	if (delta != mouseParams.delta) return false;
	if (adelta != mouseParams.adelta) return false;
	if (VKEnabler != mouseParams.VKEnabler) return false;
	if (VKMoveUp != mouseParams.VKMoveUp) return false;
	if (VKMoveDown != mouseParams.VKMoveDown) return false;
	if (VKMoveLeft != mouseParams.VKMoveLeft) return false;
	if (VKMoveRight != mouseParams.VKMoveRight) return false;
	if (VKMoveLeftUp != mouseParams.VKMoveLeftUp) return false;
	if (VKMoveLeftDown != mouseParams.VKMoveLeftDown) return false;
	if (VKMoveRightUp != mouseParams.VKMoveRightUp) return false;
	if (VKMoveRightDown != mouseParams.VKMoveRightDown) return false;
	if (VKAccelerated != mouseParams.VKAccelerated) return false;
	if (VKPressLB != mouseParams.VKPressLB) return false;
	if (VKPressRB != mouseParams.VKPressRB) return false;
	if (VKPressMB != mouseParams.VKPressMB) return false;
	if (VKWheelUp != mouseParams.VKWheelUp) return false;
	if (VKWheelDown != mouseParams.VKWheelDown) return false;
	if (minimizeOnStartup != mouseParams.minimizeOnStartup) return false;
	if (activateOnStartup != mouseParams.activateOnStartup) return false;
	if (changeCursor != mouseParams.changeCursor) return false;
	if (showNotifications != mouseParams.showNotifications) return false;
	if (VKActivationMod != mouseParams.VKActivationMod) return false;
	if (VKStickyKey != mouseParams.VKStickyKey) return false;

	if (UseHotkey())
	{
		if (VKHotkey != mouseParams.VKHotkey) return false;
		if (modHotkey != mouseParams.modHotkey) return false;
	}

	return true;
}

//---------------------------------------------------------------------------------------------------------------------
MouseParams::MouseParams(const std::wstring& name) : m_name(name)
{}


//---------------------------------------------------------------------------------------------------------------------
MouseParams::MouseParams() : m_name(_T("[Untitled]"))
{
}


//---------------------------------------------------------------------------------------------------------------------
bool MouseParams::BindingExists(int keyCode)
{
	if (VKEnabler == keyCode ||
		VKMoveUp == keyCode ||
		VKMoveDown == keyCode ||
		VKMoveLeft == keyCode ||
		VKMoveRight == keyCode ||
		VKMoveLeftUp == keyCode ||
		VKMoveRightDown == keyCode ||
		VKMoveLeftDown == keyCode ||
		VKMoveRightUp == keyCode ||
		VKAccelerated == keyCode ||
		VKActivationMod == keyCode ||
		VKStickyKey == keyCode ||
		VKPressLB == keyCode ||
		VKPressRB == keyCode ||
		VKPressMB == keyCode ||
		VKWheelUp == keyCode ||
		VKWheelDown == keyCode)
		return true;

	return false;
}


//---------------------------------------------------------------------------------------------------------------------
bool MouseParams::isModifierTaken(DWORD modifierId) const
{
	bool result = false;

	if (modifierId & MOD_CONTROL)
	{
		result |= (VKActivationMod == VK_LCONTROL) || (VKActivationMod == VK_RCONTROL) ||
			(VKAccelerated == VK_LCONTROL) || (VKAccelerated == VK_RCONTROL) ||
			(VKStickyKey == VK_LCONTROL) || (VKStickyKey == VK_RCONTROL);
	}

	if (modifierId & MOD_SHIFT)
	{
		result |= (VKActivationMod == VK_LSHIFT) || (VKActivationMod == VK_RSHIFT) ||
			(VKAccelerated == VK_LSHIFT) || (VKAccelerated == VK_RSHIFT) ||
			(VKStickyKey == VK_LSHIFT) || (VKStickyKey == VK_RSHIFT);
	}

	if (modifierId & MOD_ALT)
	{
		result |= (VKActivationMod == VK_LMENU) || (VKActivationMod == VK_RMENU) ||
			(VKAccelerated == VK_LMENU) || (VKAccelerated == VK_RMENU) ||
			(VKStickyKey == VK_LMENU) || (VKStickyKey == VK_RMENU);
	}

	return result;
}

}}