//
// Copyright © 2016 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the 
// Microsoft Public License (http://opensource.org/licenses/MS-PL) 
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include "NeatToolbar.h"
#include "resource.h"
#include "neatcommon/ui/ButtonST.h"


namespace neatmouse {

#define BTN_DEL_COUNT 13

class CNeatMouseWtlView : public CDialogImpl<CNeatMouseWtlView>
{
public:
	enum { IDD = IDD_NEATMOUSEWTL_FORM };
	
	BOOL PreTranslateMessage(MSG* pMsg);

	CNeatMouseWtlView();
	void SetToolbar(CNeatToolbar * tb);
	int CheckIfChangesSaved();
	void PopulateControls();
	void localize();

	void enableAdvancedMode();
	void disableAdvancedMode();
	void showAdvancedControls(bool show);

protected:
	BEGIN_MSG_MAP(CNeatMouseWtlView)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDC_EDIT_SPEED, EN_CHANGE, OnEditChange)
		COMMAND_HANDLER(IDC_EDIT_ALT_SPEED, EN_CHANGE, OnEditChange)
		COMMAND_HANDLER_EX(IDC_COMBO_ACTIVATION, CBN_SELCHANGE, OnComboSelChange)
		COMMAND_HANDLER_EX(IDC_COMBO_ALT_MOD, CBN_SELCHANGE, OnComboSelChange)
		COMMAND_HANDLER_EX(IDC_COMBO_UNBIND, CBN_SELCHANGE, OnComboSelChange)
		COMMAND_HANDLER_EX(IDC_COMBO_STICKYKEYS, CBN_SELCHANGE, OnComboSelChange)
		COMMAND_HANDLER_EX(IDC_CHECK_MINIMIZE, BN_CLICKED, OnMinimizeOnStartupCheck)
		COMMAND_HANDLER_EX(IDC_CHECK_AUTOACTIVATE, BN_CLICKED, OnActivateOnStartupCheck)
		COMMAND_HANDLER_EX(IDC_CHECK_CURSOR, BN_CLICKED, OnCursorCheck)
		COMMAND_HANDLER_EX(IDC_CHECK_NOTIFICATIONS, BN_CLICKED, OnShowNotificationsCheck)
		COMMAND_RANGE_CODE_HANDLER_EX(IDC_BTN_DEL1, IDC_BTN_DEL13, BN_CLICKED, OnDelBtnClick)
		COMMAND_HANDLER_EX(IDC_DEL_HOTKEY, BN_CLICKED, OnDelHotkeyClick)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_HOTKEY(OnHotKey)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	CNeatToolbar * tb;
	std::map<UINT, HBITMAP> icons;

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	
	LRESULT OnEditChange(UINT code, UINT id, HWND hwnd, BOOL & bHandled);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	void OnMinimizeOnStartupCheck(UINT uCode, int nID, HWND hwndCtrl);
	void OnActivateOnStartupCheck(UINT uCode, int nID, HWND hwndCtrl);
	void OnCursorCheck(UINT uCode, int nID, HWND hwndCtrl);
	void OnShowNotificationsCheck(UINT uCode, int nID, HWND hwndCtrl);
	void OnComboSelChange(UINT uCode, int nID, HWND hwndCtrl);
	void OnDelBtnClick(UINT uCode, int nID, HWND hwndCtrl);
	void OnDelHotkeyClick(UINT uCode, int nID, HWND hwndCtrl);
	
	void OnHotKey(int id, UINT mods, UINT vk);
	void OnDestroy();
	void SynchronizeCombos();
	void AutosizeCheckbox(CWindow & wnd);
	void FillModifierCombobox(CComboBox & comboBox, const std::vector<DWORD> & keys, const std::set<DWORD> & skip, DWORD valueToSet);

	bool EnableHotkey(UINT mods, UINT vk);
	void DisableHotkey();
	void ToggleAdvancedItems();
	void SwapItems(int item1, int item2);

	CString GetHotkeyName(UINT mods, UINT vk);

	neatcommon::ui::CButtonST btnDel[BTN_DEL_COUNT];
	neatcommon::ui::CButtonST btnDelHotkey;
	int checkBoxPadding;
};	

} // namespace neatmouse