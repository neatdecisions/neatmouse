//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
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


namespace {
	constexpr int ACTIVATION_HOTKEY_ID = 1;
}



//---------------------------------------------------------------------------------------------------------------------
void
CNeatMouseWtlView::SetToolbar(CNeatToolbar * pToolbar)
{
	m_tb = pToolbar;
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
			if (m_tb != nullptr) m_tb->RedrawMe();
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

				const USHORT scanCode = (pMsg->lParam >> 16) & 0xFF;
				int vk = pMsg->wParam;

				if (HIWORD(pMsg->lParam) & KF_EXTENDED)
				{
					vk = -vk;
				}

				if (logic::MainSingleton::Instance().GetMouseParams().BindingExists(vk)) break;

				if (pMsg->hwnd == GetDlgItem(IDC_EDIT_HOTKEY))
				{
					if (!logic::MainSingleton::Instance().GetMouseParams().UseHotkey())
					{
						ASSERT(false);
						break;
					}

					UINT mods = 0;
					if (logic::KeyboardUtils::IsKeyDown(VK_CONTROL) &&
					    !logic::MainSingleton::Instance().GetMouseParams().isModifierTaken(MOD_CONTROL))
					{
						mods |= MOD_CONTROL;
					}

					if (logic::KeyboardUtils::IsKeyDown(VK_MENU) &&
					    !logic::MainSingleton::Instance().GetMouseParams().isModifierTaken(MOD_ALT))
					{
						mods |= MOD_ALT;
					}

					if (logic::KeyboardUtils::IsKeyDown(VK_SHIFT) &&
					    !logic::MainSingleton::Instance().GetMouseParams().isModifierTaken(MOD_SHIFT))
					{
						mods |= MOD_SHIFT;
					}

					if (mods == 0) break;

					if (EnableHotkey(mods, pMsg->wParam))
					{
						auto mouseParams = logic::MainSingleton::Instance().GetMouseParams();
						mouseParams.VKHotkey = pMsg->wParam;
						mouseParams.modHotkey = mods;
						logic::MainSingleton::Instance().UpdateMouseParams(mouseParams);
						::SetWindowText(GetDlgItem(IDC_EDIT_HOTKEY), GetHotkeyName(mods, pMsg->wParam));
					} else
					{
						if (!EnableHotkey(
								logic::MainSingleton::Instance().GetMouseParams().modHotkey,
								logic::MainSingleton::Instance().GetMouseParams().VKHotkey))
						{
							auto mouseParams = logic::MainSingleton::Instance().GetMouseParams();
							mouseParams.modHotkey = 0;
							mouseParams.VKHotkey = 0;
							logic::MainSingleton::Instance().UpdateMouseParams(mouseParams);
							::SetWindowText(GetDlgItem(IDC_EDIT_HOTKEY), L"");
							ASSERT(false);
						}
					}

					SynchronizeCombos();
					UpdateToolbarButtons();
					break;
				}

				// list of the edit boxes which can accept input
				static const std::array<DWORD, 13> hwnds
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
					auto mouseParams = logic::MainSingleton::Instance().GetMouseParams();
					switch (id)
					{
					case IDC_EDIT_BTN_LEFT:
						mouseParams.VKPressLB = vk;
						break;
					case IDC_EDIT_BTN_RIGHT:
						mouseParams.VKPressRB = vk;
						break;
					case IDC_EDIT_BTN_MIDDLE:
						mouseParams.VKPressMB = vk;
						break;
					case IDC_EDIT_UP:
						mouseParams.VKMoveUp = vk;
						break;
					case IDC_EDIT_DOWN:
						mouseParams.VKMoveDown = vk;
						break;
					case IDC_EDIT_LEFT:
						mouseParams.VKMoveLeft = vk;
						break;
					case IDC_EDIT_RIGHT:
						mouseParams.VKMoveRight = vk;
						break;
					case IDC_EDIT_SCROLL_UP:
						mouseParams.VKWheelUp = vk;
						break;
					case IDC_EDIT_SCROLL_DOWN:
						mouseParams.VKWheelDown = vk;
						break;
					case IDC_EDIT_LEFTUP:
						mouseParams.VKMoveLeftUp = vk;
						break;
					case IDC_EDIT_RIGHTUP:
						mouseParams.VKMoveRightUp = vk;
						break;
					case IDC_EDIT_LEFTDOWN:
						mouseParams.VKMoveLeftDown = vk;
						break;
					case IDC_EDIT_RIGHTDOWN:
						mouseParams.VKMoveRightDown = vk;
						break;
					}
					logic::MainSingleton::Instance().UpdateMouseParams(mouseParams);

					UpdateToolbarButtons();
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

	for (const auto & aIcon : m_icons)
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
	auto mouseParams = logic::MainSingleton::Instance().GetMouseParams();
	mouseParams.minimizeOnStartup = (checkBox.GetCheck() == BST_CHECKED);
	logic::MainSingleton::Instance().UpdateMouseParams(mouseParams);
	checkBox.Detach();
	UpdateToolbarButtons();
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatMouseWtlView::OnActivateOnStartupCheck(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	CButton checkBox(GetDlgItem(IDC_CHECK_AUTOACTIVATE));
	auto mouseParams = logic::MainSingleton::Instance().GetMouseParams();
	mouseParams.activateOnStartup = (checkBox.GetCheck() == BST_CHECKED);
	logic::MainSingleton::Instance().UpdateMouseParams(mouseParams);
	checkBox.Detach();
	UpdateToolbarButtons();
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatMouseWtlView::OnCursorCheck(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	CButton checkBox(GetDlgItem(IDC_CHECK_CURSOR));
	auto mouseParams = logic::MainSingleton::Instance().GetMouseParams();
	mouseParams.changeCursor = (checkBox.GetCheck() == BST_CHECKED);
	logic::MainSingleton::Instance().UpdateMouseParams(mouseParams);
	checkBox.Detach();
	UpdateToolbarButtons();
	logic::MainSingleton::Instance().TriggerOverlay();
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatMouseWtlView::OnShowNotificationsCheck(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	CButton checkBox(GetDlgItem(IDC_CHECK_NOTIFICATIONS));
	auto mouseParams = logic::MainSingleton::Instance().GetMouseParams();
	mouseParams.showNotifications = (checkBox.GetCheck() == BST_CHECKED);
	logic::MainSingleton::Instance().UpdateMouseParams(mouseParams);
	checkBox.Detach();
	UpdateToolbarButtons();
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatMouseWtlView::OnDelHotkeyClick(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	::SetWindowText(GetDlgItem(IDC_EDIT_HOTKEY), L"");
	auto mouseParams = logic::MainSingleton::Instance().GetMouseParams();
	mouseParams.VKHotkey = 0;
	mouseParams.modHotkey = 0;
	logic::MainSingleton::Instance().UpdateMouseParams(mouseParams);
	DisableHotkey();
	SynchronizeCombos();
	UpdateToolbarButtons();
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatMouseWtlView::OnDelBtnClick(UINT /*uCode*/, int nID, HWND /*hwndCtrl*/)
{
	auto mouseParams = logic::MainSingleton::Instance().GetMouseParams();
	switch (nID)
	{
	case IDC_BTN_DEL1:
		::SetWindowText(GetDlgItem(IDC_EDIT_BTN_LEFT), L"");
		mouseParams.VKPressLB = 0;
		break;
	case IDC_BTN_DEL2:
		::SetWindowText(GetDlgItem(IDC_EDIT_BTN_RIGHT), L"");
		mouseParams.VKPressRB = 0;
		break;
	case IDC_BTN_DEL3:
		::SetWindowText(GetDlgItem(IDC_EDIT_BTN_MIDDLE), L"");
		mouseParams.VKPressMB = 0;
		break;
	case IDC_BTN_DEL4:
		::SetWindowText(GetDlgItem(IDC_EDIT_SCROLL_UP), L"");
		mouseParams.VKWheelUp = 0;
		break;
	case IDC_BTN_DEL5:
		::SetWindowText(GetDlgItem(IDC_EDIT_SCROLL_DOWN), L"");
		mouseParams.VKWheelDown = 0;
		break;
	case IDC_BTN_DEL6:
		::SetWindowText(GetDlgItem(IDC_EDIT_UP), L"");
		mouseParams.VKMoveUp = 0;
		break;
	case IDC_BTN_DEL7:
		::SetWindowText(GetDlgItem(IDC_EDIT_DOWN), L"");
		mouseParams.VKMoveDown = 0;
		break;
	case IDC_BTN_DEL8:
		::SetWindowText(GetDlgItem(IDC_EDIT_LEFT), L"");
		mouseParams.VKMoveLeft = 0;
		break;
	case IDC_BTN_DEL9:
		::SetWindowText(GetDlgItem(IDC_EDIT_RIGHT), L"");
		mouseParams.VKMoveRight = 0;
		break;
	case IDC_BTN_DEL10:
		::SetWindowText(GetDlgItem(IDC_EDIT_LEFTUP), L"");
		mouseParams.VKMoveLeftUp = 0;
		break;
	case IDC_BTN_DEL11:
		::SetWindowText(GetDlgItem(IDC_EDIT_RIGHTUP), L"");
		mouseParams.VKMoveRightUp = 0;
		break;
	case IDC_BTN_DEL12:
		::SetWindowText(GetDlgItem(IDC_EDIT_LEFTDOWN), L"");
		mouseParams.VKMoveLeftDown = 0;
		break;
	case IDC_BTN_DEL13:
		::SetWindowText(GetDlgItem(IDC_EDIT_RIGHTDOWN), L"");
		mouseParams.VKMoveRightDown = 0;
		break;
	default:
		// do not update button states
		return;
	}
	logic::MainSingleton::Instance().UpdateMouseParams(mouseParams);
	UpdateToolbarButtons();
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
	auto mouseParams = logic::MainSingleton::Instance().GetMouseParams();
	switch (nID)
	{
	case IDC_COMBO_ACTIVATION:
		{
			logic::MainSingleton::Instance().GetMouseActioner().reset();
			CComboBox cb(hwndCtrl);
			int n = cb.GetCurSel();
			ASSERT(n >= 0);
			if (n >= 0) mouseParams.VKEnabler = cb.GetItemData(n);

			if (mouseParams.UseHotkey())
			{
				::ShowWindow(GetDlgItem(IDC_EDIT_HOTKEY), SW_SHOW);
				m_btnDelHotkey.ShowWindow(SW_SHOW);

				if (mouseParams.isModifierTaken(mouseParams.modHotkey) ||
					 !EnableHotkey(mouseParams.modHotkey, mouseParams.VKHotkey))
				{
					mouseParams.modHotkey = 0;
					mouseParams.VKHotkey = 0;
					::SetWindowText(GetDlgItem(IDC_EDIT_HOTKEY), L"");
				}
			} else
			{
				::ShowWindow(GetDlgItem(IDC_EDIT_HOTKEY), SW_HIDE);
				m_btnDelHotkey.ShowWindow(SW_HIDE);
				DisableHotkey();
			}
			cb.Detach();
		}
		break;
	case IDC_COMBO_ALT_MOD:
		{
			CComboBox cb(hwndCtrl);
			int n = cb.GetCurSel();
			ASSERT(n >= 0);
			if (n >= 0) mouseParams.VKAccelerated = cb.GetItemData(n);
			cb.Detach();
		}
		break;
	case IDC_COMBO_UNBIND:
		{
			logic::MainSingleton::Instance().GetMouseActioner().reset();
			CComboBox cb(hwndCtrl);
			int n = cb.GetCurSel();
			ASSERT(n >= 0);
			if (n >= 0) mouseParams.VKActivationMod = cb.GetItemData(n);
			cb.Detach();
		}
		break;
	case IDC_COMBO_STICKYKEYS:
		{
			CComboBox cb(hwndCtrl);
			int n = cb.GetCurSel();
			ASSERT(n >= 0);
			if (n >= 0) mouseParams.VKStickyKey = cb.GetItemData(n);
			cb.Detach();
		}
		break;
	default:
		return;
	}

	logic::MainSingleton::Instance().UpdateMouseParams(mouseParams);
	SynchronizeCombos();
	UpdateToolbarButtons();
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CNeatMouseWtlView::OnEditChange(UINT /*code*/, UINT id, HWND hwnd, BOOL & bHandled)
{
	bHandled = false;

	if ((id != IDC_EDIT_SPEED) && (id != IDC_EDIT_ALT_SPEED)) return 0;

	CString s;
	int n = ::GetWindowTextLength(hwnd);
	::GetWindowText(hwnd, s.GetBuffer(n), n + 1);

	auto mouseParams = logic::MainSingleton::Instance().GetMouseParams();
	switch (id)
	{
	case IDC_EDIT_SPEED:
		mouseParams.delta = neatcommon::system::from_string_def(s.GetBuffer(0), 20);
		break;
	case IDC_EDIT_ALT_SPEED:
		mouseParams.adelta = neatcommon::system::from_string_def(s.GetBuffer(0), 1);
		break;
	}
	logic::MainSingleton::Instance().UpdateMouseParams(mouseParams);
	UpdateToolbarButtons();

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
				UpdateToolbarButtons();
				return IDYES;
			case IDCANCEL:
				UpdateToolbarButtons();
				return IDCANCEL;
			default:
				logic::MainSingleton::Instance().RevertMouseParams();
				UpdateToolbarButtons();
				return IDNO;
		}
	}

	return IDNO;
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatMouseWtlView::PopulateControls()
{
	const logic::MouseParams & mouseParams = logic::MainSingleton::Instance().GetMouseParams();

	std::wstring s = std::to_wstring(mouseParams.delta);

	GetDlgItem(IDC_EDIT_SPEED).SetWindowText(s.c_str());
	s = std::to_wstring(mouseParams.adelta);
	GetDlgItem(IDC_EDIT_ALT_SPEED).SetWindowText(s.c_str());

	GetDlgItem(IDC_EDIT_BTN_LEFT).SetWindowText(logic::KeyboardUtils::GetKeyName(mouseParams.VKPressLB, 0).c_str());
	GetDlgItem(IDC_EDIT_BTN_RIGHT).SetWindowText(logic::KeyboardUtils::GetKeyName(mouseParams.VKPressRB, 0).c_str());
	GetDlgItem(IDC_EDIT_BTN_MIDDLE).SetWindowText(logic::KeyboardUtils::GetKeyName(mouseParams.VKPressMB, 0).c_str());

	GetDlgItem(IDC_EDIT_UP).SetWindowText(logic::KeyboardUtils::GetKeyName(mouseParams.VKMoveUp, 0).c_str());
	GetDlgItem(IDC_EDIT_DOWN).SetWindowText(logic::KeyboardUtils::GetKeyName(mouseParams.VKMoveDown, 0).c_str());
	GetDlgItem(IDC_EDIT_LEFT).SetWindowText(logic::KeyboardUtils::GetKeyName(mouseParams.VKMoveLeft, 0).c_str());
	GetDlgItem(IDC_EDIT_RIGHT).SetWindowText(logic::KeyboardUtils::GetKeyName(mouseParams.VKMoveRight, 0).c_str());

	GetDlgItem(IDC_EDIT_LEFTUP).SetWindowText(logic::KeyboardUtils::GetKeyName(mouseParams.VKMoveLeftUp, 0).c_str());
	GetDlgItem(IDC_EDIT_RIGHTUP).SetWindowText(logic::KeyboardUtils::GetKeyName(mouseParams.VKMoveRightUp, 0).c_str());
	GetDlgItem(IDC_EDIT_LEFTDOWN).SetWindowText(logic::KeyboardUtils::GetKeyName(mouseParams.VKMoveLeftDown, 0).c_str());
	GetDlgItem(IDC_EDIT_RIGHTDOWN).SetWindowText(logic::KeyboardUtils::GetKeyName(mouseParams.VKMoveRightDown, 0).c_str());

	GetDlgItem(IDC_EDIT_SCROLL_UP).SetWindowText(logic::KeyboardUtils::GetKeyName(mouseParams.VKWheelUp, 0).c_str());
	GetDlgItem(IDC_EDIT_SCROLL_DOWN).SetWindowText(logic::KeyboardUtils::GetKeyName(mouseParams.VKWheelDown, 0).c_str());

	GetDlgItem(IDC_EDIT_HOTKEY).SetWindowText(GetHotkeyName(mouseParams.modHotkey, mouseParams.VKHotkey));

	CComboBox cbEnabler(GetDlgItem(IDC_COMBO_ACTIVATION));
	for (int i = 0; i < cbEnabler.GetCount(); i++)
	{
		if (static_cast<logic::KeyboardUtils::VirtualKey_t>(cbEnabler.GetItemData(i)) == mouseParams.VKEnabler)
		{
			cbEnabler.SetCurSel(i);
			break;
		}
	}
	cbEnabler.Detach();

	CComboBox cbAlt(GetDlgItem(IDC_COMBO_ALT_MOD));
	for (int i = 0; i < cbAlt.GetCount(); i++)
	{
		if (static_cast<logic::KeyboardUtils::VirtualKey_t>(cbAlt.GetItemData(i)) == mouseParams.VKAccelerated)
		{
			cbAlt.SetCurSel(i);
			break;
		}
	}
	cbAlt.Detach();

	CComboBox cbUnbind(GetDlgItem(IDC_COMBO_UNBIND));
	for (int i = 0; i < cbUnbind.GetCount(); i++)
	{
		if (static_cast<logic::KeyboardUtils::VirtualKey_t>(cbUnbind.GetItemData(i)) == mouseParams.VKActivationMod)
		{
			cbUnbind.SetCurSel(i);
			break;
		}
	}
	cbUnbind.Detach();

	CComboBox cbStickyKey(GetDlgItem(IDC_COMBO_STICKYKEYS));
	for (int i = 0; i < cbStickyKey.GetCount(); i++)
	{
		if (static_cast<logic::KeyboardUtils::VirtualKey_t>(cbStickyKey.GetItemData(i)) == mouseParams.VKStickyKey)
		{
			cbStickyKey.SetCurSel(i);
			break;
		}
	}

	CButton checkBox(GetDlgItem(IDC_CHECK_MINIMIZE));
	checkBox.SetCheck(mouseParams.minimizeOnStartup ? BST_CHECKED : BST_UNCHECKED);
	checkBox.Detach();

	checkBox.Attach(GetDlgItem(IDC_CHECK_AUTOACTIVATE));
	checkBox.SetCheck(mouseParams.activateOnStartup ? BST_CHECKED : BST_UNCHECKED);
	checkBox.Detach();

	checkBox.Attach(GetDlgItem(IDC_CHECK_CURSOR));
	checkBox.SetCheck(mouseParams.changeCursor ? BST_CHECKED : BST_UNCHECKED);
	checkBox.Detach();

	checkBox.Attach(GetDlgItem(IDC_CHECK_NOTIFICATIONS));
	checkBox.SetCheck(mouseParams.showNotifications ? BST_CHECKED : BST_UNCHECKED);
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
			comboBox.SetItemData( comboBox.AddString(itemLabel.c_str()), key );
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
	// controlling comboboxes which are used to select modifier keys
	// make sure that if a modifier has been in one of the comboboxes, it doesn't
	// show up in the others

	// keys which should be shown in the comboboxes by default, i.e. when
	// there's not hotkey specified and all comboboxes have [None] value selected
	constexpr DWORD kDefaultValue = 0; // [None] value
	static const std::vector<DWORD> keys = {
		kDefaultValue, VK_LCONTROL, VK_LMENU, VK_LSHIFT, VK_RCONTROL, VK_RMENU, VK_RSHIFT };

	// list of the comboboxes to process, with their values retrieved from the settings
	auto mouseParams = logic::MainSingleton::Instance().GetMouseParams();
	using ComboboxDescriptor_t = std::vector< std::pair<CComboBox, DWORD> >;
	ComboboxDescriptor_t comboBoxes = {
		std::make_pair( CComboBox(GetDlgItem(IDC_COMBO_ALT_MOD)), mouseParams.VKAccelerated),
		std::make_pair( CComboBox(GetDlgItem(IDC_COMBO_UNBIND)), mouseParams.VKActivationMod),
		std::make_pair( CComboBox(GetDlgItem(IDC_COMBO_STICKYKEYS)), mouseParams.VKStickyKey),
	};

	// set of the keys taken so far
	std::set<DWORD> takenKeys;

	// if hotkey is enabled, mark modifiers which it compises as taken
	if (mouseParams.UseHotkey())
	{
		const DWORD hotkeyModifiers = mouseParams.modHotkey;
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
	mouseParams.VKAccelerated = comboBoxes[0].second;
	mouseParams.VKActivationMod = comboBoxes[1].second;
	mouseParams.VKStickyKey = comboBoxes[2].second;
	logic::MainSingleton::Instance().UpdateMouseParams(mouseParams);

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

	SynchronizeCombos();
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CNeatMouseWtlView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	int n = IDC_BTN_DEL1;
	for (auto & btn: m_btnDel)
	{
		btn.SubclassWindow(GetDlgItem(n++));
		btn.SetParent(this->m_hWnd);
		btn.SetIcon(IDI_CROSSB, IDI_CROSS);
		btn.ResizeClient(16, 16);
		btn.SetWindowText(_T(""));
		btn.SetTooltipText(L"");
		btn.DrawBorder(false);
	}

	m_btnDelHotkey.SubclassWindow(GetDlgItem(IDC_DEL_HOTKEY));
	m_btnDelHotkey.SetIcon(IDI_CROSSB, IDI_CROSS);
	m_btnDelHotkey.ResizeClient(16, 16);
	m_btnDelHotkey.SetWindowText(_T(""));
	m_btnDelHotkey.SetTooltipText(L"");
	m_btnDelHotkey.DrawBorder(false);

	m_icons = {
		{ IDC_STATIC_ACTIVATION, SafeLoadPng(IDB_PNG_PLUG_CONNECT) },
		{ IDC_STATIC_SPEED, SafeLoadPng(IDB_PNG_M_SPEED) },
		{ IDC_STATIC_ALT_SPEED, SafeLoadPng(IDB_PNG_M_ALT_SPEED) },
		{ IDC_STATIC_BTN_LEFT, SafeLoadPng(IDB_PNG_M_BTN_LEFT) },
		{ IDC_STATIC_BTN_RIGHT, SafeLoadPng(IDB_PNG_M_BTN_RIGHT) },
		{ IDC_STATIC_BTN_MIDDLE, SafeLoadPng(IDB_PNG_M_BTN_MIDDLE) },
		{ IDC_STATIC_UP, SafeLoadPng(IDB_PNG_M_UP) },
		{ IDC_STATIC_DOWN, SafeLoadPng(IDB_PNG_M_DOWN) },
		{ IDC_STATIC_LEFT, SafeLoadPng(IDB_PNG_M_LEFT) },
		{ IDC_STATIC_RIGHT, SafeLoadPng(IDB_PNG_M_RIGHT) },
		{ IDC_STATIC_LEFTUP, SafeLoadPng(IDB_PNG_M_LEFTUP) },
		{ IDC_STATIC_RIGHTDOWN, SafeLoadPng(IDB_PNG_M_RIGHTDOWN) },
		{ IDC_STATIC_LEFTDOWN, SafeLoadPng(IDB_PNG_M_LEFTDOWN) },
		{ IDC_STATIC_RIGHTUP, SafeLoadPng(IDB_PNG_M_RIGHTUP) },
		{ IDC_STATIC_SCROLL_UP, SafeLoadPng(IDB_PNG_M_SCROLL_UP) },
		{ IDC_STATIC_SCROLL_DOWN, SafeLoadPng(IDB_PNG_M_SCROLL_DOWN) },
		{ IDC_STATIC_ONSTARTUP, SafeLoadPng(IDB_PNG_ROCKET) },
		{ IDC_STATIC_SHOW, SafeLoadPng(IDB_PNG_EYE) },
		{ IDC_STATIC_UNBIND, SafeLoadPng(IDB_PNG_M_ALT_MOD) },
		{ IDC_STATIC_STICKYKEYS, SafeLoadPng(IDB_PNG_CURSORLIFEBUOY) }
	};

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
		m_checkBoxPadding = r.Width();
	}

	localize();

	PopulateControls();

	if (logic::MainSingleton::Instance().GetMouseParams().activateOnStartup)
	{
		logic::MainSingleton::Instance().GetMouseActioner().activateEmulation(true);
		logic::MainSingleton::Instance().TriggerOverlay();
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
	ResizeClient(lastGroupRect.Width() + lastGroupRect.left * 2, lastGroupRect.top + lastGroupRect.Height() + 11);

	GetDlgItem(IDC_GROUP_KEYBINDINGS).SetWindowText(_("main.gb-movement"));

	showAdvancedControls(true);
	RedrawWindow();
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatMouseWtlView::disableAdvancedMode()
{
	ToggleAdvancedItems();

	CRect lastGroupRect;
	GetDlgItem(IDC_GROUP_KEYBINDINGS).GetWindowRect(&lastGroupRect);
	ScreenToClient(&lastGroupRect);
	ResizeClient(lastGroupRect.Width() + lastGroupRect.left * 2, lastGroupRect.top + lastGroupRect.Height() + 11);

	GetDlgItem(IDC_GROUP_KEYBINDINGS).SetWindowText(_("main.gb-quicksettings"));

	showAdvancedControls(false);

	RedrawWindow();
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatMouseWtlView::showAdvancedControls(bool show)
{
	static const std::array<UINT, 17> itemsToHide
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
	wnd.SetWindowPos(HWND_BOTTOM, 0, 0, sz.cx + m_checkBoxPadding, r.Height(), SWP_NOMOVE);
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


//---------------------------------------------------------------------------------------------------------------------
void CNeatMouseWtlView::UpdateToolbarButtons()
{
	if (m_tb != nullptr) m_tb->UpdateButtonStates();
}


} // namespace neatmouse