//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include "NeatMouseWtlView.h"
#include "neatcommon/ui/CustomizedControls.h"

namespace neatmouse {

class CMainFrame : public CFrameWindowImpl<CMainFrame>,
                   public CMessageFilter
{
	BEGIN_MSG_MAP(CMainFrame)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SYSCOMMAND(OnSysCommand)
		MSG_WM_DRAWITEM(OnDrawItem)
		MSG_WM_MEASUREITEM(OnMeasureItem)

		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(NEAT_TRAY_CALLBACK, OnTrayBtnClick)

		COMMAND_HANDLER_EX(ID_TOOLBAR_ADDPRESET, BN_CLICKED, OnBnClickedButtonPresetAdd)
		COMMAND_HANDLER_EX(ID_TOOLBAR_SAVEPRESET, BN_CLICKED, OnBnClickedButtonPresetSave)
		COMMAND_HANDLER_EX(ID_TOOLBAR_REMOVEPRESET, BN_CLICKED, OnBnClickedButtonPresetDelete)
		COMMAND_HANDLER_EX(ID_TOOLBAR_ADVANCEDVIEW, BN_CLICKED, OnBnClickedButtonAdvancedView)
		COMMAND_HANDLER_EX(ID_TOOLBAR_HELP, BN_CLICKED, OnAppAbout)
		COMMAND_HANDLER_EX(IDC_COMBO_PRESETS, CBN_SELCHANGE, OnComboPresetsSelChange)

		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_TRAY_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_TRAY_SHOW, OnTrayShow)
		COMMAND_ID_HANDLER(ID_TRAY_TOGGLEEMULATION, OnTrayToggleEmulation)

		// check language menu click
		if (uMsg == WM_COMMAND)
		{
			SetMsgHandled(TRUE);
			OnLanguageMenuItem((UINT)HIWORD(wParam), (int)LOWORD(wParam), (HWND)lParam);
			lResult = 0;
			if(IsMsgHandled()) return TRUE;
		}

		CHAIN_MSG_MAP_MEMBER(tb)

		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void ToggleVisible();

private:
	void OnClose();
	void OnSysCommand(UINT nID, CPoint point);
	void OnLanguageMenuItem(UINT uCode, int nID, HWND hwndCtrl);
	void OnComboPresetsSelChange(UINT uCode, int nID, HWND hwndCtrl);
	void OnDrawItem(UINT id, LPDRAWITEMSTRUCT lpdis);
	void OnMeasureItem(UINT id, LPMEASUREITEMSTRUCT lpmis);

	LRESULT OnCreate(LPCREATESTRUCT lpcs);
	LRESULT OnBnClickedButtonPresetSave(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnBnClickedButtonPresetAdd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnBnClickedButtonPresetDelete(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnBnClickedButtonAdvancedView(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);
	LRESULT OnAppAbout(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/);

	LRESULT OnTrayBtnClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTrayShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnTrayToggleEmulation(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void RedrawToolbar();
	void localize();
	void TrackTrayMenu();

	void resizeByContent();

	neatcommon::ui::CMenuBitmapsManager menuBitmapsManager;
	CNeatMouseWtlView m_view;
	CNeatToolbar tb;

	int cbPresetsSelectionIndex = 0;
	bool isVisible = true;
};

} // namespace neatmouse
