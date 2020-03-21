//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//


#include "StdAfx.h"
#include "NeatToolbar.h"
#include "resource.h"
#include "logic/MainSingleton.h"
#include "neatcommon/system/Helpers.h"

namespace neatmouse {

const TBBUTTON allButtons[] =
{
	{ -1, ID_TOOLBAR_COMBOPRESETS, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0 },
	{ 0, ID_TOOLBAR_ADDPRESET, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Add preset" },
	{ 1, ID_TOOLBAR_REMOVEPRESET, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Remove preset" },
	{ 2, ID_TOOLBAR_SAVEPRESET, TBSTATE_ENABLED, BTNS_BUTTON, {0}, 0, (INT_PTR)L"Save preset" },
	{ -1, ID_TOOLBAR_SEP2, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0},
	{ 3, ID_TOOLBAR_ADVANCEDVIEW, TBSTATE_ENABLED, BTNS_CHECK, {0}, 0, (INT_PTR)L"Advanced view" },

	{ -1, ID_TOOLBAR_SEP1, TBSTATE_ENABLED, BTNS_SEP, {0}, 0, 0},

	{ 4, ID_TOOLBAR_LANGUAGE, TBSTATE_ENABLED, BTNS_WHOLEDROPDOWN | BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Language" },
	{ 5, ID_TOOLBAR_HELP, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE, {0}, 0, (INT_PTR)L"Help" }
};


//---------------------------------------------------------------------------------------------------------------------
void
CNeatToolbar::LoadBitmaps()
{
	AddBitmap(1, SafeLoadPng(IDB_PNG_PLUS));
	AddBitmap(1, SafeLoadPng(IDB_PNG_CROSS));
	AddBitmap(1, SafeLoadPng(IDB_PNG_DISK));
	AddBitmap(1, SafeLoadPng(IDB_PNG_EQUALIZER));

	const neatcommon::system::LocaleUiDescriptor & aFallbackLocale = logic::MainSingleton::Instance().GetFallbackLocale();
	langs[aFallbackLocale.code] = AddBitmap(1, SafeLoadPng(aFallbackLocale.iconId));

	AddBitmap(1, SafeLoadPng(IDB_PNG_QUESTION));
	for (const neatcommon::system::LocaleUiDescriptor & aLocale : logic::MainSingleton::Instance().GetLocales())
	{
		langs[aLocale.code] = AddBitmap(1, SafeLoadPng(aLocale.iconId));
	}
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatToolbar::RedrawMe()
{
	RedrawWindow(0, 0, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE| RDW_ALLCHILDREN);
	CRect rect;
	GetClientRect(&rect);
	InvalidateRect(&rect, TRUE);

	MapWindowPoints(GetParent(), (POINT *) &rect, 2);
	::RedrawWindow(GetParent(), rect, 0, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE| RDW_ALLCHILDREN);
}



//---------------------------------------------------------------------------------------------------------------------
void
CNeatToolbar::UpdateButtonStates()
{
	EnableButton(ID_TOOLBAR_SAVEPRESET, logic::MainSingleton::Instance().WereParametersChanged());
	EnableButton(ID_TOOLBAR_REMOVEPRESET, !logic::MainSingleton::Instance().GetMouseParams().IsPreset());
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatToolbar::ChangeLanguageMenu(const std::string & langCode)
{
	std::map<std::string, int>::const_iterator it = langs.find(langCode);
	if (it != langs.end())
	{
		ChangeBitmap(ID_TOOLBAR_LANGUAGE, it->second);
	}
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatToolbar::SetToolbarButtonText(UINT nId, LPCTSTR text)
{
	TBBUTTONINFO bi;
	bi.cbSize = sizeof(TBBUTTONINFO);
	bi.dwMask = TBIF_TEXT;
	bi.pszText = _wcsdup(text);
	SetButtonInfo(nId, &bi);
	delete [] bi.pszText;
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatToolbar::localize()
{
	ChangeLanguageMenu(logic::MainSingleton::Instance().GetOptionsHolder().GetLanguageCode());

	CMenuHandle hMainMenu = GetMenus().GetSubMenu(1);
	MENUITEMINFO iii;

	iii.cbSize = sizeof(MENUITEMINFO);
	iii.fMask = MIIM_STRING;

	SetToolbarButtonText(ID_TOOLBAR_ADDPRESET, _("toolbar.add-new-preset"));
	SetToolbarButtonText(ID_TOOLBAR_REMOVEPRESET, _("toolbar.delete-preset"));
	SetToolbarButtonText(ID_TOOLBAR_SAVEPRESET, _("toolbar.save-preset"));
	SetToolbarButtonText(ID_TOOLBAR_ADVANCEDVIEW, _("toolbar.advanced-view"));
	labelPresets.SetText(_("toolbar.lbl-preset"));

	SetToolbarButtonText(ID_TOOLBAR_LANGUAGE, _("toolbar.language"));
	SetToolbarButtonText(ID_TOOLBAR_HELP, _("toolbar.about"));

	AutoSize();
	RepositionCombobox();
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatToolbar::AddSettings(const logic::MouseParams & mouseParams)
{
	int n = comboPresets.AddString(mouseParams.GetName().c_str());
	comboPresets.SetCurSel(n);
}


//---------------------------------------------------------------------------------------------------------------------
HBRUSH
CNeatToolbar::OnCtlColorStatic(CDCHandle dc, CStatic /*wndStatic*/)
{
  dc.SetBkMode(TRANSPARENT);
  return (HBRUSH)GetStockObject(NULL_BRUSH);
}


//---------------------------------------------------------------------------------------------------------------------
BOOL
CNeatToolbar::SubclassWindow(HWND hWnd)
{
	ATLASSERT(m_hWnd == NULL);
	ATLASSERT(::IsWindow(hWnd));
	BOOL bRet = CWindowImpl<CNeatToolbar, CToolBarCtrl>::SubclassWindow(hWnd);
	if (bRet)
	{
		Init();
	}

	return bRet;
}


//---------------------------------------------------------------------------------------------------------------------
CSize
CNeatToolbar::getGUIFontSize()
{
	CClientDC dc(m_hWnd);
	dc.SelectFont((HFONT) GetStockObject( DEFAULT_GUI_FONT ));
	TEXTMETRIC tm;
	dc.GetTextMetrics( &tm );

	return CSize( tm.tmAveCharWidth, tm.tmHeight + tm.tmExternalLeading);
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CNeatToolbar::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	RepositionRightAlignedItems();
	bHandled = false;
	return 0;
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatToolbar::RepositionRightAlignedItems()
{
	TBBUTTONINFO bi;
	CRect rc;
	GetWindowRect(rc);
	int width = rc.Width();
	GetItemRect(CommandToIndex(ID_TOOLBAR_SEP1), rc);
	width -= rc.left;

	GetItemRect(CommandToIndex(ID_TOOLBAR_LANGUAGE), rc);
	width -= rc.Width();

	GetItemRect(CommandToIndex(ID_TOOLBAR_HELP), rc);
	width -= rc.Width();

	bi.cbSize = sizeof(TBBUTTONINFO);
	bi.dwMask = TBIF_SIZE;
	bi.cx = static_cast<WORD>(width);
	SetButtonInfo(ID_TOOLBAR_SEP1, &bi);
}


//---------------------------------------------------------------------------------------------------------------------
logic::MouseParams
CNeatToolbar::GetCurrentSettings()
{
	CString s = "(Default)";
	int n = comboPresets.GetCurSel();
	if (n >= 0) comboPresets.GetLBText(n, s);
	return logic::MainSingleton::Instance().GetOptionsHolder().GetSettings(s.GetString());
}


//---------------------------------------------------------------------------------------------------------------------
bool
CNeatToolbar::DeleteCurrentSettings()
{
	const logic::MouseParams & currentSettings = GetCurrentSettings();
	if (currentSettings.IsPreset()) return false;

	int n = comboPresets.GetCurSel();
	if (n < 0) return false;

	comboPresets.DeleteString(n);
	logic::MainSingleton::Instance().GetOptionsHolder().DeleteSettings(currentSettings.GetName());
	if (n >= comboPresets.GetCount()) n--;
	if (n < 0) return false;

	comboPresets.SetCurSel(n);

	return true;
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatToolbar::FillSettings()
{
	comboPresets.Clear();
	int cbPresetsSelectionIndex = 0;

	logic::COptionsHolder & optionsHolder = logic::MainSingleton::Instance().GetOptionsHolder();
	const std::wstring & currentSettingsName = logic::MainSingleton::Instance().GetMouseParams().GetName();
	for (const std::wstring & settingName : optionsHolder.GetAllSettingNames())
	{
		comboPresets.AddString(settingName.c_str());
		if (settingName == currentSettingsName)
		{
			cbPresetsSelectionIndex = comboPresets.GetCount() - 1;
		}
	}

	comboPresets.SetCurSel(cbPresetsSelectionIndex);
	UpdateButtonStates();
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatToolbar::RepositionCombobox()
{
	int nIndex = CommandToIndex(ID_TOOLBAR_COMBOPRESETS);
	CRect rc, labelRect, comboRect;
	GetItemRect(nIndex, &rc);

	labelPresets.GetWindowRect(labelRect);
	labelPresets.GetParent().ScreenToClient(labelRect);

	labelRect.MoveToX(rc.left + 10);

	labelPresets.MoveWindow(labelRect.left, labelRect.top, labelRect.Width(), labelRect.Height());

	comboPresets.GetWindowRect(comboRect);
	comboPresets.GetParent().ScreenToClient(comboRect);

	comboRect.MoveToX(labelRect.right + 5);

	comboPresets.MoveWindow(comboRect.left, comboRect.top, comboRect.Width(), comboRect.Height());

	TBBUTTONINFO bi;
	bi.cbSize = sizeof(TBBUTTONINFO);
	bi.dwMask = TBIF_SIZE;
	bi.cx = static_cast<WORD>(labelRect.Width() + comboRect.Width() + 17);
	SetButtonInfo(ID_TOOLBAR_COMBOPRESETS, &bi);

	RepositionRightAlignedItems();
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatToolbar::CreatePresetsCombobox()
{
	TBBUTTONINFO tbi;
	RECT rc;

	tbi.cbSize = sizeof TBBUTTONINFO;
	tbi.dwMask = TBIF_STYLE;
	tbi.fsStyle = TBSTYLE_SEP;

	SetButtonInfo(ID_TOOLBAR_COMBOPRESETS, &tbi);

	int nIndex = CommandToIndex(ID_TOOLBAR_COMBOPRESETS);
	GetItemRect(nIndex, &rc);

	rc.left += 10;

	CRect labelRect = rc;

	CSize sz = getGUIFontSize();
	CString s = _T("Preset:");
	labelRect.right = rc.left + sz.cx * s.GetLength();
	labelRect.top = rc.top + (rc.bottom - rc.top - sz.cy) / 2;

	labelPresets.Create(this->GetParent(), labelRect, NULL, WS_CHILD | WS_VISIBLE, WS_EX_TRANSPARENT);
	labelPresets.SetText(s);
	labelPresets.SetParent(*this);
	labelPresets.SetFont((HFONT)GetStockObject( DEFAULT_GUI_FONT ));

	rc.top = labelRect.top - 4;
	rc.left += labelRect.Width();
	rc.bottom = rc.top + 8 * sz.cy;

	rc.right = rc.left + 150;

	comboPresets.Create(this->GetParent(), rc, NULL, CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL, 0, IDC_COMBO_PRESETS);

	comboPresets.SetParent(*this);
	comboPresets.SetFont((HFONT)GetStockObject( DEFAULT_GUI_FONT ));
	RepositionCombobox();
}


//-----------------------------------------------------------------------------
CMenuHandle
CNeatToolbar::GetMenus()
{
	return menus.m_hMenu;
}


//---------------------------------------------------------------------------------------------------------------------
void
CNeatToolbar::Init()
{
	::SetWindowLong(m_hWnd, GWL_STYLE, ::GetWindowLong(m_hWnd, GWL_STYLE) | TBSTYLE_FLAT);
	SetExtendedStyle(GetExtendedStyle() | TBSTYLE_EX_MIXEDBUTTONS | TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_DOUBLEBUFFER);

	SetButtonStructSize(sizeof(TBBUTTON));

	LoadBitmaps();

	std::vector<TBBUTTON> buttons(sizeof(allButtons) / sizeof(allButtons[0]));
	for (size_t i = 0; i < buttons.size(); i++)
	{
		buttons[i] = allButtons[i];
	}

	this->AddButtons(buttons.size(), &buttons[0]);
	CreatePresetsCombobox();
	FillSettings();

	menus.LoadMenu( MAKEINTRESOURCE( IDR_MAINFRAME ) );
}


//---------------------------------------------------------------------------------------------------------------------
LRESULT
CNeatToolbar::OnToolbarDropDown(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
    // Get the toolbar data
  NMTOOLBAR* ptb = reinterpret_cast<NMTOOLBAR*>(pnmh);

	CRect rect;
  GetItemRect(CommandToIndex(ptb->iItem), &rect);
  // Create a point
  CPoint pt(rect.left, rect.bottom);
	MapWindowPoints(HWND_DESKTOP, &pt, 1);
  // Load the menu
  CMenu menu;
	switch (ptb->iItem)
	{
	case ID_TOOLBAR_LANGUAGE:
		ASSERT(menus.GetSubMenu(0) != NULL);
		::TrackPopupMenu(menus.GetSubMenu(0), TPM_RIGHTBUTTON | TPM_VERTICAL, pt.x, pt.y, 0, this->GetParent(), 0);
		break;
	}

	return 0;
}

} // namespace neatmouse