//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//


#include "stdafx.h"

#include "MainFrm.h"

#include "AboutDlg.h"
#include "NeatMouseWtlView.h"

#include "logic/HookThread.h"
#include "logic/MainSingleton.h"
#include "neatcommon/ui/CustomizedControls.h"
#include "neatcommon/ui/InputBox.h"

namespace neatmouse {


//---------------------------------------------------------------------------------------------------------------------
BOOL
CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	return (CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		? TRUE
		: m_view.PreTranslateMessage(pMsg);
}


//---------------------------------------------------------------------------------------------------------------------
void
CMainFrame::OnClose()
{
	SetMsgHandled(m_view.CheckIfChangesSaved() == IDCANCEL);
}


//---------------------------------------------------------------------------------------------------------------------
void
CMainFrame::OnSysCommand(UINT nID, CPoint /*point*/)
{
	if (nID == SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
		isVisible = false;
	}
	else
	{
		SetMsgHandled(FALSE);
	}
}


//---------------------------------------------------------------------------------------------------------------------
void
CMainFrame::RedrawToolbar()
{
	tb.RedrawMe();
}


//---------------------------------------------------------------------------------------------------------------------
void
CMainFrame::localize()
{
	m_view.localize();
	tb.localize();
	RedrawToolbar();
}


//---------------------------------------------------------------------------------------------------------------------
void
CMainFrame::OnLanguageMenuItem(UINT /*uCode*/, int nID, HWND /*hwndCtrl*/)
{
	for (const neatcommon::system::LocaleUiDescriptor & aLocale : logic::MainSingleton::Instance().GetLocales())
	{
		if (aLocale.menuItemId == static_cast<UINT>(nID))
		{
			if (logic::MainSingleton::Instance().selectLocale(aLocale.code))
			{
				localize();
				logic::MainSingleton::Instance().GetOptionsHolder().SetLanguageCode(aLocale.code);
				tb.ChangeLanguageMenu(aLocale.code);
				this->DrawMenuBar();
				Invalidate();
				return;
			}
		}
	}
	SetMsgHandled(FALSE);
}


//---------------------------------------------------------------------------------------------------------------------
void
CMainFrame::OnComboPresetsSelChange(UINT /*uCode*/, int /*nID*/, HWND /*hwndCtrl*/)
{
	int n = tb.comboPresets.GetCurSel();
	if (cbPresetsSelectionIndex != n)
	{
		if (m_view.CheckIfChangesSaved() == IDCANCEL)
		{
			tb.comboPresets.SetCurSel(cbPresetsSelectionIndex);
			return;
		}
	}

	if (n >= 0)
	{
		logic::MainSingleton::Instance().SetMouseParams(tb.GetCurrentSettings());
		cbPresetsSelectionIndex = n;
		m_view.PopulateControls();
	}
}



//---------------------------------------------------------------------------------------------------------------------
void
CMainFrame::OnDrawItem(UINT /*id*/, LPDRAWITEMSTRUCT lpdis)
{
	if (!lpdis) return;
	if (lpdis->CtlType == ODT_MENU) menuBitmapsManager.Draw(lpdis);
	SetMsgHandled(FALSE);
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CMainFrame::OnBnClickedButtonAdvancedView(UINT /*wNotifyCode*/, int wID, HWND /*hWndCtl*/)
{
	if (tb.IsButtonChecked(wID))
	{
		m_view.enableAdvancedMode();
	} else
	{
		m_view.disableAdvancedMode();
	}
	resizeByContent();
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CMainFrame::OnBnClickedButtonPresetSave(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	logic::MainSingleton::Instance().AcceptMouseParams();
	tb.UpdateButtonStates();
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CMainFrame::OnBnClickedButtonPresetAdd(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	if (m_view.CheckIfChangesSaved() == IDCANCEL)
	{
		tb.comboPresets.SetCurSel(cbPresetsSelectionIndex);
		return 0;
	}

	neatcommon::ui::CInputBox ibox(_("toolbar.presets.add-preset-caption"), _("toolbar.presets.add-preset-prompt"),
		_("common.btn-ok"), _("common.btn-cancel"));
	if (ibox.DoModal())
	{
		logic::MainSingleton::Instance().SetMouseParams(logic::MainSingleton::Instance().GetOptionsHolder().CreateNewSettings(ibox.getText().GetBuffer(0)));
		tb.AddSettings(logic::MainSingleton::Instance().GetMouseParams());
		m_view.PopulateControls();
	}
	Invalidate();
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CMainFrame::OnBnClickedButtonPresetDelete(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	if (tb.DeleteCurrentSettings())
	{
		logic::MainSingleton::Instance().SetMouseParams(tb.GetCurrentSettings());
		m_view.PopulateControls();
	}
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
void
CMainFrame::OnMeasureItem(UINT /*id*/, LPMEASUREITEMSTRUCT lpmis)
{
	if (!lpmis) return;
	if (lpmis->CtlType == ODT_MENU) menuBitmapsManager.Measure(lpmis);
	SetMsgHandled(FALSE);
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CMainFrame::OnCreate(LPCREATESTRUCT /*lpcs*/)
{
	CenterWindow(GetParent());

	{
		neatcommon::system::ProductInfo info;
		neatcommon::system::GetProductVersion(info);

		CString s;
		s.Format(_T("NeatMouse %d.%02d.%03d"),
			info.major,
			info.minor,
			MAKELONG(info.build2, info.build1));

		SetWindowText(s);
	}

	HWND hWnd = ::CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_TOOLTIPS | TBSTYLE_LIST | CCS_ADJUSTABLE | TBSTYLE_FLAT,
		0, 0, 100, 100, m_hWnd, 0, ModuleHelper::GetModuleInstance(), 0);
	tb.SubclassWindow(hWnd);

	// remove old menu
	SetMenu(NULL);

	cbPresetsSelectionIndex = tb.comboPresets.GetCurSel();
	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(tb, _T("Title"), FALSE);

	for (const neatcommon::system::LocaleUiDescriptor & aLocale : logic::MainSingleton::Instance().GetLocales())
	{
		menuBitmapsManager.SetBitmap(aLocale.menuItemId, SafeLoadPng(aLocale.iconId));
		neatcommon::ui::SetMenuItemBitmapCallbackMode(tb.GetMenus(), aLocale.menuItemId, menuBitmapsManager.GetBitmap(aLocale.menuItemId));
	}

	menuBitmapsManager.SetBitmap(ID_HELP_ABOUT, SafeLoadPng(IDB_PNG_INFORMATION));
	neatcommon::ui::SetMenuItemBitmapCallbackMode(tb.GetMenus(), ID_HELP_ABOUT, menuBitmapsManager.GetBitmap(ID_HELP_ABOUT));

	m_hWndClient = m_view.Create(m_hWnd);
	m_view.SetToolbar(&tb);

	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);

	isVisible = true;
	localize();

	m_view.disableAdvancedMode();
	resizeByContent();

	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
void
CMainFrame::ToggleVisible()
{
	if (!isVisible)
	{
		ShowWindow(SW_SHOWNORMAL);
		SetForegroundWindow(*this);
	} else
	{
		ShowWindow(SW_HIDE);
	}
	isVisible = !isVisible;
}


//---------------------------------------------------------------------------------------------------------------------
void
CMainFrame::TrackTrayMenu()
{
	POINT pt{};
	GetCursorPos(&pt);

	CMenuHandle menu = tb.GetMenus().GetSubMenu(1);

	MENUITEMINFO iii{};

	iii.cbSize = sizeof(MENUITEMINFO);
	iii.fMask = MIIM_STRING;

	iii.dwTypeData = const_cast<LPWSTR>(_("notify.restore"));
	iii.cch = static_cast<UINT>(wcslen(iii.dwTypeData));
	SetMenuItemInfo(menu, 0, TRUE, &iii);

	iii.dwTypeData = logic::MainSingleton::Instance().GetMouseActioner().isEmulationActivated()
		? const_cast<LPWSTR>(_("notify.disable"))
		: const_cast<LPWSTR>(_("notify.enable"));
	iii.cch = static_cast<UINT>(wcslen(iii.dwTypeData));
	SetMenuItemInfo(menu, 1, TRUE, &iii);

	iii.dwTypeData = const_cast<LPWSTR>(_("notify.exit"));
	iii.cch = static_cast<UINT>(wcslen(iii.dwTypeData));
	SetMenuItemInfo(menu, 2, TRUE, &iii);

	TrackPopupMenu(menu, 0, pt.x, pt.y, 0, m_hWnd, 0);
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CMainFrame::OnTrayBtnClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	switch (lParam)
	{
	case WM_CONTEXTMENU:
	case WM_RBUTTONUP:
		TrackTrayMenu();
		break;

	case WM_LBUTTONUP:
		ToggleVisible();
		break;
	}
	bHandled = TRUE;
	return 1;
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	bHandled = FALSE;
	return 1;
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CMainFrame::OnTrayShow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	ShowWindow(SW_RESTORE);
	isVisible = true;
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CMainFrame::OnTrayToggleEmulation(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	logic::MainSingleton::Instance().GetMouseActioner().activateEmulation(
		!logic::MainSingleton::Instance().GetMouseActioner().isEmulationActivated() );
	logic::MainSingleton::Instance().TriggerOverlay();
	return 0;
}

//---------------------------------------------------------------------------------------------------------------------
LRESULT
CMainFrame::OnAppAbout(UINT /*wNotifyCode*/, int /*wID*/, HWND /*hWndCtl*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
void
CMainFrame::resizeByContent()
{
	CRect r;
	this->GetClientRect(&r);

	CRect toolbarRect;
	tb.GetClientRect(&toolbarRect);

	CRect viewRect;
	m_view.GetClientRect(&viewRect);

	this->ResizeClient(viewRect.Width(), toolbarRect.Height() + viewRect.Height());
	this->RedrawToolbar();
	this->RedrawWindow();
}

} // namespace neatmouse