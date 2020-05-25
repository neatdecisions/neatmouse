//
// Copyright © 2016–2020 Neat Decisions. All rights reserved.
//
// This file is part of NeatMouse.
// The use and distribution terms for this software are covered by the
// Microsoft Public License (http://opensource.org/licenses/MS-PL)
// which can be found in the file LICENSE at the root folder.
//

#pragma once

#include "logic/MouseParams.h"
#include "neatcommon/ui/CustomizedControls.h"
#include "neatcommon/system/Helpers.h"

namespace neatmouse {

class CNeatToolbar : public CWindowImpl<CNeatToolbar, CToolBarCtrl>
{
public:
	CComboBox comboPresets;

	BOOL SubclassWindow(HWND hWnd);

	void UpdateButtonStates();
	void ChangeLanguageMenu(const std::string & langCode);
	void SetToolbarButtonText(UINT nId, LPCTSTR text);

	logic::MouseParams GetCurrentSettings();
	void AddSettings(const logic::MouseParams & mouseParams);
	bool DeleteCurrentSettings();
	void FillSettings();

	void localize();
	CMenuHandle GetMenus();
	void Init();
	void RedrawMe();

protected:
	BEGIN_MSG_MAP(CDetwinnerToolbar)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		NOTIFY_CODE_HANDLER(TBN_DROPDOWN, OnToolbarDropDown)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
	END_MSG_MAP()

	HWND m_wndSelect = NULL;
	WTL::CMenu m_menus;
	neatcommon::ui::CAutosizeStatic m_labelPresets;
	std::map<std::string, int> m_langs;
	CFont m_font;

	HBRUSH OnCtlColorStatic(CDCHandle dc, CStatic wndStatic);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnToolbarDropDown(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);

	void LoadBitmaps();
	void RepositionRightAlignedItems();
	void RepositionCombobox();
	void CreatePresetsCombobox();

};

} // namespace neatmouse