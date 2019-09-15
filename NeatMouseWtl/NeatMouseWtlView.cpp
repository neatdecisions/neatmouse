//
// Copyright © 2016–2019 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//


#include "stdafx.h"

#include "NeatMouseWtlView.h"

#include "logic/MainSingleton.h"
#include "logic/MouseParams.h"

namespace neatmouse {


#define ACTIVATION_HOTKEY_ID 1
	

//---------------------------------------------------------------------------------------------------------------------
CNeatMouseWtlView::CNeatMouseWtlView() : tb(nullptr), checkBoxPadding(0)
{}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::SetToolbar(CNeatToolbar * pToolbar)
{
	this->tb = pToolbar;
}


//---------------------------------------------------------------------------------------------------------------------
CString 
CNeatMouseWtlView::GetHotkeyName(UINT mods, UINT vk)
{
	if ((mods == 0) || (vk == 0)) return L"";

	CString res;
	if (mods & MOD_CONTROL) res += L"Ctrl + ";
	if (mods & MOD_ALT) res += L"Alt + ";
	if (mods & MOD_SHIFT) res += L"Shift + ";
	res += logic::KeyboardUtils::GetKeyName(vk, 0).c_str();

	return res;
}


//---------------------------------------------------------------------------------------------------------------------
BOOL 
CNeatMouseWtlView::PreTranslateMessage(MSG* pMsg)
{
	static HACCEL hAccelTable = LoadAccelerators(_Module.m_hInst, MAKEINTRESOURCE(IDR_MAINFRAME));
	if (!TranslateAccelerator(pMsg->hwnd, hAccelTable, pMsg))
	{
		if (pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYUP)
		{
			if (tb != nullptr) tb->RedrawMe();
		}

		if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
		{
			// these keys won't be accepted in the binding edit boxes
			static const std::set<DWORD> bannedKeys 
			{
				VK_SHIFT, VK_CONTROL, VK_MENU, VK_NUMLOCK, VK_SCROLL, VK_CAPITAL, VK_CANCEL, VK_TAB, 
				VK_LSHIFT, VK_RSHIFT, VK_LMENU, VK_RMENU, VK_LCONTROL, VK_RCONTROL, VK_LWIN, VK_RWIN
			};
			
			TCHAR buf[128];
			do
			{
				if (bannedKeys.find(pMsg->wParam) != bannedKeys.end()) break;
									
				USHORT scanCode = (pMsg->lParam >> 16) & 0xFF;
				int vk = pMsg->wParam;

				if (HIWORD(pMsg->lParam) & KF_EXTENDED)
				{
					vk = -vk;
				}

				if (logic::MainSingleton::Instance().GetMouseParams()->BindingExists(vk)) break;
			
				if (pMsg->hwnd == GetDlgItem(IDC_EDIT_HOTKEY))
				{
					if (!logic::MainSingleton::Instance().GetMouseParams()->UseHotkey())
					{
						ASSERT(false);
						break;
					}

					UINT mods = 0;
					if (logic::KeyboardUtils::IsKeyDown(VK_CONTROL) && 
					    !logic::MainSingleton::Instance().GetMouseParams()->isModifierTaken(MOD_CONTROL))
					{
						mods |= MOD_CONTROL;
					}

					if (logic::KeyboardUtils::IsKeyDown(VK_MENU) && 
					    !logic::MainSingleton::Instance().GetMouseParams()->isModifierTaken(MOD_ALT))
					{
						mods |= MOD_ALT;
					}

					if (logic::KeyboardUtils::IsKeyDown(VK_SHIFT) && 
					    !logic::MainSingleton::Instance().GetMouseParams()->isModifierTaken(MOD_SHIFT))
					{
						mods |= MOD_SHIFT;
					}

					if (mods == 0) break;

					if (EnableHotkey(mods, pMsg->wParam))
					{
						logic::MainSingleton::Instance().GetMouseParams()->VKHotkey = pMsg->wParam;
						
						logic::MainSingleton::Instance().GetMouseParams()->modHotkey = mods;
						::SetWindowText(GetDlgItem(IDC_EDIT_HOTKEY), GetHotkeyName(mods, pMsg->wParam));
						
					} else
					{
						if (!EnableHotkey(
								logic::MainSingleton::Instance().GetMouseParams()->modHotkey, 
								logic::MainSingleton::Instance().GetMouseParams()->VKHotkey))
						{
							logic::MainSingleton::Instance().GetMouseParams()->modHotkey = 0;
							logic::MainSingleton::Instance().GetMouseParams()->VKHotkey = 0;
							::SetWindowText(GetDlgItem(IDC_EDIT_HOTKEY), L"");
							ASSERT(false);
						}
					}

					SynchronizeCombos();

					if (tb != nullptr) tb->UpdateButtonStates();

					break;
				}

				// list of the edit boxes which can accept input
				static const std::vector<DWORD> hwnds
				{
					IDC_EDIT_BTN_LEFT, 
					IDC_EDIT_BTN_RIGHT,
					IDC_EDIT_BTN_MIDDLE,
					IDC_EDIT_UP,
					IDC_EDIT_DOWN,
					IDC_EDIT_LEFT,
					IDC_EDIT_RIGHT,
					IDC_EDIT_SCROLL_UP,
					IDC_EDIT_SCROLL_DOWN,
					IDC_EDIT_LEFTUP,
					IDC_EDIT_RIGHTUP,
					IDC_EDIT_LEFTDOWN,
					IDC_EDIT_RIGHTDOWN,
				};

				DWORD id = 0;

				for (DWORD aHwnd : hwnds)
				{
					if (GetDlgItem(aHwnd) == pMsg->hwnd)
					{
						id = aHwnd;
						break;
					}
				}

				if (id == 0) break;

				if (GetKeyNameText(pMsg->lParam, buf, sizeof(buf) / sizeof(buf[0])) > 0)
				{
					std::wstring res = logic::KeyboardUtils::GetKeyName(vk, scanCode);
					::SetWindowText(pMsg->hwnd, res.c_str());
					switch (id)
					{
					case IDC_EDIT_BTN_LEFT:
						logic::MainSingleton::Instance().GetMouseParams()->VKPressLB = vk;
						break;
					case IDC_EDIT_BTN_RIGHT:
						logic::MainSingleton::Instance().GetMouseParams()->VKPressRB = vk;
						break;
					case IDC_EDIT_BTN_MIDDLE:
						logic::MainSingleton::Instance().GetMouseParams()->VKPressMB = vk;
						break;
					case IDC_EDIT_UP:
						logic::MainSingleton::Instance().GetMouseParams()->VKMoveUp = vk;
						break;
					case IDC_EDIT_DOWN:
						logic::MainSingleton::Instance().GetMouseParams()->VKMoveDown = vk;
						break;
					case IDC_EDIT_LEFT:
						logic::MainSingleton::Instance().GetMouseParams()->VKMoveLeft = vk;
						break;
					case IDC_EDIT_RIGHT:
						logic::MainSingleton::Instance().GetMouseParams()->VKMoveRight = vk;
						break;
					case IDC_EDIT_SCROLL_UP:
						logic::MainSingleton::Instance().GetMouseParams()->VKWheelUp = vk;
						break;
					case IDC_EDIT_SCROLL_DOWN:
						logic::MainSingleton::Instance().GetMouseParams()->VKWheelDown = vk;
						break;
					case IDC_EDIT_LEFTUP:
						logic::MainSingleton::Instance().GetMouseParams()->VKMoveLeftUp = vk;
						break;
					case IDC_EDIT_RIGHTUP:
						logic::MainSingleton::Instance().GetMouseParams()->VKMoveRightUp = vk;
						break;
					case IDC_EDIT_LEFTDOWN:
						logic::MainSingleton::Instance().GetMouseParams()->VKMoveLeftDown = vk;
						break;
					case IDC_EDIT_RIGHTDOWN:
						logic::MainSingleton::Instance().GetMouseParams()->VKMoveRightDown = vk;
						break;
					}

					if (tb != nullptr) tb->UpdateButtonStates();
					return TRUE;
				}
			} while (false);
		}
	}
	return CWindow::IsDialogMessage(pMsg);
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::OnHotKey(int /*id*/, UINT /*mods*/, UINT /*vk*/)
{
	logic::MainSingleton::Instance().GetMouseActioner().activateEmulation(
		!logic::MainSingleton::Instance().GetMouseActioner().isEmulationActivated());
	logic::MainSingleton::Instance().NotifyEnabling(
		logic::MainSingleton::Instance().GetMouseActioner().isEmulationActivated());
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT 
CNeatMouseWtlView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(&ps);

	for (const std::map<UINT, HBITMAP>::value_type & aIcon : icons)
	{
		CWindow wnd = GetDlgItem(aIcon.first);
		if (wnd.IsWindowVisible())
		{
			CRect Rect;
			if (wnd.GetWindowRect(&Rect))
			{
				ScreenToClient(&Rect);
				Rect.left -= 20;
				neatcommon::ui::DrawBitmapAdvanced(hDC, aIcon.second, Rect.left, Rect.top, 16, 16);
			}
		}
	}
		
	EndPaint(&ps);
	
	bHandled = FALSE;
	return 1;
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatMouseWtlView::OnDestroy()	
{
	DisableHotkey();
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::OnMinimizeOnStartupCheck(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	CButton checkBox(GetDlgItem(IDC_CHECK_MINIMIZE));
	logic::MainSingleton::Instance().GetMouseParams()->minimizeOnStartup = checkBox.GetCheck() == BST_CHECKED;
	checkBox.Detach();
	if (tb != nullptr) tb->UpdateButtonStates();
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::OnActivateOnStartupCheck(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	CButton checkBox(GetDlgItem(IDC_CHECK_AUTOACTIVATE));
	logic::MainSingleton::Instance().GetMouseParams()->activateOnStartup = checkBox.GetCheck() == BST_CHECKED;
	checkBox.Detach();
	if (tb != nullptr) tb->UpdateButtonStates();
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::OnCursorCheck(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	CButton checkBox(GetDlgItem(IDC_CHECK_CURSOR));
	logic::MainSingleton::Instance().GetMouseParams()->changeCursor = checkBox.GetCheck() == BST_CHECKED;
	checkBox.Detach();
	if (tb != nullptr) tb->UpdateButtonStates();
	logic::MainSingleton::Instance().UpdateCursor();
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::OnShowNotificationsCheck(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	CButton checkBox(GetDlgItem(IDC_CHECK_NOTIFICATIONS));
	logic::MainSingleton::Instance().GetMouseParams()->showNotifications = checkBox.GetCheck() == BST_CHECKED;
	checkBox.Detach();
	if (tb != nullptr) tb->UpdateButtonStates();
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::OnDelHotkeyClick(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	::SetWindowText(GetDlgItem(IDC_EDIT_HOTKEY), L"");
	logic::MainSingleton::Instance().GetMouseParams()->VKHotkey = 0;
	logic::MainSingleton::Instance().GetMouseParams()->modHotkey = 0;
	DisableHotkey();
	SynchronizeCombos();
	if (tb != nullptr) tb->UpdateButtonStates();
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::OnDelBtnClick(UINT /*uCode*/, int nID, HWND /*hwndCtrl*/)
{
	switch (nID)
	{
	case IDC_BTN_DEL1:
		::SetWindowText(GetDlgItem(IDC_EDIT_BTN_LEFT), L"");
		logic::MainSingleton::Instance().GetMouseParams()->VKPressLB = 0;
		break;
	case IDC_BTN_DEL2:
		::SetWindowText(GetDlgItem(IDC_EDIT_BTN_RIGHT), L"");
		logic::MainSingleton::Instance().GetMouseParams()->VKPressRB = 0;
		break;
	case IDC_BTN_DEL3:
		::SetWindowText(GetDlgItem(IDC_EDIT_BTN_MIDDLE), L"");
		logic::MainSingleton::Instance().GetMouseParams()->VKPressMB = 0;
		break;
	case IDC_BTN_DEL4:
		::SetWindowText(GetDlgItem(IDC_EDIT_SCROLL_UP), L"");
		logic::MainSingleton::Instance().GetMouseParams()->VKWheelUp = 0;
		break;
	case IDC_BTN_DEL5:
		::SetWindowText(GetDlgItem(IDC_EDIT_SCROLL_DOWN), L"");
		logic::MainSingleton::Instance().GetMouseParams()->VKWheelDown = 0;
		break;
	case IDC_BTN_DEL6:
		::SetWindowText(GetDlgItem(IDC_EDIT_UP), L"");
		logic::MainSingleton::Instance().GetMouseParams()->VKMoveUp = 0;
		break;
	case IDC_BTN_DEL7:
		::SetWindowText(GetDlgItem(IDC_EDIT_DOWN), L"");
		logic::MainSingleton::Instance().GetMouseParams()->VKMoveDown = 0;
		break;
	case IDC_BTN_DEL8:
		::SetWindowText(GetDlgItem(IDC_EDIT_LEFT), L"");
		logic::MainSingleton::Instance().GetMouseParams()->VKMoveLeft = 0;
		break;
	case IDC_BTN_DEL9:
		::SetWindowText(GetDlgItem(IDC_EDIT_RIGHT), L"");
		logic::MainSingleton::Instance().GetMouseParams()->VKMoveRight = 0;
		break;
	case IDC_BTN_DEL10:
		::SetWindowText(GetDlgItem(IDC_EDIT_LEFTUP), L"");
		logic::MainSingleton::Instance().GetMouseParams()->VKMoveLeftUp = 0;
		break;
	case IDC_BTN_DEL11:
		::SetWindowText(GetDlgItem(IDC_EDIT_RIGHTUP), L"");
		logic::MainSingleton::Instance().GetMouseParams()->VKMoveRightUp = 0;
		break;
	case IDC_BTN_DEL12:
		::SetWindowText(GetDlgItem(IDC_EDIT_LEFTDOWN), L"");
		logic::MainSingleton::Instance().GetMouseParams()->VKMoveLeftDown = 0;
		break;
	case IDC_BTN_DEL13:
		::SetWindowText(GetDlgItem(IDC_EDIT_RIGHTDOWN), L"");
		logic::MainSingleton::Instance().GetMouseParams()->VKMoveRightDown = 0;
		break;
	default:
		// do not update button states
		return;
	}

	if (tb != nullptr) tb->UpdateButtonStates();
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatMouseWtlView::DisableHotkey()
{
	UnregisterHotKey(*this, ACTIVATION_HOTKEY_ID);
}


//---------------------------------------------------------------------------------------------------------------------
bool
CNeatMouseWtlView::EnableHotkey(UINT mods, UINT vk)
{
	DisableHotkey();
	return RegisterHotKey(*this, ACTIVATION_HOTKEY_ID, mods, vk) != FALSE;
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::OnComboSelChange(UINT /*uCode*/, int nID, HWND hwndCtrl)
{
	switch (nID)
	{
	case IDC_COMBO_ACTIVATION:
		{
			logic::MainSingleton::Instance().GetMouseActioner().reset();
			CComboBox cb(hwndCtrl);
			int n = cb.GetCurSel();
			ASSERT(n >= 0);
			if (n >= 0) logic::MainSingleton::Instance().GetMouseParams()->VKEnabler = cb.GetItemData(n);

			if (logic::MainSingleton::Instance().GetMouseParams()->UseHotkey())
			{
				::ShowWindow(GetDlgItem(IDC_EDIT_HOTKEY), SW_SHOW);
				btnDelHotkey.ShowWindow(SW_SHOW);

				if (logic::MainSingleton::Instance().GetMouseParams()->isModifierTaken(logic::MainSingleton::Instance().GetMouseParams()->modHotkey) ||
					 !EnableHotkey(logic::MainSingleton::Instance().GetMouseParams()->modHotkey, logic::MainSingleton::Instance().GetMouseParams()->VKHotkey))
				{
					logic::MainSingleton::Instance().GetMouseParams()->modHotkey = 0;
					logic::MainSingleton::Instance().GetMouseParams()->VKHotkey = 0;
					::SetWindowText(GetDlgItem(IDC_EDIT_HOTKEY), L"");
				}
			} else
			{
				::ShowWindow(GetDlgItem(IDC_EDIT_HOTKEY), SW_HIDE);
				btnDelHotkey.ShowWindow(SW_HIDE);
				DisableHotkey();
			}
			cb.Detach();
		}
		SynchronizeCombos();
		break;
	case IDC_COMBO_ALT_MOD:
		{
			CComboBox cb(hwndCtrl);
			int n = cb.GetCurSel();
			ASSERT(n >= 0);
			if (n >= 0) logic::MainSingleton::Instance().GetMouseParams()->VKAccelerated = cb.GetItemData(n);
			cb.Detach();
		}
		SynchronizeCombos();
		break;
	case IDC_COMBO_UNBIND:
		{
			logic::MainSingleton::Instance().GetMouseActioner().reset();
			CComboBox cb(hwndCtrl);
			int n = cb.GetCurSel();
			ASSERT(n >= 0);
			if (n >= 0) logic::MainSingleton::Instance().GetMouseParams()->VKActivationMod = cb.GetItemData(n);
			cb.Detach();
		}
		SynchronizeCombos();
		break;
	case IDC_COMBO_STICKYKEYS:
		{
			CComboBox cb(hwndCtrl);
			int n = cb.GetCurSel();
			ASSERT(n >= 0);
			if (n >= 0) logic::MainSingleton::Instance().GetMouseParams()->VKStickyKey = cb.GetItemData(n);
			cb.Detach();
		}
		SynchronizeCombos();
		break;
	default:
		return;
	}

	if (tb != nullptr)
	{
		tb->UpdateButtonStates();
	}
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT 
CNeatMouseWtlView::OnEditChange(UINT /*code*/, UINT id, HWND hwnd, BOOL & bHandled)
{
	bHandled = false;

	if ((id != IDC_EDIT_SPEED) && (id != IDC_EDIT_ALT_SPEED))
		return 0;

	CString s;
	int n = ::GetWindowTextLength(hwnd);
	::GetWindowText(hwnd, s.GetBuffer(n), n + 1);

	switch (id)
	{
	case IDC_EDIT_SPEED:
		logic::MainSingleton::Instance().GetMouseParams()->delta = neatcommon::system::from_string_def(s.GetBuffer(0), 20);
		break;
	case IDC_EDIT_ALT_SPEED:
		logic::MainSingleton::Instance().GetMouseParams()->adelta = neatcommon::system::from_string_def(s.GetBuffer(0), 1);
		break;
	}

	if (tb != nullptr) tb->UpdateButtonStates();

	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
int 
CNeatMouseWtlView::CheckIfChangesSaved()
{
	if (logic::MainSingleton::Instance().WereParametersChanged())
	{
		switch (AtlMessageBox(
			*this, 
			_("toolbar.presets.confirm-save-prompt"), 
			_("toolbar.presets.confirm-save-caption"), 
			MB_YESNOCANCEL | MB_ICONQUESTION))
		{
			case IDYES:
				logic::MainSingleton::Instance().AcceptMouseParams();
				tb->UpdateButtonStates();
				return IDYES;
			case IDCANCEL:
				tb->UpdateButtonStates();
				return IDCANCEL;
			default:
				logic::MainSingleton::Instance().RevertMouseParams();
				tb->UpdateButtonStates();
				return IDNO;
		}
	}
		
	return IDNO;
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::PopulateControls()
{
	const logic::MouseParams::Ptr mp = logic::MainSingleton::Instance().GetMouseParams();
		
	std::wstring s = std::to_wstring(mp->delta);

	GetDlgItem(IDC_EDIT_SPEED).SetWindowText(s.c_str());
	s = std::to_wstring(mp->adelta);
	GetDlgItem(IDC_EDIT_ALT_SPEED).SetWindowText(s.c_str());

	GetDlgItem(IDC_EDIT_BTN_LEFT).SetWindowText(logic::KeyboardUtils::GetKeyName(mp->VKPressLB, 0).c_str());
	GetDlgItem(IDC_EDIT_BTN_RIGHT).SetWindowText(logic::KeyboardUtils::GetKeyName(mp->VKPressRB, 0).c_str());
	GetDlgItem(IDC_EDIT_BTN_MIDDLE).SetWindowText(logic::KeyboardUtils::GetKeyName(mp->VKPressMB, 0).c_str());

	GetDlgItem(IDC_EDIT_UP).SetWindowText(logic::KeyboardUtils::GetKeyName(mp->VKMoveUp, 0).c_str());
	GetDlgItem(IDC_EDIT_DOWN).SetWindowText(logic::KeyboardUtils::GetKeyName(mp->VKMoveDown, 0).c_str());
	GetDlgItem(IDC_EDIT_LEFT).SetWindowText(logic::KeyboardUtils::GetKeyName(mp->VKMoveLeft, 0).c_str());
	GetDlgItem(IDC_EDIT_RIGHT).SetWindowText(logic::KeyboardUtils::GetKeyName(mp->VKMoveRight, 0).c_str());

	GetDlgItem(IDC_EDIT_LEFTUP).SetWindowText(logic::KeyboardUtils::GetKeyName(mp->VKMoveLeftUp, 0).c_str());
	GetDlgItem(IDC_EDIT_RIGHTUP).SetWindowText(logic::KeyboardUtils::GetKeyName(mp->VKMoveRightUp, 0).c_str());
	GetDlgItem(IDC_EDIT_LEFTDOWN).SetWindowText(logic::KeyboardUtils::GetKeyName(mp->VKMoveLeftDown, 0).c_str());
	GetDlgItem(IDC_EDIT_RIGHTDOWN).SetWindowText(logic::KeyboardUtils::GetKeyName(mp->VKMoveRightDown, 0).c_str());

	GetDlgItem(IDC_EDIT_SCROLL_UP).SetWindowText(logic::KeyboardUtils::GetKeyName(mp->VKWheelUp, 0).c_str());
	GetDlgItem(IDC_EDIT_SCROLL_DOWN).SetWindowText(logic::KeyboardUtils::GetKeyName(mp->VKWheelDown, 0).c_str());

	GetDlgItem(IDC_EDIT_HOTKEY).SetWindowText(GetHotkeyName(mp->modHotkey, mp->VKHotkey));
		
	CComboBox cbEnabler(GetDlgItem(IDC_COMBO_ACTIVATION));
	for (int i = 0; i < cbEnabler.GetCount(); i++)
	{
		if (static_cast<logic::KeyboardUtils::VirtualKey_t>(cbEnabler.GetItemData(i)) == mp->VKEnabler)
		{
			cbEnabler.SetCurSel(i);
			break;
		}
	}
	cbEnabler.Detach();

	CComboBox cbAlt(GetDlgItem(IDC_COMBO_ALT_MOD));
	for (int i = 0; i < cbAlt.GetCount(); i++)
	{
		if (static_cast<logic::KeyboardUtils::VirtualKey_t>(cbAlt.GetItemData(i)) == mp->VKAccelerated)
		{
			cbAlt.SetCurSel(i);
			break;
		}
	}
	cbAlt.Detach();

	CComboBox cbUnbind(GetDlgItem(IDC_COMBO_UNBIND));
	for (int i = 0; i < cbUnbind.GetCount(); i++)
	{
		if (static_cast<logic::KeyboardUtils::VirtualKey_t>(cbUnbind.GetItemData(i)) == mp->VKActivationMod)
		{
			cbUnbind.SetCurSel(i);
			break;
		}
	}
	cbUnbind.Detach();

	CComboBox cbStickyKey(GetDlgItem(IDC_COMBO_STICKYKEYS));
	for (int i = 0; i < cbStickyKey.GetCount(); i++)
	{
		if (static_cast<logic::KeyboardUtils::VirtualKey_t>(cbStickyKey.GetItemData(i)) == mp->VKStickyKey)
		{
			cbStickyKey.SetCurSel(i);
			break;
		}
	}	

	CButton checkBox(GetDlgItem(IDC_CHECK_MINIMIZE));
	checkBox.SetCheck(mp->minimizeOnStartup ? BST_CHECKED : BST_UNCHECKED);
	checkBox.Detach();

	checkBox.Attach(GetDlgItem(IDC_CHECK_AUTOACTIVATE));
	checkBox.SetCheck(mp->activateOnStartup ? BST_CHECKED : BST_UNCHECKED);
	checkBox.Detach();

	checkBox.Attach(GetDlgItem(IDC_CHECK_CURSOR));
	checkBox.SetCheck(mp->changeCursor ? BST_CHECKED : BST_UNCHECKED);
	checkBox.Detach();

	checkBox.Attach(GetDlgItem(IDC_CHECK_NOTIFICATIONS));
	checkBox.SetCheck(mp->showNotifications ? BST_CHECKED : BST_UNCHECKED);
	checkBox.Detach();

	SynchronizeCombos();

	OnComboSelChange(0, IDC_COMBO_ACTIVATION, GetDlgItem(IDC_COMBO_ACTIVATION));
}


//---------------------------------------------------------------------------------------------------------------------
bool ComboBoxContains(const CComboBox & cb, DWORD val)
{
	for (int i = 0; i < cb.GetCount(); i++)
	{
		if (val == cb.GetItemData(i))
		{
			return true;
		}
	}

	return false;
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::FillModifierCombobox(
	CComboBox & comboBox, const std::vector<DWORD> & keys, const std::set<DWORD> & skip, DWORD valueToSet)
{
	while (comboBox.GetCount() > 0)
	{
		comboBox.DeleteString(0);
	}

	bool neededValueFound = false;
	for (DWORD key: keys)
	{
		if ( skip.find(key) == skip.end() )
		{
			std::wstring itemLabel = logic::KeyboardUtils::GetKeyName(key, 0);
			if (itemLabel.empty()) itemLabel = _("main.combo-item-none");
			comboBox.SetItemData(	comboBox.AddString(itemLabel.c_str()), key );
			if (key == valueToSet) neededValueFound = true;
		}
	}

	if (!neededValueFound) valueToSet = 0;

	for (int i = 0; i < comboBox.GetCount(); i++)
	{
		if (valueToSet == comboBox.GetItemData(i))
		{
			comboBox.SetCurSel(i);
			break;
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::SynchronizeCombos()
{
	// controlingl comboboxes which are used to select modifier keys
	// make sure that if a modifier has been in one of the comboboxes, it doesn't 
	// show up in the others

	// keys which should be shown in the comboboxes by default, i.e. when
	// there's not hotkey specified and all comboboxes have [None] value selected
	static const DWORD kDefaultValue = 0; // [None] value
	static const std::vector<DWORD> keys = { 
		kDefaultValue, VK_LCONTROL, VK_LMENU, VK_LSHIFT, VK_RCONTROL, VK_RMENU, VK_RSHIFT };
	
	// list of the comboboxes to process, with their values retrieved from the settings
	typedef std::vector< std::pair<CComboBox, DWORD> > ComboboxDescriptor_t;
	ComboboxDescriptor_t comboBoxes = {
		std::make_pair( CComboBox(GetDlgItem(IDC_COMBO_ALT_MOD)), logic::MainSingleton::Instance().GetMouseParams()->VKAccelerated),
		std::make_pair( CComboBox(GetDlgItem(IDC_COMBO_UNBIND)), logic::MainSingleton::Instance().GetMouseParams()->VKActivationMod),
		std::make_pair( CComboBox(GetDlgItem(IDC_COMBO_STICKYKEYS)), logic::MainSingleton::Instance().GetMouseParams()->VKStickyKey),
	};

	// set of the keys taken so far
	std::set<DWORD> takenKeys;

	// if hotkey is enabled, mark modifiers which it compises as taken
	if (logic::MainSingleton::Instance().GetMouseParams()->UseHotkey())
	{
		const DWORD hotkeyModifiers = logic::MainSingleton::Instance().GetMouseParams()->modHotkey;
		if (hotkeyModifiers & MOD_CONTROL)
		{
			takenKeys.insert(VK_LCONTROL);
			takenKeys.insert(VK_RCONTROL);
		}

		if (hotkeyModifiers & MOD_SHIFT)
		{
			takenKeys.insert(VK_LSHIFT);
			takenKeys.insert(VK_RSHIFT);
		}

		if (hotkeyModifiers & MOD_ALT)
		{
			takenKeys.insert(VK_LMENU);
			takenKeys.insert(VK_RMENU);
		}
	}

	// For each combobox, and if a value which should be assigned to it has been
	// already taken, fallback to the [None] value. 
	for (ComboboxDescriptor_t::value_type & aValue: comboBoxes)
	{
		if (!takenKeys.insert(aValue.second).second)
		{
			aValue.second = kDefaultValue;
		}

		// Clear combobox, it will be populated with the correct value later
		while (aValue.first.GetCount() > 0)
		{
			aValue.first.DeleteString(0);
		}
	}

	// fill each of the combobox with the appropriate values
	for (ComboboxDescriptor_t::value_type & aValue: comboBoxes)
	{
		// we'll pass two sets in FillModifierCombobox:
		// 1) with all possible values which might be present in a combobox
		// 2) with the values which should be skipped while filling it (i.e. keys 
		//    taken by the hotkey or the other comboboxes).
		std::set<DWORD> skipSet = takenKeys;
		// [None] value should be present in all combos; remove also the actual value of this
		// combobox from the "skip" set
		skipSet.erase(kDefaultValue);
		skipSet.erase(aValue.second);
		FillModifierCombobox(aValue.first, keys, skipSet, aValue.second);
		// normally value shouldn't be changed at this point, but propagate it back to be sure
		aValue.second = aValue.first.GetItemData(aValue.first.GetCurSel());
		aValue.first.Detach();
	}

	// since the values in the comboboxes might have been changed, propagate them back to the settings
	logic::MainSingleton::Instance().GetMouseParams()->VKAccelerated = comboBoxes[0].second;
	logic::MainSingleton::Instance().GetMouseParams()->VKActivationMod = comboBoxes[1].second;
	logic::MainSingleton::Instance().GetMouseParams()->VKStickyKey = comboBoxes[2].second;
	
	// enable or disable the Alternative Speed value input box depending on the selection
	// in the corresponding combobox
	CComboBox comboAlt(GetDlgItem(IDC_COMBO_ALT_MOD));
	GetDlgItem(IDC_EDIT_ALT_SPEED).EnableWindow(comboAlt.GetItemData(comboAlt.GetCurSel()) != 0);
	comboAlt.Detach();
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::localize()
{
	GetDlgItem(IDC_GROUP_ACTIVATION).SetWindowText(_("main.gb-activation"));

	GetDlgItem(IDC_STATIC_UNBIND).SetWindowText(_("main.lbl-activation-modifier"));

	GetDlgItem(IDC_GROUP_KEYBINDINGS).SetWindowText(_("main.gb-quicksettings"));
	GetDlgItem(IDC_GROUP_BUTTONS).SetWindowText(_("main.gb-buttons"));
	GetDlgItem(IDC_GROUP_MISC).SetWindowText(_("main.gb-miscellaneous"));

	GetDlgItem(IDC_STATIC_SPEED).SetWindowText(_("main.lbl-speed"));
	GetDlgItem(IDC_STATIC_ALT_SPEED).SetWindowText(_("main.lbl-alt-speed"));
	GetDlgItem(IDC_STATIC_BTN_LEFT).SetWindowText(_("main.lbl-left-btn"));
	GetDlgItem(IDC_STATIC_BTN_RIGHT).SetWindowText(_("main.lbl-right-btn"));
	GetDlgItem(IDC_STATIC_BTN_MIDDLE).SetWindowText(_("main.lbl-middle-btn"));

	GetDlgItem(IDC_STATIC_UP).SetWindowText(_("main.lbl-up"));
	GetDlgItem(IDC_STATIC_DOWN).SetWindowText(_("main.lbl-down"));
	GetDlgItem(IDC_STATIC_LEFT).SetWindowText(_("main.lbl-left"));
	GetDlgItem(IDC_STATIC_RIGHT).SetWindowText(_("main.lbl-right"));

	GetDlgItem(IDC_STATIC_LEFTUP).SetWindowText(_("main.lbl-left-up"));
	GetDlgItem(IDC_STATIC_RIGHTUP).SetWindowText(_("main.lbl-right-up"));
	GetDlgItem(IDC_STATIC_LEFTDOWN).SetWindowText(_("main.lbl-left-down"));
	GetDlgItem(IDC_STATIC_RIGHTDOWN).SetWindowText(_("main.lbl-right-down"));

	GetDlgItem(IDC_STATIC_SCROLL_UP).SetWindowText(_("main.lbl-scroll-up"));
	GetDlgItem(IDC_STATIC_SCROLL_DOWN).SetWindowText(_("main.lbl-scroll-down"));

	GetDlgItem(IDC_STATIC_ACTIVATION).SetWindowText(_("main.lbl-activation"));

	GetDlgItem(IDC_STATIC_STICKYKEYS).SetWindowText(_("main.lbl-sticky-keys"));

	
	// label "On startup"
	neatcommon::ui::CAutosizeStatic lblOnstartup;
	lblOnstartup.Attach(GetDlgItem(IDC_STATIC_ONSTARTUP));
	lblOnstartup.SetText(_("main.lbl-onstartup"));
	lblOnstartup.Detach();

	// label "Show"
	neatcommon::ui::CAutosizeStatic lblShow;
	lblShow.Attach(GetDlgItem(IDC_STATIC_SHOW));
	lblShow.SetText(_("main.lbl-show"));
	lblShow.Detach();

	CRect layoutRect;


	// checkbox "Minimize" (row "On startup")
	CWindow wnd;
	wnd.Attach(GetDlgItem(IDC_CHECK_MINIMIZE));
	wnd.SetWindowText(_("main.chk-minimize"));
	AutosizeCheckbox(wnd);
	wnd.GetWindowRect(&layoutRect);
	ScreenToClient(&layoutRect);
	wnd.Detach();
	LONG rowOnStartupTop = layoutRect.top;
	LONG maxRight = layoutRect.right;

	// checkbox "Icon near cursor" (row "Show")
	wnd.Attach(GetDlgItem(IDC_CHECK_CURSOR));
	wnd.SetWindowText(_("main.chk-cursor"));
	AutosizeCheckbox(wnd);
	wnd.GetWindowRect(&layoutRect);
	ScreenToClient(&layoutRect);
	wnd.Detach();
	LONG rowShowTop = layoutRect.top;
	maxRight = (maxRight < layoutRect.right) ? layoutRect.right + 7 : maxRight + 7;

	// checkbox "Activate emulation" (row "On startup")
	wnd.Attach(GetDlgItem(IDC_CHECK_AUTOACTIVATE));
	wnd.SetWindowText(_("main.chk-autoactivate"));
	wnd.SetWindowPos(HWND_BOTTOM, maxRight, rowOnStartupTop, 0, 0, SWP_NOSIZE);
	AutosizeCheckbox(wnd);
	wnd.Detach();

	// checkbox "Notification" (row "Show")
	wnd.Attach(GetDlgItem(IDC_CHECK_NOTIFICATIONS));
	wnd.SetWindowText(_("main.chk-notifications"));
	wnd.SetWindowPos(HWND_BOTTOM, maxRight, rowShowTop, 0, 0, SWP_NOSIZE);
	AutosizeCheckbox(wnd);
	wnd.Detach();

	// "Speed" row
	neatcommon::ui::CAutosizeStatic lblSpeed;
	lblSpeed.Attach(GetDlgItem(IDC_STATIC_SPEED));
	lblSpeed.SetText(_("main.lbl-speed"));
	lblSpeed.GetWindowRect(&layoutRect);
	ScreenToClient(&layoutRect);
	lblSpeed.Detach();

	// "Speed" edit box
	wnd.Attach(GetDlgItem(IDC_EDIT_SPEED));
	wnd.SetWindowPos(HWND_BOTTOM, layoutRect.right + 7, layoutRect.top - 3, 0, 0, SWP_NOSIZE);
	wnd.GetWindowRect(&layoutRect);
	ScreenToClient(&layoutRect);
	wnd.Detach();

	// "Aternative speed" label
	neatcommon::ui::CAutosizeStatic lblAltSpeed;
	lblAltSpeed.Attach(GetDlgItem(IDC_STATIC_ALT_SPEED));
	lblAltSpeed.SetText(_("main.lbl-alt-speed"));
	lblAltSpeed.GetWindowRect(&layoutRect);
	ScreenToClient(&layoutRect);
	lblAltSpeed.Detach();

	// "Alternative speed modifier" combo box
	wnd.Attach(GetDlgItem(IDC_COMBO_ALT_MOD));
	wnd.SetWindowPos(HWND_BOTTOM, layoutRect.right + 7, layoutRect.top - 2, 0, 0, SWP_NOSIZE);
	wnd.GetWindowRect(&layoutRect);
	ScreenToClient(&layoutRect);
	wnd.Detach();

	// "Alternative speed" value
	wnd.Attach(GetDlgItem(IDC_EDIT_ALT_SPEED));
	wnd.SetWindowPos(HWND_BOTTOM, layoutRect.right + 7, layoutRect.top - 1, 0, 0, SWP_NOSIZE);
	wnd.Detach();
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT 
CNeatMouseWtlView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	for (size_t i = 0; i < BTN_DEL_COUNT; i++)
	{		
		btnDel[i].SubclassWindow(GetDlgItem(IDC_BTN_DEL1 + i));
		btnDel[i].SetParent(this->m_hWnd);
		btnDel[i].SetIcon(IDI_CROSSB, IDI_CROSS);
		btnDel[i].ResizeClient(16, 16);
		btnDel[i].SetWindowText(_T(""));
		btnDel[i].SetTooltipText(L"");
		btnDel[i].DrawBorder(false);
	}

	btnDelHotkey.SubclassWindow(GetDlgItem(IDC_DEL_HOTKEY));
	btnDelHotkey.SetIcon(IDI_CROSSB, IDI_CROSS);
	btnDelHotkey.ResizeClient(16, 16);
	btnDelHotkey.SetWindowText(_T(""));
	btnDelHotkey.SetTooltipText(L"");
	btnDelHotkey.DrawBorder(false);	
	
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_ACTIVATION, SafeLoadPng(IDB_PNG_PLUG_CONNECT)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_SPEED, SafeLoadPng(IDB_PNG_M_SPEED)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_ALT_SPEED, SafeLoadPng(IDB_PNG_M_ALT_SPEED)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_BTN_LEFT, SafeLoadPng(IDB_PNG_M_BTN_LEFT)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_BTN_RIGHT, SafeLoadPng(IDB_PNG_M_BTN_RIGHT)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_BTN_MIDDLE, SafeLoadPng(IDB_PNG_M_BTN_MIDDLE)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_UP, SafeLoadPng(IDB_PNG_M_UP)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_DOWN, SafeLoadPng(IDB_PNG_M_DOWN)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_LEFT, SafeLoadPng(IDB_PNG_M_LEFT)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_RIGHT, SafeLoadPng(IDB_PNG_M_RIGHT)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_LEFTUP, SafeLoadPng(IDB_PNG_M_LEFTUP)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_RIGHTDOWN, SafeLoadPng(IDB_PNG_M_RIGHTDOWN)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_LEFTDOWN, SafeLoadPng(IDB_PNG_M_LEFTDOWN)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_RIGHTUP, SafeLoadPng(IDB_PNG_M_RIGHTUP)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_SCROLL_UP, SafeLoadPng(IDB_PNG_M_SCROLL_UP)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_SCROLL_DOWN, SafeLoadPng(IDB_PNG_M_SCROLL_DOWN)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_ONSTARTUP, SafeLoadPng(IDB_PNG_ROCKET)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_SHOW, SafeLoadPng(IDB_PNG_EYE)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_UNBIND, SafeLoadPng(IDB_PNG_M_ALT_MOD)));
	icons.insert(std::pair<UINT, HBITMAP>(IDC_STATIC_STICKYKEYS, SafeLoadPng(IDB_PNG_CURSORLIFEBUOY)));

	SynchronizeCombos();

	CComboBox cbEnabler;
	cbEnabler.Attach(GetDlgItem(IDC_COMBO_ACTIVATION));
	cbEnabler.SetItemData(cbEnabler.AddString(logic::KeyboardUtils::GetKeyName(VK_NUMLOCK, 0).c_str()), VK_NUMLOCK);
	cbEnabler.SetItemData(cbEnabler.AddString(logic::KeyboardUtils::GetKeyName(VK_CAPITAL, 0).c_str()), VK_CAPITAL);
	cbEnabler.SetItemData(cbEnabler.AddString(logic::KeyboardUtils::GetKeyName(VK_SCROLL, 0).c_str()), VK_SCROLL);
	cbEnabler.SetItemData(cbEnabler.AddString(L"Hotkey"), 0);
	cbEnabler.SetCurSel(0);
	cbEnabler.Detach();

	{
		CWindow wnd = GetDlgItem(IDC_CHECK_DUMMY);
		CRect r;
		wnd.GetClientRect(&r);
		this->checkBoxPadding = r.Width();
	}

	localize();
	
	PopulateControls();

	if (logic::MainSingleton::Instance().GetMouseParams()->activateOnStartup)
	{
		logic::MainSingleton::Instance().GetMouseActioner().activateEmulation(true);
		logic::MainSingleton::Instance().UpdateCursor();
	}
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::enableAdvancedMode()
{
	ToggleAdvancedItems();

	CRect lastGroupRect;
	GetDlgItem(IDC_GROUP_MISC).GetWindowRect(&lastGroupRect);
	ScreenToClient(&lastGroupRect);
	this->ResizeClient(lastGroupRect.Width() + lastGroupRect.left * 2, lastGroupRect.top + lastGroupRect.Height() + 11);

	GetDlgItem(IDC_GROUP_KEYBINDINGS).SetWindowText(_("main.gb-movement"));

	showAdvancedControls(true);
	this->RedrawWindow();
}
	

//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::disableAdvancedMode()
{
	ToggleAdvancedItems();

	CRect lastGroupRect;
	GetDlgItem(IDC_GROUP_KEYBINDINGS).GetWindowRect(&lastGroupRect);
	ScreenToClient(&lastGroupRect);
	this->ResizeClient(lastGroupRect.Width() + lastGroupRect.left * 2, lastGroupRect.top + lastGroupRect.Height() + 11);

	GetDlgItem(IDC_GROUP_KEYBINDINGS).SetWindowText(_("main.gb-quicksettings"));

	showAdvancedControls(false);

	this->RedrawWindow();	
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::showAdvancedControls(bool show)
{
	static const std::vector<UINT> itemsToHide
	{
		IDC_STATIC_LEFTUP,
		IDC_STATIC_LEFTDOWN,
		IDC_STATIC_RIGHTUP,
		IDC_STATIC_RIGHTDOWN,
		IDC_EDIT_LEFTUP,
		IDC_EDIT_LEFTDOWN,
		IDC_EDIT_RIGHTUP,
		IDC_EDIT_RIGHTDOWN,
		IDC_BTN_DEL10,
		IDC_BTN_DEL11,
		IDC_BTN_DEL12,
		IDC_BTN_DEL13,
		IDC_STATIC_ALT_SPEED,
		IDC_COMBO_ALT_MOD,
		IDC_EDIT_ALT_SPEED,
		IDC_GROUP_BUTTONS,
		IDC_GROUP_MISC
	};

	for (UINT itemId: itemsToHide)
	{
		GetDlgItem(itemId).ShowWindow(show ? SW_SHOW : SW_HIDE);
	}
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::AutosizeCheckbox(CWindow & wnd)
{
	CString s;
	int n = wnd.GetWindowTextLength();
	wnd.GetWindowText(s.GetBufferSetLength(n), n + 1);
	CDC dc = wnd.GetDC();
	CSize sz;
	HFONT f = dc.SelectFont(wnd.GetFont());
	dc.GetTextExtent(s, s.GetLength(), &sz);
	dc.SelectFont(f);
	CRect r;
	wnd.GetClientRect(&r);
	wnd.SetWindowPos(HWND_BOTTOM, 0, 0, sz.cx + checkBoxPadding, r.Height(), SWP_NOMOVE);
}


//---------------------------------------------------------------------------------------------------------------------
void 
CNeatMouseWtlView::SwapItems(int item1, int item2)
{
	CRect aItemRect1, aItemRect2;
	CWindow aWnd1 = GetDlgItem(item1);
	CWindow aWnd2 = GetDlgItem(item2);
	aWnd1.GetWindowRect(&aItemRect1);
	ScreenToClient(&aItemRect1);
	aWnd2.GetWindowRect(&aItemRect2);
	ScreenToClient(&aItemRect2);
	aWnd1.MoveWindow(aItemRect2.left, aItemRect2.top, aItemRect1.Width(), aItemRect1.Height());
	aWnd2.MoveWindow(aItemRect1.left, aItemRect1.top, aItemRect2.Width(), aItemRect2.Height());
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatMouseWtlView::ToggleAdvancedItems()
{
	SwapItems(IDC_EDIT_BTN_LEFT, IDC_EDIT_LEFTUP);
	SwapItems(IDC_EDIT_BTN_RIGHT, IDC_EDIT_RIGHTUP);
	SwapItems(IDC_EDIT_BTN_MIDDLE, IDC_EDIT_LEFTDOWN);

	SwapItems(IDC_STATIC_BTN_LEFT, IDC_STATIC_LEFTUP);
	SwapItems(IDC_STATIC_BTN_RIGHT, IDC_STATIC_RIGHTUP);
	SwapItems(IDC_STATIC_BTN_MIDDLE, IDC_STATIC_LEFTDOWN);

	SwapItems(IDC_BTN_DEL1, IDC_BTN_DEL10);
	SwapItems(IDC_BTN_DEL2, IDC_BTN_DEL11);
	SwapItems(IDC_BTN_DEL3, IDC_BTN_DEL12);
}

} // namespace neatmouse